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
#include "CvInternalGameCoreUtils.h"
#include "SqliteLoggerRegistrations.h"

#include "LintFree.h"

// Helpers for logging religion choices to the SQLite stats database

static const char* GetReligionChoiceBeliefType(BeliefTypes eBelief)
{
	if (eBelief == NO_BELIEF)
		return "";

	CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief);
	if (!pBelief)
		return "";

	if (pBelief->IsPantheonBelief())
		return "Pantheon";
	if (pBelief->IsFounderBelief())
		return "Founder";
	if (pBelief->IsFollowerBelief())
		return "Follower";
	if (pBelief->IsEnhancerBelief())
		return "Enhancer";
	if (pBelief->IsReformationBelief())
		return "Reformation";

	return "";
}

static void LogReligionChoice(PlayerTypes ePlayer, const char* szAction, BeliefTypes eBelief, const char* szBeliefTypeOverride = NULL)
{
	if (!MOD_SQLITE_LOGGING || ePlayer == NO_PLAYER)
		return;

	RegisterReligionChoicesTable();

	CvString strCiv = GET_PLAYER(ePlayer).getCivilizationShortDescription();
	CvString strBelief = "";
	CvString strBeliefType = "";

	if (eBelief != NO_BELIEF)
	{
		CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief);
		if (pBelief)
			strBelief = GetLocalizedText(pBelief->getShortDescription());

		if (szBeliefTypeOverride && szBeliefTypeOverride[0] != '\0')
			strBeliefType = szBeliefTypeOverride;
		else
			strBeliefType = GetReligionChoiceBeliefType(eBelief);
	}

	GET_SQLITE_LOGGER().BeginLogRow("ReligionChoices")
		.bind(strCiv.c_str())
		.bind(szAction)
		.bind(strBelief.c_str())
		.bind(strBeliefType.c_str())
		.execute();
}

static CvString GetBeliefNotificationText(BeliefTypes eBelief)
{
	if (eBelief == NO_BELIEF)
		return "";

	CvBeliefEntry* pBelief = GC.getBeliefInfo(eBelief);
	if (!pBelief)
		return "";

	Localization::String beliefText = Localization::Lookup("TXT_KEY_RELIGION_BELIEF_DESCRIPTION");
	beliefText << pBelief->getShortDescription() << pBelief->GetDescriptionKey();
	return beliefText.toUTF8();
}
 
//======================================================================================================
//					CvReligionEntry
//======================================================================================================
/// Constructor
CvReligionEntry::CvReligionEntry()
	: m_iLocalReligion(0)
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

	if (MOD_RELIGION_LOCAL_RELIGIONS)
		m_iLocalReligion = kResults.GetInt("LocalReligion");

	return true;
}

//------------------------------------------------------------------------------
CvString CvReligionEntry::GetIconString() const
{
	return m_strIconString;
}

//------------------------------------------------------------------------------
bool CvReligionEntry::IsLocalReligion() const
{
	return m_iLocalReligion != 0;
}

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
	, m_bReformed(false)
{
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
}

/// Constructor
CvReligion::CvReligion(ReligionTypes eReligion, PlayerTypes eFounder, CvCity* pHolyCity, bool bPantheon)
	: m_eReligion(eReligion)
	, m_eFounder(eFounder)
	, m_bPantheon(bPantheon)
	, m_bEnhanced(false)
	, m_bReformed(false)
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
	ASSERT(pEntry, "pEntry for religion not expected to be NULL.");
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
    : m_eReligion(NO_RELIGION),
      m_iFollowers(0),
      m_iPressure(0),
      m_iNumTradeRoutesApplyingPressure(0)
{
}

/// Constructor
CvReligionInCity::CvReligionInCity(ReligionTypes eReligion, int iFollowers, int iPressure)
    : m_eReligion(eReligion),
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
CvGameReligions::CvGameReligions(void) :
	m_iMinimumFaithForNextPantheon(0)
	, m_religionIndex(GC.GetGameReligions()->GetNumReligions(), -1)
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
		if (!kPlayer.isAlive())
			continue;

		// Loop through each of their cities
		int iLoop = 0;
		for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			// Ignore the same city
			if (pCity == pLoopCity)
				continue;

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

		if (iI >= MAX_MAJOR_CIVS)
			continue;

		ReligionTypes eStateReligion = kPlayer.GetReligions()->GetStateReligion(false);
		if (eStateReligion == NO_RELIGION)
			continue;

		// do we have their franchise that spreads pressure?
		int iFranchisePressure = kPlayer.GetFranchisePressure();
		if (iFranchisePressure > 0)
		{
			CorporationTypes eCorporation = kPlayer.GetCorporations()->GetFoundedCorporation();
			if (eCorporation != NO_CORPORATION && pCity->IsHasFranchise(eCorporation))
				pCity->GetCityReligions()->AddFranchisePressure(eStateReligion, iFranchisePressure);
		}

		// do they have a spy that spreads pressure?
		CvPlayerEspionage* pEspionage = kPlayer.GetEspionage();
		if (pEspionage && pEspionage->GetSpyIndexInCity(pCity) != -1)
		{
			CvEspionageSpy* pSpy = pEspionage->GetSpyByID(pEspionage->GetSpyIndexInCity(pCity));
			if (pSpy->GetSpyState() != SPY_STATE_TRAVELLING)
			{
				int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
				int iSpyPressureErosion = kPlayer.GetReligions()->GetSpyPressureErosion((PlayerTypes)iI);
				if (iSpyPressure > 0)
					pCity->GetCityReligions()->AddSpyPressure(eStateReligion, iSpyPressure);
				if (iSpyPressureErosion > 0)
					pCity->GetCityReligions()->DoSpyPressureErosion(eStateReligion, iSpyPressureErosion, (PlayerTypes)iI);
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
	else
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
	STradePathInfo sPathInfo;
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pFromCity, pToCity, sPathInfo))
	{
		int iPercent = (sPathInfo.iNormalizedDistanceRaw * 100) / iMaxDistanceLand;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pToCity, pFromCity, sPathInfo))
	{
		int iPercent = (sPathInfo.iNormalizedDistanceRaw * 100) / iMaxDistanceLand;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pFromCity, pToCity, sPathInfo))
	{
		int iPercent = (sPathInfo.iNormalizedDistanceRaw * 100) / iMaxDistanceSea;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pToCity, pFromCity, sPathInfo))
	{
		int iPercent = (sPathInfo.iNormalizedDistanceRaw * 100) / iMaxDistanceSea;
		iRelativeDistancePercent = min(iRelativeDistancePercent, iPercent);
	}

	return (iRelativeDistancePercent<100);
}

EraTypes CvGameReligions::GetFaithPurchaseGreatPeopleEra() const
{
	return /*ERA_INDUSTRIAL*/ static_cast<EraTypes>(GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA));
}

/// Religious activities at the start of a player's turn
void CvGameReligions::DoPlayerTurn(CvPlayer& kPlayer)
{
	int iFaithAtStartTimes100 = kPlayer.GetFaithTimes100();
	const PlayerTypes ePlayer = kPlayer.GetID();

	int iFaithPerTurnTimes100 = kPlayer.GetTotalFaithPerTurnTimes100();
	if(iFaithPerTurnTimes100 > 0)
	{
		kPlayer.ChangeFaithTimes100(iFaithPerTurnTimes100);
	}

	// If just now can afford missionary, add a notification
	bool bSendFaithPurchaseNotification = kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING) && kPlayer.GetFaithPurchaseType() == NO_AUTOMATIC_FAITH_PURCHASE;

	if (bSendFaithPurchaseNotification) 
	{
		if (MOD_RELIGION_RECURRING_PURCHASE_NOTIFY) 
		{
			bSendFaithPurchaseNotification = kPlayer.GetReligions()->CanAffordNextPurchase();
		} 
		else 
		{
			bool bCouldAtStartAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(iFaithAtStartTimes100);
			bool bCanNowAffordFaithPurchase = kPlayer.GetReligions()->CanAffordFaithPurchase(kPlayer.GetFaithTimes100());
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
			kPlayer.GetReligions()->SetFaithAtLastNotifyTimes100(kPlayer.GetFaithTimes100());
		}
	}

	// Check for pantheon or great prophet spawning (now restricted so must occur before Industrial era)
	if (kPlayer.GetFaithTimes100() > 0 && !kPlayer.isMinorCiv() && kPlayer.GetCurrentEra() <= /*RENAISSANCE*/ GD_INT_GET(RELIGION_LAST_FOUND_ERA))
	{
		if(CanCreatePantheon(kPlayer.GetID(), true) == FOUNDING_OK)
		{
			// Create the pantheon
			if(kPlayer.isHuman(ISHUMAN_AI_RELIGION_CHOICE))
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
		if (!kPlayer.isHuman(ISHUMAN_AI_RELIGION_CHOICE)) // continue from here
		{
			BeliefTypes eReformationBelief = kPlayer.GetReligionAI()->ChooseReformationBelief(ePlayer, eOwnedReligion);
			AddReformationBelief(ePlayer, eOwnedReligion, eReformationBelief);
		}
		else
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
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

	// AI shouldn't do human automatic faith purchases (e.g. if in observer mode)
	if (!kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING))
		return;

	switch (kPlayer.GetFaithPurchaseType())
	{
	case NO_AUTOMATIC_FAITH_PURCHASE:
		break; // Valid option; Just do nothing.
	case FAITH_PURCHASE_SAVE_PROPHET:
	{
		if ((eReligion <= RELIGION_PANTHEON && GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion()) ||
			kPlayer.GetCurrentEra() >= kPlayer.GetFaithPurchaseGreatPeopleEra())
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
						// Check if automatic purchase is disabled
						if (kPlayer.IsDisableAutomaticFaithPurchase())
						{
							// Send notification instead of purchasing
							CvNotifications* pNotifications = kPlayer.GetNotifications();
							if (pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FAITH_PURCHASE_AVAILABLE", szItemName);
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FAITH_PURCHASE_AVAILABLE");
								pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, pCity->getX(), pCity->getY(), -1);
							}
						}
						else
						{
							pCity->PurchaseUnit(eUnit, YIELD_FAITH);

							CvNotifications* pNotifications = kPlayer.GetNotifications();
							if (pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_AUTOMATIC_FAITH_PURCHASE", szItemName, pCity->getNameKey());
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_AUTOMATIC_FAITH_PURCHASE");
								pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, pCity->getX(), pCity->getY(), -1);
							}
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
						// Check if automatic purchase is disabled
						if (kPlayer.IsDisableAutomaticFaithPurchase())
						{
							// Send notification instead of purchasing
							CvNotifications* pNotifications = kPlayer.GetNotifications();
							if (pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FAITH_PURCHASE_AVAILABLE", szItemName);
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FAITH_PURCHASE_AVAILABLE");
								pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
							}
						}
						else
						{
							pCity->PurchaseBuilding(eBuilding, YIELD_FAITH);

							CvNotifications* pNotifications = kPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_AUTOMATIC_FAITH_PURCHASE", szItemName, pCity->getNameKey());
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_AUTOMATIC_FAITH_PURCHASE");
								pNotifications->Add(NOTIFICATION_CAN_BUILD_MISSIONARY, strBuffer, strSummary, -1, -1, -1);
							}
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
	if (ePlayer == NO_PLAYER)
		return FOUNDING_INVALID_PLAYER;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const int iFaithTimes100 = kPlayer.GetFaithTimes100();

	if (kPlayer.isMinorCiv() || kPlayer.getCapitalCity() == NULL)
	{
		return FOUNDING_INVALID_PLAYER;
	}

	if(HasCreatedPantheon(ePlayer) || kPlayer.GetReligions()->OwnsReligion())
	{
		return FOUNDING_PLAYER_ALREADY_CREATED_PANTHEON;
	}

	if(bCheckFaithTotal && iFaithTimes100 < GetMinimumFaithNextPantheon() * 100)
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

		if (MOD_EVENTS_FOUND_RELIGION)
		{
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanFoundPantheon, ePlayer) == GAMEEVENTRETURN_FALSE)
			{
				return FOUNDING_INVALID_PLAYER;
			}
		}
		else
		{
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
		}

	if (GetAvailablePantheonBeliefs(ePlayer).size() == 0)
		return FOUNDING_NO_BELIEFS_AVAILABLE;

	return FOUNDING_OK;
}

/// Get the appropriate religion for this player to found next
ReligionTypes CvGameReligions::GetReligionToFound(PlayerTypes ePlayer)
{
	if (!MOD_RELIGION_NO_PREFERENCES)
	{
		// Choose the civs preferred religion if not disabled and available
		ReligionTypes eCivReligion;
		eCivReligion = GET_PLAYER(ePlayer).getCivilizationInfo().GetReligion();
		
		if (MOD_EVENTS_FOUND_RELIGION)
		{
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetReligionToFound, ePlayer, eCivReligion, HasBeenFounded(eCivReligion)) == GAMEEVENTRETURN_VALUE)
			{
				// Defend against modder stupidity!
				if (iValue > RELIGION_PANTHEON && iValue < GC.getNumReligionInfos())
				{
					// CUSTOMLOG("GetReligionToFound: Before=%i, After=%i", eCivReligion, iValue);
					eCivReligion = (ReligionTypes)iValue;
				}
			}
		}
		else
		{
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
		}

		if (eCivReligion != NO_RELIGION && !HasBeenFounded(eCivReligion))
		{
			CvReligionEntry* pEntry = GC.getReligionInfo(eCivReligion);
			if(pEntry)
			{
				// CUSTOMLOG("GetReligionToFound: Using preferred %i", eCivReligion);
				return eCivReligion;
			}
		}
	}

	// No preferred religion, so find all the possible religions
	std::vector<ReligionTypes> availableReligions;
	
	// Need to "borrow" from another civ.  Loop through all religions looking for one that is eligible
	for(int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		ReligionTypes eReligion = (ReligionTypes)iI;
		CvReligionEntry* pEntry = GC.getReligionInfo(eReligion);
		if(!pEntry)
			continue;

		if(pEntry->GetID() == RELIGION_PANTHEON)
			continue;

		if (MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion())
			continue;

		if(HasBeenFounded((ReligionTypes)pEntry->GetID()))
			continue;

		// Only excluded religions preferred by other civs if not disabled
		if (!MOD_RELIGION_NO_PREFERENCES && IsPreferredByCivInGame(eReligion))
			continue;

		if (MOD_RELIGION_RANDOMISE)
		{
			// If we want a random religion, remember this as a possible candidate ...
			availableReligions.push_back(eReligion);
		}
		else
		{
			// ... otherwise just return it
			// CUSTOMLOG("GetReligionToFound: Using spare %i", eReligion);
			return (eReligion);
		}
	}

	if (availableReligions.empty())
	{
		// Will have to use a religion that someone else prefers
		for(int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			ReligionTypes eReligion = (ReligionTypes)iI;
			CvReligionEntry* pEntry = GC.getReligionInfo(eReligion);
			if(!pEntry)
				continue;

			if(pEntry->GetID() == RELIGION_PANTHEON)
				continue;

			if (MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion())
				continue;

			if(HasBeenFounded((ReligionTypes)pEntry->GetID()))
				continue;

			if (MOD_RELIGION_RANDOMISE)
			{
				// If we want a random religion, remember this as a possible candidate ...
				availableReligions.push_back(eReligion);
			}
			else
			{
				// ... otherwise just return it
				// CUSTOMLOG("GetReligionToFound: Using borrowed %i", eReligion);
				return (eReligion);
			}
		}
	}

	// Pick a random religion
	if (!availableReligions.empty())
	{
		uint index = 0;
		
		// Pick a random one if required
		if (MOD_RELIGION_RANDOMISE)
			index = GC.getGame().urandLimitExclusive(availableReligions.size(), CvSeeder(ePlayer));
		
		// CUSTOMLOG("GetReligionToFound: Using random %i", availableReligions[index]);
		return availableReligions[index];
	}

	// CUSTOMLOG("GetReligionToFound: Using NO_RELIGION");
	return NO_RELIGION;
}

/// Tell the game a new pantheon has been created
void CvGameReligions::FoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief)
{
	CvGame& kGame = GC.getGame();
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvReligion newReligion(RELIGION_PANTHEON, ePlayer, NULL, true);
	newReligion.m_Beliefs.AddBelief(eBelief, ePlayer);

	// Found it
	newReligion.m_Beliefs.SetReligion(RELIGION_PANTHEON);
	m_CurrentReligions.push_back(newReligion);

	kPlayer.GetPlayerTraits()->InitPlayerTraits();

	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman(ISHUMAN_AI_TECH_CHOICE))
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}

	// Update game systems
	kPlayer.UpdateReligion();
	kPlayer.ChangeFaith(-GetMinimumFaithNextPantheon());

	int iIncrement = /*5 in CP, 0 in VP*/ GD_INT_GET(RELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON);
	iIncrement *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iIncrement /= 100;
	SetMinimumFaithNextPantheon(GetMinimumFaithNextPantheon() + iIncrement);

	if (MOD_EVENTS_FOUND_RELIGION)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PantheonFounded, ePlayer, GET_PLAYER(ePlayer).getCapitalCity()->GetID(), RELIGION_PANTHEON, eBelief);
	}
	else
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();

		//Bugfix?
		if(pkScriptSystem && ePlayer != NO_PLAYER && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian()) 
		{
			CvLuaArgsHandle args;
			args->Push(ePlayer);

			CvCity* pCapital = kPlayer.getCapitalCity();
			if (!pCapital)
			{
				//just take the first city
				int iIdx = 0;
				pCapital = kPlayer.firstCity(&iIdx);
			}
			args->Push(pCapital ? pCapital->GetID() : 0);
			args->Push(RELIGION_PANTHEON);
			args->Push(eBelief);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "PantheonFounded", args.get(), bResult);
		}
	}

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
		CvString strBelief = GetBeliefNotificationText(eBelief);
		replayText << kPlayer.getCivilizationShortDescriptionKey() << strBelief;

		kGame.addReplayMessage(REPLAY_MESSAGE_PANTHEON_FOUNDED, newReligion.m_eFounder, replayText.toUTF8());

		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop)
		{
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications){
				// Message slightly different for founder player
				if(newReligion.m_eFounder == eNotifyPlayer)
				{
					Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER");
					localizedText << strBelief;
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
						unknownFoundedText << strBelief;
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

		if (MOD_SQLITE_LOGGING)
		{
			LogReligionChoice(ePlayer, "PANTHEON_FOUNDED", eBelief, "Pantheon");
		}

		//Achievements!
		if (MOD_ENABLE_ACHIEVEMENTS && ePlayer == GC.getGame().getActivePlayer())
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
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI), ePlayer, false);
		}
	}

	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman(ISHUMAN_AI_TECH_CHOICE))
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

	kReligion.m_Beliefs.AddBelief(eBelief1, ePlayer);
	kReligion.m_Beliefs.AddBelief(eBelief2, ePlayer);

	if(eBelief3 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief3, ePlayer);
	}

	if(eBelief4 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief4, ePlayer);
	}

	CvString strBeliefs;
	strBeliefs += GetBeliefNotificationText(eBelief1);
	strBeliefs += GetBeliefNotificationText(eBelief2);
	strBeliefs += GetBeliefNotificationText(eBelief3);
	strBeliefs += GetBeliefNotificationText(eBelief4);

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
		int iInitialPressure = /*0*/ GD_INT_GET(RELIGION_FOUND_AUTO_SPREAD_PRESSURE) * /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
		if (iInitialPressure > 0)
		{
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pkHolyCity == pLoopCity)
					continue;

				pLoopCity->GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_SCRIPTED_CONVERSION, eReligion, iInitialPressure);
				pLoopCity->GetCityReligions()->RecomputeFollowers(FOLLOWER_CHANGE_SCRIPTED_CONVERSION);
			}
		}
	}

	kPlayer.GetPlayerTraits()->InitPlayerTraits();

	// Update game systems
	kPlayer.UpdateReligion();
	kPlayer.GetReligions()->SetFoundingReligionCityID(-1);

	if (MOD_SQLITE_LOGGING)
	{
		if (eBelief1 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_FOUNDED", eBelief1);
		if (eBelief2 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_FOUNDED", eBelief2);
		if (eBelief3 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_FOUNDED", eBelief3);
		if (eBelief4 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_FOUNDED", eBelief4);
	}

	// In case we have another prophet sitting around, make sure he's set to this religion and is at full strength
	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->getUnitInfo().IsFoundReligion())
		{
			bool bSubtractOne = false;
			// If player is India, subtract one charge from any prophets who used one (either by founding or by building a Holy Site)
			if (kPlayer.GetPlayerTraits()->IsProphetFervor() && pLoopUnit->GetReligionData()->GetSpreadsUsed() > 0)
				bSubtractOne = true;

			pLoopUnit->GetReligionDataMutable()->SetFullStrength(kPlayer.GetID(), pLoopUnit->getUnitInfo(), eReligion);

			if (bSubtractOne)
			{
				pLoopUnit->GetReligionDataMutable()->IncrementSpreadsUsed();
				if (pLoopUnit->GetReligionData()->GetSpreadsLeft(pLoopUnit) <= 0)
				{
					kPlayer.DoGreatPersonExpended(pLoopUnit->getUnitType(), pLoopUnit);
					pLoopUnit->kill(true);
				}
			}
		}
	}

	if (MOD_EVENTS_FOUND_RELIGION)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionFounded, ePlayer, pkHolyCity->GetID(), eReligion, eBelief, eBelief1, eBelief2, eBelief3, eBelief4);
	}
	else
	{
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
	}

	// Send out messaging
	CvReligionEntry* pEntry = GC.getReligionInfo(kReligion.m_eReligion);
	if(pEntry)
	{
		//Add replay message
		CvString szReligionName = kReligion.GetName();
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_S");
		Localization::String replayText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED");
		replayText << kPlayer.getCivilizationShortDescriptionKey() << szReligionName << pkHolyCity->getNameKey() << strBeliefs;

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_RELIGION_FOUNDED, kReligion.m_eFounder, replayText.toUTF8(), kReligion.m_iHolyCityX, kReligion.m_iHolyCityY);

		// Local religions are intended to be "super-pantheons" so typically are founded immediately a pantheon is founded
		// As founding the pantheon sent a notification, don't send another here (if the modded wants one, they can always send it manually)
		if (!(MOD_RELIGION_LOCAL_RELIGIONS && pEntry->IsLocalReligion()))
		{
			//Notify the masses
			for (int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop)
			{
				PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
				CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);
				CvNotifications* pNotifications = kNotifyPlayer.GetNotifications();
				if (pNotifications)
				{
					// Message slightly different for founder player
					if(kReligion.m_eFounder == eNotifyPlayer)
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER");
						localizedText << szReligionName << pkHolyCity->getNameKey() << strBeliefs;
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
							unknownCivText << szReligionName << strBeliefs;
							pNotifications->Add(NOTIFICATION_RELIGION_FOUNDED, unknownCivText.toUTF8(), strSummary.toUTF8(), -1, -1, eReligion, -1);
						}
					}
				}
			}
		}

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
		if (MOD_ENABLE_ACHIEVEMENTS && ePlayer == GC.getGame().getActivePlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_11);
	}
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

/// Can the supplied religion be created?
CvGameReligions::FOUNDING_RESULT CvGameReligions::CanFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity)
{
	if (ePlayer == NO_PLAYER || GET_PLAYER(ePlayer).getCapitalCity() == NULL)
		return FOUNDING_INVALID_PLAYER;

	if (GET_PLAYER(ePlayer).GetReligions()->OwnsReligion())
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
			kReligion.m_Beliefs.AddBelief(beliefs.GetBelief(iI), ePlayer, false);
		}
	}

	kReligion.m_Beliefs.AddBelief(eBelief1, ePlayer, false);
	kReligion.m_Beliefs.AddBelief(eBelief2, ePlayer, false);

	if(eBelief3 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief3, ePlayer, false);
	}

	if(eBelief4 != NO_BELIEF)
	{
		kReligion.m_Beliefs.AddBelief(eBelief4, ePlayer, false);
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
		ASSERT(false, "Internal error in religion code.");
		CUSTOMLOG("Trying to enhance a religion/pantheon that doesn't exist!!!");
		return;
	}

	if (kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman(ISHUMAN_AI_TECH_CHOICE))
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}

	it->m_Beliefs.AddBelief(eBelief1, ePlayer);

	if (eBelief2 != NO_BELIEF)
		it->m_Beliefs.AddBelief(eBelief2, ePlayer);

	CvString strBeliefs = GetBeliefNotificationText(eBelief1);
	strBeliefs += GetBeliefNotificationText(eBelief2);

	if (eReligion != RELIGION_PANTHEON && bSetAsEnhanced)
		it->m_bEnhanced = true;

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
					notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName() << strBeliefs;
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
						localizedText << it->GetName() << strBeliefs;
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
							unknownText << it->GetName() << strBeliefs;
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

	if (MOD_SQLITE_LOGGING)
	{
		if (eBelief1 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_ENHANCED", eBelief1);
		if (eBelief2 != NO_BELIEF)
			LogReligionChoice(ePlayer, "RELIGION_ENHANCED", eBelief2);
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
		ASSERT(false, "Internal error in religion code.");
		return;
	}

	if(kPlayer.GetPlayerTraits()->IsAdoptionFreeTech())
	{
		if (!kPlayer.isHuman(ISHUMAN_AI_TECH_CHOICE))
		{
			kPlayer.AI_chooseFreeTech();
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_CHOSE_BELIEF_UA_CHOOSE_TECH");
			kPlayer.chooseTech(1, strBuffer.GetCString());
		}
	}

	it->m_Beliefs.AddBelief(eBelief1, ePlayer);
	kPlayer.GetPlayerTraits()->InitPlayerTraits();

	CvString strBelief = GetBeliefNotificationText(eBelief1);

	it->m_bReformed = true;

	// Update game systems
	UpdateAllCitiesThisReligion(eReligion);
	kPlayer.UpdateReligion();

	if (MOD_EVENTS_FOUND_RELIGION)
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ReligionReformed, ePlayer, eReligion, eBelief1);

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
			notificationText << kPlayer.getCivilizationShortDescriptionKey() << it->GetName() << strBelief;

			// Message slightly different for reformation player
			if (ePlayer == eNotifyPlayer)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER");
				localizedText << it->GetName() << strBelief;

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
					unknownText << it->GetName() << strBelief;

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

	if (MOD_SQLITE_LOGGING)
	{
		LogReligionChoice(ePlayer, "RELIGION_REFORMED", eBelief1, "Reformation");
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
	if (eReligion == NO_RELIGION)
		return NULL;

	//caching for performance (but only for real religions, not pantheons)
	if (m_religionIndex[eReligion] != -1)
		return &m_CurrentReligions[m_religionIndex[eReligion]];

	for (size_t iI = 0; iI < m_CurrentReligions.size(); iI++)
	{
		const CvReligion* pReligion = &m_CurrentReligions[iI];
		if (pReligion->m_eReligion == eReligion)
		{
			if (eReligion != RELIGION_PANTHEON)
			{
				// Update the cache
				m_religionIndex[eReligion] = iI;
				return pReligion;
			}
			// If talking about a pantheon, make sure to match the player
			else if (pReligion->m_eFounder == ePlayer)
			{
				return pReligion;
			}
		}
	}

	return NULL;
}

/// Has some religion already claimed this belief?
bool CvGameReligions::IsInSomeReligion(BeliefTypes eBelief, PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER)
		ePlayer = GC.getGame().getActivePlayer();

	bool bAnyBelief = (ePlayer == NO_PLAYER) ? false : GET_PLAYER(ePlayer).GetPlayerTraits()->IsAnyBelief();

	ReligionList::const_iterator it;
	for (it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (it->m_Beliefs.HasBelief(eBelief))
		{
			if (it->m_eFounder == ePlayer)
			{
				// If it's in my religion I definitely can't have it again
				return true;
			}
			else if (bAnyBelief)
			{
				// In the religion of someone else, but I can have any belief, so I can have it as well
				continue;
			}

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
		else if(!it->m_bPantheon && GET_PLAYER(ePlayer).GetReligions()->GetStateReligion() == it->m_eReligion)
		{
			return (BeliefTypes)it->m_Beliefs.GetBelief(0);
		}
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
std::vector<BeliefTypes> CvGameReligions::GetAvailablePantheonBeliefs(PlayerTypes ePlayer)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();

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

	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if (MOD_BALANCE_ANY_PANTHEON || !IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsPantheonBelief())
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS) {
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanHaveBelief, ePlayer, eBelief) == GAMEEVENTRETURN_FALSE) {
						bAvailable = false;
					}
				}

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
				if (bAvailable)
				{
					availableBeliefs.push_back(eBelief);
				}
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
bool CvGameReligions::IsPantheonBeliefAvailable(BeliefTypes eBelief, PlayerTypes ePlayer)
{
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();

	if (MOD_BALANCE_ANY_PANTHEON || !IsInSomeReligion(eBelief, ePlayer))
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

/// Number of followers of this religion (include ePlayer for followers in just that player's cities, although I think this duplicates GetNumDomesticFollowers())
int CvGameReligions::GetNumFollowers(ReligionTypes eReligion, PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(ePlayer != NO_PLAYER && ePlayer != (PlayerTypes)iI)
				continue;
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
	ReligionTypes eReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionCreatedByPlayer();
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
int CvGameReligions::GetNumReligionsFounded(bool bIgnoreLocal) const
{
	int iRtnValue = 0;
	for (ReligionList::const_iterator it = m_CurrentReligions.begin(); it != m_CurrentReligions.end(); it++)
	{
		if (!it->m_bPantheon)
		{
			if (MOD_RELIGION_LOCAL_RELIGIONS && bIgnoreLocal && GC.getReligionInfo(it->m_eReligion)->IsLocalReligion())
				continue;

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
int CvGameReligions::GetNumReligionsStillToFound(bool bIgnoreLocal, PlayerTypes ePlayer) const
{
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
		int iMaxReligions = GC.getGame().GetNumMajorCivsEver() * 100 / /*200*/ max(GD_INT_GET(RELIGION_MAXIMUM_PER_PLAYER_DIVISOR), 1);
		iMaxReligions += /*1*/ GD_INT_GET(RELIGION_MAXIMUM_FIXED_AMOUNT);
		return range(iMaxReligions, 1, /*8*/ GD_INT_GET(RELIGION_MAXIMUM_CAP)) - GetNumReligionsFounded(bIgnoreLocal);
	}

	return (GC.getMap().getWorldInfo().getMaxActiveReligions() - GetNumReligionsFounded(bIgnoreLocal));
}

/// List of beliefs that can be adopted by religion founders
std::vector<BeliefTypes> CvGameReligions::GetAvailableFounderBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
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
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if(!IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsFounderBelief())
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE)
						bAvailable = false;
				}
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
				if (bAvailable)
					availableBeliefs.push_back(eBelief);
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that can be adopted by religion followers
std::vector<BeliefTypes> CvGameReligions::GetAvailableFollowerBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
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
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if(!IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsFollowerBelief())
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE)
						bAvailable = false;
				}
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
				if (bAvailable)
					availableBeliefs.push_back(eBelief);
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that enhance religions
std::vector<BeliefTypes> CvGameReligions::GetAvailableEnhancerBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
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
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if(!IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsEnhancerBelief())
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE)
						bAvailable = false;
				}
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
				if (bAvailable)
					availableBeliefs.push_back(eBelief);
			}
		}
	}

	return availableBeliefs;
}

/// List of all beliefs still available
std::vector<BeliefTypes> CvGameReligions::GetAvailableBonusBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
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
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if(!IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && (pEntry->IsEnhancerBelief() || pEntry->IsFollowerBelief() || pEntry->IsFounderBelief() || pEntry->IsPantheonBelief()))
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE)
						bAvailable = false;
				}
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
				if (bAvailable)
					availableBeliefs.push_back(eBelief);
			}
		}
	}

	return availableBeliefs;
}

/// List of beliefs that are added with Reformation social policy
std::vector<BeliefTypes> CvGameReligions::GetAvailableReformationBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	std::vector<BeliefTypes> availableBeliefs;

	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	const int iNumBeleifs = pkBeliefs->GetNumBeliefs();
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
	availableBeliefs.reserve(iNumBeleifs);
	for(int iI = 0; iI < iNumBeleifs; iI++)
	{
		const BeliefTypes eBelief(static_cast<BeliefTypes>(iI));
		if(!IsInSomeReligion(eBelief, ePlayer))
		{
			CvBeliefEntry* pEntry = pkBeliefs->GetEntry(eBelief);
			if(pEntry && pEntry->IsReformationBelief())
			{
				bool bAvailable = true;

				if (MOD_EVENTS_ACQUIRE_BELIEFS)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_ReligionCanHaveBelief, ePlayer, eReligion, eBelief) == GAMEEVENTRETURN_FALSE)
						bAvailable = false;
				}
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
				if (bAvailable)
					availableBeliefs.push_back(eBelief);
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

	// Does this city have a majority religion?
	ReligionTypes eMajorityReligion = pFromCity->GetCityReligions()->GetReligiousMajority();
	if (eMajorityReligion != eReligion)
	{
		return 0;
	}

	int iBasePressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity();
	int iBasePressureMod = 0;
	int iPressureMod = 0;

	// India: +10% base pressure per follower
	if (GET_PLAYER(pFromCity->getOwner()).GetPlayerTraits()->IsPopulationBoostReligion())
	{
		if (eReligion == GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetStateReligion(true))
		{
			int iPopExtraPressure = pFromCity->GetCityReligions()->GetNumFollowers(eReligion);
			iBasePressureMod += min(35, iPopExtraPressure) * 10;
		}
	}

	// Global base pressure modifier from buildings
	int iPlayerBasePressureMod = GET_PLAYER(pFromCity->getOwner()).GetBasePressureModifier();
	if (iPlayerBasePressureMod != 0)
	{
		if (eReligion == GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetStateReligion(true))
		{
			iBasePressureMod += iPlayerBasePressureMod;
		}
	}
	
	if (iBasePressureMod != 0)
	{
		iBasePressure *= 100 + iBasePressureMod;
		iBasePressure /= 100;
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
		if (MOD_BALANCE_PASSIVE_SPREAD_BY_CONNECTION)
		{
			//no trade route and no city connection, no pressure!
			if (pFromCity->getOwner() != pToCity->getOwner() || !GET_PLAYER(pFromCity->getOwner()).IsCityConnectedToCity(pFromCity,pToCity))
				return 0;
		}

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
		//If the faith being spread is our founded faith, or our adopted faith, we get the bonus.
		if (eReligion == GET_PLAYER(pFromCity->getOwner()).GetReligions()->GetStateReligion(true))
		{
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
		iPressureMod += /*100*/ GD_INT_GET(VASSAL_PRESSURE_PERCENT);
	}

	// Modify iPressure based on city defenses, but only against hostile cities (i.e., any not the same player as this city)
	PlayerTypes eFromPlayer = pFromCity->getOwner();
	PlayerTypes eToPlayer = pToCity->getOwner();
	
	if (eFromPlayer != eToPlayer)
	{
		CvPlayer& pToPlayer = GET_PLAYER(eToPlayer);
		int iCityModifier = pToCity->GetConversionModifier() + pToPlayer.GetConversionModifier() + pToPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CONVERSION_MODIFIER);
		
		if (MOD_BALANCE_QUEST_CHANGES && iCityModifier < 0 && pToPlayer.isMinorCiv())
		{
			if (pToPlayer.GetMinorCivAI()->IsActiveQuestForPlayer(eFromPlayer, MINOR_CIV_QUEST_SPREAD_RELIGION) && pToPlayer.GetMinorCivAI()->GetQuestData1(eFromPlayer, MINOR_CIV_QUEST_SPREAD_RELIGION) == eReligion)
				iCityModifier = 0; // The City-State actively wants this religion
		}

		iPressureMod += iCityModifier;
	}

	int iPressure = iBasePressure * (100 + iPressureMod);

	// CUSTOMLOG("GetAdjacentCityReligiousPressure for %i from %s to %s is %i", eReligion, pFromCity->getName().c_str(), pToCity->getName().c_str(), iPressure);
	return max(0, iPressure / 100);
}

/// How much does this prophet cost (recursive)
int CvGameReligions::GetFaithGreatProphetNumber(int iNum) const
{
	int iRtnValue = 0;

	if (iNum >= 1)
	{
		if (iNum == 1)
		{
			iRtnValue = /*200 in CP, 800 in VP*/ GD_INT_GET(RELIGION_MIN_FAITH_FIRST_PROPHET);
		}
		else if (MOD_BALANCE_NEW_GREAT_PERSON_ATTRIBUTES && iNum == 2)
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

	// mod for civs keeping their pantheon belief forever
	if (MOD_BALANCE_PERMANENT_PANTHEONS)
	{
		if (HasCreatedPantheon(eWinningPlayer))
		{
			const CvReligion* pPantheon = GetReligion(RELIGION_PANTHEON, eWinningPlayer);
			BeliefTypes ePantheonBelief = GetBeliefInPantheon(eWinningPlayer);
			if (pPantheon != NULL && ePantheonBelief != NO_BELIEF)
			{
				const CvReligion* pReligion = GetReligion(eReligion, eWinningPlayer);
				if (pReligion == NULL || !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eReligion, eWinningPlayer)) // check that the our religion does not have our belief, to prevent double counting
				{
					iRtnValue += MAX(0, pPantheon->m_Beliefs.GetFaithFromKills(iDistance, eWinningPlayer, pLoopCity));
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
	UnitTypes eUnit = kPlayer.GetSpecificUnitType("UNITCLASS_PROPHET", true);

	if (eUnit == NO_UNIT)
	{
		return false;
	}

	bool prophetboughtwithfaith = false;
	const CvReligion* pReligion = NULL;
	const int iFaithTimes100 = kPlayer.GetFaithTimes100();
	int iCost = kPlayer.GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, true /*bAdjustForSpeedDifficulty*/, MOD_GLOBAL_TRULY_FREE_GP);

	ReligionTypes ePlayerReligion = GET_PLAYER(kPlayer.GetID()).GetReligions()->GetOwnedReligion();
	if (ePlayerReligion != NO_RELIGION)
	{
		pReligion = GetReligion(ePlayerReligion, kPlayer.GetID());

		// Don't check this in Classical for Byzantium, if a religion is owned
		if (kPlayer.GetCurrentEra() >= kPlayer.GetFaithPurchaseGreatPeopleEra())
			return false;
	}

	// If player hasn't founded a religion yet, drop out of this if all religions have been founded
	else if (GetNumReligionsStillToFound() <= 0 && !kPlayer.GetPlayerTraits()->IsAlwaysReligion())
	{
		return false;
	}

	if(iFaithTimes100 < iCost * 100)
	{
		return false;
	}

	int iChance = /*5 in CP, 100 in VP*/ GD_INT_GET(RELIGION_BASE_CHANCE_PROPHET_SPAWN);

	int iBaseChance = iChance;
	iChance += (iFaithTimes100 / 100 - iCost);

	int iRand = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x88a8fa44).mix(kPlayer.GetID()));
	if (iRand > iChance)
		return false;

	CvCity* pSpawnCity = pReligion ? pReligion->GetHolyCity() : NULL;
	if(pSpawnCity != NULL && pSpawnCity->getOwner() == kPlayer.GetID())
	{
		if(MOD_BALANCE_NO_AUTO_SPAWN_PROPHET)
		{
			if (kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING))
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
							kPlayer.GetReligions()->SetFaithAtLastNotifyTimes100(kPlayer.GetFaithTimes100());
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
		{
			pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
		}

		if (MOD_RELIGION_KEEP_PROPHET_OVERFLOW && iBaseChance >= 100)
		{
			if(MOD_BALANCE_NO_AUTO_SPAWN_PROPHET)
			{
				if (!kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING) || prophetboughtwithfaith)
				{
					kPlayer.ChangeFaith(-1 * iCost);
				}
			}
			else
				kPlayer.ChangeFaith(-1 * iCost);
		}
		else
		{
			if(MOD_BALANCE_NO_AUTO_SPAWN_PROPHET)
			{
				if (!kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING) || prophetboughtwithfaith)
				{
					kPlayer.SetFaithTimes100(0);
				}
			}
			else
				kPlayer.SetFaithTimes100(0);
		kPlayer.SetFaithTimes100(0);

		}
	}
	else
	{
		//Random GP spawn/holy city.
		CvCity* pBestCity = NULL;
		if (MOD_BALANCE_RANDOMIZED_GREAT_PROPHET_SPAWNS)
		{
			int iBestWeight = 0;

			int iTempWeight = 0;

			CvCity* pLoopCity = NULL;
			int iLoop = 0;
			CvGame& theGame = GC.getGame();
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				iTempWeight = pLoopCity->getYieldRateTimes100(YIELD_FAITH) / 20;
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
			if(MOD_BALANCE_NO_AUTO_SPAWN_PROPHET)
			{
				if (kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING))
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
								kPlayer.GetReligions()->SetFaithAtLastNotifyTimes100(kPlayer.GetFaithTimes100());
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
			{
				pSpawnCity->GetCityCitizens()->DoSpawnGreatPerson(eUnit, true /*bIncrementCount*/, true, false);
			}

			if (!MOD_BALANCE_NO_AUTO_SPAWN_PROPHET || !kPlayer.isHuman(ISHUMAN_AI_FAITH_SPENDING) || prophetboughtwithfaith)
			{
				if (MOD_RELIGION_KEEP_PROPHET_OVERFLOW && iBaseChance >= 100)
					kPlayer.ChangeFaith(-1 * iCost);
				else
					kPlayer.SetFaithTimes100(0);
			}
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
	m_iNumFreeProphetsSpawned(0),
	m_iNumProphetsSpawned(0),
	m_iFoundingReligionCityID(-1),
	m_iFaithAtLastNotifyTimes100(0),
	m_eMajorityReligion(NO_RELIGION),
	m_eStateReligion(NO_RELIGION),
	m_eStateReligionOverride(NO_RELIGION),
	m_bOwnsStateReligion(false)
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
	m_iFoundingReligionCityID = -1;
	m_iNumFreeProphetsSpawned = 0;
	m_iNumProphetsSpawned = 0;
	m_iFaithAtLastNotifyTimes100 = 0;
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
	visitor(playerReligions.m_iFoundingReligionCityID);
	visitor(playerReligions.m_eMajorityReligion);
	visitor(playerReligions.m_eStateReligionOverride);
	visitor(playerReligions.m_eStateReligion);
	visitor(playerReligions.m_bOwnsStateReligion);
	visitor(playerReligions.m_iFaithAtLastNotifyTimes100);
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
int CvPlayerReligions::GetNumProphetsSpawned(bool bExcludeFree) const
{
	int iCount = m_iNumProphetsSpawned;
	if (bExcludeFree)
		iCount -= m_iNumFreeProphetsSpawned;
	return iCount;
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
void CvPlayerReligions::ChangeNumProphetsSpawned(int iValue, bool bIsFree)
{
	if (bIsFree)
		m_iNumFreeProphetsSpawned += iValue;
	m_iNumProphetsSpawned += iValue;
}

/// How much will the next prophet cost this player?
int CvPlayerReligions::GetCostNextProphet(bool bIncludeBeliefDiscounts, bool bAdjustForSpeedDifficulty, bool bExcludeFree) const
{
	int iCost = GC.getGame().GetGameReligions()->GetFaithGreatProphetNumber(GetNumProphetsSpawned(bExcludeFree) + 1);

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

			if (!m_pPlayer->isHuman(ISHUMAN_HANDICAP))
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
bool CvPlayerReligions::HasCreatedReligion(bool bIgnoreLocal) const
{
	return GC.getGame().GetGameReligions()->HasCreatedReligion(m_pPlayer->GetID(), bIgnoreLocal);
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
bool CvPlayerReligions::CanAffordFaithPurchase(int iMinimumFaithTimes100) const
{
	int iFaithTimes100 = m_pPlayer->GetFaithTimes100();
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
					int iCostTimes100 = pCapital->GetFaithPurchaseCost(eUnit, true) * 100;
					if (iCostTimes100 != 0 && iFaithTimes100 > iCostTimes100 * 100 && iCostTimes100 > iMinimumFaithTimes100)
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
					int iCostTimes100 = pCapital->GetFaithPurchaseCost(eBuilding) * 100;
					if (iCostTimes100 != 0 && iFaithTimes100 > iCostTimes100 && iCostTimes100 > iMinimumFaithTimes100)
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
	int iPlayerFaithTimes100 = m_pPlayer->GetFaithTimes100();

	if (iPlayerFaithTimes100 < m_iFaithAtLastNotifyTimes100) {
		// We've spent faith, so reduce the threshold we're checking at
		m_iFaithAtLastNotifyTimes100 = iPlayerFaithTimes100;
	}

	return CanAffordFaithPurchase(m_iFaithAtLastNotifyTimes100);
}

void CvPlayerReligions::SetFaithAtLastNotifyTimes100(int iFaithTimes100)
{
	m_iFaithAtLastNotifyTimes100 = iFaithTimes100;
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
	if (OwnsReligion())
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
ReligionTypes CvPlayerReligions::GetOwnedReligion(bool bIgnoreLocal) const
{
	if (!MOD_BALANCE_VP)
	{
		// in CP, players can only own a religion if they have founded it
		return GetReligionCreatedByPlayer();
	}
	else
	{
		// in VP, religions can also be obtained by conquering holy cities
		if (!m_bOwnsStateReligion || m_eStateReligion == RELIGION_PANTHEON)
			return NO_RELIGION;

		if (bIgnoreLocal && GC.getReligionInfo(m_eStateReligion)->IsLocalReligion())
			return NO_RELIGION;

		return m_eStateReligion;
	}
}

// Do we own a holy city?
bool CvPlayerReligions::OwnsReligion(bool bIgnoreLocal) const
{
	return GetOwnedReligion(bIgnoreLocal) != NO_RELIGION;
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
	if (m_pPlayer->GetNotifications() && bOwnsReligion)
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
		if (m_pPlayer->GetFaithTimes100() > 0 && !m_pPlayer->GetPlayerTraits()->IsAlwaysReligion())
		{
			m_pPlayer->ChangeGoldenAgeProgressMeterTimes100(m_pPlayer->GetFaithTimes100());
			m_pPlayer->changeInstantYieldValue(YIELD_GOLDEN_AGE_POINTS, m_pPlayer->GetFaithTimes100() / 100);
			m_pPlayer->SetFaithTimes100(0);
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
				if(pLoopCity->GetCityReligions()->GetReligiousMajority() == GetStateReligion())
				{
					iNum++;
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
				if(pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
				{
					iNum++;
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

/// Does this player benefit from a boost in Spy NP generation?
int CvPlayerReligions::GetEspionageNetworkPoints(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iRtnValue += pReligion->m_Beliefs.GetEspionageNetworkPoints(ePlayer, pHolyCity);
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

/// Do this player's spies erode the pressure of other religions?
int CvPlayerReligions::GetSpyPressureErosion(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			iRtnValue += pReligion->m_Beliefs.GetSpyPressureErosion(ePlayer, pHolyCity);
		}
	}
	return iRtnValue;
}

/// Does this religion get religious pressure from franchises?
int CvPlayerReligions::GetFranchisePressure() const
{
	int iRtnValue = 0;
	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		if (pReligion)
		{
			iRtnValue += m_pPlayer->GetFranchisePressure();
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

		if(religionIt->m_iFollowers > iMostFollowers || (religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostFollowerPressure))
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

		if(religionIt->m_iFollowers > iMostFollowers || (religionIt->m_iFollowers == iMostFollowers && religionIt->m_iPressure > iMostFollowerPressure))
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
	if (iIndex < m_ReligionStatus.size() && m_ReligionStatus[iIndex].m_iPressure > 0)
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
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isAlive())
			continue;

		// Loop through each of their cities
		int iLoop = 0;
		for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
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

		if (iI >= MAX_MAJOR_CIVS)
			continue;

		ReligionTypes eStateReligion = kPlayer.GetReligions()->GetStateReligion(false);
		if (eStateReligion == NO_RELIGION)
			continue;

		// Include any pressure from Franchises
		if (eStateReligion == eReligion)
		{
			int iFranchisePressure = kPlayer.GetFranchisePressure();
			if (iFranchisePressure > 0)
			{
				CorporationTypes eCorporation = kPlayer.GetCorporations()->GetFoundedCorporation();
				if (eCorporation != NO_CORPORATION && m_pCity->IsHasFranchise(eCorporation))
					iPressure += iFranchisePressure * max(1, GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
			}
		}

		CvPlayerEspionage* pEspionage = kPlayer.GetEspionage();
		if (pEspionage && pEspionage->GetSpyIndexInCity(m_pCity) != -1)
		{
			CvEspionageSpy* pSpy = pEspionage->GetSpyByID(pEspionage->GetSpyIndexInCity(m_pCity));
			if (pSpy->GetSpyState() != SPY_STATE_TRAVELLING)
			{
				// Do they have a spy that applies pressure to this religion?
				if (eStateReligion == eReligion)
				{
					int iSpyPressure = kPlayer.GetReligions()->GetSpyPressure((PlayerTypes)iI);
					if (iSpyPressure > 0)
						iPressure += iSpyPressure * max(1, GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
				}
				// Do they have a spy that erodes pressure from other religions?
				else
				{
					int iSpyPressureErosion = kPlayer.GetReligions()->GetSpyPressureErosion((PlayerTypes)iI);
					if (iSpyPressureErosion > 0)
						iPressure -= iSpyPressureErosion * max(1, GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
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
		iCount++;
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

	if (m_pCity->getPopulation() > 0)
	{
		RecomputeFollowers(FOLLOWER_CHANGE_POP_CHANGE);
	}
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
		case FOLLOWER_CHANGE_ADOPT_FULLY:
			return "ADOPT_FULLY";
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
			int iPantheonPressureChange = -iPressureChange;
			if (MOD_BALANCE_RESILIENT_PANTHEONS)
				iPantheonPressureChange /= 2;

			it->m_iPressure = max(0, (it->m_iPressure + iPantheonPressureChange));
			LogPressureChange(eReason, it->m_eReligion, iPantheonPressureChange, it->m_iPressure, eResponsiblePlayer);
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

/// Simulate inquisitor
void CvCityReligions::SimulateErodeOtherReligiousPressure(ReligionTypes eExemptedReligion, int iErosionPercent, bool bAllowRetention, bool bLeaveAtheists)
{
	m_SimulatedStatus = m_ReligionStatus;

	if (iErosionPercent < 1)
		return;

	ReligionInCityList::iterator it;
	for (it = m_SimulatedStatus.begin(); it != m_SimulatedStatus.end(); it++)
	{
		//ignore atheists if desired
		if (it->m_eReligion == NO_RELIGION && bLeaveAtheists)
			continue;

		//do not touch the exempted religion or dead ones
		if (eExemptedReligion == it->m_eReligion || it->m_iPressure == 0)
			continue;

		//default
		int iReductionPercent = min(100, iErosionPercent);

		//some beliefs are resistant
		if (it->m_eReligion > RELIGION_PANTHEON && bAllowRetention)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(it->m_eReligion, m_pCity->getOwner());
			if (pReligion)
			{
				int iRetentionPercent = pReligion->m_Beliefs.GetInquisitorPressureRetention(m_pCity->getOwner());  // Normally 0
				iReductionPercent = iReductionPercent * (100 - iRetentionPercent) / 100;
				iReductionPercent = max(0, iReductionPercent);
			}
		}

		//make it so!
		int iReductionAmount = iReductionPercent * it->m_iPressure / 100;
		it->m_iPressure -= iReductionAmount;
	}

	SimulateFollowers();
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
			int iPantheonPressureChange = -iPressure;
			if (MOD_BALANCE_RESILIENT_PANTHEONS)
				iPantheonPressureChange /= 2;

			it->m_iPressure = max(0, (it->m_iPressure + iPantheonPressureChange));
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

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddHolyCityPressure()
{
	ReligionTypes eHolyReligion = GetReligionForHolyCity();
	if (eHolyReligion != NO_RELIGION)
	{
		int iHolyPressure = GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity() * /*5*/ GD_INT_GET(RELIGION_PER_TURN_FOUNDING_CITY_PRESSURE);
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

/// Remove pressure from followers of other religions
void CvCityReligions::DoSpyPressureErosion(ReligionTypes eReligion, int iBasePressure, PlayerTypes eResponsiblePlayer)
{
	ReligionInCityList::iterator it;
	for (it = m_ReligionStatus.begin(); it != m_ReligionStatus.end(); it++)
	{
		//ignore atheists and pantheons
		if (it->m_eReligion == NO_RELIGION || it->m_eReligion <= RELIGION_PANTHEON)
			continue;
		
		//do not touch the exempted religion or dead ones
		if (eReligion == it->m_eReligion || it->m_iPressure==0)
			continue;

		// make it so!
		int iReductionAmount = max(iBasePressure * GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity() * -1, -it->m_iPressure);
		it->m_iPressure += iReductionAmount;
		LogPressureChange(FOLLOWER_CHANGE_SPY_PRESSURE, it->m_eReligion, iReductionAmount, it->m_iPressure, eResponsiblePlayer);
	}
	RecomputeFollowers(FOLLOWER_CHANGE_SPY_PRESSURE);
}

/// Add pressure to recruit followers to a religion
void CvCityReligions::AddFranchisePressure(ReligionTypes eReligion, int iBasePressure)
{
	AddReligiousPressure(FOLLOWER_CHANGE_FRANCHISE_PRESSURE, eReligion, iBasePressure*GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity());
	RecomputeFollowers(FOLLOWER_CHANGE_FRANCHISE_PRESSURE);
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
	STradePathInfo sPathInfo; //trade routes are not necessarily symmetric in case of of unrevealed tiles etc
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, pOtherCity, m_pCity, sPathInfo))
	{
		iApparentDistance = min(iApparentDistance, sPathInfo.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(false, m_pCity, pOtherCity, sPathInfo))
	{
		iApparentDistance = min(iApparentDistance, sPathInfo.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, pOtherCity, m_pCity, sPathInfo))
	{
		iApparentDistance = min(iApparentDistance, sPathInfo.iNormalizedDistanceRaw);
	}
	if (GC.getGame().GetGameTrade()->HavePotentialTradePath(true, m_pCity, pOtherCity, sPathInfo))
	{
		iApparentDistance = min(iApparentDistance, sPathInfo.iNormalizedDistanceRaw);
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

/// How many followers would we have after using an inquisitor here?
int CvCityReligions::GetNumFollowersAfterInquisitor(ReligionTypes eReligion)
{
	SimulateErodeOtherReligiousPressure(eReligion, /*100 in CP, 50 in VP*/ GD_INT_GET(INQUISITION_EFFECTIVENESS), true, true);
	return GetNumSimulatedFollowers(eReligion);
}
/// What would the majority religion be after using an inquisitor here?
ReligionTypes CvCityReligions::GetMajorityReligionAfterInquisitor(ReligionTypes eReligion)
{
	SimulateErodeOtherReligiousPressure(eReligion, /*100 in CP, 50 in VP*/ GD_INT_GET(INQUISITION_EFFECTIVENESS), true, true);
	return GetSimulatedReligiousMajority();
}

/// How many followers would we have having a prophet add religious pressure here?
int CvCityReligions::GetNumFollowersAfterProphetSpread(ReligionTypes eReligion, int iConversionStrength)
{
	SimulateProphetSpread(eReligion, iConversionStrength);
	return GetNumSimulatedFollowers(eReligion);
}

/// What would the majority religion be after adding this religious pressure here?
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
		ASSERT(false, "Invalid city population when recomputing followers");
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
		ASSERT(false, "Internal religion data error. Send save to Ed");
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

		PlayerTypes eReligionController = NO_PLAYER;
		CvCity* pHolyCity = pNewReligion->GetHolyCity();
		if(pHolyCity != NULL)
			eReligionController = pHolyCity->getOwner();

		//bonuses might change ...
		GET_PLAYER(pNewReligion->m_eFounder).UpdateReligion();

		// Process first-time conversion effects
		if(!m_pCity->HasPaidAdoptionBonus(eMajority))
		{
			m_pCity->SetPaidAdoptionBonus(eMajority, true);

			// accomplishment doesn't require you to also own the religion, unlike the instant bonuses below
			if (eResponsibleParty != NO_PLAYER && GET_PLAYER(eResponsibleParty).GetReligions()->GetStateReligion(false) == eMajority)
			{
				GET_PLAYER(eResponsibleParty).CompleteAccomplishment(ACCOMPLISHMENT_CITY_CONVERTED);
			}


			if (eReligionController != NO_PLAYER)
			{
				CvPlayer& kController = GET_PLAYER(eReligionController);

				if (kController.GetReligions()->GetStateReligion(false) == eMajority)
				{
					// does the religion controller gain yields for their religion spreading?

					kController.doInstantYield(INSTANT_YIELD_TYPE_CONVERSION, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pHolyCity);
					kController.doInstantYield(INSTANT_YIELD_TYPE_CONVERSION_EXPO, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pHolyCity);
					// vanilla column for gold on religion spreading
					int iGoldBonus = 0;
					if (eResponsibleParty != NO_PLAYER)
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
					if (iGoldBonus > 0)
					{
						kController.GetTreasury()->ChangeGold(iGoldBonus);

						if (eReligionController == GC.getGame().getActivePlayer())
						{
							char text[256] = {0};
							sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGoldBonus);
							SHOW_PLOT_POPUP(m_pCity->plot(), NO_PLAYER, text);
						}
					}

					// does the religion controller gain historic events from religion spread?

					int iTourism = kController.GetHistoricEventTourism(HISTORIC_EVENT_RELIGION_SPREAD);
					// Culture boost based on previous turns
					if(iTourism > 0)
					{
						kController.ChangeNumHistoricEvents(HISTORIC_EVENT_RELIGION_SPREAD, 1);
						kController.GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
						if(eReligionController == GC.getGame().getActivePlayer())
						{
							CvCity* pCity = kController.getCapitalCity();
							if(pCity != NULL)
							{
								char text[256] = {0};
								sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
								SHOW_PLOT_POPUP(pCity->plot(), eReligionController, text);

								CvNotifications* pNotification = kController.GetNotifications();
								if(pNotification)
								{
									CvString strMessage;
									CvString strSummary;
									strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_RELIGION_SPREAD", iTourism);
									strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
									pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, pCity->getX(), pCity->getY(), eReligionController);
								}
							}
						}
					}
				}
			}
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
			if (MOD_ENABLE_ACHIEVEMENTS && eOwnerPlayer == GC.getGame().getActivePlayer())
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

		if (MOD_ENABLE_ACHIEVEMENTS)
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
		if (eMajority != eOldMajority && eResponsibleParty != NO_PLAYER && GET_PLAYER(eResponsibleParty).getTeam() != m_pCity->getTeam() && kOwnerPlayer.isMajorCiv())
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

void CvUnitReligion::SetFullStrength(PlayerTypes eOwner, const CvUnitEntry& kUnitInfo, ReligionTypes eReligion)
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
	m_pBeliefs(NULL)
	, m_pPlayer(NULL)
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
	if (m_pPlayer->isHuman(ISHUMAN_AI_FAITH_SPENDING))
		return;

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
				int iFaith = pLoopCity->getYieldRateTimes100(YIELD_FAITH);

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
}

/// Select the belief most helpful to this pantheon
BeliefTypes CvReligionAI::ChoosePantheonBelief(PlayerTypes ePlayer)
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes> beliefChoices;

	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailablePantheonBeliefs(ePlayer);

	CvWeightedVector<int> viPlotWeights = CalculatePlotWeightsForBeliefSelection();
	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry, viPlotWeights);
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

/// Candidates for each belief slot when founding a religion: pantheon (if we don't have one yet), founder, follower, and bonus (if our traits give us one)
vector<vector<BeliefTypes>> CvReligionAI::GetFoundingBeliefCandidates(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	vector<vector<BeliefTypes>> vvCandidates;
	if (!kPlayer.GetReligions()->HasCreatedPantheon())
	{
		vvCandidates.push_back(pGameReligions->GetAvailablePantheonBeliefs(ePlayer));
	}
	vvCandidates.push_back(pGameReligions->GetAvailableFounderBeliefs(ePlayer, eReligion));
	vvCandidates.push_back(pGameReligions->GetAvailableFollowerBeliefs(ePlayer, eReligion));
	if (kPlayer.GetPlayerTraits()->IsBonusReligiousBelief())
	{
		vvCandidates.push_back(pGameReligions->GetAvailableBonusBeliefs(ePlayer, eReligion));
	}

	return vvCandidates;
}

/// Selects the beliefs for a new religion: pantheon belief (only if we don't have a pantheon yet), founder belief, follower belief and bonus belief (only if we have the trait for it)
/// Returns a vector with the four beliefs (NO_BELIEF for unavaliable slots)
vector<BeliefTypes> CvReligionAI::ChooseFoundingBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	return ChooseBeliefCombination(GetFoundingBeliefCandidates(ePlayer, eReligion), CvSeeder::fromRaw(0x9db23f3c).mix(GET_PLAYER(ePlayer).GetID()));
}

vector<vector<BeliefTypes>> CvReligionAI::GetEnhancingBeliefCandidates(PlayerTypes ePlayer, ReligionTypes eReligion) const
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();

	vector<vector<BeliefTypes>> vvCandidates;
	vvCandidates.push_back(pGameReligions->GetAvailableFollowerBeliefs(ePlayer, eReligion));
	vvCandidates.push_back(pGameReligions->GetAvailableEnhancerBeliefs(ePlayer, eReligion));
	return vvCandidates;
}

/// Selects the beliefs to enhance our religion
/// Returns a vector with follower and enhancer belief
vector<BeliefTypes> CvReligionAI::ChooseEnhancingBeliefs(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	return ChooseBeliefCombination(GetEnhancingBeliefCandidates(ePlayer, eReligion), CvSeeder::fromRaw(0x3a862bb8).mix(GET_PLAYER(ePlayer).GetID()));
}

/// Scores all viable combinations of founding beliefs
void CvReligionAI::ScoreFoundingBeliefCombinations(PlayerTypes ePlayer, ReligionTypes eReligion, CvWeightedVector<int>& combinationChoices, vector<vector<BeliefTypes>>& vvCombinations)
{
	vector<int> viSlots;
	ScoreBeliefCombinations(GetFoundingBeliefCandidates(ePlayer, eReligion), combinationChoices, vvCombinations, viSlots);
}

/// Scores all viable combinations of enhancing beliefs
void CvReligionAI::ScoreEnhancingBeliefCombinations(PlayerTypes ePlayer, ReligionTypes eReligion, CvWeightedVector<int>& combinationChoices, vector<vector<BeliefTypes>>& vvCombinations)
{
	vector<int> viSlots;
	ScoreBeliefCombinations(GetEnhancingBeliefCandidates(ePlayer, eReligion), combinationChoices, vvCombinations, viSlots);
}

/// Scores every viable combination of beliefs from vvCandidates
/// vvCandidates (input) is a vector of belief lists, each combination consists of one belief from each list.
/// The score of a combination is the sum of the scores of its beliefs, where each belief is scored taking the other beliefs of the combination into account.
/// output: vvCombinations - list of valid combinations. combinationChoices - score for each combination, the Elements of the vector are the indices of vvCombinations
void CvReligionAI::ScoreBeliefCombinations(const vector<vector<BeliefTypes>>& vvCandidates, CvWeightedVector<int>& combinationChoices, vector<vector<BeliefTypes>>& vvCombinations, vector<int>& viSlots) const
{
	// ScoreBelief can consider up to three additional beliefs, so we can't have more than four slots
	ASSERT(vvCandidates.size() <= 4, "Too many belief slots for ScoreBeliefCombinations");

	CvWeightedVector<int> viPlotWeights = CalculatePlotWeightsForBeliefSelection();

	// which slots have candidates?
	for (size_t iSlot = 0; iSlot < vvCandidates.size(); iSlot++)
	{
		if (!vvCandidates[iSlot].empty())
			viSlots.push_back((int)iSlot);
	}
	const int iNumSlots = (int)viSlots.size();
	ASSERT(iNumSlots > 0, "No beliefs to choose from");
	if (iNumSlots == 0)
		return;

	// how many candidates per slot can we afford to consider? with the values below a maximum of 125 combined scores are calculated
	// note that for iNumSlots == 1 this value is irrelevant as all individual scores are always calculated
	int iCandidatesPerSlot = 0;
	if (iNumSlots == 4)
		iCandidatesPerSlot = 3;
	else if (iNumSlots == 3)
		iCandidatesPerSlot = 5;
	else
		iCandidatesPerSlot = 10;

	// score each belief on its own and keep the best ones for each slot. the same belief may be a candidate in several slots (bonus belief), so cache the scores
	std::map<BeliefTypes, int> mapIndividualScores;
	vector<vector<BeliefTypes>> vvTopCandidates; // stores for each valid slot index a vector of the iCandidatesPerSlot top candidates
	for (int iSlot = 0; iSlot < iNumSlots; iSlot++)
	{
		const vector<BeliefTypes>& vCandidates = vvCandidates[viSlots[iSlot]];
		CvWeightedVector<BeliefTypes> beliefChoices;
		for (vector<BeliefTypes>::const_iterator it = vCandidates.begin(); it != vCandidates.end(); ++it)
		{
			CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(*it);
			if (!pEntry)
				continue;

			std::map<BeliefTypes, int>::iterator itScore = mapIndividualScores.find(*it);
			int iBeliefScore = 0;
			if (itScore == mapIndividualScores.end())
			{
				iBeliefScore = ScoreBelief(pEntry, viPlotWeights);
				mapIndividualScores.insert(std::make_pair(*it, iBeliefScore));
			}
			else
			{
				iBeliefScore = itScore->second;
			}

			beliefChoices.push_back(*it, iBeliefScore);
		}
		beliefChoices.StableSortItems();

		vector<BeliefTypes> vTopCandidates;
		for (int i = 0; i < beliefChoices.size() && i < iCandidatesPerSlot; i++)
			vTopCandidates.push_back(beliefChoices.GetElement(i));

		ASSERT(!vTopCandidates.empty(), "List of beliefs in slot to choose from shouldn't be empty");
		if (vTopCandidates.empty())
			return;

		vvTopCandidates.push_back(vTopCandidates);
	}

	// go through all combinations of the top candidates
	vector<vector<BeliefTypes>> vvSortedCombinations; // the combinations we've scored, with the belief IDs of each combination in ascending order

	int iNumScoredCombinations = 0;
	vector<int> viIndex(iNumSlots, 0); // the indices of the combination being scored
	bool bDone = false;
	while (!bDone)
	{
		vector<BeliefTypes> vCombination;
		for (int iSlot = 0; iSlot < iNumSlots; iSlot++)
			vCombination.push_back(vvTopCandidates[iSlot][viIndex[iSlot]]); // the belief IDs of the combination we're scoring

		vector<BeliefTypes> vSorted = vCombination;
		std::sort(vSorted.begin(), vSorted.end());

		// the same belief mustn't be in two slots ...
		if (std::adjacent_find(vSorted.begin(), vSorted.end()) == vSorted.end())
		{
			// and this combination of beliefs must not have been scored yet ...
			if (std::find(vvSortedCombinations.begin(), vvSortedCombinations.end(), vSorted) == vvSortedCombinations.end())
			{
				int iScore = 0;
				for (int iSlot = 0; iSlot < iNumSlots; iSlot++)
				{
					BeliefTypes eOtherBeliefs[3] = { NO_BELIEF, NO_BELIEF, NO_BELIEF };
					int iNumOtherBeliefs = 0;
					for (int iOtherSlot = 0; iOtherSlot < iNumSlots; iOtherSlot++)
					{
						if (iOtherSlot != iSlot)
						{
							eOtherBeliefs[iNumOtherBeliefs] = vCombination[iOtherSlot];
							iNumOtherBeliefs++;
						}
					}

					if (iNumOtherBeliefs == 0)
					{
						// we're scoring only one slot? then we can re-use the individual scores from above
						iScore += mapIndividualScores[vCombination[iSlot]];
					}
					else
					{
						// score belief iSlot with the other beliefs from the combination as additional beliefs
						// we're doing this for all slots so the total score of the combination will be the sum of the individual scores
						iScore += ScoreBelief(m_pBeliefs->GetEntry(vCombination[iSlot]), viPlotWeights, true, NO_RELIGION, eOtherBeliefs[0], eOtherBeliefs[1], eOtherBeliefs[2]);
					}
				}
				
				// we use the number of scored combinations as Element of the weighted vector, so we can later retrieve this combination via vvCombinations[Element]
				combinationChoices.push_back(iNumScoredCombinations, iScore);
				vvCombinations.push_back(vCombination);
				vvSortedCombinations.push_back(vSorted);
				iNumScoredCombinations++;
			}
		}

		// calculate the indices of the next combination
		int iSlot = 0;
		while (iSlot < iNumSlots)
		{
			// increase the index of this slot if we're below the maximum number of candidates here ...
			if (viIndex[iSlot] < (int)vvTopCandidates[iSlot].size() - 1)
			{
				viIndex[iSlot]++;
				// ... and don't change the values in the other slots
				break;
			}
			else
			{
				// if this slot is already at max index, reset it to zero and continue with the next slot
				viIndex[iSlot] = 0;
				iSlot++;
			}
		}
		// if all slots have been reset to zero, there are no more combinations left to score
		bDone = (iSlot == iNumSlots);
	}

	// sort by highest score
	combinationChoices.StableSortItems();
}

/// Select one combination from a vector of belief lists, so that the combination of beliefs is as good as possible
/// Returns one belief per slot, NO_BELIEF for slots without candidates
vector<BeliefTypes> CvReligionAI::ChooseBeliefCombination(const vector<vector<BeliefTypes>>& vvCandidates, CvSeeder seed)
{
	vector<BeliefTypes> vResult(vvCandidates.size(), NO_BELIEF);

	CvWeightedVector<int> combinationChoices;
	vector<vector<BeliefTypes>> vvCombinations;
	vector<int> viSlots;
	ScoreBeliefCombinations(vvCandidates, combinationChoices, vvCombinations, viSlots);

	if (combinationChoices.size() == 0)
		return vResult;

	int iChoice = combinationChoices.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getBeliefChoiceCutoffThreshold(), seed.mix(combinationChoices.size()));
	LogBeliefCombinationChoices(combinationChoices, vvCombinations, iChoice);

	for (size_t iSlot = 0; iSlot < viSlots.size(); iSlot++)
		vResult[viSlots[iSlot]] = vvCombinations[iChoice][iSlot];

	return vResult;
}

BeliefTypes CvReligionAI::ChooseReformationBelief(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	CvGameReligions* pGameReligions = GC.getGame().GetGameReligions();
	CvWeightedVector<BeliefTypes> beliefChoices;

	std::vector<BeliefTypes> availableBeliefs = pGameReligions->GetAvailableReformationBeliefs(ePlayer, eReligion);

	CvWeightedVector<int> viPlotWeights = CalculatePlotWeightsForBeliefSelection();

	for(std::vector<BeliefTypes>::iterator it = availableBeliefs.begin();
	        it!= availableBeliefs.end(); ++it)
	{
		const BeliefTypes eBelief = (*it);
		CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(eBelief);
		if(pEntry)
		{
			const int iScore = ScoreBelief(pEntry, viPlotWeights);
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

// current number of cities following a religion
int CvReligionAI::GetNumCitiesWithReligion(ReligionTypes eReligion, bool bFoundingReligion, bool bFoundingPantheon, bool bOnlyOurCities) const
{
	if (bFoundingPantheon)
	{
		// If we're founding a pantheon, all of our cities will immediately be converted to it
		return m_pPlayer->getNumCities();
	}

	if (bFoundingReligion)
	{
		// if we're founding a religion, only our holy city will follow it initially
		return 1;
	}

	if (eReligion == NO_RELIGION)
		return 0;

	// if we're evaluating an existing religion, calculate how many cities are following it
	int iNumTotalCities = 0;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (!bOnlyOurCities || iPlayerLoop == m_pPlayer->GetID())
		{
			if (kLoopPlayer.isAlive())
			{
				int iLoop = 0;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
					{
						iNumTotalCities++;
					}
				}
			}
		}
	}

	return iNumTotalCities;
}

// estimated number of cities to spread religion to
int CvReligionAI::GetNumCitiesToSpreadReligionTo(ReligionTypes eReligion, int& iNumNearbyFutureFollowers, bool bFoundingReligion, bool bFoundingPantheon) const
{
	if (bFoundingPantheon)
	{
		// Can't spread pantheons
		iNumNearbyFutureFollowers = 0;
		return 0;
	}

	iNumNearbyFutureFollowers = 0;

	int iForeignCityPercentMultiplier = 100;
	// if there are still religions to be founded, lower the value for foreign cities, the other players might found (but probably not if they don't have a pantheon yet)
	int iNumReligionsStillToFound = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound();
	// exclude our own religion if we're founding
	if (bFoundingReligion)
		iNumReligionsStillToFound--;
	if (iNumReligionsStillToFound > 0)
	{
		int iOtherPlayersWithPantheon = 0;
		int iMajorLoop;
		for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			// players without a religion, but with a pantheon
			if (GET_PLAYER((PlayerTypes)iMajorLoop).isAlive() && iMajorLoop != m_pPlayer->GetID() && GET_PLAYER((PlayerTypes)iMajorLoop).GetReligions()->GetStateReligion(false) == NO_RELIGION && GET_PLAYER((PlayerTypes)iMajorLoop).GetReligions()->GetStateReligion(true) != NO_RELIGION)
			{
				iOtherPlayersWithPantheon++;
			}
		}

		// assume all players with a pantheon who haven't founded yet have an equal chance of founding
		if (iOtherPlayersWithPantheon > 0)
			iForeignCityPercentMultiplier = max(100 - 100 * iNumReligionsStillToFound / iOtherPlayersWithPantheon, 0);
	}

	int iNumTotalCities = 0; /* calculated in Times100 */
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (kLoopPlayer.isAlive() && GET_TEAM(m_pPlayer->getTeam()).isHasMet(kLoopPlayer.getTeam()))
		{
			// exclude players who own a religion or have asked us not to spread
			if (kLoopPlayer.isMajorCiv() && m_pPlayer->GetDiplomacyAI()->IsBadTheftTarget(kLoopPlayer.GetID(), THEFT_TYPE_CONVERSION))
				continue;


			int iNumCities = 0;
			int iSumCityPopulation = 0;
			int iLoop = 0;

			CvCity* pLoopCity = NULL;
			for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (bFoundingReligion || pLoopCity->GetCityReligions()->GetReligiousMajority() != eReligion)
				{
					// our own capital will immediately be converted when we're founding a religion
					if (bFoundingReligion && kLoopPlayer.GetID() == m_pPlayer->GetID() && pLoopCity->isCapital())
						continue;

					iNumCities++;
					iSumCityPopulation += pLoopCity->getPopulation();
				}
			}

			int iMod = 100;
			// Always convert our own cities
			if (kLoopPlayer.GetID() == m_pPlayer->GetID())
			{
				iNumTotalCities += iNumCities * 100;
				iNumNearbyFutureFollowers += iSumCityPopulation * 100;
			}
			else
			{
				// only reduce score for players with a pantheon, players without one probably won't found
				if (kLoopPlayer.isMajorCiv() && kLoopPlayer.GetReligions()->GetStateReligion(true) != NO_RELIGION)
				{
					iMod *= iForeignCityPercentMultiplier;
					iMod /= 100;
				}

				if (kLoopPlayer.isMajorCiv() && (kLoopPlayer.GetReligions()->OwnsReligion(true) || kLoopPlayer.GetPlayerTraits()->IsAlwaysReligion()))
				{
					iMod /= 4;
				}

				if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_FAR)
				{
					iMod /= 2;
				}
				else if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_DISTANT)
				{
					iMod /= 5;
				}

				iNumTotalCities += iNumCities * iMod;
				iNumNearbyFutureFollowers += iSumCityPopulation * iMod * 2 / 3;
			}
		}
	}

	iNumNearbyFutureFollowers /= 100;
	iNumTotalCities /= 100;

	return iNumTotalCities;
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

	//if we already used the prophet for something (india!) then spreading is the only option; accept basically any target
	if (pUnit && !pUnit->GetReligionData()->IsFullStrength())
		iMinScore = 10;

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
	bool bAnyOwnCityNotConverted = false;

	// Look at our cities first, checking them for followers of other religions
	int iLoop = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		ReligionTypes eMajorityReligion = pLoopCity->GetCityReligions()->GetReligiousMajority();
		int iHeretics = pLoopCity->GetCityReligions()->GetFollowersOtherReligions(eReligion,true) + pLoopCity->GetCityReligions()->GetNumFollowers(NO_RELIGION);
		int iDistanceToHolyCity = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pHolyCity->getX(), pHolyCity->getY());

		if (eMajorityReligion == eReligion || !CvReligionAIHelpers::PassesTeammateReligionCheck(eMajorityReligion, m_pPlayer->GetID(), true))
			continue;
		else
		{
			bAnyOwnCityNotConverted = true;

			// If this is the holy city and it has been converted, want to go there no matter what
			if (pLoopCity == pHolyCity)
			{
				vCandidates.push_back(SPlotWithScore(pLoopCity->plot(), 100000));
				continue;
			}
		}

		if (pLoopCity->isUnderSiege())
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

	// India will only convert other players if its own cities are all converted
	if (!bAnyOwnCityNotConverted || !m_pPlayer->GetPlayerTraits()->IsProphetFervor())
	{
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

			if (m_pPlayer->GetDiplomacyAI()->IsBadTheftTarget(kLoopPlayer.GetID(), THEFT_TYPE_CONVERSION))
				continue;

			int iCityLoop = 0;
			for (pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iPlayerLoop).nextCity(&iCityLoop))
			{
				//We don't want to spread our faith to unowned cities if it doesn't spread naturally and we have a unique belief (as its probably super good).
				// Unless only we can benefit from it
				if (!MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV && m_pPlayer->GetPlayerTraits()->IsUniqueBeliefsOnly() &&
					m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pLoopCity->getOwner() != m_pPlayer->GetID())
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
					int iHeretics = pCR->GetFollowersOtherReligions(eReligion, true);
					if (iHeretics == 0)
						continue;

					ReligionTypes eMajorityReligion = pCR->GetReligiousMajority();
					if (eMajorityReligion == eReligion)
						continue;

					if (!CvReligionAIHelpers::PassesTeammateReligionCheck(eMajorityReligion, m_pPlayer->GetID(), false))
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
					for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
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

					if (iScore > iMinScore)
						vCandidates.push_back( SPlotWithScore(pLoopCity->plot(),iScore));
				}
			}
		}
	}

	//sort descending
	std::stable_sort(vCandidates.begin(),vCandidates.end());
	std::reverse(vCandidates.begin(),vCandidates.end());

	//look at the top two and take the one that is closest
	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;
	if (pUnit && vCandidates.size()>1)
	{
		int iTurnsToTargetA = INT_MAX;
		int iTurnsToTargetB = INT_MAX;
		int iScoreA = 0;
		int iScoreB = 0;

		if (pUnit->GeneratePath(vCandidates[0].pPlot, iFlags, INT_MAX, &iTurnsToTargetA) && pUnit->CachedPathIsSafeForCivilian())
			iScoreA = vCandidates[0].score / (iTurnsToTargetA + 3); //add some bias for close targets
		if (pUnit->GeneratePath(vCandidates[1].pPlot, iFlags, INT_MAX, &iTurnsToTargetB) && pUnit->CachedPathIsSafeForCivilian())
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
	{
		if (pUnit->GeneratePath(vCandidates[0].pPlot, iFlags, INT_MAX, piTurns) && pUnit->CachedPathIsSafeForCivilian())
			return vCandidates.front().pPlot->getPlotCity();
	}

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

		CvWeightedVector<int> viPlotWeights = CalculatePlotWeightsForBeliefSelection();

		int iScore = 0;
		for (int i = 0; i < itR->m_Beliefs.GetNumBeliefs(); i++)
		{
			BeliefTypes b = itR->m_Beliefs.GetBelief(i);
			CvBeliefEntry* pEntry = m_pBeliefs->GetEntry(b);

			//ignore founder beliefs!
			if (pEntry && !pEntry->IsFounderBelief() && !pEntry->IsEnhancerBelief())
				iScore += ScoreBelief(pEntry,viPlotWeights,false,itR->m_eReligion);
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
		if (eBuilding != NO_BUILDING && pCity->GetCityBuildings()->GetNumBuilding(eBuilding) < 1 && pCity->IsCanPurchase(true, true, NO_UNIT, eBuilding, NO_PROJECT, YIELD_FAITH))
		{
			if(BuyFaithBuilding(pCity, eBuilding))
			{
				if(GC.getLogging())
				{
					strLogMsg += ", Bought a Faith Building";
					CvString strFaith;
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
								strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
								strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
					int iTempWeight = 100;
					iTempWeight = pCity->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, iTempWeight, true);
					if (iTempWeight > 0)
					{
						CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
						if (pUnitEntry)
						{
							if (eUnit == pCity->GetUnitForOperation() || eUnit == m_pPlayer->GetMilitaryAI()->GetUnitTypeForArmy(pCity))
							{
								pCity->PurchaseUnit(eUnit, YIELD_FAITH);
								if (GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Bought a Non-Faith military Unit, %s, Faith: %.2f", pUnitEntry->GetDescriptionKey(), (float)m_pPlayer->GetFaithTimes100() / 100);
									strLogMsg += strFaith;
									GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
								}
								return true;
							}
							else if (m_pPlayer->GetNumUnitsWithUnitAI(pUnitEntry->GetDefaultUnitAIType(), false) <= iPurchaseAmount)
							{
								pCity->PurchaseUnit(eUnit, YIELD_FAITH);
								if (GC.getLogging())
								{
									CvString strFaith;
									strFaith.Format(", Bought a Non-Faith military Unit, %s, (up to %d), Faith: %.2f", pUnitEntry->GetDescriptionKey(), iPurchaseAmount, (float)m_pPlayer->GetFaithTimes100() / 100);
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
		int iCostTimes100 = pLoopCity->GetFaithPurchaseCost(eInquisitor, true /*bIncludeBeliefDiscounts*/) * 100;
		if (iCostTimes100 > m_pPlayer->GetFaithTimes100())
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
						pLoopCity->PurchaseUnit(eInquisitor, YIELD_FAITH);
						bResult = true;
						break;
					}
				}
			}
		}
	}

	return bResult;
}

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

	if (MOD_BALANCE_QUEST_CHANGES)
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
		if (pLoopUnit->GetReligionData()->GetSpreadsLeft(pLoopUnit) > 0)
			if (pLoopUnit->GetReligionData()->GetReligion() == eReligionWeFounded || pLoopUnit->GetReligionData()->GetReligion() == eReligionToSpread)
				iNumMissionaries++;
	}

	//Let's see about our religious flavor...
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	int iFlavorReligion = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));

	// Do we get benefits from spreading our religion?
	int iDesireToSpread = (eReligionWeFounded != NO_RELIGION) ? GetSpreadScore() : 0;

	//exceptions from the rule
	static UnitClassTypes eUnitClassMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");
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
				strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
				strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}

			return true; //do not allow any other faith purchases
		}
	}

	// SECOND PRIORITY
	// If in Industrial, see if we want to save for buying a great person
	if (m_pPlayer->GetCurrentEra() >= GC.getGame().GetGameReligions()->GetFaithPurchaseGreatPeopleEra() && GetDesiredFaithGreatPerson() != NO_UNIT)
	{
		UnitTypes eGPType = GetDesiredFaithGreatPerson();

		//check if it's worth the wait
		//eventually the waiting time will be short enough after we run out of other stuff to buy
		CvCity* pCapital = m_pPlayer->getCapitalCity();
		if (pCapital)
		{
			int iFaithPerTurnTimes100 = m_pPlayer->GetTotalFaithPerTurnTimes100();
			int iFaithStoredTimes100 = m_pPlayer->GetFaithTimes100();
			int iFaithNeededTimes100 = pCapital->GetFaithPurchaseCost(eGPType, true) * 100;
			int iTurnsRemaining = (iFaithNeededTimes100 - iFaithStoredTimes100) / max(1, iFaithPerTurnTimes100);
			if (iTurnsRemaining < max(13,23-iFlavorReligion))
			{
				if (BuyGreatPerson(eGPType, (eGPType == eProphetType) ? eReligionWeFounded : NO_RELIGION))
				{
					if (GC.getLogging())
					{
						CvString strLogMsg = strPlayer + ", Bought a Great Person b/c Industrial age, ";
						strLogMsg += GC.getUnitInfo(eGPType)->GetDescription();
						CvString strFaith;
						strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
						strFaith.Format(", Faith: %d, Turns left: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100, iTurnsRemaining);
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
				strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
				strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
				strLogMsg += strFaith;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}

			return true; //do not allow any other faith purchases
		}
	}

	// FOURTH PRIO: FOREIGN CITIES
	if (pMyReligion && !bTooManyMissionaries && bAllConvertedCore && eReligionToSpread == eReligionWeFounded)
	{
		if (!MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV && m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread())
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
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
					strFaith.Format(", Faith: %.2f", (float)m_pPlayer->GetFaithTimes100() / 100);
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
		int iCostTimes100 = pCapital->GetFaithPurchaseCost(eMissionary, true /*bIncludeBeliefDiscounts*/) * 100;
		if (iCostTimes100 <= m_pPlayer->GetFaithTimes100())
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

				pBestCity->PurchaseUnit(eMissionary, YIELD_FAITH);
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
		int iCostTimes100 = pCapital->GetFaithPurchaseCost(eInquisitor, true /*bIncludeBeliefDiscounts*/) * 100;
		if (iCostTimes100 <= m_pPlayer->GetFaithTimes100())
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

				pBestCity->PurchaseUnit(eInquisitor, YIELD_FAITH);
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
			int iCostTimes100 = pBestCity->GetFaithPurchaseCost(eUnit, true /*bIncludeBeliefDiscounts*/) * 100;
			if (iCostTimes100 <= m_pPlayer->GetFaithTimes100())
			{
				pBestCity->PurchaseUnit(eUnit, YIELD_FAITH);
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
	
	int iBuildingCostTimes100 = pCity->GetFaithPurchaseCost(eBuilding) * 100;
	if (iBuildingCostTimes100 <= kPlayer.GetFaithTimes100())
	{
		pCity->PurchaseBuilding(eBuilding, YIELD_FAITH);
		return true;
	}

	return false;
}
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
							pLoopCity->PurchaseUnit(eUnit, YIELD_FAITH);
							bPurchased = true;
						}	
					}
				}
			}
		}
	}
	return bPurchased;
}
/// Any building that we can build with Faith (not Faith-generating ones)
bool CvReligionAI::BuyAnyAvailableNonFaithBuilding()
{
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iLoop = 0;
	for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iI);
			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);

				// Check to make sure this isn't a Faith-generating building
				if (pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
				{
					if(pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						pLoopCity->PurchaseBuilding(eBuilding, YIELD_FAITH);
						return true;
					}
				}
				//If there are still buildings to buy, buy them.
				else if (pBuildingEntry)
				{
					if (pLoopCity->IsCanPurchase(true, true, (UnitTypes)-1, eBuilding, (ProjectTypes)-1, YIELD_FAITH))
					{
						pLoopCity->PurchaseBuilding(eBuilding, YIELD_FAITH);
						return true;
					}
				}
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
						pLoopCity->PurchaseBuilding(eBuilding, YIELD_FAITH);
						return true;
					}
				}
			}
		}
	}
	return false;
}

// returns a weighted list of plots within or near our territory for belief selection. Plots weights are 10 for plots that are currently being worked by our cities, unworked or unowned plots have lower weights
CvWeightedVector<int> CvReligionAI::CalculatePlotWeightsForBeliefSelection() const
{
	CvWeightedVector<int> viPlotList; 

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if (!pCapital)
		return viPlotList;

	int x_max = 0; int x_min = GC.getMap().getGridWidth();
	int y_max = 0; int y_min = GC.getMap().getGridHeight();

	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			x_min = min(x_min, pPlot->getX());
			x_max = max(x_max, pPlot->getX());
			y_min = min(y_min, pPlot->getY());
			y_max = max(y_max, pPlot->getY());
		}
	}

	// Open the log file
	//FILogFile* pLog = NULL;
	//pLog = LOGFILEMGR.GetLog("TotalBeliefScoringReligionLog.csv", FILogFile::kDontTimeStamp);
	//CvString strTemp;

	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();
	// how far do we want to expand
	bool bConsiderExpansion = !pPlayerTraits->IsNoAnnexing();
	int iExplorationRange = 0;
	if (bConsiderExpansion)
	{
		iExplorationRange = 9; // default
		// which early game policy have we adopted?
		static PolicyBranchTypes eTradition = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_TRADITION", true);
		static PolicyBranchTypes eProgress = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_LIBERTY", true);
		static PolicyBranchTypes eAuthority = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_HONOR", true);
		CvPlayerPolicies* pPlayerPolicies = m_pPlayer->GetPlayerPolicies();
		if (pPlayerPolicies->IsPolicyBranchUnlocked(eTradition))
			iExplorationRange -= 3;
		else if (pPlayerPolicies->IsPolicyBranchUnlocked(eAuthority))
			iExplorationRange += 1;
		else if (pPlayerPolicies->IsPolicyBranchUnlocked(eProgress))
			iExplorationRange += 3;
		else
		{
			// no early-game policy unlocked, or unlocked fealty (CP only): determine exploration range based on player traits
			if (pPlayerTraits->IsExpansionist())
			{
				iExplorationRange += 2;
			}
			if (pPlayerTraits->IsSmaller())
			{
				iExplorationRange -= 2;
			}
		}
		//strTemp.Format("%s, Exploration Range: %d", m_pPlayer->getName(), iExplorationRange);
		//pLog->Msg(strTemp);
	}

	// which landmasses are we on
	vector<int> vLandmasses;
	int iOurCityLoop = 0;
	for (const CvCity* pLoopCity = m_pPlayer->firstCity(&iOurCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iOurCityLoop))
	{
		int iLandmass = pLoopCity->plot()->getLandmass();
		if (std::find(vLandmasses.begin(), vLandmasses.end(), iLandmass) == vLandmasses.end())
			vLandmasses.push_back(iLandmass);
	}

	// find all cities of players that we know that are close to us
	vector<CvCity*>vKnownCitiesWithinReach;
	if (bConsiderExpansion)
	{
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			if ((PlayerTypes)iLoopPlayer == m_pPlayer->GetID())
				continue;

			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);

			// ignore Venice
			if (kLoopPlayer.GetPlayerTraits()->IsNoAnnexing())
				continue;

			if (GET_TEAM(m_pPlayer->getTeam()).isHasMet(kLoopPlayer.getTeam()) && GET_TEAM(m_pPlayer->getTeam()).IsHasFoundPlayersTerritory((PlayerTypes)iLoopPlayer))
			{
				int iLoop = 0;
				for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
				{
					if (plotDistance(pCapital->getX(), pCapital->getY(), pLoopCity->getX(), pLoopCity->getY()) < 2 * iExplorationRange)
					{
						vKnownCitiesWithinReach.push_back(pLoopCity);
					}
				}
			}
		}
	}

	for (int y = y_max; y >= y_min; y--)
	{
		//CvString logStr = (y % 2 == 0) ? "" : "_";
		for (int x = x_min; x <= x_max; x++)
		{
			CvPlot* pPlot = GC.getMap().plot(x, y);
			if (pPlot->isRevealed(m_pPlayer->getTeam()))
			{
				int iPlotWeight = 0;
				PlayerTypes ePlotOwner = pPlot->getOwner();
				if (pPlot->isCity())
				{
					iPlotWeight = (ePlotOwner == m_pPlayer->GetID()) ? 10 : 0;
					//logStr += "c ";
				}
				else
				{
					// special calculation for mountains: they count towards bonuses even if not owned and can give bonuses to multiple cities at once
					bool bMountainInCityRange = false;
					if (pPlot->getTerrainType() == TERRAIN_MOUNTAIN)
					{
						int iCityLoop = 0;
						CvCity* pLoopCity;
						for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
						{
							if (pLoopCity->IsWithinWorkRange(pPlot))
							{
								bMountainInCityRange = true;
								iPlotWeight += 10;
							}
						}
					}

					if (!bMountainInCityRange)
					{
						if (ePlotOwner == m_pPlayer->GetID())
						{
							if (pPlot->isBeingWorked())
							{
								iPlotWeight = 10;
							}
							else
							{
								CvCity* pOwningCity = pPlot->getEffectiveOwningCity();
								if (pOwningCity && pOwningCity->IsWithinWorkRange(pPlot))
								{
									iPlotWeight = 9;
								}
							}
						}
						// We also evaluate unowned plots nearby unless we can't build settlers
						else if (bConsiderExpansion && ePlotOwner == NO_PLAYER)
						{
							// Only consider plots within iExplorationRange from the capital, or plots close to our other cities
							int iDistanceToCapital = plotDistance(pCapital->getX(), pCapital->getY(), pPlot->getX(), pPlot->getY());
							if (iDistanceToCapital <= iExplorationRange || m_pPlayer->GetClosestCity(pPlot, 3, false))
							{
								CvCity* pClosestCity = m_pPlayer->GetClosestCity(pPlot, iExplorationRange, false);
								if (pClosestCity->IsWithinWorkRange(pPlot))
								{
									if (pPlot->isAdjacentOwned())
									{
										iPlotWeight = 7;
									}
									else
									{
										iPlotWeight = 4;
									}
								}
								else
								{
									// plot is outside the working range of our cities
									// if another known civ has a city that's closer to the plot than our nearest city, skip the plot. otherwise, give it a low value
									int iOurDistance = plotDistance(pClosestCity->getX(), pClosestCity->getY(), pPlot->getX(), pPlot->getY());
									bool bCloserCityFound = false;
									if (!vKnownCitiesWithinReach.empty())
									{
										for (unsigned int iI = 0; iI < vKnownCitiesWithinReach.size(); iI++)
										{
											CvCity* pLoopCity = vKnownCitiesWithinReach[iI];
											if (plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY()) < iOurDistance)
											{
												bCloserCityFound = true;
												break;
											}
										}
									}
									if (!bCloserCityFound)
									{
										if ((iDistanceToCapital - pCapital->getWorkPlotDistance()) <= (iExplorationRange - pCapital->getWorkPlotDistance()) / 2)
										{
											// on a different landmass than our cities?
											if (!pPlot->isWater() && std::find(vLandmasses.begin(), vLandmasses.end(), pPlot->getLandmass()) == vLandmasses.end())
											{
												iPlotWeight = 1;
											}
											else
											{
												iPlotWeight = 3;
											}
										}
										else
										{
											iPlotWeight = 1;
										}
										if (pPlot->getTerrainType() == TERRAIN_MOUNTAIN)
										{
											// bonus for mountains
											iPlotWeight *= 4;
										}
									}
								}
							}
						}
						/*
						if (iPlotWeight == 10)
						{
							strTemp = "X ";
						}
						else
						{
							strTemp.Format("%d ", iPlotWeight);
						}
						logStr += strTemp;*/
					}
				}
				if (iPlotWeight > 0)
				{
					viPlotList.push_back(pPlot->GetPlotIndex(), iPlotWeight);
				}
			}
			/*else
			{
				logStr += "  ";
			}*/
		}
		//pLog->Msg(logStr);
	}

	return viPlotList;
}

/// AI's perceived worth of a belief
int CvReligionAI::ScoreBelief(CvBeliefEntry* pEntry, CvWeightedVector<int> viPlotWeights, bool bConsiderFutureTech, ReligionTypes eForeignReligion, BeliefTypes eSelectedAdditionalBelief1, BeliefTypes eSelectedAdditionalBelief2, BeliefTypes eSelectedAdditionalBelief3) const
{

	// special handing for civs that start with a pantheon: randomly choose between beliefs with flag AI_GoodStartingPantheon set in database
	if (eForeignReligion == NO_RELIGION && pEntry->IsPantheonBelief() && m_pPlayer->GetPlayerTraits()->StartsWithPantheon())
	{
		return pEntry->IsAIGoodStartingPantheon() ? 1000 : 1;
	}

	/// ///////////////////////////
	// PART 1: General evaluations
	// Which victory condition are we going for? Are we threatened by neighboring enemies? How much do we focus on wonders? etc.
	/// ///////////////////////////

	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();
	bool bIsExpansion = pDiploAI->IsGoingForDiploVictory();
	PolicyBranchTypes eAuthority = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_HONOR", true);

	static EconomicAIStrategyTypes eEnoughExpansion = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
	if (m_pPlayer->GetEconomicAI()->IsUsingStrategy(eEnoughExpansion))
	{
		bIsExpansion = false;
	}

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();


	int iNumNeighbors = 0;
	int iNeighborWarmongerThreat = 0;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID() && GET_TEAM(m_pPlayer->getTeam()).isHasMet(kLoopPlayer.getTeam()))
		{
			if (kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE || m_pPlayer->IsAtWarWith((PlayerTypes)iPlayerLoop))
			{
				iNumNeighbors++;

				int iProximityScore = kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_NEIGHBORS ? 2 : 1;
				int iDangerScore = 0;
				CvPlayerTraits* pLoopPlayerTraits = kLoopPlayer.GetPlayerTraits();
				if (m_pPlayer->IsAtWarWith((PlayerTypes)iPlayerLoop))
				{
					int iWarScore = m_pPlayer->GetWarScore((PlayerTypes)iPlayerLoop);
					// high danger score if we're losing the war
					iDangerScore = max(1, 3 - (iWarScore / 2));
					if (iWarScore < -10)
						iDangerScore -= (iWarScore + 10) / 2;
				}
				else if (kLoopPlayer.GetDiplomacyAI()->GetSurfaceApproach(m_pPlayer->GetID()) != CIV_APPROACH_FRIENDLY && (pLoopPlayerTraits->IsWarmonger() || kLoopPlayer.GetPlayerPolicies()->IsPolicyBranchUnlocked(eAuthority)))
				{
					iDangerScore = iProximityScore;
					if (kLoopPlayer.GetDiplomacyAI()->GetSurfaceApproach(m_pPlayer->GetID()) == CIV_APPROACH_HOSTILE)
						iDangerScore *= 2;

					switch (pDiploAI->GetMilitaryStrengthComparedToUs((PlayerTypes)iPlayerLoop))
					{
					case STRENGTH_IMMENSE:
						iDangerScore *= 300;
						break;
					case STRENGTH_POWERFUL:
						iDangerScore *= 200;
						break;
					case STRENGTH_STRONG:
						iDangerScore *= 100;
						break;
					case STRENGTH_AVERAGE:
						iDangerScore *= 50;
						break;
					default:
						// don't need to worry about them
						iDangerScore *= 0;
						break;
					}
					iDangerScore /= 100;
				}

				iNeighborWarmongerThreat += iDangerScore;
			}

		}
	}

	// iOffensePriority: value between 0 and 10
	int iOffensePriority = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) / 2;
	if (m_pPlayer->IsAtWar())
		iOffensePriority += 2;
	if (pDiploAI->IsGoingForWorldConquest())
		iOffensePriority += 5;

	if (iNumNeighbors == 0)
	{
		iOffensePriority = 0;
	}

	// iDefensePriority: value typically between 0 and 10, can go up to 25 in some cases
	int iDefensePriority = (pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE")) + pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"))) / 4;
	iDefensePriority += iNeighborWarmongerThreat;

	if (pDiploAI->IsGoingForWorldConquest())
		iDefensePriority /= 4;

	iDefensePriority = min(iDefensePriority, 25);

	int iWonderPriority = pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER")) / 2 + pDiploAI->GetWonderCompetitiveness() / 2;
	iWonderPriority *= (100 + pPlayerTraits->GetWonderProductionModifier() + pPlayerTraits->GetWonderProductionModGA() / 3);
	iWonderPriority /= 100;
	if (m_pPlayer->GetCurrentEra() <= 1 && m_pPlayer->getCapitalCity())
	{
		//in the early game, modify wonder priority based on how much production our city has
		int iOurProductionInCapital = m_pPlayer->getCapitalCity()->getYieldRateTimes100(YIELD_PRODUCTION, true);
		int iHighestProductionOtherPlayers = 100;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
			if (kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID() && kLoopPlayer.getCapitalCity())
			{
				iHighestProductionOtherPlayers = max(iHighestProductionOtherPlayers, kLoopPlayer.getCapitalCity()->getYieldRateTimes100(YIELD_PRODUCTION, true));
			}
		}
		iWonderPriority *= max(50, min(150, 100 * iOurProductionInCapital / iHighestProductionOtherPlayers));
		iWonderPriority /= 100;
	}
	else if (GC.getGame().GetMedianTechsResearched() > 0)
	{
		// in later stages of the game, modify wonder priority based on whether we're ahead or behind in techs
		iWonderPriority *= max(50, min(150, 100 * GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() / GC.getGame().GetMedianTechsResearched()));
		iWonderPriority /= 100;
	}

	UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");

	//Trait-specific things to consider.
	bool bNoMissionary = m_pPlayer->GetPlayerTraits()->NoTrain(eMissionary);
	bool bNoNaturalSpread = m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread();

	ReligionTypes eReligion = eForeignReligion != NO_RELIGION ? eForeignReligion : m_pPlayer->GetReligions()->GetStateReligion(false); // if a value is passed in for eForeignReligion, we evaluate the belief for that religion, otherwise we evaluate it for our own religion

	bool bFoundingReligion = (m_pPlayer->GetReligions()->GetFoundingReligionCityID() != -1);
	bool bFoundingPantheon = !bFoundingReligion && m_pPlayer->GetReligions()->GetStateReligion(true) == NO_RELIGION && eForeignReligion == NO_RELIGION;

	CvCity* pHolyCity = m_pPlayer->GetHolyCity();
	if (!pHolyCity && m_pPlayer->GetReligions()->GetFoundingReligionCityID() != -1)
		pHolyCity = m_pPlayer->getCity(m_pPlayer->GetReligions()->GetFoundingReligionCityID());
	if (!pHolyCity && eForeignReligion == NO_RELIGION)
		pHolyCity = m_pPlayer->getCapitalCity();

	int iEnemyReligionsNearby = 0;
	int iNumNearbyPlayersWithoutReligion = 0;

	// don't evaluate bonuses from spreading foreign religions
	if (eForeignReligion == NO_RELIGION)
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
			if (iPlayerLoop != m_pPlayer->GetID() && kLoopPlayer.isAlive() && GET_TEAM(m_pPlayer->getTeam()).isHasMet(kLoopPlayer.getTeam()) && kLoopPlayer.GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE)
			{
				if (eReligion != NO_RELIGION && kLoopPlayer.GetReligions()->GetStateReligion(false) == eReligion)
				{
					//they are already following our religion
					continue;
				}

				if (kLoopPlayer.GetReligions()->GetStateReligion(false) != NO_RELIGION)
					iEnemyReligionsNearby++;
				else
					iNumNearbyPlayersWithoutReligion++;
			}
		}
	}

	// what do we want to do with our faith?
	// use it to buy GPs?
	// use it to buy units?
	// use it for spreading?

	// which beliefs do we already have in our religion?
	BeliefList vOurReligionBeliefs;
	if (eForeignReligion == NO_RELIGION && eReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_pPlayer->GetID());
		if (pReligion)
		{
			CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
			const int iNumBeliefs = pkBeliefs->GetNumBeliefs();
			for (int iBeliefLoop = 0; iBeliefLoop < iNumBeliefs; iBeliefLoop++)
			{
				const BeliefTypes eBelief(static_cast<BeliefTypes>(iBeliefLoop));
				CvBeliefEntry* pLoopEntry = pkBeliefs->GetEntry(eBelief);
				if (pLoopEntry && pReligion->m_Beliefs.HasBelief(eBelief))
				{
					vOurReligionBeliefs.push_back(iBeliefLoop);
				}
			}
		}
	}

	// which beliefs are we planning to add?
	BeliefList vOtherPlannedBeliefs;
	if (eSelectedAdditionalBelief1 != NO_BELIEF)
	{
		vOtherPlannedBeliefs.push_back((int)eSelectedAdditionalBelief1);
		vOurReligionBeliefs.push_back((int)eSelectedAdditionalBelief1);
	}
	if (eSelectedAdditionalBelief2 != NO_BELIEF)
	{
		vOtherPlannedBeliefs.push_back((int)eSelectedAdditionalBelief2);
		vOurReligionBeliefs.push_back((int)eSelectedAdditionalBelief2);
	}
	if (eSelectedAdditionalBelief3 != NO_BELIEF)
	{
		vOtherPlannedBeliefs.push_back((int)eSelectedAdditionalBelief3);
		vOurReligionBeliefs.push_back((int)eSelectedAdditionalBelief3);
	}

	bool bReligionBuyUnitsFocus = false;
	bool bReligionGPFocus = false;
	bool bReligionSpreadFocus = false;

	if (eForeignReligion == NO_RELIGION)
	{
		if (iOffensePriority > 5)
		{
			if (pPlayerTraits->IsCanPurchaseNavalUnitsFaith())
			{
				bReligionBuyUnitsFocus = true;
			}
			else
			{
				for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
				{
					CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
					if (pkBeliefInfo)
					{
						// Unlocks units?
						for (int i = (int)m_pPlayer->GetCurrentEra(); i < GC.getNumEraInfos(); i++)
						{
							if (pkBeliefInfo->IsFaithUnitPurchaseEra(i))
							{
								bReligionBuyUnitsFocus = true;
								break;
							}
						}
					}
				}
			}
		}

		if (!bNoMissionary && !bNoNaturalSpread)
		{
			int iNumReligionsStillToFound = GC.getGame().GetGameReligions()->GetNumReligionsStillToFound();
			// exclude our own religion if we're founding
			if (bFoundingReligion)
				iNumReligionsStillToFound--;

			if (iNumNearbyPlayersWithoutReligion - iNumReligionsStillToFound > 0)
			{
				for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
				{
					CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
					if (pkBeliefInfo)
					{
						if (pkBeliefInfo->GetHappinessPerXPeacefulForeignFollowers() > 0 || pkBeliefInfo->GetGoldWhenCityAdopts() > 0)
						{
							bReligionSpreadFocus = true;
							break;
						}
						for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							if (pkBeliefInfo->GetYieldFromSpread(iI) > 0 || pkBeliefInfo->GetYieldFromForeignSpread(iI) > 0 || pkBeliefInfo->GetYieldChangePerXForeignFollowers(iI) > 0)
							{
								bReligionSpreadFocus = true;
								break;
							}
						}
					}
				}
			}
		}

		if (!bReligionBuyUnitsFocus && !bReligionSpreadFocus)
		{
			if (pDiploAI->IsGoingForCultureVictory())
			{
				bReligionGPFocus = true;
			}
			else
			{
				for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
				{
					CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
					if (pkBeliefInfo)
					{
						for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							if (pkBeliefInfo->GetYieldFromGPUse(iI) > 0)
							{
								bReligionGPFocus = true;
								break;
							}
							for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
							{
								if (pkBeliefInfo->GetGreatPersonExpendedYield(iJ, iI) > 0 || pkBeliefInfo->GetGreatPersonBornYield(iJ, iI) > 0)
								{
									bReligionGPFocus = true;
									break;
								}
							}
						}
						if (bReligionGPFocus)
							break;
					}
				}
			}
		}
	}


	/// ///////////////////////////
	// PART 2: Yield scoring
	// Based on the evaluations above calculate a yield score for each yield type. Yield scores are between 10 and 1000
	/// ///////////////////////////

	vector<int> vYieldScores;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		vYieldScores.push_back(ScoreYieldForReligionTimes100((YieldTypes)iI, vOurReligionBeliefs, bReligionBuyUnitsFocus || bReligionSpreadFocus, bFoundingPantheon));
	}

	/// ///////////////////////////
	// PART 3: Score belief for plots
	//
	// In the evaluations here and in the parts below, a belief that provides a value of +1 [YIELD_TYPE] per turn to the player
	// will be given a score of AvailabilityModifier * YieldScore(YIELD_TYPE) / 100.
	// 
	// AvailabilityModifier has a value of 10 if the yield bonus is granted immediately upon adopting the belief.
	// It has a lower value if the yield bonus is unlocked later (e.g. if it's tied to a building that still needs to be built in
	// a city) or if there's a chance the player may not benefit from it (e.g. yields on unowned plots; the plots might also be 
	// settled by someone else).
	//
	/// ///////////////////////////


	int iScorePlot = 0;
	int iNumNearbyUnownedLandTilesToExpand = 0;

	// Loop through each nearby plot that we own or might own in the future
	for (int iI = 0; iI < viPlotWeights.size(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(viPlotWeights.GetElement(iI));
		if (pPlot->getOwner() == NO_PLAYER && pPlot->getDomain() == DOMAIN_LAND && viPlotWeights.GetWeight(iI) > 0)
		{
			// count unowned land tiles for later
			iNumNearbyUnownedLandTilesToExpand++;
		}

		// Score it
		int iScoreAtPlotTimes100 = ScoreBeliefAtPlotTimes100(pEntry, pPlot, bConsiderFutureTech, vYieldScores);
		if (iScoreAtPlotTimes100 <= 0)
			continue;

		iScorePlot += viPlotWeights.GetWeight(iI) * iScoreAtPlotTimes100;
	}
	iScorePlot /= 100;

	/// ///////////////////////////
	// PART 4: Score belief for cities
	/// ///////////////////////////

	int iScoreCityOwned = 0;
	int iScoreCityPotential = 0;

	int iNumSettlersOwned = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, false);
	int iNumSettlersTraining = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true) - iNumSettlersOwned;
	int iPreferredNewCities = min(6, max(iNumSettlersOwned + iNumSettlersTraining, iNumNearbyUnownedLandTilesToExpand / 20));

	int iNumCurrentFollowers = 0;
	int iNumNearbyCitiesToSpreadTo = 0;
	int iNumNearbyFutureFollowers = 0;

	if (eForeignReligion == NO_RELIGION)
	{
		iNumNearbyCitiesToSpreadTo = GetNumCitiesToSpreadReligionTo(eReligion, iNumNearbyFutureFollowers, bFoundingReligion, bFoundingPantheon) + iPreferredNewCities;
		iNumNearbyFutureFollowers += iPreferredNewCities * 5;

		iNumCurrentFollowers = bFoundingPantheon ? 0 : (bFoundingReligion ? (m_pPlayer->getCapitalCity()->getPopulation() * 2 / 3) : (m_pPlayer->GetReligions()->GetNumDomesticFollowers(eReligion) + m_pPlayer->GetReligions()->GetNumForeignFollowers(false, eReligion)));
	}

	// store some values that we need for city-level and player-level belief evaluation
	ScoreBeliefContext kContext;
	kContext.eReligion = eReligion;
	kContext.bFoundingReligion = bFoundingReligion;
	kContext.pHolyCity = pHolyCity;
	kContext.iOffensePriority = iOffensePriority;
	kContext.iDefensePriority = iDefensePriority;
	kContext.iWonderPriority = iWonderPriority;
	kContext.iEnemyReligionsNearby = iEnemyReligionsNearby;
	kContext.iNumNearbyCitiesToSpreadTo = iNumNearbyCitiesToSpreadTo;
	kContext.vOtherPlannedBeliefs = vOtherPlannedBeliefs;
	kContext.vOurReligionBeliefs = vOurReligionBeliefs;
	kContext.bIsExpansion = bIsExpansion;
	kContext.bReligionBuyUnitsFocus = bReligionBuyUnitsFocus;
	kContext.bReligionGPFocus = bReligionGPFocus;
	kContext.bReligionSpreadFocus = bReligionSpreadFocus;
	kContext.iNumNeighbors = iNumNeighbors;
	kContext.iNeighborWarmongerThreat = iNeighborWarmongerThreat;
	kContext.iNumNearbyFutureFollowers = iNumNearbyFutureFollowers;
	kContext.iNumCurrentFollowers = iNumCurrentFollowers;

	vector<CvCity*> vCityList;
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		vCityList.push_back(pLoopCity);

	}
	vCityList.push_back(NULL); // we use this to score potential cities

	int iPotentialCityBaseScore = 0;
	for (vector<CvCity*>::iterator it = vCityList.begin(); it != vCityList.end(); ++it)
	{
		int iScoreAtCity = ScoreBeliefAtCity(pEntry, *it, eForeignReligion, vYieldScores, kContext);
		if (*it != NULL)
		{
			// we evaluated one of our existing cities
			iScoreCityOwned += iScoreAtCity;
		}
		else
		{
			// we evaluated a potential new city
			iPotentialCityBaseScore = iScoreAtCity;
		}
	}

	if (iPreferredNewCities > 0)
	{
		// value of slightly below 100 for every settler we currently have, diminishing value for all other new potential cities
		iScoreCityPotential = (90 * iNumSettlersOwned + 80 * iNumSettlersTraining) * iPotentialCityBaseScore / 100;

		// for cities we don't have a settler for yet, reduce the value every time
		for (int iLoop = 0; iLoop < iPreferredNewCities - iNumSettlersOwned - iNumSettlersTraining; iLoop++)
		{
			iPotentialCityBaseScore *= 60;
			iPotentialCityBaseScore /= 100;
			iScoreCityPotential += iPotentialCityBaseScore;
		}
	}

	/// ///////////////////////////
	// PART 5: Score belief for player
	/// ///////////////////////////

	ScoreBeliefPlayerBreakdown kPlayerBreakdown;
	int iScorePlayer = ScoreBeliefForPlayer(pEntry, eForeignReligion, vYieldScores, kContext, &kPlayerBreakdown);


	/// ///////////////////////////
	// PART 6: Put it all together; modify score based on belief requirements
	/// ///////////////////////////

	int iRtnValue = iScorePlot + iScoreCityOwned + iScoreCityPotential + iScorePlayer;

	//Final calculations
	int iMultiplier = 100;
	if (pEntry->RequiresPeace())
	{
		if (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
			iMultiplier = 0;
		else if (m_pPlayer->IsAtWar())
			iMultiplier /= 5;
		else
		{
			int iWorstApproach = (int)CIV_APPROACH_FRIENDLY;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
				if (kLoopPlayer.isAlive() && iPlayerLoop != m_pPlayer->GetID() && GET_TEAM(m_pPlayer->getTeam()).isHasMet(kLoopPlayer.getTeam()))
				{
					iWorstApproach = min(iWorstApproach, (int)kLoopPlayer.GetDiplomacyAI()->GetSurfaceApproach(m_pPlayer->GetID()));
				}
			}
			if (iWorstApproach == CIV_APPROACH_WAR)
				iMultiplier /= 5;
			else if (iWorstApproach == CIV_APPROACH_HOSTILE)
				iMultiplier /= 4;
		}
	}
	iRtnValue *= iMultiplier;
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
		pLog = LOGFILEMGR.GetLog("TotalBeliefScoringReligionLog.csv", FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strBaseString += playerName + ", ";

		strDesc = GetLocalizedText(pEntry->getShortDescription());
		strTemp.Format("Belief %s (%d), Plot: %d, Owned Cities: %d, Potential Cities (%d planned, %d settlers): %d, Player: %d", strDesc.GetCString(), pEntry->GetID(), iScorePlot, iScoreCityOwned, iPreferredNewCities, iNumSettlersOwned + iNumSettlersTraining, iScoreCityPotential, iScorePlayer);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("Player Score for Belief %s: %d. War: %d, Defense: %d, Happiness: %d, Foreign Cities: %d, Passive Spread: %d, Active Spread: %d, Sacred Sites: %d, Diplo: %d, Great Persons: %d, Misc: %d", strDesc.GetCString(), iRtnValue, kPlayerBreakdown.iWar, kPlayerBreakdown.iDefense, kPlayerBreakdown.iHappiness, kPlayerBreakdown.iForeignCity, kPlayerBreakdown.iPassiveSpread, kPlayerBreakdown.iActiveSpread, kPlayerBreakdown.iBuilding, kPlayerBreakdown.iDiplo, kPlayerBreakdown.iGreatPerson, kPlayerBreakdown.iMisc);
		strOutBuf = strBaseString + strTemp;

		if (iMultiplier != 100)
		{
			strTemp.Format(". Belief requirements score reduction: %d%", iMultiplier - 100);
			strOutBuf = strOutBuf + strTemp;
		}
		pLog->Msg(strOutBuf);
	}

	return max(0, iRtnValue);
}

int CvReligionAI::ScoreBeliefForPlayer(CvBeliefEntry* pEntry, ReligionTypes eForeignReligion, vector<int>& vYieldScores, const ScoreBeliefContext& kContext, ScoreBeliefPlayerBreakdown* pBreakdown) const
{
	ReligionTypes eReligion = kContext.eReligion;
	bool bFoundingReligion = kContext.bFoundingReligion;
	CvCity* pHolyCity = kContext.pHolyCity;
	int iOffensePriority = kContext.iOffensePriority;
	int iDefensePriority = kContext.iDefensePriority;
	int iWonderPriority = kContext.iWonderPriority;
	int iEnemyReligionsNearby = kContext.iEnemyReligionsNearby;
	int iNumNearbyCitiesToSpreadTo = kContext.iNumNearbyCitiesToSpreadTo;
	const BeliefList& vOtherPlannedBeliefs = kContext.vOtherPlannedBeliefs;
	const BeliefList& vOurReligionBeliefs = kContext.vOurReligionBeliefs;
	bool bReligionBuyUnitsFocus = kContext.bReligionBuyUnitsFocus;
	bool bReligionGPFocus = kContext.bReligionGPFocus;
	bool bReligionSpreadFocus = kContext.bReligionSpreadFocus;
	int iNumNeighbors = kContext.iNumNeighbors;
	int iNeighborWarmongerThreat = kContext.iNeighborWarmongerThreat;
	int iNumNearbyFutureFollowers = kContext.iNumNearbyFutureFollowers;
	int iNumCurrentFollowers = kContext.iNumCurrentFollowers;
	bool bIsExpansion = kContext.bIsExpansion;

	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();
	bool bIsCulture = pDiploAI->IsGoingForCultureVictory();
	bool bIsWarmonger = pDiploAI->IsGoingForWorldConquest();

	int iEraScaleFactorTimes100 = 100 * max(1, (int)m_pPlayer->GetCurrentEra()) + 25 * (GC.getNumEraInfos() - m_pPlayer->GetCurrentEra() - 1);
	int iGameSpeedInstantYieldPercent = GC.getGame().getGameSpeedInfo().getInstantYieldPercent();

	UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY");
	bool bNoMissionary = m_pPlayer->GetPlayerTraits()->NoTrain(eMissionary);
	bool bNoNaturalSpread = m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread();

	bool bFoundingPantheon = !bFoundingReligion && m_pPlayer->GetReligions()->GetStateReligion(true) == NO_RELIGION && eForeignReligion == NO_RELIGION;

	int iScorePlayer = 0;
	int iTemp = 0;

	//Let's look at all cities and get their religious status. Gives us a feel for what we can expect to gain in the near future.
	int iNumOurCitiesWithReligion = GetNumCitiesWithReligion(eReligion, bFoundingReligion, bFoundingPantheon, true);
	int iNumCitiesWithReligionTotal = GetNumCitiesWithReligion(eReligion, bFoundingReligion, bFoundingPantheon, false);

	//////////////////
	//Conquest-related player bonuses.
	///////////////////////
	int iWarTemp = 0;
	int iNumUnits = m_pPlayer->getNumMilitaryUnits();
	if (iOffensePriority > 0)
	{
		// modifiers
		int iMaxDistanceMod = 100;
		if (pEntry->GetMaxDistance() != 0)
		{
			iMaxDistanceMod = min(100, pEntry->GetMaxDistance() * 10);
		}

		if (pEntry->GetFaithFromKills() > 0)
		{
			iWarTemp += (iOffensePriority * iNumUnits * pEntry->GetFaithFromKills() * vYieldScores[YIELD_FAITH] / 10000) * iMaxDistanceMod / 300;
		}
		if (pEntry->GetFaithFromDyingUnits() > 0)
		{
			iWarTemp += (iOffensePriority * iNumUnits * pEntry->GetFaithFromDyingUnits() * vYieldScores[YIELD_FAITH] / 10000) * iMaxDistanceMod / 1200;
		}
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pEntry->GetYieldFromBarbarianKills((YieldTypes)iI))
			{
				iWarTemp += iOffensePriority * iNumUnits * pEntry->GetYieldFromBarbarianKills((YieldTypes)iI) * vYieldScores[iI] / 30000;
			}
			if (pEntry->GetYieldFromKills((YieldTypes)iI))
			{
				// modifiers
				int iMaxDistanceMod = 100;
				if (pEntry->GetMaxDistance() != 0)
				{
					iMaxDistanceMod = min(100, pEntry->GetMaxDistance() * 10);
				}
				iWarTemp += (iOffensePriority * iNumUnits * pEntry->GetYieldFromKills((YieldTypes)iI) * vYieldScores[iI] / 10000) * iMaxDistanceMod / 100;
			}
			if (pEntry->GetYieldFromConquest(iI) > 0)
			{
				iWarTemp += iOffensePriority * iNumUnits * pEntry->GetYieldFromConquest(iI) * vYieldScores[iI] / 100000;
			}
			if (pEntry->GetYieldFromRemoveHeresy((YieldTypes)iI) > 0)
			{
				iWarTemp += iOffensePriority * iNumUnits * pEntry->GetYieldFromRemoveHeresy((YieldTypes)iI) * vYieldScores[iI] / 50000;
			}
			if (pEntry->GetYieldFromPillageGlobal((YieldTypes)iI, false) > 0 || pEntry->GetYieldFromPillageGlobal((YieldTypes)iI, true) > 0)
			{
				iWarTemp += iOffensePriority * iNumUnits * (100 * pEntry->GetYieldFromPillageGlobal((YieldTypes)iI, false) + iEraScaleFactorTimes100 * pEntry->GetYieldFromPillageGlobal((YieldTypes)iI, true)) * vYieldScores[iI] / 100000;
			}
		}
		if (pEntry->GetUnitProductionModifier() > 0)
		{
			iWarTemp += iOffensePriority * iNumUnits * pEntry->GetUnitProductionModifier() / 20;
		}
		if (pEntry->GetCombatModifierEnemyCities() > 0)
		{
			iWarTemp += iOffensePriority * iNumUnits * pEntry->GetCombatModifierEnemyCities() / 20;
		}
		// todo: Belief_FreePromotions and Belief_SpecificFaithUnitPurchase
		if (pEntry->GetCombatBonusTheirLands() > 0)
		{
			iWarTemp += iOffensePriority * iNumUnits * pEntry->GetCombatBonusTheirLands() / 100;
		}
		if (pEntry->GetCombatBonusVersusOtherReligionTheirLands() > 0)
		{
			iWarTemp += iOffensePriority * iNumUnits * pEntry->GetCombatBonusVersusOtherReligionTheirLands() / 200;
		}

		GreatPersonTypes eGeneral = (GreatPersonTypes)GC.getInfoTypeForString("GREATPERSON_GENERAL");
		if (pEntry->GetGreatPersonRateModifier(eGeneral) > 0)
		{
			iWarTemp += (iOffensePriority + pPlayerTraits->GetGreatPersonGWAM(eGeneral) / 5) * pEntry->GetGreatPersonRateModifier(eGeneral) / 3;

			UnitClassTypes eGeneralClass = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL");
			CvUnitClassInfo* pkGeneralClassInfo = GC.getUnitClassInfo(eGeneralClass);
			if (pkGeneralClassInfo && m_pPlayer->GetSpecificUnitType(eGeneralClass) != (UnitTypes)pkGeneralClassInfo->getDefaultUnitIndex())
			{
				iWarTemp += 5 * pEntry->GetGreatPersonRateModifier(eGeneral);
			}
		}

		GreatPersonTypes eAdmiral = (GreatPersonTypes)GC.getInfoTypeForString("GREATPERSON_ADMIRAL");
		if (pEntry->GetGreatPersonRateModifier(eAdmiral) > 0)
		{
			iWarTemp += (iOffensePriority + pPlayerTraits->GetGreatPersonGWAM(eAdmiral) / 5) * pEntry->GetGreatPersonRateModifier(eAdmiral) / 3;

			UnitClassTypes eAdmiralClass = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_GREAT_ADMIRAL");
			CvUnitClassInfo* pkAdmiralClassInfo = GC.getUnitClassInfo(eAdmiralClass);
			if (pkAdmiralClassInfo && m_pPlayer->GetSpecificUnitType(eAdmiralClass) != (UnitTypes)pkAdmiralClassInfo->getDefaultUnitIndex())
			{
				iWarTemp += 5 * pEntry->GetGreatPersonRateModifier(eAdmiral);
			}
		}


		/*if (pEntry->ConvertsBarbarians())
		{
			// no additional score, the AI doesn't know how to use this
		}*/

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
		iWarTemp += iOffensePriority * iNumUnlockEras * min(50 + 2 * m_pPlayer->GetTotalFaithPerTurnTimes100() / 100, 200) / 10 / ((bReligionSpreadFocus || bReligionGPFocus) ? 2 : 1);
	}
	iScorePlayer += iWarTemp;


	//////////////////
	//Defense-related player bonuses.
	///////////////////////
	int iDefenseTemp = 0;

	if (pEntry->GetFriendlyHealChange() > 0)
	{
		iDefenseTemp += iDefensePriority * iNumUnits * pEntry->GetFriendlyHealChange() / 30;
	}
	if (pEntry->GetCityRangeStrikeModifier() > 0)
	{
		iDefenseTemp += iDefensePriority * pEntry->GetCityRangeStrikeModifier() / 4;
	}
	if (pEntry->GetUnitProductionModifier() > 0)
	{
		iDefenseTemp += iDefensePriority * pEntry->GetUnitProductionModifier() / 8;
	}
	if (pEntry->GetCombatModifierFriendlyCities() > 0)
	{
		iDefenseTemp += iDefensePriority * min(10, m_pPlayer->getNumMilitaryUnits()) * pEntry->GetCombatModifierFriendlyCities() / 20;
	}
	if (pEntry->GetFaithFromDyingUnits() > 0)
	{
		iDefenseTemp += iDefensePriority * min(10, m_pPlayer->getNumMilitaryUnits()) * pEntry->GetFaithFromDyingUnits() / 200;
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerHeal((YieldTypes)iI))
		{
			iDefenseTemp += iDefensePriority * pEntry->GetYieldPerHeal((YieldTypes)iI) * vYieldScores[iI] / 100000 / (pEntry->RequiresOwnTerritory() ? 2 : 1);
		}
	}
	if (pEntry->GetCombatBonusOwnLands() > 0)
	{
		iDefenseTemp += iDefensePriority * pEntry->GetCombatBonusOwnLands() / 100;
	}
	if (pEntry->GetCombatBonusVersusOtherReligionOwnLands() > 0)
	{
		iDefenseTemp += iDefensePriority * pEntry->GetCombatBonusVersusOtherReligionOwnLands() / 200;
	}


	GreatPersonTypes eGeneral = (GreatPersonTypes)GC.getInfoTypeForString("GREATPERSON_GENERAL");
	if (pEntry->GetGreatPersonRateModifier(eGeneral) > 0)
	{
		iDefenseTemp += (min(15, iDefensePriority) / 2 + pPlayerTraits->GetGreatPersonGWAM(eGeneral) / 5) * pEntry->GetGreatPersonRateModifier(eGeneral);
	}

	GreatPersonTypes eAdmiral = (GreatPersonTypes)GC.getInfoTypeForString("GREATPERSON_ADMIRAL");
	if (pEntry->GetGreatPersonRateModifier(eAdmiral) > 0)
	{
		iDefenseTemp += (min(15, iDefensePriority) / 2 + pPlayerTraits->GetGreatPersonGWAM(eAdmiral) / 5) * pEntry->GetGreatPersonRateModifier(eAdmiral);
	}

	iScorePlayer += iDefenseTemp;

	//////////////////
	//Wonder-related player bonuses.
	///////////////////////
	int iWonderTemp = 0;
	if (pEntry->GetWonderProductionModifier() > 0)
	{
		iWonderTemp = iWonderPriority * pEntry->GetWonderProductionModifier() / 2;
		if (pEntry->GetObsoleteEra() > 0)
		{
			if (pEntry->GetObsoleteEra() > GC.getGame().getCurrentEra())
			{
				iWonderTemp *= pEntry->GetObsoleteEra() - GC.getGame().getCurrentEra();
				iWonderTemp /= 5;
			}
			else
			{
				iWonderTemp = 0;
			}
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerWorldWonderConstruction(iI) > 0)
		{
			iWonderTemp += iWonderPriority * pEntry->GetYieldPerWorldWonderConstruction(iI) * vYieldScores[iI] / 5000;
		}
		if (pEntry->GetYieldChangeWorldWonder(iI) > 0)
		{
			// current wonders give bonuses now, add small value for future wonders
			iWonderTemp += (iWonderPriority + 10 * m_pPlayer->GetNumWonders()) * pEntry->GetYieldChangeWorldWonder(iI) * vYieldScores[iI] / 100;
		}
	}
	iScorePlayer += iWonderTemp;

	//////////////////
	// Happiness
	///////////////////////
	int iHappinessTemp = 0;
	int iHappinessValueTimes100 = m_pPlayer->GetHappinessValueTimes100();
	if (pEntry->GetPlayerHappiness() > 0)
	{
		iHappinessTemp += 10 * pEntry->GetPlayerHappiness() * iHappinessValueTimes100 / 100;
	}
	if (!GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (pEntry->GetHappinessFromSpies() != 0)
		{
			iHappinessTemp += 12 * pEntry->GetHappinessFromSpies() * max(1, m_pPlayer->GetEspionage()->GetNumSpies()) * iHappinessValueTimes100 / 100;
		}
		if (pEntry->GetHappinessFromForeignSpies() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
		{
			iHappinessTemp += 8 * pEntry->GetHappinessFromForeignSpies() * max(1, m_pPlayer->GetEspionage()->GetNumSpies()) * iHappinessValueTimes100 / 100;
		}
	}

	for (int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
	{
		ResourceTypes eResource = (ResourceTypes)iJ;
		if (pEntry->GetResourceHappiness(iJ) > 0)
		{
			iHappinessTemp += 10 * m_pPlayer->getNumResourceFromTiles(eResource) * pEntry->GetResourceHappiness(iJ) * iHappinessValueTimes100 / 100;
		}
	}
	if (pEntry->GetHappinessPerPantheon() > 0)
	{
		int iPantheon = GC.getGame().GetGameReligions()->GetNumPantheonsCreated();
		if (bFoundingPantheon)
			iPantheon++;

		if (iPantheon > 8)
		{
			iPantheon = 8;
		}
		// current pantheons
		iHappinessTemp += 7 * iPantheon * pEntry->GetHappinessPerPantheon() * iHappinessValueTimes100 / 100;
		// future pantheons
		iHappinessTemp += 2 * (8 - iPantheon) * pEntry->GetHappinessPerPantheon() * iHappinessValueTimes100 / 100;
	}
	iScorePlayer += iHappinessTemp;

	//////////////////
	//Spread bonuses.
	///////////////////////

	int iPassiveSpreadTemp = 0;
	int iActiveSpreadTemp = 0;

	//don't evaluate spread for foreign religions. don't evaluate spread if no enemy religion is nearby or can be founded
	if (eForeignReligion == NO_RELIGION && (iEnemyReligionsNearby > 0 || GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() - (bFoundingReligion ? 1 : 0) > 0))
	{
		if (!bNoNaturalSpread)
		{
			if (pEntry->GetPressureChangeTradeRoute() != 0 && !m_pPlayer->GetPlayerTraits()->IsNoOpenTrade())
			{
				iPassiveSpreadTemp += (pEntry->GetPressureChangeTradeRoute() * m_pPlayer->GetTrade()->GetNumTradeRoutesPossible()) / 4;
			}
			if (pEntry->GetSpreadDistanceModifier() != 0)
			{
				iPassiveSpreadTemp += pEntry->GetSpreadDistanceModifier() * iNumCitiesWithReligionTotal * 3 / 2;
			}

			if (pEntry->GetSpreadStrengthModifier() != 0)
			{
				iPassiveSpreadTemp += pEntry->GetSpreadStrengthModifier() * iNumCitiesWithReligionTotal;
				if (pEntry->GetSpreadModifierDoublingTech() != NO_TECH)
				{
					TechTypes eDoublingTech = pEntry->GetSpreadModifierDoublingTech();
					int iAvailabilityMod;
					if (m_pPlayer->HasTech(eDoublingTech) || m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == eDoublingTech)
					{
						iAvailabilityMod = 10;
					}
					else
					{
						CvTechEntry* pkTechInfo = GC.getTechInfo(eDoublingTech);
						int iEraNeeded = pkTechInfo->GetEra();
						int iCurrentEra = m_pPlayer->GetCurrentEra();
						iAvailabilityMod = max(0, 7 - 3 * (iEraNeeded - iCurrentEra));
					}
					iPassiveSpreadTemp += pEntry->GetSpreadStrengthModifier() * iNumCitiesWithReligionTotal * iAvailabilityMod / 10;
				}
			}

			if (pEntry->GetSpyPressure() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
			{
				iPassiveSpreadTemp += pEntry->GetSpyPressure() * 5 * max(1, min(3, m_pPlayer->GetEspionage()->GetNumSpies()));
			}

			if (pEntry->GetSpyPressureErosion() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
			{
				iPassiveSpreadTemp += pEntry->GetSpyPressureErosion() * 5 * max(1, min(3, m_pPlayer->GetEspionage()->GetNumSpies()));
			}

			if (!m_pPlayer->GetPlayerTraits()->IsForeignReligionSpreadImmune())
			{
				if (pEntry->GetInquisitorPressureRetention() > 0)
				{
					iPassiveSpreadTemp += pEntry->GetInquisitorPressureRetention() * min(4, iEnemyReligionsNearby) / 2;
					iPassiveSpreadTemp += pEntry->GetOtherReligionPressureErosion() * min(4, iEnemyReligionsNearby) / 2;
				}
			}

			// passive spread is good if we can't build missionaries
			if (bNoMissionary)
				iPassiveSpreadTemp *= 2;
			// if we want to spread with missionaries, discourage this
			if (bReligionSpreadFocus)
				iPassiveSpreadTemp /= 2;
			if (bReligionBuyUnitsFocus || bReligionGPFocus)
			{
				// if we want to use our faith to buy units or GP, encourage this slightly, it saves us missionaries/inquisitors
				iPassiveSpreadTemp *= 2;
				iPassiveSpreadTemp /= 3;
			}
		}

		if (!bNoMissionary)
		{
			if (pEntry->GetGoldWhenCityAdopts() > 0)
			{
				// this is a one-time yield, it doesn't scale well, give it a rather low value
				iActiveSpreadTemp += iNumNearbyCitiesToSpreadTo * pEntry->GetGoldWhenCityAdopts() * vYieldScores[YIELD_GOLD] / 3000;
			}
			if (pEntry->GetMissionaryStrengthModifier() > 0 || pEntry->GetMissionaryCostModifier() != 0)
			{
				iActiveSpreadTemp += iNumNearbyCitiesToSpreadTo * (pEntry->GetMissionaryStrengthModifier() - pEntry->GetMissionaryCostModifier()) * (100 + iEnemyReligionsNearby * 25) / 600;
			}
			if (pEntry->GetOtherReligionPressureErosion() > 0)
			{
				iActiveSpreadTemp += iNumNearbyCitiesToSpreadTo * iEnemyReligionsNearby * pEntry->GetOtherReligionPressureErosion() / 50;
			}

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				if (pEntry->GetYieldFromSpread(iI) > 0)
				{
					iActiveSpreadTemp += (iNumNearbyCitiesToSpreadTo + 1) * pEntry->GetYieldFromSpread(iI) * vYieldScores[iI] / 100 * iGameSpeedInstantYieldPercent * (100 + m_pPlayer->GetTotalFaithPerTurnTimes100() / 100) / 10000;
				}
				if (pEntry->GetYieldFromForeignSpread(iI) > 0)
				{
					iActiveSpreadTemp += (iNumNearbyCitiesToSpreadTo - (m_pPlayer->getNumCities() - iNumOurCitiesWithReligion)) * iEnemyReligionsNearby * pEntry->GetYieldFromForeignSpread(iI) * vYieldScores[iI] / 250 * iGameSpeedInstantYieldPercent * (100 + m_pPlayer->GetTotalFaithPerTurnTimes100() / 100) / 10000;
				}
				if (pEntry->GetYieldFromConversion(iI) > 0)
				{
					// calculate the total amount of yields we'll get
					// first sum up the scalars. we start at 0 to take into account the initial conversion of the holy city
					int iTotalYieldTimes100 = 0;
					for (int iCnt = 0; iCnt <= iNumNearbyCitiesToSpreadTo; iCnt++)
					{
						iTotalYieldTimes100 += 100 + min(iCnt, pEntry->GetCityScalerLimiter()) * min(iCnt, pEntry->GetCityScalerLimiter());
					}
					// multiply with yield amount and score
					iTotalYieldTimes100 *= pEntry->GetYieldFromConversion(iI) * vYieldScores[iI] / 100;

					// iAvailabilityModifier = 10, divide by 100 because of Times100, divide by 150 because it's a one-time yield and not a yield per turn. the actual yields scale with game speed, this is covered in the evaluation by dividing by the fixed value instead of an estimate the remaining game turns
					iActiveSpreadTemp += iTotalYieldTimes100 / 1500;
				}
				if (pEntry->GetYieldFromConversionExpo(iI) > 0)
				{
					// calculate the total amount of yields we'll get
					// first sum up the scalars. we start at 0 to take into account the initial conversion of the holy city
					int iTotalYield = 0;
					for (int iCnt = 0; iCnt <= iNumNearbyCitiesToSpreadTo; iCnt++)
					{
						iTotalYield += (iCnt + 1);
					}
					// multiply with yield amount and score
					iTotalYield *= pEntry->GetYieldFromConversionExpo(iI) * vYieldScores[iI] / 100;

					// iAvailabilityModifier = 10, divide by 150 because it's a one-time yield and not a yield per turn. the actual yields scale with game speed, this is covered in the evaluation by dividing by the fixed value instead of an estimate the remaining game turns
					iActiveSpreadTemp += iTotalYield / 15;
				}
			}

			// extra missionary strength?
			iActiveSpreadTemp *= (100 + m_pPlayer->GetMissionaryExtraStrength() + pPlayerTraits->GetExtraMissionaryStrength());
			iActiveSpreadTemp /= 100;
			iActiveSpreadTemp *= 100 - pPlayerTraits->GetFaithCostModifier();
			iActiveSpreadTemp /= 100;
		}

		// spreading using prophets
		if (pEntry->GetProphetStrengthModifier() > 0 || pEntry->GetProphetCostModifier() != 0)
		{
			iActiveSpreadTemp += iNumNearbyCitiesToSpreadTo * (pPlayerTraits->IsProphetFervor() ? 3 : 1) * (pEntry->GetProphetStrengthModifier() - pEntry->GetProphetCostModifier()) * (100 - pPlayerTraits->GetFaithCostModifier()) / 100 * (100 + iEnemyReligionsNearby * 25) / (bNoMissionary ? 1 : 2) / 200;
		}

		// extra number of spreads
		/* 2 is the base number of missionary spreads. todo: get from db */
		iActiveSpreadTemp *= 2 + m_pPlayer->GetNumMissionarySpreads();
		iActiveSpreadTemp /= 2;

		// modifiers
		if (bReligionSpreadFocus)
			iActiveSpreadTemp *= 2;
		if (bReligionGPFocus)
		{
			iActiveSpreadTemp *= 2;
			iActiveSpreadTemp /= 3;
		}
		if (bReligionBuyUnitsFocus)
			iActiveSpreadTemp /= 2;

	}

	iScorePlayer += iPassiveSpreadTemp;
	iScorePlayer += iActiveSpreadTemp;


	//////////////////
	//Yield from Foreign Cities (own cities have already been evaluated in ScoreBeliefForCity)
	///////////////////////

	int iForeignCityYields = 0;
	if (eForeignReligion == NO_RELIGION)
	{
		int iNumForeignCitiesToSpreadTo = iNumNearbyCitiesToSpreadTo - (m_pPlayer->getNumCities() - iNumOurCitiesWithReligion);
		int iAvailability = bReligionSpreadFocus ? 5 : 3; // todo: evaluate general strength of the religion compared to the competitors
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pEntry->GetYieldChangePerForeignCity(iI) > 0)
			{
				iForeignCityYields += (10 * m_pPlayer->GetReligions()->GetNumForeignCitiesFollowing(eReligion) + iAvailability * iNumForeignCitiesToSpreadTo) * pEntry->GetYieldChangePerForeignCity(iI) * vYieldScores[iI] / 100;
			}
			if (pEntry->GetYieldChangePerXForeignFollowers(iI) > 0)
			{
				iForeignCityYields += (10 * m_pPlayer->GetReligions()->GetNumForeignFollowers(false, eReligion) + 5 * iAvailability * iNumForeignCitiesToSpreadTo) * vYieldScores[iI] / 100 / pEntry->GetYieldChangePerXForeignFollowers(iI);
			}
		}

		// score only foreign cities here
		if (pEntry->GetGoldPerXFollowers() > 0)
		{
			iForeignCityYields += (10 * m_pPlayer->GetReligions()->GetNumForeignFollowers(false, eReligion) + 5 * iAvailability * iNumForeignCitiesToSpreadTo) * vYieldScores[YIELD_GOLD] / 100 / pEntry->GetGoldPerXFollowers();
		}
		if (pEntry->GetGoldPerFollowingCity() > 0)
		{
			iForeignCityYields += (10 * m_pPlayer->GetReligions()->GetNumForeignCitiesFollowing(eReligion) + iAvailability * iNumForeignCitiesToSpreadTo) * pEntry->GetGoldPerFollowingCity() * vYieldScores[YIELD_GOLD] / 100;
		}
		if (pEntry->GetHappinessPerFollowingCity() > 0)
		{
			iForeignCityYields += (int)(pEntry->GetHappinessPerFollowingCity() * (10 * m_pPlayer->GetReligions()->GetNumForeignCitiesFollowing(eReligion) + iAvailability * iNumForeignCitiesToSpreadTo) * iHappinessValueTimes100 / 100);
		}
		if (pEntry->GetHappinessPerXPeacefulForeignFollowers() > 0 && !bIsWarmonger)
		{
			iForeignCityYields += (10 * m_pPlayer->GetReligions()->GetNumForeignFollowers(false, eReligion) + 5 * iAvailability * iNumForeignCitiesToSpreadTo) * iHappinessValueTimes100 / pEntry->GetHappinessPerXPeacefulForeignFollowers() / max(iNeighborWarmongerThreat / 2, 1) / 100;
		}
	}
	iScorePlayer += iForeignCityYields;

	//////////////////
	//Diplo bonuses.
	///////////////////////
	int iDiploTemp = 0;
	bool bDiploVictoryEnabled = GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true));
	if (bDiploVictoryEnabled && (pEntry->GetCityStateMinimumInfluence() > 0 || pEntry->GetFriendlyCityStateSpreadModifier() || pEntry->GetCityStateInfluenceModifier() > 0 || pEntry->GetCityStateInfluenceModifier() > 0))
	{
		// guaranteed friendship with all CS following our religion is good
		int iInfluenceValue = (pEntry->GetCityStateMinimumInfluence() + m_pPlayer->GetMinorFriendshipAnchorMod() > GD_INT_GET(FRIENDSHIP_THRESHOLD_FRIENDS)) ? 2 : 1;
		iInfluenceValue *= pDiploAI->IsGoingForDiploVictory() ? 3 : 1;
		int iMinorScore = 0;
		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
			CvPlayer& minorPlayer = GET_PLAYER(eMinor);

			if (!minorPlayer.isAlive())
				continue;
			if (!GET_TEAM(m_pPlayer->getTeam()).isHasMet(minorPlayer.getTeam()))
				continue;

			if (minorPlayer.GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE)
			{
				iMinorScore += minorPlayer.GetProximityToPlayer(m_pPlayer->GetID()) == PLAYER_PROXIMITY_NEIGHBORS ? 2 : 1;
			}
		}
		iDiploTemp += pEntry->GetCityStateMinimumInfluence() * iInfluenceValue * iMinorScore / 2;
		iDiploTemp += pEntry->GetFriendlyCityStateSpreadModifier() * iInfluenceValue * iMinorScore / 30;
		iDiploTemp += pEntry->GetCityStateInfluenceModifier() * iInfluenceValue * iMinorScore / 10;
		iDiploTemp += pEntry->GetCSYieldBonus() * iInfluenceValue * iMinorScore / 10;

		if (pEntry->GetCityStateInfluenceModifier() > 0 && GD_INT_GET(CSD_GOLD_GIFT_DISABLED) == 0)
		{
			int iAvgGPT = m_pPlayer->GetTreasury()->AverageIncome100(10) / 100;
			iDiploTemp += (pDiploAI->IsGoingForDiploVictory() ? 3 : 1) * pEntry->GetCityStateInfluenceModifier() * min(200, max(0, (iAvgGPT + 25) * 2)) / 200;
		}
	}


	if (pEntry->GetExtraVotes())
	{
		iDiploTemp += 50 * (pDiploAI->IsGoingForDiploVictory() ? 5 : 2) * pEntry->GetExtraVotes() / (bDiploVictoryEnabled ? 1 : 3);
	}
	for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
	{
		if (pEntry->GetImprovementVoteChange((ImprovementTypes)iJ) > 0)
		{
			int iNumImprovements = m_pPlayer->getImprovementCount((ImprovementTypes)iJ);

			if (iNumImprovements == 0 && iJ == GC.getInfoTypeForString("IMPROVEMENT_LANDMARK"))
			{
				bool bCanSeeSites = false;
				// do we have archaeology yet?
				for (int iTech = 0; iTech < GC.getNumTechInfos(); iTech++)
				{
					CvTechEntry* pkTech = GC.getTechInfo((TechTypes)iTech);
					if (pkTech)
					{
						if (pkTech->IsTriggersArchaeologicalSites())
						{
							bCanSeeSites = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)pkTech->GetID());
							break;
						}
					}
				}
				if (!bCanSeeSites)
				{
					// estimate number of landmarks based on territory
					iNumImprovements = m_pPlayer->GetNumPlots() / 30;
				}
			}
			iNumImprovements += 2; // small fixed value for future improvements
			iDiploTemp += 50 * (pDiploAI->IsGoingForDiploVictory() ? 5 : 2) * iNumImprovements / pEntry->GetImprovementVoteChange((ImprovementTypes)iJ) / (bDiploVictoryEnabled ? 1 : 4);
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldFromHost(iI) > 0)
		{
			int iAvailability = 0;
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				if (pLeague->GetHostMember() == m_pPlayer->GetID())
				{
					iAvailability = 10;
				}
				else
				{
					int iOurVotes = pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID());
					int iHighestVotesOtherPlayers = 0;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes ePlayerLoop = (PlayerTypes)iPlayerLoop;
						if (iPlayerLoop == m_pPlayer->GetID() || !GET_PLAYER(ePlayerLoop).isAlive())
							continue;

						int iTheirVotes = pLeague->CalculateStartingVotesForMember(ePlayerLoop);
						iHighestVotesOtherPlayers = max(iHighestVotesOtherPlayers, iTheirVotes);
					}

					iAvailability = min(8, max(0, 5 + (iOurVotes - iHighestVotesOtherPlayers)));
				}
			}
			else
			{
				iAvailability = pDiploAI->IsGoingForDiploVictory() ? 3 : 1;
			}

			iDiploTemp += iAvailability * pEntry->GetYieldFromHost(iI) * iEraScaleFactorTimes100 * vYieldScores[iI] / 10000 / (bDiploVictoryEnabled ? 1 : 3);
		}
		if (pEntry->GetYieldFromProposal(iI) > 0)
		{
			int iAvailability = 0;
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				int iOurVotes = pLeague->CalculateStartingVotesForMember(m_pPlayer->GetID());
				int iSumOtherVotesFriends = 0;
				int iSumOtherVotesNonFriends = 0;
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes ePlayerLoop = (PlayerTypes)iPlayerLoop;
					if (iPlayerLoop == m_pPlayer->GetID() || !GET_PLAYER(ePlayerLoop).isAlive())
						continue;

					int iTheirVotes = pLeague->CalculateStartingVotesForMember(ePlayerLoop);

					if (pDiploAI->GetCivOpinion(ePlayerLoop) >= CIV_OPINION_FAVORABLE)
						iSumOtherVotesFriends += iTheirVotes;
					else
						iSumOtherVotesNonFriends += iTheirVotes;
				}

				iAvailability = 10 * (iOurVotes + iSumOtherVotesFriends / 2) / (iOurVotes + iSumOtherVotesFriends + iSumOtherVotesNonFriends);
			}
			else
			{
				iAvailability = pDiploAI->IsGoingForDiploVictory() ? 3 : 1;
			}
			iDiploTemp += iAvailability * pEntry->GetYieldFromProposal(iI) * iEraScaleFactorTimes100 * vYieldScores[iI] / 10000 / 150 / (bDiploVictoryEnabled ? 1 : 5);
		}
	}

	iScorePlayer += iDiploTemp;


	//////////////////
	//Buildings
	///////////////////////

	// building effects specific to a single city (or a potential future city) are scored in ScoreBeliefAtCity, together with iScoreCityOwned / iScoreCityPotential
	int iBuildingTemp = 0;

	// sacred sites
	if (pEntry->GetFaithBuildingTourism() > 0)
	{
		// how many buildings do we have or can we buy with faith?
		// tourism is a long-term goal, don't reduce the score for buildings that we can purchase but haven't purchased yet
		int iNumFaithBuildings = 0;
		for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
		{
			BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iK);
			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);
				if (pBuildingEntry->IsFaithPurchaseOnly())
				{
					for (BeliefList::const_iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
					{

						CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
						if (pkBeliefInfo && pkBeliefInfo->IsBuildingClassEnabled(iK))
						{
							iNumFaithBuildings++;
							break;
						}
					}
				}
			}
		}

		iBuildingTemp += 10 * (m_pPlayer->getNumCities() - m_pPlayer->GetNumPuppetCities()) * iNumFaithBuildings * pEntry->GetFaithBuildingTourism() * vYieldScores[YIELD_TOURISM] / 100;
	}

	iScorePlayer += iBuildingTemp;

	//////////////////
	//Great persons
	///////////////////////

	int iGPTemp = 0;
	if (pEntry->FaithPurchaseAllGreatPeople())
	{
		int iValue = min(100 + m_pPlayer->GetTotalFaithPerTurnTimes100() / 100, 300);
		// faith cost mod
		iValue *= 100;
		iValue /= (100 + pEntry->GetGreatPeopleFaithCostMod());

		if (bReligionGPFocus)
		{
			iValue *= 2;
		}
		else if (bReligionBuyUnitsFocus)
		{
			iValue *= 2;
			iValue /= 3;
		}
		if (bIsCulture)
		{
			iValue *= 3;
			iValue /= 2;
		}
		iGPTemp += iValue;
	}


	if (pEntry->GetGreatPersonExpendedFaith() > 0)
	{
		for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
		{
			GreatPersonTypes eGP = (GreatPersonTypes)iJ;
			if (eGP == NO_GREATPERSON)
				continue;

			iGPTemp += 10 * pEntry->GetGreatPersonExpendedFaith() * vYieldScores[YIELD_FAITH] / 100 / m_pPlayer->EstimateGreatPersonRate(eGP);
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldFromGPUse(iI) > 0)
		{
			int iScorePerGP = 10 * min(pEntry->GetCityScalerLimiter(), iNumCitiesWithReligionTotal + iNumNearbyCitiesToSpreadTo) * pEntry->GetYieldFromGPUse(iI) * vYieldScores[iI] * iEraScaleFactorTimes100 / 10000;

			for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
			{
				GreatPersonTypes eGP = (GreatPersonTypes)iJ;
				if (eGP == NO_GREATPERSON)
					continue;

				iGPTemp += iScorePerGP / m_pPlayer->EstimateGreatPersonRate(eGP) * (100 + pEntry->GetGreatPersonRateModifier(eGP)) / 100;
			}
		}
	}

	for (int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
	{
		GreatPersonTypes eGP = (GreatPersonTypes)iJ;
		if (eGP == NO_GREATPERSON)
			continue;

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pEntry->GetGreatPersonExpendedYield(iJ, iI) > 0)
			{
				iGPTemp += 10 * min(pEntry->GetCityScalerLimiter(), iNumCitiesWithReligionTotal + iNumNearbyCitiesToSpreadTo) * pEntry->GetGreatPersonExpendedYield(iJ, iI) * vYieldScores[iI] / 100 / m_pPlayer->EstimateGreatPersonRate(eGP) * (100 + pEntry->GetGreatPersonRateModifier(eGP)) / 100;
			}
			if (pEntry->GetGreatPersonBornYield(eGP, iI) > 0)
			{
				iGPTemp += 10 * min(pEntry->GetCityScalerLimiter(), iNumCitiesWithReligionTotal + iNumNearbyCitiesToSpreadTo) * pEntry->GetGreatPersonBornYield(iJ, iI) * vYieldScores[iI] / 100 / m_pPlayer->EstimateGreatPersonRate(eGP) * (100 + pEntry->GetGreatPersonRateModifier(eGP)) / 100;
			}
		}

		if (pEntry->GetGoldenAgeGreatPersonRateModifier(iJ) > 0)
		{
			if (eGP != GC.getInfoTypeForString("GREATPERSON_GENERAL") && eGP != GC.getInfoTypeForString("GREATPERSON_ADMIRAL"))
				iGPTemp += (bIsCulture ? 2 : 1) * pEntry->GetGoldenAgeGreatPersonRateModifier(iJ) * m_pPlayer->EstimateGoldenAgePercentage() / GC.getNumGreatPersonInfos() / 100 / 2;
		}
		if (pEntry->GetGreatPersonRateModifier(iJ) > 0)
		{
			if (eGP != GC.getInfoTypeForString("GREATPERSON_GENERAL") && eGP != GC.getInfoTypeForString("GREATPERSON_ADMIRAL"))
				iGPTemp += (bIsCulture ? 2 : 1) * pEntry->GetGreatPersonRateModifier(iJ) / GC.getNumGreatPersonInfos() / 2;
		}
	}

	iScorePlayer += iGPTemp;

	//////////////////
	//Misc player bonuses.
	///////////////////////

	int iMisc = 0;

	if (pEntry->GetCivilianWorkRate() > 0)
	{
		if (pPlayerTraits->IsExpansionist())
			iMisc += pEntry->GetCivilianWorkRate();
		else if (pPlayerTraits->IsSmaller())
			iMisc += pEntry->GetCivilianWorkRate() / 8;
		else
			iMisc += pEntry->GetCivilianWorkRate() / 2;
	}

	if (pEntry->GetEspionageNetworkPoints() != 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		iMisc += pEntry->GetEspionageNetworkPoints() * max(2, m_pPlayer->GetEspionage()->GetNumSpies() * 6) / 20;
	}

	iMisc += pEntry->GetBorderGrowthRateIncreaseGlobal();

	if (pEntry->GetInquisitorCostModifier() < 0)
	{
		iMisc += iEnemyReligionsNearby * (-1 * pEntry->GetInquisitorCostModifier()) * (bIsWarmonger ? 2 : 1) / 2;
	}

	for (int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
	{
		ResourceTypes eResource = (ResourceTypes)iJ;
		if (pEntry->GetResourceQuantityModifier(iJ) > 0)
		{
			iMisc += m_pPlayer->getNumResourceFromTiles(eResource) * pEntry->GetResourceQuantityModifier(iJ) / 200;
		}
	}

	if (pEntry->GetPlayerCultureModifier() > 0)
	{
		iMisc += 20 * pEntry->GetPlayerCultureModifier() * m_pPlayer->GetYieldRateFromCitiesTimes100(YIELD_CULTURE) * vYieldScores[YIELD_CULTURE] / 10000;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (pEntry->GetYieldPerHolyCityBirth(iI) > 0 && eForeignReligion == NO_RELIGION)
		{
			int iExpectedTurnsToGrow = GetExpectedTurnsToGrow(pHolyCity, eReligion, vOtherPlannedBeliefs);

			iMisc += 10 * min(pEntry->GetCityScalerLimiter(), iNumCitiesWithReligionTotal + iNumNearbyCitiesToSpreadTo) * pEntry->GetYieldPerHolyCityBirth(iI) * iGameSpeedInstantYieldPercent * vYieldScores[iI] / 10000 / iExpectedTurnsToGrow;
		}

		if (pEntry->GetYieldFromKnownPantheons(iI) > 0)
		{
			int iPantheon = GC.getGame().GetGameReligions()->GetNumPantheonsCreated();
			if (bFoundingPantheon)
				iPantheon++;

			if (iPantheon > 8)
			{
				iPantheon = 8;
			}
			// current pantheons
			iMisc += 7 * iPantheon * pEntry->GetYieldFromKnownPantheons(iI) * vYieldScores[iI] / 10000; // GetYieldFromKnownPantheons is Times100
			// future pantheons
			iMisc += 2 * (8 - iPantheon) * pEntry->GetYieldFromKnownPantheons(iI) * vYieldScores[iI] / 10000; // GetYieldFromKnownPantheons is Times100
		}

		if (pEntry->GetYieldFromFaithPurchase(iI) > 0)
		{
			// percent of faith used for purchases is converted
			iMisc += 3 * m_pPlayer->GetTotalFaithPerTurnTimes100() / 100 * pEntry->GetYieldFromFaithPurchase(iI) * vYieldScores[iI] / 10000;
		}

		if (pEntry->GetYieldFromImprovementBuild((YieldTypes)iI, false) > 0 || pEntry->GetYieldFromImprovementBuild((YieldTypes)iI, true) > 0)
		{
			iMisc += (100 * pEntry->GetYieldFromImprovementBuild((YieldTypes)iI, false) + iEraScaleFactorTimes100 * pEntry->GetYieldFromImprovementBuild((YieldTypes)iI, true)) * vYieldScores[iI] / 10000;
		}

		if (pEntry->GetYieldFromPolicyUnlock(iI) > 0)
		{
			int iCulturePerTurnTimes100 = m_pPlayer->GetTotalJONSCulturePerTurnTimes100();
			// check our other beliefs. do they provide extra science?
			for (BeliefList::const_iterator it = vOtherPlannedBeliefs.begin(); it != vOtherPlannedBeliefs.end(); ++it)
			{
				CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
				if (pkBeliefInfo)
				{
					if (pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_CULTURE) > 0)
					{
						int iNumDomesticFollowers = m_pPlayer->GetReligions()->GetNumDomesticFollowers(eReligion);
						int iNumOwnFutureFollowers = 0;
						int iCityLoop = 0;
						for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
						{
							if (pLoopCity == pHolyCity && bFoundingReligion)
							{
								iNumDomesticFollowers += pLoopCity->getPopulation() * 3 / 4;
							}
							else if (pLoopCity->GetCityReligions()->GetReligiousMajority() != eReligion)
								iNumOwnFutureFollowers += pLoopCity->getPopulation() * 3 / 4;
						}
						int iTmp = 100 * iNumDomesticFollowers / pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_CULTURE);
						iTmp += 100 *iNumOwnFutureFollowers * 8 / 10 / (pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_CULTURE));
						if (pEntry->GetMaxYieldPerFollower(YIELD_CULTURE) > 0)
						{
							iTmp = min(iTmp, 100 * pEntry->GetMaxYieldPerFollower(YIELD_CULTURE));
						}
						iCulturePerTurnTimes100 += iTmp;
					}
					if (pkBeliefInfo->GetYieldPerPop(YIELD_CULTURE) > 0)
					{
						int iCityLoop = 0;
						for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
						{
							if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
								iCulturePerTurnTimes100 += 100 * pLoopCity->getPopulation() * pkBeliefInfo->GetYieldPerPop(YIELD_CULTURE);
							else
								iCulturePerTurnTimes100 += 100 * pLoopCity->getPopulation() * pkBeliefInfo->GetYieldPerPop(YIELD_CULTURE) * 8 / 10;
						}
					}
					if (pkBeliefInfo->GetYieldFromTechUnlock(YIELD_CULTURE))
					{
						int iTurnsPerTechUnlock = 10 * GC.getGame().getGameSpeedInfo().getResearchPercent() / 100; // a rough estimation is enough here
						iCulturePerTurnTimes100 += pkBeliefInfo->GetYieldFromTechUnlock(YIELD_CULTURE) * GC.getGame().getGameSpeedInfo().getInstantYieldPercent() / iTurnsPerTechUnlock;
					}
					for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
					{
						if (!pEntry->IsBuildingClassEnabled(iK))
							continue;

						BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iK);
						if (eBuilding == NO_BUILDING)
							continue;

						CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);

						if (pBuildingEntry->IsReformation()) // we can skip those here
							continue;

						if (pBuildingEntry->GetYieldChange(YIELD_CULTURE) > 0)
						{
							int iCityLoop = 0;
							for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
							{
								if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
									iCulturePerTurnTimes100 += 100 * pBuildingEntry->GetYieldChange(YIELD_CULTURE) * 7 / 10;
								else
									iCulturePerTurnTimes100 += 100 * pBuildingEntry->GetYieldChange(YIELD_CULTURE) * 5 / 10;
							}
						}
						if (pBuildingEntry->GetYieldFromWLTKD(YIELD_CULTURE) > 0)
						{
							int iWLTKDAvailability = m_pPlayer->EstimateWLTKDAvailability();
							int iCityLoop = 0;
							for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
							{
								if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
									iCulturePerTurnTimes100 += 7 * iWLTKDAvailability * pLoopCity->getYieldRateTimes100(YIELD_CULTURE) * pBuildingEntry->GetYieldFromWLTKD(YIELD_CULTURE) / 10000;
								else
									iCulturePerTurnTimes100 += 5 * iWLTKDAvailability * pLoopCity->getYieldRateTimes100(YIELD_CULTURE) * pBuildingEntry->GetYieldFromWLTKD(YIELD_CULTURE) / 10000;
							}
						}
					}
				}
			}

			int iTurnsPerPolicyUnlock = iCulturePerTurnTimes100 > 0 ? (100 * m_pPlayer->GetPlayerPolicies()->GetNextPolicyCost() / iCulturePerTurnTimes100) : 50;
			if (bIsCulture)
			{
				iTurnsPerPolicyUnlock *= 2;
				iTurnsPerPolicyUnlock /= 3;
			}
			if (bIsExpansion || bIsWarmonger)
			{
				iTurnsPerPolicyUnlock *= 3;
				iTurnsPerPolicyUnlock /= 2;
			}

			iTurnsPerPolicyUnlock = max(1, iTurnsPerPolicyUnlock);
			if (pPlayerTraits->GetFreeSocialPoliciesPerEra() > 0)
			{
				// assume era change every 50 turns on standard speed
				int iTurnsPerPolicyUnlockFromTraits = (pPlayerTraits->IsOddEraScaler() ? 2 : 1) * 50 * iGameSpeedInstantYieldPercent / 100;
				// combine the turn rates
				iTurnsPerPolicyUnlock = max(1, (iTurnsPerPolicyUnlock * iTurnsPerPolicyUnlockFromTraits) / (iTurnsPerPolicyUnlock + iTurnsPerPolicyUnlockFromTraits));
			}
			if (pPlayerTraits->GetExtraTenetsFirstAdoption() > 0 && m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree() == NO_POLICY_BRANCH_TYPE)
			{
				// only once in the game
				int iTurnsPerPolicyUnlockFromTraits = max(50, 500 - GC.getGame().getGameTurn()) / pPlayerTraits->GetExtraTenetsFirstAdoption() * iGameSpeedInstantYieldPercent / 100;
				// combine the turn rates
				iTurnsPerPolicyUnlock = max(1, (iTurnsPerPolicyUnlock * iTurnsPerPolicyUnlockFromTraits) / (iTurnsPerPolicyUnlock + iTurnsPerPolicyUnlockFromTraits));
			}
			iMisc += 10 * min(pEntry->GetFollowerScalerLimiter(), 2 * iNumCurrentFollowers + iNumNearbyFutureFollowers) * pEntry->GetYieldFromPolicyUnlock(iI) * vYieldScores[iI] / 100 / iTurnsPerPolicyUnlock * iGameSpeedInstantYieldPercent / 100;
		}

		if (pEntry->GetYieldFromTechUnlock((YieldTypes)iI, false) > 0 || pEntry->GetYieldFromTechUnlock((YieldTypes)iI, true) > 0)
		{
			// calculate average turns to research the available techs
			int iAverageTechCost = 0;
			int iNumTechsAvailable = 0;
			vector<TechTypes> dummy;

			for (int i = 0; i < GC.getNumTechInfos(); i++)
			{
				TechTypes eTech = (TechTypes)i;
				if (GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
					continue;

				if (GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasPrereqTechs(eTech, dummy))
				{
					iAverageTechCost += m_pPlayer->GetPlayerTechs()->GetResearchCost(eTech);
					iNumTechsAvailable++;
				}
			}
			if (iNumTechsAvailable > 0)
			{
				iAverageTechCost /= iNumTechsAvailable;

				int iSciencePerTurn = m_pPlayer->GetScience() + pPlayerTraits->GetYieldFromRouteMovement(YIELD_SCIENCE) * m_pPlayer->GetTrade()->GetNumTradeUnits(true);
				// check our other beliefs. do they provide extra science?
				for (BeliefList::const_iterator it = vOtherPlannedBeliefs.begin(); it != vOtherPlannedBeliefs.end(); ++it)
				{
					CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
					if (pkBeliefInfo)
					{
						if (pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_SCIENCE) > 0)
						{
							int iNumDomesticFollowers = m_pPlayer->GetReligions()->GetNumDomesticFollowers(eReligion);
							int iNumOwnFutureFollowers = 0;
							int iCityLoop = 0;
							for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
							{
								if (pLoopCity == pHolyCity && bFoundingReligion)
								{
									iNumDomesticFollowers += pLoopCity->getPopulation() * 3 / 4;
								}
								else if (pLoopCity->GetCityReligions()->GetReligiousMajority() != eReligion)
									iNumOwnFutureFollowers += pLoopCity->getPopulation() * 3 / 4;
							}
							int iTmp = iNumDomesticFollowers / pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_SCIENCE);
							iTmp += iNumOwnFutureFollowers * 8 / 10 / (pkBeliefInfo->GetFollowerRequiredPerYield(YIELD_SCIENCE));
							if (pEntry->GetMaxYieldPerFollower(YIELD_SCIENCE) > 0)
							{
								iTmp = min(iTmp, pEntry->GetMaxYieldPerFollower(YIELD_SCIENCE));
							}
							iSciencePerTurn += iTmp;
						}
						if (pkBeliefInfo->GetYieldPerPop(YIELD_SCIENCE) > 0)
						{
							int iCityLoop = 0;
							for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
							{
								if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
									iSciencePerTurn += pLoopCity->getPopulation() * pkBeliefInfo->GetYieldPerPop(YIELD_SCIENCE);
								else
									iSciencePerTurn += pLoopCity->getPopulation() * pkBeliefInfo->GetYieldPerPop(YIELD_SCIENCE) * 8 / 10;
							}
						}
						if (pkBeliefInfo->GetYieldFromPolicyUnlock(YIELD_SCIENCE))
						{
							int iTurnsPerPolicyUnlock = m_pPlayer->GetTotalJONSCulturePerTurnTimes100() > 0 ? (100 * m_pPlayer->GetPlayerPolicies()->GetNextPolicyCost() / m_pPlayer->GetTotalJONSCulturePerTurnTimes100()) : 50;
							iSciencePerTurn += pkBeliefInfo->GetYieldFromPolicyUnlock(YIELD_SCIENCE) * GC.getGame().getGameSpeedInfo().getInstantYieldPercent() / 100 / iTurnsPerPolicyUnlock;
						}
						for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
						{
							if (!pEntry->IsBuildingClassEnabled(iK))
								continue;

							BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iK);
							if (eBuilding == NO_BUILDING)
								continue;

							CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);

							if (pBuildingEntry->IsReformation()) // we can skip those here
								continue;

							if (pBuildingEntry->GetYieldChange(YIELD_SCIENCE) > 0)
							{
								int iCityLoop = 0;
								for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
								{
									if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
										iSciencePerTurn += pBuildingEntry->GetYieldChange(YIELD_SCIENCE) * 7 / 10;
									else
										iSciencePerTurn += pBuildingEntry->GetYieldChange(YIELD_SCIENCE) * 5 / 10;
								}
							}
							if (pBuildingEntry->GetYieldFromWLTKD(YIELD_SCIENCE) > 0)
							{
								int iWLTKDAvailability = m_pPlayer->EstimateWLTKDAvailability();
								int iCityLoop = 0;
								for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
								{
									if ((pLoopCity == pHolyCity && bFoundingReligion) || pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
										iSciencePerTurn += 7 * iWLTKDAvailability * pLoopCity->getYieldRateTimes100(YIELD_SCIENCE) / 100 * pBuildingEntry->GetYieldFromWLTKD(YIELD_SCIENCE) / 10000;
									else
										iSciencePerTurn += 5 * iWLTKDAvailability * pLoopCity->getYieldRateTimes100(YIELD_SCIENCE) / 100 * pBuildingEntry->GetYieldFromWLTKD(YIELD_SCIENCE) / 10000;
								}
							}
						}
					}
				}

				int iTurnsPerTechUnlockTimes100 = (iSciencePerTurn > 0) ? (100 * iAverageTechCost / iSciencePerTurn) : 99999;
				if (pPlayerTraits->IsNerd())
				{
					iTurnsPerTechUnlockTimes100 *= 2;
					iTurnsPerTechUnlockTimes100 /= 3;
				}
				iTurnsPerTechUnlockTimes100 = max(1, iTurnsPerTechUnlockTimes100);
				iMisc += 10 * min(pEntry->GetFollowerScalerLimiter(), 2 * iNumCurrentFollowers + iNumNearbyFutureFollowers) * (100 * pEntry->GetYieldFromTechUnlock((YieldTypes)iI, false) + iEraScaleFactorTimes100 * pEntry->GetYieldFromTechUnlock((YieldTypes)iI, true)) * vYieldScores[iI] / 100 / iTurnsPerTechUnlockTimes100 * iGameSpeedInstantYieldPercent / 100;
			}
		}

		if (eReligion != NO_RELIGION && pEntry->GetYieldPerOtherReligionFollower(iI) > 0)
		{
			iTemp = 0;
			// current yields from foreign followers
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				iTemp += 10 * pLoopCity->GetCityReligions()->GetFollowersOtherReligions(eReligion, false) / pEntry->GetYieldPerOtherReligionFollower(iI);
			}
			// reduce if we want to spread
			if (bReligionSpreadFocus)
				iTemp /= 2;

			// increase based on the number of other religions nearby, more so if we want to conquer them
			iTemp *= (100 + (bIsWarmonger ? 50 : 15) * iEnemyReligionsNearby);
			iTemp /= 100;

			iTemp *= vYieldScores[iI];
			iTemp /= 100;

			iMisc += iTemp;
		}

		if (pEntry->GetYieldChangePerXCityStateFollowers(iI) > 0)
		{
			iMisc += 15 * (pDiploAI->IsGoingForDiploVictory() ? 2 : 1) * ((eReligion != NO_RELIGION) ? m_pPlayer->GetReligions()->GetNumCityStateFollowers(eReligion) : 0) * vYieldScores[iI] / 100 / pEntry->GetYieldChangePerXCityStateFollowers(iI);

		}

		if (pEntry->GetYieldFromEraUnlock(iI) > 0)
		{
			iMisc += min(pEntry->GetCityScalerLimiter(), iNumCitiesWithReligionTotal + iNumNearbyCitiesToSpreadTo) * pEntry->GetYieldFromEraUnlock(iI) * iEraScaleFactorTimes100 * vYieldScores[iI] / 100000 * iGameSpeedInstantYieldPercent / 100;
		}
		if (pEntry->GetGreatWorkYieldChange(iI) > 0)
		{
			iMisc += (10 * m_pPlayer->GetCulture()->GetNumGreatWorks() + (bIsCulture ? 3 : 1)) * pEntry->GetGreatWorkYieldChange(iI) * vYieldScores[iI] / 100;
		}

		for (int iJ = 0; iJ < NUM_DOMAIN_TYPES; iJ++)
		{
			if (pEntry->GetTradeRouteYieldChange(iJ, iI) > 0)
			{
				// future trade routes or currently unused trade routes: will they be internal or international?
				int iNumFutureOrUnusedTR = ((100 + pPlayerTraits->GetNumTradeRoutesModifier()) / 100) + m_pPlayer->GetTrade()->GetNumTradeRoutesPossible() - m_pPlayer->GetTrade()->GetNumberOfTradeRoutes();
				int iInternalTRPercent = 0;
				if (iNumNeighbors == 0)
					iInternalTRPercent = 100;
				else
				{
					iInternalTRPercent = 50;
					if (bIsCulture)
						iInternalTRPercent -= 25;
					iInternalTRPercent += iNeighborWarmongerThreat * 3;
					iInternalTRPercent = min(iInternalTRPercent, 100);
				}

				if ((YieldTypes)iI == YIELD_PRODUCTION || (YieldTypes)iI == YIELD_FOOD)
				{
					// internal trade routes
					iMisc += (10 * m_pPlayer->GetTrade()->GetNumberOfInternalTradeRoutes() + 3 * iNumFutureOrUnusedTR * iInternalTRPercent / 100) * iEraScaleFactorTimes100 * vYieldScores[iI] / 10000;

				}
				else if ((YieldTypes)iI == YIELD_GOLD || (YieldTypes)iI == YIELD_SCIENCE || (YieldTypes)iI == YIELD_CULTURE)
				{
					// international trade routes
					iMisc += (10 * m_pPlayer->GetTrade()->GetNumberOfInternationalTradeRoutes(true) + 3 * iNumFutureOrUnusedTR * (100 - iInternalTRPercent) / 100) * iEraScaleFactorTimes100 * vYieldScores[iI] / 10000;
				}
			}
		}
	}

	if (pEntry->GetPlotCultureCostModifier() < 0)
	{
		iMisc += (-pEntry->GetPlotCultureCostModifier()) * (iOffensePriority + iDefensePriority) / 2;
	}

	if (pEntry->GetIgnorePolicyRequirementsAmount() > 0)
	{
		iMisc += pEntry->GetIgnorePolicyRequirementsAmount() * 20;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		// here we only calculate potential future trade routes, existing trade routes are evaluated in ScoreBeliefAtCity
		if (pEntry->GetYieldPerActiveTR(YieldTypes(iI)) > 0)
		{
			// this bonus is good also if we don't have any neighbors, as internal TRs give bonuses too (and even in two cities at once). it's only bad if we're surrounded by warmongers as they might plunder our TRs
			iTemp = max(0, 5 - iNeighborWarmongerThreat) * pEntry->GetYieldPerActiveTR(YieldTypes(iI)) * vYieldScores[(YieldTypes)iI] / 100;
			if (bIsWarmonger)
			{
				iTemp /= 2;
			}
			iTemp *= (100 + pPlayerTraits->GetNumTradeRoutesModifier());
			iTemp /= 100;

			iMisc += iTemp;
		}
	}

	iScorePlayer += iMisc;

	if (pBreakdown)
	{
		pBreakdown->iWar = iWarTemp;
		pBreakdown->iDefense = iDefenseTemp;
		pBreakdown->iHappiness = iHappinessTemp;
		pBreakdown->iForeignCity = iForeignCityYields;
		pBreakdown->iPassiveSpread = iPassiveSpreadTemp;
		pBreakdown->iActiveSpread = iActiveSpreadTemp;
		pBreakdown->iBuilding = iBuildingTemp;
		pBreakdown->iDiplo = iDiploTemp;
		pBreakdown->iGreatPerson = iGPTemp;
		pBreakdown->iMisc = iMisc;
	}

	return iScorePlayer;
}

int CvReligionAI::GetValidPlotYieldTimes100(CvBeliefEntry* pEntry, CvPlot* pPlot, YieldTypes iI, bool bConsiderFutureTech) const
{
	TerrainTypes eTerrain = pPlot->getTerrainType();
	FeatureTypes eFeature = pPlot->getFeatureType();
	ResourceTypes eResource = pPlot->getResourceType(m_pPlayer->getTeam());
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	PlotTypes ePlot = pPlot->getPlotType();

	if (pEntry->RequiresNoFeature() && pPlot->isHills())
		return 0;

	if (pEntry->RequiresResource() && eResource == NO_RESOURCE)
		return 0;

	bool bFeatureRemovable = false;
	// how likely is it we want to remove the feature in the future?
	int iFeatureRemoveInFutureLikelihood = 0;
	// performance: check this only if we have any feature-related yields
	if (eFeature != NO_FEATURE && (pEntry->RequiresNoFeature() || pEntry->GetFeatureYieldChange(eFeature, iI) > 0 || pEntry->GetYieldPerXFeatureTimes100(eFeature, iI) > 0))
	{
		for (int iK = 0; iK < GC.getNumBuildInfos(); ++iK)
		{
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iK);
			if (pkBuildInfo && pkBuildInfo->isFeatureRemove(eFeature))
			{
				bFeatureRemovable = true;
				break;
			}
		}

		if (pEntry->RequiresNoFeature() && !bFeatureRemovable)
		{
			// we know we won't get any yields on this plot
			return 0;
		}

		if (bFeatureRemovable && bConsiderFutureTech)
		{
			if (eResource != NO_RESOURCE)
			{
				// is the resource on the plot already connected? don't remove the feature
				if (pPlot->IsResourceLinkedCityActive())
				{
					iFeatureRemoveInFutureLikelihood = 0;
				}
				else
				{
					// will building an improvement to connect the resource remove the feature?
					bool bFeatureRemoveForResource = false;
					ImprovementTypes eFutureImprovementToConnectResource = NO_IMPROVEMENT;
					int iNumImprovementInfos = GC.getNumImprovementInfos();
					for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
					{
						CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
						if (pkImprovementInfo && !pkImprovementInfo->IsCreatedByGreatPerson())
						{
							if (pEntry->RequiresResource() && !pkImprovementInfo->IsConnectsResource(eResource))
								continue;

							if (pkImprovementInfo->IsSpecificCivRequired())
							{
								CivilizationTypes eRequiredCiv = pkImprovementInfo->GetRequiredCivilization();
								if (eRequiredCiv != m_pPlayer->getCivilizationType())
									continue;
							}

							if (pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
							{
								eFutureImprovementToConnectResource = (ImprovementTypes)jJ;
							}
						}
					}
					if (eFutureImprovementToConnectResource != NO_IMPROVEMENT)
					{
						for (int iK2 = 0; iK2 < GC.getNumBuildInfos(); ++iK2)
						{
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iK2);
							if (!pkBuildInfo)
							{
								continue;
							}

							if ((ImprovementTypes)(pkBuildInfo->getImprovement()) == eFutureImprovementToConnectResource)
							{
								bFeatureRemoveForResource = pkBuildInfo->isFeatureRemove(eFeature);
								break;
							}
						}
					}
					if (bFeatureRemoveForResource)
					{
						// there's a resource on the plot and we need to remove the feature to connect it
						iFeatureRemoveInFutureLikelihood = 75;
					}
					else
					{
						// don't need to remove the feature to improve the resource
						iFeatureRemoveInFutureLikelihood = 0;
					}
				}
			}
			else
			{
				// no resource on the plot. might want to remove the feature anyway
				iFeatureRemoveInFutureLikelihood = 25;
				PolicyBranchTypes eTradition = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_TRADITION", true);
				if (m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eTradition))
				{
					// features more likely to be removed by great person improvements
					iFeatureRemoveInFutureLikelihood += 15;
				}
				if (m_pPlayer->GetPlayerTraits()->IsWoodlandMovementBonus() && (eFeature == FEATURE_FOREST || eFeature == FEATURE_JUNGLE))
				{
					iFeatureRemoveInFutureLikelihood -= 20;
				}
				else if (bConsiderFutureTech)
				{
					// can we build a unique improvement here in the future?
					ImprovementTypes eUniqueImprovement = NO_IMPROVEMENT;
					bool bRemoveFeatureForUI = false;
					int iNumImprovementInfos = GC.getNumImprovementInfos();
					for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
					{
						CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
						if (pkImprovementInfo && pkImprovementInfo->IsSpecificCivRequired())
						{
							CivilizationTypes eRequiredCiv = pkImprovementInfo->GetRequiredCivilization();
							if (eRequiredCiv == m_pPlayer->getCivilizationType())
							{
								if (pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
								{
									eUniqueImprovement = (ImprovementTypes)jJ;
									break;

								}
							}
						}
					}
					if (eUniqueImprovement != NO_IMPROVEMENT)
					{
						for (int iK2 = 0; iK2 < GC.getNumBuildInfos(); ++iK2)
						{
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iK2);
							if (!pkBuildInfo)
							{
								continue;
							}

							if ((ImprovementTypes)(pkBuildInfo->getImprovement()) == eUniqueImprovement)
							{
								bRemoveFeatureForUI = pkBuildInfo->isFeatureRemove(eFeature);
								break;
							}
						}
					}
					if (bRemoveFeatureForUI)
					{
						iFeatureRemoveInFutureLikelihood += 50;
					}
				}
			}
		}
	}

	int iRtnValue = 0;
	int iModifier = 0; 
	// iModifier is between 0 and 100. 100 for yields that are instantly available, lower value if it takes time to get them (build improvements, remove features etc.)

	// When RequiresImprovement=1 and no improvement is present, compute a tech-based confidence modifier
	int iRequiresImprovementModifier = 75; // fallback when bConsiderFutureTech=false
	if (pEntry->RequiresImprovement() && eImprovement == NO_IMPROVEMENT && bConsiderFutureTech)
	{
		iRequiresImprovementModifier = 0; // will be raised if any improvement can be built here
		for (int jJ = 0; jJ < GC.getNumImprovementInfos(); jJ++)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
			if (!pkImprovementInfo || pkImprovementInfo->IsCreatedByGreatPerson())
				continue;
			if (pEntry->RequiresResource() && (eResource == NO_RESOURCE || !pkImprovementInfo->IsConnectsResource(eResource)))
				continue;
			if (!pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
				continue;
			if (pkImprovementInfo->IsSpecificCivRequired() && pkImprovementInfo->GetRequiredCivilization() != m_pPlayer->getCivilizationType())
				continue;
			BuildTypes eThisBuild = NO_BUILD;
			for (int iK = 0; iK < GC.getNumBuildInfos(); ++iK)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iK);
				if (pkBuildInfo && (ImprovementTypes)pkBuildInfo->getImprovement() == (ImprovementTypes)jJ)
				{
					eThisBuild = (BuildTypes)iK;
					break;
				}
			}
			if (eThisBuild != NO_BUILD)
			{
				int iModifier;
				TechTypes eBuildTech = (TechTypes)GC.getBuildInfo(eThisBuild)->getTechPrereq();
				// already have the tech or researching it?
				if (eBuildTech == NO_TECH || m_pPlayer->HasTech(eBuildTech) || m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == eBuildTech)
				{
					iModifier = m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == eBuildTech ? 80 : 90;
					// do we have workers to build the improvement?
					int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true);
					iModifier -= min(10, max(0, 10 - iNumWorkers * 5));
					// for currently unowned plots it will take yet a bit longer
					if (pPlot->getOwner() == NO_PLAYER)
						iModifier -= 5;
				}
				else
				{
					iModifier = 50;
				}

				if (eFeature != NO_FEATURE && GC.getBuildInfo(eThisBuild)->isFeatureRemove(eFeature))
				{
					iModifier *= 80;
					iModifier /= 100;
				}

				iRequiresImprovementModifier = max(iRequiresImprovementModifier, iModifier);
			}
		}
	}

	if (eTerrain != NO_TERRAIN)
	{
		int iTerrainYieldChangeTimes100 = pEntry->GetTerrainYieldChange(eTerrain, iI) * 100;
		iTerrainYieldChangeTimes100 += pEntry->GetYieldPerXTerrainTimes100(eTerrain, iI) / 5; // reduced value because usually not all tiles of a given terrain are being worked
		if (iTerrainYieldChangeTimes100 > 0)
		{
			iModifier = 100;
			if (pEntry->RequiresImprovement() && eImprovement == NO_IMPROVEMENT)
			{
				iModifier = iRequiresImprovementModifier;
			}
			else if (pEntry->RequiresNoImprovement() && eImprovement != NO_IMPROVEMENT)
			{
				iModifier = 10; // we don't want to remove existing improvements
			}

			if (eFeature != NO_FEATURE && (pEntry->RequiresNoFeature() || GC.getFeatureInfo(eFeature)->isYieldNotAdditive()))
			{
				iModifier = iFeatureRemoveInFutureLikelihood;
			}

			if ((eTerrain == TERRAIN_DESERT || eTerrain == TERRAIN_TUNDRA) && eFeature == NO_FEATURE && eResource == NO_RESOURCE && !pPlot->isHills())
			{
				// desert and tundra tiles without features, resources or hills are unlikely to be worked
				iModifier = 25;
			}
			iRtnValue += iTerrainYieldChangeTimes100 * iModifier / 100;
		}
	}

	if (ePlot != NO_PLOT)
	{
		int iPlotYieldChangeTimes100 = pEntry->GetPlotYieldChange(ePlot, iI) * 100;
		if (iPlotYieldChangeTimes100 > 0)
		{
			iModifier = 100;
			if (pEntry->RequiresImprovement() && eImprovement == NO_IMPROVEMENT)
			{
				iModifier = iRequiresImprovementModifier;
			}
			else if (pEntry->RequiresNoImprovement() && eImprovement != NO_IMPROVEMENT)
			{
				iModifier = 10; // we don't want to remove existing improvements
			}

			if (pEntry->RequiresNoFeature() && eFeature != NO_FEATURE)
			{
				iModifier *= iFeatureRemoveInFutureLikelihood;
				iModifier /= 100;
			}
			iRtnValue += iPlotYieldChangeTimes100 * iModifier / 100;
		}
	}

	if (eFeature != NO_FEATURE)
	{
		int iFeatureYieldChangeTimes100 = pEntry->GetFeatureYieldChange(eFeature, iI) * 100;
		iFeatureYieldChangeTimes100 += pEntry->GetYieldPerXFeatureTimes100(eFeature, iI) * 3 / 4; // lower value because it's difficult to get many tiles worked

		if (iFeatureYieldChangeTimes100 > 0)
		{
			iFeatureYieldChangeTimes100 *= (100 - iFeatureRemoveInFutureLikelihood);
			iFeatureYieldChangeTimes100 /= 100;
			iRtnValue += iFeatureYieldChangeTimes100;
		}

		if (pPlot->IsNaturalWonder())
		{
			iRtnValue += pEntry->GetYieldChangeNaturalWonder(iI) * 100;
			iRtnValue += pPlot->getYield(iI) * pEntry->GetYieldModifierNaturalWonder(iI);
		}
		else
		{
			if (eImprovement == NO_IMPROVEMENT)
			{
				// lower value because we might want to build an improvement here anyway
				iRtnValue += pEntry->GetUnimprovedFeatureYieldChange(eFeature, iI) * 50;
				iRtnValue += pEntry->GetCityYieldFromUnimprovedFeature(eFeature, iI) * 50;
			}
		}
	}

	//Lake
	if (pPlot->isLake())
	{
		iRtnValue += pEntry->GetLakePlotYieldChange(iI) * 100;
	}

	// Resource
	if (eResource != NO_RESOURCE)
	{
		iRtnValue += pEntry->GetResourceYieldChange(eResource, iI) * 100;
		if (pEntry->GetResourceQuantityModifier(eResource) > 0)
		{
			iRtnValue += ((pPlot->getNumResource() * pEntry->GetResourceQuantityModifier(eResource)) * 5);
		}
	}

	// Improvement
	if (bConsiderFutureTech)
	{
		//look at what could be build there
		int iBestImprovementValue = 0;
		int iNumImprovementInfos = GC.getNumImprovementInfos();
		for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)jJ);
			if (pkImprovementInfo)
			{
				if (pEntry->RequiresResource() && (eResource == NO_RESOURCE || !pkImprovementInfo->IsConnectsResource(eResource)))
					continue;

				int iImprovementChange = pEntry->GetImprovementYieldChange((ImprovementTypes)jJ, iI);
				if (iImprovementChange > 0)
				{
					// modify value based on how much time we still need to build the improvement
					iModifier = 0;
					if (pPlot->HasImprovement((ImprovementTypes)jJ))
					{
						// already have the improvement
						iModifier = 100;
					}
					else if (!pkImprovementInfo->IsCreatedByGreatPerson() && pPlot->canHaveImprovement((ImprovementTypes)jJ, m_pPlayer->GetID()))
					{
						// great person improvements can't be built often, ignore them as potential improvements
						if (pkImprovementInfo->IsSpecificCivRequired())
						{
							CivilizationTypes eRequiredCiv = pkImprovementInfo->GetRequiredCivilization();
							if (eRequiredCiv != m_pPlayer->getCivilizationType())
								continue;
						}

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
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eThisBuild);
							TechTypes eBuildTech = (TechTypes)GC.getBuildInfo(eThisBuild)->getTechPrereq();
							// already have the tech or researching it?
							if (eBuildTech == NO_TECH || m_pPlayer->HasTech(eBuildTech) || m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == eBuildTech)
							{
								iModifier = m_pPlayer->GetPlayerTechs()->GetCurrentResearch() == eBuildTech ? 80 : 90;
								// do we have workers to build the improvement?
								int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true);
								iModifier -= min(10, max(0, 10 - iNumWorkers * 5));
								// for currently unowned plots it will take yet a bit longer
								if (pPlot->getOwner() == NO_PLAYER)
									iModifier -= 5;
							}
							else
							{
								iModifier = 50;
							}

							if (eFeature != NO_FEATURE && pkBuildInfo->isFeatureRemove(eFeature))
							{
								iModifier *= 80;
								iModifier /= 100;
							}
						}
					}
					iBestImprovementValue = max(iBestImprovementValue, iImprovementChange * iModifier);
				}
			}
		}
		iRtnValue += iBestImprovementValue;
	}
	else
	{
		//only look at the current improvement
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iRtnValue += (pEntry->GetImprovementYieldChange(pPlot->getImprovementType(), iI)) * 100;
		}
	}

	return iRtnValue;
}
/// AI's evaluation of a certain yield
int CvReligionAI::ScoreYieldForReligionTimes100(YieldTypes eYield, BeliefList& vOurReligionBeliefs, bool bFaithFocus, bool bFoundingPantheon) const
{
	int iValue = 0;
	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();
	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	switch (eYield)
	{
	case YIELD_FOOD:
	{
		iValue += 125 + pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH")) * 5;
		// higher value if going for culture victory
		// if founding a pantheon it's still early in the game, don't put to much weight on diplo AI evaluations
		if (pDiploAI->IsGoingForCultureVictory())
		{
			iValue += 20 / (bFoundingPantheon ? 2 : 1);
		}
		// lower value if going for domination
		if (pDiploAI->IsGoingForWorldConquest())
		{
			iValue -= 50 / (bFoundingPantheon ? 2 : 1);
		}
		// lower value if unhappy
		if (m_pPlayer->IsEmpireUnhappy())
			iValue -= 50;
		if (m_pPlayer->IsEmpireVeryUnhappy())
			iValue -= 50;
		if (m_pPlayer->IsEmpireSuperUnhappy())
			iValue -= 50;

		if (m_pPlayer->IsEmpireVeryHappy())
			iValue += 50;


		// food decreases in value as the game progresses
		iValue *= max(25, 100 - 200 * m_pPlayer->GetCurrentEra() / GC.getNumEraInfos());
		iValue /= 100;


		bool bYieldsFromBirth = false;
		for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
		{
			CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
			if (pkBeliefInfo)
			{
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					if (pkBeliefInfo->GetYieldPerBirth((YieldTypes)iI) > 0 || pkBeliefInfo->GetYieldPerHolyCityBirth((YieldTypes)iI) > 0)
					{
						bYieldsFromBirth = true;
						break;
					}
				}
			}
		}
		if (bYieldsFromBirth)
			iValue += 25;

		iValue = max(iValue, 10);
		break;
	}
	case YIELD_PRODUCTION:
	{
		iValue = 175 + pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION")) * 5;
		if (pDiploAI->IsGoingForWorldConquest())
			iValue += 40 / (bFoundingPantheon ? 2 : 1);
		if (pDiploAI->IsGoingForDiploVictory())
			iValue += 40 / (bFoundingPantheon ? 2 : 1);
		if (pPlayerTraits->GetMinorInfluencePerGiftedUnit())
			iValue += 25;
		break;
	}
	case YIELD_GOLD:
	{
		iValue = MOD_BALANCE_VP ? 75 : 150;
		iValue += pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD")) * 5;
		if (pDiploAI->IsGoingForDiploVictory())
			iValue += 50 / (bFoundingPantheon ? 2 : 1);
		if (pPlayerTraits->GetMinorInfluencePerGiftedUnit())
			iValue += 20;
		//emphasize gold if we're in the red
		int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
		if (iGPT < -1)
			iValue += (int)(sqrt((float)-iGPT) * (MOD_BALANCE_VP ? 10 : 25));
		break;
	}
	case YIELD_SCIENCE:
	{
		iValue = 200;
		iValue += pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE")) * 5;
		if (pDiploAI->IsGoingForSpaceshipVictory())
			iValue += 100 / (bFoundingPantheon ? 2 : 1);
		if (pDiploAI->IsGoingForWorldConquest())
			iValue += 50 / (bFoundingPantheon ? 2 : 1);
		bool bYieldsFromTechUnlock = false;
		for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
		{
			CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
			if (pkBeliefInfo)
			{
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					if (pkBeliefInfo->GetYieldFromTechUnlock((YieldTypes)iI, false) > 0 || pkBeliefInfo->GetYieldFromTechUnlock((YieldTypes)iI, true) > 0)
					{
						bYieldsFromTechUnlock = true;
						break;
					}
				}
			}
		}
		if (bYieldsFromTechUnlock)
			iValue += 25;

		break;
	}
	case YIELD_CULTURE:
	{
		iValue = MOD_BALANCE_VP ? 225 : 175;
		iValue += pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE")) * 5;
		if (pDiploAI->IsGoingForCultureVictory())
			iValue += 50 / (bFoundingPantheon ? 2 : 1);
		if (pDiploAI->IsGoingForWorldConquest())
			iValue -= 25 / (bFoundingPantheon ? 2 : 1);

		bool bYieldsFromPolicyUnlock = false;
		for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
		{
			CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
			if (pkBeliefInfo)
			{
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					if (pkBeliefInfo->GetYieldFromPolicyUnlock((YieldTypes)iI) > 0)
					{
						bYieldsFromPolicyUnlock = true;
						break;
					}
				}
			}
		}
		if (bYieldsFromPolicyUnlock)
			iValue += 25;

		break;
	}
	case YIELD_FAITH:
	{
		if (m_pPlayer->GetReligions()->GetStateReligion(true) == NO_RELIGION && m_pPlayer->GetReligions()->GetFoundingReligionCityID() == -1)
		{
			// founding a pantheon. Unless our trait allows us to always found, faith is very high-value
			iValue = pPlayerTraits->IsAlwaysReligion() ? 500 : 1000;
		}
		else
		{
			iValue = 175;
			iValue += pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION")) * 5;
			iValue += bFaithFocus ? 50 : 0;
		}


		break;
	}
	case YIELD_TOURISM:
	{
		return pDiploAI->IsGoingForCultureVictory() ? 400 : 40;
	}
	case YIELD_GOLDEN_AGE_POINTS:
	{
		iValue = 15;
		iValue += pPlayerTraits->GetGoldenAgeCombatModifier();

		iValue *= 100 + pPlayerTraits->GetGoldenAgeDurationModifier();
		iValue /= 100;

		bool bExtraYieldsWhenInGoldenAge= false;
		for (BeliefList::iterator it = vOurReligionBeliefs.begin(); it != vOurReligionBeliefs.end(); ++it)
		{
			CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
			if (pkBeliefInfo)
			{
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					if (pkBeliefInfo->GetYieldBonusGoldenAge((YieldTypes)iI) > 0)
					{
						bExtraYieldsWhenInGoldenAge = true;
						break;
					}
				}
			}
		}
		if (bExtraYieldsWhenInGoldenAge)
			iValue += 50;

		break;
	}
	default:
		iValue = 200;
	}
	return max(10, iValue);
}

/// AI's evaluation of this belief's usefulness at this one plot
int CvReligionAI::ScoreBeliefAtPlotTimes100(CvBeliefEntry* pEntry, CvPlot* pPlot, bool bConsiderFutureTech, vector<int>& vYieldScores) const
{
	int iRtnValue = 0;
	int iTotalRtnValue = 0;

	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (iI > YIELD_FAITH)
			continue;

		iRtnValue = GetValidPlotYieldTimes100(pEntry, pPlot, (YieldTypes)iI, bConsiderFutureTech);
		if (iRtnValue <= 0)
			continue;

		iTotalRtnValue += iRtnValue * vYieldScores[iI] / 100;
	}

	return iTotalRtnValue;
}

/// Returns an availability modifier between 1 and 10 for eTech, based on how long we'd need to research it
int CvReligionAI::GetTechAvailabilityModifier(TechTypes eTech, bool bPotentialCity) const
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if (!pkTechInfo)
		return 0;

	// more than one era away? we won't get this quickly
	if (pkTechInfo->GetEra() - m_pPlayer->GetCurrentEra() > 1)
		return 1;

	CvTeamTechs* pTeamTechs = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs();

	set<TechTypes> requiredTechs = pTeamTechs->GetTechsToResearchFor(eTech, 23);

	int iBeakersLeft = 0;
	for (set<TechTypes>::const_iterator it = requiredTechs.begin(); it != requiredTechs.end(); ++it)
		iBeakersLeft += pTeamTechs->GetResearchLeftTimes100(*it);

	int iScienceRate = max(1, m_pPlayer->GetScienceTimes100());
	int iTurnsNeeded = iBeakersLeft / iScienceRate;

	int iAvailabilityModifier = 7 - iTurnsNeeded / 10;  // lose remaining value the longer research will take

	// if we're evaluating this for a potential city, we will have made progress to the tech by the time we've founded the city
	// scores for potential cities are already reduced, don't double-penalize
	if (bPotentialCity)
		iAvailabilityModifier++;

	return max(1, iAvailabilityModifier);
}

/// Expected number of turns until pCity (or a potential new city, if pCity is NULL) grows by one population
int CvReligionAI::GetExpectedTurnsToGrow(CvCity* pCity, ReligionTypes eReligion, const BeliefList& vOtherPlannedBeliefs) const
{
	int iExpectedTurnsToGrow = 20;
	if (pCity)
	{
		int iExcessFoodPerTurn = pCity->getYieldRateTimes100(YIELD_FOOD, false, true);
		// do our other beliefs provide extra food?
		for (BeliefList::const_iterator it = vOtherPlannedBeliefs.begin(); it != vOtherPlannedBeliefs.end(); ++it)
		{
			CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
			if (pkBeliefInfo)
			{
				// use simplified calculations here
				if (pkBeliefInfo->GetYieldPerXFollowers(YIELD_FOOD) > 0)
				{
					iExcessFoodPerTurn += 100 * pCity->getPopulation() / pkBeliefInfo->GetYieldPerXFollowers(YIELD_FOOD);
				}
				if (pkBeliefInfo->GetYieldChangeAnySpecialist(YIELD_FOOD) > 0)
				{
					iExcessFoodPerTurn += 100 * pkBeliefInfo->GetYieldChangeAnySpecialist(YIELD_FOOD) * (pCity->GetCityCitizens()->GetTotalSpecialistCount() > 0 ? 10 : 7) / 10;
				}
				for (int iJ = 0; iJ < NUM_DOMAIN_TYPES; iJ++)
				{
					if (pkBeliefInfo->GetTradeRouteYieldChange(iJ, YIELD_FOOD) > 0)
					{
						int iNumIncomingFoodTradeRoutes = 0;
						CvGameTrade* pTrade = GC.getGame().GetGameTrade();
						const std::vector<int>& vConnections = pTrade->GetTradeConnectionsForPlayer(pCity->getOwner());
						for (uint ui = 0; ui < vConnections.size(); ui++)
						{
							if (pTrade->IsTradeRouteIndexEmpty(vConnections[ui]))
								continue;

							const TradeConnection& kConnection = pTrade->GetTradeConnection(vConnections[ui]);
							if (kConnection.m_eConnectionType == TRADE_CONNECTION_FOOD && kConnection.m_eDomain == (DomainTypes)iJ &&
								kConnection.m_iDestX == pCity->getX() && kConnection.m_iDestY == pCity->getY())
							{
								iNumIncomingFoodTradeRoutes++;
							}
						}

						iExcessFoodPerTurn += 100 * max(1, (int)GET_PLAYER(pCity->getOwner()).GetCurrentEra()) * pkBeliefInfo->GetTradeRouteYieldChange(iJ, YIELD_FOOD) * iNumIncomingFoodTradeRoutes;
					}
				}
				for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
				{
					if (!pkBeliefInfo->IsBuildingClassEnabled(iK))
						continue;

					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iK);
					if (eBuilding == NO_BUILDING)
						continue;

					CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);

					if (pBuildingEntry->IsReformation()) // skip these here
						continue;

					if (pBuildingEntry->GetYieldChange(YIELD_FOOD) > 0)
					{
						iExcessFoodPerTurn += 50 * pBuildingEntry->GetYieldChange(YIELD_FOOD);
					}
					if (pBuildingEntry->GetYieldModifier(YIELD_FOOD) > 0)
					{
						iExcessFoodPerTurn += 75 * pCity->getYieldRateTimes100(YIELD_FOOD) * pBuildingEntry->GetYieldModifier(YIELD_FOOD) / 10000;
					}
				}
			}
		}
		if (iExcessFoodPerTurn > 0)
		{
			iExpectedTurnsToGrow = 100 * pCity->growthThreshold() / iExcessFoodPerTurn;

			// growth is expected to become less frequent in the future
			iExpectedTurnsToGrow *= 5;
			iExpectedTurnsToGrow /= 3;

			// reduce based on the number of cities we have
			iExpectedTurnsToGrow *= 100;
			iExpectedTurnsToGrow /= max(50, 100 - 2 * m_pPlayer->getNumCities());
		}
		else
		{
			iExpectedTurnsToGrow = 99 * GC.getGame().getGameSpeedInfo().getGrowthPercent() / 100;
		}
	}
	if (m_pPlayer->GetPlayerTraits()->IsPopulationBoostReligion() && eReligion <= RELIGION_PANTHEON)
	{
		iExpectedTurnsToGrow *= 3;
		iExpectedTurnsToGrow /= 4;
	}
	if (pCity)
	{
		iExpectedTurnsToGrow *= (100 - min(99, pCity->getMaxFoodKeptPercent()));
		iExpectedTurnsToGrow /= 100;
	}

	iExpectedTurnsToGrow = max(15 * GC.getGame().getGameSpeedInfo().getGrowthPercent() / 100, iExpectedTurnsToGrow);

	return iExpectedTurnsToGrow;
}

/// Expected number of turns per border growth of pCity (or a potential new city, if pCity is null)
int CvReligionAI::GetExpectedTurnsPerBorderGrowthTimes100(CvCity* pCity, const CvBeliefEntry* pEntry) const
{
	// Future modifier from policies? Only check policies from the branches we have unlocked
	int iFutureBGRate = 0;
	int iFutureBGModifier = 0;
	int iCultureFromKills = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_KILLS) + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 3;
	for (int iPoliciesLoop = 0; iPoliciesLoop < GC.getNumPolicyInfos(); iPoliciesLoop++)
	{
		PolicyTypes ePolicy = (PolicyTypes)iPoliciesLoop;
		if (m_pPlayer->GetPlayerPolicies()->HasPolicy(ePolicy))
			continue;

		CvPolicyEntry* pkPolicyEntry = GC.getPolicyInfo(ePolicy);
		if (pkPolicyEntry == NULL)
			continue;

		PolicyBranchTypes eBranch = (PolicyBranchTypes)pkPolicyEntry->GetPolicyBranchType();
		if (eBranch != NO_POLICY_BRANCH_TYPE && m_pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
		{
			// building yields
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				if (pkPolicyEntry->GetFreeChosenBuilding(iBuildingClassLoop) > 0)
				{
					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iBuildingClassLoop);
					if (eBuilding != NO_BUILDING)
					{
						iFutureBGModifier += (GC.getBuildingInfo(eBuilding)->IsCapitalOnly() && (!pCity || !pCity->isCapital())) ? 0 : (GC.getBuildingInfo(eBuilding)->GetYieldModifier(YIELD_CULTURE_LOCAL));
						if (pCity)
						{
							for (int iBuildingClassLoop2 = 0; iBuildingClassLoop2 < GC.getNumBuildingClassInfos(); iBuildingClassLoop2++)
							{
								int iYieldChange = GC.getBuildingInfo(eBuilding)->GetBuildingClassYieldChange(iBuildingClassLoop2, YIELD_CULTURE) + GC.getBuildingInfo(eBuilding)->GetBuildingClassYieldChange(iBuildingClassLoop2, YIELD_CULTURE_LOCAL);
								if (iYieldChange > 0)
								{
									BuildingTypes eBuilding2 = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iBuildingClassLoop2);
									if (pCity->HasBuilding(eBuilding2) || pCity->canConstruct(eBuilding2))
									{
										iFutureBGRate += iYieldChange;
									}
								}
							}
						}
					}
				}
			}
			// resource yields
			if (pCity)
			{
				for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					if (pkPolicyEntry->GetResourceYieldChanges(iResourceLoop, YIELD_CULTURE) > 0 || pkPolicyEntry->GetResourceYieldChanges(iResourceLoop, YIELD_CULTURE_LOCAL) > 0)
					{
						iFutureBGRate += (pkPolicyEntry->GetResourceYieldChanges(iResourceLoop, YIELD_CULTURE) + pkPolicyEntry->GetResourceYieldChanges(iResourceLoop, YIELD_CULTURE_LOCAL)) * (pCity->GetNumResourceLocal((ResourceTypes)iResourceLoop, true) + pCity->GetNumResourceLocal((ResourceTypes)iResourceLoop, false));
					}
				}
			}
			// city strength
			iFutureBGRate += pkPolicyEntry->GetYieldPerCityOverStrengthThreshold(YIELD_CULTURE) + pkPolicyEntry->GetYieldPerCityOverStrengthThreshold(YIELD_CULTURE_LOCAL);
			// culture from kills
			iCultureFromKills += (pkPolicyEntry->GetCultureFromKills() * 3 / 4 + pkPolicyEntry->GetCultureFromBarbarianKills() / 4);
		}
	}

	int iTurnsPerBorderGrowthTimes100 = 0;
	if (pCity)
	{
		int iBorderGrowthRate = pCity->getYieldRateTimes100(YIELD_CULTURE_LOCAL) + iFutureBGRate * 100 * 3 / 4;
		// assume yields from kills go to the capital
		if (pCity->isCapital() && iCultureFromKills > 0)
		{
			iBorderGrowthRate += iCultureFromKills * m_pPlayer->getNumMilitaryUnits() / 15;
		}
		// average culture threshold for the next three levels
		int iCultureNeededForNextTile = (pCity->GetJONSCultureThreshold() + pCity->GetJONSCultureThreshold(1) + pCity->GetJONSCultureThreshold(2)) * 100 / 3;
		iTurnsPerBorderGrowthTimes100 = 100 * iCultureNeededForNextTile / max(1, iBorderGrowthRate);
	}
	else
	{
		iTurnsPerBorderGrowthTimes100 = max(100, (GD_INT_GET(CULTURE_COST_FIRST_PLOT) - iFutureBGRate) * GC.getGame().getGameSpeedInfo().getCulturePercent());
		int iModifier = m_pPlayer->GetPlotCultureCostModifier();
		if (iModifier != 0)
		{
			iModifier = max(iModifier, /*-85*/ GD_INT_GET(CULTURE_PLOT_COST_MOD_MINIMUM));	// value cannot reduced by more than 85%
			iTurnsPerBorderGrowthTimes100 *= 100;
			iTurnsPerBorderGrowthTimes100 /= (100 + iModifier);
		}
	}

	iTurnsPerBorderGrowthTimes100 *= 100;
	iTurnsPerBorderGrowthTimes100 /= (100 + pEntry->GetBorderGrowthRateIncreaseGlobal() + iFutureBGModifier * 3 / 4);

	return iTurnsPerBorderGrowthTimes100;
}

/// AI's evaluation of this belief's usefulness at this city (or at a potential new city, if pCity is NULL)

int CvReligionAI::ScoreBeliefAtCity(CvBeliefEntry* pEntry, CvCity* pCity, ReligionTypes eForeignReligion, vector<int>& vYieldScores, const ScoreBeliefContext& kContext) const
{
	if (m_pPlayer->getCapitalCity() == NULL)
		return 0;

	ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion(true);
	if (eForeignReligion != NO_RELIGION)
		eReligion = eForeignReligion;

	// Consistent with the other belief scoring functions, if a belief provides +1 [YIELD_TYPE] per turn in a city, it is scored as iAvailabilityModifier * vYieldScores(YIELD_TYPE) / 100.
	// iAvailabilityModifier is 10 if the yield is given immediately upon adopting the belief, and lower otherwise.

	int iAvailabilityModifier = 0;

	int iRtnValue = 0;

	int iHappinessValue = m_pPlayer->GetHappinessValueTimes100();
	
	// if the yields given by a belief scale with era, we add 0.25 of the yield for every future era yet to come. That way era scaling is taken into account but isn't overvalued
	int iEraScaleFactorTimes100 = 100 * max(1, (int)m_pPlayer->GetCurrentEra()) + 25 * (GC.getNumEraInfos() - m_pPlayer->GetCurrentEra() - 1);

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();

	bool bIsCapital = pCity && pCity->isCapital();

	CvPlayerTraits* pPlayerTraits = m_pPlayer->GetPlayerTraits();

	//let's establish some mid-game goals for the AI.
	int iCurrentCityPop = pCity ? pCity->getPopulation() : 1;

	int iExpectedTurnsToGrow = GetExpectedTurnsToGrow(pCity, eReligion, kContext.vOtherPlannedBeliefs);

	int iExpectedGrowth = min(10, 50 / iExpectedTurnsToGrow);

	// modifier if a belief has a population requirement
	int iMinPopulationModifier = 100;
	if (pEntry->GetMinPopulation() > 0)
	{
		if (iCurrentCityPop < pEntry->GetMinPopulation())
		{
			iMinPopulationModifier = 100 - 10 * (pEntry->GetMinPopulation() - iCurrentCityPop) - (pCity ? 0 : 5);
		}
	}

	// We use this scaling factor for yield modifiers given by the belief to take into account that city yields increase over time
	int iYieldModEraScaleFactorTimes100 = (100 * ((int)m_pPlayer->GetCurrentEra() + 1) + 50 * (GC.getNumEraInfos() - m_pPlayer->GetCurrentEra())) / ((int)m_pPlayer->GetCurrentEra() + 1);

	bool bIsHolyCity = pCity && pCity->GetCityReligions()->IsHolyCityForReligion(eReligion);
	if (pCity && !bIsHolyCity && pCity->GetID() == m_pPlayer->GetReligions()->GetFoundingReligionCityID())
	{
		// founding a religion here
		bIsHolyCity = true;
	}

	int iCurrentFollowers = 0;
	int iExpectedAdditionalFollowers = 0;
	bool bFollowingReligion = false;

	if (eReligion == NO_RELIGION)
	{
		// founding a pantheon, all cities get initial followers
		iCurrentFollowers = max(1, iCurrentCityPop * 3 / 4);
		bFollowingReligion = true;
		iExpectedAdditionalFollowers = iExpectedGrowth * 3 / 4;
	}
	else if (eForeignReligion == NO_RELIGION && eReligion == RELIGION_PANTHEON)
	{
		// founding a religion. the new holy city gets initial followers, all others do not
		iCurrentFollowers = bIsHolyCity ? (iCurrentCityPop * 3 / 4) : 0;
		bFollowingReligion = bIsHolyCity;
		iExpectedAdditionalFollowers = max(1, iCurrentCityPop + iExpectedGrowth * 3 / 4 - iCurrentFollowers);
	}
	else
	{
		// enhancing an existing religion or evaluating a foreign religion for our cities
		iCurrentFollowers = pCity ? pCity->GetCityReligions()->GetNumFollowers(eReligion) : 0;
		bFollowingReligion = pCity && pCity->GetCityReligions()->GetReligiousMajority() == eReligion;
		iExpectedAdditionalFollowers = max(1, iCurrentCityPop + iExpectedGrowth * 3 / 4 - iCurrentFollowers);
	}

	// value of Great Person Points
	int iGPValueTimes100 = 100 * pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GREAT_PEOPLE")) / 6;

	// Beliefs giving production modifiers to unit combat classes. This cannot be evaluated as AvailabilityModifier * YieldScore / 100, so a flat score is given
	for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
		if (pEntry->GetUnitCombatProductionModifiers(eUnitCombatClass) > 0)
		{
			if (eUnitCombatClass == GC.getInfoTypeForString("UNITCOMBAT_DIPLOMACY"))
			{
				if (m_pPlayer->GetPlayerTraits()->IsDiplomat())
					iRtnValue += pEntry->GetUnitCombatProductionModifiers(eUnitCombatClass);
				else
					iRtnValue += pEntry->GetUnitCombatProductionModifiers(eUnitCombatClass) / 2;
			}
			// assume military
			else
			{
				if (m_pPlayer->GetPlayerTraits()->IsWarmonger() || m_pPlayer->GetPlayerTraits()->IsExpansionist())
					iRtnValue += pEntry->GetUnitCombatProductionModifiers(eUnitCombatClass);
				else
					iRtnValue += pEntry->GetUnitCombatProductionModifiers(eUnitCombatClass) / 2;
			}
		}
	}

	////////////////////
	// Happiness
	///////////////////

	// No happiness is given in puppets, unless we're Venice
	if ((pCity && !pCity->IsPuppet()) || !pPlayerTraits->IsNoAnnexing())
	{
		// River happiness
		if (pEntry->GetRiverHappiness() > 0)
		{
			if (pCity)
			{
				iAvailabilityModifier = pCity->plot()->isRiver() ? 10 : 0;
			}
			else
			{
				// not all cities we'll found will be at a river
				iAvailabilityModifier = 3;
			}
			iRtnValue += iAvailabilityModifier * pEntry->GetRiverHappiness() * iHappinessValue / 100;
		}

		// Happiness per city
		if (pEntry->GetHappinessPerCity() > 0)
		{
			iRtnValue += 10 * pEntry->GetHappinessPerCity() * iHappinessValue * iMinPopulationModifier / 10000;
		}

		// Building class happiness
		for (int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			if (pEntry->GetBuildingClassHappiness(jJ) > 0)
			{
				BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings((BuildingClassTypes)jJ);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				bool bCapitalOnly = pkBuildingInfo->IsCapital() || ::isNationalWonderClass(pkBuildingInfo->GetBuildingClassInfo()) || pkBuildingInfo->IsCapitalOnly();

				if (pCity && pCity->GetCityBuildings()->HasBuildingClass((BuildingClassTypes)jJ))
				{
					iAvailabilityModifier = bCapitalOnly ? 8 : 10; // lower value for buildings only in the capital because there is no scaling potential at all
				}
				else if (pCity && pCity->canConstruct(eBuilding))
				{
					iAvailabilityModifier = 8;
				}
				else
				{
					if (bCapitalOnly && !bIsCapital)
					{
						iAvailabilityModifier = 0;
					}
					else if (pkBuildingInfo->GetLocalResourceOrSize() > 0)
					{
						// we need a local resource to build this? assume the building will be very rare
						iAvailabilityModifier = 1;
					}
					else
					{
						TechTypes ePrereqTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();

						if (ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
						{
							// we have the tech to build this. if pCity != NULL then canConstruct above failed for other reasons, assume a lower availability.
							// if pCity == NULL and we're evaluating this for a potential city, assume we can construct it immediately
							iAvailabilityModifier = pCity ? 4 : 8;
						}
						else
						{
							iAvailabilityModifier = GetTechAvailabilityModifier(ePrereqTech, pCity == NULL);
						}

						// for defense buildings check if we need them
						if (pkBuildingInfo->GetDefenseModifier() > 0)
						{
							iAvailabilityModifier += kContext.iDefensePriority / 5 - 2;
						}

						if (!pCity)
						{
							// reduce availability for all buildings in potential cities as they have low production and need to build many things at the beginning
							iAvailabilityModifier--;
						}

						// unique building, assume we focus on getting it quickly
						if (m_pPlayer->getCivilizationInfo().isCivilizationBuildingOverridden(pkBuildingInfo->GetBuildingClassType()))
						{
							iAvailabilityModifier += 1;
						}
						iAvailabilityModifier = max(1, min(10, iAvailabilityModifier));
					}
				}
				iRtnValue += iAvailabilityModifier * pEntry->GetBuildingClassHappiness(jJ) * iHappinessValue / 100;
			}
		}
		if (pEntry->GetHappinessPerFollowingCity() > 0)
		{
			int iAvailabilityModifier = 0;
			if (pCity)
			{
				iAvailabilityModifier = bFollowingReligion ? 10 : 7;
			}
			else
			{
				iAvailabilityModifier = 3;
			}
			iRtnValue += (int)(pEntry->GetHappinessPerFollowingCity() * iAvailabilityModifier * iHappinessValue / 100);
		}
		if (pEntry->GetFullyConvertedHappiness() > 0)
		{
			// this is difficult to achieve, don't assume we'll get fully converted cities other than the ones we already have
			if (eReligion != NO_RELIGION && pCity && pCity->GetCityReligions()->GetFollowersOtherReligions(eReligion) <= 0)
			{
				iRtnValue += 8 * pEntry->GetFullyConvertedHappiness() * iHappinessValue / 100;
			}
		}
	}

	////////////////////
	// Growth
	///////////////////

	if (pEntry->GetCityGrowthModifier() > 0)
	{
		if (pCity)
		{
			int iExcessFood = (pCity ? pCity->getYieldRateTimes100(YIELD_FOOD, false, true) : 1000) / 100;
			iRtnValue += 10 * iYieldModEraScaleFactorTimes100 * iExcessFood * pEntry->GetCityGrowthModifier() / 10000 * vYieldScores[YIELD_FOOD] / 100;
		}
	}

	////////////////////
	// Great People
	///////////////////

	if ((pEntry->IsPantheonBelief() && bIsCapital && eForeignReligion == NO_RELIGION) || bIsHolyCity)
	{
		for (int jJ = 0; jJ < GC.getNumGreatPersonInfos(); jJ++)
		{
			GreatPersonTypes eGP = (GreatPersonTypes)jJ;
			if (eGP == NO_GREATPERSON)
				continue;

			if (pEntry->GetGreatPersonPoints(eGP) > 0)
			{
				int iTmp = 10 * pEntry->GetGreatPersonPoints(eGP) * iGPValueTimes100 / 100;
				
				int iMod = m_pPlayer->getGreatPeopleRateModifier() + m_pPlayer->GetGreatPersonRateModifier(eGP);
				int iNumPuppets = m_pPlayer->GetNumPuppetCities();
				if (iNumPuppets > 0)
				{
					iMod += iNumPuppets * m_pPlayer->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGP);
				}
				iMod += m_pPlayer->getSpecificGreatPersonRateModifierFromMonopoly(eGP);
				iMod += pCity ? pCity->getGreatPeopleRateModifier() : 0;

				iRtnValue += iTmp * (100 + iMod) / 100;
			}
		}
	}


	////////////////////
	// Yield Changes
	///////////////////

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		// the TempValue variables will contain the values (AvailabiltyModifier * YieldChange) for the different belief effects.
		// They will be multiplied with the yield scores and added to the total score below

		int iTempValue = 0; // for yields per turn
		int iTempValueInstant = 0; // for instant yields
		int iTempValueYieldMod = 0; // for yield modifiers
		int iTempValueCapital = 0; // for yields per turn in the capital

		// City yield change
		iTempValue += 10 * pEntry->GetCityYieldChange(iI) * iMinPopulationModifier / 100;

		if (bIsCapital)
		{
			iTempValue += 10 * pEntry->GetCapitalYieldChange(iI) * iMinPopulationModifier / 100;
		}
		if (bIsHolyCity)
		{
			iTempValue += 10 * pEntry->GetHolyCityYieldChange(iI) * iMinPopulationModifier / 100;
		}

		if (pEntry->GetYieldPerPop(iI) > 0)
		{
			// population we have
			iTempValue += 10 * iCurrentCityPop / pEntry->GetYieldPerPop(iI);
			// additional population we expect to get in the near future
			iTempValue += 5 * iExpectedGrowth / pEntry->GetYieldPerPop(iI);
		}
		if (pEntry->GetYieldPerXFollowers(iI) > 0)
		{
			// these yields are given in the capital
			// followers we have
			iTempValueCapital += 10 * iCurrentFollowers / pEntry->GetYieldPerXFollowers(iI);
			// additional followers we expect to get in the near future
			iTempValueCapital += 8 * iExpectedAdditionalFollowers / pEntry->GetYieldPerXFollowers(iI);
		}

		// yield per birth
		if (pEntry->GetYieldPerBirth(iI) > 0)
		{
			iTempValueInstant += 10 * pEntry->GetYieldPerBirth(iI) / iExpectedTurnsToGrow;
		}

		if (pEntry->GetCoastalCityYieldChange(iI) > 0)
		{
			if (pCity)
			{
				iAvailabilityModifier = pCity->isCoastal() ? 10 : 0;
			}
			else
			{
				iAvailabilityModifier = 4; // todo: check surrounding terrain, how many of our future cities are expected to be coastal?
			}
			iTempValue += iAvailabilityModifier * pEntry->GetCoastalCityYieldChange(iI) * iMinPopulationModifier / 100;
		}

		// Nearby terrain city yield change (max across terrain types - city qualifies once for any matching terrain)
		{
			int iMaxNearbyTerrainScore = 0;
			for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
			{
				if (pEntry->GetNearbyTerrainYieldChange(iTerrain, iI) > 0)
				{
					if (pCity)
					{
						iAvailabilityModifier = (pCity->plot()->getTerrainType() == (TerrainTypes)iTerrain || pCity->IsAdjacentToTerrain((TerrainTypes)iTerrain)) ? 10 : 0;
					}
					else
					{
						iAvailabilityModifier = 3;
					}
					iMaxNearbyTerrainScore = max(iMaxNearbyTerrainScore, iAvailabilityModifier * pEntry->GetNearbyTerrainYieldChange(iTerrain, iI));
				}
			}
			iTempValue += iMaxNearbyTerrainScore * iMinPopulationModifier / 100;
		}

		// Trade Route (City Connection) yield change
		if (pEntry->GetYieldChangeTradeRoute(iI) > 0)
		{
			int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true);

			if (pCity && (pCity->isCapital() || pCity->IsRouteToCapitalConnected()))
			{
				iAvailabilityModifier = 10;
			}
			else
			{
				iAvailabilityModifier = 3 + min(iNumWorkers, 3);
				if (!pCity)
					iAvailabilityModifier -= 2;

				// CP Carthage gives free harbors in every coastal city
				if (pPlayerTraits->GetFreeBuilding() != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(pPlayerTraits->GetFreeBuilding());
					if (pBuildingInfo && pBuildingInfo->AllowsWaterRoutes())
					{
						iAvailabilityModifier += 5;
					}
				}
				if (pPlayerTraits->IsWoodlandMovementBonus())
				{
					iAvailabilityModifier += 3;
				}
			}
			iAvailabilityModifier = min(iAvailabilityModifier, 10);

			iTempValue += iAvailabilityModifier * pEntry->GetYieldChangeTradeRoute(iI);
		}

		// Yield per Science
		if (pEntry->GetYieldPerScience(iI) > 0)
		{
			// taking into account both current and future yields
			int iExpectedScienceInCityTimes100 = pCity ? (pCity->getYieldRateTimes100(YIELD_SCIENCE) * iYieldModEraScaleFactorTimes100 / 100) : (200 * (m_pPlayer->GetCurrentEra() + 1));
			iTempValue += 10 * min((iCurrentFollowers + iExpectedAdditionalFollowers * 8 / 10) / 2, iExpectedScienceInCityTimes100 / 100 / pEntry->GetYieldPerScience(iI));
		}


		// Yield per GPT
		if (pEntry->GetYieldPerGPT(iI) > 0)
		{
			// taking into account both current and future yields
			int iExpectedGPTInCityTimes100 = pCity ? (pCity->getYieldRateTimes100(YIELD_GOLD) * iYieldModEraScaleFactorTimes100 / 100) : (300 * (m_pPlayer->GetCurrentEra() + 1));
			iTempValue += 10 * min((iCurrentFollowers + iExpectedAdditionalFollowers * 8 / 10) / 2, iExpectedGPTInCityTimes100 / 100 / pEntry->GetYieldPerGPT(iI));
		}

		// Yield from unimproved feature
		if (pCity)
		{
			for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
			{
				FeatureTypes eFeature = static_cast<FeatureTypes>(iJ);

				if (!GC.getFeatureInfo(eFeature)->IsNaturalWonder(true))
				{
					int iBaseYield = pEntry->GetCityYieldFromUnimprovedFeature(eFeature, (YieldTypes)iI);
					if (iBaseYield > 0)
					{
						int iAdjacentFeatures = 0;

						for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
						{
							CvPlot* pAdjacentPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iDirectionLoop));
							if (pAdjacentPlot && pAdjacentPlot->getFeatureType() == eFeature && pAdjacentPlot->getImprovementType() == NO_IMPROVEMENT)
							{
								iAdjacentFeatures++;
							}
						}

						int iYield = 0;
						if (iAdjacentFeatures > 2)
						{
							iYield += MOD_BALANCE_ALTERNATE_CELTS_TRAIT ? iBaseYield * 3 : iBaseYield * 2;
						}
						else if (iAdjacentFeatures > 1 && MOD_BALANCE_ALTERNATE_CELTS_TRAIT)
						{
							iYield += iBaseYield * 2;
						}
						else if (iAdjacentFeatures > 0)
						{
							iYield += iBaseYield;
						}
						iTempValue += 10 * iYield;
					}
				}
			}
		}

		// Specialist yield change
		if (pEntry->GetYieldChangeAnySpecialist(iI) > 0)
		{
			// do we have a specialist already?
			if (pCity)
			{
				if (pCity->GetCityCitizens()->GetTotalSpecialistCount() > 0)
				{
					iAvailabilityModifier = (pCity->GetCityCitizens()->GetTotalSpecialistCount() >= 3) ? 10 : 9;
				}
				// if not, current population gives us an idea how long it'll take to get one
				else if (pCity->GetCityCitizens()->GetSpecialistSlotsTotal() > 0)
				{
					iAvailabilityModifier = max(5, min(9, iCurrentCityPop));
				}
				else
				{
					iAvailabilityModifier = max(1, min(3, iCurrentCityPop) - 1);
				}
			}
			else
			{
				iAvailabilityModifier = 1;
			}
			iTempValue += iAvailabilityModifier * pEntry->GetYieldChangeAnySpecialist(iI);
		}

		if (!pCity || !pCity->IsPuppet())
		{
			for (int jJ = 0; jJ < GC.getNumSpecialistInfos(); jJ++)
			{
				if (pEntry->GetSpecialistYieldChange((SpecialistTypes)jJ, iI) > 0)
				{
					if (pCity)
					{
						// do we have a specialist already?
						if (pCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes)jJ) > 0)
						{
							iTempValue += 10 * pEntry->GetSpecialistYieldChange((SpecialistTypes)jJ, iI) * pCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes)jJ);
						}
						// if not, current population gives us an indicator how long it'll take to get one
						else if (pCity->GetCityCitizens()->GetSpecialistSlots((SpecialistTypes)jJ) > 0)
						{
							iAvailabilityModifier = max(4, min(8, iCurrentCityPop));
							iTempValue += iAvailabilityModifier * pEntry->GetSpecialistYieldChange((SpecialistTypes)jJ, iI);
						}
						else
						{
							iAvailabilityModifier = min(4, iCurrentCityPop);
							iTempValue += iAvailabilityModifier * pEntry->GetSpecialistYieldChange((SpecialistTypes)jJ, iI);
						}
					}
					else
					{
						iTempValue += pEntry->GetSpecialistYieldChange((SpecialistTypes)jJ, iI);
					}
				}
			}
		}

		// Yield per border growth
		if (pEntry->GetYieldPerBorderGrowth((YieldTypes)iI, false) > 0 || pEntry->GetYieldPerBorderGrowth((YieldTypes)iI, true) > 0)
		{
			int iTurnsPerBorderGrowthTimes100 = GetExpectedTurnsPerBorderGrowthTimes100(pCity, pEntry);

			iTempValueInstant += 10 * (100 * pEntry->GetYieldPerBorderGrowth((YieldTypes)iI, false) + iEraScaleFactorTimes100 * pEntry->GetYieldPerBorderGrowth((YieldTypes)iI, true)) / max(100, iTurnsPerBorderGrowthTimes100);
		}

		// Luxuries. count only in capital
		if (bIsCapital && pEntry->GetYieldPerLux(iI) > 0)
		{
			int iNumLuxNow = 0;
			int iNumLuxUnimproved = 0;
			ResourceTypes eResource;
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes)iResourceLoop;

				if (m_pPlayer->GetHappinessFromLuxury(eResource) > 0)
				{
					if ((m_pPlayer->getNumResourceTotal(eResource, true) + m_pPlayer->getResourceExport(eResource)) > 0)
						iNumLuxNow++;
					else if (m_pPlayer->getNumResourceUnimproved(eResource) > 0)
						iNumLuxUnimproved++;
				}
			}

			bool bTraitLuxuryImport = pPlayerTraits->IsImportsCountTowardsMonopolies();
			if (!bTraitLuxuryImport)
			{
				for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
				{
					YieldTypes eYield = static_cast<YieldTypes>(iYieldLoop);
					if (pPlayerTraits->GetYieldFromImport(eYield) > 0)
					{
						bTraitLuxuryImport = true;
						break;
					}
				}
			}

			int iNumFutureLuxEstimate = 1;
			if (m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
				iNumFutureLuxEstimate += 1;
			if (bTraitLuxuryImport)
				iNumFutureLuxEstimate += 2;
			if (pPlayerTraits->GetUniqueLuxuryQuantity() > 0 && MOD_BALANCE_ALTERNATE_INDONESIA_TRAIT)
			{
				for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					ResourceTypes eResource = (ResourceTypes)iResourceLoop;
					CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
					if (pkResource != NULL && pkResource->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
					{
						if (m_pPlayer->getNumResourceTotal(eResource, false) == 0 && m_pPlayer->getNumResourceUnimproved(eResource) == 0)
							iNumFutureLuxEstimate++;
					}
				}
			}
	
			iTempValueCapital += (10 * iNumLuxNow + 5 * iNumLuxUnimproved + 3 * iNumFutureLuxEstimate) * pEntry->GetYieldPerLux(iI);
		}

		if (pCity && pEntry->GetYieldPerActiveTR(iI) > 0)
		{
			iTempValue += 10 * (m_pPlayer->GetTrade()->GetNumberOfTradeRoutesCity(pCity) + m_pPlayer->GetTrade()->GetNumberOfCityStateTradeRoutesFromCity(pCity));
		}

		if (pCity && pEntry->GetGreatWorkYieldChange(iI) > 0)
		{
			iTempValue += 10 * pCity->GetCityCulture()->GetNumGreatWorks() * iMinPopulationModifier / 100;
		}

		// Building class yield change
		for (int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)jJ);
			if (!pkBuildingClassInfo)
			{
				continue;
			}

			if (pEntry->GetBuildingClassYieldChange(jJ, iI) > 0)
			{
				BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings((BuildingClassTypes)jJ);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				bool bCapitalOnly = pkBuildingInfo->IsCapital() || ::isNationalWonderClass(pkBuildingInfo->GetBuildingClassInfo()) || pkBuildingInfo->IsCapitalOnly();

				if (pCity && pCity->GetCityBuildings()->HasBuildingClass((BuildingClassTypes)jJ))
				{
					iAvailabilityModifier = bCapitalOnly ? 8 : 10; // lower value for buildings only in the capital because there is no scaling potential at all
				}
				else if (pCity && pCity->canConstruct(eBuilding))
				{
					iAvailabilityModifier = 8;
				}
				else
				{
					if (bCapitalOnly && !bIsCapital)
					{
						iAvailabilityModifier = 0;
					}
					else if (pkBuildingInfo->GetLocalResourceOrSize() > 0)
					{
						// we need a local resource to build this? assume the building will be very rare
						iAvailabilityModifier = 1;
					}
					else
					{
						TechTypes ePrereqTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();

						if (ePrereqTech == NO_TECH || GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
						{
							// we have the tech to build this. if pCity != NULL then canConstruct above failed for other reasons, assume a lower availability.
							// if pCity == NULL and we're evaluating this for a potential city, assume we can construct it immediately
							iAvailabilityModifier = pCity ? 4 : 8;
						}
						else
						{
							iAvailabilityModifier = GetTechAvailabilityModifier(ePrereqTech, pCity == NULL);
						}

						// for defense buildings check if we need them
						if (pkBuildingInfo->GetDefenseModifier() > 0)
						{
							iAvailabilityModifier += kContext.iDefensePriority / 5 - 2;
						}

						if (!pCity)
						{
							// reduce availability for all buildings in potential cities as they have low production and need to build many things at the beginning
							iAvailabilityModifier--;
						}

						// unique building, assume we focus on getting it quickly
						if (m_pPlayer->getCivilizationInfo().isCivilizationBuildingOverridden(pkBuildingInfo->GetBuildingClassType()))
						{
							iAvailabilityModifier += 1;
						}
						iAvailabilityModifier = max(1, min(10, iAvailabilityModifier));
					}
				}
				iTempValue += iAvailabilityModifier * pEntry->GetBuildingClassYieldChange(jJ, iI);
			}
		}

		if (pEntry->GetYieldPerConstruction(iI) > 0)
		{
			int iTurnsPerConstruction = 0;
			if (pCity)
			{
				// calculate the average price of the buildings we can build now or in the near future
				int iAverageBuildingPrice = 0;
				int iNumValidBuildings = 0;
				for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					BuildingClassTypes eBuildingClass = BuildingClassTypes(iBuildingClassLoop);
					BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
					if (eBuilding == NO_BUILDING)
						continue;

					CvBuildingEntry* pBuilding = GC.getBuildingInfo(eBuilding);

					// no wonders
					if (isWorldWonderClass(pBuilding->GetBuildingClassInfo()) || isNationalWonderClass(pBuilding->GetBuildingClassInfo()))
						continue;
					
					bool bBuildingValid = pCity->canConstruct(eBuilding);
					if (!bBuildingValid)
					{
						if (pBuilding->GetProductionCost() - 1)
							continue;

						if (pCity->HasBuilding(eBuilding))
							continue;

						if (pBuilding->GetPolicyType() != NO_POLICY || pBuilding->IsUnlockedByBelief() || !pCity->hasBuildingPrerequisites(eBuilding))
							continue;

						// can we build it soon?
						std::vector<int> vPrereqTechs;
						if (pBuilding->GetPrereqAndTech() != NO_TECH)
							vPrereqTechs.push_back(pBuilding->GetPrereqAndTech());

						for (int iPrereqTechLoop = 0; iPrereqTechLoop < /*3*/ GD_INT_GET(NUM_BUILDING_AND_TECH_PREREQS); iPrereqTechLoop++)
						{
							if (pBuilding->GetPrereqAndTechs(iPrereqTechLoop) != NO_TECH)
								vPrereqTechs.push_back(pBuilding->GetPrereqAndTechs(iPrereqTechLoop));
						}

						bool bCanResearch = true;
						for (std::vector<int>::iterator it = vPrereqTechs.begin(); it != vPrereqTechs.end(); ++it)
						{
							if (!m_pPlayer->HasTech((TechTypes)(*it)) && !m_pPlayer->GetPlayerTechs()->CanResearch((TechTypes)(*it)))
							{
								bCanResearch = false;
								break;
							}
						}
						bBuildingValid = bCanResearch;
					}

					if (bBuildingValid)
					{
						iNumValidBuildings++;
						iAverageBuildingPrice += pCity->getProductionNeeded(eBuilding, true);
					}
				}
				if (iNumValidBuildings > 0)
				{
					iAverageBuildingPrice /= iNumValidBuildings;
					// assume we're constructing buildings two thirds of the time
					iTurnsPerConstruction = 3 * iAverageBuildingPrice / max(1, pCity->getYieldRateTimes100(YIELD_PRODUCTION) / 100) / 2;
				}
				else
				{
					// fallback
					iTurnsPerConstruction = 10;
					iTurnsPerConstruction *= GC.getGame().getGameSpeedInfo().getConstructPercent();
					iTurnsPerConstruction /= 100;
				}
			}

			else
			{
				// in new cities, assume one building every 10 turns
				iTurnsPerConstruction = 10;
				iTurnsPerConstruction *= GC.getGame().getGameSpeedInfo().getConstructPercent();
				iTurnsPerConstruction /= 100;
			}
			iTempValueInstant += 10 * pEntry->GetYieldPerConstruction(iI) / max(1, iTurnsPerConstruction);

		}
		int iFollowerRequiredPerYield = pEntry->GetFollowerRequiredPerYield(iI);
		if (iFollowerRequiredPerYield > 0)
		{
			int iYieldFromFollowers = (10 * iCurrentFollowers + 8 * iExpectedAdditionalFollowers) / iFollowerRequiredPerYield;
			if (pEntry->GetMaxYieldPerFollower(iI) > 0)
			{
				iYieldFromFollowers = min(iYieldFromFollowers, 10 * pEntry->GetMaxYieldPerFollower(iI));
			}
			iTempValue += iYieldFromFollowers;
		}

		if ((YieldTypes)iI == YIELD_GOLD && pEntry->GetGoldPerFollowingCity() > 0)
		{
			int iAvailabilityModifier = 0;
			if (pCity)
			{
				iAvailabilityModifier = bFollowingReligion ? 10 : 7;
			}
			else
			{
				iAvailabilityModifier = 3;
			}
			iTempValue += iAvailabilityModifier * pEntry->GetGoldPerFollowingCity();
		}
		if ((YieldTypes)iI == YIELD_GOLD && pEntry->GetGoldPerXFollowers() > 0)
		{
			iTempValue += (10 * iCurrentFollowers + 8 * iExpectedAdditionalFollowers) / pEntry->GetGoldPerXFollowers();
		}

		// yield modifiers

		// iTempValueYieldMod = AvailabilityModifier * YieldModifier
		int iReligionYieldModifierMaxFollowers = pEntry->GetMaxYieldModifierPerFollower(iI);
		int iReligionYieldModifierMaxFollowersPercent = pEntry->GetMaxYieldModifierPerFollowerPercent(iI);
		if (iReligionYieldModifierMaxFollowersPercent > 0)
		{
			iTempValueYieldMod += 10 * min(iReligionYieldModifierMaxFollowers, max(1, (iCurrentFollowers + iExpectedAdditionalFollowers * 8 / 10) * iReligionYieldModifierMaxFollowersPercent / 100));
		}
		else if (iReligionYieldModifierMaxFollowers > 0)
		{
			iTempValueYieldMod += 10 * min(iReligionYieldModifierMaxFollowers, iCurrentFollowers + iExpectedAdditionalFollowers * 8 / 10);
		}

		if (pEntry->GetYieldFromWLTKD(iI) > 0)
		{
			iAvailabilityModifier = m_pPlayer->EstimateWLTKDAvailability();
			iTempValueYieldMod += iAvailabilityModifier * pEntry->GetYieldFromWLTKD(iI);
		}

		if (pEntry->GetYieldBonusGoldenAge(iI) > 0 && bIsHolyCity)
		{
			iTempValueYieldMod += 10 * pEntry->GetYieldBonusGoldenAge(iI) * m_pPlayer->EstimateGoldenAgePercentage() / 100;
		}
		
		// score the yields and add them to the total score
		if (iTempValue > 0)
		{
			// per-turn yields are affected by city yield modifiers
			int iCityYieldMod = pCity ? pCity->getBaseYieldRateModifier((YieldTypes)iI) : (100 + m_pPlayer->getYieldRateModifier((YieldTypes)iI));
			if (pCity)
			{
				iCityYieldMod *= pCity->getYieldModifierMultiplicative((YieldTypes)iI);
				iCityYieldMod /= 100;
			}
			else if (pPlayerTraits->IsNoAnnexing())
			{
				iCityYieldMod *= GD_INT_GET(PUPPET_YIELD_AND_SUPPLY_MODIFIER_MULTIPLICATIVE) + m_pPlayer->GetPuppetYieldAndSupplyModifierChange() + m_pPlayer->GetPlayerTraits()->GetPuppetYieldAndSupplyModifierChange();
				iCityYieldMod /= 100;
			}
			iRtnValue += iTempValue * iCityYieldMod * vYieldScores[iI] / 10000;
		}
		if (iTempValueCapital > 0)
		{
			CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
			int iCapitalYieldMod = pCapitalCity ? pCapitalCity->getBaseYieldRateModifier((YieldTypes)iI) : (100 + m_pPlayer->getYieldRateModifier((YieldTypes)iI));
			if (pCapitalCity)
			{
				iCapitalYieldMod *= pCapitalCity->getYieldModifierMultiplicative((YieldTypes)iI);
				iCapitalYieldMod /= 100;
			}
			iRtnValue += iTempValueCapital * iCapitalYieldMod * vYieldScores[iI] / 10000;
		}
		if (iTempValueInstant > 0)
		{
			// instant yields are not affected by city modifiers. but they are affected by game speed
			iRtnValue += iTempValueInstant * GC.getGame().getGameSpeedInfo().getInstantYieldPercent() / 100 * vYieldScores[iI] / 100;
		}
		if (iTempValueYieldMod > 0)
		{
			// yield modifiers. multiply them with current city yield and apply an era scaling factor to take into account future yields
			int iCityYieldTimes100 = pCity ? (pCity->getBaseYieldRateTimes100((YieldTypes)iI) * iYieldModEraScaleFactorTimes100 / 100) : (300 * (m_pPlayer->GetCurrentEra() + 1));
			iRtnValue += iTempValueYieldMod * iCityYieldTimes100 / 100 * vYieldScores[iI] / 10000;
		}
	}

	//////////////////
	//Buildings
	///////////////////////

	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();
	bool bIsCulture = pDiploAI->IsGoingForCultureVictory();
	bool bIsWarmonger = pDiploAI->IsGoingForWorldConquest();
	bool bDiploVictoryEnabled = GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true));
	int iNumUnits = m_pPlayer->getNumMilitaryUnits();

	for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
	{
		if (!pEntry->IsBuildingClassEnabled(iK))
			continue;

		BuildingTypes eBuilding = (BuildingTypes)m_pPlayer->getCivilizationInfo().getCivilizationBuildings(iK);
		if (eBuilding == NO_BUILDING)
			continue;

		CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);

		// don't evaluate the Reformation Wonder unlocked by this belief if we already have a Founder Belief (Trait AnyBelief). All Reformation Wonders are mutually exclusive
		if (pBuildingEntry->IsReformation() && pPlayerTraits->IsAnyBelief())
		{
			bool bAlreadyHaveFounderBelief = false;
			for (BeliefList::const_iterator it = kContext.vOurReligionBeliefs.begin(); it != kContext.vOurReligionBeliefs.end(); ++it)
			{
				CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
				if (pkBeliefInfo && pkBeliefInfo->IsFounderBelief())
				{
					bAlreadyHaveFounderBelief = true;
					break;
				}
			}
			if (bAlreadyHaveFounderBelief)
				continue;
		}

		if (pCity && pCity->HasBuilding(eBuilding))
			continue;

		if (pCity && CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(pCity) && !MOD_GLOBAL_PURCHASE_FAITH_BUILDINGS_IN_PUPPETS)
			continue;

		if (pCity && pCity->IsRazing())
			continue;

		// do we have the techs to build the building?
		int iTechCityAvailability = 10;
		if (pBuildingEntry->GetPrereqAndTech() != NO_TECH)
		{
			TechTypes eTech = (TechTypes)pBuildingEntry->GetPrereqAndTech();
			if (!m_pPlayer->HasTech(eTech))
			{
				// reduce building value based on how long it'll take for us to get the tech
				iTechCityAvailability *= GetTechAvailabilityModifier(eTech, pCity == NULL);
				iTechCityAvailability /= 10;
			}
		}

		for (int iPrereqTechLoop = 0; iPrereqTechLoop < /*3*/ GD_INT_GET(NUM_BUILDING_AND_TECH_PREREQS); iPrereqTechLoop++)
		{
			if (pBuildingEntry->GetPrereqAndTechs(iPrereqTechLoop) != NO_TECH)
			{
				TechTypes eTech = (TechTypes)pBuildingEntry->GetPrereqAndTechs(iPrereqTechLoop);
				if (m_pPlayer->HasTech(eTech))
					continue;

				iTechCityAvailability *= GetTechAvailabilityModifier(eTech, pCity == NULL);
				iTechCityAvailability /= 10;
			}
		}

		int iFaithCost = m_pPlayer->getCapitalCity()->GetFaithPurchaseCost(eBuilding); // we can use the capital city here, the cost doesn't depend on the city. (todo: move the function to CvPlayer?)

		// if this building can be built with either faith or production, the belief doesn't actually unlock the building, it just allows us to get it earlier. give a score based on the comparison of production and faith value
		if (iFaithCost > 0 && pBuildingEntry->GetProductionCost() > 0)
		{
			// compare the value of the faith we'd have to spend and the value of the production, with a low base score as minimum value
			iRtnValue += iTechCityAvailability * max(100, (m_pPlayer->getProductionNeeded(eBuilding, true) * vYieldScores[YIELD_PRODUCTION] - m_pPlayer->getCapitalCity()->GetFaithPurchaseCost(eBuilding, true) * vYieldScores[YIELD_FAITH]) / GC.getGame().getGameSpeedInfo().getConstructPercent()) / 300;

			// skip the evaluation of the building's effects
			continue;
		}
		
		// this building can only be purchased with faith, so we adjust the availability modifier based on our faith output. as that is independent from the tech availability modifier calculated above,
		// we calculate a separate value and use the minimum of the two for the scoring below
		int iCityAvailability = 0;

		if (pBuildingEntry->IsReformation())
		{
			if (pCity == kContext.pHolyCity)
			{
				iCityAvailability = 3 + kContext.iNumNearbyCitiesToSpreadTo / 10; // only founders unlock reformation buildings, so we don't need to check how many cities already follow our religion
			}
			else
			{
				continue;
			}
		}
		else
		{
			if ((kContext.bFoundingReligion && pCity && pCity->isCapital()) || (kContext.eReligion != NO_RELIGION && pCity && pCity->GetCityReligions()->GetReligiousMajority() == kContext.eReligion))
				iCityAvailability = 9;
			else
			{
				// we need to spread our religion to the city first. that also means we need faith for missionaries before we can buy the building
				iCityAvailability = 6;
			}

			// if we don't earn much faith, it would take much longer to buy buildings
			int iFaithCollectTurns = m_pPlayer->getCapitalCity()->GetFaithPurchaseCost(eBuilding, true) / max(m_pPlayer->GetTotalFaithPerTurnTimes100() / 100, 1);
			iCityAvailability *= max(0, 100 - (int)pow((double)iFaithCollectTurns, 1.15));
			iCityAvailability /= 100;
		}

		iCityAvailability = min(iCityAvailability, iTechCityAvailability);

		// now we evaluate the effects. the calculations here should be consistent with the ones for the direct belief effects from above, if a corresponding belief effects exists
		// (with the exception of the availability modifier that takes into account the time until we can build the building)
		// note that for the evaluation of the building effects it would be nice if we could use code from BuildingProductionAI, but that logic is so bad that that's not really possible right now
		// as of now, only those building effects are evaluated that are actually used by a religious building in CP or VP
		
		// don't score unhappiness reductions in puppets. also, score flat unhappiness reductions only if the city is actually unhappy
		if (pCity && !CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(pCity))
		{
			if (pBuildingEntry->GetPovertyFlatReduction() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetPovertyFlatReduction(), pCity->GetPoverty(false)) * iHappinessValue / 100;
			}
			if (pBuildingEntry->GetIlliteracyFlatReduction() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetIlliteracyFlatReduction(), pCity->GetIlliteracy(false)) * iHappinessValue / 100;
			}
			if (pBuildingEntry->GetBoredomFlatReduction() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetBoredomFlatReduction(), pCity->GetBoredom(false)) * iHappinessValue / 100;
			}
			if (pBuildingEntry->GetDistressFlatReduction() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetDistressFlatReduction(), pCity->GetDistress(false)) * iHappinessValue / 100;
			}
			if (pBuildingEntry->GetReligiousUnrestFlatReduction() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetReligiousUnrestFlatReduction(), pCity->GetUnhappinessFromReligiousUnrest()) * iHappinessValue / 100;
			}
			if (pBuildingEntry->GetNoUnhappfromXSpecialists() > 0)
			{
				iRtnValue += iCityAvailability * min(pBuildingEntry->GetNoUnhappfromXSpecialists(), pCity->getUnhappinessFromSpecialists(pCity->GetCityCitizens()->GetTotalSpecialistCount())) * iHappinessValue / 100;
			}
		}


		if (pBuildingEntry->GetAlwaysHeal() > 0)
		{
			iRtnValue += iCityAvailability * kContext.iDefensePriority * pBuildingEntry->GetAlwaysHeal() / 20;
		}
		if (pBuildingEntry->GetExtraSpies() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
		{
			// assume the base value of a spy is 100, modified by espionage flavor
			int iSpyValue = 100 * pBuildingEntry->GetExtraSpies();
			if (MOD_BALANCE_SPY_POINTS)
				iSpyValue *= GD_INT_GET(ESPIONAGE_SPY_POINT_UNIT) / max(1, GC.getGame().GetSpyThreshold());
			iRtnValue += iCityAvailability * iSpyValue * (50 + pFlavorManager->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_ESPIONAGE")) * 5) / 100 / (GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE) ? 5 : 1);
		}
		if (pBuildingEntry->GetMilitaryProductionModifier() > 0)
		{
			iRtnValue += iCityAvailability * (kContext.iOffensePriority + kContext.iDefensePriority / 3) * pBuildingEntry->GetMilitaryProductionModifier() / 500;
		}

		if (pBuildingEntry->GetSpySecurityModifierPerXPop() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE))
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetSpySecurityModifierPerXPop() * (iCurrentCityPop + iExpectedGrowth) / GD_INT_GET(ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER) / 20;
		}
		if (pBuildingEntry->GetSpySecurityModifier() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE))
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetSpySecurityModifier() / 20;
		}
		if (pBuildingEntry->GetGlobalSpySecurityModifier() > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE))
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetGlobalSpySecurityModifier() * m_pPlayer->getNumCities() / 20;
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			YieldTypes eYield = (YieldTypes)iI;

			// TempValue = sum of (AvailabiltyModifier * Yield)

			int iTempValue = 0; // for yields per turn
			int iTempValueInstant = 0; // for instant yields
			int iTempValueYieldMod = 0; // for yield modifiers
			int iTempValueOther = 0; // for yields from other sources

			// yields per turn
			if (pBuildingEntry->GetYieldChange(iI) > 0)
			{
				iTempValue += iCityAvailability * pBuildingEntry->GetYieldChange(iI);
			}
			if (pBuildingEntry->GetYieldChangePerReligion(iI) > 0)
			{
				// YieldChangePerReligion is Times100
				iTempValue += iCityAvailability * (pCity ? pCity->GetCityReligions()->GetNumReligionsWithFollowers() : 1) * pBuildingEntry->GetYieldChangePerReligion(iI) / 100;
			}
			if (pBuildingEntry->GetGreatWorkYieldChangeLocal(iI) > 0)
			{
				// take into account future great works in the city
				int iNumGreatWorks = (pCity ? pCity->GetCityCulture()->GetNumGreatWorks() : 0) + (bIsCulture ? 2 : 1);
				iTempValue += iCityAvailability * iNumGreatWorks * pBuildingEntry->GetGreatWorkYieldChangeLocal(iI);
			}

			if (pCity)
			{
				for (int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
				{
					ResourceTypes eResource = (ResourceTypes)iJ;
					if (pBuildingEntry->GetResourceYieldChange(eResource, eYield) > 0)
					{
						iTempValue += iCityAvailability * pBuildingEntry->GetResourceYieldChange(eResource, eYield) * (100 * pCity->GetNumResourceLocal(eResource, true) + 50 * pCity->GetNumResourceLocal(eResource, false)) / 100;
					}
				}
				for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					if (pBuildingEntry->GetImprovementYieldChange((ImprovementTypes)iJ, eYield) > 0)
					{
						iTempValue += iCityAvailability * pBuildingEntry->GetImprovementYieldChange((ImprovementTypes)iJ, eYield) * pCity->GetNumImprovementWorked((ImprovementTypes)iJ);
					}
					if (pBuildingEntry->GetImprovementYieldChangeGlobal((ImprovementTypes)iJ, eYield) > 0)
					{
						int iLoop2 = 0;
						CvCity* pLoopCity2 = NULL;
						for (pLoopCity2 = m_pPlayer->firstCity(&iLoop2); pLoopCity2 != NULL; pLoopCity2 = m_pPlayer->nextCity(&iLoop2))
						{
							int iNumImprovementsWorked = pLoopCity2->GetNumImprovementWorked((ImprovementTypes)iJ);
							if (iNumImprovementsWorked > 0)
							{
								int iLoopCityYieldMod = pLoopCity2->getBaseYieldRateModifier((YieldTypes)iI);
								iLoopCityYieldMod *= pLoopCity2->getYieldModifierMultiplicative((YieldTypes)iI);
								iLoopCityYieldMod /= 100;

								iTempValueOther += iCityAvailability * iNumImprovementsWorked * pBuildingEntry->GetImprovementYieldChangeGlobal((ImprovementTypes)iJ, eYield) * iLoopCityYieldMod / 100;
							}
						}
					}
				}
			}

			// do we have sacred sites or could we potentially select it later?
			if (eYield == YIELD_TOURISM)
			{
				int iFaithBuildingTourismTimes100 = 0;
				bool bHaveReformationBelief = pEntry->IsReformationBelief(); // if we're picking a reformation belief now, we can't take another one later

				for (BeliefList::const_iterator it = kContext.vOurReligionBeliefs.begin(); it != kContext.vOurReligionBeliefs.end(); ++it)
				{
					CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)*it);
					if (pkBeliefInfo)
					{
						iFaithBuildingTourismTimes100 += pkBeliefInfo->GetFaithBuildingTourism() * 100;
						bHaveReformationBelief |= pkBeliefInfo->IsReformationBelief();
					}
				}

				if (!bHaveReformationBelief)
				{
					// if we can still choose a reformation belief, could we take sacred sites?
					for (int iBeliefLoop = 0; iBeliefLoop < GC.getNumBeliefInfos(); iBeliefLoop++)
					{
						CvBeliefEntry* pkBeliefInfo = GC.getBeliefInfo((BeliefTypes)iBeliefLoop);
						if (pkBeliefInfo && pkBeliefInfo->IsReformationBelief() && pkBeliefInfo->GetFaithBuildingTourism() > 0)
						{
							// can we always pick this belief?
							if (pPlayerTraits->IsAnyBelief() || !GC.getGame().GetGameReligions()->IsInSomeReligion((BeliefTypes)iBeliefLoop))
							{
								iFaithBuildingTourismTimes100 += pkBeliefInfo->GetFaithBuildingTourism() * 25;
							}
						}
					}
				}
				iTempValue += iCityAvailability * iFaithBuildingTourismTimes100 / 100;
			}

			// instant yields

			if (pBuildingEntry->GetYieldFromVictoryGlobal(iI) > 0 || pBuildingEntry->GetYieldFromVictoryGlobalEraScaling(iI) > 0)
			{
				iTempValueInstant += iCityAvailability * (100 * pBuildingEntry->GetYieldFromVictoryGlobal(iI) + iEraScaleFactorTimes100 * pBuildingEntry->GetYieldFromVictoryGlobalEraScaling(iI)) / 100 * kContext.iOffensePriority * iNumUnits / 150;
			}
			if (pBuildingEntry->GetYieldFromVictory(iI) > 0 || pBuildingEntry->GetYieldFromVictoryEraScaling(iI) > 0)
			{
				// assume all cities have produced the same number of units, so this is the value for GetYieldFromVictoryGlobal divided by the number of cities
				iTempValueInstant += iCityAvailability * (100 * pBuildingEntry->GetYieldFromVictory(iI) + iEraScaleFactorTimes100 * pBuildingEntry->GetYieldFromVictoryEraScaling(iI)) / 100 * kContext.iOffensePriority * iNumUnits / m_pPlayer->getNumCities() / 150;
			}
			if (pBuildingEntry->GetYieldFromBorderGrowth(iI) > 0)
			{
				int iTurnsPerBorderGrowthTimes100 = GetExpectedTurnsPerBorderGrowthTimes100(pCity, pEntry);

				iTempValueInstant += iCityAvailability * pBuildingEntry->GetYieldFromBorderGrowth(iI) * 100 / max(100, iTurnsPerBorderGrowthTimes100);
			}

			if (pBuildingEntry->GetYieldFromBirth(iI) > 0 || pBuildingEntry->GetYieldFromBirthEraScaling(iI) > 0)
			{
				iTempValueInstant += iCityAvailability * (100 * pBuildingEntry->GetYieldFromBirth(iI) + iEraScaleFactorTimes100 * pBuildingEntry->GetYieldFromBirthEraScaling(iI)) / 100 / iExpectedTurnsToGrow;
			}

			if ((pBuildingEntry->GetYieldFromSpyDefense(iI) > 0 || pBuildingEntry->GetYieldFromSpyDefenseOrID(iI)) && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE))
			{
				// assume one kill every 150 turns, this happens very rarely. todo: better estimate for that number. maybe based on: how many enemies do we have? how far are we ahead?
				iTempValueInstant += iCityAvailability * (pBuildingEntry->GetYieldFromSpyDefense(iI) + pBuildingEntry->GetYieldFromSpyDefenseOrID(iI)) * iEraScaleFactorTimes100 / 100 / 150;
			}
			if ((pBuildingEntry->GetYieldFromSpyIdentify(iI) > 0 || pBuildingEntry->GetYieldFromSpyDefenseOrID(iI) > 0) && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE))
			{
				// assume one ID'd spy every 75 turns. todo: better estimate, see comment above
				iTempValueInstant += iCityAvailability * (pBuildingEntry->GetYieldFromSpyIdentify(iI) + pBuildingEntry->GetYieldFromSpyDefenseOrID(iI)) * iEraScaleFactorTimes100 / 100 / 75;
			}
			if (pBuildingEntry->GetYieldFromSpyRigElection(iI) > 0 && !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && !GC.getGame().isOption(GAMEOPTION_PASSIVE_ESPIONAGE) && bDiploVictoryEnabled)
			{
				// assume one rigged election every 90 turns, 30 if going for Diplo Victory. these spy estimates are all not great, but given that all the spy effects are only used on reformation buildings and are not super strong, it should be okay for now
				iTempValueInstant += iCityAvailability * pBuildingEntry->GetYieldFromSpyRigElection(iI) * iEraScaleFactorTimes100 / 100 / (pDiploAI->IsGoingForDiploVictory() ? 30 : 90);
			}

			// yield modifiers
			// iTempValueYieldMod = AvailabilityModifier * YieldModifier
			if (pBuildingEntry->GetYieldModifier(iI) > 0 || pBuildingEntry->GetYieldModifierEraScaling(iI) > 0)
			{
				iTempValueYieldMod += iCityAvailability * (100 * pBuildingEntry->GetYieldModifier(iI) + iEraScaleFactorTimes100 * pBuildingEntry->GetYieldModifierEraScaling(iI)) / 100;
			}
			if (pBuildingEntry->GetYieldFromWLTKD(iI) > 0) // this is also a yield modifier
			{
				iTempValueYieldMod += iCityAvailability * pBuildingEntry->GetYieldFromWLTKD(iI) * m_pPlayer->EstimateWLTKDAvailability() / 10;
			}
			if (pBuildingEntry->GetGoldenAgeYieldMod(iI) > 0)
			{
				iTempValueYieldMod += iCityAvailability * pBuildingEntry->GetGoldenAgeYieldMod(iI) * m_pPlayer->EstimateGoldenAgePercentage() / 100;
			}


			if (pBuildingEntry->GetYieldFromUnitProduction(iI) > 0)
			{
				int iCityProductionYieldTimes100 = pCity ? (pCity->getBaseYieldRateTimes100(YIELD_PRODUCTION) * iYieldModEraScaleFactorTimes100 / 100) : (300 * (m_pPlayer->GetCurrentEra() + 1));
				// this converts a percentage of unit production costs to yield iI. note that is it an instant yield, but it doesn't scale with game speed
				// assume we use 20% of our production for units, 50% if warmonger
				iTempValueOther += iCityAvailability * pBuildingEntry->GetYieldFromUnitProduction(iI) * iCityProductionYieldTimes100 / 100 * (bIsWarmonger ? 50 : 20) / 10000;
			}

			// score the yields and add them to the total score
			if (iTempValue > 0)
			{
				// per-turn yields are affected by city yield modifiers
				int iCityYieldMod = pCity ? pCity->getBaseYieldRateModifier((YieldTypes)iI) : (100 + m_pPlayer->getYieldRateModifier((YieldTypes)iI));
				if (pCity)
				{
					iCityYieldMod *= pCity->getYieldModifierMultiplicative((YieldTypes)iI);
					iCityYieldMod /= 100;
				}
				else if (pPlayerTraits->IsNoAnnexing())
				{
					iCityYieldMod *= GD_INT_GET(PUPPET_YIELD_AND_SUPPLY_MODIFIER_MULTIPLICATIVE) + m_pPlayer->GetPuppetYieldAndSupplyModifierChange() + m_pPlayer->GetPlayerTraits()->GetPuppetYieldAndSupplyModifierChange();
					iCityYieldMod /= 100;
				}
				iRtnValue += iTempValue * iCityYieldMod * vYieldScores[iI] / 10000;
			}
			if (iTempValueInstant > 0)
			{
				// instant yields are not affected by city modifiers. but they are affected by game speed
				iRtnValue += iTempValueInstant * GC.getGame().getGameSpeedInfo().getInstantYieldPercent() / 100 * vYieldScores[iI] / 100;
			}
			if (iTempValueYieldMod > 0)
			{
				// yield modifiers. multiply them with current city yield and apply an era scaling factor to take into account future yields
				int iCityYieldTimes100 = pCity ? (pCity->getBaseYieldRateTimes100((YieldTypes)iI) * iYieldModEraScaleFactorTimes100 / 100) : (300 * (m_pPlayer->GetCurrentEra() + 1));
				iRtnValue += iTempValueYieldMod * iCityYieldTimes100 / 100 * vYieldScores[iI] / 10000;
			}
			if (iTempValueOther > 0)
			{
				// modifiers have already been taken into account here
				iRtnValue += iTempValueOther * vYieldScores[iI] / 100;
			}
		}

		// faith cost
		if (pBuildingEntry->GetFaithCost() > 0)
		{
			// split the one-time cost up on 100 turns, scaling with game speed, so we divide by (100 * GC.getGame().getGameSpeedInfo().getConstructPercent() / 100)
			iRtnValue -= iCityAvailability * m_pPlayer->getCapitalCity()->GetFaithPurchaseCost(eBuilding, true) * vYieldScores[YIELD_FAITH] / 100 / GC.getGame().getGameSpeedInfo().getConstructPercent();
		}

		// other effects
		if (pBuildingEntry->GetHappiness() > 0)
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetHappiness() * iHappinessValue / 100;
		}
		if (pBuildingEntry->GetCitySupplyFlat() > 0)
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetCitySupplyFlat() * kContext.iOffensePriority * 3 / 2;
		}
		if (pBuildingEntry->GetGreatWorkCount() > 0 && pDiploAI->IsGoingForCultureVictory())
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetGreatWorkCount();
		}
		for (int iJ = 0; iJ < GC.getNumUnitDomainInfos(); iJ++)
		{
			if (pBuildingEntry->GetDomainFreeExperience(iJ) > 0)
			{
				iRtnValue += iCityAvailability * pBuildingEntry->GetDomainFreeExperience(iJ) * kContext.iOffensePriority * min(150, 100 + 4 * (m_pPlayer->GetNumUnitsSupplied() - m_pPlayer->GetNumUnitsToSupply())) / 5000;
			}
		}
		if (pBuildingEntry->GetReligiousPressureModifier() > 0)
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetReligiousPressureModifier() * kContext.iEnemyReligionsNearby / 200;
		}
		if (pBuildingEntry->GetConversionModifier() > 0)
		{
			iRtnValue += iCityAvailability * pBuildingEntry->GetConversionModifier() * kContext.iEnemyReligionsNearby / 200;
		}
		if (pBuildingEntry->CityRangedStrikeModifier() > 0 || pBuildingEntry->GetBuildingDefenseModifier() > 0)
		{
			iRtnValue += iCityAvailability * (pBuildingEntry->CityRangedStrikeModifier() + pBuildingEntry->GetBuildingDefenseModifier()) * kContext.iDefensePriority / 200;
		}
		if (pBuildingEntry->GetFreePromotion() != NO_PROMOTION)
		{
			// we don't evaluate the effects of the promotion itself here
			iRtnValue += iCityAvailability * (kContext.iOffensePriority + min(10, kContext.iDefensePriority)) / 20;
		}
		if (pBuildingEntry->GetWLTKDTurns() > 0 && m_pPlayer->EstimateWLTKDAvailability() < 10)
		{
			// low flat value, this is a one-time effect
			iRtnValue += iCityAvailability * pBuildingEntry->GetWLTKDTurns() / 100;
			// todo: existing beliefs with WLTKD yield mods?
		}
	}

	// Reductions for belief requirements
	if (pEntry->GetMinFollowers() > 0)
	{
		if (iCurrentFollowers < pEntry->GetMinFollowers())
		{
			iRtnValue *= (100 - 10 * (pEntry->GetMinFollowers() - iCurrentFollowers));
			iRtnValue /= 100;
		}
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
	// Unless only we can benefit from it
	if (!MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV && m_pPlayer->GetPlayerTraits()->IsNoNaturalReligionSpread() && pCity->getOwner() != m_pPlayer->GetID())
	{
		if (pSpreadReligion->m_Beliefs.GetUniqueCiv() == m_pPlayer->getCivilizationType())
		{
			return 0;
		}
	}

	// Major civ - promised not to convert, or bad target?
	if (GET_PLAYER(pCity->getOwner()).isMajorCiv())
	{
		if (m_pPlayer->GetDiplomacyAI()->IsBadTheftTarget(pCity->getOwner(), THEFT_TYPE_CONVERSION))
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

	ReligionTypes eMajorityReligion = pCity->GetCityReligions()->GetReligiousMajority();
	if (!CvReligionAIHelpers::PassesTeammateReligionCheck(eMajorityReligion, m_pPlayer->GetID(), pCity->getOwner() == m_pPlayer->GetID()))
		return 0;

	if (eMajorityReligion <= RELIGION_PANTHEON)
	{
		//fifty percent bonus if not religion at the moment
		iScore *= 3;
		iScore /= 2;
	}

	//don't target inquisitor-protected cities ...
	if (pCity->GetCityReligions()->IsDefendedAgainstSpread(eSpreadReligion))
	{
		if (MOD_BALANCE_INQUISITOR_NERF)
		{
			iScore *= /*50*/ GD_INT_GET(INQUISITOR_CONVERSION_REDUCTION_FACTOR);
			iScore /= 100;
		}
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
		if (kCityPlayer.isMajorCiv() && !kCityPlayer.GetReligions()->OwnsReligion() && kCityPlayer.GetReligions()->GetStateReligion() != eSpreadReligion)
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
	if (CvReligionAIHelpers::ShouldRemoveHeresy(pCity,eMyReligion,m_pPlayer->GetID()))
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
	if (CvReligionAIHelpers::ShouldRemoveHeresy(pCity,eMyReligion,m_pPlayer->GetID()))
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

	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->GetCityReligions()->GetReligiousMajority() == eReligion)
		{
			if (bIncludePuppets || !pLoopCity->IsPuppet())
			{
				BuildingClassTypes eFaithBuildingClass = FaithBuildingAvailable(eReligion, pLoopCity);
				if (eFaithBuildingClass == NO_BUILDINGCLASS)
					continue;

				if (!pLoopCity->HasBuildingClass(eFaithBuildingClass))
					return false;
			}
		}
	}

	return true;
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
			if (pUnit->GetReligionData()->GetReligion() == eReligion)
				iNumInquisitors++;
	}

	// Need one for every city in our realm that is of another religion
	int iNumNeeded = 0;
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (m_pPlayer->GetReligionAI()->ScoreCityForInquisitorOffensive(pCity, NULL, eReligion) > 0)
			iNumNeeded++;
	}

	/*
	//basic sanity check
	int iThreshold = max(3, m_pPlayer->getNumCities() / 5);
	if (iNumNeeded > iThreshold && iNumInquisitors > iThreshold)
	{
		CUSTOMLOG("Warning: Player %d seems to need more inquisitors but already has a lot.", m_pPlayer->GetID());
		iNumNeeded = 3;
	}
	*/

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

	std::vector<BuildingClassTypes> choices;

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
						//Let's not do this for the national wonders, okay?
						if(!pBuildingEntry->IsReformation())
							choices.push_back((BuildingClassTypes)iI);
					}
				}
			}
		}
	}

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

	return NO_BUILDINGCLASS;
}

bool CvReligionAI::IsProphetGainRateAcceptable()
{
	int iFaithPerTurn = m_pPlayer->GetTotalFaithPerTurnTimes100();

	int iFaithToNextProphet = m_pPlayer->GetReligions()->GetCostNextProphet(true, true, true) * 100;
	
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
				CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);

				// Make sure it costs faith, and isn't a religious-specific building
				if(pBuildingEntry && pBuildingEntry->GetFaithCost() > 0 && pBuildingEntry->GetReligiousPressureModifier() <= 0)
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

/// Log belief combinations considered when founding or enhancing a religion
void CvReligionAI::LogBeliefCombinationChoices(CvWeightedVector<int>& combinationChoices, const vector<vector<BeliefTypes>>& vvCombinations, int iChoice)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog = NULL;
		pLog = LOGFILEMGR.GetLog("TotalBeliefScoringReligionLog.csv", FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d, ", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strBaseString += playerName + ", ";

		// Dump out the weight of each combination
		for(int iI = 0; iI < combinationChoices.size(); iI++)
		{
			CvString strDesc;
			const vector<BeliefTypes>& vCombination = vvCombinations[combinationChoices.GetElement(iI)];
			for(size_t iJ = 0; iJ < vCombination.size(); iJ++)
			{
				if(iJ > 0)
					strDesc += " + ";
				strDesc += GetLocalizedText(GC.GetGameBeliefs()->GetEntry(vCombination[iJ])->getShortDescription());
			}
			strTemp.Format("Belief Combination, %s, %d", strDesc.GetCString(), combinationChoices.GetWeight(iI));
			if(combinationChoices.GetElement(iI) == iChoice)
				strTemp += ", CHOSEN";
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
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

bool CvReligionAIHelpers::ShouldRemoveHeresy(CvCity* pCity, ReligionTypes eTrueReligion, PlayerTypes ePlayer)
{
	ReligionTypes eMajorityReligion = pCity->GetCityReligions()->GetReligiousMajority();
	if (eMajorityReligion == NO_RELIGION)
	{
		ReligionTypes eMostPowerfulReligion = pCity->GetCityReligions()->GetReligionByAccumulatedPressure(0);
		if (eMostPowerfulReligion <= RELIGION_PANTHEON)
			return false;
		if (!CvReligionAIHelpers::PassesTeammateReligionCheck(eMostPowerfulReligion, ePlayer, true))
			return false;
		if (eMostPowerfulReligion != eTrueReligion)
			return true;

		ReligionTypes eRunnerUpReligion = pCity->GetCityReligions()->GetReligionByAccumulatedPressure(1);
		if (!CvReligionAIHelpers::PassesTeammateReligionCheck(eRunnerUpReligion, ePlayer, true))
			return false;

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
	else if (eMajorityReligion != eTrueReligion && CvReligionAIHelpers::PassesTeammateReligionCheck(eMajorityReligion, ePlayer, true))
	{
		//the easy case
		return true;
	}

	return false;
}

/// Avoid competing with our teammates' religions.
bool CvReligionAIHelpers::PassesTeammateReligionCheck(ReligionTypes eReligion, PlayerTypes ePlayer, bool bMustBeHuman)
{
	PlayerTypes eController = NO_PLAYER;
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
	if (MOD_BALANCE_VP)
	{
		if (pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			if (pHolyCity != NULL)
			{
				eController = pHolyCity->getOwner();
			}
		}
	}
	else if (pReligion)
	{
		eController = pReligion->m_eFounder;
	}

	if (eController == NO_PLAYER || eController == ePlayer)
		return true;

	if (bMustBeHuman && !GET_PLAYER(eController).isHuman(ISHUMAN_MECHANICS))
		return true;

	return GET_PLAYER(eController).getTeam() != GET_PLAYER(ePlayer).getTeam();
}
