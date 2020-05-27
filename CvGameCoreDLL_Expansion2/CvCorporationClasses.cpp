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
	m_iBaseFranchises(0),
	m_iTradeRouteRecipientBonus(0),
	m_iTradeRouteTargetBonus(0),
	m_iBaseSpreadChance(0),
	m_iTourismMod(0),
	m_bTradeRoutesInvulnerable(false),
	m_piResourceMonopolyAnd(NULL),
	m_piResourceMonopolyOrs(NULL),
	m_piNumFreeResource(NULL),
	m_piTradeRouteMod(NULL),
	m_piTradeRouteCityMod(NULL),
	m_ppiBuildingClassYieldChanges(NULL),
	m_ppaiSpecialistYieldChange(NULL),
	m_ppaiResourceYieldChange(NULL),
	m_strOfficeBenefitHelper(""),
	m_strTradeRouteBenefitHelper("")
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

int CvCorporationEntry::GetBaseFranchises() const
{
	return m_iBaseFranchises;
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

int CvCorporationEntry::GetRandomSpreadChance() const
{
	return m_iBaseSpreadChance;
}

int CvCorporationEntry::GetTourismMod() const
{
	return m_iTourismMod;
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

CvString CvCorporationEntry::GetTradeRouteBenefitHelper() const
{
	return m_strTradeRouteBenefitHelper;
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

	m_iBaseFranchises = kResults.GetInt("BaseFranchises");
	m_iNumFreeTradeRoutes = kResults.GetInt("NumFreeTradeRoutes");
	m_iTradeRouteLandDistanceModifier = kResults.GetInt("TradeRouteLandDistanceModifier");
	m_iTradeRouteSeaDistanceModifier = kResults.GetInt("TradeRouteSeaDistanceModifier");
	m_iTradeRouteSpeedModifier = kResults.GetInt("TradeRouteSpeedModifier");
	m_iTradeRouteVisionBoost = kResults.GetInt("TradeRouteVisionBoost");
	m_bTradeRoutesInvulnerable = kResults.GetBool("TradeRoutesInvulnerable");
	m_iTradeRouteRecipientBonus = kResults.GetInt("TradeRouteRecipientBonus");
	m_iTradeRouteTargetBonus = kResults.GetInt("TradeRouteTargetBonus");
	m_iBaseSpreadChance = kResults.GetInt("RandomSpreadChance");
	m_iTourismMod = kResults.GetInt("TourismModNumFranchises");

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

	szTextVal = kResults.GetText("TradeRouteBonusHelp");
	m_strTradeRouteBenefitHelper = szTextVal;

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
	m_iCorporationOfficesAsFranchises(0),
	m_iCorporationRandomForeignFranchiseMod(0),
	m_iCorporationFreeFranchiseAbovePopular(0),
	m_bIsNoForeignCorpsInCities(false),
	m_bIsNoFranchisesInForeignCities(false)
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
	m_iCorporationOfficesAsFranchises = 0;
	m_iCorporationRandomForeignFranchiseMod = 0;
	m_iCorporationFreeFranchiseAbovePopular = 0;
	m_bIsNoForeignCorpsInCities = false;
	m_bIsNoFranchisesInForeignCities = false;
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

	MOD_SERIALIZE_READ(79, kStream, m_iCorporationOfficesAsFranchises, 0);
	MOD_SERIALIZE_READ(79, kStream, m_iCorporationRandomForeignFranchiseMod, 0);
	MOD_SERIALIZE_READ(79, kStream, m_iCorporationFreeFranchiseAbovePopular, 0);
	MOD_SERIALIZE_READ(79, kStream, m_bIsNoForeignCorpsInCities, false);
	MOD_SERIALIZE_READ(79, kStream, m_bIsNoFranchisesInForeignCities, false);

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

	MOD_SERIALIZE_WRITE(kStream, m_iCorporationOfficesAsFranchises);
	MOD_SERIALIZE_WRITE(kStream, m_iCorporationRandomForeignFranchiseMod);
	MOD_SERIALIZE_WRITE(kStream, m_iCorporationFreeFranchiseAbovePopular);

	MOD_SERIALIZE_WRITE(kStream, m_bIsNoForeignCorpsInCities);
	MOD_SERIALIZE_WRITE(kStream, m_bIsNoFranchisesInForeignCities);

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
	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetOfficeBuildingClass();
	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetFranchiseBuildingClass();

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
			GC.getGame().decrementBuildingClassCreatedCount(eHeadquartersClass);
		}

		// City has office?
		if(pCity->HasBuilding(eOffice))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eOffice, 0);
		}
	}

	//and destroy our franchises too!
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

int CvPlayerCorporations::GetCorporationOfficesAsFranchises() const
{
	return m_iCorporationOfficesAsFranchises;
}

void CvPlayerCorporations::SetCorporationOfficesAsFranchises(int iValue)
{
	if (iValue != 0)
	{
		m_iCorporationOfficesAsFranchises += iValue;
	}
}

int CvPlayerCorporations::GetCorporationRandomForeignFranchiseMod() const
{
	return m_iCorporationRandomForeignFranchiseMod;
}

void CvPlayerCorporations::SetCorporationRandomForeignFranchiseMod(int iValue)
{
	if (iValue != 0)
		m_iCorporationRandomForeignFranchiseMod += iValue;
}

int CvPlayerCorporations::GetCorporationFreeFranchiseAbovePopular() const
{
	return m_iCorporationFreeFranchiseAbovePopular;
}

void CvPlayerCorporations::SetCorporationFreeFranchiseAbovePopular(int iValue)
{
	if (iValue != 0)
		m_iCorporationFreeFranchiseAbovePopular += iValue;
}

bool CvPlayerCorporations::IsNoForeignCorpsInCities()
{
	return m_bIsNoForeignCorpsInCities;
}
void CvPlayerCorporations::SetNoForeignCorpsInCities(bool bValue)
{
	if (bValue != m_bIsNoForeignCorpsInCities)
	{
		m_bIsNoForeignCorpsInCities = bValue;
		if (bValue)
		{
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity != NULL)
				{
					ClearCorporationFromCity(pLoopCity, GetFoundedCorporation(), true);
				}
			}
		}
	}
}
bool CvPlayerCorporations::IsNoFranchisesInForeignCities()
{
	return m_bIsNoFranchisesInForeignCities;
}
void CvPlayerCorporations::SetNoFranchisesInForeignCities(bool bValue)
{
	if (bValue != m_bIsNoFranchisesInForeignCities)
	{
		m_bIsNoFranchisesInForeignCities = bValue;
		if (bValue)
		{
			ClearCorporationFromForeignCities();
		}
	}
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

	if (GetCorporationOfficesAsFranchises() > 0)
	{
		m_iNumFranchises = GetNumOffices() * GetCorporationOfficesAsFranchises();
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			YieldTypes eYield;

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				eYield = (YieldTypes)iI;
				pLoopCity->UpdateYieldFromCorporationFranchises(eYield);
			}
		}
		return;
	}

	// TODO: figure out a better way to get this
	int iFranchises = 0;
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
						if (GetCorporationFreeFranchiseAbovePopular() != 0 && m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_POPULAR)
						{
							iFreeFranchises += GetCorporationFreeFranchiseAbovePopular();
						}
					}
				}
			}
		}
	}

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

	// Get our associated Corporation office
	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (pkCorporationInfo == NULL)
		return;

	if (!CanCreateFranchiseInCity(pOriginCity, pDestCity))
		return;

	BuildingTypes eFranchise = (BuildingTypes) m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetFranchiseBuildingClass());
	if (eFranchise == NO_BUILDING)
		return;

	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eFranchise);
	if (pBuildingInfo == NULL)
		return;

	// If we've passed all the checks above, we are ready to go!
	pDestCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 1, true);

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
		LogCorporationMessage(strLogString);
	}
}

void CvPlayerCorporations::BuildRandomFranchiseInCity()
{
	if (!HasFoundedCorporation())
		return;

	bool bIsVassalOrMaster = GET_TEAM(m_pPlayer->getTeam()).IsVassalOfSomeone() || GET_TEAM(m_pPlayer->getTeam()).GetMaster() != NO_TEAM;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague && !bIsVassalOrMaster && pLeague->IsPlayerEmbargoed(m_pPlayer->GetID()))
		return;

	int iFranchises = GetNumFranchises();
	int iMaxFranchises = GetMaxNumFranchises();

	// Cannot build a franchise because we are at the limit
	if (iFranchises >= iMaxFranchises)
		return;

	if (GetCorporationOfficesAsFranchises() > 0)
		return;

	CvCorporationEntry* pkCorporation = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporation == NULL)
		return;

	BuildingTypes eFranchiseBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporation->GetFranchiseBuildingClass());
	if (eFranchiseBuilding == NO_BUILDING)
		return;

	int iBaseChance = pkCorporation->GetRandomSpreadChance();
	iBaseChance += GetCorporationRandomForeignFranchiseMod();

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

		if (!m_pPlayer->GetTrade()->IsConnectedToPlayer(ePlayer))
			continue;

		//does not affect vassals
		bool bMasterVassalRelationship = GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(ePlayer).getTeam()) || GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsVassal(m_pPlayer->getTeam());

		if (pLeague && !bMasterVassalRelationship && pLeague->IsPlayerEmbargoed(ePlayer))
			continue;

		if (GET_PLAYER(ePlayer).GetCorporations()->IsNoForeignCorpsInCities())
			continue;

		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if (pLoopCity != NULL)
			{
				if (m_pPlayer->GetTrade()->IsCityAlreadyConnectedByTrade(pLoopCity))
					continue;

				// City can not contain our franchise already
				if (pLoopCity->IsHasFranchise(GetFoundedCorporation()))
					continue;

				int iChance = GC.getGame().getSmallFakeRandNum(100, *pLoopCity->plot()) + GC.getGame().getSmallFakeRandNum(100, pLoopCity->getFood());

				int iScore = 500 - iChance;

				int iLoop2;
				for (CvCity* pLoopCity2 = m_pPlayer->firstCity(&iLoop2); pLoopCity2 != NULL; pLoopCity2 = m_pPlayer->nextCity(&iLoop2))
				{
					if (pLoopCity2 != NULL)
					{
						if (!m_pPlayer->GetTrade()->CanCreateTradeRoute(pLoopCity2, pLoopCity, DOMAIN_LAND, TRADE_CONNECTION_INTERNATIONAL, false) && !m_pPlayer->GetTrade()->CanCreateTradeRoute(pLoopCity2, pLoopCity, DOMAIN_SEA, TRADE_CONNECTION_INTERNATIONAL, false))
							continue;

						//Prioritize closer cities first.
						int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCapital->getX(), pCapital->getY());
						iScore -= (iDistance * 5);
						if (iScore <= 0)
							iScore = 1;
					}
				}
				if (iScore > iBestScore)
				{
					iBestScore = iScore;
					pBestCity = pLoopCity;
				}
			}
		}
	}
	if (pBestCity != NULL && iBestScore != 0)
	{
		int iSpreadChance = GC.getGame().getSmallFakeRandNum(100, *pBestCity->plot());
		if (iSpreadChance <= iBaseChance)
		{
			CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eFranchiseBuilding);
			if (pBuildingInfo)
			{
				pBestCity->GetCityBuildings()->SetNumRealBuilding(eFranchiseBuilding, 1, true);

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
					strLogString.Format("Spread Corporate Building via Randomized Function. City: %s. Building: %s.", pBestCity->getName().c_str(), pBuildingInfo->GetText());
					LogCorporationMessage(strLogString);
				}
			}
		}
	}
}

void CvPlayerCorporations::LogCorporationMessage(const CvString& strMsg)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2;
		CvString strPlayerName;
		FILogFile* pLog;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvPlayerCorporations::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerCorporationLog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerCorporationLog.csv";
	}

	return strLogName;
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

	BuildingTypes eOffice = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetOfficeBuildingClass());
	CvBuildingEntry* pkOfficeInfo = GC.getBuildingInfo(eOffice);
	if (pkOfficeInfo == NULL)
		return "";

	// Great Person Rate bonus?
	if (pkOfficeInfo->GetGPRateModifierPerXFranchises() > 0)
	{
		iCurrentValue = iNumFranchises * pkOfficeInfo->GetGPRateModifierPerXFranchises();

		if (szOfficeBenefit != "")
		{
			szOfficeBenefit += ", ";
		}

		szOfficeBenefit += GetLocalizedText("TXT_KEY_CORPORATION_GREAT_PERSON_RATE_BENEFIT_DETAILS", iCurrentValue);
	}

	// Free Resource?
	for (int iI = 0; iI < GC.getNumResourceInfos(); iI++)
	{
		ResourceTypes eResource = (ResourceTypes)iI;
		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkOfficeInfo->GetResourceQuantityPerXFranchises(eResource) > 0)
		{
			iCurrentValue = iNumFranchises / pkOfficeInfo->GetResourceQuantityPerXFranchises(eResource);

			if (szOfficeBenefit != "")
			{
				szOfficeBenefit += ", ";
			}

			szOfficeBenefit += GetLocalizedText("TXT_KEY_CORPORATION_RESOURCE_BENEFIT_DETAILS", pkResourceInfo->GetDescriptionKey(), pkResourceInfo->GetIconString(), iCurrentValue);
		}
	}

	// Yield Change?
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		CvYieldInfo* pYield = GC.getYieldInfo(eYield);
		if (pkOfficeInfo->GetYieldPerFranchise(eYield) > 0)
		{
			iCurrentValue = iNumFranchises * pkOfficeInfo->GetYieldPerFranchise(eYield);

			if (szOfficeBenefit != "")
			{
				szOfficeBenefit += ", ";
			}

			szOfficeBenefit += GetLocalizedText("TXT_KEY_CORPORATION_YIELD_BENEFIT_DETAILS", pYield->GetDescriptionKey(), pYield->getIconString(), iCurrentValue);
		}
	}

	szOfficeBenefit = GetLocalizedText(pkCorporationInfo->GetOfficeBenefitHelper(), szOfficeBenefit);

	return szOfficeBenefit;
}

// Returns a string representing our current franchise benefit(if we have one) for the Corporation Overview
CvString CvPlayerCorporations::GetTradeRouteBenefit()
{
	if (!HasFoundedCorporation())
		return "";

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporationInfo == NULL)
		return "";

	return GetCurrentOfficeBenefit();
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

	if (GetCorporationOfficesAsFranchises() > 0)
		return GetNumOffices() * GetCorporationOfficesAsFranchises();

	int iReturnValue = pkCorporationInfo->GetBaseFranchises();
	iReturnValue += (int)(m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() * GC.getMOD_BALANCE_CORE_CORP_OFFICE_TR_CONVERSION());
	iReturnValue += (int)(GetNumOffices() * GC.getMOD_BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION());

	// Add in any "bonus" franchises from policies
	iReturnValue += GetAdditionalNumFranchises();

	// Search all players for Franchises (Autocracy)
	if(GetCorporationFreeFranchiseAbovePopular() > 0)
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
							iReturnValue += GetCorporationFreeFranchiseAbovePopular();
						}
					}
				}
			}
		}
	}

	iReturnValue *= 100 + GetAdditionalNumFranchisesMod();
	iReturnValue /= 100;

	return std::max(0, iReturnValue);
}

CvString CvPlayerCorporations::GetNumFranchisesTooltip()
{
	CvString strTooltip = "";

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(GetFoundedCorporation());
	if (pkCorporationInfo == NULL)
		return strTooltip;

	if (GetCorporationOfficesAsFranchises() > 0)
	{
		int iVal = GetNumOffices() * GetCorporationOfficesAsFranchises();
		strTooltip = GetLocalizedText("TXT_KEY_CORP_MAX_FRANCHISE_NATIONALIZATION_TT", iVal);
		return strTooltip;
	}

	int iTotal = pkCorporationInfo->GetBaseFranchises();
	int iNumTrades = (int)(m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() * GC.getMOD_BALANCE_CORE_CORP_OFFICE_TR_CONVERSION());
	iTotal += iNumTrades;

	iTotal += (int)(GetNumOffices() * GC.getMOD_BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION() /*.5f*/);

	// Add in any "bonus" franchises from policies
	int iPolicy = GetAdditionalNumFranchises();

	int iModTotal = iTotal + iPolicy;
	iModTotal *= 100 + GetAdditionalNumFranchisesMod();
	iModTotal /= 100;

	iModTotal -= iTotal;
	if (iModTotal <= 0)
		iModTotal = 0;

	
	strTooltip = GetLocalizedText("TXT_KEY_CORP_MAX_FRANCHISE_TT", pkCorporationInfo->GetBaseFranchises(), (GetNumOffices() * GC.getMOD_BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION()), iNumTrades, iModTotal);

	return strTooltip;
}

// Clear our Corporation from pCity
void CvPlayerCorporations::ClearAllCorporationsFromCity(CvCity* pCity)
{
	if (pCity == NULL)
		return;

	const std::vector<BuildingTypes>& vBuildings = pCity->GetCityBuildings()->GetAllBuildingsHere();
	for (size_t jJ = 0; jJ < vBuildings.size(); jJ++)
	{
		BuildingTypes eBuilding = vBuildings[jJ];

		if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) <= 0)
			continue;

		CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
		if (!pkBuilding)
			continue;

		if (!pkBuilding->IsCorp())
			continue;
	
		pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Corporation Building Destroyed in City: %s. Building: %s.", pCity->getName().c_str(), pkBuilding->GetText());
			LogCorporationMessage(strLogString);
		}

		if (pkBuilding->GetBuildingClassInfo().IsHeadquarters())
		{
			GC.getGame().GetGameCorporations()->DestroyCorporation((pkBuilding->GetBuildingClassInfo().getCorporationType()));
		}
	}
}

// Clear foreign Corporations from pCity
void CvPlayerCorporations::ClearCorporationFromCity(CvCity* pCity, CorporationTypes eCorporation, bool bAllButThis)
{
	if (pCity == NULL)
		return;

	if (eCorporation == NO_CORPORATION)
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (pkCorporationInfo == NULL)
		return;

	// Explicitly destroy all corporation buildings from this city
	BuildingTypes eHeadquarters = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetHeadquartersBuildingClass());
	BuildingTypes eOffice = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetOfficeBuildingClass());
	BuildingTypes eFranchise = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(pkCorporationInfo->GetFranchiseBuildingClass());

	const std::vector<BuildingTypes>& vBuildings = pCity->GetCityBuildings()->GetAllBuildingsHere();
	for (size_t jJ = 0; jJ < vBuildings.size(); jJ++)
	{
		BuildingTypes eBuilding = vBuildings[jJ];

		if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) <= 0)
			continue;

		CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
		if (!pkBuilding)
			continue;

		if (!pkBuilding->IsCorp())
			continue;

		if (bAllButThis)
		{
			if (eBuilding == eHeadquarters || eBuilding == eOffice || eFranchise == eBuilding)
				continue;

			pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);

			if (pkBuilding->GetBuildingClassInfo().IsHeadquarters())
			{
				GC.getGame().GetGameCorporations()->DestroyCorporation(pkBuilding->GetBuildingClassInfo().getCorporationType());
			}

			PlayerTypes eFounder = GC.getGame().GetCorporationFounder(pkBuilding->GetBuildingClassInfo().getCorporationType());
			if (eFounder != NO_PLAYER && eFounder != m_pPlayer->GetID())
			{
				CvNotifications* pNotifications = GET_PLAYER(eFounder).GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary;
					Localization::String strMessage;

					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
					strSummary << pkBuilding->GetTextKey();
					strSummary << pCity->getNameKey();
					strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
					strMessage << pkBuilding->GetTextKey();
					strMessage << pCity->getNameKey();
					strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
				}

				pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary;
					Localization::String strMessage;

					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
					strSummary << pkBuilding->GetTextKey();
					strSummary << pCity->getNameKey();
					strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
					strMessage << pkBuilding->GetTextKey();
					strMessage << pCity->getNameKey();
					strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
				}

				GET_PLAYER(eFounder).GetCorporations()->RecalculateNumFranchises();

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Corporation Building Removed from City: %s. Building: %s.", pCity->getName().c_str(), pkBuilding->GetText());
					LogCorporationMessage(strLogString);
				}
			}
		}
		else
		{
			if (eBuilding == eHeadquarters || eBuilding == eOffice || eFranchise == eBuilding)
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);

				if (eBuilding == eHeadquarters)
					GC.getGame().GetGameCorporations()->DestroyCorporation(eCorporation);

				PlayerTypes eFounder = GC.getGame().GetCorporationFounder(pkBuilding->GetBuildingClassInfo().getCorporationType());
				if (eFounder != NO_PLAYER && eFounder != m_pPlayer->GetID())
				{
					CvNotifications* pNotifications = GET_PLAYER(eFounder).GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary;
						Localization::String strMessage;

						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
						strSummary << pkBuilding->GetTextKey();
						strSummary << pCity->getNameKey();
						strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
						strMessage << pkBuilding->GetTextKey();
						strMessage << pCity->getNameKey();
						strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
					}
					pNotifications = m_pPlayer->GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary;
						Localization::String strMessage;

						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
						strSummary << pkBuilding->GetTextKey();
						strSummary << pCity->getNameKey();
						strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
						strMessage << pkBuilding->GetTextKey();
						strMessage << pCity->getNameKey();
						strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
					}

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Corporation Building Removed from City: %s. Building: %s.", pCity->getName().c_str(), pkBuilding->GetText());
						LogCorporationMessage(strLogString);
					}

					GET_PLAYER(eFounder).GetCorporations()->RecalculateNumFranchises();
				}
			}
		}
	}
	RecalculateNumFranchises();
}

// Clear our Corporation from foreign cities
void CvPlayerCorporations::ClearCorporationFromForeignCities(bool bMinorsOnly /* = false */, bool bFromEmbargo /* = false */)
{
	if (!HasFoundedCorporation())
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(m_eFoundedCorporation);
	if (pkCorporationInfo == NULL)
		return;

	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetFranchiseBuildingClass();

	// Get Corporation Buildings
	BuildingTypes eFranchise = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eFranchiseClass);

	CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eFranchise);
	if (!pkBuilding || !pkBuilding->IsCorp())
		return;

	//and destroy our franchises!
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (!GET_PLAYER(eLoopPlayer).isAlive()) 
			continue;

		if (bMinorsOnly && !GET_PLAYER(eLoopPlayer).isMinorCiv())
			continue;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// Don't clear franchises between masters and vassals if it's from a World Congress embargo
		if (MOD_DIPLOMACY_CIV4_FEATURES && bFromEmbargo)
		{
			if (GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(eLoopPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsVassal(m_pPlayer->getTeam()))
			{
				continue;
			}
		}
#endif

		int iLoop = 0;
		for (CvCity* pCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pCity != NULL; pCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
		{
			if (pCity->HasBuilding(eFranchise))
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 0);

				CvNotifications* pNotifications = GET_PLAYER(pCity->getOwner()).GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary;
					Localization::String strMessage;

					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
					strSummary << pkBuilding->GetTextKey();
					strSummary << pCity->getNameKey();
					strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
					strMessage << pkBuilding->GetTextKey();
					strMessage << pCity->getNameKey();
					strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
				}

				pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary;
					Localization::String strMessage;

					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
					strSummary << pkBuilding->GetTextKey();
					strSummary << pCity->getNameKey();
					strSummary << m_pPlayer->getCivilizationShortDescriptionKey();
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
					strMessage << pkBuilding->GetTextKey();
					strMessage << pCity->getNameKey();
					strMessage << m_pPlayer->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
				}

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Corporation Cleared from Foreign City: %s. Building: %s.", pCity->getName().c_str(), pkBuilding->GetText());
					LogCorporationMessage(strLogString);
				}
			}
		}
	}
	RecalculateNumFranchises();
}

bool CvPlayerCorporations::CanCreateFranchiseInCity(CvCity* pOriginCity, CvCity* pTargetCity)
{
	if (pOriginCity == NULL || pTargetCity == NULL)
		return false;

	if (!HasFoundedCorporation() || IsNoFranchisesInForeignCities() || GET_PLAYER(pTargetCity->getOwner()).GetCorporations()->IsNoForeignCorpsInCities() || GetCorporationOfficesAsFranchises() > 0)
		return false;

	if (pTargetCity->IsHasFranchise(m_eFoundedCorporation) || !pOriginCity->IsHasOffice())
		return false;

	int iFranchises = m_pPlayer->GetCorporations()->GetNumFranchises();
	int iMax = m_pPlayer->GetCorporations()->GetMaxNumFranchises();
	if (iFranchises >= iMax)
		return false;

	bool bVassalRelations = GET_TEAM(GET_PLAYER(pTargetCity->getOwner()).getTeam()).IsVassal(m_pPlayer->getTeam()) || GET_TEAM(m_pPlayer->getTeam()).IsVassal(GET_PLAYER(pTargetCity->getOwner()).getTeam());

	if (!bVassalRelations)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague)
			if (pLeague->IsPlayerEmbargoed(pTargetCity->getOwner()) || pLeague->IsPlayerEmbargoed(m_pPlayer->GetID()))
				return false;
	}

	return true;
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

int CvPlayerCorporations::GetFranchiseTourismMod(PlayerTypes ePlayer, bool bJustCheckOne) const
{
	CorporationTypes eFounded = GetFoundedCorporation();
	if (eFounded == NO_CORPORATION)
		return 0;

	if (GetCorporationEntry()->GetTourismMod() == 0)
		return 0;

	int iNumFranchises = 0;

	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if (pLoopCity != NULL)
		{
			if (pLoopCity->IsHasFranchise(eFounded))
			{
				iNumFranchises++;
				if (bJustCheckOne)
					return iNumFranchises;

				// Free franchise above Popular?
				if (GetCorporationFreeFranchiseAbovePopular() != 0 && m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_POPULAR)
				{
					iNumFranchises += GetCorporationFreeFranchiseAbovePopular();
				}
			}
		}
	}

	return iNumFranchises * GetCorporationEntry()->GetTourismMod();
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
	if (eCorporation == NO_CORPORATION)
		return;

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

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Corporation Eliminated: %s.", GC.getCorporationInfo(eCorporation)->GetDescriptionKey());
				kPlayer.GetCorporations()->LogCorporationMessage(strLogString);
			}

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
		kPlayer.GetCorporations()->RecalculateNumOffices();
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

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		strLogString.Format("Corporation Founded in City: %s. Corp: %s.", pHeadquarters->getName().c_str(), pkCorporationInfo->GetDescriptionKey());
		kPlayer.GetCorporations()->LogCorporationMessage(strLogString);
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