#pragma once

#ifndef CIV5_CONTRACT_CLASSES_H
#define CIV5_CONTRACT_CLASSES_H

#if defined(MOD_BALANCE_CORE)

#include "CvWeightedVector.h"

class CvContractEntry: public CvBaseInfo
{
public:
	CvContractEntry(void);
	~CvContractEntry(void);
	
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	const char* GetCategory() const;
	const char* GetAdjective() const;
	const char* GetPrereqPietyLevel() const;
	CivilizationTypes GetPrereqCivilization() const;
	EraTypes GetPrereqEra() const;
	EraTypes GetObsoleteEra() const;
	ReligionTypes GetPrereqReligion() const;
	bool IsHolyOrder() const;
	bool IsSlavery() const;
	int GetMaintenance() const;
	int GetRating() const;
	int GetDisbandChance() const;
	int GetMutinyChance() const;
	PolicyTypes GetPolicyType() const;
	PromotionTypes GetPromotionType() const;
	int GetTurns() const;
	UnitTypes GetContractUnit() const;
	int GetYieldCost(YieldTypes eYield) const;
	int GetFlavorValue(FlavorTypes eFlavor) const;
	
protected:
	CvString m_strCategory;
	CvString m_strAdjective;
	CvString m_strPrereqPietyLevel;

	int m_iPrereqCivilization;
	int m_iPrereqEra;
	int m_iObsoleteEra;
	int m_iPrereqReligion;
	bool m_bHolyOrder;
	bool m_bSlavery;
	int m_iMaintenance;
	int m_iRating;
	int m_iDisbandChance;
	int m_iMutinyChance;
	int m_iPolicyType;
	int m_iPromotionType;
	int m_iTurns;
	int m_iContractUnit;
	int* m_piYieldCost;
	int* m_piFlavor;


private:
	CvContractEntry(const CvContractEntry&);
	CvContractEntry& operator=(const CvContractEntry&);
};

class CvContractXMLEntries
{
public:
	CvContractXMLEntries(void);
	~CvContractXMLEntries(void);

	// Accessor functions
	std::vector<CvContractEntry*>& GetContractEntries();
	int GetNumContracts();
	CvContractEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvContractEntry*> m_paContractEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvContract
//!  \brief All the information about a single contract
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvContract
{
public:
	CvContract();
	CvContract(ContractTypes eContract, PlayerTypes eContractHolder, UnitTypes eContractUnit, int iTurns, int iMaintenance);

	// Public data
	ContractTypes m_eContract;
	PlayerTypes m_eContractHolder;
	UnitTypes m_eContractUnit;
	int m_iContractTurns;
	int m_iContractTurnStart;
	int m_iContractMaintenance;
};

FDataStream& operator>>(FDataStream&, CvContract&);
FDataStream& operator<<(FDataStream&, const CvContract&);

FDataStream& operator>>(FDataStream&, ContractTypes&);
FDataStream& operator<<(FDataStream&, const ContractTypes&);

class CvPlayerContracts;

typedef FStaticVector<CvContract, 16, false, c_eCiv5GameplayDLL > ContractList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvGameContracts
//!  \brief		All the information about contracts available and active in the game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameContracts
{
public:
	CvGameContracts(void);
	~CvGameContracts(void);

	void Init();
	void DoTurn();

	void DoUpdateContracts();

	int GetNumActiveContracts() const;
	int GetNumInactiveContracts() const;

	int GetNumAvailableContracts() const;
	int GetNumUnavailableContracts() const;

	CvContract* GetActiveContract(ContractTypes eContract);
	CvContract* GetInactiveContract(ContractTypes eContract);

	bool IsContractActive(ContractTypes eContract);
	bool IsContractAvailable(ContractTypes eContract);

	void StartContract(CvContract kContract);
	void EndContract(ContractTypes eContract, PlayerTypes ePlayer);

	UnitTypes GetContractUnit(ContractTypes eContract);

	ContractList m_ActiveContracts;
	ContractList m_InactiveContracts;
};

FDataStream& operator>>(FDataStream&, CvGameContracts&);
FDataStream& operator<<(FDataStream&, const CvGameContracts&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvPlayerContracts
//!  \brief		All the information about contracts for a player
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerContracts
{
public:
	CvPlayerContracts(void);
	~CvPlayerContracts(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	CvContract* GetContract() const;

	void DoTurn();
	bool PlayerHasContract(ContractTypes eContract) const;
	bool PlayerHasAnyContract() const;

	bool UnitIsContractResourceFree(UnitTypes eUnit);
	bool UnitIsMaintenanceFree(UnitTypes eUnit);

	int GetContractTurnsRemaining() const;

	ContractTypes GetActiveContract() const;
	void SetActiveContract(ContractTypes eContract);
	void StartContract(ContractTypes eContract);

	CvContractEntry* GetContractEntry() const;



private:
	
	CvPlayer* m_pPlayer;
	ContractTypes m_eActiveContract;
};

#endif
#endif