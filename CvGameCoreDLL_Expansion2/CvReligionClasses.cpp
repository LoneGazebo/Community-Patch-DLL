/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvMilitaryAI.h"
#include "cvStopWatch.h"

#include "LintFree.h"
 
//======================================================================================================
//					CvReligionEntry
//======================================================================================================
/// Constructor
CvReligionEntry::CvReligionEntry()
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	: m_iLocalReligion(0)
#endif
{
}

/// Destructor
CvReligionEntry::~CvReligionEntry()
{
}

/// Load XML data
bool CvReligionEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_strIconString = kResults.GetText("IconString");
	 
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	if (MOD_RELIGION_LOCAL_RELIGIONS) {
		m_iLocalReligion = kResults.GetInt("LocalReligion");
	}
#endif

	return true;
}

//------------------------------------------------------------------------------
CvString CvReligionEntry::GetIconString() const
{
	return m_strIconString;
}

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
//------------------------------------------------------------------------------
bool CvReligionEntry::IsLocalReligion() const
{
	return (m_iLocalReligion != 0);
}
#endif

//=====================================
// CvReligionXMLEntries
//=====================================
/// Constructor
CvReligionXMLEntries::CvReligionXMLEntries(void)
{

}

/// Destructor
CvReligionXMLEntries::~CvReligionXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of trait entries
std::vector<CvReligionEntry*>& CvReligionXMLEntries::GetReligionEntries()
{
	return m_paReligionEntries;
}

/// Number of defined traits
int CvReligionXMLEntries::GetNumReligions()
{
	return m_paReligionEntries.size();
}

/// Clear trait entries
void CvReligionXMLEntries::DeleteArray()
{
	for(std::vector<CvReligionEntry*>::iterator it = m_paReligionEntries.begin(); it != m_paReligionEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paReligionEntries.clear();
}

/// Get a specific entry
CvReligionEntry* CvReligionXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_RELIGION) ? m_paReligionEntries[index] : NULL;
#else
	return m_paReligionEntries[index];
#endif
}

//=====================================
// CvReligion
//=====================================
/// Default Constructor
CvReligion::CvReligion()
	: m_eReligion(NO_RELIGION)
	, m_eFounder(NO_PLAYER)
	, m_iHolyCityX(-1)
	, m_iHolyCityY(-1)
	, m_iTurnFounded(-1)
	, m_bPantheon(false)
	, m_bEnhanced(false)
#if defined(MOD_BALANCE_CORE)
	, m_bReformed(false)
#endif
{
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
}

/// Constructor
CvReligion::CvReligion(ReligionTypes eReligion, PlayerTypes eFounder, CvCity* pHolyCity, bool bPantheon)
	: m_eReligion(eReligion)
	, m_eFounder(eFounder)
	, m_bPantheon(bPantheon)
	, m_bEnhanced(false)
#if defined(MOD_BALANCE_CORE)
	, m_bReformed(false)
#endif
{
	if(pHolyCity)
	{
		m_iHolyCityX = pHolyCity->getX();
		m_iHolyCityY = pHolyCity->getY();
	}
	m_iTurnFounded = GC.getGame().getGameTurn();
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvReligion& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_eReligion;
	loadFrom >> writeTo.m_eFounder;
	loadFrom >> writeTo.m_iHolyCityX;
	loadFrom >> writeTo.m_iHolyCityY;
	loadFrom >> writeTo.m_iTurnFounded;

	if(uiVersion >= 2)
	{
		loadFrom >> writeTo.m_bPantheon;
	}
	else
	{
		writeTo.m_bPantheon = false;
	}

	if(uiVersion >= 4)
	{
		loadFrom >> writeTo.m_bEnhanced;
	}
	else
	{
		writeTo.m_bEnhanced = false;
	}

	ZeroMemory(writeTo.m_szCustomName, sizeof(writeTo.m_szCustomName));
	if(uiVersion >= 3)
	{
		loadFrom >> writeTo.m_szCustomName;
	}
#if defined(MOD_BALANCE_CORE)
	loadFrom >> writeTo.m_bReformed;
#endif

	writeTo.m_Beliefs.Read(loadFrom);

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvReligion& readFrom)
{
	uint uiVersion = 4;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eReligion;
	saveTo << readFrom.m_eFounder;
	saveTo << readFrom.m_iHolyCityX;
	saveTo << readFrom.m_iHolyCityY;
	saveTo << readFrom.m_iTurnFounded;
	saveTo << readFrom.m_bPantheon;
	saveTo << readFrom.m_bEnhanced;
	saveTo << readFrom.m_szCustomName;
#if defined(MOD_BALANCE_CORE)
	saveTo << readFrom.m_bReformed;
#endif

	readFrom.m_Beliefs.Write(saveTo);

	return saveTo;
}

CvString CvReligion::GetName() const
{
	CvReligionEntry* pEntry = GC.getReligionInfo(m_eReligion);
	CvAssertMsg(pEntry, "pEntry for religion not expected to be NULL. Please send Anton or Ed your save file and version.");
	if (pEntry)
	{
		CvString szReligionName = (m_szCustomName == NULL || strlen(m_szCustomName) == 0) ? pEntry->GetDescriptionKey() : m_szCustomName;
		return szReligionName;
	}

	const char* szReligionNameBackup = "No Religion";
	return szReligionNameBackup;
}

//=====================================
// CvReligionInCity
//=====================================
/// Default Constructor
CvReligionInCity::CvReligionInCity()
{
	m_eReligion = NO_RELIGION;
	m_bFoundedHere = false;
	m_iFollowers = 0;
	m_iPressure = 0;
	m_iNumTradeRoutesApplyingPressure = 0;
	m_iTemp = 0;
}

/// Constructor
CvReligionInCity::CvReligionInCity(ReligionTypes eReligion, bool bFoundedHere, int iFollowers, int iPressure):
	m_eReligion(eReligion),
	m_bFoundedHere(bFoundedHere),
	m_iFollowers(iFollowers),
	m_iPressure(iPressure),
	m_iNumTradeRoutesApplyingPressure(0),
	m_iTemp(0)
{
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvReligionInCity& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_eReligion;
	loadFrom >> writeTo.m_bFoundedHere;
	loadFrom >> writeTo.m_iFollowers;
	loadFrom >> writeTo.m_iPressure;

	if (uiVersion == 2)
	{
		loadFrom >> writeTo.m_iNumTradeRoutesApplyingPressure;
	}
	else
	{
		writeTo.m_iNumTradeRoutesApplyingPressure = 0;
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvReligionInCity& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eReligion;
	saveTo << readFrom.m_bFoundedHere;
	saveTo << readFrom.m_iFollowers;
	saveTo << readFrom.m_iPressure;
	saveTo << readFrom.m_iNumTradeRoutesApplyingPressure;

	return saveTo;
}

//=====================================
// CvGameReligions
//=====================================
/// Constructor
CvGameReligions::CvGameReligions(void):
	m_iMinimumFaithForNextPantheon(0)
{
}

/// Destructor
CvGameReligions::~CvGameReligions(void)
{

}

/// Initialize class data
void CvGameReligions::Init()
{
	m_iMinimumFaithForNextPantheon = GC.getRELIGION_MIN_FAITH_FIRST_PANTHEON();
	m_iMinimumFaithForNextPantheon *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	m_iMinimumFaithForNextPantheon /= 100;
}

/// Handle turn-by-turn religious updates
void CvGameReligions::DoTurn()
{
	SpreadReligion();
}

/// Spread religious pressure into adjacent cities
void CvGameReligions::SpreadReligion()
{
	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && !kPlayer.isBarbarian())
		{
			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				SpreadReligionToOneCity(pLoopCity);
			}
		}
	}
}

/// Spread religious pressure to one city
void CvGameReligions::SpreadReligionToOneCity(CvCity* pCity)
{
	// Used to calculate how many trade routes are applying pressure to this city. This resets the value so we get a true count every turn.
	pCity->GetCityReligions()->ResetNumTradeRoutePressure();

	// Is this a city where a religion was founded?
	if(pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		pCity->GetCityReligions()->AddHolyCityPressure();
	}

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
			if (iSpyPressure > 0)
			{
				if (kPlayer.GetEspionage()->GetSpyIndexInCity(pCity) != -1)
				{
					ReligionTypes eReligionFounded = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
					if(eReligionFounded == NO_RELIGION)
					{
						eReligionFounded = kPlayer.GetReligions()->GetReligionInMostCities();
					}
					if(eReligionFounded != NO_RELIGION && eReligionFounded > RELIGION_PANTHEON)
					{
						pCity->GetCityReligions()->AddSpyPressure(eReligionFounded, iSpyPressure);
					}
				}
			}

			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Ignore the same city
				if(pCity == pLoopCity)
				{
					continue;
				}

				// Does this city have a majority religion?
				ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
				if(eMajorityReligion == NO_RELIGION)
				{
					continue;
				}

				int iNumTradeRoutes = 0;
				int iPressure = GetAdjacentCityReligiousPressure (eMajorityReligion, pLoopCity, pCity, iNumTradeRoutes, true, false);
				if (iPressure > 0)
				{
					pCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_ADJACENT_PRESSURE, eMajorityReligion, iPressure);
					if (iNumTradeRoutes != 0)
					{
						pCity->GetCityReligions()->IncrementNumTradeRouteConnections(eMajorityReligion, iNumTradeRoutes);
					}
				}
			}
		}
	}
}

/// Religious activities at the start of a player's turn
void CvGameReligions::DoPlayerTurn(CvPlayer& kPlayer)
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvGameReligions::DoPlayerTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), kPlayer.getCivilizationShortDescription()) );
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	int iFaithAtStart = kPlayer.GetFaith();
#else
	bool bCouldAtStartAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase();
#endif
	const PlayerTypes ePlayer = kPlayer.GetID();

	int iFaithPerTurn = kPlayer.GetTotalFaithPerTurn();
	if(iFaithPerTurn > 0)
	{
		kPlayer.ChangeFaith(iFaithPerTurn);
	}

	// If just now can afford missionary, add a notification
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	bool bSendFaithPurchaseNotification = (kPlayer.isHuman() && kPlayer.GetFaithPurchaseType() == NO_AUTOMATIC_FAITH_PURCHASE);

	if (bSendFaithPurchaseNotification) {
		if (MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY) {
			bSendFaithPurchaseNotification = kPlayer.GetReligions()->CanAffordNextPurchase();
		} else {
			bool bCouldAtStartAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(iFaithAtStart);
			bool bCanNowAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(kPlayer.GetFaith());

			bSendFaithPurchaseNotification = !bCouldAtStartAffordFaithPurchase && bCanNowAffordFaithPurchase;
		}
	}

	if (bSendFaithPurchaseNotification)
#else
	bool bCanNowAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase();
	if (kPlayer.GetFaithPurchaseType() == NO_AUTOMATIC_FAITH_PURCHASE && !bCouldAtStartAffordFaithPurchase && bCanNowAffordFaithPurchase)
#endif
	{
		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_MISSIONARY");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_MISSIONARY");
			pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);

#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
			kPlayer.GetReligions()->SetFaithAtLastNotify(kPlayer.GetFaith());
#endif
		}
	}

	// Check for pantheon or great prophet spawning (now restricted so must occur before Industrial era)
#if defined(MOD_CONFIG_GAME_IN_XML)
	if(kPlayer.GetFaith() > 0 && !kPlayer.isMinorCiv() && kPlayer.GetCurrentEra() <= GD_INT_GET(RELIGION_LAST_FOUND_ERA))
#else
	if(kPlayer.GetFaith() > 0 && !kPlayer.isMinorCiv() && kPlayer.GetCurrentEra() < GC.getInfoTypeForString("ERA_INDUSTRIAL"))
#endif
	{
		if(CanCreatePantheon(kPlayer.GetID(), true) == FOUNDING_OK)
		{
			// Create the pantheon
			if(kPlayer.isHuman())
			{
				//If the player is human then a net message will be received which will pick the pantheon.
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_PANTHEON");

					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_PANTHEON");
					pNotifications->Add(NOTIFICATION_FOUND_PANTHEON, strBuffer, strSummary, -1, -1, -1);
				}
			}
			else
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				const BeliefTypes eBelief = kPlayer.GetReligionAI()->ChoosePantheonBelief(ePlayer);
#else
				const BeliefTypes eBelief = kPlayer.GetReligionAI()->ChoosePantheonBelief();
#endif

				FoundPantheon(ePlayer, eBelief);
			}
		}

		switch (kPlayer.GetFaithPurchaseType())
		{
		case NO_AUTOMATIC_FAITH_PURCHASE:
		case FAITH_PURCHASE_SAVE_PROPHET:
				CheckSpawnGreatProphet(kPlayer);
				break;
		}
	}

	// Pick a Reformation belief?
#if defined(MOD_BALANCE_CORE)
	ReligionTypes eReligionCreated = GetFounderBenefitsReligion(ePlayer);
#else
	ReligionTypes eReligionCreated = GetReligionCreatedByPlayer(ePlayer);
#endif
#if defined(MOD_BALANCE_CORE)
	if (eReligionCreated > RELIGION_PANTHEON && !HasAddedReformationBelief(ePlayer) && (kPlayer.GetPlayerPolicies()->HasPolicyGrantingReformationBelief() || kPlayer.IsReformation()))
#else
	if (eReligionCreated > RELIGION_PANTHEON && !HasAddedReformationBelief(ePlayer) && kPlayer.GetPlayerPolicies()->HasPolicyGrantingReformationBelief())
#endif
	{
		if (!kPlayer.isHuman())
		{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
			BeliefTypes eReformationBelief = kPlayer.GetReligionAI()->ChooseReformationBelief(ePlayer, eReligionCreated);
#else
			BeliefTypes eReformationBelief = kPlayer.GetReligionAI()->ChooseReformationBelief();
#endif
			AddReformationBelief(ePlayer, eReligionCreated, eReformationBelief);
		}
		else
		{
			CvNotifications* pNotifications;
			pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ADD_REFORMATION_BELIEF");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ADD_REFORMATION_BELIEF");
				pNotifications->Add(NOTIFICATION_ADD_REFORMATION_BELIEF, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}

	// Automatic faith purchases?
	bool bSelectionStillValid = true;
	CvString szItemName = "";
	ReligionTypes eReligion = kPlayer.GetReligionAI()->GetReligionToSpread();

	switch (kPlayer.GetFaithPurchaseType())
	{
	case FAITH_PURCHASE_SAVE_PROPHET:
#if defined(MOD_BALANCE_CORE)
		if (eReligion <= RELIGION_PANTHEON && GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
#else
		if (eReligion <= RELIGION_PANTHEON && GetNumReligionsStillToFound() <= 0)
#endif
		{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			UnitTypes eProphetType = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET_PARAM", GC.getUnitInfo(eProphetType)->GetDescription());
#else
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET");
#endif
			bSelectionStillValid = false;
		}
#if defined(MOD_CONFIG_GAME_IN_XML)
		else if (kPlayer.GetCurrentEra() > GD_INT_GET(RELIGION_LAST_FOUND_ERA))
#else
		else if (kPlayer.GetCurrentEra() >= GC.getInfoTypeForString("ERA_INDUSTRIAL"))
#endif
		{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			UnitTypes eProphetType = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET_PARAM", GC.getUnitInfo(eProphetType)->GetDescription());
#else
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET");
#endif
			bSelectionStillValid = false;
		}
		break;

	case FAITH_PURCHASE_UNIT:
		{
			UnitTypes eUnit = (UnitTypes)kPlayer.GetFaithPurchaseIndex();
			CvUnitEntry *pkUnit = GC.getUnitInfo(eUnit);
			if (pkUnit)
			{
				szItemName = pkUnit->GetDescriptionKey();
			}

			if (!kPlayer.IsCanPurchaseAnyCity(false, false /* Don't worry about faith balance */, eUnit, NO_BUILDING, YIELD_FAITH))
			{
				bSelectionStillValid = false;
			}
			else
			{
				if (kPlayer.IsCanPurchaseAnyCity(true, true /* Check faith balance */, eUnit, NO_BUILDING, YIELD_FAITH))
				{
					CvCity *pCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(kPlayer, eUnit, eReligion);
					if (pCity)
					{
						pCity->Purchase(eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH);

						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if(pNotifications)
						{
							CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_AUTOMATIC_FAITH_PURCHASE", szItemName, pCity->getNameKey());
							CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_AUTOMATIC_FAITH_PURCHASE");
							pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, pCity->getX(), pCity->getY(), -1);
						}
					}
					else
					{
						bSelectionStillValid = false;
					}
				}
			}
		}
		break;
	case FAITH_PURCHASE_BUILDING:
		{
			BuildingTypes eBuilding = (BuildingTypes)kPlayer.GetFaithPurchaseIndex();
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
			if (pkBuilding)
			{
				szItemName = pkBuilding->GetDescriptionKey();
			}

			if (!kPlayer.IsCanPurchaseAnyCity(false, false, NO_UNIT, eBuilding, YIELD_FAITH))
			{
				bSelectionStillValid = false;
			}
			else
			{
				if (kPlayer.IsCanPurchaseAnyCity(true, true /* Check faith balance */, NO_UNIT, eBuilding, YIELD_FAITH))
				{
					CvCity *pCity = CvReligionAIHelpers::GetBestCityFaithBuildingPurchase(kPlayer, eBuilding, eReligion);
					if (pCity)
					{
						pCity->Purchase(NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH);

						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if(pNotifications)
						{
							CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_AUTOMATIC_FAITH_PURCHASE", szItemName, pCity->getNameKey());
							CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_AUTOMATIC_FAITH_PURCHASE");
							pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
						}
					}
					else
					{
						bSelectionStillValid = false;
					}
				}
			}
		}
		break;
	}

	if (!bSelectionStillValid)
	{
		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_NEED_NEW_AUTOMATIC_FAITH_SELECTION", szItemName);
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_NEED_NEW_AUTOMATIC_FAITH_SELECTION");
			pNotifications->Add(NOTIFICATION_AUTOMATIC_FAITH_PURCHASE_STOPPED, strBuffer, strSummary, -1, -1, -1);
		}

		gDLL->SendFaithPurchase(kPlayer.GetID(), NO_AUTOMATIC_FAITH_PURCHASE, 0);
	}
}

/// Time to create a pantheon?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanCreatePantheon(PlayerTypes ePlayer, bool bCheckFaithTotal)
{
	if(ePlayer == NO_PLAYER)
		return FOUNDING_INVALID_PLAYER;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const int iFaith = kPlayer.GetFaith();

	if(kPlayer.isMinorCiv())
	{
		return FOUNDING_INVALID_PLAYER;
	}

	if(HasCreatedPantheon(ePlayer) || HasCreatedReligion(ePlayer))
	{
		return FOUNDING_PLAYER_ALREADY_CREATED_PANTHEON;
	}

	if(bCheckFaithTotal && iFaith < GetMinimumFaithNextPantheon())
	{
		return FOUNDING_NOT_ENOUGH_FAITH;
	}

	// Has a religion been enhanced yet (and total number of religions/pantheons is equal to number of religions allowed)?
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_bEnhanced)
		{
			if (GetNumPantheonsCreated() >= GC.getMap().getWorldInfo().getMaxActiveReligions())
			{
				return FOUNDING_RELIGION_ENHANCED;
			}
		}
	}

#if defined(MOD_EVENTS_FOUND_RELIGION)
		if (MOD_EVENTS_FOUND_RELIGION) {
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanFoundPantheon, ePlayer) == GAMEEVENTRETURN_FALSE) {
				return FOUNDING_INVALID_PLAYER;
			}
		} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if(pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(ePlayer);

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanFoundPantheon", args.get(), bResult))
				{
					if (bResult == false) 
					{
						return FOUNDING_INVALID_PLAYER;
					}
				}
			}
#if defined(MOD_EVENTS_FOUND_RELIGION)
		}
#endif

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	if (GetAvailablePantheonBeliefs(ePlayer).size() == 0)
#else
	if (GetAvailablePantheonBeliefs().size() == 0)
#endif
		return FOUNDING_NO_BELIEFS_AVAILABLE;

	return FOUNDING_OK;
}

/// Get the appropriate religion for this player to found next
ReligionTypes CvGameReligions::GetReligionToFound(PlayerTypes ePlayer)
{
#if defined(MOD_RELIGION_NO_PREFERRENCES)
	if (!MOD_RELIGION_NO_PREFERRENCES) {
		// Choose the civs preferred religion if not disabled and available
#endif
		ReligionTypes eCivReligion;
		eCivReligion = GET_PLAYER(ePlayer).getCivilizationInfo().GetReligion();
		
#if defined(MOD_EVENTS_FOUND_RELIGION)
		if (MOD_EVENTS_FOUND_RELIGION) {
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetReligionToFound, ePlayer, eCivReligion, HasBeenFounded(eCivReligion)) == GAMEEVENTRETURN_VALUE) {
				// Defend against modder stupidity!
				if (iValue > RELIGION_PANTHEON && iValue < GC.getNumReligionInfos()) {
					// CUSTOMLOG("GetReligionToFound: Before=%i, After=%i", eCivReligion, iValue);
					eCivReligion = (ReligionTypes)iValue;
				}
			}
		} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(ePlayer);
		args->Push(eCivReligion);
		args->Push(HasBeenFounded(eCivReligion));

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetReligionToFound", args.get(), iValue)) 
		{
			if (iValue >= 0 && iValue < GC.getNumReligionInfos() && iValue != RELIGION_PANTHEON)
			{
				eCivReligion = (ReligionTypes)iValue;
			}
		}
	}
#if defined(MOD_EVENTS_FOUND_RELIGION)
		}
#endif

		if(!HasBeenFounded(eCivReligion))
		{
			CvReligionEntry* pEntry = GC.getReligionInfo(eCivReligion);
			if(pEntry)
			{
				// CUSTOMLOG("GetReligionToFound: Using preferred %i", eCivReligion);
				return eCivReligion;
			}
		}
#if defined(MOD_RELIGION_NO_PREFERRENCES)
	}
#endif

#if defined(MOD_RELIGION_RANDOMISE)
	// No preferred religion, so find all the possible religions
	std::vector<ReligionTypes> availableReligions;
#endif
	
	// Need to "borrow" from another civ.  Loop through all religions looking for one that is eligible
	for(int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		ReligionTypes eReligion = (ReligionTypes)iI;
		CvReligionEntry* pEntry = GC.getReligionInfo(eReligion);
		if(!pEntry)
		{
			continue;
		}

		if(pEntry->GetID() == RELIGION_PANTHEON)
		{
			continue;
		}
		
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
		if (MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion()) {
			continue;
		}
#endif

		if(HasBeenFounded((ReligionTypes)pEntry->GetID()))
		{
			continue;
		}

#if defined(MOD_RELIGION_NO_PREFERRENCES)
		// Only excluded religions preferred by other civs if not disabled
		if (!MOD_RELIGION_NO_PREFERRENCES) {
#endif
			if(IsPreferredByCivInGame(eReligion))
			{
				continue;
			}
#if defined(MOD_RELIGION_NO_PREFERRENCES)
		}
#endif

#if defined(MOD_RELIGION_RANDOMISE)
		if (MOD_RELIGION_RANDOMISE) {
			// If we want a random religion, remember this as a possible candidate ...
			availableReligions.push_back(eReligion);
		} else {
			// ... otherwise just return it
#endif
			// CUSTOMLOG("GetReligionToFound: Using spare %i", eReligion);
			return (eReligion);
#if defined(MOD_RELIGION_RANDOMISE)
		}
#endif
	}

#if defined(MOD_RELIGION_RANDOMISE)
	if (availableReligions.empty()) {
#endif

		// Will have to use a religion that someone else prefers
		for(int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			ReligionTypes eReligion = (ReligionTypes)iI;
			CvReligionEntry* pEntry = GC.getReligionInfo(eReligion);
			if(!pEntry)
			{
				continue;
			}

			if(pEntry->GetID() == RELIGION_PANTHEON)
			{
				continue;
			}

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
			if (MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion()) {
				continue;
			}
#endif

			if(HasBeenFounded((ReligionTypes)pEntry->GetID()))
			{
				continue;
			}

#if defined(MOD_RELIGION_RANDOMISE)
			if (MOD_RELIGION_RANDOMISE) {
				// If we want a random religion, remember this as a possible candidate ...
				availableReligions.push_back(eReligion);
			} else {
				// ... otherwise just return it
#endif
			// CUSTOMLOG("GetReligionToFound: Using borrowed %i", eReligion);
			return (eReligion);
#if defined(MOD_RELIGION_RANDOMISE)
			}
#endif
		}

#if defined(MOD_RELIGION_RANDOMISE)
	}
#endif

#if defined(MOD_RELIGION_RANDOMISE)
	// Pick a random religion
	if (!availableReligions.empty()) {
		int index = 0;
		
		// Pick a random one if required
		if (MOD_RELIGION_RANDOMISE) {
			index = GC.getGame().getJonRandNum(availableReligions.size(), "Random Religion To Found");
		}
		
		// CUSTOMLOG("GetReligionToFound: Using random %i", availableReligions[index]);
		return availableReligions[index];
	}
#endif

	// CUSTOMLOG("GetReligionToFound: Using NO_RELIGION");
	return NO_RELIGION;
}

/// Tell the game a new pantheon has been created
void CvGameReligions::FoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief)
{
	CvGame& kGame = GC.getGame();
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion newReligion(RELIGION_PANTHEON, ePlayer, NULL, true);
	newReligion.m_Beliefs.AddBelief(eBelief);

	// Found it
	m_CurrentReligions.push_back(newReligion);
#if defined(MOD_TRAITS_OTHER_PREREQS)
	if (MOD_TRAITS_OTHER_PREREQS) {
		kPlayer.GetPlayerTraits()->InitPlayerTraits();
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman())
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}
#endif
	// Update game systems
	kPlayer.UpdateReligion();
	kPlayer.ChangeFaith(-GetMinimumFaithNextPantheon());

	int iIncrement = GC.getRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON();
	iIncrement *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iIncrement /= 100;
	SetMinimumFaithNextPantheon(GetMinimumFaithNextPantheon() + iIncrement);

#if defined(MOD_EVENTS_FOUND_RELIGION)
	if (MOD_EVENTS_FOUND_RELIGION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PantheonFounded, ePlayer, GET_PLAYER(ePlayer).getCapitalCity()->GetID(), RELIGION_PANTHEON, eBelief);
	} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
#if defined(MOD_BALANCE_CORE)
		//Bugfix?
		if(pkScriptSystem && ePlayer != NO_PLAYER && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian()) 
#else
		if (pkScriptSystem)
#endif
		{
			CvLuaArgsHandle args;
			args->Push(ePlayer);
#if defined(MOD_BALANCE_CORE)
			CvCity* pCapital = kPlayer.getCapitalCity();
			if (!pCapital)
			{
				//just take the first city
				int iIdx;
				pCapital = kPlayer.firstCity(&iIdx);
			}
			args->Push(pCapital ? pCapital->GetID() : 0);
#else
			args->Push(GET_PLAYER(ePlayer).getCapitalCity()->GetID());
#endif
			args->Push(RELIGION_PANTHEON);
			args->Push(eBelief);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "PantheonFounded", args.get(), bResult);
		}
#if defined(MOD_EVENTS_FOUND_RELIGION)
	}
#endif

	// Spread the pantheon into each of their cities
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		// Add enough pressure to make this the likely majority religion
		pLoopCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_PANTHEON_FOUNDED, newReligion.m_eReligion, GC.getRELIGION_ATHEISM_PRESSURE_PER_POP() * pLoopCity->getPopulation() * 2);
	}

	// Send out messaging
	CvReligionEntry* pEntry = GC.getReligionInfo(newReligion.m_eReligion);
	CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief);
	if(pEntry && pBelief)
	{
		//Add replay message.
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_S");
		Localization::String replayText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED");
		replayText << kPlayer.getCivilizationShortDescriptionKey() << pBelief->getShortDescription() << pBelief->GetDescriptionKey();

		kGame.addReplayMessage(REPLAY_MESSAGE_PANTHEON_FOUNDED, newReligion.m_eFounder, replayText.toUTF8());

		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications){
				// Message slightly different for founder player
				if(newReligion.m_eFounder == eNotifyPlayer)
				{
					Localization::String localizedText = GetLocalizedText("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER").c_str();
					localizedText << pBelief->getShortDescription() << pBelief->GetDescriptionKey();

#if defined(MOD_API_EXTENSIONS)
					pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
#else
					pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
				}
				else
				{
					//If the notify player has not met this civ yet, display a more ambiguous notification.
					CvTeam& kTeam = GET_TEAM(kCurNotifyPlayer.getTeam());
					if(kTeam.isHasMet(kPlayer.getTeam()))
					{
#if defined(MOD_API_EXTENSIONS)
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
#else
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
					}
					else
					{
						Localization::String unknownFoundedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_UNKNOWN");
						unknownFoundedText << pBelief->getShortDescription() << pBelief->GetDescriptionKey();

#if defined(MOD_API_EXTENSIONS)
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, unknownFoundedText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
#else
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, unknownFoundedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
					}
				}
			}
		}

		// Logging
		if(GC.getLogging())
		{
			CvString strLogMsg;
			strLogMsg = kPlayer.getCivilizationShortDescription();
			strLogMsg += ", PANTHEON FOUNDED, ";
			strLogMsg += pEntry->GetDescription();
			LogReligionMessage(strLogMsg);
		}

#if !defined(NO_ACHIEVEMENTS)
		//Achievements!
		if(ePlayer == GC.getGame().getActivePlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_10);
		}
#endif
	}

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Create a new religion
void CvGameReligions::FoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion kReligion(eReligion, ePlayer, pkHolyCity, false);

	// Copy over belief from your pantheon
	BeliefTypes eBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(kPlayer.GetID());
	if(eBelief != NO_BELIEF)
#if defined(MOD_API_RELIGION)
	{
		CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, ePlayer)->m_Beliefs;
		for (int iI = 0; iI < beliefs.GetNumBeliefs(); iI++) {
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI));
		}
	}
#else
		kReligion.m_Beliefs.AddBelief(eBelief);
#endif
#if defined(MOD_BALANCE_CORE)
	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman())
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}
	kReligion.m_Beliefs.SetReligion(eReligion);
#endif
	kReligion.m_Beliefs.AddBelief(eBelief1);
	kReligion.m_Beliefs.AddBelief(eBelief2);

	if(eBelief3 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief3);
	}

	if(eBelief4 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief4);
	}

	if(szCustomName != NULL && strlen(szCustomName) <= sizeof(kReligion.m_szCustomName))
	{
		strcpy_s(kReligion.m_szCustomName, szCustomName);
	}

	// Found it
	m_CurrentReligions.push_back(kReligion);

	// Inform the holy city
	pkHolyCity->GetCityReligions()->DoReligionFounded(kReligion.m_eReligion);

#if defined(MOD_TRAITS_OTHER_PREREQS)
	if (MOD_TRAITS_OTHER_PREREQS) {
		kPlayer.GetPlayerTraits()->InitPlayerTraits();
	}
#endif

	// Update game systems
	kPlayer.UpdateReligion();
	kPlayer.GetReligions()->SetFoundingReligion(false);

	// In case we have another prophet sitting around, make sure he's set to this religion
	int iLoopUnit;
	CvUnit* pLoopUnit;
	for(pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
	{
		if(pLoopUnit->getUnitInfo().IsFoundReligion())
		{
			pLoopUnit->GetReligionData()->SetReligion(eReligion);
			pLoopUnit->GetReligionData()->SetSpreadsLeft(pLoopUnit->getUnitInfo().GetReligionSpreads());
			pLoopUnit->GetReligionData()->SetReligiousStrength(pLoopUnit->getUnitInfo().GetReligiousStrength());
		}
	}

#if defined(MOD_EVENTS_FOUND_RELIGION)
	if (MOD_EVENTS_FOUND_RELIGION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionFounded, ePlayer, pkHolyCity->GetID(), eReligion, eBelief, eBelief1, eBelief2, eBelief3, eBelief4);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(ePlayer);
		args->Push(pkHolyCity->GetID());
		args->Push(eReligion);
		args->Push(eBelief);
		args->Push(eBelief1);
		args->Push(eBelief2);
		args->Push(eBelief3);
		args->Push(eBelief4);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "ReligionFounded", args.get(), bResult);
	}
#if defined(MOD_EVENTS_FOUND_RELIGION)
	}
#endif

	// Send out messaging
	CvReligionEntry* pEntry = GC.getReligionInfo(kReligion.m_eReligion);
	if(pEntry)
	{
		//Add replay message
		CvString szReligionName = kReligion.GetName();
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_S");
		Localization::String replayText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED");
		replayText << kPlayer.getCivilizationShortDescriptionKey() << szReligionName << pkHolyCity->getNameKey();

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_RELIGION_FOUNDED, kReligion.m_eFounder, replayText.toUTF8(), kReligion.m_iHolyCityX, kReligion.m_iHolyCityY);

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
		// Local religions are intended to be "super-pantheons" so typically are founded immediately a pantheon is founded
		// As founding the pantheon sent a notification, don't send another here (if the modded wants one, they can always send it manually)
		if (!(MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion())) {
#endif
			//Notify the masses
			for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
				PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
				CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
				CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
				if(pNotifications){
					// Message slightly different for founder player
					if(kReligion.m_eFounder == eNotifyPlayer)
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER");
						localizedText << szReligionName << pkHolyCity->getNameKey();

#if defined(MOD_API_EXTENSIONS)
						pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
						pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
					}
					else
					{
						CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());

						if(kNotifyTeam.isHasMet(kPlayer.getTeam()))
						{
#if defined(MOD_API_EXTENSIONS)
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
						}
						else
						{
							Localization::String unknownCivText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_UNKNOWN");
							unknownCivText << szReligionName;

#if defined(MOD_API_EXTENSIONS)
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, unknownCivText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, unknownCivText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
						}
					}
				}
			}
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
		}
#endif

		// Logging
		if(GC.getLogging())
		{
			CvString strLogMsg;
			strLogMsg = kPlayer.getCivilizationShortDescription();
			strLogMsg += ", RELIGION FOUNDED, ";
			strLogMsg += pkHolyCity->getName();
			strLogMsg += ", ";
			strLogMsg += pEntry->GetDescription();
			LogReligionMessage(strLogMsg);
		}

#if !defined(NO_ACHIEVEMENTS)
		//Achievements!
		if(ePlayer == GC.getGame().getActivePlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_11);
		}
#endif
	}
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Can the supplied religion be created?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity)
{
#if defined(MOD_BALANCE_CORE)
	if(GetNumReligionsStillToFound() <= 0 && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
#else
	if(GetNumReligionsStillToFound() <= 0)
#endif
		return FOUNDING_NO_RELIGIONS_AVAILABLE;

	if(ePlayer == NO_PLAYER)
		return FOUNDING_INVALID_PLAYER;

	if(HasCreatedReligion(ePlayer))
		return FOUNDING_PLAYER_ALREADY_CREATED_RELIGION;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion kReligion(eReligion, ePlayer, pkHolyCity, false);

	// Copy over belief from your pantheon
#if defined(MOD_API_RELIGION)
	if (HasCreatedPantheon(ePlayer)) {
		CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, kPlayer.GetID())->m_Beliefs;
		for(int iI = 0; iI < beliefs.GetNumBeliefs(); iI++) {
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI));
		}
	}
#else
	BeliefTypes eBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(kPlayer.GetID());
	if(eBelief != NO_BELIEF)
		kReligion.m_Beliefs.AddBelief(eBelief);
#endif

	kReligion.m_Beliefs.AddBelief(eBelief1);
	kReligion.m_Beliefs.AddBelief(eBelief2);

	if(eBelief3 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief3);
	}

	if(eBelief4 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief4);
	}

	if(szCustomName != NULL && strlen(szCustomName) <= sizeof(kReligion.m_szCustomName))
	{
		strcpy_s(kReligion.m_szCustomName, szCustomName);
	}

	// Now see if there are any conflicts.
	for(ReligionList::const_iterator it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if ((*it).m_eFounder != ePlayer)	// Only check other player's religions
		{
			if(kReligion.m_eReligion == (*it).m_eReligion)
				return FOUNDING_RELIGION_IN_USE;

			for(int iSrcBelief = (*it).m_Beliefs.GetNumBeliefs(); iSrcBelief--;)
			{
				BeliefTypes eSrcBelief = (*it).m_Beliefs.GetBelief(iSrcBelief);
				if(eSrcBelief != NO_BELIEF)
				{
					for(int iDestBelief = kReligion.m_Beliefs.GetNumBeliefs(); iDestBelief--;)
					{
						BeliefTypes eDestBelief = kReligion.m_Beliefs.GetBelief(iDestBelief);
#if defined(MOD_BALANCE_CORE)
						if(eDestBelief != NO_BELIEF && eDestBelief == eSrcBelief)
						{
							CvBeliefEntry* pBelief = GC.getBeliefInfo(eDestBelief);
							if(pBelief && pBelief->IsFounderBelief() && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
							{
								return FOUNDING_BELIEF_IN_USE;
							}
							if(pBelief && pBelief->IsFollowerBelief() && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
							{
								return FOUNDING_BELIEF_IN_USE;
							}
						}
#else
						if(eDestBelief != NO_BELIEF && eDestBelief == eSrcBelief)
							return FOUNDING_BELIEF_IN_USE;
#endif
					}
				}
			}
		}
	}

	return FOUNDING_OK;
}

/// Add new beliefs to an existing religion
#if defined(MOD_API_RELIGION)
void CvGameReligions::EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2, bool bNotify)
#else
void CvGameReligions::EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2)
#endif
{
	bool bFoundIt = false;
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
#if defined(MOD_API_RELIGION)
		// We use the same code for enhancing a pantheon, so make sure we find the pantheon for the player!
		if(it->m_eReligion == eReligion && it->m_eReligion == RELIGION_PANTHEON)
		{
			if(it->m_eFounder == ePlayer)
			{
				bFoundIt = true;
				break;
			}
		}
		else 
#endif
		if(it->m_eReligion == eReligion)
		{
			bFoundIt = true;
			break;
		}
	}
	if(!bFoundIt)
	{
		CvAssertMsg(false, "Internal error in religion code.");
		CUSTOMLOG("Trying to enhance a religion/pantheon that doesn't exist!!!");
		return;
	}
#if defined(MOD_BALANCE_CORE)
	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman())
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}
#endif
	it->m_Beliefs.AddBelief(eBelief1);
#if defined(MOD_API_RELIGION)
	if(eBelief2 != NO_BELIEF)
#endif
		it->m_Beliefs.AddBelief(eBelief2);

#if defined(MOD_API_RELIGION)
	if(eReligion != RELIGION_PANTHEON)
#endif
		it->m_bEnhanced = true;

#if defined(MOD_TRAITS_OTHER_PREREQS)
	if (MOD_TRAITS_OTHER_PREREQS) {
		kPlayer.GetPlayerTraits()->InitPlayerTraits();
	}
#endif

	// Update game systems
	UpdateAllCitiesThisReligion(eReligion);
	kPlayer.UpdateReligion();

#if defined(MOD_EVENTS_FOUND_RELIGION)
	if (MOD_EVENTS_FOUND_RELIGION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionEnhanced, ePlayer, eReligion, eBelief1, eBelief2);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(ePlayer);
		args->Push(eReligion);
		args->Push(eBelief1);
		args->Push(eBelief2);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "ReligionEnhanced", args.get(), bResult);
	}
#if defined(MOD_EVENTS_FOUND_RELIGION)
	}
#endif

#if defined(MOD_API_RELIGION)
	if (bNotify) {
#endif
	//Notify the masses
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
		if(pNotifications){
#if defined(MOD_API_RELIGION)
			Localization::String strSummary;
			Localization::String notificationText;
			if (eReligion == RELIGION_PANTHEON) {
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_S");
				notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED");
				notificationText << kPlayer.getCivilizationShortDescriptionKey();
			} else {
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_S");
				notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED");
				notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName();
			}
#else
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_S");
			Localization::String notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED");
			notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName();
#endif

			// Message slightly different for enhancing player
			if(ePlayer == eNotifyPlayer)
			{
#if defined(MOD_API_RELIGION)
				Localization::String localizedText;
				if (eReligion == RELIGION_PANTHEON) {
					localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_ACTIVE_PLAYER");
				} else {
					localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER");
					localizedText << it->GetName();
				}
#else
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER");
				localizedText << it->GetName();
#endif

#if defined(MOD_API_EXTENSIONS)
				pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
				pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
			}
			else
			{
				CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());
				if(kNotifyTeam.isHasMet(kPlayer.getTeam()))
				{
#if defined(MOD_API_EXTENSIONS)
					pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, notificationText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
					pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, notificationText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
				}
				else
				{
#if defined(MOD_API_RELIGION)
					Localization::String unknownText;
					if (eReligion == RELIGION_PANTHEON) {
						unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_UNKNOWN");
					} else {
						unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_UNKNOWN");
						unknownText << it->GetName();
					}
#else
					Localization::String unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_UNKNOWN");
					unknownText << it->GetName();
#endif

#if defined(MOD_API_EXTENSIONS)
					pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, unknownText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
					pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, unknownText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
				}
			}
		}

		// Logging
		if(GC.getLogging())
		{
			CvString strLogMsg;
			strLogMsg = kPlayer.getCivilizationShortDescription();
			strLogMsg += ", RELIGION ENHANCED";
			LogReligionMessage(strLogMsg);
		}
	}
#if defined(MOD_API_RELIGION)
	}
#endif

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Can the new beliefs be added to the religion?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2)
{
	bool bFoundIt = false;
#if defined(MOD_BALANCE_CORE)
	if(eReligion > RELIGION_PANTHEON && ePlayer != NO_PLAYER && GetFounderBenefitsReligion(ePlayer) == eReligion)
	{
		bFoundIt = true;
	}
#else
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eReligion == eReligion && it->m_eFounder == ePlayer)
		{
			bFoundIt = true;
			break;
		}
	}
#endif

	if(bFoundIt)
	{
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(eBelief1 != NO_BELIEF && IsInSomeReligion(eBelief1, ePlayer))
		{
			CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief1);
			if(pBelief && (pBelief->IsEnhancerBelief() || pBelief->IsFollowerBelief()) && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
			{
				return FOUNDING_BELIEF_IN_USE;
			}
		}
		if(eBelief2 != NO_BELIEF && IsInSomeReligion(eBelief2, ePlayer))
		{
			CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief2);
			if(pBelief && (pBelief->IsEnhancerBelief() || pBelief->IsFollowerBelief()) && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
			{
				return FOUNDING_BELIEF_IN_USE;
			}
		}
#else
		if(eBelief1 != NO_BELIEF && IsInSomeReligion(eBelief1))
			return FOUNDING_BELIEF_IN_USE;
		if(eBelief2 != NO_BELIEF && IsInSomeReligion(eBelief2))
			return FOUNDING_BELIEF_IN_USE;
#endif

		return FOUNDING_OK;
	}

	return FOUNDING_RELIGION_IN_USE;
}

/// Add an extra belief to a religion (through a policy)
void CvGameReligions::AddReformationBelief(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1)
{
	bool bFoundIt = false;
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			bFoundIt = true;
			break;
		}
	}
	if(!bFoundIt)
	{
		CvAssertMsg(false, "Internal error in religion code.");
		return;
	}
#if defined(MOD_BALANCE_CORE)
	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman())
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}
#endif
	it->m_Beliefs.AddBelief(eBelief1);
#if defined(MOD_TRAITS_OTHER_PREREQS)
	if (MOD_TRAITS_OTHER_PREREQS) {
		kPlayer.GetPlayerTraits()->InitPlayerTraits();
	}
#endif
#if defined(MOD_BALANCE_CORE)
	it->m_bReformed = true;
#endif

	// Update game systems
	UpdateAllCitiesThisReligion(eReligion);
	kPlayer.UpdateReligion();

#if defined(MOD_EVENTS_FOUND_RELIGION)
	if (MOD_EVENTS_FOUND_RELIGION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionReformed, ePlayer, eReligion, eBelief1);
	}
#endif

	//Notify the masses
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
		if(pNotifications){
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_S");
			Localization::String notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED");
			notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName();

			// Message slightly different for reformation player
			if(ePlayer == eNotifyPlayer)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER");
				localizedText << it->GetName();

				pNotifications->Add(NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			else
			{
				CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());
				if(kNotifyTeam.isHasMet(kPlayer.getTeam()))
				{
					pNotifications->Add(NOTIFICATION_REFORMATION_BELIEF_ADDED, notificationText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
				else
				{
					Localization::String unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_UNKNOWN");
					unknownText << it->GetName();

					pNotifications->Add(NOTIFICATION_REFORMATION_BELIEF_ADDED, unknownText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
			}
		}

		// Logging
		if(GC.getLogging())
		{
			CvString strLogMsg;
			strLogMsg = kPlayer.getCivilizationShortDescription();
			strLogMsg += ", REFORMATION BELIEF ADDED";
			LogReligionMessage(strLogMsg);
		}
	}
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Move the Holy City for a religion (useful for scenario scripting)
void CvGameReligions::SetHolyCity(ReligionTypes eReligion, CvCity* pkHolyCity)
{
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion)
		{
			it->m_iHolyCityX = pkHolyCity->getX();
			it->m_iHolyCityY = pkHolyCity->getY();
			break;
		}
	}
}

/// Switch founder for a religion (useful for scenario scripting)
void CvGameReligions::SetFounder(ReligionTypes eReligion, PlayerTypes eFounder)
{
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion)
		{
			it->m_eFounder = eFounder;
			break;
		}
	}
}
#if defined(MOD_BALANCE_CORE)
/// Switch founder for a religion (useful for scenario scripting)
void CvGameReligions::SetFoundYear(ReligionTypes eReligion, int iValue)
{
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion)
		{
			it->m_iTurnFounded = iValue;
			break;
		}
	}
}
/// Switch founder for a religion (useful for scenario scripting)
int CvGameReligions::GetFoundYear(ReligionTypes eReligion)
{
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion)
		{
			return it->m_iTurnFounded;
		}
	}
	return -1;
}
#endif

/// After a religion is enhanced, the newly chosen beliefs need to be turned on in all cities
void CvGameReligions::UpdateAllCitiesThisReligion(ReligionTypes eReligion)
{
	int iLoop;

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isAlive())
		{
			for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				if(pCity->GetCityReligions()->GetReligiousMajority() == eReligion)
				{
					pCity->UpdateReligion(eReligion);
				}
			}
		}
	}
}

/// Return a pointer to a religion that has been founded
const CvReligion* CvGameReligions::GetReligion(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
#if defined(MOD_BALANCE_CORE)
	if(eReligion == NO_RELIGION)
	{
		return NULL;
	}
#endif
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion && it->m_eReligion == RELIGION_PANTHEON)
		{
			if(it->m_eFounder == ePlayer)
			{
				return it;
			}
		}
		else if(it->m_eReligion == eReligion)
		{
			return it;
		}
	}

	return NULL;
}

/// Has some religion already claimed this belief?
#if defined(MOD_TRAITS_ANY_BELIEF)
// We don't care if either ePlayer has the "can have any belief" trait or it's in the "religion" of a player with that trait
bool CvGameReligions::IsInSomeReligion(BeliefTypes eBelief, PlayerTypes ePlayer) const
#else
bool CvGameReligions::IsInSomeReligion(BeliefTypes eBelief) const
#endif
{
#if defined(MOD_TRAITS_ANY_BELIEF)
	if(ePlayer == NO_PLAYER)
	{
		ePlayer = GC.getGame().getActivePlayer();
	}
	bool bAnyBelief = (ePlayer == NO_PLAYER) ? false : GET_PLAYER(ePlayer).GetPlayerTraits()->IsAnyBelief();
#endif

	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{

		if(it->m_Beliefs.HasBelief(eBelief))
		{
#if defined(MOD_TRAITS_ANY_BELIEF)
			if (MOD_TRAITS_ANY_BELIEF) {
				if (it->m_eFounder == ePlayer) {
					// If it's in my religion I definitely can't have it again
					return true;
				} else if (bAnyBelief) {
					// In the religion of someone else, but I can have any belief, so I can have it as well
					continue;
				} else if (GET_PLAYER(it->m_eFounder).GetPlayerTraits()->IsAnyBelief() && !GET_PLAYER(it->m_eFounder).GetPlayerTraits()->IsAlwaysReligion()) {
					// In a religion of someone who can have any belief, so I can have it as well
					// But not if we're guaranteed a religion.
					continue;
				}
			}
#endif

			return true;
		}
	}

	return false;
}

/// Get the belief in this player's pantheon
BeliefTypes CvGameReligions::GetBeliefInPantheon(PlayerTypes ePlayer) const
{
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eFounder == ePlayer && it->m_bPantheon)
		{
			return (BeliefTypes)it->m_Beliefs.GetBelief(0);
		}
#if defined(MOD_BALANCE_CORE)
		else if(!it->m_bPantheon && GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion() == it->m_eReligion)
		{
			return (BeliefTypes)it->m_Beliefs.GetBelief(0);
		}
#endif
	}

	return NO_BELIEF;
}

/// Has this player created a pantheon?
bool CvGameReligions::HasCreatedPantheon(PlayerTypes ePlayer) const
{
#if defined(MOD_BALANCE_CORE)
	if(GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion() != NO_RELIGION)
	{
		return true;
	}
#endif
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eFounder == ePlayer && it->m_bPantheon)
		{
			return true;
		}
	}

	return false;
}

/// How many players have created a pantheon?
int CvGameReligions::GetNumPantheonsCreated() const
{
	int iRtnValue = 0;

	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ReligionList::const_iterator it;
		for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
		{
			if (it->m_eFounder == iI)
			{
				iRtnValue++;
				break;
			}
		}
	}

	return iRtnValue;
}

/// List of beliefs that can be adopted by pantheons
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailablePantheonBeliefs(PlayerTypes ePlayer)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailablePantheonBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && pEntry2->IsPantheonBelief() && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsPantheonBelief())
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanHaveBelief, ePlayer, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

// Is the supplied belief available to a pantheon?
#if defined(MOD_TRAITS_ANY_BELIEF)
bool CvGameReligions::IsPantheonBeliefAvailable(BeliefTypes eBelief, PlayerTypes ePlayer)
#else
bool CvGameReligions::IsPantheonBeliefAvailable(BeliefTypes eBelief)
#endif
{
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
#if defined(MOD_TRAITS_ANY_BELIEF)
	if(!IsInSomeReligion(eBelief, ePlayer))
#else
	if(!IsInSomeReligion(eBelief))
#endif
	{
		CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
		if(pEntry && pEntry->IsPantheonBelief())
		{
			return true;
		}
	}

	return false;
}

/// Number of followers of this religion
#if defined(MOD_BALANCE_CORE)
int CvGameReligions::GetNumFollowers(ReligionTypes eReligion, PlayerTypes ePlayer) const
#else
int CvGameReligions::GetNumFollowers(ReligionTypes eReligion) const
#endif
{
	int iRtnValue = 0;
	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
#if defined(MOD_BALANCE_CORE)
			if(ePlayer != NO_PLAYER && ePlayer != (PlayerTypes)iI)
				continue;
#endif
			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				iRtnValue += pLoopCity->GetCityReligions()->GetNumFollowers(eReligion);
			}
		}
	}
	return iRtnValue;
}

/// Number of cities following this religion
int CvGameReligions::GetNumCitiesFollowing(ReligionTypes eReligion) const
{
	int iRtnValue = 0;

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if(pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
				{
					iRtnValue++;
				}
			}
		}
	}
	return iRtnValue;
}

/// Has this player created a religion?
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
bool CvGameReligions::HasCreatedReligion(PlayerTypes ePlayer, bool bIgnoreLocal) const
#else
bool CvGameReligions::HasCreatedReligion(PlayerTypes ePlayer) const
#endif
{
    if (GetReligionCreatedByPlayer(ePlayer) > RELIGION_PANTHEON)
	{
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
		if (MOD_RELIGION_LOCAL_RELIGIONS && bIgnoreLocal) {
			return !(GC.getReligionInfo(GetReligionCreatedByPlayer(ePlayer))->IsLocalReligion());
		}
#endif

		return true;
	}

	return false;
}

/// Has this player reformed their religion?
bool CvGameReligions::HasAddedReformationBelief(PlayerTypes ePlayer) const
{
#if !defined(MOD_BALANCE_CORE)
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
#endif
	ReligionTypes eReligion = GetReligionCreatedByPlayer(ePlayer);
    if (eReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pMyReligion = GetReligion(eReligion, ePlayer);
#if defined(MOD_BALANCE_CORE)
		if (pMyReligion && pMyReligion->m_bReformed)
		{
			return true;
		}
#else
		if (pMyReligion)
		{
			for(int iI = 0; iI < pMyReligion->m_Beliefs.GetNumBeliefs(); iI++)
			{
				const BeliefTypes eBelief = pMyReligion->m_Beliefs.GetBelief(iI);
				CvBeliefEntry* pEntry = pkBeliefs->GetEntry((int)eBelief);
				if (pEntry && pEntry->IsReformationBelief())
				{
					return true;
				}
			}
		}
#endif
	}

	return false;
}

/// Does this player qualify for founder benefits?
bool CvGameReligions::IsEligibleForFounderBenefits(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	const CvReligion* religion = GetReligion(eReligion, NO_PLAYER);
	if(religion)
	{
		CvPlot* pHolyCityPlot = NULL;
		pHolyCityPlot = GC.getMap().plot(religion->m_iHolyCityX, religion->m_iHolyCityY);

#if defined(MOD_BALANCE_CORE)
		if(religion->m_bPantheon && pHolyCityPlot == NULL)
		{
			if(religion->m_eFounder == ePlayer)
			{		
				return true;
			}
		}
		else
		{
			if(religion->m_eFounder == ePlayer && (pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer))
			{		
				return true;
			}
			else if(pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
			{
				return true;
			}
		}
#else
		if(religion->m_eFounder == ePlayer)
		{
			if(!religion->m_bPantheon)
			{
				if(pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
				{
					return true;
				}
			}
		}
#endif
	}

	return false;
}

/// Is this city state friendly with the player that founded this religion?
bool CvGameReligions::IsCityStateFriendOfReligionFounder(ReligionTypes eReligion, PlayerTypes ePlayer)
{
	const CvReligion* religion = GetReligion(eReligion, NO_PLAYER);
	if(religion)
	{
		CvPlayer& kMinor = GET_PLAYER(ePlayer);
		CvPlayer& kFounder = GET_PLAYER(religion->m_eFounder);
		if(!kFounder.isMinorCiv() && kMinor.GetMinorCivAI()->IsFriends(religion->m_eFounder))
		{
			return true;
		}
	}

	return false;
}

/// Get the religion this player created
ReligionTypes CvGameReligions::GetReligionCreatedByPlayer(PlayerTypes ePlayer) const
{
#if defined(MOD_BALANCE_CORE)
	if(ePlayer != NO_PLAYER)
	{
		return GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion();
	}
	else
	{
		return NO_RELIGION;
	}
#else
	ReligionTypes eRtnValue = NO_RELIGION;

	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eFounder == ePlayer)
		{
			if(!it->m_bPantheon)
			{
				eRtnValue = it->m_eReligion;
			}
		}
	}
	return eRtnValue;
#endif
}

/// Get the religion for which this player is eligible for founder benefits
ReligionTypes CvGameReligions::GetFounderBenefitsReligion(PlayerTypes ePlayer) const
{
#if defined(MOD_BALANCE_CORE)
	ReligionTypes eReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionCreatedByPlayer(true);
	if(eReligion == RELIGION_PANTHEON)
		return eReligion;
#else
	ReligionTypes eReligion = GetReligionCreatedByPlayer(ePlayer);
#endif
	if(IsEligibleForFounderBenefits(eReligion, ePlayer))
	{
		return eReligion;
	}

	return NO_RELIGION;
}
#if defined(MOD_BALANCE_CORE)
void CvGameReligions::DoUpdateReligion(PlayerTypes ePlayer)
{
	ReligionTypes eRtnValue = NO_RELIGION;
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eFounder == ePlayer)
		{
			if (it->m_eReligion != RELIGION_PANTHEON)
			{
				//If we founded a religion, but no longer control the holy city, it isn't ours anymore!
				//I guess this technically means we could found another one...
				CvPlot* pHolyCityPlot = NULL;
				pHolyCityPlot = GC.getMap().plot(it->m_iHolyCityX, it->m_iHolyCityY);
				if(pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
				{		
					eRtnValue = it->m_eReligion;
					if(kPlayer.GetReligions()->GetCurrentReligion() != eRtnValue)
					{
						kPlayer.GetReligions()->SetPlayerReligion(eRtnValue);
					}
					return;
				}
			}
		}
	}
	//We didn't found a religion, or we lost our holy city? Okay, let's see if we've conquered a holy city, then.
	if(eRtnValue == NO_RELIGION)
	{
		int iNumHolyCities = 0;
		for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
		{
			//We own this holy city? It is ours to work with now.
			if (it->m_eReligion != RELIGION_PANTHEON)
			{
				CvPlot* pHolyCityPlot = NULL;
				pHolyCityPlot = GC.getMap().plot(it->m_iHolyCityX, it->m_iHolyCityY);
				if(pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
				{
					eRtnValue = it->m_eReligion;
					//Keep track of how many go through this way.
					iNumHolyCities++;
				}
			}
		}
		//If we have 1 holy city, return it.
		if(iNumHolyCities == 1)
		{
			if(kPlayer.GetReligions()->GetCurrentReligion() != eRtnValue)
			{
				kPlayer.GetReligions()->SetPlayerReligion(eRtnValue);
			}
			return;
		}
		//Ugh, we have multiple? Okay, let's prioritize by # of followers in our empire. Biggest faith wins!
		else if(iNumHolyCities > 1)
		{
			ReligionTypes eBestReligion = NO_RELIGION;
			int iBestValue = -1;
			for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
			{
				if(it->m_eReligion != RELIGION_PANTHEON)
				{
					CvPlot* pHolyCityPlot = NULL;
					pHolyCityPlot = GC.getMap().plot(it->m_iHolyCityX, it->m_iHolyCityY);
					if(pHolyCityPlot && pHolyCityPlot->getOwner() == ePlayer)
					{
						//Size matters.
						int iValue = GET_PLAYER(ePlayer).GetReligions()->GetNumDomesticFollowers(it->m_eReligion);
						if(iValue <= 0)
						{
							iValue = 1;
						}
						if(it->m_eFounder == ePlayer)
						{
							iValue += 5;
						}
						if(it->m_bEnhanced)
						{
							iValue += 1;
						}
						if(it->m_bReformed)
						{
							iValue += 1;
						}
						if(iValue > iBestValue)
						{
							eBestReligion = it->m_eReligion;
							iBestValue = iValue;
						}
					}
				}
			}
			kPlayer.GetReligions()->SetPlayerReligion(eBestReligion);
			return;
		}
	}
	//Okay, so are we at 'pantheon' point now? Let's go!
	if(eRtnValue == NO_RELIGION)
	{
		for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
		{
			//Our Pantheon?
			if(it->m_bPantheon && it->m_eFounder)
			{
				//Our national majority?
				if(kPlayer.GetReligions()->GetReligionInMostCities() == it->m_eReligion)
				{
					eRtnValue = it->m_eReligion;
					if(kPlayer.GetReligions()->GetCurrentReligion() != eRtnValue)
					{
						kPlayer.GetReligions()->SetPlayerReligion(eRtnValue);
					}
					return;
				}
				//Our capital majority at minimum?
				else if(kPlayer.getCapitalCity() != NULL && kPlayer.getCapitalCity()->GetCityReligions()->GetReligiousMajority() == it->m_eReligion)
				{
					eRtnValue = it->m_eReligion;
					if(kPlayer.GetReligions()->GetCurrentReligion() != eRtnValue)
					{
						kPlayer.GetReligions()->SetPlayerReligion(eRtnValue);
					}
					return;
				}
			}
		}
	}
	kPlayer.GetReligions()->SetPlayerReligion(NO_RELIGION);
}
#endif
/// Number of religions founded so far (does not include pantheons)
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
int CvGameReligions::GetNumReligionsFounded(bool bIgnoreLocal) const
#else
int CvGameReligions::GetNumReligionsFounded() const
#endif
{
	int iRtnValue = 0;

	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(!it->m_bPantheon)
		{
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
			if (MOD_RELIGION_LOCAL_RELIGIONS && bIgnoreLocal && GC.getReligionInfo(it->m_eReligion)->IsLocalReligion()) {
				continue;
			}
#endif

			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Number of religions enhanced so far
int CvGameReligions::GetNumReligionsEnhanced() const
{
	int iRtnValue = 0;

	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (it->m_bEnhanced)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Number of religions that still can be founded on this size map
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
int CvGameReligions::GetNumReligionsStillToFound(bool bIgnoreLocal) const
#else
int CvGameReligions::GetNumReligionsStillToFound() const
#endif
{
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	return (GC.getMap().getWorldInfo().getMaxActiveReligions() - GetNumReligionsFounded(bIgnoreLocal));
#else
	return (GC.getMap().getWorldInfo().getMaxActiveReligions() - GetNumReligionsFounded());
#endif
}

/// List of beliefs that can be adopted by religion founders
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailableFounderBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailableFounderBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && pEntry2->IsFounderBelief() && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsFounderBelief())
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
					if(ePlayer == NO_PLAYER)
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that can be adopted by religion followers
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailableFollowerBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailableFollowerBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && pEntry2->IsFollowerBelief() && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsFollowerBelief())
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
					if(ePlayer == NO_PLAYER)
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that enhance religions
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailableEnhancerBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailableEnhancerBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && pEntry2->IsEnhancerBelief() && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsEnhancerBelief())
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
					if(ePlayer == NO_PLAYER)
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

/// List of all beliefs still available
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailableBonusBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailableBonusBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && (pEntry2->IsEnhancerBelief() || pEntry2->IsFollowerBelief() || pEntry2->IsFounderBelief() || pEntry2->IsPantheonBelief()) && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && (pEntry->IsEnhancerBelief() || pEntry->IsFollowerBelief() || pEntry->IsFounderBelief() || pEntry->IsPantheonBelief()))
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
					if(ePlayer == NO_PLAYER)
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that are added with Reformation social policy
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
std::vector<BeliefTypes> CvGameReligions::GetAvailableReformationBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
std::vector<BeliefTypes> CvGameReligions::GetAvailableReformationBeliefs()
#endif
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
#if defined(MOD_BALANCE_CORE)
	bool bUniqueExists = false;
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
	{
		for(int iJ = 0; iJ < iNumBeleifs; iJ++)
		{
			const BeliefTypes eBelief2(static_cast<BeliefTypes>(iJ));
			CvBeliefEntry* pEntry2 = pkBeliefs->GetEntry(eBelief2);
			if(pEntry2 && pEntry2->IsReformationBelief() && pEntry2->GetRequiredCivilization() != NO_CIVILIZATION)
			{
				if(GET_PLAYER(ePlayer).getCivilizationType() == pEntry2->GetRequiredCivilization())
				{
					bUniqueExists = true;
					break;
				}
			}
		}
	}
#endif
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
#if defined(MOD_TRAITS_ANY_BELIEF)
		if(!IsInSomeReligion(eBelief, ePlayer))
#else
		if(!IsInSomeReligion(eBelief))
#endif
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsReformationBelief())
			{
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if(bUniqueExists)
					{
						if(pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if(pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable) {
#endif
					availableBeliefs.push_back(eBelief);
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
				}
#endif
			}
		}
	}

	return availableBeliefs;
}

/// How much pressure is exerted between these cities?
int CvGameReligions::GetAdjacentCityReligiousPressure (
	ReligionTypes eReligion, CvCity *pFromCity, CvCity *pToCity, int& iNumTradeRoutesInfluencing, 
	bool bActualValue, bool bPretendTradeConnection) //if bActualValue==false, then assume bPretendTradeConnection
{
	int iPressure = 0;
	iNumTradeRoutesInfluencing = 0;

	if (eReligion <= RELIGION_PANTHEON)
	{
		return iPressure;
	}

	const CvReligion* pReligion = GetReligion(eReligion, pFromCity->getOwner());
	if (!pReligion)
	{
		return iPressure;
	}
#if defined(MOD_BALANCE_CORE)
	if(pFromCity->getOwner() != pToCity->getOwner())
	{
		if(GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pFromCity->getOwner());
			if (pReligion2 && (pFromCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pFromCity->getOwner()).getCivilizationType())
			{
				return iPressure;
			}
		}
		if(GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pToCity->getOwner());
			if (pReligion2 && (pToCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pToCity->getOwner()).getCivilizationType())
			{
				return iPressure;
			}
		}
	}
	else if(pFromCity->getOwner() == pToCity->getOwner())
	{
		if(GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pFromCity->getOwner());
			if (ePantheon != eReligion && pReligion2 && (pFromCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pFromCity->getOwner()).getCivilizationType())
			{
				return iPressure;
			}
		}
		if(GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pToCity->getOwner());
			if (ePantheon != eReligion && pReligion2 && (pToCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pToCity->getOwner()).getCivilizationType())
			{
				return iPressure;
			}
		}
	}
	if(!GET_PLAYER(pToCity->getOwner()).isMinorCiv() && GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsForeignReligionSpreadImmune())
	{
		ReligionTypes eToCityReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(false);
		if((eToCityReligion != NO_RELIGION) && (eReligion != eToCityReligion))
		{
			return iPressure;
		}
		eToCityReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionInMostCities();
		if((eToCityReligion != NO_RELIGION) && (eReligion > RELIGION_PANTHEON) && (eReligion != eToCityReligion))
		{
			return iPressure;
		}
	}
	if(GET_PLAYER(pToCity->getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(pToCity->getOwner()).GetMinorCivAI()->GetAlly();
		if(eAlly != NO_PLAYER)
		{
			if(GET_PLAYER(eAlly).GetPlayerTraits()->IsForeignReligionSpreadImmune())
			{
				ReligionTypes eToCityReligion = GET_PLAYER(eAlly).GetReligions()->GetReligionCreatedByPlayer(false);
				if((eToCityReligion != NO_RELIGION) && (eReligion != eToCityReligion))
				{
					return iPressure;
				}
				eToCityReligion = GET_PLAYER(eAlly).GetReligions()->GetReligionInMostCities();
				if((eToCityReligion != NO_RELIGION) && (eReligion > RELIGION_PANTHEON) && (eReligion != eToCityReligion))
				{
					return iPressure;
				}
			}
		}
	}
#endif

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eReligion)->IsLocalReligion()) {
		// Can only spread a local religion to our own cities or City States
		if (pToCity->getOwner() < MAX_MAJOR_CIVS && pFromCity->getOwner() != pToCity->getOwner()) {
			return iPressure;
		}

		// Cannot spread if either city is occupied or a puppet
		if ((pFromCity->IsOccupied() && !pFromCity->IsNoOccupiedUnhappiness()) || pFromCity->IsPuppet() ||
			(pToCity->IsOccupied() && !pToCity->IsNoOccupiedUnhappiness()) || pToCity->IsPuppet()) {
			return iPressure;
		}
	}
#endif

	// Are the cities within the minimum distance?
	int iMaxDistance = GC.getRELIGION_ADJACENT_CITY_DISTANCE();

	// Boost to distance due to belief?
	int iDistanceMod = pReligion->m_Beliefs.GetSpreadDistanceModifier(pFromCity->getOwner());
#if defined(MOD_BALANCE_CORE)
	//Boost from policy of other player?
	if(GET_PLAYER(pToCity->getOwner()).GetReligionDistance() != 0)
	{
		if(pToCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
		{
			//Do we have a religion?
			ReligionTypes ePlayerReligion = GetReligionCreatedByPlayer(pToCity->getOwner());
			
			if(ePlayerReligion <= RELIGION_PANTHEON)
			{
				//No..but did we adopt one?
				ePlayerReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionInMostCities();
				//Nope, so full power.
				if(ePlayerReligion <= RELIGION_PANTHEON)
				{
					iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
				}
				//Yes, so only apply distance bonus to adopted faith.
				else if(eReligion == ePlayerReligion)
				{
					iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
				}
			}
			//We did! Only apply bonuses if we founded this faith or it is the religion we have in most of our cities.
			else if((pReligion->m_eFounder == pToCity->getOwner()) || (eReligion == GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionInMostCities()))
			{
				iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
			}
		}
	}
#endif
	if(iDistanceMod > 0)
	{
		iMaxDistance *= (100 + iDistanceMod);
		iMaxDistance /= 100;
	}

	//estimate the distance between the cities from the traderoute cost. will be influences by terrain features, routes, open borders etc
	int iApparentDistance = INT_MAX;
	SPath path; //trade routes are not necessarily symmetric in case of of unrevealed tiles etc
	if ( GC.getGame().GetGameTrade()->HavePotentialTradePath(false,pFromCity,pToCity,&path) )
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistance );
	}
	if ( GC.getGame().GetGameTrade()->HavePotentialTradePath(false,pToCity,pFromCity,&path) )
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistance );
	}
	if ( GC.getGame().GetGameTrade()->HavePotentialTradePath(true,pFromCity,pToCity,&path) )
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistance );
	}
	if ( GC.getGame().GetGameTrade()->HavePotentialTradePath(true,pToCity,pFromCity,&path) )
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistance );
	}

	bool bWithinDistance = (iApparentDistance <= iMaxDistance);
	bool bConnectedWithTrade = bActualValue ? GC.getGame().GetGameTrade()->CitiesHaveTradeConnection(pFromCity, pToCity) : bPretendTradeConnection;

	if(bWithinDistance || bConnectedWithTrade)
	{
		iPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();

		//scale by relative faith output (the weaker city cannot influence the stronger city much)
		float fFaithPowerMod = sqrt( float(pFromCity->getYieldRateTimes100(YIELD_FAITH,true)) / max(pToCity->getYieldRateTimes100(YIELD_FAITH,true),100) );
		int iFaithPowerMod = int(100*fFaithPowerMod+0.5f);
		if (iFaithPowerMod<100)
		{
			iPressure *= iFaithPowerMod;
			iPressure /= 100;
		}

		if (bConnectedWithTrade)
		{
			if (bActualValue)
				iNumTradeRoutesInfluencing++;

			int iTradeReligionModifer = GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->GetTradeReligionModifier();
#if defined(MOD_BALANCE_CORE_POLICIES)
			iTradeReligionModifer += GET_PLAYER(pFromCity->getOwner()).GetTradeReligionModifier();
#endif
#if defined(MOD_BALANCE_CORE)
			if(pFromCity->GetReligiousTradeModifier() != 0)
			{
				iTradeReligionModifer += pFromCity->GetReligiousTradeModifier();
			}
			int iReligionTradeMod = pReligion->m_Beliefs.GetPressureChangeTradeRoute(pFromCity->getOwner());
			if(iReligionTradeMod != 0)
			{
				iTradeReligionModifer += iReligionTradeMod;
			}
#endif
			if (iTradeReligionModifer != 0)
			{
				iPressure *= 100 + iTradeReligionModifer;
				iPressure /= 100;
			}
		}

		// If we are spreading to a friendly city state, increase the effectiveness if we have the right belief
		if(IsCityStateFriendOfReligionFounder(eReligion, pToCity->getOwner()))
		{
			int iFriendshipMod = pReligion->m_Beliefs.GetFriendlyCityStateSpreadModifier(pFromCity->getOwner());
			if(iFriendshipMod > 0)
			{
				iPressure *= (100 + iFriendshipMod);
				iPressure /= 100;
			}
		}

		// Have a belief that always strengthens spread?
		int iStrengthMod = pReligion->m_Beliefs.GetSpreadStrengthModifier(pFromCity->getOwner());
		if(iStrengthMod > 0)
		{
			TechTypes eDoublingTech = pReligion->m_Beliefs.GetSpreadModifierDoublingTech(pFromCity->getOwner());
			if(eDoublingTech != NO_TECH)
			{
				CvPlayer& kPlayer = GET_PLAYER(pReligion->m_eFounder);
				if(GET_TEAM(kPlayer.getTeam()).GetTeamTechs()->HasTech(eDoublingTech))
				{
					iStrengthMod *= 2;
				}
			}
			iPressure *= (100 + iStrengthMod);
			iPressure /= 100;
		}
#if defined(MOD_BALANCE_CORE)
		int iPolicyMod = GET_PLAYER(pFromCity->getOwner()).GetPressureMod();
		if(iPolicyMod != 0)
		{
			//If the faith being spread is our founded faith, or our adopted faith...
			if(eReligion > RELIGION_PANTHEON && ((GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(pFromCity->getOwner()) == eReligion) || (eReligion == (GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionInMostCities()))))
			{
				//...and the target city doesn't have a majority religion, we get the bonus.
				if(pToCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
				{
					iPressure *= (100 + iPolicyMod);
					iPressure /= 100;
				}
			}
		}
#endif

		// Strengthened spread from World Congress? (World Religion)
		int iLeaguesMod = GC.getGame().GetGameLeagues()->GetReligionSpreadStrengthModifier(pFromCity->getOwner(), eReligion);
#if defined(MOD_API_EXTENSIONS)
		// Trust the modder if they set a negative mod
		if (iLeaguesMod != 0)
#else
		if (iLeaguesMod > 0)
#endif
		{
			iPressure *= (100 + iLeaguesMod);
			iPressure /= 100;
		}

		// Building that boosts pressure from originating city?
		int iModifier = pFromCity->GetCityReligions()->GetReligiousPressureModifier();
#if defined(MOD_API_EXTENSIONS)
		// Trust the modder if they set a negative mod
		if (iModifier != 0)
#else
		if (iModifier > 0)
#endif
		{
			iPressure *= (100 + iModifier);
			iPressure /= 100;
		}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(pToCity->getOwner()).getTeam()).IsVassal(GET_PLAYER(pFromCity->getOwner()).getTeam()))
		{
			iPressure *= 2;
		}
#endif
#if defined(MOD_BALANCE_CORE)
		if(GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsPopulationBoostReligion())
		{
			if(eReligion > RELIGION_PANTHEON && ((GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(pFromCity->getOwner()) == eReligion) || (GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionInMostCities() == eReligion)))
			{
				int iPopReligionModifer = (pFromCity->GetCityReligions()->GetNumFollowers(eReligion) * 12);
				if(iPopReligionModifer > 450)
				{
					iPopReligionModifer = 450;
				}
				if (iPopReligionModifer != 0)
				{
					iPressure *= 100 + iPopReligionModifer;
					iPressure /= 100;
				}
			}
		}
#endif

		//if there is an explicit trade route, there is no distance taper
		if (!bConnectedWithTrade)
		{
			int iDistanceScale = MapToPercent( iApparentDistance, iMaxDistance, iMaxDistance/2+1 );
			iPressure *= iDistanceScale;
			iPressure /= 100;
		}
	}

#if defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	if (MOD_RELIGION_CONVERSION_MODIFIERS) {
		// Modify iPressure based on city defenses, but only against hostile cities (ie any not the same player as this city)
		PlayerTypes eFromPlayer = pFromCity->getOwner();
		PlayerTypes eToPlayer = pToCity->getOwner();
		
		if (eFromPlayer != eToPlayer) {
			CvPlayer& pToPlayer = GET_PLAYER(eToPlayer);
			int iCityModifier = pToCity->GetConversionModifier();
			
			if (pToPlayer.isMinorCiv() && pToPlayer.GetMinorCivAI()->IsActiveQuestForPlayer(eFromPlayer, MINOR_CIV_QUEST_SPREAD_RELIGION)) {
				// The City State actively wants this religion
				iCityModifier *= -1;
			}

			iPressure *= (100 + (iCityModifier + pToPlayer.GetConversionModifier() + pToPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CONVERSION_MODIFIER)));
			iPressure /= 100;
		}
	}
#endif
	// CUSTOMLOG("GetAdjacentCityReligiousPressure for %i from %s to %s is %i", eReligion, pFromCity->getName().c_str(), pToCity->getName().c_str(), iPressure);
	return iPressure;
}

/// How much does this prophet cost (recursive)
int CvGameReligions::GetFaithGreatProphetNumber(int iNum) const
{
	int iRtnValue = 0;

	if(iNum >= 1)
	{
		if(iNum == 1)
		{
			iRtnValue = GC.getRELIGION_MIN_FAITH_FIRST_PROPHET();
		}
#if defined(MOD_BALANCE_CORE)
		else if(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && iNum == 1)
		{
			iRtnValue = GC.getRELIGION_MIN_FAITH_SECOND_PROPHET();
		}
#endif
		else
		{
			iRtnValue = (GC.getRELIGION_FAITH_DELTA_NEXT_PROPHET() * (iNum - 1)) + GetFaithGreatProphetNumber(iNum - 1);
		}
	}

	return iRtnValue;
}

/// How much does this great person cost (recursive)
int CvGameReligions::GetFaithGreatPersonNumber(int iNum) const
{
	int iRtnValue = 0;

	if(iNum >= 1)
	{
		if(iNum == 1)
		{
			iRtnValue = GC.getRELIGION_MIN_FAITH_FIRST_GREAT_PERSON();
		}
		else
		{
			iRtnValue = (GC.getRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON() * (iNum - 1)) + GetFaithGreatPersonNumber(iNum - 1);
		}
	}

	return iRtnValue;
}

/// Does the religion in nearby city give this battle winner a yield? If so return multipler of losing unit strength
int CvGameReligions::GetBeliefYieldForKill(YieldTypes eYield, int iX, int iY, PlayerTypes eWinningPlayer)
{
	int iRtnValue = 0;
	int iMultiplier;
	int iLoop;
	CvCity* pLoopCity;

	// Only Faith supported for now
	if(eYield != YIELD_FAITH)
	{
		return iRtnValue;
	}

	for(pLoopCity = GET_PLAYER(eWinningPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWinningPlayer).nextCity(&iLoop))
	{
		// Find religion in this city
		ReligionTypes eReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();

		if(eReligion != NO_RELIGION)
		{
			// Find distance to this city
			int iDistance = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());

			// Do we have a yield from this?
			iMultiplier = GetReligion(eReligion, eWinningPlayer)->m_Beliefs.GetFaithFromKills(iDistance, eWinningPlayer);
			if(iMultiplier > 0)
			{
				// Just looking for one city providing this
				iRtnValue = iMultiplier;
				break;
			}
			else
			{
				BeliefTypes eSecondaryPantheon = pLoopCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iMultiplier = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFaithFromKills();
					if(iMultiplier > 0 && iDistance <= GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetMaxDistance())
					{
						// Just looking for one city providing this
						iRtnValue = iMultiplier;
						break;
					}	
				}
			}
		}
	}

	return iRtnValue;
}

/// Build log filename
CvString CvGameReligions::GetLogFileName() const
{
	CvString strLogName;
	strLogName = "ReligionLog.csv";
	return strLogName;
}

// PRIVATE METHODS

/// Has this religion already been founded?
bool CvGameReligions::HasBeenFounded(ReligionTypes eReligion)
{
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			return true;
		}
	}

	return false;
}

/// Does any civ in the game like this religion?
bool CvGameReligions::IsPreferredByCivInGame(ReligionTypes eReligion)
{
	PlayerTypes eLoopPlayer;

	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		eLoopPlayer = (PlayerTypes) iI;
		CvPlayer& loopPlayer = GET_PLAYER(eLoopPlayer);

		if(!loopPlayer.isAlive())
		{
			continue;
		}

		ReligionTypes eCivReligion = loopPlayer.getCivilizationInfo().GetReligion();
		if(eCivReligion == eReligion)
		{
			return true;
		}
	}

	return false;
}

/// Time to spawn a Great Prophet?
bool CvGameReligions::CheckSpawnGreatProphet(CvPlayer& kPlayer)
{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eUnit = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
#else
	UnitTypes eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
#endif
	if (eUnit == NO_UNIT)
	{
		return false;
	}
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
	bool prophetboughtwithfaith = false;
#endif
	const CvReligion* pReligion = NULL;
	const int iFaith = kPlayer.GetFaith();
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int iCost = kPlayer.GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/, MOD_GLOBAL_TRULY_FREE_GP);
#else
	int iCost = kPlayer.GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/);
#endif

	ReligionTypes ePlayerReligion = GetReligionCreatedByPlayer(kPlayer.GetID());
	if(ePlayerReligion > RELIGION_PANTHEON)
	{
		pReligion = GetReligion(ePlayerReligion, kPlayer.GetID());
	}

	// If player hasn't founded a religion yet, drop out of this if all religions have been founded
#if defined(MOD_BALANCE_CORE)
	else if(GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
#else
	else if(GetNumReligionsStillToFound() <= 0)
#endif
	{
		return false;
	}

	if(iFaith < iCost)
	{
		return false;
	}

#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
	int iBaseChance = GC.getRELIGION_BASE_CHANCE_PROPHET_SPAWN();
	int iChance = iBaseChance;
#else
	int iChance = GC.getRELIGION_BASE_CHANCE_PROPHET_SPAWN();
#endif
	iChance += (iFaith - iCost);

	int iRand = GC.getGame().getJonRandNum(100, "Religion: spawn Great Prophet roll.");
	if(iRand >= iChance)
	{
		return false;
	}

	CvCity* pSpawnCity = NULL;
	if(pReligion)
	{
		pSpawnCity = GC.getMap().plot(pReligion->m_iHolyCityX, pReligion->m_iHolyCityY)->getPlotCity();
	}

	if(pSpawnCity != NULL && pSpawnCity->getOwner() == kPlayer.GetID())
	{
#if defined(MOD_BUGFIX_MINOR)
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
		if(MOD_NO_AUTO_SPAWN_PROPHET)
		{
			if (kPlayer.isHuman())
			{
				switch (kPlayer.GetFaithPurchaseType())
				{
					case FAITH_PURCHASE_SAVE_PROPHET:
						pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
						prophetboughtwithfaith = true;
						break;
					case NO_AUTOMATIC_FAITH_PURCHASE:
						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if(pNotifications)
						{
							CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_MISSIONARY");
							CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_MISSIONARY");
							pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
							kPlayer.GetReligions()->SetFaithAtLastNotify(kPlayer.GetFaith());
						}
						break;
				}
			}
			else
			{		
				pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
			}
		}
		else
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
#else
		pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
#endif	
#else
		pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true);
#endif
#else
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, false /*bIncrementCount*/, true, false);
#else
		pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, false /*bIncrementCount*/, true);
#endif
#endif
#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
		if (MOD_RELIGION_KEEP_PROPHET_OVERFLOW && iBaseChance >= 100)
		{
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{
				if (!kPlayer.isHuman() || prophetboughtwithfaith)
				{
					kPlayer.ChangeFaith(-1 * iCost);
				}
			}
			else
				kPlayer.ChangeFaith(-1 * iCost);
#else
			kPlayer.ChangeFaith(-1 * iCost);
#endif
		}
		else
		{
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{
				if (!kPlayer.isHuman() || prophetboughtwithfaith)
				{
					kPlayer.SetFaith(0);
				}
			}
			else
				kPlayer.SetFaith(0);
#endif
#endif
		kPlayer.SetFaith(0);

#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
		}
#endif
	}
	else
	{
#if defined(MOD_BALANCE_CORE_BELIEFS)
		//Random GP spawn/holy city.
		CvCity* pBestCity = NULL;
		if(MOD_BALANCE_CORE_BELIEFS)
		{		
			int iBestWeight = 0;

			int iTempWeight;

			CvCity* pLoopCity;
			int iLoop;
			CvGame& theGame = GC.getGame();
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				iTempWeight = pLoopCity->GetFaithPerTurn() * 5;
				iTempWeight += theGame.getJonRandNum(15, "Faith rand weight.");

				if(iTempWeight > iBestWeight)
				{
					iBestWeight = iTempWeight;
					pBestCity = pLoopCity;
				}
			}
		}
		if(pBestCity != NULL)
		{
			pSpawnCity = pBestCity;
		}
		else
		{
#endif
		pSpawnCity = kPlayer.getCapitalCity();
#if defined(MOD_BALANCE_CORE_BELIEFS)
		}
#endif
		if(pSpawnCity != NULL)
		{
#if defined(MOD_BUGFIX_MINOR)
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{
				if (kPlayer.isHuman())
				{
					switch (kPlayer.GetFaithPurchaseType())
					{
						case FAITH_PURCHASE_SAVE_PROPHET:
							pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
							prophetboughtwithfaith = true;
							break;
						case NO_AUTOMATIC_FAITH_PURCHASE:
							CvNotifications* pNotifications = kPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_MISSIONARY");
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_MISSIONARY");
								pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
								kPlayer.GetReligions()->SetFaithAtLastNotify(kPlayer.GetFaith());
							}
							break;
					}
				}
				else
				{		
					pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
				}
			}
			else
				pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
#else
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
#endif	
#else
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true);
#endif
#else
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, false /*bIncrementCount*/, true, false);
#else
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, false /*bIncrementCount*/, true);
#endif
#endif
#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
		if (MOD_RELIGION_KEEP_PROPHET_OVERFLOW && iBaseChance >= 100)
		{
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{			
				if (!kPlayer.isHuman() || prophetboughtwithfaith)
				{
					kPlayer.ChangeFaith(-1 * iCost);
				}
			}
			else
				kPlayer.ChangeFaith(-1 * iCost);
		}
#else
			kPlayer.ChangeFaith(-1 * iCost);
#endif
		else
		{
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{
				if (!kPlayer.isHuman() || prophetboughtwithfaith)
				{
					kPlayer.SetFaith(0);
				}
			}
			else
				kPlayer.SetFaith(0);
#endif
#endif
		kPlayer.SetFaith(0);
#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
		}
#endif
		}
	}

	// Logging
	if(GC.getLogging() && pSpawnCity)
	{
		CvString strLogMsg;
		strLogMsg = kPlayer.getCivilizationShortDescription();
		strLogMsg += ", PROPHET SPAWNED, ";
		strLogMsg += pSpawnCity->getName();
		strLogMsg += ", Faith: 0";
		LogReligionMessage(strLogMsg);
	}

	return true;
}

/// Log a message with status information
void CvGameReligions::LogReligionMessage(CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

// Notify the supplied player (if they are the local player) of an error when founding/modifying a religion/pantheon
void CvGameReligions::NotifyPlayer(PlayerTypes ePlayer, CvGameReligions::FOUNDING_RESULT eResult)
{
	CvString strMessage;
	CvString strSummary;

	NotificationTypes eNotificationType = NOTIFICATION_RELIGION_ERROR;

	switch(eResult)
	{
	case FOUNDING_OK:
		break;
	case FOUNDING_BELIEF_IN_USE:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_PANTHEON_BELIEF_IN_USE");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_PANTHEON_BELIEF_IN_USE");
		break;
	case FOUNDING_RELIGION_IN_USE:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_IN_USE");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_RELIGION_IN_USE");
		break;
	case FOUNDING_NOT_ENOUGH_FAITH:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_NOT_ENOUGH_FAITH_FOR_PANTHEON");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_NOT_ENOUGH_FAITH_FOR_PANTHEON");
		break;
	case FOUNDING_NO_RELIGIONS_AVAILABLE:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_NO_RELIGIONS_AVAILABLE");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_NO_RELIGIONS_AVAILABLE");
		break;
	case FOUNDING_INVALID_PLAYER:
		break;
	case FOUNDING_PLAYER_ALREADY_CREATED_RELIGION:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_ALREADY_CREATED_RELIGION");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ALREADY_CREATED_RELIGION");
		break;
	case FOUNDING_PLAYER_ALREADY_CREATED_PANTHEON:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_ALREADY_CREATED_PANTHEON");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ALREADY_CREATED_PANTHEON");
		break;
	case FOUNDING_NAME_IN_USE:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_NAME_IN_USE");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_RELIGION_NAME_IN_USE");
		break;
	case FOUNDING_RELIGION_ENHANCED:
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_ENHANCED_RELIGION_IN_USE");
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENHANCED_RELIGION_IN_USE");
		break;
	}

	if(strMessage.GetLength() != 0)
		CvNotifications::AddToPlayer(ePlayer, eNotificationType, strMessage.c_str(), strSummary.c_str());
}

// SERIALIZATION

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameReligions& writeTo)
{
	uint uiVersion;
	int iMinFaith;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	if(uiVersion >= 3)
	{
		loadFrom >> iMinFaith;
		writeTo.SetMinimumFaithNextPantheon(iMinFaith);
	}
	else
	{
		writeTo.SetMinimumFaithNextPantheon(0);
	}

	if(uiVersion < 4)
	{
		loadFrom >> iMinFaith;
		//	writeTo.SetMinimumFaithNextGreatProphet(iMinFaith);  -- eliminated in Version 4
	}

	if(uiVersion >= 2)
	{
		int iEntriesToRead;
		CvReligion tempItem;

		writeTo.m_CurrentReligions.clear();
		loadFrom >> iEntriesToRead;
		for(int iI = 0; iI < iEntriesToRead; iI++)
		{
			loadFrom >> tempItem;
			writeTo.m_CurrentReligions.push_back(tempItem);
		}
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameReligions& readFrom)
{
	uint uiVersion = 4;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.GetMinimumFaithNextPantheon();

	ReligionList::const_iterator it;
	saveTo << readFrom.m_CurrentReligions.size();
	for(it = readFrom.m_CurrentReligions.begin(); it != readFrom.m_CurrentReligions.end(); it++)
	{
		saveTo << *it;
	}

	return saveTo;
}

//=====================================
// CvPlayerReligions
//=====================================
/// Constructor
CvPlayerReligions::CvPlayerReligions(void):
	m_pPlayer(NULL),
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	m_iFaithAtLastNotify(0),
#endif
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	m_iNumFreeProphetsSpawned(0),
#endif
#if defined(MOD_BALANCE_CORE)
	m_majorityPlayerReligion(NO_RELIGION),
	m_ePlayerCurrentReligion(NO_RELIGION),
	m_PlayerStateReligion(NO_RELIGION),
#endif
	m_iNumProphetsSpawned(0),
	m_bFoundingReligion(false)
{
}

/// Destructor
CvPlayerReligions::~CvPlayerReligions(void)
{
	Uninit();
}

/// Initialize class data
void CvPlayerReligions::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}

/// Cleanup
void CvPlayerReligions::Uninit()
{

}

/// Reset
void CvPlayerReligions::Reset()
{
	m_bFoundingReligion = false;
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	m_iNumFreeProphetsSpawned = 0;
#endif
	m_iNumProphetsSpawned = 0;
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	m_iFaithAtLastNotify = 0;
#endif
#if defined(MOD_BALANCE_CORE)
	m_majorityPlayerReligion = NO_RELIGION;
	m_ePlayerCurrentReligion = NO_RELIGION;
	m_PlayerStateReligion = NO_RELIGION;
#endif
}

/// Serialization read
void CvPlayerReligions::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	MOD_SERIALIZE_READ(61, kStream, m_iNumFreeProphetsSpawned, 0);
#endif
	kStream >> m_iNumProphetsSpawned;
	kStream >> m_bFoundingReligion;
#if defined(MOD_BALANCE_CORE)
	kStream >> m_majorityPlayerReligion;
	kStream >> m_ePlayerCurrentReligion;
	kStream >> m_PlayerStateReligion;
#endif
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	MOD_SERIALIZE_READ(42, kStream, m_iFaithAtLastNotify, 0);
#endif
}

/// Serialization write
void CvPlayerReligions::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	MOD_SERIALIZE_WRITE(kStream, m_iNumFreeProphetsSpawned);
#endif
	kStream << m_iNumProphetsSpawned;
	kStream << m_bFoundingReligion;
#if defined(MOD_BALANCE_CORE)
	kStream << m_majorityPlayerReligion;
	kStream << m_ePlayerCurrentReligion;
	kStream << m_PlayerStateReligion;
#endif
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	MOD_SERIALIZE_WRITE(kStream, m_iFaithAtLastNotify);
#endif
}

/// How many prophets have we spawned
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
int CvPlayerReligions::GetNumProphetsSpawned(bool bExcludeFree) const
#else
int CvPlayerReligions::GetNumProphetsSpawned() const
#endif
{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int iCount = m_iNumProphetsSpawned;
	if (bExcludeFree) iCount -= m_iNumFreeProphetsSpawned;
	return iCount;
#else
	return m_iNumProphetsSpawned;
#endif
}

/// Change count of prophets spawned
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
void CvPlayerReligions::ChangeNumProphetsSpawned(int iValue, bool bIsFree)
#else
void CvPlayerReligions::ChangeNumProphetsSpawned(int iValue)
#endif
{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	if (bIsFree) m_iNumFreeProphetsSpawned += iValue;
#endif
	m_iNumProphetsSpawned += iValue;
}

/// How much will the next prophet cost this player?
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
int CvPlayerReligions::GetCostNextProphet(bool bIncludeBeliefDiscounts, bool bAdjustForSpeedDifficulty, bool bExcludeFree) const
#else
int CvPlayerReligions::GetCostNextProphet(bool bIncludeBeliefDiscounts, bool bAdjustForSpeedDifficulty) const
#endif
{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int iCost = GC.getGame().GetGameReligions()->GetFaithGreatProphetNumber(GetNumProphetsSpawned(bExcludeFree) + 1);
#else
	int iCost = GC.getGame().GetGameReligions()->GetFaithGreatProphetNumber(m_iNumProphetsSpawned + 1);
#endif

	// Boost to faith due to belief?
	ReligionTypes ePlayerReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if(ePlayerReligion == NO_RELIGION)
	{
		ePlayerReligion = GetReligionInMostCities();
	}
	if (bIncludeBeliefDiscounts && ePlayerReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pReligion = NULL;
		pReligion = GC.getGame().GetGameReligions()->GetReligion(ePlayerReligion, m_pPlayer->GetID());
		if(pReligion)
		{
			int iProphetCostMod = pReligion->m_Beliefs.GetProphetCostModifier(m_pPlayer->GetID());

			if(iProphetCostMod != 0)
			{
				iCost *= (100 + iProphetCostMod);
				iCost /= 100;
			}
		}
	}

	if (bAdjustForSpeedDifficulty)
	{
		// Adjust for game speed
		iCost *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iCost /= 100;

		// Adjust for difficulty
		if(!m_pPlayer->isHuman() && !m_pPlayer->IsAITeammateOfHuman() && !m_pPlayer->isBarbarian())
		{
			iCost *= GC.getGame().getHandicapInfo().getAITrainPercent();
			iCost /= 100;
		}
	}

	return iCost;
}

/// Has this player created a pantheon?
bool CvPlayerReligions::HasCreatedPantheon() const
{
	return GC.getGame().GetGameReligions()->HasCreatedPantheon(m_pPlayer->GetID());
}

/// Has this player created a religion?
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
bool CvPlayerReligions::HasCreatedReligion(bool bIgnoreLocal) const
#else
bool CvPlayerReligions::HasCreatedReligion() const
#endif
{
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	return GC.getGame().GetGameReligions()->HasCreatedReligion(m_pPlayer->GetID(), bIgnoreLocal);
#else
	return GC.getGame().GetGameReligions()->HasCreatedReligion(m_pPlayer->GetID());
#endif
}

/// Has this player reformed his religion
bool CvPlayerReligions::HasAddedReformationBelief() const
{
	return GC.getGame().GetGameReligions()->HasAddedReformationBelief(m_pPlayer->GetID());
}

/// Get the religion this player created
ReligionTypes CvPlayerReligions::GetReligionCreatedByPlayer(bool bIncludePantheon) const
{
#if defined(MOD_BALANCE_CORE)
	return GetCurrentReligion(bIncludePantheon);
#else
	return GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(m_pPlayer->GetID());
#endif
}
#if defined(MOD_BALANCE_CORE)
void CvPlayerReligions::SetPlayerReligion(ReligionTypes eReligion)
{
	//New state faith? Let's announce this.
	if(m_ePlayerCurrentReligion != eReligion)
	{
		// Message slightly different for founder player
		if(m_pPlayer->GetNotifications() && eReligion != NO_RELIGION && eReligion != RELIGION_PANTHEON)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
			if(pReligion)
			{
				CvString szReligionName = pReligion->GetName();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_STATE_RELIGION_S");
				strSummary << szReligionName;
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_STATE_RELIGION");
				localizedText << szReligionName;
#if defined(MOD_API_EXTENSIONS)
				m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
				m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
			}
		}
		m_ePlayerCurrentReligion = eReligion;
	}
}
ReligionTypes CvPlayerReligions::GetCurrentReligion(bool bIncludePantheon) const
{
	if(m_ePlayerCurrentReligion == RELIGION_PANTHEON && bIncludePantheon)
	{
		return m_ePlayerCurrentReligion;
	}
	else if(m_ePlayerCurrentReligion == RELIGION_PANTHEON && !bIncludePantheon)
	{
		return NO_RELIGION;
	}
	return m_ePlayerCurrentReligion;
}
#endif

/// Does this player have enough faith to buy a religious unit or building?
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
bool CvPlayerReligions::CanAffordFaithPurchase(int iMinimumFaith) const
#else
bool CvPlayerReligions::CanAffordFaithPurchase() const
#endif
{
	int iFaith = m_pPlayer->GetFaith();
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if(pCapital)
	{
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			const UnitTypes eUnit = static_cast<UnitTypes>(iI);
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if(pkUnitInfo)
			{
				if (m_pPlayer->IsCanPurchaseAnyCity(false, false, eUnit, NO_BUILDING, YIELD_FAITH))
				{
					int iCost = pCapital->GetFaithPurchaseCost(eUnit, true);
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
					if(iCost != 0 && iFaith > iCost && iCost > iMinimumFaith)
#else
					if(iCost != 0 && iFaith > iCost)
#endif
					{
						return true;
					}
				}
			}
		}
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				if (m_pPlayer->IsCanPurchaseAnyCity(false, false, NO_UNIT, eBuilding, YIELD_FAITH))
				{
					int iCost = pCapital->GetFaithPurchaseCost(eBuilding);
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
					if(iCost != 0 && iFaith > iCost && iCost > iMinimumFaith)
#else
					if(iCost != 0 && iFaith > iCost)
#endif
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
bool CvPlayerReligions::CanAffordNextPurchase()
{
	int iPlayerFaith = m_pPlayer->GetFaith();

	if (iPlayerFaith < m_iFaithAtLastNotify) {
		// We've spent faith, so reduce the threshold we're checking at
		m_iFaithAtLastNotify = iPlayerFaith;
	}

	return CanAffordFaithPurchase(m_iFaithAtLastNotify);
}

void CvPlayerReligions::SetFaithAtLastNotify(int iFaith)
{
	m_iFaithAtLastNotify = iFaith;
}
#endif

/// Does this player have a city following a religion?
bool CvPlayerReligions::HasReligiousCity() const
{
	int iLoop = 0;
	CvCity* pCity = NULL;

	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pCity)
		{
			if(pCity->GetCityReligions()->GetReligiousMajority() > RELIGION_PANTHEON)
			{
				return true;
			}
		}
	}

	return false;
}

/// Is this player happily following this other player's religion?
bool CvPlayerReligions::HasOthersReligionInMostCities(PlayerTypes eOtherPlayer) const
{
	// Not happy about it if have their own religion
	if(HasCreatedReligion())
	{
		return false;
	}

	ReligionTypes eOtherReligion = GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(eOtherPlayer);

	if(eOtherReligion <= RELIGION_PANTHEON)
	{
		return false;
	}

	int iNumFollowingCities = 0;
	int iLoop = 0;
	CvCity* pCity = NULL;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pCity)
		{
			if(pCity->GetCityReligions()->GetReligiousMajority() == eOtherReligion)
			{
				iNumFollowingCities++;
			}
		}
	}

	// Over half?
	return (iNumFollowingCities * 2 > m_pPlayer->getNumCities());
}

/// Do a majority of this player's cities follow a specific religion?
bool CvPlayerReligions::HasReligionInMostCities(ReligionTypes eReligion) const
{
	if (eReligion <= RELIGION_PANTHEON)
	{
		return false;
	}

	int iNumFollowingCities = 0;
	int iLoop = 0;
	CvCity* pCity = NULL;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pCity)
		{
			if(pCity->GetCityReligions()->GetReligiousMajority() == eReligion)
			{
				iNumFollowingCities++;
			}
		}
	}

	// Over half?
#if defined(MOD_BALANCE_CORE)
	// Equal to make OCC/Venice possible.
	return (iNumFollowingCities * 2 >= m_pPlayer->getNumCities());
#else
	return (iNumFollowingCities * 2 > m_pPlayer->getNumCities());
#endif
}

#if defined(MOD_BALANCE_CORE)
/// What religion is followed in a majority of our cities?
ReligionTypes CvPlayerReligions::GetReligionInMostCities() const
{
	return m_majorityPlayerReligion;
}
#if defined(MOD_BALANCE_CORE)
	//JFD
/// What is our state religion?
ReligionTypes CvPlayerReligions::GetStateReligion() const
{
	return m_PlayerStateReligion;
}
/// What is our state religion?
void CvPlayerReligions::SetStateReligion(ReligionTypes eReligion)
{
	if(GetStateReligion() == NO_RELIGION)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_StateReligionAdopted, m_pPlayer->GetID(), eReligion, GetStateReligion());
	}
	else
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_StateReligionChanged, m_pPlayer->GetID(), eReligion, GetStateReligion());
	}
	m_PlayerStateReligion = eReligion;
}
int CvPlayerReligions::GetNumCitiesWithStateReligion(ReligionTypes eReligion)
{
	int iNum = 0;
	if(eReligion == NO_RELIGION)
	{
		if(GetStateReligion() == NO_RELIGION)
		{
			return 0;
		}
		else
		{
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity != NULL)
				{
					if(pLoopCity->GetCityReligions()->GetReligiousMajority() == GetStateReligion())
					{
						iNum++;
					}
				}
			}
		}
	}
	else
	{
		int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity != NULL)
				{
					if(pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
					{
						iNum++;
					}
				}
			}
	}

	return iNum;
}
#endif
/// What religion is followed in a majority of our cities?
bool CvPlayerReligions::ComputeMajority(bool bNotifications)
{
	for (int iI = RELIGION_PANTHEON + 1; iI < GC.GetGameReligions()->GetNumReligions(); iI++)
	{
		ReligionTypes eReligion = (ReligionTypes)iI;
		if (HasReligionInMostCities(eReligion))
		{
#if defined(MOD_BALANCE_CORE)
			//New state faith? Let's announce this.
			if(bNotifications && m_majorityPlayerReligion != eReligion && m_majorityPlayerReligion != NO_RELIGION)
			{
				// Message slightly different for founder player
				if(m_pPlayer->GetNotifications())
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
					if(pReligion)
					{
						CvString szReligionName = pReligion->GetName();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_MAJORITY_S");
						strSummary << szReligionName;
						Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_MAJORITY");
						localizedText << szReligionName;
#if defined(MOD_API_EXTENSIONS)
						m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
#else
						m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
#endif
					}
				}
			}
#endif
			m_majorityPlayerReligion = eReligion;
			return true;
		}
	}
	m_majorityPlayerReligion = NO_RELIGION;
	return false;
}

#else
/// What religion is followed in a majority of our cities?
ReligionTypes CvPlayerReligions::GetReligionInMostCities() const
{
	for (int iI = RELIGION_PANTHEON + 1; iI < GC.GetGameReligions()->GetNumReligions(); iI++)
	{
		ReligionTypes eReligion = (ReligionTypes)iI;
		if (HasReligionInMostCities(eReligion))
		{
			return eReligion;
		}
	}
	return NO_RELIGION;
}

#endif

/// Does this player get a default influence boost with city states following this religion?
int CvPlayerReligions::GetCityStateMinimumInfluence(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	int iMinInfluence = 0;

	ReligionTypes eFounderBenefitReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if(eFounderBenefitReligion == NO_RELIGION)
	{
		eFounderBenefitReligion = m_pPlayer->GetReligions()->GetReligionInMostCities();
	}
	if (eReligion == eFounderBenefitReligion)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		if(eFounderBenefitReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = pReligions->GetReligion(eFounderBenefitReligion, ePlayer);
			if(pReligion)
			{
				iMinInfluence += pReligion->m_Beliefs.GetCityStateMinimumInfluence(ePlayer);
			}
		}
	}

	return iMinInfluence;
}

/// Does this player get a modifier to city state influence boosts?
int CvPlayerReligions::GetCityStateInfluenceModifier(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if(eReligion == NO_RELIGION)
	{
		eReligion = GetReligionInMostCities();
	}
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if(pReligion)
		{
			iRtnValue += pReligion->m_Beliefs.GetCityStateInfluenceModifier(ePlayer);
		}
	}
	return iRtnValue;
}

/// Does this player get religious pressure from spies?
int CvPlayerReligions::GetSpyPressure(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pPlayer->GetID());
	//if(eReligion == NO_RELIGION)
	//{
	//	eReligion = GetReligionInMostCities();
	//}
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if(pReligion)
		{
			iRtnValue += pReligion->m_Beliefs.GetSpyPressure(ePlayer);
		}
	}
	return iRtnValue;
}

/// How many foreign cities are following a religion we founded?
int CvPlayerReligions::GetNumForeignCitiesFollowing(ReligionTypes eReligion) const
{
	CvCity *pLoopCity;
	int iCityLoop;
	int iRtnValue = 0;

	if (eReligion > RELIGION_PANTHEON)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
			if(kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID())
			{
				for(pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
				{
					if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
					{
						iRtnValue++;
					}
				}
			}
		}
	}

	return iRtnValue;
}

/// How many foreign citizens are following a religion we founded?
int CvPlayerReligions::GetNumForeignFollowers(bool bAtPeace, ReligionTypes eReligion) const
{
	CvCity *pLoopCity;
	int iCityLoop;
	int iRtnValue = 0;
	
	if (eReligion > RELIGION_PANTHEON)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
			if(kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID())
			{
				if (!bAtPeace || !atWar(m_pPlayer->getTeam(), kLoopPlayer.getTeam()))
				{
					for(pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
					{
						iRtnValue += pLoopCity->GetCityReligions()->GetNumFollowers(eReligion);
					}
				}
			}
		}
	}

	return iRtnValue;
}
#if defined(MOD_BALANCE_CORE)
int CvPlayerReligions::GetNumDomesticFollowers(ReligionTypes eReligion) const
{
	CvCity *pLoopCity;
	int iCityLoop;
	int iRtnValue = 0;
	
	if (eReligion > RELIGION_PANTHEON)
	{
		for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		{
			iRtnValue += pLoopCity->GetCityReligions()->GetNumFollowers(eReligion);
		}
	}

	return iRtnValue;
}
#endif
//=====================================
// CvCityReligions
//=====================================
/// Constructor
CvCityReligions::CvCityReligions(void):
#if !defined(MOD_BALANCE_CORE)
	m_bHasPaidAdoptionBonus(false),
#endif
#if defined(MOD_BALANCE_CORE)
	m_pCity(NULL),
	m_majorityCityReligion(NO_RELIGION),
#endif
	m_iReligiousPressureModifier(0)
{
	m_ReligionStatus.clear();
}

/// Destructor
CvCityReligions::~CvCityReligions(void)
{

}

/// Initialize class data
void CvCityReligions::Init(CvCity* pCity)
{
	m_pCity = pCity;
#if !defined(MOD_BALANCE_CORE)
	m_bHasPaidAdoptionBonus = false;
#endif
	m_iReligiousPressureModifier = 0;
	m_ReligionStatus.clear();
#if defined(MOD_BALANCE_CORE)
	m_majorityCityReligion = NO_RELIGION;
#endif
}

/// Cleanup
void CvCityReligions::Uninit()
{

}

/// Copy data from old city into new (for conquest)
void CvCityReligions::Copy(CvCityReligions* pOldCity)
{
	m_ReligionStatus.clear();

#if !defined(MOD_BALANCE_CORE)
	SetPaidAdoptionBonus(pOldCity->HasPaidAdoptionBonus());
#endif
	SetReligiousPressureModifier(pOldCity->GetReligiousPressureModifier());

	ReligionInCityList::iterator religionIt;
	for(religionIt = pOldCity->m_ReligionStatus.begin(); religionIt != pOldCity->m_ReligionStatus.end(); ++religionIt)
	{
		m_ReligionStatus.push_back(*religionIt);
	}
}

/// How many citizens here are following this religion?
int CvCityReligions::GetNumFollowers(ReligionTypes eReligion)
{
	ReligionInCityList::iterator religionIt;

	// Find the religion in the list
	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_eReligion == eReligion)
		{
			return religionIt->m_iFollowers;
		}
	}

	return 0;
}

/// Number of followers of this religion
int CvCityReligions::GetNumSimulatedFollowers(ReligionTypes eReligion)
{
	ReligionInCityList::iterator religionIt;

	// Find the religion in the list
	for(religionIt = m_SimulatedStatus.begin(); religionIt != m_SimulatedStatus.end(); ++religionIt)
	{
		if(religionIt->m_eReligion == eReligion)
		{
			return religionIt->m_iFollowers;
		}
	}

	return 0;
}

/// How many religions have at least 1 follower?
int CvCityReligions::GetNumReligionsWithFollowers()
{
	int iRtnValue = 0;
	ReligionInCityList::iterator religionIt;

	// Find the religion in the list
	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_iFollowers > 0 && religionIt->m_eReligion > RELIGION_PANTHEON)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

///Any religion in this city?
bool CvCityReligions::IsReligionInCity()
{
	ReligionInCityList::iterator religionIt;

	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_eReligion != NO_RELIGION)
		{
			return true;
		}
	}

	return false;
}

/// Is this the holy city for a specific religion?
bool CvCityReligions::IsHolyCityForReligion(ReligionTypes eReligion)
{
	ReligionInCityList::iterator religionIt;

	// Find the religion in the list
	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_eReligion == eReligion)
		{
			return religionIt->m_bFoundedHere;
		}
	}

	return false;
}

/// Is this the holy city for any religion?
bool CvCityReligions::IsHolyCityAnyReligion()
{
	ReligionInCityList::iterator religionIt;
	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_bFoundedHere)
		{
			return true;
		}
	}

	return false;
}
#if defined(MOD_BALANCE_CORE)
/// Is this the holy city for any religion?
ReligionTypes CvCityReligions::GetReligionForHolyCity()
{
	ReligionInCityList::iterator religionIt;
	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if(religionIt->m_bFoundedHere)
		{
			return religionIt->m_eReligion;
		}
	}

	return NO_RELIGION;
}
#endif
/// Is there a "heretical" religion here that can be stomped out?
bool CvCityReligions::IsReligionHereOtherThan(ReligionTypes eReligion)
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion != NO_RELIGION && it->m_eReligion != eReligion)
		{
			return true;
		}
	}
	return false;
}

/// Is there an inquisitor from another religion here?
bool CvCityReligions::IsDefendedAgainstSpread(ReligionTypes eReligion)
{
	CvUnit* pLoopUnit;

	CvPlot* pCityPlot = m_pCity->plot();
	if(pCityPlot)
	{
		for(int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
		{
			pLoopUnit = pCityPlot->getUnitByIndex(iUnitLoop);
			CvUnitEntry* pkEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
			if(pkEntry && pkEntry->IsProhibitsSpread())
			{
#if defined(MOD_RELIGION_ALLIED_INQUISITORS)
				bool bProtected = (pLoopUnit->getOwner() == m_pCity->getOwner() && pLoopUnit->GetReligionData()->GetReligion() != eReligion);
				if (!bProtected && MOD_RELIGION_ALLIED_INQUISITORS) {
					CvPlayer* pCityPlayer = &GET_PLAYER(m_pCity->getOwner());
					if (pCityPlayer->isMinorCiv() && pCityPlayer->GetMinorCivAI()->GetAlly() == pLoopUnit->getOwner()) {
						bProtected = true;
					}
				}
				
				if (bProtected)
#else
				if(pLoopUnit->getOwner() == m_pCity->getOwner() && pLoopUnit->GetReligionData()->GetReligion() != eReligion)
#endif
				{
					return true;
				}
			}
		}
	}

	CvPlot* pAdjacentPlot;
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
	{
		pAdjacentPlot = plotDirection(m_pCity->getX(), m_pCity->getY(), ((DirectionTypes)iDirectionLoop));

		if(pAdjacentPlot != NULL)
		{
			for(int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
			{
				pLoopUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
				CvUnitEntry* pkEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
				if(pkEntry && pkEntry->IsProhibitsSpread())
				{
#if defined(MOD_RELIGION_ALLIED_INQUISITORS)
					bool bProtected = (pLoopUnit->getOwner() == m_pCity->getOwner() && pLoopUnit->GetReligionData()->GetReligion() != eReligion);
					if (!bProtected && MOD_RELIGION_ALLIED_INQUISITORS) {
						CvPlayer* pCityPlayer = &GET_PLAYER(m_pCity->getOwner());
						if (pCityPlayer->isMinorCiv() && pCityPlayer->GetMinorCivAI()->GetAlly() == pLoopUnit->getOwner()) {
							bProtected = true;
						}
					}
					
					if (bProtected)
#else
					if(pLoopUnit->getOwner() == m_pCity->getOwner() && pLoopUnit->GetReligionData()->GetReligion() != eReligion)
#endif
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

#if defined(MOD_BALANCE_CORE)
ReligionTypes CvCityReligions::GetReligiousMajority()
{
	return m_majorityCityReligion;
}

bool CvCityReligions::ComputeReligiousMajority(bool bNotifications)
{
#else
/// Is there a religion that at least half of the population follows?
ReligionTypes CvCityReligions::GetReligiousMajority()
{
#endif
	int iTotalFollowers = 0;
	int iMostFollowerPressure = 0;
	int iMostFollowers = -1;
	ReligionTypes eMostFollowers = NO_RELIGION;
	ReligionInCityList::iterator religionIt;

	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		iTotalFollowers += religionIt->m_iFollowers;

		if(religionIt->m_iFollowers > iMostFollowers || religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostFollowerPressure)
		{
			iMostFollowers = religionIt->m_iFollowers;
			iMostFollowerPressure = religionIt->m_iPressure;
			eMostFollowers = religionIt->m_eReligion;
		}
	}

#if defined(MOD_BALANCE_CORE)
	//update local majority
	ReligionTypes oldMajority = m_majorityCityReligion;
	m_majorityCityReligion = (iMostFollowers*2 >= iTotalFollowers) ? eMostFollowers : NO_RELIGION;

	//update player majority
	if (m_majorityCityReligion!=oldMajority)
	{
		GET_PLAYER(m_pCity->getOwner()).GetReligions()->ComputeMajority(bNotifications);
	}
	return (m_majorityCityReligion!=NO_RELIGION);
#else
	if ((iMostFollowers * 2) >= iTotalFollowers)
	{
		return eMostFollowers;
	}
	else
	{
		return NO_RELIGION;
	}
#endif

}

/// Just asked to simulate a conversion - who would be the majority religion?
ReligionTypes CvCityReligions::GetSimulatedReligiousMajority()
{
	int iTotalFollowers = 0;
	int iMostFollowerPressure = 0;
	int iMostFollowers = -1;
	ReligionTypes eMostFollowers = NO_RELIGION;
	ReligionInCityList::iterator religionIt;

	for(religionIt = m_SimulatedStatus.begin(); religionIt != m_SimulatedStatus.end(); ++religionIt)
	{
		iTotalFollowers += religionIt->m_iFollowers;

		if(religionIt->m_iFollowers > iMostFollowers || religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostFollowerPressure)
		{
			iMostFollowers = religionIt->m_iFollowers;
			iMostFollowerPressure = religionIt->m_iPressure;
			eMostFollowers = religionIt->m_eReligion;
		}
	}

	if ((iMostFollowers * 2) >= iTotalFollowers)
	{
		return eMostFollowers;
	}
	else
	{
		return NO_RELIGION;
	}
}

/// What is the second most popular religion in this city with a majority religion?
ReligionTypes CvCityReligions::GetSecondaryReligion()
{
	int iMostFollowers = -1;
	int iMostPressure = -1;
	ReligionTypes eMajority = GetReligiousMajority();
	ReligionTypes eMostFollowers = NO_RELIGION;
	ReligionInCityList::iterator religionIt;

	if (eMajority != NO_RELIGION)
	{	
		for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
		{
			if (religionIt->m_eReligion != eMajority && religionIt->m_eReligion > RELIGION_PANTHEON && religionIt->m_iFollowers > 0)
			{
				if (religionIt->m_iFollowers > iMostFollowers)
				{
					iMostFollowers = religionIt->m_iFollowers;
					eMostFollowers = religionIt->m_eReligion;
					iMostPressure = religionIt->m_iPressure;
				}
				else if (religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostPressure)
				{
					iMostFollowers = religionIt->m_iFollowers;
					eMostFollowers = religionIt->m_eReligion;
					iMostPressure = religionIt->m_iPressure;
				}
			}
		}
	}

	return eMostFollowers;
}

/// Is there a pantheon belief in the secondary religion here?
BeliefTypes CvCityReligions::GetSecondaryReligionPantheonBelief()
{
	BeliefTypes eRtnValue = NO_BELIEF;

	// Check for the policy that allows a secondary religion to be active
	if (GET_PLAYER(m_pCity->getOwner()).IsSecondReligionPantheon())
	{
		ReligionTypes eSecondary = GetSecondaryReligion();
		if (eSecondary != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eSecondary, m_pCity->getOwner());
			if(pReligion)
			{
				for(int iI = 0; iI < pReligion->m_Beliefs.GetNumBeliefs(); iI++)
				{
					const BeliefTypes eBelief = pReligion->m_Beliefs.GetBelief(iI);
					CvBeliefEntry* pEntry = GC.GetGameBeliefs()->GetEntry((int)eBelief);
					if(pEntry && pEntry->IsPantheonBelief())
					{
						eRtnValue = eBelief;
						break;
					}
				}
			}
		}
	}

	return eRtnValue;
}

/// How many followers are there of religions OTHER than this one?
int CvCityReligions::GetFollowersOtherReligions(ReligionTypes eReligion)
{
	int iOtherFollowers = 0;
	ReligionInCityList::iterator religionIt;

	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if (religionIt->m_eReligion > RELIGION_PANTHEON && religionIt->m_eReligion != eReligion)
		{
			iOtherFollowers += religionIt->m_iFollowers;
		}
	}

	return iOtherFollowers;
}

/// Total pressure exerted by all religions
int CvCityReligions::GetTotalPressure()
{
	int iTotalPressure = 0;

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		iTotalPressure += it->m_iPressure;
	}

	return iTotalPressure;
}

/// Pressure exerted by one religion
int CvCityReligions::GetPressure(ReligionTypes eReligion)
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			return it->m_iPressure;
		}
	}

	return 0;
}

/// Pressure exerted by one religion per turn
int CvCityReligions::GetPressurePerTurn(ReligionTypes eReligion, int& iNumTradeRoutesInvolved)
{
	int iPressure = 0;
	iNumTradeRoutesInvolved = 0;
	
	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Ignore the same city
				if (m_pCity == pLoopCity)
				{
					continue;
				}

				// Does this city have the matching majority religion?
				ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
				if(eMajorityReligion == eReligion)
				{
					int iNumTradeRoutes;
					iPressure += GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure (eMajorityReligion, pLoopCity, m_pCity, iNumTradeRoutes, true, false);
					iNumTradeRoutesInvolved += iNumTradeRoutes;
				}
			}

#if defined(MOD_BUGFIX_RELIGIOUS_SPY_PRESSURE)
			// Include any pressure from "Underground Sects"
			if (eReligion > RELIGION_PANTHEON && (kPlayer.GetReligions()->GetReligionInMostCities() == eReligion || GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID()) == eReligion))
			{
				int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
				if (iSpyPressure > 0)
				{
					if (kPlayer.GetEspionage()->GetSpyIndexInCity(m_pCity) != -1)
					{
						if (GetNumFollowers(eReligion) != 0)
						{
							iPressure += iSpyPressure * GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
						}
					}
				}
			}
#endif
		}
	}

	// Holy city for this religion?
	if (IsHolyCityForReligion(eReligion))
	{
		int iHolyCityPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
		iHolyCityPressure *=  GC.getRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE();
		iPressure += iHolyCityPressure;
	}
	
	// CUSTOMLOG("GetPressurePerTurn for %i on %s is %i", eReligion, m_pCity->getName().c_str(), iPressure);
	return iPressure;
}

/// How many trade routes are applying pressure to this city
int CvCityReligions::GetNumTradeRouteConnections (ReligionTypes eReligion)
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			return it->m_iNumTradeRoutesApplyingPressure;
		}
	}

	return 0;
}

/// Would this city exert religious pressure toward the target city if connected with a trade route
bool CvCityReligions::WouldExertTradeRoutePressureToward (CvCity* pTargetCity, ReligionTypes& eReligion, int& iAmount)
{
	eReligion = GetReligiousMajority();

	// if there isn't a religious connection, whatvz
	if (eReligion == NO_RELIGION)
	{
		iAmount = 0;
		return false;
	}

	int iNumTradeRoutes = 0;
	int iWithTR = GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure(eReligion, m_pCity, pTargetCity, iNumTradeRoutes, false, true);
	int iNoTR = GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure(eReligion, m_pCity, pTargetCity, iNumTradeRoutes, false, false);

	iAmount = (iWithTR-iNoTR);
	return (iAmount>0);
}


/// Handle a change in the city population
void CvCityReligions::DoPopulationChange(int iChange)
{
	ReligionTypes eMajorityReligion = GetReligiousMajority();

	// Only add pressure if the population went up; if starving, leave pressure alone (but recompute followers)
	if(iChange > 0)
	{
		AddReligiousPressure(FOLLOWER_CHANGE_POP_CHANGE, eMajorityReligion, iChange * GC.getRELIGION_ATHEISM_PRESSURE_PER_POP());
	}
	else if (iChange < 0)
	{
		RecomputeFollowers(FOLLOWER_CHANGE_POP_CHANGE, eMajorityReligion);
	}
#if defined(MOD_BALANCE_CORE)
	m_pCity->GetCityCitizens()->SetDirty(true);
#endif
}

/// Note that a religion was founded here
void CvCityReligions::DoReligionFounded(ReligionTypes eReligion)
{
	int iInitialPressure;
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	iInitialPressure = GC.getRELIGION_INITIAL_FOUNDING_CITY_PRESSURE() * m_pCity->getPopulation();
	CvReligionInCity newReligion(eReligion, true, 0, iInitialPressure);
	m_ReligionStatus.push_back(newReligion);

	RecomputeFollowers(FOLLOWER_CHANGE_RELIGION_FOUNDED, eOldMajorityReligion);
}

/// Prophet spread is very powerful: eliminates all existing religions and adds to his
void CvCityReligions::AddProphetSpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer)
{
	int iAtheismPressure = 0;
	int iReligionPressure = 0;
	int iPressureRetained = 0;
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();
	ReligionTypes eHolyCityReligion = NO_RELIGION;
	ReligionTypes ePressureRetainedReligion = NO_RELIGION;
	bool bProphetsReligionFoundedHere = false;

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if (it->m_eReligion == NO_RELIGION)
		{
			iAtheismPressure = it->m_iPressure;
		}
		else if (eReligion == it->m_eReligion)
		{
			iReligionPressure = it->m_iPressure;
			if (it->m_bFoundedHere)
			{
				bProphetsReligionFoundedHere = true;
			}
		}
		else if (it->m_bFoundedHere)
		{
			eHolyCityReligion = it->m_eReligion;
		}

		if (it->m_eReligion > RELIGION_PANTHEON &&  it->m_eReligion != eReligion)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(it->m_eReligion, NO_PLAYER);
			if(pReligion)
			{
				if(eResponsiblePlayer != NO_PLAYER)
				{
					int iPressureRetention = pReligion->m_Beliefs.GetInquisitorPressureRetention(eResponsiblePlayer);  // Normally 0
					if (iPressureRetention > 0)
					{
						ePressureRetainedReligion = it->m_eReligion;
						iPressureRetained = it->m_iPressure * iPressureRetention / 100;
					}
				}
				else
				{
					int iPressureRetention = pReligion->m_Beliefs.GetInquisitorPressureRetention(eResponsiblePlayer);  // Normally 0
					if (iPressureRetention > 0)
					{
						ePressureRetainedReligion = it->m_eReligion;
						iPressureRetained = it->m_iPressure * iPressureRetention / 100;
					}
				}
			}
		}
	}

	// Clear list
	m_ReligionStatus.clear();

	// Add atheists and this back in
	CvReligionInCity atheism(NO_RELIGION, false/*bFoundedHere*/, 0, iAtheismPressure);
	m_ReligionStatus.push_back(atheism);
	CvReligionInCity prophetReligion(eReligion, bProphetsReligionFoundedHere, 0, iReligionPressure + iPressure);
	m_ReligionStatus.push_back(prophetReligion);

	// Reestablish Holy City religion
	if (eHolyCityReligion != NO_RELIGION && !bProphetsReligionFoundedHere)
	{
		if (eHolyCityReligion == ePressureRetainedReligion)
		{
			CvReligionInCity holyCityReligion(eHolyCityReligion, true/*bFoundedHere*/, 0, iPressureRetained);
			m_ReligionStatus.push_back(holyCityReligion);			
		}
		else
		{
			CvReligionInCity holyCityReligion(eHolyCityReligion, true/*bFoundedHere*/, 0, 0);
			m_ReligionStatus.push_back(holyCityReligion);
		}
	}

	// Reestablish pressure-retained religion (if wasn't Holy City religion)
	if (ePressureRetainedReligion != NO_RELIGION && eHolyCityReligion != ePressureRetainedReligion)
	{
		CvReligionInCity pressureRetainedReligion(ePressureRetainedReligion, false/*bFoundedHere*/, 0, iPressureRetained);
		m_ReligionStatus.push_back(pressureRetainedReligion);
	}

	RecomputeFollowers(FOLLOWER_CHANGE_PROPHET, eOldMajorityReligion, eResponsiblePlayer);
}

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer)
{
	bool bFoundIt = false;

	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			it->m_iPressure += iPressure;
			bFoundIt = true;
		}

		//  If this is pressure from a real religion, reduce presence of pantheon by the same amount
		else if(eReligion > RELIGION_PANTHEON && it->m_eReligion == RELIGION_PANTHEON)
		{
			it->m_iPressure = max(0, (it->m_iPressure - iPressure));
		}

		else if (it->m_eReligion > RELIGION_PANTHEON && eReason == FOLLOWER_CHANGE_MISSIONARY)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			if(eResponsiblePlayer != NO_PLAYER)
			{
				int iPressureErosion = pReligion->m_Beliefs.GetOtherReligionPressureErosion(eResponsiblePlayer);  // Normally 0
				if (iPressureErosion > 0)
				{
					int iErosionAmount = iPressureErosion * iPressure / 100;
					it->m_iPressure = max(0, (it->m_iPressure - iErosionAmount));
				}
			}
			else
			{
				int iPressureErosion = pReligion->m_Beliefs.GetOtherReligionPressureErosion();  // Normally 0
				if (iPressureErosion > 0)
				{
					int iErosionAmount = iPressureErosion * iPressure / 100;
					it->m_iPressure = max(0, (it->m_iPressure - iErosionAmount));
				}
			}
		}
	}

	// Didn't find it, add new entry
	if(!bFoundIt)
	{
		CvReligionInCity newReligion(eReligion, false, 0, iPressure);
		m_ReligionStatus.push_back(newReligion);
	}

	RecomputeFollowers(eReason, eOldMajorityReligion, eResponsiblePlayer);
}

/// Simulate prophet spread
void CvCityReligions::SimulateProphetSpread(ReligionTypes eReligion, int iPressure)
{
	int iAtheismPressure = 0;
	int iReligionPressure = 0;
	int iPressureRetained = 0;
	ReligionTypes ePressureRetainedReligion = NO_RELIGION;

	CopyToSimulatedStatus();

	ReligionInCityList::iterator it;
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		if(it->m_eReligion == NO_RELIGION)
		{
			iAtheismPressure = it->m_iPressure;
		}
		else if(eReligion == it->m_eReligion)
		{
			iReligionPressure = it->m_iPressure;
		}

		if (it->m_eReligion > RELIGION_PANTHEON && eReligion != it->m_eReligion)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(it->m_eReligion, NO_PLAYER);
			if(pReligion)
			{
				int iPressureRetention = pReligion->m_Beliefs.GetInquisitorPressureRetention();  // Normally 0
				if (iPressureRetention > 0)
				{
					ePressureRetainedReligion = it->m_eReligion;
					iPressureRetained = it->m_iPressure * iPressureRetention / 100;
				}
			}
		}
	}

	// Clear list
	m_SimulatedStatus.clear();

	// Add atheists and this back in
	CvReligionInCity atheism(NO_RELIGION, false, 0, iAtheismPressure);
	m_SimulatedStatus.push_back(atheism);
	CvReligionInCity prophetReligion(eReligion, false, 0, iReligionPressure + iPressure);
	m_SimulatedStatus.push_back(prophetReligion);

	if (ePressureRetainedReligion != NO_RELIGION)
	{
		CvReligionInCity pressureRetainedReligion(ePressureRetainedReligion, false, 0, iPressureRetained);
		m_SimulatedStatus.push_back(pressureRetainedReligion);

	}

	SimulateFollowers();
}

/// Simulate religious pressure addition
void CvCityReligions::SimulateReligiousPressure(ReligionTypes eReligion, int iPressure)
{
	bool bFoundIt = false;

	CopyToSimulatedStatus();

#if defined(MOD_BALANCE_CORE)
	if(eReligion == NO_RELIGION)
	{
		return;
	}
#endif

	ReligionInCityList::iterator it;
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			it->m_iPressure += iPressure;
			bFoundIt = true;
		}

		//  If this is pressure from a real religion, reduce presence of pantheon by the same amount
		else if(eReligion > RELIGION_PANTHEON && it->m_eReligion == RELIGION_PANTHEON)
		{
			it->m_iPressure = max(0, (it->m_iPressure - iPressure));
		}

		else if (it->m_eReligion > RELIGION_PANTHEON)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
			int iPressureErosion = pReligion->m_Beliefs.GetOtherReligionPressureErosion();  // Normally 0
			if (iPressureErosion > 0)
			{
				int iErosionAmount = iPressureErosion * iPressure / 100;
				it->m_iPressure = max(0, (it->m_iPressure - iErosionAmount));
			}
		}
	}

	// Didn't find it, add new entry
	if(!bFoundIt)
	{
		CvReligionInCity newReligion(eReligion, false, 0, iPressure);
		m_SimulatedStatus.push_back(newReligion);
	}

	SimulateFollowers();
}

/// Convert some percentage of followers from one religion to another
void CvCityReligions::ConvertPercentFollowers(ReligionTypes eToReligion, ReligionTypes eFromReligion, int iPercent)
{
	int iPressureConverting = 0;

	// Find old religion
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eFromReligion)
		{
			iPressureConverting = it->m_iPressure * iPercent / 100;
			it->m_iPressure -= iPressureConverting;
			if (it->m_iPressure < 0)
			{
				it->m_iPressure = 0;
			}
		}
	}
	AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPressureConverting, NO_PLAYER);
}
#if defined(MOD_BALANCE_CORE)
/// Convert some percentage of followers from one religion to another
void CvCityReligions::ConvertPercentForcedFollowers(ReligionTypes eToReligion, int iPercent)
{
	int iPressureConverting = 0;

	// Find religion
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		//Do for every religion in City, as we're converting x% of all citizens. 
		if(it->m_iFollowers > 0)
		{
			iPressureConverting = it->m_iPressure * iPercent / 100;
			it->m_iPressure -= iPressureConverting;
			if (it->m_iPressure < 0)
			{
				it->m_iPressure = 0;
			}
		}
	}
	AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPressureConverting, NO_PLAYER);
}
/// Convert some number of followers from one religion to another
void CvCityReligions::ConvertNumberFollowers(ReligionTypes eToReligion, int iPop)
{
	int iPressureConverting = 0;

	iPop *= 100;
	int iTotalPop = m_pCity->getPopulation();
	iPop /= iTotalPop;

	if(iPop > 100)
	{
		iPop = 100;
	}

	// Find religion
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		//Do for every religion in City, as we're converting x% of all citizens. 
		if(it->m_iFollowers > 0)
		{
			iPressureConverting = it->m_iPressure * iPop / 100;
			it->m_iPressure -= iPressureConverting;
			if (it->m_iPressure < 0)
			{
				it->m_iPressure = 0;
			}
		}
	}
	AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPressureConverting, NO_PLAYER);
}
#endif

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddHolyCityPressure()
{
	bool bRecompute = false;
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_bFoundedHere)
		{
			int iPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
			iPressure *=  GC.getRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE();
			it->m_iPressure += iPressure;

			// Found it, so we're done
			bRecompute = true;
		}
	}

	// Didn't find it, add new entry
	if(bRecompute)
	{
		RecomputeFollowers(FOLLOWER_CHANGE_HOLY_CITY, eOldMajorityReligion);
	}
}

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddSpyPressure(ReligionTypes eReligion, int iBasePressure)
{
	bool bRecompute = false;
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			int iPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
			iPressure *= iBasePressure;
			it->m_iPressure += iPressure;

			// Found it, so we're done
			bRecompute = true;
		}
	}

	// Didn't find it, add new entry
	if(bRecompute)
	{
		RecomputeFollowers(FOLLOWER_CHANGE_SPY_PRESSURE, eOldMajorityReligion);
	}
}

/// Set this city to have all citizens following a religion (mainly for scripting)
void CvCityReligions::AdoptReligionFully(ReligionTypes eReligion)
{
	m_ReligionStatus.clear();

	CvReligionInCity religion;

	// Add 1 pop of Atheism (needed in case other religions wiped out by an Inquisitor/Prophet
	religion.m_bFoundedHere = false;
	religion.m_eReligion = NO_RELIGION;
	religion.m_iFollowers = 1;
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS) || defined(MOD_RELIGION_LOCAL_RELIGIONS)
	// This needs to be less than the pressure in a city with a pop of 1
	religion.m_iPressure = religion.m_iFollowers * GC.getRELIGION_ATHEISM_PRESSURE_PER_POP() - 1;
#else
	religion.m_iPressure = religion.m_iFollowers * GC.getRELIGION_ATHEISM_PRESSURE_PER_POP();
#endif
	m_ReligionStatus.push_back(religion);

	// Now add full pop of this religion
	religion.m_bFoundedHere = false;
	religion.m_eReligion = eReligion;
	religion.m_iFollowers = m_pCity->getPopulation();
	religion.m_iPressure = religion.m_iFollowers * GC.getRELIGION_ATHEISM_PRESSURE_PER_POP();
	m_ReligionStatus.push_back(religion);

#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	if (MOD_GLOBAL_RELIGIOUS_SETTLERS) {
		RecomputeFollowers(FOLLOWER_CHANGE_ADOPT_FULLY, NO_RELIGION);
	}
#endif
	m_pCity->UpdateReligion(eReligion);
}

/// Remove presence of old owner's pantheon (used when a city is conquered)
void CvCityReligions::RemoveFormerPantheon()
{
	bool bFoundIt = false;
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end() && !bFoundIt; it++)
	{
		if(it->m_eReligion == RELIGION_PANTHEON)
		{
			m_ReligionStatus.erase(it);

			// Found it, so we're done
			bFoundIt = true;
		}
	}

	RecomputeFollowers(FOLLOWER_CHANGE_CONQUEST, eOldMajorityReligion);
}

/// Remove other religions in a city (used by Inquisitor)
void CvCityReligions::RemoveOtherReligions(ReligionTypes eReligion, PlayerTypes eResponsiblePlayer)
{
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();

	// Copy list
	ReligionInCityList tempList;
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		tempList.push_back(*it);
	}

	// Erase old list
	m_ReligionStatus.clear();

	// Recopy just what we want to keep
	for(it = tempList.begin(); it != tempList.end(); it++)
	{
		int iPressureRetained = 0;

		ReligionTypes eLoopReligion = it->m_eReligion;
		if (eLoopReligion > RELIGION_PANTHEON && eLoopReligion != eReligion)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(eLoopReligion, NO_PLAYER);
			iPressureRetained = pReligion->m_Beliefs.GetInquisitorPressureRetention();  // Normally 0
		}

		if (eLoopReligion == NO_RELIGION || eLoopReligion == eReligion || iPressureRetained > 0)
		{
			if (iPressureRetained > 0)
			{
				it->m_iPressure = it->m_iPressure * iPressureRetained / 100;
			}

			m_ReligionStatus.push_back(*it);
		}
	}

	RecomputeFollowers(FOLLOWER_CHANGE_REMOVE_HERESY, eOldMajorityReligion, eResponsiblePlayer);
}

/// Called from the trade system when a trade connection is made between two cities
void CvCityReligions::UpdateNumTradeRouteConnections(CvCity* pOtherCity)
{
	ReligionTypes eReligiousMajority = GetReligiousMajority();

	// if there isn't a religious connection, whatvz
	if (eReligiousMajority == NO_RELIGION)
	{
		return;
	}

	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligiousMajority, m_pCity->getOwner());
	if (!pReligion)
	{
		return;
	}

	// Are the cities within the minimum distance?
	int iDistance = GC.getRELIGION_ADJACENT_CITY_DISTANCE();

	// Boost to distance due to belief?
	int iDistanceMod = pReligion->m_Beliefs.GetSpreadDistanceModifier(m_pCity->getOwner());
	if(iDistanceMod > 0)
	{
		iDistance *= (100 + iDistanceMod);
		iDistance /= 100;
	}

	bool bWithinDistance = (plotDistance(m_pCity->getX(), m_pCity->getY(), pOtherCity->getX(), pOtherCity->getY()) <= iDistance);

	// if not within distance, then we're using a trade route
	if (!bWithinDistance) 
	{
		pOtherCity->GetCityReligions()->IncrementNumTradeRouteConnections(eReligiousMajority, 1);
	}
}

/// Increment the number of trade connections a city has
void CvCityReligions::IncrementNumTradeRouteConnections(ReligionTypes eReligion, int iNum)
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			it->m_iNumTradeRoutesApplyingPressure += iNum;
			break;
		}
	}
}

/// How many followers would we have adding this religious pressure here?
int CvCityReligions::GetNumFollowersAfterSpread(ReligionTypes eReligion, int iConversionStrength)
{
	SimulateReligiousPressure(eReligion, iConversionStrength);
	return GetNumSimulatedFollowers(eReligion);
}

/// How many followers would we have having a prophet add religious pressure here?
int CvCityReligions::GetNumFollowersAfterProphetSpread(ReligionTypes eReligion, int iConversionStrength)
{
	SimulateProphetSpread(eReligion, iConversionStrength);
	return GetNumSimulatedFollowers(eReligion);
}

/// What would the majority religion be adding this religious pressure here?
ReligionTypes CvCityReligions::GetMajorityReligionAfterSpread(ReligionTypes eReligion, int iConversionStrength)
{
	SimulateReligiousPressure(eReligion, iConversionStrength);
	return GetSimulatedReligiousMajority();
}

/// What would the majority religion be adding this religious pressure here?
ReligionTypes CvCityReligions::GetMajorityReligionAfterProphetSpread(ReligionTypes eReligion, int iConversionStrength)
{
	SimulateProphetSpread(eReligion, iConversionStrength);
	return GetSimulatedReligiousMajority();
}

/// Resets the number of trade routes pressuring a city
void CvCityReligions::ResetNumTradeRoutePressure()
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		it->m_iNumTradeRoutesApplyingPressure = 0;
	}
}

// PRIVATE METHODS

/// Calculate the number of followers for each religion
void CvCityReligions::RecomputeFollowers(CvReligiousFollowChangeReason eReason, ReligionTypes eOldMajorityReligion, PlayerTypes eResponsibleParty)
{
	int iOldFollowers = GetNumFollowers(eOldMajorityReligion);
	int iUnassignedFollowers = m_pCity->getPopulation();
	int iPressurePerFollower;

	// Safety check to avoid divide by zero
	if (iUnassignedFollowers < 1)
	{
		CvAssertMsg (false, "Invalid city population when recomputing followers");
		return;
	}
	// Find total pressure
	int iTotalPressure = 0;
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		iTotalPressure += it->m_iPressure;
	}

	// safety check - if pressure was wiped out somehow, just rebuild pressure of 1 atheist
	if (iTotalPressure <= 0)
	{
		m_ReligionStatus.clear();

		CvReligionInCity religion;
		religion.m_bFoundedHere = false;
		religion.m_eReligion = NO_RELIGION;
#if defined(MOD_BALANCE_CORE)
		if(eReason == FOLLOWER_CHANGE_ADOPT_FULLY)
		{
			religion.m_iFollowers = 0;
			religion.m_iPressure = 0;
		}
		else
		{
#endif
		religion.m_iFollowers = 1;
		religion.m_iPressure = GC.getRELIGION_ATHEISM_PRESSURE_PER_POP();
#if defined(MOD_BALANCE_CORE)
		}
#endif
		m_ReligionStatus.push_back(religion);

		iTotalPressure = GC.getRELIGION_ATHEISM_PRESSURE_PER_POP();
	}

	iPressurePerFollower = iTotalPressure / iUnassignedFollowers;

	// Loop through each religion
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		it->m_iFollowers = it->m_iPressure / iPressurePerFollower;
		iUnassignedFollowers -= it->m_iFollowers;
		it->m_iTemp = it->m_iPressure - (it->m_iFollowers * iPressurePerFollower);  // Remainder
	}

	// Assign out the remainder
	for (int iI = 0; iI < iUnassignedFollowers; iI++)
	{
		ReligionInCityList::iterator itLargestRemainder = NULL;
		int iLargestRemainder = 0;

		for (it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
		{
			if (it->m_iTemp > iLargestRemainder)
			{
				iLargestRemainder = it->m_iTemp;
				itLargestRemainder = it;
			}
		}

		if (itLargestRemainder && iLargestRemainder > 0)
		{
			itLargestRemainder->m_iFollowers++;
			itLargestRemainder->m_iTemp = 0;
		}
	}

#if defined(MOD_BALANCE_CORE)
	ComputeReligiousMajority(true);
#endif

	ReligionTypes eMajority = GetReligiousMajority();
	int iFollowers = GetNumFollowers(eMajority);

	if(eMajority != eOldMajorityReligion || iFollowers != iOldFollowers)
	{
		CityConvertsReligion(eMajority, eOldMajorityReligion, eResponsibleParty);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		LogFollowersChange(eReason);
	}
}

/// Calculate the number of followers for each religion from simulated data
void CvCityReligions::SimulateFollowers()
{
	int iUnassignedFollowers = m_pCity->getPopulation();
	int iPressurePerFollower;

	// Find total pressure
	int iTotalPressure = 0;
	ReligionInCityList::iterator it;
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		iTotalPressure += it->m_iPressure;
	}

	// safety check
	if (iTotalPressure == 0 || iUnassignedFollowers == 0)
	{
		CvAssertMsg(false, "Internal religion data error. Send save to Ed");
		return;
	}

	iPressurePerFollower = iTotalPressure / iUnassignedFollowers;

	// Loop through each religion
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		it->m_iFollowers = it->m_iPressure / iPressurePerFollower;
		iUnassignedFollowers -= it->m_iFollowers;
		it->m_iTemp = it->m_iPressure - (it->m_iFollowers * iPressurePerFollower);  // Remainder
	}

	// Assign out the remainder
	for (int iI = 0; iI < iUnassignedFollowers; iI++)
	{
		ReligionInCityList::iterator itLargestRemainder = NULL;
		int iLargestRemainder = 0;

		for (it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
		{
			if (it->m_iTemp > iLargestRemainder)
			{
				iLargestRemainder = it->m_iTemp;
				itLargestRemainder = it;
			}
		}

		if (itLargestRemainder && iLargestRemainder > 0)
		{
			itLargestRemainder->m_iFollowers++;
			itLargestRemainder->m_iTemp = 0;
		}
	}
}

/// Copy data to prepare to simulate a conversion
void CvCityReligions::CopyToSimulatedStatus()
{
	m_SimulatedStatus.clear();

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		m_SimulatedStatus.push_back(*it);
	}
}

/// Implement changes from a city changing religion
void CvCityReligions::CityConvertsReligion(ReligionTypes eMajority, ReligionTypes eOldMajority, PlayerTypes eResponsibleParty)
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	m_pCity->UpdateReligion(eMajority);

	if(eOldMajority > RELIGION_PANTHEON)
	{
		const CvReligion* pOldReligion = pReligions->GetReligion(eOldMajority, NO_PLAYER);
		GET_PLAYER(pOldReligion->m_eFounder).UpdateReligion();
	}

	if(eMajority > RELIGION_PANTHEON)
	{
		const CvReligion* pNewReligion = pReligions->GetReligion(eMajority, NO_PLAYER);
#if defined(MOD_BALANCE_CORE)
		CvCity* pHolyCity = NULL;
		PlayerTypes eReligionController = NO_PLAYER;
		CvPlot* pkPlot = GC.getMap().plot(pNewReligion->m_iHolyCityX, pNewReligion->m_iHolyCityY);
		if(pkPlot != NULL)
		{
			pHolyCity = pkPlot->getPlotCity();
		}
		if(pHolyCity != NULL)
		{
			eReligionController = pHolyCity->getOwner();
		}
#endif
		GET_PLAYER(pNewReligion->m_eFounder).UpdateReligion();

		// Pay adoption bonuses (if any)
#if defined(MOD_BALANCE_CORE)
		if(!m_pCity->HasPaidAdoptionBonus(eMajority))
#else
		if(!m_bHasPaidAdoptionBonus)
#endif
		{
			int iGoldBonus = 0;
			if(eResponsibleParty != NO_PLAYER)
			{
				iGoldBonus = pNewReligion->m_Beliefs.GetGoldWhenCityAdopts(eResponsibleParty);
				iGoldBonus *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iGoldBonus /= 100;
			}
			else
			{
				iGoldBonus = pNewReligion->m_Beliefs.GetGoldWhenCityAdopts();
				iGoldBonus *= GC.getGame().getGameSpeedInfo().getTrainPercent();
				iGoldBonus /= 100;
			}

			if(iGoldBonus > 0)
			{
				GET_PLAYER(pNewReligion->m_eFounder).GetTreasury()->ChangeGold(iGoldBonus);
#if defined(MOD_BALANCE_CORE)
				m_pCity->SetPaidAdoptionBonus(eMajority, true);
#else
				SetPaidAdoptionBonus(true);
#endif

				if(pNewReligion->m_eFounder == GC.getGame().getActivePlayer())
				{
					char text[256] = {0};
					sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGoldBonus);
#if defined(SHOW_PLOT_POPUP)
					SHOW_PLOT_POPUP(m_pCity->plot(), NO_PLAYER, text, 0.5f);
#else
					GC.GetEngineUserInterface()->AddPopupText(m_pCity->getX(), m_pCity->getY(), text, 0.5f);
#endif
				}
			}
#if defined(MOD_BALANCE_CORE_BELIEFS)
			if(eReligionController != NO_PLAYER && GET_PLAYER(eReligionController).GetReligions()->GetCurrentReligion() == eMajority)
			{
				GET_PLAYER(eReligionController).doInstantYield(INSTANT_YIELD_TYPE_CONVERSION, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pHolyCity);
				for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					YieldTypes eYield = (YieldTypes)iI;
					if(eYield == NO_YIELD)
						continue;

					int iValue = pNewReligion->m_Beliefs.GetYieldFromConversion(eYield, eReligionController, pHolyCity);
					if(iValue > 0)
					{
						m_pCity->SetPaidAdoptionBonus(eMajority, true);
						break;
					}
				}
			}
#endif
		}

		// Notification if the player's city was converted to a religion they didn't found
		PlayerTypes eOwnerPlayer = m_pCity->getOwner();
		CvPlayerAI& kOwnerPlayer = GET_PLAYER(eOwnerPlayer);
		const ReligionTypes eOwnerPlayerReligion = kOwnerPlayer.GetReligions()->GetReligionCreatedByPlayer();
#if defined(MOD_BALANCE_CORE)
		if(eOwnerPlayer != eResponsibleParty && eMajority != eOldMajority && eReligionController != eOwnerPlayer && eOwnerPlayerReligion > RELIGION_PANTHEON)
#else
		if(eOwnerPlayer != eResponsibleParty && eMajority != eOldMajority && pNewReligion->m_eFounder != eOwnerPlayer
			&& eOwnerPlayerReligion > RELIGION_PANTHEON)
#endif
		{
			if(kOwnerPlayer.GetNotifications())
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_ACTIVE_PLAYER", m_pCity->getName());
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_ACTIVE_PLAYER_S");
#if defined(MOD_API_EXTENSIONS)
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), eMajority, -1);
#else
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), -1);
#endif
			}

#if !defined(NO_ACHIEVEMENTS)
			//Achievements!
			if(eOwnerPlayer == GC.getGame().getActivePlayer()){
				const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eOwnerPlayerReligion, eOwnerPlayer);
				if(pkReligion != NULL)
				{
					if(m_pCity->getX() == pkReligion->m_iHolyCityX && m_pCity->getY() == pkReligion->m_iHolyCityY)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_20);
					}
				}
			}
#endif
		}

		else if(eOwnerPlayer != eResponsibleParty && eMajority != eOldMajority && eOldMajority == NO_RELIGION)
		{
			if(kOwnerPlayer.GetNotifications())
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_INITIAL_CONVERSION", m_pCity->getName());
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_INITIAL_CONVERSION_S");
#if defined(MOD_API_EXTENSIONS)
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD_NATURAL, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), eMajority, -1);
#else
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD_NATURAL, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), -1);
#endif
			}
		}

#if !defined(NO_ACHIEVEMENTS)
		//More Achievements
		if(m_pCity->getOwner() != GC.getGame().getActivePlayer() && pNewReligion->m_eFounder == GC.getGame().getActivePlayer())
		{
			if(m_pCity->GetCityReligions()->IsHolyCityAnyReligion() && !m_pCity->GetCityReligions()->IsHolyCityForReligion(pNewReligion->m_eReligion))
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_18);
			}
		}

		if(m_pCity->isCapital() && pNewReligion->m_eFounder == GC.getGame().getActivePlayer())
		{
			//Determine if this is a standard size or larger map.
			bool bIsStandardOrLarger = false;
			Database::Connection* pDB = GC.GetGameDatabase();
			Database::Results kStandardSize;
			if(pDB->SelectAt(kStandardSize, "Worlds", "Type", "WORLDSIZE_STANDARD"))
			{
				if(kStandardSize.Step())
				{
					int idColumn = kStandardSize.ColumnPosition("ID");
					if(idColumn >= 0)
					{
						WorldSizeTypes eWorldSize = GC.getMap().getWorldSize();
						int standardWorldSize = kStandardSize.GetInt(idColumn);
						if(eWorldSize >= standardWorldSize)
						{
							bIsStandardOrLarger = true;
						}
					}
				}
			}

			if(bIsStandardOrLarger)
			{
				//Determine if this religion has spread to all capitals
				bool bSpreadToAllCapitals = true;
				for(int i = 0; i < MAX_MAJOR_CIVS; ++i)
				{
					CvPlayerAI& kPlayer = GET_PLAYER(static_cast<PlayerTypes>(i));
					if(kPlayer.isAlive())
					{
						CvCity* pCapital = kPlayer.getCapitalCity();
						if(pCapital != NULL)
						{
							CvCityReligions* pCityReligions = pCapital->GetCityReligions();
							if(pCityReligions != NULL)
							{
								if(pCityReligions->GetReligiousMajority() != pNewReligion->m_eFounder)
								{
									bSpreadToAllCapitals = false;
									break;
								}
							}
						}
					}

					if(bSpreadToAllCapitals)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_19);
					}
				}
			}
		}
#endif

		// Diplo implications (there must have been religion switch and a responsible party)
		if(eMajority != eOldMajority && eResponsibleParty != NO_PLAYER)
		{
			// Is the city owner not the founder of this religion?
			if(pNewReligion->m_eFounder != m_pCity->getOwner())
			{
				CvPlayer& kCityOwnerPlayer = GET_PLAYER(m_pCity->getOwner());

				// Did he found another religion?
				ReligionTypes eCityOwnerReligion = kCityOwnerPlayer.GetReligions()->GetReligionCreatedByPlayer();
				if(eCityOwnerReligion >= RELIGION_PANTHEON)
				{
					int iPoints = 0;

					// His religion wasn't present here, minor hit
					if(eOldMajority != eCityOwnerReligion)
					{
						iPoints = GC.getRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY(); /*1*/
					}

					// This was his holy city; huge hit!
					else if(m_pCity->GetCityReligions()->IsHolyCityForReligion(eCityOwnerReligion))
					{
						iPoints = GC.getRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY(); /*25*/
					}

					// He had established his religion here, major hit
					else
					{
						iPoints = GC.getRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY(); /*3*/
					}

					kCityOwnerPlayer.GetDiplomacyAI()->ChangeNegativeReligiousConversionPoints(eResponsibleParty, iPoints);
#if defined(MOD_BALANCE_CORE)
					kCityOwnerPlayer.GetDiplomacyAI()->SetReligiousConversionTurn(eResponsibleParty, GC.getGame().getGameTurn());
#endif
				}
			}
		}

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pCity->getOwner());
			args->Push(eMajority);
			args->Push(m_pCity->getX());
			args->Push(m_pCity->getY());

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "CityConvertsReligion", args.get(), bResult);
		}
	}
}

/// Log a message with status information
void CvCityReligions::LogFollowersChange(CvReligiousFollowChangeReason eReason)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strReasonString;
		CvString temp;
		FILogFile* pLog;
		CvCityReligions* pCityRel = m_pCity->GetCityReligions();

		pLog = LOGFILEMGR.GetLog(GC.getGame().GetGameReligions()->GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strOutBuf.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strOutBuf += m_pCity->getName();
		strOutBuf += ", ";

		// Add a reason string
		switch(eReason)
		{
		case FOLLOWER_CHANGE_ADJACENT_PRESSURE:
			strReasonString = "Adjacent city pressure";
			break;
		case FOLLOWER_CHANGE_HOLY_CITY:
			strReasonString = "Holy city pressure";
			break;
		case FOLLOWER_CHANGE_POP_CHANGE:
			strReasonString = "Population change";
			break;
		case FOLLOWER_CHANGE_RELIGION_FOUNDED:
			strReasonString = "Religion founded";
			break;
		case FOLLOWER_CHANGE_PANTHEON_FOUNDED:
			strReasonString = "Pantheon founded";
			break;
		case FOLLOWER_CHANGE_CONQUEST:
			strReasonString = "City captured";
			break;
		case FOLLOWER_CHANGE_MISSIONARY:
			strReasonString = "Missionary expended";
			break;
		case FOLLOWER_CHANGE_PROPHET:
			strReasonString = "Prophet spreading";
			break;
		case FOLLOWER_CHANGE_REMOVE_HERESY:
			strReasonString = "Remove heresy";
			break;
		case FOLLOWER_CHANGE_SCRIPTED_CONVERSION:
			strReasonString = "Scripted conversion";
			break;
		case FOLLOWER_CHANGE_SPY_PRESSURE:
			strReasonString = "Spy pressure";
			break;
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
		case FOLLOWER_CHANGE_ADOPT_FULLY:
			strReasonString = "Adopt fully";
			break;
#endif
#if defined(MOD_BALANCE_CORE_PANTHEON_RESET_FOUND)
		case FOLLOWER_CHANGE_PANTHEON_OBSOLETE:
			strReasonString = "Pantheon Obsolete";
			break;
#endif
		}
		strOutBuf += strReasonString + ", ";
		temp.Format("Pop: %d", m_pCity->getPopulation());
		strOutBuf += temp;
		if(pCityRel->IsReligionInCity())
		{
			ReligionTypes eMajority = pCityRel->GetReligiousMajority();
			if(eMajority != NO_RELIGION)
			{
				CvReligionEntry* pEntry = GC.getReligionInfo(eMajority);
				if(pEntry)
				{
					strOutBuf += ", Majority: ";
					strOutBuf += pEntry->GetDescription();
					temp.Format("(%d)", pCityRel->GetNumFollowers(eMajority));
					strOutBuf += temp;
				}
			}
			
			ReligionTypes eSecondary = pCityRel->GetSecondaryReligion();
			if (eSecondary != NO_RELIGION)
			{
				CvReligionEntry* pEntry = GC.getReligionInfo(eSecondary);
				if(pEntry)
				{
					strOutBuf += ", Secondary: ";
					strOutBuf += pEntry->GetDescription();
					temp.Format("(%d)", pCityRel->GetNumFollowers(eSecondary));
					strOutBuf += temp;
				}
			}

			temp.Format("Atheists: %d", pCityRel->GetNumFollowers(NO_RELIGION));
			strOutBuf += ", " + temp;
		}
		else
		{
			strOutBuf += ", No religion in city";
		}

		pLog->Msg(strOutBuf);
	}
}
/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCityReligions& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);
#if !defined(MOD_BALANCE_CORE)
	if(uiVersion >= 2)
	{
		bool bTemp;
		loadFrom >> bTemp;
		writeTo.SetPaidAdoptionBonus(bTemp);
	}
	else
	{
		writeTo.SetPaidAdoptionBonus(false);
	}
#endif

	if (uiVersion >= 3)
	{
		int iTemp;
		loadFrom >> iTemp;
		writeTo.SetReligiousPressureModifier(iTemp);
	}
	else
	{
		writeTo.SetReligiousPressureModifier(0);
	}

	int iEntriesToRead;
	CvReligionInCity tempItem;

	writeTo.m_ReligionStatus.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_ReligionStatus.push_back(tempItem);
	}
#if defined(MOD_BALANCE_CORE)
	writeTo.ComputeReligiousMajority();
#endif

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvCityReligions& readFrom)
{
	uint uiVersion = 3;

	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);
#if !defined(MOD_BALANCE_CORE)
	saveTo << readFrom.HasPaidAdoptionBonus();
#endif

	saveTo << readFrom.GetReligiousPressureModifier();

	ReligionInCityList::const_iterator it;
	saveTo << readFrom.m_ReligionStatus.size();
	for(it = readFrom.m_ReligionStatus.begin(); it != readFrom.m_ReligionStatus.end(); it++)
	{
		saveTo << *it;
	}

	return saveTo;
}

//=====================================
// CvGameReligions
//=====================================
/// Constructor
CvUnitReligion::CvUnitReligion(void):
	m_eReligion(NO_RELIGION),
	m_iStrength(0),
	m_iSpreadsLeft(0)
{
}

/// Initialize class data
void CvUnitReligion::Init()
{
	m_eReligion = NO_RELIGION;
	m_iStrength = 0;
	m_iSpreadsLeft = 0;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvUnitReligion& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int temp;
	loadFrom >> temp;
	writeTo.SetReligion((ReligionTypes)temp);
	loadFrom >> temp;
	writeTo.SetReligiousStrength(temp);

	if(uiVersion >= 2)
	{
		loadFrom >> temp;
		writeTo.SetSpreadsLeft(temp);
	}
	else
	{
		writeTo.SetSpreadsLeft(0);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvUnitReligion& readFrom)
{
	uint uiVersion = 2;

	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.GetReligion();
	saveTo << readFrom.GetReligiousStrength();
	saveTo << readFrom.GetSpreadsLeft();

	return saveTo;
}

//=====================================
// CvReligionAI
//=====================================
/// Constructor
CvReligionAI::CvReligionAI(void):
	m_pPlayer(NULL)
{
}

/// Destructor
CvReligionAI::~CvReligionAI(void)
{
	Uninit();
}

/// Initialize class data
void CvReligionAI::Init(CvBeliefXMLEntries* pBeliefs, CvPlayer* pPlayer)
{
	m_pBeliefs = pBeliefs;
	m_pPlayer = pPlayer;

	Reset();
}

/// Cleanup
void CvReligionAI::Uninit()
{

}

/// Reset
void CvReligionAI::Reset()
{

}

/// Serialization read
void CvReligionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Serialization write
void CvReligionAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Called every turn to see what to spend Faith on
void CvReligionAI::DoTurn()
{
	// Only AI players use this function for now
	if(m_pPlayer->isHuman())
	{
		return;
	}

	AI_PERF_FORMAT("AI-perf.csv", ("Religion AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
#if defined(MOD_BALANCE_CORE)
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity && !pLoopCity->IsResistance() && !pLoopCity->IsPuppet() && pLoopCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
		{
			DoFaithPurchasesInCities(pLoopCity);
		}
	}
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity && !pLoopCity->IsResistance() && pLoopCity->IsPuppet() && pLoopCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
		{
			DoFaithPurchasesInCities(pLoopCity);
		}
	}
#endif
	DoFaithPurchases();
}

/// Select the belief most helpful to this pantheon
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChoosePantheonBelief(PlayerTypes ePlayer)
#else
BeliefTypes CvReligionAI::ChoosePantheonBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailablePantheonBeliefs(ePlayer);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailablePantheonBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
			if(iScore > 0)
			{
#endif
				beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),3);   // Throw out two-thirds of the choices -- this was way too loose as choices way down were being selected now only top 3

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Select the belief most helpful to this pantheon
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChooseFounderBelief(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
BeliefTypes CvReligionAI::ChooseFounderBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableFounderBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableFounderBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
			if(iScore > 0)
			{
#endif
				beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),4);   // Throw out 1/4 of the choices -- this was way too loose as choices way down were being selected now only top 4

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Select the belief most helpful to this pantheon
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChooseFollowerBelief(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
BeliefTypes CvReligionAI::ChooseFollowerBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableFollowerBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableFollowerBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
			if(iScore > 0)
			{
#endif
				beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),3);   // Throw out two-thirds of the choices -- this was way too loose as choices way down were being selected now only top 3

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Select the belief most helpful to enhance this religion
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChooseEnhancerBelief(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
BeliefTypes CvReligionAI::ChooseEnhancerBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableEnhancerBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableEnhancerBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
			if(iScore > 0)
			{
#endif
				beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),3);   // Throw out two-thirds of the choices -- this was way too loose as choices way down were being selected now only top 3

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Select the belief most helpful to enhance this religion
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChooseBonusBelief(PlayerTypes ePlayer, ReligionTypes eReligion, int iExcludeBelief1, int iExcludeBelief2, int iExcludeBelief3)
#else
BeliefTypes CvReligionAI::ChooseBonusBelief(int iExcludeBelief1, int iExcludeBelief2, int iExcludeBelief3)
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableBonusBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableBonusBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			if (pEntry->GetID() != iExcludeBelief1 && pEntry->GetID() != iExcludeBelief2 && pEntry->GetID() != iExcludeBelief3)
			{
				const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
				if(iScore > 0)
				{
#endif
					beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
				}
#endif
			}
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),3);   // Throw out two-thirds of the choices -- this was way too loose as choices way down were being selected now only top 3

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Select the belief most helpful to gain from Reformation social policy
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChooseReformationBelief(PlayerTypes ePlayer, ReligionTypes eReligion)
#else
BeliefTypes CvReligionAI::ChooseReformationBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true> beliefChoices;

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableReformationBeliefs(ePlayer, eReligion);
#else
	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableReformationBeliefs();
#endif

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry);
#if !defined(MOD_BUGFIX_MINOR)
			if(iScore > 0)
			{
#endif
				beliefChoices.push_back(eBelief, iScore);
#if !defined(MOD_BUGFIX_MINOR)
			}
#endif
		}
	}

	// Choose from weighted vector
	beliefChoices.SortItems();
	int iNumChoices = MIN(beliefChoices.size(),3);   // Throw out two-thirds of the choices -- this was way too loose as choices way down were being selected now only top 3

#if defined(MOD_BALANCE_CORE)
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
#ifdef AUI_RELIGION_RELATIVE_BELIEF_SCORE
		if ( beliefChoices.GetWeight(0) - beliefChoices.GetWeight(beliefChoices.size()-1) != 0)
			for (int iI = 0; iI < beliefChoices.size(); iI++)
				beliefChoices.IncreaseWeight(iI, -beliefChoices.GetWeight(beliefChoices.size()-1));
#endif // AUI_RELIGION_RELATIVE_BELIEF_SCORE
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
		LogBeliefChoices(beliefChoices, rtnValue);
	}
#else
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	BeliefTypes rtnValue = beliefChoices.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing belief from Top Choices");
	LogBeliefChoices(beliefChoices, rtnValue);
#endif

	return rtnValue;
}

/// Find the city where a missionary should next spread his religion
CvCity* CvReligionAI::ChooseMissionaryTargetCity(CvUnit* pUnit, int* piTurns)
{
	ReligionTypes eMyReligion = GetReligionToSpread();
	if(eMyReligion <= RELIGION_PANTHEON)
		return NULL;

	std::vector<SPlotWithScore> vTargets;

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			// Loop through each of their cities
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if(pLoopCity && pUnit->CanSpreadReligion(pLoopCity->plot()) && pUnit->GetDanger(pLoopCity->plot())==0 && !pLoopCity->IsRazing())
				{
					int iScore = ScoreCityForMissionary(pLoopCity, pUnit);
					if (iScore>0)
						vTargets.push_back(SPlotWithScore(pLoopCity->plot(),iScore));
				}
			}
		}
	}

	//this sorts ascending
	std::sort(vTargets.begin(),vTargets.end());
	//so reverse it
	std::reverse(vTargets.begin(),vTargets.end());

	for (std::vector<SPlotWithScore>::iterator it=vTargets.begin(); it!=vTargets.end(); ++it)
	{
		if (pUnit->GeneratePath(it->pPlot,CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX, piTurns) )
			return it->pPlot->getPlotCity();
	}

	return NULL;
}

/// Find the city where an inquisitor should next remove heresy
CvCity* CvReligionAI::ChooseInquisitorTargetCity(CvUnit* pUnit, int* piTurns)
{
	ReligionTypes eMyReligion = GetReligionToSpread();
	if(eMyReligion <= RELIGION_PANTHEON)
		return NULL;

	std::vector<SPlotWithScore> vTargets;

	// Loop through each of my cities
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity && pUnit->CanRemoveHeresy(pLoopCity->plot()) && pUnit->GetDanger(pLoopCity->plot())==0)
		{
			int iScore = ScoreCityForInquisitor(pLoopCity, pUnit);
			if (iScore>0)
				vTargets.push_back(SPlotWithScore(pLoopCity->plot(),iScore));
		}
	}


	//this sorts ascending
	std::sort(vTargets.begin(),vTargets.end());
	//so reverse it
	std::reverse(vTargets.begin(),vTargets.end());

	for (std::vector<SPlotWithScore>::iterator it=vTargets.begin(); it!=vTargets.end(); ++it)
	{
		if (pUnit->GeneratePath(it->pPlot,CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX, piTurns) )
			return it->pPlot->getPlotCity();
	}

	//nobody to burn at the stake? find a city to garrison him in
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity && pLoopCity->plot()->getMaxFriendlyUnitsOfType(pUnit)==0)
			return pLoopCity;
	}

	return NULL;
}

/// If we were going to use a prophet to convert a city, which one would it be?
CvCity *CvReligionAI::ChooseProphetConversionCity(bool bOnlyBetterThanEnhancingReligion, CvUnit* pUnit, int* piTurns) const
{
	CvCity *pHolyCity = NULL;
	int iMinScore = 50;  // Not zero because we don't want prophets to ALWAYS pick something up

	// Make sure we're spreading a religion and find holy city
	ReligionTypes eReligion = GetReligionToSpread();
	if (eReligion <= RELIGION_PANTHEON)
	{
		return NULL;
	}

	const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
	if (pkReligion)
	{
		CvPlot* pkPlot = GC.getMap().plot(pkReligion->m_iHolyCityX, pkReligion->m_iHolyCityY);
		if(pkPlot)
		{
			pHolyCity = pkPlot->getPlotCity();
		}
	}
	if (!pHolyCity)
	{
		return NULL;
	}

	std::vector<SPlotWithScore> vCandidates;

	// Look at our cities first, checking them for followers of other religions
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
		int iHeretics = pLoopCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
		int iDistanceToHolyCity = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pHolyCity->getX(), pHolyCity->getY());

		// If this is the holy city and it has been converted, want to go there no matter what
		if (pLoopCity == pHolyCity && eMajorityReligion != eReligion)
		{
#if defined(MOD_BALANCE_CORE)
			vCandidates.push_back(SPlotWithScore(pLoopCity->plot(), 100000));
#else
			return pHolyCity;
#endif
		}

		CvTacticalDominanceZone* pZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopCity,false);
		if (pZone && pZone->GetEnemyStrength()>0)
			continue;

		if(pUnit && !pUnit->CanSpreadReligion(pLoopCity->plot()))
			continue;

		// Otherwise score this city
		int iScore = (iHeretics * 100) / (iDistanceToHolyCity + 1);
		if (eMajorityReligion != eReligion && eMajorityReligion > RELIGION_PANTHEON)
		{
			iScore *= 3;
		}

		// Don't give it any score if our religion hasn't been enhanced yet
		else if (bOnlyBetterThanEnhancingReligion)
		{
			iScore = 0;
		}

		if (iScore > iMinScore)
			vCandidates.push_back( SPlotWithScore(pLoopCity->plot(),iScore));
	}

	// Now try other players, assuming don't need to enhance religion
	if (!bOnlyBetterThanEnhancingReligion)
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);

			if(kLoopPlayer.GetPlayerTraits()->IsForeignReligionSpreadImmune())
				continue;

			if(kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID()
#if defined(MOD_BALANCE_CORE_BELIEFS)
				//Make sure we aren't at war with the target player.
				&& (!GET_TEAM(m_pPlayer->getTeam()).isAtWar(kLoopPlayer.getTeam()))
#endif
				)
			{
				int iCityLoop;
				for(pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
				{
					if (!pLoopCity->GetCityReligions()->IsDefendedAgainstSpread(eReligion))
					{
						ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
						int iHeretics = pLoopCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
						int iDistanceToHolyCity = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pHolyCity->getX(), pHolyCity->getY());

						// Score this city
						int iScore = (iHeretics * 50) / (iDistanceToHolyCity + 1);

						//    - high score if this city has another religion as its majority
						if (eMajorityReligion != eReligion)
						{
							iScore *= 3;
						}

						//    - Holy city will anger folks, let's not do that one right away
						ReligionTypes eCityOwnersReligion = GET_PLAYER((PlayerTypes)iPlayerLoop).GetReligions()->GetReligionCreatedByPlayer();
						if (eCityOwnersReligion > RELIGION_PANTHEON && pLoopCity->GetCityReligions()->IsHolyCityForReligion(eCityOwnersReligion))
						{
							iScore /= 2;
						}

						//    - City not owned by religion founder, won't anger folks as much
						const CvReligion* pkMajorityReligion = GC.getGame().GetGameReligions()->GetReligion(eMajorityReligion, NO_PLAYER);
						if (pkMajorityReligion && pkMajorityReligion->m_eFounder != pLoopCity->getOwner())
						{
							iScore *= 2;
						}

#if defined(MOD_BALANCE_CORE_BELIEFS)
						//	- Do we have a belief that promotes foreign cities? If so, promote them.
						if(MOD_BALANCE_CORE_BELIEFS)
						{
							for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
							{
								YieldTypes eYield = (YieldTypes)iI;
								if(pkReligion->m_Beliefs.GetYieldFromForeignSpread(eYield) > 0)
								{
									iScore *= 2;
								}						
								else if(pkReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(eYield) > 0)
								{
									iScore *= 2;
								}
								else if(pkReligion->m_Beliefs.GetYieldChangePerForeignCity(eYield) > 0)
								{
									iScore *= 2;
								}
							}
							if(pkReligion->m_Beliefs.GetHappinessPerXPeacefulForeignFollowers() > 0)
							{
								iScore *= 2;
							}	
						}
#endif

						if (iScore > iMinScore)
							vCandidates.push_back( SPlotWithScore(pLoopCity->plot(),iScore));
					}
				}
			}
		}
	}

	//sort descending
	std::sort(vCandidates.begin(),vCandidates.end());
	std::reverse(vCandidates.begin(),vCandidates.end());

	for (size_t i=0; i<vCandidates.size(); i++)
	{
		if (!pUnit || pUnit->GeneratePath(vCandidates[i].pPlot,CvUnit::MOVEFLAG_APPROX_TARGET_RING1,INT_MAX,piTurns))
		{
			return vCandidates[i].pPlot->getPlotCity();
		}
	}

	return NULL;
}

/// What religion should this AI civ be spreading?
ReligionTypes CvReligionAI::GetReligionToSpread() const
{
	ReligionTypes eRtnValue = NO_RELIGION;

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetReligionToSpread", args.get(), iValue))
		{
			eRtnValue = (ReligionTypes)iValue;
			return eRtnValue;
		}
	}

	CvGameReligions *pReligions = GC.getGame().GetGameReligions();
	eRtnValue = pReligions->GetReligionCreatedByPlayer(m_pPlayer->GetID());
	if(eRtnValue > RELIGION_PANTHEON)
	{
		return eRtnValue;
	}
#if defined(MOD_BALANCE_CORE)
	eRtnValue = m_pPlayer->GetReligions()->GetReligionInMostCities();
	if(eRtnValue > RELIGION_PANTHEON)
	{
		return eRtnValue;
	}
#endif

	return NO_RELIGION;
}

// PRIVATE METHODS

/// Spend faith if already have an enhanced religion
#if defined(MOD_BALANCE_CORE)
void CvReligionAI::DoFaithPurchasesInCities(CvCity* pCity)
{
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
	if(kPlayer.GetID() == NO_PLAYER)
	{
		return;
	}
	if(pCity == NULL)
	{
		return;
	}
	ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
	if(eReligion == NO_RELIGION)
	{
		return;
	}
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());

	if(!pMyReligion->m_bEnhanced)
	{
		return;
	}

	BuildingClassTypes eFaithBuilding = FaithBuildingAvailable(eReligion);

	CvString strLogMsg;
	if(GC.getLogging())
	{
		strLogMsg = m_pPlayer->getCivilizationShortDescription();
	}

	// FIRST PRIORITY - OUR RELIGION'S BUILDING(S) IN CORE CITIES
	if((eReligion != NO_RELIGION) && (eFaithBuilding != NO_BUILDINGCLASS))
	{
		BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eFaithBuilding);
		if(pCity->GetCityBuildings()->GetNumBuilding(eBuilding) < 1 && pCity->IsCanPurchase(true, true, NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH))
		{
			if(BuyFaithBuilding(pCity, eBuilding))
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Bought a Faith Building";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}
			else
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving up for a Faith Building.";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}
		}
	}
	// SECOND PRIORITY - NON-FAITH BUILDINGS
	if(eReligion != NO_RELIGION)
	{
		// FIRST SUB-PRIORITY
		// Try to build other buildings with Faith if we took that belief
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eNonFaithBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			if(eNonFaithBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eNonFaithBuilding);
				//Changed - let's make sure it costs faith, and isn't a religious-specific building
				if(pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
				{
					if(pCity->GetCityBuildings()->GetNumBuilding(eNonFaithBuilding) < 1 && pCity->IsCanPurchase(true, true, NO_UNIT, eNonFaithBuilding, NO_PROJECT, YIELD_FAITH))
					{
						if(BuyFaithBuilding(pCity, eNonFaithBuilding))
						{
							if(GC.getLogging())
							{
								strLogMsg += ", Bought a Non-Faith Building";
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return;
						}
						else
						{
							if(GC.getLogging())
							{
								strLogMsg += ", Focusing on Non-Faith Buildings, have belief that allows this";
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return;
						}
					}
				}
			}
		}
	}
	// THIRD PRIORITY -- UNITS -- Only applicable if all main cities are converted.
	// Try to build other units with Faith if we took that belief
	if((eReligion != NO_RELIGION) && AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/))
	{
		if((kPlayer.GetTreasury()->CalculateBaseNetGoldTimes100() > 0) && (kPlayer.GetNumUnitsOutOfSupply() <= 0))
		{
			for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
			{
				UnitTypes eUnit = (UnitTypes)kPlayer.getCivilizationInfo().getCivilizationUnits(iI);
				if(eUnit != NO_UNIT)
				{
					CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(eUnit);

					// Check to make sure this is a war unit.
					if(pUnitEntry && !pUnitEntry->IsSpreadReligion() && !pUnitEntry->IsRemoveHeresy() && (pUnitEntry->GetFaithCost() > 0) && pUnitEntry->GetSpecialUnitType() == NO_SPECIALUNIT)
					{
						if(pCity->IsCanPurchase(true, true, eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH))
						{
							if(eUnit == pCity->GetUnitForOperation() || eUnit == kPlayer.GetMilitaryAI()->GetUnitForArmy(pCity))
							{
								pCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
								if(GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Bought a Non-Faith military Unit, %s", pUnitEntry->GetDescriptionKey());
									strLogMsg += strFaith;
									GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								}
								if(GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
									strLogMsg += strFaith;
									GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								}
								return;
							}
							else if(m_pPlayer->GetNumUnitsWithUnitAI(pUnitEntry->GetDefaultUnitAIType()) <= 5)
							{
								pCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
								if(GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Bought a Non-Faith Unit %s (up to four)", pUnitEntry->GetDescriptionKey());
									strLogMsg += strFaith;
									GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								}
								if(GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
									strLogMsg += strFaith;
									GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								}
								return;
							}
						}
					}
				}
			}
		}
	}
}
#endif
void CvReligionAI::DoFaithPurchases()
{
	ReligionTypes eReligion = GetReligionToSpread();

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
#if !defined(MOD_BALANCE_CORE)
	bool bTooManyMissionaries = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_MISSIONARY) > GC.getRELIGION_MAX_MISSIONARIES();
#endif

	CvString strLogMsg;
	if(GC.getLogging())
	{
		strLogMsg = m_pPlayer->getCivilizationShortDescription();
	}

#if defined(MOD_BALANCE_CORE)
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
	if(kPlayer.GetID() == NO_PLAYER)
	{
		return;
	}
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	CvCity *pHolyCity = NULL;
	if(pMyReligion)
	{
		CvPlot* pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		if (pHolyCityPlot)
		{
			pHolyCity = pHolyCityPlot->getPlotCity();

			if (pHolyCity && (pHolyCity->getOwner() == kPlayer.GetID()))
			{
				pCapital = pHolyCity;
			}
		}
	}
	if(pCapital == NULL)
	{
		return;
	}
	UnitTypes eProphetType = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
	
	UnitClassTypes eUnitClassMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");
	int iNumMissionaries =  m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_MISSIONARY);
	int iMaxMissionaries = GC.getRELIGION_MAX_MISSIONARIES();
	
	//Do we have any useful beliefs to consider?
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	int iBonusValue = 0;
	const int iNumBeliefs = pkBeliefs->GetNumBeliefs();
	for(int iI = 0; iI < iNumBeliefs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
		if(pEntry && m_pPlayer->HasBelief(eBelief))
		{
			for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				if(pEntry->GetYieldFromConversion((YieldTypes)iI) > 0)
				{
					iBonusValue++;
				}
				if(pEntry->GetYieldFromForeignSpread((YieldTypes)iI) > 0)
				{
					iBonusValue++;
				}
				if(pEntry->GetYieldFromSpread((YieldTypes)iI) > 0)
				{
					iBonusValue++;
				}
				if(pEntry->GetYieldPerFollowingCity((YieldTypes)iI) > 0)
				{
					iBonusValue++;
				}
				if(pEntry->GetYieldPerXFollowers((YieldTypes)iI) > 0)
				{
					iBonusValue++;
				}
			}
			if(pEntry->GetMissionaryInfluenceCS() > 0)
			{
				iBonusValue++;
			}
		}
	}
	iMaxMissionaries += iBonusValue;
	bool bTooManyMissionaries = false;
	if(eUnitClassMissionary != NO_UNITCLASS && m_pPlayer->GetPlayerTraits()->NoTrain(eUnitClassMissionary))
	{
		bTooManyMissionaries = true;
	}
	//Let's not spread a non-founder religion outside of our owned cities. That makes us a pawn!
	else if(eReligion != kPlayer.GetReligions()->GetReligionCreatedByPlayer())
	{
		iMaxMissionaries = 1;
		if(AreAllOurCitiesConverted(eReligion, true /*bIncludePuppets*/))
		{
			iMaxMissionaries = 0;
			bTooManyMissionaries = true;
		}
		else if(iNumMissionaries >= iMaxMissionaries)
		{
			bTooManyMissionaries = true;
		}
	}
	else if(iNumMissionaries >= iMaxMissionaries)
	{
		bTooManyMissionaries = true;
	}

	//Let's see about our religious flavor...
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int iFlavorReligion = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
	//Religion bonuses should artificially boost flavors.
	iFlavorReligion += iBonusValue;
	// UNITS - DOMESTIC
	if(pCapital != NULL)
	{
		//FIRST PRIORITY
		//Let's make sure our faith is enhanced.
		if(pMyReligion != NULL && !pMyReligion->m_bEnhanced)
		{
			if(eProphetType != NO_UNIT)
			{
				if(BuyGreatPerson(eProphetType))
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Bought a Prophet for religion enhancement.";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
				}
				else
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Saving up for a Prophet for religion enhancement.";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
					return;
				}
			}
		}
		// SECOND PRIORITY
		// If in Industrial, see if we want to save for buying a great person, but only if we've already got a Reformation belief and our cities are in good shape.
		if (m_pPlayer->GetCurrentEra() >= GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA) && GetDesiredFaithGreatPerson() != NO_UNIT)
		{
			if(pMyReligion != NULL)
			{
				if(m_pPlayer->IsReformation() && AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/))
				{
					UnitTypes eGPType = GetDesiredFaithGreatPerson();
					if(eGPType != NO_UNIT)
					{
						if(BuyGreatPerson(eGPType))
						{
							if(GC.getLogging())
							{
								strLogMsg += ", Bought a Great Person, as we're in the Industrial age, we've reformed, and all our cities are converted.";
								strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
						}
						else
						{
							if(GC.getLogging())
							{
								strLogMsg += ", Saving up for a Great Person, as we're in the Industrial age, we've reformed, and all our cities are converted.";
								strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return;
						}
					}
				}
			}
			else
			{
				UnitTypes eGPType = GetDesiredFaithGreatPerson();
				if(eGPType != NO_UNIT)
				{
					if(BuyGreatPerson(eGPType))
					{
						if(GC.getLogging())
						{
							strLogMsg += ", Bought a Great Person, as we don't have a faith and it is the Industrial age.";
							strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
							GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							CvString strFaith;
							strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
							strLogMsg += strFaith;
							GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						}
					}
					else
					{
						if(GC.getLogging())
						{
							strLogMsg += ", Saving up for a Great Person, as we don't have a faith and it is the Industrial age.";
							strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
							GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							CvString strFaith;
							strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
							strLogMsg += strFaith;
							GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						}
						return;
					}
				}
			}

		}
		//THIRD PRIORITY
		//Let's make sure all of our non-puppet cities are converted.
		if((eReligion != NO_RELIGION) && !AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/) && !bTooManyMissionaries)
		{
			if ((eProphetType != NO_UNIT) && ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/) && (m_pPlayer->GetReligions()->GetNumProphetsSpawned(true) <= iFlavorReligion))
			{
				if(BuyGreatPerson(eProphetType))
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Bought a Prophet, badly need to Convert Non-Puppet Cities";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
				}
				else if (!m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Saving up for a Prophet, as we need to convert Non-Puppet Cities.";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
					return;
				}
			}
			else if (!m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
			{
				if(BuyMissionary(eReligion))
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Bought a Missionary, need to Convert Non-Puppet Cities";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
				}
				else
				{
					if(GC.getLogging())
					{
						strLogMsg += ", Saving up for a Prophet or Missionary, as we need to convert Non-Puppet Cities.";
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
					return;
				}
			}
		}
		//FOURTH PRIORITY
		// Might as well convert puppet-cities to build our religious strength
		if((eReligion != NO_RELIGION) && m_pPlayer->GetNumPuppetCities() > 0 && !AreAllOurCitiesConverted(eReligion, true /*bIncludePuppets*/) && !bTooManyMissionaries && !m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
		{
			if(BuyMissionary(eReligion))
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Bought a Missionary, Need to Convert Puppet Cities";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
			}
			else
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving up for a Missionary, as we need to convert Puppet Cities.";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}
		}
	}
	// FOREIGN UNITS
	bool bStillTooManyMissionaries = (m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_MISSIONARY) > iMaxMissionaries);
	if((eReligion != NO_RELIGION) && (pMyReligion != NULL) && (pCapital != NULL) && !bStillTooManyMissionaries && AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/) && !m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
	{
		if(m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			if(pMyReligion->m_Beliefs.GetUniqueCiv(m_pPlayer->GetID()) == m_pPlayer->getCivilizationType())
			{
				return;
			}
		}
		// FLAVOR DEPENDENCIES
		// FIRST PRIORITY
		//Let's start with the highest-flavor stuff and work our way down...
		//Are we super religious? Target all cities, and always get Missionaries.
		if(iFlavorReligion >= 8 && HaveNearbyConversionTarget(eReligion, true /*bCanIncludeReligionStarter*/))
		{
			if(BuyMissionary(eReligion))
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Focusing on Missionaries, Need to Convert EVERYONE because religious zealotry";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}	
			else
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving up for Missionaries, Need to Convert EVERYONE because religious zealotry";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}

		}
		//SECOND PRIORITY
		// Have civs nearby to target who didn't start a religion?
		if((iFlavorReligion >= 5) && HaveNearbyConversionTarget(eReligion, false /*bCanIncludeReligionStarter*/))
		{
			if(BuyMissionary(eReligion))
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Focusing on Missionaries, Need to Convert Cities of Non-Religion Starters";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}
			else
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving up for Missionaries, Need to Convert Cities of Non-Religion Starters";
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return;
			}
		}
	}
	// FINAL PRIORITY
	// Have cities Inquisitors can defend?
	if((eReligion != NO_RELIGION) && !HaveEnoughInquisitors(eReligion))
	{
		if(BuyInquisitor(eReligion))
		{
			if(GC.getLogging())
			{
				strLogMsg += ", Bought an Inquisitor, Need to Defend Our Cities";
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
		}
		else
		{
			if(GC.getLogging())
			{
				strLogMsg += ", Focusing on Inquisitors, Need to Defend Our Cities";
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
			return;
		}
	}
	return;
}
#else
	// Save for another prophet if haven't enhanced religion yet
	if(pMyReligion == NULL || !pMyReligion->m_bEnhanced)
	{
		// Unless all religions gone and we didn't start one
		if(pMyReligion == NULL && pReligions->GetNumReligionsStillToFound() <= 0)
		{
			// Fill our cities with any Faith buildings possible
			if(!BuyAnyAvailableFaithBuilding())
			{
#if defined(MOD_CONFIG_GAME_IN_XML)
				if(m_pPlayer->GetCurrentEra() >= GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA))
#else
				if(m_pPlayer->GetCurrentEra() >= GC.getInfoTypeForString("ERA_INDUSTRIAL"))
#endif
				{
					UnitTypes eGPType = GetDesiredFaithGreatPerson();
					if (eGPType != NO_UNIT)
					{
						BuyGreatPerson(eGPType);

						if(GC.getLogging())
						{
							strLogMsg += ", Saving for Great Person, ";
							CvUnitEntry *pkEntry = GC.getUnitInfo(eGPType);
							if (pkEntry)
							{
								strLogMsg += pkEntry->GetDescription();
							}
						}				
					}
				}
			}
		}
	}
	else
	{
		// Do we need a prophet pronto to reestablish our religion?
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		UnitTypes eProphetType = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);
#else
		UnitTypes eProphetType = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
#endif
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		if (eProphetType != NO_UNIT && ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/) && m_pPlayer->GetReligions()->GetNumProphetsSpawned(false) <= 5)
#else
		if (eProphetType != NO_UNIT && ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/) && m_pPlayer->GetReligions()->GetNumProphetsSpawned() <= 5)
#endif
		{
			BuyGreatPerson(eProphetType);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Prophet, ";
				strLogMsg += GC.getUnitInfo(eProphetType)->GetDescription();
			}				
		}

		// Besides prophets, first priority is to convert all our non-puppet cities
		else if(!bTooManyMissionaries && !AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/))
		{
			BuyMissionary(eReligion);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Missionary, Need to Convert Non-Puppet Cities";
			}
		}

		// Next priority is to establish our faith building in every non-puppet city
		else if(eFaithBuilding != NO_BUILDINGCLASS && !AreAllOurCitiesHaveFaithBuilding(eReligion, false /*bIncludePuppets*/))
		{
			BuyFaithBuilding(eReligion, eFaithBuilding);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Faith Building, For Our Non-Puppet Cities";
			}
		}

		// Try to build other buildings with Faith if we took that belief
		else if (CanBuyNonFaithBuilding())
		{
			if (!BuyAnyAvailableNonFaithBuilding())
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving for Science Building, Have Reformation Belief";
				}
			}
		}

		// If in Industrial, see if we want to save for buying a great person
#if defined(MOD_CONFIG_GAME_IN_XML)
		else if (m_pPlayer->GetCurrentEra() >= GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA) && GetDesiredFaithGreatPerson() != NO_UNIT)
#else
		else if (m_pPlayer->GetCurrentEra() >= GC.getInfoTypeForString("ERA_INDUSTRIAL") && GetDesiredFaithGreatPerson() != NO_UNIT)
#endif
		{
			UnitTypes eGPType = GetDesiredFaithGreatPerson();
			BuyGreatPerson(eGPType);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Great Person, ";
				strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
			}				
		}

		// Might as well convert puppet-cities to build our religious strength
		else if(!bTooManyMissionaries && eFaithBuilding != NO_BUILDINGCLASS && !AreAllOurCitiesHaveFaithBuilding(eReligion, true /*bIncludePuppets*/))
		{
			BuyMissionary(eReligion);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Missionary, Need to Convert Puppet Cities";
			}
		}

		// Have civs nearby to target who didn't start a religion?
		else if(!bTooManyMissionaries && HaveNearbyConversionTarget(eReligion, false /*bCanIncludeReligionStarter*/))
		{
			BuyMissionary(eReligion);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Missionary, Need to Convert Cities of Non-Religion Starters";
			}
		}

		// Have cities Inquisitors can defend?
		else if(!HaveEnoughInquisitors(eReligion))
		{
			BuyInquisitor(eReligion);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Inquisitors, Need to Defend Our Cities";
			}
		}

		// Have civs nearby to target who did start a religion?
		else if(!bTooManyMissionaries && HaveNearbyConversionTarget(eReligion, true /*bCanIncludeReligionStarter*/))
		{
			BuyMissionary(eReligion);

			if(GC.getLogging())
			{
				strLogMsg += ", Saving for Missionary, Need to Convert Cities of Religion Starters";
			}
		}

		// Any faith buildings from other religions we can buy?
		else
		{
			BuyAnyAvailableFaithBuilding();
		}

		if(GC.getLogging())
		{
			CvString strFaith;
			strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
			strLogMsg += strFaith;
			GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
		}
	}
}
#endif
/// Pick the right city to purchase a missionary in
bool CvReligionAI::BuyMissionary(ReligionTypes eReligion)
{
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eMissionary = kPlayer.GetSpecificUnitType("UNITCLASS_MISSIONARY");
#else
	UnitTypes eMissionary = (UnitTypes)GC.getInfoTypeForString("UNIT_MISSIONARY");
#endif
	CvCity *pCapital = kPlayer.getCapitalCity();
	if (pCapital)
	{
		int iCost = pCapital->GetFaithPurchaseCost(eMissionary, true /*bIncludeBeliefDiscounts*/);
		if (iCost <= kPlayer.GetFaith())
		{
			CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(kPlayer, eMissionary, eReligion);
			if (pBestCity)
			{
				pBestCity->Purchase(eMissionary, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
				return true;
			}
		}
	}
	return false;
}

/// Pick the right city to purchase an inquisitor in
bool CvReligionAI::BuyInquisitor(ReligionTypes eReligion)
{
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eInquisitor = kPlayer.GetSpecificUnitType("UNITCLASS_INQUISITOR");
#else
	UnitTypes eInquisitor = (UnitTypes)GC.getInfoTypeForString("UNIT_INQUISITOR");
#endif
	CvCity *pCapital = kPlayer.getCapitalCity();
	if (pCapital)
	{
		int iCost = pCapital->GetFaithPurchaseCost(eInquisitor, true /*bIncludeBeliefDiscounts*/);
		if (iCost <= kPlayer.GetFaith())
		{
			CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(kPlayer, eInquisitor, eReligion);
			if (pBestCity)
			{
				pBestCity->Purchase(eInquisitor, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
				return true;
			}
		}
	}
	return false;
}

/// Pick the right city to purchase a great person in
bool CvReligionAI::BuyGreatPerson(UnitTypes eUnit)
{
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
	CvCity *pCapital = kPlayer.getCapitalCity();
	if (pCapital)
	{
		int iCost = pCapital->GetFaithPurchaseCost(eUnit, true /*bIncludeBeliefDiscounts*/);
		if (iCost <= kPlayer.GetFaith())
		{
			CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(kPlayer, eUnit, NO_RELIGION);
			if (pBestCity)
			{
				pBestCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
				return true;
			}
		}
	}
	return false;
}

/// Pick the right city to purchase a faith building in
bool CvReligionAI::BuyFaithBuilding(CvCity* pCity, BuildingTypes eBuilding)
{
	CvPlayer &kPlayer = GET_PLAYER(m_pPlayer->GetID());
	
	int iBuildingCost = pCity->GetFaithPurchaseCost(eBuilding);
	if (iBuildingCost <= kPlayer.GetFaith())
	{
		pCity->Purchase((UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH);
		return true;
	}

	return false;
}
#if defined(MOD_BALANCE_CORE_BELIEFS)
/// Any building that we can build with Faith (not Faith-generating ones)
bool CvReligionAI::BuyAnyAvailableNonFaithUnit()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();
	if(m_pPlayer->getCapitalCity() == NULL)
	{
		return false;
	}
	bool bPurchased = false;
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitTypes eUnit = m_pPlayer->getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(UNITAI_ATTACK, true);
			if(eUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(eUnit);

				if(pUnitEntry && pUnitEntry->GetCombat() > 0)
				{
					if(pLoopCity->IsCanPurchase(true, true, eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH))
					{
						if((m_pPlayer->GetTreasury()->CalculateBaseNetGoldTimes100() > 0) && (m_pPlayer->GetNumUnitsOutOfSupply() <= 0))
						{
							pLoopCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
							bPurchased = true;
						}	
					}
				}
			}
		}
	}
	if(bPurchased == true)
	{
		return true;
	}
	return false;
}
#endif
/// Any building that we can build with Faith (not Faith-generating ones)
bool CvReligionAI::BuyAnyAvailableNonFaithBuilding()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

				// Check to make sure this isn't a Faith-generating building
#if defined(MOD_BALANCE_CORE)
				if(pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
#else
				if(pBuildingEntry && pBuildingEntry->GetYieldChange(YIELD_FAITH) == 0)
#endif
				{
					if(pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						pLoopCity->Purchase((UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH);
						return true;
					}
				}
#if defined(MOD_BALANCE_CORE_BELIEFS)
				//If there are still buildings to buy, buy them.
				else if(pBuildingEntry)
				{
					if(pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						pLoopCity->Purchase((UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH);
						return true;
					}
				}
#endif
			}
		}
	}
	return false;
}

/// We didn't start a religion but we can still buy Faith buildings of other religions
bool CvReligionAI::BuyAnyAvailableFaithBuilding()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		ReligionTypes eReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
		if(eReligion > RELIGION_PANTHEON)
		{
			BuildingClassTypes eBuildingClass = FaithBuildingAvailable(eReligion);
			if(eBuildingClass != NO_BUILDINGCLASS)
			{
				BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
				if(eBuilding != NO_BUILDING)
				{
					if(pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						pLoopCity->Purchase((UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH);
						return true;
					}
				}
			}
		}
	}
	return false;
}

/// AI's perceived worth of a belief
int CvReligionAI::ScoreBelief(CvBeliefEntry* pEntry)
{
	int iRtnValue = 5;  // Base value since everything has SOME value

	// Loop through each plot on map
	int iPlotLoop;
	CvPlot* pPlot;
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		// Skip if not revealed or in enemy territory
		PlayerTypes ePlotOwner = pPlot->getOwner();
		if(pPlot->isRevealed(m_pPlayer->getTeam()) && (ePlotOwner == NO_PLAYER || ePlotOwner == m_pPlayer->GetID()))
		{
			// Also skip if closest city of ours is not within 2 turns
			if (m_pPlayer->GetCityDistanceInEstimatedTurns(pPlot)>2)
				continue;

			// Score it
			int iScoreAtPlot = ScoreBeliefAtPlot(pEntry, pPlot);

			// Apply multiplier based on whether or not being worked, within culture borders, or not
			if(pPlot->isBeingWorked())
			{
				iScoreAtPlot *= GC.getRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER();
			}
			else if(ePlotOwner != NO_PLAYER)
			{
				iScoreAtPlot *= GC.getRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER();
			}
			else
			{
				iScoreAtPlot *= GC.getRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER();
			}

			iRtnValue += iScoreAtPlot;
		}
	}

	// Add in value at city level
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		int iScoreAtCity = ScoreBeliefAtCity(pEntry, pLoopCity);
		iScoreAtCity *= GC.getRELIGION_BELIEF_SCORE_CITY_MULTIPLIER();

		iRtnValue += iScoreAtCity;
	}

	// Add in player-level value
	iRtnValue += ScoreBeliefForPlayer(pEntry);

	// Divide by 2 if a Pantheon belief (to deemphasize these to Byzantine bonus belief)
	if (pEntry->IsPantheonBelief())
	{
		iRtnValue /= 2;
	}

	return iRtnValue;
}

/// AI's evaluation of this belief's usefulness at this one plot
int CvReligionAI::ScoreBeliefAtPlot(CvBeliefEntry* pEntry, CvPlot* pPlot)
{
	int iRtnValue = 0;

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_FLAVOR_YIELDS
	int iTotalRtnValue = 0;
#endif

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
#ifdef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_FLAVOR_YIELDS
		int iFlavor = 1;
		CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
		switch (iI)
		{
		case YIELD_FOOD:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
			break;
		case YIELD_PRODUCTION:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
			break;
		case YIELD_GOLD:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			break;
		case YIELD_SCIENCE:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			break;
		case YIELD_CULTURE:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			break;
		case YIELD_FAITH:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
			break;
		}

		iRtnValue = 0;
#endif

		// Terrain
		TerrainTypes eTerrain = pPlot->getTerrainType();
#ifdef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_SCORE_TERRAIN_CONSIDER_FEATURE
		FeatureTypes eFeature = pPlot->getFeatureType();
		if(eTerrain != NO_TERRAIN && (eFeature == NO_FEATURE || !GC.getFeatureInfo(eFeature)->isYieldNotAdditive()))
#else
		if(eTerrain != NO_TERRAIN)
#endif // AUI_RELIGION_SCORE_BELIEF_AT_PLOT_SCORE_TERRAIN_CONSIDER_FEATURE
		{
#if defined(MOD_BALANCE_CORE_BELIEFS)
			//We only want to check for resources, as all Pantheons need a resource for their bonuses.
			ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
			if(eResource != NO_RESOURCE)
			{
#endif
			iRtnValue += pEntry->GetTerrainYieldChange(eTerrain, iI);
#if defined(MOD_BALANCE_CORE_BELIEFS)
			}
#endif
		}

#if defined(MOD_RELIGION_PLOT_YIELDS)
		if (MOD_RELIGION_PLOT_YIELDS) {
			// Plot
			PlotTypes ePlot = pPlot->getPlotType();
			if(ePlot != NO_PLOT)
			{
				iRtnValue += pEntry->GetPlotYieldChange(ePlot, iI);
				if(pPlot->isMountain())
				{
					iRtnValue /= 3;
				}
			}
		}
#endif

		// Feature
#ifndef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_SCORE_TERRAIN_CONSIDER_FEATURE
		FeatureTypes eFeature = pPlot->getFeatureType();
#endif
		if(eFeature != NO_FEATURE)
		{
			iRtnValue += pEntry->GetFeatureYieldChange(eFeature, iI);
#if defined(MOD_PSEUDO_NATURAL_WONDER)
			if(pPlot->IsNaturalWonder(true))
#else
			if(pPlot->IsNaturalWonder())
#endif
			{
				iRtnValue += pEntry->GetYieldChangeNaturalWonder(iI);
				iRtnValue += (pEntry->GetYieldModifierNaturalWonder(iI) / 25);
			}
		}
#if defined(MOD_BALANCE_CORE)
		//Lake
		if(pPlot->isLake())
		{
			iRtnValue += pEntry->GetLakePlotYieldChange(iI);
		}
#endif
		// Resource
		ResourceTypes eResource = pPlot->getResourceType();
		if(eResource != NO_RESOURCE)
		{
			iRtnValue += pEntry->GetResourceYieldChange(eResource, iI);
#if defined(MOD_BALANCE_CORE)
			if (pEntry->GetResourceQuantityModifier(eResource) > 0)
			{
				iRtnValue += (pEntry->GetResourceQuantityModifier(eResource) / 10);
			}
#endif
			// Improvement
			int iNumImprovementInfos = GC.getNumImprovementInfos();
			for(int jJ = 0; jJ < iNumImprovementInfos; jJ++)
			{
#if defined(MOD_BALANCE_CORE_BELIEFS)
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
				if(pkImprovementInfo && !pkImprovementInfo->IsCreatedByGreatPerson() && pkImprovementInfo->IsImprovementResourceTrade(eResource))
				{				
#endif
				if(pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
				{
#if defined(MOD_BALANCE_CORE_BELIEFS)
					iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI));
#else
					iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI) * 2);
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
				}
			}
#endif
			}
		}

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_FLAVOR_YIELDS
		iTotalRtnValue += iRtnValue*iFlavor;
#endif

	}

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_PLOT_FLAVOR_YIELDS
	return iTotalRtnValue;
#else
	return iRtnValue;
#endif
}

/// AI's evaluation of this belief's usefulness at this one plot
int CvReligionAI::ScoreBeliefAtCity(CvBeliefEntry* pEntry, CvCity* pCity)
{
	int iRtnValue = 0;
	int iTempValue;
	int iMinPop;
	int iMinFollowers;
	int iHappinessMultiplier = 2;

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int iFlavorOffense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iFlavorCityDefense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE"));
	int iFlavorHappiness = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_HAPPINESS"));

	int iHappinessNeedFactor = iFlavorOffense * 2 + iFlavorHappiness - iFlavorDefense;
	if (iHappinessNeedFactor > 15)
	{
		iHappinessMultiplier = 3;
	}
	else if (iHappinessNeedFactor < 6)
	{
		iHappinessMultiplier = 1;
	}

	iMinPop = pEntry->GetMinPopulation();
	iMinFollowers = pEntry->GetMinFollowers();

	// Simple ones
	iRtnValue += pEntry->GetCityGrowthModifier() / 3;
	if(pEntry->RequiresPeace())
	{
#ifdef AUI_RELIGION_SCORE_BELIEF_AT_CITY_CONSIDER_GRAND_STRATEGY
		iRtnValue /= 2 + (m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == (AIGrandStrategyTypes)GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST") ? 1 : -1);
#else
		iRtnValue /= 2;
#endif
	}
	iRtnValue += (-pEntry->GetPlotCultureCostModifier() * 2) / 10;
#if defined(MOD_BALANCE_CORE_BELIEFS)
	iRtnValue += (pEntry->GetCityRangeStrikeModifier() / 25) * MAX(iFlavorDefense,iFlavorCityDefense);
#else
	iRtnValue += (pEntry->GetCityRangeStrikeModifier() / 10) * MAX(iFlavorDefense,iFlavorCityDefense);
#endif
	iRtnValue += pEntry->GetFriendlyHealChange() / 10;

	// Wonder production multiplier
	if(pEntry->GetObsoleteEra() > 0)
	{	
		if (pEntry->GetObsoleteEra() > GC.getGame().getCurrentEra())
		{
			iRtnValue += (pEntry->GetWonderProductionModifier() * pEntry->GetObsoleteEra()) / 10;
		}
	}
	else
	{
		iRtnValue += pEntry->GetWonderProductionModifier() / 3;
	}

	// River happiness
#ifdef AUI_RELIGION_FIX_SCORE_BELIEF_AT_CITY_RIVER_HAPPINESS
	if (pCity->plot()->isRiver())
	{
#endif
	iTempValue = pEntry->GetRiverHappiness() * iHappinessMultiplier;
	if(iMinPop > 0)
	{
		if(pCity->getPopulation() >= iMinPop)
		{
			iTempValue *= 2;
		}
	}
	iRtnValue += iTempValue;
#ifdef AUI_RELIGION_FIX_SCORE_BELIEF_AT_CITY_RIVER_HAPPINESS
	}
#endif

	// Happiness per city
	iTempValue = pEntry->GetHappinessPerCity() * iHappinessMultiplier;
	if(iMinPop > 0)
	{
		if(pCity->getPopulation() >= iMinPop)
		{
			iTempValue *= 2;
		}
	}
	iRtnValue += iTempValue;

	// Building class happiness
	for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
	{
		iTempValue = pEntry->GetBuildingClassHappiness(jJ) * iHappinessMultiplier;
		if(iMinFollowers > 0)
		{
			if(pCity->getPopulation() >= iMinFollowers)
			{
				iTempValue *= 2;
			}
		}
		iRtnValue += iTempValue;
	}

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_CITY_FLAVOR_YIELDS
	int iTotalRtnValue = iRtnValue;
#endif

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
#ifdef AUI_RELIGION_SCORE_BELIEF_AT_CITY_FLAVOR_YIELDS
		iRtnValue = 0;
#endif
		// City yield change
		iTempValue = pEntry->GetCityYieldChange(iI);
		if(iMinPop > 0)
		{
			if(pCity->getPopulation() >= iMinPop)
			{
				iTempValue *= 2;
			}
		}
		iRtnValue += iTempValue;

#if defined(MOD_API_UNIFIED_YIELDS)
		if (pCity->isCapital()) {
			iTempValue = pEntry->GetCapitalYieldChange(iI);
			if(iMinPop > 0)
			{
				if(pCity->getPopulation() >= iMinPop)
				{
					iTempValue *= 2;
				}
			}
			iRtnValue += iTempValue;
		}

		if (pCity->isCoastal()) {
			iTempValue = pEntry->GetCoastalCityYieldChange(iI);
			if(iMinPop > 0)
			{
				if(pCity->getPopulation() >= iMinPop)
				{
					iTempValue *= 2;
				}
			}
			iRtnValue += iTempValue;
		}
		for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
		{
			int iValidTiles = 0;
			TerrainTypes eTerrain = (TerrainTypes) iJ;
			if(eTerrain != NO_TERRAIN && pEntry->GetYieldPerXTerrainTimes100(iJ, iI) > 0)
			{
				if(eTerrain == TERRAIN_MOUNTAIN)
				{
					iValidTiles = pCity->GetNearbyMountains();
				}
				else
				{
					const std::vector<int>& vWorkedPlots =  pCity->GetCityCitizens()->GetWorkedPlots();
					for (size_t ui=0; ui<vWorkedPlots.size(); ui++)
					{
						CvPlot* pPlot = GC.getMap().plotByIndex(vWorkedPlots[ui]);
						if(pPlot->getTerrainType() == eTerrain)
						{
							if(pEntry->RequiresNoImprovement() && pPlot->getImprovementType() == NO_IMPROVEMENT)
							{
								iValidTiles++;
							}
							else if(pEntry->RequiresImprovement() && pPlot->getImprovementType() != NO_IMPROVEMENT)
							{
								iValidTiles++;
							}
							else if(pEntry->RequiresResource() && pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE)
							{
								iValidTiles++;
							}
							else if(pEntry->RequiresNoFeature() && pPlot->getFeatureType() == NO_FEATURE)
							{
								iValidTiles++;
							}
							else
							{
								iValidTiles++;
							}
						}
					}
				}
				iRtnValue += ((iValidTiles * pEntry->GetYieldPerXTerrainTimes100(iJ, iI)) / 100);
			}
		}
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			int iValidTiles = 0;
			FeatureTypes eFeature = (FeatureTypes) iJ;
			if(eFeature != NO_FEATURE && pEntry->GetYieldPerXFeatureTimes100(iJ, iI) > 0)
			{
				const std::vector<int>& vWorkedPlots =  pCity->GetCityCitizens()->GetWorkedPlots();
				for (size_t ui=0; ui<vWorkedPlots.size(); ui++)
				{
					CvPlot* pPlot = GC.getMap().plotByIndex(vWorkedPlots[ui]);
					if(pPlot->getFeatureType() == eFeature)
					{
						if(pEntry->RequiresNoImprovement() && pPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							iValidTiles++;
						}
						else if(pEntry->RequiresImprovement() && pPlot->getImprovementType() != NO_IMPROVEMENT)
						{
							iValidTiles++;
						}
						else if(pEntry->RequiresResource() && pPlot->getResourceType(m_pPlayer->getTeam()) != NO_RESOURCE)
						{
							iValidTiles++;
						}
						else
						{
							iValidTiles++;
						}
					}
				}

				iRtnValue += ((iValidTiles * pEntry->GetYieldPerXFeatureTimes100(iJ, iI)) / 100);
			}
		}
#endif

		// Trade route yield change
		iTempValue = pEntry->GetYieldChangeTradeRoute(iI);
		if(iMinPop > 0)
		{
			if(pCity->getPopulation() >= iMinPop)
			{
				iTempValue *= 2;
			}
		}
		if(pCity->IsRouteToCapitalConnected())
		{
			iTempValue *= 2;
		}
		iRtnValue += iTempValue;

		// Specialist yield change
		iTempValue = pEntry->GetYieldChangeAnySpecialist(iI);
		if(pCity->getPopulation() >= 8)  // Like it more with large cities
		{
			iTempValue *= 3;
		}
#if defined(MOD_BALANCE_CORE)
		if(pCity->GetCityCitizens()->GetTotalSpecialistCount() > 0)
		{
			iTempValue *= 2;
		}
		else
		{
			iTempValue /= 4;
		}
#endif
		iRtnValue += iTempValue;

		// Building class yield change
		for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)jJ);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			iTempValue = pEntry->GetBuildingClassYieldChange(jJ, iI);
			if(iMinFollowers > 0)
			{
				if(pCity->getPopulation() < iMinFollowers)
				{
					iTempValue /= 2;
				}
			}

			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(jJ);
			if (eBuilding != NO_BUILDING)
			{
				if(pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					iTempValue *= 2;
				}
			}

			if(pkBuildingClassInfo->getMaxPlayerInstances() == 1)
			{
#if defined(AUI_RELIGION_SCORE_BELIEF_AT_CITY_YIELDS_FROM_WONDERS_COUNT_ONCE)
				iTempValue /= MAX(1,MIN(iTempValue,m_pPlayer->getNumCities()));
#else
				iTempValue /= 2;
#endif
			}

			iRtnValue += iTempValue;
		}

		// World wonder change
		iRtnValue += pEntry->GetYieldChangeWorldWonder(iI) * 3 / 2;

		// Yield per follower
		if (pEntry->GetMaxYieldModifierPerFollower(iI) > 0)
		{
			iTempValue = pEntry->GetMaxYieldModifierPerFollower(iI) / 2;
			iRtnValue += iTempValue;
		}

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_CITY_FLAVOR_YIELDS
		int iFlavor = 1;
		CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
		switch (iI)
		{
		case YIELD_FOOD:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
			break;
		case YIELD_PRODUCTION:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
			break;
		case YIELD_GOLD:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			break;
		case YIELD_SCIENCE:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			break;
		case YIELD_CULTURE:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			break;
		case YIELD_FAITH:
			iFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
			break;
		}

		iTotalRtnValue += iRtnValue*iFlavor;
#endif
	}

#ifdef AUI_RELIGION_SCORE_BELIEF_AT_CITY_FLAVOR_YIELDS
	return iTotalRtnValue;
#else
	return iRtnValue;
#endif
}

/// AI's evaluation of this belief's usefulness to this player
int CvReligionAI::ScoreBeliefForPlayer(CvBeliefEntry* pEntry)
{
	int iRtnValue = 0;
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();

	//--------------------
	// GET BACKGROUND DATA
	//--------------------
	int iFlavorOffense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iFlavorHappiness = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_HAPPINESS"));
	int iFlavorCulture = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
	int iFlavorGold = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
	int iFlavorGP = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GREAT_PEOPLE"));
	int iFlavorScience = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
	int iFlavorDiplomacy = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
	int iFlavorExpansion = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int iFlavorReligion = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
	int iFlavorGrowth = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	int iFlavorEspionage = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_ESPIONAGE"));
#endif
#if defined(MOD_BALANCE_CORE)
	bool bIndia = m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion();
#endif

	int iNumEnhancedReligions = pGameReligions->GetNumReligionsEnhanced();
	int iReligionsEnhancedPercent = (100 * iNumEnhancedReligions) / GC.getMap().getWorldInfo().getMaxActiveReligions();

	//------------------------------
	// PLAYER-LEVEL PANTHEON BELIEFS
	//------------------------------
	if(pEntry->GetFaithFromKills() > 0)
	{
		int iTemp = pEntry->GetFaithFromKills() * pEntry->GetMaxDistance() * iFlavorOffense / 100;
		if (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			iTemp *= 2;
		}
		iRtnValue += iTemp;
	}

	//-------
	// UNUSED
	//-------
	iRtnValue += pEntry->GetPlayerHappiness() * iFlavorHappiness;
	iRtnValue += pEntry->GetPlayerCultureModifier() * iFlavorCulture;

#if defined(MOD_BALANCE_CORE)
	iRtnValue += pEntry->GetPressureChangeTradeRoute() * iFlavorGold;
#endif

	//-----------------
	// FOLLOWER BELIEFS
	//-----------------
	// Unlocks a building
#if defined(MOD_BALANCE_CORE_BELIEFS)
	ReligionTypes eReligionFounded = m_pPlayer->GetReligions()->GetReligionCreatedByPlayer();
	BuildingClassTypes eFaithBuildingClass = NO_BUILDINGCLASS;
	if(eReligionFounded != NO_RELIGION)
	{
		eFaithBuildingClass = FaithBuildingAvailable(eReligionFounded);
	}
#endif
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		if (pEntry->IsBuildingClassEnabled(iI))
		{
			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

			if(pBuildingEntry && pFlavorManager)
			{
				for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
				{
					int iFlavorValue = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)iFlavorLoop) * pBuildingEntry->GetFlavorValue(iFlavorLoop);

					// If can also be built with hammers, much less valuable
					if (pBuildingEntry->GetProductionCost() > 0)
					{
						iFlavorValue /= 10;
					}
					iRtnValue += iFlavorValue;
#if defined(MOD_BALANCE_CORE_BELIEFS)
				}
			}
			if(pBuildingEntry)
			{
				//Do we already have a faith building? Let's not double down.									
				//If the byzantines, let's get two national wonders!
				if(m_pPlayer->GetPlayerTraits()->IsBonusReligiousBelief())
				{
					if(pBuildingEntry->IsReformation())
					{
						iRtnValue *= 20;
					}
				}
				else if(eFaithBuildingClass != NO_BUILDINGCLASS)
				{
					//Only penalize if we're considering getting a second faith building.
					if (pBuildingEntry && pBuildingEntry->GetReligiousPressureModifier() > 0)
					{
						iRtnValue /= 5;
					}
				}
				else
				{
					iRtnValue *= 10;
#endif
				}
			}
		}
	}

	// Unlocks units?	
#ifdef AUI_RELIGION_FIX_SCORE_BELIEF_FOR_PLAYER_UNLOCKS_UNITS_DISREGARD_OLD_ERAS
	for (int i = (int)m_pPlayer->GetCurrentEra(); i < GC.getNumEraInfos(); i++)
#else
	for(int i = 0; i < GC.getNumEraInfos(); i++)
#endif // AUI_RELIGION_FIX_SCORE_BELIEF_FOR_PLAYER_UNLOCKS_UNITS_DISREGARD_OLD_ERAS
	{
		// Add in for each era enabled
		if (pEntry->IsFaithUnitPurchaseEra(i))
		{
			int iTemp = (iFlavorOffense) + (iFlavorDefense / 2);
			if (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
			{
				iTemp *= 2;
			}
			iRtnValue += iTemp;
		}
	}

	//----------------
	// FOUNDER BELIEFS
	//----------------
	iRtnValue += (int)(pEntry->GetHappinessPerFollowingCity() * (float)(iFlavorHappiness + 2 * iFlavorOffense - iFlavorDefense));
#if defined(MOD_BALANCE_CORE)
	if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
	{
#endif
	if (pEntry->GetHappinessPerXPeacefulForeignFollowers() > 0 && iFlavorDefense > 0)
	{
		iRtnValue += (iFlavorHappiness * 10 * iFlavorDefense) / (pEntry->GetHappinessPerXPeacefulForeignFollowers() * iFlavorDefense);
	}
	iRtnValue += iFlavorScience * pEntry->GetSciencePerOtherReligionFollower() / 5;
	iRtnValue += pEntry->GetGoldPerFollowingCity() * iFlavorGold;
	if(pEntry->GetGoldPerXFollowers() > 0)
	{
		iRtnValue += (iFlavorGold * 4 / pEntry->GetGoldPerXFollowers());
	}
	iRtnValue += iFlavorGold * pEntry->GetGoldWhenCityAdopts() / 50;
#if defined(MOD_BALANCE_CORE)
	}
#endif

	// Minimum influence with city states
	iRtnValue += iFlavorDiplomacy * pEntry->GetCityStateMinimumInfluence() / 7;

	// Yields for foreign followers
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (iI == YIELD_CULTURE)
		{
			iRtnValue += pEntry->GetYieldChangePerForeignCity(iI) * 2 * iFlavorCulture;
#if defined(MOD_BALANCE_CORE)
			if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && (pEntry->GetYieldChangePerXForeignFollowers(iI) > 0))
#else
			if (pEntry->GetYieldChangePerXForeignFollowers(iI) > 0)
#endif
			{
				iRtnValue += 10 * iFlavorCulture / pEntry->GetYieldChangePerXForeignFollowers(iI);
			}
		}
		else
		{
			iRtnValue += pEntry->GetYieldChangePerForeignCity(iI) * 10;
#if defined(MOD_BALANCE_CORE)
			if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && (pEntry->GetYieldChangePerXForeignFollowers(iI) > 0))
#else
			if (pEntry->GetYieldChangePerXForeignFollowers(iI) > 0)
#endif
			{
				iRtnValue += 50 / pEntry->GetYieldChangePerXForeignFollowers(iI);
			}
		}
#if defined(MOD_BALANCE_CORE_BELIEFS)
		if (pEntry->GetYieldPerPop(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldPerPop(iI) * iFlavorGrowth);
		}
		if (pEntry->GetYieldPerGPT(iI) > 0)
		{
			iRtnValue += ((m_pPlayer->GetTreasury()->CalculateGrossGold() / pEntry->GetYieldPerGPT(iI)) * 3);
		}
		if (pEntry->GetYieldPerLux(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldPerLux(iI) * iFlavorCulture);
		}
		if (pEntry->GetYieldPerBirth(iI) > 0)
		{
			iRtnValue += ((pEntry->GetYieldPerBirth(iI) * iFlavorGrowth) / 2);
		}
		if (pEntry->GetYieldPerBorderGrowth(iI) > 0)
		{
			iRtnValue += ((pEntry->GetYieldPerBorderGrowth(iI) * iFlavorExpansion) / 5);
		}
		if (pEntry->GetYieldPerHeal(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldPerHeal(iI) * (iFlavorDefense / 2));
		}
		if (pEntry->GetYieldPerScience(iI) > 0)
		{
			iRtnValue += ((m_pPlayer->GetScience() / pEntry->GetYieldPerScience(iI)) * 3);
		}
		if (pEntry->GetMaxYieldPerFollower(iI) > 0)
		{
			iRtnValue += (pEntry->GetMaxYieldPerFollower(iI) * iFlavorExpansion);
		}
		if (pEntry->GetYieldFromGPUse(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromGPUse(iI) * iFlavorGP);
		}
		if (pEntry->GetYieldBonusGoldenAge(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldBonusGoldenAge(iI) * iFlavorHappiness);
			if(m_pPlayer->GetPlayerTraits()->GetGoldenAgeDurationModifier() > 0)
			{
				iRtnValue *= 10;
			}
		}
		if (!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pEntry->GetYieldFromSpread(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromSpread(iI) * iFlavorReligion);
		}
		if (!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pEntry->GetYieldFromForeignSpread(iI) > 0)
		{
			if(bIndia)
			{
				if(iI == YIELD_TOURISM)
				{
					iRtnValue += (pEntry->GetYieldFromForeignSpread(iI) * iFlavorCulture) / 3;
				}
				else
				{
					iRtnValue += (pEntry->GetYieldFromForeignSpread(iI) * iFlavorReligion) / 3;
				}
			}
			else
			{
				if(iI == YIELD_TOURISM)
				{
					iRtnValue += (pEntry->GetYieldFromForeignSpread(iI) * iFlavorCulture);
				}
				else
				{
					iRtnValue += (pEntry->GetYieldFromForeignSpread(iI) * iFlavorReligion);
				}
			}
		}
		if (pEntry->GetYieldFromConquest(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromConquest(iI) * iFlavorOffense) / 4;
		}
		if (pEntry->GetYieldFromPolicyUnlock(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromPolicyUnlock(iI) * iFlavorCulture);
			if(m_pPlayer->GetPlayerTraits()->GetFreeSocialPoliciesPerEra() > 0)
			{
				iRtnValue *= 10;
			}
		}
		if (pEntry->GetYieldFromEraUnlock(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromEraUnlock(iI) * (GC.getNumEraInfos() - m_pPlayer->GetCurrentEra()));
			//Big numbers skew value.
			iRtnValue /= ((m_pPlayer->GetCurrentEra() * 2) + 1);
		}
		if (!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pEntry->GetYieldFromConversion(iI) > 0)
		{
			if(bIndia)
			{
				iRtnValue += (pEntry->GetYieldFromConversion(iI) * iFlavorReligion) / 3;
			}
			else
			{
				iRtnValue += (pEntry->GetYieldFromConversion(iI) * iFlavorReligion);
			}
		}
		if (pEntry->GetYieldFromWLTKD(iI) > 0)
		{
			iRtnValue += (pEntry->GetYieldFromWLTKD(iI) * iFlavorExpansion);
			if(m_pPlayer->GetPlayerTraits()->GetWLTKDGATimer() > 0)
			{
				iRtnValue += m_pPlayer->GetPlayerTraits()->GetWLTKDGATimer() * 2;
			}
		}
		for(int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			if (pEntry->GetSpecialistYieldChange((SpecialistTypes)iJ, iI) > 0)
			{
				iRtnValue += (pEntry->GetSpecialistYieldChange((SpecialistTypes)iJ, iI) * (iFlavorGP * 3));
			}
		}
		if(pEntry->GetYieldPerXFollowers(iI) > 0)
		{
			iRtnValue += ((iFlavorGrowth + iFlavorExpansion + m_pPlayer->getTotalPopulation() * 5) / pEntry->GetYieldPerXFollowers(iI));
		}
		if(pEntry->GetYieldFromKnownPantheons(iI) > 0)
		{
			iRtnValue += (iFlavorDiplomacy * max(2, GC.getGame().GetGameReligions()->GetNumPantheonsCreated()));
		}
		if(pEntry->GetYieldFromHost(iI) > 0)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if(pLeague != NULL)
			{
				iRtnValue += ((pEntry->GetYieldFromHost(iI) / 2) * (iFlavorDiplomacy / 2));
				if(pLeague->GetHostMember() == m_pPlayer->GetID())
				{
					iRtnValue *= (pEntry->GetYieldFromHost(iI) / 2);
				}
			}
			else
			{
				iRtnValue += ((pEntry->GetYieldFromHost(iI) / 2) * (iFlavorDiplomacy / 2));
			}
		}
		if(pEntry->GetYieldFromProposal(iI) > 0)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if(pLeague != NULL)
			{
				iRtnValue += pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID());
				iRtnValue += ((pEntry->GetYieldFromProposal(iI) / 5) * (iFlavorDiplomacy / 2));
			}
			else
			{
				iRtnValue += ((pEntry->GetYieldFromProposal(iI) / 5) * (iFlavorDiplomacy / 2));
			}
		}

		ImprovementTypes eAcademy = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_ACADEMY");
		ImprovementTypes eHolySite = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_HOLY_SITE");
		ImprovementTypes eLandmark = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
		ImprovementTypes eCustomHouse = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CUSTOMS_HOUSE");
		ImprovementTypes eManufactory = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_MANUFACTORY");
		ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");
		//GP Improvement? Let's drill down.
		if(pEntry->GetImprovementYieldChange(eAcademy, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eAcademy, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eAcademy));
			iRtnValue *= iFlavorGP;
		}
		if(pEntry->GetImprovementYieldChange(eHolySite, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eHolySite, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eHolySite));
			iRtnValue *= iFlavorGP;
		}
		if(pEntry->GetImprovementYieldChange(eLandmark, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eLandmark, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eLandmark));
			iRtnValue *= iFlavorGP;
		}
		if(pEntry->GetImprovementYieldChange(eCustomHouse, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eCustomHouse, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eCustomHouse));
			iRtnValue *= iFlavorGP;
		}
		if(pEntry->GetImprovementYieldChange(eManufactory, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eManufactory, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eManufactory));
			iRtnValue *= iFlavorGP;
		}
		if(pEntry->GetImprovementYieldChange(eCitadel, (YieldTypes)iI) > 0)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(eCitadel, (YieldTypes)iI) * m_pPlayer->getImprovementCount(eCitadel));
			iRtnValue *= iFlavorGP;
		}
		if((YieldTypes)iI == YIELD_GOLDEN_AGE_POINTS && m_pPlayer->GetPlayerTraits()->GetWLTKDGATimer() > 0)
		{
			iRtnValue *= 2;
		}
#endif
	}

	//-----------------
	// ENHANCER BELIEFS
	//-----------------
	iRtnValue += iFlavorGP * pEntry->GetGreatPersonExpendedFaith() / 10;
#if defined(MOD_BALANCE_CORE)
	if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
	{
#endif
	iRtnValue += iFlavorDiplomacy * pEntry->GetFriendlyCityStateSpreadModifier() / 20;
#if defined(MOD_BALANCE_CORE)
	}
#endif
	iRtnValue += iFlavorDefense * pEntry->GetCombatModifierFriendlyCities() / 4;
	iRtnValue += iFlavorOffense * pEntry->GetCombatModifierEnemyCities() / 4;
#if defined(MOD_BALANCE_CORE)
	iRtnValue += iFlavorReligion * pEntry->GetPolicyReductionWonderXFollowerCities();
#endif
	// Chosen EARLY?
	if (iReligionsEnhancedPercent < 33)
	{
		// Good early
#if defined(MOD_BALANCE_CORE)
		if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
#endif
		iRtnValue += (2 * iFlavorExpansion) + pEntry->GetSpreadDistanceModifier();
		iRtnValue += (2 * iFlavorScience) + pEntry->GetSpreadStrengthModifier();

		iRtnValue += pEntry->GetMissionaryStrengthModifier() * 2;
		iRtnValue += (-1 * pEntry->GetMissionaryCostModifier()) * 2;

		// Poor early
		iRtnValue += pEntry->GetProphetStrengthModifier() / 4;
		iRtnValue += (-1 * pEntry->GetProphetCostModifier()) / 4;
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}

	// Chosen in the MIDDLE?
	else if (iReligionsEnhancedPercent < 66)
	{
#if defined(MOD_BALANCE_CORE)
		if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
#endif
		iRtnValue += iFlavorExpansion + pEntry->GetSpreadDistanceModifier() / 2;
		iRtnValue += iFlavorScience + pEntry->GetSpreadStrengthModifier() / 2;
		iRtnValue += pEntry->GetProphetStrengthModifier() / 2;
		iRtnValue += (-1 * pEntry->GetProphetCostModifier()) / 2;
		iRtnValue += pEntry->GetMissionaryStrengthModifier();
		iRtnValue += (-1 * pEntry->GetMissionaryCostModifier());
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}

	// Chosen LATE?
	else
	{
		// Poor late
#if defined(MOD_BALANCE_CORE)
		if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
#endif
		iRtnValue += ((2 * iFlavorExpansion) + pEntry->GetSpreadDistanceModifier()) / 4;
		iRtnValue += ((2 * iFlavorScience) + pEntry->GetSpreadStrengthModifier()) / 4;
		iRtnValue += pEntry->GetMissionaryStrengthModifier() / 2;
		iRtnValue += (-1 * pEntry->GetMissionaryCostModifier()) / 2;

		// Good late
		iRtnValue += pEntry->GetProphetStrengthModifier();
		iRtnValue += (-1 * pEntry->GetProphetCostModifier());
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}

	//-----------------
	// REFORMATION BELIEFS
	//-----------------
	if (pEntry->FaithPurchaseAllGreatPeople())
	{
		// Count number of GP branches we have still to open and score based on that
		int iTemp = 0;
#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
		if (MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
		{
			// Count the number of policies we DON'T have that unlock Great People, for the time being we won't worry about multiple policies unlocking the same GP
			for (int iPolicyLoop = 0; iPolicyLoop < m_pPlayer->GetPlayerPolicies()->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
			{
				const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
				CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
				if (pkLoopPolicyInfo && !m_pPlayer->HasPolicy(eLoopPolicy))
				{
					// We don't have this policy, but does it permit any GP to be bought with faith
					if (pkLoopPolicyInfo->HasFaithPurchaseUnitClasses())
					{
						iTemp++;
					}
				}
			}
			
			CUSTOMLOG("FaithPurchaseAllGreatPeople unlocks %i GPs for %s", iTemp, m_pPlayer->getCivilizationDescription());
		}
		else
		{
#endif
			PolicyBranchTypes eBranch;
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_TRADITION", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_HONOR", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_AESTHETICS", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_COMMERCE", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_EXPLORATION", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
			eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_RATIONALISM", true /*bHideAssert*/);
			if (eBranch != NO_POLICY_BRANCH_TYPE && (!m_pPlayer->GetPlayerPolicies()->IsPolicyBranchFinished(eBranch) || m_pPlayer->GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch)))
			{
				iTemp++;
			}
#if defined(MOD_RELIGION_POLICY_BRANCH_FAITH_GP)
		}
#endif

		iRtnValue += (iTemp * 6);
	}
	if (pEntry->ConvertsBarbarians())
	{
		MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if (m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyBarbs))
		{
			iRtnValue += 40;
		}
		else
		{
			iRtnValue += 15;
		}
	}
	if (pEntry->GetCityStateInfluenceModifier() > 0)
	{
		iRtnValue += (pEntry->GetCityStateInfluenceModifier() * iFlavorDiplomacy / 5);
	}
	if (pEntry->GetInquisitorPressureRetention() > 0)
	{
#if defined(MOD_BALANCE_CORE)
		iRtnValue += (pEntry->GetInquisitorPressureRetention() * iFlavorReligion);
#else
		iRtnValue += (pEntry->GetInquisitorPressureRetention() / 2);
#endif
	}
#if defined(MOD_BALANCE_CORE)
	if(!m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
	{
#endif
	if (pEntry->GetSpyPressure() > 0)
	{
#if defined(MOD_BALANCE_CORE)
		iRtnValue += (pEntry->GetSpyPressure() * iFlavorDiplomacy * iFlavorEspionage);
#else
		iRtnValue += (pEntry->GetSpyPressure()* 5);
#endif
	}
#if defined(MOD_BALANCE_CORE)
	}
#endif
	if (pEntry->GetFaithBuildingTourism() > 0)
	{
#if defined(MOD_BALANCE_CORE)
		iRtnValue += (pEntry->GetFaithBuildingTourism() * iFlavorCulture);
#else
		iRtnValue += (pEntry->GetFaithBuildingTourism() * 20);
#endif
	}
#if defined(MOD_BALANCE_CORE_BELIEFS)
	if (pEntry->GetMissionaryInfluenceCS() > 0)
	{
		iRtnValue += (pEntry->GetMissionaryInfluenceCS() * (iFlavorDiplomacy * 2));
	}
	if (pEntry->GetCombatVersusOtherReligionOwnLands() > 0)
	{
		iRtnValue += (pEntry->GetCombatVersusOtherReligionOwnLands() * (iFlavorDefense * 3));
	}
	if (pEntry->GetCombatVersusOtherReligionTheirLands() > 0)
	{
		iRtnValue += (pEntry->GetCombatVersusOtherReligionTheirLands() * (iFlavorOffense * 5));
	}
	if (pEntry->GetExtraVotes() > 0)
	{
		iRtnValue += (pEntry->GetExtraVotes() * (iFlavorDiplomacy * 10));
	}
	if (pEntry->GetHappinessPerPantheon() > 0)
	{
		int iPantheons = 0;
		for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			// Only civs we have met
			if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				if(GET_PLAYER((PlayerTypes)iI).GetReligions()->HasCreatedPantheon())
				{
					iPantheons++;
				}
			}
		}
		iRtnValue += (pEntry->GetHappinessPerPantheon() * iPantheons * iFlavorReligion);			
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if((bIndia || m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread()) && ((pEntry->GetMissionaryCostModifier() < 0)))
	{
		iRtnValue = 0;
	}
	if(bIndia && (pEntry->GetMissionaryInfluenceCS() > 0))
	{
		iRtnValue = 0;
	}
	if((bIndia || m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread()) && (pEntry->GetMissionaryStrengthModifier() > 0))
	{
		iRtnValue = 0;
	}
	if(bIndia && pEntry->ConvertsBarbarians())
	{
		iRtnValue = 0;
	}
	if((pEntry->GetRequiredCivilization() != NO_CIVILIZATION) && (pEntry->GetRequiredCivilization() == m_pPlayer->getCivilizationType()))
	{
		iRtnValue *= 100;
	}
#endif

	return iRtnValue;
}

/// AI's evaluation of this city as a target for a missionary
int CvReligionAI::ScoreCityForMissionary(CvCity* pCity, CvUnit* pUnit)
{
	int iScore = 0;
#if defined(MOD_BALANCE_CORE)
	ReligionTypes eMyReligion = pUnit->GetReligionData()->GetReligion();
#else
	ReligionTypes eMyReligion = GetReligionToSpread();
#endif
	
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eMyReligion)->IsLocalReligion()) {
		if (pCity->getOwner() != pUnit->getOwner()) {
			return iScore;
		}

		if (pCity->IsOccupied() || pCity->IsPuppet()) {
			return iScore;
		}
	}
#endif
#if defined(MOD_BALANCE_CORE)
	//We don't want to spread our faith to unowned cities if it doesn't spread naturally and we have a unique belief (as its probably super good).
	if(m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pCity->getOwner() != m_pPlayer->GetID())
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
		if (pMyReligion)
		{
			if(pMyReligion->m_Beliefs.GetUniqueCiv() == m_pPlayer->getCivilizationType())
			{
				return iScore;
			}
		}
	}
#endif

	// Skip if not revealed
	if(!pCity->plot()->isRevealed(m_pPlayer->getTeam()))
	{
		return iScore;
	}

	// Skip if already our religion
	if(pCity->GetCityReligions()->GetReligiousMajority() == eMyReligion)
	{
		return iScore;
	}

#if defined(MOD_BALANCE_CORE)
	//If this religion is not our founder faith, don't spread it beyond our borders!
	if(eMyReligion != m_pPlayer->GetReligions()->GetReligionCreatedByPlayer())
	{
		if(pCity->getOwner() != m_pPlayer->GetID())
		{
			return iScore;
		}
	}
#endif

	// Skip if at war with city owner
	if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(pCity->getOwner()).getTeam()))
	{
		return iScore;
	}

	// Base score based on if we are establishing majority
	iScore = 100;
	if(ShouldBecomeNewMajority(pCity, eMyReligion, pUnit->GetReligionData()->GetReligiousStrength() * GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER()))
	{
		iScore *= 2;
#ifdef AUI_RELIGION_SCORE_CITY_FOR_MISSIONARY_DIVIDER_IF_PASSIVE_PRESSURE_ENOUGH
		// We don't actually need a missionary if passive pressure is enough to convert the city
		if (ShouldBecomeNewMajority(pCity, eMyReligion, 0))
		{
			iScore /= 3;
			iScore *= 2;
		}
#endif // AUI_RELIGION_SCORE_CITY_FOR_MISSIONARY_DIVIDER_IF_PASSIVE_PRESSURE_ENOUGH
	}

	if (!GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		if (m_pPlayer->GetDiplomacyAI()->IsPlayerAgreeNotToConvert(pCity->getOwner()))
		{
			return 0;
		}
	}

#if defined(MOD_BALANCE_CORE_BELIEFS)
	if(MOD_BALANCE_CORE_HAPPINESS && pCity->getOwner() == m_pPlayer->GetID())
	{
		int iUnhappy = pCity->getUnhappinessFromReligion();
		if(iUnhappy > 0)
		{
			iScore *= (pCity->getUnhappinessFromReligion() + 1);
		}
	}
	//if a CS, and we have a bonus for that, emphasize.
	if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
		if (pMyReligion)
		{
			if(pMyReligion->m_Beliefs.GetMissionaryInfluenceCS() > 0)
			{
				iScore *= 10;
			}
#if defined(MOD_DIPLOMACY_CITYSTATES)
			if(MOD_DIPLOMACY_CITYSTATES && GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->IsNoAlly() && GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->IsFriends(m_pPlayer->GetID()))
			{
				iScore /= 4;
			}
			if(MOD_DIPLOMACY_CITYSTATES && GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->GetPermanentAlly() == m_pPlayer->GetID())
			{
				iScore /= 4;
			}
#endif
		}
	}
#endif

	CvPlayer& kCityPlayer = GET_PLAYER(pCity->getOwner());
	// Much better score if our own city or if this city owner isn't starting a religion
	if(pCity->getOwner() == m_pPlayer->GetID())
	{
		iScore *= 5;
	}
	else if(!kCityPlayer.GetReligions()->HasCreatedReligion())
	{
		iScore *= 3;
	}

	// Then subtract distance
	iScore -= plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY());

#if !defined(MOD_BALANCE_CORE)
	// Multiplier by how safe it is
	if(!atWar(m_pPlayer->getTeam(), kCityPlayer.getTeam()))
	{
		iScore *= 2;
	}
#endif

	// Holy city will anger folks, let's not do that one right away
	ReligionTypes eCityOwnersReligion = kCityPlayer.GetReligions()->GetReligionCreatedByPlayer();
	if (eCityOwnersReligion > RELIGION_PANTHEON && pCity->GetCityReligions()->IsHolyCityForReligion(eCityOwnersReligion))
	{
		iScore /= 2;
	}

	return iScore;
}

/// AI's evaluation of this city as a target for an inquisitor
int CvReligionAI::ScoreCityForInquisitor(CvCity* pCity, CvUnit* pUnit)
{
	int iScore = 0;
	ReligionTypes eMyReligion = GetReligionToSpread();

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eMyReligion)->IsLocalReligion()) {
		if (pCity->getOwner() != pUnit->getOwner()) {
			return iScore;
		}

		if (pCity->IsOccupied() || pCity->IsPuppet()) {
			return iScore;
		}
	}
#endif

	// How many other followers are there?
	int iOtherFollowers = pCity->GetCityReligions()->GetFollowersOtherReligions(eMyReligion);
	if (iOtherFollowers==0)
		return 0;

	// Base score based on if we are establishing majority
	iScore = 100 + iOtherFollowers;

	// Then subtract distance
	iScore -= plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY());

	// Not so pressing if already our religion
	if(pCity->GetCityReligions()->GetReligiousMajority() == eMyReligion)
	{
		iScore /= 2;
	}

	return iScore;
}

/// Is this enough new pressure to establish this as the new religious majority?
bool CvReligionAI::ShouldBecomeNewMajority(CvCity* pCity, ReligionTypes eReligion, int iNewPressure) const
{
	CvCityReligions* pCityReligions = pCity->GetCityReligions();
	int iTotalPressure = pCityReligions->GetTotalPressure() + iNewPressure;
	int iReligionPressure = pCityReligions->GetPressure(eReligion) + iNewPressure;

	return (iReligionPressure * 2) > iTotalPressure;
}

/// Are all of our own cities our religion?
bool CvReligionAI::AreAllOurCitiesConverted(ReligionTypes eReligion, bool bIncludePuppets) const
{
	bool bRtnValue = true;

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityReligions()->GetReligiousMajority() != eReligion)
		{
			if(bIncludePuppets || !pLoopCity->IsPuppet())
			{
				bRtnValue = false;
				break;
			}
		}
	}

	return bRtnValue;
}

/// Do all of our own cities have this religion's faith building if possible?
bool CvReligionAI::AreAllOurCitiesHaveFaithBuilding(ReligionTypes eReligion, bool bIncludePuppets) const
{
#ifdef AUI_RELIGION_FIX_ARE_ALL_OUR_CITIES_HAVE_FAITH_BUILDING_VENICE_PUPPETS
	if (m_pPlayer->GetPlayerTraits()->IsNoAnnexing())
		bIncludePuppets = true;
#endif // AUI_RELIGION_FIX_ARE_ALL_OUR_CITIES_HAVE_FAITH_BUILDING_VENICE_PUPPETS

	bool bRtnValue = true;
	BuildingClassTypes eFaithBuildingClass = FaithBuildingAvailable(eReligion);
	BuildingTypes eFaithBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eFaithBuildingClass);

	if(eFaithBuildingClass == NO_BUILDINGCLASS || eFaithBuilding == NO_BUILDING)
	{
		return true;
	}

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
		{
			if(bIncludePuppets || !pLoopCity->IsPuppet())
			{
				if(pLoopCity->GetCityBuildings()->GetNumBuilding(eFaithBuilding) < 1)
				{
					bRtnValue = false;
					break;
				}
			}
		}
	}

	return bRtnValue;
}

// Is there a civ nearby that isn't pressing religion?
bool CvReligionAI::HaveNearbyConversionTarget(ReligionTypes eReligion, bool bCanIncludeReligionStarter) const
{
	int iLoop;
	bool bStartedOwnReligion;
	TeamTypes eTeam = m_pPlayer->getTeam();
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eMissionary = m_pPlayer->GetSpecificUnitType("UNITCLASS_MISSIONARY");
#else
	UnitTypes eMissionary = (UnitTypes)GC.getInfoTypeForString("UNIT_MISSIONARY");
#endif
	int iMissionaryMoves = GC.getUnitInfo(eMissionary)->GetMoves();
#if defined(MOD_BALANCE_CORE)
	if(m_pPlayer->GetID() == -1)
	{
		return false;
	}
#endif
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if(pCapital == NULL)
	{
		return false;
	}

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;

		// Must be someone else
		if(ePlayer == m_pPlayer->GetID())
		{
			continue;

		}
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isAlive())
		{
			if(kPlayer.isMinorCiv())
			{
				bStartedOwnReligion = false;
			}
			else
			{
				if (!kPlayer.isBarbarian())
				{
					if (m_pPlayer->GetDiplomacyAI()->IsPlayerAgreeNotToConvert(ePlayer))
					{
						continue;
					}
				}

				if(kPlayer.GetReligionAI()->GetReligionToSpread() > RELIGION_PANTHEON)
				{
					bStartedOwnReligion = true;
				}
				else
				{
					bStartedOwnReligion = false;
				}
			}

			if(!bStartedOwnReligion || bCanIncludeReligionStarter)
			{
				for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
				{
#ifdef AUI_RELIGION_HAVE_NEARBY_CONVERSION_TARGET_IGNORE_TARGET_THAT_WILL_CONVERT_PASSIVELY
					ReligionTypes eMajorityReligion = pCity->GetCityReligions()->GetReligiousMajority();
					// Revealed, not at war, not currently our religion
					if (pCity->isRevealed(eTeam, false) && !GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(pCity->getOwner()).getTeam())
						&& eMajorityReligion != eReligion)
					{
						CvCityReligions* pCityReligions = pCity->GetCityReligions();
						// Religion wouldn't take over city passively
						if ((pCityReligions->GetPressure(eMajorityReligion) > pCityReligions->GetPressure(eReligion))
							|| ((pCityReligions->GetPressure(eMajorityReligion) == pCityReligions->GetPressure(eReligion)) &&
							(pCityReligions->GetNumFollowers(eMajorityReligion) >= pCityReligions->GetNumFollowers(eReligion))))
						{
#ifdef AUI_RELIGION_CONVERSION_TARGET_NOT_JUST_CAPITAL
							int iLoop;
							CvCity* pLoopCity;
							for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
							{
								if (m_pPlayer->GetPlayerTraits()->IsNoAnnexing() || !pLoopCity->IsPuppet())
								{
									// Within 8 times Missionary movement allowance of one of our non-puppet cities
									if (plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCity->getX(), pCity->getY()) <= (iMissionaryMoves * GC.getRELIGION_MISSIONARY_RANGE_IN_TURNS()))
									{
										return true;
									}
								}
							}
#else
							// Within 8 times Missionary movement allowance of our capital
							if (plotDistance(pCapital->getX(), pCapital->getY(), pCity->getX(), pCity->getY()) <= (iMissionaryMoves * GC.getRELIGION_MISSIONARY_RANGE_IN_TURNS()))
							{
								return true;
							}
#endif // AUI_RELIGION_CONVERSION_TARGET_NOT_JUST_CAPITAL
						}
					}
#else
					// Revealed and not currently our religion
					if(pCity->isRevealed(eTeam, false) && pCity->GetCityReligions()->GetReligiousMajority() != eReligion)
					{
#ifdef AUI_RELIGION_CONVERSION_TARGET_NOT_JUST_CAPITAL
						int iLoop;
						CvCity* pLoopCity;
						for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
						{
							if (m_pPlayer->GetPlayerTraits()->IsNoAnnexing() || !pLoopCity->IsPuppet())
							{
								// Within 8 times Missionary movement allowance of one of our non-puppet cities
								if (plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCity->getX(), pCity->getY()) <= (iMissionaryMoves * GC.getRELIGION_MISSIONARY_RANGE_IN_TURNS()))
								{
									return true;
								}
							}
						}
#else
						// Within 10 times Missionary movement allowance of our
						if(plotDistance(pCapital->getX(), pCapital->getY(), pCity->getX(), pCity->getY()) <= (iMissionaryMoves * GC.getRELIGION_MISSIONARY_RANGE_IN_TURNS()))
						{
							return true;
						}
#endif // AUI_RELIGION_CONVERSION_TARGET_NOT_JUST_CAPITAL
					}
#endif // AUI_RELIGION_HAVE_NEARBY_CONVERSION_TARGET_IGNORE_TARGET_THAT_WILL_CONVERT_PASSIVELY
				}
			}
		}
	}
	return false;
}

// Do we have as many Inquisitors as we need
bool CvReligionAI::HaveEnoughInquisitors(ReligionTypes eReligion) const
{
	int iLoop;
#if defined(MOD_BALANCE_CORE)
	UnitClassTypes eUnitClassInquisitor = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_INQUISITOR");
	if(eUnitClassInquisitor != NO_UNITCLASS && m_pPlayer->GetPlayerTraits()->NoTrain(eUnitClassInquisitor))
	{
		return true;
	}
	if (m_pPlayer->GetPlayerTraits()->IsReconquista() && m_pPlayer->GetPlayerTraits()->IsForeignReligionSpreadImmune())
	{
		return true;
	}
#endif
	// Need one for every city in our realm that is of another religion, plus one for defense
	int iNumNeeded = 1;
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		ReligionTypes eCityReligion = pCity->GetCityReligions()->GetReligiousMajority();
		if(eCityReligion > RELIGION_PANTHEON && eCityReligion != eReligion)
		{
			iNumNeeded++;
		}
	}

	// Count Inquisitors of our religion
	int iNumInquisitors = 0;
	for(CvUnit* pUnit = m_pPlayer->firstUnit(&iLoop); pUnit != NULL; pUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(pUnit->getUnitInfo().IsRemoveHeresy())
		{
			if(pUnit->GetReligionData()->GetReligion() == eReligion)
			{
				iNumInquisitors++;
			}
		}
	}

	return iNumInquisitors >= iNumNeeded;
}

/// Do we have a belief that allows a faith generating building to be constructed?
BuildingClassTypes CvReligionAI::FaithBuildingAvailable(ReligionTypes eReligion) const
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());

#if defined(MOD_BALANCE_CORE_BELIEFS)
	std::vector<BuildingClassTypes> choices;
#endif

	if (pMyReligion)
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (pMyReligion->m_Beliefs.IsBuildingClassEnabled((BuildingClassTypes)iI, m_pPlayer->GetID()))
			{
				BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings((BuildingClassTypes)iI);
				if(eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);
					if (pBuildingEntry && pBuildingEntry->GetYieldChange(YIELD_FAITH) > 0)
					{
#if defined(MOD_BALANCE_CORE_BELIEFS)
						//Let's not do this for the national wonders, okay?
						if(!pBuildingEntry->IsReformation())
							choices.push_back((BuildingClassTypes)iI);
#else
						return (BuildingClassTypes)iI;
#endif
					}
				}
			}
		}
	}

#if defined(MOD_BALANCE_CORE_BELIEFS)
	//pick a random building class
	if (choices.size()>1)
		return choices[ GC.getGame().getJonRandNum(choices.size(),"Faith Building Class") ];
	else if (choices.size()==1)
		return choices[0];
#endif

	return NO_BUILDINGCLASS;
}
#if defined(MOD_BALANCE_CORE_BELIEFS)
/// Can we buy a non-Faith generating building?
bool CvReligionAI::CanBuyNonFaithUnit() const
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitTypes eUnit = (UnitTypes)m_pPlayer->getCivilizationInfo().getCivilizationUnits(iI);
			if(eUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(eUnit);

				// Check to make sure this is a war unit.
				if(pUnitEntry && pUnitEntry->GetCombat() > 0)
				{
					if(pLoopCity->IsCanPurchase(true, true, eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
#endif
/// Can we buy a non-Faith generating building?
bool CvReligionAI::CanBuyNonFaithBuilding() const
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

				// Check to make sure this isn't a Faith-generating building
#if defined(MOD_BALANCE_CORE)
				//Changed - let's make sure it costs faith, and isn't a religious-specific building
				if(pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
#else
				if(pBuildingEntry && pBuildingEntry->GetYieldChange(YIELD_FAITH) == 0)
#endif
				{
					if(pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

/// Which Great Person should we buy with Faith?
UnitTypes CvReligionAI::GetDesiredFaithGreatPerson() const
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
	UnitTypes eRtnValue = NO_UNIT;
	int iBestScore = 0;
	ReligionTypes eReligion = GetReligionToSpread();

	// Loop through all Units and see if they're possible
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

		if(pkUnitInfo == NULL)
			continue;

		UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitInfo->GetUnitClassType();

		// Can't be able to train it
		if(pkUnitInfo->GetProductionCost() != -1)
		{
			continue;
		}

		// Must be a Great Person (defined in SpecialUnitType in Unit XML)
		if(pkUnitInfo->GetSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		// Must be a Great Person for this player's civ
		if(!m_pPlayer->canTrain(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
		{
			continue;
		}

		// Can we purchase this one in the capital?
		CvCity *pCapital = m_pPlayer->getCapitalCity();
		if (pCapital)
		{
			if (pCapital->IsCanPurchase(false/*bTestPurchaseCost*/, false/*bTestTrainable*/, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
			{
				int iScore = 0;
				AIGrandStrategyTypes eVictoryStrategy = m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();

				// Score it
				if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_PROPHET"))
				{
					if (GetReligionToSpread() > RELIGION_PANTHEON)
					{
						if (ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/))
						{
							iScore = 1500;
						}
						else if (ChooseProphetConversionCity(false/*bOnlyBetterThanEnhancingReligion*/))
						{
							iScore = 750;
						}
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
						iScore /= (1+ m_pPlayer->GetReligions()->GetNumProphetsSpawned(false));
#else
						iScore /= (1+ m_pPlayer->GetReligions()->GetNumProphetsSpawned());
#endif
#ifdef AUI_RELIGION_FIX_DO_FAITH_PURCHASES_ENHANCE_RELIGION
						const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
						if (pMyReligion && !pMyReligion->m_bEnhanced)
							iScore *= 2;
#endif
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getWritersFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getArtistsFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getMusiciansFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore = 1000;
					iScore /= (1+ m_pPlayer->getScientistsFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getMerchantsFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
				{
					EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH");
					if (eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
					{
						iScore = 2000;
					}
					else
					{
#ifdef AUI_RELIGION_GET_DESIRED_FAITH_GREAT_PERSON_ENGINEER_USES_WONDER_COMPETITIVENESS
						iScore = MAX(2000, int(AUI_RELIGION_GET_DESIRED_FAITH_GREAT_PERSON_ENGINEER_USES_WONDER_COMPETITIVENESS * m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() + 0.5));
#else
						iScore = 500;
#endif // AUI_RELIGION_GET_DESIRED_FAITH_GREAT_PERSON_ENGINEER_USES_WONDER_COMPETITIVENESS
					}
					iScore /= (1+ m_pPlayer->getEngineersFromFaith());
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST")&& !(GC.getMap().GetAIMapHint() & ciMapHint_Naval))
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getGeneralsFromFaith() + m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_GENERAL));
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_ADMIRAL"))
				{
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST") && GC.getMap().GetAIMapHint() & ciMapHint_Naval)
					{
						iScore = 1000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getAdmiralsFromFaith() + m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_ADMIRAL));
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MISSIONARY"))
				{
					if (HaveNearbyConversionTarget(eReligion, false /*bCanIncludeReligionStarter*/))
					{
						iScore = 1000 / (m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_MISSIONARY) + 1);		
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_INQUISITOR"))
				{
#ifdef AUI_RELIGION_FIX_GET_DESIRED_FAITH_GREAT_PERSON_INQUISITOR_CHECK
					if (!HaveEnoughInquisitors(eReligion))
#else
					if (HaveEnoughInquisitors(eReligion))
#endif // AUI_RELIGION_FIX_GET_DESIRED_FAITH_GREAT_PERSON_INQUISITOR_CHECK
					{
						iScore = 1000 / (m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_INQUISITOR) + 1);
					}
				}
#if defined(MOD_DIPLOMACY_CITYSTATES)
				else if (MOD_DIPLOMACY_CITYSTATES && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
				{
					EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");
					if (eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
					{
						iScore = 800;
					}
					if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_DIPLOMACY"))
					{
						iScore = 2000;
					}
					else
					{
						iScore = 400;
					}
					iScore /= (1+ m_pPlayer->getDiplomatsFromFaith() + m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_DIPLOMAT));
				}
#endif

				if (iScore > iBestScore)
				{
					iBestScore = iScore;
					eRtnValue = eUnit;
				}
			}
		}
	}

	return eRtnValue;
}

/// Log choices considered for beliefs
void CvReligionAI::LogBeliefChoices(CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true>& beliefChoices, int iChoice)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		BeliefTypes eBelief;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GC.getGame().GetGameReligions()->GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strBaseString += playerName + ", ";

		// Dump out the weight of each belief item
		for(int iI = 0; iI < beliefChoices.size(); iI++)
		{
			eBelief = beliefChoices.GetElement(iI);
			strDesc = GetLocalizedText(GC.GetGameBeliefs()->GetEntry(eBelief)->getShortDescription());
			strTemp.Format("Belief, %s, %d", strDesc.GetCString(), beliefChoices.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}

		// Finally the chosen one
		eBelief = (BeliefTypes)iChoice;
		strDesc = GetLocalizedText(GC.GetGameBeliefs()->GetEntry(eBelief)->getShortDescription());
		strTemp.Format("CHOSEN, %s", strDesc.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

// AI HELPER ROUTINES

CvCity *CvReligionAIHelpers::GetBestCityFaithUnitPurchase(CvPlayer &kPlayer, UnitTypes eUnit, ReligionTypes eReligion)
{
	bool bReligious = false;
	CvCity *pHolyCity = NULL;
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
	int iLoop;
	CvCity* pLoopCity;

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, kPlayer.GetID());

	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(eUnit);
	if (pkUnitEntry)
	{
		// Religious unit?
		if (pkUnitEntry->IsSpreadReligion() || pkUnitEntry->IsRemoveHeresy())
		{
			bReligious = true;
		}
	
		// Great person?
		else if (pkUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
		{
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->IsCanPurchase(true/*bTestPurchaseCost*/, true/*bTestTrainable*/, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
				{
					return pLoopCity;
				}
			}
			return NULL;
		}
	}

	// If religious, try to buy in the city with the Great Mosque first if a Missionary
	if (bReligious && pMyReligion)
	{
		for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			if (pLoopCity->getOwner() != kPlayer.GetID())
			{
				continue;
			}

			if (pLoopCity->GetCityBuildings()->GetMissionaryExtraSpreads() < 1 || pkUnitEntry->GetReligionSpreads() < 1)
			{
				continue;
			}

			if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
			{
				if(pLoopCity->IsCanPurchase(true, true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
				{
					return pLoopCity;
				}
			}
		}
	}

	// If religious, next try to buy in the holy city, assuming it hasn't been converted
	if (bReligious && pMyReligion)
	{
		CvPlot* pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		if (pHolyCityPlot)
		{
			pHolyCity = pHolyCityPlot->getPlotCity();

			if (pHolyCity && (pHolyCity->getOwner() == kPlayer.GetID()))
			{
				if (pHolyCity->GetCityReligions()->GetReligiousMajority() == eReligion && pHolyCity->IsCanPurchase(true, true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
				{
					return pHolyCity;
				}
			}
		}
	}

	// Now see if there is another city with our majority religion
	for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if (bReligious && pMyReligion && pLoopCity == pHolyCity)
		{
			continue;
		}

		if (pLoopCity->getOwner() != kPlayer.GetID())
		{
			continue;
		}

		if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
		{
			if(pLoopCity->IsCanPurchase(true, true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
			{
				return pLoopCity;
			}
		}
	}

	return NULL;
}

CvCity *CvReligionAIHelpers::GetBestCityFaithBuildingPurchase(CvPlayer &kPlayer, BuildingTypes eBuilding, ReligionTypes eReligion)
{
	CvCity *pHolyCity = NULL;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, kPlayer.GetID());

	// Try to buy in the holy city first
	if (pMyReligion)
	{
		CvPlot* pHolyCityPlot = GC.getMap().plot(pMyReligion->m_iHolyCityX, pMyReligion->m_iHolyCityY);
		if (pHolyCityPlot)
		{
			pHolyCity = pHolyCityPlot->getPlotCity();

			if (pHolyCity && (pHolyCity->getOwner() == kPlayer.GetID()) && pHolyCity->IsCanPurchase(true, true, NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH))
			{
				return pHolyCity;
			}
		}
	}

	// Now see if there is another city with our majority religion
	int iLoop;
	CvCity* pLoopCity;
	for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if(pLoopCity == pHolyCity)
		{
			continue;
		}

		if(pLoopCity->getOwner() != kPlayer.GetID())
		{
			continue;
		}

		if(pLoopCity->IsCanPurchase(true, true, NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH))
		{
			return pLoopCity;
		}
	}

	return NULL;
}

bool CvReligionAIHelpers::DoesUnitPassFaithPurchaseCheck(CvPlayer &kPlayer, UnitTypes eUnit)
{
	bool bRtnValue = true;

	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(eUnit);
	if (pkUnitEntry)
	{
		if (pkUnitEntry->IsSpreadReligion() || pkUnitEntry->IsRemoveHeresy())
		{
			bRtnValue = false;

			// Religious unit, have to find a city with the majority religion we started
			// Now see if there is another city with our majority religion
			int iLoop;
			CvCity* pLoopCity;

			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = pReligions->GetReligionCreatedByPlayer(kPlayer.GetID());

			if (eReligion > RELIGION_PANTHEON)
			{
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->getOwner() != kPlayer.GetID())
					{
						continue;
					}

					if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
					{
						if(pLoopCity->IsCanPurchase(false /*bTestPurchaseCost*/, true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return bRtnValue;
}
