/*	-------------------------------------------------------------------------------------------------------
	? 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include "CvCitySpecializationAI.h"

#include "CvBarbarians.h"

#include "CvNotifications.h"
#include "cvStopWatch.h"
#include "CvCityManager.h"
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
#include "CvInternalGameCoreUtils.h"
#include "CvWonderProductionAI.h"
#endif
#if defined(MOD_BALANCE_CORE_DEALS)
#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"
#endif
#ifdef AUI_TRADE_SCORE_INTERNATIONAL_TOURISM_SCORE_USES_GRAND_STRATEGY
#include "CvGrandStrategyAI.h"
#endif

#include "LintFree.h"

//=====================================
// CvGameTrade
//=====================================

#define TEMPORARY_POPUPROUTE_ID (-1) //m_aTradeConnections will never use this as an index
#define MAX_PLOTS_TO_DISPLAY 256

//	--------------------------------------------------------------------------------
/// Default constructor
CvGameTrade::CvGameTrade()
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Destructor
CvGameTrade::~CvGameTrade(void)
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Init
void CvGameTrade::Init (void)
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Uninit
void CvGameTrade::Uninit (void)
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Uninit
void CvGameTrade::Reset (void)
{
	m_aTradeConnections.clear();
	m_iNextID = 0;
	m_CurrentTemporaryPopupRoute.iPlotX = 0;
	m_CurrentTemporaryPopupRoute.iPlotY = 0;
	m_CurrentTemporaryPopupRoute.type = TRADE_CONNECTION_INTERNATIONAL;
}

//	--------------------------------------------------------------------------------
/// DoTurn
void CvGameTrade::DoTurn (void)
{
	ResetTechDifference();
	ResetPolicyDifference();
	BuildTechDifference();
	BuildPolicyDifference();
}

// helper function
bool HaveTradePathInCache(const TradePathLookup& cache, int iCityA, int iCityB)
{
	TradePathLookup::const_iterator itA = cache.find(iCityA);
	if (itA!=cache.end())
	{
		TradePathLookup::value_type::second_type::const_iterator itB = itA->second.find(iCityB);
		return (itB!=itA->second.end());
	}
	
	return false;
}

// helper function
bool AddTradePathToCache(TradePathLookup& cache, int iCityA, int iCityB, const SPath& path)
{
	TradePathLookup::const_iterator itA = cache.find(iCityA);
	if (itA!=cache.end())
	{
		TradePathLookup::value_type::second_type::const_iterator itB = itA->second.find(iCityB);
		if (itB!=itA->second.end())
			cache[iCityA][iCityB] = path;
		else
			cache[iCityA].insert( std::make_pair(iCityB,path) );
	}
	else
	{
		TradePathLookup::value_type::second_type newDestinations;
		newDestinations.insert( std::make_pair(iCityB,path) );
		cache.insert( std::make_pair(iCityA,newDestinations) );
	}
	
	return false;
}

bool CvGameTrade::HavePotentialTradePath(bool bWater, CvCity* pOriginCity, CvCity* pDestCity, SPath* pPathOut)
{
	if (!pOriginCity || !pDestCity)
		return false;

	//important. see which trade paths are valid
	PlayerTypes eOriginPlayer = pOriginCity->getOwner();
	UpdateTradePathCache(eOriginPlayer);

	//can't use const here, otherwise the [] operator does not work ...
	TradePathLookup& cache = bWater ? m_aPotentialTradePathsWater : m_aPotentialTradePathsLand;

	int iCityA = pOriginCity->GetID();
	int iCityB = pDestCity->GetID();
	if (HaveTradePathInCache(cache, iCityA, iCityB))
	{
		if (pPathOut)
			*pPathOut = cache[iCityA][iCityB];
		return true;
	}

	return false;
}

void CvGameTrade::InvalidateTradePathCache(uint iPlayer)
{
	m_lastTradePathUpdate[iPlayer] = -1;
}

void CvGameTrade::UpdateTradePathCache(uint iPlayer1)
{
	CvPlayer& kPlayer1 = GET_PLAYER((PlayerTypes)iPlayer1);
	if (!kPlayer1.isAlive() || kPlayer1.isBarbarian())
		return;

	//check if we have anything to do
	std::map<uint,int>::iterator lastUpdate = m_lastTradePathUpdate.find(iPlayer1);
	if (lastUpdate!=m_lastTradePathUpdate.end() && lastUpdate->second==GC.getGame().getGameTurn())
		return;

	//do not check whether we are at war here! the trade route cache is also used for military target selection
	//OutputDebugString(CvString::format("updating trade path cache for player %d, turn %d\n", iPlayer1, GC.getGame().getGameTurn()).c_str());

	vector<CvPlot*> vDestPlots;
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		int iCity;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
			vDestPlots.push_back(pDestCity->plot());
	}

	int iOriginCityLoop;
	for (CvCity* pOriginCity = kPlayer1.firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = kPlayer1.nextCity(&iOriginCityLoop))
	{
		//first see how far we can go from this city on water
		int iMaxNormDistSea = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);
		SPathFinderUserData data((PlayerTypes)iPlayer1,PT_TRADE_WATER);
		data.iMaxNormalizedDistance = iMaxNormDistSea;

		//get all paths
		map<CvPlot*,SPath> waterpaths = GC.GetStepFinder().GetMultiplePaths( pOriginCity->plot(), vDestPlots, data );
		for (map<CvPlot*,SPath>::iterator it=waterpaths.begin(); it!=waterpaths.end(); ++it)
		{
			// if this is the origin city, nothing to do
			if (pOriginCity->plot() == it->first)
				continue;

			CvCity* pDestCity = it->first->getPlotCity();
			AddTradePathToCache(m_aPotentialTradePathsWater,pOriginCity->GetID(),pDestCity->GetID(),it->second);
		}

		//now for land routes
		int iMaxNormDistLand = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_LAND, pOriginCity);
		data.iMaxNormalizedDistance = iMaxNormDistLand;
		data.ePathType = PT_TRADE_LAND;

		//get all paths
		map<CvPlot*,SPath> landpaths = GC.GetStepFinder().GetMultiplePaths( pOriginCity->plot(), vDestPlots, data );
		for (map<CvPlot*,SPath>::iterator it=landpaths.begin(); it!=landpaths.end(); ++it)
		{
			// if this is the origin city, nothing to do
			if (pOriginCity->plot() == it->first)
				continue;

			CvCity* pDestCity = it->first->getPlotCity();
			AddTradePathToCache(m_aPotentialTradePathsLand,pOriginCity->GetID(),pDestCity->GetID(),it->second);
		}

	}

	m_lastTradePathUpdate[iPlayer1]=GC.getGame().getGameTurn();

	for (CvCity* pOriginCity = kPlayer1.firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = kPlayer1.nextCity(&iOriginCityLoop))
	{
		int iDistanceLand = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_LAND, pOriginCity);
		int iDistanceSea = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);

		kPlayer1.GetTrade()->UpdateFurthestPossibleTradeRoute(DOMAIN_LAND, pOriginCity, iDistanceLand);
		kPlayer1.GetTrade()->UpdateFurthestPossibleTradeRoute(DOMAIN_SEA, pOriginCity, iDistanceSea);
	}
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::CanCreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath)
{
	if (pOriginCity == NULL)
	{
		return false;
	}

	if (pDestCity == NULL)
	{
		return false;
	}

	// A city may not trade with itself
	if (pOriginCity == pDestCity)
	{
		return false;
	}

	TeamTypes eOriginTeam = pOriginCity->getTeam();
	TeamTypes eDestTeam = pDestCity->getTeam();

	if (eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		// can't have an international trade route within the same team
		if (eOriginTeam == eDestTeam)
		{
			return false;
		}
	}
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	else if (eConnectionType == TRADE_CONNECTION_PRODUCTION || eConnectionType == TRADE_CONNECTION_FOOD || (MOD_TRADE_WONDER_RESOURCE_ROUTES && eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE))
#else
	else if (eConnectionType == TRADE_CONNECTION_PRODUCTION || eConnectionType == TRADE_CONNECTION_FOOD)
#endif
	{
		// can't have production or food connections internationally
		if (eOriginTeam != eDestTeam)
		{
			return false;
		}

		if (eConnectionType == TRADE_CONNECTION_FOOD)
		{
			if (!GET_PLAYER(pOriginCity->getOwner()).IsFoodRoutesAllCities() && !pOriginCity->IsFoodRoutes())
			{
				return false;
			}
		}
		else if (eConnectionType == TRADE_CONNECTION_PRODUCTION)
		{
			if (!GET_PLAYER(pOriginCity->getOwner()).IsProductionRoutesAllCities() && !pOriginCity->IsProductionRoutes())
			{
				return false;
			}
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
		}
		else if (eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE)
		{
			bool bAllowsWonderResourceConnection = false;
			ResourceTypes eWonderResource = ::getWonderResource();

			// We need the wonder resource at pOriginCity AND not at pDestCity
			if (eWonderResource != NO_RESOURCE) {
				bAllowsWonderResourceConnection	= (pOriginCity->GetNumResourceLocal(eWonderResource, true) > 0 && !pDestCity->IsHasResourceLocal(eWonderResource, true));
			}

			if (bAllowsWonderResourceConnection) {
				// No existing resource trade route from pOriginCity (ie we can't ship it out twice)
				int iOriginX = pOriginCity->getX();
				int iOriginY = pOriginCity->getY();

				for (uint i = 0; i < m_aTradeConnections.size(); i++) {
					if (m_aTradeConnections[i].m_eConnectionType == eConnectionType && m_aTradeConnections[i].m_iOriginX == iOriginX && m_aTradeConnections[i].m_iOriginY == iOriginY) {
						return false;
					}
				}
			}

			if (!bAllowsWonderResourceConnection)
			{
				return false;
			}
#endif
		}
#if defined(MOD_BUGFIX_MINOR)
	}
	else
	{
		// Unknown trade connection type
		return false;
#endif
	}

	// teams at war can't fight
	if (GET_TEAM(eOriginTeam).isAtWar(eDestTeam))
	{
		return false;
	}

	// teams that have not met cannot trade
	if (!GET_TEAM(eOriginTeam).isHasMet(eDestTeam))
	{
		return false;
	}

	if (pOriginCity->IsRazing() || pDestCity->IsRazing())
	{
		return false;
	}

	if (!bIgnoreExisting)
	{
		int iOriginX = pOriginCity->getX();
		int iOriginY = pOriginCity->getY();
		int iDestX = pDestCity->getX();
		int iDestY = pDestCity->getY();

		// check for duplicate routes
		for (uint i = 0; i < m_aTradeConnections.size(); i++)
		{
			if (m_aTradeConnections[i].m_iOriginX == iOriginX && m_aTradeConnections[i].m_iOriginY == iOriginY && m_aTradeConnections[i].m_iDestX == iDestX && m_aTradeConnections[i].m_iDestY == iDestY)
			{
				return false;
			}
		}
#if defined(MOD_BALANCE_CORE)
		//Only matters if there's more than one option on the table.
		if (MOD_TRADE_ROUTE_SCALING && eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
		{
			// check for duplicate routes from the owner
			for (uint i = 0; i < m_aTradeConnections.size(); i++)
			{
				if (m_aTradeConnections[i].m_eOriginOwner != pOriginCity->getOwner())
					continue;

				if (m_aTradeConnections[i].m_iDestX == iDestX && m_aTradeConnections[i].m_iDestY == iDestY)
				{
					return false;
				}
			}
		}
#endif
	}

	if (bCheckPath && !IsValidTradeRoutePath(pOriginCity, pDestCity, eDomain))
	{
		return false;
	}

#if defined(MOD_EVENTS_TRADE_ROUTES)
	if (MOD_EVENTS_TRADE_ROUTES) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanCreateTradeRoute, pOriginCity->getOwner(), pOriginCity->GetID(), pDestCity->getOwner(), pDestCity->GetID(), eDomain, eConnectionType) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif				
	
	return true;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::CanCreateTradeRoute(PlayerTypes eOriginPlayer, PlayerTypes eDestPlayer, DomainTypes eDomainRestriction)
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	int iOriginCityLoop;
	CvCity* pOriginLoopCity = NULL;
	for(pOriginLoopCity = GET_PLAYER(eOriginPlayer).firstCity(&iOriginCityLoop); pOriginLoopCity != NULL; pOriginLoopCity = GET_PLAYER(eOriginPlayer).nextCity(&iOriginCityLoop))
	{
		int iDestCityLoop;
		CvCity* pDestLoopCity = NULL;
		for(pDestLoopCity = GET_PLAYER(eDestPlayer).firstCity(&iDestCityLoop); pDestLoopCity != NULL; pDestLoopCity = GET_PLAYER(eDestPlayer).nextCity(&iDestCityLoop))
		{
			for (uint uiDomain = 0; uiDomain < NUM_DOMAIN_TYPES; uiDomain++)
			{
				DomainTypes eDomain = (DomainTypes)uiDomain;
				// check only the domain we want
				if (eDomainRestriction != NO_DOMAIN && eDomainRestriction != eDomain)
				{
					continue;
				}

				if (eDomain != DOMAIN_LAND && eDomain != DOMAIN_SEA)
				{
					continue;
				}

				for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
				{
					// Check the route, but not the path
					if (CanCreateTradeRoute(pOriginLoopCity, pDestLoopCity, eDomain, (TradeConnectionType)uiConnectionTypes, false, false))
					{
						// Check the path
						if (pGameTrade->IsValidTradeRoutePath(pOriginLoopCity, pDestLoopCity, eDomain))
							return true;

						// else we can just break out of the loop
						break;
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::CreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, int& iRouteID)
{
	iRouteID = -1;

	//don't need to check path here, will do so below
	if (!CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnectionType, false, false))
	{
		return false;
	}

	PlayerTypes eOriginPlayer = pOriginCity->getOwner();
	PlayerTypes eDestPlayer = pDestCity->getOwner();

	//get path from cache
	SPath path;
	if (!HavePotentialTradePath( (eDomain == DOMAIN_SEA), pOriginCity, pDestCity, &path))
	{
		return false;
	}

	int iNewTradeRouteIndex = GetEmptyTradeRouteIndex();
	CvAssertMsg(iNewTradeRouteIndex < (int)m_aTradeConnections.size(), "iNewTradeRouteIndex out of bounds");

	// couldn't find valid connection, bail out
	if (iNewTradeRouteIndex < 0)
	{
		TradeConnection kNewTradeConnection;
		m_aTradeConnections.push_back(kNewTradeConnection);
		iNewTradeRouteIndex = m_aTradeConnections.size() - 1;
	}

	iRouteID = m_iNextID;
	m_aTradeConnections[iNewTradeRouteIndex].m_iID = m_iNextID;
	m_aTradeConnections[iNewTradeRouteIndex].SetCities(pOriginCity,pDestCity);
	m_aTradeConnections[iNewTradeRouteIndex].m_eDomain = eDomain;
	m_aTradeConnections[iNewTradeRouteIndex].m_eConnectionType = eConnectionType;
	m_aTradeConnections[iNewTradeRouteIndex].m_unitID = -1;

	// increment m_iNextID for the next connection
	m_iNextID += 1;

	CopyPathIntoTradeConnection(path, &(m_aTradeConnections[iNewTradeRouteIndex]));

	// try to make the trade units move faster on "faster" routes
	m_aTradeConnections[iNewTradeRouteIndex].m_iSpeedFactor = (100 * path.length()) / path.iNormalizedDistance;

	// reveal all plots to the player who created the trade route
	TeamTypes eOriginTeam = GET_PLAYER(eOriginPlayer).getTeam();
	for (uint ui = 0; ui < m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size(); ui++)
	{
		GC.getMap().plot(m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iX, m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iY)->setRevealed(eOriginTeam, true, NULL, true);
	}

	m_aTradeConnections[iNewTradeRouteIndex].m_iTradeUnitLocationIndex = 0;
	m_aTradeConnections[iNewTradeRouteIndex].m_bTradeUnitMovingForward = true;

	int iCircuitsToComplete = 0;
	int iTurns = GetTradeRouteTurns(pOriginCity, pDestCity, eDomain, &iCircuitsToComplete);
	m_aTradeConnections[iNewTradeRouteIndex].m_iCircuitsCompleted = 0;
	m_aTradeConnections[iNewTradeRouteIndex].m_iCircuitsToComplete = iCircuitsToComplete;
	m_aTradeConnections[iNewTradeRouteIndex].m_iTurnRouteComplete = iTurns + GC.getGame().getGameTurn();

#if defined(MOD_API_TRADEROUTES)
	m_aTradeConnections[iNewTradeRouteIndex].m_bTradeUnitRecalled = false;
#endif

	GET_PLAYER(eOriginPlayer).GetTrade()->UpdateTradeConnectionValues();
	if (eDestPlayer != eOriginPlayer)
	{
		GET_PLAYER(eDestPlayer).GetTrade()->UpdateTradeConnectionValues();
	}

	pOriginCity->GetCityReligions()->UpdateNumTradeRouteConnections(pDestCity);
	pDestCity->GetCityReligions()->UpdateNumTradeRouteConnections(pOriginCity);

	CreateTradeUnitForRoute(iNewTradeRouteIndex);
	MoveUnit(iNewTradeRouteIndex);

#if defined(MOD_BALANCE_CORE_POLICIES)
	if(MOD_BALANCE_CORE_POLICIES && (GET_PLAYER(eOriginPlayer).IsGoldInternalTrade()) && ((eConnectionType == TRADE_CONNECTION_FOOD) || (eConnectionType == TRADE_CONNECTION_PRODUCTION)))
	{
		GET_PLAYER(eOriginPlayer).GetTreasury()->DoUpdateCityConnectionGold();
	}
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	if(MOD_BALANCE_CORE_POLICIES && MOD_TRADE_WONDER_RESOURCE_ROUTES && (GET_PLAYER(eOriginPlayer).IsGoldInternalTrade()) && (eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE))
	{
		GET_PLAYER(eOriginPlayer).GetTreasury()->DoUpdateCityConnectionGold();
	}
#endif
#endif
#if defined(MOD_BALANCE_CORE_DEALS)
	if (eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		if (pDestCity->getOwner() != NO_PLAYER && GET_PLAYER(pDestCity->getOwner()).isMajorCiv())
		{
			int iGrowthTourism = GET_PLAYER(pOriginCity->getOwner()).GetCulture()->GetInfluenceTradeRouteGrowthBonus(pDestCity->getOwner());
			if (iGrowthTourism != 0)
			{
				pOriginCity->ChangeGrowthFromTourism(iGrowthTourism);
			}
		}

	}
	if(MOD_BALANCE_CORE_DEALS && (eConnectionType == TRADE_CONNECTION_INTERNATIONAL))
	{
		if(!GET_PLAYER(eDestPlayer).isHuman() && !GET_PLAYER(eDestPlayer).isMinorCiv() && !GET_PLAYER(eOriginPlayer).isMinorCiv())
		{
			int iFlavorGoldDest = GET_PLAYER(eDestPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			int iFlavorScienceDest = GET_PLAYER(eDestPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			int iFlavorDiplomacyDest = GET_PLAYER(eDestPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
			int iFlavorCultureDest = GET_PLAYER(eDestPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			int iGoldDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_GOLD] / 10);
			int iScienceDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_SCIENCE] / 10);
			int iCultureDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_CULTURE] / 10);
			int iTradeValueDest = ((iScienceDest + iCultureDest + iGoldDest + iFlavorCultureDest + iFlavorGoldDest + iFlavorScienceDest + iFlavorDiplomacyDest) / 4);
			if(iTradeValueDest > 0)
			{
				GET_PLAYER(eDestPlayer).GetDiplomacyAI()->ChangeRecentTradeValue(eOriginPlayer, iTradeValueDest);
			}
		}
		if(!GET_PLAYER(eOriginPlayer).isHuman() && !GET_PLAYER(eOriginPlayer).isMinorCiv() && !GET_PLAYER(eDestPlayer).isMinorCiv())
		{
			int iFlavorGoldOrigin = GET_PLAYER(eOriginPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			int iFlavorScienceOrigin = GET_PLAYER(eOriginPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			int iFlavorDiplomacyOrigin = GET_PLAYER(eOriginPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
			int iFlavorCultureOrigin = GET_PLAYER(eOriginPlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			int iGoldOrigin = (m_aTradeConnections[iNewTradeRouteIndex].m_aiOriginYields[YIELD_GOLD] / 10);
			int iScienceOrigin = (m_aTradeConnections[iNewTradeRouteIndex].m_aiOriginYields[YIELD_SCIENCE] / 10);
			int iCultureDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_CULTURE] / 10);
			int iTradeValueOrigin = ((iScienceOrigin + iCultureDest + iFlavorCultureOrigin + iGoldOrigin + iFlavorGoldOrigin + iFlavorScienceOrigin + iFlavorDiplomacyOrigin) / 4);
			if(iTradeValueOrigin > 0)
			{
				GET_PLAYER(eOriginPlayer).GetDiplomacyAI()->ChangeRecentTradeValue(eDestPlayer, iTradeValueOrigin);
			}
		}
	}
	if(MOD_BALANCE_CORE_HAPPINESS)
	{
		GET_PLAYER(eOriginPlayer).CalculateNetHappiness();
		if(eOriginPlayer != eDestPlayer)
		{
			GET_PLAYER(eDestPlayer).CalculateNetHappiness();
		}
	}
#endif

	if(GC.getLogging())
	{
		CvString strMsg;
		CvString strDomain;
		switch (eDomain)
		{
		case DOMAIN_LAND:
			strDomain = "land";
			break;
		case DOMAIN_SEA:
			strDomain = "sea";
			break;
		}

		CvString strTRType;
		switch (eConnectionType)
		{
		case TRADE_CONNECTION_FOOD:
			strTRType = "food";
			break;
		case TRADE_CONNECTION_PRODUCTION:
			strTRType = "production";
			break;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
		case TRADE_CONNECTION_WONDER_RESOURCE:
			strTRType = "wonder resource";
			break;
#endif
		case TRADE_CONNECTION_INTERNATIONAL:
			strTRType = "international";
			break;
		}

		strMsg.Format("%s, New Trade Route, %s, %s, %s, %i, %s", 
			GET_PLAYER(pOriginCity->getOwner()).getCivilizationShortDescription(), strDomain.c_str(), 
			pOriginCity->getName().c_str(), pDestCity->getName().c_str(), 
			m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size(), strTRType.c_str());
		LogTradeMsg(strMsg);
	}
#if defined(MOD_BALANCE_CORE)
	 if(m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size() > 0)
	 {
		for (uint ui = 0; ui < m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size(); ui++)
		{
			int iPlotX = m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iX;
			int iPlotY = m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iY;
			if(iPlotX != NULL && iPlotY != NULL)
			{
				CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
				if(pPlot != NULL)
				{
					pPlot->updateYield();
				}
			}
		}
	}
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	if(pTrade)
	{
		pTrade->UpdateTradePlots();
	}
#endif
	

	return true;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsValidTradeRoutePath(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPath, bool bWarCheck)
{
	SPath path;
	//if we did not get an external pointer, make up our own
	if (!pPath)
		pPath = &path;

	if (bWarCheck && GET_PLAYER(pOriginCity->getOwner()).IsAtWarWith(pDestCity->getOwner()))
		return false;

	if (HavePotentialTradePath(eDomain==DOMAIN_SEA,pOriginCity,pDestCity,pPath))
	{
		// check if beyond the origin player's trade range
		int iMaxNormDist = GET_PLAYER(pOriginCity->getOwner()).GetTrade()->GetTradeRouteRange(eDomain, pOriginCity);

		int iNormDist = pPath->iNormalizedDistance;
		if (iNormDist>0 && iNormDist<=iMaxNormDist)
			return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetValidTradeRoutePathLength(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, SPath* pPath, bool bWarCheck)
{
	SPath path;
	//if we did not get an external pointer, make up our own
	if (!pPath)
		pPath = &path;

	if (bWarCheck && GET_PLAYER(pOriginCity->getOwner()).IsAtWarWith(pDestCity->getOwner()))
		return false;

	if (HavePotentialTradePath(eDomain == DOMAIN_SEA, pOriginCity, pDestCity, pPath))
	{
		// check if beyond the origin player's trade range
		int iMaxNormDist = GET_PLAYER(pOriginCity->getOwner()).GetTrade()->GetTradeRouteRange(eDomain, pOriginCity);

		int iNormDist = pPath->iNormalizedDistance;
		if (iNormDist>0 && iNormDist <= iMaxNormDist)
			return iNormDist;
	}

	return -1;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsDestinationExclusive(const TradeConnection& kTradeConnection)
{
	TradeConnection* pConnection = NULL;
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		pConnection = &(m_aTradeConnections[ui]);
		if (pConnection->m_iDestX == kTradeConnection.m_iDestX && pConnection->m_iDestY == kTradeConnection.m_iDestY)
		{
			// if we're double counting the same route, ignore
			if (pConnection->m_iOriginX == kTradeConnection.m_iOriginX && pConnection->m_iOriginY == kTradeConnection.m_iOriginY)
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsConnectionInternational	(const TradeConnection& kConnection)
{
	TeamTypes eOriginTeam = NO_TEAM;
	TeamTypes eDestTeam = NO_TEAM;

	if (kConnection.m_eOriginOwner != NO_PLAYER)
	{
		eOriginTeam = GET_PLAYER(kConnection.m_eOriginOwner).getTeam();
	}

	if (kConnection.m_eDestOwner != NO_PLAYER)
	{
		eDestTeam = GET_PLAYER(kConnection.m_eDestOwner).getTeam();
	}

	return (eOriginTeam != eDestTeam);
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsCityConnectedToPlayer (CvCity* pCity, PlayerTypes eOtherPlayer, bool bOnlyOwnedByCityOwner)
{
	PlayerTypes eCityOwnerPlayer = pCity->getOwner();
	int iCityX = pCity->getX();
	int iCityY = pCity->getY();

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}
		
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (pConnection->m_eOriginOwner == eCityOwnerPlayer && pConnection->m_eDestOwner == eOtherPlayer)
		{
			if (pConnection->m_iOriginX == iCityX && pConnection->m_iOriginY == iCityY)
			{
				return true;
			}
		}
		else if (!bOnlyOwnedByCityOwner && pConnection->m_eOriginOwner == eOtherPlayer && pConnection->m_eDestOwner == eCityOwnerPlayer)
		{
			if (pConnection->m_iDestX == iCityX && pConnection->m_iDestY == iCityY)
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvGameTrade::CopyPathIntoTradeConnection (const SPath& path, TradeConnection* pTradeConnection)
{
	//todo: remove this unnecessary conversion step ...
	for (size_t i = 0; i<path.vPlots.size(); i++)
	{
		TradeConnectionPlot kTradeConnectionPlot;
		kTradeConnectionPlot.m_iX = path.vPlots[i].x;
		kTradeConnectionPlot.m_iY = path.vPlots[i].y;
		pTradeConnection->m_aPlotList.push_back(kTradeConnectionPlot);
	}
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetDomainModifierTimes100 (DomainTypes eDomain)
{
	if (eDomain == DOMAIN_SEA)
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		return GD_INT_GET(TRADE_ROUTE_BASE_SEA_MODIFIER);
#else
		return 100;
#endif
	}
	else
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		return GD_INT_GET(TRADE_ROUTE_BASE_LAND_MODIFIER);
#else
		return 0;
#endif
	}
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsPlayerConnectedToPlayer(PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer, bool bFirstPlayerOnly)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == eFirstPlayer && pConnection->m_eDestOwner == eSecondPlayer)
		{
			return true;
		}
		else if (!bFirstPlayerOnly && pConnection->m_eOriginOwner == eSecondPlayer && pConnection->m_eDestOwner == eFirstPlayer)
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvGameTrade::CountNumPlayerConnectionsToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	int iCount = 0;

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == eFirstPlayer && pConnection->m_eDestOwner == eSecondPlayer)
		{
			iCount++;
		}
		else if (pConnection->m_eOriginOwner == eSecondPlayer && pConnection->m_eDestOwner == eFirstPlayer)
		{
			iCount++;
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::CitiesHaveTradeConnection(CvCity* pFirstCity, CvCity* pSecondCity)
{
	int iFirstCityX = pFirstCity->getX();
	int iFirstCityY = pFirstCity->getY();
	int iSecondCityX = pSecondCity->getX();
	int iSecondCityY = pSecondCity->getY();

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_iOriginX == iFirstCityX && pConnection->m_iOriginY == iFirstCityY && pConnection->m_iDestX == iSecondCityX && pConnection->m_iDestY == iSecondCityY)
		{
			return true;
		}
		else if (pConnection->m_iOriginX == iSecondCityX && pConnection->m_iOriginY == iSecondCityY && pConnection->m_iDestX == iFirstCityX && pConnection->m_iDestY == iFirstCityY)
		{
			return true;
		}
	}

	return false;	
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetNumTimesOriginCity (CvCity* pCity, bool bOnlyInternational)
{
	int iCount = 0;
	int iCityX = pCity->getX();
	int iCityY = pCity->getY();

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_iOriginX == iCityX && pConnection->m_iOriginY == iCityY)
		{
			if (bOnlyInternational)
			{
				if (pConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
				{
					iCount++;
				}
			}
			else
			{
				iCount++;
			}
		}
	}

	return iCount;	
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetNumTimesDestinationCity (CvCity* pCity, bool bOnlyInternational)
{
	int iCount = 0;
	int iCityX = pCity->getX();
	int iCityY = pCity->getY();

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		TradeConnection* pConnection = &(m_aTradeConnections[ui]);

		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_iDestX == iCityX && pConnection->m_iDestY == iCityY)
		{
			if (bOnlyInternational)
			{
				if (pConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
				{
					iCount++;
				}
			}
			else
			{
				iCount++;
			}
		}
	}

	return iCount;	
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetEmptyTradeRouteIndex (void)
{
	for (uint i = 0; i < m_aTradeConnections.size(); i++)
	{
		if (IsTradeRouteIndexEmpty(i))
		{
			return i;
		}
	}

	return -1;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::IsTradeRouteIndexEmpty(int iIndex)
{
	if (m_aTradeConnections[iIndex].m_iOriginX == -1 && m_aTradeConnections[iIndex].m_iOriginY == -1 && m_aTradeConnections[iIndex].m_iDestX == -1 && m_aTradeConnections[iIndex].m_iDestY == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::EmptyTradeRoute(int iIndex)
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return false;
	}

	TradeConnection& kTradeConnection = m_aTradeConnections[iIndex];
	PlayerTypes eOriginPlayer = kTradeConnection.m_eOriginOwner;
	PlayerTypes eDestPlayer = kTradeConnection.m_eDestOwner;

	// Remove any visualization
	if (kTradeConnection.m_unitID != -1)
	{
		CvUnit* pkUnit = GET_PLAYER(kTradeConnection.m_eOriginOwner).getUnit(kTradeConnection.m_unitID);
		if (pkUnit)
		{
			pkUnit->kill(false);
		}
	}
#if defined(MOD_BALANCE_CORE)
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		int iDestX = kTradeConnection.m_iDestX;
		int iDestY = kTradeConnection.m_iDestY;
		int iOriginX = kTradeConnection.m_iOriginX;
		int iOriginY = kTradeConnection.m_iOriginY;
		//Free lump resource when you complete an international trade route.
		CvPlot* pOriginPlot = GC.getMap().plot(iOriginX, iOriginY);
		CvPlot* pDestPlot = GC.getMap().plot(iDestX, iDestY);

		CvCity* pOriginCity = NULL;
		CvCity* pDestCity = NULL;
		if (pOriginPlot != NULL)
		{
			pOriginCity = pOriginPlot->getPlotCity();
		}
		if (pDestPlot != NULL)
		{
			pDestCity = pDestPlot->getPlotCity();
		}
		if (pOriginCity != NULL && pDestCity != NULL)
		{
			if (pDestCity->getOwner() != NO_PLAYER && GET_PLAYER(pDestCity->getOwner()).isMajorCiv())
			{
				int iGrowthTourism = GET_PLAYER(pOriginCity->getOwner()).GetCulture()->GetInfluenceTradeRouteGrowthBonus(pDestCity->getOwner());
				if (iGrowthTourism != 0)
				{
					pOriginCity->ChangeGrowthFromTourism(-iGrowthTourism);
				}
			}
		}
	}
#endif

	//reset to default
	kTradeConnection = TradeConnection();

	GET_PLAYER(eOriginPlayer).GetTrade()->UpdateTradeConnectionValues();
	GET_PLAYER(eDestPlayer).GetTrade()->UpdateTradeConnectionValues();

	gDLL->TradeVisuals_DestroyRoute(iIndex, eOriginPlayer);
#if defined(MOD_BALANCE_CORE)
	UpdateTradePlots();
#endif
	return true;
}
#if defined(MOD_BALANCE_CORE)
void CvGameTrade::UpdateTradePlots()
{
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for(int iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(pLoopPlot == NULL)
			continue;

		pLoopPlot->SetTradeUnitRoute(false);
	}

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
			continue;

		for (uint uiPlot = 0; uiPlot < m_aTradeConnections[ui].m_aPlotList.size(); uiPlot++)
		{
			CvPlot *pPlot = GC.getMap().plot( m_aTradeConnections[ui].m_aPlotList[uiPlot].m_iX, m_aTradeConnections[ui].m_aPlotList[uiPlot].m_iY );
			if (pPlot)
				pPlot->SetTradeUnitRoute(true);
		}
	}
}
//	--------------------------------------------------------------------------------
//	Returns number of turns to complete the TR between two cities
//	iCircuitsToComplete calculated so it can be used in CreateTradeRoute()
int CvGameTrade::GetTradeRouteTurns(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, int* piCircuitsToComplete)
{
	// calculate distance
	SPath path;
	bool bTradeAvailable = GC.getGame().GetGameTrade()->IsValidTradeRoutePath(pOriginCity, pDestCity, eDomain, &path);
	if (!bTradeAvailable)
		return -1;
	int iDistance = path.length();

	// calculate turns per circuit
	int iRawSpeed = GET_PLAYER(pOriginCity->getOwner()).GetTrade()->GetTradeRouteSpeed(eDomain);
	int iSpeedFactor = (100 * path.length() / path.iNormalizedDistance);
	int iRouteSpeed = int(0.5f + iSpeedFactor*iRawSpeed / 100.f);

	int iTurnsPerCircuit = 1; // real number of turns to complete a circuit
	if (iRouteSpeed != 0)
		iTurnsPerCircuit = ((iDistance - 1) * 2) / iRouteSpeed; // need to check why there is -1 and not simply distance * 2
	
#if defined(MOD_TRADE_ROUTE_SCALING)
	int iTargetTurns = GD_INT_GET(TRADE_ROUTE_BASE_TARGET_TURNS); // how many turns do we want the cycle to consume
	int iEra = max((int)GET_PLAYER(pOriginCity->getOwner()).GetCurrentEra(), 1);
	iTargetTurns -= iEra;
	iTargetTurns = iTargetTurns * GC.getGame().getGameSpeedInfo().getTradeRouteSpeedMod() / 100;
	iTargetTurns = max(iTargetTurns, 1);
#else
	int iTargetTurns = 30; // how many turns do we want the cycle to consume
#endif

	// calculate how many circuits do we want this trade route to run to reach the target turns
	int iCircuitsToComplete = 1; 
	if (iTurnsPerCircuit != 0)
		iCircuitsToComplete = max(iTargetTurns / iTurnsPerCircuit, 2);

	// return values
	if (piCircuitsToComplete != NULL) *piCircuitsToComplete = iCircuitsToComplete;
	return iTurnsPerCircuit * iCircuitsToComplete;
}
#endif
//	--------------------------------------------------------------------------------
/// Called when a city changes hands
#if defined(MOD_BUGFIX_MINOR)
void CvGameTrade::ClearAllCityTradeRoutes (CvPlot* pPlot, bool bIncludeTransits)
#else
void CvGameTrade::ClearAllCityTradeRoutes (CvPlot* pPlot)
#endif
{
	CvAssert(pPlot != NULL);

	if (pPlot)
	{
		int iX = pPlot->getX();
		int iY = pPlot->getY();
		for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
		{
			if (IsTradeRouteIndexEmpty(ui))
			{
				continue;
			}

			bool bMatchesOrigin = (m_aTradeConnections[ui].m_iOriginX == iX && m_aTradeConnections[ui].m_iOriginY == iY);
			bool bMatchesDest = (m_aTradeConnections[ui].m_iDestX == iX && m_aTradeConnections[ui].m_iDestY == iY);

			if (bMatchesOrigin || bMatchesDest)
			{
				// if the destination was wiped, the origin gets a trade unit back
				if (bMatchesDest && GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isAlive())
				{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
					CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
					UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);
#else
					UnitTypes eUnitType = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain);
#endif
					CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
					if (eUnitType != NO_UNIT)
					{
						GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
					}
				}

				EmptyTradeRoute(ui);
			}		
			
#if defined(MOD_BUGFIX_MINOR)
			// If we have any water routes transiting via this city, we need to cancel them
			if (bIncludeTransits && m_aTradeConnections[ui].m_eDomain == DOMAIN_SEA) {
				TradeConnectionPlotList aPlotList = m_aTradeConnections[ui].m_aPlotList;

				for (uint uiPlotIndex = 0; uiPlotIndex < aPlotList.size(); uiPlotIndex++) {
					if (aPlotList[uiPlotIndex].m_iX == iX && aPlotList[uiPlotIndex].m_iY == iY) {
						CUSTOMLOG("Cancelling water trade route in plot (%i, %i) as city destroyed", iX, iY);
						m_aTradeConnections[ui].m_iCircuitsCompleted = m_aTradeConnections[ui].m_iCircuitsToComplete;
						break;
					}
				}
			}
#endif
		}
	}
}

//	--------------------------------------------------------------------------------
//  Reset all Civ to Civ trade routes involving ePlayer.  Trade routes involving city-states are not reset.
void CvGameTrade::ClearAllCivTradeRoutes (PlayerTypes ePlayer)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		bool bMatchesOrigin = (m_aTradeConnections[ui].m_eOriginOwner == ePlayer);
		bool bMatchesDest = (m_aTradeConnections[ui].m_eDestOwner == ePlayer);
		bool bCityStateRoute = (GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isMinorCiv() || GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).isMinorCiv());

		if (!bCityStateRoute)
		{
			if (bMatchesOrigin && bMatchesDest)
			{
				// Internal trade route - don't wipe it
				continue;
			}
			else if (bMatchesOrigin || bMatchesDest)
			{
				// if the destination was wiped, the origin gets a trade unit back
				if (GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isAlive())
				{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
					CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
					UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);
#else
					UnitTypes eUnitType = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain);
#endif
					CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
					if (eUnitType != NO_UNIT)
					{
						GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
					}
				}

				EmptyTradeRoute(ui);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
//  Reset all Civ to City-State trade routes for all players.
void CvGameTrade::ClearAllCityStateTradeRoutes (void)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		bool bMatchesDest = (GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).isMinorCiv());
		if (bMatchesDest)
		{
			// if the destination was wiped, the origin gets a trade unit back
			if (GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isAlive())
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
				UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);
#else
				UnitTypes eUnitType = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain);
#endif
				CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
				if (eUnitType != NO_UNIT)
				{
					GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
				}
			}

			EmptyTradeRoute(ui);
		}		
	}
}
#if defined(MOD_BALANCE_CORE)
//  Reset all Civ to City-State trade routes for all players.
void CvGameTrade::ClearAllCityStateTradeRoutesSpecial (void)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		bool bMatchesDest = (GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).isMinorCiv());
		if(bMatchesDest)
		{
			if(GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).GetMinorCivAI()->GetAlly() == m_aTradeConnections[ui].m_eOriginOwner)
			{
				bMatchesDest = false;
			}
		}
		if (bMatchesDest)
		{
			// if the destination was wiped, the origin gets a trade unit back
			if (GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isAlive())
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
				UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);
#else
				UnitTypes eUnitType = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain);
#endif
				CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
				if (eUnitType != NO_UNIT)
				{
					GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
				}
			}

			EmptyTradeRoute(ui);
		}		
	}
}
//	--------------------------------------------------------------------------------
//  Reset all Civ to Civ trade routes involving ePlayer and eToPlayer.  Trade routes involving city-states are not reset.
void CvGameTrade::ClearTradePlayerToPlayer(PlayerTypes ePlayer, PlayerTypes eToPlayer)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}
		if(GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isMinorCiv() || GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).isMinorCiv())
		{
			continue;
		}
		//Ignore internal routes.
		if(m_aTradeConnections[ui].m_eOriginOwner == ePlayer && m_aTradeConnections[ui].m_eDestOwner == ePlayer)
		{
			continue;
		}
		//Ignore internal routes.
		if(m_aTradeConnections[ui].m_eOriginOwner == eToPlayer && m_aTradeConnections[ui].m_eDestOwner == eToPlayer)
		{
			continue;
		}

		//Origin one of these two?
		if(m_aTradeConnections[ui].m_eOriginOwner == ePlayer || m_aTradeConnections[ui].m_eOriginOwner == eToPlayer)
		{
			//Destination one of these two?
			if(m_aTradeConnections[ui].m_eDestOwner == ePlayer || m_aTradeConnections[ui].m_eDestOwner == eToPlayer)
			{
				// if the destination was wiped, the origin gets a trade unit back
				if (GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).isAlive())
				{
					CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
					UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);
					CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
					if (eUnitType != NO_UNIT)
					{
						GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
					}
				}

				EmptyTradeRoute(ui);
			}
		}
	}
}
#endif
//	--------------------------------------------------------------------------------
/// Called when war is declared between teams
void CvGameTrade::CancelTradeBetweenTeams (TeamTypes eTeam1, TeamTypes eTeam2)
{
	// there shouldn't be trade routes between the same team
	if (eTeam1 == eTeam2)
	{
		return;
	}

	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		TeamTypes eOriginTeam = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).getTeam();
		TeamTypes eDestTeam = GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).getTeam();
		if ((eOriginTeam == eTeam1 && eDestTeam == eTeam2) || (eOriginTeam == eTeam2 && eDestTeam == eTeam1)) 
		{
			EmptyTradeRoute(ui);
		}
	}
}

//	--------------------------------------------------------------------------------
// when war is declared, both sides plunder each others trade routes for cash!
void CvGameTrade::DoAutoWarPlundering(TeamTypes eTeam1, TeamTypes eTeam2)
{
	// walk through each team
	for (uint uiTeam = 0; uiTeam < 2; uiTeam++)
	{
		TeamTypes eTRTeam = NO_TEAM;
		TeamTypes ePlunderTeam = NO_TEAM;
		switch (uiTeam)
		{
		case 0:
			eTRTeam = eTeam1;
			ePlunderTeam = eTeam2;
			break;
		case 1:
			eTRTeam = eTeam2;
			ePlunderTeam = eTeam1;
		}

		// walk through each player on the team
		for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
		{
			PlayerTypes ePlayer = (PlayerTypes)uiPlayer;
			if (GET_PLAYER(ePlayer).getTeam() != eTRTeam)
			{
				continue;
			}
			
			// walk through each of the trade routes
			for (uint uiTradeRoute = 0; uiTradeRoute < m_aTradeConnections.size(); uiTradeRoute++)
			{
				if (IsTradeRouteIndexEmpty(uiTradeRoute))
				{
					continue;
				}

				// if it's not my trade route
				if (m_aTradeConnections[uiTradeRoute].m_eOriginOwner != ePlayer)
				{
					continue;
				}

				// if the destination is not the civ I'm going to war with
				if (GET_PLAYER(m_aTradeConnections[uiTradeRoute].m_eDestOwner).getTeam() != ePlunderTeam)
				{
					continue;
				}

				// get the plot where the trade route is
				int iLocationIndex = m_aTradeConnections[uiTradeRoute].m_iTradeUnitLocationIndex;
				int iPlotX = m_aTradeConnections[uiTradeRoute].m_aPlotList[iLocationIndex].m_iX;
				int iPlotY = m_aTradeConnections[uiTradeRoute].m_aPlotList[iLocationIndex].m_iY;
				CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);

				// see if there is unit from the other team above it
				IDInfo* pUnitNode = pPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
					if(pLoopUnit)
					{
						if (GET_PLAYER(pLoopUnit->getOwner()).getTeam() == ePlunderTeam)
						{
							if (pLoopUnit->canPlunderTradeRoute(pPlot, true))
							{
								// cheating to get around war!
#if defined(MOD_API_EXTENSIONS)
								GET_PLAYER(pLoopUnit->getOwner()).GetTrade()->PlunderTradeRoute(m_aTradeConnections[uiTradeRoute].m_iID, pLoopUnit);
#else
								GET_PLAYER(pLoopUnit->getOwner()).GetTrade()->PlunderTradeRoute(m_aTradeConnections[uiTradeRoute].m_iID);
#endif
								break;
							}
						}
					}

					pUnitNode = pPlot->nextUnitNode(pUnitNode);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// GetIndexFromID
int CvGameTrade::GetIndexFromID (int iID)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (m_aTradeConnections[ui].m_iID == iID)
		{
			return ui;
		}
	}

	return -1;
}

//	--------------------------------------------------------------------------------
/// GetOwnerFromID
PlayerTypes CvGameTrade::GetOwnerFromID (int iID)
{
	int iIndex = GetIndexFromID(iID);
	if (iIndex < -1)
	{
		return NO_PLAYER;
	}

	return m_aTradeConnections[iIndex].m_eOriginOwner;
}

//	--------------------------------------------------------------------------------
/// GetDestFromID
PlayerTypes CvGameTrade::GetDestFromID (int iID)
{
	int iIndex = GetIndexFromID(iID);
	if (iIndex < -1)
	{
		return NO_PLAYER;
	}

	return m_aTradeConnections[iIndex].m_eDestOwner;
}

//	--------------------------------------------------------------------------------
/// GetIndexFromUnitID
int CvGameTrade::GetIndexFromUnitID(int iUnitID, PlayerTypes eOwner)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
		if (m_aTradeConnections[ui].m_unitID == iUnitID && m_aTradeConnections[ui].m_eOriginOwner == eOwner)
			return ui;
	return -1;
}

//	--------------------------------------------------------------------------------
/// Is this unit id used? This indicates that this unit is automated
bool CvGameTrade::IsUnitIDUsed (int iUnitID)
{
	for (uint ui = 0; ui < m_aTradeConnections.size(); ui++)
	{
		if (m_aTradeConnections[ui].m_unitID == iUnitID)
		{
			return true;
		}
	}

	return false;
}

const TradeConnection * CvGameTrade::GetConnectionFromIndex(int iIndex) const
{
	if (iIndex>=0 && iIndex<(int)m_aTradeConnections.size())
		return &(m_aTradeConnections[iIndex]);
	return NULL;
}

//	--------------------------------------------------------------------------------
CvCity* CvGameTrade::GetOriginCity(const TradeConnection& kTradeConnection)
{
	CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);

	CvAssertMsg(pPlot, "CvPlayerTrade - plot is null");
	if (!pPlot)
	{
		return NULL;
	}

	return pPlot->getPlotCity();
}

//	--------------------------------------------------------------------------------
CvCity* CvGameTrade::GetDestCity(const TradeConnection& kTradeConnection)
{
	CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);

	CvAssertMsg(pPlot, "CvPlayerTrade - plot is null");
	if (!pPlot)
	{
		return NULL;
	}

	return pPlot->getPlotCity();	
}

//	--------------------------------------------------------------------------------
void CvGameTrade::ResetTechDifference ()
{
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
		{
			m_aaiTechDifference[ui][ui2] = -1; // undefined
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGameTrade::BuildTechDifference ()
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return;
	}

	// for each major civ
	for (uint uiPlayer1 = 0; uiPlayer1 < MAX_MAJOR_CIVS; uiPlayer1++)
	{
		PlayerTypes ePlayer1 = (PlayerTypes)uiPlayer1;
		TeamTypes eTeam1 = GET_PLAYER(ePlayer1).getTeam();

		for (uint uiPlayer2 = 0; uiPlayer2 < MAX_MAJOR_CIVS; uiPlayer2++)
		{	
			PlayerTypes ePlayer2 = (PlayerTypes)uiPlayer2;
			TeamTypes eTeam2 = GET_PLAYER(ePlayer2).getTeam();

			if (eTeam1 == eTeam2) 
			{
				m_aaiTechDifference[uiPlayer1][uiPlayer2] = 0;
			}
			else if (!GET_PLAYER(ePlayer1).isAlive() || !GET_PLAYER(ePlayer2).isAlive())
			{
				m_aaiTechDifference[uiPlayer1][uiPlayer2] = 0;
			}
			else
			{
				int iTechDifference = 0;
				
				CvPlayerTechs* pPlayerTechs = GET_PLAYER(ePlayer1).GetPlayerTechs();
				for(int iTechLoop = 0; iTechLoop < pPlayerTechs->GetTechs()->GetNumTechs(); iTechLoop++)
				{
					TechTypes eTech = (TechTypes)iTechLoop;
					bool bPlayer1Knows = GET_TEAM(eTeam1).GetTeamTechs()->HasTech(eTech);
					bool bPlayer2Knows = GET_TEAM(eTeam2).GetTeamTechs()->HasTech(eTech);
					if (bPlayer2Knows && !bPlayer1Knows)
					{
						iTechDifference++;
					}
				}

				m_aaiTechDifference[uiPlayer1][uiPlayer2] = iTechDifference;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetTechDifference (PlayerTypes ePlayer, PlayerTypes ePlayer2)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return 0;
	}

	if (GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
	{
		return 0;
	}

	if (GET_PLAYER(ePlayer2).isMinorCiv() || GET_PLAYER(ePlayer2).isBarbarian())
	{
#if defined(MOD_BALANCE_CORE)
		if(GET_PLAYER(ePlayer2).isMinorCiv() && MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		{
			if(GET_PLAYER(ePlayer2).GetMinorCivAI()->GetAlly() == ePlayer)
			{
				int iAllyScience = GD_INT_GET(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA);
				if(iAllyScience <= 0)
				{
					return 0;
				}
				return 	iAllyScience;
			}
			else if(GET_PLAYER(ePlayer2).GetMinorCivAI()->IsFriends(ePlayer))
			{
				int iFriendScience = GD_INT_GET(TRADE_ROUTE_CS_FRIEND_SCIENCE_DELTA);
				if(iFriendScience <= 0)
				{
					return 0;
				}
				return 	iFriendScience;
			}
		}
#endif
		return 0;
	}

	return m_aaiTechDifference[ePlayer][ePlayer2];
}

//	--------------------------------------------------------------------------------
void CvGameTrade::ResetPolicyDifference()
{
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
		{
			m_aaiPolicyDifference[ui][ui2] = -1; // undefined
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGameTrade::BuildPolicyDifference()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return;
	}

	// for each major civ
	for (uint uiPlayer1 = 0; uiPlayer1 < MAX_MAJOR_CIVS; uiPlayer1++)
	{
		PlayerTypes ePlayer1 = (PlayerTypes)uiPlayer1;
		TeamTypes eTeam1 = GET_PLAYER(ePlayer1).getTeam();

		for (uint uiPlayer2 = 0; uiPlayer2 < MAX_MAJOR_CIVS; uiPlayer2++)
		{
			PlayerTypes ePlayer2 = (PlayerTypes)uiPlayer2;
			TeamTypes eTeam2 = GET_PLAYER(ePlayer2).getTeam();

			if (eTeam1 == eTeam2)
			{
				m_aaiPolicyDifference[uiPlayer1][uiPlayer2] = 0;
			}
			else if (!GET_PLAYER(ePlayer1).isAlive() || !GET_PLAYER(ePlayer2).isAlive())
			{
				m_aaiPolicyDifference[uiPlayer1][uiPlayer2] = 0;
			}
			else
			{		
				CvPlayerPolicies* pPlayerPolicies1 = GET_PLAYER(ePlayer1).GetPlayerPolicies();
				CvPlayerPolicies* pPlayerPolicies2 = GET_PLAYER(ePlayer2).GetPlayerPolicies();
				
				int iPolicyDifference = pPlayerPolicies2->GetNumPoliciesOwned(true, true) - pPlayerPolicies1->GetNumPoliciesOwned(true, true);

				m_aaiPolicyDifference[uiPlayer1][uiPlayer2] = iPolicyDifference;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvGameTrade::GetPolicyDifference(PlayerTypes ePlayer, PlayerTypes ePlayer2)
{
	if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}

	if (GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
	{
		return 0;
	}

	if (!MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		return 0;

	if (GET_PLAYER(ePlayer2).isMinorCiv() || GET_PLAYER(ePlayer2).isBarbarian())
	{
#if defined(MOD_BALANCE_CORE)
		if (GET_PLAYER(ePlayer2).isMinorCiv() && MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		{
			if (GET_PLAYER(ePlayer2).GetMinorCivAI()->GetAlly() == ePlayer)
			{
				int iAllyCulture = GD_INT_GET(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA);
				if (iAllyCulture <= 0)
				{
					return 0;
				}
				return 	iAllyCulture;
			}
			else if (GET_PLAYER(ePlayer2).GetMinorCivAI()->IsFriends(ePlayer))
			{
				int iFriendCulture = GD_INT_GET(TRADE_ROUTE_CS_FRIEND_CULTURE_DELTA);
				if (iFriendCulture <= 0)
				{
					return 0;
				}
				return 	iFriendCulture;
			}
		}
#endif
		return 0;
	}

	return m_aaiPolicyDifference[ePlayer][ePlayer2];
}

#if defined(MOD_API_TRADEROUTES)
bool CvGameTrade::IsRecalledUnit (int iIndex) {
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return false;
	}

	TradeConnection &kTradeConnection = m_aTradeConnections[iIndex];
	return kTradeConnection.m_bTradeUnitRecalled;
}

//	--------------------------------------------------------------------------------
/// recall a trade unit
void CvGameTrade::RecallUnit (int iIndex, bool bImmediate) {
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return;
	}

	TradeConnection &kTradeConnection = m_aTradeConnections[iIndex];
	kTradeConnection.m_iCircuitsCompleted = kTradeConnection.m_iCircuitsToComplete-1;
	kTradeConnection.m_bTradeUnitRecalled = true;

	if (bImmediate) {
		kTradeConnection.m_bTradeUnitMovingForward = false;
	}
}

//	--------------------------------------------------------------------------------
/// end a trade route
void CvGameTrade::EndTradeRoute (int iIndex) {
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return;
	}

	TradeConnection &kTradeConnection = m_aTradeConnections[iIndex];
	kTradeConnection.m_iCircuitsCompleted = kTradeConnection.m_iCircuitsToComplete;
	kTradeConnection.m_bTradeUnitRecalled = true;
}
#endif

//	--------------------------------------------------------------------------------
/// move a trade unit along its path for all its movement points
bool CvGameTrade::MoveUnit (int iIndex) 
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return false;
	}

	TradeConnection &kTradeConnection = m_aTradeConnections[iIndex];
	int iMoves = kTradeConnection.GetMovementSpeed();
	for (int i = 0; i < iMoves; i++)
	{
		if (kTradeConnection.m_iCircuitsCompleted >= kTradeConnection.m_iCircuitsToComplete)
		{
			break;
		}

		bool bAbleToStep = StepUnit(iIndex);
		if (!bAbleToStep)
		{
			break;
		}
	}

	gDLL->TradeVisuals_UpdateRouteDirection(iIndex, kTradeConnection.m_bTradeUnitMovingForward);

	CvUnit *pkUnit = GetTradeUnitForRoute(iIndex);
	if (pkUnit)
	{
#if defined(MOD_BALANCE_CORE)
		//Free resources when your trade units move.
		CvGameTrade* pTrade = GC.getGame().GetGameTrade();
		int iTrIndex = pTrade->GetIndexFromUnitID(pkUnit->GetID(),pkUnit->getOwner());
		if (iTrIndex>=0)
		{
			const TradeConnection* pTradeConnection = pTrade->GetConnectionFromIndex(iTrIndex);
			if (pTradeConnection)
			{
				CvPlot* pPlot = GC.getMap().plot(pTradeConnection->m_iOriginX, pTradeConnection->m_iOriginY);		
				if(pPlot && pPlot->isCity())
				{
					CvCity* pCity = pPlot->getPlotCity();
					if(pCity)
					{
						GET_PLAYER(pCity->getOwner()).doInstantYield(
							INSTANT_YIELD_TYPE_TR_MOVEMENT, false, NO_GREATPERSON, NO_BUILDING, 0, true, 
							NO_PLAYER, NULL, true, pCity, (pTradeConnection->m_eDomain == DOMAIN_SEA));
					}
				}
			}
		}
#endif
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// move a trade unit a single step along its path (called by MoveUnit)
bool CvGameTrade::StepUnit (int iIndex)
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return false;
	}

	// if the unit needs to turn around
	TradeConnection &kTradeConnection = m_aTradeConnections[iIndex];
	bool bAtEndGoingForward = (kTradeConnection.m_bTradeUnitMovingForward && kTradeConnection.m_iTradeUnitLocationIndex >= ((int)kTradeConnection.m_aPlotList.size() - 1));
	bool bAtEndGoingBackward = (!kTradeConnection.m_bTradeUnitMovingForward && kTradeConnection.m_iTradeUnitLocationIndex <= 0);
	if (bAtEndGoingForward || bAtEndGoingBackward)
	{
		kTradeConnection.m_bTradeUnitMovingForward = !kTradeConnection.m_bTradeUnitMovingForward;
	}

	// if moving forward
	if (kTradeConnection.m_bTradeUnitMovingForward)
	{
		kTradeConnection.m_iTradeUnitLocationIndex += 1;
	}
	else // if moving back
	{
		kTradeConnection.m_iTradeUnitLocationIndex -= 1;
		if (kTradeConnection.m_iTradeUnitLocationIndex == 0)
		{
			kTradeConnection.m_iCircuitsCompleted += 1;
		}
	}

	// Move the visualization
	CvUnit *pkUnit = GetTradeUnitForRoute(iIndex);
	if (pkUnit)
	{
		pkUnit->setXY(kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY, true, false, true, true);
		pkUnit->finishMoves();
	}

	// auto-pillage when a trade unit moves under an enemy unit
	CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY);
	if (pPlot)
	{
		CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(kTradeConnection.m_eOriginOwner);
		if (pEnemyUnit)
		{
			if (pEnemyUnit->canPlunderTradeRoute(pPlot, false))
			{
#if defined(MOD_API_EXTENSIONS)
				GET_PLAYER(pEnemyUnit->getOwner()).GetTrade()->PlunderTradeRoute(kTradeConnection.m_iID, pEnemyUnit);
#else
				GET_PLAYER(pEnemyUnit->getOwner()).GetTrade()->PlunderTradeRoute(kTradeConnection.m_iID);
#endif
				// done died!
				return false;
			}
		}
	}

	return true;
}

//	----------------------------------------------------------------------------
void CvGameTrade::CreateTradeUnitForRoute(int iIndex)
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return;
	}

	TradeConnection& kTradeConnection = m_aTradeConnections[iIndex];

	if (kTradeConnection.m_unitID == -1)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
		UnitTypes eUnitType = CvPlayerTrade::GetTradeUnit(kTradeConnection.m_eDomain, &kPlayer);
#else
		UnitTypes eUnitType = CvPlayerTrade::GetTradeUnit(kTradeConnection.m_eDomain);
#endif
		if (eUnitType != NO_UNIT)
		{
#if !defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
#endif
			CvUnit* pkUnit = kPlayer.initUnit(eUnitType, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY, NO_UNITAI, NO_DIRECTION, false, true, TRADE_UNIT_MAP_LAYER);
			if (pkUnit)
			{
				pkUnit->SetAutomateType(AUTOMATE_TRADE);
				kTradeConnection.m_unitID = pkUnit->GetID();
			}
		}
	}
}

//	----------------------------------------------------------------------------
CvUnit* CvGameTrade::GetTradeUnitForRoute(int iIndex)
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
	{
		return NULL;
	}

	TradeConnection& kTradeConnection = m_aTradeConnections[iIndex];
	if (kTradeConnection.m_unitID != -1)
	{
		CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
		return kPlayer.getUnit(kTradeConnection.m_unitID);
	}

	return NULL;
}

//	----------------------------------------------------------------------------
void CvGameTrade::DisplayTemporaryPopupTradeRoute(int iDestX, int iDestY, TradeConnectionType type, DomainTypes eDomain)
{
	int iOriginX,iOriginY;
	PlayerTypes eOriginPlayer;

	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());
	CvAssert(pkSelectedUnit != NULL);
	if (pkSelectedUnit)
	{
		iOriginX = pkSelectedUnit->getX();
		iOriginY = pkSelectedUnit->getY();
		eOriginPlayer = pkSelectedUnit->getOwner();
	}
	else
	{
		return;
	}

	gDLL->TradeVisuals_DestroyRoute(TEMPORARY_POPUPROUTE_ID,GC.getGame().getActivePlayer());

	SPath path;
	CvPlot* pOriginPlot = GC.getMap().plot(iOriginX, iOriginY);
	CvPlot* pDestPlot = GC.getMap().plot(iDestX, iDestY);
	if (pOriginPlot && pDestPlot && pOriginPlot->isCity() && pDestPlot->isCity())
	{
		if (HavePotentialTradePath(eDomain==DOMAIN_SEA,pOriginPlot->getPlotCity(),pDestPlot->getPlotCity(),&path))
		{
			size_t n = path.vPlots.size();
			if (n>0 && n<=MAX_PLOTS_TO_DISPLAY)
			{
				int plotsX[MAX_PLOTS_TO_DISPLAY], plotsY[MAX_PLOTS_TO_DISPLAY];
				for (size_t i=0;i<n;++i)
				{
					plotsX[i] = path.vPlots[i].x;
					plotsY[i] = path.vPlots[i].y;
				}
				gDLL->TradeVisuals_NewRoute(TEMPORARY_POPUPROUTE_ID,eOriginPlayer,type,n,plotsX,plotsY);
				gDLL->TradeVisuals_ActivatePopupRoute(TEMPORARY_POPUPROUTE_ID);
			}
		}
	}

	m_CurrentTemporaryPopupRoute.iPlotX = iDestX;
	m_CurrentTemporaryPopupRoute.iPlotY = iDestY;
	m_CurrentTemporaryPopupRoute.type = type;
}

//	----------------------------------------------------------------------------
void CvGameTrade::HideTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type)
{
	if (iPlotX == m_CurrentTemporaryPopupRoute.iPlotX &&
		iPlotY == m_CurrentTemporaryPopupRoute.iPlotY &&
		type == m_CurrentTemporaryPopupRoute.type) 
	{
		gDLL->TradeVisuals_DeactivatePopupRoute();
		gDLL->TradeVisuals_DestroyRoute(TEMPORARY_POPUPROUTE_ID,GC.getGame().getActivePlayer());
	}
}

//	----------------------------------------------------------------------------
CvString CvGameTrade::GetLogFileName() const
{
	CvString strLogName;
	strLogName = "TradeRouteLog.csv";
	return strLogName;
}

//	----------------------------------------------------------------------------
void CvGameTrade::LogTradeMsg(CvString& strMsg)
{
	if(GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

//	----------------------------------------------------------------------------
/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, TradeConnection& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iID;
	loadFrom >> writeTo.m_iOriginID;
	loadFrom >> writeTo.m_iOriginX;
	loadFrom >> writeTo.m_iOriginY;
	loadFrom >> writeTo.m_iDestID;
	loadFrom >> writeTo.m_iDestX;
	loadFrom >> writeTo.m_iDestY;
	loadFrom >> writeTo.m_eOriginOwner;
	loadFrom >> writeTo.m_eDestOwner;
	int iDomain;
	loadFrom >> iDomain;
	writeTo.m_eDomain = (DomainTypes)iDomain;
	int iConnectionType;
	loadFrom >> iConnectionType;
	writeTo.m_eConnectionType = (TradeConnectionType)iConnectionType;

	loadFrom >> writeTo.m_iTradeUnitLocationIndex;
	loadFrom >> writeTo.m_bTradeUnitMovingForward;

	loadFrom >> writeTo.m_unitID;
	loadFrom >> writeTo.m_iSpeedFactor;
	loadFrom >> writeTo.m_iCircuitsCompleted;
	loadFrom >> writeTo.m_iCircuitsToComplete;
	loadFrom >> writeTo.m_iTurnRouteComplete;

#if defined(MOD_API_TRADEROUTES)
	MOD_SERIALIZE_READ(23, loadFrom, writeTo.m_bTradeUnitRecalled, false);
#endif

	int nPlots;
	loadFrom >> nPlots;
	for (int i2 = 0; i2 < nPlots; i2++)
	{
		TradeConnectionPlot kTradeConnectionPlot;
		writeTo.m_aPlotList.push_back(kTradeConnectionPlot);
		loadFrom >> writeTo.m_aPlotList[i2].m_iX;
		loadFrom >> writeTo.m_aPlotList[i2].m_iY;
		
	}

	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		loadFrom >> writeTo.m_aiOriginYields[ui];
		loadFrom >> writeTo.m_aiDestYields[ui];
	}

	return loadFrom;
}

FDataStream& operator>>(FDataStream& loadFrom, CvGameTrade& writeTo)
{
	int plotsX[MAX_PLOTS_TO_DISPLAY];
	int plotsY[MAX_PLOTS_TO_DISPLAY];
	int nPlots;

	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iNum = 0;
	loadFrom >> iNum;
	for (int i = 0; i < iNum; i++)
	{
		TradeConnection kTradeConnection;
		writeTo.m_aTradeConnections.push_back(kTradeConnection);
		loadFrom >> writeTo.m_aTradeConnections[i];

		nPlots = min(MAX_PLOTS_TO_DISPLAY,(int)writeTo.m_aTradeConnections[i].m_aPlotList.size());
		if (nPlots > 0)
		{
			for (int i2=0; i2<nPlots; i2++)
			{
				plotsX[i2] = writeTo.m_aTradeConnections[i].m_aPlotList[i2].m_iX;
				plotsY[i2] = writeTo.m_aTradeConnections[i].m_aPlotList[i2].m_iY;
			}

			gDLL->TradeVisuals_NewRoute(i,writeTo.m_aTradeConnections[i].m_eOriginOwner,writeTo.m_aTradeConnections[i].m_eConnectionType,nPlots,plotsX,plotsY);
			gDLL->TradeVisuals_UpdateRouteDirection(i, writeTo.m_aTradeConnections[i].m_bTradeUnitMovingForward);
		}
	}

	if (uiVersion >= 3)
	{
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
			{
				loadFrom >> writeTo.m_aaiTechDifference[ui][ui2];
			}
		}
	}
	else
	{
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
			{
				writeTo.m_aaiTechDifference[ui][ui2] = 0;
			}
		}
	}

	if (uiVersion >= 3)
	{
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
			{
				loadFrom >> writeTo.m_aaiPolicyDifference[ui][ui2];
			}
		}
	}
	else
	{
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
			{
				writeTo.m_aaiPolicyDifference[ui][ui2] = 0;
			}
		}
	}

	loadFrom >> writeTo.m_iNextID;

	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const TradeConnection& readFrom)
{
	uint uiVersion = 3;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_iID;
	saveTo << readFrom.m_iOriginID;
	saveTo << readFrom.m_iOriginX;
	saveTo << readFrom.m_iOriginY;
	saveTo << readFrom.m_iDestID;
	saveTo << readFrom.m_iDestX;
	saveTo << readFrom.m_iDestY;
	saveTo << readFrom.m_eOriginOwner;
	saveTo << readFrom.m_eDestOwner;
	saveTo << (int)readFrom.m_eDomain;
	saveTo << (int)readFrom.m_eConnectionType;
	saveTo << readFrom.m_iTradeUnitLocationIndex;
	saveTo << readFrom.m_bTradeUnitMovingForward;
	saveTo << readFrom.m_unitID;
	saveTo << readFrom.m_iSpeedFactor;
	saveTo << readFrom.m_iCircuitsCompleted;
	saveTo << readFrom.m_iCircuitsToComplete;
	saveTo << readFrom.m_iTurnRouteComplete;
#if defined(MOD_API_TRADEROUTES)
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_bTradeUnitRecalled);
#endif

	saveTo << readFrom.m_aPlotList.size();
	for (uint ui2 = 0; ui2 < readFrom.m_aPlotList.size(); ui2++)
	{
		saveTo << readFrom.m_aPlotList[ui2].m_iX;
		saveTo << readFrom.m_aPlotList[ui2].m_iY;
	}

	for (uint ui2 = 0; ui2 < NUM_YIELD_TYPES; ui2++)
	{
		saveTo << readFrom.m_aiOriginYields[ui2];
		saveTo << readFrom.m_aiDestYields[ui2];
	}

	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvGameTrade& readFrom)
{
	uint uiVersion = 3;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_aTradeConnections.size();
	for (uint ui = 0; ui < readFrom.m_aTradeConnections.size(); ui++)
	{
		saveTo << readFrom.m_aTradeConnections[ui];
	}

	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
		{
			saveTo << readFrom.m_aaiTechDifference[ui][ui2];
		}
	}

	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		for (uint ui2 = 0; ui2 < MAX_MAJOR_CIVS; ui2++)
		{
			saveTo << readFrom.m_aaiPolicyDifference[ui][ui2];
		}
	}

	saveTo << readFrom.m_iNextID;

	return saveTo;
}

//=====================================
// CvPlayerTrade
//=====================================
//	--------------------------------------------------------------------------------
/// Default Constructor
CvPlayerTrade::CvPlayerTrade()
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Destructor
CvPlayerTrade::~CvPlayerTrade(void)
{
	Reset();
}

//	--------------------------------------------------------------------------------
/// Init
void CvPlayerTrade::Init(CvPlayer* pPlayer)
{
	Reset();
	m_pPlayer = pPlayer;
}

//	--------------------------------------------------------------------------------
/// Uninit
void CvPlayerTrade::Uninit(void)
{
	Reset();
}

//	--------------------------------------------------------------------------------
void CvPlayerTrade::Reset(void)
{
	m_pPlayer = NULL;
	m_aRecentlyExpiredConnections.clear();
	m_aTradeConnectionWasPlundered.clear();
}

//	--------------------------------------------------------------------------------
/// DoTurn
void CvPlayerTrade::DoTurn(void)
{
	m_aRecentlyExpiredConnections.clear();
	UpdateTradeConnectionValues();
	UpdateTradeConnectionWasPlundered();
	MoveUnits();
}

//	--------------------------------------------------------------------------------
/// MoveUnits
void CvPlayerTrade::MoveUnits (void)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(ui));
		if (pTradeConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			pTrade->MoveUnit(ui);

			// check to see if the trade route is still active but the circuit is completed
			if (!pTrade->IsTradeRouteIndexEmpty(ui) && pTradeConnection->m_iCircuitsCompleted >= pTradeConnection->m_iCircuitsToComplete)
			{
				m_aRecentlyExpiredConnections.push_back(*pTradeConnection);

				// get data before we wipe the trade route
				int iOriginX = pTradeConnection->m_iOriginX;
				int iOriginY = pTradeConnection->m_iOriginY;
				DomainTypes eDomain = pTradeConnection->m_eDomain;

#if defined(MOD_BALANCE_CORE)
				int iDestX = pTradeConnection->m_iDestX;
				int iDestY = pTradeConnection->m_iDestY;
				//Free lump resource when you complete an international trade route.
				CvPlot* pPlot = GC.getMap().plot(iOriginX, iOriginY);
				
				CvCity* pOriginCity = NULL;
				if(pPlot != NULL)
				{
					pOriginCity = pPlot->getPlotCity();
				}
				if(pOriginCity != NULL)
				{
					bool bInternational = false;
					if(pTradeConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
					{
						bInternational = true;
					}
					m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_TR_END, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pOriginCity, false, bInternational);
					if(bInternational)
					{			
						CvPlot* pDestPlot = GC.getMap().plot(iDestX, iDestY);
				
						CvCity* pDestCity = NULL;
						if(pDestPlot != NULL)
						{
							pDestCity = pDestPlot->getPlotCity();
						}
						if(pDestCity != NULL && pOriginCity != NULL)
						{
							// Corporation expansion system
							// Note: These functions will filter out if this is actually possible, so don't worry about it here
							GET_PLAYER(pOriginCity->getOwner()).GetCorporations()->BuildFranchiseInCity(pOriginCity, pDestCity);
							GET_PLAYER(pDestCity->getOwner()).GetCorporations()->BuildFranchiseInCity(pDestCity, pOriginCity);

							//Tourism Bonus from Buildings
							if(!GET_PLAYER(pDestCity->getOwner()).isMinorCiv())
							{
								if(pTradeConnection->m_eDomain == DOMAIN_LAND)
								{
									int iTourism = GET_PLAYER(pOriginCity->getOwner()).GetHistoricEventTourism(HISTORIC_EVENT_TRADE_LAND, pOriginCity);
									if (iTourism > 0)
									{
										GET_PLAYER(pOriginCity->getOwner()).GetCulture()->ChangeInfluenceOn(pDestCity->getOwner(), iTourism, true, true);

										//store off this data
										GET_PLAYER(pOriginCity->getOwner()).changeInstantYieldValue(YIELD_TOURISM, iTourism);

										// Show tourism spread
										if (pOriginCity->getOwner() == GC.getGame().getActivePlayer() && pDestCity->plot() != NULL && pDestCity->plot()->isRevealed(pOriginCity->getTeam()))
										{
											CvString strInfluenceText;
											InfluenceLevelTypes eLevel = GET_PLAYER(pOriginCity->getOwner()).GetCulture()->GetInfluenceLevel(pDestCity->getOwner());

											if (eLevel == INFLUENCE_LEVEL_UNKNOWN)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_UNKNOWN" );
											else if (eLevel == INFLUENCE_LEVEL_EXOTIC)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_EXOTIC");
											else if (eLevel == INFLUENCE_LEVEL_FAMILIAR)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_FAMILIAR");
											else if (eLevel == INFLUENCE_LEVEL_POPULAR)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_POPULAR");
											else if (eLevel == INFLUENCE_LEVEL_INFLUENTIAL)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_INFLUENTIAL");
											else if (eLevel == INFLUENCE_LEVEL_DOMINANT)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_DOMINANT");

 											char text[256] = {0};
											sprintf_s(text, "[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourism, strInfluenceText.c_str());
 											float fDelay = 3.0f;
 											DLLUI->AddPopupText(pDestCity->getX(), pDestCity->getY(), text, fDelay);
											CvNotifications* pNotification = GET_PLAYER(pOriginCity->getOwner()).GetNotifications();
											if(pNotification)
											{
												Localization::String strSummary;
												Localization::String strMessage;
												strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_LAND");
												strMessage << iTourism;
												strMessage << GET_PLAYER(pDestCity->getOwner()).getCivilizationAdjectiveKey();
												strMessage << pOriginCity->getNameKey();
												strMessage << pDestCity->getNameKey();
												strMessage << GET_PLAYER(pDestCity->getOwner()).getCivilizationShortDescriptionKey();
												if(GC.getGame().isGameMultiPlayer() &&GET_PLAYER(pDestCity->getOwner()).isHuman())
												{
													strMessage << GET_PLAYER(pDestCity->getOwner()).getNickName();
												}
												else
												{
													strMessage << GET_PLAYER(pDestCity->getOwner()).getNameKey();
												}
												strSummary = Localization::Lookup("TXT_KEY_TOURISM_EVENT_SUMMARY_TRADE");
												pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pOriginCity->getX(), pOriginCity->getY(), pOriginCity->getOwner());
											}
 										}
									}
								}
								else if(pTradeConnection->m_eDomain == DOMAIN_SEA)
								{
									int iTourism = GET_PLAYER(pOriginCity->getOwner()).GetHistoricEventTourism(HISTORIC_EVENT_TRADE_SEA, pOriginCity);						
									if (iTourism > 0)
									{
										GET_PLAYER(pOriginCity->getOwner()).GetCulture()->ChangeInfluenceOn(pDestCity->getOwner(), iTourism, true, true);

										//store off this data
										GET_PLAYER(pOriginCity->getOwner()).changeInstantYieldValue(YIELD_TOURISM, iTourism);

										// Show tourism spread
										if (pOriginCity->getOwner() == GC.getGame().getActivePlayer() && pDestCity->plot() != NULL && pDestCity->plot()->isRevealed(pOriginCity->getTeam()))
										{
											CvString strInfluenceText;
											InfluenceLevelTypes eLevel = GET_PLAYER(pOriginCity->getOwner()).GetCulture()->GetInfluenceLevel(pDestCity->getOwner());

											if (eLevel == INFLUENCE_LEVEL_UNKNOWN)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_UNKNOWN" );
											else if (eLevel == INFLUENCE_LEVEL_EXOTIC)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_EXOTIC");
											else if (eLevel == INFLUENCE_LEVEL_FAMILIAR)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_FAMILIAR");
											else if (eLevel == INFLUENCE_LEVEL_POPULAR)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_POPULAR");
											else if (eLevel == INFLUENCE_LEVEL_INFLUENTIAL)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_INFLUENTIAL");
											else if (eLevel == INFLUENCE_LEVEL_DOMINANT)
												strInfluenceText = GetLocalizedText( "TXT_KEY_CO_DOMINANT");

 											char text[256] = {0};
											sprintf_s(text, "[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourism, strInfluenceText.c_str());
 											float fDelay = 4.0f;
 											DLLUI->AddPopupText(pDestCity->getX(), pDestCity->getY(), text, fDelay);

											CvNotifications* pNotification = GET_PLAYER(pOriginCity->getOwner()).GetNotifications();
											if(pNotification)
											{
												Localization::String strSummary;
												Localization::String strMessage;
												strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_SEA");
													strMessage << iTourism;
													strMessage << GET_PLAYER(pDestCity->getOwner()).getCivilizationAdjectiveKey();
													strMessage << pOriginCity->getNameKey();
													strMessage << pDestCity->getNameKey();
													strMessage << GET_PLAYER(pDestCity->getOwner()).getCivilizationShortDescriptionKey();
												strSummary = Localization::Lookup("TXT_KEY_TOURISM_EVENT_SUMMARY_TRADE");
												pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pOriginCity->getX(), pOriginCity->getY(), pOriginCity->getOwner());
											}
										}
									}
								}
							}
							else if(GET_PLAYER(pDestCity->getOwner()).isMinorCiv() && GET_PLAYER(pOriginCity->getOwner()).GetEventTourismCS() > 0)
							{
								int iTourism = GET_PLAYER(pOriginCity->getOwner()).GetHistoricEventTourism(HISTORIC_EVENT_TRADE_CS);
								GET_PLAYER(pOriginCity->getOwner()).ChangeNumHistoricEvents(HISTORIC_EVENT_TRADE_CS, 1);
								if (iTourism > 0)
								{
									GET_PLAYER(pOriginCity->getOwner()).GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
									CvNotifications* pNotification = GET_PLAYER(pOriginCity->getOwner()).GetNotifications();
									if(pNotification)
									{
										Localization::String strSummary;
										Localization::String strMessage;
										strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_CS_BONUS");
										strMessage << iTourism;
											strMessage << GET_PLAYER(pDestCity->getOwner()).getCivilizationShortDescriptionKey();
										strSummary = Localization::Lookup("TXT_KEY_TOURISM_EVENT_SUMMARY_TRADE_CS");
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pOriginCity->getX(), pOriginCity->getY(), pOriginCity->getOwner());
									}
								}
							}
						}
					}


#if defined(MOD_CIV6_ROADS)
					if (MOD_CIV6_ROADS)
					{
						//build land routes

						// i didn't go by plot->changeBuildProgress to not fire unwanted things, like "BuildFinished".
						// so it's mostly a copy-paste from the road section of this method
						RouteTypes autoBuildRoadType = ROUTE_ROAD;
						// check if the rairoad is unlocked (by tech and by era).
						int minEra = GD_INT_GET(TRADE_ROUTE_CREATE_RAILROADS_ERA);
						int iTech = GD_INT_GET(TRADE_ROUTE_CREATE_RAILROADS_TECH_ID);
						if ((GET_PLAYER(pTradeConnection->m_eOriginOwner).GetCurrentEra() >= minEra)
							&& (iTech >= 0 && iTech < GC.getNumTechInfos() && GET_TEAM(GET_PLAYER(pTradeConnection->m_eOriginOwner).getTeam()).GetTeamTechs()->HasTech((TechTypes)iTech)))
						{
							autoBuildRoadType = ROUTE_RAILROAD;
						}

						//iterate on plots
						uint nbPLotsInTradeRoute = (uint)pTradeConnection->m_aPlotList.size();
						for (uint ui = 0; ui < nbPLotsInTradeRoute; ui++){
							CvPlot* pRoadToBuildPlot = GC.getMap().plot(pTradeConnection->m_aPlotList[ui].m_iX, pTradeConnection->m_aPlotList[ui].m_iY);

							//don't build roads on sea
							if (!pRoadToBuildPlot->IsPlotOcean())
							{
								
								//don't build road / change nationality (for the moment) of roads if already built.
								if (pRoadToBuildPlot->getRouteType() != autoBuildRoadType)
								{
									CvRouteInfo* pkRouteInfo = GC.getRouteInfo(autoBuildRoadType);
									if (pkRouteInfo)
									{
										pRoadToBuildPlot->setRouteType(autoBuildRoadType);

										// Unowned plot, someone has to foot the bill
										if (pRoadToBuildPlot->getOwner() == NO_PLAYER)
										{
											if (pRoadToBuildPlot->MustPayMaintenanceHere(pTradeConnection->m_eOriginOwner))
											{
												GET_PLAYER(pTradeConnection->m_eOriginOwner).GetTreasury()->ChangeBaseImprovementGoldMaintenance(pkRouteInfo->GetGoldMaintenance());
											}
											pRoadToBuildPlot->SetPlayerResponsibleForRoute(pTradeConnection->m_eOriginOwner);
										}
									}
								}
							}
						}
					}
#endif

				}	
#endif
#if defined(MOD_EVENTS_TRADE_ROUTES)
				if (MOD_EVENTS_TRADE_ROUTES) 
				{
					CvPlot* pDestPlot = GC.getMap().plot(iDestX, iDestY);
					CvCity* pDestCity = NULL;
					if(pDestPlot != NULL)
					{
						pDestCity = pDestPlot->getPlotCity();
					}
					if(pDestCity != NULL && pOriginCity != NULL)
					{
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerTradeRouteCompleted, pOriginCity->getOwner(), pOriginCity->GetID(), pDestCity->getOwner(), pDestCity->GetID(), eDomain, pTradeConnection->m_eConnectionType);
					}
				}
#endif

				// wipe trade route
				pTrade->EmptyTradeRoute(ui);

				// create new unit
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				UnitTypes eUnitType = GetTradeUnit(eDomain, m_pPlayer);
#else
				UnitTypes eUnitType = GetTradeUnit(eDomain);
#endif
#if defined(MOD_BUGFIX_MINOR)
				m_pPlayer->initUnit(eUnitType, iOriginX, iOriginY, UNITAI_TRADE_UNIT);
#else
				CvUnit* pRebornUnit = m_pPlayer->initUnit(eUnitType, iOriginX, iOriginY, UNITAI_TRADE_UNIT);
				DEBUG_VARIABLE(pRebornUnit);
				CvAssertMsg(pRebornUnit, "pRebornUnit is null. This is bad!!");
#endif
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionBaseValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	if (bAsOriginPlayer)
	{
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
		{
			if (eYield == YIELD_GOLD)
			{
				int iResult = 0;
				int iBase = GC.getINTERNATIONAL_TRADE_BASE();
				iResult = iBase;
				return iResult;
			}
			else if (eYield == YIELD_SCIENCE)
			{
				int iTechDifference = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
				int iAdjustedTechDifference = 0;
				if (iTechDifference > 0)
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					int iCeilTechDifference = iTechDifference * 100 / GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);
#else
					int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
					iAdjustedTechDifference = max(iCeilTechDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
					if(iAdjustedTechDifference > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
					{
						iAdjustedTechDifference *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
					}
#endif

					// Policy bump
					int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

					iAdjustedTechDifference += iPolicyBump;

				}
				// Cultural influence bump
				int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteScienceBonus(kTradeConnection.m_eDestOwner);

				iAdjustedTechDifference += iInfluenceBoost;

				return iAdjustedTechDifference * 100;
			}
			else if (eYield == YIELD_CULTURE)
			{
				int iCultureDifference = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
				int iAdjustedCultureDifference = 0;
				if (iCultureDifference > 0)
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					int iCeilCultureDifference = iCultureDifference * 100 / GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
#else
					int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
					iAdjustedCultureDifference = max(iCeilCultureDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
					if (iAdjustedCultureDifference > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
					{
						iAdjustedCultureDifference *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
					}
#endif				
					// Policy bump
					int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

					iAdjustedCultureDifference += iPolicyBump;
				}				

				return iAdjustedCultureDifference * 100;
			}
		}
	}
	else
	{
		if (eYield == YIELD_SCIENCE)
		{
			int iTechDifference = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eDestOwner, kTradeConnection.m_eOriginOwner);
			int iAdjustedTechDifference = 0;
			if (iTechDifference > 0)
			{
#if defined(MOD_TRADE_ROUTE_SCALING)
				int iCeilTechDifference = iTechDifference * 100 / GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);
#else
				int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
				iAdjustedTechDifference = max(iCeilTechDifference, 1);
			}

			return  iAdjustedTechDifference * 100;
		}
		else if (eYield == YIELD_CULTURE)
		{
			int iCultureDifference = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eDestOwner, kTradeConnection.m_eOriginOwner);
			int iAdjustedCultureDifference = 0;
			if (iCultureDifference > 0)
			{
#if defined(MOD_TRADE_ROUTE_SCALING)
				int iCeilCultureDifference = iCultureDifference * 100 / GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
#else
				int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
				iAdjustedCultureDifference = max(iCeilCultureDifference, 1);
			}

			return  iAdjustedCultureDifference * 100;
		}
		else
		{
			return 0;
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionGPTValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer, bool bOriginCity)
{
	if (bAsOriginPlayer)
	{
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
		{
			if (eYield == YIELD_GOLD)
			{
				int iX, iY;
				if (bOriginCity)
				{
					iX = kTradeConnection.m_iOriginX;
					iY = kTradeConnection.m_iOriginY;
				}
				else
				{
					iX = kTradeConnection.m_iDestX;
					iY = kTradeConnection.m_iDestY;
				}

				CvPlot* pPlot = GC.getMap().plot(iX, iY);

				CvAssertMsg(pPlot, "CvPlayerTrade - plot is null");
				if (!pPlot)
				{
					return 0;
				}

				CvCity* pCity = pPlot->getPlotCity();
				CvAssertMsg(pCity, "CvPlayerTrade - pCity is null");	
				if (!pCity)
				{
					return 0;
				}

				int iDivisor = GC.getINTERNATIONAL_TRADE_CITY_GPT_DIVISOR();
				if (iDivisor == 0)
				{
					iDivisor = 1;
				}

#if defined(MOD_BALANCE_CORE)
				// Cultural influence negation.
				int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eDestOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eOriginOwner);
				if(iInfluenceBoost > 0)
				{
					return (pCity->getYieldRateTimes100(eYield, true) - iInfluenceBoost) / iDivisor;
				}
#endif
				return pCity->getYieldRateTimes100(eYield, true) / iDivisor;
			}
		}
	}
	else
	{
		return 0;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionResourceValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	if (bAsOriginPlayer)
	{
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
		{
			if (eYield == YIELD_GOLD)
			{
				CvPlot* pOriginPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
				CvPlot* pDestPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);

				CvAssertMsg(pOriginPlot && pDestPlot, "pOriginPlot or pDestPlot are null");
				if (!pOriginPlot || !pDestPlot)
				{
					return 0;
				}

				CvCity* pOriginCity = pOriginPlot->getPlotCity();
				CvCity* pDestCity = pDestPlot->getPlotCity();

				CvAssertMsg(pOriginCity && pDestCity, "pOriginCity or pDestCity are null");
				if (!pOriginCity || !pDestCity)
				{
					return 0;
				}

				int iValue = 0;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
				if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
				{
					int iOurResources = 0;
					int iTheirResources = 0;
					for (int i = 0; i < GC.getNumResourceInfos(); i++)
					{
						ResourceTypes eResource = (ResourceTypes)i;
						const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
						if (pkResourceInfo)
						{
							bool bOurMonopoly = false;
							bool bTheirMonopoly = false;
							if (GET_PLAYER(pOriginCity->getOwner()).HasGlobalMonopoly(eResource))
								bOurMonopoly = true;;
							if (GET_PLAYER(pDestCity->getOwner()).HasGlobalMonopoly(eResource))
								bTheirMonopoly = true;

							if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
							{
								iOurResources += bOurMonopoly ? pOriginCity->GetNumResourceLocal(eResource, true) * 2 : pOriginCity->GetNumResourceLocal(eResource, true);
								iTheirResources += bTheirMonopoly ? pDestCity->GetNumResourceLocal(eResource, true) * 2 : pDestCity->GetNumResourceLocal(eResource, true);
							}
							if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
							{
								if (pOriginCity->GetNumResourceLocal(eResource, true) > 0)
									iOurResources += bOurMonopoly ? 2 : 1;
								if (pDestCity->GetNumResourceLocal(eResource, true) > 0)
									iTheirResources += bTheirMonopoly ? 2 : 1;
							}
						}
					}
					int iDelta = iOurResources - iTheirResources;
					iValue = GD_INT_GET(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE) * iDelta;

					//Can't go above +100 or below -50
					if (iValue > 100)
						iValue = 100;
					else if (iValue <= -50)
						iValue = -50;
				}
				else
				{
#endif
					for (int i = 0; i < GC.getNumResourceInfos(); i++)
					{
						ResourceTypes eResource = (ResourceTypes)i;
						const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
						if (pkResourceInfo)
						{
							if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
							{
								if (pOriginCity->IsHasResourceLocal(eResource, true) != pDestCity->IsHasResourceLocal(eResource, true))
								{
#if defined(MOD_TRADE_ROUTE_SCALING)
									iValue += GD_INT_GET(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE);
#else
									iValue += 50;
#endif
								}
							}
						}
					}
				}

				int iModifer = 100 + GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetTradeRouteResourceModifier() + pOriginCity->GetResourceDiversityModifier();
				if (iValue > 0)
				{
					iValue *= iModifer;
					iValue /= 100;
				}
				else
				{
					iValue *= 100;
					iValue /= max(1, iModifer);
				}


				return iValue;
			}
		}
	}
	else
	{
		return 0;	
	}

	return 0;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionYourBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	// this only applies to international trade routes, so otherwise, buzz off!
	if (!GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
	{
		return 0;
	}
	
	if (eYield != YIELD_GOLD)
	{
		return 0;
	}

	CvPlot* pOriginPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
	CvPlot* pDestPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);

	CvAssertMsg(pOriginPlot && pDestPlot, "pOriginPlot or pDestPlot are null");
	if (!pOriginPlot || !pDestPlot)
	{
		return 0;
	}

	CvCity* pDestCity = pDestPlot->getPlotCity();
	if (!pDestCity)
	{
		return 0;
	}

	int iBonus = 0;
	if (bAsOriginPlayer)
	{
		CvCity* pOriginCity = CvGameTrade::GetOriginCity(kTradeConnection);

		if (kTradeConnection.m_eDomain == DOMAIN_SEA)
		{
			iBonus += pOriginCity->GetTradeRouteSeaGoldBonus();
		}
		else if (kTradeConnection.m_eDomain == DOMAIN_LAND)
		{
			iBonus += pOriginCity->GetTradeRouteLandGoldBonus();
		}
	}

	if (bAsOriginPlayer)
	{
		iBonus *= (100 + GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetTradeBuildingModifier());
		iBonus /= 100;
	}
	else
	{
		iBonus *= (100 + GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetTradeBuildingModifier());
		iBonus /= 100;
	}	

	return iBonus;
}
#if defined(MOD_BALANCE_CORE)
int CvPlayerTrade::GetMinorCivGoldBonus(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	int iResult = 0;
	if(bAsOriginPlayer && eYield == YIELD_GOLD)
	{
		if(GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		{
			if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsAllies(kTradeConnection.m_eOriginOwner))
			{
				int iAllyGold = (GD_INT_GET(TRADE_ROUTE_CS_ALLY_GOLD) * 100);
				if(iAllyGold > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
				{
					iAllyGold *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
				}
				iResult = iAllyGold;
			}
			else if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsFriends(kTradeConnection.m_eOriginOwner))
			{
				int iFriendGold = (GD_INT_GET(TRADE_ROUTE_CS_FRIEND_GOLD) * 100);
				if(iFriendGold > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
				{
					iFriendGold *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
				}
				iResult = iFriendGold;
			}
		}
	}
	return iResult;
}
#endif
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionTheirBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	// this only applies to international trade routes, so otherwise, buzz off!
	if (!GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
	{
		return 0;
	}

	if (eYield != YIELD_GOLD)
	{
		return 0;
	}

	CvPlot* pOriginPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
	CvPlot* pDestPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);

	CvAssertMsg(pOriginPlot && pDestPlot, "pOriginPlot or pDestPlot are null");
	if (!pOriginPlot || !pDestPlot)
	{
		return 0;
	}

	CvCity* pDestCity = pDestPlot->getPlotCity();
	if (!pDestCity)
	{
		return 0;
	}

	int iBonus = 0;

	// minor civs should not get bonuses for others trading with them
	if (!GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && !GET_PLAYER(kTradeConnection.m_eOriginOwner).isMinorCiv())
	{
		if (kTradeConnection.m_eOriginOwner == m_pPlayer->GetID())
		{
			iBonus += pDestCity->GetTradeRouteTargetBonus() * 100;

#if defined(MOD_BALANCE_CORE)
			CorporationTypes eCorporation = GET_PLAYER(kTradeConnection.m_eDestOwner).GetCorporations()->GetFoundedCorporation();
			if (eCorporation != NO_CORPORATION)
			{
				CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
				if (pkCorporation)
				{
					iBonus += pkCorporation->GetTradeRouteTargetBonus() * 100;
				}
			}
#endif
		}

		if (kTradeConnection.m_eDestOwner == m_pPlayer->GetID())
		{
			iBonus += pDestCity->GetTradeRouteRecipientBonus() * 100;

#if defined(MOD_BALANCE_CORE)
			CorporationTypes eCorporation = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCorporations()->GetFoundedCorporation();
			if (eCorporation != NO_CORPORATION)
			{
				CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
				if (pkCorporation)
				{
					iBonus += pkCorporation->GetTradeRouteRecipientBonus() * 100;
				}
			}
#endif
		}
	}

	if (bAsOriginPlayer)
	{
		iBonus *= (100 + GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetTradeBuildingModifier());
		iBonus /= 100;
	}
	else
	{
		iBonus *= (100 + GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetTradeBuildingModifier());
		iBonus /= 100;
	}	

	return iBonus;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionExclusiveValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield)
{
	// unnecessary code to make it compile for now
	if (eYield != NO_YIELD)
	{
		eYield = eYield;
	}

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	if (pTrade->IsDestinationExclusive(kTradeConnection))
	{
		return GC.getINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION();
	}
	else
	{
		return 0;
	}
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionPolicyValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield)
{
	int iValue = 0;

#if defined(MOD_API_UNIFIED_YIELDS)
	CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
	iValue += kPlayer.getTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100;
	iValue += kPlayer.GetPlayerTraits()->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100;

	CvCity* pCity = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY)->getPlotCity();
	ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
		if(pReligion)
		{
			CvCity* pHolyCity = NULL;
			CvPlot* pHolyCityPlot = GC.getMap().plot(pReligion->m_iHolyCityX, pReligion->m_iHolyCityY);
			if (pHolyCityPlot)
			{
				pHolyCity = pHolyCityPlot->getPlotCity();
			}
			iValue += pReligion->m_Beliefs.GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield, kPlayer.GetID(), pHolyCity) * 100;
			BeliefTypes eSecondaryPantheon = pCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
			if (eSecondaryPantheon != NO_BELIEF)
			{
				iValue += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100;
			}
		}
	}
#endif

	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
#if defined(MOD_API_UNIFIED_YIELDS)
	  if (eYield == YIELD_GOLD) {
#endif
		// domain type bonuses
		if (kTradeConnection.m_eDomain == DOMAIN_LAND)
		{
			iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_LAND_TRADE_GOLD_CHANGE);
		}
		else if (kTradeConnection.m_eDomain == DOMAIN_SEA)
		{
			iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SEA_TRADE_GOLD_CHANGE);
		}

		// policy tree bonuses
		CvPlayerPolicies* pOwnerPlayerPolicies = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies();
		CvPlayerPolicies* pDestPlayerPolicies = GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerPolicies();

		PolicyBranchTypes eFreedom = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
		PolicyBranchTypes eAutocracy = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
		PolicyBranchTypes eOrder = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();

		bool bBothAutocracy = false;
		bool bBothOrder = false;
		bool bBothFreedom = false;
		if (eAutocracy != NO_POLICY_BRANCH_TYPE)
		{
			bBothAutocracy = pOwnerPlayerPolicies->IsPolicyBranchUnlocked(eAutocracy) && pDestPlayerPolicies->IsPolicyBranchUnlocked(eAutocracy);
		}
		if (eOrder != NO_POLICY_BRANCH_TYPE)
		{
			bBothOrder = pOwnerPlayerPolicies->IsPolicyBranchUnlocked(eOrder) && pDestPlayerPolicies->IsPolicyBranchUnlocked(eOrder);
		}
		if (eFreedom != NO_POLICY_BRANCH_TYPE)
		{
			bBothFreedom = pOwnerPlayerPolicies->IsPolicyBranchUnlocked(eFreedom) && pDestPlayerPolicies->IsPolicyBranchUnlocked(eFreedom);
		}
	
		if (bBothAutocracy || bBothOrder || bBothFreedom)
		{
			iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SHARED_IDEOLOGY_TRADE_CHANGE) * 100;
		}

		// city state bonus
		if (GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
		{
			iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CITY_STATE_TRADE_CHANGE);
		}
#if defined(MOD_API_UNIFIED_YIELDS)
	  }
#endif
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionOtherTraitValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	int iValue = 0;
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		if (bAsOriginPlayer)
		{
			iValue += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldChangeIncomingTradeRoute(eYield) * 100;
		}
#if defined(MOD_API_UNIFIED_YIELDS)
	}
	else
	{
		iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetYieldChangeIncomingTradeRoute(eYield) * 100;
#endif
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionDomainValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield)
{
	// unnecessary code to make it compile for now
	if (eYield != NO_YIELD)
	{
		eYield = eYield;
	}

	return GC.getGame().GetGameTrade()->GetDomainModifierTimes100(kTradeConnection.m_eDomain);
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionDistanceValueModifierTimes100(const TradeConnection& kTradeConnection)
{
	if (!MOD_BALANCE_CORE_SCALING_TRADE_DISTANCE)
		return 0;

	if (m_pPlayer->GetPlayerTraits()->IsIgnoreTradeDistanceScaling())
		return 0;

	//distance scaling doesn't apply to internal trade
	if (kTradeConnection.m_eDestOwner == kTradeConnection.m_eOriginOwner)
		return 0;

	CvCity* pOriginCity = NULL;
	CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
	if (pStartPlot)
	{
		pOriginCity = pStartPlot->getPlotCity();
	}

	CvCity* pEndCity = NULL;
	CvPlot* pEndPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);
	if (pEndPlot)
	{
		pEndCity = pEndPlot->getPlotCity();
	}

	if (pOriginCity == NULL || pEndCity == NULL)
		return 0;

	if (GET_PLAYER(pEndCity->getOwner()).isMajorCiv() && GET_PLAYER(pEndCity->getOwner()).GetPlayerTraits()->IsIgnoreTradeDistanceScaling())
		return 0;

	int iLength = GC.getGame().GetGameTrade()->GetValidTradeRoutePathLength(pOriginCity, pEndCity, kTradeConnection.m_eDomain);
	if (iLength <= 0)
		return 0;
		
	int iDistance = pOriginCity->GetLongestPotentialTradeRoute(kTradeConnection.m_eDomain);

	iLength *= 100;
	iLength /= max(1, iDistance);

	int iReduction = 100 - iLength;

	return max(0, min(50, iReduction));
}


//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionRiverValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	// unnecessary code to make it compile for now
	if (eYield != NO_YIELD)
	{
		eYield = eYield;
	}

	int iModifier = 0;
	if (eYield == YIELD_GOLD && kTradeConnection.m_eDomain == DOMAIN_LAND)
	{
		if (bAsOriginPlayer)
		{
			CvCity* pOriginCity = CvGameTrade::GetOriginCity(kTradeConnection);
			CvAssert(pOriginCity != NULL);
			if (pOriginCity)
			{
				CvPlot* pOriginCityPlot = pOriginCity->plot();
				if (pOriginCityPlot->isRiver())
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					iModifier = GD_INT_GET(TRADE_ROUTE_RIVER_CITY_MODIFIER);
#else
					iModifier = 25;
#endif
				}
			}
		}
		else
		{
			CvCity* pDestCity = CvGameTrade::GetDestCity(kTradeConnection);
			CvAssert(pDestCity != NULL);
			if (pDestCity)
			{
				CvPlot* pDestCityPlot = pDestCity->plot();
				if (pDestCityPlot->isRiver())
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					iModifier = GD_INT_GET(TRADE_ROUTE_RIVER_CITY_MODIFIER);
#else
					iModifier = 25;
#endif
				}
			}
		}
	}

	return iModifier;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionOpenBordersModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	// unnecessary code to make it compile for now
	if (eYield != NO_YIELD)
	{
		eYield = eYield;
	}

	int iModifier = 0;
	CvCity* pDestCity = CvGameTrade::GetDestCity(kTradeConnection);
	if(pDestCity == NULL)
	{
		return 0;
	}
	CvCity* pOriginCity = CvGameTrade::GetOriginCity(kTradeConnection);
	if(pOriginCity == NULL)
	{
		return 0;
	}
	PlayerTypes eOriginPlayer = pOriginCity->getOwner();
	PlayerTypes eDestPlayer = pDestCity->getOwner();

	if(bAsOriginPlayer && eOriginPlayer != NO_PLAYER && eDestPlayer != NO_PLAYER && eYield == YIELD_GOLD)
	{
		if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) && GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*20*/ GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD();
		}
		else if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*20*/ (GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD() / 2);
		}
	}
	else if(!bAsOriginPlayer && eOriginPlayer != NO_PLAYER && eDestPlayer != NO_PLAYER && eYield == YIELD_GOLD)
	{
		if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) && GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*20*/ (GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD() / 2);
		}
		else if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*20*/ (GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD() / 4);
		}
	}

	return iModifier;
}
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionCorporationModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	// unnecessary code to make it compile for now
	if (eYield != NO_YIELD)
	{
		eYield = eYield;
	}

	int iModifier = 0;
	CvCity* pDestCity = CvGameTrade::GetDestCity(kTradeConnection);
	if (pDestCity == NULL)
	{
		return 0;
	}
	CvCity* pOriginCity = CvGameTrade::GetOriginCity(kTradeConnection);
	if (pOriginCity == NULL)
	{
		return 0;
	}

	CorporationTypes eCorporation = GET_PLAYER(pOriginCity->getOwner()).GetCorporations()->GetFoundedCorporation();
	if (eCorporation == NO_CORPORATION)
	{
		return 0;
	}

	CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
	if (pkCorporationInfo == NULL)
	{
		return 0;
	}

	if (bAsOriginPlayer)
	{
		bool bOfficesAsFranchises = GET_PLAYER(pOriginCity->getOwner()).GetCorporations()->IsCorporationOfficesAsFranchises();
		//If nationalized, we must be targetting our own city
		if (bOfficesAsFranchises && pOriginCity->getOwner() == pDestCity->getOwner())
		{
			// Target only trade routes to cities with a Franchise - IsHasFranchise() also counts Nationalized Offices as franchises!
			if (pOriginCity->IsHasOffice() && pDestCity->IsHasFranchise(eCorporation))
			{
				iModifier += pkCorporationInfo->GetTradeRouteMod((int)eYield);
			}
		}

		// Do we have a building that grants a bonus toward cities with Franchises?
		else if (pDestCity->IsHasFranchise(eCorporation) && pOriginCity->IsHasOffice() && pkCorporationInfo->GetTradeRouteMod((int)eYield) > 0)
		{
			iModifier += pkCorporationInfo->GetTradeRouteMod((int)eYield);
		}
	}
	return iModifier;
}
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionPolicyModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	if (bAsOriginPlayer) 
	{
		CvCity* pOriginCity = GC.getGame().GetGameTrade()->GetOriginCity(kTradeConnection);
		if (pOriginCity == NULL)
		{
			return 0;
		}
		return GET_PLAYER(pOriginCity->getOwner()).GetPlayerPolicies()->GetInternationalRouteYieldModifier(eYield);
	}
	else 
	{
		CvCity* pDestCity = GC.getGame().GetGameTrade()->GetDestCity(kTradeConnection);
		if (pDestCity == NULL)
		{
			return 0;
		}
		return GET_PLAYER(pDestCity->getOwner()).GetPlayerPolicies()->GetInternationalRouteYieldModifier(eYield);
	}
}
#endif
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionValueTimes100 (const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iValue = 0;

	if (bAsOriginPlayer)
	{
		if (pTrade->IsConnectionInternational(kTradeConnection))
		{
			switch (eYield)
			{
			case YIELD_GOLD:
				{
					int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iOriginPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, eYield, bAsOriginPlayer, true);
					int iDestPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, eYield, bAsOriginPlayer, false);
					int iResourceBonus = GetTradeConnectionResourceValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, eYield);
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#if defined(MOD_BALANCE_CORE)
					// Cultural influence bump
					int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eDestOwner);
					//Minor Civ Bump
					int iMinorCivGold = GetMinorCivGoldBonus(kTradeConnection, eYield, true);
#endif

					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#if defined(MOD_BALANCE_CORE)
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#endif

					iValue = iBaseValue;
					iValue += iOriginPerTurnBonus;
					iValue += iDestPerTurnBonus;
					iValue += iExclusiveBonus;
					iValue += iYourBuildingBonus;
					iValue += iTheirBuildingBonus;
					iValue += iResourceBonus;
					iValue += iPolicyBonus;
					iValue += iTraitBonus;
#if defined(MOD_BALANCE_CORE)
					// Cultural influence bump
					iValue += iInfluenceBoost;
					iValue += iMinorCivGold;
#endif
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += iOriginRiverModifier;
#if defined(MOD_BALANCE_CORE)
					iModifier += iCorporationModifier;
					iModifier += iOpenBordersModifier;
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					iModifier += iPolicyModifier;
#endif
#if defined(MOD_BALANCE_CORE)
					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);
					}
#endif

					iValue *= iModifier;
					iValue /= 100;
					iValue = max(100, iValue);
				}
				break;
#if defined(MOD_API_UNIFIED_YIELDS)
			case YIELD_CULTURE:
			case YIELD_FAITH:
#endif
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
			case YIELD_TOURISM:
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
			case YIELD_GOLDEN_AGE_POINTS:
#endif
			case YIELD_SCIENCE:
#if defined(MOD_API_UNIFIED_YIELDS)
				{
#endif
					int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#if defined(MOD_API_UNIFIED_YIELDS)
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#endif

					iValue = iBaseValue;
#if defined(MOD_API_UNIFIED_YIELDS)
					iValue += iPolicyBonus;
					iValue += iTraitBonus;
#endif

					int iModifier = 100;
#if defined(MOD_BALANCE_CORE)
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					iModifier -= iDistanceModifier;
					iModifier += iCorporationModifier;
#endif
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					iModifier += iPolicyModifier;
#endif
#if defined(MOD_BALANCE_CORE)
					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);
					}
#endif
				
					iValue *= iModifier;
					iValue /= 100;
#if defined(MOD_API_UNIFIED_YIELDS)
				}
#endif
				break;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
/// TODO: Integrate modifier logic into the PRODUCTION and FOOD cases of international trade
#endif
#if defined(MOD_BALANCE_CORE)
			case YIELD_PRODUCTION:
				{
					if(MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsSiphoned(kTradeConnection.m_eOriginOwner))
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOriginPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, true);
						int iDestPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, false);
						int iResourceBonus = GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, YIELD_GOLD);
						int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, YIELD_GOLD);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eDestOwner);
						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, YIELD_GOLD);
						int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						iValue = iBaseValue;
						iValue += iOriginPerTurnBonus;
						iValue += iDestPerTurnBonus;
						iValue += iExclusiveBonus;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iResourceBonus;
						iValue += iPolicyBonus;
						iValue += iTraitBonus;
						iValue += iInfluenceBoost;

						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iOriginRiverModifier;
						iModifier += iCorporationModifier;
						iModifier += iOpenBordersModifier;
#if defined(MOD_BALANCE_CORE)
						CvCity* pOriginCity = NULL;
						CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
						if (pStartPlot)
						{
							pOriginCity = pStartPlot->getPlotCity();
						}
						if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
						{
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);
						}
#endif

						iValue *= iModifier;
						if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsAllies(kTradeConnection.m_eOriginOwner))
						{
							iValue /= 150;
						}
						else if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsFriends(kTradeConnection.m_eOriginOwner))
						{
							iValue /= 200;
						}
						else
						{
							iValue /= 250;
						}
						iValue = max(100, iValue);
					}
				}
				break;
			case YIELD_FOOD:
				{
					if (MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsSiphoned(kTradeConnection.m_eOriginOwner))
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOriginPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, true);
						int iDestPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, false);
						int iResourceBonus = GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, YIELD_GOLD);
						int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, YIELD_GOLD);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, YIELD_GOLD);
						int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						iValue = iBaseValue;
						iValue += iOriginPerTurnBonus;
						iValue += iDestPerTurnBonus;
						iValue += iExclusiveBonus;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iResourceBonus;
						iValue += iPolicyBonus;
						iValue += iTraitBonus;
						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iOriginRiverModifier;
						iModifier += iCorporationModifier;
						iModifier += iOpenBordersModifier;
#if defined(MOD_BALANCE_CORE)
						CvCity* pOriginCity = NULL;
						CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
						if (pStartPlot)
						{
							pOriginCity = pStartPlot->getPlotCity();
						}
						if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
						{
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);
						}
#endif

						iValue *= iModifier;
						if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsAllies(kTradeConnection.m_eOriginOwner))
						{
							iValue /= 200;
						}
						else if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsFriends(kTradeConnection.m_eOriginOwner))
						{
							iValue /= 300;
						}
						else
						{
							iValue /= 400;
						}
						iValue = max(100, iValue);
					}
				}
				break;
#endif
			}
		}
	}
	else
	{
		if (pTrade->IsConnectionInternational(kTradeConnection))
		{
			if (kTradeConnection.m_eDestOwner == m_pPlayer->GetID())
			{
				switch (eYield)
				{
				case YIELD_GOLD:
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);

#if defined(MOD_BALANCE_CORE)
						// Cultural influence bump
						int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eDestOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eOriginOwner);
#endif

						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
						int iDestRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, eYield, false);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
						int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, false /*bAsOriginPlayer*/);
#endif

						iValue = iBaseValue;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iTraitBonus;

#if defined(MOD_BALANCE_CORE)
						// Cultural influence bump
						iValue += iInfluenceBoost;
#endif

						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iDestRiverModifier;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
						iModifier += iPolicyModifier;
#endif

						iValue *= iModifier;
						iValue /= 100;
					}
					break;
#if defined(MOD_API_UNIFIED_YIELDS)
				case YIELD_CULTURE:
				case YIELD_FAITH:
#endif
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
				case YIELD_TOURISM:
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
				case YIELD_GOLDEN_AGE_POINTS:
#endif
				case YIELD_SCIENCE:
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#if defined(MOD_API_UNIFIED_YIELDS)
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
#endif

						int iModifier = 100;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, false /*bAsOriginPlayer*/);
						iModifier -= iDistanceModifier;
						iModifier += iPolicyModifier;
#endif

						iValue = iBaseValue;
#if defined(MOD_API_UNIFIED_YIELDS)
						iValue += iTraitBonus;
#endif

						iValue *= iModifier;
						iValue /= 100;						
					}
					break;
				}
			}
		}
		else
		{
			switch (kTradeConnection.m_eConnectionType)
			{
			case TRADE_CONNECTION_FOOD:
				if (eYield == YIELD_FOOD)
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					iValue = GD_INT_GET(TRADE_ROUTE_BASE_FOOD_VALUE);
#else
					iValue = 300;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
					iValue += GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					iValue += GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
#endif
					iValue += GC.getEraInfo(GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra())->getTradeRouteFoodBonusTimes100();
					iValue *= GC.getEraInfo(GC.getGame().getStartEra())->getGrowthPercent();
					iValue /= 100;
#if defined(MOD_BALANCE_YIELD_SCALE_ERA)
					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					CvCity* pEndCity = NULL;
					CvPlot* pEndPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);
					if (pEndPlot)
					{
						pEndCity = pEndPlot->getPlotCity();
					}
					if(MOD_BALANCE_YIELD_SCALE_ERA && pOriginCity != NULL && pEndCity != NULL)
					{
						int iStartCityPop = pOriginCity->getPopulation();
						int iEndCityPop = pEndCity->getPopulation();
						if(iStartCityPop > iEndCityPop)
						{
							int iDelta = ((iStartCityPop - iEndCityPop) / 3);
							iValue += max((iDelta * 100), 100);
						}

						// Tech Progress increases City Strength
						int iTechProgress = GET_TEAM(pOriginCity->getTeam()).GetTeamTechs()->GetNumTechsKnown() * 100 / GC.getNumTechInfos();
						iTechProgress /= 2;

						iValue *= (iTechProgress + 100);
						iValue /= 100;
					}
#endif
#if defined(MOD_BALANCE_CORE)
					if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsConquestOfTheWorld() && GET_PLAYER(kTradeConnection.m_eDestOwner).isGoldenAge())
					{
						iValue *= 2;
					}
#endif
					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_MODIFIER);
#if defined(MOD_BALANCE_CORE)				
					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER);
					}
#endif
					iValue *= iModifier;
					iValue /= 100;
				}
				break;
			case TRADE_CONNECTION_PRODUCTION:
				if (eYield == YIELD_PRODUCTION)
				{
#if defined(MOD_TRADE_ROUTE_SCALING)
					iValue = GD_INT_GET(TRADE_ROUTE_BASE_PRODUCTION_VALUE);
#else
					iValue = 300;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
					iValue += GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					iValue += GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
#endif
					iValue += GC.getEraInfo(GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra())->getTradeRouteProductionBonusTimes100();
					iValue *= (GC.getEraInfo(GC.getGame().getStartEra())->getConstructPercent() + GC.getEraInfo(GC.getGame().getStartEra())->getTrainPercent()) / 2;
					iValue /= 100;
#if defined(MOD_BALANCE_YIELD_SCALE_ERA)
					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					CvCity* pEndCity = NULL;
					CvPlot* pEndPlot = GC.getMap().plot(kTradeConnection.m_iDestX, kTradeConnection.m_iDestY);
					if (pEndPlot)
					{
						pEndCity = pEndPlot->getPlotCity();
					}
					if(MOD_BALANCE_YIELD_SCALE_ERA && pOriginCity != NULL && pEndCity != NULL)
					{
						int iStartCityPop = pOriginCity->getPopulation();
						int iEndCityPop = pEndCity->getPopulation();
						if(iStartCityPop > iEndCityPop)
						{
							int iDelta = ((iStartCityPop - iEndCityPop) / 3);
							iValue += max((iDelta * 100), 100);
						}
					}
#endif
#if defined(MOD_BALANCE_CORE)
					if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsConquestOfTheWorld() && GET_PLAYER(kTradeConnection.m_eDestOwner).isGoldenAge())
					{
						iValue *= 2;
					}
#endif
					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_MODIFIER);
#if defined(MOD_BALANCE_CORE)
					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER);
					}
#endif
					iValue *= iModifier;
					iValue /= 100;
				}
				break;
			}
		}
	}

	return iValue;	
}

//	--------------------------------------------------------------------------------
void CvPlayerTrade::UpdateTradeConnectionValues (void)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			for (uint uiYields = 0; uiYields < NUM_YIELD_TYPES; uiYields++)
			{
				pTrade->SetOriginYields(ui,uiYields,GetTradeConnectionValueTimes100(*pConnection, (YieldTypes)uiYields, true));
			}
		}

		if (pConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			for (uint uiYields = 0; uiYields < NUM_YIELD_TYPES; uiYields++)
			{
				pTrade->SetDestYields(ui,uiYields,GetTradeConnectionValueTimes100(*pConnection, (YieldTypes)uiYields, false));
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeValuesAtCityTimes100 (const CvCity *const pCity, YieldTypes eYield)
{
	int iResult = 0;
	int iCityX = pCity->getX();
	int iCityY = pCity->getY();

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if (pConnection->m_iOriginX == iCityX && pConnection->m_iOriginY == iCityY)
			{
				iResult += pConnection->m_aiOriginYields[eYield];
			}
		}
		
		if (pConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			if (pConnection->m_iDestX == iCityX && pConnection->m_iDestY == iCityY)
			{
				iResult += pConnection->m_aiDestYields[eYield];
			}
		}
	}

	return iResult;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetAllTradeValueTimes100 (YieldTypes eYield)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iTotal = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			iTotal += pConnection->m_aiOriginYields[eYield];
		}

		if (pConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			iTotal += pConnection->m_aiDestYields[eYield];
		}
	}

	return iTotal;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetAllTradeValueFromPlayerTimes100 (YieldTypes eYield, PlayerTypes ePlayer)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iTotal = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID() && pConnection->m_eDestOwner == ePlayer)
		{
			iTotal += pConnection->m_aiOriginYields[eYield];
		}

		if (pConnection->m_eDestOwner == m_pPlayer->GetID() && pConnection->m_eOriginOwner == ePlayer)
		{
			iTotal += pConnection->m_aiDestYields[eYield];
		}
	}

	return iTotal;
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::IsConnectedToPlayer(PlayerTypes eOtherPlayer)
{
	return GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), eOtherPlayer);
}


//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CanCreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath /* = true */)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();

	// if you can't see the plot, you're not allowed to connect it
	if (!pDestCity->plot()->isRevealed(m_pPlayer->getTeam(), false))
	{
		return false;
	}

	if (!m_pPlayer->IsAllowedToTradeWith(pDestCity->getOwner()))
	{
		return false;
	}

	if (pTrade->CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnectionType, bIgnoreExisting, bCheckPath))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CanCreateTradeRoute(PlayerTypes eOtherPlayer, DomainTypes eDomain)
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	int iCityLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		int iCityLoop2;
		CvCity* pLoopCity2;
		for (pLoopCity2 = GET_PLAYER(eOtherPlayer).firstCity(&iCityLoop2); pLoopCity2 != NULL; pLoopCity2 = GET_PLAYER(eOtherPlayer).nextCity(&iCityLoop2))
		{
			for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
			{
				// Check the route, but not the path
				if (CanCreateTradeRoute(pLoopCity, pLoopCity2, eDomain, (TradeConnectionType)uiConnectionTypes, false, false))
				{
					// Check the path
					if (pGameTrade->IsValidTradeRoutePath(pLoopCity, pLoopCity2, eDomain))
						return true;

					// else we can just break out of the loop
					break;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
//	Can the player create any trade routes in the domain, even with itself.
bool CvPlayerTrade::CanCreateTradeRoute(DomainTypes eDomain)
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	int iCityLoop;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		// Get a sorted list of nearby cities 
		const CvCityManager::CityList& kNearbyCities = CvCityManager::GetNearbyCities(pLoopCity);
		for (CvCityManager::CityList::const_iterator itr = kNearbyCities.begin(); itr != kNearbyCities.end(); ++itr)
		{
			CvCity* pLoopCity2 = (*itr);
			for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
			{
				// Check the route, but not the path
				if (CanCreateTradeRoute(pLoopCity, pLoopCity2, eDomain, (TradeConnectionType)uiConnectionTypes, false, false))
				{
					// Check the path
					if (pGameTrade->IsValidTradeRoutePath(pLoopCity, pLoopCity2, eDomain))
						return true;

					// else we can just break out of the loop
					break;
				}
			}
		}
	}

	return false;

}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType)
{
	int plotsX[MAX_PLOTS_TO_DISPLAY], plotsY[MAX_PLOTS_TO_DISPLAY];

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iRouteID = -1;
	bool bResult = pTrade->CreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnectionType, iRouteID);
	if (!bResult)
	{
		return false;
	}

	int iRouteIndex = pTrade->GetIndexFromID(iRouteID);
	if (iRouteIndex != -1)
	{
		int nPlots = pTrade->GetTradeConnection(iRouteIndex).m_aPlotList.size();
		if (nPlots > 0)
		{
			if (nPlots > MAX_PLOTS_TO_DISPLAY)
				nPlots = MAX_PLOTS_TO_DISPLAY;
			for (uint ui = 0; ui < (uint)nPlots; ui++) 
			{
				plotsX[ui] = pTrade->GetTradeConnection(iRouteIndex).m_aPlotList[ui].m_iX;
				plotsY[ui] = pTrade->GetTradeConnection(iRouteIndex).m_aPlotList[ui].m_iY;
			}
			gDLL->TradeVisuals_NewRoute(iRouteIndex, m_pPlayer->GetID(),pTrade->GetTradeConnection(iRouteIndex).m_eConnectionType, nPlots, plotsX, plotsY);
			gDLL->TradeVisuals_UpdateRouteDirection(iRouteIndex, pTrade->GetTradeConnection(iRouteIndex).m_bTradeUnitMovingForward);
#if defined(MOD_BALANCE_CORE)
			if(eConnectionType != TRADE_CONNECTION_INTERNATIONAL)
			{
				m_pPlayer->GetTreasury()->DoInternalTradeRouteGoldBonus();
			}
#endif
#if defined(MOD_BALANCE_CORE)
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes) iI;
				if(eYield == NO_YIELD)
					continue;

				pOriginCity->UpdateCityYields(eYield);
				pDestCity->UpdateCityYields(eYield);
			}
#endif
		}
	}

#if !defined(NO_ACHIEVEMENTS)
	if (m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		bool bConnectedToArabs = false;
		bool bConnectedToPersia = false;
		bool bConnectedToMongols = false;
		bool bConnectedToChina = false;
		bool bConnectedToIndia = false;

		CvGameTrade* pTrade = GC.getGame().GetGameTrade();
		for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
		{
			if (pTrade->IsTradeRouteIndexEmpty(ui))
			{
				continue;
			}

			const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));
			if (pConnection->m_eOriginOwner != m_pPlayer->GetID())
			{
				continue;
			}

			if (pConnection->m_eDomain != DOMAIN_LAND)
			{
				continue;
			}

			if (strcmp(GET_PLAYER(pConnection->m_eDestOwner).getCivilizationTypeKey(), "CIVILIZATION_ARABIA") == 0)
			{
				bConnectedToArabs = true;
			}
			else if (strcmp(GET_PLAYER(pConnection->m_eDestOwner).getCivilizationTypeKey(), "CIVILIZATION_PERSIA") == 0)
			{
				bConnectedToPersia = true;
			}
			else if (strcmp(GET_PLAYER(pConnection->m_eDestOwner).getCivilizationTypeKey(), "CIVILIZATION_MONGOL") == 0)
			{
				bConnectedToMongols = true;
			}
			else if (strcmp(GET_PLAYER(pConnection->m_eDestOwner).getCivilizationTypeKey(), "CIVILIZATION_CHINA") == 0)
			{
				bConnectedToChina = true;
			}
			else if (strcmp(GET_PLAYER(pConnection->m_eDestOwner).getCivilizationTypeKey(), "CIVILIZATION_INDIA") == 0)
			{
				bConnectedToIndia = true;
			}
		}

		if (bConnectedToArabs && bConnectedToPersia && bConnectedToMongols && bConnectedToChina && bConnectedToIndia)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_29);
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
const TradeConnection* CvPlayerTrade::GetTradeConnection(CvCity* pOriginCity, CvCity* pDestCity)
{
	int iOriginX = pOriginCity->getX();
	int iOriginY = pOriginCity->getY();
	int iDestX = pDestCity->getX();
	int iDestY = pDestCity->getY();

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));
		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if (pConnection->m_iOriginX == iOriginX && pConnection->m_iOriginY == iOriginY && pConnection->m_iDestX == iDestX && pConnection->m_iDestY == iDestY)
			{
				return pConnection;
			}
		}
	}

	return NULL;
}

//Returns the number of city state trade routes connected to a city
int CvPlayerTrade::GetNumberOfCityStateTradeRoutes()
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iNumConnections = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if(GET_PLAYER(pConnection->m_eDestOwner).isMinorCiv())
			{
				iNumConnections++;
			}
		}
	}

	return iNumConnections;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
//Returns the number of internal trade routes in your empire
int CvPlayerTrade::GetNumberOfInternalTradeRoutes()
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iNumConnections = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if(pConnection->m_eDestOwner == m_pPlayer->GetID())
			{
				iNumConnections++;
			}
		}
	}
	return iNumConnections;
}

//Returns the number of international trade routes to other empires
int CvPlayerTrade::GetNumberOfInternationalTradeRoutes(bool bOutgoing)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iNumConnections = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (bOutgoing && pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if(pConnection->m_eDestOwner != m_pPlayer->GetID())
			{
				iNumConnections++;
			}
		}

		if (!bOutgoing && pConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			if(pConnection->m_eOriginOwner != m_pPlayer->GetID())
			{
				iNumConnections++;
			}
		}
	}
	return iNumConnections;
}

//Returns the number of trade routes in your empire
int CvPlayerTrade::GetNumberOfTradeRoutes()
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iNumConnections = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			iNumConnections++;
		}
	}
	return iNumConnections;
}
#endif

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::IsPreviousTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType)
{
	int iOriginCityX = pOriginCity->getX();
	int iOriginCityY = pOriginCity->getY();
	int iDestCityX = pDestCity->getX();
	int iDestCityY = pDestCity->getY();

	for (uint ui = 0; ui < m_aRecentlyExpiredConnections.size(); ui++)
	{
		if (m_aRecentlyExpiredConnections[ui].m_iOriginX == iOriginCityX &&
			m_aRecentlyExpiredConnections[ui].m_iOriginY == iOriginCityY &&
			m_aRecentlyExpiredConnections[ui].m_iDestX == iDestCityX &&
			m_aRecentlyExpiredConnections[ui].m_iDestY == iDestCityY &&
			m_aRecentlyExpiredConnections[ui].m_eDomain == eDomain &&
			m_aRecentlyExpiredConnections[ui].m_eConnectionType == eConnectionType) 
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetNumPotentialConnections (CvCity* pFromCity, DomainTypes eDomain, bool bNeedOnlyOne)
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	CvWeightedVector<CvCity*> vPossibleTargets;

	int iNumValid = 0;
	for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;

		if (!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		if (GET_PLAYER(ePlayer).IsAtWarWith(m_pPlayer->GetID()))
		{
			continue;
		}

		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			int iDistance = ::plotDistance(pFromCity->getX(), pFromCity->getY(), pLoopCity->getX(), pLoopCity->getY());
			vPossibleTargets.push_back(pLoopCity, 1000 - iDistance);
		}
	}

	//this should put the closest cities first
	vPossibleTargets.SortItems();

	for (int i = 0; i < vPossibleTargets.size(); i++)
	{
		CvCity* pLoopCity = vPossibleTargets.GetElement(i);

		bool bCheckPath = true;
		for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
		{
			// Check the trade route, ignoring the path for now
			if (CanCreateTradeRoute(pFromCity, pLoopCity, eDomain, (TradeConnectionType)uiConnectionTypes, false, false))
			{
				// Now test the path
				if (bCheckPath)
				{
					bool bTradeAvailable = pGameTrade->IsValidTradeRoutePath(pFromCity, pLoopCity, eDomain);
					if (!bTradeAvailable)
						break;		// If there is no path for this domain, just skip the rest of the connection tests.

					bCheckPath = false;		// No need to check the path for this domain again
				}

				iNumValid++;

				if (bNeedOnlyOne)
					return 1;
			}
		}
	}

	return iNumValid;
}

//	--------------------------------------------------------------------------------
std::vector<int> CvPlayerTrade::GetTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar)
{
	std::vector<int> aiTradeConnectionIDs;

	if (pPlot == NULL)
	{
		return aiTradeConnectionIDs;
	}

	int iX = pPlot->getX();
	int iY = pPlot->getY();

	TeamTypes eMyTeam = m_pPlayer->getTeam();

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(uiConnection));
		if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
		{
			continue;
		}

		TeamTypes eOtherTeam = GET_PLAYER(pConnection->m_eOriginOwner).getTeam();

		bool bIgnore = false;
		if (bExcludingMe && eOtherTeam == eMyTeam)
		{
			bIgnore = true;
		}

		if (bOnlyWar && !GET_TEAM(eMyTeam).isAtWar(eOtherTeam))
		{
			if (m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar())
			{
				if (pConnection->m_eDestOwner == m_pPlayer->GetID())
					bIgnore = true;
			}
			else
			{
				bIgnore = true;
			}
		}

		if (bIgnore)
		{
			continue;
		}

		if (pConnection->m_aPlotList[pConnection->m_iTradeUnitLocationIndex].m_iX == iX && pConnection->m_aPlotList[pConnection->m_iTradeUnitLocationIndex].m_iY == iY)
		{
			aiTradeConnectionIDs.push_back(pConnection->m_iID);
			if (bFailAtFirstFound)
			{
				break;
			}
		}
	}

	return aiTradeConnectionIDs;	
}


//	--------------------------------------------------------------------------------
std::vector<int> CvPlayerTrade::GetTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound, bool bExcludingMe, bool bOnlyWar)
{
	std::vector<int> aiTradeConnectionIDs;

	if (pPlot == NULL)
	{
		return aiTradeConnectionIDs;
	}

	int iX = pPlot->getX();
	int iY = pPlot->getY();

	TeamTypes eMyTeam = m_pPlayer->getTeam();

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(uiConnection));
		if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
		{
			continue;
		}

		TeamTypes eOtherTeam = GET_PLAYER(pConnection->m_eOriginOwner).getTeam();

		bool bIgnore = false;
		if (bExcludingMe && eOtherTeam == eMyTeam)
		{
			bIgnore = true;
		}

		if (bOnlyWar && !GET_TEAM(eMyTeam).isAtWar(eOtherTeam))
		{
			if (m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar())
			{
				if (pConnection->m_eDestOwner == m_pPlayer->GetID())
					bIgnore = true;
				else if (!m_pPlayer->isHuman() && m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(pConnection->m_eOriginOwner) >= MAJOR_CIV_OPINION_NEUTRAL)
					bIgnore = true;
				else if (!m_pPlayer->isHuman() && m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(pConnection->m_eOriginOwner, true) >= MAJOR_CIV_APPROACH_AFRAID)
					bIgnore = true;
			}
			else
			{
				bIgnore = true;
			}
		}

		if (bIgnore)
		{
			continue;
		}

		for (uint ui = 0; ui < pConnection->m_aPlotList.size(); ui++)
		{
			if (pConnection->m_aPlotList[ui].m_iX == iX && pConnection->m_aPlotList[ui].m_iY == iY)
			{
				aiTradeConnectionIDs.push_back(pConnection->m_iID);
				if (bFailAtFirstFound)
				{
					break;
				}
			}
		}

		if (bFailAtFirstFound && aiTradeConnectionIDs.size() > 0)
		{
			break;
		}
	}

	return aiTradeConnectionIDs;	
}

//	--------------------------------------------------------------------------------
std::vector<int> CvPlayerTrade::GetOpposingTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound)
{
	return GetTradeUnitsAtPlot(pPlot, bFailAtFirstFound, true, false);
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::ContainsOpposingPlayerTradeUnit(const CvPlot* pPlot)
{
	std::vector<int> aiTradeConnectionIDs;
	aiTradeConnectionIDs = GetOpposingTradeUnitsAtPlot(pPlot, true);
	if (aiTradeConnectionIDs.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
std::vector<int> CvPlayerTrade::GetEnemyTradeUnitsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound)
{
	return GetTradeUnitsAtPlot(pPlot, bFailAtFirstFound, true, true);
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::ContainsEnemyTradeUnit(const CvPlot* pPlot)
{
	std::vector<int> aiTradeConnectionIDs;
	aiTradeConnectionIDs = GetEnemyTradeUnitsAtPlot(pPlot, true);
	if (aiTradeConnectionIDs.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
std::vector<int> CvPlayerTrade::GetEnemyTradePlotsAtPlot(const CvPlot* pPlot, bool bFailAtFirstFound)
{
	return GetTradePlotsAtPlot(pPlot, bFailAtFirstFound, true, true);
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::ContainsEnemyTradePlot(const CvPlot* pPlot)
{
	std::vector<int> aiTradeConnectionIDs;
	aiTradeConnectionIDs = GetEnemyTradePlotsAtPlot(pPlot, true);
	if (aiTradeConnectionIDs.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
#if defined(MOD_API_EXTENSIONS)
bool CvPlayerTrade::PlunderTradeRoute(int iTradeConnectionID, CvUnit* pUnit)
#else
bool CvPlayerTrade::PlunderTradeRoute(int iTradeConnectionID)
#endif
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iTradeConnectionIndex = pTrade->GetIndexFromID(iTradeConnectionID);

	CvAssertMsg(iTradeConnectionIndex >= 0, "iTradeConnectionIndex < 0");
	if (iTradeConnectionIndex < 0)
	{
		return false;
	}

	const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(iTradeConnectionIndex));
#if defined(MOD_EVENTS_TRADE_ROUTE_PLUNDERED)
	TradeConnectionType eConnectionType = pTradeConnection->m_eConnectionType;
#endif
	DomainTypes eDomain = pTradeConnection->m_eDomain;
	PlayerTypes eOwningPlayer = pTradeConnection->m_eOriginOwner;
	PlayerTypes eDestPlayer = pTradeConnection->m_eDestOwner;
	TeamTypes eOwningTeam = GET_PLAYER(eOwningPlayer).getTeam();
	TeamTypes eDestTeam = GET_PLAYER(eDestPlayer).getTeam();
	CvPlot* pPlunderPlot = GC.getMap().plot(pTradeConnection->m_aPlotList[pTradeConnection->m_iTradeUnitLocationIndex].m_iX, pTradeConnection->m_aPlotList[pTradeConnection->m_iTradeUnitLocationIndex].m_iY);
	int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(*pTradeConnection, YIELD_GOLD);

	// add trade connnection to broken list
	GET_PLAYER(eOwningPlayer).GetTrade()->AddTradeConnectionWasPlundered(*pTradeConnection);

	CvCity* pOriginCity = NULL;
	CvCity* pDestCity = NULL;
	CvPlot* pPlot = GC.getMap().plot(pTradeConnection->m_iOriginX, pTradeConnection->m_iOriginY);
	if (pPlot)
	{
		pOriginCity = pPlot->getPlotCity();
	}
	pPlot = GC.getMap().plot(pTradeConnection->m_iDestX, pTradeConnection->m_iDestY);
	if (pPlot)
	{
		pDestCity = pPlot->getPlotCity();
	}
	CvAssertMsg(pOriginCity, "pOriginCity doesn't exist");
	CvAssertMsg(pDestCity, "pDestCity doesn't exist");

	bool bEmptyResult = pTrade->EmptyTradeRoute(iTradeConnectionIndex);

	// if the trade route was not broken
	if (!bEmptyResult)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(pOriginCity != NULL && pDestCity != NULL)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			YieldTypes eYield = (YieldTypes) iI;
			if(eYield == NO_YIELD)
				continue;

			pOriginCity->UpdateCityYields(eYield);
			pDestCity->UpdateCityYields(eYield);
		}
	}
#endif

#if defined(MOD_TRADE_ROUTE_SCALING)
	int iPlunderGoldValue = GD_INT_GET(TRADE_ROUTE_BASE_PLUNDER_GOLD);
#else
	int iPlunderGoldValue = 100;
#endif
	iPlunderGoldValue *= 100 + iDomainModifier;
	iPlunderGoldValue /= 100;
#if defined(MOD_BALANCE_CORE)
	iPlunderGoldValue *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	if(pUnit && pUnit->isHighSeaRaider())
	{
		iPlunderGoldValue *= 3;
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iJ);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
			if(pkPromotionInfo != NULL)
			{
				if(pkPromotionInfo->IsHighSeaRaider())
				{
					pUnit->setHasPromotion(eLoopPromotion, true);
				}
			}
		}
	}
	iPlunderGoldValue /= 100;
#endif
	m_pPlayer->GetTreasury()->ChangeGold(iPlunderGoldValue);

	// do the floating popup
	if (GC.getGame().getActivePlayer() == m_pPlayer->GetID())
	{
		char text[256] = {0};
		sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iPlunderGoldValue);
#if defined(SHOW_PLOT_POPUP)
		SHOW_PLOT_POPUP(pPlunderPlot, m_pPlayer->GetID(), text, 0.0f);
#else
		float fDelay = 0.0f;
		DLLUI->AddPopupText(pPlunderPlot->getX(), pPlunderPlot->getY(), text, fDelay);
#endif
		CvString strBuffer;

#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPlunderGoldValue, GC.getUnitInfo(GetTradeUnit(eDomain, m_pPlayer))->GetDescriptionKey());
#else
		if (eDomain == DOMAIN_LAND)
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPlunderGoldValue, "TXT_KEY_UNIT_CARAVAN");
		}
		else
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPlunderGoldValue, "TXT_KEY_UNIT_CARGO_SHIP");
		}
#endif
		
		DLLUI->AddMessage(0, m_pPlayer->GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
	}

	// barbarians get a bonus unit out of the deal!
	if (m_pPlayer->isBarbarian() && pPlot)
	{
		int iLoop = 0;
		for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			if (pLoopUnit->plot() == pPlunderPlot)
			{
				CvBarbarians::DoSpawnBarbarianUnit(pPlunderPlot, true, true);
				break;
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	if((eOwningPlayer != NO_PLAYER && !m_pPlayer->isBarbarian() && !GET_PLAYER(eOwningPlayer).isBarbarian()) && GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOwningPlayer).getTeam()))
	{
		// Notify Diplo AI that damage has been done
		int iValue = iPlunderGoldValue;
		if(iValue > 0)
		{
			int iWarscoremod = m_pPlayer->GetWarScoreModifier();
			if (iWarscoremod != 0)
			{
				iValue *= (iWarscoremod + 100);
				iValue /= 100;
			}

			// My viewpoint
			m_pPlayer->GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(eOwningPlayer, m_pPlayer->GetID(), iValue);
			// Bad guy's viewpoint
			GET_PLAYER(eOwningPlayer).GetDiplomacyAI()->ChangeWarValueLost(m_pPlayer->GetID(), iValue);
		}
	}
	if (pPlunderPlot->isVisible(eOwningTeam) && m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar() && !GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOwningPlayer).getTeam()))
	{
		GET_PLAYER(eOwningPlayer).GetDiplomacyAI()->ChangeNumTimesRobbedBy(m_pPlayer->GetID(), 2);
	}
#endif
	// do the notification stuff
	if (pOriginCity && pDestCity)
	{
		// send notification to owner player
		CvNotifications* pNotifications = GET_PLAYER(eOwningPlayer).GetNotifications();
		if(pNotifications)
		{
			Localization::String strSummary;
			Localization::String strMessage;

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADER_SUMMARY");
			if (m_pPlayer->isBarbarian() || (m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar() && !pPlunderPlot->isVisible(eOwningTeam)))
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADER_BARBARIAN");
				strMessage << pOriginCity->getNameKey();
				strMessage << pDestCity->getNameKey();
			}
			else if (GET_TEAM(eOwningTeam).isHasMet(m_pPlayer->getTeam()))
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADER_KNOWN");
				if(GC.getGame().isGameMultiPlayer() && m_pPlayer->isHuman())
				{
					strMessage << m_pPlayer->getNickName();
				}
				else
				{
					strMessage << m_pPlayer->getNameKey();
				}
				strMessage << pOriginCity->getNameKey();
				strMessage << pDestCity->getNameKey();
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADER_UNKNOWN");
				strMessage << pOriginCity->getNameKey();
				strMessage << pDestCity->getNameKey();
			}

			pNotifications->Add(NOTIFICATION_TRADE_ROUTE_BROKEN, strMessage.toUTF8(), strSummary.toUTF8(), pPlunderPlot->getX(), pPlunderPlot->getY(), -1);
		}

		if (!GET_PLAYER(eDestPlayer).isMinorCiv() && eOwningPlayer != eDestPlayer && eDestPlayer != m_pPlayer->GetID())
		{
			// send notification to destination player
			pNotifications = GET_PLAYER(eDestPlayer).GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary;
				Localization::String strMessage;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADEE_SUMMARY");
				if (m_pPlayer->isBarbarian())
				{
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADEE_BARBARIANS");
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eOwningPlayer).isHuman())
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNickName();
					}
					else
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNameKey();
					}
					strMessage << pOriginCity->getNameKey();
					strMessage << pDestCity->getNameKey();
				}
				else if (GET_TEAM(eDestTeam).isHasMet(m_pPlayer->getTeam()))
				{
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADEE_KNOWN");
					if(GC.getGame().isGameMultiPlayer() && m_pPlayer->isHuman())
					{
						strMessage << m_pPlayer->getNickName();
					}
					else
					{
						strMessage << m_pPlayer->getNameKey();
					}

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eOwningPlayer).isHuman())
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNickName();
					}
					else
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNameKey();
					}

					strMessage << pOriginCity->getNameKey();
					strMessage << pDestCity->getNameKey();
				}
				else
				{
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADEE_UNKNOWN");

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(eOwningPlayer).isHuman())
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNickName();
					}
					else
					{
						strMessage << GET_PLAYER(eOwningPlayer).getNameKey();
					}

					strMessage << pOriginCity->getNameKey();
					strMessage << pDestCity->getNameKey();
				}

				pNotifications->Add(NOTIFICATION_TRADE_ROUTE_BROKEN, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
	}

#if !defined(NO_ACHIEVEMENTS)
	if (eDomain == DOMAIN_LAND && m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP2_28);
	}
#endif

#if defined(MOD_EVENTS_TRADE_ROUTE_PLUNDERED)
	if (MOD_EVENTS_TRADE_ROUTE_PLUNDERED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerPlunderedTradeRoute, pUnit->getOwner(), pUnit->GetID(), iPlunderGoldValue, pOriginCity->getOwner(), pOriginCity->GetID(), pDestCity->getOwner(), pDestCity->GetID(), eConnectionType, eDomain);
	}
#endif
#if defined(MOD_BALANCE_CORE)
	 if(pTradeConnection->m_aPlotList.size() > 0)
	 {
		for (uint ui = 0; ui < pTradeConnection->m_aPlotList.size(); ui++)
		{
			int iPlotX = pTradeConnection->m_aPlotList[ui].m_iX;
			int iPlotY = pTradeConnection->m_aPlotList[ui].m_iY;
			if(iPlotX != NULL && iPlotY != NULL)
			{
				CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
				if(pPlot != NULL)
				{
					pPlot->updateYield();
				}
			}
		}
	}
#endif

	return true;
}
void CvPlayerTrade::UpdateFurthestPossibleTradeRoute(DomainTypes eDomain, CvCity* pOriginCity, int iMaxRange)
{
	int iLongestRoute = 0;

	// Check the route, but not the path
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
	{
		int iCity;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
		{
			if (CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, TRADE_CONNECTION_INTERNATIONAL, false, false))
			{
				int iLength = GC.getGame().GetGameTrade()->GetValidTradeRoutePathLength(pOriginCity, pDestCity, eDomain);
				if (iLength <= 0)
					continue;

				if (iLength > iLongestRoute)
				{
					iLongestRoute = iLength;
					if (iLongestRoute == iMaxRange)
						break;
				}
			}
		}
	}
	pOriginCity->SetLongestPotentialTradeRoute(iLongestRoute == 0 ? iMaxRange : iLongestRoute, eDomain);
}
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeRouteRange (DomainTypes eDomain, CvCity* pOriginCity)
{
	int iRange = 0;
	int iBaseRange = 0;
	switch (eDomain)
	{
	case DOMAIN_SEA:
#if defined(MOD_TRADE_ROUTE_SCALING)
		iBaseRange = GD_INT_GET(TRADE_ROUTE_BASE_SEA_DISTANCE);
#else
		iBaseRange = 20;
#endif
		break;
	case DOMAIN_LAND:
#if defined(MOD_TRADE_ROUTE_SCALING)
		iBaseRange = GD_INT_GET(TRADE_ROUTE_BASE_LAND_DISTANCE);
#else
		iBaseRange = 10;
#endif
		break;
	default:
		CvAssertMsg(false, "Undefined domain for trade route range");
		return -1;
		break;
	}

	int iTraitRange = 0;
	switch (eDomain)
	{
	case DOMAIN_SEA:
#if defined(MOD_TRAITS_TRADE_ROUTE_BONUSES)
		iTraitRange = m_pPlayer->GetPlayerTraits()->GetSeaTradeRouteRangeBonus();
#else
		// not implemented
		iTraitRange = 0;
#endif
		break;
	case DOMAIN_LAND:
		iTraitRange = m_pPlayer->GetPlayerTraits()->GetLandTradeRouteRangeBonus();
		break;
	}

	int iExtendedRange = GET_TEAM(GET_PLAYER(m_pPlayer->GetID()).getTeam()).getTradeRouteDomainExtraRange(eDomain);

	int iRangeModifier = 0;
	if (pOriginCity != NULL)
	{
		switch (eDomain)
		{
		case DOMAIN_SEA:
			iRangeModifier = pOriginCity->GetTradeRouteSeaDistanceModifier();
#if defined(MOD_BALANCE_CORE)
			iRangeModifier += m_pPlayer->getTradeRouteSeaDistanceModifier();
#endif
			break;
		case DOMAIN_LAND:
			iRangeModifier = pOriginCity->GetTradeRouteLandDistanceModifier();
#if defined(MOD_BALANCE_CORE)
			iRangeModifier += m_pPlayer->getTradeRouteLandDistanceModifier();
#endif
			break;
		}
	}

#if defined(MOD_BALANCE_CORE)
	CorporationTypes eCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();
	if (eCorporation != NO_CORPORATION)
	{
		CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
		if (pkCorporationInfo)
		{
			if (eDomain == DOMAIN_LAND && pkCorporationInfo->GetTradeRouteLandDistanceModifier() != 0)
			{
				iRangeModifier += pkCorporationInfo->GetTradeRouteLandDistanceModifier();
			}
			else if (eDomain == DOMAIN_SEA && pkCorporationInfo->GetTradeRouteSeaDistanceModifier() != 0)
			{
				iRangeModifier += pkCorporationInfo->GetTradeRouteSeaDistanceModifier();
			}
		}
	}
#endif

	iRange = iBaseRange;
	iRange += iTraitRange;
	iRange += iExtendedRange;

#if defined(MOD_TRADE_ROUTE_SCALING)
	int iRouteModifier = GC.getMap().getWorldInfo().getTradeRouteDistanceMod();
#else
	int iRouteModifier = 0;
#endif

	iRange = (iRange * (iRouteModifier + iRangeModifier)) / 100;
	return iRange;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeRouteSpeed (DomainTypes eDomain)
{
#if defined(MOD_TRADE_ROUTE_SCALING)
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
	UnitTypes eUnitType = GetTradeUnit(eDomain, m_pPlayer);
#else
	UnitTypes eUnitType = GetTradeUnit(eDomain);
#endif
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);

	if (pkUnitInfo) {
#if defined(MOD_BALANCE_CORE)
		int iMoves = pkUnitInfo->GetMoves();
		if (m_pPlayer->GetTRSpeedBoost() > 0)
		{
			iMoves *= m_pPlayer->GetTRSpeedBoost();
		}

		// Corporation trade route modifier
		CorporationTypes ePlayerCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();
		if (ePlayerCorporation != NO_CORPORATION)
		{
			CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(ePlayerCorporation);
			if (pkCorporationInfo && pkCorporationInfo->GetTradeRouteSpeedModifier() > 0)
			{
				iMoves *= pkCorporationInfo->GetTradeRouteSpeedModifier();
				iMoves /= 100;
			}
		}
		return iMoves;
#else
		return pkUnitInfo->GetMoves();
#endif
	}
#else
	switch (eDomain)
	{
	case DOMAIN_SEA:
		return 4;
		break;
	case DOMAIN_LAND:
		return 2;
		break;
	}
#endif

	CvAssertMsg(false, "Undefined domain for trade route speed");
	return -1;
}

//	--------------------------------------------------------------------------------
uint CvPlayerTrade::GetNumTradeRoutesPossible (void)
{
	int iNumRoutes = 0;

	CvAssert(m_pPlayer->getCivilizationType() != NO_CIVILIZATION);
	if (m_pPlayer->getCivilizationType() == NO_CIVILIZATION)
		return 0;

	CvPlayerTechs* pMyPlayerTechs = m_pPlayer->GetPlayerTechs();
	CvTeamTechs* pMyTeamTechs = GET_TEAM(GET_PLAYER(m_pPlayer->GetID()).getTeam()).GetTeamTechs();
	CvTechEntry* pTechInfo = NULL; 

	CvTechXMLEntries* pMyPlayerTechEntries = pMyPlayerTechs->GetTechs();
	CvAssert(pMyPlayerTechEntries);
	if (pMyPlayerTechEntries == NULL)
		return 0;

	for(int iTechLoop = 0; iTechLoop < pMyPlayerTechEntries->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;
		if (!pMyTeamTechs->HasTech(eTech))
		{
			continue;
		}

		pTechInfo = pMyPlayerTechEntries->GetEntry(eTech);
		CvAssertMsg(pTechInfo, "null tech entry");
		if (pTechInfo)
		{
			iNumRoutes += pTechInfo->GetNumInternationalTradeRoutesChange();
		}
	}

	const CvCivilizationInfo& kCivInfo = m_pPlayer->getCivilizationInfo();
	int iLoop = 0;
	CvCity* pLoopCity;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)kCivInfo.getCivilizationBuildings(iI);
			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);
				if (!pBuildingEntry)
				{
					continue;
				}

				if (pBuildingEntry)
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)pBuildingEntry->GetID()))
					{
						int iNumRouteBonus = pBuildingEntry->GetNumTradeRouteBonus();
						if (iNumRouteBonus != 0)
						{
#if defined(MOD_BUGFIX_MINOR)
							iNumRoutes += (iNumRouteBonus * pLoopCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)pBuildingEntry->GetID()));
#else
							iNumRoutes += iNumRouteBonus;
#endif
						}
					}
				}
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	CorporationTypes eCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();
	if (eCorporation != NO_CORPORATION)
	{
		CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
		if (pkCorporationInfo)
		{
			int iNumTradeRouteBonus = pkCorporationInfo->GetNumFreeTradeRoutes();
			if (iNumTradeRouteBonus)
			{
				iNumRoutes += iNumTradeRouteBonus;
			}
		}
	}
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
	if(m_pPlayer->GetFreeTradeRoute() > 0)
	{
		iNumRoutes += m_pPlayer->GetFreeTradeRoute();
	}
#endif

	int iModifier = 100 + m_pPlayer->GetPlayerTraits()->GetNumTradeRoutesModifier();
	iNumRoutes *= iModifier;
	iNumRoutes /= 100;

	return iNumRoutes;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetNumTradeUnits(bool bIncludeBeingBuilt)
{
	int iReturnValue = 0;
	int iLoop;

	// look for units on the map
	CvUnit* pLoopUnit;
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->isTrade())
		{
			iReturnValue++;
		}
	}

	// look inside cities
	if (bIncludeBeingBuilt)
	{
		CvCity* pLoopCity;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			iReturnValue += pLoopCity->getNumTrainUnitAI(UNITAI_TRADE_UNIT);
		}
	}

	return iReturnValue;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetNumTradeUnitsRemaining (bool bIncludeBeingBuilt)
{
	return (GetNumTradeRoutesPossible() - GetNumTradeUnits(bIncludeBeingBuilt));
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetNumDifferentTradingPartners (void)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();

	std::vector<bool> abConnections;
	abConnections.resize(MAX_CIV_PLAYERS, false);

	int iResult = 0;

	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(ui));
		if (pTradeConnection->m_eOriginOwner == pTradeConnection->m_eDestOwner)
		{
			continue;
		}

		// this involves us
		if (pTradeConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if (!abConnections[pTradeConnection->m_eDestOwner])
			{
				abConnections[pTradeConnection->m_eDestOwner] = true;
				iResult++;
			}
		}
		else if (pTradeConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			if (!abConnections[pTradeConnection->m_eOriginOwner])
			{
				abConnections[pTradeConnection->m_eOriginOwner] = true;
				iResult++;
			}
		}
	}

	return iResult;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetNumDifferentMajorCivTradingPartners(void)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();

	std::vector<bool> abConnections;
	abConnections.resize(MAX_CIV_PLAYERS, false);

	int iResult = 0;

	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(ui));
		if (pTradeConnection->m_eOriginOwner == pTradeConnection->m_eDestOwner)
		{
			continue;
		}

		if (GET_PLAYER(pTradeConnection->m_eDestOwner).isMinorCiv())
			continue;

		if (GET_PLAYER(pTradeConnection->m_eOriginOwner).isMinorCiv())
			continue;

		// this involves us
		if (pTradeConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if (!abConnections[pTradeConnection->m_eDestOwner])
			{
				abConnections[pTradeConnection->m_eDestOwner] = true;
				iResult++;
			}
		}
		else if (pTradeConnection->m_eDestOwner == m_pPlayer->GetID())
		{
			if (!abConnections[pTradeConnection->m_eOriginOwner])
			{
				abConnections[pTradeConnection->m_eOriginOwner] = true;
				iResult++;
			}
		}
	}

	return iResult;
}
#endif

//	--------------------------------------------------------------------------------
void CvPlayerTrade::UpdateTradeConnectionWasPlundered()
{
#if defined(MOD_TRADE_ROUTE_SCALING)
	int iTurnsUntilForget = GD_INT_GET(TRADE_ROUTE_PLUNDER_TURNS_COUNTER);
	iTurnsUntilForget = iTurnsUntilForget * GC.getGame().getGameSpeedInfo().getTradeRouteSpeedMod() / 100;
#else
	int iTurnsUntilForget = 30;
#endif
	for (uint ui = 0; ui < m_aTradeConnectionWasPlundered.size(); ui++)
	{
		if ((m_aTradeConnectionWasPlundered[ui].m_iTurnPlundered + iTurnsUntilForget) <= GC.getGame().getGameTurn())
		{
			// setting turn plundered to -1 to flag it as not counting
			m_aTradeConnectionWasPlundered[ui].m_iTurnPlundered = -1;
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlayerTrade::AddTradeConnectionWasPlundered(const TradeConnection& kTradeConnection)
{
	bool bAdded = false;
	for (uint ui = 0; ui < m_aTradeConnectionWasPlundered.size(); ui++)
	{
		if (m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iOriginX == kTradeConnection.m_iOriginX &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iOriginY == kTradeConnection.m_iOriginY &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iDestX   == kTradeConnection.m_iDestX &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iDestY   == kTradeConnection.m_iDestY)
		{
			m_aTradeConnectionWasPlundered[ui].m_iTurnPlundered = GC.getGame().getGameTurn();
			bAdded = true;
			break;
		}
	}

	if (!bAdded)
	{
		TradeConnectionWasPlundered kWasPlundered;
		uint uiIndex = m_aTradeConnectionWasPlundered.size();
		m_aTradeConnectionWasPlundered.push_back(kWasPlundered);
		m_aTradeConnectionWasPlundered[uiIndex].m_kTradeConnection = kTradeConnection;
		m_aTradeConnectionWasPlundered[uiIndex].m_iTurnPlundered = GC.getGame().getGameTurn();
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CheckTradeConnectionWasPlundered(const TradeConnection& kTradeConnection)
{
	for (uint ui = 0; ui < m_aTradeConnectionWasPlundered.size(); ui++)
	{
		if (m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iOriginX == kTradeConnection.m_iOriginX &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iOriginY == kTradeConnection.m_iOriginY &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iDestX   == kTradeConnection.m_iDestX &&
			m_aTradeConnectionWasPlundered[ui].m_kTradeConnection.m_iDestY   == kTradeConnection.m_iDestY &&
			m_aTradeConnectionWasPlundered[ui].m_iTurnPlundered != -1)
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
UnitTypes CvPlayerTrade::GetTradeUnit (DomainTypes eDomain, CvPlayer* pPlayer)
#else
UnitTypes CvPlayerTrade::GetTradeUnit (DomainTypes eDomain)
#endif
{
	UnitTypes eUnitType = NO_UNIT;
	if (eDomain == DOMAIN_LAND)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		eUnitType = pPlayer->GetSpecificUnitType("UNITCLASS_CARAVAN");
#else
		eUnitType = (UnitTypes)GC.getInfoTypeForString("UNIT_CARAVAN");
#endif
	}
	else if (eDomain == DOMAIN_SEA)
	{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
		eUnitType = pPlayer->GetSpecificUnitType("UNITCLASS_CARGO_SHIP");
#else
		eUnitType = (UnitTypes)GC.getInfoTypeForString("UNIT_CARGO_SHIP");
#endif
	}

	return eUnitType;
}

//	--------------------------------------------------------------------------------
std::vector<CvString> CvPlayerTrade::GetPlotToolTips (CvPlot* pPlot)
{
	std::vector<CvString> aToolTips;

	if (!pPlot)
	{
		return aToolTips;
	}

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	PlayerTypes ePlayer = m_pPlayer->GetID();
	int iX = pPlot->getX();
	int iY = pPlot->getY();
	CvCity* pOriginCity = NULL;
	CvCity* pDestCity = NULL;

	Localization::String strResult;

	for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
		{
			continue;
		}

		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(uiConnection));

		bool bDisplayInfo = false;
		if (pConnection->m_eOriginOwner == ePlayer || pConnection->m_eDestOwner == ePlayer)
		{
			bDisplayInfo = true;
		}
		else
		{
			int iTradeUnitIndex = pConnection->m_iTradeUnitLocationIndex;
			int iTradeUnitX = pConnection->m_aPlotList[iTradeUnitIndex].m_iX;
			int iTradeUnitY = pConnection->m_aPlotList[iTradeUnitIndex].m_iY;
			if (iTradeUnitX == iX && iTradeUnitY == iY)
			{
				bDisplayInfo = true;
			}
		}

		// if this trade route involves us
		if (bDisplayInfo)
		{
			for (uint uiPlotIndex = 0; uiPlotIndex < pConnection->m_aPlotList.size(); uiPlotIndex++)
			{
				if (pConnection->m_aPlotList[uiPlotIndex].m_iX == iX && pConnection->m_aPlotList[uiPlotIndex].m_iY == iY)
				{
					pOriginCity = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY)->getPlotCity();
					pDestCity = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY)->getPlotCity();

					// Trade Routes:
					// (if major to major)
					// Kyoto (Japan) ~~ London (England)
					// (if major to minor)
					// Kyoto (Japan) ~~ Monaco (City-State)

					if (uiPlotIndex == pConnection->m_iTradeUnitLocationIndex)
					{
						if (pConnection->m_eDomain == DOMAIN_LAND)
						{
							Localization::String strTradeUnit = Localization::Lookup("TXT_KEY_TRADE_ROUTE_TT_PLOT_CARAVAN");
							strTradeUnit << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationAdjectiveKey();
							aToolTips.push_back(strTradeUnit.toUTF8());
						}
						else if (pConnection->m_eDomain == DOMAIN_SEA)
						{
							Localization::String strTradeUnit = Localization::Lookup("TXT_KEY_TRADE_ROUTE_TT_PLOT_CARGO_SHIP");
							strTradeUnit << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationAdjectiveKey();
							aToolTips.push_back(strTradeUnit.toUTF8());
						}
					}

					Localization::String strTradeLine;

					if (GET_PLAYER(pConnection->m_eDestOwner).isMinorCiv()) // if a minor civ, have a special help text
					{
						strTradeLine = Localization::Lookup("TXT_KEY_TRADE_ROUTE_TT_PLOT_MAJOR_MINOR");
						strTradeLine << ((pOriginCity)?pOriginCity->getNameKey():"NULL Origin City");
						strTradeLine << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationShortDescriptionKey();
						strTradeLine << ((pDestCity)?pDestCity->getNameKey():"NULL Dest City");
					}
					else
					{
						strTradeLine = Localization::Lookup("TXT_KEY_TRADE_ROUTE_TT_PLOT_MAJOR_MAJOR");
						strTradeLine << ((pOriginCity)?pOriginCity->getNameKey():"NULL Origin City");
						strTradeLine << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationShortDescriptionKey();
						strTradeLine << ((pDestCity)?pDestCity->getNameKey():"NULL Dest City");

						strTradeLine << GET_PLAYER(pConnection->m_eDestOwner).getCivilizationShortDescriptionKey();
					}

					aToolTips.push_back(strTradeLine.toUTF8());
				}
			}
		}
	}

	return aToolTips;
}

//	--------------------------------------------------------------------------------
std::vector<CvString> CvPlayerTrade::GetPlotMouseoverToolTips (CvPlot* pPlot)
{
	std::vector<CvString> aToolTips;

	if (!pPlot)
	{
		return aToolTips;
	}

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	PlayerTypes ePlayer = m_pPlayer->GetID();
	int iX = pPlot->getX();
	int iY = pPlot->getY();
	CvCity* pOriginCity = NULL;
	CvCity* pDestCity = NULL;

	Localization::String strResult;

	for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
	{
		if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
		{
			continue;
		}

		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(uiConnection));

		bool bDisplayInfo = false;
		if (pConnection->m_eOriginOwner == ePlayer || pConnection->m_eDestOwner == ePlayer)
		{
			bDisplayInfo = true;
		}
		else
		{
			int iTradeUnitIndex = pConnection->m_iTradeUnitLocationIndex;
			int iTradeUnitX = pConnection->m_aPlotList[iTradeUnitIndex].m_iX;
			int iTradeUnitY = pConnection->m_aPlotList[iTradeUnitIndex].m_iY;
			if (iTradeUnitX == iX && iTradeUnitY == iY)
			{
				bDisplayInfo = true;
			}
		}

		// if this trade route involves us
		if (bDisplayInfo)
		{
			for (uint uiPlotIndex = 0; uiPlotIndex < pConnection->m_aPlotList.size(); uiPlotIndex++)
			{
				if (pConnection->m_aPlotList[uiPlotIndex].m_iX == iX && pConnection->m_aPlotList[uiPlotIndex].m_iY == iY)
				{
					pOriginCity = GC.getMap().plot(pConnection->m_iOriginX, pConnection->m_iOriginY)->getPlotCity();
					pDestCity = GC.getMap().plot(pConnection->m_iDestX, pConnection->m_iDestY)->getPlotCity();
					if (uiPlotIndex == pConnection->m_iTradeUnitLocationIndex)
					{
						Localization::String strLine;
						if(strcmp(GET_PLAYER(pConnection->m_eOriginOwner).getNickName(), "") != 0)
						{
							strLine = Localization::Lookup("TXT_KEY_MULTIPLAYER_UNIT_TT");
							strLine << GET_PLAYER(pConnection->m_eOriginOwner).getNickName();
							strLine << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationAdjectiveKey();
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain, &GET_PLAYER(pConnection->m_eOriginOwner)))->GetDescription();
#else
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain))->GetDescription();
#endif
						}
						else
						{
							strLine = Localization::Lookup("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV");
							strLine << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationAdjectiveKey();
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain, &GET_PLAYER(pConnection->m_eOriginOwner)))->GetDescription();
#else
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain))->GetDescription();
#endif
						}

						
						Localization::String strColorString;
						TeamTypes eTeam = GET_PLAYER(pConnection->m_eOriginOwner).getTeam();
						if (eTeam == GC.getGame().getActiveTeam())
						{
							strColorString = "[COLOR_WHITE]{1_InnerStr}[ENDCOLOR]";
							strColorString << strLine;
						}
						else if (GET_TEAM(GC.getGame().getActiveTeam()).isAtWar(eTeam))
						{
							strColorString = "[COLOR_NEGATIVE_TEXT]{1_InnerStr}[ENDCOLOR]";
							strColorString << strLine;
						}
						else
						{
							strColorString = "[COLOR_POSITIVE_TEXT]{1_InnerStr}[ENDCOLOR]";
							strColorString << strLine;
						}

						aToolTips.push_back(strColorString.toUTF8());
					}
				}
			}
		}
	}

	return aToolTips;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerTrade& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	if (uiVersion >= 1) 
	{
		int iNum = 0;
		loadFrom >> iNum;
		for (int i = 0; i < iNum; i++)
		{
			TradeConnection kTradeConnection;
			writeTo.m_aRecentlyExpiredConnections.push_back(kTradeConnection);
			loadFrom >> writeTo.m_aRecentlyExpiredConnections[i];
		}
	}
	else
	{
		int iEmptyInt;
		loadFrom >> iEmptyInt;
	}

	if (uiVersion >= 2)
	{
		int iNum = 0;
		loadFrom >> iNum;
		for (int i = 0; i < iNum; i++)
		{
			TradeConnectionWasPlundered kTradeConnectionWasPlundered;
			writeTo.m_aTradeConnectionWasPlundered.push_back(kTradeConnectionWasPlundered);
			loadFrom >> writeTo.m_aTradeConnectionWasPlundered[i].m_iTurnPlundered;
			loadFrom >> writeTo.m_aTradeConnectionWasPlundered[i].m_kTradeConnection;
		}
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerTrade& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_aRecentlyExpiredConnections.size();
	for (uint ui = 0; ui < readFrom.m_aRecentlyExpiredConnections.size(); ui++)
	{
		saveTo << readFrom.m_aRecentlyExpiredConnections[ui];
	}

	saveTo << readFrom.m_aTradeConnectionWasPlundered.size();
	for (uint ui = 0; ui < readFrom.m_aTradeConnectionWasPlundered.size(); ui++)
	{
		saveTo << readFrom.m_aTradeConnectionWasPlundered[ui].m_iTurnPlundered;
		saveTo << readFrom.m_aTradeConnectionWasPlundered[ui].m_kTradeConnection;
	}

	return saveTo;
}

//=====================================
// CvTradeAI
//=====================================
/// Constructor
CvTradeAI::CvTradeAI(void):
m_pPlayer(NULL)
{
}

/// Destructor
CvTradeAI::~CvTradeAI(void)
{
	Uninit();
}

/// Initialize class data
void CvTradeAI::Init(CvPlayer* pPlayer)
{
	Reset();
	m_pPlayer = pPlayer;
	m_iRemovableValue = 0;
}

/// Cleanup
void CvTradeAI::Uninit()
{
	Reset();
}

/// Reset
void CvTradeAI::Reset()
{
	m_pPlayer = NULL;
}

/// DoTurn
void CvTradeAI::DoTurn()
{

}

/// Get all available TR
void CvTradeAI::GetAvailableTR(TradeConnectionList& aTradeConnectionList, bool bSkipExisting)
{
	//important. see which trade paths are still valid
	GC.getGame().GetGameTrade()->UpdateTradePathCache(m_pPlayer->GetID());

	aTradeConnectionList.clear();

	// build trade route list
	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();
	int iOriginCityLoop;
	CvCity* pOriginCity = NULL;
	for (pOriginCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		PlayerTypes eOtherPlayer = NO_PLAYER;
		for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
		{
			eOtherPlayer = (PlayerTypes)ui;

			if (!GET_PLAYER(eOtherPlayer).isAlive())
				continue;

			if (GET_PLAYER(eOtherPlayer).isBarbarian())
				continue;

			if (m_pPlayer->IsAtWarWith(eOtherPlayer))
				continue;

			int iDestCityLoop;
			CvCity* pDestCity = NULL;
			for (pDestCity = GET_PLAYER(eOtherPlayer).firstCity(&iDestCityLoop); pDestCity != NULL; pDestCity = GET_PLAYER(eOtherPlayer).nextCity(&iDestCityLoop))
			{
				// if this is the same city
				if (pOriginCity == pDestCity)
					continue;

				DomainTypes eDomain = NO_DOMAIN;
				for (eDomain = (DomainTypes)0; eDomain < NUM_DOMAIN_TYPES; eDomain = (DomainTypes)(eDomain + 1))
				{
					// if this isn't a valid trade domain, ignore
					if (eDomain != DOMAIN_LAND && eDomain != DOMAIN_SEA)
						continue;

					// Now get the path
					SPath path;
					bool bTradeAvailable = pGameTrade->IsValidTradeRoutePath(pOriginCity, pDestCity, eDomain, &path);
					if (!bTradeAvailable)
						continue;		// If there is no path for this domain, just skip the rest of the connection tests.

					TradeConnectionType eConnection = NUM_TRADE_CONNECTION_TYPES;
					for (uint uiConnectionTypes = 0; uiConnectionTypes < NUM_TRADE_CONNECTION_TYPES; uiConnectionTypes++)
					{
						eConnection = (TradeConnectionType)uiConnectionTypes;

						// Check the trade route ignoring the path
						if (!pPlayerTrade->CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnection, !bSkipExisting, false))
							continue;

						TradeConnection kConnection;
						kConnection.SetCities(pOriginCity,pDestCity);
						kConnection.m_eConnectionType = eConnection;
						kConnection.m_eDomain = eDomain;

						GC.getGame().GetGameTrade()->CopyPathIntoTradeConnection(path, &kConnection);
						aTradeConnectionList.push_back(kConnection);
					}
				}
			}
		}
	}
}

/// Score 
std::vector<int> CvTradeAI::ScoreInternationalTR(const TradeConnection& kTradeConnection, bool bHaveTourism)
{
	std::vector<int> ValuesVector;
	// don't evaluate other trade types
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL)
	{
		return ValuesVector;
	}

	// if this was recently plundered, 0 the score
	if (m_pPlayer->GetTrade()->CheckTradeConnectionWasPlundered(kTradeConnection))
	{
		return ValuesVector;
	}

#ifdef AUI_TRADE_SCORE_INTERNATIONAL_MAX_DELTA_WITH_MINORS
	bool bIsToMinor = false;
	if (GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
	{
		bIsToMinor = true;
	}
#endif

	CvCity* pToCity = CvGameTrade::GetDestCity(kTradeConnection);
	CvCity* pFromCity = CvGameTrade::GetOriginCity(kTradeConnection);

	if (!pToCity || !pFromCity)
		return ValuesVector;

	if (pToCity->isUnderSiege() || pFromCity->isUnderSiege())
		return ValuesVector;

	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();
	CvPlayerTrade* pOtherPlayerTrade = GET_PLAYER(kTradeConnection.m_eDestOwner).GetTrade();
	int iDangerSum = 1; // can't be zero because we divide by it!
	for (uint uiPlotList = 0; uiPlotList < kTradeConnection.m_aPlotList.size(); uiPlotList++)
	{
		CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[uiPlotList].m_iX, kTradeConnection.m_aPlotList[uiPlotList].m_iY);
		CvAssertMsg(pPlot, "pPlot is null when trying to evaluate the list");
		if (pPlot == NULL)
		{
			break;
		}

		//danger is hard to quantify for routes which be definition pass by a lot of invisible plots
		int iDangerValue = 0;

#ifdef AUI_TRADE_SCORE_TRADE_ROUTE_BASE_DANGER
		iDangerValue += AUI_TRADE_SCORE_TRADE_ROUTE_BASE_DANGER;
#endif // AUI_TRADE_SCORE_TRADE_ROUTE_BASE_DANGER

		if (!pPlot->isVisible(m_pPlayer->getTeam()))
		{
			iDangerValue += 1;
		}
#if defined(MOD_BALANCE_CORE)
		if(!pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			iDangerValue += 1;
		}
#endif

		if (pPlot->getTeam() != NO_TEAM && GET_TEAM(m_pPlayer->getTeam()).isAtWar(pPlot->getTeam()))
		{
			iDangerValue += 1000;
		}

#if defined(MOD_BALANCE_CORE)
		//AI cheating here. no visibility check!
		int iRange = 3;
		for(int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for(int iDY = -iRange; iDY <= iRange; iDY++)
			{
				CvPlot* pLoopPlot = ::plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);

				if(pLoopPlot != NULL && pLoopPlot->getNumUnits() > 0)
				{
					CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
					if(pUnit && pUnit->IsCombatUnit() && GET_TEAM(pUnit->getTeam()).isAtWar(m_pPlayer->getTeam()))
					{
						iDangerValue += 100;
					}
				}
			}
		}
#endif

		iDangerSum += iDangerValue;
	}

	// gold
	int iGoldAmount = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, true);

#if defined(MOD_BALANCE_CORE)
	int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	if(iGPT <= 0)
	{
		iGoldAmount *= (iGPT * -2);
	}
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	//if a city is impoverished, let's send trade routes from there (multiply based on amount of unhappiness.
	if(MOD_BALANCE_CORE_HAPPINESS)
	{		
		if(pFromCity->getUnhappinessFromGold() > 0)
		{
			iGoldAmount *= (pFromCity->getUnhappinessFromGold() * 10);
		}
	}
	//If we are somewhat influential, let's count that here.
	if(bHaveTourism && !GET_PLAYER(pFromCity->getOwner()).isMinorCiv())
	{
		//If we get a bonus for tourism, let's count that.
		if(kTradeConnection.m_eDomain == DOMAIN_LAND)
		{
			if(pFromCity->GetLandTourismBonus() > 0)
			{
				iGoldAmount += pFromCity->GetLandTourismBonus();
				//If our influence is pretty good, double the bonus. Not if we're already influential, though.
				if(GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceLevel(kTradeConnection.m_eDestOwner) >= INFLUENCE_LEVEL_FAMILIAR && GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceLevel(kTradeConnection.m_eDestOwner) < INFLUENCE_LEVEL_INFLUENTIAL)
				{
					iGoldAmount += pFromCity->GetLandTourismBonus() * 25;
				}
			}
		}
		//If we get a bonus for tourism, let's count that.
		else if(kTradeConnection.m_eDomain == DOMAIN_SEA)
		{
			if(pFromCity->GetSeaTourismBonus() > 0)
			{
				iGoldAmount += pFromCity->GetSeaTourismBonus();
				//If our influence is pretty good, double the bonus. Not if we're already influential, though.
				if(GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceLevel(kTradeConnection.m_eDestOwner) >= INFLUENCE_LEVEL_FAMILIAR && GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceLevel(kTradeConnection.m_eDestOwner) < INFLUENCE_LEVEL_INFLUENTIAL)
				{
					iGoldAmount += pFromCity->GetSeaTourismBonus() * 25;
				}
			}
		}
	}
#endif
	int iOtherGoldAmount = pOtherPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, false);
#ifdef AUI_TRADE_SCORE_INTERNATIONAL_MAX_DELTA_WITH_MINORS
	if (bIsToMinor)
		iOtherGoldAmount = 0;
#endif // AUI_TRADE_SCORE_INTERNATIONAL_MAX_DELTA_WITH_MINORS
#if defined(MOD_BALANCE_CORE_DEALS_ADVANCED)
	//If we are friends with the player, let's not care about how much gold they make.
	if (m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(kTradeConnection.m_eDestOwner, false) >= MAJOR_CIV_APPROACH_AFRAID)
	{
		iOtherGoldAmount /= 10;
	}
	else if (m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(kTradeConnection.m_eDestOwner) >= MAJOR_CIV_OPINION_NEUTRAL)
	{
		iOtherGoldAmount /= 10;
	}
#endif
	int iGoldDelta = iGoldAmount - iOtherGoldAmount;

	// getting out of a logjam at the beginning of the game on an archepeligo map
	// if the player has made a trade unit but all the trade routes yields will be negative to the player
	// still try to trade with some of the minor civs
	if (iGoldDelta <= 0 && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
	{
		iGoldDelta = 1;
	}

	// tech
	int iAdjustedTechDifferenceP1fromP2 = 0;
	int iTechDifferenceP1fromP2 = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
	if (iTechDifferenceP1fromP2 > 0)
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		int iCeilTechDifference = iTechDifferenceP1fromP2 * 100 / GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);
#else
		int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
		iAdjustedTechDifferenceP1fromP2 = max(iCeilTechDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		if (iAdjustedTechDifferenceP1fromP2 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedTechDifferenceP1fromP2 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
#endif

		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedTechDifferenceP1fromP2 += iPolicyBump;
	}
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	//if a city is illiterate, let's send trade routes from there (add based on amount of unhappiness).
	if(MOD_BALANCE_CORE_HAPPINESS)
	{
		if(pFromCity->getUnhappinessFromScience() > 0)
		{
			iAdjustedTechDifferenceP1fromP2 += pFromCity->getUnhappinessFromScience();
		}
	}
#endif
	int iTechDifferenceP2fromP1 = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eDestOwner,   kTradeConnection.m_eOriginOwner);
	int iAdjustedTechDifferenceP2fromP1 = 0;
	if (iTechDifferenceP2fromP1 > 0)
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		int iCeilTechDifference = iTechDifferenceP2fromP1 * 100 / GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);
#else
		int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
		iAdjustedTechDifferenceP2fromP1 = max(iCeilTechDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		if (iTechDifferenceP2fromP1 > 0 && (GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra() > 0))
		{
			iAdjustedTechDifferenceP2fromP1 *= GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra();
		}
#endif

		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eOriginOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eDestOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedTechDifferenceP2fromP1 += iPolicyBump;
	}

#if defined(MOD_BALANCE_CORE_DEALS_ADVANCED)
	//If we are friends with the player, let's not care about how much science they make.
	if(m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(kTradeConnection.m_eDestOwner, false) >= MAJOR_CIV_APPROACH_NEUTRAL)
	{
		iAdjustedTechDifferenceP2fromP1 /= 3;
	}
	else if(m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(kTradeConnection.m_eDestOwner) >= MAJOR_CIV_OPINION_NEUTRAL)
	{
		iAdjustedTechDifferenceP2fromP1 /= 3;
	}
#endif
	int iTechDelta = iAdjustedTechDifferenceP1fromP2 - iAdjustedTechDifferenceP2fromP1;

	// culture
	int iCultureDifferenceP1fromP2 = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
	
	int iAdjustedCultureDifferenceP1fromP2 = 0;
	if (iCultureDifferenceP1fromP2 > 0)
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		int iCeilCultureDifference = iCultureDifferenceP1fromP2 * 100 / GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
#else
		int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
		iAdjustedCultureDifferenceP1fromP2 = max(iCeilCultureDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		if (iAdjustedCultureDifferenceP1fromP2 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedCultureDifferenceP1fromP2 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
#endif				
		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedCultureDifferenceP1fromP2 += iPolicyBump;
	}

#if defined(MOD_BALANCE_CORE_HAPPINESS)
	//if a city is illiterate, let's send trade routes from there (add based on amount of unhappiness).
	if (MOD_BALANCE_CORE_HAPPINESS)
	{
		if (pFromCity->getUnhappinessFromCulture() > 0)
		{
			iAdjustedCultureDifferenceP1fromP2 += pFromCity->getUnhappinessFromCulture();
		}
	}
#endif
	int iCultureDifferenceP2fromP1 = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eDestOwner, kTradeConnection.m_eOriginOwner);
	int iAdjustedCultureDifferenceP2fromP1 = 0;
	if (iCultureDifferenceP2fromP1 > 0)
	{
#if defined(MOD_TRADE_ROUTE_SCALING)
		int iCeilCultureDifference = iCultureDifferenceP2fromP1 * 100 / GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
#else
		int iCeilTechDifference = (int)ceil(iTechDifference / 2.0f);
#endif
		iAdjustedCultureDifferenceP2fromP1 = max(iCeilCultureDifference, 1);
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
		if (iAdjustedCultureDifferenceP2fromP1 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedCultureDifferenceP2fromP1 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
#endif				
		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedCultureDifferenceP2fromP1 += iPolicyBump;
	}

#if defined(MOD_BALANCE_CORE_DEALS_ADVANCED)
	//If we are friends with the player, let's not care about how much science they make.
	if (m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(kTradeConnection.m_eDestOwner, false) >= MAJOR_CIV_APPROACH_NEUTRAL)
	{
		iAdjustedCultureDifferenceP2fromP1 /= 3;
	}
	else if (m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(kTradeConnection.m_eDestOwner) >= MAJOR_CIV_OPINION_NEUTRAL)
	{
		iAdjustedCultureDifferenceP2fromP1 /= 3;
	}
#endif
	int iCultureDelta = iAdjustedCultureDifferenceP1fromP2 - iAdjustedCultureDifferenceP2fromP1;

	// religion
	ReligionTypes eOwnerFoundedReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if (eOwnerFoundedReligion == NO_RELIGION)
	{
		eOwnerFoundedReligion = m_pPlayer->GetReligions()->GetReligionInMostCities();
	}
	int iReligionDelta = 0;
	if (eOwnerFoundedReligion != NO_RELIGION)
	{
		ReligionTypes eToReligion = NO_RELIGION;
		int iToPressure = 0;
		ReligionTypes eFromReligion = NO_RELIGION;
		int iFromPressure = 0;
		bool bAnyFromCityPressure = pFromCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pToCity, eToReligion, iToPressure);
		bool bAnyToCityPressure = pToCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pFromCity, eFromReligion, iFromPressure);

#ifndef AUI_TRADE_SCORE_INTERNATIONAL_RELATIVE_RELIGION_SCORING
		// Internally pressure is now 10 times greater than what is shown to user
		iToPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
		iFromPressure /= GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER();
#endif // AUI_TRADE_SCORE_INTERNATIONAL_RELATIVE_RELIGION_SCORING

		// if anyone is exerting pressure
		if (bAnyFromCityPressure || bAnyToCityPressure)
		{
			// "to" and "from" religions need to be different for us to care
			if (eToReligion != eFromReligion)
			{
#ifdef AUI_TRADE_SCORE_INTERNATIONAL_RELATIVE_RELIGION_SCORING
				int iExistingToPressureAtFrom = pFromCity->GetCityReligions()->GetPressure(eFromReligion);
				int iExistingGoodPressureAtTo = pToCity->GetCityReligions()->GetPressure(eOwnerFoundedReligion);
				if (eToReligion != eOwnerFoundedReligion)
					iToPressure = 0;
				if (eFromReligion == eOwnerFoundedReligion)
					iFromPressure = 0;
				double dExistingPressureModFrom = sqrt(log((double)MAX(1, iExistingToPressureAtFrom + iFromPressure) / (double)MAX(1, iExistingToPressureAtFrom)) / log(2.));
				double dExistingPressureModTo = sqrt(log((double)MAX(1, iExistingGoodPressureAtTo + iToPressure) / (double)MAX(1, iExistingGoodPressureAtTo)) / log(2.));

				iReligionDelta += int(iToPressure * dExistingPressureModTo / GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER() + 0.5);
				iReligionDelta -= int(iFromPressure * dExistingPressureModFrom / GC.getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER() + 0.5);
#else
				if (eToReligion == eOwnerFoundedReligion)
				{
					iReligionDelta += iToPressure;
				}

				if (eFromReligion != eOwnerFoundedReligion)
				{
					iReligionDelta -= iFromPressure;
				}
#endif // AUI_TRADE_SCORE_INTERNATIONAL_RELATIVE_RELIGION_SCORING
			}
		}
	}

	int iScore = 0;
#if defined(MOD_BALANCE_CORE)
	int iFlavorGold = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
	int iFlavorScience = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
	int iFlavorReligion = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
	int iFlavorCulture = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
	int iGoldScore = iGoldDelta / max(1, (10-iFlavorGold));
	int iScienceScore = (iTechDelta *  iFlavorScience);
	int iCultureScore = (iCultureDelta * iFlavorCulture);
	int iReligionScore = (iReligionDelta *  (iFlavorReligion / 2));
	iScore += iGoldScore + iScienceScore + iCultureScore + iReligionScore;

	if (GET_PLAYER(kTradeConnection.m_eDestOwner).isMajorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
	{
		iScore *= 5;
	}

	//abort if we're negative
	if(iScore <= 0)
		return ValuesVector;
#else
	iScore += iGoldDelta;
	iScore += iTechDelta * 3; // 3 science = 1 gold
	iScore += iReligionDelta * 2; // 2 religion = 1 gold
#endif
#if defined(MOD_BALANCE_CORE)
	for(int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		for (uint uiPlotList = 0; uiPlotList < kTradeConnection.m_aPlotList.size(); uiPlotList++)
		{
			CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[uiPlotList].m_iX, kTradeConnection.m_aPlotList[uiPlotList].m_iY);
			CvAssertMsg(pPlot, "pPlot is null when trying to evaluate the list");
			if (pPlot == NULL)
			{
				continue;
			}
			if(pPlot->getOwner() != m_pPlayer->GetID())
			{
				continue;
			}
			if(pPlot->getTerrainType() == NO_TERRAIN)
			{
				continue;
			}
			iScore += (m_pPlayer->GetPlayerTraits()->GetTerrainYieldChange(pPlot->getTerrainType(), ((YieldTypes)iJ)) * 10);
		}
	}
	int iDistance = (kTradeConnection.m_aPlotList.size() / 3);
	iDistance += iDangerSum;
	int iEra = max(1, (int)m_pPlayer->GetCurrentEra()); // More international trade late game, please.
	iScore = ((iScore * (iEra)) / max(1, (iDistance)));
#else
	iScore = (iScore * 10) / iDangerSum;
#endif
#if defined(MOD_BALANCE_CORE)
	//Let's encourage TRs to feitorias.
	if (MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsSiphoned(m_pPlayer->GetID()))
	{
		iScore *= 10;
	}
#endif
	// if we have any tourism and the destination owner is not a minor civ
#if defined(MOD_BALANCE_CORE)
	if (bHaveTourism && !GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
#else
	if (m_pPlayer->GetCulture()->GetTourism() > 0 && !GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
#endif
	{
		// if we're not connected to a player, double the value to that player
		if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner))
		{
#ifdef AUI_TRADE_SCORE_INTERNATIONAL_TOURISM_SCORE_USES_GRAND_STRATEGY
			if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
			{
				iScore *= 5;
			}
#else
			iScore *= 2;
#endif
		}
	}
#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE)
	{
		//If we aren't connected to a player, and we benefit from this, ramp up the score!
		for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
		{
			if(m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner((YieldTypes)iYieldLoop) > 0)
			{
				if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner))
				{
					iScore *= 10;
				}
			}
		}
		if (m_pPlayer->GetPlayerTraits()->IsNoOpenTrade() && GET_PLAYER(kTradeConnection.m_eDestOwner).isMajorCiv())
		{
			if (GET_PLAYER(kTradeConnection.m_eDestOwner).GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				iScore /= 10;
			}
			else
			{
				if (m_pPlayer->GetTrade()->GetNumDifferentMajorCivTradingPartners() <= 0)
				{
					iScore *= 10;
				}
				else
				{
					if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner, true))
					{
						iScore *= 10;
					}
					else
					{
						iScore /= 10;
					}
				}
			}
		}
		if(GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
		{
			int iCityLoop;
			CvCity* pLoopCity;
			for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					if(pLoopCity->GetCityBuildings()->GetCityStateTradeRouteProductionModifier() > 0)
					{
						if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner))
						{
							iScore *= 10;
						}
					}
				}
			}
			if(m_pPlayer->GetEventTourismCS() > 0)
			{
				iScore *= m_pPlayer->GetEventTourismCS();
			}
		}
		if(m_pPlayer->GetCorporations()->HasFoundedCorporation())
		{
			CorporationTypes eCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();

			CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
			if (pkCorporationInfo != NULL)
			{

				int iX = kTradeConnection.m_iDestX;
				int iY = kTradeConnection.m_iDestY;

				int iOX = kTradeConnection.m_iOriginX;
				int iOY = kTradeConnection.m_iOriginY;
				if (iX != -1 && iY != -1 && iOX != -1 && iOY != -1)
				{
					CvPlot* pPlot = GC.getMap().plot(iX, iY);
					CvPlot* pPlot2 = GC.getMap().plot(iOX, iOY);
					if (pPlot != NULL && pPlot2 != NULL)
					{
						CvCity* pDestCity = pPlot->getPlotCity();
						CvCity* pOriginCity = pPlot2->getPlotCity();
						if (pDestCity != NULL && pOriginCity != NULL)
						{
							if (pOriginCity->IsHasOffice())
							{
								int iFranchises = m_pPlayer->GetCorporations()->GetNumFranchises();
								int iMax = m_pPlayer->GetCorporations()->GetMaxNumFranchises();
								//Not franchised? Let's see what we get if we franchise it.
								if ((iFranchises < iMax) && !m_pPlayer->GetCorporations()->IsCorporationOfficesAsFranchises() && !pDestCity->IsHasFranchise(eCorporation))
								{
									int iGPYieldFromCorp = pOriginCity->GetGPRateModifierPerXFranchises();
									if (iGPYieldFromCorp > 0)
									{
										iScore *= (iGPYieldFromCorp * 5);
									}
									BuildingClassTypes eOffice = pkCorporationInfo->GetOfficeBuildingClass();
									if(eOffice != NO_BUILDINGCLASS)
									{
										CvBuildingClassInfo* pInfo = GC.getBuildingClassInfo(eOffice);
										if (pInfo)
										{
											BuildingTypes eOfficeBuilding = (BuildingTypes)GET_PLAYER(pOriginCity->getOwner()).getCivilizationInfo().getCivilizationBuildings(eOffice);
											if(eOfficeBuilding != NO_BUILDING)
											{
												CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eOfficeBuilding);
												for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
												{
													if(pkBuildingInfo->GetYieldPerFranchise(iYield) > 0)
													{
														iScore *= (pkBuildingInfo->GetYieldPerFranchise(iYield) * 5);
													}
												}
											}
										}
									}
									if (m_pPlayer->GetCulture()->GetInfluenceLevel(pDestCity->getOwner()) >= INFLUENCE_LEVEL_POPULAR && m_pPlayer->GetCorporations()->IsCorporationFreeFranchiseAbovePopular())
									{
										iScore *= 10;
									}
								}
								//Franchised? Do we get a bonus for sending TRs here again?
								else
								{
									for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
									{
										int iYieldFromCorp = pOriginCity->GetTradeRouteCityMod((YieldTypes)iYield);
										if (iYieldFromCorp > 0)
										{
											iScore *= (iYieldFromCorp * 5);
										}
									}

									if (pOriginCity->IsHasOffice())
									{
										for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
										{
											if (pkCorporationInfo->GetTradeRouteMod(iYield) > 0)
											{
												iScore *= (pkCorporationInfo->GetTradeRouteMod(iYield) * 5);
											}
										}
									}
								}
							}
							//Foreign city have an office? Let's reduce the score just a little to keep from feeding their corporation.
							else if (pDestCity->IsHasOffice() && GET_PLAYER(pDestCity->getOwner()).isMajorCiv())
							{
								int iFranchises = (GET_PLAYER(pDestCity->getOwner()).GetCorporations()->GetNumFranchises() / 2);
								//Care less if we like this guy.
								if (m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(kTradeConnection.m_eDestOwner, false) == MAJOR_CIV_APPROACH_FRIENDLY ||
									m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(kTradeConnection.m_eDestOwner) >= MAJOR_CIV_OPINION_FAVORABLE)
								{
									iFranchises /= 2;
								}

								iScore /= max(1, iFranchises);
							}
						}
					}
				}
			}
		}		
	}
	if(m_pPlayer->IsAtWar())
	{
		iScore /= max(2, m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false));
	}
	ValuesVector.push_back(iScore);
	ValuesVector.push_back(iCultureScore);
	ValuesVector.push_back(iGoldScore);
	ValuesVector.push_back(iScienceScore);
	ValuesVector.push_back(iReligionScore);

#endif
	return ValuesVector;
}

/// Score Food TR
int CvTradeAI::ScoreInternalTR(const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList)
{
	// if we're not going to a target from our list, ignore
	bool bValidTarget = false;
	for (uint ui = 0; ui < aTargetCityList.size(); ui++)
	{
		if (kTradeConnection.m_iDestID == aTargetCityList[ui]->GetID())
		{
			bValidTarget = true;
			break;
		}
	}

	if (!bValidTarget)
	{
		return 0;
	}

	// if this was recently plundered, zero the score, don't try again 
	if (m_pPlayer->GetTrade()->CheckTradeConnectionWasPlundered(kTradeConnection))
	{
		return 0;
	}

	CvCity* pOriginCity = GET_PLAYER(kTradeConnection.m_eOriginOwner).getCity(kTradeConnection.m_iOriginID);
	CvCity* pDestCity = GET_PLAYER(kTradeConnection.m_eDestOwner).getCity(kTradeConnection.m_iDestID);
	if(pDestCity == NULL || pOriginCity == NULL)
	{
		return 0;
	}

	if (pDestCity->isUnderSiege() || pOriginCity->isUnderSiege())
		return 0;

#if defined(MOD_BALANCE_CORE)
	int iDistance = (kTradeConnection.m_aPlotList.size() / 5);
#else
	int iDistance = kTradeConnection.m_aPlotList.size();
#endif

	int iScore = 0;
	switch (kTradeConnection.m_eConnectionType)
	{
		case TRADE_CONNECTION_FOOD:
		{
			iScore = ((pOriginCity->foodDifference() + pOriginCity->getPopulation() * 3) - (pDestCity->foodDifference() + pDestCity->getPopulation()));
			if(pDestCity->GetCityCitizens()->GetFocusType() == CITY_AI_FOCUS_TYPE_FOOD)
				iScore *= 20;
			break;
		}
		case TRADE_CONNECTION_PRODUCTION:
		case TRADE_CONNECTION_WONDER_RESOURCE:
		{
			iScore = (pOriginCity->getBaseYieldRate(YIELD_PRODUCTION) * 3 - pDestCity->getBaseYieldRate(YIELD_PRODUCTION));
			if(pDestCity->GetCityCitizens()->GetFocusType() == CITY_AI_FOCUS_TYPE_PRODUCTION)
				iScore *= 20;
			BuildingTypes eBuilding = pDestCity->getProductionBuilding();
			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pBuildingInfo)
				{
					CvBuildingClassInfo* pInfo = GC.getBuildingClassInfo((BuildingClassTypes)pBuildingInfo->GetBuildingClassType());
					if (pInfo && pInfo->getMaxGlobalInstances() == 1)
					{
						iScore *= 20;
					}
				}
			}
			break;
		}
	}

	//direct connection to capital via TR
	if(pDestCity->getUnhappinessFromConnection() > 0 && pOriginCity->isCapital())
	{
		iScore *= 3;
	}
	if(pOriginCity->getUnhappinessFromConnection() > 0 && pDestCity->isCapital())
	{
		iScore *= 3;
	}
	//indirect connection to capital also counts
	if(pOriginCity->getUnhappinessFromConnection() > 0 && pDestCity->getUnhappinessFromConnection() <= 0)
	{
		iScore *= 2;
	}

#if defined(MOD_BALANCE_CORE)
	int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	if(iGPT <= 0)
	{
		iScore += (iGPT * 5);
	}
#endif

	for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
	{
		if(m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner((YieldTypes)iYieldLoop) > 0)
		{
			iScore -= (m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner((YieldTypes)iYieldLoop));
		}
		if(m_pPlayer->GetGoldInternalTrade() > 0)
		{
			iScore += m_pPlayer->GetGoldInternalTrade();
		}
	}

	int iDangerSum = 1; // can't be zero because we divide by it!
	for (uint uiPlotList = 0; uiPlotList < kTradeConnection.m_aPlotList.size(); uiPlotList++)
	{
		CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[uiPlotList].m_iX, kTradeConnection.m_aPlotList[uiPlotList].m_iY);
		CvAssertMsg(pPlot, "pPlot is null when trying to evaluate the list");
		if (pPlot == NULL)
		{
			break;
		}

		if (m_pPlayer->GetPlotDanger(*pPlot)>0)
			iDangerSum++;
	}

	for(int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		for (uint uiPlotList = 0; uiPlotList < kTradeConnection.m_aPlotList.size(); uiPlotList++)
		{
			CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[uiPlotList].m_iX, kTradeConnection.m_aPlotList[uiPlotList].m_iY);
			CvAssertMsg(pPlot, "pPlot is null when trying to evaluate the list");
			if (pPlot == NULL)
			{
				continue;
			}
			if(pPlot->getOwner() != m_pPlayer->GetID())
			{
				continue;
			}
			if(pPlot->getTerrainType() == NO_TERRAIN)
			{
				continue;
			}
			iScore += (m_pPlayer->GetPlayerTraits()->GetTerrainYieldChange(pPlot->getTerrainType(), ((YieldTypes)iJ)) * 10);
		}
	}

	//internal traderoutes should be preferred when at war
	if(m_pPlayer->IsAtWar())
	{
		iScore *= MAX(2, m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(true));
	}
#if defined(MOD_BALANCE_CORE)
	// turn it up some for Conquest of the World player during golden ages
	if(m_pPlayer->GetPlayerTraits()->IsConquestOfTheWorld() && m_pPlayer->isGoldenAge())
	{
		iScore *= 5;
	}
#endif

	return (iScore * 2) / (iDistance + iDangerSum);
}

int CvTradeAI::ScoreFoodTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList)
{
	// only consider food trades
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_FOOD)
	{
		return 0;
	}

	return ScoreInternalTR(kTradeConnection,aTargetCityList);
}

/// Score Production TR
int CvTradeAI::ScoreProductionTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList)
{
	// only consider production trades
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_PRODUCTION)
	{
		return 0;
	}

	return ScoreInternalTR(kTradeConnection,aTargetCityList);
}

#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
/// Score Wonder TR
int CvTradeAI::ScoreWonderTR (const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList)
{
	// only consider wonder trades
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_WONDER_RESOURCE)
	{
		return 0;
	}

	return ScoreInternalTR(kTradeConnection,aTargetCityList);
}
#endif

// sort player numbers
struct TRSortElement
{
	TradeConnection m_kTradeConnection;
	int m_iScore;
	int m_iGoldScore;
	int m_iCultureScore;
	int m_iReligionScore;
	int m_iScienceScore;
};

struct SortTR
{
	bool operator()(TRSortElement const& a, TRSortElement const& b) const
	{
		return a.m_iScore < b.m_iScore;
	}
};
#if defined(MOD_BALANCE_CORE)
struct SortTRHigh
{
	bool operator()(TRSortElement const& a, TRSortElement const& b) const
	{
		return a.m_iScore > b.m_iScore;
	}
};
#endif

/// Prioritize TRs
void CvTradeAI::GetPrioritizedTradeRoutes(TradeConnectionList& aTradeConnectionList, bool bSkipExisting)
{	
	GetAvailableTR(aTradeConnectionList, bSkipExisting);

	// if the list is empty, bail
	if (aTradeConnectionList.size() == 0 || m_pPlayer->getNumCities()==0)
	{
		return;
	}

	// score TR
	std::vector<TRSortElement> aProductionSortedTR;
	std::vector<TRSortElement> aFoodSortedTR;
	std::vector<TRSortElement> aGoldSortedTR;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	std::vector<TRSortElement> aWonderSortedTR;
#endif

	// FOOD FOOD FOOD FOOD
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	std::vector<CvCity*> apFoodTargetCities;
	if (m_pPlayer->GetHappiness() >= 0 || m_pPlayer->getUnhappinessFromCityStarving() >= 0)
#else
	if (m_pPlayer->GetHappiness() >= 0)
#endif
	{
		// - Find avg city size
		int iAvgFood = 0;
		int iCityLoop;
		for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
		{
			iAvgFood += pCity->getBaseYieldRate(YIELD_FOOD);
		}
		iAvgFood /= m_pPlayer->getNumCities();

		for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if (pCity->getBaseYieldRate(YIELD_FOOD) < iAvgFood)
			{
				apFoodTargetCities.push_back(pCity);
			}
		}

		if (apFoodTargetCities.size() > 0)
		{
			aFoodSortedTR.clear();
			for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
			{
				TRSortElement kElement;
				kElement.m_kTradeConnection = aTradeConnectionList[ui];
				kElement.m_iScore = ScoreFoodTR(aTradeConnectionList[ui], apFoodTargetCities);
				if (kElement.m_iScore > 0)
				{
					aFoodSortedTR.push_back(kElement);
				}
			}
			std::stable_sort(aFoodSortedTR.begin(), aFoodSortedTR.end(), SortTR());
		}
	}

	// PRODUCTION PRODUCTION PRODUCTION PRODUCTION
	// - Search for wonder city
	// - Search for spaceship city
	std::vector<CvCity*> apProductionTargetCities;
	CvCity* pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
	if (pWonderCity)
	{
		apProductionTargetCities.push_back(pWonderCity);
	}

	int iCityLoop;
	for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
	{
		UnitTypes eUnit = pCity->getProductionUnit();
		if (eUnit != -1)
		{
			CvUnitEntry *pkUnit = GC.GetGameUnits()->GetEntry(eUnit);
			if (pkUnit)
			{
				if (pkUnit->GetSpaceshipProject() != NO_PROJECT)
				{
					apProductionTargetCities.push_back(pCity);
				}
			}
		}
	}

	//if no wonders and no spaceship, simply target below average cities
	if (apProductionTargetCities.empty())
	{
		int iAvgProd = 0;
		for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
		{
			iAvgProd += pCity->getBaseYieldRate(YIELD_PRODUCTION);
		}
		iAvgProd /= m_pPlayer->getNumCities();

		for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if (pCity->getBaseYieldRate(YIELD_PRODUCTION) < iAvgProd)
				apProductionTargetCities.push_back(pCity);
		}
	}

	if (apProductionTargetCities.size() > 0)
	{
		int iBestScore = -1;
		aProductionSortedTR.clear();
		for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
		{
			TRSortElement kElement;
			kElement.m_kTradeConnection = aTradeConnectionList[ui];
			kElement.m_iScore = ScoreProductionTR(aTradeConnectionList[ui], apProductionTargetCities);
			if (kElement.m_iScore > iBestScore)
			{
				iBestScore = kElement.m_iScore;
				aProductionSortedTR.push_back(kElement);
			}
		}
		std::stable_sort(aProductionSortedTR.begin(), aProductionSortedTR.end(), SortTR());
	}

#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	if (MOD_TRADE_WONDER_RESOURCE_ROUTES) {
		// WONDER WONDER WONDER WONDER
		ResourceTypes eWonderResource = ::getWonderResource();

		if (eWonderResource != NO_RESOURCE) {
			std::vector<CvCity*> apWonderTargetCities;
			CvCity* pCity = NULL;
			int iCityLoop;
			for (pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop)) {
				// Only interested in cities that don't have the wonder resource locally and are actually building a wonder!
				if (!pCity->IsHasResourceLocal(eWonderResource, true))
				{
					BuildingTypes eBuilding = pCity->getProductionBuilding();
					if (eBuilding != -1)
					{
						CvBuildingEntry *pkBuilding = GC.GetGameBuildings()->GetEntry(eBuilding);
						if (pkBuilding)
						{
							if (GET_PLAYER(pCity->getOwner()).GetWonderProductionAI()->IsWonder(*pkBuilding))
							{
								bool bAddCity = true;
								int iDestX = pCity->getX();
								int iDestY = pCity->getY();

								// Only add if no existing wonder resource trade route to here
								CvGameTrade* pTrade = GC.getGame().GetGameTrade();
								for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
								{
									if (pTrade->IsTradeRouteIndexEmpty(ui))
									{
										continue;
									}

									const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(ui));
									if (pTradeConnection->m_eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE && pTradeConnection->m_iDestX == iDestX && pTradeConnection->m_iDestY == iDestY)
									{
										bAddCity = false;
										break;
									}
								}
							
								if (bAddCity) {
									apWonderTargetCities.push_back(pCity);
									// CUSTOMLOG("%s is a potential destination for a Wonder Resource trade route", pCity->getName().c_str());
								}
							}
						}
					}
				}
			}
			if (apWonderTargetCities.size() > 0)
			{
				int iBestScore = -1;
				aWonderSortedTR.clear();
				for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
				{
					TRSortElement kElement;
					kElement.m_kTradeConnection = aTradeConnectionList[ui];
					kElement.m_iScore = ScoreWonderTR(aTradeConnectionList[ui], apWonderTargetCities);
					if (kElement.m_iScore > iBestScore)
					{
						iBestScore = kElement.m_iScore;
						aWonderSortedTR.push_back(kElement);
					}
				}
				std::stable_sort(aWonderSortedTR.begin(), aWonderSortedTR.end(), SortTR());
			}
		}
	}
#endif

#if defined(MOD_BALANCE_CORE)
	//precalculate this, it's expensive
	bool bHaveTourism = (m_pPlayer->GetCulture()->GetTourism() > 0);
#endif

	// GOLD GOLD GOLD GOLD
	aGoldSortedTR.clear();
	int iBestScore = -1;
	for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
	{
		TRSortElement kElement;
		kElement.m_kTradeConnection = aTradeConnectionList[ui];
		std::vector<int>ValuesVector;
		ValuesVector = ScoreInternationalTR(aTradeConnectionList[ui], bHaveTourism);
		if (ValuesVector.size() > 0)
		{
			kElement.m_iScore = ValuesVector[0];
			kElement.m_iCultureScore = ValuesVector[1];
			kElement.m_iGoldScore = ValuesVector[2];
			kElement.m_iScienceScore = ValuesVector[3];
			kElement.m_iReligionScore = ValuesVector[4];
		}

		if (kElement.m_iScore > iBestScore)
		{
			iBestScore = kElement.m_iScore;
			aGoldSortedTR.push_back(kElement);
		}
	}
	std::stable_sort(aGoldSortedTR.begin(), aGoldSortedTR.end(), SortTR());

	// clear list
	aTradeConnectionList.clear();

#ifdef AUI_TRADE_UNBIASED_PRIORITIZE
	std::vector<TRSortElement> aTotalList;
	aTotalList.clear();
	for (uint ui = 0; ui < aGoldSortedTR.size(); ui++)
	{
		if (aGoldSortedTR[ui].m_iScore > 0)
		{
			aTotalList.push_back(aGoldSortedTR[ui]);
		}
	}
	for (uint ui = 0; ui < aFoodSortedTR.size(); ui++)
	{
		if (aFoodSortedTR[ui].m_iScore > 0)
		{
			aTotalList.push_back(aFoodSortedTR[ui]);
		}
	}

#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	if (MOD_TRADE_WONDER_RESOURCE_ROUTES) {
		for (uint ui = 0; ui < aWonderSortedTR.size(); ui++)
		{
			if (aWonderSortedTR[ui].m_iScore > 0)
			{
				aTotalList.push_back(aWonderSortedTR[ui]);
			}
		}
	}
#endif

	for (uint ui = 0; ui < aProductionSortedTR.size(); ui++)
	{
		if (aProductionSortedTR[ui].m_iScore > 0)
		{
			aTotalList.push_back(aProductionSortedTR[ui]);
		}
	}

	std::stable_sort(aTotalList.begin(), aTotalList.end(), SortTRHigh());

	for (uint ui = 0; ui < aTotalList.size(); ui++)
	{
		aTradeConnectionList.push_back(aTotalList[ui].m_kTradeConnection);
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		for (uint ui = 0; ui < aTotalList.size(); ui++)
		{
			DomainTypes eDomain = aTotalList[ui].m_kTradeConnection.m_eDomain;
			TradeConnectionType eConnectionType = aTotalList[ui].m_kTradeConnection.m_eConnectionType;
			CvCity* pOriginCity = GC.getMap().plot(aTotalList[ui].m_kTradeConnection.m_iOriginX, aTotalList[ui].m_kTradeConnection.m_iOriginY)->getPlotCity();
			CvCity* pDestCity = GC.getMap().plot(aTotalList[ui].m_kTradeConnection.m_iDestX, aTotalList[ui].m_kTradeConnection.m_iDestY)->getPlotCity();

			if (pOriginCity == NULL || pDestCity == NULL)
				continue;

			if (GC.getLogging())
			{
				CvString strMsg;
				CvString strDomain;
				switch (eDomain)
				{
				case DOMAIN_LAND:
					strDomain = "land";
					break;
				case DOMAIN_SEA:
					strDomain = "sea";
					break;
				}

				CvString strTRType;
				switch (eConnectionType)
				{
				case TRADE_CONNECTION_FOOD:
					strTRType = "food";
					break;
				case TRADE_CONNECTION_PRODUCTION:
					strTRType = "production";
					break;
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
				case TRADE_CONNECTION_WONDER_RESOURCE:
					strTRType = "wonder resource";
					break;
#endif
				case TRADE_CONNECTION_INTERNATIONAL:
					strTRType = "international";
					break;
				}

				if (eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
				{
					strMsg.Format("Trade Route Options, %s, %s, %s, %i, %s, Total Value: %i, ScienceValue: %i, GoldValue: %i, CultureValue: %i, ReligionValue: %i",
						strDomain.c_str(),
						pOriginCity->getName().c_str(),
						pDestCity->getName().c_str(),
						aTotalList[ui].m_kTradeConnection.m_aPlotList.size(),
						strTRType.c_str(),
						aTotalList[ui].m_iScore,
						aTotalList[ui].m_iScienceScore,
						aTotalList[ui].m_iGoldScore,
						aTotalList[ui].m_iCultureScore,
						aTotalList[ui].m_iReligionScore);
				}
				else
				{
					strMsg.Format("Trade Route Options, %s, %s, %s, %i, %s, Total Value: %i",
						strDomain.c_str(),
						pOriginCity->getName().c_str(),
						pDestCity->getName().c_str(),
						aTotalList[ui].m_kTradeConnection.m_aPlotList.size(),
						strTRType.c_str(),
						aTotalList[ui].m_iScore);
				}
				
				CvString playerName;
				FILogFile* pLog;
				CvString strBaseString;
				CvString strOutBuf;
				CvString strFileName = "TradeRouteChoices.csv";
				playerName = GET_PLAYER(pOriginCity->getOwner()).getCivilizationShortDescription();
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
				strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
				strBaseString += playerName + ", ";
				strBaseString += strMsg;
				pLog->Msg(strBaseString);
			}
		}
	}
	//Reset values.
	int iOriginCityLoop;
	CvCity* pOriginLoopCity = NULL;
	for(pOriginLoopCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginLoopCity != NULL; pOriginLoopCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		if(pOriginLoopCity != NULL)
		{
			pOriginLoopCity->SetTradePrioritySea(-1);
			pOriginLoopCity->SetTradePriorityLand(-1);
		}
	}
	//Let's store off values for 'good' trade unit cities to grab elsewhere. Uses the priority model (so 0 = best, 1 = second-best, etc.)
	for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
	{
		int iX = aTradeConnectionList[ui].m_iOriginX;
		int iY = aTradeConnectionList[ui].m_iOriginY;

		CvPlot* pPlot = GC.getMap().plot(iX, iY);
		if(pPlot != NULL)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if(pCity != NULL && pCity->getOwner() == m_pPlayer->GetID())
			{
				if(aTradeConnectionList[ui].m_eDomain == DOMAIN_SEA && pCity->GetTradePrioritySea() == -1)
				{
					pCity->SetTradePrioritySea(ui);
				}
				else if(pCity->GetTradePriorityLand() == -1)
				{
					pCity->SetTradePriorityLand(ui);
				}
			}
		}
	}
#else
	// add first food
	if (aFoodSortedTR.size() > 0) 
	{
		aTradeConnectionList.push_back(aFoodSortedTR[0].m_kTradeConnection);
	}
	
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	if (MOD_TRADE_WONDER_RESOURCE_ROUTES) {
		// add first wonder resource tr
		if (aWonderSortedTR.size() > 0)
		{
			aTradeConnectionList.push_back(aWonderSortedTR[0].m_kTradeConnection);
		}
	}
#endif

	// add first production tr
	if (aProductionSortedTR.size() > 0)
	{
		aTradeConnectionList.push_back(aProductionSortedTR[0].m_kTradeConnection);
	}

	// add all the gold trs
	uint uiNumInternationalTradesToAdd = 2;
	for (uint ui = uiNumInternationalTradesToAdd; ui < aGoldSortedTR.size(); ui++)
	{
		aTradeConnectionList.push_back(aGoldSortedTR[ui].m_kTradeConnection);
	}
#endif // AUI_TRADE_UNBIASED_PRIORITIZE

}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvTradeAI& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iRemovableValue;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvTradeAI& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_iRemovableValue;

	return saveTo;
}

int TradeConnection::GetMovementSpeed()
{
	if (m_eOriginOwner == NO_PLAYER)
		return 0;

	int iRawSpeed = GET_PLAYER(m_eOriginOwner).GetTrade()->GetTradeRouteSpeed(m_eDomain);

	//unfortunately have to round the result to integer, so often the speed factor won't matter
	return int(0.5f + m_iSpeedFactor*iRawSpeed / 100.f);
}
