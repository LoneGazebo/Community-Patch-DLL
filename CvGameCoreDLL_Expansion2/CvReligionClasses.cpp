/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include "CvTacticalAI.h"
#include "CvTacticalAnalysisMap.h"

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
	return (index!=NO_RELIGION) ? m_paReligionEntries[index] : NULL;
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
	if (pHolyCity)
	{
		m_iHolyCityX = pHolyCity->getX();
		m_iHolyCityY = pHolyCity->getY();
	}
	m_iTurnFounded = GC.getGame().getGameTurn();
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
}

///
template<typename Religion, typename Visitor>
void CvReligion::Serialize(Religion& religion, Visitor& visitor)
{
	visitor(religion.m_eReligion);
	visitor(religion.m_eFounder);
	visitor(religion.m_iHolyCityX);
	visitor(religion.m_iHolyCityY);
	visitor(religion.m_iTurnFounded);
	visitor(religion.m_bPantheon);
	visitor(religion.m_bEnhanced);
	visitor(religion.m_szCustomName);
	visitor(religion.m_bReformed);

	visitor(religion.m_Beliefs);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvReligion& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvReligion::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvReligion& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvReligion::Serialize(readFrom, serialVisitor);
	return saveTo;
}

CvString CvReligion::GetName() const
{
	CvReligionEntry* pEntry = GC.getReligionInfo(m_eReligion);
	CvAssertMsg(pEntry, "pEntry for religion not expected to be NULL. Please send Anton or Ed your save file and version.");
	if (pEntry)
	{
		CvString szReligionName = strlen(m_szCustomName) == 0 ? pEntry->GetDescriptionKey() : m_szCustomName;
		return szReligionName;
	}

	const char* szReligionNameBackup = "No Religion";
	return szReligionNameBackup;
}

CvCity * CvReligion::GetHolyCity() const
{
	CvPlot* pHolyCityPlot = GC.getMap().plot(m_iHolyCityX, m_iHolyCityY);
	if (pHolyCityPlot)
		return pHolyCityPlot->getPlotCity();

	return NULL;
}

//=====================================
// CvReligionInCity
//=====================================
/// Default Constructor
CvReligionInCity::CvReligionInCity()
{
	m_eReligion = NO_RELIGION;
	m_iFollowers = 0;
	m_iPressure = 0;
	m_iNumTradeRoutesApplyingPressure = 0;
}

/// Constructor
CvReligionInCity::CvReligionInCity(ReligionTypes eReligion, int iFollowers, int iPressure) :
	m_eReligion(eReligion),
	m_iFollowers(iFollowers),
	m_iPressure(iPressure),
	m_iNumTradeRoutesApplyingPressure(0)
{
}

template<typename ReligionInCity, typename Visitor>
void CvReligionInCity::Serialize(ReligionInCity& religionInCity, Visitor& visitor)
{
	visitor(religionInCity.m_eReligion);
	visitor(religionInCity.m_iFollowers);
	visitor(religionInCity.m_iPressure);
	visitor(religionInCity.m_iNumTradeRoutesApplyingPressure);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvReligionInCity& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvReligionInCity::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvReligionInCity& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvReligionInCity::Serialize(readFrom, serialVisitor);
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
	m_iMinimumFaithForNextPantheon = /*10 in CP, 50 in VP*/ GD_INT_GET(RELIGION_MIN_FAITH_FIRST_PANTHEON);
	m_iMinimumFaithForNextPantheon *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	m_iMinimumFaithForNextPantheon /= 100;

	//extremely important, this vector should never be reallocated
	//because we cache pointers to its entries in CvCityReligions!
	m_CurrentReligions.reserve(MAX_CIV_PLAYERS);
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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
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
	if (pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		pCity->GetCityReligions()->AddHolyCityPressure();
	}

	// Loop through all the players
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
			if (iSpyPressure > 0)
			{
				if (kPlayer.GetEspionage()->GetSpyIndexInCity(pCity) != -1)
				{
					ReligionTypes eReligionFounded = kPlayer.GetReligions()->GetStateReligion();
					if (eReligionFounded != NO_RELIGION)
					{
						pCity->GetCityReligions()->AddSpyPressure(eReligionFounded, iSpyPressure);
					}
				}
			}

			// Loop through each of their cities
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Ignore the same city
				if (pCity == pLoopCity)
				{
					continue;
				}

				for (int iI = RELIGION_PANTHEON + 1; iI < GC.GetGameReligions()->GetNumReligions(); iI++)
				{
					ReligionTypes eReligion = (ReligionTypes)iI;

					if (!IsValidTarget(eReligion, pLoopCity, pCity))
						continue;

					if (pLoopCity->GetCityReligions()->GetNumFollowers(eReligion) > 0)
					{
						bool bConnectedWithTrade = false;
						int iRelativeDistancePercent = 0;
						if (!IsCityConnectedToCity(eReligion, pLoopCity, pCity, bConnectedWithTrade, iRelativeDistancePercent))
							continue;

						int iNumTradeRoutes = 0;
						int iPressure = GetAdjacentCityReligiousPressure(eReligion, pLoopCity, pCity, iNumTradeRoutes, true, false, bConnectedWithTrade, iRelativeDistancePercent);
						if (iPressure > 0)
						{
							pCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_ADJACENT_PRESSURE, eReligion, iPressure);
							pCity->GetCityReligions()->RecomputeFollowers(FOLLOWER_CHANGE_ADJACENT_PRESSURE);
							if (iNumTradeRoutes != 0)
							{
								pCity->GetCityReligions()->IncrementNumTradeRouteConnections(eReligion, iNumTradeRoutes);
							}
						}
					}
				}
			}
		}
	}
}

bool CvGameReligions::IsValidTarget(ReligionTypes eReligion, CvCity* pFromCity, CvCity* pToCity)
{
	if (pFromCity->getOwner() != pToCity->getOwner())
	{
		if (GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pFromCity->getOwner());
			if (pReligion2 && (pFromCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pFromCity->getOwner()).getCivilizationType())
			{
				return false;
			}
		}
		if (GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pToCity->getOwner());
			if (pReligion2 && (pToCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pToCity->getOwner()).getCivilizationType())
			{
				return false;
			}
		}
	}
	else if (pFromCity->getOwner() == pToCity->getOwner())
	{
		if (GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pFromCity->getOwner());
			if (ePantheon != eReligion && pReligion2 && (pFromCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pFromCity->getOwner()).getCivilizationType())
			{
				return false;
			}
		}
		if (GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			ReligionTypes ePantheon = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetReligionCreatedByPlayer(true);
			const CvReligion* pReligion2 = GetReligion(ePantheon, pToCity->getOwner());
			if (ePantheon != eReligion && pReligion2 && (pToCity->GetCityReligions()->GetNumFollowers(ePantheon) > 0) && pReligion2->m_Beliefs.GetUniqueCiv() == GET_PLAYER(pToCity->getOwner()).getCivilizationType())
			{
				return false;
			}
		}
	}
	if (!GET_PLAYER(pToCity->getOwner()).isMinorCiv() && GET_PLAYER(pToCity->getOwner()).GetPlayerTraits()->IsForeignReligionSpreadImmune())
	{
		ReligionTypes eToCityReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetStateReligion();
		if (eToCityReligion != NO_RELIGION && eReligion != eToCityReligion)
		{
			return false;
		}
	}
	if (GET_PLAYER(pToCity->getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(pToCity->getOwner()).GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER)
		{
			if (GET_PLAYER(eAlly).GetPlayerTraits()->IsForeignReligionSpreadImmune())
			{
				ReligionTypes eToCityReligion = GET_PLAYER(eAlly).GetReligions()->GetStateReligion();
				if (eToCityReligion != NO_RELIGION && eReligion != eToCityReligion)
				{
					return false;
				}
			}
		}
	}

	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eReligion)->IsLocalReligion())
	{
		// Can only spread a local religion to our own cities or City States
		if (pToCity->getOwner() < MAX_MAJOR_CIVS && pFromCity->getOwner() != pToCity->getOwner()) 
		{
			return false;
		}

		// Cannot spread if either city is occupied or a puppet
		if ((pFromCity->IsOccupied() && !pFromCity->IsNoOccupiedUnhappiness()) || pFromCity->IsPuppet() ||
			(pToCity->IsOccupied() && !pToCity->IsNoOccupiedUnhappiness()) || pToCity->IsPuppet())
		{
			return false;
		}
	}

	return true;
}

bool CvGameReligions::IsCityConnectedToCity(ReligionTypes eReligion, CvCity* pFromCity, CvCity* pToCity, bool& bConnectedWithTrade, int& iRelativeDistancePercent)
{
	if (eReligion <= RELIGION_PANTHEON)
	{
		return false;
	}

	const CvReligion* pReligion = GetReligion(eReligion, pFromCity->getOwner());
	if (!pReligion)
	{
		return false;
	}

	bConnectedWithTrade = false;
	iRelativeDistancePercent = INT_MAX;

	if (eReligion == pFromCity->GetCityReligions()->GetReligiousMajority())
	{
		bConnectedWithTrade = GC.getGame().GetGameTrade()->CitiesHaveTradeConnection(pFromCity, pToCity);
		if (bConnectedWithTrade)
		{
			iRelativeDistancePercent = 1; //very close
			return true;
		}
	}

	// Boost to distance due to belief?
	int iDistanceMod = pReligion->m_Beliefs.GetSpreadDistanceModifier(pFromCity->getOwner());

	//Boost from policy of other player?
	if (GET_PLAYER(pToCity->getOwner()).GetReligionDistance() != 0)
	{
		if (pToCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
		{
			//Do we have a religion?
			ReligionTypes ePlayerReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetOwnedReligion();

			if (ePlayerReligion <= RELIGION_PANTHEON)
			{
				//No..but did we adopt one?
				ePlayerReligion = GET_PLAYER(pToCity->getOwner()).GetReligions()->GetStateReligion();

				//Nope, so full power.
				if (ePlayerReligion <= RELIGION_PANTHEON)
				{
					iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
				}
				//Yes, so only apply distance bonus to adopted faith.
				else if (eReligion == ePlayerReligion)
				{
					iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
				}
			}
			//We did! Only apply bonuses if it is our state religion.
			else if (eReligion == GET_PLAYER(pToCity->getOwner()).GetReligions()->GetStateReligion())
			{
				iDistanceMod += GET_PLAYER(pToCity->getOwner()).GetReligionDistance();
			}
		}
	}

	int iMaxDistanceLand = GET_PLAYER(pFromCity->getOwner()).GetTrade()->GetTradeRouteRange(DOMAIN_LAND, pFromCity)*SPath::getNormalizedDistanceBase();
	int iMaxDistanceSea = GET_PLAYER(pFromCity->getOwner()).GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pFromCity)*SPath::getNormalizedDistanceBase();

	if (iDistanceMod > 0)
	{
		iMaxDistanceLand *= (100 + iDistanceMod);
		iMaxDistanceLand /= 100;
		iMaxDistanceSea *= (100 + iDistanceMod);
		iMaxDistanceSea /= 100;
	}

	//estimate the distance between the cities from the traderoute cost. 
	//will be influences by terrain features, routes, open borders etc
	//note: trade routes are not necessarily symmetric in case of of unrevealed tiles etc
	SPath path;
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pFromCity, pToCity, &path))
	{
		int iPercent = (path.iNormalizedDistanceRaw * 100) / iMaxDistanceLand;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pToCity, pFromCity, &path))
	{
		int iPercent = (path.iNormalizedDistanceRaw * 100) / iMaxDistanceLand;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pFromCity, pToCity, &path))
	{
		int iPercent = (path.iNormalizedDistanceRaw * 100) / iMaxDistanceSea;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pToCity, pFromCity, &path))
	{
		int iPercent = (path.iNormalizedDistanceRaw * 100) / iMaxDistanceSea;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}

	return (iRelativeDistancePercent<100);
}

EraTypes CvGameReligions::GetFaithPurchaseGreatPeopleEra(CvPlayer* pPlayer)
{
	EraTypes eGPEra = /*INDUSTRIAL*/ (EraTypes)GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA);
	EraTypes eSpecialEra = pPlayer ? (EraTypes)pPlayer->GetPlayerTraits()->GetGPFaithPurchaseEra() : NO_ERA;
	if (eSpecialEra != NO_ERA && eSpecialEra < eGPEra)
	{
		return eSpecialEra;
	}
	return eGPEra;
}

/// Religious activities at the start of a player's turn
void CvGameReligions::DoPlayerTurn(CvPlayer& kPlayer)
{
	int iFaithAtStart = kPlayer.GetFaith();
	const PlayerTypes ePlayer = kPlayer.GetID();

	int iFaithPerTurn = kPlayer.GetTotalFaithPerTurn();
	if(iFaithPerTurn > 0)
	{
		kPlayer.ChangeFaith(iFaithPerTurn);
	}

	// If just now can afford missionary, add a notification
	bool bSendFaithPurchaseNotification = (kPlayer.isHuman() && kPlayer.GetFaithPurchaseType() == NO_AUTOMATIC_FAITH_PURCHASE);

	if (bSendFaithPurchaseNotification) 
	{
		if (MOD_RELIGION_RECURRING_PURCHASE_NOTIFY) 
		{
			bSendFaithPurchaseNotification = kPlayer.GetReligions()->CanAffordNextPurchase();
		} 
		else 
		{
			bool bCouldAtStartAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(iFaithAtStart);
			bool bCanNowAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(kPlayer.GetFaith());
			bSendFaithPurchaseNotification = !bCouldAtStartAffordFaithPurchase && bCanNowAffordFaithPurchase;
		}
	}

	if (bSendFaithPurchaseNotification)
	{
		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if(pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_MISSIONARY");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_MISSIONARY");
			pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
			kPlayer.GetReligions()->SetFaithAtLastNotify(kPlayer.GetFaith());
		}
	}

	// Check for pantheon or great prophet spawning (now restricted so must occur before Industrial era)
	if (kPlayer.GetFaith() > 0 && !kPlayer.isMinorCiv() && kPlayer.GetCurrentEra() <= /*RENAISSANCE*/ GD_INT_GET(RELIGION_LAST_FOUND_ERA))
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
				const BeliefTypes eBelief = kPlayer.GetReligionAI()->ChoosePantheonBelief(ePlayer);
				FoundPantheon(ePlayer, eBelief);
			}
		}

		switch (kPlayer.GetFaithPurchaseType())
		{
		case NO_AUTOMATIC_FAITH_PURCHASE:
		case FAITH_PURCHASE_SAVE_PROPHET:
			CheckSpawnGreatProphet(kPlayer);
			break;
		default:
			break;
		}
	}

	// Pick a Reformation belief?
	ReligionTypes eOwnedReligion = GET_PLAYER(ePlayer).GetReligions()->GetOwnedReligion();
	if (eOwnedReligion != NO_RELIGION && !HasAddedReformationBelief(ePlayer) && (kPlayer.GetPlayerPolicies()->HasPolicyGrantingReformationBelief() || kPlayer.IsReformation()))
	{
		if (!kPlayer.isHuman())
		{
			BeliefTypes eReformationBelief = kPlayer.GetReligionAI()->ChooseReformationBelief(ePlayer, eOwnedReligion);
			AddReformationBelief(ePlayer, eOwnedReligion, eReformationBelief);
		}
		else
		{
			CvNotifications* pNotifications = NULL;
			pNotifications = kPlayer.GetNotifications();
			if (pNotifications)
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
	ReligionTypes eReligion = kPlayer.GetReligionAI()->GetReligionToSpread(true);

	switch (kPlayer.GetFaithPurchaseType())
	{
	case NO_AUTOMATIC_FAITH_PURCHASE:
		break; // Valid option; Just do nothing.
	case FAITH_PURCHASE_SAVE_PROPHET:
	{
		if (eReligion <= RELIGION_PANTHEON && GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
		{
			UnitTypes eProphetType = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET_PARAM", GC.getUnitInfo(eProphetType)->GetDescription());
			bSelectionStillValid = false;
		}
		else if (kPlayer.GetCurrentEra() >= GetFaithPurchaseGreatPeopleEra(&kPlayer))
		{
			UnitTypes eProphetType = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);
			szItemName = GetLocalizedText("TXT_KEY_RO_AUTO_FAITH_PROPHET_PARAM", GC.getUnitInfo(eProphetType)->GetDescription());

			bSelectionStillValid = false;
		}
		break;
	}

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
					if (!bResult) 
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
		uint index = 0;
		
		// Pick a random one if required
		if (MOD_RELIGION_RANDOMISE) {
			index = GC.getGame().urandLimitExclusive(availableReligions.size(), CvSeeder(ePlayer));
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
	newReligion.m_Beliefs.SetReligion(RELIGION_PANTHEON);
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

	int iIncrement = /*5 in CP, 0 in VP*/ GD_INT_GET(RELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON);
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
				int iIdx = 0;
				pCapital = kPlayer.firstCity(&iIdx);
			}
			args->Push(pCapital ? pCapital->GetID() : 0);
#else
			args->Push(GET_PLAYER(ePlayer).getCapitalCity()->GetID());
#endif
			args->Push(RELIGION_PANTHEON);
			args->Push(eBelief);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "PantheonFounded", args.get(), bResult);
		}
#if defined(MOD_EVENTS_FOUND_RELIGION)
	}
#endif

	// Spread the pantheon into each of their cities
	int iLoop = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		// Add enough pressure to make this the likely majority religion
		int iInitialPressure = /*1000*/ GD_INT_GET(RELIGION_ATHEISM_PRESSURE_PER_POP) * pLoopCity->getPopulation() * 2;
		pLoopCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_PANTHEON_FOUNDED, newReligion.m_eReligion, iInitialPressure);
		pLoopCity->GetCityReligions()->RecomputeFollowers(FOLLOWER_CHANGE_PANTHEON_FOUNDED);
	}

	UpdateAllCitiesThisReligion(newReligion.m_eReligion);

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
					Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER");
					localizedText << pBelief->getShortDescription() << pBelief->GetDescriptionKey();
					pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
				}
				else
				{
					//If the notify player has not met this civ yet, display a more ambiguous notification.
					CvTeam& kTeam = GET_TEAM(kCurNotifyPlayer.getTeam());
					if(kTeam.isHasMet(kPlayer.getTeam()))
					{
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
					}
					else
					{
						Localization::String unknownFoundedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_UNKNOWN");
						unknownFoundedText << pBelief->getShortDescription() << pBelief->GetDescriptionKey();
						pNotifications->Add(NOTIFICATION_PANTHEON_FOUNDED, unknownFoundedText.toUTF8(), strSummary.toUTF8(), -1, -1, RELIGION_PANTHEON, -1);
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

		//Achievements!
		if (MOD_API_ACHIEVEMENTS && ePlayer == GC.getGame().getActivePlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_10);
	}

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Create a new religion
void CvGameReligions::FoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion kReligion(eReligion, ePlayer, pkHolyCity, false);

	// Copy over belief from your pantheon
	BeliefTypes eBelief = kPlayer.GetReligions()->HasCreatedPantheon() ? GC.getGame().GetGameReligions()->GetBeliefInPantheon(kPlayer.GetID()) : NO_BELIEF;
	if (eBelief != NO_BELIEF)
	{
		CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, ePlayer)->m_Beliefs;
		for (int iI = 0; iI < beliefs.GetNumBeliefs(); iI++) 
		{
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI));
		}
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

	if (kPlayer.GetPlayerTraits()->IsPopulationBoostReligion())
	{
		int iLoop = 0;
		CvCity* pLoopCity = NULL;
		for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			if (pkHolyCity == pLoopCity)
				continue;

			int iInitialPressure = /*1000*/ GD_INT_GET(RELIGION_FOUND_AUTO_SPREAD_PRESSURE) * /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
			pLoopCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eReligion, iInitialPressure);
			pLoopCity->GetCityReligions()->RecomputeFollowers(FOLLOWER_CHANGE_SCRIPTED_CONVERSION);
		}
	}

#if defined(MOD_TRAITS_OTHER_PREREQS)
	if (MOD_TRAITS_OTHER_PREREQS) {
		kPlayer.GetPlayerTraits()->InitPlayerTraits();
	}
#endif

	// Update game systems
	kPlayer.UpdateReligion();
	kPlayer.GetReligions()->SetFoundingReligion(false);

	// In case we have another prophet sitting around, make sure he's set to this religion
	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->getUnitInfo().IsFoundReligion())
			pLoopUnit->GetReligionDataMutable()->SetReligion(eReligion);
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

		bool bResult = false;
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
						pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
					}
					else
					{
						CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());

						if(kNotifyTeam.isHasMet(kPlayer.getTeam()))
						{
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, replayText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
						}
						else
						{
							Localization::String unknownCivText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_UNKNOWN");
							unknownCivText << szReligionName;
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, unknownCivText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
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

		//Achievements!
		if (MOD_API_ACHIEVEMENTS && ePlayer == GC.getGame().getActivePlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_11);
	}
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Can the supplied religion be created?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity)
{
	if (ePlayer == NO_PLAYER)
		return FOUNDING_INVALID_PLAYER;

	if (HasCreatedReligion(ePlayer))
		return FOUNDING_PLAYER_ALREADY_CREATED_RELIGION;

	if (GetNumReligionsStillToFound() <= 0 && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
		return FOUNDING_NO_RELIGIONS_AVAILABLE;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion kReligion(eReligion, ePlayer, pkHolyCity, false);

	// Copy over belief from your pantheon
	if (HasCreatedPantheon(ePlayer)) 
	{
		CvReligionBeliefs beliefs = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, kPlayer.GetID())->m_Beliefs;
		for (int iI = 0; iI < beliefs.GetNumBeliefs(); iI++) 
		{
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI));
		}
	}

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
void CvGameReligions::EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2, bool bNotify, bool bSetAsEnhanced)
{
	bool bFoundIt = false;
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	ReligionList::iterator it;

	for (it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// We use the same code for enhancing a pantheon, so make sure we find the pantheon for the player!
		if (it->m_eReligion == eReligion)
		{
			if (it->m_eReligion == RELIGION_PANTHEON)
			{
				if (it->m_eFounder == ePlayer)
				{
					bFoundIt = true;
					break;
				}
			}
			else
			{
				bFoundIt = true;
				break;
			}
		}
	}
	if (!bFoundIt)
	{
		CvAssertMsg(false, "Internal error in religion code.");
		CUSTOMLOG("Trying to enhance a religion/pantheon that doesn't exist!!!");
		return;
	}

	if (kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
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

	it->m_Beliefs.AddBelief(eBelief1);

	if (eBelief2 != NO_BELIEF)
		it->m_Beliefs.AddBelief(eBelief2);

	if (eReligion != RELIGION_PANTHEON && bSetAsEnhanced)
		it->m_bEnhanced = true;

	if (MOD_TRAITS_OTHER_PREREQS)
		kPlayer.GetPlayerTraits()->InitPlayerTraits();

	// Update game systems
	UpdateAllCitiesThisReligion(eReligion);
	kPlayer.UpdateReligion();

	if (MOD_EVENTS_FOUND_RELIGION) 
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionEnhanced, ePlayer, eReligion, eBelief1, eBelief2);
	} 
	else 
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(ePlayer);
			args->Push(eReligion);
			args->Push(eBelief1);
			args->Push(eBelief2);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "ReligionEnhanced", args.get(), bResult);
		}
	}

	if (bNotify) 
	{
		//Notify the masses
		for (int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop)
		{
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary;
				Localization::String notificationText;
				if (eReligion == RELIGION_PANTHEON)
				{
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_S");
					notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED");
					notificationText << kPlayer.getCivilizationShortDescriptionKey();
				}
				else
				{
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_S");
					notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED");
					notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName();
				}

				// Message slightly different for enhancing player
				if (ePlayer == eNotifyPlayer)
				{
					Localization::String localizedText;
					if (eReligion == RELIGION_PANTHEON)
					{
						localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_ACTIVE_PLAYER");
					}
					else
					{
						localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER");
						localizedText << it->GetName();
					}

					pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
				}
				else
				{
					CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());
					if (kNotifyTeam.isHasMet(kPlayer.getTeam()))
					{
						pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, notificationText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
					}
					else
					{
						Localization::String unknownText;
						if (eReligion == RELIGION_PANTHEON)
						{
							unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_ENHANCED_UNKNOWN");
						}
						else
						{
							unknownText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_ENHANCED_UNKNOWN");
							unknownText << it->GetName();
						}

						pNotifications->Add(NOTIFICATION_RELIGION_ENHANCED, unknownText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
					}
				}
			}
		}
	}

	// Logging
	if (GC.getLogging())
	{
		CvString strLogMsg;
		strLogMsg = kPlayer.getCivilizationShortDescription();
		strLogMsg += ", RELIGION ENHANCED";
		LogReligionMessage(strLogMsg);
	}

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Can the new beliefs be added to the religion?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2)
{
	bool bFoundIt = false;
	if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetReligions()->GetOwnedReligion() == eReligion)
	{
		bFoundIt = true;
	}

	if (bFoundIt)
	{
		if (eBelief1 != NO_BELIEF && IsInSomeReligion(eBelief1, ePlayer))
		{
			CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief1);
			if (pBelief && (pBelief->IsEnhancerBelief() || pBelief->IsFollowerBelief()) && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
			{
				return FOUNDING_BELIEF_IN_USE;
			}
		}
		if (eBelief2 != NO_BELIEF && IsInSomeReligion(eBelief2, ePlayer))
		{
			CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief2);
			if (pBelief && (pBelief->IsEnhancerBelief() || pBelief->IsFollowerBelief()) && !GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion())
			{
				return FOUNDING_BELIEF_IN_USE;
			}
		}

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
	for (int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop)
	{
		PlayerTypes eNotifyPlayer = (PlayerTypes)iNotifyLoop;
		CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
		if (pNotifications)
		{
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_S");
			Localization::String notificationText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED");
			notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName();

			// Message slightly different for reformation player
			if (ePlayer == eNotifyPlayer)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER");
				localizedText << it->GetName();

				pNotifications->Add(NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			else
			{
				CvTeam& kNotifyTeam = GET_TEAM(kNotifyPlayer.getTeam());
				if (kNotifyTeam.isHasMet(kPlayer.getTeam()))
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
	}

	// Logging
	if(GC.getLogging())
	{
		CvString strLogMsg;
		strLogMsg = kPlayer.getCivilizationShortDescription();
		strLogMsg += ", REFORMATION BELIEF ADDED";
		LogReligionMessage(strLogMsg);
	}
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

//returns the religion of the holy city *if* the test city is a holy city
ReligionTypes CvGameReligions::GetHolyCityReligion(const CvCity* pkTestCity) const
{
	//this is called a lot, but there isn't really a way to make it more efficient, a small map is not faster than a small vector
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (it->m_iHolyCityX == pkTestCity->getX() && it->m_iHolyCityY == pkTestCity->getY())
			return it->m_eReligion;
	}

	return NO_RELIGION;
}


/// Move the Holy City for a religion (useful for scenario scripting)
void CvGameReligions::SetHolyCity(ReligionTypes eReligion, const CvCity* pkHolyCity)
{
	ReligionList::iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			if (pkHolyCity != NULL)
			{
				//need to save coordinates here, the city ID changes on conquest!
				it->m_iHolyCityX = pkHolyCity->getX();
				it->m_iHolyCityY = pkHolyCity->getY();
			}
			else
			{
				// Holy City was destroyed or its status removed!
				it->m_iHolyCityX = -1;
				it->m_iHolyCityY = -1;
			}
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

/// Switch founding year for a religion (useful for scenario scripting)
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
/// Switch founding year for a religion (useful for scenario scripting)
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

/// After a religion is enhanced, the newly chosen beliefs need to be turned on in all cities
void CvGameReligions::UpdateAllCitiesThisReligion(ReligionTypes eReligion)
{
	int iLoop = 0;

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
	if(eReligion == NO_RELIGION)
		return NULL;

	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		// If talking about a pantheon, make sure to match the player
		if(it->m_eReligion == eReligion && it->m_eReligion == RELIGION_PANTHEON)
		{
			if(it->m_eFounder == ePlayer)
			{
				return &(*it);
			}
		}
		else if(it->m_eReligion == eReligion)
		{
			return &(*it);
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
		else if(!it->m_bPantheon && GET_PLAYER(ePlayer).GetReligions()->GetStateReligion() == it->m_eReligion)
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
	return static_cast<bool>(GET_PLAYER(ePlayer).GetReligions()->GetReligionCreatedByPlayer(true) != NO_RELIGION);
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

int CvGameReligions::GetNumPantheonsPossible(bool bExcludeUnique) const
{
	int iRtnValue = 0;

	for(int iI = 0; iI < GC.getNumBeliefInfos(); iI++)
	{
		BeliefTypes eBelief = (BeliefTypes)iI;
		if (eBelief == NO_BELIEF)
			continue;

		CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief);
		if (pBelief && pBelief->IsPantheonBelief())
		{
			if (bExcludeUnique)
			{
				if (pBelief->GetRequiredCivilization() == NO_CIVILIZATION)
				{
					iRtnValue++;
				}
			}
			else
			{
				iRtnValue++;
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
#if defined(MOD_ANY_PANTHEON)
		if (MOD_ANY_PANTHEON || !IsInSomeReligion(eBelief, ePlayer))
#else
		if (!IsInSomeReligion(eBelief, ePlayer))
#endif
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
	//zero? uh oh.
	if (availableBeliefs.size() <= 0)
	{
		for (int iI = 0; iI < iNumBeleifs; iI++)
		{
			const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));

			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if (pEntry && pEntry->IsPantheonBelief())
			{
				bool bAvailable = true;
#if defined(MOD_BALANCE_CORE)
				if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).GetPlayerTraits()->IsUniqueBeliefsOnly())
				{
					if (bUniqueExists)
					{
						if (pEntry->GetRequiredCivilization() != GET_PLAYER(ePlayer).getCivilizationType())
						{
							bAvailable = false;
						}
					}
				}

				if (pEntry->GetRequiredCivilization() != NO_CIVILIZATION)
				{
					if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getCivilizationType() != pEntry->GetRequiredCivilization())
					{
						bAvailable = false;
					}
				}
#endif
				if (bAvailable)
				{
					availableBeliefs.push_back(eBelief);
				}
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

#if defined(MOD_ANY_PANTHEON)
	if (MOD_ANY_PANTHEON || !IsInSomeReligion(eBelief, ePlayer))
#else
	if(!IsInSomeReligion(eBelief, ePlayer))
#endif
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
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
	if (GC.getGame().GetGameReligions()->GetNumPantheonsCreated() >= GC.getGame().GetGameReligions()->GetNumPantheonsPossible(true))
	{
		return true;
	}
#endif

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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
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
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			// Loop through each of their cities
			int iLoop = 0;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
				{
					iRtnValue++;
				}
			}
		}
	}
	return iRtnValue;
}

int CvGameReligions::GetNumDomesticCitiesFollowing(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	int iRtnValue = 0;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (!kPlayer.isAlive())
		return 0;

	// Loop through each of their cities
	int iLoop = 0;
	for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Has this player created a religion?
bool CvGameReligions::HasCreatedReligion(PlayerTypes ePlayer, bool bIgnoreLocal) const
{
	ReligionTypes eReligion = GET_PLAYER(ePlayer).GetReligions()->GetOwnedReligion();
    if (eReligion != NO_RELIGION)
	{
		if (MOD_RELIGION_LOCAL_RELIGIONS && bIgnoreLocal) 
		{
			return !(GC.getReligionInfo(eReligion)->IsLocalReligion());
		}

		return true;
	}

	return false;
}

/// Has this player reformed their religion?
bool CvGameReligions::HasAddedReformationBelief(PlayerTypes ePlayer) const
{
	ReligionTypes eReligion = GET_PLAYER(ePlayer).GetReligions()->GetOwnedReligion();
    if (eReligion != NO_RELIGION)
	{
		const CvReligion* pMyReligion = GetReligion(eReligion, ePlayer);
		if (pMyReligion && pMyReligion->m_bReformed)
		{
			return true;
		}
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

/// Get the religion this player created IF he currently owns it (can include pantheons)
ReligionTypes CvGameReligions::GetReligionCreatedByPlayer(PlayerTypes ePlayer, bool bIncludePantheon) const
{
	ReligionTypes eReligion = NO_RELIGION;
	ReligionList::const_iterator it;
	for(it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if(it->m_eFounder == ePlayer)
		{
			if (!bIncludePantheon && it->m_bPantheon)
				continue;
			
			CvCity* pHolyCity = it->GetHolyCity();
			if (pHolyCity && pHolyCity->getOwner()==ePlayer)
				return it->m_eReligion;
			// return pantheon if we can't find a religion that the player both founded and currently owns
			else if (it->m_bPantheon)
				eReligion = it->m_eReligion;
		}
	}
	return eReligion;
}

/// Get the pantheon this player created
ReligionTypes CvGameReligions::GetPantheonCreatedByPlayer(PlayerTypes ePlayer) const
{
	ReligionList::const_iterator it;
	for (it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (it->m_eFounder == ePlayer)
		{
			if (it->m_bPantheon)
			{
				return it->m_eReligion;
			}
		}
	}
	return NO_RELIGION;
}

/// Return the religion that this player created (player never loses control of pantheon, so don't use this function)
ReligionTypes CvGameReligions::GetOriginalReligionCreatedByPlayer(PlayerTypes ePlayer) const
{
	ReligionList::const_iterator it;
	for (it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (it->m_eFounder == ePlayer)
		{
			if (!it->m_bPantheon)
			{
				return it->m_eReligion;
			}
		}
	}
	return NO_RELIGION;
}

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
int CvGameReligions::GetNumReligionsStillToFound(bool bIgnoreLocal, PlayerTypes ePlayer) const
#else
int CvGameReligions::GetNumReligionsStillToFound() const
#endif
{
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	if (ePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(ePlayer).GetPlayerTraits()->IsAlwaysReligion() && GET_PLAYER(ePlayer).GetReligions()->GetStateReligion() <= RELIGION_PANTHEON)
		{
			if (GC.getMap().getWorldInfo().getMaxActiveReligions() - GetNumReligionsFounded(bIgnoreLocal) == 0)
				return 1;
		}
	}

	// VP: Max # of religions is based on number of players, not map size
	if (MOD_BALANCE_VP)
	{
		int iMaxReligions = GC.getGame().countMajorCivsEverAlive() * 100 / /*200*/ max(GD_INT_GET(RELIGION_MAXIMUM_PER_PLAYER_DIVISOR), 1);
		iMaxReligions += /*1*/ GD_INT_GET(RELIGION_MAXIMUM_FIXED_AMOUNT);
		return range(iMaxReligions, 1, /*8*/ GD_INT_GET(RELIGION_MAXIMUM_CAP)) - GetNumReligionsFounded(bIgnoreLocal);
	}

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
int CvGameReligions::GetAdjacentCityReligiousPressure(ReligionTypes eReligion, CvCity *pFromCity, CvCity *pToCity, int& iNumTradeRoutesInfluencing, bool bActualValue, 
	bool bPretendTradeConnection, bool bConnectedWithTrade, int iRelativeDistancePercent) //if bActualValue==false, then assume bPretendTradeConnection
{
	iNumTradeRoutesInfluencing = 0;

	//no pressure from pantheons
	if (eReligion <= RELIGION_PANTHEON)
	{
		return 0;
	}

	const CvReligion* pReligion = GetReligion(eReligion, pFromCity->getOwner());
	if (!pReligion)
	{
		return 0;
	}

	int iBasePressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
	int iPressureMod = 0;

	// India: +10% base pressure per follower
	if (GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsPopulationBoostReligion())
	{
		if (eReligion == GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetStateReligion(true))
		{
			int iPopExtraPressure = pFromCity->GetCityReligions()->GetNumFollowers(eReligion);
			int iBasePressureMod = min(35, iPopExtraPressure) * 10;
			iBasePressure *= 100 + iBasePressureMod;
			iBasePressure /= 100;
		}
	}

	// Does this city have a majority religion?
	ReligionTypes eMajorityReligion = pFromCity->GetCityReligions()->GetReligiousMajority();
	if (eMajorityReligion != eReligion)
	{
		return 0;
	}

	//do we have a trade route or pretend to have one
	if (bConnectedWithTrade || bPretendTradeConnection)
	{
		if (bActualValue)
			iNumTradeRoutesInfluencing++;

		int iTradeReligionModifer = GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->GetTradeReligionModifier();
		iTradeReligionModifer += GET_PLAYER(pFromCity->getOwner()).GetTradeReligionModifier();
		iTradeReligionModifer += pFromCity->GetReligiousTradeModifier();
		iTradeReligionModifer += pReligion->m_Beliefs.GetPressureChangeTradeRoute(pFromCity->getOwner());

		iPressureMod += iTradeReligionModifer;
	}
	else
	{
#if defined(MOD_RELIGION_PASSIVE_SPREAD_WITH_CONNECTION_ONLY)
		if (MOD_RELIGION_PASSIVE_SPREAD_WITH_CONNECTION_ONLY)
		{
			//no trade route and no city connection, no pressure!
			if (pFromCity->getOwner() != pToCity->getOwner() || !GET_PLAYER(pFromCity->getOwner()).IsCityConnectedToCity(pFromCity,pToCity))
				return 0;
		}
#endif

		//if there is no traderoute, base pressure falls off with distance
		int iPressurePercent = max(100 - iRelativeDistancePercent,1);
		//make the scaling quadratic - four times as many cities in range if we double the radius!
		iBasePressure = (iBasePressure*iPressurePercent*iPressurePercent) / (100*100);
	}

	// If we are spreading to a friendly city state, increase the effectiveness if we have the right belief
	if(IsCityStateFriendOfReligionFounder(eReligion, pToCity->getOwner()))
	{
		iPressureMod += pReligion->m_Beliefs.GetFriendlyCityStateSpreadModifier(pFromCity->getOwner());
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

		iPressureMod += iStrengthMod;
	}

	int iPolicyMod = GET_PLAYER(pFromCity->getOwner()).GetPressureMod();
	if (iPolicyMod != 0)
	{
		//If the faith being spread is our founded faith, or our adopted faith...
		if (eReligion == GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetStateReligion(true))
		{
			//...and the target city doesn't have our majority religion, we get the bonus.
			if (pToCity->GetCityReligions()->GetReligiousMajority() != eReligion)
				iPressureMod += iPolicyMod;
		}
	}

	// Strengthened spread from World Congress? (World Religion)
	iPressureMod += GC.getGame().GetGameLeagues()->GetReligionSpreadStrengthModifier(pFromCity->getOwner(), eReligion);

	// Building that boosts pressure from originating city?
	iPressureMod += pFromCity->GetCityReligions()->GetReligiousPressureModifier(eReligion);

	// Double pressure to vassals
	if (GET_TEAM(GET_PLAYER(pToCity->getOwner()).getTeam()).IsVassal(GET_PLAYER(pFromCity->getOwner()).getTeam()))
	{
		iPressureMod += 100;
	}

	if (MOD_RELIGION_CONVERSION_MODIFIERS) 
	{
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

			iPressureMod += iCityModifier + pToPlayer.GetConversionModifier() + pToPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CONVERSION_MODIFIER);
		}
	}

	int iPressure = iBasePressure * (100 + iPressureMod);

	// CUSTOMLOG("GetAdjacentCityReligiousPressure for %i from %s to %s is %i", eReligion, pFromCity->getName().c_str(), pToCity->getName().c_str(), iPressure);
	return max(0, iPressure / 100);
}

/// How much does this prophet cost (recursive)
int CvGameReligions::GetFaithGreatProphetNumber(int iNum) const
{
	int iRtnValue = 0;

	if(iNum >= 1)
	{
		if(iNum == 1)
		{
			iRtnValue = /*200 in CP, 800 in VP*/ GD_INT_GET(RELIGION_MIN_FAITH_FIRST_PROPHET);
		}
		else if(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && iNum == 1)
		{
			iRtnValue = /*600 in CP, 1200 in VP*/ GD_INT_GET(RELIGION_MIN_FAITH_SECOND_PROPHET);
		}
		else
		{
			iRtnValue = (/*100 in CP, 300 in VP*/ GD_INT_GET(RELIGION_FAITH_DELTA_NEXT_PROPHET) * (iNum - 1)) + GetFaithGreatProphetNumber(iNum - 1);
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
			iRtnValue = /*1000*/ GD_INT_GET(RELIGION_MIN_FAITH_FIRST_GREAT_PERSON);
		}
		else
		{
			iRtnValue = (/*500 in CP, 1500 in VP*/ GD_INT_GET(RELIGION_FAITH_DELTA_NEXT_GREAT_PERSON) * (iNum - 1)) + GetFaithGreatPersonNumber(iNum - 1);
		}
	}

	return iRtnValue;
}

/// Does the religion in nearby city give this battle winner a yield? If so return multipler of losing unit strength
int CvGameReligions::GetBeliefYieldForKill(YieldTypes eYield, int iX, int iY, PlayerTypes eWinningPlayer)
{
	int iRtnValue = 0;
	int iMultiplier = 0;
	int iLoop = 0;
	int iDistance = 0;
	CvCity* pLoopCity = NULL;
	ReligionTypes eReligion = NO_RELIGION;

	// Only Faith supported for now
	if(eYield != YIELD_FAITH)
	{
		return iRtnValue;
	}

	for(pLoopCity = GET_PLAYER(eWinningPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWinningPlayer).nextCity(&iLoop))
	{
		// Find religion in this city
		eReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();

		if (eReligion != NO_RELIGION && eReligion == GET_PLAYER(eWinningPlayer).GetReligions()->GetStateReligion(true))
		{
			// Find distance to this city
			iDistance = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());
			iMultiplier = GetReligion(eReligion, eWinningPlayer)->m_Beliefs.GetFaithFromKills(iDistance, eWinningPlayer, pLoopCity);

			if (iMultiplier > 0)
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
					if (iMultiplier > 0 && iDistance <= GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetMaxDistance())
					{
						// Just looking for one city providing this
						iRtnValue = iMultiplier;
						break;
					}	
				}
			}
		}
	}

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
	// mod for civs keeping their pantheon belief forever
	if (MOD_RELIGION_PERMANENT_PANTHEON)
	{
		if (HasCreatedPantheon(eWinningPlayer))
		{
			const CvReligion* pPantheon = GetReligion(RELIGION_PANTHEON, eWinningPlayer);
			BeliefTypes ePantheonBelief = GetBeliefInPantheon(eWinningPlayer);
			if (pPantheon != NULL && ePantheonBelief != NO_BELIEF)
			{
				const CvReligion* pReligion = GetReligion(eReligion, eWinningPlayer);
				if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eReligion, eWinningPlayer))) // check that the our religion does not have our belief, to prevent double counting
				{
					iRtnValue += MAX(0, pPantheon->m_Beliefs.GetFaithFromKills(iDistance, eWinningPlayer, pLoopCity));
				}
			}
		}
	}
#endif

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
	UnitTypes eUnit = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);

	if (eUnit == NO_UNIT)
	{
		return false;
	}

	// Don't check this in Classical (for Byzantium) or Industrial (for other civs)
	if (kPlayer.GetCurrentEra() >= GetFaithPurchaseGreatPeopleEra(&kPlayer))
		return false;

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

	ReligionTypes ePlayerReligion = GET_PLAYER(kPlayer.GetID()).GetReligions()->GetOwnedReligion();
	if (ePlayerReligion > RELIGION_PANTHEON)
	{
		pReligion = GetReligion(ePlayerReligion, kPlayer.GetID());
	}

	// If player hasn't founded a religion yet, drop out of this if all religions have been founded
	else if (GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
	{
		return false;
	}

	if(iFaith < iCost)
	{
		return false;
	}

	int iChance = /*5 in CP, 100 in VP*/ GD_INT_GET(RELIGION_BASE_CHANCE_PROPHET_SPAWN);

#if defined(MOD_RELIGION_KEEP_PROPHET_OVERFLOW)
	int iBaseChance = iChance;
#endif

	iChance += (iFaith - iCost);

	int iRand = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x88a8fa44).mix(kPlayer.GetID()));
	if (iRand > iChance)
		return false;

	CvCity* pSpawnCity = pReligion ? pReligion->GetHolyCity() : NULL;
	if(pSpawnCity != NULL && pSpawnCity->getOwner() == kPlayer.GetID())
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
		if(MOD_NO_AUTO_SPAWN_PROPHET)
		{
			if (kPlayer.isHuman())
			{
				switch (kPlayer.GetFaithPurchaseType())
				{
					case FAITH_PURCHASE_BUILDING:
					case FAITH_PURCHASE_UNIT:
						break; // Player is saving for something else so just do nothing.
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
		//Random GP spawn/holy city.
		CvCity* pBestCity = NULL;
		if (MOD_BALANCE_CORE_RANDOMIZED_GREAT_PROPHET_SPAWNS)
		{
			int iBestWeight = 0;

			int iTempWeight = 0;

			CvCity* pLoopCity = NULL;
			int iLoop = 0;
			CvGame& theGame = GC.getGame();
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				iTempWeight = pLoopCity->GetFaithPerTurn() * 5;
				iTempWeight += theGame.randRangeExclusive(0, 15, CvSeeder(kPlayer.GetPseudoRandomSeed()).mix(iLoop));

				if(iTempWeight > iBestWeight)
				{
					iBestWeight = iTempWeight;
					pBestCity = pLoopCity;
				}
			}
		}
		if (pBestCity != NULL)
		{
			pSpawnCity = pBestCity;
		}
		else
		{
			pSpawnCity = kPlayer.getCapitalCity();
		}

		if(pSpawnCity != NULL)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
#if defined(MOD_NO_AUTO_SPAWN_PROPHET)
			if(MOD_NO_AUTO_SPAWN_PROPHET)
			{
				if (kPlayer.isHuman())
				{
					switch (kPlayer.GetFaithPurchaseType())
					{
						case FAITH_PURCHASE_BUILDING:
						case FAITH_PURCHASE_UNIT:
							break; // Player is saving for something else so just do nothing.
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
		FILogFile* pLog = NULL;

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
	case FOUNDING_NO_BELIEFS_AVAILABLE: // <- No localization key exists for this result.
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

///
template<typename GameReligions, typename Visitor>
void CvGameReligions::Serialize(GameReligions& gameReligions, Visitor& visitor)
{
	visitor(gameReligions.m_iMinimumFaithForNextPantheon);
	visitor(gameReligions.m_CurrentReligions);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameReligions& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameReligions::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameReligions& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameReligions::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvPlayerReligions
//=====================================
/// Constructor
CvPlayerReligions::CvPlayerReligions(void):
	m_pPlayer(NULL),
	m_iFaithAtLastNotify(0),
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	m_iNumFreeProphetsSpawned(0),
#endif
	m_eMajorityReligion(NO_RELIGION),
	m_eStateReligionOverride(NO_RELIGION),
	m_eStateReligion(NO_RELIGION),
	m_bOwnsStateReligion(false),
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
	m_iFaithAtLastNotify = 0;
	m_eMajorityReligion = NO_RELIGION;
	m_eStateReligionOverride = NO_RELIGION;
	m_eStateReligion = NO_RELIGION;
	m_bOwnsStateReligion = false;
}

///
template<typename PlayerReligions, typename Visitor>
void CvPlayerReligions::Serialize(PlayerReligions& playerReligions, Visitor& visitor)
{
	visitor(playerReligions.m_iNumFreeProphetsSpawned);
	visitor(playerReligions.m_iNumProphetsSpawned);
	visitor(playerReligions.m_bFoundingReligion);
	visitor(playerReligions.m_eMajorityReligion);
	visitor(playerReligions.m_eStateReligionOverride);
	visitor(playerReligions.m_eStateReligion);
	visitor(playerReligions.m_bOwnsStateReligion);
	visitor(playerReligions.m_iFaithAtLastNotify);
}

/// Serialization read
void CvPlayerReligions::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvPlayerReligions::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
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

FDataStream& operator>>(FDataStream& stream, CvPlayerReligions& playerReligions)
{
	playerReligions.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvPlayerReligions& playerReligions)
{
	playerReligions.Write(stream);
	return stream;
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
	ReligionTypes ePlayerReligion = GetStateReligion();
	if (bIncludeBeliefDiscounts)
	{
		const CvReligion* pReligion = NULL;
		pReligion = GC.getGame().GetGameReligions()->GetReligion(ePlayerReligion, m_pPlayer->GetID());
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			int iProphetCostMod = pReligion->m_Beliefs.GetProphetCostModifier(m_pPlayer->GetID(), pHolyCity);
			iProphetCostMod += m_pPlayer->GetPlayerTraits()->GetFaithCostModifier();

			if (iProphetCostMod != 0)
			{
				iCost *= (100 + iProphetCostMod);
				iCost /= 100;
			}
		}
	}

	UnitClassTypes eUnitClassProphet = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_PROPHET");
	int iMod = m_pPlayer->GetPlayerTraits()->GetGreatPersonCostReduction(GetGreatPersonFromUnitClass(eUnitClassProphet));
	if (iMod != 0)
	{
		iCost *= (100 + iMod);
		iCost /= 100;
	}

	if (bAdjustForSpeedDifficulty)
	{
		// Adjust for game speed
		iCost *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iCost /= 100;

		// Adjust for difficulty
		if (m_pPlayer->isMajorCiv())
		{
			iCost *= m_pPlayer->getHandicapInfo().getProphetPercent();
			iCost /= 100;

			if (!m_pPlayer->isHuman())
			{
				iCost *= GC.getGame().getHandicapInfo().getAIProphetPercent();
				iCost /= 100;
			}
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
	return GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(m_pPlayer->GetID(), bIncludePantheon);
}

ReligionTypes CvPlayerReligions::GetOriginalReligionCreatedByPlayer() const
{
	return GC.getGame().GetGameReligions()->GetOriginalReligionCreatedByPlayer(m_pPlayer->GetID());
}

/// Does this player have enough faith to buy a religious unit or building?
bool CvPlayerReligions::CanAffordFaithPurchase(int iMinimumFaith) const
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
					if (iCost != 0 && iFaith > iCost && iCost > iMinimumFaith)
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
					if (iCost != 0 && iFaith > iCost && iCost > iMinimumFaith)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

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

/// Does this player have a city following a religion?
bool CvPlayerReligions::HasCityWithMajorityReligion(ReligionTypes eReligionToCheck) const
{
	int iLoop = 0;
	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pCity->GetCityReligions()->GetReligiousMajority() == eReligionToCheck)
			return true;
		
		if (eReligionToCheck == NO_RELIGION && pCity->GetCityReligions()->GetReligiousMajority() > RELIGION_PANTHEON)
			return true;
	}

	return false;
}

/// Is this player happily following this other player's religion?
bool CvPlayerReligions::HasOthersReligionInMostCities(PlayerTypes eOtherPlayer) const
{
	// Not happy about it if have their own religion
	if (HasCreatedReligion())
	{
		return false;
	}

	ReligionTypes eOtherReligion = GET_PLAYER(eOtherPlayer).GetReligions()->GetOwnedReligion();
	if (eOtherReligion == NO_RELIGION)
	{
		return false;
	}

	return GetStateReligion() == eOtherReligion;
}

/// Do a majority of this player's cities follow a specific religion?
bool CvPlayerReligions::HasReligionInMostCities(ReligionTypes eReligion) const
{
	if (eReligion == NO_RELIGION)
		return false;

	int iNumFollowingCities = 0;
	int iLoop = 0;
	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pCity->IsIgnoreCityForHappiness())
			continue;

		if (pCity->GetCityReligions()->GetReligiousMajority() == eReligion)
			iNumFollowingCities++;
	}

	// Require a true majority
	return (iNumFollowingCities * 2 > m_pPlayer->getNumCities());
}

/// What religion is followed in a majority of our cities?
ReligionTypes CvPlayerReligions::GetReligionInMostCities() const
{
	return m_eMajorityReligion;
}

/// What is our state religion?
ReligionTypes CvPlayerReligions::GetStateReligion(bool bIncludePantheon) const
{
	if (!bIncludePantheon && m_eStateReligion == RELIGION_PANTHEON)
		return NO_RELIGION;

	return m_eStateReligion;
}

// Do we own the holy city of our state religion?
ReligionTypes CvPlayerReligions::GetOwnedReligion() const
{
	if (!m_bOwnsStateReligion || m_eStateReligion == RELIGION_PANTHEON)
		return NO_RELIGION;

	return m_eStateReligion;
}

/// What is our state religion?
bool CvPlayerReligions::UpdateStateReligion()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();
	bool bOwnsReligion = false;

	//if we have a forced state religion, just use that
	if (m_eStateReligionOverride != NO_RELIGION)
	{
		CvCity* pHolyCity = GC.getGame().GetGameReligions()->GetReligion(m_eStateReligionOverride, ePlayer)->GetHolyCity();
		if (pHolyCity && pHolyCity->getOwner() == ePlayer)
			bOwnsReligion = true;
		return SetStateReligion(m_eStateReligionOverride, bOwnsReligion);
	}

	//by default use the religion we founded (if we still control it)
	ReligionTypes eNewStateReligion = GetReligionCreatedByPlayer();

	if (eNewStateReligion == NO_RELIGION)
	{
		//We didn't found a religion, or we lost our holy city? Okay, let's see if we've conquered any holy cities
		const ReligionList& allReligions = GC.getGame().GetGameReligions()->m_CurrentReligions;
		vector<OptionWithScore<ReligionTypes>> vHolyReligions;
		for (ReligionList::const_iterator it = allReligions.begin(); it != allReligions.end(); it++)
		{
			//We own this holy city? It is ours to work with now.
			if (it->m_eReligion != RELIGION_PANTHEON)
			{
				CvCity* pHolyCity = it->GetHolyCity();
				if (pHolyCity && pHolyCity->getOwner() == ePlayer)
				{
					int iValue = GetNumDomesticFollowers(it->m_eReligion);
					vHolyReligions.push_back(OptionWithScore<ReligionTypes>(it->m_eReligion, iValue));
				}
			}
		}

		if (!vHolyReligions.empty())
		{
			std::stable_sort(vHolyReligions.begin(), vHolyReligions.end());
			eNewStateReligion = vHolyReligions.front().option;
		}
	}

	if (eNewStateReligion == NO_RELIGION)
	{
		//No holy cities at all ... use our majority religion
		eNewStateReligion = GetReligionInMostCities();
	}
	else
	{
		// We own the holy city of our state religion based on previous calculations
		bOwnsReligion = true;
	}
	return SetStateReligion(eNewStateReligion, bOwnsReligion);
}

bool CvPlayerReligions::SetStateReligion(ReligionTypes eNewStateReligion, bool bOwnsReligion)
{
	//we may own our state religion's holy city now
	m_bOwnsStateReligion = bOwnsReligion;
	//no change, nothing to do
	if (GetStateReligion() == eNewStateReligion)
		return false;

	if(GetStateReligion() == NO_RELIGION)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_StateReligionAdopted, m_pPlayer->GetID(), eNewStateReligion, GetStateReligion());
	}
	else
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_StateReligionChanged, m_pPlayer->GetID(), eNewStateReligion, GetStateReligion());
	}

	// Message slightly different for founder player
	if (MOD_BALANCE_CORE_BELIEFS && m_pPlayer->GetNotifications() && bOwnsReligion)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eNewStateReligion, m_pPlayer->GetID());
		if (pReligion)
		{
			CvString szReligionName = pReligion->GetName();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_STATE_RELIGION_S");
			strSummary << szReligionName;
			Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_NEW_PLAYER_STATE_RELIGION");
			localizedText << szReligionName;
			m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eNewStateReligion, -1);
		}
		// Compensate zeroing of faith with golden age points
		if (m_pPlayer->GetFaith() > 0 && !m_pPlayer->GetPlayerTraits()->IsAlwaysReligion())
		{
			m_pPlayer->ChangeGoldenAgeProgressMeter(m_pPlayer->GetFaith());
			m_pPlayer->changeInstantYieldValue(YIELD_GOLDEN_AGE_POINTS, m_pPlayer->GetFaith());
			m_pPlayer->SetFaith(0);
		}
	}

	m_eStateReligion = eNewStateReligion;
	return true;
}

void CvPlayerReligions::SetStateReligionOverride(ReligionTypes eReligion)
{
	m_eStateReligionOverride = eReligion;

	UpdateStateReligion();
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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
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

/// What religion is followed in a majority of our cities?
bool CvPlayerReligions::ComputeMajority(bool bNotifications)
{
	for (int iI = RELIGION_PANTHEON; iI < GC.GetGameReligions()->GetNumReligions(); iI++)
	{
		ReligionTypes eReligion = (ReligionTypes)iI;
		if (HasReligionInMostCities(eReligion))
		{
#if defined(MOD_BALANCE_CORE)
			//New state faith? Let's announce this.
			if(bNotifications && m_eMajorityReligion != eReligion && m_eMajorityReligion != NO_RELIGION)
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
						m_pPlayer->GetNotifications()->Add(NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER, localizedText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
					}
				}
			}
#endif
			m_eMajorityReligion = eReligion;
			return true;
		}
	}
	m_eMajorityReligion = NO_RELIGION;
	return false;
}

/// Does this player get a default influence boost with city states following this religion?
int CvPlayerReligions::GetCityStateMinimumInfluence(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	int iMinInfluence = 0;

	ReligionTypes eFounderBenefitReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eReligion == eFounderBenefitReligion && eFounderBenefitReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFounderBenefitReligion, ePlayer);
		if(pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iMinInfluence += pReligion->m_Beliefs.GetCityStateMinimumInfluence(ePlayer, pHolyCity);
		}
	}

	return iMinInfluence;
}

/// Does this player get a modifier to city state influence boosts?
int CvPlayerReligions::GetCityStateInfluenceModifier(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if(pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iRtnValue += pReligion->m_Beliefs.GetCityStateInfluenceModifier(ePlayer, pHolyCity);
		}
	}
	return iRtnValue;
}

int CvPlayerReligions::GetCityStateYieldModifier(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iRtnValue += pReligion->m_Beliefs.GetCSYieldBonus(ePlayer, pHolyCity);
		}
	}
	return iRtnValue;
} 

/// Does this player get religious pressure from spies?
int CvPlayerReligions::GetSpyPressure(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iRtnValue += pReligion->m_Beliefs.GetSpyPressure(ePlayer, pHolyCity);
		}
	}
	return iRtnValue;
}

/// How many foreign cities are following a religion we founded?
int CvPlayerReligions::GetNumForeignCitiesFollowing(ReligionTypes eReligion) const
{
	CvCity *pLoopCity = NULL;
	int iCityLoop = 0;
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
	CvCity *pLoopCity = NULL;
	int iCityLoop = 0;
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

int CvPlayerReligions::GetNumCityStateFollowers(ReligionTypes eReligion) const
{
	CvCity *pLoopCity = NULL;
	int iCityLoop = 0;
	int iRtnValue = 0;
	
	if (eReligion > RELIGION_PANTHEON)
	{
		for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
			if (kLoopPlayer.isAlive() && kLoopPlayer.isMinorCiv())
			{
				for (pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
				{
					iRtnValue += pLoopCity->GetCityReligions()->GetNumFollowers(eReligion);
				}
			}
		}
	}

	return iRtnValue;
}

int CvPlayerReligions::GetNumDomesticFollowers(ReligionTypes eReligion) const
{
	int iRtnValue = 0;
	
	if (eReligion > RELIGION_PANTHEON)
	{
		int iCityLoop = 0;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if (pLoopCity->IsIgnoreCityForHappiness())
				continue;

			iRtnValue += pLoopCity->GetCityReligions()->GetNumFollowers(eReligion);
		}
	}

	return iRtnValue;
}

//=====================================
// CvCityReligions
//=====================================
/// Constructor
CvCityReligions::CvCityReligions(void):
	m_pCity(NULL),
	m_eMajorityCityReligion(NO_RELIGION),
	m_pMajorityReligionCached(NULL)
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

	m_ReligionStatus.clear();
	m_eMajorityCityReligion = NO_RELIGION;
	m_pMajorityReligionCached = NULL;
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

/// Is this the Holy City for a specific religion?
bool CvCityReligions::IsHolyCityForReligion(ReligionTypes eReligion)
{
	if (eReligion == NO_RELIGION)
		return false;

	return eReligion == GC.getGame().GetGameReligions()->GetHolyCityReligion(m_pCity);
}

/// Is this the Holy City for any religion?
bool CvCityReligions::IsHolyCityAnyReligion()
{
	return NO_RELIGION != GC.getGame().GetGameReligions()->GetHolyCityReligion(m_pCity);
}

/// What is the religion of this Holy City?
ReligionTypes CvCityReligions::GetReligionForHolyCity()
{
	return GC.getGame().GetGameReligions()->GetHolyCityReligion(m_pCity);
}

/// Is there a "heretical" religion here that can be stomped out?
bool CvCityReligions::IsReligionHereOtherThan(ReligionTypes eReligion, int iMinFollowers)
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if (it->m_eReligion != NO_RELIGION && it->m_eReligion != eReligion && it->m_iFollowers >= iMinFollowers)
		{
			return true;
		}
	}
	return false;
}

/// Is there an inquisitor from our religion here?
bool CvCityReligions::IsDefendedByOurInquisitor(ReligionTypes eReligion, CvUnit* pIgnoreUnit)
{
	if (eReligion == NO_RELIGION)
		return false;

	PlayerTypes eCityOwner = m_pCity->getOwner();

	for (int i=0; i<RING1_PLOTS; i++)
	{
		CvPlot* pAdjacentPlot = iterateRingPlots(m_pCity->plot(),i);
		if (!pAdjacentPlot)
			continue;

		for (int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
		{
			CvUnit* pLoopUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
			if (pLoopUnit == NULL || pIgnoreUnit == pLoopUnit)
				continue;

			// Only consider Inquisitors
			CvUnitEntry* pkEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
			if (!pkEntry || !pkEntry->IsProhibitsSpread())
				continue;

			// Ignore any units that belong to someone else
			PlayerTypes eUnitOwner = pLoopUnit->getOwner();
			if (eUnitOwner != eCityOwner)
			{
				bool bAllyUnit = false;
				if (MOD_RELIGION_ALLIED_INQUISITORS && GET_PLAYER(eCityOwner).isMinorCiv())
				{
					PlayerTypes eAlly = GET_PLAYER(eCityOwner).GetMinorCivAI()->GetAlly();
					if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eUnitOwner).getTeam())
						bAllyUnit = true;
				}
				if (!bAllyUnit)
					continue;
			}

			// Inquisitor's religion must match the desired religion
			ReligionTypes eInquisitorReligion = pLoopUnit->GetReligionData()->GetReligion();
			if (eInquisitorReligion == eReligion)
				return true;
		}
	}

	return false;
}

/// Is there an inquisitor from another religion here?
bool CvCityReligions::IsDefendedAgainstSpread(ReligionTypes eReligion)
{
	PlayerTypes eCityOwner = m_pCity->getOwner();

	for (int i=0; i<RING1_PLOTS; i++)
	{
		CvPlot* pAdjacentPlot = iterateRingPlots(m_pCity->plot(),i);
		if (!pAdjacentPlot)
			continue;

		for (int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
		{
			CvUnit* pLoopUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
			if (pLoopUnit == NULL)
				continue;

			// Only consider Inquisitors
			CvUnitEntry* pkEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
			if (!pkEntry || !pkEntry->IsProhibitsSpread())
				continue;

			// Ignore any units that belong to someone else
			PlayerTypes eUnitOwner = pLoopUnit->getOwner();
			if (eUnitOwner != eCityOwner)
			{
				bool bAllyUnit = false;
				if (MOD_RELIGION_ALLIED_INQUISITORS && GET_PLAYER(eCityOwner).isMinorCiv())
				{
					PlayerTypes eAlly = GET_PLAYER(eCityOwner).GetMinorCivAI()->GetAlly();
					if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eUnitOwner).getTeam())
						bAllyUnit = true;
				}
				if (!bAllyUnit)
					continue;
			}

			// Inquisitor's religion must be different from the other religion
			ReligionTypes eInquisitorReligion = pLoopUnit->GetReligionData()->GetReligion();
			if (eInquisitorReligion == NO_RELIGION || eReligion == eInquisitorReligion)
				continue;

			return true;
		}
	}

	return false;
}

/// Is there a missionary from another religion near here? Get em!
bool CvCityReligions::IsForeignMissionaryNearby(ReligionTypes eReligion)
{
	for (int i=0; i<RING3_PLOTS; i++)
	{
		CvPlot* pPlot = iterateRingPlots(m_pCity->plot(),i);
		if (!pPlot)
			continue;

		for (int iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
		{
			CvUnit* pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
			CvUnitEntry* pkEntry = GC.getUnitInfo(pLoopUnit->getUnitType());
			if (pkEntry && pkEntry->IsSpreadReligion())
			{
				if (pLoopUnit->getOwner() != m_pCity->getOwner() && pLoopUnit->GetReligionData()->GetReligion() != eReligion)
				{
					return true;
				}
			}
		}
	}

	return false;
}

ReligionTypes CvCityReligions::GetReligiousMajority() const
{
	return m_eMajorityCityReligion;
}

bool CvCityReligions::ComputeReligiousMajority(bool bNotifications)
{
	int iTotalFollowers = 0;
	int iMostFollowerPressure = 0;
	int iMostFollowers = -1;
	ReligionTypes eMostFollowers = NO_RELIGION;
	ReligionInCityList::iterator religionIt;

	//for sanity check
	int iMaxPressure = 0;
	ReligionTypes eMaxPressure = NO_RELIGION;

	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		iTotalFollowers += religionIt->m_iFollowers;

		if(religionIt->m_iFollowers > iMostFollowers || religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostFollowerPressure)
		{
			iMostFollowers = religionIt->m_iFollowers;
			iMostFollowerPressure = religionIt->m_iPressure;
			eMostFollowers = religionIt->m_eReligion;
		}

		if (religionIt->m_iPressure > iMaxPressure)
		{
			iMaxPressure = religionIt->m_iPressure;
			eMaxPressure = religionIt->m_eReligion;
		}
	}

	if (eMostFollowers != eMaxPressure)
		OutputDebugString("city religion state inconsistent!\n");

	//update local majority
	ReligionTypes eOldMajority = m_eMajorityCityReligion;

	m_eMajorityCityReligion = (iMostFollowers*2 >= iTotalFollowers) ? eMostFollowers : NO_RELIGION;

	//update player majority
	if (m_eMajorityCityReligion != eOldMajority)
	{
		m_pMajorityReligionCached = NULL; //reset this
		GET_PLAYER(m_pCity->getOwner()).GetReligions()->ComputeMajority(bNotifications);
	}

	return (m_eMajorityCityReligion != NO_RELIGION);
}

const CvReligion * CvCityReligions::GetMajorityReligion()
{
	if (m_eMajorityCityReligion != NO_RELIGION && m_pMajorityReligionCached == NULL)
		m_pMajorityReligionCached = GC.getGame().GetGameReligions()->GetReligion(m_eMajorityCityReligion, m_pCity->getOwner());

	return m_pMajorityReligionCached;
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

/// What is the Nth most popular religion in this city with a majority religion?
ReligionTypes CvCityReligions::GetReligionByAccumulatedPressure(size_t iIndex) const
{
	//RecomputeFollowers orders religions by pressure!
	if (iIndex < m_ReligionStatus.size())
		return m_ReligionStatus[iIndex].m_eReligion;

	return NO_RELIGION;
}

/// Is there a pantheon belief in the secondary religion here?
BeliefTypes CvCityReligions::GetSecondaryReligionPantheonBelief()
{
	BeliefTypes eRtnValue = NO_BELIEF;

	// Check for the policy that allows a secondary religion to be active
	if (GET_PLAYER(m_pCity->getOwner()).IsSecondReligionPantheon())
	{
		ReligionTypes eSecondary = GetReligionByAccumulatedPressure(1);
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
int CvCityReligions::GetFollowersOtherReligions(ReligionTypes eReligion, bool bIncludePantheons)
{
	int iOtherFollowers = 0;
	ReligionInCityList::iterator religionIt;

	for(religionIt = m_ReligionStatus.begin(); religionIt != m_ReligionStatus.end(); ++religionIt)
	{
		if (bIncludePantheons)
		{
			if (religionIt->m_eReligion >= RELIGION_PANTHEON && religionIt->m_eReligion != eReligion)
			{
				iOtherFollowers += religionIt->m_iFollowers;
			}
		}
		else if (religionIt->m_eReligion > RELIGION_PANTHEON && religionIt->m_eReligion != eReligion)
		{
			iOtherFollowers += religionIt->m_iFollowers;
		}
	}

	return iOtherFollowers;
}

int CvCityReligions::GetReligiousPressureModifier(ReligionTypes eReligion) const
{
	return m_pCity->GetReligiousPressureModifier(eReligion);
}
void CvCityReligions::SetReligiousPressureModifier(ReligionTypes eReligion, int iNewValue)
{
	m_pCity->SetReligiousPressureModifier(eReligion, iNewValue);
}
void CvCityReligions::ChangeReligiousPressureModifier(ReligionTypes eReligion, int iNewValue)
{
	m_pCity->ChangeReligiousPressureModifier(eReligion, iNewValue);
}

/// Total pressure exerted by all religions
int CvCityReligions::GetTotalAccumulatedPressure(bool bIncludePantheon) const
{
	int iTotalPressure = 0;

	ReligionInCityList::const_iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if (!bIncludePantheon && it->m_eReligion <= RELIGION_PANTHEON)
			continue;

		iTotalPressure += it->m_iPressure;
	}

	return iTotalPressure;
}

/// Pressure exerted by one religion
int CvCityReligions::GetPressureAccumulated(ReligionTypes eReligion) const
{
	ReligionInCityList::const_iterator it;
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
int CvCityReligions::GetPressurePerTurn(ReligionTypes eReligion, int* piNumSourceCities)
{
	int iPressure = 0;
	int iCount = 0;
	
	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			// Loop through each of their cities
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				// Ignore the same city
				if (m_pCity == pLoopCity)
					continue;

				if (pLoopCity->GetCityReligions()->GetNumFollowers(eReligion) <= 0)
					continue;

				if (!GC.getGame().GetGameReligions()->IsValidTarget(eReligion, pLoopCity, m_pCity))
					continue;

				//it would be nice to use CvGameTrade::GetAllPotentialTradeRoutesFromCity() for each of our cities
				//to save the loop over all players, but unfortunately we also need to check incoming trade routes
				bool bConnectedWithTrade = false;
				int iRelativeDistancePercent = 0;
				if (!GC.getGame().GetGameReligions()->IsCityConnectedToCity(eReligion, pLoopCity, m_pCity, bConnectedWithTrade, iRelativeDistancePercent))
					continue;

				int iNumTradeRoutes = 0;
				int iNewPressure = GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure(eReligion, pLoopCity, m_pCity, iNumTradeRoutes, false, false, bConnectedWithTrade, iRelativeDistancePercent);

				if (iNewPressure > 0)
				{
					iPressure += iNewPressure;
					iCount++;
				}
			}

			// Include any pressure from "Underground Sects"
			if (eReligion > RELIGION_PANTHEON && kPlayer.GetReligions()->GetStateReligion() == eReligion)
			{
				int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
				if (iSpyPressure > 0)
				{
					if (kPlayer.GetEspionage()->GetSpyIndexInCity(m_pCity) != -1)
					{
						iPressure += iSpyPressure * max(1, GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
					}
				}
			}
		}
	}

	// Holy city for this religion?
	if (IsHolyCityForReligion(eReligion))
	{
		int iHolyCityPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
		iHolyCityPressure *=  /*5*/ GD_INT_GET(RELIGION_PER_TURN_FOUNDING_CITY_PRESSURE);
		iPressure += iHolyCityPressure;
	}
	
	if (piNumSourceCities)
		*piNumSourceCities = iCount;

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

	if (!GC.getGame().GetGameReligions()->IsValidTarget(eReligion, pTargetCity, m_pCity))
	{
		iAmount = 0;
		return false;
	}
	
	int iNumTradeRoutes = 0;

	bool bConnectedWithTrade = false;
	int iRelativeDistancePercent = 0;
	GC.getGame().GetGameReligions()->IsCityConnectedToCity(eReligion, m_pCity, pTargetCity, bConnectedWithTrade, iRelativeDistancePercent);

	int iWithTR = GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure(eReligion, m_pCity, pTargetCity, iNumTradeRoutes, false, true, bConnectedWithTrade, iRelativeDistancePercent);
	int iNoTR = GC.getGame().GetGameReligions()->GetAdjacentCityReligiousPressure(eReligion, m_pCity, pTargetCity, iNumTradeRoutes, false, false, bConnectedWithTrade, iRelativeDistancePercent);

	iAmount = (iWithTR - iNoTR);
	return iAmount>0;
}


/// Handle a change in the city population
void CvCityReligions::DoPopulationChange(int iChange)
{
	// Only add pressure if the population went up
	if(iChange > 0)
		AddReligiousPressure(FOLLOWER_CHANGE_POP_CHANGE, GetReligiousMajority(), iChange * /*1000*/ GD_INT_GET(RELIGION_ATHEISM_PRESSURE_PER_POP));

	RecomputeFollowers(FOLLOWER_CHANGE_POP_CHANGE);
	m_pCity->GetCityCitizens()->SetDirty(true);
}

/// Note that a religion was founded here
void CvCityReligions::DoReligionFounded(ReligionTypes eReligion)
{
	int iInitialPressure = m_pCity->getPopulation() * /*5000*/ GD_INT_GET(RELIGION_INITIAL_FOUNDING_CITY_PRESSURE);
	AddReligiousPressure(FOLLOWER_CHANGE_RELIGION_FOUNDED, eReligion, iInitialPressure);
	RecomputeFollowers(FOLLOWER_CHANGE_RELIGION_FOUNDED);
}

/// Prophet spread is very powerful: eliminates all existing religions and adds to his
void CvCityReligions::AddMissionarySpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer)
{
	//some missionaries are mini inquisitors 
	const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
	int iErosion = pReligion ? pReligion->m_Beliefs.GetOtherReligionPressureErosion(eResponsiblePlayer) : 0;
	ErodeOtherReligiousPressure(FOLLOWER_CHANGE_MISSIONARY, eReligion, iErosion, false, true, eResponsiblePlayer);
	AddReligiousPressure(FOLLOWER_CHANGE_MISSIONARY, eReligion, iPressure, eResponsiblePlayer);
	RecomputeFollowers(FOLLOWER_CHANGE_MISSIONARY, eResponsiblePlayer);
}


/// Prophet spread is very powerful: eliminates all existing religions and adds to his
void CvCityReligions::AddProphetSpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer)
{
	ErodeOtherReligiousPressure(FOLLOWER_CHANGE_PROPHET, eReligion, 100, true, true, eResponsiblePlayer);
	AddReligiousPressure(FOLLOWER_CHANGE_PROPHET, eReligion, iPressure, eResponsiblePlayer);
	RecomputeFollowers(FOLLOWER_CHANGE_PROPHET, eResponsiblePlayer);
}

const char* GetFollowerChangeString(CvReligiousFollowChangeReason eReason) 
{
	switch (eReason)
	{
		case FOLLOWER_CHANGE_POP_CHANGE:
			return "POP_CHANGE";
		case FOLLOWER_CHANGE_HOLY_CITY:
			return "HOLY_CITY";
		case FOLLOWER_CHANGE_ADJACENT_PRESSURE:
			return "ADJACENT_PRESSURE";
		case FOLLOWER_CHANGE_RELIGION_FOUNDED:
			return "RELIGION_FOUNDED";
		case FOLLOWER_CHANGE_PANTHEON_FOUNDED:
			return "PANTHEON_FOUNDED";
		case FOLLOWER_CHANGE_CONQUEST:
			return "CONQUEST";
		case FOLLOWER_CHANGE_MISSIONARY:
			return "MISSIONARY";
		case FOLLOWER_CHANGE_PROPHET:
			return "PROPHET";
		case FOLLOWER_CHANGE_REMOVE_HERESY:
			return "REMOVE_HERESY";
		case FOLLOWER_CHANGE_SCRIPTED_CONVERSION:
			return "SCRIPTED_CONVERSION";
		case FOLLOWER_CHANGE_SPY_PRESSURE:
			return "SPY_PRESSURE";
		case FOLLOWER_CHANGE_INSTANT_YIELD:
			return "INSTANT_YIELD";
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
		case FOLLOWER_CHANGE_ADOPT_FULLY:
			return "ADOPT_FULLY";
#endif
#if defined(MOD_BALANCE_CORE_PANTHEON_RESET_FOUND)
		case FOLLOWER_CHANGE_PANTHEON_OBSOLETE:
			return "PANTHEON_OBSOLETE";
#endif
		default:
			return "unknown_reason";
	}
};

void CvCityReligions::LogPressureChange(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressureChange, int iAccPressure, PlayerTypes eResponsiblePlayer)
{
	//do not log passive effects, it's too much ...
	if (eReason == FOLLOWER_CHANGE_ADJACENT_PRESSURE || eReason == FOLLOWER_CHANGE_HOLY_CITY)
		return;

	if (GC.getLogging() && GC.getAILogging())
	{
		FILogFile* pLog = LOGFILEMGR.GetLog("ReligiousPressureLog.csv", FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite);

		CvString strLog;
		CvString strCivName = GET_PLAYER(m_pCity->getOwner()).getNameKey();
		strLog.Format("%03d, %s, ", GC.getGame().getElapsedGameTurns(), strCivName.c_str());

		CvString strMsg;
		CvString strCityName = m_pCity->getNameNoSpace();

		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pCity->getOwner());
		CvString strRelName = pReligion ? pReligion->GetName() : "NONE";

		strMsg.Format("City, %s, Religion, %s, Acc, %d, Reason, %s, Change, %d, Responsible, %d",
			strCityName.c_str(), strRelName.c_str(), iAccPressure, GetFollowerChangeString(eReason), iPressureChange, eResponsiblePlayer);

		strLog += strMsg;
		pLog->Msg(strLog);
	}
}

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressureChange, PlayerTypes eResponsiblePlayer)
{
	bool bExisting = false;

	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			it->m_iPressure += iPressureChange;
			bExisting = true;

			LogPressureChange(eReason, eReligion, iPressureChange, it->m_iPressure, eResponsiblePlayer);
		}
		// If this is pressure from a real religion, reduce presence of pantheon by the same amount
		else if(eReligion > RELIGION_PANTHEON && it->m_eReligion == RELIGION_PANTHEON && it->m_iPressure > 0)
		{
#if defined(MOD_CORE_RESILIENT_PANTHEONS)
			//do it a bit more slowly
			it->m_iPressure = max(0, (it->m_iPressure - iPressureChange/2));
			LogPressureChange(eReason, it->m_eReligion, -iPressureChange/2, it->m_iPressure, eResponsiblePlayer);
#else
			it->m_iPressure = max(0, (it->m_iPressure - iPressureChange));
			LogPressureChange(eReason, it->m_eReligion, iPressureChange, it->m_iPressure, eResponsiblePlayer);
#endif
		}
	}

	// Didn't find it, add new entry
	if(!bExisting)
	{
		CvReligionInCity newReligion(eReligion, 0, iPressureChange);
		m_ReligionStatus.push_back(newReligion);

		LogPressureChange(eReason, eReligion, iPressureChange, iPressureChange, eResponsiblePlayer);
	}
}

void CvCityReligions::ErodeOtherReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eExemptedReligion, int iErosionPercent, bool bAllowRetention, bool bLeaveAtheists, PlayerTypes eResponsiblePlayer)
{
	if (iErosionPercent < 1)
		return;

	ReligionInCityList::iterator it;
	for (it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		//ignore atheists if desired
		if (it->m_eReligion == NO_RELIGION && bLeaveAtheists)
			continue;
		
		//do not touch the exempted religion or dead ones
		if (eExemptedReligion == it->m_eReligion || it->m_iPressure==0)
			continue;

		//default
		int iReductionPercent = min(100,iErosionPercent);

		//some beliefs are resistant
		if (it->m_eReligion > RELIGION_PANTHEON && bAllowRetention)
		{
			const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(it->m_eReligion, m_pCity->getOwner());
			if(pReligion)
			{
				int iRetentionPercent = pReligion->m_Beliefs.GetInquisitorPressureRetention(m_pCity->getOwner());  // Normally 0
				iReductionPercent = iReductionPercent * (100 - iRetentionPercent) / 100;
				iReductionPercent = max(0, iReductionPercent);
			}
		}

		//make it so!
		int iReductionAmount = iReductionPercent * it->m_iPressure / 100;
		it->m_iPressure -= iReductionAmount;

		LogPressureChange(eReason, it->m_eReligion, -iReductionAmount, it->m_iPressure, eResponsiblePlayer);
	}
}

/// Simulate prophet spread
void CvCityReligions::SimulateProphetSpread(ReligionTypes eReligion, int iPressure)
{
	int iAtheismPressure = 0;
	int iReligionPressure = 0;
	int iPressureRetained = 0;
	ReligionTypes ePressureRetainedReligion = NO_RELIGION;

	m_SimulatedStatus = m_ReligionStatus;
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
				int iPressureRetention = pReligion->m_Beliefs.GetInquisitorPressureRetention(m_pCity->getOwner());  // Normally 0
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
	CvReligionInCity atheism(NO_RELIGION, 0, iAtheismPressure);
	m_SimulatedStatus.push_back(atheism);
	CvReligionInCity prophetReligion(eReligion, 0, iReligionPressure + iPressure);
	m_SimulatedStatus.push_back(prophetReligion);

	if (ePressureRetainedReligion != NO_RELIGION)
	{
		CvReligionInCity pressureRetainedReligion(ePressureRetainedReligion, 0, iPressureRetained);
		m_SimulatedStatus.push_back(pressureRetainedReligion);

	}

	SimulateFollowers();
}

/// Simulate religious pressure addition
void CvCityReligions::SimulateReligiousPressure(ReligionTypes eReligion, int iPressure)
{
	if(eReligion == NO_RELIGION)
		return;

	bool bFoundIt = false;

	m_SimulatedStatus = m_ReligionStatus;
	ReligionInCityList::iterator it;
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		if(it->m_eReligion == eReligion)
		{
			it->m_iPressure += iPressure;
			bFoundIt = true;
		}
		// If this is pressure from a real religion, reduce presence of pantheon by the same amount
		else if(eReligion > RELIGION_PANTHEON && it->m_eReligion == RELIGION_PANTHEON)
		{
#if defined(MOD_CORE_RESILIENT_PANTHEONS)
			//don't need to check for CvReligiousFollowChangeReason - this method is only used for prophets/missionaries
			it->m_iPressure = max(0, (it->m_iPressure - iPressure/2));
#else
			it->m_iPressure = max(0, (it->m_iPressure - iPressure));
#endif
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
		CvReligionInCity newReligion(eReligion, 0, iPressure);
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
				it->m_iPressure = 0;
		}
	}

	AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPressureConverting, NO_PLAYER);
	RecomputeFollowers(FOLLOWER_CHANGE_SCRIPTED_CONVERSION);
}

#if defined(MOD_BALANCE_CORE)
/// Convert some percentage of followers from ALL religions to another
void CvCityReligions::ConvertPercentAllOtherFollowers(ReligionTypes eToReligion, int iPercent)
{
	//this is a bit stupid, AddReligiousPressure does not take a percent value so we have to compute the change amount manually
	int iPressureToAdd = 0;
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		//Do for every religion in City, as we're converting x% of all citizens. 
		if (it->m_eReligion == eToReligion)
		{
			iPressureToAdd = it->m_iPressure * iPercent / 100;
			break;
		}
	}

	ErodeOtherReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPercent, false, false);
	AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eToReligion, iPressureToAdd, NO_PLAYER);
	RecomputeFollowers(FOLLOWER_CHANGE_SCRIPTED_CONVERSION);
}

/// Convert some number of followers from ALL religions to another
void CvCityReligions::ConvertNumberAllOtherFollowers(ReligionTypes eToReligion, int iPop)
{
	//convert to percent
	int iPercent = (iPop*100)/m_pCity->getPopulation();
	//sanity
	if(iPercent > 100)
		iPercent = 100;

	ConvertPercentAllOtherFollowers(eToReligion, iPercent);
}
#endif

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddHolyCityPressure()
{
	ReligionTypes eHolyReligion = GetReligionForHolyCity();
	if (eHolyReligion != NO_RELIGION)
	{
		int iHolyPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity() *  /*5*/ GD_INT_GET(RELIGION_PER_TURN_FOUNDING_CITY_PRESSURE);
		AddReligiousPressure(FOLLOWER_CHANGE_HOLY_CITY, eHolyReligion, iHolyPressure);
		RecomputeFollowers(FOLLOWER_CHANGE_HOLY_CITY);
	}
}

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddSpyPressure(ReligionTypes eReligion, int iBasePressure)
{
	AddReligiousPressure(FOLLOWER_CHANGE_SPY_PRESSURE, eReligion, iBasePressure*GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
	RecomputeFollowers(FOLLOWER_CHANGE_SPY_PRESSURE);
}

/// Set this city to have all citizens following a religion (mainly for scripting)
void CvCityReligions::AdoptReligionFully(ReligionTypes eReligion)
{
	ErodeOtherReligiousPressure(FOLLOWER_CHANGE_ADOPT_FULLY, eReligion, 100, false, false);
	AddReligiousPressure(FOLLOWER_CHANGE_ADOPT_FULLY, eReligion, m_pCity->getPopulation() * /*1000*/ GD_INT_GET(RELIGION_ATHEISM_PRESSURE_PER_POP));
	RecomputeFollowers(FOLLOWER_CHANGE_ADOPT_FULLY);
}

/// Remove presence of old owner's pantheon (used when a city is conquered)
void CvCityReligions::RemoveFormerPantheon()
{
	ReligionInCityList::iterator it;
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		if(it->m_eReligion == RELIGION_PANTHEON)
		{
			m_ReligionStatus.erase(it);
			break;
		}
	}

	RecomputeFollowers(FOLLOWER_CHANGE_CONQUEST);
}

/// Remove other religions in a city (used by Inquisitor)
void CvCityReligions::RemoveOtherReligions(ReligionTypes eReligion, PlayerTypes eResponsiblePlayer)
{
	// Using an inquisitor from a different religion removes the Holy City status
	ReligionTypes eCurrentHolyCityReligion = GetReligionForHolyCity();
	if (eCurrentHolyCityReligion != NO_RELIGION && eCurrentHolyCityReligion != eReligion)
	{
		GC.getGame().GetGameReligions()->SetHolyCity(eCurrentHolyCityReligion, NULL);
	}

	ErodeOtherReligiousPressure(FOLLOWER_CHANGE_REMOVE_HERESY, eReligion, /*100 in CP, 50 in VP*/ GD_INT_GET(INQUISITION_EFFECTIVENESS), true, true, eResponsiblePlayer);

	//not calling add pressure here, instead recompute directly
	RecomputeFollowers(FOLLOWER_CHANGE_REMOVE_HERESY, eResponsiblePlayer);
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

	CvCity* pHolyCity = pReligion->GetHolyCity();
	
	//Increases with era and mapsize.
	int iEraScaler = GC.getGame().getCurrentEra() * 3;
	iEraScaler /= 2;
	iEraScaler *= GC.getMap().getWorldInfo().getTradeRouteDistanceMod();
	iEraScaler /= 100;
	// Are the cities within the minimum distance?
	int iDistance = /*9*/ GD_INT_GET(RELIGION_ADJACENT_CITY_DISTANCE) + iEraScaler;

	// Boost to distance due to belief?
	int iDistanceMod = pReligion->m_Beliefs.GetSpreadDistanceModifier(m_pCity->getOwner(), pHolyCity);
	if(iDistanceMod > 0)
	{
		iDistance *= (100 + iDistanceMod);
		iDistance /= 100;
	}

	//Boost from policy of other player?
	if (GET_PLAYER(pOtherCity->getOwner()).GetReligionDistance() != 0)
	{
		if (pOtherCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
		{
			//Do we have a religion?
			ReligionTypes ePlayerReligion = GET_PLAYER(pOtherCity->getOwner()).GetReligions()->GetOwnedReligion();

			if (ePlayerReligion <= RELIGION_PANTHEON)
			{
				//No..but did we adopt one?
				ePlayerReligion = GET_PLAYER(pOtherCity->getOwner()).GetReligions()->GetStateReligion();

				//Nope, so full power.
				if (ePlayerReligion <= RELIGION_PANTHEON)
				{
					iDistanceMod += GET_PLAYER(pOtherCity->getOwner()).GetReligionDistance();
				}
				//Yes, so only apply distance bonus to adopted faith.
				else if (eReligiousMajority == ePlayerReligion)
				{
					iDistanceMod += GET_PLAYER(pOtherCity->getOwner()).GetReligionDistance();
				}
			}
			//We did! Only apply bonuses if it is our state religion.
			else if (eReligiousMajority == GET_PLAYER(pOtherCity->getOwner()).GetReligions()->GetStateReligion())
			{
				iDistanceMod += GET_PLAYER(pOtherCity->getOwner()).GetReligionDistance();
			}
		}
	}
	if (iDistanceMod > 0)
	{
		iDistance *= (100 + iDistanceMod);
		iDistance /= 100;
	}

	//estimate the distance between the cities from the traderoute cost. will be influences by terrain features, routes, open borders etc
	int iApparentDistance = INT_MAX;
	SPath path; //trade routes are not necessarily symmetric in case of of unrevealed tiles etc
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pOtherCity, m_pCity, &path))
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, m_pCity, pOtherCity, &path))
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pOtherCity, m_pCity, &path))
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, m_pCity, pOtherCity, &path))
	{
		iApparentDistance = min(iApparentDistance, path.iNormalizedDistanceRaw);
	}

	bool bWithinDistance = (iApparentDistance <= iDistance*SPath::getNormalizedDistanceBase());

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
void CvCityReligions::RecomputeFollowers(CvReligiousFollowChangeReason eReason, PlayerTypes eResponsibleParty)
{
	ReligionTypes eOldMajorityReligion = GetReligiousMajority();
	int iOldFollowers = GetNumFollowers(eOldMajorityReligion);
	int iUnassignedFollowers = m_pCity->getPopulation();

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
		iTotalPressure = /*1000*/ GD_INT_GET(RELIGION_ATHEISM_PRESSURE_PER_POP);

		m_ReligionStatus.clear();
		m_ReligionStatus.push_back(CvReligionInCity(NO_RELIGION,1,iTotalPressure));
	}

	int iPressurePerFollower = iTotalPressure / iUnassignedFollowers;

	vector<int> remainders;

	// Loop through each religion
	for(it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		it->m_iFollowers = it->m_iPressure / iPressurePerFollower;
		iUnassignedFollowers -= it->m_iFollowers;

		//typically there will be a remainder
		remainders.push_back( it->m_iPressure - (it->m_iFollowers * iPressurePerFollower) );
	}

	// Assign out the remainder
	for (int iI = 0; iI < iUnassignedFollowers; iI++)
	{
		size_t iNextRecipient = 0;
		int iLargestRemainder = 0;

		for (size_t i=0; i<remainders.size(); ++i)
		{
			if (remainders[i] > iLargestRemainder)
			{
				iLargestRemainder = remainders[i];
				iNextRecipient = i;
			}
		}

		if (iLargestRemainder > 0)
		{
			m_ReligionStatus[iNextRecipient].m_iFollowers++;
			remainders[iNextRecipient] = 0;
		}
	}

	ComputeReligiousMajority(true);

	ReligionTypes eNewMajorityReligion = GetReligiousMajority();
	int iFollowers = GetNumFollowers(eNewMajorityReligion);

	if (MOD_ISKA_PANTHEONS && eNewMajorityReligion == RELIGION_PANTHEON && eOldMajorityReligion == NO_RELIGION)
	{
		CityConvertsPantheon();
	}

	if(eNewMajorityReligion != eOldMajorityReligion || iFollowers != iOldFollowers)
	{
		CityConvertsReligion(eNewMajorityReligion, eOldMajorityReligion, eResponsibleParty);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		LogFollowersChange(eReason);
	}

	struct PrSortByPressureDesc {
		//no religion should go last
		bool operator()(const CvReligionInCity& lhs, const CvReligionInCity& rhs) const { return lhs.m_iPressure > rhs.m_iPressure && lhs.m_eReligion != NO_RELIGION; }
	};

	//just for convenience, sort the local religions by accumulated pressure
	std::stable_sort(m_ReligionStatus.begin(), m_ReligionStatus.end(), PrSortByPressureDesc());
}

/// Calculate the number of followers for each religion from simulated data
void CvCityReligions::SimulateFollowers()
{
	int iUnassignedFollowers = m_pCity->getPopulation();
	int iPressurePerFollower = 0;

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

	vector<int> remainders;

	// Loop through each religion
	for(it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		it->m_iFollowers = it->m_iPressure / iPressurePerFollower;
		iUnassignedFollowers -= it->m_iFollowers;

		//typically there will be a remainder
		remainders.push_back( it->m_iPressure - (it->m_iFollowers * iPressurePerFollower) );
	}

	// Assign out the remainder
	for (int iI = 0; iI < iUnassignedFollowers; iI++)
	{
		size_t iNextRecipient = 0;
		int iLargestRemainder = 0;

		for (size_t i=0; i<remainders.size(); ++i)
		{
			if (remainders[i] > iLargestRemainder)
			{
				iLargestRemainder = remainders[i];
				iNextRecipient = i;
			}
		}

		if (iLargestRemainder > 0)
		{	
			m_SimulatedStatus[iNextRecipient].m_iFollowers++;
			remainders[iNextRecipient] = 0;
		}
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
		PlayerTypes eReligionController = NO_PLAYER;
		CvCity* pHolyCity = pNewReligion->GetHolyCity();
		if(pHolyCity != NULL)
			eReligionController = pHolyCity->getOwner();
#endif
		//bonuses might change ...
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
				iGoldBonus = pNewReligion->m_Beliefs.GetGoldWhenCityAdopts(eResponsibleParty, pHolyCity);
				iGoldBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
				iGoldBonus /= 100;
			}
			else
			{
				iGoldBonus = pNewReligion->m_Beliefs.GetGoldWhenCityAdopts();
				iGoldBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
				iGoldBonus /= 100;
			}

			if (eReligionController != NO_PLAYER && GET_PLAYER(eReligionController).GetReligions()->GetStateReligion(false) == eMajority)
			{
				bool paid = false;
				if (iGoldBonus > 0)
				{
					GET_PLAYER(eReligionController).GetTreasury()->ChangeGold(iGoldBonus);
#if defined(MOD_BALANCE_CORE)
					paid = true;
#else
					SetPaidAdoptionBonus(true);
#endif

					if (eReligionController == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGoldBonus);
						SHOW_PLOT_POPUP(m_pCity->plot(), NO_PLAYER, text);
					}
				}
#if defined(MOD_BALANCE_CORE_BELIEFS)
				GET_PLAYER(eReligionController).doInstantYield(INSTANT_YIELD_TYPE_CONVERSION, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pHolyCity);
				GET_PLAYER(eReligionController).doInstantYield(INSTANT_YIELD_TYPE_CONVERSION_EXPO, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pHolyCity);
				
				for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					YieldTypes eYield = (YieldTypes)iI;
					if(eYield == NO_YIELD)
						continue;

					int iValue = pNewReligion->m_Beliefs.GetYieldFromConversion(eYield, eReligionController, pHolyCity);
					iValue += pNewReligion->m_Beliefs.GetYieldFromConversionExpo(eYield, eReligionController, pHolyCity);
					if(iValue > 0)
					{
						paid = true;
						break;
					}
				}
				if (paid)
					m_pCity->SetPaidAdoptionBonus(eMajority, true);
			}
#endif
		}

		// Notification if the player's city was converted to a religion they didn't found
		PlayerTypes eOwnerPlayer = m_pCity->getOwner();
		CvPlayerAI& kOwnerPlayer = GET_PLAYER(eOwnerPlayer);
		const ReligionTypes eOwnerPlayerReligion = kOwnerPlayer.GetReligions()->GetOwnedReligion();

		if (eOwnerPlayer != eResponsibleParty && eMajority != eOldMajority && eReligionController != eOwnerPlayer && eOwnerPlayerReligion > RELIGION_PANTHEON)
		{
			if(kOwnerPlayer.GetNotifications())
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_ACTIVE_PLAYER", m_pCity->getName());
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_ACTIVE_PLAYER_S");
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), eMajority, -1);
			}

			//Achievements!
			if (MOD_API_ACHIEVEMENTS && eOwnerPlayer == GC.getGame().getActivePlayer())
			{
				const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eOwnerPlayerReligion, eOwnerPlayer);
				if(pkReligion != NULL)
				{
					if(m_pCity->getX() == pkReligion->m_iHolyCityX && m_pCity->getY() == pkReligion->m_iHolyCityY)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_20);
					}
				}
			}
		}

		else if(eOwnerPlayer != eResponsibleParty && eMajority != eOldMajority && eOldMajority == NO_RELIGION)
		{
			if(kOwnerPlayer.GetNotifications())
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_INITIAL_CONVERSION", m_pCity->getName());
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_SPREAD_INITIAL_CONVERSION_S");
				kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_SPREAD_NATURAL, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), eMajority, -1);
			}
		}

		if (MOD_API_ACHIEVEMENTS)
		{
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
									if(pCityReligions->GetReligiousMajority() != pNewReligion->m_eReligion)
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
		}

		// Diplo implications (there must have been religion switch and a responsible party)
		if (eMajority != eOldMajority && eResponsibleParty != NO_PLAYER)
		{
			// Is the city owner not the founder of this religion?
			if (pNewReligion->m_eFounder != m_pCity->getOwner())
			{
				CvPlayer& kCityOwnerPlayer = GET_PLAYER(m_pCity->getOwner());

				// Did he found another religion?
				ReligionTypes eCityOwnerReligion = kCityOwnerPlayer.GetReligions()->GetOwnedReligion();
				if (eCityOwnerReligion != NO_RELIGION && eCityOwnerReligion != eMajority)
				{
					int iPoints = 0;

					// His religion wasn't present here, minor hit
					if (eOldMajority != eCityOwnerReligion)
					{
						iPoints = /*1*/ GD_INT_GET(RELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY);
					}

					// This was his holy city; huge hit!
					else if (m_pCity->GetCityReligions()->IsHolyCityForReligion(eCityOwnerReligion))
					{
						iPoints = /*25*/ GD_INT_GET(RELIGION_DIPLO_HIT_CONVERT_HOLY_CITY);
					}

					// He had established his religion here, major hit
					else
					{
						iPoints = /*3*/ GD_INT_GET(RELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY);
					}

					kCityOwnerPlayer.GetDiplomacyAI()->ChangeNegativeReligiousConversionPoints(eResponsibleParty, iPoints);
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

#if defined(MOD_ISKA_PANTHEONS)
void CvCityReligions::CityConvertsPantheon()
{
	// Notification if the player's city was converted to a pantheon
	PlayerTypes eOwnerPlayer = m_pCity->getOwner();
	CvPlayerAI& kOwnerPlayer = GET_PLAYER(eOwnerPlayer);

	if (kOwnerPlayer.GetNotifications())
	{
		Localization::String strMessage;
		Localization::String strSummary;
		strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_PANTHEON_SPREAD_TITLE", m_pCity->getName());
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_SPREAD_DESC");
		kOwnerPlayer.GetNotifications()->Add(NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER, strMessage.toUTF8(), strSummary.toUTF8(), m_pCity->getX(), m_pCity->getY(), -1);
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pCity->getOwner());
		args->Push(m_pCity->getX());
		args->Push(m_pCity->getY());

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "CityConvertsPantheon", args.get(), bResult);
	}
}
#endif

/// Log a message with status information
void CvCityReligions::LogFollowersChange(CvReligiousFollowChangeReason eReason)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strReasonString = GetFollowerChangeString(eReason);
		CvString temp;
		FILogFile* pLog = NULL;
		CvCityReligions* pCityRel = m_pCity->GetCityReligions();

		pLog = LOGFILEMGR.GetLog(GC.getGame().GetGameReligions()->GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strOutBuf.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strOutBuf += m_pCity->getName();
		strOutBuf += ", ";

		// Add a reason string
		strOutBuf += strReasonString + ", ";
		temp.Format("Pop: %d", m_pCity->getPopulation());
		strOutBuf += temp;
		if(pCityRel->IsReligionInCity())
		{
			ReligionTypes eFirst = pCityRel->GetReligionByAccumulatedPressure(0);
			if (eFirst != NO_RELIGION)
			{
				CvReligionEntry* pEntry = GC.getReligionInfo(eFirst);
				if (pEntry)
				{
					strOutBuf += ", First: ";
					strOutBuf += pEntry->GetDescription();
					temp.Format("(%d)", pCityRel->GetNumFollowers(eFirst));
					strOutBuf += temp;
				}
			}

			ReligionTypes eSecond = pCityRel->GetReligionByAccumulatedPressure(1);
			if (eSecond != NO_RELIGION)
			{
				CvReligionEntry* pEntry = GC.getReligionInfo(eSecond);
				if (pEntry)
				{
					strOutBuf += ", Second: ";
					strOutBuf += pEntry->GetDescription();
					temp.Format("(%d)", pCityRel->GetNumFollowers(eSecond));
					strOutBuf += temp;
				}
			}

			temp.Format("Nonreligious: %d", pCityRel->GetNumFollowers(NO_RELIGION));
			strOutBuf += ", " + temp;
		}
		else
		{
			strOutBuf += ", No religion in city";
		}

		pLog->Msg(strOutBuf);
	}
}

///
template<typename CityReligions, typename Visitor>
void CvCityReligions::Serialize(CityReligions& cityReligions, Visitor& visitor)
{
	visitor(cityReligions.m_ReligionStatus);
	visitor(cityReligions.m_eMajorityCityReligion);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCityReligions& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvCityReligions::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvCityReligions& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvCityReligions::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvGameReligions
//=====================================
/// Constructor
CvUnitReligion::CvUnitReligion(void):
	m_eReligion(NO_RELIGION),
	m_iStrength(0),
	m_iSpreadsUsed(0),
	m_iMaxStrength(0)
{
}

/// Initialize class data
void CvUnitReligion::Init()
{
	m_eReligion = NO_RELIGION;
	m_iStrength = 0;
	m_iSpreadsUsed = 0;
	m_iMaxStrength = 0;
}

int CvUnitReligion::GetMaxSpreads(const CvUnit* pUnit) const
{
	if (!pUnit || m_iStrength <= 0) //no strength, no spread!
		return 0;

	//missionary spreads can be buffed but not prophets
	int iReligionSpreads = pUnit->getUnitInfo().GetReligionSpreads();
	if (!pUnit->getUnitInfo().IsFoundReligion())
	{
		CvCity* pOriginCity = pUnit->getOriginCity();
		iReligionSpreads += pOriginCity ? pOriginCity->GetCityBuildings()->GetMissionaryExtraSpreads() : 0;
		iReligionSpreads += GET_PLAYER(pUnit->getOwner()).GetNumMissionarySpreads();
	}

	return iReligionSpreads;
}

void CvUnitReligion::SetFullStrength(PlayerTypes eOwner, const CvUnitEntry& kUnitInfo, ReligionTypes eReligion, CvCity * /*pOriginCity*/)
{
	if (eOwner == NO_PLAYER || eReligion <= RELIGION_PANTHEON)
		return;

	//strength can be buffed
	int iExtraStrength = kUnitInfo.GetReligiousStrength() * (GET_PLAYER(eOwner).GetMissionaryExtraStrength() + GET_PLAYER(eOwner).GetPlayerTraits()->GetExtraMissionaryStrength());
	unsigned short iReligiousStrength = kUnitInfo.GetReligiousStrength() + (unsigned short)(iExtraStrength / 100);

	m_eReligion = eReligion;
	m_iSpreadsUsed = 0;
	m_iStrength = iReligiousStrength;
	m_iMaxStrength = iReligiousStrength;
}

bool CvUnitReligion::IsFullStrength() const
{
	return m_iSpreadsUsed == 0 && m_iStrength == m_iMaxStrength;
}

///
template<typename UnitReligion, typename Visitor>
void CvUnitReligion::Serialize(UnitReligion& unitReligion, Visitor& visitor)
{
	visitor(unitReligion.m_eReligion);
	visitor(unitReligion.m_iStrength);
	visitor(unitReligion.m_iMaxStrength);
	visitor(unitReligion.m_iSpreadsUsed);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvUnitReligion& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvUnitReligion::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvUnitReligion& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvUnitReligion::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvReligionAI
//=====================================
/// Constructor
CvReligionAI::CvReligionAI(void):
	m_pPlayer(NULL)
	, m_pBeliefs(NULL)
	, m_eReligionToSpread(NO_RELIGION)
	, m_iTurnReligionToSpreadUpdated(-1)
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
	Reset();
}

/// Reset
void CvReligionAI::Reset()
{
	m_eReligionToSpread = NO_RELIGION;
	m_iTurnReligionToSpreadUpdated = -1;
}

///
template<typename ReligionAI, typename Visitor>
void CvReligionAI::Serialize(ReligionAI& /*religionAI*/, Visitor& /*visitor*/)
{
}

/// Serialization read
void CvReligionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}
 
/// Serialization write
void CvReligionAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvReligionAI& religionAI)
{
	religionAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvReligionAI& religionAI)
{
	religionAI.Write(stream);
	return stream;
}

/// Called every turn to see what to spend Faith on
void CvReligionAI::DoTurn()
{
	// Only AI players use this function for now
	if (m_pPlayer->isHuman())
		return;

#if defined(MOD_BALANCE_CORE)
	//buy inquisitors in unprotected cities if an enemy prophet is near or buy missionaries to spread our faith
	bool bSpreadingOrDefending = DoFaithPurchases() || DoReligionDefenseInCities();
	bool bShouldSaveForFounding = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() > 0 && m_pPlayer->GetReligions()->GetOwnedReligion() <= RELIGION_PANTHEON;

	//If we have leftover faith, let's look at city purchases.
	if (!bSpreadingOrDefending && !bShouldSaveForFounding)
	{
		CvWeightedVector<CvCity*> m_aFaithPriorities;

		//Sort by faith production
		int iLoop = 0;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if (pLoopCity && !pLoopCity->IsResistance() && !pLoopCity->isUnderSiege())
			{
				int iFaith = pLoopCity->getYieldRateTimes100(YIELD_FAITH, false);

				if (pLoopCity->IsPuppet())
					iFaith /= 2;

				if (pLoopCity->GetCityReligions()->IsHolyCityAnyReligion())
					iFaith *= 2;

				m_aFaithPriorities.push_back(pLoopCity, iFaith);
			}
		}

		m_aFaithPriorities.StableSortItems();
		for (int iLoop = 0; iLoop < m_aFaithPriorities.size(); iLoop++)
			DoFaithPurchasesInCities(m_aFaithPriorities.GetElement(iLoop));
	}
#endif
}

/// Select the belief most helpful to this pantheon
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
BeliefTypes CvReligionAI::ChoosePantheonBelief(PlayerTypes ePlayer)
#else
BeliefTypes CvReligionAI::ChoosePantheonBelief()
#endif
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes> beliefChoices;

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
			beliefChoices.push_back(eBelief, iScore);
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0x0af7fe29).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

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
	CvWeightedVector<BeliefTypes> beliefChoices;

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
			beliefChoices.push_back(eBelief, iScore);
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0x9db23f3c).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

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
	CvWeightedVector<BeliefTypes> beliefChoices;

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
			beliefChoices.push_back(eBelief, iScore);
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0x93c8983d).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

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
	CvWeightedVector<BeliefTypes> beliefChoices;

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
			beliefChoices.push_back(eBelief, iScore);
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0x3a862bb8).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

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
	CvWeightedVector<BeliefTypes> beliefChoices;

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
				const int iScore = ScoreBelief(pEntry, true);
				beliefChoices.push_back(eBelief, iScore);
			}
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0xc0809801).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

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
	CvWeightedVector<BeliefTypes> beliefChoices;

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
			beliefChoices.push_back(eBelief, iScore);
		}
	}

	// Choose from weighted vector
	beliefChoices.StableSortItems();
	BeliefTypes rtnValue = NO_BELIEF;
	if (beliefChoices.size() > 0)
	{
		rtnValue = beliefChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), CvSeeder::fromRaw(0x894eaafe).mix(GET_PLAYER(ePlayer).GetID()).mix(availableBeliefs.size()));
		LogBeliefChoices(beliefChoices, rtnValue);
	}

	return rtnValue;
}

/// Find the city where a missionary should next spread his religion
CvCity* CvReligionAI::ChooseMissionaryTargetCity(CvUnit* pUnit, const vector<pair<int,int>>& vIgnoreTargets, int* piTurns) const
{
	ReligionTypes eOwnedReligion = m_pPlayer->GetReligions()->GetOwnedReligion();
	ReligionTypes eSpreadReligion = GetReligionToSpread(true);
	if(eSpreadReligion <= RELIGION_PANTHEON)
		return NULL;

	//do not use captured missionaries with weird religions
	if (pUnit->GetReligionData()->GetReligion() != eSpreadReligion)
		return NULL;

	std::vector<SPlotWithScore> vTargets;

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			//do not spread other players' religion to non-owned cities
			if (eSpreadReligion != eOwnedReligion && kPlayer.GetID() != m_pPlayer->GetID())
				continue;

			// Loop through each of their cities
			int iLoop = 0;
			for(CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				//ignore mere fog danger ... it's volatile, leading to missionaries going back and forth
 				if (pUnit->GetDanger(pLoopCity->plot()) > pUnit->GetCurrHitPoints()/4 || pLoopCity->IsRazing())
					continue;

				//we often have multiple missionaries active at the same time, don't all go to the same target
				vector<pair<int, int>>::const_iterator it = std::find_if(vIgnoreTargets.begin(), vIgnoreTargets.end(), CompareSecond(pLoopCity->plot()->GetPlotIndex()));
				if (it != vIgnoreTargets.end() && it->first != pUnit->GetID())
					continue;

				//ignore far-flung cities - little chance we can establish our religion there
				if (m_pPlayer->GetCityDistancePathLength(pLoopCity->plot()) > 37)
					continue;

				if(pUnit->CanSpreadReligion(pLoopCity->plot()))
				{
					int iScore = ScoreCityForMissionary(pLoopCity, pUnit, pUnit->GetReligionData()->GetReligion());
					if (iScore>0)
						vTargets.push_back(SPlotWithScore(pLoopCity->plot(),iScore));
				}
			}
		}
	}

	//this sorts ascending
	std::stable_sort(vTargets.begin(),vTargets.end());
	//so reverse it
	std::reverse(vTargets.begin(),vTargets.end());

	for (std::vector<SPlotWithScore>::iterator it=vTargets.begin(); it!=vTargets.end(); ++it)
	{
		//cache the path, we're about to reuse it
		int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1| CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;
		if (pUnit->GeneratePath(it->pPlot,iFlags,INT_MAX,piTurns) )
			return it->pPlot->getPlotCity();
	}

	return NULL;
}

/// Find the city where an inquisitor should next remove heresy
CvCity* CvReligionAI::ChooseInquisitorTargetCity(CvUnit* pUnit, const vector<pair<int,int>>& vIgnoreTargets, int* piTurns) const
{
	ReligionTypes eMyReligion = GetReligionToSpread(true);
	if(eMyReligion <= RELIGION_PANTHEON)
		return NULL;

	std::vector<SPlotWithScore> vTargetsO, vTargetsD;
	vector<PlayerTypes> vUnfriendlyMajors = m_pPlayer->GetUnfriendlyMajors();

	// Loop through each of my cities
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pUnit->GetDanger(pLoopCity->plot()) > 0)
			continue;

		//we often have multiple inquisitors active at the same time, don't all go to the same target
		vector<pair<int, int>>::const_iterator it = std::find_if(vIgnoreTargets.begin(), vIgnoreTargets.end(), CompareSecond(pLoopCity->plot()->GetPlotIndex()));
		if (it != vIgnoreTargets.end() && it->first != pUnit->GetID())
			continue;

		if (pLoopCity->GetCityReligions()->IsDefendedByOurInquisitor(pUnit->GetReligionData()->GetReligion(),pUnit))
			continue;

		int iScoreO = ScoreCityForInquisitorOffensive(pLoopCity, pUnit, pUnit->GetReligionData()->GetReligion());
		if (iScoreO>0)
			vTargetsO.push_back(SPlotWithScore(pLoopCity->plot(),iScoreO));
		int iScoreD = ScoreCityForInquisitorDefensive(pLoopCity, pUnit, pUnit->GetReligionData()->GetReligion(), vUnfriendlyMajors);
		if (iScoreD>0)
			vTargetsD.push_back(SPlotWithScore(pLoopCity->plot(),iScoreD));
	}

	//offensive targets first, until we run out
	std::stable_sort(vTargetsO.begin(),vTargetsO.end());
	std::reverse(vTargetsO.begin(),vTargetsO.end());

	for (std::vector<SPlotWithScore>::iterator it=vTargetsO.begin(); it!=vTargetsO.end(); ++it)
	{
		int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1| CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;
		if (pUnit->GeneratePath(it->pPlot,iFlags,INT_MAX,piTurns) )
			return it->pPlot->getPlotCity();
	}

	//defensive targets as fallback
	std::stable_sort(vTargetsD.begin(),vTargetsD.end());
	std::reverse(vTargetsD.begin(),vTargetsD.end());

	for (std::vector<SPlotWithScore>::iterator it=vTargetsD.begin(); it!=vTargetsD.end(); ++it)
	{
		int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1| CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;
		if (pUnit->GeneratePath(it->pPlot,iFlags,INT_MAX,piTurns) )
			return it->pPlot->getPlotCity();
	}

	return NULL;
}

/// If we were going to use a prophet to convert a city, which one would it be?
CvCity *CvReligionAI::ChooseProphetConversionCity(CvUnit* pUnit, int* piTurns) const
{
	if (piTurns)
		*piTurns = INT_MAX;

	int iDistanceBias = 7; //score drops linearly with distance from holy city
	int iMinScore = 500;  //equivalent to converting 10 heretics at a distance of 13 plots to our holy city
	if (pUnit && !pUnit->GetReligionData()->IsFullStrength())
		iMinScore = 200;

	// Make sure we're spreading a religion and find holy city
	ReligionTypes eReligion = GetReligionToSpread(false);
	if (eReligion <= RELIGION_PANTHEON)
	{
		return NULL;
	}

	const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
	CvCity* pHolyCity = pkReligion ? pkReligion->GetHolyCity() : NULL;
	if (!pHolyCity)
	{
		return NULL;
	}

	std::vector<SPlotWithScore> vCandidates;

	// Look at our cities first, checking them for followers of other religions
	int iLoop = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
		int iHeretics = pLoopCity->GetCityReligions()->GetFollowersOtherReligions(eReligion);
		int iDistanceToHolyCity = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pHolyCity->getX(), pHolyCity->getY());

		// If this is the holy city and it has been converted, want to go there no matter what
		if (pLoopCity == pHolyCity && eMajorityReligion != eReligion)
		{
			vCandidates.push_back(SPlotWithScore(pLoopCity->plot(), 100000));
			continue;
		}

		CvTacticalDominanceZone* pZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByCity(pLoopCity,false);
		if (pZone && pZone->GetOverallEnemyStrength()>0)
			continue;

		if(pUnit && !pUnit->CanSpreadReligion(pLoopCity->plot()))
			continue;

		// Otherwise score this city
		int iScore = (iHeretics * 100) / (iDistanceToHolyCity + 1);
		if (eMajorityReligion != eReligion && eMajorityReligion > RELIGION_PANTHEON)
		{
			iScore *= 3;
		}

		if (iScore > iMinScore)
			vCandidates.push_back( SPlotWithScore(pLoopCity->plot(),iScore));
	}

	// Now try other players
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);

		if (!kLoopPlayer.isAlive() || iPlayerLoop == m_pPlayer->GetID())
			continue;

		if (kLoopPlayer.GetPlayerTraits()->IsForeignReligionSpreadImmune())
			continue;

		if (m_pPlayer->IsAtWarWith(kLoopPlayer.GetID()))
			continue;

		if (m_pPlayer->GetDiplomacyAI()->IsPlayerBadTheftTarget(kLoopPlayer.GetID(), THEFT_TYPE_CONVERSION))
			continue;

		int iCityLoop = 0;
		for (pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
		{
			//We don't want to spread our faith to unowned cities if it doesn't spread naturally and we have a unique belief (as its probably super good).
			if (!m_pPlayer->GetPlayerTraits()->IsUniqueBeliefsOnly() && m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pLoopCity->getOwner() != m_pPlayer->GetID())
			{
				CvGameReligions* pReligions = GC.getGame().GetGameReligions();
				const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
				if (pMyReligion)
				{
					if (pMyReligion->m_Beliefs.GetUniqueCiv() == m_pPlayer->getCivilizationType())
					{
						continue;
					}
				}
			}

			//ignore far-flung cities
			if (m_pPlayer->GetCityDistancePathLength(pLoopCity->plot()) > 23)
				continue;

			CvCityReligions* pCR = pLoopCity->GetCityReligions();
			if (!pCR->IsDefendedAgainstSpread(eReligion))
			{
				int iHeretics = pCR->GetFollowersOtherReligions(eReligion);
				if (iHeretics == 0)
					continue;

				ReligionTypes eMajorityReligion = pCR->GetReligiousMajority();
				if (eMajorityReligion == eReligion)
					continue;

				int iOurPressure = max(1,pCR->GetPressurePerTurn(eReligion));
				int iMajorityPressure = pCR->GetPressurePerTurn(eMajorityReligion);
				int iDistanceToHolyCity = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pHolyCity->getX(), pHolyCity->getY());

				// Score this city
				int iScore = (iHeretics * 1000) / (iDistanceToHolyCity + iDistanceBias);

				//    - Low score if we would soon convert this city anyway
				//	(but not the other way around: do not go for the most difficult targets first!)
				if (iMajorityPressure < iOurPressure)
				{
					iScore = (iScore*iMajorityPressure) / iOurPressure;
				}

				//    - Holy city will anger folks, let's not do that one right away
				ReligionTypes eCityOwnersReligion = GET_PLAYER((PlayerTypes)iPlayerLoop).GetReligions()->GetOwnedReligion();
				if (eCityOwnersReligion > RELIGION_PANTHEON && pCR->IsHolyCityForReligion(eCityOwnersReligion))
				{
					iScore /= 2;
				}

				//    - City not owned by religion founder, won't anger folks as much
				const CvReligion* pkMajorityReligion = GC.getGame().GetGameReligions()->GetReligion(eMajorityReligion, NO_PLAYER);
				if (pkMajorityReligion && pkMajorityReligion->m_eFounder != pLoopCity->getOwner())
				{
					iScore *= 2;
				}

				//	- Do we have a belief that promotes foreign cities? If so, promote them.
				if (MOD_BALANCE_CORE_BELIEFS)
				{
					for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
					{
						YieldTypes eYield = (YieldTypes)iI;
						if (pkReligion->m_Beliefs.GetYieldFromForeignSpread(eYield, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iScore *= 2;
						}						
						else if (pkReligion->m_Beliefs.GetYieldChangePerXForeignFollowers(eYield, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iScore *= 2;
						}
						else if (pkReligion->m_Beliefs.GetYieldChangePerForeignCity(eYield, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iScore *= 2;
						}
						if (kLoopPlayer.isMinorCiv() && pkReligion->m_Beliefs.GetYieldChangePerXCityStateFollowers(eYield, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iScore *= 2;
						}
					}
					if (pkReligion->m_Beliefs.GetHappinessPerXPeacefulForeignFollowers(m_pPlayer->GetID(), pHolyCity) > 0)
					{
						iScore *= 2;
					}
				}

				if (iScore > iMinScore)
					vCandidates.push_back( SPlotWithScore(pLoopCity->plot(),iScore));
			}
		}
	}

	//sort descending
	std::stable_sort(vCandidates.begin(),vCandidates.end());
	std::reverse(vCandidates.begin(),vCandidates.end());

	//look at the top two and take the one that is closest
	if (pUnit && vCandidates.size()>1)
	{
		int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;

		int iTurnsToTargetA = INT_MAX;
		int iTurnsToTargetB = INT_MAX;
		int iScoreA = 0;
		int iScoreB = 0;

		if (pUnit->GeneratePath(vCandidates[0].pPlot, iFlags, INT_MAX, &iTurnsToTargetA))
			iScoreA = vCandidates[0].score / (iTurnsToTargetA + 3); //add some bias for close targets
		if (pUnit->GeneratePath(vCandidates[1].pPlot, iFlags, INT_MAX, &iTurnsToTargetB))
			iScoreB = vCandidates[1].score / (iTurnsToTargetB + 3); //add some bias for close targets

		if (iScoreA > 0 && iScoreA > iScoreB)
		{
			if (piTurns)
				*piTurns = iTurnsToTargetA;
			return vCandidates[0].pPlot->getPlotCity();
		}
		if (iScoreB > 0 && iScoreB > iScoreA)
		{
			if (piTurns)
				*piTurns = iTurnsToTargetB;
			return vCandidates[1].pPlot->getPlotCity();
		}
	}
	else if (!vCandidates.empty())
		return vCandidates.front().pPlot->getPlotCity();

	return NULL;
}

/// What religion should this AI civ be spreading?
ReligionTypes CvReligionAI::GetReligionToSpread(bool bConsiderForeign) const
{
	//recompute only once per turn
	if (GC.getGame().getGameTurn() == m_iTurnReligionToSpreadUpdated)
		return m_eReligionToSpread;

	//need to update, start from scratch
	m_eReligionToSpread = NO_RELIGION;
	m_iTurnReligionToSpreadUpdated = GC.getGame().getGameTurn();

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetReligionToSpread", args.get(), iValue))
		{
			m_eReligionToSpread = (ReligionTypes)iValue;
			return m_eReligionToSpread;
		}
	}

	//minors do not spread
	if (!m_pPlayer->isMajorCiv())
		return m_eReligionToSpread;

	//state religion by default
	m_eReligionToSpread = m_pPlayer->GetReligions()->GetStateReligion();
	if(m_eReligionToSpread > RELIGION_PANTHEON)
		return m_eReligionToSpread;

	//or something imported as fallback
	if (bConsiderForeign)
	{
		//this call is expensive ...
		m_eReligionToSpread = GetFavoriteForeignReligion(true);
		if (m_eReligionToSpread > RELIGION_PANTHEON)
			return m_eReligionToSpread;
	}

	return m_eReligionToSpread;
}

//check all existing religions and see which one fits us best
ReligionTypes CvReligionAI::GetFavoriteForeignReligion(bool bForInternalSpread) const
{
	//hold off while not all religions have been founded
	if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() > 0)
		return NO_RELIGION;

	//wtf, public members?
	ReligionList allReligions = GC.getGame().GetGameReligions()->m_CurrentReligions;

	int iBestOverallScore = 0;
	ReligionTypes eBestOverall = NO_RELIGION;

	int iBestValidScore = 0;
	ReligionTypes eBestValid = NO_RELIGION;

	for (ReligionList::iterator itR = allReligions.begin(); itR != allReligions.end(); ++itR)
	{
		//ignore pantheons
		if (itR->m_eReligion <= RELIGION_PANTHEON)
			continue;

		//should not happen but ...
		if (itR->m_eFounder == m_pPlayer->GetID())
			continue;

		//what's in it for us? problem is, some religions might have more beliefs than others ... bad luck for them.
		int iScore = 0;
		for (int i = 0; i < itR->m_Beliefs.GetNumBeliefs(); i++)
		{
			BeliefTypes b = itR->m_Beliefs.GetBelief(i);
			CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(b);

			//ignore founder beliefs!
			if (pEntry && !pEntry->IsFounderBelief() && !pEntry->IsEnhancerBelief())
				iScore += ScoreBelief(pEntry,false,false);
		}

		//consider whether we like the founder or not
		switch(m_pPlayer->GetDiplomacyAI()->GetCivOpinion(itR->m_eFounder))
		{
			case CIV_OPINION_ALLY:
				iScore *= 150;
				break;
			case CIV_OPINION_FRIEND:
				iScore *= 130;
				break;
			case CIV_OPINION_FAVORABLE:
				iScore *= 115;
				break;
			case CIV_OPINION_NEUTRAL:
				iScore *= 100;
				break;
			case CIV_OPINION_COMPETITOR:
				iScore *= 50;
				break;
			default:
				//we do not like them at all
				iScore *= 20;
				break;
		}

		iScore /= 100;

		if (iScore > iBestOverallScore)
		{
			iBestOverallScore = iScore;
			eBestOverall = itR->m_eReligion;
		}

		if (m_pPlayer->GetReligions()->HasCityWithMajorityReligion(itR->m_eReligion))
		{
			if (iScore > iBestValidScore)
			{
				iBestValidScore = iScore;
				eBestValid = itR->m_eReligion;
			}
		}
	}

	// if we cannot create missionaries for our favorite we may want to pass ... a bird in the hand is worth two in the bush
	if (bForInternalSpread && iBestValidScore*3 < iBestOverallScore*2 )
		eBestValid = NO_RELIGION;

	if(GC.getLogging() && eBestValid != NO_RELIGION)
	{
		CvString strFavorite;
		strFavorite.Format(", Favorite Foreign Religion is, %s", GC.getReligionInfo(eBestValid)->GetDescription());
		CvString strLogMsg = m_pPlayer->getCivilizationShortDescription();
		strLogMsg += strFavorite;
		GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
	}

	return eBestValid;
}

// PRIVATE METHODS

/// Spend faith if already have an enhanced religion
#if defined(MOD_BALANCE_CORE)
bool CvReligionAI::DoFaithPurchasesInCities(CvCity* pCity)
{
	if(pCity == NULL)
		return false;

	ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
	if(eReligion == NO_RELIGION)
		return false;

	BuildingClassTypes eFaithBuilding = FaithBuildingAvailable(eReligion, pCity, true);
	CvString strLogMsg = m_pPlayer->getCivilizationShortDescription();

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
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return true;
			}
			else
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Saving up for a Faith Building.";
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
				return false;
			}
		}
	}

	// SECOND PRIORITY - NON-FAITH BUILDINGS
	if (eReligion != NO_RELIGION)
	{
		// FIRST SUB-PRIORITY
		// Try to build other buildings with Faith if we took that belief
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eNonFaithBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			if (eNonFaithBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eNonFaithBuilding);
				//Changed - let's make sure it costs faith, and isn't a religious-specific building
				if (pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
				{
					if (pCity->GetCityBuildings()->GetNumBuilding(eNonFaithBuilding) < 1 && pCity->IsCanPurchase(true, true, NO_UNIT, eNonFaithBuilding, NO_PROJECT, YIELD_FAITH))
					{
						if (BuyFaithBuilding(pCity, eNonFaithBuilding))
						{
							if (GC.getLogging())
							{
								strLogMsg += ", Bought a Non-Faith Building";
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return true;
						}
						else
						{
							if (GC.getLogging())
							{
								strLogMsg += ", Focusing on Non-Faith Buildings, have belief that allows this";
								CvString strFaith;
								strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return false;
						}
					}
				}
			}
		}
	}

	// FOURTH PRIORITY - OTHER UNITS
	// Try to build other units with Faith if we took that belief
	if((eReligion != NO_RELIGION) && AreAllOurCitiesConverted(eReligion, false /*bIncludePuppets*/))
	{
		int iPurchaseAmount = 5;
		if (m_pPlayer->GetPlayerTraits()->IsCanPurchaseNavalUnitsFaith())
		{
			iPurchaseAmount *= 10;
		}
		if((m_pPlayer->GetTreasury()->CalculateBaseNetGoldTimes100() > 0) && (m_pPlayer->GetNumUnitsOutOfSupply() <= 0))
		{
			CvCityBuildable selection = (pCity->GetCityStrategyAI()->ChooseHurry(true, true));
			if (selection.m_eBuildableType != NOT_A_CITY_BUILDABLE)
			{
				UnitTypes eUnit = (UnitTypes)selection.m_iIndex;
				if (eUnit != NO_UNIT)
				{
					CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
					if (pUnitEntry)
					{
						if (eUnit == pCity->GetUnitForOperation() || eUnit == m_pPlayer->GetMilitaryAI()->GetUnitTypeForArmy(pCity))
						{
							pCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
							if (GC.getLogging())
							{
								CvString strFaith;
								strFaith.Format(", Bought a Non-Faith military Unit, %s, Faith: %d", pUnitEntry->GetDescriptionKey(), m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return true;
						}
						else if (m_pPlayer->GetNumUnitsWithUnitAI(pUnitEntry->GetDefaultUnitAIType(), false) <= iPurchaseAmount)
						{
							pCity->Purchase(eUnit, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
							if (GC.getLogging())
							{
								CvString strFaith;
								strFaith.Format(", Bought a Non-Faith military Unit, %s, (up to %d), Faith: %d", pUnitEntry->GetDescriptionKey(), iPurchaseAmount, m_pPlayer->GetFaith());
								strLogMsg += strFaith;
								GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
							}
							return false;
						}
					}
				}
			}
		}
	}

	return false;
}

bool CvReligionAI::DoReligionDefenseInCities()
{
	ReligionTypes eDesired = m_pPlayer->GetReligionAI()->GetReligionToSpread(true);
	UnitTypes eInquisitor = m_pPlayer->GetSpecificUnitType("UNITCLASS_INQUISITOR");
	bool bResult = false;

	//Sort by faith production
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		//ignore cities which have the wrong religion to begin with
		if (pLoopCity->GetCityReligions()->GetReligiousMajority() != eDesired)
			continue;
		
		//do we have enough faith
		int iCost = pLoopCity->GetFaithPurchaseCost(eInquisitor, true /*bIncludeBeliefDiscounts*/);
		if (iCost > m_pPlayer->GetFaith())
			continue;

		//already have an inquisitor around
		if (pLoopCity->GetCityReligions()->IsDefendedByOurInquisitor(eDesired))
			continue;

		for (int i=RING0_PLOTS; i<RING4_PLOTS; i++)
		{
			CvPlot* pPlot = iterateRingPlots(pLoopCity->plot(), i);
			if (!pPlot)
				continue;

			for (int i = 0; i < pPlot->getNumUnits(); i++)
			{
				CvUnit* pUnit = pPlot->getUnitByIndex(i);
				//if it's a foreign prophet with the wrong religion ...
				if (pUnit->getTeam() != m_pPlayer->getTeam() && pUnit->AI_getUnitAIType() == UNITAI_PROPHET && pUnit->GetReligionData()->GetReligion() != eDesired)
				{
					if (pUnit->TurnsToReachTarget(pLoopCity->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1, 1)==0)
					{
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Buying an emergency inquisitor in %s", pLoopCity->getName().c_str());
							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}
						pLoopCity->Purchase(eInquisitor, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
						bResult = true;
						break;
					}
				}
			}
		}
	}

	return bResult;
}
#endif

//do we even want to spread our religion?
int CvReligionAI::GetSpreadScore() const
{
	int iScore = 0;

	//Do we have any useful beliefs to consider?
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	for(int iI = 0; iI < pkBeliefs->GetNumBeliefs(); iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
		if(pEntry && m_pPlayer->HasBelief(eBelief))
		{
			for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				if(pEntry->GetYieldFromConversion((YieldTypes)iI) > 0)
				{
					iScore++;
				}
				if (pEntry->GetYieldFromConversionExpo((YieldTypes)iI) > 0)
				{
					iScore++;
				}
				if(pEntry->GetYieldFromForeignSpread((YieldTypes)iI) > 0)
				{
					iScore++;
				}
				if(pEntry->GetYieldFromSpread((YieldTypes)iI) > 0)
				{
					iScore++;
				}
				if(pEntry->GetYieldPerFollowingCity((YieldTypes)iI) > 0)
				{
					iScore++;
				}
				if (pEntry->GetYieldPerXFollowers((YieldTypes)iI) > 0)
				{
					iScore++;
				}
			}
			if(pEntry->GetMissionaryInfluenceCS() > 0)
			{
				iScore++;
			}
		}
	}

	if (MOD_BALANCE_CORE_QUEST_CHANGES)
	{
		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
			if (eMinor != NO_PLAYER)
			{
				if (GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_CONTEST_FAITH))
				{
					iScore++;
				}
			}
		}
	}

	return iScore;
}

bool CvReligionAI::DoFaithPurchases()
{
	ReligionTypes eReligionWeFounded = m_pPlayer->GetReligions()->GetOwnedReligion(); //founded or conquered
	ReligionTypes eReligionToSpread = GetReligionToSpread(true); //independent of founding ...

	CvString strPlayer = m_pPlayer->getCivilizationShortDescription();
	UnitTypes eProphetType = m_pPlayer->GetSpecificUnitType("UNITCLASS_PROPHET", true);
	bool bAllConvertedCore = AreAllOurCitiesConverted(eReligionToSpread, false /*bIncludePuppets*/);
	bool bAllConvertedInclPuppets = AreAllOurCitiesConverted(eReligionToSpread, true /*bIncludePuppets*/);

	// Count missionaries / prophets
	int iNumMissionaries = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->GetReligionData() != NULL && pLoopUnit->GetReligionData()->GetSpreadsLeft(pLoopUnit) > 0)
			if ( pLoopUnit->GetReligionData()->GetReligion() == eReligionWeFounded || pLoopUnit->GetReligionData()->GetReligion() == eReligionToSpread)
				iNumMissionaries++;
	}

	//Let's see about our religious flavor...
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int iFlavorReligion = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));

	// Do we get benefits from spreading our religion?
	int iDesireToSpread = (eReligionWeFounded != NO_RELIGION) ? GetSpreadScore() : 0;

	//exceptions from the rule
	UnitClassTypes eUnitClassMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");
	int iMaxMissionaries = /*2 in CP, 3 in VP*/ GD_INT_GET(RELIGION_MAX_MISSIONARIES);
	if(m_pPlayer->GetPlayerTraits()->NoTrain(eUnitClassMissionary)) //india
	{
		iMaxMissionaries = 0;
	}
	else if (eReligionWeFounded == NO_RELIGION || eReligionToSpread != eReligionWeFounded)
	{
		//don't be a pawn spreading others' religion
		iMaxMissionaries = bAllConvertedInclPuppets ? 0 : 1;
	}
	else
	{
		//default
		iMaxMissionaries += iDesireToSpread;
	}

	//should we spread or save up for a prophet?
	bool bTooManyMissionaries = (iNumMissionaries >= iMaxMissionaries);
	bool bHaveEasyTargets = HaveNearbyConversionTarget(eReligionToSpread,true,true);
	bool bWantToEnhance = ((!bHaveEasyTargets || iDesireToSpread < 1) && bAllConvertedCore && IsProphetGainRateAcceptable()) || bTooManyMissionaries;

	//FIRST PRIORITY
	//Let's make sure our faith is enhanced.
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligionWeFounded, m_pPlayer->GetID());
	if (pMyReligion && !pMyReligion->m_bEnhanced && bWantToEnhance && eProphetType != NO_UNIT)
	{
		if (BuyGreatPerson(eProphetType, eReligionWeFounded))
		{
			if (GC.getLogging())
			{
				CvString strLogMsg = strPlayer + ", Bought a Prophet for religion enhancement.";
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
		}
		else
		{
			if (GC.getLogging())
			{
				CvString strLogMsg = strPlayer + ", Saving up for a Prophet for religion enhancement.";
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}

			return true; //do not allow any other faith purchases
		}
	}

	// SECOND PRIORITY
	// If in Industrial, see if we want to save for buying a great person
	if (m_pPlayer->GetCurrentEra() >= GC.getGame().GetGameReligions()->GetFaithPurchaseGreatPeopleEra(NULL) && GetDesiredFaithGreatPerson() != NO_UNIT)
	{
		UnitTypes eGPType = GetDesiredFaithGreatPerson();

		//check if it's worth the wait
		//eventually the waiting time will be short enough after we run out of other stuff to buy
		CvCity* pCapital = m_pPlayer->getCapitalCity();
		if (pCapital)
		{
			int iFaithPerTurn = m_pPlayer->GetTotalFaithPerTurn();
			int iFaithStored = m_pPlayer->GetFaith();
			int iFaithNeeded = pCapital->GetFaithPurchaseCost(eGPType, true);
			int iTurnsRemaining = (iFaithNeeded - iFaithStored) / max(1, iFaithPerTurn);
			if (iTurnsRemaining < max(13,23-iFlavorReligion))
			{
				if (BuyGreatPerson(eGPType, (eGPType == eProphetType) ? eReligionWeFounded : NO_RELIGION))
				{
					if (GC.getLogging())
					{
						CvString strLogMsg = strPlayer + ", Bought a Great Person b/c Industrial age, ";
						strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
						CvString strFaith;
						strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}
				}
				else
				{
					if (GC.getLogging())
					{
						CvString strLogMsg = strPlayer + ", Waiting to buy a Great Person b/c Industrial age, ";
						strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
						CvString strFaith;
						strFaith.Format(", Faith: %d, Turns left: %d", m_pPlayer->GetFaith(), iTurnsRemaining);
						strLogMsg += strFaith;
						GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
					}

					return true; //do not allow any other faith purchases
				}
			}
		}
	}

	//the rest needs missionaries / inquisitors of a certain religion
	//we can only create them if we have a city with that religion
	if (!m_pPlayer->GetReligions()->HasCityWithMajorityReligion(eReligionToSpread))
		return false;

	//THIRD PRIORITY
	// Might as well convert puppet-cities to build our religious strength
	const CvReligion* pSpreadReligion = GC.getGame().GetGameReligions()->GetReligion(eReligionToSpread, m_pPlayer->GetID());
	if (pSpreadReligion && !bTooManyMissionaries && !bAllConvertedInclPuppets)
	{
		//they will pick their own target!
		if (BuyMissionaryOrInquisitor(eReligionToSpread))
		{
			if (GC.getLogging())
			{
				CvString strLogMsg = strPlayer + ", Bought a Missionary/Inquisitor, Need to Convert our Cities";
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
		}
		else
		{
			if (GC.getLogging())
			{
				CvString strLogMsg = strPlayer + ", Saving up for a Missionary/Inquisitor, need to convert our Cities.";
				CvString strFaith;
				strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}

			return true; //do not allow any other faith purchases
		}
	}

	// FOURTH PRIO: FOREIGN CITIES
	if (pMyReligion && !bTooManyMissionaries && bAllConvertedCore && eReligionToSpread == eReligionWeFounded)
	{
		if (m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
		{
			if (pMyReligion->m_Beliefs.GetUniqueCiv(m_pPlayer->GetID()) == m_pPlayer->getCivilizationType())
			{
				return false;
			}
		}

		// FLAVOR DEPENDENCIES
		//Let's start with the highest-flavor stuff and work our way down...
		//Are we super religious? Target all cities, and always get Missionaries.
		if (iFlavorReligion >= 10 && HaveNearbyConversionTarget(eReligionWeFounded, true, false))
		{
			//they will pick their own target!
			if (BuyMissionary(eReligionWeFounded))
			{
				if (GC.getLogging())
				{
					CvString strLogMsg = strPlayer + ", Focusing on Missionaries, Need to Convert EVERYONE because religious zealotry";
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
			}
			else
			{
				if (GC.getLogging())
				{
					CvString strLogMsg = strPlayer + ", Saving up for Missionaries, Need to Convert EVERYONE because religious zealotry";
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
			}

			return true; //do not allow any other faith purchases
		}

		// Have civs nearby to target who didn't start a religion?
		else if ( (iFlavorReligion >= (7 - (bHaveEasyTargets ? 2 : 0)) ) && HaveNearbyConversionTarget(eReligionWeFounded, false, false))
		{
			//they will pick their own target!
			if (BuyMissionary(eReligionWeFounded))
			{
				if (GC.getLogging())
				{
					CvString strLogMsg = strPlayer + ", Focusing on Missionaries, Need to Convert Cities of Non-Religion Starters";
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}
			}
			else
			{
				if (GC.getLogging())
				{
					CvString strLogMsg = strPlayer + ", Saving up for Missionaries, Need to Convert Cities of Non-Religion Starters";
					CvString strFaith;
					strFaith.Format(", Faith: %d", m_pPlayer->GetFaith());
					strLogMsg += strFaith;
					GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
				}

				return true; //do not allow any other faith purchases
			}
		}
	}

	return false; //this allows purchasing buildings and units with leftover faith
}

// check whether a missionary or an inquisitor is better
bool CvReligionAI::BuyMissionaryOrInquisitor(ReligionTypes eReligion)
{
	//inquisitors first
	if (!HaveEnoughInquisitors(eReligion))
		return BuyInquisitor(eReligion);

	//now missionaries for all targets
	if (HaveNearbyConversionTarget(eReligion, true, false))
		return BuyMissionary(eReligion);

	return false;
}

/// Pick the right city to purchase a missionary in
bool CvReligionAI::BuyMissionary(ReligionTypes eReligion)
{
	UnitTypes eMissionary = m_pPlayer->GetSpecificUnitType("UNITCLASS_MISSIONARY");

	CvCity *pCapital = m_pPlayer->getCapitalCity();
	if (pCapital)
	{
		int iCost = pCapital->GetFaithPurchaseCost(eMissionary, true /*bIncludeBeliefDiscounts*/);
		if (iCost <= m_pPlayer->GetFaith())
		{
			CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(*m_pPlayer, eMissionary, eReligion);
			if (pBestCity)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Buying a missionary in %s", pBestCity->getName().c_str());
					m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
				}

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
	UnitTypes eInquisitor = m_pPlayer->GetSpecificUnitType("UNITCLASS_INQUISITOR");

	CvCity *pCapital = m_pPlayer->getCapitalCity();
	if (pCapital)
	{
		int iCost = pCapital->GetFaithPurchaseCost(eInquisitor, true /*bIncludeBeliefDiscounts*/);
		if (iCost <= m_pPlayer->GetFaith())
		{
			CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(*m_pPlayer, eInquisitor, eReligion);
			if (pBestCity)
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Buying an inquisitor in %s", pBestCity->getName().c_str());
					m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
				}

				pBestCity->Purchase(eInquisitor, (BuildingTypes)-1, (ProjectTypes)-1, YIELD_FAITH);
				return true;
			}
		}
	}
	return false;
}

/// Pick the right city to purchase a great person in
bool CvReligionAI::BuyGreatPerson(UnitTypes eUnit, ReligionTypes eReligion)
{
	if (eUnit!=NO_UNIT)
	{
		CvCity *pBestCity = CvReligionAIHelpers::GetBestCityFaithUnitPurchase(*m_pPlayer, eUnit, eReligion);
		if (pBestCity)
		{
			int iCost = pBestCity->GetFaithPurchaseCost(eUnit, true /*bIncludeBeliefDiscounts*/);
			if (iCost <= m_pPlayer->GetFaith())
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
	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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
	return bPurchased == true;
}
#endif
/// Any building that we can build with Faith (not Faith-generating ones)
bool CvReligionAI::BuyAnyAvailableNonFaithBuilding()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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

	int iLoop = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		ReligionTypes eReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
		if(eReligion > RELIGION_PANTHEON)
		{
			BuildingClassTypes eBuildingClass = FaithBuildingAvailable(eReligion, pLoopCity, true);
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
int CvReligionAI::ScoreBelief(CvBeliefEntry* pEntry, bool bForBonus, bool bConsiderFutureTech) const
{
	int iScorePlot = 0;
	int iScoreCity = 0;
	int iScorePlayer = 0;

	// Add in value at city level
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int i = 0; i < pLoopCity->GetNumWorkablePlots(); i++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);

			// Skip if not revealed or in enemy territory
			PlayerTypes ePlotOwner = pPlot->getOwner();
			if (pPlot->isRevealed(m_pPlayer->getTeam()) && (ePlotOwner == NO_PLAYER || ePlotOwner == m_pPlayer->GetID()))
			{
				// Score it
				int iScoreAtPlot = ScoreBeliefAtPlot(pEntry, pPlot, bConsiderFutureTech);
				if (iScoreAtPlot <= 0)
					continue;

				// Apply multiplier based on whether or not we currently own the plot
				if (ePlotOwner == m_pPlayer->GetID())
					iScoreAtPlot *= /*8*/ GD_INT_GET(RELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER);
				else
					iScoreAtPlot *= /*4*/ GD_INT_GET(RELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER);

				iScorePlot += iScoreAtPlot;
			}
		}

		int iScoreAtCity = ScoreBeliefAtCity(pEntry, pLoopCity);
		iScoreAtCity *= /*6*/ GD_INT_GET(RELIGION_BELIEF_SCORE_CITY_MULTIPLIER);
		iScoreCity += iScoreAtCity;
	}

	// Add in player-level value
	iScorePlayer = ScoreBeliefForPlayer(pEntry);

	int iRtnValue = iScorePlot + iScoreCity + iScorePlayer;

	//Final calculations
	if ((pEntry->GetRequiredCivilization() != NO_CIVILIZATION) && (pEntry->GetRequiredCivilization() == m_pPlayer->getCivilizationType()))
	{
		iRtnValue *= 5;
	}
	if (m_pPlayer->GetPlayerTraits()->IsBonusReligiousBelief() && bForBonus)
	{
		int iModifier = 0;
		if (pEntry->IsFounderBelief())
			iModifier += 8;
		else if(pEntry->IsPantheonBelief())
			iModifier += -5;
		else if (pEntry->IsEnhancerBelief())
			iModifier += 2;
		else if (pEntry->IsFollowerBelief())
		{
			bool bNoBuilding = true;
			for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				if (pEntry->IsBuildingClassEnabled(iI))
				{
					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
					CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

					if (pBuildingEntry)
					{
						bNoBuilding = false;
						if (m_pPlayer->GetPlayerTraits()->GetFaithCostModifier() != 0)
						{
							iModifier += 5;
						}
						else
						{
							iModifier += 1;
						}
						break;
					}
				}
			}
			if (bNoBuilding)
				iModifier += -2;
		}

		if (iModifier != 0)
		{
			iModifier *= 100;
			bool ShouldSpread = false;

			//Increase based on nearby cities that lack our faith.
			//Subtract the % of enhanced faiths. More enhanced = less room for spread.
			int iNumEnhancedReligions = GC.getGame().GetGameReligions()->GetNumReligionsEnhanced();
			int iReligionsEnhancedPercent = (100 * iNumEnhancedReligions) / GC.getMap().getWorldInfo().getMaxActiveReligions();

			//Let's look at all cities and get their religious status. Gives us a feel for what we can expect to gain in the near future.
			int iNumNearbyCities = GetNumCitiesWithReligionCalculator(m_pPlayer->GetReligions()->GetStateReligion(), pEntry->IsPantheonBelief());

			int iSpreadTemp = 100;
			//Increase based on nearby cities that lack our faith.
			iSpreadTemp *= iNumNearbyCities;
			//Divide by estimated total # of cities on map.
			iSpreadTemp /= GC.getMap().getWorldInfo().GetEstimatedNumCities();

			if (iReligionsEnhancedPercent <= 50 || iSpreadTemp >= 25)
				ShouldSpread = true;

			iRtnValue += ShouldSpread ? iModifier : iModifier*-1;
			if (iRtnValue <= 0)
				iRtnValue = 1;
		}
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog = NULL;
		pLog = LOGFILEMGR.GetLog("TotalBeliefScoringReligionLog.csv", FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strBaseString += playerName + ", ";

		strDesc = GetLocalizedText(pEntry->getShortDescription());
		strTemp.Format("Belief, %s, Plot: %d, City: %d, Player: %d", strDesc.GetCString(), iScorePlot, iScoreCity, iScorePlayer);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}

	return max(0, iRtnValue);
}

int CvReligionAI::GetValidPlotYield(CvBeliefEntry* pEntry, CvPlot* pPlot, YieldTypes iI, bool bConsiderFutureTech) const
{
	TerrainTypes eTerrain = pPlot->getTerrainType();
	FeatureTypes eFeature = pPlot->getFeatureType();
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	PlotTypes ePlot = pPlot->getPlotType();

	int iRtnValue = 0;

	if (eTerrain != NO_TERRAIN)
	{
		iRtnValue += pEntry->GetTerrainYieldChange(eTerrain, iI);
	}

	if (ePlot != NO_PLOT)
	{
		iRtnValue += pEntry->GetPlotYieldChange(ePlot, iI);
	}

	if (eFeature != NO_FEATURE)
	{
		iRtnValue += pEntry->GetFeatureYieldChange(eFeature, iI);
		if (pPlot->IsNaturalWonder())
		{
			iRtnValue += pEntry->GetYieldChangeNaturalWonder(iI);
			iRtnValue += (pEntry->GetYieldModifierNaturalWonder(iI) / 10);
		}

		if (eImprovement == NO_IMPROVEMENT)
		{
			iRtnValue += pEntry->GetUnimprovedFeatureYieldChange(eFeature, iI);
			iRtnValue += pEntry->GetCityYieldFromUnimprovedFeature(eFeature, iI);
		}
	}

	//Lake
	if (pPlot->isLake())
	{
		iRtnValue += pEntry->GetLakePlotYieldChange(iI) * 10;
	}

	// Resource
	if (eResource != NO_RESOURCE)
	{
		iRtnValue += pEntry->GetResourceYieldChange(eResource, iI);
		if (pEntry->GetResourceQuantityModifier(eResource) > 0)
		{
			iRtnValue += ((pPlot->getNumResource() * pEntry->GetResourceQuantityModifier(eResource)) / 20);
		}
	}

	// Improvement
	if (bConsiderFutureTech)
	{
		//look at what could be build there
		int iNumImprovementInfos = GC.getNumImprovementInfos();
		for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
			if (pkImprovementInfo && !pkImprovementInfo->IsCreatedByGreatPerson())
			{
				if (pEntry->RequiresResource() && (eResource == NO_RESOURCE || !pkImprovementInfo->IsConnectsResource(eResource)))
					continue;

				if (pPlot->HasImprovement((ImprovementTypes)jJ))
				{
					iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI)) * 3;
				}
				else if (pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
				{
					BuildTypes eThisBuild = NO_BUILD;
					for (int iK = 0; iK < GC.getNumBuildInfos(); ++iK)
					{
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iK);
						if (!pkBuildInfo)
						{
							continue;
						}

						ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

						if ((ImprovementTypes)jJ == eLoopImprovement)
						{
							eThisBuild = (BuildTypes)iK;
							break;
						}
					}
					if (eThisBuild != NO_BUILD)
					{
						if (m_pPlayer->HasTech((TechTypes)GC.getBuildInfo(eThisBuild)->getTechPrereq()))
						{
							iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI)) * 3;
						}
						else if (m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == ((TechTypes)GC.getBuildInfo(eThisBuild)->getTechPrereq()))
						{
							if (eFeature == NO_FEATURE)
								iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI)) * 2;
							else
								iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI));
						}
						else
						{
							iRtnValue += (pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, (YieldTypes)iI));
						}
					}
				}
			}
		}
	}
	else
	{
		//only look at the current improvement
		if (pPlot->getImprovementType()!=NO_IMPROVEMENT)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(pPlot->getImprovementType(), (YieldTypes)iI)) * 3;
		}
	}

	return iRtnValue;
}

/// AI's evaluation of this belief's usefulness at this one plot
int CvReligionAI::ScoreBeliefAtPlot(CvBeliefEntry* pEntry, CvPlot* pPlot, bool bConsiderFutureTech) const
{
	int iRtnValue = 0;
	int iTotalRtnValue = 0;

	// Terrain
	FeatureTypes eFeature = pPlot->getFeatureType();
	ResourceTypes eResource = pPlot->getResourceType();

	if (pEntry->RequiresResource() && eResource == NO_RESOURCE)
	{
		return 0;
	}

	ImprovementTypes eImprovement = pPlot->getImprovementType();

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (iI > YIELD_FAITH)
			continue;

		int iPersonFlavor = 0;
		CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
		switch (iI)
		{
		case YIELD_FOOD:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
			break;
		case YIELD_PRODUCTION:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
			break;
		case YIELD_GOLD:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			break;
		case YIELD_SCIENCE:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			break;
		case YIELD_CULTURE:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			break;
		case YIELD_FAITH:
			iPersonFlavor = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
			break;
		}

		iRtnValue = GetValidPlotYield(pEntry, pPlot, (YieldTypes)iI, bConsiderFutureTech);
		if (iRtnValue <= 0)
			continue;

		iTotalRtnValue += iRtnValue*iPersonFlavor;

		if (pEntry->RequiresNoImprovement() && eImprovement != NO_IMPROVEMENT)
		{
			iRtnValue *= 7;
			iRtnValue /= 10;
		}
		else if (pEntry->RequiresImprovement() && eImprovement == NO_IMPROVEMENT)
		{
			iRtnValue *= 9;
			iRtnValue /= 10;
		}
		else if (pEntry->RequiresNoFeature() && eFeature != NO_FEATURE)
		{
			iRtnValue *= 8;
			iRtnValue /= 10;
		}
		//generally means we get the bonus instantly...
		else 
		{
			iRtnValue *= 13;
			iRtnValue /= 10;
		}
	}

	return iTotalRtnValue;
}

/// AI's evaluation of this belief's usefulness at this one plot
int CvReligionAI::ScoreBeliefAtCity(CvBeliefEntry* pEntry, CvCity* pCity) const
{
	int iRtnValue = 0;
	int iTempValue = 0;
	int iMinPop = 0;
	int iMinFollowers = 0;
	int iHappinessMultiplier = 3;

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int iFlavorOffense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iFlavorCityDefense = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE"));
	int iFlavorHappiness = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_HAPPINESS"));
	int iFlavorGP = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GREAT_PEOPLE"));

	int iHappinessNeedFactor = iFlavorOffense * 2 + iFlavorHappiness - iFlavorDefense;
	if (iHappinessNeedFactor > 15)
	{
		iHappinessMultiplier = 15;
	}
	else if (iHappinessNeedFactor < 6)
	{
		iHappinessMultiplier = 6;
	}

	iMinPop = pEntry->GetMinPopulation();
	iMinFollowers = pEntry->GetMinFollowers();

	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();

	//let's establish some mid-game goals for the AI.
	int iIdealCityPop = max(m_pPlayer->getCapitalCity()->getPopulation(), 30);
	int iIdealEmpireSize = max(m_pPlayer->getNumCities(), GC.getMap().getWorldInfo().getTargetNumCities());
	if (pPlayerTraits->IsSmaller())
	{
		iIdealCityPop += 5;
		iIdealEmpireSize--;
	}
	if (pPlayerTraits->IsExpansionist())
	{
		iIdealCityPop -= 4;
		iIdealEmpireSize += 5;
	}
	if (pPlayerTraits->IsWarmonger())
	{
		iIdealCityPop -= 2;
		iIdealEmpireSize += 3;
	}


	// Simple ones
	iRtnValue += m_pPlayer->GetPlayerTraits()->IsSmaller() ? pEntry->GetCityGrowthModifier() * 2 : pEntry->GetCityGrowthModifier();
	if(pEntry->RequiresPeace())
	{
		iRtnValue /= 2 + (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest() ? 1 : -1);
	}
	iRtnValue += (pEntry->GetBorderGrowthRateIncreaseGlobal() / 7) * MAX(pEntry->GetBorderGrowthRateIncreaseGlobal() / 7, iFlavorDefense - iFlavorOffense);
	iRtnValue += (-pEntry->GetPlotCultureCostModifier() / 7) * MAX(-pEntry->GetPlotCultureCostModifier() / 7, iFlavorDefense - iFlavorOffense);

	iRtnValue += (pEntry->GetCityRangeStrikeModifier() / 3) * MAX(pEntry->GetCityRangeStrikeModifier() / 3, iFlavorCityDefense - iFlavorOffense);

	iRtnValue += (pEntry->GetFriendlyHealChange() * iFlavorDefense) / max(1, iFlavorOffense);

	// Wonder production multiplier
	if(pEntry->GetObsoleteEra() > 0)
	{	
		if (pEntry->GetObsoleteEra() > GC.getGame().getCurrentEra())
		{
			iRtnValue += (pEntry->GetWonderProductionModifier() * pEntry->GetObsoleteEra()) / 5;
		}
	}
	else
	{
		iRtnValue += pEntry->GetWonderProductionModifier() / 3;
	}

	if (m_pPlayer->GetPlayerTraits()->IsWarmonger() || m_pPlayer->GetPlayerTraits()->IsExpansionist())
		iRtnValue += pEntry->GetUnitProductionModifier();
	else
		iRtnValue += pEntry->GetUnitProductionModifier() / 2;

	// River happiness
	if (pCity->plot()->isRiver())
	{
		iTempValue = pEntry->GetRiverHappiness() * iHappinessMultiplier;
		if(iMinPop > 0)
		{
			if(pCity->getPopulation() >= iMinPop)
			{
				iTempValue *= 2;
			}
		}
		iRtnValue += iTempValue;
	}

	// Happiness per city
	iTempValue = pEntry->GetHappinessPerCity() * iHappinessMultiplier;
	if(iMinPop > 0)
	{
		if(pCity->getPopulation() >= iMinPop)
		{
			iTempValue *= 3;
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

	int iTotalRtnValue = iRtnValue;

	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion(true);

	////////////////////
	// Expansion
	///////////////////

	int iCulture = pCity->getJONSCulturePerTurn() * iIdealEmpireSize;

	bool bIsHolyCity = pCity->GetCityReligions()->IsHolyCityForReligion(eReligion);
	if (!bIsHolyCity)
	{
		int iLoopUnit = 0;
		CvUnit* pLoopUnit = NULL;
		for (pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			if (pLoopUnit == NULL)
				continue;

			if (pLoopUnit->getUnitInfo().IsFoundReligion())
			{
				if (pLoopUnit->plot()->getEffectiveOwningCity() == pCity)
				{
					bIsHolyCity = true;
					break;
				}
			}
		}
	}

	int iNumLuxuries = 0;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY && 
			(pCity->GetNumResourceLocal(eResource,false) > 0 || pCity->GetNumResourceLocal(eResource,true) > 0 || m_pPlayer->getNumResourceAvailable(eResource) > 0))
		{
			iNumLuxuries++;
		}
	}

	int iFood = max(1, pCity->getYieldRate(YIELD_FOOD, false) * iIdealCityPop);
	iTempValue = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerPop(iI) > 0)
		{
			iTempValue += iFood / pEntry->GetYieldPerPop(iI);
			if (m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
			{
				iTempValue *= 2;
			}
		}
		if (bIsHolyCity)
		{
			if (pEntry->GetHolyCityYieldChange(iI) > 0)
			{
				iTempValue += pEntry->GetHolyCityYieldChange(iI);
			}
			int iTR = pEntry->GetYieldPerActiveTR(iI);
			if (iTR > 0)
			{
				iTR = pEntry->GetYieldPerActiveTR(iI) *  pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
				if (m_pPlayer->GetPlayerTraits()->IsSmaller() || m_pPlayer->GetPlayerTraits()->IsDiplomat())
					iTR *= 5;

				iTempValue += iTR;
			}
		}
		if (pEntry->GetYieldPerLux(iI) > 0)
		{
			int ModifierValue = m_pPlayer->GetPlayerTraits()->IsExpansionist() ? 5 : 2;
			
			if (m_pPlayer->GetPlayerTraits()->GetLuxuryHappinessRetention() || m_pPlayer->GetPlayerTraits()->GetUniqueLuxuryQuantity() != 0 || m_pPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
			{
				ModifierValue += 2;
			}
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				if (m_pPlayer->GetPlayerTraits()->GetYieldFromImport((YieldTypes)iJ) != 0)
				{
					ModifierValue += 2;
				}
				if (m_pPlayer->GetPlayerTraits()->GetYieldFromExport((YieldTypes)iJ) != 0)
				{
					ModifierValue += 2;
				}
			}

			iTempValue += (pEntry->GetYieldPerLux(iI) * max(1, iNumLuxuries)) * ModifierValue;
		}
		if (pEntry->GetYieldPerBorderGrowth((YieldTypes)iI) > 0) // FIXME: also evaluate the yields that are scaling with era
		{
			int iVal = ((pEntry->GetYieldPerBorderGrowth((YieldTypes)iI) * iCulture) / max(4, pCity->GetJONSCultureLevel() * 4));
			if (m_pPlayer->GetPlayerTraits()->IsBuyOwnedTiles()) // America UA has an anti-synergy with this
			{
				iVal /= 2;
			}
			else if (m_pPlayer->GetPlayerTraits()->IsExpansionist() && m_pPlayer->GetPlayerTraits()->GetExtraFoundedCityTerritoryClaimRange() == 0)
			{
				iVal *= 2;
			}
			else if (m_pPlayer->GetPlayerTraits()->IsSmaller())
			{
				iVal /= 2;
			}
			iTempValue += iVal;
		}
	}

	iTotalRtnValue += iTempValue;

	////////////////////
	// Great People
	///////////////////
	iTempValue = 0;
	if (pCity->isCapital() || pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
		{
			GreatPersonTypes eGP = (GreatPersonTypes)iJ;
			if (eGP == NO_GREATPERSON)
				continue;

			if (pEntry->GetGreatPersonPoints(eGP) > 0)
			{
				iTempValue += (pEntry->GetGreatPersonPoints(eGP) * iFlavorGP) / 10;
			}
		}
	}

	iTotalRtnValue += iTempValue;

	////////////////////
	// Growth
	///////////////////

	iTempValue = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
#if defined(MOD_BALANCE_CORE_BELIEFS)		
		if (pEntry->GetYieldPerBirth(iI) > 0)
		{
			int iEvaluator = 400;
			if (pEntry->IsPantheonBelief())
			{
				iEvaluator -= 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsSmaller())
			{
				iEvaluator -= 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsExpansionist() || m_pPlayer->GetPlayerTraits()->IsWarmonger())
			{
				iEvaluator += 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
			{
				iTempValue += 100;
			}
			iTempValue += (pEntry->GetYieldPerBirth(iI) * (iFood / max(1, iEvaluator)));
		}
		if (bIsHolyCity && pEntry->GetYieldPerHolyCityBirth(iI) > 0)
		{
			int iEvaluator = 400;
			if (pEntry->IsPantheonBelief())
			{
				iEvaluator -= 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsSmaller())
			{
				iEvaluator -= 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsExpansionist() || m_pPlayer->GetPlayerTraits()->IsWarmonger())
			{
				iEvaluator += 50;
			}
			if (m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
			{
				iTempValue += 100;
			}
			iTempValue += (pEntry->GetYieldPerHolyCityBirth(iI) * (iFood / max(1, iEvaluator)));
		}
		if (pEntry->GetYieldFromWLTKD(iI) > 0)
		{
			iTempValue = pEntry->GetYieldFromWLTKD(iI) * 2;
			if (m_pPlayer->GetPlayerTraits()->GetWLTKDGATimer() > 0)
			{
				iTempValue += m_pPlayer->GetPlayerTraits()->GetWLTKDGATimer() * 2;
			}
			if (m_pPlayer->GetPlayerTraits()->IsGPWLTKD())
			{
				iTempValue *= 5;
			}
			if (m_pPlayer->GetPlayerTraits()->GetWLTKDGPImprovementModifier() != 0)
			{
				iTempValue *= 5;
			}
			if (m_pPlayer->GetPlayerTraits()->GetPermanentYieldChangeWLTKD((YieldTypes)iI) != 0)
			{
				iTempValue *= 5;
			}
			if (m_pPlayer->GetPlayerTraits()->GetWLTKDCulture() != 0)
			{
				iTempValue *= 5;
			}
			if (m_pPlayer->GetPlayerTraits()->IsGreatWorkWLTKD())
			{
				iTempValue *= 5;
			}
			if (m_pPlayer->GetPlayerTraits()->IsExpansionWLTKD())
			{
				iTempValue *= 5;
			}
			if (pCity->GetWeLoveTheKingDayCounter() > 0)
			{
				iTempValue += pCity->GetWeLoveTheKingDayCounter();
			}
			if (pCity->GetYieldFromWLTKD((YieldTypes)iI) > 0)
			{
				iTempValue += pCity->GetYieldFromWLTKD((YieldTypes)iI);
			}
		}
#endif
	}

	iTotalRtnValue += iTempValue;



	int iEraBonus = (GC.getNumEraInfos() - (int)m_pPlayer->GetCurrentEra());
	iEraBonus /= 2;
	if (iEraBonus <= 0)
	{
		iEraBonus = 1;
	}

	iTempValue = 0;
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		iRtnValue = 0;

		// City yield change
		iTempValue = pEntry->GetCityYieldChange(iI) * (iEraBonus + pCity->getPopulation());
		if (iMinPop > 0 && pCity->getPopulation() >= iMinPop)
		{
			iTempValue *= 2;
		}
		iRtnValue += iTempValue;

		if (pCity->isCapital()) {
			iTempValue = pEntry->GetCapitalYieldChange(iI) * iEraBonus;
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
			iTempValue = pEntry->GetCoastalCityYieldChange(iI) * iEraBonus;
			if(iMinPop > 0)
			{
				if(pCity->getPopulation() >= iMinPop)
				{
					iTempValue *= 3;
				}
			}
			iRtnValue += iTempValue;
		}
		for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
		{
			int iValidTiles = 0;
			int iUnownedTiles = 0;
			TerrainTypes eTerrain = (TerrainTypes)iJ;
			if(eTerrain != NO_TERRAIN && pEntry->GetYieldPerXTerrainTimes100(iJ, iI) > 0)
			{
				if(eTerrain == TERRAIN_MOUNTAIN)
				{
					iValidTiles = pCity->GetNearbyMountains() * 2;
				}
				else
				{
					CvPlot* pPlot = NULL;
					int iK = 0;
					CvCityCitizens* pCityCitizens = pCity->GetCityCitizens();
					for (iK = 0; iK < pCity->GetNumWorkablePlots(); iK++)
					{
						pPlot = pCityCitizens->GetCityPlotFromIndex(iK);

						if (pPlot != NULL)
						{
							if (pPlot->isCity())
								continue;

							if (pPlot->getTerrainType() == eTerrain)
							{
								bool bValid = true;

								if (pEntry->RequiresResource() && pPlot->getResourceType(m_pPlayer->getTeam()) == NO_RESOURCE)
								{
									bValid = false;
								}
								else if (pEntry->RequiresNoFeature() && (pPlot->getFeatureType() != NO_FEATURE || pPlot->isHills()))
								{
									bValid = false;
								}
								else if (pEntry->RequiresNoImprovement() && pPlot->getImprovementType() != NO_IMPROVEMENT)
								{
									bValid = false;
								}

								if (bValid)
								{
									if (pPlot->getOwner() == NO_PLAYER)
										iUnownedTiles++;
									else
										iValidTiles++;
								}
							}
						}
					}
				}

				if (iValidTiles > 0)
				{
					if (iUnownedTiles > 0)
					{
						iValidTiles += (iUnownedTiles / 2);
					}
					iRtnValue += ((iValidTiles * iEraBonus * pEntry->GetYieldPerXTerrainTimes100(iJ, iI)) / 100);
				}
			}
		}
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			int iValidTiles = 0;
			int iUnownedTiles = 0;
			FeatureTypes eFeature = (FeatureTypes) iJ;
			if(eFeature != NO_FEATURE && pEntry->GetYieldPerXFeatureTimes100(iJ, iI) > 0)
			{
				CvPlot* pPlot = NULL;
				int iK = 0;
				CvCityCitizens* pCityCitizens = pCity->GetCityCitizens();
				for (iK = 0; iK < pCity->GetNumWorkablePlots(); iK++)
				{
					pPlot = pCityCitizens->GetCityPlotFromIndex(iK);

					if (pPlot != NULL)
					{
						if (pPlot->isCity())
							continue;

						if (pPlot->getFeatureType() == eFeature)
						{
							bool bValid = true;

							if (pEntry->RequiresResource() && pPlot->getResourceType(m_pPlayer->getTeam()) == NO_RESOURCE)
							{
								bValid = false;
							}
							else if (pEntry->RequiresNoImprovement() && pPlot->getImprovementType() != NO_IMPROVEMENT)
							{
								bValid = false;
							}

							if (bValid)
							{
								if (pPlot->getOwner() == NO_PLAYER)
									iUnownedTiles++;
								else
									iValidTiles++;
							}
						}
					}
				}

				if (iValidTiles > 0)
				{
					if (iUnownedTiles > 0)
					{
						iValidTiles += (iUnownedTiles / 2);
					}

					iRtnValue += ((iValidTiles * iEraBonus * pEntry->GetYieldPerXFeatureTimes100(iJ, iI)) / 100);
				}
			}
		}

		// Trade route yield change
		iTempValue = pEntry->GetYieldChangeTradeRoute(iI) * iEraBonus;
		if(iMinPop > 0)
		{
			if(pCity->getPopulation() >= iMinPop)
			{
				iTempValue *= 5;
			}
		}
		if(pCity->IsRouteToCapitalConnected())
		{
			iTempValue *= 5;
		}
		iRtnValue += iTempValue;

		// Specialist yield change
		int iSpecialistValue = pEntry->GetYieldChangeAnySpecialist(iI) * iEraBonus;
		if (iSpecialistValue > 0)  // Like it more with large cities
		{
			iSpecialistValue += pCity->getPopulation();
		}

		if (pCity->GetCityCitizens()->GetSpecialistSlotsTotal() > 0)
		{
			iTempValue *= 2;
		}

		iRtnValue += iTempValue;


		// Building class yield change
		for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)jJ);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			iTempValue = pEntry->GetBuildingClassYieldChange(jJ, iI) * iEraBonus;
			if(iMinFollowers > 0)
			{
				if(pCity->getPopulation() < iMinFollowers)
				{
					iTempValue /= 2;
				}
			}

			BuildingTypes eBuilding = NO_BUILDING;

			if (MOD_BUILDINGS_THOROUGH_PREREQUISITES)
			{
				eBuilding = pCity->GetCityBuildings()->GetBuildingTypeFromClass((BuildingClassTypes)jJ);
			}
			else
			{
				eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings((BuildingClassTypes)jJ);
			}
			if (eBuilding != NO_BUILDING)
			{
				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					iTempValue *= 2;
				}
			}

			if(pkBuildingClassInfo->getMaxPlayerInstances() == 1 || pkBuildingClassInfo->getMaxGlobalInstances() == 1)
			{
				iTempValue /= 2;
			}

			if (m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra() && (YieldTypes)iI == YIELD_SCIENCE)
			{
				iTempValue = 0;
			}

			iRtnValue += iTempValue;
		}

		// World wonder change
		iRtnValue += pEntry->GetYieldChangeWorldWonder(iI) * m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness();

		// Yield per follower
		if (pEntry->GetMaxYieldModifierPerFollower(iI) > 0)
		{
			iTempValue = pEntry->GetMaxYieldModifierPerFollower(iI) * (pCity->getPopulation() * pCity->getPopulation()) / 10;
			iRtnValue += iTempValue;
		}

		// Yield per follower
		if (pEntry->GetMaxYieldModifierPerFollowerPercent(iI) > 0)
		{
			iTempValue = pEntry->GetMaxYieldModifierPerFollowerPercent(iI) * (pCity->getPopulation() * pCity->getPopulation()) / 100;
			iRtnValue += iTempValue;
		}

		if (pEntry->GetYieldPerConstruction(iI) > 0)
		{
			iTempValue = pEntry->GetYieldPerConstruction(iI) * (pCity->getPopulation() * pCity->getYieldRate(YIELD_PRODUCTION, false)) / 10;
			iRtnValue += iTempValue;
		}

		if (pEntry->GetYieldPerWorldWonderConstruction(iI) > 0)
		{
			iTempValue = pEntry->GetYieldPerWorldWonderConstruction(iI) * (pCity->getPopulation() * pCity->getYieldRate(YIELD_PRODUCTION, false)) / 20;
			iRtnValue += iTempValue;
		}

		iTotalRtnValue += iRtnValue;
	}

	return iTotalRtnValue;
}

#if defined(MOD_BALANCE_CORE)
int CvReligionAI::GetNumCitiesWithReligionCalculator(ReligionTypes eReligion, bool bForPantheon) const
{
	int iNumTotalCities = 0;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (kLoopPlayer.isAlive())
		{
			if (bForPantheon)
			{
				if (iPlayerLoop != m_pPlayer->GetID())
					continue;
			}

			//Ignore conversion requests.
			if (!m_pPlayer->GetDiplomacyAI()->IsPlayerMadeNoConvertPromise(kLoopPlayer.GetID()))
			{
				int iNumCities = 0;
				int iLoop = 0;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
				{
					if (eReligion == NO_RELIGION)
					{
						//No faith? Let's get em!
						if (pLoopCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
						{
							iNumCities += 2;
						}
					}
					else if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
					{
						iNumCities++;
					}
				}

				//Emphasis on our own!
				if (kLoopPlayer.GetID() == m_pPlayer->GetID())
					iNumCities *= 2;

				if (!kLoopPlayer.GetReligions()->HasCreatedReligion(true))
				{
					iNumCities *= 2;
				}

				if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_CLOSE)
				{
					iNumCities /= 2;
				}

				if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_FAR)
				{
					iNumCities /= 4;
				}

				if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_DISTANT)
				{
					iNumCities /= 6;
				}

				iNumTotalCities += iNumCities;
			}
		}
	}

	//Let's make some predictions. Earlier in the game = more cities to make. And they'll all potentially be ours!
	int iEraBonus = (GC.getNumEraInfos() - (int)m_pPlayer->GetCurrentEra());
	if (iEraBonus <= 0)
	{
		iEraBonus = 1;
	}
	iNumTotalCities *= iEraBonus;
	iNumTotalCities /= 2;

	return iNumTotalCities;
}
#endif
/// AI's evaluation of this belief's usefulness to this player
int CvReligionAI::ScoreBeliefForPlayer(CvBeliefEntry* pEntry, bool bReturnConquest, bool bReturnCulture, bool bReturnScience, bool bReturnDiplo) const
{
	int iRtnValue = 0;
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();

	if (m_pPlayer->getCapitalCity() == NULL)
		return 0;

	// == Grand Strategy ==
	int iDiploInterest = 0;
	int iConquestInterest = 0;
	int iScienceInterest = 0;
	int iCultureInterest = 0;

	int iGrandStrategiesLoop = 0;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy = NULL;
	CvString strGrandStrategyName;

	// Loop through all GrandStrategies and get priority. Since these are usually 100+, we will divide by 10 later
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GC.GetGameAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes)iGrandStrategiesLoop;
		pGrandStrategy = GC.GetGameAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString)pGrandStrategy->GetType();

		if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iConquestInterest += m_pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy) / 10;
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iCultureInterest += m_pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy) / 10;
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iDiploInterest += m_pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy) / 10;
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iScienceInterest += m_pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy) / 10;
		}
	}
	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();

	//let's establish some mid-game goals for the AI.
	int iIdealCityPop = max(m_pPlayer->getCapitalCity()->getPopulation(), 30);
	int iIdealEmpireSize = max(m_pPlayer->getNumCities(), GC.getMap().getWorldInfo().getTargetNumCities());
	if (pPlayerTraits->IsSmaller())
	{
		iIdealCityPop += 5;
		iIdealEmpireSize--;
	}
	if (pPlayerTraits->IsExpansionist())
	{
		iIdealCityPop -= 4;
		iIdealEmpireSize += 5;
	}
	if (pPlayerTraits->IsWarmonger())
	{
		iIdealCityPop -= 2;
		iIdealEmpireSize += 3;
	}


	if (pPlayerTraits->IsWarmonger())
	{
		iConquestInterest *= 3;
		iScienceInterest *= 2;
	}
	if (pPlayerTraits->IsExpansionist())
	{
		iConquestInterest *= 2;
		iCultureInterest *= 3;
	}
	if (pPlayerTraits->IsNerd())
	{
		iCultureInterest *= 2;
		iScienceInterest *= 3;
	}
	if (pPlayerTraits->IsDiplomat())
	{
		iConquestInterest *= 2;
		iDiploInterest *= 3;
	}
	if (pPlayerTraits->IsSmaller())
	{
		iCultureInterest *= 2;
		iScienceInterest *= 3;
	}
	if (pPlayerTraits->IsTourism())
	{
		iCultureInterest *= 3;
		iDiploInterest *= 2;
	}
	if (pPlayerTraits->IsReligious())
	{
		iCultureInterest *= 2;
		iDiploInterest *= 3;
	}

	UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");
	
	//Trait-specific things to consider.
	bool bNoMissionary = m_pPlayer->GetPlayerTraits()->NoTrain(eMissionary);
	bool bNoNaturalSpread = m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread();
	bool bForeignSpreadImmune = m_pPlayer->GetPlayerTraits()->IsForeignReligionSpreadImmune();

	int iNumEnhancedReligions = pGameReligions->GetNumReligionsEnhanced();
	int iReligionsEnhancedPercent = (100 * iNumEnhancedReligions) / GC.getMap().getWorldInfo().getMaxActiveReligions();

	//Let's look at all cities and get their religious status. Gives us a feel for what we can expect to gain in the near future.
	int iNumNearbyCities = GetNumCitiesWithReligionCalculator(m_pPlayer->GetReligions()->GetStateReligion(), pEntry->IsPantheonBelief());

	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion(true);

	//////////////////
	//Conquest-related player bonuses.
	///////////////////////
	int iWarTemp = 0;

	int iNumNeighbors = 0;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID() && kLoopPlayer.isMajorCiv())
		{
			if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE)
			{
				iNumNeighbors++;
				if (m_pPlayer->CanCrossOcean() && m_pPlayer->GetDiplomacyAI()->GetCivApproach((PlayerTypes)iPlayerLoop) <= CIV_APPROACH_GUARDED)
				{
					iNumNeighbors++;
				}
			}
		}
	}


	if (iNumNeighbors > 0)
	{
		if (pEntry->GetFaithFromKills() > 0)
		{
			iWarTemp += ((pEntry->GetFaithFromKills() * iNumNeighbors * iNumNeighbors) / 20);

			if (pEntry->GetMaxDistance() != 0)
			{
				iWarTemp -= pEntry->GetMaxDistance() * 2;
			}
		}

		// Unlocks units?
		int iNumUnlockEras = 0;
		for (int i = (int)m_pPlayer->GetCurrentEra(); i < GC.getNumEraInfos(); i++)
		{
			// Add in for each era enabled
			if (pEntry->IsFaithUnitPurchaseEra(i))
			{
				iNumUnlockEras++;
			}
		}

		iWarTemp += (iNumUnlockEras * iNumNeighbors);

		int iNumUnits = m_pPlayer->getNumMilitaryUnits();
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pEntry->GetYieldPerHeal(iI) > 0)
			{
				iWarTemp += (pEntry->GetYieldPerHeal(iI) * iNumUnits) / 3;
			}
			if (pEntry->GetYieldFromConquest(iI) > 0)
			{
				iWarTemp += (pEntry->GetYieldFromConquest(iI) * iNumNeighbors) / 4;
			}
			if (pEntry->GetYieldFromConquest(iI) > 0)
			{
				iWarTemp += (pEntry->GetYieldFromRemoveHeresy((YieldTypes)iI) * iNumNeighbors);
			}
			if (pEntry->GetYieldFromKills((YieldTypes)iI))
			{
				iWarTemp += (pEntry->GetYieldFromKills((YieldTypes)iI) * iNumNeighbors) / 2;

				if (pEntry->GetMaxDistance() != 0)
				{
					iWarTemp -= pEntry->GetMaxDistance();
				}
				if (m_pPlayer->GetYieldFromKills((YieldTypes)iI) > 0)
				{
					iWarTemp *= 4;
				}
				if (m_pPlayer->GetYieldFromBarbarianKills((YieldTypes)iI) > 0)
				{
					iWarTemp *= 4;
				}
			}
		}

		if (pEntry->GetCombatModifierFriendlyCities() > 0)
		{
			iWarTemp += (pEntry->GetCombatModifierFriendlyCities() * iIdealEmpireSize * iNumNeighbors) / 2;
		}

		if (pEntry->GetCombatModifierEnemyCities() > 0)
		{
			iWarTemp += (pEntry->GetCombatModifierEnemyCities() * iNumNeighbors) * 2;
		}

		if (pEntry->GetCombatVersusOtherReligionOwnLands() > 0)
		{
			iWarTemp += (pEntry->GetCombatVersusOtherReligionOwnLands() * iIdealEmpireSize * iNumNeighbors) / 4;
		}

		if (pEntry->GetCombatVersusOtherReligionTheirLands() > 0)
		{
			iWarTemp += (pEntry->GetCombatVersusOtherReligionTheirLands() * iNumNeighbors) * 2;
		}

		MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if (m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyBarbs))
		{
			if (pEntry->ConvertsBarbarians() && !bNoMissionary)
			{
				iWarTemp *= 2;
			}
		}

		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
			if (pReligion)
			{
				CvCity* pHolyCity = pReligion->GetHolyCity();

				if (pReligion->m_Beliefs.GetFaithFromKills(-1, m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iWarTemp *= 2;
				}
				if (pReligion->m_Beliefs.GetCombatModifierEnemyCities(m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iWarTemp *= 2;
				}
				if (pReligion->m_Beliefs.GetCombatModifierFriendlyCities(m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iWarTemp *= 2;
				}
				if (pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands(m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iWarTemp *= 2;
				}
				if (pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands(m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iWarTemp *= 2;
				}
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					const YieldTypes eYield = static_cast<YieldTypes>(iI);
					if (eYield != NO_YIELD)
					{
						if (pReligion->m_Beliefs.GetYieldFromKills(eYield, m_pPlayer->GetID()) > 0)
						{
							iWarTemp *= 2;
						}
					}
				}
			}
		}

		if (m_pPlayer->IsAtWarAnyMajor() || m_pPlayer->GetDiplomacyAI()->GetMeanness() > 6)
			iWarTemp *= 5;

		if (!bForeignSpreadImmune)
		{
			int iForeignReligions = 0;
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity == NULL)
					continue;

				if (pLoopCity->GetCityReligions()->IsReligionHereOtherThan(eReligion, 1))
					iForeignReligions++;
			}

			int iInquisitor = 0;
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iInquisitor += pEntry->GetYieldFromRemoveHeresy((YieldTypes)iI);
				
			}

			if (iForeignReligions > 0 )
				iInquisitor *= iForeignReligions;
			
			iWarTemp += iInquisitor;

			iWarTemp += (pEntry->GetInquisitorCostModifier() * -1 * max(1, iForeignReligions));
		}
	}

	////////////////////
	// Happiness
	///////////////////

	int iHappinessTemp = 0;
	if (pEntry->GetPlayerHappiness() > 0)
	{
		iHappinessTemp += max(0, pEntry->GetPlayerHappiness() * iIdealEmpireSize);
	}
	if (pEntry->GetHappinessPerFollowingCity() > 0)
	{
		int iFloatToInt = (int)((pEntry->GetHappinessPerFollowingCity() * (iNumNearbyCities + iIdealEmpireSize)) / 5);
		iHappinessTemp += max(0, iFloatToInt);
	}

	if (pEntry->GetFullyConvertedHappiness() > 0)
	{
		int iTemp = (pEntry->GetFullyConvertedHappiness() * iIdealEmpireSize * (m_pPlayer->GetPlayerTraits()->IsReligious() ? 4 : 2));
		iHappinessTemp += max(0, iTemp);
	}

	if (pEntry->GetHappinessPerPantheon() > 0)
	{
		int iPantheons = 0;
		for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			// Only civs we have met
			if (GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				if (GET_PLAYER((PlayerTypes)iI).GetReligions()->HasCreatedPantheon())
				{
					iPantheons++;
				}
			}
		}

		iHappinessTemp += (pEntry->GetHappinessPerPantheon() * max(3, iPantheons) * (15 - iIdealEmpireSize));
	}

	////////////////////
	// Culture
	///////////////////

	int iCultureTemp = 0;

	int iCulture = m_pPlayer->GetTotalJONSCulturePerTurn() * iIdealEmpireSize;
	iCulture /= 5;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldFromPolicyUnlock(iI) > 0)
		{
			iCultureTemp += ((pEntry->GetYieldFromPolicyUnlock(iI)) / max(1, iCulture));

			if ((YieldTypes)iI == YIELD_SCIENCE && m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
			{
				iCultureTemp /= 2;
			}
		}
	}

	////////////////////
	// Science
	///////////////////

	int iScienceTemp = 0;
	int iScience = m_pPlayer->GetScience() * 100 * iIdealEmpireSize;
	iScience /= 10;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerScience(iI) > 0)
		{		
			iScienceTemp += (iScience / pEntry->GetYieldPerScience(iI));
		}
		if (pEntry->GetYieldFromEraUnlock(iI) > 0)
		{
			iScienceTemp += (pEntry->GetYieldFromEraUnlock(iI) * (GC.getNumEraInfos() - m_pPlayer->GetCurrentEra()));
			//Big numbers skew value.
			iScienceTemp /= ((m_pPlayer->GetCurrentEra() +1) * 2);
		}
		if (m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
		{
			iScienceTemp /= 2;
		}
	}
	////////////////////
	// Gold
	///////////////////

	int iGoldTemp = 0;
	int iGrossGold = max(15, (m_pPlayer->GetTreasury()->CalculateGrossGold() * iIdealEmpireSize));
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerGPT(iI) > 0)
		{
			iGoldTemp += (iGrossGold / pEntry->GetYieldPerGPT(iI));
		}
		if (pEntry->GetYieldFromRemoveHeresy(YIELD_GOLD) > 0)
		{
			iGoldTemp += pEntry->GetYieldFromRemoveHeresy(YIELD_GOLD) / 25;
		}
	}

	////////////////////
	// Spread
	///////////////////

	int iSpreadTemp = 0;
	int iMissionary = 0;

	if (!bNoNaturalSpread)
	{
		if (pEntry->GetPressureChangeTradeRoute() != 0 && !m_pPlayer->GetPlayerTraits()->IsNoOpenTrade())
		{
			iSpreadTemp += (pEntry->GetPressureChangeTradeRoute() * m_pPlayer->GetTrade()->GetNumTradeRoutesPossible()) / 2;
			if (m_pPlayer->GetPlayerTraits()->GetNumTradeRoutesModifier() != 0)
			{
				iSpreadTemp *= 2;
			}
		}

		if (m_pPlayer->GetDiplomacyAI()->GetMeanness() <= 6)
		{
			iSpreadTemp += (pEntry->GetHappinessPerXPeacefulForeignFollowers()) * 5;
		}

		iSpreadTemp += (pEntry->GetSciencePerOtherReligionFollower()) * 2;

		iSpreadTemp += (pEntry->GetGoldPerFollowingCity()) * 2;

		iSpreadTemp += (pEntry->GetGoldPerXFollowers()) * 3;

		iSpreadTemp += (pEntry->GetGoldWhenCityAdopts()) / 2;

		iSpreadTemp += (pEntry->GetSpreadDistanceModifier()) / 2;

		iSpreadTemp += (pEntry->GetSpreadStrengthModifier()) / 2;

		if (pEntry->GetSpreadModifierDoublingTech() != NO_TECH && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(pEntry->GetSpreadModifierDoublingTech()))
		{
			iSpreadTemp *= 2;
		}

		iMissionary += (pEntry->GetMissionaryStrengthModifier()) / 2;
		iMissionary += (-1 * pEntry->GetMissionaryCostModifier()) / 2;
		iMissionary += pEntry->GetMissionaryInfluenceCS();

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			iMissionary += pEntry->GetYieldFromSpread(iI) / 5;

			iMissionary += pEntry->GetYieldFromForeignSpread(iI) / 5;

			iMissionary += pEntry->GetYieldFromConversion(iI) / 5;

			iMissionary += pEntry->GetYieldFromConversionExpo(iI) / 5;

			if ((YieldTypes)iI == YIELD_SCIENCE && m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
			{
				iMissionary /= 2;
			}
		}

		//Best for high faith civs.
		if (iMissionary > 0)
			iMissionary += ((m_pPlayer->GetTotalFaithPerTurn() * iIdealEmpireSize) / 2);

		if (m_pPlayer->GetDiplomacyAI()->GetMeanness() > 6)
		{
			iMissionary /= 2;
		}

		iSpreadTemp += iMissionary;

		if (pEntry->GetProphetStrengthModifier() != 0 || pEntry->GetProphetCostModifier() != 0)
		{
			int iProphet = 0;
			iProphet += (pEntry->GetProphetStrengthModifier()) / 2;
			iProphet += (-1 * pEntry->GetProphetCostModifier()) / 2;

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iProphet += pEntry->GetYieldFromSpread(iI) / 4;

				iProphet += pEntry->GetYieldFromForeignSpread(iI) / 5;

				iProphet += pEntry->GetYieldFromConversion(iI) / 4;

				iProphet += pEntry->GetYieldFromConversionExpo(iI) / 4;

				if ((YieldTypes)iI == YIELD_SCIENCE && m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
				{
					iProphet = 0;
				}
			}

			//Best for high faith civs.
			if (iProphet > 0)
				iProphet += m_pPlayer->GetTotalFaithPerTurn() * iIdealEmpireSize;

			iSpreadTemp += iProphet;
		}

		if (pEntry->GetFriendlyCityStateSpreadModifier() != 0)
		{
			int iSpreadTempCS = (pEntry->GetFriendlyCityStateSpreadModifier() * (m_pPlayer->GetNumCSAllies() + m_pPlayer->GetNumCSFriends() + GC.getGame().GetNumMinorCivsAlive())) / 2;
			iSpreadTemp += iSpreadTempCS;
		}

		if (pEntry->GetSpyPressure() != 0)
		{
			iSpreadTemp += (pEntry->GetSpyPressure() * m_pPlayer->GetEspionage()->GetNumSpies());
			iSpreadTemp /= 2;

			if (m_pPlayer->GetEspionageModifier() != 0)
			{
				iSpreadTemp *= 2;
			}

			if (m_pPlayer->GetFreeSpy() != 0)
			{
				iSpreadTemp *= 2;
			}
		}

		if (!bForeignSpreadImmune)
		{
			iSpreadTemp += (pEntry->GetInquisitorPressureRetention() / 5);
			iSpreadTemp += (pEntry->GetOtherReligionPressureErosion() / 5);
		}

		int iSpreadYields = 0;
		int iSpreadYieldsLocal = 0;
		// Yields for followers and follower cities
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			iSpreadYieldsLocal += pEntry->GetYieldFromFaithPurchase(iI) * 2;

			iSpreadYields += pEntry->GetYieldChangePerForeignCity(iI) * 2;

			iSpreadYields += pEntry->GetYieldChangePerXForeignFollowers(iI) * 2;

			if (pEntry->GetYieldChangePerXCityStateFollowers(iI) > 0)
				iSpreadYields += pEntry->GetYieldChangePerXCityStateFollowers(iI) * (m_pPlayer->GetNumCSAllies() + m_pPlayer->GetNumCSFriends() + GC.getGame().GetNumMinorCivsAlive()) / 2;

			if (pEntry->GetMaxYieldPerFollowerPercent(iI) > 0)
			{
				iSpreadYieldsLocal += pEntry->GetMaxYieldPerFollowerPercent(iI) * 25 / max(1, 100 - pEntry->GetMaxYieldPerFollower((YieldTypes)iI));
			}
			else
			{
				if (pEntry->GetYieldPerXFollowers((YieldTypes)iI) > 0)
				{
					int iVal = iIdealCityPop / pEntry->GetYieldPerXFollowers((YieldTypes)iI);
					iVal *= 100;
					iVal /= (100 + max(0, (2 * (iIdealCityPop - m_pPlayer->getCapitalCity()->getPopulation()))));

					if (iVal > pEntry->GetMaxYieldPerFollower(iI))
					{
						iVal = pEntry->GetMaxYieldPerFollower(iI);
					}
					if (pEntry->IsPantheonBelief())
						iVal /= 4;

					iSpreadYieldsLocal += iVal;
				}
			}

			if ((YieldTypes)iI == YIELD_SCIENCE && m_pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
			{
				iSpreadYields /= 2;
				iSpreadYieldsLocal /= 2;
			}
		}


		iSpreadYieldsLocal = (iSpreadYieldsLocal * max(m_pPlayer->getNumCities(), (iIdealCityPop / iIdealEmpireSize))) / 2;

		if (bNoNaturalSpread)
			iSpreadYields = 0;

		iSpreadTemp += iSpreadYields;

		if (iSpreadTemp > 0)
		{
			//Subtract the % of enhanced faiths. More enhanced = less room for spread.
			iSpreadTemp *= (100 - iReligionsEnhancedPercent);
			iSpreadTemp /= 100;

			//Increase based on nearby cities that lack our faith.
			iSpreadTemp *= max(1, iNumNearbyCities);
			//Divide by estimated total # of cities on map.
			iSpreadTemp /= GC.getMap().getWorldInfo().GetEstimatedNumCities();
		}

		iSpreadTemp += iSpreadYieldsLocal;
	}

	////////////////////
	// Great People
	///////////////////

	int iGPTemp = 0;

	for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
	{
		GreatPersonTypes eGP = (GreatPersonTypes)iJ;
		if (eGP == NO_GREATPERSON)
			continue;

		if (pEntry->GetGoldenAgeGreatPersonRateModifier(eGP) > 0)
		{
			iGPTemp += pEntry->GetGoldenAgeGreatPersonRateModifier(eGP) * 2;
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pEntry->GetGreatPersonExpendedYield(eGP, iI) > 0)
			{
				iGPTemp += pEntry->GetGreatPersonExpendedYield(eGP, iI) * 2;
			}
		}
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldFromGPUse(iI) > 0)
		{
			iGPTemp += pEntry->GetYieldFromGPUse(iI) * 2;
		}

		for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			if (pEntry->GetSpecialistYieldChange((SpecialistTypes)iJ, iI) > 0)
			{
				iGPTemp += (pEntry->GetSpecialistYieldChange((SpecialistTypes)iJ, iI) * 5);
			}
		}
	}

	iGPTemp += pEntry->GetGreatPersonExpendedFaith();

	if (iGPTemp != 0 && m_pPlayer->getGreatPeopleRateModifier() != 0)
	{
		iGPTemp += m_pPlayer->getGreatPeopleRateModifier() * 2;
	}

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

		iGPTemp += (iTemp * 10);
	}

	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();

			if (pReligion->m_Beliefs.GetGreatPersonExpendedFaith(m_pPlayer->GetID(), pHolyCity) > 0)
			{
				iGPTemp += (pReligion->m_Beliefs.GetGreatPersonExpendedFaith(m_pPlayer->GetID(), pHolyCity) / 2);
			}
			for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
			{
				GreatPersonTypes eGP = (GreatPersonTypes)iJ;
				if (eGP == NO_GREATPERSON)
					continue;

				if (pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, m_pPlayer->GetID(), pHolyCity) > 0)
				{
					iGPTemp += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, m_pPlayer->GetID(), pHolyCity);
				}

				for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
				{
					YieldTypes yield = (YieldTypes)ui;

					if (yield == NO_YIELD)
						continue;

					if (pReligion->m_Beliefs.GetGreatPersonExpendedYield(eGP, yield, m_pPlayer->GetID(), pHolyCity) > 0)
					{
						iGPTemp += (pReligion->m_Beliefs.GetGreatPersonExpendedYield(eGP, yield, m_pPlayer->GetID(), pHolyCity) / 2);
					}
				}
			}
		}
	}
	
	////////////////////
	// Buildings
	///////////////////

	int iBuildingTemp = 0;
	BuildingClassTypes eFaithBuildingClass = NO_BUILDINGCLASS;

	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
		if (pReligion != NULL)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();

			eFaithBuildingClass = FaithBuildingAvailable(eReligion, pHolyCity);
			for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				if (pEntry->IsBuildingClassEnabled(iI))
				{
					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
					CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

					if (pBuildingEntry)
					{
						////Sanity and AI Optimization Check
						int iSanity = pEntry->IsFollowerBelief() ? 6 : 1;

						if (FaithBuildingAvailable(eReligion, pHolyCity == NULL ? m_pPlayer->getCapitalCity() : pHolyCity) == NO_BUILDINGCLASS)
						{
							iSanity = pEntry->IsFollowerBelief() ? 25 : 2;
						}

						if (pHolyCity != NULL)
						{
							int iValue = pHolyCity->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, iSanity, false, true, true);
							if (iValue > 0)
								iBuildingTemp += iValue;
						}
						else
						{
							int iValue = m_pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, iSanity, true, true, true);
							if (iValue > 0)
								iBuildingTemp += iValue;
						}

						//Do we already have a faith building? Let's not double down.									
						//If the byzantines, let's get two national wonders!
						if (m_pPlayer->GetPlayerTraits()->IsBonusReligiousBelief())
						{
							if (pBuildingEntry->IsReformation())
							{
								iBuildingTemp *= 10;
							}
						}
						else if (eFaithBuildingClass != NO_BUILDINGCLASS)
						{
							//Only penalize if we're considering getting a second faith building.
							if (!pBuildingEntry->IsReformation())
							{
								iBuildingTemp /= 5;
							}
						}

						//special case for Orders...we really only want this if we're a warmonger.
						if (pBuildingEntry->GetFreePromotion() != NO_PROMOTION)
						{
							if (!m_pPlayer->GetPlayerTraits()->IsWarmonger())
								iBuildingTemp /= 10;
						}
					}
				}
				if (pEntry->GetBuildingClassTourism(iI) > 0)
				{
					int iLoop = 0;
					CvCity* pLoopCity = NULL;
					for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						iBuildingTemp += max(1, (pEntry->GetBuildingClassTourism(iI) * pLoopCity->GetCityBuildings()->GetNumBuildingClass((BuildingClassTypes)iI) * 5));
					}
				}
			}

			if (pEntry->GetFaithBuildingTourism() > 0)
			{
				int iLoop = 0;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuildingsFromFaith() > 0)
					{
						iBuildingTemp += max(1, (pEntry->GetFaithBuildingTourism() * pLoopCity->GetCityBuildings()->GetNumBuildingsFromFaith() * 5));
					}
				}
			}

			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity == NULL)
					continue;

				int iEraBonus = (GC.getNumEraInfos() - (int)m_pPlayer->GetCurrentEra());
				int iGW = pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots() + iEraBonus + 1;
				if (iGW > 0)
				{
					for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
					{
						YieldTypes yield = (YieldTypes)ui;

						if (yield == NO_YIELD)
							continue;

						if (pEntry->GetGreatWorkYieldChange(yield) > 0)
						{
							iBuildingTemp += (pEntry->GetGreatWorkYieldChange(yield) *iGW);
						}
					}
				}
			}
			
			if (pEntry->IsFollowerBelief())
				iBuildingTemp += iIdealEmpireSize * 10;
		}
	}

	////////////////////
	// Diplomacy
	///////////////////
	// Minimum influence with city states

	int iDiploTemp = 0;
	if (!m_pPlayer->GetPlayerTraits()->IsBullyAnnex() && !m_pPlayer->GetPlayerTraits()->IsNoAnnexing())
	{
		if (pEntry->GetCityStateMinimumInfluence() != 0)
		{
			int iNumCS = 0;

			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
				if (kLoopPlayer.isAlive() && kLoopPlayer.isMinorCiv())
				{
					iNumCS++;
					if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE)
					{
						iNumCS++;
					}
				}
			}

			iDiploTemp += (pEntry->GetCityStateMinimumInfluence() * iNumCS) / 10;
		}
	}

	if (pEntry->GetHappinessFromForeignSpies() != 0)
	{
		iDiploTemp += pEntry->GetHappinessFromForeignSpies() * max(2, m_pPlayer->GetEspionage()->GetNumSpies() * 25);
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (!bForeignSpreadImmune && !bNoNaturalSpread)
			iDiploTemp += pEntry->GetYieldPerOtherReligionFollower(iI) * 2;

		if (pEntry->GetYieldFromKnownPantheons(iI) > 0)
		{
			int iPantheonValue = (GC.getGame().GetGameReligions()->GetNumPantheonsCreated() * pEntry->GetYieldFromKnownPantheons(iI)) / 100;
			iDiploTemp += iPantheonValue * (GC.getGame().GetGameReligions()->GetNumPantheonsCreated() / 2);
		}
		if (pEntry->GetYieldFromHost(iI) > 0)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				if (pEntry->GetYieldFromHost(iI) != 0)
				{
					iDiploTemp += (pEntry->GetYieldFromHost(iI) * pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID())) / 2;
				}
				if (pLeague->GetHostMember() == m_pPlayer->GetID())
				{
					iDiploTemp *= 10;
				}
			}
			else
			{
				if (pEntry->GetYieldFromHost(iI) != 0)
				{
					iDiploTemp += (pEntry->GetYieldFromHost(iI) * m_pPlayer->GetNumCSFriends());
				}
			}
		}
		if (pEntry->GetYieldFromProposal(iI) > 0)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				iDiploTemp += ((pEntry->GetYieldFromProposal(iI) / 2) * pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID())) / 2;
			}
			else
			{
				iDiploTemp += ((pEntry->GetYieldFromProposal(iI) / 2)  * m_pPlayer->GetNumCSFriends());
			}
		}
	}
	if (pEntry->GetCSYieldBonus() > 0)
	{
		iDiploTemp += (pEntry->GetCSYieldBonus() * m_pPlayer->GetNumCSFriends()) / 2;
	}

	int iNumImprovementInfos = GC.getNumImprovementInfos();
	pair<int, int> fVoteRatio = make_pair(0, 1);
	for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
	{
		int potentialVotes = pEntry->GetImprovementVoteChange((ImprovementTypes)jJ);
		if (potentialVotes > 0)
		{
			int numImprovements = max(m_pPlayer->getImprovementCount((ImprovementTypes)jJ), 1);
			AddFractionToReference(fVoteRatio, make_pair(numImprovements, potentialVotes));
		}
	}
	iDiploTemp += 80 * fVoteRatio.first / fVoteRatio.second;
		
	if (pEntry->GetCityStateInfluenceModifier() > 0)
	{
		iDiploTemp += (pEntry->GetCityStateInfluenceModifier() * m_pPlayer->GetNumCSFriends()) / 2;
	}

	if (pEntry->GetExtraVotes() > 0)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL)
		{
			iDiploTemp += (pEntry->GetExtraVotes() * pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID()) * 2);
		}
		else
		{
			iDiploTemp += (pEntry->GetExtraVotes() * m_pPlayer->GetNumCSFriends() * 2);
		}
	}

	DomainTypes eDomain;
	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		eDomain = (DomainTypes)iI;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (pEntry->GetTradeRouteYieldChange(eDomain, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					iDiploTemp += pEntry->GetTradeRouteYieldChange(eDomain, eYield) * 4;
				}
				else
				{
					iDiploTemp += pEntry->GetTradeRouteYieldChange(eDomain, eYield) * 2;
				}
			}
		}
	}

	////////////////////
	// Other
	///////////////////

	int iPolicyGainTemp = 0;

	bool bHasPolicyBelief = false;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();

	for (int iI = 0; iI < pkBeliefs->GetNumBeliefs(); iI++)
	{
		if (GC.getGame().GetGameReligions()->IsInSomeReligion((BeliefTypes)iI, m_pPlayer->GetID()))
		{
			if (GC.GetGameBeliefs()->GetEntry((BeliefTypes)iI)->GetPolicyReductionWonderXFollowerCities() != 0)
			{
				bHasPolicyBelief = true;
				break;
			}
			else if (GC.GetGameBeliefs()->GetEntry((BeliefTypes)iI)->GetIgnorePolicyRequirementsAmount() != 0)
			{
				bHasPolicyBelief = true;
				break;
			}
		}
	}
	if (pEntry->GetIgnorePolicyRequirementsAmount() != 0 && !bHasPolicyBelief)
	{
		iPolicyGainTemp += m_pPlayer->getWonderProductionModifier() + m_pPlayer->GetPlayerTraits()->GetWonderProductionModifier();
		if (m_pPlayer->getCapitalCity() != NULL)
		{
			iPolicyGainTemp += m_pPlayer->getCapitalCity()->getProduction();
		}
	}

	if (!bHasPolicyBelief && pEntry->GetPolicyReductionWonderXFollowerCities() != 0)
	{
		iPolicyGainTemp += m_pPlayer->getWonderProductionModifier() + m_pPlayer->GetPlayerTraits()->GetWonderProductionModifier();
		if (m_pPlayer->getCapitalCity() != NULL)
		{
			iPolicyGainTemp += m_pPlayer->getCapitalCity()->getProduction();
		}
	}

	int iGoldenAgeTemp = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldBonusGoldenAge(iI) > 0)
		{
			iGoldenAgeTemp += pEntry->GetYieldBonusGoldenAge(iI) * 5;

			ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
			if (eReligion != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
				if (pReligion)
				{
					CvCity* pHolyCity = pReligion->GetHolyCity();

					for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
					{
						GreatPersonTypes eGP = (GreatPersonTypes)iJ;
						if (eGP == NO_GREATPERSON)
							continue;

						if (pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iGoldenAgeTemp += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, m_pPlayer->GetID(), pHolyCity) * 2;
						}
					}
					for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
					{
						YieldTypes yield = (YieldTypes)ui;

						if (yield == NO_YIELD)
							continue;

						if (pReligion->m_Beliefs.GetYieldBonusGoldenAge(yield, m_pPlayer->GetID(), pHolyCity) > 0)
						{
							iGoldenAgeTemp += pReligion->m_Beliefs.GetYieldBonusGoldenAge(yield, m_pPlayer->GetID(), pHolyCity) * 5;
						}
					}
				}
			}
		}
	}

	//sanity check - we don't want buildings to be the sole reason we get a founder.
	if (pEntry->IsFounderBelief() && (iWarTemp + iHappinessTemp + iGoldenAgeTemp + iScienceTemp + iGPTemp + iCultureTemp + iPolicyGainTemp + iGoldTemp + iSpreadTemp + iDiploTemp) <= 250)
		iBuildingTemp /= 100;

	if (pPlayerTraits->IsWarmonger())
	{
		iWarTemp *= 3;
		iHappinessTemp *= 2;
	}
	if (pPlayerTraits->IsNerd())
	{
		iScienceTemp *= 3;
		iGoldenAgeTemp *= 2;
	}
	if (pPlayerTraits->IsTourism())
	{
		iCultureTemp *= 3;
		iGoldenAgeTemp *= 2;
	}
	if (pPlayerTraits->IsDiplomat())
	{
		iSpreadTemp *= pEntry->IsPantheonBelief() ? 1 : 3;
		iGoldTemp *= 3;
	}
	if (pPlayerTraits->IsReligious())
	{
		iSpreadTemp *= pEntry->IsPantheonBelief() ? 2 : 3;
		iGPTemp *= 2;
	}
	if (pPlayerTraits->IsExpansionist())
	{
		iSpreadTemp *= pEntry->IsPantheonBelief() ? 2 : 3;
		iHappinessTemp *= 3;
		iPolicyGainTemp *= 2;
	}
	if (pPlayerTraits->IsSmaller())
	{
		iGoldenAgeTemp *= 2;
		iGPTemp *= 3;
	}

	//add in the existing modifier values of our other beliefs to influence this one.
	if (!bReturnConquest && !bReturnCulture && !bReturnDiplo && !bReturnScience)
	{
		ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
			if (pReligion)
			{
				CvReligionBeliefs beliefs = pReligion->m_Beliefs;
				for (int iI = 0; iI < beliefs.GetNumBeliefs(); iI++)
				{
					iWarTemp += ScoreBeliefForPlayer(GC.getBeliefInfo(beliefs.GetBelief(iI)), true) / 5;
					iCultureTemp += ScoreBeliefForPlayer(GC.getBeliefInfo(beliefs.GetBelief(iI)), false, true) / 5;
					iScienceTemp += ScoreBeliefForPlayer(GC.getBeliefInfo(beliefs.GetBelief(iI)), false, false, true) / 5;
					iGoldTemp += ScoreBeliefForPlayer(GC.getBeliefInfo(beliefs.GetBelief(iI)), false, false, false, true) / 5;
				}
			}
		}
	}

	//Take the bonus from above and multiply it by the priority value / 10 (as most are 100+, so we're getting a % interest here).

	iWarTemp *= (100 + (iConquestInterest / 10));
	iWarTemp /= 100;

	iHappinessTemp *= (100 + (iConquestInterest / 10));
	iHappinessTemp /= 100;

	iGoldenAgeTemp *= (100 + (iConquestInterest / 10));
	iGoldenAgeTemp /= 100;

	if (bReturnConquest)
		return(iWarTemp + iHappinessTemp + iGoldenAgeTemp + iBuildingTemp) / 2;

	iCultureTemp *= (100 + (iCultureInterest / 10));
	iCultureTemp /= 100;

	iGPTemp *= (100 + (iCultureInterest / 10));
	iGPTemp /= 100;

	iPolicyGainTemp *= (100 + (iCultureInterest / 10));
	iPolicyGainTemp /= 100;

	iGoldenAgeTemp *= (100 + (iCultureInterest / 10));
	iGoldenAgeTemp /= 100;

	if (bReturnCulture)
		return(iCultureTemp + iGPTemp + iPolicyGainTemp + iGoldenAgeTemp + iBuildingTemp) / 3;

	iGoldTemp *= (100 + (iDiploInterest / 10));
	iGoldTemp /= 100;

	iDiploTemp *= (100 + (iDiploInterest / 10));
	iDiploTemp /= 100;

	iSpreadTemp *= (100 + (iDiploInterest / 10));
	iSpreadTemp /= 100;

	if (bReturnDiplo)
		return(iGoldTemp + iDiploTemp + iSpreadTemp + iBuildingTemp) / 4;

	iScienceTemp *= (100 + (iScienceInterest / 10));
	iScienceTemp /= 100;

	iBuildingTemp *= (100 + (iScienceInterest / 10));
	iBuildingTemp /= 100;

	iGoldenAgeTemp *= (100 + (iScienceInterest / 10));
	iGoldenAgeTemp /= 100;

	if (bReturnScience)
		return(iGoldTemp + iScienceTemp + iBuildingTemp + iGPTemp + iGoldenAgeTemp) / 2;

	iRtnValue = (iWarTemp + iHappinessTemp + iGoldenAgeTemp + iScienceTemp + iGPTemp + iCultureTemp + iPolicyGainTemp + iGoldTemp + iSpreadTemp +  iBuildingTemp + iDiploTemp);

	if (iMissionary > 0 && bNoMissionary)
		iRtnValue /= 100;

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog = NULL;
		pLog = LOGFILEMGR.GetLog("PlayerBeliefReligionLog.csv", FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strBaseString += playerName + ", ";

		strDesc = GetLocalizedText(pEntry->getShortDescription());
		strTemp.Format("Belief, %s, War: %d, Happiness: %d, Culture: %d, Science: %d, Gold: %d, Spread: %d, GP: %d, Diplo: %d, Building: %d, Policies: %d, Golden Ages: %d", strDesc.GetCString(), iWarTemp, iHappinessTemp, iCultureTemp, iScienceTemp, iGoldTemp, iSpreadTemp, iGPTemp, iDiploTemp, iBuildingTemp, iPolicyGainTemp, iGoldenAgeTemp);
		strOutBuf = strBaseString + strTemp;
		strTemp.Format(" --- Total Value: %d. Conquest Interest: %d, Culture Interest: %d, SS Interest: %d, WC Interest: %d", iRtnValue, iConquestInterest, iCultureInterest, iScienceInterest, iDiploInterest);
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}

	return iRtnValue;
}

/// AI's evaluation of this city as a target for a missionary
int CvReligionAI::ScoreCityForMissionary(CvCity* pCity, CvUnit* pUnit, ReligionTypes eSpreadReligion) const
{
	if (pCity == NULL)
	{
		return 0;
	}

	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eSpreadReligion)->IsLocalReligion())
	{
		if (pCity->getOwner() != m_pPlayer->GetID())
		{
			return 0;
		}

		if (pCity->IsOccupied() || pCity->IsPuppet())
		{
			return 0;
		}
	}

	// Skip if not revealed
	if(!pCity->isRevealed(m_pPlayer->getTeam(),false,true))
	{
		return 0;
	}

	// Skip if at war with city owner
	if (m_pPlayer->IsAtWarWith(pCity->getOwner()))
	{
		return 0;
	}

	// Skip if already our religion
	if (pCity->GetCityReligions()->GetReligiousMajority() == eSpreadReligion)
	{
		return 0;
	}

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pSpreadReligion = pReligions->GetReligion(eSpreadReligion, m_pPlayer->GetID());
	if (!pSpreadReligion)
	{
		return 0;
	}

	//We don't want to spread our faith to unowned cities if it doesn't spread naturally and we have a unique belief (as its probably super good).
	if (m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pCity->getOwner() != m_pPlayer->GetID())
	{
		if (pSpreadReligion->m_Beliefs.GetUniqueCiv() == m_pPlayer->getCivilizationType())
		{
			return 0;
		}
	}

	// Major civ - promised not to convert, or bad target?
	if (GET_PLAYER(pCity->getOwner()).isMajorCiv())
	{
		if (m_pPlayer->GetDiplomacyAI()->IsPlayerBadTheftTarget(pCity->getOwner(), THEFT_TYPE_CONVERSION))
		{
			return 0;
		}
	}
	// Minor civ - Barbarians expected?
	else if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		if (GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_HORDE) || GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_REBELLION))
		{
			return 0;
		}
	}

	// Base score based on distance
	CvCity* pHolyCity = pSpreadReligion->GetHolyCity();
	int iDistToHolyCity = pHolyCity ? plotDistance(*pCity->plot(), *pHolyCity->plot()) : 0;
	int iDistToUnit = pUnit ? plotDistance(*pCity->plot(), *pUnit->plot()) : 0;
	int iScore = max(0, 50 - iDistToHolyCity - iDistToUnit);

	UnitTypes eMissionary = m_pPlayer->GetSpecificUnitType("UNITCLASS_MISSIONARY");
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eMissionary);
	//assume we spread multiple times with same strength for simplicity
	int iMissionaryStrength = pkUnitInfo ? pkUnitInfo->GetReligiousStrength()*pkUnitInfo->GetReligionSpreads() : 1;

	// In the early game there is little accumulated pressure and conversion is easy
	int iPressureFromUnit = iMissionaryStrength * /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
	int iTotalPressure = max(1, pCity->GetCityReligions()->GetTotalAccumulatedPressure(false));
	// Freshly founded cities have zero accumulated pressure, so limit the impact to a sane value
	int iImpactPercent = min(100, (iPressureFromUnit * 100) / iTotalPressure);

	//see if our missionary can make a dent
	int iOurPressure = pCity->GetCityReligions()->GetPressureAccumulated(eSpreadReligion);
	int iCurrentRatio = (iOurPressure * 100) / iTotalPressure;

	//make up some thresholds ...
	int iImmediateEffectScore = max(0, iImpactPercent - 23);
	int iCumulativeEffectScore = max(0, iCurrentRatio+iImpactPercent - 54);
	iScore += iImmediateEffectScore * 3;
	iScore += iCumulativeEffectScore * 3;

	//if a CS, and we have a bonus for that, emphasize.
	if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		CvCity* pHolyCity = pSpreadReligion->GetHolyCity();
		if (pSpreadReligion->m_Beliefs.GetMissionaryInfluenceCS(m_pPlayer->GetID(), pHolyCity) > 0)
		{
			iScore *= 3;
			iScore /= 2;
		}
	}

	if (pCity->GetCityReligions()->GetReligiousMajority() <= RELIGION_PANTHEON)
	{
		//fifty percent bonus if not religion at the moment
		iScore *= 3;
		iScore /= 2;
	}

	//don't target inquisitor-protected cities ...
	if (pCity->GetCityReligions()->IsDefendedAgainstSpread(eSpreadReligion))
	{
		if (MOD_BALANCE_CORE_INQUISITOR_TWEAKS)
			iScore /= max(/*2*/ GD_INT_GET(INQUISITOR_CONVERSION_REDUCTION_FACTOR), 1);
		else
			return 0;
	}

	//prefer to convert our own cities ...
	if (pCity->getOwner() == m_pPlayer->GetID())
	{
		iScore *= 2;
	}
	else
	{
		// Better score if city owner isn't starting a religion and can easily be converted to our side
		CvPlayer& kCityPlayer = GET_PLAYER(pCity->getOwner());
		if (kCityPlayer.isMajorCiv() && !kCityPlayer.GetReligions()->HasCreatedReligion() && kCityPlayer.GetReligions()->GetStateReligion() != eSpreadReligion)
		{
			iScore *= 3;
			iScore /= 2;
		}

		// Holy city will anger folks, let's not do that one right away
		ReligionTypes eCityOwnersReligion = kCityPlayer.GetReligions()->GetOwnedReligion();
		if (eCityOwnersReligion != NO_RELIGION && pCity->GetCityReligions()->IsHolyCityForReligion(eCityOwnersReligion))
		{
			iScore /= 2;
		}
	}

	return iScore;
}

/// AI's evaluation of this city as a target for an inquisitor
int CvReligionAI::ScoreCityForInquisitorOffensive(CvCity* pCity, CvUnit* pUnit, ReligionTypes eMyReligion) const
{
	if (pCity == NULL)
		return 0;

	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eMyReligion)->IsLocalReligion())
	{
		if (pCity->IsOccupied() || pCity->IsPuppet())
			return 0;
	}

	//Don't go if there are enemies around
	if (pCity->isUnderSiege())
		return 0;

	//Can only target owned cities
	if(pCity->getOwner() != m_pPlayer->GetID())
		return 0;

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eMyReligion, m_pPlayer->GetID());
	if (!pMyReligion)
		return 0;

	//Inquisitors are more expensive than Missionaries, so don't be overly zealous here

	//Looking to remove heresy?
	if (CvReligionAIHelpers::ShouldRemoveHeresy(pCity,eMyReligion))
	{
		// How much impact would using the inquistor have? let's ignore resilience here ...
		int iNumOtherFollowers = pCity->GetCityReligions()->GetFollowersOtherReligions(eMyReligion);

		//should we consider GD_INT_GET(INQUISITION_EFFECTIVENESS)?
		//it could happen that the inquisitor changes nothing ...
		//but doesn't matter usually, we still want to target the same cities!
	
		// More pressing if majority is another religion
		if (pCity->GetCityReligions()->GetReligiousMajority() != eMyReligion)
			iNumOtherFollowers *= 2;

		// Distance to the unit is the single criterion once we decide a city is eligible
		if (iNumOtherFollowers>6)
			return pUnit ? plotDistance(*pCity->plot(), *pUnit->plot()) : iNumOtherFollowers;

		//not worth spending an inquisitor yet
		return 0;
	}

	return 0;
}

/// AI's evaluation of this city as a target for an inquisitor
int CvReligionAI::ScoreCityForInquisitorDefensive(CvCity* pCity, CvUnit* pUnit, ReligionTypes eMyReligion, vector<PlayerTypes>& vUnfriendlyMajors) const
{
	//do not check whether the city already has an inquisitor, that is done on a higher level!
	if (pCity == NULL)
		return 0;

	if (MOD_RELIGION_LOCAL_RELIGIONS && GC.getReligionInfo(eMyReligion)->IsLocalReligion())
	{
		if (pCity->IsOccupied() || pCity->IsPuppet())
			return 0;
	}

	//Don't go if there are enemies around
	if (pCity->isUnderSiege())
		return 0;

	//Can only target owned cities
	if(pCity->getOwner() != m_pPlayer->GetID())
		return 0;

	//sometimes we need more active measures
	if (CvReligionAIHelpers::ShouldRemoveHeresy(pCity,eMyReligion))
		return 0;

	//see how much we want to defend passively here
	//todo: vUnfriendlyMajors may want to focus on religious threats here, not the usual threats...enemies can share a religion with no issues
	int iScore = pCity->GetCityReligions()->IsHolyCityForReligion(eMyReligion) ? 7 : 0;
	if (!vUnfriendlyMajors.empty() && pCity->isBorderCity(vUnfriendlyMajors))
		iScore += 11;

	//how vulnerable is the city to foreign missionaries trying to flip it?
	//assume their missionaries are same strength as ours
	UnitTypes eMissionary = m_pPlayer->GetSpecificUnitType("UNITCLASS_MISSIONARY");
	CvUnitEntry* pkMissionaryInfo = GC.getUnitInfo(eMissionary);
	//assume we spread multiple times with same strength for simplicity
	//note that we do not consider number of foreign followers here; that is done in offensive scoring
	int iMissionaryStrength = pkMissionaryInfo ? pkMissionaryInfo->GetReligiousStrength()*pkMissionaryInfo->GetReligionSpreads() : 1;
	int iPressureFromUnit = iMissionaryStrength * /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
	int iTotalPressure = max(1, pCity->GetCityReligions()->GetTotalAccumulatedPressure(false));
	int iImpactPercent = min(100,(iPressureFromUnit * 100) / iTotalPressure);
	iScore += iImpactPercent;

	//now we want inquisitors to stay in a city once posted, so distance must be the most important score
	//use the others only as an eligibility criterion
	if (iScore>23)
		return pUnit ? plotDistance(*pCity->plot(), *pUnit->plot()) : iScore;

	//not eligible for protection
	return 0;
}

/// Are all of our own cities our religion?
bool CvReligionAI::AreAllOurCitiesConverted(ReligionTypes eReligion, bool bIncludePuppets) const
{
	bool bRtnValue = true;

	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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
	if (m_pPlayer->GetPlayerTraits()->IsNoAnnexing())
		bIncludePuppets = true;

	bool bRtnValue = true;
	BuildingClassTypes eFaithBuildingClass = NO_BUILDINGCLASS;

	int iLoop = 0;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
		{
			if(bIncludePuppets || !pLoopCity->IsPuppet())
			{
				eFaithBuildingClass = FaithBuildingAvailable(eReligion, pLoopCity);
				BuildingTypes eFaithBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eFaithBuildingClass);

				if (eFaithBuildingClass == NO_BUILDINGCLASS)
				{
					continue;
				}

				//Exception if the option to check for all buildings in a class is enabled
				if (MOD_BUILDINGS_THOROUGH_PREREQUISITES)
				{
					if (!pLoopCity->HasBuildingClass(eFaithBuildingClass))
					{
						bRtnValue = false;
						break;
					}
				}
				else if (eFaithBuilding != NO_BUILDING)
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuilding(eFaithBuilding) < 1)
					{
						bRtnValue = false;
						break;
					}
				}
			}
		}
	}

	return bRtnValue;
}

// Is there a civ nearby that isn't pressing religion?
bool CvReligionAI::HaveNearbyConversionTarget(ReligionTypes eReligion, bool bCanIncludeReligionStarter, bool bHeathensOnly) const
{
	UnitTypes eMissionary = m_pPlayer->GetSpecificUnitType("UNITCLASS_MISSIONARY");
	int iMissionaryMoves = GC.getUnitInfo(eMissionary)->GetMoves();
	int iMaxRange = iMissionaryMoves * /*20*/ GD_INT_GET(RELIGION_MISSIONARY_RANGE_IN_TURNS);

	for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		if (m_pPlayer->IsAtWarWith(ePlayer))
			continue;

		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		bool bStartedOwnReligion = (kPlayer.GetReligionAI()->GetReligionToSpread(false) > RELIGION_PANTHEON);
		if (bStartedOwnReligion && !bCanIncludeReligionStarter)
			continue;

		int iLoop = 0;
		for(CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
		{
			if (m_pPlayer->GetCityDistanceInPlots(pCity->plot()) > iMaxRange)
				continue;

			if (bHeathensOnly)
			{
				CvCityReligions* pRel = pCity->GetCityReligions();
				if (pRel->GetReligiousMajority() != eReligion)
				{
					int iHeathens = pRel->GetNumFollowers(NO_RELIGION) + pRel->GetNumFollowers(RELIGION_PANTHEON);
					int iPopMinusTrueReligion = pCity->getPopulation() - pRel->GetNumFollowers(eReligion);

					//conversion targets should be the majority, ignore cities which already have significant presence from other religions
					if (iHeathens < iPopMinusTrueReligion / 2)
						continue;
				}
			}

			if (m_pPlayer->GetReligionAI()->ScoreCityForMissionary(pCity, NULL, eReligion) > 0)
				return true;
		}
	}

	return false;
}

bool CvReligionAI::CanHaveInquisitors(ReligionTypes eReligion) const
{
	UnitClassTypes eUnitClassInquisitor = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_INQUISITOR");
	if(eUnitClassInquisitor != NO_UNITCLASS && m_pPlayer->GetPlayerTraits()->NoTrain(eUnitClassInquisitor))
		return false;

	if (m_pPlayer->GetPlayerTraits()->IsReconquista() && m_pPlayer->GetPlayerTraits()->IsForeignReligionSpreadImmune())
		return false;

	UnitTypes eInquisitor = m_pPlayer->GetSpecificUnitType("UNITCLASS_INQUISITOR");
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eInquisitor);
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
	if (pMyReligion == NULL || pkUnitInfo == NULL)
		return false;
	if (pkUnitInfo->IsRequiresEnhancedReligion() && !pMyReligion->m_bEnhanced)
		return false;

	return true;
}

// Do we have as many Inquisitors as we need
bool CvReligionAI::HaveEnoughInquisitors(ReligionTypes eReligion) const
{
	if (!CanHaveInquisitors(eReligion))
		return true;

	//We didn't found? Don't waste too much faith on inquisitors, that's not your problem, man.
	if (eReligion != m_pPlayer->GetReligions()->GetOwnedReligion())
		return true;

	// Count Inquisitors of our religion
	int iNumInquisitors = 0;
	int iLoop = 0;
	for (CvUnit* pUnit = m_pPlayer->firstUnit(&iLoop); pUnit != NULL; pUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pUnit->getUnitInfo().IsRemoveHeresy())
		{
			if (pUnit->GetReligionData()->GetReligion() == eReligion)
			{
				iNumInquisitors++;
			}
		}
	}

	// Need one for every city in our realm that is of another religion
	int iNumNeeded = 0;
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (m_pPlayer->GetReligionAI()->ScoreCityForInquisitorOffensive(pCity, NULL, eReligion) > 0)
			iNumNeeded++;
	}

	if (iNumNeeded > 3  && iNumInquisitors > 3)
	{
		CUSTOMLOG("Warning: Player %d seems to need of inquisitors but already has a lot.", m_pPlayer->GetID());
		iNumNeeded = 3;
	}

	// In later phases we may want to have defensive inquisitors ...
	// This condition is for Spain in particular, they should go for missionaries in the beginning even though they could have inquisitors
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
	int iNumDefensive = (pMyReligion && pMyReligion->m_bEnhanced) ? 2 : 0;

	return iNumInquisitors >= iNumNeeded + iNumDefensive;
}

/// Do we have a belief that allows a faith generating building to be constructed?
BuildingClassTypes CvReligionAI::FaithBuildingAvailable(ReligionTypes eReligion, CvCity* pCity, bool bEvaluateBestPurchase) const
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());

#if defined(MOD_BALANCE_CORE_BELIEFS)
	std::vector<BuildingClassTypes> choices;
#endif

	if (pMyReligion)
	{
		CvCity* pHolyCity = pCity ? pCity : pMyReligion->GetHolyCity();

		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (pMyReligion->m_Beliefs.IsBuildingClassEnabled((BuildingClassTypes)iI, m_pPlayer->GetID(), pHolyCity))
			{
				BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings((BuildingClassTypes)iI);
				if(eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);
					if (pBuildingEntry && (pBuildingEntry->GetYieldChange(YIELD_FAITH) > 0 || pBuildingEntry->GetConversionModifier() != 0))
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
	if (choices.size() > 1)
	{
		if (bEvaluateBestPurchase)
		{
			if (pCity != NULL)
			{
				int iBest = 0;
				BuildingClassTypes eBestBuilding = NO_BUILDINGCLASS;
				////Sanity and AI Optimization Check
				for (unsigned int iI = 0; iI < choices.size(); iI++)
				{
					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(choices[iI]);
					if (eBuilding != NO_BUILDING)
					{
						int iValue = pCity->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 10, false, true, true);
						if (iValue > iBest)
						{
							iBest = iValue;
							eBestBuilding = choices[iI];
						}
					}
				}
				if (eBestBuilding != NO_BUILDINGCLASS)
					return eBestBuilding;
				else
					return choices[GC.getGame().urandLimitExclusive(choices.size(), CvSeeder(pCity->plot()->GetPseudoRandomSeed()))];
			}
			else
				return choices[GC.getGame().urandLimitExclusive(choices.size(), CvSeeder(pCity->plot()->GetPseudoRandomSeed()))];
		}
		else
			return choices[0];
	}
	else if (choices.size()==1)
		return choices[0];
#endif

	return NO_BUILDINGCLASS;
}
#if defined(MOD_BALANCE_CORE_BELIEFS)
bool CvReligionAI::IsProphetGainRateAcceptable()
{
	int iFaithPerTurn = m_pPlayer->GetTotalFaithPerTurn();

	int iFaithToNextProphet = m_pPlayer->GetReligions()->GetCostNextProphet(true, true, true);
	
	//Let's see how long it is going to take at this rate...
	int iTurns = (iFaithToNextProphet / max(1, iFaithPerTurn));

	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = GET_PLAYER(m_pPlayer->GetID()).GetReligions()->GetOwnedReligion();

	int iMaxTurns = 30;
	if (eReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());
		if (pMyReligion != NULL)
		{
			CvCity* pHolyCity = pMyReligion->GetHolyCity();

			if (FaithBuildingAvailable(eReligion, pHolyCity) != NO_BUILDINGCLASS)
			{
				iMaxTurns = 25;
			}
			//we should try to get at least one or two cities converted before we wait for a prophet again.
			if (pReligions->GetNumCitiesFollowing(eReligion) <= 2)
			{
				iMaxTurns = 20;
			}
		}
	}
	//More than max turns? Let's build some buildings or units.
	if (iTurns >= iMaxTurns)
		return false;

	return true;
}
/// Can we buy a non-Faith generating unit?
bool CvReligionAI::CanBuyNonFaithUnit() const
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop = 0;
	for(CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitTypes eUnit = m_pPlayer->GetSpecificUnitType((UnitClassTypes)iI);
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

	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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
	ReligionTypes eReligion = GetReligionToSpread(false);

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
		if(!m_pPlayer->canTrainUnit(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
		{
			continue;
		}

		// Can we purchase this one in the capital?
		CvCity *pCapital = m_pPlayer->getCapitalCity();
		if (pCapital)
		{
			if (pCapital->IsCanPurchase(false/*bTestPurchaseCost*/, false/*bTestTrainable*/, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
			{
				//let's be diverse, as hoarding faith isn't terribly useful.
				int iScore = max(0, 20000 - pCapital->GetFaithPurchaseCost(eUnit, true));

				// Score it
				if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_PROPHET"))
				{
					//make sure we can really purchase it
					if (pCapital->GetCityReligions()->GetReligiousMajority() != eReligion)
					{
						//second chance
						CvCity *pHolyCity = m_pPlayer->GetHolyCity();
						if (!pHolyCity || pHolyCity->GetCityReligions()->GetReligiousMajority() != eReligion)
							continue; //apparently we've been overwhelmed by foreign religions?
					}

					if (GetReligionToSpread(false) > RELIGION_PANTHEON)
					{
						const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
						if (pMyReligion && !pMyReligion->m_bEnhanced)
							iScore *= 2;
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
				{
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
					{
						iScore += 200;
					}
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
						if (kLoopPlayer.isAlive() && kLoopPlayer.isMajorCiv())
						{
							if (kLoopPlayer.GetDiplomacyAI()->IsCloseToCultureVictory())
							{
								iScore += 100;
							}
						}
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
				{
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
					{
						iScore += 200;
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
				{
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
					{
						iScore += 200;
					}
					else if (m_pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory())
					{
						iScore += 100;
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
				{
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory())
					{
						iScore += 200;
					}
					else
					{
						iScore += 100;
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
				{
					if (!MOD_BALANCE_VP && m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
					{
						iScore += 200;
					}
					else
					{
						iScore += 100;
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
				{
					EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH");
					if (eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
					{
						iScore += 200;
					}
					else
					{
						iScore += MAX(100, int((100.0/3.0) * (m_pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() + 0.3)));
					}
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL"))
				{
					iScore += 100;
				}
				else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_ADMIRAL"))
				{
					iScore += 100;
				}
				else if (MOD_BALANCE_VP && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
				{
					EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");
					if (eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
					{
						iScore += 200;
					}
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
					{
						iScore += 200;
					}
					else
					{
						iScore += 100;
					}
				}

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
void CvReligionAI::LogBeliefChoices(CvWeightedVector<BeliefTypes>& beliefChoices, int iChoice)
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
		FILogFile* pLog = NULL;
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
	int iLoop = 0;
	CvCity* pLoopCity = NULL;

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
				if (bReligious && pLoopCity->isUnderSiege())
					continue;

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
		CvCity* pHolyCity = pMyReligion->GetHolyCity();
		if (pHolyCity && (pHolyCity->getOwner() == kPlayer.GetID()))
		{
			if (pHolyCity->GetCityReligions()->GetReligiousMajority() == eReligion && pHolyCity->IsCanPurchase(true, true, eUnit, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
			{
				return pHolyCity;
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
		CvCity* pHolyCity = pMyReligion->GetHolyCity();
		if (pHolyCity && (pHolyCity->getOwner() == kPlayer.GetID()) && pHolyCity->IsCanPurchase(true, true, NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH))
		{
			return pHolyCity;
		}
	}

	// Now see if there is another city with our majority religion
	int iLoop = 0;
	CvCity* pLoopCity = NULL;
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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;

			ReligionTypes eReligion = GET_PLAYER(kPlayer.GetID()).GetReligions()->GetOwnedReligion();

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

bool CvReligionAIHelpers::ShouldRemoveHeresy(CvCity* pCity, ReligionTypes eTrueReligion)
{
	ReligionTypes eMajorityReligion = pCity->GetCityReligions()->GetReligiousMajority();
	if (eMajorityReligion == NO_RELIGION)
	{
		ReligionTypes eMostPowerfulReligion = pCity->GetCityReligions()->GetReligionByAccumulatedPressure(0);
		if (eMostPowerfulReligion <= RELIGION_PANTHEON)
			return false;
		if (eMostPowerfulReligion != eTrueReligion)
			return true;

		ReligionTypes eRunnerUpReligion = pCity->GetCityReligions()->GetReligionByAccumulatedPressure(1);
		int iPPT1 = pCity->GetCityReligions()->GetPressurePerTurn(eTrueReligion);
		int iPPT2 = pCity->GetCityReligions()->GetPressurePerTurn(eRunnerUpReligion);
		//want a missionary if we are losing ground
		if (iPPT2 > iPPT1)
			return true;
	}
	else if (eMajorityReligion == RELIGION_PANTHEON)
	{
		//never waste an inquisitor on a pantheon city, use a missionary instead
		return false;
	}
	else if (eMajorityReligion != eTrueReligion)
	{
		//the easy case
		return true;
	}

	return false;
}