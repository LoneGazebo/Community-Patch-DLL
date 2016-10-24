#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE)

CvCorporationEntry::CvCorporationEntry(void):
	m_eHeadquartersBuildingClass(NO_BUILDINGCLASS),
	m_eOfficeBuildingClass(NO_BUILDINGCLASS),
	m_eFranchiseBuildingClass(NO_BUILDINGCLASS),
	m_iTradeRouteSeaDistanceModifier(0),
	m_iTradeRouteLandDistanceModifier(0),
	m_iTradeRouteSpeedModifier(0),
	m_iTradeRouteVisionBoost(0),
	m_iNumFreeTradeRoutes(0),
	m_iMaxFranchises(0),
	m_iTradeRouteRecipientBonus(0),
	m_iTradeRouteTargetBonus(0),
	m_bTradeRoutesInvulnerable(false),
	m_piResourceMonopolyAnd(NULL),
	m_piResourceMonopolyOrs(NULL),
	m_piNumFreeResource(NULL),
	m_piTradeRouteMod(NULL),
	m_piTradeRouteCityMod(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_ppaiSpecialistYieldChange(NULL),
	m_ppaiResourceYieldChange(NULL),
	m_strOfficeBenefitHelper("")
{
}

CvCorporationEntry::~CvCorporationEntry(void)
{
	SAFE_DELETE_ARRAY(m_piResourceMonopolyAnd);
	SAFE_DELETE_ARRAY(m_piResourceMonopolyOrs);
	SAFE_DELETE_ARRAY(m_piNumFreeResource);
	SAFE_DELETE_ARRAY(m_piTradeRouteMod);
	SAFE_DELETE_ARRAY(m_piTradeRouteCityMod);
	CvDatabaseUtility::SafeDelete2DArray(m_ppiBuildingClassYieldChanges);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiSpecialistYieldChange);
	CvDatabaseUtility::SafeDelete2DArray(m_ppaiResourceYieldChange);
}

int CvCorporationEntry::GetMaxFranchises() const
{
	return m_iMaxFranchises;
}

BuildingClassTypes CvCorporationEntry::GetHeadquartersBuildingClass() const
{
	return m_eHeadquartersBuildingClass;
}

BuildingClassTypes CvCorporationEntry::GetOfficeBuildingClass() const
{
	return m_eOfficeBuildingClass;
}

BuildingClassTypes CvCorporationEntry::GetFranchiseBuildingClass() const
{
	return m_eFranchiseBuildingClass;
}

int CvCorporationEntry::GetTradeRouteLandDistanceModifier() const
{
	return m_iTradeRouteLandDistanceModifier;
}

int CvCorporationEntry::GetTradeRouteSeaDistanceModifier() const
{
	return m_iTradeRouteSeaDistanceModifier;
}

int CvCorporationEntry::GetTradeRouteSpeedModifier() const
{
	return m_iTradeRouteSpeedModifier;
}

int CvCorporationEntry::GetNumFreeTradeRoutes() const
{
	return m_iNumFreeTradeRoutes;
}

int CvCorporationEntry::GetTradeRouteVisionBoost() const
{
	return m_iTradeRouteVisionBoost;
}

int CvCorporationEntry::GetTradeRouteRecipientBonus() const
{
	return m_iTradeRouteRecipientBonus;
}

int CvCorporationEntry::GetTradeRouteTargetBonus() const
{
	return m_iTradeRouteTargetBonus;
}

bool CvCorporationEntry::IsTradeRoutesInvulnerable() const
{
	return m_bTradeRoutesInvulnerable;
}

int CvCorporationEntry::GetResourceMonopolyAnd(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceMonopolyAnd ? m_piResourceMonopolyAnd[i] : -1;
}

/// Prerequisite resources with OR
int CvCorporationEntry::GetResourceMonopolyOr(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceMonopolyOrs ? m_piResourceMonopolyOrs[i] : -1;
}

int CvCorporationEntry::GetNumFreeResource(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piNumFreeResource ? m_piNumFreeResource[i] : -1;
}

int CvCorporationEntry::GetUnitResourceProductionModifier(int i) const
{
	CvAssertMsg(i < GC.getNUM_BUILDING_RESOURCE_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitResourceProductionModifier ? m_piUnitResourceProductionModifier[i] : -1;
}

/// Yield Modifier for Trade Routes to cities from an office to cities with a Franchise
int CvCorporationEntry::GetTradeRouteMod(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piTradeRouteMod ? m_piTradeRouteMod[i] : -1;
}

int CvCorporationEntry::GetTradeRouteCityMod(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piTradeRouteCityMod ? m_piTradeRouteCityMod[i] : -1;
}

/// Change to Resource yield by type
int CvCorporationEntry::GetResourceYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
}

/// Array of changes to Resource yield
int* CvCorporationEntry::GetResourceYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange[i];
}

/// Change to specialist yield by type
int CvCorporationEntry::GetSpecialistYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppaiSpecialistYieldChange ? m_ppaiSpecialistYieldChange[i][j] : -1;
}

/// Array of changes to specialist yield
int* CvCorporationEntry::GetSpecialistYieldChangeArray(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_ppaiSpecialistYieldChange[i];
}

CvString CvCorporationEntry::GetOfficeBenefitHelper() const
{
	return m_strOfficeBenefitHelper;
}

/// Yield change for a specific BuildingClass by yield type
int CvCorporationEntry::GetBuildingClassYieldChange(int i, int j) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiBuildingClassYieldChanges[i][j];
}

bool CvCorporationEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_iMaxFranchises = kResults.GetInt("MaxFranchises");
	m_iNumFreeTradeRoutes = kResults.GetInt("NumFreeTradeRoutes");
	m_iTradeRouteLandDistanceModifier = kResults.GetInt("TradeRouteLandDistanceModifier");
	m_iTradeRouteSeaDistanceModifier = kResults.GetInt("TradeRouteSeaDistanceModifier");
	m_iTradeRouteSpeedModifier = kResults.GetInt("TradeRouteSpeedModifier");
	m_iTradeRouteVisionBoost = kResults.GetInt("TradeRouteVisionBoost");
	m_bTradeRoutesInvulnerable = kResults.GetBool("TradeRoutesInvulnerable");
	m_iTradeRouteRecipientBonus = kResults.GetInt("TradeRouteRecipientBonus");
	m_iTradeRouteTargetBonus = kResults.GetInt("TradeRouteTargetBonus");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("HeadquartersBuildingClass");
	m_eHeadquartersBuildingClass = (BuildingClassTypes) GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("OfficeBuildingClass");
	m_eOfficeBuildingClass = (BuildingClassTypes) GC.getInfoTypeForString(szTextVal, true);
	
	szTextVal = kResults.GetText("FranchiseBuildingClass");
	m_eFranchiseBuildingClass = (BuildingClassTypes) GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("OfficeBenefitHelper");
	m_strOfficeBenefitHelper = szTextVal;

	// This is not ideal, but Corporations are loaded last, and I want an easy way to tell if a building class is owned by a Corporation
	// Note: Intellisense may lie here! This will compile (declared as friend)
	CvBuildingClassInfo* pkBuildingInfo = GC.getBuildingClassInfo(m_eHeadquartersBuildingClass);
	if (pkBuildingInfo)
	{
		pkBuildingInfo->m_eCorporationType = (CorporationTypes) GetID();
		pkBuildingInfo->m_bIsHeadquarters = true;
	}
	pkBuildingInfo = GC.getBuildingClassInfo(m_eOfficeBuildingClass);
	if (pkBuildingInfo)
	{
		pkBuildingInfo->m_eCorporationType = (CorporationTypes) GetID();
		pkBuildingInfo->m_bIsOffice = true;
	}
	pkBuildingInfo = GC.getBuildingClassInfo(m_eFranchiseBuildingClass);
	if (pkBuildingInfo)
	{
		pkBuildingInfo->m_eCorporationType = (CorporationTypes) GetID();
		pkBuildingInfo->m_bIsFranchise = true;
	}

	const char* szCorporationType = GetType();

	kUtility.PopulateArrayByExistence(m_piResourceMonopolyAnd, "Resources", "Corporation_ResourceMonopolyAnds", "ResourceType", "CorporationType", szCorporationType);
	kUtility.PopulateArrayByExistence(m_piResourceMonopolyOrs, "Resources", "Corporation_ResourceMonopolyOrs", "ResourceType", "CorporationType", szCorporationType);
	kUtility.PopulateArrayByValue(m_piNumFreeResource, "Resources", "Corporation_NumFreeResource", "ResourceType", "CorporationType", szCorporationType, "NumResource");
	kUtility.PopulateArrayByValue(m_piUnitResourceProductionModifier, "Resources", "Corporation_UnitResourceProductionModifier", "ResourceType", "CorporationType", szCorporationType, "Modifier");
	kUtility.SetYields(m_piTradeRouteCityMod, "Corporation_TradeRouteCityYield", "CorporationType", szCorporationType);
	kUtility.SetYields(m_piTradeRouteMod, "Corporation_TradeRouteMod", "CorporationType", szCorporationType);

	//BuildingClassYieldChanges
	{
		kUtility.Initialize2DArray(m_ppiBuildingClassYieldChanges, "BuildingClasses", "Yields");

		std::string strKey("Corporation_BuildingClassYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select BuildingClasses.ID as BuildingClassID, Yields.ID as YieldID, YieldChange from Corporation_BuildingClassYieldChanges inner join BuildingClasses on BuildingClasses.Type = BuildingClassType inner join Yields on Yields.Type = YieldType where CorporationType = ?");
		}

		pResults->Bind(1, szCorporationType);

		while (pResults->Step())
		{
			const int BuildingClassID = pResults->GetInt(0);
			const int iYieldID = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiBuildingClassYieldChanges[BuildingClassID][iYieldID] = iYieldChange;
		}
	}
	//ResourceYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiResourceYieldChange, "Resources", "Yields");

		std::string strKey("Corporation_ResourceYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Resources.ID as ResourceID, Yields.ID as YieldID, Yield from Corporation_ResourceYieldChanges inner join Resources on Resources.Type = ResourceType inner join Yields on Yields.Type = YieldType where CorporationType = ?");
		}

		pResults->Bind(1, szCorporationType);

		while (pResults->Step())
		{
			const int ResourceID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiResourceYieldChange[ResourceID][YieldID] = yield;
		}
	}

	//SpecialistYieldChanges
	{
		kUtility.Initialize2DArray(m_ppaiSpecialistYieldChange, "Specialists", "Yields");

		std::string strKey("Corporation_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Corporation_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where CorporationType = ?");
		}

		pResults->Bind(1, szCorporationType);

		while (pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppaiSpecialistYieldChange[SpecialistID][YieldID] = yield;
		}
	}

	return true;
}

//=====================================
// CvCorporationXMLEntries
//=====================================
/// Constructor
CvCorporationXMLEntries::CvCorporationXMLEntries(void)
{

}

/// Destructor
CvCorporationXMLEntries::~CvCorporationXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of Corporation entries
std::vector<CvCorporationEntry*>& CvCorporationXMLEntries::GetCorporationEntries()
{
	return m_paCorporationEntries;
}

/// Number of defined Corporations
int CvCorporationXMLEntries::GetNumCorporations()
{
	return m_paCorporationEntries.size();
}

/// Clear Corporation entries
void CvCorporationXMLEntries::DeleteArray()
{
	for(std::vector<CvCorporationEntry*>::iterator it = m_paCorporationEntries.begin(); it != m_paCorporationEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paCorporationEntries.clear();
}

/// Get a specific entry
CvCorporationEntry* CvCorporationXMLEntries::GetEntry(int index)
{
	return (index != NO_CORPORATION) ? m_paCorporationEntries[index] : NULL;
}

//=====================================
// CvCorporation
//=====================================

CvCorporation::CvCorporation()
	: m_eCorporation(NO_CORPORATION)
	, m_eFounder(NO_PLAYER)
	, m_iHeadquartersCityX(-1)
	, m_iHeadquartersCityY(-1)
	, m_iTurnFounded(-1)
{

}

CvCorporation::CvCorporation(CorporationTypes eCorporation, PlayerTypes eFounder, CvCity* pHeadquarters)
	: m_eCorporation(eCorporation)
	, m_eFounder(eFounder)
	, m_iHeadquartersCityX(-1)
	, m_iHeadquartersCityY(-1)
{
	if(pHeadquarters)
	{
		m_iHeadquartersCityX = pHeadquarters->getX();
		m_iHeadquartersCityY = pHeadquarters->getY();
	}
	m_iTurnFounded = GC.getGame().getGameTurn();
}

bool CvCorporation::IsCorporationBuilding(BuildingClassTypes eBuildingClass)
{
	CvBuildingClassInfo* pBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	if (pBuildingClassInfo == NULL)
		return false;

	if (pBuildingClassInfo->getCorporationType() == NO_CORPORATION)
		return false;

	return pBuildingClassInfo->getCorporationType() == m_eCorporation;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCorporation& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_eCorporation;
	loadFrom >> writeTo.m_eFounder;
	loadFrom >> writeTo.m_iHeadquartersCityX;
	loadFrom >> writeTo.m_iHeadquartersCityY;

	loadFrom >> writeTo.m_iTurnFounded;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvCorporation& readFrom)
{
	uint uiVersion = 4;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eCorporation;
	saveTo << readFrom.m_eFounder;
	saveTo << readFrom.m_iHeadquartersCityX;
	saveTo << readFrom.m_iHeadquartersCityY;
	saveTo << readFrom.m_iTurnFounded;

	return saveTo;
}

// For some reason CvSerializationInfoHelpers is not working for this.
FDataStream& operator<<(FDataStream& saveTo, const CorporationTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CorporationTypes& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<CorporationTypes>(v);
	return loadFrom;
}

//=====================================
// CvPlayerCorporations
//=====================================
/// Constructor
CvPlayerCorporations::CvPlayerCorporations(void):
	m_pPlayer(NULL),
	m_eFoundedCorporation(NO_CORPORATION),
	m_iNumOffices(0),
	m_iNumFranchises(0),
	m_iAdditionalNumFranchises(0),
	m_iAdditionalNumFranchisesMod(0),
	m_bCorporationOfficesAsFranchises(false),
	m_bCorporationRandomForeignFranchise(false),
	m_bCorporationFreeFranchiseAbovePopular(false)
{
}

	/// Destructor
CvPlayerCorporations::~CvPlayerCorporations(void)
{
	Uninit();
}

/// Initialize class data
void CvPlayerCorporations::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}

/// Cleanup
void CvPlayerCorporations::Uninit()
{
	SetFoundedCorporation(NO_CORPORATION);
	m_iNumFranchises = 0;
	m_iNumOffices = 0;
	m_iAdditionalNumFranchises = 0;
	m_iAdditionalNumFranchisesMod = 0;
	m_bCorporationOfficesAsFranchises = false;
	m_bCorporationRandomForeignFranchise = false;
	m_bCorporationFreeFranchiseAbovePopular = false;
}

/// Reset
void CvPlayerCorporations::Reset()
{
}

/// Serialization read
void CvPlayerCorporations::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	MOD_SERIALIZE_READ(79, kStream, m_eFoundedCorporation, NO_CORPORATION);

	MOD_SERIALIZE_READ(79, kStream, m_iNumOffices, 0);
	MOD_SERIALIZE_READ(79, kStream, m_iNumFranchises, 0);
	MOD_SERIALIZE_READ(79, kStream, m_iAdditionalNumFranchises, 0);
	MOD_SERIALIZE_READ(79, kStream, m_iAdditionalNumFranchisesMod, 0);

	MOD_SERIALIZE_READ(79, kStream, m_bCorporationOfficesAsFranchises, false);
	MOD_SERIALIZE_READ(79, kStream, m_bCorporationRandomForeignFranchise, false);
	MOD_SERIALIZE_READ(79, kStream, m_bCorporationFreeFranchiseAbovePopular, false);
}

/// Serialization write
void CvPlayerCorporations::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	MOD_SERIALIZE_WRITE(kStream, m_eFoundedCorporation);

	MOD_SERIALIZE_WRITE(kStream, m_iNumOffices);
	MOD_SERIALIZE_WRITE(kStream, m_iNumFranchises);
	MOD_SERIALIZE_WRITE(kStream, m_iAdditionalNumFranchises);
	MOD_SERIALIZE_WRITE(kStream, m_iAdditionalNumFranchisesMod);

	MOD_SERIALIZE_WRITE(kStream, m_bCorporationOfficesAsFranchises);
	MOD_SERIALIZE_WRITE(kStream, m_bCorporationRandomForeignFranchise);
	MOD_SERIALIZE_WRITE(kStream, m_bCorporationFreeFranchiseAbovePopular);
}

CvCorporation * CvPlayerCorporations::GetCorporation() const
{
	return GC.getGame().GetGameCorporations()->GetCorporation(m_eFoundedCorporation);
}

// Destroys all offices and franchises associated with this corporation
// Do not call this function outside of CvGameCorporations::DestroyCorporation()! It will mess things up!
void CvPlayerCorporations::DestroyCorporation()
{
	if(!HasFoundedCorporation())
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(m_eFoundedCorporation);
	if(pkCorporationInfo == NULL)
		return;

	BuildingClassTypes eHeadquartersClass = pkCorporationInfo->GetHeadquartersBuildingClass();
	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetHeadquartersBuildingClass();
	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetHeadquartersBuildingClass();

	// Get Corporation Buildings
	BuildingTypes eHeadquarters = (BuildingTypes) m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eHeadquartersClass);
	BuildingTypes eOffice = (BuildingTypes) m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eOfficeClass);
	BuildingTypes eFranchise = (BuildingTypes) m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eFranchiseClass);

	int iLoop = 0;
	// Destroy our headquarters and offices
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pCity == NULL)
			continue;

		// City has headquarters?
		if(pCity->HasBuilding(eHeadquarters))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eHeadquarters, 0);
		}

		// City has office?
		if(pCity->HasBuilding(eOffice))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eOffice, 0);
		}
	}

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop=0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(!GET_PLAYER(eLoopPlayer).isAlive()) continue;
		
		iLoop = 0;
		for(CvCity* pCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
		{
			if(pCity->HasBuilding(eFranchise))
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 0);
			}
		}
	}

	m_pPlayer->processCorporations(m_eFoundedCorporation, -1);

	// Push notification to all players about destroyed Corporation
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iLoopPlayer;
		if (GET_PLAYER(eLoopPlayer).isAlive())
		{
			CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
			if (pNotifications && m_pPlayer->GetID())
			{
				Localization::String strSummary;
				Localization::String strMessage;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_DESTROYED_SUMMARY");
				strSummary << pkCorporationInfo->GetDescription();
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_DESTROYED");
				strMessage << pkCorporationInfo->GetDescription();
				pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1, -1);
			}
		}
	}

	Uninit();
}

bool CvPlayerCorporations::IsCorporationOfficesAsFranchises() const
{
	return m_bCorporationOfficesAsFranchises;
}

void CvPlayerCorporations::SetCorporationOfficesAsFranchises(bool bValue)
{
	if(bValue != m_bCorporationOfficesAsFranchises)
		m_bCorporationOfficesAsFranchises = bValue;
}

bool CvPlayerCorporations::IsCorporationRandomForeignFranchise() const
{
	return m_bCorporationRandomForeignFranchise;
}

void CvPlayerCorporations::SetCorporationRandomForeignFranchise(bool bValue)
{
	if (bValue != m_bCorporationRandomForeignFranchise)
		m_bCorporationRandomForeignFranchise = bValue;
}

bool CvPlayerCorporations::IsCorporationFreeFranchiseAbovePopular() const
{
	return m_bCorporationFreeFranchiseAbovePopular;
}

void CvPlayerCorporations::SetCorporationFreeFranchiseAbovePopular(bool bValue)
{
	if(bValue != m_bCorporationFreeFranchiseAbovePopular)
		m_bCorporationFreeFranchiseAbovePopular = bValue;
}

// Get our headquarters
CvCity* CvPlayerCorporations::GetHeadquarters() const
{
	if(!HasFoundedCorporation())
		return NULL;

	CvCorporation* pCorporation = GC.getGame().GetGameCorporations()->GetCorporation(GetFoundedCorporation());
	if(pCorporation)
	{
		CvPlot* pHQPlot = GC.getMap().plot(pCorporation->m_iHeadquartersCityX, pCorporation->m_iHeadquartersCityY);
		if(pHQPlot)
		{
			return pHQPlot->getPlotCity();
		}
	}

	return NULL;
}

void CvPlayerCorporations::DoTurn()
{
	if (!HasFoundedCorporation())
		return;

	// Are you free enough?
	if (IsCorporationRandomForeignFranchise())
		BuildRandomFranchiseInCity();

	RecalculateNumOffices();
	RecalculateNumFranchises();
}

void CvPlayerCorporations::RecalculateNumOffices()
{
	CorporationTypes eCorporation = GetFoundedCorporation();
	if (eCorporation == NO_CORPORATION)
		return;
	
	int iOffices = 0;
	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity != NULL)
		{
			if (pLoopCity->IsHasOffice())
			{
				iOffices++;
			}
		}
	}

	m_iNumOffices = iOffices;
}

// Recalculate number of franchises
void CvPlayerCorporations::RecalculateNumFranchises()
{
	CorporationTypes eCorporation = GetFoundedCorporation();
	if (eCorporation == NO_CORPORATION)
		return;

	// TODO: figure out a better way to get this
	int iFranchises = 0;
	int iMaxFranchises = GetMaxNumFranchises();
	int iFreeFranchises = 0; // From Infiltration

	// Search all players for Franchises
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_CIV_PLAYERS; iLoopPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
		if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isBarbarian())
		{
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
			{
				if (pLoopCity != NULL)
				{
					if (pLoopCity->IsHasFranchise(eCorporation))
					{
						iFranchises++;

						// Free franchise above Popular?
						if (IsCorporationFreeFranchiseAbovePopular() && m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_POPULAR)
						{
							iFreeFranchises++;
						}
					}
				}
			}
		}
	}

	iFranchises = std::min(iFranchises, iMaxFranchises);
	iFranchises += iFreeFranchises;

	m_iNumFranchises = iFranchises;

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		YieldTypes eYield;

		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			eYield = (YieldTypes) iI;
			pLoopCity->UpdateYieldFromCorporationFranchises(eYield);
		}
	}
}

// Attempt to spread a corporation to pDestCity
void CvPlayerCorporations::BuildFranchiseInCity(CvCity* pOriginCity, CvCity* pDestCity)
{
	CorporationTypes eCorporation = GetFoundedCorporation();
	if (eCorporation == NO_CORPORATION)
		return;

	if (pOriginCity == NULL)
		return;

	if (pDestCity == NULL)
		return;

	PlayerTypes eOriginPlayer = pOriginCity->getOwner();
	CvPlayer& kOriginPlayer = GET_PLAYER(eOriginPlayer);

	PlayerTypes eDestPlayer = pDestCity->getOwner();
	CvPlayer& kDestPlayer = GET_PLAYER(eDestPlayer);

	// Cannot spread a franchise to yourself
	if (eOriginPlayer == eDestPlayer)
		return;

	// We are at our limit: do not establish another franchise
	if (GetNumFranchises() >= GetMaxNumFranchises())
		return;

	// Get our associated Corporation office
	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (pkCorporationInfo == NULL)
		return;

	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetOfficeBuildingClass();
	if (eOfficeClass == NO_BUILDINGCLASS)
		return;

	// Origin city must contain an office
	if (!pOriginCity->HasBuildingClass(eOfficeClass))
		return;

	BuildingTypes eFranchise = (BuildingTypes) m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetFranchiseBuildingClass());
	if (eFranchise == NO_BUILDING)
		return;

	// Destination city cannot have a franchise
	if (pDestCity->HasBuilding(eFranchise))
		return;

	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eFranchise);
	if (pBuildingInfo == NULL)
		return;

	// If we've passed all the checks above, we are ready to go!
	pDestCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 1);

	RecalculateNumFranchises();

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		pOriginCity->UpdateYieldFromCorporationFranchises((YieldTypes)iI);
		pDestCity->UpdateYieldFromCorporationFranchises((YieldTypes)iI);
	}

	// send notification to owner player and target player
	CvNotifications* pNotifications = NULL;
	if (eOriginPlayer == GC.getGame().getActivePlayer())
	{
		pNotifications = kOriginPlayer.GetNotifications();
		if (pNotifications)
		{
			Localization::String strSummary;
			Localization::String strMessage;

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_SUMMARY");
			strSummary << pBuildingInfo->GetTextKey();
			strSummary << pDestCity->getNameKey();
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING");
			strMessage << pDestCity->getNameKey();
			strMessage << pBuildingInfo->GetTextKey();
			strMessage << pOriginCity->getNameKey();
			pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pDestCity->getX(), pDestCity->getY(), -1, -1);
		}
	}
	else if (eDestPlayer == GC.getGame().getActivePlayer())
	{
		pNotifications = kDestPlayer.GetNotifications();
		if (pNotifications)
		{
			Localization::String strSummary;
			Localization::String strMessage;

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FOREIGN_SUMMARY");
			strSummary << pBuildingInfo->GetTextKey();
			strSummary << pDestCity->getNameKey();
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FOREIGN");
			strMessage << pDestCity->getNameKey();
			strMessage << pBuildingInfo->GetTextKey();
			strMessage << pOriginCity->getNameKey();
			pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pDestCity->getX(), pDestCity->getY(), -1, -1);
		}
	}

	if ((GC.getLogging() && GC.getAILogging()))
	{
		CvString strLogString;
		strLogString.Format("Spread Corporate Building via Owner Trade Route. City: %s. Building: %s.", pDestCity->getName().c_str(), pBuildingInfo->GetText());
		GET_PLAYER(pOriginCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLogString);
	}
}

void CvPlayerCorporations::BuildRandomFranchiseInCity()
{
	if (!HasFoundedCorporation())
		return;

	int iFranchises = GetNumFranchises();
	int iMaxFranchises = GetMaxNumFranchises();

	// Cannot build a franchise because we are at the limit
	if (iFranchises >= iMaxFranchises)
		return;

	CvCorporationEntry* pkCorporation = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporation == NULL)
		return;

	BuildingTypes eFranchiseBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporation->GetFranchiseBuildingClass());
	if (eFranchiseBuilding == NO_BUILDING)
		return;

	CvCity* pBestCity = 0;
	int iBestScore = 0;
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_CIV_PLAYERS; iLoopPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;

		if (!GET_PLAYER(ePlayer).isAlive())
			continue;

		if (GET_PLAYER(ePlayer).isBarbarian())
			continue;

		if (GET_PLAYER(ePlayer).GetID() == m_pPlayer->GetID())
			continue;

		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if (pLoopCity != NULL)
			{
				// City can not contain our franchise already
				if (!pLoopCity->IsHasFranchise(GetFoundedCorporation()))
				{
					int iScore = GC.getGame().getJonRandNum(100, "Random Corp Spread");
					if (m_pPlayer->GetTrade()->IsConnectedToPlayer(pLoopCity->getOwner()))
					{
						iScore += GC.getGame().getJonRandNum(100, "Random Corp Spread");
					}
					if (pCapital != NULL)
					{
						//Prioritize closer cities first.
						int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCapital->getX(), pCapital->getY());
						iScore -= (iDistance * 5);
					}
					if (iScore > iBestScore)
					{
						iBestScore = iScore;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}
	if (pBestCity != NULL && iBestScore != 0)
	{
		int iSpreadChance = GC.getGame().getJonRandNum((1500 + (GetNumFranchises() * 10)), "Random Corp Spread");
		if (iSpreadChance <= iBestScore)
		{
			CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eFranchiseBuilding);
			if (pBuildingInfo)
			{
				pBestCity->GetCityBuildings()->SetNumRealBuilding(eFranchiseBuilding, 1);

				for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					pBestCity->UpdateYieldFromCorporationFranchises((YieldTypes)iI);
					CvCity* pLoopCity;
					int iLoop;
					for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if (pLoopCity != NULL)
						{
							pLoopCity->UpdateYieldFromCorporationFranchises((YieldTypes)iI);
						}
					}
				}

				// send notification to owner player and target player
				if (m_pPlayer->GetID() == GC.getGame().getActivePlayer())
				{
					CvNotifications* pNotifications = m_pPlayer->GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary;
						Localization::String strMessage;

						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FREEDOM_SUMMARY");
						strSummary << pBuildingInfo->GetTextKey();
						strSummary << pBestCity->getNameKey();
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FREEDOM");
						strMessage << pBestCity->getNameKey();
						strMessage << pBuildingInfo->GetTextKey();
						pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pBestCity->getX(), pBestCity->getY(), -1, -1);
					}
				}
				else if(pBestCity->getOwner() == GC.getGame().getActivePlayer())
				{
					CvNotifications* pNotifications = GET_PLAYER(pBestCity->getOwner()).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary;
						Localization::String strMessage;

						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FOREIGN_FREEDOM_SUMMARY");
						strSummary << pBuildingInfo->GetTextKey();
						strSummary << pBestCity->getNameKey();
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_CORPORATION_BUILDING_FOREIGN_FREEDOM");
						strMessage << pBestCity->getNameKey();
						strMessage << pBuildingInfo->GetTextKey();
						strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pBestCity->getX(), pBestCity->getY(), -1, -1);
					}
				}
				RecalculateNumFranchises();

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Spread Corporate Building via Freedom Function. City: %s. Building: %s.", pBestCity->getName().c_str(), pBuildingInfo->GetText());
					m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

// Returns a string representing our current corporation benefit (if we have one) for the Corporation Overview
CvString CvPlayerCorporations::GetCurrentOfficeBenefit()
{
	if (!HasFoundedCorporation())
		return "";

	CvString szOfficeBenefit = "";

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporationInfo == NULL)
		return "";

	// Calculate what our input into the corporation helper we need
	int iCurrentValue = 0;
	int iNumFranchises = GetNumFranchises();

	bool bFoundOne = false;

	BuildingTypes eOffice = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetOfficeBuildingClass());
	CvBuildingEntry* pkOfficeInfo = GC.getBuildingInfo(eOffice);
	if (pkOfficeInfo == NULL)
		return "";

	// Note: only look for one number, if we find one, don't consider others.
	if (iNumFranchises > 0)
	{
		// Civilized Jewelers
		if (pkOfficeInfo->GetGPRateModifierPerXFranchises() > 0)
		{
			iCurrentValue = iNumFranchises * pkOfficeInfo->GetGPRateModifierPerXFranchises();
			bFoundOne = true;
		}

		if (!bFoundOne)
		{
			// Free Resource?
			for (int iI = 0; iI < GC.getNumResourceInfos(); iI++)
			{
				ResourceTypes eResource = (ResourceTypes)iI;
				if (pkOfficeInfo->GetResourceQuantityPerXFranchises(eResource) > 0)
				{
					iCurrentValue = iNumFranchises / pkOfficeInfo->GetResourceQuantityPerXFranchises(eResource);
					bFoundOne = true;
					break;
				}
			}
		}

		if (!bFoundOne)
		{
			// Yield Change?
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes)iI;
				if (pkOfficeInfo->GetYieldPerFranchise(eYield) > 0)
				{
					iCurrentValue = iNumFranchises * pkOfficeInfo->GetYieldPerFranchise(eYield);
					bFoundOne = true;
					break;
				}
			}
		}
	}

	szOfficeBenefit = GetLocalizedText(pkCorporationInfo->GetOfficeBenefitHelper(), iCurrentValue);

	return szOfficeBenefit;
}

// Wrapper function to quickly get the CorporationEntry of a player's corporation
CvCorporationEntry * CvPlayerCorporations::GetCorporationEntry() const
{
	CvCorporation* pCorporation = GetCorporation();
	if (!pCorporation)
		return NULL;

	return GC.getCorporationInfo(pCorporation->m_eCorporation);
}

// How many franchises can we establish?
int CvPlayerCorporations::GetMaxNumFranchises() const
{
	if (!HasFoundedCorporation())
		return 0;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporationInfo == NULL)
		return 0;

	if (IsCorporationOfficesAsFranchises())
		return m_pPlayer->getNumCities();

	int iReturnValue = 0;

	// We cannot have more franchises than twice the number of trade routes we can establish
	int iMaximum = m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() * 2;

	iReturnValue = iMaximum * GC.getMap().getWorldInfo().GetEstimatedNumCities();
	iReturnValue /= 100;

	// Do not go above the defined maximum
	iReturnValue = std::min(iReturnValue, iMaximum);

	// Do not go above the defined maximum for this corporation info
	iReturnValue = std::min(iReturnValue, pkCorporationInfo->GetMaxFranchises());

	// Add in any "bonus" franchises from policies
	iReturnValue += GetAdditionalNumFranchises();

	int iFreeFranchises = 0; // From Infiltration

	// Search all players for Franchises (Autocracy)
	if(IsCorporationFreeFranchiseAbovePopular())
	{
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_CIV_PLAYERS; iLoopPlayer++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
			if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isBarbarian())
			{
				if(m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) < INFLUENCE_LEVEL_POPULAR)
					continue;

				CvCity* pLoopCity;
				int iLoop;
				for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
				{
					if (pLoopCity != NULL)
					{
						if (pLoopCity->IsHasFranchise(GetFoundedCorporation()))
						{
							iFreeFranchises++;
						}
					}
				}
			}
		}
		iReturnValue += iFreeFranchises;
	}

	int iModifier = 100 + GetAdditionalNumFranchisesMod();
	
	iReturnValue *= iModifier;
	iReturnValue /= 100;

	return std::max(0, iReturnValue);
}

// Clear our Corporation from pCity
void CvPlayerCorporations::ClearCorporationFromCity(CvCity* pCity)
{
	if (pCity == NULL)
		return;

	CorporationTypes eCorporation = GetFoundedCorporation();
	if (eCorporation == NO_CORPORATION)
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (pkCorporationInfo == NULL)
		return;

	// Explicitly destroy all corporation buildings from this city
	BuildingTypes eHeadquarters = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetHeadquartersBuildingClass());
	if (eHeadquarters != NO_BUILDING)
	{
		if (pCity->HasBuilding(eHeadquarters))
		{
			// Lost HQ? Destroy everything! Mwhahaha
			GC.getGame().GetGameCorporations()->DestroyCorporation(m_eFoundedCorporation);
			// Can return now, we've already done the job
			return;
		}
	}


	BuildingTypes eOffice = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetHeadquartersBuildingClass());
	if (eOffice != NO_BUILDING)
	{
		if (pCity->HasBuilding(eOffice))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eOffice, 0);
		}
	}

	BuildingTypes eFranchise = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetHeadquartersBuildingClass());
	if (eFranchise != NO_BUILDING)
	{
		if (pCity->HasBuilding(eFranchise))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 0);
		}
	}
}

int CvPlayerCorporations::GetAdditionalNumFranchises() const
{
	return m_iAdditionalNumFranchises;
}

int CvPlayerCorporations::GetAdditionalNumFranchisesMod() const
{
	return m_iAdditionalNumFranchisesMod;
}

void CvPlayerCorporations::ChangeAdditionalNumFranchisesMod(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalNumFranchisesMod = m_iAdditionalNumFranchisesMod + iChange;
	}
}

void CvPlayerCorporations::ChangeAdditionalNumFranchises(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalNumFranchises = m_iAdditionalNumFranchises + iChange;
	}
}

int CvPlayerCorporations::GetNumFranchises() const
{
	return m_iNumFranchises;
}

int CvPlayerCorporations::GetNumOffices() const
{
	return m_iNumOffices;
}

void CvPlayerCorporations::SetFoundedCorporation(CorporationTypes eCorporation)
{
	m_eFoundedCorporation = eCorporation;
}

bool CvPlayerCorporations::HasFoundedCorporation() const
{
	return GetFoundedCorporation() != NO_CORPORATION;
}

CorporationTypes CvPlayerCorporations::GetFoundedCorporation() const
{
	return m_eFoundedCorporation;
}

//=====================================
// CvGameCorporations
//=====================================
/// Constructor
CvGameCorporations::CvGameCorporations(void)
{
}

/// Destructor
CvGameCorporations::~CvGameCorporations(void)
{

}

// Initialize class data
void CvGameCorporations::Init()
{

}

/// Handle turn-by-turn religious updates
void CvGameCorporations::DoTurn()
{
}

// Get this Corporation, or NULL if it does not exist
CvCorporation* CvGameCorporations::GetCorporation(CorporationTypes eCorporation)
{
	CorporationList::iterator it;
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); it++)
	{
		if(it->m_eCorporation == eCorporation)
		{
			return it;
		}
	}

	return NULL;
}

int CvGameCorporations::GetNumActiveCorporations() const
{
	return m_ActiveCorporations.size();
}

int CvGameCorporations::GetNumAvailableCorporations() const
{
	return GC.getNumCorporationInfos() - GetNumActiveCorporations();
}

// Destroy eCorporation
void CvGameCorporations::DestroyCorporation(CorporationTypes eCorporation)
{
	CorporationList::iterator it;
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); it++)
	{
		CvCorporation kCorporation = (*it);
		if(kCorporation.m_eCorporation == eCorporation)
		{
			PlayerTypes eCorporationFounder = kCorporation.m_eFounder;
			CvPlayer& kPlayer = GET_PLAYER(eCorporationFounder);

			// Destroy corporation for this player
			kPlayer.GetCorporations()->DestroyCorporation();

			m_ActiveCorporations.erase(it);
			break;
		}
	}
}

// ePlayer founds eCorporation
void CvGameCorporations::FoundCorporation(PlayerTypes ePlayer, CorporationTypes eCorporation, CvCity* pHeadquarters)
{
	// Cannot found this corporation
	if(!CanFoundCorporation(ePlayer, eCorporation))
		return;

	// Cannot found a corporation without a headquarters!
	if(pHeadquarters == NULL || pHeadquarters->getOwner() != ePlayer)
		return;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvCorporation kCorporation = CvCorporation(eCorporation, ePlayer, pHeadquarters);
	
	// Set the player's founded corporation
	kPlayer.GetCorporations()->SetFoundedCorporation(eCorporation);
	
	// Free office in headquarters
	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetOfficeBuildingClass();
	if(eOfficeClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eOffice = (BuildingTypes) GET_PLAYER(ePlayer).getCivilizationInfo().getCivilizationBuildings(eOfficeClass);
		pHeadquarters->GetCityBuildings()->SetNumFreeBuilding(eOffice, 1);
	}

	kPlayer.processCorporations(eCorporation, 1);

	// Add corporation to game active corporations
	m_ActiveCorporations.push_back(kCorporation);

	// Push notification to all players
	PlayerTypes eLoopPlayer = NO_PLAYER;
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		eLoopPlayer = (PlayerTypes) iI;
			
		if(!GET_PLAYER(eLoopPlayer).isAlive())
			continue;

		CvNotifications* pNotification = GET_PLAYER(eLoopPlayer).GetNotifications();
		if(!pNotification)
			continue;

		// We founded the Corporation
		if(ePlayer == eLoopPlayer)
		{
			Localization::String strMessage;
			Localization::String strSummary;
			strMessage = Localization::Lookup("TXT_KEY_CORPORATION_FOUNDED");
			strMessage << (pkCorporationInfo->GetDescription());
			strSummary = Localization::Lookup("TXT_KEY_CORPORATION_FOUNDED_SUMMARY");
			if(pNotification)
			{
				pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1, ePlayer);
			}
		}
		// Someone else founded the Corporation
		else
		{
			Localization::String strMessage;
			Localization::String strSummary;
			strMessage = Localization::Lookup("TXT_KEY_CORPORATION_FOUNDED_OTHER_PLAYER");
			strSummary = Localization::Lookup("TXT_KEY_CORPORATION_FOUNDED_OTHER_PLAYER_SUMMARY");
			// Notify player has met this team
			if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				CvPlayerAI& player = GET_PLAYER(ePlayer);
				if(GC.getGame().isGameMultiPlayer() && player.isHuman())
				{
					strMessage << player.getNickName();
					strSummary << player.getNickName();
				}
				else
				{
					strMessage << player.getName();
					strSummary << player.getName();
				}
			}
			// Has not met this team
			else
			{
				Localization::String unmetPlayer = Localization::Lookup("TXT_KEY_UNMET_PLAYER");
				strMessage << unmetPlayer;
				strSummary << unmetPlayer;
			}

			strMessage << (pkCorporationInfo->GetDescriptionKey());
			 
			if(pNotification)
			{
				pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1, ePlayer);
			}
		}
	}
}

// Can eCorporation be founded?
bool CvGameCorporations::CanFoundCorporation(PlayerTypes ePlayer, CorporationTypes eCorporation) const
{
	// Player must be alive
	if(!GET_PLAYER(ePlayer).isAlive())
		return false;

	// Only major civs
	if(GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
		return false;

	// Must be a valid corporation
	if(eCorporation < 0 || eCorporation >= GC.getNumCorporationInfos())
		return false;

	// Corporation cannot be founded
	if(IsCorporationFounded(eCorporation))
		return false;

	return true;
}

// Has eCorporation been founded yet?
bool CvGameCorporations::IsCorporationFounded(CorporationTypes eCorporation) const
{
	CorporationList::const_iterator it;
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); it++)
	{
		if((*it).m_eCorporation == eCorporation)
			return true;
	}

	return false;
}

// Is pCity a Corporations headquarters?
bool CvGameCorporations::IsCorporationHeadquarters(CvCity* pCity) const
{
	if(pCity == NULL)
		return false;

	CorporationList::const_iterator it;
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); it++)
	{
		if((*it).m_iHeadquartersCityX == pCity->getX() &&
			(*it).m_iHeadquartersCityY == pCity->getY())
			return true;
	}

	return false;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameCorporations& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iEntriesToRead;
	CvCorporation tempItem;

	writeTo.m_ActiveCorporations.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_ActiveCorporations.push_back(tempItem);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameCorporations& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	CorporationList::const_iterator it;
	saveTo << readFrom.m_ActiveCorporations.size();
	for(it = readFrom.m_ActiveCorporations.begin(); it != readFrom.m_ActiveCorporations.end(); it++)
	{
		saveTo << *it;
	}

	return saveTo;
}

#endif