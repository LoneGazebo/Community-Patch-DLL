#pragma once

#ifndef CIV5_CORPORATION_CLASSES_H
#define CIV5_CORPORATION_CLASSES_H

#if defined(MOD_BALANCE_CORE)

class CvCorporationEntry: public CvBaseInfo
{
public:
	CvCorporationEntry(void);
	~CvCorporationEntry(void);
	
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int GetBaseFranchises() const;
	BuildingClassTypes GetHeadquartersBuildingClass() const;
	BuildingClassTypes GetOfficeBuildingClass() const;
	BuildingClassTypes GetFranchiseBuildingClass() const;

	int GetTradeRouteSeaDistanceModifier() const;
	int GetTradeRouteLandDistanceModifier() const;
	int GetTradeRouteSpeedModifier() const;
	int GetNumFreeTradeRoutes() const;
	bool IsTradeRoutesInvulnerable() const;
	int GetTradeRouteVisionBoost() const;

	int GetTradeRouteRecipientBonus() const;
	int GetTradeRouteTargetBonus() const;
	int GetRandomSpreadChance() const;
	int GetTourismMod() const;

	uint GetResourceMonopolyAndSize() const;
	int GetResourceMonopolyAnd(uint ui) const;
	uint GetResourceMonopolyOrSize() const;
	int GetResourceMonopolyOr(uint ui) const;
	int GetNumFreeResource(int i) const;
	int GetUnitResourceProductionModifier(int i) const;

	int GetTradeRouteMod(int i) const;
	int GetTradeRouteCityMod(int i) const;

	int GetResourceYieldChange(int i, int j) const;
	int* GetResourceYieldChangeArray(int i) const;

	int GetBuildingClassYieldChange(int i, int j) const;

	int GetSpecialistYieldChange(int i, int j) const;
	int* GetSpecialistYieldChangeArray(int i) const;

	CvString GetOfficeBenefitHelper() const;
	CvString GetTradeRouteBenefitHelper() const;
protected:
	BuildingClassTypes m_eHeadquartersBuildingClass;
	BuildingClassTypes m_eOfficeBuildingClass;
	BuildingClassTypes m_eFranchiseBuildingClass;

	int m_iTradeRouteSeaDistanceModifier;
	int m_iTradeRouteLandDistanceModifier;
	int m_iTradeRouteSpeedModifier;
	int m_iNumFreeTradeRoutes;
	int m_iBaseFranchises;
	bool m_bTradeRoutesInvulnerable;
	int m_iTradeRouteVisionBoost;
	
	int m_iTradeRouteRecipientBonus;
	int m_iTradeRouteTargetBonus;
	int m_iBaseSpreadChance;
	int m_iTourismMod;

	std::vector<int> m_viResourceMonopolyAnds;
	std::vector<int> m_viResourceMonopolyOrs;
	int* m_piNumFreeResource;
	int* m_piUnitResourceProductionModifier;

	int* m_piTradeRouteMod;
	int* m_piTradeRouteCityMod;

	int** m_ppiBuildingClassYieldChanges;
	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiResourceYieldChange;

	CvString m_strOfficeBenefitHelper;

	CvString m_strTradeRouteBenefitHelper;
private:
	CvCorporationEntry(const CvCorporationEntry&);
	CvCorporationEntry& operator=(const CvCorporationEntry&);
};

class CvCorporationXMLEntries
{
public:
	CvCorporationXMLEntries(void);
	~CvCorporationXMLEntries(void);

	// Accessor functions
	std::vector<CvCorporationEntry*>& GetCorporationEntries();
	int GetNumCorporations();
	CvCorporationEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvCorporationEntry*> m_paCorporationEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvCorporation
//!  \brief All the information about a single corporation
//
//!  Key Attributes:
//!  - Stores the founder and headquarter city
//!	 - Stores the statistics for the corporation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvCorporation
{
public:
	CvCorporation();
	CvCorporation(CorporationTypes eCorporation, PlayerTypes eFounder, CvCity* pHeadquarters);

	bool IsCorporationBuilding(BuildingClassTypes eBuildingClass) const;

	template<typename Corporation, typename Visitor>
	static void Serialize(Corporation& corporation, Visitor& visitor);

	// Public data
	CorporationTypes m_eCorporation;
	PlayerTypes m_eFounder;
	int m_iHeadquartersCityX;
	int m_iHeadquartersCityY;

	int m_iTurnFounded;
};

FDataStream& operator>>(FDataStream&, CvCorporation&);
FDataStream& operator<<(FDataStream&, const CvCorporation&);

FDataStream& operator>>(FDataStream&, CorporationTypes&);
FDataStream& operator<<(FDataStream&, const CorporationTypes&);

class CvPlayerCorporations;

typedef vector<CvCorporation> CorporationList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvGameCorporations
//!  \brief		All the information about corporations founded and active in the game
//
//!  Key Attributes:
//!  - Core data in this class is a list of CvCorporations
//!  - This object is created inside the CvGame object and accessed through CvGame
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on the corporations in place
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameCorporations
{
public:
	CvGameCorporations(void);
	~CvGameCorporations(void);

	void Init();
	void DoTurn();

	template<typename GameCorporations, typename Visitor>
	static void Serialize(GameCorporations& gameCorporations, Visitor& visitor);

	CvCorporation* GetCorporation(CorporationTypes eCorporation);
	int GetNumActiveCorporations() const;
	int GetNumAvailableCorporations() const;

	void DestroyCorporation(CorporationTypes eCorporation);
	void FoundCorporation(PlayerTypes ePlayer, CorporationTypes eCorporation, CvCity* pHeadquarters);
	bool CanFoundCorporation(PlayerTypes ePlayer, CorporationTypes eCorporation) const;

	bool IsCorporationFounded(CorporationTypes eCorporation) const;
	bool IsCorporationHeadquarters(CvCity* pCity) const;

	CorporationList m_ActiveCorporations;
};

FDataStream& operator>>(FDataStream&, CvGameCorporations&);
FDataStream& operator<<(FDataStream&, const CvGameCorporations&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvPlayerCorporations
//!  \brief		All the information about corporations for a player
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerCorporations
{
	friend void CvGameCorporations::DestroyCorporation(CorporationTypes eCorporation);

public:
	CvPlayerCorporations();
	~CvPlayerCorporations();
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	template<typename PlayerCorporations, typename Visitor>
	static void Serialize(PlayerCorporations& playerCorporations, Visitor& visitor);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	CvCorporation* GetCorporation() const;

	CvCity* GetHeadquarters() const;

	void DoTurn();

	int GetAdditionalNumFranchises() const;
	void ChangeAdditionalNumFranchises(int iChange);

	int GetAdditionalNumFranchisesMod() const;
	void ChangeAdditionalNumFranchisesMod(int iChange);

	int GetMaxNumFranchises() const;
	int GetNumFranchises() const;

	CvString GetNumFranchisesTooltip();

	int GetFranchiseTourismMod(PlayerTypes ePlayer, bool bJustCheckOne = false) const;

	int GetNumOffices() const;

	void RecalculateNumOffices();
	void RecalculateNumFranchises();

	void BuildFranchiseInCity(CvCity* pOriginCity, CvCity* pDestCity);
	void BuildRandomFranchiseInCity();

	void LogCorporationMessage(const CvString& strMsg);
	CvString GetLogFileName(CvString& playerName) const;

	CvString GetCurrentOfficeBenefit();

	CvString GetTradeRouteBenefit();

	CvCorporationEntry* GetCorporationEntry() const;

	void ClearAllCorporationsFromCity(CvCity* pCity);

	void ClearCorporationFromCity(CvCity* pCity, CorporationTypes eCorporation, bool bAllButThis = false);

	void ClearCorporationFromForeignCities(bool bMinorsOnly = false, bool bExcludeVassals = false, bool bExcludeMasters = false);

	bool CanCreateFranchiseInCity(CvCity* pOriginCity, CvCity* pTargetCity);

	bool HasFoundedCorporation() const;
	CorporationTypes GetFoundedCorporation() const;
	void SetFoundedCorporation(CorporationTypes eCorporation);

	bool IsCorporationOfficesAsFranchises() const;
	int GetCorporationOfficesAsFranchises() const;
	void ChangeCorporationOfficesAsFranchises(int iValue);

	int GetCorporationRandomForeignFranchiseMod() const;
	void ChangeCorporationRandomForeignFranchiseMod(int iValue);

	int GetCorporationFreeFranchiseAbovePopular() const;
	void ChangeCorporationFreeFranchiseAbovePopular(int iValue);

	bool IsNoForeignCorpsInCities() const;
	void ChangeNoForeignCorpsInCities(int iValue);

	bool IsNoFranchisesInForeignCities() const;
	void ChangeNoFranchisesInForeignCities(int iValue);

	int GetFranchisesPerImprovement(ImprovementTypes eIndex) const;
	void ChangeFranchisesPerImprovement(ImprovementTypes eIndex, int iValue);

	void DestroyCorporation();

private:

	CvPlayer* m_pPlayer;
	CorporationTypes m_eFoundedCorporation;

	int m_iNumOffices;
	int m_iNumFranchises;
	int m_iAdditionalNumFranchises;
	int m_iAdditionalNumFranchisesMod;

	int m_iCorporationOfficesAsFranchises;
	int m_iCorporationRandomForeignFranchiseMod;
	int m_iCorporationFreeFranchiseAbovePopular;

	std::vector<int> m_aiFranchisesPerImprovement;

	int m_iNoForeignCorpsInCities;
	int m_iNoFranchisesInForeignCities;
};

FDataStream& operator>>(FDataStream&, CvPlayerCorporations&);
FDataStream& operator<<(FDataStream&, const CvPlayerCorporations&);

#endif
#endif