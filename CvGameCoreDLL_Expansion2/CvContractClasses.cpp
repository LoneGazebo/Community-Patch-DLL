#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE)

CvContractEntry::CvContractEntry(void):
	m_strCategory(""),
	m_strAdjective(""),
	m_strPrereqPietyLevel(""),
	m_iPrereqCivilization(NO_CIVILIZATION),
	m_iPrereqEra(NO_ERA),
	m_iObsoleteEra(NO_ERA),
	m_iPrereqReligion(NO_RELIGION),
	m_bHolyOrder(false),
	m_bSlavery(false),
	m_iMaintenance(-1),
	m_iRating(-1),
	m_iDisbandChance(-1),
	m_iMutinyChance(-1),
	m_iPolicyType(NO_POLICY),
	m_iPromotionType(NO_PROMOTION),
	m_iTurns(-1),
	m_piYieldCost(NULL),
	m_piFlavor(NULL)
{
}

CvContractEntry::~CvContractEntry(void)
{
	SAFE_DELETE_ARRAY(m_piYieldCost);
	SAFE_DELETE_ARRAY(m_piFlavor);
}

const char* CvContractEntry::GetCategory() const
{
	return m_strCategory.c_str();
}
const char* CvContractEntry::GetAdjective() const
{
	return m_strAdjective.c_str();
}

const char* CvContractEntry::GetPrereqPietyLevel() const
{
	return m_strPrereqPietyLevel.c_str();
}
CivilizationTypes CvContractEntry::GetPrereqCivilization() const
{
	return (CivilizationTypes)m_iPrereqCivilization;
}

EraTypes CvContractEntry::GetPrereqEra() const
{
	return (EraTypes)m_iPrereqEra;
}

EraTypes CvContractEntry::GetObsoleteEra() const
{
	return (EraTypes)m_iPrereqEra;
}

ReligionTypes CvContractEntry::GetPrereqReligion() const
{
	return (ReligionTypes)m_iPrereqReligion;
}


bool CvContractEntry::IsHolyOrder() const
{
	return m_bHolyOrder;
}

bool CvContractEntry::IsSlavery() const
{
	return m_bSlavery;
}

int CvContractEntry::GetMaintenance() const
{
	return m_iMaintenance;
}

int CvContractEntry::GetRating() const
{
	return m_iRating;
}

int CvContractEntry::GetDisbandChance() const
{
	return m_iDisbandChance;
}

int CvContractEntry::GetMutinyChance() const
{
	return m_iMutinyChance;
}

PolicyTypes CvContractEntry::GetPolicyType() const
{
	return (PolicyTypes)m_iPolicyType;
}
PromotionTypes CvContractEntry::GetPromotionType() const
{
	return (PromotionTypes)m_iPromotionType;
}
int CvContractEntry::GetTurns() const
{
	return m_iTurns;
}
int CvContractEntry::GetYieldCost(YieldTypes eYield) const
{
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eYield > -1, "Index out of bounds");
	return m_piYieldCost ? m_piYieldCost[eYield] : -1;
}
int CvContractEntry::GetFlavorValue(FlavorTypes eFlavor) const
{
	CvAssertMsg(eFlavor < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(eFlavor > -1, "Index out of bounds");
	return m_piFlavor ? m_piFlavor[eFlavor] : -1;
}

bool CvContractEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_iTurns = kResults.GetInt("ContractTurns");
	m_iMutinyChance = kResults.GetInt("MutinyChance");
	m_iDisbandChance = kResults.GetInt("DisbandChance");
	m_iRating = kResults.GetInt("ContractRating");
	m_iMaintenance = kResults.GetInt("Maintenance");
	m_bSlavery = kResults.GetBool("Slavery");
	m_bHolyOrder = kResults.GetBool("HolyOrder");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("PrereqPietyLevel");
	m_strPrereqPietyLevel = szTextVal;

	szTextVal = kResults.GetText("PrereqCiv");
	m_iPrereqCivilization = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PrereqEra");
	m_iPrereqEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteEra");
	m_iObsoleteEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PrereqReligion");
	m_iPrereqReligion = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PolicyType");
	m_iPolicyType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PromotionType");
	m_iPromotionType = GC.getInfoTypeForString(szTextVal, true);
	
	szTextVal = kResults.GetText("Category");
	m_strCategory = szTextVal;

	szTextVal = kResults.GetText("Adjective");
	m_strAdjective = szTextVal;

	const char* szContractType = GetType();

	kUtility.SetYields(m_piYieldCost, "Contract_YieldCost", "ContractType", szContractType);
	kUtility.SetFlavors(m_piFlavor, "Contract_Flavor", "ContractType", szContractType);

	return true;
}

//=====================================
// CvContractXMLEntries
//=====================================
/// Constructor
CvContractXMLEntries::CvContractXMLEntries(void)
{

}

/// Destructor
CvContractXMLEntries::~CvContractXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of Corporation entries
std::vector<CvContractEntry*>& CvContractXMLEntries::GetContractEntries()
{
	return m_paContractEntries;
}

/// Number of defined Corporations
int CvContractXMLEntries::GetNumContracts()
{
	return m_paContractEntries.size();
}

/// Clear Corporation entries
void CvContractXMLEntries::DeleteArray()
{
	for(std::vector<CvContractEntry*>::iterator it = m_paContractEntries.begin(); it != m_paContractEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paContractEntries.clear();
}

/// Get a specific entry
CvContractEntry* CvContractXMLEntries::GetEntry(int index)
{
	return (index != NO_CONTRACT) ? m_paContractEntries[index] : NULL;
}

//=====================================
// CvContract
//=====================================

CvContract::CvContract()
	: m_eContract(NO_CONTRACT)
	, m_eContractHolder(NO_PLAYER)
	, m_iContractTurns(-1)
	, m_iContractTurnStart(-1)
	, m_iContractMaintenance(-1)
{
}

CvContract::CvContract(ContractTypes eContract, PlayerTypes eContractHolder, int iTurns, int iMaintenance)
	: m_eContract(eContract)
	, m_eContractHolder(eContractHolder)
	, m_iContractTurns(iTurns)
	, m_iContractMaintenance(iMaintenance)
{
	m_iContractTurnStart = GC.getGame().getGameTurn();
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvContract& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_eContract;
	loadFrom >> writeTo.m_eContractHolder;
	loadFrom >> writeTo.m_iContractTurns;
	loadFrom >> writeTo.m_iContractMaintenance;

	loadFrom >> writeTo.m_iContractTurnStart;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvContract& readFrom)
{
	uint uiVersion = 4;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eContract;
	saveTo << readFrom.m_eContractHolder;
	saveTo << readFrom.m_iContractTurns;
	saveTo << readFrom.m_iContractMaintenance;

	saveTo << readFrom.m_iContractTurnStart;

	return saveTo;
}

// For some reason CvSerializationInfoHelpers is not working for this.
FDataStream& operator<<(FDataStream& saveTo, const ContractTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, ContractTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<ContractTypes>(v);
	return loadFrom;
}

//=====================================
// CvPlayerContracts
//=====================================
/// Constructor
CvPlayerContracts::CvPlayerContracts() :
	m_pPlayer(NULL)
{
}

/// Destructor
CvPlayerContracts::~CvPlayerContracts()
{
	Uninit();
}

/// Initialize class data
void CvPlayerContracts::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	Reset();
}

/// Cleanup
void CvPlayerContracts::Uninit()
{
}

/// Reset
void CvPlayerContracts::Reset()
{

}

CvContract* CvPlayerContracts::GetContract(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return NULL;

	CvContractEntry* pContract = GC.getContractInfo(eContract);
	if(pContract)
	{
		return GC.getGame().GetGameContracts()->GetActiveContract(eContract);
	}
	return NULL;
}
bool CvPlayerContracts::PlayerHasContract(ContractTypes eContract) const
{
	return m_pPlayer->PlayerHasContract(eContract);
}
bool CvPlayerContracts::PlayerHasAnyContract() const
{
	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		CvContractEntry* pContract = GC.getContractInfo((ContractTypes)iI);
		if(pContract && PlayerHasContract((ContractTypes) iI))
		{
			return true;
		}
	}
	return false;
}

void CvPlayerContracts::SetActiveContract(ContractTypes eContract, bool bValue)
{
	m_pPlayer->SetActiveContract(eContract, bValue);
}

//Found a contract - this is the core starter for contracts.
void CvPlayerContracts::StartContract(ContractTypes eContract)
{
	CvContractEntry* pContract = GC.getContractInfo((ContractTypes)eContract);
	if(pContract)
	{
		int iTurns = pContract->GetTurns();
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetContractDuration, m_pPlayer->GetID(), eContract) == GAMEEVENTRETURN_VALUE) 
		{
			if(iValue != -1)
			{
				iTurns = iValue;
			}
		}
		int iMaintenance = pContract->GetMaintenance();
		iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetContractMaintenance, m_pPlayer->GetID(), eContract) == GAMEEVENTRETURN_VALUE) 
		{
			if(iValue != -1)
			{
				iMaintenance = iValue;
			}
		}
		CvContract kContract = CvContract(eContract, m_pPlayer->GetID(), iTurns, iMaintenance);
	
		// Set the contract as active.
		SetActiveContract(eContract, true);

		InitContractUnits(eContract);
	
		//Add to active list.
		GC.getGame().GetGameContracts()->StartContract(kContract);

		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ContractStarted, m_pPlayer->GetID(), eContract, iTurns, iMaintenance);
	}
}

// Destroy contract
void CvPlayerContracts::EndContract(ContractTypes eContract)
{
	CvContractEntry* pContract = GC.getContractInfo(eContract);
	if(pContract)
	{
		SetActiveContract(eContract, false);
	}
}

int CvPlayerContracts::GetContractGoldMaintenance()
{
	int iMaintenance = 0;
	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		CvContractEntry* pContract = GC.getContractInfo((ContractTypes)iI);
		if(pContract && PlayerHasContract((ContractTypes)iI))
		{
			CvContract* pContractInfo = GetContract((ContractTypes)iI);
			if(pContractInfo)
			{
				if(pContractInfo->m_iContractMaintenance != 0)
				{
					iMaintenance += pContractInfo->m_iContractMaintenance;
				}
			}
		}
	}
	return iMaintenance;
}
void CvPlayerContracts::ChangeContractEndTurn(ContractTypes eContract, int iValue)
{
	CvContract* pContract = GetContract(eContract);
	if(pContract && iValue != 0)
	{
		pContract->m_iContractTurns += iValue;
	}
}

bool CvPlayerContracts::UnitIsActiveContractUnit(UnitTypes eUnit)
{
	if(eUnit == NO_UNIT)
		return false;

	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		CvContractEntry* pContract = GC.getContractInfo((ContractTypes)iI);
		if(pContract && PlayerHasContract((ContractTypes)iI))
		{
			CvContract* pContractInfo = GetContract((ContractTypes)iI);
			if(pContractInfo)
			{
				if(GC.getGame().GetContractUnits((ContractTypes)iI, eUnit) > 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CvPlayerContracts::InitContractUnits(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return;

	CvContractEntry* pContract = GC.getContractInfo(eContract);
	if(pContract)
	{
		for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			UnitTypes eUnit = (UnitTypes)iI;
				
			if(eUnit == NO_UNIT)
				continue;

			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if(pkUnitInfo)
			{
				int iNumUnits = GC.getGame().GetContractUnits(eContract, eUnit);
				if(iNumUnits > 0)
				{
					CvCity* pLoopCity;
					CvCity* pBestCity = NULL;
					int iLoop;
					int iBestPop = 0;
					for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if(pLoopCity == NULL)
							continue;

						if(pkUnitInfo->GetDomainType() == DOMAIN_SEA)
						{
							if(!pLoopCity->isCoastal())
								continue;

							if(pLoopCity->isCapital())
							{
								pBestCity = pLoopCity;
								break;
							}
							else if(pLoopCity->getPopulation() > iBestPop)
							{
								pBestCity = pLoopCity;
							}
						}
						else
						{
							if(pLoopCity->isCapital())
							{
								pBestCity = pLoopCity;
								break;
							}
							else if(pLoopCity->getPopulation() > iBestPop)
							{
								pBestCity = pLoopCity;
							}
						}
					}
					if(pBestCity != NULL)
					{
						// Init unit
						CvUnit* pUnit = m_pPlayer->initUnit(eUnit, pBestCity->getX(), pBestCity->getY(), pkUnitInfo->GetDefaultUnitAIType(), NO_DIRECTION, true, true, 0, 0, eContract);
						iNumUnits--;	// Reduce the count since we just added the seed rebel

						// Valid Promotion for this Unit?
						PromotionTypes ePromotion = pContract->GetPromotionType();
						if(pUnit && ePromotion != NO_PROMOTION && ::IsPromotionValidForUnitCombatType(ePromotion, pUnit->getUnitType()))
						{
							pUnit->setHasPromotion(ePromotion, true);
						}

						// Loop until all rebels are placed
						do
						{
							iNumUnits--;

							// Init unit
							pUnit = m_pPlayer->initUnit(eUnit, pBestCity->getX(), pBestCity->getY(), pkUnitInfo->GetDefaultUnitAIType(), NO_DIRECTION, false, true, 0, 0, eContract);
							CvAssert(pUnit);
							if (pUnit)
							{
								if (!pUnit->jumpToNearestValidPlotWithinRange(5))
									pUnit->kill(false);		// Could not find a spot!

								// Valid Promotion for this Unit?
								PromotionTypes ePromotion = pContract->GetPromotionType();
								if(pUnit && ePromotion != NO_PROMOTION && ::IsPromotionValidForUnitCombatType(ePromotion, pUnit->getUnitType()))
								{
									pUnit->setHasPromotion(ePromotion, true);
								}
							}
						}
						while(iNumUnits > 0);
					}
				}
			}
		}
	}
}
void CvPlayerContracts::DisbandContractUnits(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return;

	CvUnit* pLoopUnit;
	int iLoop = 0;

	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(pLoopUnit != NULL && pLoopUnit->getContract() == eContract)
		{
			pLoopUnit->kill(true);
		}
	}
}


int CvPlayerContracts::GetContractTurnsRemaining(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return -1;

	CvContract* pContract = GetContract(eContract);
	if(pContract)
	{
		int iStart = pContract->m_iContractTurnStart;
		int iTurns = pContract->m_iContractTurns;
		int iCurrent = GC.getGame().getGameTurn();
		iStart += iTurns;
		iStart -= iCurrent;
		return iStart;
	}
	return -1;
}
int CvPlayerContracts::GetNumActivePlayerContracts() const
{
	int iNum = 0;
	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		CvContractEntry* pContract = GC.getContractInfo((ContractTypes)iI);
		if(pContract && PlayerHasContract((ContractTypes) iI))
		{
			iNum++;
		}
	}
	return iNum;
}

void CvPlayerContracts::DoTurn()
{
	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		CvContractEntry* pContract = GC.getContractInfo((ContractTypes)iI);
		if(pContract && PlayerHasContract((ContractTypes)iI))
		{
			if(GetContractTurnsRemaining((ContractTypes)iI) <= 0)
			{
				GC.getGame().GetGameContracts()->EndContract((ContractTypes)iI, m_pPlayer->GetID());
			}
		}
	}
}

// Wrapper function to quickly get the ContractEntry of a player's contract
CvContractEntry* CvPlayerContracts::GetContractEntry(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return NULL;

	CvContract* pContract = GetContract(eContract);
	if (!pContract)
		return NULL;

	return GC.getContractInfo(pContract->m_eContract);
}

//=====================================
// CvGameContracts
//=====================================
/// Constructor
CvGameContracts::CvGameContracts(void)
{
}

/// Destructor
CvGameContracts::~CvGameContracts(void)
{
}

// Initialize class data
void CvGameContracts::Init()
{
	Reset();
	for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
	{
		ContractTypes eContract = (ContractTypes)iI;
		if(eContract == NO_CONTRACT)
			continue;

		CvContractEntry* pContract = GC.getContractInfo(eContract);
		if(pContract)
		{
			int iTurns = pContract->GetTurns();
			int iMaintenance = pContract->GetMaintenance();

			CvContract kContract = CvContract(eContract, NO_PLAYER, iTurns, iMaintenance);
			// Add contract to game inactive contracts
			m_InactiveContracts.push_back(kContract);
		}
	}
}

/// Cleanup
void CvGameContracts::Uninit()
{
}

/// Reset
void CvGameContracts::Reset()
{
}

/// Handle turn-by-turn contract updates
void CvGameContracts::DoTurn()
{

}

void CvGameContracts::DoUpdateContracts()
{
	
	//This is expensive, so do it sparingly!
	ContractList::iterator it;
	for(it = m_InactiveContracts.begin(); it != m_InactiveContracts.end(); it++)
	{
		CvContract kContract = (*it);
		for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			UnitTypes eUnit = (UnitTypes)iI;
				
			if(eUnit == NO_UNIT)
				continue;

			CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
			if(pkUnitEntry)
			{
				bool bIn = false;
				int iValue = 0;
				if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetNumContractUnit, it->m_eContract, eUnit) == GAMEEVENTRETURN_VALUE) 
				{		
					if(iValue > 0)
					{
						GC.getGame().SetContractUnits(it->m_eContract, eUnit, iValue);
						bIn = true;
					}
				}
				if(!bIn)
				{
					//Zero out so we don't waste memory.
					GC.getGame().SetContractUnits(it->m_eContract, eUnit, 0);
				}
			}
		}
	}

	GAMEEVENTINVOKE_HOOK(GAMEEVENT_ContractsRefreshed);
}

// Get this contract, or NULL if it does not exist
CvContract* CvGameContracts::GetActiveContract(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return NULL;

	ContractList::iterator it;
	for(it = m_ActiveContracts.begin(); it != m_ActiveContracts.end(); it++)
	{
		if(it->m_eContract == eContract)
		{
			return it;
		}
	}

	return NULL;
}
CvContract* CvGameContracts::GetInactiveContract(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return NULL;

	ContractList::iterator it;
	for(it = m_InactiveContracts.begin(); it != m_InactiveContracts.end(); it++)
	{
		if(it->m_eContract == eContract)
		{
			return it;
		}
	}

	return NULL;
}

int CvGameContracts::GetNumActiveContracts() const
{
	return m_ActiveContracts.size();
}
int CvGameContracts::GetNumInactiveContracts() const
{
	return m_InactiveContracts.size();
}

int CvGameContracts::GetNumAvailableContracts() const
{
	return GC.getNumContractInfos() - GetNumActiveContracts();
}
int CvGameContracts::GetNumUnavailableContracts() const
{
	return GC.getNumContractInfos() - GetNumInactiveContracts();
}
bool CvGameContracts::IsContractActive(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return false;

	ContractList::iterator it;
	for(it = m_ActiveContracts.begin(); it != m_ActiveContracts.end(); it++)
	{
		CvContract kContract = (*it);
		if(kContract.m_eContract == eContract)
		{
			return true;
		}
	}
	return false;
}
bool CvGameContracts::IsContractAvailable(ContractTypes eContract)
{
	if(eContract == NO_CONTRACT)
		return false;

	ContractList::iterator it;
	for(it = m_InactiveContracts.begin(); it != m_InactiveContracts.end(); it++)
	{
		CvContract kContract = (*it);
		if(kContract.m_eContract == eContract)
		{
			return true;
		}
	}
	return false;
}

// Create contract
void CvGameContracts::StartContract(CvContract kContract)
{
	// Add contract to game active contracts
	m_ActiveContracts.push_back(kContract);

	//And remove it from the inactive list.
	ContractList::iterator it;
	for(it = m_InactiveContracts.begin(); it != m_InactiveContracts.end(); it++)
	{
		CvContract kInactiveContract = (*it);
		if(kContract.m_eContract == kInactiveContract.m_eContract)
		{
			m_InactiveContracts.erase(it);
			break;
		}
	}
}
// Destroy contract
void CvGameContracts::EndContract(ContractTypes eContract, PlayerTypes ePlayer)
{
	ContractList::iterator it;
	for(it = m_ActiveContracts.begin(); it != m_ActiveContracts.end(); it++)
	{
		CvContract kContract = (*it);
		if(kContract.m_eContract == eContract && kContract.m_eContractHolder == ePlayer)
		{
			PlayerTypes eHolder = kContract.m_eContractHolder;
			CvPlayer& kPlayer = GET_PLAYER(eHolder);

			// Destroy contract for this player
			kPlayer.GetContracts()->EndContract(eContract);

			//Erase from active list.
			m_ActiveContracts.erase(it);

			GAMEEVENTINVOKE_HOOK(GAMEEVENT_ContractEnded, eHolder, eContract);

			//Initialize values.
			kContract.m_eContractHolder = NO_PLAYER;
			kContract.m_iContractTurnStart = -1;

			// Add contract back to game inactive list
			m_InactiveContracts.push_back(kContract);

			break;
		}
	}
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameContracts& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iEntriesToRead;
	CvContract tempItem;

	writeTo.m_ActiveContracts.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_ActiveContracts.push_back(tempItem);
	}

	writeTo.m_InactiveContracts.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_InactiveContracts.push_back(tempItem);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameContracts& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	ContractList::const_iterator it;
	saveTo << readFrom.m_ActiveContracts.size();
	for(it = readFrom.m_ActiveContracts.begin(); it != readFrom.m_ActiveContracts.end(); it++)
	{
		saveTo << *it;
	}
	saveTo << readFrom.m_InactiveContracts.size();
	for(it = readFrom.m_InactiveContracts.begin(); it != readFrom.m_InactiveContracts.end(); it++)
	{
		saveTo << *it;
	}

	return saveTo;
}

#endif