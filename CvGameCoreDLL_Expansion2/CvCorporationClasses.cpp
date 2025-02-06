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
	m_viResourceMonopolyAnds(),
	m_viResourceMonopolyOrs(),
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
	m_viResourceMonopolyAnds.clear();
	m_viResourceMonopolyOrs.clear();
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

/// Prerequisite resources with AND
uint CvCorporationEntry::GetResourceMonopolyAndSize() const
{
	return m_viResourceMonopolyAnds.size();
}
int CvCorporationEntry::GetResourceMonopolyAnd(uint ui) const
{
	ASSERT_DEBUG(ui < m_viResourceMonopolyAnds.size(), "Index out of bounds");
	return m_viResourceMonopolyAnds[ui];
}

/// Prerequisite resources with OR
uint CvCorporationEntry::GetResourceMonopolyOrSize() const
{
	return m_viResourceMonopolyOrs.size();
}
int CvCorporationEntry::GetResourceMonopolyOr(uint ui) const
{
	ASSERT_DEBUG(ui < m_viResourceMonopolyOrs.size(), "Index out of bounds");
	return m_viResourceMonopolyOrs[ui];
}

int CvCorporationEntry::GetNumFreeResource(int i) const
{
	ASSERT_DEBUG(i < GC.getNumResourceInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piNumFreeResource ? m_piNumFreeResource[i] : -1;
}

int CvCorporationEntry::GetUnitResourceProductionModifier(int i) const
{
	ASSERT_DEBUG(i < GC.getNumResourceInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piUnitResourceProductionModifier ? m_piUnitResourceProductionModifier[i] : -1;
}

/// Yield Modifier for Trade Routes to cities from an office to cities with a Franchise
int CvCorporationEntry::GetTradeRouteMod(int i) const
{
	ASSERT_DEBUG(i < NUM_YIELD_TYPES, "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piTradeRouteMod ? m_piTradeRouteMod[i] : -1;
}

int CvCorporationEntry::GetTradeRouteCityMod(int i) const
{
	ASSERT_DEBUG(i < NUM_YIELD_TYPES, "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piTradeRouteCityMod ? m_piTradeRouteCityMod[i] : -1;
}

/// Change to Resource yield by type
int CvCorporationEntry::GetResourceYieldChange(int i, int j) const
{
	ASSERT_DEBUG(i < GC.getNumResourceInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	ASSERT_DEBUG(j < NUM_YIELD_TYPES, "Index out of bounds");
	ASSERT_DEBUG(j > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange ? m_ppaiResourceYieldChange[i][j] : -1;
}

/// Array of changes to Resource yield
int* CvCorporationEntry::GetResourceYieldChangeArray(int i) const
{
	ASSERT_DEBUG(i < GC.getNumResourceInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_ppaiResourceYieldChange[i];
}

/// Change to specialist yield by type
int CvCorporationEntry::GetSpecialistYieldChange(int i, int j) const
{
	ASSERT_DEBUG(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	ASSERT_DEBUG(j < NUM_YIELD_TYPES, "Index out of bounds");
	ASSERT_DEBUG(j > -1, "Index out of bounds");
	return m_ppaiSpecialistYieldChange ? m_ppaiSpecialistYieldChange[i][j] : -1;
}

/// Array of changes to specialist yield
int* CvCorporationEntry::GetSpecialistYieldChangeArray(int i) const
{
	ASSERT_DEBUG(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
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
	ASSERT_DEBUG(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	ASSERT_DEBUG(j < NUM_YIELD_TYPES, "Index out of bounds");
	ASSERT_DEBUG(j > -1, "Index out of bounds");
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

	kUtility.PopulateVector(m_viResourceMonopolyAnds, "Resources", "Corporation_ResourceMonopolyAnds", "ResourceType", "CorporationType", szCorporationType);
	kUtility.PopulateVector(m_viResourceMonopolyOrs, "Resources", "Corporation_ResourceMonopolyOrs", "ResourceType", "CorporationType", szCorporationType);
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

bool CvCorporation::IsCorporationBuilding(BuildingClassTypes eBuildingClass) const
{
	CvBuildingClassInfo* pBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	if (pBuildingClassInfo == NULL)
		return false;

	if (pBuildingClassInfo->getCorporationType() == NO_CORPORATION)
		return false;

	return pBuildingClassInfo->getCorporationType() == m_eCorporation;
}

template<typename Corporation, typename Visitor>
void CvCorporation::Serialize(Corporation& corporation, Visitor& visitor)
{
	visitor(corporation.m_eCorporation);
	visitor(corporation.m_eFounder);
	visitor(corporation.m_iHeadquartersCityX);
	visitor(corporation.m_iHeadquartersCityY);
	visitor(corporation.m_iTurnFounded);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCorporation& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvCorporation::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvCorporation& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvCorporation::Serialize(readFrom, serialVisitor);
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
	int v = 0;
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
	m_iNoForeignCorpsInCities(0),
	m_iNoFranchisesInForeignCities(0),
	m_aiFranchisesPerImprovement()
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

	m_aiFranchisesPerImprovement.clear();
	m_aiFranchisesPerImprovement.resize(GC.getNumImprovementInfos(), 0);

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
	m_iNoForeignCorpsInCities = 0;
	m_iNoFranchisesInForeignCities = 0;
}

/// Reset
void CvPlayerCorporations::Reset()
{
}

template<typename PlayerCorporations, typename Visitor>
void CvPlayerCorporations::Serialize(PlayerCorporations& playerCorporations, Visitor& visitor)
{
	visitor(playerCorporations.m_eFoundedCorporation);

	visitor(playerCorporations.m_iNumOffices);
	visitor(playerCorporations.m_iNumFranchises);
	visitor(playerCorporations.m_iAdditionalNumFranchises);
	visitor(playerCorporations.m_iAdditionalNumFranchisesMod);

	visitor(playerCorporations.m_iCorporationOfficesAsFranchises);
	visitor(playerCorporations.m_iCorporationRandomForeignFranchiseMod);
	visitor(playerCorporations.m_iCorporationFreeFranchiseAbovePopular);
	visitor(playerCorporations.m_iNoForeignCorpsInCities);
	visitor(playerCorporations.m_iNoFranchisesInForeignCities);
	visitor(playerCorporations.m_aiFranchisesPerImprovement);
}

/// Serialization read
void CvPlayerCorporations::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvPlayerCorporations::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvPlayerCorporations& playerCorporations)
{
	playerCorporations.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvPlayerCorporations& playerCorporations)
{
	playerCorporations.Write(stream);
	return stream;
}

CvCorporation * CvPlayerCorporations::GetCorporation() const
{
	return GC.getGame().GetGameCorporations()->GetCorporation(m_eFoundedCorporation);
}

// Destroys all offices and franchises associated with this corporation
// Do not call this function outside of CvGameCorporations::DestroyCorporation()! It will mess things up!
void CvPlayerCorporations::DestroyCorporation()
{
	if (!HasFoundedCorporation())
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(m_eFoundedCorporation);
	if (!pkCorporationInfo)
		return;

	BuildingClassTypes eHeadquartersClass = pkCorporationInfo->GetHeadquartersBuildingClass();
	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetOfficeBuildingClass();
	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetFranchiseBuildingClass();

	int iLoop = 0;
	// Destroy our headquarters and offices
	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		BuildingTypes eHeadquarters = pCity->GetBuildingTypeFromClass(eHeadquartersClass);
		BuildingTypes eOffice = pCity->GetBuildingTypeFromClass(eOfficeClass);

		// City has headquarters?
		if (eHeadquarters != NO_BUILDING && pCity->HasBuilding(eHeadquarters))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eHeadquarters, 0);
			GC.getGame().decrementBuildingClassCreatedCount(eHeadquartersClass);
		}

		// City has office?
		if (eOffice != NO_BUILDING && pCity->HasBuilding(eOffice))
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eOffice, 0);
		}
	}

	// And destroy our franchises too!
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iPlayerLoop);
		CvPlayer& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if (!kLoopPlayer.isAlive())
			continue;
		
		int iLoop = 0;
		for (CvCity* pCity = kLoopPlayer.firstCity(&iLoop); pCity != NULL; pCity = kLoopPlayer.nextCity(&iLoop))
		{
			BuildingTypes eFranchise = pCity->GetBuildingTypeFromClass(eFranchiseClass);
			if (eFranchise != NO_BUILDING && pCity->HasBuilding(eFranchise))
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 0);
			}
		}

		// Push notification to all players about destroyed Corporation
		CvNotifications* pNotifications = kLoopPlayer.GetNotifications();
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

	m_pPlayer->processCorporations(m_eFoundedCorporation, -1);
	Uninit();
}

bool CvPlayerCorporations::IsCorporationOfficesAsFranchises() const
{
	return (m_iCorporationOfficesAsFranchises > 0);
}

int CvPlayerCorporations::GetCorporationOfficesAsFranchises() const
{
	return m_iCorporationOfficesAsFranchises;
}

void CvPlayerCorporations::ChangeCorporationOfficesAsFranchises(int iValue)
{
	if (iValue != 0)
	{
		m_iCorporationOfficesAsFranchises += iValue;
		RecalculateNumFranchises();
	}
}

int CvPlayerCorporations::GetCorporationRandomForeignFranchiseMod() const
{
	return m_iCorporationRandomForeignFranchiseMod;
}

void CvPlayerCorporations::ChangeCorporationRandomForeignFranchiseMod(int iValue)
{
	m_iCorporationRandomForeignFranchiseMod += iValue;
}

int CvPlayerCorporations::GetCorporationFreeFranchiseAbovePopular() const
{
	return m_iCorporationFreeFranchiseAbovePopular;
}

void CvPlayerCorporations::ChangeCorporationFreeFranchiseAbovePopular(int iValue)
{
	if (iValue != 0)
	{
		m_iCorporationFreeFranchiseAbovePopular += iValue;
		RecalculateNumFranchises();
	}
}

bool CvPlayerCorporations::IsNoForeignCorpsInCities() const
{
	return (m_iNoForeignCorpsInCities > 0);
}

void CvPlayerCorporations::ChangeNoForeignCorpsInCities(int iValue)
{
	int iOldValue = m_iNoForeignCorpsInCities;
	m_iNoForeignCorpsInCities += iValue;
	if (iOldValue <= 0 && m_iNoForeignCorpsInCities > 0)
	{
		int iLoop = 0;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			ClearCorporationFromCity(pLoopCity, GetFoundedCorporation(), true);
		}
	}
}

bool CvPlayerCorporations::IsNoFranchisesInForeignCities() const
{
	return (m_iNoFranchisesInForeignCities > 0);
}

void CvPlayerCorporations::ChangeNoFranchisesInForeignCities(int iValue)
{
	int iOldValue = m_iNoFranchisesInForeignCities;
	m_iNoFranchisesInForeignCities += iValue;
	if (iOldValue <= 0 && m_iNoFranchisesInForeignCities > 0)
	{
		ClearCorporationFromForeignCities(false, true);
	}
}

int CvPlayerCorporations::GetFranchisesPerImprovement(ImprovementTypes eIndex) const
{
	ASSERT_DEBUG(eIndex < GC.getNumImprovementInfos(), "Index out of bounds");
	ASSERT_DEBUG(eIndex > -1, "Index out of bounds");
	return m_aiFranchisesPerImprovement[eIndex];
}

void CvPlayerCorporations::ChangeFranchisesPerImprovement(ImprovementTypes eIndex, int iValue)
{
	ASSERT_DEBUG(eIndex < GC.getNumImprovementInfos(), "Index out of bounds");
	ASSERT_DEBUG(eIndex > -1, "Index out of bounds");
	if (iValue != 0)
	{
		m_aiFranchisesPerImprovement[eIndex] += iValue;
		RecalculateNumFranchises();
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
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
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
	int iFreeFranchises = 0; // From Infiltration

	if (GetCorporationOfficesAsFranchises() > 0)
	{
		iFranchises = GetNumOffices() * GetCorporationOfficesAsFranchises();
	}
	else
	{
		// Search all players for Franchises (if no Nationalization Tenet)
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_CIV_PLAYERS; iLoopPlayer++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
			if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isBarbarian())
			{
				CvCity* pLoopCity = NULL;
				int iLoop = 0;
				for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
				{
					if (pLoopCity != NULL)
					{
						if (pLoopCity->IsHasFranchise(eCorporation))
						{
							iFranchises++;

							// Free franchise above Popular?
							if (GetCorporationFreeFranchiseAbovePopular() != 0 && GET_PLAYER(ePlayer).isMajorCiv() && m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_POPULAR)
							{
								iFreeFranchises += GetCorporationFreeFranchiseAbovePopular();
							}
						}
					}
				}
			}
		}

		iFranchises += iFreeFranchises;
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		if (GetFranchisesPerImprovement((ImprovementTypes)iI) > 0)
		{
			iFranchises += (m_pPlayer->getImprovementCount((ImprovementTypes)iI, true) * GetFranchisesPerImprovement((ImprovementTypes)iI));
		}
	}

	m_iNumFranchises = iFranchises;

	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();

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

	int iSpreadChance = pkCorporation->GetRandomSpreadChance();
	iSpreadChance += GetCorporationRandomForeignFranchiseMod();

	// Randomly roll to see if we spread anywhere this turn
	int iSpreadRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x89d2f3fe).mix(m_pPlayer->GetID()));
	if (iSpreadRoll > iSpreadChance)
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

		if (pLeague != NULL && pLeague->IsTradeEmbargoed(m_pPlayer->GetID(), ePlayer))
			continue;

		if (!m_pPlayer->GetTrade()->IsConnectedToPlayer(ePlayer))
			continue;

		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetMaster() != m_pPlayer->getTeam() && GET_PLAYER(ePlayer).GetCorporations()->IsNoForeignCorpsInCities())
			continue;

		int iLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if (pLoopCity != NULL)
			{
				if (m_pPlayer->GetTrade()->IsCityAlreadyConnectedByTrade(pLoopCity))
					continue;

				// City can not contain our franchise already
				if (pLoopCity->IsHasFranchise(GetFoundedCorporation()))
					continue;

				int iChance = GC.getGame().randRangeExclusive(0, 101, CvSeeder::fromRaw(0x61e74940).mix(pLoopCity->plot()->GetPseudoRandomSeed())) + GC.getGame().randRangeExclusive(0, 101, CvSeeder::fromRaw(0x533dfdcd).mix(pLoopCity->plot()->GetPseudoRandomSeed()));
				int iScore = 500 - iChance;

				int iLoop2 = 0;
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
		CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eFranchiseBuilding);
		if (pBuildingInfo)
		{
			pBestCity->GetCityBuildings()->SetNumRealBuilding(eFranchiseBuilding, 1, true);

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				pBestCity->UpdateYieldFromCorporationFranchises((YieldTypes)iI);
				CvCity* pLoopCity = NULL;
				int iLoop = 0;
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
			else if (pBestCity->getOwner() == GC.getGame().getActivePlayer())
			{
				CvNotifications* pNotifications = GET_PLAYER(pBestCity->getOwner()).GetNotifications();
				if (pNotifications)
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

void CvPlayerCorporations::LogCorporationMessage(const CvString& strMsg)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString szTemp2;
		CvString strPlayerName;
		FILogFile* pLog = NULL;

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
	if (eOffice == NO_BUILDING)
		return "";

	CvBuildingEntry* pkOfficeInfo = GC.getBuildingInfo(eOffice);

	// Great Person Rate bonus?
	if (pkOfficeInfo->GetGPRateModifierPerXFranchises() > 0)
	{
		iCurrentValue = iNumFranchises * pkOfficeInfo->GetGPRateModifierPerXFranchises();

		if (!szOfficeBenefit.empty())
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

			if (!szOfficeBenefit.empty())
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

			if (!szOfficeBenefit.empty())
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

	int iReturnValue = 0;

	if (GetCorporationOfficesAsFranchises() > 0) 
	{
		iReturnValue += GetNumOffices() * GetCorporationOfficesAsFranchises();
	}
	else
	{
		iReturnValue += pkCorporationInfo->GetBaseFranchises();
		iReturnValue += (int)(m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() * /*1.0f*/ GD_FLOAT_GET(BALANCE_CORE_CORP_OFFICE_TR_CONVERSION));
		iReturnValue += (int)(GetNumOffices() * /*0.5f*/ GD_FLOAT_GET(BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION));

		// Add in any "bonus" franchises from policies
		iReturnValue += GetAdditionalNumFranchises();

		// Search all players for Franchises (Autocracy)
		if (GetCorporationFreeFranchiseAbovePopular() > 0)
		{
			for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
			{
				PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
				if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive())
				{
					if (m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) < INFLUENCE_LEVEL_POPULAR)
						continue;

					CvCity* pLoopCity = NULL;
					int iLoop = 0;
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
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		if (GetFranchisesPerImprovement((ImprovementTypes)iI) > 0)
		{
			iReturnValue += (m_pPlayer->getImprovementCount((ImprovementTypes)iI, true) * GetFranchisesPerImprovement((ImprovementTypes)iI));
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
	int iNumTrades = (int)(m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() * /*1.0f*/ GD_FLOAT_GET(BALANCE_CORE_CORP_OFFICE_TR_CONVERSION));
	iTotal += iNumTrades;

	iTotal += (int)(GetNumOffices() * /*0.5f*/ GD_FLOAT_GET(BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION));

	// Add in any "bonus" franchises from policies
	int iPolicy = GetAdditionalNumFranchises();

	int iModTotal = iTotal + iPolicy;
	iModTotal *= 100 + GetAdditionalNumFranchisesMod();
	iModTotal /= 100;

	iModTotal -= iTotal;
	if (iModTotal <= 0)
		iModTotal = 0;

	
	strTooltip = GetLocalizedText("TXT_KEY_CORP_MAX_FRANCHISE_TT", pkCorporationInfo->GetBaseFranchises(), (GetNumOffices() * /*0.5f*/ GD_FLOAT_GET(BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION)), iNumTrades, iModTotal);

	return strTooltip;
}

// Clear our Corporation from pCity
void CvPlayerCorporations::ClearAllCorporationsFromCity(CvCity* pCity)
{
	if (pCity == NULL)
		return;

	// don't use a reference variable here, we're changing which buildings are in the city
	const std::vector<BuildingTypes> vBuildings = pCity->GetCityBuildings()->GetAllBuildingsHere();
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
	
		if (pkBuilding->GetBuildingClassInfo().IsHeadquarters())
		{
			GC.getGame().GetGameCorporations()->DestroyCorporation((pkBuilding->GetBuildingClassInfo().getCorporationType()));
		}
		else
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
		}

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Corporation Building Destroyed in City: %s. Building: %s.", pCity->getName().c_str(), pkBuilding->GetText());
			LogCorporationMessage(strLogString);
		}
	}
}

// Clear foreign Corporations from pCity
void CvPlayerCorporations::ClearCorporationFromCity(CvCity* pCity, CorporationTypes eCorporation, bool bAllButThis)
{
	ASSERT_DEBUG(pCity);
	ASSERT_DEBUG(eCorporation != NO_CORPORATION);

	if (!pCity)
		return;

	if (eCorporation == NO_CORPORATION)
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (!pkCorporationInfo)
		return;

	BuildingClassTypes eHeadquartersClass = pkCorporationInfo->GetHeadquartersBuildingClass();
	BuildingClassTypes eOfficeClass = pkCorporationInfo->GetOfficeBuildingClass();
	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetFranchiseBuildingClass();

	// Explicitly destroy all corporation buildings from this city
	BuildingTypes eHeadquarters = pCity->GetBuildingTypeFromClass(eHeadquartersClass);
	BuildingTypes eOffice = pCity->GetBuildingTypeFromClass(eOfficeClass);
	BuildingTypes eFranchise = pCity->GetBuildingTypeFromClass(eFranchiseClass);

	// don't use a reference variable here, we're changing which buildings are in the city
	const std::vector<BuildingTypes> veBuildings = pCity->GetCityBuildings()->GetAllBuildingsHere(); 
	for (vector<BuildingTypes>::const_iterator it = veBuildings.begin(); it != veBuildings.end(); ++it)
	{
		if (pCity->GetCityBuildings()->GetNumBuilding(*it) <= 0)
			continue;

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(*it);
		if (!pkBuildingInfo)
			continue;

		if (!pkBuildingInfo->IsCorp())
			continue;

		if (bAllButThis && (*it == eHeadquarters || *it == eOffice || *it == eFranchise))
			continue;

		if (!bAllButThis && *it != eHeadquarters && *it != eOffice && *it != eFranchise)
			continue;

		pCity->GetCityBuildings()->SetNumRealBuilding(*it, 0);
		const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
		CorporationTypes eThisCorporation = kBuildingClassInfo.getCorporationType();

		if (kBuildingClassInfo.IsHeadquarters())
			GC.getGame().GetGameCorporations()->DestroyCorporation(eThisCorporation);

		PlayerTypes eFounder = GC.getGame().GetCorporationFounder(eThisCorporation);
		if (eFounder != NO_PLAYER)
		{
			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			CvNotifications* pOtherNotifications = GET_PLAYER(eFounder).GetNotifications();
			if (pNotifications || pOtherNotifications)
			{
				Localization::String strSummary;
				Localization::String strMessage;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
				strSummary << pkBuildingInfo->GetTextKey();
				strSummary << pCity->getNameKey();
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
				strMessage << pkBuildingInfo->GetTextKey();
				strMessage << pCity->getNameKey();
				strMessage << m_pPlayer->getCivilizationShortDescriptionKey();

				if (pNotifications)
					pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);

				// Skip if we are the founder (already sent above)
				if (eFounder != m_pPlayer->GetID() && pOtherNotifications)
					pOtherNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
			}

			GET_PLAYER(eFounder).GetCorporations()->RecalculateNumFranchises();
		}

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Corporation Building Removed from City: %s. Building: %s.", pCity->getName().c_str(), pkBuildingInfo->GetText());
			LogCorporationMessage(strLogString);
		}
	}

	RecalculateNumFranchises();
}

// Clear our Corporation from foreign cities
void CvPlayerCorporations::ClearCorporationFromForeignCities(bool bMinorsOnly, bool bExcludeVassals, bool bExcludeMasters)
{
	if (!HasFoundedCorporation())
		return;

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(m_eFoundedCorporation);
	if (!pkCorporationInfo)
		return;

	BuildingClassTypes eFranchiseClass = pkCorporationInfo->GetFranchiseBuildingClass();

	// Loop all foreign cities and destroy our franchises!
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = static_cast<PlayerTypes>(iPlayerLoop);
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		if (!kPlayer.isAlive())
			continue;

		if (ePlayer == m_pPlayer->GetID())
			continue;

		if (bMinorsOnly && !kPlayer.isMinorCiv())
			continue;

		if (bExcludeVassals && GET_TEAM(kPlayer.getTeam()).IsVassal(m_pPlayer->getTeam()))
			continue;

		if (bExcludeMasters && GET_TEAM(m_pPlayer->getTeam()).IsVassal(kPlayer.getTeam()))
			continue;

		int iLoop = 0;
		for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
		{
			BuildingTypes eFranchise = pCity->GetBuildingTypeFromClass(eFranchiseClass);
			if (eFranchise == NO_BUILDING)
				continue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eFranchise);
			if (!pkBuildingInfo)
				continue;

			if (pCity->HasBuilding(eFranchise))
			{
				pCity->GetCityBuildings()->SetNumRealBuilding(eFranchise, 0);

				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				CvNotifications* pOtherNotifications = kPlayer.GetNotifications();
				if (pNotifications || pOtherNotifications)
				{
					Localization::String strSummary;
					Localization::String strMessage;

					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY_S");
					strSummary << pkBuildingInfo->GetTextKey();
					strSummary << pCity->getNameKey();
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CORPORATION_BUILDINGS_REMOVED_CITY");
					strMessage << pkBuildingInfo->GetTextKey();
					strMessage << pCity->getNameKey();
					strMessage << m_pPlayer->getCivilizationShortDescriptionKey();

					if (pNotifications)
						pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);

					if (pOtherNotifications)
						pOtherNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1, -1);
				}

				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Corporation Cleared from Foreign City: %s. Building: %s.", pCity->getName().c_str(), pkBuildingInfo->GetText());
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

	if (!HasFoundedCorporation())
		return false;

	//no local franchises? check ownership.
	if (GetCorporationOfficesAsFranchises() > 0 && pOriginCity->getOwner() == m_pPlayer->GetID())
		return false;

	//no foreign franchises? Exception for vassals.
	if (GET_TEAM(pTargetCity->getTeam()).GetMaster() != pOriginCity->getTeam())
	{
		if (IsNoFranchisesInForeignCities())
			return false;
		if (GET_PLAYER(pTargetCity->getOwner()).GetCorporations()->IsNoForeignCorpsInCities())
			return false;
	}

	if (pTargetCity->IsHasFranchise(m_eFoundedCorporation) || !pOriginCity->IsHasOffice())
		return false;

	int iFranchises = m_pPlayer->GetCorporations()->GetNumFranchises();
	int iMax = m_pPlayer->GetCorporations()->GetMaxNumFranchises();
	if (iFranchises >= iMax)
		return false;

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	return pLeague == NULL || !pLeague->IsTradeEmbargoed(m_pPlayer->GetID(), pTargetCity->getOwner());
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

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
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
				if (GetCorporationFreeFranchiseAbovePopular() != 0 && GET_PLAYER(ePlayer).isMajorCiv() && m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_POPULAR)
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
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); ++it)
	{
		if(it->m_eCorporation == eCorporation)
		{
			return &(*it);
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
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); ++it)
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
	if (eOfficeClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eOffice = pHeadquarters->GetBuildingTypeFromClass(eOfficeClass, true);
		if (eOffice != NO_BUILDING)
		{
			pHeadquarters->SetNumFreeBuilding(eOffice, 1, true, false);
			kPlayer.GetCorporations()->RecalculateNumOffices();
		}
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
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); ++it)
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
	for(it = m_ActiveCorporations.begin(); it != m_ActiveCorporations.end(); ++it)
	{
		if((*it).m_iHeadquartersCityX == pCity->getX() &&
			(*it).m_iHeadquartersCityY == pCity->getY())
			return true;
	}

	return false;
}

template<typename GameCorporations, typename Visitor>
void CvGameCorporations::Serialize(GameCorporations& gameCorporations, Visitor& visitor)
{
	visitor(gameCorporations.m_ActiveCorporations);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameCorporations& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameCorporations::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameCorporations& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameCorporations::Serialize(readFrom, serialVisitor);
	return saveTo;
}

#endif