/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvTechAI.h"
#include "CvFlavorManager.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"

/// Constructor
CvTechEntry::CvTechEntry(void):
	m_iAIWeight(0),
	m_iAITradeModifier(0),
	m_iResearchCost(0),
	m_iAdvancedStartCost(0),
	m_iEra(NO_ERA),
	m_iFeatureProductionModifier(0),
	m_iUnitFortificationModifier(0),
	m_iUnitBaseHealModifier(0),
	m_iWorkerSpeedModifier(0),
	m_iFirstFreeUnitClass(NO_UNITCLASS),
	m_iFirstFreeTechs(0),
	m_iEmbarkedMoveChange(0),
	m_iInternationalTradeRoutesChange(0),
	m_iInfluenceSpreadModifier(0),
	m_iExtraVotesPerDiplomat(0),
	m_iGridX(0),
	m_iGridY(0),
	m_bEndsGame(false),
	m_bAllowsEmbarking(false),
	m_bAllowsDefensiveEmbarking(false),
	m_bEmbarkedAllWaterPassage(false),
	m_bAllowsBarbarianBoats(false),
	m_bRepeat(false),
	m_bTrade(false),
	m_bDisable(false),
	m_bGoodyTech(false),
	m_bExtraWaterSeeFrom(false),
	m_bMapCentering(false),
	m_bMapVisible(false),
	m_bMapTrading(false),
	m_bTechTrading(false),
	m_bGoldTrading(false),
	m_bAllowEmbassyTradingAllowed(false),
	m_bOpenBordersTradingAllowed(false),
	m_bDefensivePactTradingAllowed(false),
	m_bResearchAgreementTradingAllowed(false),
	m_bTradeAgreementTradingAllowed(false),
	m_bPermanentAllianceTrading(false),
#if defined(MOD_TECHS_CITY_WORKING)
	m_iCityWorkingChange(0),
#endif
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	m_iCityAutomatonWorkersChange(0),
#endif
	m_bBridgeBuilding(false),
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	m_bCityLessEmbarkCost(false),
	m_bCityNoEmbarkCost(false),
#endif
	m_bWaterWork(false),
	m_bTriggersArchaeologicalSites(false),
	m_bAllowsWorldCongress(false),
	m_piDomainExtraMoves(NULL),
	m_piTradeRouteDomainExtraRange(NULL),
	m_piFlavorValue(NULL),
	m_piPrereqOrTechs(NULL),
	m_piPrereqAndTechs(NULL),
#if defined(MOD_BALANCE_CORE)
	m_iHappiness(0),
	m_ppiTechYieldChanges(NULL),
	m_bCorporationsEnabled(false),
#endif
#if defined(MOD_CIV6_EUREKA)
	m_iEurekaPerMillion(0),
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bVassalageTradingAllowed(false),
#endif
	m_bUnlocksEspionageAdvancedActions(false),
	m_pabFreePromotion(NULL)
{
}

/// Destructor
CvTechEntry::~CvTechEntry(void)
{
	SAFE_DELETE_ARRAY(m_piDomainExtraMoves);
	SAFE_DELETE_ARRAY(m_piTradeRouteDomainExtraRange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPrereqOrTechs);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
#if defined(MOD_BALANCE_CORE)
	CvDatabaseUtility::SafeDelete2DArray(m_ppiTechYieldChanges);
#endif
}

bool CvTechEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iAIWeight = kResults.GetInt("AIWeight");
	m_iAITradeModifier = kResults.GetInt("AITradeModifier");
	m_iResearchCost = kResults.GetInt("Cost");
	m_iAdvancedStartCost = kResults.GetInt("AdvancedStartCost");
	m_iFeatureProductionModifier = kResults.GetInt("FeatureProductionModifier");
	m_iUnitFortificationModifier = kResults.GetInt("UnitFortificationModifier");
	m_iUnitBaseHealModifier = kResults.GetInt("UnitBaseHealModifier");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iFirstFreeTechs = kResults.GetInt("FirstFreeTechs");
	m_iEmbarkedMoveChange = kResults.GetInt("EmbarkedMoveChange");
	m_iInternationalTradeRoutesChange = kResults.GetInt("InternationalTradeRoutesChange");
	m_iInfluenceSpreadModifier = kResults.GetInt("InfluenceSpreadModifier");
	m_iExtraVotesPerDiplomat = kResults.GetInt("ExtraVotesPerDiplomat");
	m_bEndsGame = kResults.GetBool("EndsGame");
	m_bAllowsEmbarking = kResults.GetBool("AllowsEmbarking");
	m_bAllowsDefensiveEmbarking = kResults.GetBool("AllowsDefensiveEmbarking");
	m_bEmbarkedAllWaterPassage = kResults.GetBool("EmbarkedAllWaterPassage");
	m_bAllowsBarbarianBoats = kResults.GetBool("AllowsBarbarianBoats");
	m_bRepeat = kResults.GetBool("Repeat");
	m_bTrade = kResults.GetBool("Trade");
	m_bDisable = kResults.GetBool("Disable");
	m_bGoodyTech = kResults.GetBool("GoodyTech");
	m_bTriggersArchaeologicalSites = kResults.GetBool("TriggersArchaeologicalSites");
	m_bAllowsWorldCongress = kResults.GetBool("AllowsWorldCongress");
	m_bExtraWaterSeeFrom = kResults.GetBool("ExtraWaterSeeFrom");
	m_bMapCentering = kResults.GetBool("MapCentering");
	m_bMapVisible = kResults.GetBool("MapVisible");
	m_bMapTrading = kResults.GetBool("MapTrading");
	m_bTechTrading = kResults.GetBool("TechTrading");
	m_bGoldTrading = kResults.GetBool("GoldTrading");
	m_bAllowEmbassyTradingAllowed = kResults.GetBool("AllowEmbassyTradingAllowed");
	m_bOpenBordersTradingAllowed = kResults.GetBool("OpenBordersTradingAllowed");
	m_bDefensivePactTradingAllowed = kResults.GetBool("DefensivePactTradingAllowed");
	m_bResearchAgreementTradingAllowed = kResults.GetBool("ResearchAgreementTradingAllowed");
	m_bTradeAgreementTradingAllowed = kResults.GetBool("TradeAgreementTradingAllowed");
	m_bPermanentAllianceTrading = kResults.GetBool("PermanentAllianceTradingAllowed");
#if defined(MOD_TECHS_CITY_WORKING)
	m_iCityWorkingChange = kResults.GetInt("CityWorkingChange");
#endif
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	m_iCityAutomatonWorkersChange = kResults.GetInt("CityAutomatonWorkersChange");
#endif
	m_bBridgeBuilding = kResults.GetBool("BridgeBuilding");
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	m_bCityLessEmbarkCost = kResults.GetBool("CityLessEmbarkCost");
	m_bCityNoEmbarkCost = kResults.GetBool("CityNoEmbarkCost");
#endif
	m_bWaterWork = kResults.GetBool("WaterWork");
	m_iGridX = kResults.GetInt("GridX");
	m_iGridY = kResults.GetInt("GridY");
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		m_bVassalageTradingAllowed = kResults.GetBool("VassalageTradingAllowed");
	}
	m_bUnlocksEspionageAdvancedActions = kResults.GetBool("UnlocksEspionageAdvancedActions");
#endif
#if defined(MOD_BALANCE_CORE)
	m_iHappiness = kResults.GetInt("Happiness");
	m_bCorporationsEnabled = kResults.GetBool("CorporationsEnabled");
#endif
#if defined(MOD_CIV6_EUREKA)
	m_iEurekaPerMillion = kResults.GetInt("EurekaPerMillion");
#endif

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("Era");
	m_iEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FirstFreeUnitClass");
	m_iFirstFreeUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Quote");
	SetQuoteKey(szTextVal);

	szTextVal = kResults.GetText("AudioIntro");
	SetSound(szTextVal);

	szTextVal = kResults.GetText("AudioIntroHeader");
	SetSoundMP(szTextVal);

	//Arrays
	const char* szTechType = GetType();
	kUtility.PopulateArrayByValue(m_piDomainExtraMoves, "Domains", "Technology_DomainExtraMoves", "DomainType", "TechType", szTechType, "Moves", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piTradeRouteDomainExtraRange, "Domains", "Technology_TradeRouteDomainExtraRange", "DomainType", "TechType", szTechType, "Range", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByExistence(m_pabFreePromotion, "UnitPromotions", "Technology_FreePromotions", "PromotionType", "TechType", szTechType);
	kUtility.SetFlavors(m_piFlavorValue, "Technology_Flavors", "TechType", szTechType);

	const size_t TechnologiesCount = kUtility.MaxRows("Technologies");

	//ORPrereqTechs
	{
		//PrereqTech array must be initialized to NO_TECH.

		kUtility.InitializeArray(m_piPrereqOrTechs, TechnologiesCount, NO_TECH);

		std::string strKey = "Technologies - Technology_ORPrereqTechs";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Technologies.ID from Technology_ORPrereqTechs inner join Technologies on Technologies.Type = PrereqTech where TechType = ?;");
		}

		pResults->Bind(1, szTechType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqOrTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//PrereqTechs
	{
		//PrereqTech array must be initialized to NO_TECH.
		kUtility.InitializeArray(m_piPrereqAndTechs, TechnologiesCount, NO_TECH);

		std::string strKey = "Technologies - Technology_PrereqTechs";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Technologies.ID from Technology_PrereqTechs inner join Technologies on Technologies.Type = PrereqTech where TechType = ?;");
		}

		pResults->Bind(1, szTechType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}
#if defined(MOD_BALANCE_CORE)
	{
		kUtility.Initialize2DArray(m_ppiTechYieldChanges, "Specialists", "Yields");

		std::string strKey("Tech_SpecialistYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Specialists.ID as SpecialistID, Yields.ID as YieldID, Yield from Tech_SpecialistYieldChanges inner join Specialists on Specialists.Type = SpecialistType inner join Yields on Yields.Type = YieldType where TechType = ?");
		}

		pResults->Bind(1, szTechType);

		while(pResults->Step())
		{
			const int SpecialistID = pResults->GetInt(0);
			const int YieldID = pResults->GetInt(1);
			const int yield = pResults->GetInt(2);

			m_ppiTechYieldChanges[SpecialistID][YieldID] = yield;
		}
	}
#endif
	return true;
}

/// Additional weight to having AI purchase this
int CvTechEntry::GetAIWeight() const
{
	return m_iAIWeight;
}

/// Additional weight to having AI trade for this
int CvTechEntry::GetAITradeModifier() const
{
	return m_iAITradeModifier;
}

/// Research/science points required to obtain tech
int CvTechEntry::GetResearchCost() const
{
	return m_iResearchCost;
}

/// Cost if starting midway through game
int CvTechEntry::GetAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

/// Historical era within tech tree
int CvTechEntry::GetEra() const
{
	return m_iEra;
}

/// Changes builder production
int CvTechEntry::GetFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}

/// Changes combat bonus from fortification
int CvTechEntry::GetUnitFortificationModifier() const
{
	return m_iUnitFortificationModifier;
}

/// Changes base unit healing rate
int CvTechEntry::GetUnitBaseHealModifier() const
{
	return m_iUnitBaseHealModifier;
}

/// Improvement in worker speed
int CvTechEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

/// Free unit if first to discover this tech
int CvTechEntry::GetFirstFreeUnitClass() const
{
	return m_iFirstFreeUnitClass;
}

/// Earn another tech for free if first to discover this one
int CvTechEntry::GetFirstFreeTechs() const
{
	return m_iFirstFreeTechs;
}

/// Number of additional moves provided to land Units embarked on the water
int CvTechEntry::GetEmbarkedMoveChange() const
{
	return m_iEmbarkedMoveChange;
}

/// Number of additional land trade routes provided
int CvTechEntry::GetNumInternationalTradeRoutesChange (void) const
{
	return m_iInternationalTradeRoutesChange;
}

/// Boost to cultural influence output
int CvTechEntry::GetInfluenceSpreadModifier() const
{
	return m_iInfluenceSpreadModifier;
}

/// Number of votes gained from each Diplomat
int CvTechEntry::GetExtraVotesPerDiplomat() const
{
	return m_iExtraVotesPerDiplomat;
}

/// X location on tech tree page
int CvTechEntry::GetGridX() const
{
	return m_iGridX;
}

/// Y location on tech tree page
int CvTechEntry::GetGridY() const
{
	return m_iGridY;
}

/// Does this Tech end the game?
bool CvTechEntry::IsEndsGame() const
{
	return m_bEndsGame;
}

/// Unlocks the ability to embark land Units onto self-carried boats
bool CvTechEntry::IsAllowsEmbarking() const
{
	return m_bAllowsEmbarking;
}

/// Allows embarked units to defend themselves
bool CvTechEntry::IsAllowsDefensiveEmbarking() const
{
	return m_bAllowsDefensiveEmbarking;
}

/// Allows embarked Units to enter deep ocean
bool CvTechEntry::IsEmbarkedAllWaterPassage() const
{
	return m_bEmbarkedAllWaterPassage;
}

/// Are the Barbarians allowed to have Boats yet?
bool CvTechEntry::IsAllowsBarbarianBoats() const
{
	return m_bAllowsBarbarianBoats;
}

/// Can this be researched repeatedly (future tech)
bool CvTechEntry::IsRepeat() const
{
	return m_bRepeat;
}

/// Is it tradeable?
bool CvTechEntry::IsTrade() const
{
	return m_bTrade;
}

/// Currently disabled (not in game)?
bool CvTechEntry::IsDisable() const
{
	return m_bDisable;
}

/// Can you receive it from a goody hut?
bool CvTechEntry::IsGoodyTech() const
{
	return m_bGoodyTech;
}

/// Does this tech trigger the spawning of Archaeological Sites?
bool CvTechEntry::IsTriggersArchaeologicalSites() const
{
	return m_bTriggersArchaeologicalSites;
}

/// Does this tech allow the founding of the World Congress?
bool CvTechEntry::IsAllowsWorldCongress() const
{
	return m_bAllowsWorldCongress;
}

/// Expand visibility over water?
bool CvTechEntry::IsExtraWaterSeeFrom() const
{
	return m_bExtraWaterSeeFrom;
}

/// Does it center your mini-map in the overall world?
bool CvTechEntry::IsMapCentering() const
{
	return m_bMapCentering;
}

/// Does this tech reveal the entire map?
bool CvTechEntry::IsMapVisible() const
{
	return m_bMapVisible;
}

/// Does it enable world map trades?
bool CvTechEntry::IsMapTrading() const
{
	return m_bMapTrading;
}

/// Does it enable technology trading?
bool CvTechEntry::IsTechTrading() const
{
	return m_bTechTrading;
}

/// Does it enable gold trading?
bool CvTechEntry::IsGoldTrading() const
{
	return m_bGoldTrading;
}

/// Can you permit allow embassy to be traded?
bool CvTechEntry::IsAllowEmbassyTradingAllowed() const
{
	return m_bAllowEmbassyTradingAllowed;
}

/// Can you permit open borders?
bool CvTechEntry::IsOpenBordersTradingAllowed() const
{
	return m_bOpenBordersTradingAllowed;
}

/// Can you form defensive treaties?
bool CvTechEntry::IsDefensivePactTradingAllowed() const
{
	return m_bDefensivePactTradingAllowed;
}

/// Can you form Research Agreements?
bool CvTechEntry::IsResearchAgreementTradingAllowed() const
{
	return m_bResearchAgreementTradingAllowed;
}

/// Can you form Trade Agreements?
bool CvTechEntry::IsTradeAgreementTradingAllowed() const
{
	return m_bTradeAgreementTradingAllowed;
}

/// Can you form a permanent alliance?
bool CvTechEntry::IsPermanentAllianceTrading() const
{
	return m_bPermanentAllianceTrading;
}

#if defined(MOD_TECHS_CITY_WORKING)
/// Change in number of rings a city can work
int CvTechEntry::GetCityWorkingChange() const
{
	return m_iCityWorkingChange;
}
#endif

#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
/// Change the number of automaton workers a city can have
int CvTechEntry::GetCityAutomatonWorkersChange() const
{
	return m_iCityAutomatonWorkersChange;
}
#endif

/// Are river crossings treated as bridges?
bool CvTechEntry::IsBridgeBuilding() const
{
	return m_bBridgeBuilding;
}

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
/// Do cities cost less embark movement?
bool CvTechEntry::IsCityLessEmbarkCost() const
{
	return m_bCityLessEmbarkCost;
}
/// Do cities no longer cost embark movement?
bool CvTechEntry::IsCityNoEmbarkCost() const
{
	return m_bCityNoEmbarkCost;
}
#endif

/// Enable working of water tiles?
bool CvTechEntry::IsWaterWork() const
{
	return m_bWaterWork;
}

/// Grants free promotion?
int CvTechEntry::IsFreePromotion(int i) const
{
	return m_pabFreePromotion ? m_pabFreePromotion[i] : -1;
}

/// Provides historical quote
const char* CvTechEntry::GetQuote()
{
	return m_wstrQuote.c_str();
}

/// Set text key for quote
void CvTechEntry::SetQuoteKey(const char* szVal)
{
	m_strQuoteKey = szVal;
	m_wstrQuote = GetLocalizedText(m_strQuoteKey);
}

/// Get sound effect related to tech
const char* CvTechEntry::GetSound() const
{
	return m_strSound;
}

/// Set sound effect related to tech
void CvTechEntry::SetSound(const char* szVal)
{
	m_strSound = szVal;
}

/// Get sound effect related to tech (team)
const char* CvTechEntry::GetSoundMP() const
{
	return m_strSoundMP;
}

/// Set sound effect related to tech (team)
void CvTechEntry::SetSoundMP(const char* szVal)
{
	m_strSoundMP = szVal;
}

// ARRAYS

/// How much extra movement does it give you in this domain?
int CvTechEntry::GetDomainExtraMoves(int i) const
{
	return m_piDomainExtraMoves ? m_piDomainExtraMoves[i] : -1;
}

/// How much is range extended in this domain?
int CvTechEntry::GetTradeRouteDomainExtraRange(int i) const
{
	return m_piTradeRouteDomainExtraRange ? m_piTradeRouteDomainExtraRange[i] : -1;
}


/// Find value of flavors associated with this tech
int CvTechEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}

/// Prerequisite techs with OR
int CvTechEntry::GetPrereqOrTechs(int i) const
{
	return m_piPrereqOrTechs ? m_piPrereqOrTechs[i] : -1;
}

/// Prerequisite techs with AND
int CvTechEntry::GetPrereqAndTechs(int i) const
{
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvTechEntry::GetTechYieldChanges(int i, int j) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	CvAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(j > -1, "Index out of bounds");
	return m_ppiTechYieldChanges[i][j];
}
int CvTechEntry::GetHappiness() const
{
	return m_iHappiness;
}
bool CvTechEntry::IsCorporationsEnabled() const
{
	return m_bCorporationsEnabled;
}
#endif

#if defined(MOD_CIV6_EUREKA)
int CvTechEntry::GetEurekaPerMillion() const
{
	return m_iEurekaPerMillion;
}
#endif

//=====================================
// CvTechXMLEntries
//=====================================
/// Constructor
CvTechXMLEntries::CvTechXMLEntries(void)
{

}

/// Destructor
CvTechXMLEntries::~CvTechXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of tech entries
std::vector<CvTechEntry*>& CvTechXMLEntries::GetTechEntries()
{
	return m_paTechEntries;
}

/// Number of defined techs
int CvTechXMLEntries::GetNumTechs()
{
	return m_paTechEntries.size();
}

/// Clear tech entries
void CvTechXMLEntries::DeleteArray()
{
	for(std::vector<CvTechEntry*>::iterator it = m_paTechEntries.begin(); it != m_paTechEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTechEntries.clear();
}

/// Get a specific entry
CvTechEntry* CvTechXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_TECH) ? m_paTechEntries[index] : NULL;
#else
	return m_paTechEntries[index];
#endif
}


//=====================================
// CvPlayerTechs
//=====================================
/// Constructor
CvPlayerTechs::CvPlayerTechs():
	m_pabResearchingTech(NULL),
	m_piCivTechPriority(NULL),
	m_piLocaleTechPriority(NULL),
#if defined(MOD_BALANCE_CORE)
	m_piGSTechPriority(NULL),
	m_bHasUUTech(false),
	m_bWillHaveUUTechSoon(false),
#endif
	m_peLocaleTechResources(NULL),
	m_peCivTechUniqueBuildings(NULL),
	m_peCivTechUniqueUnits(NULL),
	m_peCivTechUniqueImprovements(NULL),
	m_pTechs(NULL),
	m_pPlayer(NULL),
	m_pTechAI(NULL)
{
}

/// Destructor
CvPlayerTechs::~CvPlayerTechs(void)
{
}

/// Initialize
void CvPlayerTechs::Init(CvTechXMLEntries* pTechs, CvPlayer* pPlayer, bool bIsCity)
{
	// Init base class
	CvFlavorRecipient::Init();

	// Store off the pointers to objects we'll need later
	m_bIsCity = bIsCity;
	m_pTechs = pTechs;
	m_pPlayer = pPlayer;

	// Initialize arrays
	const int iNumTechs = m_pTechs->GetNumTechs();

	CvAssertMsg(m_pabResearchingTech==NULL, "about to leak memory, CvPlayerTechs::m_pabResearchingTech");
	m_pabResearchingTech = FNEW(bool[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_piCivTechPriority==NULL, "about to leak memory, CvPlayerTechs::m_piCivTechPriority");
	m_piCivTechPriority = FNEW(int[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_piLocaleTechPriority==NULL, "about to leak memory, CvPlayerTechs::m_piLocaleTechPriority");
	m_piLocaleTechPriority = FNEW(int[iNumTechs], c_eCiv5GameplayDLL, 0);
#if defined(MOD_BALANCE_CORE)
	CvAssertMsg(m_piGSTechPriority==NULL, "about to leak memory, CvPlayerTechs::m_piGSTechPriority");
	m_piGSTechPriority = FNEW(int[iNumTechs], c_eCiv5GameplayDLL, 0);
#endif
	CvAssertMsg(m_peLocaleTechResources==NULL, "about to leak memory, CvPlayerTechs::m_peLocaleTechResources");
	m_peLocaleTechResources = FNEW(ResourceTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueUnits==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueUnits");
	m_peCivTechUniqueUnits = FNEW(UnitTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueBuildings==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueBuildings");
	m_peCivTechUniqueBuildings = FNEW(BuildingTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueImprovements==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueImprovements");
	m_peCivTechUniqueImprovements = FNEW(ImprovementTypes[iNumTechs], c_eCiv5GameplayDLL, 0);

	// Create AI object
	m_pTechAI = FNEW(CvTechAI(this), c_eCiv5GameplayDLL, 0);

	Reset();
}

/// Deallocate memory created in initialize
void CvPlayerTechs::Uninit()
{
	// Uninit base class
	CvFlavorRecipient::Uninit();

	SAFE_DELETE_ARRAY(m_pabResearchingTech);
	SAFE_DELETE_ARRAY(m_piCivTechPriority);
	SAFE_DELETE_ARRAY(m_piLocaleTechPriority);
#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE_ARRAY(m_piGSTechPriority);
#endif
	SAFE_DELETE_ARRAY(m_peLocaleTechResources);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueBuildings);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueUnits);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueImprovements);
	SAFE_DELETE(m_pTechAI);
}

/// Reset tech status array to all false
void CvPlayerTechs::Reset()
{
	int iI;
	CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_pabResearchingTech[iI] = false;
		m_piCivTechPriority[iI] = 1;
		m_piLocaleTechPriority[iI] = 1;
#if defined(MOD_BALANCE_CORE)
		m_piGSTechPriority[iI] = 1;
#endif
		m_peLocaleTechResources[iI] = NO_RESOURCE;
		m_peCivTechUniqueUnits[iI] = NO_UNIT;
		m_peCivTechUniqueBuildings[iI] = NO_BUILDING;
		m_peCivTechUniqueImprovements[iI] = NO_IMPROVEMENT;
	}

	m_bHasUUTech = false;
	m_bWillHaveUUTechSoon = false;

	// Tweak tech priorities to recognize unique properties of this civ
	if(!m_pPlayer->isMinorCiv() && !m_pPlayer->isBarbarian() && m_pPlayer->getCivilizationType() != NO_CIVILIZATION)
	{
		CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
		if(pkInfo)
		{
			// Loop through all building classes
			for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(pkInfo->getCivilizationBuildings(iI));
				CvBuildingEntry* pkBuildingInfo = NULL;

				if(eBuilding != -1)
					pkBuildingInfo = pkGameBuildings->GetEntry(eBuilding);

				if(pkBuildingInfo)
				{
					// Is this one overridden for our civ?
					if(pkInfo->isCivilizationBuildingOverridden(iI))
					{
						int iTech = pkBuildingInfo->GetPrereqAndTech();
						if(iTech != NO_TECH)
						{
							m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM();
							m_peCivTechUniqueBuildings[iTech] = eBuilding;
						}
					}

#if defined(AUI_PLAYERTECHS_RESET_IDEOLOGY_UNLOCKERS_COUNT_AS_UNIQUE)
					// Does this building unlock an ideology?
					if (pkBuildingInfo->GetXBuiltTriggersIdeologyChoice() > 0)
					{
						int iTech = pkBuildingInfo->GetPrereqAndTech();
						if (iTech != NO_TECH)
						{
							m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM();
						}
					}
#endif // AUI_PLAYERTECHS_RESET_IDEOLOGY_UNLOCKERS_COUNT_AS_UNIQUE
					if (GC.getGame().getElapsedGameTurns() > 5)
					{
						if (pkBuildingInfo->GetPolicyBranchType() != NO_POLICY_BRANCH_TYPE)
						{
							if (m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked((PolicyBranchTypes)pkBuildingInfo->GetPolicyBranchType()))
							{
								int iTech = pkBuildingInfo->GetPrereqAndTech();
								if (iTech != NO_TECH)
								{
									m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM() / 2;
								}
							}
						}

						if (pkBuildingInfo->GetPolicyType() != NO_POLICY)
						{
							if (m_pPlayer->GetPlayerPolicies()->HasPolicy((PolicyTypes)pkBuildingInfo->GetPolicyType()))
							{
								int iTech = pkBuildingInfo->GetPrereqAndTech();
								if (iTech != NO_TECH)
								{
									m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM() / 2;
								}
							}
							else
							{
								PolicyBranchTypes eBranch = (PolicyBranchTypes)GC.getPolicyInfo((PolicyTypes)pkBuildingInfo->GetPolicyType())->GetPolicyBranchType();
								if (eBranch != NO_POLICY_BRANCH_TYPE)
								{
									if (m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
									{
										int iTech = pkBuildingInfo->GetPrereqAndTech();
										if (iTech != NO_TECH)
										{
											m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM() / 3;
										}
									}
								}
							}
						}
					}
				}
			}

			// Loop through all units
			for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
			{
				// Is this one overridden for our civ?
				if(pkInfo->isCivilizationUnitOverridden(iI))
				{
					UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(iI));
					if(eCivilizationUnit != NO_UNIT)
					{
						CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
						if(pkUnitEntry)
						{
							int iTech = pkUnitEntry->GetPrereqAndTech();
							if(iTech != NO_TECH)
							{
								m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM();
								m_peCivTechUniqueUnits[iTech] = (UnitTypes)pkInfo->getCivilizationUnits(iI);
							}
						}
					}
				}
			}

			// Loop through all improvements
			for(iI = 0; iI < GC.getNumImprovementInfos(); iI++)
			{
				CvImprovementEntry* pkImprovementEntry = GC.getImprovementInfo((ImprovementTypes)iI);
				if(pkImprovementEntry)
				{
					if(pkImprovementEntry->IsSpecificCivRequired() && pkImprovementEntry->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
					{
						// Find corresponding build
						for(int jJ = 0; jJ < GC.getNumBuildInfos(); jJ++)
						{
							CvBuildInfo* pkBuildEntry = GC.getBuildInfo((BuildTypes)jJ);
							if(pkBuildEntry && pkBuildEntry->getImprovement() == iI)
							{
								int iTech = pkBuildEntry->getTechPrereq();
								if(iTech != NO_TECH)
								{
									m_piCivTechPriority[iTech] += GC.getTECH_PRIORITY_UNIQUE_ITEM();
									m_peCivTechUniqueImprovements[iTech] = static_cast<ImprovementTypes>(iI);
								}
							}
						}
					}
				}
			}
		}

		// Player Traits
		int iNumTraits = GC.getNumTraitInfos();
		for(int iTraitLoop = 0; iTraitLoop < iNumTraits; iTraitLoop++)
		{
			TraitTypes eTraitLoop = (TraitTypes) iTraitLoop;
			// Do we have this trait?
			const CvLeaderHeadInfo* pkLeaderInfo = &m_pPlayer->getLeaderInfo();
			if(pkLeaderInfo)
			{
				if(!pkLeaderInfo->hasTrait(iTraitLoop))  // This trait check disregards tech prereqs and obsoletes
					continue;

				CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTraitLoop);
				if(pkTraitInfo)
				{
					// Maya Calendar trait - We want to heavily weight the unlock tech
					if(pkTraitInfo->IsMayaCalendarBonuses())
					{
						int iPrereqTech = pkTraitInfo->GetPrereqTech();
						if(iPrereqTech != NO_TECH)
						{
							m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
						}
					}

					// Venice needs love
					int iPrereqTech = pkTraitInfo->GetFreeUnitPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}

					iPrereqTech = pkTraitInfo->GetCapitalFreeBuildingPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}


					iPrereqTech = pkTraitInfo->GetFreeBuildingPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}
				}
			}
		}
	}

	// Reset AI too
	m_pTechAI->Reset();
}

/// Serialization read
void CvPlayerTechs::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	// TODO: If m_pTechs is NULL then the stream will not be advanced causing errors to occur.
	CvAssertMsg(m_pTechs != NULL && m_pTechs->GetNumTechs() > 0, "Number of techs to serialize is expected to greater than 0");
	if(m_pTechs != NULL)
	{
		const int iNumTechs = m_pTechs->GetNumTechs();

		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabResearchingTech, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piCivTechPriority, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piLocaleTechPriority, iNumTechs);
#if defined(MOD_BALANCE_CORE)
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piGSTechPriority, iNumTechs);
#endif
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peLocaleTechResources, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueUnits, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueBuildings, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueImprovements, iNumTechs);
	}

	// Now for AI
	m_pTechAI->Read(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> kLatestFlavorWrapper(iNumFlavors, m_piLatestFlavorValues);
	kStream >> kLatestFlavorWrapper;

	kStream >> m_bHasUUTech;
	kStream >> m_bWillHaveUUTechSoon;
}

/// Serialization write
void CvPlayerTechs::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	// TODO: If m_pTechs is NULL then the stream will not advance
	CvAssertMsg(m_pTechs != NULL && m_pTechs->GetNumTechs() > 0, "Number of techs to serialize is expected to greater than 0");
	if(m_pTechs != NULL)
	{
		const int iNumTechs = m_pTechs->GetNumTechs();

		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, bool>(kStream, m_pabResearchingTech, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, int>(kStream, m_piCivTechPriority, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, int>(kStream, m_piLocaleTechPriority, iNumTechs);
#if defined(MOD_BALANCE_CORE)
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, int>(kStream, m_piGSTechPriority, iNumTechs);
#endif
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, ResourceTypes>(kStream, m_peLocaleTechResources, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, UnitTypes>(kStream, m_peCivTechUniqueUnits, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, BuildingTypes>(kStream, m_peCivTechUniqueBuildings, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, ImprovementTypes>(kStream, m_peCivTechUniqueImprovements, iNumTechs);
	}

	// Now for AI
	m_pTechAI->Write(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);

	kStream << m_bHasUUTech;
	kStream << m_bWillHaveUUTechSoon;
}

/// Respond to a new set of flavor values
void CvPlayerTechs::FlavorUpdate()
{
	SetLocalePriorities();
#if defined(MOD_BALANCE_CORE)
	SetGSPriorities();
	if(GetPlayer()->HasUUPeriod())
	{
		CheckHasUUTech();
		CheckWillHaveUUTechSoon();
	}
#endif
	AddFlavorAsStrategies(GC.getTECH_WEIGHT_PROPAGATION_PERCENT());
}

/// Accessor: Player object
CvPlayer* CvPlayerTechs::GetPlayer()
{
	return m_pPlayer;
}

/// Accessor: TechAI object
CvTechAI* CvPlayerTechs::GetTechAI()
{
	return m_pTechAI;
}

/// Accessor: is a player researching a tech?
bool CvPlayerTechs::IsResearchingTech(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabResearchingTech[eIndex];
}

/// Accessor: set whether player is researching a tech
void CvPlayerTechs::SetResearchingTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabResearchingTech[eIndex] != bNewValue)
	{
		m_pabResearchingTech[eIndex] = bNewValue;
	}
}

/// Accessor: set Civ's priority multiplier for researching techs (for instance techs that unlock civ unique bonuses)
void CvPlayerTechs::SetCivTechPriority(TechTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_piCivTechPriority[eIndex] = iNewValue;
}

/// Accessor: get Civ's priority multiplier for researching techs (for instance techs that unlock civ unique bonuses)
int CvPlayerTechs::GetCivTechPriority(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_piCivTechPriority[eIndex];
}

/// Accessor: set locale priority multiplier for researching techs (for instance techs that unlock nearby resources)
void CvPlayerTechs::SetLocaleTechPriority(TechTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_piLocaleTechPriority[eIndex] = iNewValue;
}

/// Accessor: get locale priority multiplier for researching techs (for instance techs that unlock nearby resources)
int CvPlayerTechs::GetLocaleTechPriority(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_piLocaleTechPriority[eIndex];
}
#if defined(MOD_BALANCE_CORE)
/// Accessor: get GC priority multiplier for researching techs
int CvPlayerTechs::GetGSTechPriority(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_piGSTechPriority[eIndex];
}
/// Accessor: set locale priority multiplier for researching techs
void CvPlayerTechs::SetGSTechPriority(TechTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_piGSTechPriority[eIndex] = iNewValue;
}
#endif
ResourceTypes CvPlayerTechs::GetLocaleTechResource(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peLocaleTechResources[eIndex];
}

UnitTypes CvPlayerTechs::GetCivTechUniqueUnit(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueUnits[eIndex];
}

BuildingTypes CvPlayerTechs::GetCivTechUniqueBuilding(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueBuildings[eIndex];
}

ImprovementTypes CvPlayerTechs::GetCivTechUniqueImprovement(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueImprovements[eIndex];
}

/// Recompute weights taking into account tech cost
void CvPlayerTechs::SetLocalePriorities()
{
	int iLoop;
	CvCity* pCity;
	int iI;

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_piLocaleTechPriority[iI] = 1;
		m_peLocaleTechResources[iI] = NO_RESOURCE;
	}

	// Loop through all our cities
	int iBestMultiplier = 0;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		// Look at all Tiles this City could potentially work to see if there are any non-water resources that could be improved
		for(int iPlotLoop = 0; iPlotLoop < pCity->GetNumWorkablePlots() ; iPlotLoop++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pCity->getX(), pCity->getY(), iPlotLoop);

			if(pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pCity->getX(), pCity->getY());

				if(pLoopPlot->getOwner() == pCity->getOwner() || (iDistance <= 4 && pLoopPlot->getOwner() == NO_PLAYER))
				{
					int multiplierValue = 3 - iDistance;
				
					ResourceTypes eResource = pLoopPlot->getResourceType(m_pPlayer->getTeam());
					if(eResource == NO_RESOURCE)
					{
						continue;
					}

					//already connected?
					if (pLoopPlot->IsResourceLinkedCityActive())
						continue;

					if (m_pPlayer->getNumResourceTotal(eResource, false) <= 0)
						multiplierValue++;

					//somehow close on monopolies?
					if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
					{
						int iMonopolyVal = m_pPlayer->GetMonopolyPercent(eResource);
						if (iMonopolyVal > GC.getSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD() && iMonopolyVal <= GC.getGLOBAL_RESOURCE_MONOPOLY_THRESHOLD())
							multiplierValue++;
					}

					// Loop through the build types to find one that we can use
					ImprovementTypes eCorrectImprovement = NO_IMPROVEMENT;
					BuildTypes eCorrectBuild = NO_BUILD;
					int iBuildIndex;
					for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
					{
						const BuildTypes eBuild = static_cast<BuildTypes>(iBuildIndex);
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
						if(pkBuildInfo)
						{
							// If this is the improvement we're looking for
							const ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
							if(eImprovement != NO_IMPROVEMENT)
							{
								CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
								if (pkImprovementInfo && pkImprovementInfo->IsExpandedImprovementResourceTrade(eResource))
								{
									if (pLoopPlot->canHaveImprovement(eImprovement))
										multiplierValue++;

									eCorrectBuild = eBuild;
									eCorrectImprovement = eImprovement;
									break;
								}
							}
						}
					}

					// No valid build found
					if(eCorrectBuild == NO_BUILD || eCorrectImprovement == NO_IMPROVEMENT)
					{
						continue;
					}


					// Looking for cases where we can't build the improvement for the resource
					if (!m_pPlayer->canBuild(pLoopPlot, eCorrectBuild, false, false))
						multiplierValue++;

					// Find the tech associated with this build and increment its multiplier
					int iTech = GC.getBuildInfo(eCorrectBuild)->getTechPrereq();
					if (iTech != -1)
					{
						m_piLocaleTechPriority[iTech] += multiplierValue;
						if (multiplierValue > iBestMultiplier)
						{
							iBestMultiplier = multiplierValue;
							m_peLocaleTechResources[iTech] = eResource;								
						}
					}
				}
			}
		}
	}
}
#if defined(MOD_BALANCE_CORE)
void CvPlayerTechs::SetGSPriorities()
{
	for(int iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_piGSTechPriority[iI] = 1;
	}

	//preparation
	map<TechTypes, vector<UnitTypes>> unitPrereqTechs;
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		UnitTypes eUnit = (UnitTypes)iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

		if (pkUnitInfo && pkUnitInfo->GetPrereqAndTech() != NO_TECH)
			unitPrereqTechs[(TechTypes)pkUnitInfo->GetPrereqAndTech()].push_back(eUnit);
	}

	//preparation pt2
	map<TechTypes, vector<BuildingTypes>> buildingPrereqTechs;
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo && pkBuildingInfo->GetPrereqAndTech() != NO_TECH)
			buildingPrereqTechs[(TechTypes)pkBuildingInfo->GetPrereqAndTech()].push_back(eBuilding);
	}

	// == Grand Strategy ==
	AIGrandStrategyTypes eGrandStrategy = m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
	bool bSeekingDiploVictory = (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS")) || m_pPlayer->GetDiplomacyAI()->IsCloseToDiploVictory() || m_pPlayer->GetPlayerTraits()->IsDiplomat();
	bool bSeekingConquestVictory = (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST") ) || m_pPlayer->GetDiplomacyAI()->IsCloseToDominationVictory() || m_pPlayer->GetPlayerTraits()->IsWarmonger();
	bool bSeekingCultureVictory = (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE") ) || m_pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory() || m_pPlayer->GetPlayerTraits()->IsTourism();
	bool bSeekingScienceVictory = (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP") ) || m_pPlayer->GetDiplomacyAI()->IsCloseToSSVictory() || m_pPlayer->GetPlayerTraits()->IsNerd();
	for(int iTechLoop = 0; iTechLoop < GetTechs()->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;
		if(eTech == NO_TECH)
			continue;

		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo == NULL)
			continue;

		for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
		{
			FlavorTypes eFlavor = (FlavorTypes)iFlavor;
			if(eFlavor == NO_FLAVOR)
				continue;

			if(pkTechInfo->GetFlavorValue(eFlavor) > 0)
			{
				if (bSeekingDiploVictory && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DIPLOMACY"
					|| GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GOLD"
					|| GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ESPIONAGE"))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if(bSeekingConquestVictory && (
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_OFFENSE" || 
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MILITARY_TRAINING" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MOBILE" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RANGED" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DEFENSE" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIR" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL" ))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if(bSeekingCultureVictory && (
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_CULTURE" || 
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GREAT_PEOPLE" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_WONDER"))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if(bSeekingScienceVictory && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SCIENCE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SPACESHIP"))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if (m_pPlayer->GetPlayerTraits()->IsSmaller() && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GROWTH" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL_GROWTH"
					|| GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_CITY_DEFENSE"
					))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if (m_pPlayer->GetPlayerTraits()->IsReligious() && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RELIGION" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GREAT_PEOPLE"))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if (m_pPlayer->GetPlayerTraits()->IsExpansionist() && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_INFRASTRUCTURE" ||
					GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_EXPANSION"))
				{
					m_piGSTechPriority[iTechLoop]++;
				}
			}
		}
		//Let's look at grandstrategy values for buildings as well and add those in to techs.
		for(size_t i=0; i<buildingPrereqTechs[eTech].size(); i++)
		{
			const BuildingTypes eBuilding = buildingPrereqTechs[eTech][i];
			int iTechGSValue = CityStrategyAIHelpers::GetBuildingGrandStrategyValue(NULL, eBuilding, m_pPlayer->GetID());
			if (iTechGSValue > 0)
			{
				m_piGSTechPriority[iTechLoop]++;
			}
		}

		//Let's look at grandstrategy values for units as well and add those in to techs.
		for(size_t i=0; i<unitPrereqTechs[eTech].size(); i++)
		{
			const UnitTypes eUnit = unitPrereqTechs[eTech][i];
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

			if (pkUnitInfo && pkUnitInfo->GetPrereqAndTech() == iTechLoop)
			{
				if (pkUnitInfo->IsFound() || pkUnitInfo->IsFoundAbroad() || pkUnitInfo->IsFoundMid() || pkUnitInfo->IsFoundLate())
				{
					m_piGSTechPriority[iTechLoop]+= max(1, 10 - m_pPlayer->getNumCities());
				}
				if (bSeekingConquestVictory)
				{
					if (pkUnitInfo->GetCombat() > 0 || pkUnitInfo->GetRangedCombat() > 0)
					{
						m_piGSTechPriority[iTechLoop]++;
					}
				}
				if (bSeekingScienceVictory && pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if (bSeekingDiploVictory && pkUnitInfo->IsTrade() || pkUnitInfo->GetBaseGold() > 0 || pkUnitInfo->GetDefaultUnitAIType() == UNITAI_MESSENGER)
				{
					m_piGSTechPriority[iTechLoop]++;
				}
				if (bSeekingCultureVictory && pkUnitInfo->GetDefaultUnitAIType() == UNITAI_ARCHAEOLOGIST)
				{
					m_piGSTechPriority[iTechLoop]++;
				}
			}
		}

		if (pkTechInfo->IsUnlocksEspionageAdvancedActions())
		{
			if (m_pPlayer->GetPlayerTraits()->GetExtraSpies() || m_pPlayer->GetPlayerTraits()->GetStartingSpyRank() > 0 || m_pPlayer->GetPlayerTraits()->GetStartingSpies() > 0)
			{
				m_piGSTechPriority[iTechLoop]++;
			}

			if (m_pPlayer->GetEspionage()->GetNumSpies() > 0)
			{
				m_piGSTechPriority[iTechLoop]++;
			}
		}
	}
}
#endif

/// Accessor: Can we start research?
bool CvPlayerTechs::IsResearch() const
{
	// Have we founded a city?
	if(!m_pPlayer->isFoundedFirstCity())
	{
		return false;
	}

	return true;
}

/// Accessor: Is this tech disabled?
bool CvPlayerTechs::CanEverResearch(TechTypes eTech) const
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return false;
	}

	if(pkTechInfo->IsDisable())
	{
		return false;
	}

	if(m_pPlayer->getCivilizationInfo().isCivilizationDisableTechs(eTech))
	{
		return false;
	}
#if defined(MOD_ERA_RESTRICTION)
	if(MOD_ERA_RESTRICTION)
	{
		if(GET_TEAM(m_pPlayer->getTeam()).GetCurrentEra() < pkTechInfo->GetEra())
		{
			return false;
		}
	}
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eTech);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanEverResearch", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}


	return true;
}

/// Accessor: Is it possible to research this tech?
bool CvPlayerTechs::CanResearch(TechTypes eTech, bool bTrade) const
{
	bool bFoundPossible;
	bool bFoundValid;
	int iI;

	CvTechEntry* pkTechEntry = GC.getTechInfo(eTech);
	if(pkTechEntry == NULL)
		return false;

	if(!IsResearch() && m_pPlayer->getAdvancedStartPoints() < 0)
	{
		return false;
	}

	// Do we already have this tech?
	if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
	{
		return false;
	}

	bFoundPossible = false;
	bFoundValid = false;

	// See if it is possible based on OR prereqs
	for(iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)pkTechEntry->GetPrereqOrTechs(iI);
		if(ePrereq != NO_TECH)
		{
			bFoundPossible = true;

			if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereq))
			{
				if(!bTrade || !GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->IsNoTradeTech(ePrereq))
				{
					bFoundValid = true;
					break;
				}
			}
		}
	}

	if(bFoundPossible && !bFoundValid)
	{
		return false;
	}

	// See if it is possible based on AND prereqs
	for(iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)pkTechEntry->GetPrereqAndTechs(iI);
		if(ePrereq != NO_TECH)
		{
			if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereq))
			{
				return false;
			}

			if(bTrade && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->IsNoTradeTech(ePrereq))
			{
				return false;
			}
		}
	}

	// Is it disabled for some reason?
	if(!CanEverResearch(eTech))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eTech);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanResearch", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}

/// Can we pick this as a Free Tech (ex. from a Wonder)?
bool CvPlayerTechs::CanResearchForFree(TechTypes eTech) const
{
	CvAssertMsg(eTech >= 0, "eTech is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTech < GC.getNumTechInfos(), "eTech is expected to be within maximum bounds (invalid Index)");
	if(eTech < 0 || eTech >= GC.getNumTechInfos()) return false;

	// We can pick any tech that we are able to research
	return CanResearch(eTech);
}

/// Accessor: Which tech are we researching?
TechTypes CvPlayerTechs::GetCurrentResearch() const
{
	CLLNode<TechTypes>* pResearchNode;

	pResearchNode = m_pPlayer->headResearchQueueNode();

	if(pResearchNode != NULL)
	{
		return pResearchNode->m_data;
	}
	else
	{
		return NO_TECH;
	}
}

/// Accessor: Are we currently researching something that can be repeatedly researched?
bool CvPlayerTechs::IsCurrentResearchRepeat() const
{
	const TechTypes eCurrentResearch = GetCurrentResearch();
	if(eCurrentResearch == NO_TECH)
	{
		return false;
	}

	CvTechEntry* pkTechInfo = GC.getTechInfo(eCurrentResearch);
	if(pkTechInfo == NULL)
	{
		return false;
	}

	return pkTechInfo->IsRepeat();
}

/// Accessor: Is there anything left to research?
bool CvPlayerTechs::IsNoResearchAvailable() const
{
	int iI;

	if(GetCurrentResearch() != NO_TECH)
	{
		return false;
	}

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if(CanResearch((TechTypes)iI))
		{
			return false;
		}
	}
	return true;
}


///Check for Achievement
void CvPlayerTechs::CheckForTechAchievement() const
{
#if !defined(NO_ACHIEVEMENTS)
	if(m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		//Check for Catherine Achievement
		if((CvString)m_pPlayer->getLeaderTypeKey() == "LEADER_CATHERINE")
		{
			for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
			{
				const TechTypes eTech = static_cast<TechTypes>(iI);
				CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
				if(pkTechInfo)
				{
					CvString szCurrentTech = (CvString) pkTechInfo->GetType();
					if(szCurrentTech == "TECH_HORSEBACK_RIDING")
					{
						if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
						{
							int iJ;
							int iNumPlayersWith = 0;
							for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
							{
								if(!GET_PLAYER((PlayerTypes)iJ).isBarbarian() && !GET_PLAYER((PlayerTypes)iJ).isMinorCiv())
								{
									if(GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).GetTeamTechs()->HasTech(eTech))
									{
										iNumPlayersWith++;
									}
								}
							}
							if(iNumPlayersWith <= 1)  //For only the human player
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_PONY);
							}
						}
					}
				}
			}
		}
		//Check for all achievements
		if(m_pPlayer->GetPlayerTechs()->IsCurrentResearchRepeat())
		{
			for(int iI = 0; iI < GC.getNumTechInfos() - 1; iI++)
			{
				const TechTypes eTech = static_cast<TechTypes>(iI);
				CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
				if(pkTechInfo)
				{
					if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
					{
						return;
					}
				}

			}

			if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetTechCount((TechTypes)m_pPlayer->GetPlayerTechs()->GetCurrentResearch()) < 1)
			{
				return;
			}
			gDLL->UnlockAchievement(ACHIEVEMENT_ALL_TECHS);
		}
	}
#endif
}

/// Accessor: How many turns of research left?
int CvPlayerTechs::GetResearchTurnsLeft(TechTypes eTech, bool bOverflow, int iAssumedResearchRate) const
{
	int iTurnsLeft = GetResearchTurnsLeftTimes100(eTech, bOverflow, iAssumedResearchRate);

	if(iTurnsLeft == INT_MAX)
	{
		return INT_MAX;
	}

	iTurnsLeft = (iTurnsLeft + 99) / 100; // round up

	return std::max(1, iTurnsLeft);
}

/// Accessor: How many turns of research left? (in hundredths)
int CvPlayerTechs::GetResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow, int iAssumedResearchRate) const
{
	int iResearchRate = 0;
	int iOverflow = 0;
	int iTurnsLeft;
	int iI;

	if (iAssumedResearchRate>0)
	{
		iResearchRate = iAssumedResearchRate;
		iOverflow = 0;
	}
	else
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive())
			{
				// Find everyone on our team
				if(kPlayer.getTeam() == m_pPlayer->getTeam())
				{
					// If this is us or if the tech matches, then increment totals
					if((iI == m_pPlayer->GetID()) || kPlayer.GetPlayerTechs()->GetCurrentResearch() == eTech)
					{
						iResearchRate += kPlayer.GetScienceTimes100();
						iOverflow += (kPlayer.getOverflowResearch() * m_pPlayer->calculateResearchModifier(eTech)) / 100;
					}
				}
			}
		}
	}

	if(iResearchRate == 0)
	{
		return INT_MAX;
	}

	int iResearchCost = GetResearchCost(eTech);				// Get our research cost (not the 'team' one which doesn't use our player modifier)
	// Get the team progress
	int iResearchProgress = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eTech);
	// Get the raw amount left
	int iResearchLeft = std::max(0, (iResearchCost - iResearchProgress));

	// Removed any current overflow if requested.
	if(bOverflow)
	{
		iResearchLeft -= iOverflow;
	}

	iResearchLeft *= 10000;

	iTurnsLeft = (iResearchLeft / iResearchRate);

	if(iTurnsLeft * iResearchRate < iResearchLeft)
	{
		++iTurnsLeft;
	}

	return std::max(1, iTurnsLeft);
}

/// How many techs can we research at present?
int CvPlayerTechs::GetNumTechsCanBeResearched() const
{
	int rtnValue = 0;

	for(int iTechLoop = 0; iTechLoop < GetTechs()->GetNumTechs(); iTechLoop++)
	{
		if(CanResearch((TechTypes)iTechLoop))
		{
			rtnValue++;
		}
	}

	return rtnValue;
}

/// Return the tech data (from XML)
CvTechXMLEntries* CvPlayerTechs::GetTechs() const
{
	return m_pTechs;
}

//	----------------------------------------------------------------------------
/// Return the research cost for a tech for this player.  This will be different from the team research cost as it will
/// include the player's research adjustment
int CvPlayerTechs::GetResearchCost(TechTypes eTech) const
{
	// Get the research cost for the team
	int iResearchCost = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchCost(eTech);
	
	// Adjust to the player's research modifier
	int iResearchMod = std::max(0, m_pPlayer->calculateResearchModifier(eTech) - 100);

	// Mod for City Count
	int iCityCountMod = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();	// Default is 40, gets smaller on larger maps

#if defined(MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
	iCityCountMod += m_pPlayer->GetTechCostXCitiesModifier();

	if (MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
	{
		iCityCountMod *= m_pPlayer->GetMaxEffectiveCities(/*bIncludePuppets*/ false);
	}
	else
	{
		iCityCountMod *= m_pPlayer->GetMaxEffectiveCities(/*bIncludePuppets*/ true);
	}
#else
	iMod *= m_pPlayer->GetMaxEffectiveCities(/*bIncludePuppets*/ true);
#endif

	//apply the modifiers
	if (iResearchCost<10000)
		return iResearchCost + (iResearchCost * (iCityCountMod - iResearchMod))/100;
	else
	return iResearchCost + (iResearchCost/100) * (iCityCountMod - iResearchMod);
}

//	----------------------------------------------------------------------------
/// Return the research progress for a tech for this player.
int CvPlayerTechs::GetResearchProgress(TechTypes eTech) const
{
	// Get the research progress for the team
	int iResearchProgress = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eTech);

	// Add in any overflow we have yet to accumulate into the research progress.
	// Overflow is the leftover research from the previous research.  It is automatically rolled into the main progress value
	// the next time research is 'updated'.
	return iResearchProgress + m_pPlayer->getOverflowResearch();
}

/// Median value of a tech we can research (that's what's awarded for research agreements now)
int CvPlayerTechs::GetMedianTechResearch() const
{
	vector<int> aiTechCosts;
	int iRtnValue = 0;

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;

		if(CanResearch(eTech))
		{
			aiTechCosts.push_back(GetResearchCost(eTech));
		}
	}

	int iNumEntries = aiTechCosts.size();
	if(iNumEntries > 0)
	{
		std::stable_sort(aiTechCosts.begin(), aiTechCosts.end());

		// Odd number, take middle?
		if((iNumEntries / 2) * 2 != iNumEntries)
		{
			iRtnValue = aiTechCosts[iNumEntries / 2];
		}

		// Even number, average middle 2
		else
		{
			iRtnValue = (aiTechCosts[(iNumEntries - 1) / 2] + aiTechCosts[iNumEntries / 2]) / 2;
		}
	}

	return iRtnValue;
}

bool CvPlayerTechs::HasUUTech() const
{
	return m_bHasUUTech;
}

void CvPlayerTechs::CheckHasUUTech()
{
	bool bHas = false;
	CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
	if (pkInfo)
	{
		// Loop through all units
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			if (bHas)
				break;

			// Is this one overridden for our civ?
			if (pkInfo->isCivilizationUnitOverridden(iI))
			{
				UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(iI));
				if (eCivilizationUnit != NO_UNIT)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
					if (pkUnitEntry)
					{
						int iTech = pkUnitEntry->GetPrereqAndTech();
						int iObsoleteTech = pkUnitEntry->GetObsoleteTech();
						if (iTech != NO_TECH && m_pPlayer->HasTech((TechTypes)iTech))
						{
							bHas = true;
						}
						else if (iTech == NO_TECH)
						{
							bHas = true;
						}
						if (iObsoleteTech != NO_TECH && m_pPlayer->HasTech((TechTypes)iObsoleteTech))
						{
							bHas = false;
						}			
					}
				}
			}
		}
	}
	if (m_bHasUUTech != bHas)
	{
		m_bHasUUTech = bHas;
		if ((GC.getLogging() && GC.getAILogging()))
		{
			CvString strLogString;
			strLogString.Format("We have our unique unit now - tuning our diplomacy to match");
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}
}

void CvPlayerTechs::CheckWillHaveUUTechSoon()
{
	//Already have? Set this false!
	if (HasUUTech())
	{
		if (m_bWillHaveUUTechSoon)
			m_bWillHaveUUTechSoon = false;

		return;
	}

	//Already know this? Shutup, baby, I know it!
	if (m_bWillHaveUUTechSoon)
		return;

	bool bWillHaveSoon = false;
	CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
	if (pkInfo)
	{
		// Loop through all units
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			// Is this one overridden for our civ?
			if (pkInfo->isCivilizationUnitOverridden(iI))
			{
				UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(iI));
				if (eCivilizationUnit != NO_UNIT)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
					if (pkUnitEntry)
					{
						int iTech = pkUnitEntry->GetPrereqAndTech();
						if (iTech != NO_TECH && !m_pPlayer->HasTech((TechTypes)iTech))
						{
							CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes)iTech);
							if (pkTechInfo)
							{
								if (IsResearchingTech((TechTypes)iTech))
								{
									bWillHaveSoon = true;
									break;
								}
								else
								{
									// See if it is possible based on AND prereqs
									for (int iJ = 0; iJ < GC.getNUM_AND_TECH_PREREQS(); iJ++)
									{
										TechTypes ePrereq = (TechTypes)pkTechInfo->GetPrereqAndTechs(iJ);
										if (ePrereq != NO_TECH && IsResearchingTech(ePrereq))
										{
											bWillHaveSoon = true;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (m_bWillHaveUUTechSoon != bWillHaveSoon)
	{
		m_bWillHaveUUTechSoon = bWillHaveSoon;
		if ((GC.getLogging() && GC.getAILogging()))
		{
			CvString strLogString;
			strLogString.Format("We are about to have our unique unit - tuning our diplomacy to match");
			m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}
}

bool CvPlayerTechs::WillHaveUUTechSoon() const
{
	return m_bWillHaveUUTechSoon;
}

// PRIVATE METHODS

// Internal method to add all of this leaderheads' flavors as strategies for tech AI
void CvPlayerTechs::AddFlavorAsStrategies(int iPropagatePercent)
{
	// Start by resetting the AI
	m_pTechAI->Reset();

	int iBiggestFlavor = -1000;
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);
		if(iFlavorValue > iBiggestFlavor)
		{
			iBiggestFlavor = iFlavorValue;
		}
	}

	// Now populate the AI with the current flavor information
#if defined(MOD_AI_SMART_V3)
	int iDifficultyBonus = (200 - ((GC.getGame().getHandicapInfo().getAIGrowthPercent() + GC.getGame().getHandicapInfo().getAITrainPercent()) / 2));
	int estimatedTurnsWithDiff = MOD_AI_SMART_V3 ? (GC.getGame().getDefaultEstimateEndTurn() * 90) / iDifficultyBonus : GC.getGame().getDefaultEstimateEndTurn();
	int iGameProgressFactor = (GC.getGame().getElapsedGameTurns() * 1000) / estimatedTurnsWithDiff;
#else
	int iGameProgressFactor = (GC.getGame().getElapsedGameTurns() * 1000) / GC.getGame().getDefaultEstimateEndTurn();
#endif
	iGameProgressFactor = min(900,max(100,iGameProgressFactor));
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iCurrentFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);

		// Scale the current to the same scale as the personality
		iCurrentFlavorValue = (iCurrentFlavorValue * 10) / max(1,iBiggestFlavor);

#if defined(MOD_AI_SMART_V3)
		int iPersonalityFlavorValue = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavor, MOD_AI_SMART_V3 /*bBoostGSMainFlavor*/);
#else
		int iPersonalityFlavorValue = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavor);
#endif

		// this should give a more even blend between the personality and long term strategy and the more fickle current needs
		// in the beginning of the game it will be responsive to current events, but later it should try to go for the goal more strongly
		int iFlavorValue = ((iCurrentFlavorValue * (1000 - iGameProgressFactor)) + (iPersonalityFlavorValue * iGameProgressFactor)) / 1000;

#if defined(MOD_AI_SMART_V3)
		// Try always give a significant flavor, as is easily zeroed with previous computations...
		if (MOD_AI_SMART_V3 && iFlavorValue < 10)
		{
			int flavorDivisor = (iGameProgressFactor > 500) ? 8 : 4;
			int boostValue = (10 - iFlavorValue) / flavorDivisor;
			iFlavorValue += boostValue;
		}
#endif

		if(iFlavorValue > 0)
		{
			m_pTechAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue, iPropagatePercent);
		}
	}

	//don't care about minors.
	if (m_pPlayer->isMinorCiv())
		return;

	/*
	// Loop through all our techs
	for (int iTech = 0; iTech < GC.getNumTechInfos(); iTech++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTech);
		CvTechEntry* entry = GC.GetGameTechs()->GetEntry(iTech);
		if (entry)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strOutBuf;
				CvString strBaseString;
				CvString strTemp;
				CvString playerName;
				CvString strDesc;

				// Find the name of this civ
				playerName = m_pPlayer->getCivilizationShortDescription();

				FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

				// Get the leading info for this line
				strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
				strBaseString += playerName + ", ";

				const char* szTechType = (entry != NULL) ? entry->GetType() : "Unknown Tech";

				strTemp.Format("Special Weights for %s are: Civ Val: %d, Locale Val: %d, GS Val: %d ", szTechType, m_pPlayer->GetPlayerTechs()->GetCivTechPriority(eTech), m_pPlayer->GetPlayerTechs()->GetLocaleTechPriority(eTech), m_pPlayer->GetPlayerTechs()->GetGSTechPriority(eTech));

				strOutBuf = strBaseString + strTemp;
				pLog->Msg(strOutBuf);
			}
		}
	}
	*/
}

/// Build log filename
CvString CvPlayerTechs::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "TechAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "TechAILog.csv";
	}

	return strLogName;
}

void CvPlayerTechs::LogFlavors(FlavorTypes eFlavor)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "TechAILog.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Dump out the setting for each flavor
		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_piLatestFlavorValues[iI]);
				strOutBuf = strBaseString + strTemp;
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), m_piLatestFlavorValues[eFlavor]);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

//=====================================
// CvTeamTechs
//=====================================
/// Constructor
CvTeamTechs::CvTeamTechs():
	m_pabHasTech(NULL),
	m_pabNoTradeTech(NULL),
	m_paiResearchProgress(NULL),
#if defined(MOD_CIV6_EUREKA)
	m_paiEurekaCounter(NULL),
#endif
	m_paiTechCount(NULL)
{
}

/// Destructor
CvTeamTechs::~CvTeamTechs(void)
{
}

/// Initialize
void CvTeamTechs::Init(CvTechXMLEntries* pTechs, CvTeam* pTeam)
{
	// Store off the pointer to the techs active for this game and pointer to our team
	m_pTechs = pTechs;
	m_pTeam = pTeam;

	// Initialize status arrays
	CvAssertMsg(m_pabHasTech==NULL, "about to leak memory, CvTeamTechs::m_pabHasTech");
	m_pabHasTech = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_pabNoTradeTech==NULL, "about to leak memory, CvTeamTechs::m_pabNoTradeTech");
	m_pabNoTradeTech = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_paiResearchProgress==NULL, "about to leak memory, CvTeamTechs::m_paiResearchProgress");
	m_paiResearchProgress = FNEW(int [m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
#if defined(MOD_CIV6_EUREKA)
	CvAssertMsg(m_paiEurekaCounter == NULL, "about to leak memory, CvTeamTechs::m_paiEurekaCounter");
	m_paiEurekaCounter = FNEW(int[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
#endif
	CvAssertMsg(m_paiTechCount==NULL, "about to leak memory, CvTeamTechs::m_paiTechCount");
	m_paiTechCount = FNEW(int [m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);

	Reset();
}

/// Deallocate memory created in initialize
void CvTeamTechs::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabHasTech);
	SAFE_DELETE_ARRAY(m_pabNoTradeTech);
	SAFE_DELETE_ARRAY(m_paiResearchProgress);
#if defined(MOD_CIV6_EUREKA)
	SAFE_DELETE_ARRAY(m_paiEurekaCounter);
#endif
	SAFE_DELETE_ARRAY(m_paiTechCount);
}

/// Reset tech status arrays
void CvTeamTechs::Reset()
{
	int iI;

	m_eLastTechAcquired = NO_TECH;
	m_iNumTechs = 0;

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_pabHasTech[iI] = false;
		m_pabNoTradeTech[iI] = false;
		m_paiResearchProgress[iI] = 0;
#if defined(MOD_CIV6_EUREKA)
		m_paiEurekaCounter[iI] = 0;
#endif
		m_paiTechCount[iI] = 0;
	}
}

//	---------------------------------------------------------------------------
/// Serialization read
void CvTeamTechs::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_eLastTechAcquired;
	kStream >> m_iNumTechs;

	// Read the number of techs
	int iNumSavedTechs;
	kStream >> iNumSavedTechs;

	if(iNumSavedTechs)
	{
		int iNumActiveTechs = m_pTechs->GetNumTechs();

		// Next is an array of the tech IDs that were available when the save was made.
		CvAssert(m_pTechs == GC.GetGameTechs());	// The hash to indices conversion will convert the hash to the index in the main game techs array, so these better be the same.
		int* paTechIDs = (int*)_malloca(iNumSavedTechs * sizeof(int));
		CvInfosSerializationHelper::ReadHashedTypeArray(kStream, iNumSavedTechs, paTechIDs, iNumSavedTechs);

		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabHasTech, iNumActiveTechs, paTechIDs);
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabNoTradeTech, iNumActiveTechs, paTechIDs);
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_paiResearchProgress, iNumActiveTechs, paTechIDs);
#if defined(MOD_CIV6_EUREKA)
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_paiEurekaCounter, iNumActiveTechs, paTechIDs);
#endif
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_paiTechCount, iNumActiveTechs, paTechIDs);

		_freea(paTechIDs);
	}
}

//	---------------------------------------------------------------------------
/// Serialization write
void CvTeamTechs::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_eLastTechAcquired;
	kStream << m_iNumTechs;

	if(m_pTechs != NULL && m_pTechs->GetNumTechs())
	{
		// Write out an array of all the active tech's hash types so we can re-map on loading if need be.
		int iNumTechs = m_pTechs->GetNumTechs();
		kStream << (int)iNumTechs;

		for(int i = 0; i < iNumTechs; ++i)
			CvInfosSerializationHelper::WriteHashed(kStream, m_pTechs->GetEntry(i));

		kStream << ArrayWrapper<bool>(iNumTechs, m_pabHasTech);
		kStream << ArrayWrapper<bool>(iNumTechs, m_pabNoTradeTech);
		kStream << ArrayWrapper<int>(iNumTechs, m_paiResearchProgress);
#if defined(MOD_CIV6_EUREKA)
		kStream << ArrayWrapper<int>(iNumTechs, m_paiEurekaCounter);
#endif
		kStream << ArrayWrapper<int>(iNumTechs, m_paiTechCount);
	}
	else
	{
		kStream << (int)0;
	}
}

/// Accessor: set whether team owns a tech
void CvTeamTechs::SetHasTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabHasTech[eIndex] != bNewValue)
	{
		m_pabHasTech[eIndex] = bNewValue;

		if(bNewValue)
			SetLastTechAcquired(eIndex);

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pTeam->GetID());
			args->Push(eIndex);
			args->Push(bNewValue);

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "TeamSetHasTech", args.get(), bResult);
		}
	}
}

/// Accessor: does team have a tech?
bool CvTeamTechs::HasTech(TechTypes eIndex) const
{
	if(eIndex == NO_TECH)
	{
		return true;
	}

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pabHasTech != NULL, "m_pabHasTech is not expected to be equal with NULL");
	if(m_pabHasTech != NULL)
		return m_pabHasTech[eIndex];
	else
		return false;
}

/// What was the most recent tech acquired?
TechTypes CvTeamTechs::GetLastTechAcquired() const
{
	return m_eLastTechAcquired;
}

/// What was the most recent tech acquired?
void CvTeamTechs::SetLastTechAcquired(TechTypes eTech)
{
	CvAssertMsg(eTech >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTech < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_eLastTechAcquired = eTech;
}

/// How many total Techs does this team have?
int CvTeamTechs::GetNumTechsKnown() const
{
	return m_iNumTechs;
}
void CvTeamTechs::ChangeNumTechsKnown(int iChange)
{
	if (iChange != 0)
		m_iNumTechs += iChange;
}

/// Has this team researched all techs once?
bool CvTeamTechs::HasResearchedAllTechs() const
{
	return (m_iNumTechs >= m_pTechs->GetNumTechs());
}

/// Accessor: set whether team owns a tech
void CvTeamTechs::SetNoTradeTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabNoTradeTech[eIndex] != bNewValue)
	{
		m_pabNoTradeTech[eIndex] = bNewValue;
	}
}

/// Accessor: does team have a tech?
bool CvTeamTechs::IsNoTradeTech(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabNoTradeTech[eIndex];
}

/// Accessor: increment count of times this has been researched
void CvTeamTechs::IncrementTechCount(TechTypes eIndex)
{
	m_paiTechCount[eIndex]++;
}

/// Accessor: get count of times this has been researched
int CvTeamTechs::GetTechCount(TechTypes eIndex)const
{
	return m_paiTechCount[eIndex];
}

/// Accessor: set research done on one tech
void CvTeamTechs::SetResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
{
	SetResearchProgressTimes100(eIndex, iNewValue * 100, ePlayer);
}

/// Accessor: set research done on one tech (in hundredths)
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
void CvTeamTechs::SetResearchProgressTimes100(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer, int iPlayerOverflow, int iPlayerOverflowDivisorTimes100)
#else
void CvTeamTechs::SetResearchProgressTimes100(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
#endif
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
#if defined(MOD_BALANCE_CORE)
	//Crash failsafe.
	if(ePlayer == NO_PLAYER || eIndex == -1)
	{
		return;
	}
#endif
	if(GetResearchProgressTimes100(eIndex) != iNewValue)
	{
		m_paiResearchProgress[eIndex] = max(0,iNewValue);
		CvAssert(GetResearchProgressTimes100(eIndex) >= 0);

		if(m_pTeam->GetID() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
		}
		int iResearchProgress = GetResearchProgressTimes100(eIndex);
		int iResearchCost = GetResearchCost(eIndex) * 100;

		// Player modifiers to cost
		int iResearchMod = std::max(1, GET_PLAYER(ePlayer).calculateResearchModifier(eIndex));
		iResearchCost = (iResearchCost * 100) / iResearchMod;
		int iNumCitiesMod = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();	// Default is 40, gets smaller on larger maps
		iNumCitiesMod += GET_PLAYER(ePlayer).GetTechCostXCitiesModifier();
#if defined(MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
		if (MOD_BALANCE_CORE_PURCHASE_COST_INCREASE)
		{
			iNumCitiesMod = iNumCitiesMod * GET_PLAYER(ePlayer).GetMaxEffectiveCities(/*bIncludePuppets*/ false);
		}
		else
		{
			iNumCitiesMod = iNumCitiesMod * GET_PLAYER(ePlayer).GetMaxEffectiveCities(/*bIncludePuppets*/ true);
		}
#else
		iNumCitiesMod = iNumCitiesMod * GET_PLAYER(ePlayer).GetMaxEffectiveCities(/*bIncludePuppets*/ true);
#endif
		iResearchCost = iResearchCost * (100 + iNumCitiesMod) / 100;
		
		int iOverflow = iResearchProgress - iResearchCost;
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
		if (!MOD_BUGFIX_RESEARCH_OVERFLOW) {
#endif
		// April 2014 Balance Patch change - EFB
		//    Don't allow the overflow to get out of hand
		int iMaxOverflow = GetMaxResearchOverflow(eIndex, ePlayer);
		if (iOverflow > iMaxOverflow)
		{
			iOverflow = iMaxOverflow;
		}
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
		}
#endif
		if(iOverflow >= 0)
		{
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
			if (MOD_BUGFIX_RESEARCH_OVERFLOW) {
				// iNewValue = iPlayerBeakersThisTurn + ((iPlayerOverflow * iPlayerOverflowDivisorTimes100) / 100)
				if (iOverflow > iPlayerOverflow) {
					// If we completed the tech using only iBeakersThisTurn, we need to hand back the remaining iPlayerBeakersThisTurn and the scaled down iPlayerOverflow
					iOverflow = (iOverflow - iPlayerOverflow) + (iPlayerOverflow * 100 / iPlayerOverflowDivisorTimes100); 
				} else {
					// Otherwise we used all of iBeakersThisTurn and some of iPlayerOverflow, so we need to hand back the scaled down iOverflow
					iOverflow = iOverflow * 100 / iPlayerOverflowDivisorTimes100;
				}
			}
#endif
			GET_PLAYER(ePlayer).changeOverflowResearchTimes100(iOverflow);
			m_pTeam->setHasTech(eIndex, true, ePlayer, true, true);
			SetNoTradeTech(eIndex, true);

			// Mark city specialization dirty
			GET_PLAYER(ePlayer).GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_RESEARCH_COMPLETE);

			// Culture bonus for Player researching a Tech
			PlayerTypes eLoopPlayer;
			int iCulture;
			TeamTypes eTeamID = m_pTeam->GetID();
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);

				if(kLoopPlayer.getTeam() == eTeamID)
				{
					iCulture = kLoopPlayer.GetCulturePerTechResearched();
					if(iCulture != 0)
					{
						kLoopPlayer.changeJONSCulture(iCulture);
					}
				}
			}
		}
	}
}

/// Accessor: get research done on one tech
int CvTeamTechs::GetResearchProgress(TechTypes eIndex) const
{
#if defined(MOD_BUGFIX_MINOR)
	return GetResearchProgressTimes100(eIndex) / 100;
#else
	if(eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex] / 100;
	}
	else
	{
		return 0;
	}
#endif
}

/// Accessor: get research done on one tech (in hundredths)
int CvTeamTechs::GetResearchProgressTimes100(TechTypes eIndex) const
{
	if(eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex];
	}
	else
	{
		return 0;
	}
}

/// Accessor: what is the cost of researching this tech (taking all modifiers into account)
int CvTeamTechs::GetResearchCost(TechTypes eTech) const
{
	CvAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return 0;
	}

	//to avoid overflow, we have to work a bit differently here
	int iCost = pkTechInfo->GetResearchCost();
	int iModifier = 0;

	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(m_pTeam->getHandicapType());
	if(pkHandicapInfo)
		iModifier += (pkHandicapInfo->getResearchPercent() - 100);

	iModifier += (GC.getMap().getWorldInfo().getResearchPercent() - 100);
	iModifier += (GC.getGame().getGameSpeedInfo().getResearchPercent() - 100);
	iModifier += (GC.getGame().getStartEraInfo().getResearchPercent() - 100);
	iModifier += std::max(0, GC.getTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER() * (m_pTeam->getNumMembers() - 1));

#if defined(MOD_CIV6_EUREKA)
	iModifier += (std::max(0, (1000000 - (pkTechInfo->GetEurekaPerMillion() * m_paiEurekaCounter[eTech]) / max(1, m_pTeam->getNumMembers())) / 10000) - 100);
#endif

	if (iCost<10000)
		//avoid rounding errors
		return std::max(1, iCost + (iCost*iModifier)/100);
	else
		//avoid overflow
	return std::max(1, iCost + (iCost/100)*iModifier);
}

#if defined(MOD_CIV6_EUREKA)
int CvTeamTechs::GetEurekaDiscount(TechTypes eTech) const
{
	CvAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if (pkTechInfo == NULL)
	{
		return 0;
	}
	return std::max(0, (1000000 - pkTechInfo->GetEurekaPerMillion() * m_paiEurekaCounter[eTech]) / 10000);
}
#endif

/// Accessor: how many beakers of research to go for this tech?
int CvTeamTechs::GetResearchLeft(TechTypes eTech) const
{
	return std::max(0, (GetResearchCost(eTech) - GetResearchProgress(eTech)));
}

/// Return the tech data (from XML)
CvTechXMLEntries* CvTeamTechs::GetTechs() const
{
	return m_pTechs;
}

/// Add an increment of research to a tech
void CvTeamTechs::ChangeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
{
	ChangeResearchProgressTimes100(eIndex, iChange * 100, ePlayer);
}

/// Add an increment of research to a tech (in hundredths)
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
void CvTeamTechs::ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer, int iPlayerOverflow, int iPlayerOverflowDivisorTimes100)
#else
void CvTeamTechs::ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
#endif
{
#if defined(MOD_BUGFIX_RESEARCH_OVERFLOW)
	SetResearchProgressTimes100(eIndex, (GetResearchProgressTimes100(eIndex) + iChange), ePlayer, iPlayerOverflow, iPlayerOverflowDivisorTimes100);
#else
	SetResearchProgressTimes100(eIndex, (GetResearchProgressTimes100(eIndex) + iChange), ePlayer);
#endif
}

/// Add research for a tech to a specified percent complete
int CvTeamTechs::ChangeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer)
{
	int iBeakers = 0;

	if(0 != iPercent && !HasTech(eIndex))
	{
		if(iPercent > 0)
		{
			iBeakers = std::min(GetResearchLeft(eIndex), (GetResearchCost(eIndex) * iPercent) / 100);
		}
		else
		{
			iBeakers = std::max(GetResearchLeft(eIndex) - GetResearchCost(eIndex), (GetResearchCost(eIndex) * iPercent) / 100);
		}

		ChangeResearchProgress(eIndex, iBeakers, ePlayer);
	}

	return iBeakers;
}

// PRIVATE FUNCTIONS

int CvTeamTechs::GetMaxResearchOverflow(TechTypes eTech, PlayerTypes ePlayer) const
{
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);

	// 5 turns of science is a reasonable allowance of overflow (about equal to a standard research agreement award)
	int iReturnValue = kPlayer.GetScienceTimes100() * 5;   

	// Alternatively let it be the raw cost of the tech (times 100)
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return 0;
	}

	int iCost = pkTechInfo->GetResearchCost() * 100;

	iReturnValue = max(iCost, iReturnValue);

	return iReturnValue;
}


#if defined(MOD_CIV6_EUREKA)
int CvTeamTechs::GetEurekaCounter(TechTypes eTech) const
{
	if (eTech != NO_TECH)
	{
		return m_paiEurekaCounter[eTech];
	}
	else
	{
		return 0;
	}
}
void CvTeamTechs::SetEurekaCounter(TechTypes eTech, int newEurakaCount)
{
	if (eTech != NO_TECH)
	{
		m_paiEurekaCounter[eTech] = newEurakaCount;
	}
}
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
/// Can you permit vassalage to be traded?
bool CvTechEntry::IsVassalageTradingAllowed() const
{
	return m_bVassalageTradingAllowed;
}
#endif

bool CvTechEntry::IsUnlocksEspionageAdvancedActions() const
{
	return m_bUnlocksEspionageAdvancedActions;
}
