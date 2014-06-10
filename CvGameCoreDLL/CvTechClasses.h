/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TECH_CLASSES_H
#define CIV5_TECH_CLASSES_H

// Forward definitions
class CvTeam;
class CvTechAI;

#include "CvInfos.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTechEntry
//!  \brief		A single entry in the tech tree
//
//!  Key Attributes:
//!  - Used to be called CvTechInfo
//!  - Populated from XML\Technologies\CIV5TechInfos.xml
//!  - Array of these contained in CvTechXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTechEntry: public CvBaseInfo
{
public:
	CvTechEntry(void);
	~CvTechEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions (Non-Arrays)
	int GetAIWeight() const;
	int GetAITradeModifier() const;
	int GetResearchCost() const;
	int GetAdvancedStartCost() const;
	int GetEra() const;
	int GetFeatureProductionModifier() const;
	int GetWorkerSpeedModifier() const;
	int GetFirstFreeUnitClass() const;
	int GetFirstFreeTechs() const;
	int GetEmbarkedMoveChange() const;

	int GetGridX() const;
	int GetGridY() const;

	bool IsEndsGame() const;
	bool IsAllowsEmbarking() const;
	bool IsAllowsDefensiveEmbarking() const;
	bool IsEmbarkedAllWaterPassage() const;
	bool IsAllowsBarbarianBoats() const;
	bool IsRepeat() const;
	bool IsTrade() const;
	bool IsDisable() const;
	bool IsGoodyTech() const;
	bool IsExtraWaterSeeFrom() const;
	bool IsMapCentering() const;
	bool IsMapVisible() const;
	bool IsMapTrading() const;
	bool IsTechTrading() const;
	bool IsGoldTrading() const;
	bool IsOpenBordersTradingAllowed() const;
	bool IsDefensivePactTradingAllowed() const;
	bool IsResearchAgreementTradingAllowed() const;
	bool IsTradeAgreementTradingAllowed() const;
	bool IsPermanentAllianceTrading() const;
	bool IsBridgeBuilding() const;
	bool IsWaterWork() const;

	std::string pyGetQuote() { return GetQuote(); }
	const char* GetQuote();
	void SetQuoteKey(const char* szVal);
	const char* GetSound() const;
	void SetSound(const char* szVal);
	const char* GetSoundMP() const;
	void SetSoundMP(const char* szVal);

	// Accessor Functions (Arrays)
	int GetDomainExtraMoves(int i) const;
	int GetFlavorValue(int i) const;
	int GetPrereqOrTechs(int i) const;
	int GetPrereqAndTechs(int i) const;

private:
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
	int m_iAdvancedStartCost;
	int m_iEra;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;
	int m_iFirstFreeUnitClass;
	int m_iFirstFreeTechs;
	int m_iEmbarkedMoveChange;

	int m_iGridX;
	int m_iGridY;

	bool m_bEndsGame;
	bool m_bAllowsEmbarking;
	bool m_bAllowsDefensiveEmbarking;
	bool m_bEmbarkedAllWaterPassage;
	bool m_bAllowsBarbarianBoats;
	bool m_bRepeat;
	bool m_bTrade;
	bool m_bDisable;
	bool m_bGoodyTech;
	bool m_bExtraWaterSeeFrom;
	bool m_bMapCentering;
	bool m_bMapVisible;
	bool m_bMapTrading;
	bool m_bTechTrading;
	bool m_bGoldTrading;
	bool m_bOpenBordersTradingAllowed;
	bool m_bDefensivePactTradingAllowed;
	bool m_bResearchAgreementTradingAllowed;
	bool m_bTradeAgreementTradingAllowed;
	bool m_bPermanentAllianceTrading;
	bool m_bBridgeBuilding;
	bool m_bWaterWork;

	CvString m_strQuoteKey;
	CvString m_wstrQuote;
	CvString m_strSound;
	CvString m_strSoundMP;

	// Arrays
	int* m_piDomainExtraMoves;
	int* m_piFlavorValue;
	int* m_piPrereqOrTechs;
	int* m_piPrereqAndTechs;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTechXMLEntries
//!  \brief		Game-wide information about the tech tree
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\Technologies\CIV5TechInfos.xml
//! - Contains an array of CvTechEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvPlayerTechs class (which stores the tech state for 1 player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTechXMLEntries
{
public:
	CvTechXMLEntries(void);
	~CvTechXMLEntries(void);

	// Accessor functions
	std::vector<CvTechEntry*>& GetTechEntries();
	int GetNumTechs();
	_Ret_maybenull_ CvTechEntry *GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvTechEntry*> m_paTechEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPlayerTechs
//!  \brief		Information about the technologies being researched by a single player
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to check technology research state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerTechs: public CvFlavorRecipient
{
public:
	CvPlayerTechs(void);
	~CvPlayerTechs(void);
	void Init(CvTechXMLEntries *pTechs, CvPlayer *pPlayer, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Flavor recipient required function
	void FlavorUpdate ();

	CvPlayer* GetPlayer();
	CvTechAI* GetTechAI();

	// Accessor functions
	void SetResearchingTech(TechTypes eIndex, bool bNewValue);
    bool IsResearchingTech (TechTypes eIndex) const;
	void SetCivTechPriority(TechTypes eIndex, int iNewValue);
	int GetCivTechPriority(TechTypes eIndex) const;
	void SetLocaleTechPriority(TechTypes eIndex, int iNewValue);
	int GetLocaleTechPriority(TechTypes eIndex) const;
	ResourceTypes GetLocaleTechResource(TechTypes eIndex) const;
	UnitTypes GetCivTechUniqueUnit(TechTypes eIndex) const;
	BuildingTypes GetCivTechUniqueBuilding(TechTypes eIndex) const;
	ImprovementTypes GetCivTechUniqueImprovement(TechTypes eIndex) const;
	void SetLocalePriorities();
	bool IsResearch() const;
	bool CanEverResearch(TechTypes eTech) const;
	bool CanResearch(TechTypes eTech, bool bTrade = false) const;
	TechTypes GetCurrentResearch() const;
	bool IsCurrentResearchRepeat() const;
	bool IsNoResearchAvailable() const;
	void CheckForTechAchievement() const;
	int GetResearchTurnsLeft(TechTypes eTech, bool bOverflow) const;
	int GetResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const;
	int GetNumTechsCanBeResearched () const;
	CvTechXMLEntries *GetTechs() const;
	int GetResearchCost(TechTypes eTech) const;
	int GetResearchProgress(TechTypes eTech) const;
	int GetMedianTechResearch() const;

private:
	void AddFlavorAsStrategies(int iPropagatePercent);

	// Logging functions
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

	bool* m_pabResearchingTech;
	int* m_piCivTechPriority;
	int* m_piLocaleTechPriority;
	ResourceTypes* m_peLocaleTechResources;
	UnitTypes* m_peCivTechUniqueUnits;
	BuildingTypes* m_peCivTechUniqueBuildings;
	ImprovementTypes* m_peCivTechUniqueImprovements;
	CvTechXMLEntries *m_pTechs;
	CvPlayer* m_pPlayer;
    CvTechAI *m_pTechAI;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTeamTechs
//!  \brief		Information about the technologies owned by a team of players
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvTeamState object within CvTeam class
//!  - One instance for each team of civs
//!  - Accessed by any class that needs to check technology ownership
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTeamTechs
{
public:
	CvTeamTechs(void);
	~CvTeamTechs(void);
	void Init(CvTechXMLEntries *pTechs, CvTeam *pTeam);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Accessor functions
	void SetHasTech(TechTypes eIndex, bool bNewValue);
	bool HasTech (TechTypes eIndex) const;

	TechTypes GetLastTechAcquired() const;
	void SetLastTechAcquired(TechTypes eTech);

	int GetNumTechsKnown() const;
	bool HasResearchedAllTechs() const;

	void SetNoTradeTech(TechTypes eIndex, bool bNewValue);
	bool IsNoTradeTech (TechTypes eIndex) const;
	void IncrementTechCount (TechTypes eIndex);
	int GetTechCount (TechTypes eIndex) const;
	void SetResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
    void SetResearchProgressTimes100 (TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
	int GetResearchProgress (TechTypes eIndex) const;
	int GetResearchProgressTimes100 (TechTypes eIndex) const;
	void ChangeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
	void ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
	int ChangeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer);
	int GetResearchCost (TechTypes eTech) const;
	int GetResearchLeft(TechTypes eTech) const;
	CvTechXMLEntries *GetTechs() const;

private:
	TechTypes m_eLastTechAcquired;

	bool* m_pabHasTech;
	bool* m_pabNoTradeTech;
	int* m_paiResearchProgress;  // Stored in hundredths
	int* m_paiTechCount;
	CvTechXMLEntries *m_pTechs;
	CvTeam* m_pTeam;
};

#endif //CIV5_TECH_CLASSES_H