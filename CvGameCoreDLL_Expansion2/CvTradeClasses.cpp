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
#include "CvCitySpecializationAI.h"

#include "CvBarbarians.h"

#include "CvNotifications.h"
#include "cvStopWatch.h"
#include "CvCityManager.h"
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
#include "CvInternalGameCoreUtils.h"
#include "CvWonderProductionAI.h"
#endif

#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"

#include "CvGrandStrategyAI.h"

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
	m_lastTradePathUpdate = vector<int>(MAX_PLAYERS, -1);
	m_lastTradePathUpdateUi = vector<int>(MAX_PLAYERS, -1);
	m_routesPerPlayer = vector<vector<int>>(MAX_PLAYERS);
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

int CvGameTrade::GetLongestPotentialTradeRoute(int iCityIndex, DomainTypes eDomain)
{
	std::map<int, std::map<DomainTypes, int>>::const_iterator cityIt = m_aiLongestPotentialTradeRoute.find(iCityIndex);
	if (cityIt == m_aiLongestPotentialTradeRoute.end())
		return -1;

	std::map<DomainTypes, int>::const_iterator domainIt = (cityIt->second).find(eDomain);
	if (domainIt == (cityIt->second).end())
		return -1;

	return domainIt->second;
}

void CvGameTrade::SetLongestPotentialTradeRoute(int iValue, int iCityIndex, DomainTypes eDomain)
{
	m_aiLongestPotentialTradeRoute[iCityIndex].insert(std::make_pair(eDomain, iValue));
}

TradePathLookup& CvGameTrade::GetTradePathsCache(bool bWater)
{
	if (gDLL->IsGameCoreThread())
	{
		if (bWater)
			return m_aPotentialTradePathsWater;

		return m_aPotentialTradePathsLand;
	}

	if (bWater)
		return m_aPotentialTradePathsWaterUi;

	return m_aPotentialTradePathsLandUi;
}

int CvGameTrade::GetTradePathsCacheUpdateCounter(PlayerTypes ePlayer)
{
	//we have two sets of caches ...
	return gDLL->IsGameCoreThread() ? m_lastTradePathUpdate[ePlayer] : m_lastTradePathUpdateUi[ePlayer];
}

void CvGameTrade::SetTradePathsCacheUpdateCounter(PlayerTypes ePlayer, int iValue)
{
	//we have two sets of caches ...
	if (gDLL->IsGameCoreThread())
		m_lastTradePathUpdate[ePlayer] = iValue;
	else
		m_lastTradePathUpdateUi[ePlayer] = iValue;
}

// helper function
bool HaveTradePathInCache(const TradePathLookup& cache, int iCityPlotA, int iCityPlotB)
{
	TradePathLookup::const_iterator itA = cache.find(iCityPlotA);
	if (itA!=cache.end())
	{
		TradePathLookup::value_type::second_type::const_iterator itB = itA->second.find(iCityPlotB);
		return (itB!=itA->second.end());
	}
	
	return false;
}

// helper function
bool AddTradePathToCache(TradePathLookup& cache, int iCityPlotA, int iCityPlotB, const SPath& path)
{
	TradePathLookup::const_iterator itA = cache.find(iCityPlotA);
	if (itA!=cache.end())
	{
		TradePathLookup::value_type::second_type::const_iterator itB = itA->second.find(iCityPlotB);
		if (itB!=itA->second.end())
			cache[iCityPlotA][iCityPlotB] = path;
		else
			cache[iCityPlotA].insert( std::make_pair(iCityPlotB,path) );
	}
	else
	{
		TradePathLookup::value_type::second_type newDestinations;
		newDestinations.insert( std::make_pair(iCityPlotB,path) );
		cache.insert( std::make_pair(iCityPlotA,newDestinations) );
	}
	
	return false;
}

const std::map<int, SPath>& CvGameTrade::GetAllPotentialTradeRoutesFromCity(CvCity* pOriginCity, bool bWater)
{
	if (!pOriginCity)
		return m_dummyTradePaths; //always empty

	//make sure we're up to date
	UpdateTradePathCache(pOriginCity->getOwner());

	const TradePathLookup& cache = GetTradePathsCache(bWater);
	TradePathLookup::const_iterator it = cache.find(pOriginCity->plot()->GetPlotIndex());
	if (it != cache.end())
		return it->second;
	else
		return m_dummyTradePaths; //always empty
}

bool CvGameTrade::HavePotentialTradePath(bool bWater, CvCity* pOriginCity, CvCity* pDestCity, SPath* pPathOut)
{
	if (!pOriginCity || !pDestCity)
		return false;

	//important. see which trade paths are valid
	PlayerTypes eOriginPlayer = pOriginCity->getOwner();
	UpdateTradePathCache(eOriginPlayer);

	//can't use const here, otherwise the [] operator does not work ...
	TradePathLookup& cache = GetTradePathsCache(bWater);

	int iCityPlotA = pOriginCity->plot()->GetPlotIndex();
	int iCityPlotB = pDestCity->plot()->GetPlotIndex();
	if (HaveTradePathInCache(cache, iCityPlotA, iCityPlotB))
	{
		if (pPathOut)
			*pPathOut = cache[iCityPlotA][iCityPlotB];
		return true;
	}

	return false;
}

void CvGameTrade::InvalidateTradePathCache()
{
	for (uint uiPlayer1 = 0; uiPlayer1 < MAX_PLAYERS; uiPlayer1++)
	{
		PlayerTypes ePlayer1 = (PlayerTypes)uiPlayer1;
		InvalidateTradePathCache(ePlayer1);
	}
	m_aPotentialTradePathsWater.clear();
	m_aPotentialTradePathsWaterUi.clear();
	m_aPotentialTradePathsLand.clear();
	m_aPotentialTradePathsLandUi.clear();
}

void CvGameTrade::InvalidateTradePathTeamCache(TeamTypes eTeam)
{
	vector<PlayerTypes> vFromTeam = GET_TEAM(eTeam).getPlayers();
	for (size_t j = 0; j < vFromTeam.size(); j++)
	{
		GC.getGame().GetGameTrade()->InvalidateTradePathCache(vFromTeam[j]);
	}
}

void CvGameTrade::InvalidateTradePathCache(PlayerTypes ePlayer)
{
	m_lastTradePathUpdate[ePlayer] = -1;
	m_lastTradePathUpdateUi[ePlayer] = -1;
}

void CvGameTrade::UpdateTradePathCache(PlayerTypes ePlayer1)
{
	CvPlayer& kPlayer1 = GET_PLAYER(ePlayer1);
	if (!kPlayer1.isAlive() || kPlayer1.isBarbarian() || kPlayer1.isMinorCiv())
		return;

	//check if we have anything to do
	int lastUpdate = GetTradePathsCacheUpdateCounter(ePlayer1);
	if (lastUpdate==GC.getGame().getGameTurn())
		return;

	//do not check whether we are at war here! the trade route cache is also used for military target selection
	//OutputDebugString(CvString::format("updating trade path cache for player %d, turn %d\n", iPlayer1, GC.getGame().getGameTurn()).c_str());

	vector<CvPlot*> vDestPlots;
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		int iCity = 0;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
			if (pDestCity->plot()->isCity()) //idiot proofing
				vDestPlots.push_back(pDestCity->plot());
	}

	int iOriginCityLoop = 0;
	for (CvCity* pOriginCity = kPlayer1.firstCity(&iOriginCityLoop); pOriginCity != NULL; pOriginCity = kPlayer1.nextCity(&iOriginCityLoop))
	{
		CvPlot* pCityPlot = pOriginCity->plot();
		int iCityPlotIndex = pCityPlot->GetPlotIndex();
		TradePathLookup& m_aLandPaths = GetTradePathsCache(false);
		TradePathLookup& m_aWaterPaths = GetTradePathsCache(true);
		//throw away the old data before adding the new
		m_aLandPaths[iCityPlotIndex].clear();
		m_aWaterPaths[iCityPlotIndex].clear();

		//first see how far we can go from this city on water
		int iMaxNormDistSea = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_SEA, pOriginCity);
		SPathFinderUserData data(ePlayer1,PT_TRADE_WATER);
		data.iMaxNormalizedDistance = iMaxNormDistSea;

		//get all paths
		//paths from origin city to dest city
		//map<index of dest's city plot, path to this city>
		map<int,SPath> waterpaths = GC.GetStepFinder().GetMultiplePaths(pCityPlot, vDestPlots, data);
		for (map<int,SPath>::iterator it=waterpaths.begin(); it!=waterpaths.end(); ++it)
		{
			CvPlot* plot = GC.getMap().plotByIndex(it->first);
			// if this is the origin city, nothing to do
			if (pCityPlot == plot)
				continue;

			CvCity* pDestCity = plot->getPlotCity();
			AddTradePathToCache(m_aWaterPaths,iCityPlotIndex,pDestCity->plot()->GetPlotIndex(),it->second);
		}

		//now for land routes
		int iMaxNormDistLand = kPlayer1.GetTrade()->GetTradeRouteRange(DOMAIN_LAND, pOriginCity);
		data.iMaxNormalizedDistance = iMaxNormDistLand;
		data.ePath = PT_TRADE_LAND;

		//get all paths
		map<int,SPath> landpaths = GC.GetStepFinder().GetMultiplePaths(pCityPlot, vDestPlots, data);
		for (map<int,SPath>::iterator it=landpaths.begin(); it!=landpaths.end(); ++it)
		{
			CvPlot* plot = GC.getMap().plotByIndex(it->first);
			// if this is the origin city, nothing to do
			if (pCityPlot == plot)
				continue;

			CvCity* pDestCity = plot->getPlotCity();
			AddTradePathToCache(m_aLandPaths,iCityPlotIndex,pDestCity->plot()->GetPlotIndex(),it->second);
		}

	}

	SetTradePathsCacheUpdateCounter(ePlayer1, GC.getGame().getGameTurn());

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
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	else if (eConnectionType == TRADE_CONNECTION_PRODUCTION || eConnectionType == TRADE_CONNECTION_FOOD || (MOD_TRADE_WONDER_RESOURCE_ROUTES && eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE) || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES) && eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
#elif defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
	else if (eConnectionType == TRADE_CONNECTION_PRODUCTION || eConnectionType == TRADE_CONNECTION_FOOD || (MOD_TRADE_WONDER_RESOURCE_ROUTES && eConnectionType == TRADE_CONNECTION_WONDER_RESOURCE))
#elif defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	else if (eConnectionType == TRADE_CONNECTION_PRODUCTION || eConnectionType == TRADE_CONNECTION_FOOD || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES) && eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
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
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		}
		else if (eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
		{
			if (!GET_PLAYER(pOriginCity->getOwner()).GetPlayerTraits()->IsCanGoldInternalTradeRoutes())
			{
				return false;
			}
#endif
		}
	}
	else
	{
		// Unknown trade connection type
		return false;
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

				//only one TR per player and city. unless venice is involved.
				if (m_aTradeConnections[i].m_iDestX == iDestX && m_aTradeConnections[i].m_iDestY == iDestY)
				{
					if (!GET_PLAYER(m_aTradeConnections[i].m_eOriginOwner).GetPlayerTraits()->IsNoAnnexing() && !GET_PLAYER(m_aTradeConnections[i].m_eDestOwner).GetPlayerTraits()->IsNoAnnexing() )
						return false;
				}
			}
		}
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		else if (MOD_TRADE_ROUTE_SCALING && MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
		{
			// check for duplicate routes from the owner
			for (uint i = 0; i < m_aTradeConnections.size(); i++)
			{
				if (m_aTradeConnections[i].m_eOriginOwner != pOriginCity->getOwner())
					continue;

				// Cannot send more than one gold internal trade routes to a city (similar rule to international). 
				// Still can send production or food trade routes from another origin city (remember the rule, there can only be one route of any type for an origin-destination pair).
				if (m_aTradeConnections[i].m_eConnectionType == eConnectionType && m_aTradeConnections[i].m_iDestX == iDestX && m_aTradeConnections[i].m_iDestY == iDestY)
				{
					return false;
				}
			}
		}
#endif
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

	int iOriginCityLoop = 0;
	CvCity* pOriginLoopCity = NULL;
	for(pOriginLoopCity = GET_PLAYER(eOriginPlayer).firstCity(&iOriginCityLoop); pOriginLoopCity != NULL; pOriginLoopCity = GET_PLAYER(eOriginPlayer).nextCity(&iOriginCityLoop))
	{
		int iDestCityLoop = 0;
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
	m_aTradeConnections[iNewTradeRouteIndex].m_iSpeedFactor = (100 * SPath::getNormalizedDistanceBase() * path.length()) / max(1,path.iNormalizedDistanceRaw);

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
	m_aTradeConnections[iNewTradeRouteIndex].m_bTradeUnitRecalled = false;

	//update cache
	m_routesPerPlayer[eOriginPlayer].push_back(iNewTradeRouteIndex);

	if (eOriginPlayer != eDestPlayer)
		m_routesPerPlayer[eDestPlayer].push_back(iNewTradeRouteIndex);

	//OutputDebugString(CvString::format("created TR from player %d to %d at index %d\n",eOriginPlayer,eDestPlayer,iNewTradeRouteIndex).c_str());

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
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_POLICIES && MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && (GET_PLAYER(eOriginPlayer).IsGoldInternalTrade()) && (eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
	{
		GET_PLAYER(eOriginPlayer).GetTreasury()->DoUpdateCityConnectionGold();
	}
#endif
#endif
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
	// Apply a diplomacy bonus for recent trade in both directions
	if (eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		if (!GET_PLAYER(eDestPlayer).isHuman() && GET_PLAYER(eDestPlayer).isMajorCiv())
		{
			int iFlavorGoldDest = GET_PLAYER(eDestPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			int iFlavorScienceDest = GET_PLAYER(eDestPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			int iFlavorDiplomacyDest = GET_PLAYER(eDestPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
			int iFlavorCultureDest = GET_PLAYER(eDestPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			int iGoldDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_GOLD] / 10);
			int iScienceDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_SCIENCE] / 10);
			int iCultureDest = (m_aTradeConnections[iNewTradeRouteIndex].m_aiDestYields[YIELD_CULTURE] / 10);
			int iTradeValueDest = ((iScienceDest + iCultureDest + iGoldDest + iFlavorCultureDest + iFlavorGoldDest + iFlavorScienceDest + iFlavorDiplomacyDest) / 4);
			if (iTradeValueDest > 0)
			{
				GET_PLAYER(eDestPlayer).GetDiplomacyAI()->ChangeRecentTradeValue(eOriginPlayer, iTradeValueDest);
			}
		}
		if (!GET_PLAYER(eOriginPlayer).isHuman() && GET_PLAYER(eOriginPlayer).isMajorCiv())
		{
			int iFlavorGoldOrigin = GET_PLAYER(eOriginPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			int iFlavorScienceOrigin = GET_PLAYER(eOriginPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			int iFlavorDiplomacyOrigin = GET_PLAYER(eOriginPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
			int iFlavorCultureOrigin = GET_PLAYER(eOriginPlayer).GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			int iGoldOrigin = (m_aTradeConnections[iNewTradeRouteIndex].m_aiOriginYields[YIELD_GOLD] / 10);
			int iScienceOrigin = (m_aTradeConnections[iNewTradeRouteIndex].m_aiOriginYields[YIELD_SCIENCE] / 10);
			int iCultureOrigin = (m_aTradeConnections[iNewTradeRouteIndex].m_aiOriginYields[YIELD_CULTURE] / 10);
			int iTradeValueOrigin = ((iScienceOrigin + iCultureOrigin + iFlavorCultureOrigin + iGoldOrigin + iFlavorGoldOrigin + iFlavorScienceOrigin + iFlavorDiplomacyOrigin) / 4);
			if (iTradeValueOrigin > 0)
			{
				GET_PLAYER(eOriginPlayer).GetDiplomacyAI()->ChangeRecentTradeValue(eDestPlayer, iTradeValueOrigin);
			}
		}
	}
	if (MOD_BALANCE_VP)
	{
		GET_PLAYER(eOriginPlayer).CalculateNetHappiness();
		if (eOriginPlayer != eDestPlayer)
			GET_PLAYER(eDestPlayer).CalculateNetHappiness();
	}
	GET_PLAYER(eOriginPlayer).UpdateReligion();
	if (eOriginPlayer != eDestPlayer)
		GET_PLAYER(eDestPlayer).UpdateReligion();

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
		default:
			UNREACHABLE(); // Non-applicable domain.
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
#if defined (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		case TRADE_CONNECTION_GOLD_INTERNAL:
			strTRType = "gold internal";
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
	 CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	 if (pTrade)
	 {
		 pTrade->UpdateTradePlots();
	 }
	 if(m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size() > 0)
	 {
		for (uint ui = 0; ui < m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList.size(); ui++)
		{
			int iPlotX = m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iX;
			int iPlotY = m_aTradeConnections[iNewTradeRouteIndex].m_aPlotList[ui].m_iY;
			if(iPlotX != -1 && iPlotY != -1)
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

		int iNormDist = pPath->iNormalizedDistanceRaw;
		if (iNormDist>0 && iNormDist<=iMaxNormDist*SPath::getNormalizedDistanceBase())
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

		int iNormDist = pPath->iNormalizedDistanceRaw;
		if (iNormDist>0 && iNormDist <= iMaxNormDist*SPath::getNormalizedDistanceBase())
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
		return /*100 in CP, 25 in VP*/ GD_INT_GET(TRADE_ROUTE_BASE_SEA_MODIFIER);
	}

	return /*0*/ GD_INT_GET(TRADE_ROUTE_BASE_LAND_MODIFIER);
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
int CvGameTrade::CountNumPlayerConnectionsToPlayer (PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer, bool bFirstPlayerOnly)
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
		else if (!bFirstPlayerOnly && pConnection->m_eOriginOwner == eSecondPlayer && pConnection->m_eDestOwner == eFirstPlayer)
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
	return m_aTradeConnections[iIndex].m_iOriginX == -1 && m_aTradeConnections[iIndex].m_iOriginY == -1 && m_aTradeConnections[iIndex].m_iDestX == -1 && m_aTradeConnections[iIndex].m_iDestY == -1;
}

//	--------------------------------------------------------------------------------
bool CvGameTrade::ClearTradeRoute(int iIndex)
{
	CvAssertMsg(iIndex >= 0 && iIndex < (int)m_aTradeConnections.size(), "iIndex out of bounds");
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
		return false;

	TradeConnection& kTradeConnection = m_aTradeConnections[iIndex];
	if (!kTradeConnection.isValid())
		return false;

	PlayerTypes eOriginPlayer = kTradeConnection.m_eOriginOwner;
	PlayerTypes eDestPlayer = kTradeConnection.m_eDestOwner;

	// Remove any visualization
	if (kTradeConnection.m_unitID != -1)
	{
		CvUnit* pkUnit = GET_PLAYER(kTradeConnection.m_eOriginOwner).getUnit(kTradeConnection.m_unitID);
		if (pkUnit)
		{
			//remember some things
			CvPlot* pPlot = pkUnit->plot();
			TeamTypes eOwnerTeam = pkUnit->getTeam();

			pkUnit->kill(false);

			//force a visibility update so that it's possible to plunder a traderoute with impunity
			pPlot->flipVisibility(eOwnerTeam);
		}
	}
#if defined(MOD_BALANCE_CORE)
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
	{
		CvCity* pOriginCity = GET_PLAYER(eOriginPlayer).getCity(kTradeConnection.m_iOriginID);
		CvCity* pDestCity = GET_PLAYER(eDestPlayer).getCity(kTradeConnection.m_iDestID);
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
	
	//update cache
	vector<int>& originRoutes = m_routesPerPlayer[eOriginPlayer];
	vector<int>& destRoutes = m_routesPerPlayer[eDestPlayer];
	originRoutes.erase(std::remove(originRoutes.begin(), originRoutes.end(), iIndex), originRoutes.end());
	destRoutes.erase(std::remove(destRoutes.begin(), destRoutes.end(), iIndex), destRoutes.end());
	//OutputDebugString(CvString::format("cleared TR from player %d to %d at index %d\n",eOriginPlayer,eDestPlayer,iIndex).c_str());

	//reset to default
	kTradeConnection = TradeConnection();

	GET_PLAYER(eOriginPlayer).GetTrade()->UpdateTradeConnectionValues();
	GET_PLAYER(eDestPlayer).GetTrade()->UpdateTradeConnectionValues();

	GET_PLAYER(eOriginPlayer).UpdateReligion();
	if (eOriginPlayer != eDestPlayer)
		GET_PLAYER(eDestPlayer).UpdateReligion();

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
	int iSpeedFactor = (100 * SPath::getNormalizedDistanceBase() * path.length()) / max(1,path.iNormalizedDistanceRaw);
	int iRouteSpeed = int(0.5f + iSpeedFactor*iRawSpeed / 100.f);

	float fTurnsPerCircuit = 1;
	if (iRouteSpeed != 0)
		fTurnsPerCircuit = (iDistance * 2.f - 2) / iRouteSpeed;

	int iTargetTurns = /*30*/ GD_INT_GET(TRADE_ROUTE_BASE_TARGET_TURNS); // how many turns do we want the cycle to consume
	iTargetTurns = iTargetTurns * GC.getGame().getGameSpeedInfo().getTradeRouteSpeedMod() / 100;
	iTargetTurns = max(iTargetTurns, 1);
	iTargetTurns += (int)(iTargetTurns * (GET_PLAYER(pOriginCity->getOwner()).GetTrade()->GetTradeRouteTurnMod(pOriginCity) / 100.0));

	// calculate how many circuits do we want this trade route to run to reach the target turns
	int iCircuitsToComplete = 1; 
	if (fTurnsPerCircuit != 0)
		iCircuitsToComplete = max( int(iTargetTurns/fTurnsPerCircuit), 2);

	// return values
	if (piCircuitsToComplete != NULL) 
		*piCircuitsToComplete = iCircuitsToComplete;
	return int(fTurnsPerCircuit * iCircuitsToComplete);
}
#endif
//	--------------------------------------------------------------------------------
/// Called when a city changes hands
void CvGameTrade::ClearAllCityTradeRoutes (CvPlot* pPlot, bool bIncludeTransits)
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
					CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
					UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);

					CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
					if (eUnitType != NO_UNIT)
					{
						GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
					}
				}

				ClearTradeRoute(ui);
			}		

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
		}
	}
}

//	--------------------------------------------------------------------------------
//  Reset all Civ to Civ trade routes involving ePlayer.  Trade routes involving city-states are not reset.
void CvGameTrade::ClearAllCivTradeRoutes (PlayerTypes ePlayer, bool bFromEmbargo /* = false */)
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
				if (bFromEmbargo)
				{
					// Master/vassal trade routes aren't cancelled by an embargo
					if (GET_TEAM(GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).getTeam()).IsVassal(GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).getTeam()) || GET_TEAM(GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).getTeam()).IsVassal(GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).getTeam()))
						continue;
				}

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

				ClearTradeRoute(ui);
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
				CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
				UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);

				CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
				if (eUnitType != NO_UNIT)
				{
					GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
				}
			}

			ClearTradeRoute(ui);
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
				CvPlayer& kPlayer = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner);
				UnitTypes eUnitType = kPlayer.GetTrade()->GetTradeUnit(m_aTradeConnections[ui].m_eDomain, &kPlayer);

				CvAssertMsg(eUnitType != NO_UNIT, "No trade unit found");
				if (eUnitType != NO_UNIT)
				{
					GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).initUnit(eUnitType, m_aTradeConnections[ui].m_iOriginX, m_aTradeConnections[ui].m_iOriginY, UNITAI_TRADE_UNIT);
				}
			}

			ClearTradeRoute(ui);
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

				ClearTradeRoute(ui);
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
			continue;

		if (IsRecalledUnit(ui))
			continue;

		TeamTypes eOriginTeam = GET_PLAYER(m_aTradeConnections[ui].m_eOriginOwner).getTeam();
		TeamTypes eDestTeam = GET_PLAYER(m_aTradeConnections[ui].m_eDestOwner).getTeam();
		if ((eOriginTeam == eTeam1 && eDestTeam == eTeam2) || (eOriginTeam == eTeam2 && eDestTeam == eTeam1)) 
		{
			ClearTradeRoute(ui);
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
			PlayerTypes eTRPlayer = (PlayerTypes)uiPlayer;
			if (GET_PLAYER(eTRPlayer).getTeam() != eTRTeam)
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
				if (m_aTradeConnections[uiTradeRoute].m_eOriginOwner != eTRPlayer)
				{
					continue;
				}

				// if the destination is not the civ I'm going to war with
				if (GET_PLAYER(m_aTradeConnections[uiTradeRoute].m_eDestOwner).getTeam() != ePlunderTeam)
				{
					continue;
				}

				// Recall trade units
				if (MOD_BALANCE_CORE || GET_PLAYER(eTRPlayer).GetPlayerTraits()->IsNoAnnexing())
				{
					RecallUnit(uiTradeRoute, true);
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
								GET_PLAYER(pLoopUnit->getOwner()).GetTrade()->PlunderTradeRoute(m_aTradeConnections[uiTradeRoute].m_iID, pLoopUnit);
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
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
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
	if (iIndex < 0 || iIndex >= (int)m_aTradeConnections.size())
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
		if(GET_PLAYER(ePlayer2).isMinorCiv() && MOD_BALANCE_CORE)
		{
			if(GET_PLAYER(ePlayer2).GetMinorCivAI()->GetAlly() == ePlayer)
			{
				int iAllyScience = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA);
				if(iAllyScience <= 0)
				{
					return 0;
				}
				return iAllyScience;
			}
			else if(GET_PLAYER(ePlayer2).GetMinorCivAI()->IsFriends(ePlayer))
			{
				int iFriendScience = /*0*/ GD_INT_GET(TRADE_ROUTE_CS_FRIEND_SCIENCE_DELTA);
				if(iFriendScience <= 0)
				{
					return 0;
				}
				return iFriendScience;
			}
		}

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

	if (!MOD_BALANCE_CORE)
		return 0;

	if (GET_PLAYER(ePlayer2).isMinorCiv() || GET_PLAYER(ePlayer2).isBarbarian())
	{
		if (GET_PLAYER(ePlayer2).isMinorCiv() && MOD_BALANCE_CORE)
		{
			if (GET_PLAYER(ePlayer2).GetMinorCivAI()->GetAlly() == ePlayer)
			{
				int iAllyCulture = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA);
				if (iAllyCulture <= 0)
				{
					return 0;
				}
				return 	iAllyCulture;
			}
			else if (GET_PLAYER(ePlayer2).GetMinorCivAI()->IsFriends(ePlayer))
			{
				int iFriendCulture = /*0*/ GD_INT_GET(TRADE_ROUTE_CS_FRIEND_CULTURE_DELTA);
				if (iFriendCulture <= 0)
				{
					return 0;
				}
				return 	iFriendCulture;
			}
		}

		return 0;
	}

	return m_aaiPolicyDifference[ePlayer][ePlayer2];
}

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

	CvUnit *pkUnit = GetTradeUnitForRoute(iIndex);
	if (pkUnit)
	{
		//show the movement animation
		pkUnit->finishMoves();

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

#if defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
						if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY && pkUnit->IsInForeignOwnedTerritory())
						{
							GET_PLAYER(pCity->getOwner()).doInstantYield(
								INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN, false, NO_GREATPERSON, NO_BUILDING, 0, true,
								NO_PLAYER, NULL, false, pCity, (pTradeConnection->m_eDomain == DOMAIN_SEA), true, false, NO_YIELD, pkUnit);

							CvPlot* pDestPlot = GC.getMap().plot(pTradeConnection->m_iDestX, pTradeConnection->m_iDestY);

							if (pDestPlot && pDestPlot->isCity())
							{
								CvCity* pDestCity = GC.getMap().plot(pTradeConnection->m_iDestX, pTradeConnection->m_iDestY)->getPlotCity();
								PlayerTypes eDestPlayer = pDestCity->getOwner();

								if (pDestCity != NULL && eDestPlayer != NO_PLAYER && GET_PLAYER(eDestPlayer).isBarbarian() == false && pkUnit->plot()->getOwner() == eDestPlayer)
								{
									GET_PLAYER(eDestPlayer).doInstantYield(
										INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN, false, NO_GREATPERSON, NO_BUILDING, 0, true,
										NO_PLAYER, NULL, false, pDestCity, (pTradeConnection->m_eDomain == DOMAIN_SEA), true, false, NO_YIELD, pkUnit);
								}
							}
						}
#endif
					}
				}
			}
		}
#endif
	}

	gDLL->TradeVisuals_UpdateRouteDirection(iIndex, kTradeConnection.m_bTradeUnitMovingForward);
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
		pkUnit->setXY(kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY, true, false, true, true);

	// auto-pillage when a trade unit moves under an enemy unit
	CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY);
	if (pPlot)
	{
#if defined(MOD_CIV6_ROADS)
		if (MOD_CIV6_ROADS)
		{
			//build land routes
			RouteTypes autoBuildRoadType = ROUTE_ROAD;
			// check if the rairoad is unlocked (by tech and by era).
			EraTypes eModern = (EraTypes)GC.getInfoTypeForString("ERA_MODERN", true);
			TechTypes iRequiredTech = (TechTypes)GC.getInfoTypeForString("TECH_RAILROAD", true);
			if ((GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() >= eModern) && (GET_TEAM(GET_PLAYER(kTradeConnection.m_eOriginOwner).getTeam()).GetTeamTechs()->HasTech(iRequiredTech)))
			{
				autoBuildRoadType = ROUTE_RAILROAD;
			}

			//don't build roads on sea
			if (!pPlot->IsPlotOcean())
			{
				//don't build a road if route is already a Railroad
				if (pPlot->getRouteType() != ROUTE_RAILROAD)
				{
					CvRouteInfo* pkRouteInfo = GC.getRouteInfo(autoBuildRoadType);
					if (pkRouteInfo)
					{
						pPlot->setRouteType(autoBuildRoadType, kTradeConnection.m_eOriginOwner);
					}
				}
			}
		}
#endif
		CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(kTradeConnection.m_eOriginOwner);
		if (pEnemyUnit)
		{
			if (pEnemyUnit->canPlunderTradeRoute(pPlot, false))
			{
				GET_PLAYER(pEnemyUnit->getOwner()).GetTrade()->PlunderTradeRoute(kTradeConnection.m_iID, pEnemyUnit);
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
		CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
		UnitTypes eUnitType = CvPlayerTrade::GetTradeUnit(kTradeConnection.m_eDomain, &kPlayer);

		if (eUnitType != NO_UNIT)
		{
			CvUnit* pkUnit = kPlayer.initUnit(eUnitType, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iX, kTradeConnection.m_aPlotList[kTradeConnection.m_iTradeUnitLocationIndex].m_iY, NO_UNITAI, REASON_DEFAULT, false, true, TRADE_UNIT_MAP_LAYER);
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
	int iOriginX = 0;
	int iOriginY = 0;
	PlayerTypes eOriginPlayer;

	CvInterfacePtr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
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
				int plotsX[MAX_PLOTS_TO_DISPLAY];
				int plotsY[MAX_PLOTS_TO_DISPLAY];
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
void CvGameTrade::HideTemporaryPopupTradeRoute(int iPlotX, int iPlotY, TradeConnectionType type) const
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
void CvGameTrade::LogTradeMsg(CvString& strMsg) const
{
	if (GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog = NULL;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

void CvPlayerTrade::LogTradeMsg(CvString& strMsg) const
{
	if (GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog = NULL;

		CvString logName;
		logName.Format("TradePlayerRouteLog_%s.csv", m_pPlayer->getCivilizationShortDescription());

		pLog = LOGFILEMGR.GetLog(logName.c_str(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

const vector<int>& CvGameTrade::GetTradeConnectionsForPlayer(PlayerTypes ePlayer) const
{
	if (ePlayer>=0 && (size_t)ePlayer<m_routesPerPlayer.size())
		return m_routesPerPlayer[ePlayer];

	//should be the barbarians, always empty
	return m_routesPerPlayer.back();
}

//	--------------------------------------------------------------------------------
template<typename GameTrade, typename Visitor>
void CvGameTrade::Serialize(GameTrade& gameTrade, Visitor& visitor)
{
	visitor(gameTrade.m_aTradeConnections);
	visitor(gameTrade.m_routesPerPlayer);
	visitor(gameTrade.m_aaiTechDifference);
	visitor(gameTrade.m_aaiPolicyDifference);
	visitor(gameTrade.m_iNextID);
}

//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvGameTrade& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameTrade::Serialize(readFrom, serialVisitor);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvGameTrade& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameTrade::Serialize(writeTo, serialVisitor);

	int plotsX[MAX_PLOTS_TO_DISPLAY];
	int plotsY[MAX_PLOTS_TO_DISPLAY];
	for (int i = 0; i < int(writeTo.m_aTradeConnections.size()); ++i)
	{
		int nPlots = std::min(MAX_PLOTS_TO_DISPLAY, int(writeTo.m_aTradeConnections[i].m_aPlotList.size()));
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

	return loadFrom;
}

//	----------------------------------------------------------------------------
template<typename TradeConnectionPlotT, typename Visitor>
void TradeConnectionPlot::Serialize(TradeConnectionPlotT& tradeConnectionPlot, Visitor& visitor)
{
	visitor(tradeConnectionPlot.m_iX);
	visitor(tradeConnectionPlot.m_iY);
}

FDataStream& operator<<(FDataStream& saveTo, const TradeConnectionPlot& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	TradeConnectionPlot::Serialize(readFrom, serialVisitor);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, TradeConnectionPlot& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	TradeConnectionPlot::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

//	----------------------------------------------------------------------------
template<typename TradeConnectionT, typename Visitor>
void TradeConnection::Serialize(TradeConnectionT& tradeConnection, Visitor& visitor)
{
	visitor(tradeConnection.m_iID);
	visitor(tradeConnection.m_iOriginID);
	visitor(tradeConnection.m_iOriginX);
	visitor(tradeConnection.m_iOriginY);
	visitor(tradeConnection.m_iDestID);
	visitor(tradeConnection.m_iDestX);
	visitor(tradeConnection.m_iDestY);
	visitor(tradeConnection.m_eOriginOwner);
	visitor(tradeConnection.m_eDestOwner);
	visitor(tradeConnection.m_eDomain);
	visitor(tradeConnection.m_eConnectionType);
	visitor(tradeConnection.m_iTradeUnitLocationIndex);
	visitor(tradeConnection.m_bTradeUnitMovingForward);
	visitor(tradeConnection.m_unitID);
	visitor(tradeConnection.m_iSpeedFactor);
	visitor(tradeConnection.m_iCircuitsCompleted);
	visitor(tradeConnection.m_iCircuitsToComplete);
	visitor(tradeConnection.m_iTurnRouteComplete);
	visitor(tradeConnection.m_bTradeUnitRecalled);
	visitor(tradeConnection.m_aPlotList);
	visitor(tradeConnection.m_aiOriginYields);
	visitor(tradeConnection.m_aiDestYields);
}

FDataStream& operator<<(FDataStream& saveTo, const TradeConnection& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	TradeConnection::Serialize(readFrom, serialVisitor);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, TradeConnection& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	TradeConnection::Serialize(writeTo, serialVisitor);
	return loadFrom;
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

				int iDestX = pTradeConnection->m_iDestX;
				int iDestY = pTradeConnection->m_iDestY;

				//Free lump resource when you complete an international trade route.
				CvPlot* pPlot = GC.getMap().plot(iOriginX, iOriginY);

				CvCity* pOriginCity = NULL;
				if (pPlot != NULL)
				{
					pOriginCity = pPlot->getPlotCity();
				}

				if (!pTradeConnection->m_bTradeUnitRecalled)
				{	
					if (pOriginCity != NULL)
					{
						bool bInternational = false;
						if (pTradeConnection->m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
						{
							bInternational = true;
						}

						CvPlot* pDestPlot = GC.getMap().plot(iDestX, iDestY);

						CvCity* pDestCity = NULL;
						if (pDestPlot != NULL)
						{
							pDestCity = pDestPlot->getPlotCity();
						}

						m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_TR_END, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pOriginCity, false, bInternational, false, NO_YIELD, NULL, NO_TERRAIN, NULL, pDestCity);
						if (pDestCity != NULL && pOriginCity != NULL)
						{
							CvPlayer& kOriginPlayer = GET_PLAYER(pOriginCity->getOwner());
							CvPlayer& kDestPlayer = GET_PLAYER(pDestCity->getOwner());
							if (bInternational || m_pPlayer->IsInternalTRTourism())
							{
								// Corporation expansion system
								// Note: These functions will filter out if this is actually possible, so don't worry about it here
								kOriginPlayer.GetCorporations()->BuildFranchiseInCity(pOriginCity, pDestCity);
								kDestPlayer.GetCorporations()->BuildFranchiseInCity(pDestCity, pOriginCity);

								//Tourism Bonus from Buildings
								if (!kDestPlayer.isMinorCiv())
								{
									Localization::String strMessage;
									HistoricEventTypes eHistoricEvent;
									if (pTradeConnection->m_eDomain == DOMAIN_LAND)
									{
										strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_LAND");
										eHistoricEvent = HISTORIC_EVENT_TRADE_LAND;
									}
									else if (pTradeConnection->m_eDomain == DOMAIN_SEA)
									{
										strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_SEA");
										eHistoricEvent = HISTORIC_EVENT_TRADE_SEA;
									}
									else
									{
										UNREACHABLE();
									}

									int iTourism = kOriginPlayer.GetHistoricEventTourism(eHistoricEvent, pOriginCity);
									if (iTourism > 0)
									{
										kOriginPlayer.ChangeNumHistoricEvents(eHistoricEvent, 1);
										if (bInternational)
											kOriginPlayer.GetCulture()->ChangeInfluenceOn(pDestCity->getOwner(), iTourism, true, true);
										kOriginPlayer.GetCulture()->AddTourismAllKnownCivsOtherCivWithModifiers(pDestCity->getOwner(), iTourism / 3);

										// Show tourism spread
										if (pOriginCity->getOwner() == GC.getGame().getActivePlayer() && pDestCity->plot() != NULL && pDestCity->plot()->isRevealed(pOriginCity->getTeam()))
										{
											InfluenceLevelTypes eLevel = kOriginPlayer.GetCulture()->GetInfluenceLevel(pDestCity->getOwner());
											SHOW_PLOT_POPUP(pDestCity->plot(), pOriginCity->getOwner(), CultureHelpers::GetInfluenceText(eLevel, iTourism).c_str());

											CvNotifications* pNotification = kOriginPlayer.GetNotifications();
											if (pNotification)
											{
												Localization::String strSummary;
												strMessage << iTourism;
												strMessage << kDestPlayer.getCivilizationAdjectiveKey();
												strMessage << pOriginCity->getNameKey();
												strMessage << pDestCity->getNameKey();
												strMessage << kDestPlayer.getCivilizationShortDescriptionKey();
												strMessage << (iTourism / 3);
												if (GC.getGame().isGameMultiPlayer() && kDestPlayer.isHuman())
												{
													strMessage << kDestPlayer.getNickName();
												}
												else
												{
													strMessage << kDestPlayer.getNameKey();
												}
												strSummary = Localization::Lookup("TXT_KEY_TOURISM_EVENT_SUMMARY_TRADE");
												pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pOriginCity->getX(), pOriginCity->getY(), pOriginCity->getOwner());
											}
										}
									}

									// The Difficulty Bonus trigger is decoupled from the general Historic Event code so it doesn't trigger on internal Trade Route completion.
									if (bInternational)
									{
										kOriginPlayer.DoDifficultyBonus(eHistoricEvent);
									}
								}
								else
								{
									if (kOriginPlayer.GetEventTourismCS() > 0)
									{
										int iTourism = kOriginPlayer.GetHistoricEventTourism(HISTORIC_EVENT_TRADE_CS);
										kOriginPlayer.ChangeNumHistoricEvents(HISTORIC_EVENT_TRADE_CS, 1);
										if (iTourism > 0)
										{
											kOriginPlayer.GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
											CvNotifications* pNotification = kOriginPlayer.GetNotifications();
											if (pNotification)
											{
												Localization::String strSummary;
												Localization::String strMessage;
												strMessage = Localization::Lookup("TXT_KEY_TOURISM_EVENT_TRADE_CS_BONUS");
												strMessage << iTourism;
												strMessage << kDestPlayer.getCivilizationShortDescriptionKey();
												strSummary = Localization::Lookup("TXT_KEY_TOURISM_EVENT_SUMMARY_TRADE_CS");
												pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pOriginCity->getX(), pOriginCity->getY(), pOriginCity->getOwner());
											}
										}
									}
									else
									{
										kOriginPlayer.DoDifficultyBonus(HISTORIC_EVENT_TRADE_CS);
									}
								}
							}
						}
					}
				}

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

				// wipe trade route
				pTrade->ClearTradeRoute(ui);

				// create new unit
				UnitTypes eUnitType = GetTradeUnit(eDomain, m_pPlayer);
				m_pPlayer->initUnit(eUnitType, iOriginX, iOriginY, UNITAI_TRADE_UNIT);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionBaseValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer) const
{
	if (bAsOriginPlayer)
	{
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection) || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
#else
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
#endif
		{
			if (eYield == YIELD_GOLD)
			{
				int iResult = 0;
				int iBase = /*100 in CP, 80 in VP*/ GD_INT_GET(INTERNATIONAL_TRADE_BASE) * (m_pPlayer->GetCurrentEra()+2);
				iResult = iBase;
				return iResult;
			}
			else if (eYield == YIELD_SCIENCE)
			{
				int iTechDifference = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
				if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
				{
					iTechDifference = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA);
				}
#endif
				int iAdjustedTechDifference = 0;
				if (iTechDifference > 0)
				{
					int iCeilTechDifference = int( sqrt((float)iTechDifference) * 200.f / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100));
					iAdjustedTechDifference = max(iCeilTechDifference, 1);

					if(iAdjustedTechDifference > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
					{
						iAdjustedTechDifference *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
					}

					// Policy bump
					int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
					if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
					{
						iPolicyBump = 0;
					}
#endif

					iAdjustedTechDifference += iPolicyBump;

				}
				// Cultural influence bump
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
				int iInfluenceBoost = 0;
				if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
				{
					iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteScienceBonus(kTradeConnection.m_eDestOwner);
				}
#else
				int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteScienceBonus(kTradeConnection.m_eDestOwner);
#endif

				if (iInfluenceBoost > 0)
					iAdjustedTechDifference += iInfluenceBoost;

				return iAdjustedTechDifference * 100;
			}
			else if (eYield == YIELD_CULTURE)
			{
				int iCultureDifference = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
				if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
				{
					iCultureDifference = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA);
				}
#endif
				int iAdjustedCultureDifference = 0;
				if (iCultureDifference > 0)
				{
					int iCeilCultureDifference = int( sqrt((float)iCultureDifference) * 200.f / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100));
					iAdjustedCultureDifference = max(iCeilCultureDifference, 1);

					if (iAdjustedCultureDifference > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
					{
						iAdjustedCultureDifference *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
					}
			
					// Policy bump
					int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
					if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
					{
						iPolicyBump = 0;
					}
#endif

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
				int iCeilTechDifference = int( sqrt((float)iTechDifference) * 200.f / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100));
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
				int iCeilCultureDifference = int( sqrt((float)iCultureDifference) * 200.f / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100));
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
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection) || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
#else
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
#endif
		{
			if (eYield == YIELD_GOLD)
			{
				int iX = 0;
				int iY = 0;
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

				int iDivisor = /*20 in CP, 80 in VP*/ GD_INT_GET(INTERNATIONAL_TRADE_CITY_GPT_DIVISOR);
				if (iDivisor == 0)
				{
					iDivisor = 1;
				}

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
		if (GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection) || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
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

				for (int i = 0; i < GC.getNumResourceInfos(); i++)
				{
					ResourceTypes eResource = (ResourceTypes)i;
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					if (pkResourceInfo)
					{
						bool bMonopoly = false;
						bool bIsBonus = false;
						if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
						{	
							int eResourceClassCurrent = pkResourceInfo->getResourceClassType();
							if (GET_PLAYER(pOriginCity->getOwner()).HasGlobalMonopoly(eResource) || GET_PLAYER(pDestCity->getOwner()).HasGlobalMonopoly(eResource))
								bMonopoly = true;

							if (RESOURCEUSAGE_BONUS == eResourceClassCurrent)
								bIsBonus = true;
						}
						
						ResourceUsageTypes eResourceUsageCurrent = pkResourceInfo->getResourceUsage();
						if (eResourceUsageCurrent == RESOURCEUSAGE_LUXURY || eResourceUsageCurrent == RESOURCEUSAGE_STRATEGIC || bIsBonus)
						{		
							if (pOriginCity->IsHasResourceLocal(eResource, true) != pDestCity->IsHasResourceLocal(eResource, true))
							{
								iValue += (bMonopoly ? 2 : 1 ) * /*50 in CP, 5 in VP*/ GD_INT_GET(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE);
							}
						}
					}
				}

				int iModifier = 100 + GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetTradeRouteResourceModifier() + pOriginCity->GetResourceDiversityModifier();
				if (iValue > 0)
				{
					iValue *= iModifier;
					iValue /= 100;
				}
				else
				{
					iValue *= 100;
					iValue /= max(1, iModifier);
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
	// por favor, don't buzz off please, gold internal trade routes
	if (!GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection) && !(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
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
		if(GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && MOD_BALANCE_CORE)
		{
			if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsAllies(kTradeConnection.m_eOriginOwner))
			{
				int iAllyGold = (/*2*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_GOLD) * 100);
				if(iAllyGold > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
				{
					iAllyGold *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
				}
				iResult = iAllyGold;
			}
			else if(GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsFriends(kTradeConnection.m_eOriginOwner))
			{
				int iFriendGold = (/*1*/ GD_INT_GET(TRADE_ROUTE_CS_FRIEND_GOLD) * 100);
				if(iFriendGold > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
				{
					iFriendGold *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
				}
				iResult = iFriendGold;
			}
		}
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
		// gold internal trade routes get bonus gold as if trading with an allied city state
		else if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && MOD_BALANCE_CORE && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
		{
			int iAllyGold = (/*2*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_GOLD) * 100);
			if (iAllyGold > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
			{
				iAllyGold *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
			}
			iResult = iAllyGold;
		}
#endif
	}
	return iResult;
}
#endif
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionTheirBuildingValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer) const
{
	// this only applies to international trade routes, so otherwise, buzz off!
	// por favor, don't buzz off please, gold internal trade routes
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (!GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection) && !(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
#else
	if (!GC.getGame().GetGameTrade()->IsConnectionInternational(kTradeConnection))
#endif
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
int CvPlayerTrade::GetTradeConnectionExclusiveValueTimes100(const TradeConnection& kTradeConnection, YieldTypes /*eYield*/)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	if (pTrade->IsDestinationExclusive(kTradeConnection))
	{
		return /*0*/ GD_INT_GET(INTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION);
	}
	else
	{
		return 0;
	}
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionPolicyValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield) const
{
	int iValue = 0;

	CvPlayer& kPlayer = GET_PLAYER(kTradeConnection.m_eOriginOwner);
	iValue += kPlayer.getTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100;
	iValue += kPlayer.GetPlayerTraits()->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100;

	CvCity* pCity = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY)->getPlotCity();
	ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
	BeliefTypes eSecondaryPantheon = NO_BELIEF;
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
		if(pReligion)
		{
			int iEra = m_pPlayer->GetCurrentEra();
			if (iEra <= 0)
				iEra = 1;

			CvCity* pHolyCity = pReligion->GetHolyCity();
			eSecondaryPantheon = pCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
			if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL && (eYield == YIELD_GOLD || eYield == YIELD_CULTURE || eYield == YIELD_SCIENCE))
			{
				iValue += pReligion->m_Beliefs.GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield, kPlayer.GetID(), pHolyCity) * 100 * iEra;
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iValue += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100 * iEra;
				}
			}

			if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL && (eYield == YIELD_PRODUCTION || eYield == YIELD_FOOD))
			{
				iValue += pReligion->m_Beliefs.GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield, kPlayer.GetID(), pHolyCity) * 100 * iEra;
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iValue += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100 * iEra;
				}
			}
		}
	}

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
	// Mod for civs keeping their pantheon belief forever
	if (MOD_RELIGION_PERMANENT_PANTHEON)
	{
		if (GC.getGame().GetGameReligions()->HasCreatedPantheon(kTradeConnection.m_eOriginOwner))
		{
			const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, kTradeConnection.m_eOriginOwner);
			BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(kTradeConnection.m_eOriginOwner);
			if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
				if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, kTradeConnection.m_eOriginOwner))) // check that the our religion does not have our belief, to prevent double counting
				{
					iValue += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetTradeRouteYieldChange(kTradeConnection.m_eDomain, eYield) * 100 * MAX(1, (int)m_pPlayer->GetCurrentEra());
				}
			}
		}
	}
#endif

#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
#else
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
#endif
	{
	  if (eYield == YIELD_GOLD) 
	  {
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

		PolicyBranchTypes eFreedom = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_FREEDOM);
		PolicyBranchTypes eAutocracy = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_AUTOCRACY);
		PolicyBranchTypes eOrder = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_ORDER);

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
	  }
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionOtherTraitValueTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
	int iValue = 0;
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL || (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL))
#else
	if (kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL)
#endif
	{
		if (bAsOriginPlayer)
		{
			iValue += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldChangeIncomingTradeRoute(eYield) * 100;
		}
	}
	else
	{
		iValue += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerTraits()->GetYieldChangeIncomingTradeRoute(eYield) * 100;
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionDomainValueModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes /*eYield*/)
{
	return GC.getGame().GetGameTrade()->GetDomainModifierTimes100(kTradeConnection.m_eDomain);
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionDistanceValueModifierTimes100(const TradeConnection& kTradeConnection) const
{
	if (!MOD_BALANCE_CORE_SCALING_TRADE_DISTANCE)
		return 0;

	if (m_pPlayer->GetPlayerTraits()->IsIgnoreTradeDistanceScaling())
		return 0;

	//distance scaling doesn't apply to internal trade (except gold internal trade)
#if defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (kTradeConnection.m_eDestOwner == kTradeConnection.m_eOriginOwner && kTradeConnection.m_eConnectionType != TRADE_CONNECTION_GOLD_INTERNAL)
#else
	if (kTradeConnection.m_eDestOwner == kTradeConnection.m_eOriginOwner)
#endif
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
					iModifier = /*25*/ GD_INT_GET(TRADE_ROUTE_RIVER_CITY_MODIFIER);
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
					iModifier = /*25*/ GD_INT_GET(TRADE_ROUTE_RIVER_CITY_MODIFIER);
				}
			}
		}
	}

	return iModifier;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionDiplomatModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield)
{
	if (!MOD_BALANCE_VP)
		return 0;

	int iModifier = 0;
	if (eYield == YIELD_GOLD)
	{
		CvCity* pOriginCity = CvGameTrade::GetOriginCity(kTradeConnection);
		CvCity* pDestCity = CvGameTrade::GetDestCity(kTradeConnection);
		CvAssert(pOriginCity != NULL);
		CvAssert(pDestCity != NULL);
		if (pOriginCity && pDestCity)
		{
			PlayerTypes eOriginPlayer = pOriginCity->getOwner();
			PlayerTypes eDestPlayer = pDestCity->getOwner();

			CvCity* pDestCapital = GET_PLAYER(eDestPlayer).getCapitalCity();
			if (pDestCapital && GET_PLAYER(eDestPlayer).isMajorCiv())
			{
				CvPlayerEspionage* pOriginPlayerEspionage = GET_PLAYER(eOriginPlayer).GetEspionage();
				if (pOriginPlayerEspionage && pOriginPlayerEspionage->IsAnySchmoozing(pDestCapital))
				{
					CvCityEspionage* pDestCapitalEspionage = pDestCapital->GetCityEspionage();
					if (pDestCapitalEspionage)
					{
						iModifier += pDestCapitalEspionage->GetDiplomatTradeBonus(eOriginPlayer);
					}
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
	int iModifier = 0;
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	// internal gold trade routes always get the full open border bonus
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
	{
		if (bAsOriginPlayer)
		{
			iModifier = /*20*/ GD_INT_GET(OPEN_BORDERS_MODIFIER_TRADE_GOLD);
		}
	}
#endif
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
			iModifier = /*20*/ GD_INT_GET(OPEN_BORDERS_MODIFIER_TRADE_GOLD);
		}
		else if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*10*/ (GD_INT_GET(OPEN_BORDERS_MODIFIER_TRADE_GOLD) / 2);
		}
	}
	else if(!bAsOriginPlayer && eOriginPlayer != NO_PLAYER && eDestPlayer != NO_PLAYER && eYield == YIELD_GOLD)
	{
		if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) && GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*10*/ (GD_INT_GET(OPEN_BORDERS_MODIFIER_TRADE_GOLD) / 2);
		}
		else if(GET_TEAM(GET_PLAYER(eOriginPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eDestPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eDestPlayer).getTeam()).IsAllowsOpenBordersToTeam(GET_PLAYER(eOriginPlayer).getTeam()))
		{
			iModifier = /*5*/ (GD_INT_GET(OPEN_BORDERS_MODIFIER_TRADE_GOLD) / 4);
		}
	}

	return iModifier;
}
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeConnectionCorporationModifierTimes100(const TradeConnection& kTradeConnection, YieldTypes eYield, bool bAsOriginPlayer)
{
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

	if (bAsOriginPlayer && pkCorporationInfo->GetTradeRouteMod((int)eYield) != 0)
	{
		int iOfficesAsFranchisesVal = GET_PLAYER(pOriginCity->getOwner()).GetCorporations()->GetCorporationOfficesAsFranchises();
		//If nationalized, we must be targetting our own city
		if (iOfficesAsFranchisesVal > 0 && pOriginCity->getOwner() == pDestCity->getOwner())
		{
			// Target only trade routes to cities with a Franchise - IsHasFranchise() also counts Nationalized Offices as franchises!
			if (pOriginCity->IsHasOffice() && pDestCity->IsHasFranchise(eCorporation))
			{
				iModifier += pkCorporationInfo->GetTradeRouteMod((int)eYield) * iOfficesAsFranchisesVal;
			}
		}
		// Do we have a building that grants a bonus toward cities with Franchises?
		else if (pDestCity->IsHasFranchise(eCorporation) && pOriginCity->IsHasOffice())
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
	// TODO: This switch should probably go in a function.
	switch (eYield)
	{
	case YIELD_FOOD:
	case YIELD_PRODUCTION:
	case YIELD_GOLD:
	case YIELD_SCIENCE:
	case YIELD_CULTURE:
	case YIELD_FAITH:
	case YIELD_TOURISM:
	case YIELD_GOLDEN_AGE_POINTS:
		break; // Yields applicable to trade.
	case NO_YIELD:
	case YIELD_GREAT_GENERAL_POINTS:
	case YIELD_GREAT_ADMIRAL_POINTS:
	case YIELD_POPULATION:
	case YIELD_CULTURE_LOCAL:
	case YIELD_JFD_HEALTH:
	case YIELD_JFD_DISEASE:
	case YIELD_JFD_CRIME:
	case YIELD_JFD_LOYALTY:
	case YIELD_JFD_SOVEREIGNTY:
		return 0; // Yields not applicable to trade.
	}

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
					int iResourceBonus = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? 0 : GetTradeConnectionResourceValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, eYield);
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);

					// Cultural influence bump
					int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eDestOwner);
					//Minor Civ Bump
					int iMinorCivGold = GetMinorCivGoldBonus(kTradeConnection, eYield, true);

					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					int iResourceModifier = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? GetTradeConnectionResourceValueTimes100(kTradeConnection, eYield, bAsOriginPlayer) : 0;
					int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iDiplomatModifier = GetTradeConnectionDiplomatModifierTimes100(kTradeConnection, eYield);
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);

#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#endif

					iValue = iBaseValue;
					iValue += iOriginPerTurnBonus;
					iValue += iDestPerTurnBonus;
					iValue += iResourceBonus;
					iValue += iExclusiveBonus;
					iValue += iYourBuildingBonus;
					iValue += iTheirBuildingBonus;
					iValue += iPolicyBonus;
					iValue += iTraitBonus;

					// Cultural influence bump
					iValue += iInfluenceBoost;
					iValue += iMinorCivGold;

					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += iResourceModifier;
					iModifier += iOriginRiverModifier;
					iModifier += iDiplomatModifier;
					iModifier += iCorporationModifier;
					iModifier += iOpenBordersModifier;

#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					iModifier += iPolicyModifier;
#endif

					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity != NULL)
					{
						if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);
					}

					iValue *= iModifier;
					iValue /= 100;
					iValue = max(100, iValue);
				}
				break;
			case YIELD_CULTURE:
			case YIELD_FAITH:
			case YIELD_TOURISM:
			case YIELD_GOLDEN_AGE_POINTS:
			case YIELD_SCIENCE:
				{
					int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);

					iValue = iBaseValue;
					iValue += iPolicyBonus;
					iValue += iTraitBonus;

					int iModifier = 100;
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					if (eYield == YIELD_CULTURE || eYield == YIELD_SCIENCE)
					{
						iDistanceModifier = 0;
					}
					iModifier -= iDistanceModifier;
					iModifier += iCorporationModifier;

#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					iModifier += iPolicyModifier;
#endif

					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity != NULL)
					{
						if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);
					}
				
					iValue *= iModifier;
					iValue /= 100;
				}
				break;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
/// TODO: Integrate modifier logic into the PRODUCTION and FOOD cases of international trade
#endif
			case YIELD_PRODUCTION:
				{
					if(MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsSiphoned(kTradeConnection.m_eOriginOwner))
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOriginPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, true);
						int iDestPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer, false);
						int iResourceBonus = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? 0 : GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, YIELD_GOLD);
						int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, YIELD_GOLD);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eDestOwner);
						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, YIELD_GOLD);
						int iResourceModifier = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer) : 0;
						int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						iValue = iBaseValue;
						iValue += iOriginPerTurnBonus;
						iValue += iDestPerTurnBonus;
						iValue += iResourceBonus;
						iValue += iExclusiveBonus;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iPolicyBonus;
						iValue += iTraitBonus;
						iValue += iInfluenceBoost;

						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iResourceModifier;
						iModifier += iOriginRiverModifier;
						iModifier += iCorporationModifier;
						iModifier += iOpenBordersModifier;

						CvCity* pOriginCity = NULL;
						CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
						if (pStartPlot)
						{
							pOriginCity = pStartPlot->getPlotCity();
						}
						if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);

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
						int iResourceBonus = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? 0 : GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, YIELD_GOLD);
						int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, YIELD_GOLD);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, YIELD_GOLD);
						int iResourceModifier = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? GetTradeConnectionResourceValueTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer) : 0;
						int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);
						int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, YIELD_GOLD, bAsOriginPlayer);

						iValue = iBaseValue;
						iValue += iOriginPerTurnBonus;
						iValue += iDestPerTurnBonus;
						iValue += iResourceBonus;
						iValue += iExclusiveBonus;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iPolicyBonus;
						iValue += iTraitBonus;
						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iResourceModifier;
						iModifier += iOriginRiverModifier;
						iModifier += iCorporationModifier;
						iModifier += iOpenBordersModifier;

						CvCity* pOriginCity = NULL;
						CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
						if (pStartPlot)
						{
							pOriginCity = pStartPlot->getPlotCity();
						}
						if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
							iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);

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
			default:
				UNREACHABLE();
			}
		}
		// Gold internal trade routes, note that only the origin city gets any yields
		else if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES && kTradeConnection.m_eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
		{
			switch (eYield)
			{
			case YIELD_GOLD:
				{
					int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iOriginPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, eYield, bAsOriginPlayer, true);
					int iDestPerTurnBonus = GetTradeConnectionGPTValueTimes100(kTradeConnection, eYield, bAsOriginPlayer, false);
					int iResourceBonus = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? 0 : GetTradeConnectionResourceValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iExclusiveBonus = GetTradeConnectionExclusiveValueTimes100(kTradeConnection, eYield);
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					//Minor Civ Bump
					int iMinorCivGold = GetMinorCivGoldBonus(kTradeConnection, eYield, true);

					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					int iResourceModifier = MOD_BALANCE_CORE_RESOURCE_MONOPOLIES ? GetTradeConnectionResourceValueTimes100(kTradeConnection, eYield, bAsOriginPlayer) : 0;
					int iOriginRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iOpenBordersModifier = GetTradeConnectionOpenBordersModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
#endif

					iValue = iBaseValue;
					iValue += iOriginPerTurnBonus;
					iValue += iDestPerTurnBonus;
					iValue += iResourceBonus;
					iValue += iExclusiveBonus;
					iValue += iYourBuildingBonus;
					iValue += iTheirBuildingBonus;
					iValue += iPolicyBonus;
					iValue += iTraitBonus;
					iValue += iMinorCivGold;
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += iResourceModifier;
					iModifier += iOriginRiverModifier;
					iModifier += iCorporationModifier;
					iModifier += iOpenBordersModifier;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					iModifier += iPolicyModifier;
#endif

					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

					iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);

					iValue *= iModifier;
					iValue /= 100;
				}
				break;
			case YIELD_CULTURE:
			case YIELD_FAITH:
			case YIELD_TOURISM:
			case YIELD_GOLDEN_AGE_POINTS:
			case YIELD_SCIENCE:
				{
					int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iPolicyBonus = GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);

					iValue = iBaseValue;
					iValue += iPolicyBonus;
					iValue += iTraitBonus;

					int iModifier = 100;
					int iCorporationModifier = GetTradeConnectionCorporationModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					if (eYield == YIELD_CULTURE || eYield == YIELD_SCIENCE)
					{
						iDistanceModifier = 0;
					}
					iModifier -= iDistanceModifier;
					iModifier += iCorporationModifier;

#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
					int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, bAsOriginPlayer);
					iModifier += iPolicyModifier;
#endif

					CvCity* pOriginCity = NULL;
					CvPlot* pStartPlot = GC.getMap().plot(kTradeConnection.m_iOriginX, kTradeConnection.m_iOriginY);
					if (pStartPlot)
					{
						pOriginCity = pStartPlot->getPlotCity();
					}
					if (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion())
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_CAPITAL_MODIFIER);

					iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MODIFIER);

					iValue *= iModifier;
					iValue /= 100;
				}
				break;

			case YIELD_FOOD:
			case YIELD_PRODUCTION:
				break;
			default:
				UNREACHABLE();
			}
		}
	}
	else
	{
		if (pTrade->IsConnectionInternational(kTradeConnection))
		{
			CvAssert(kTradeConnection.m_eConnectionType == TRADE_CONNECTION_INTERNATIONAL);
			if (kTradeConnection.m_eDestOwner == m_pPlayer->GetID())
			{
				switch (eYield)
				{
				case YIELD_GOLD:
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
						int iYourBuildingBonus = GetTradeConnectionYourBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
						int iTheirBuildingBonus = GetTradeConnectionTheirBuildingValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);

						// Cultural influence bump
						int iInfluenceBoost = GET_PLAYER(kTradeConnection.m_eDestOwner).GetCulture()->GetInfluenceTradeRouteGoldBonus(kTradeConnection.m_eOriginOwner);

						int iModifier = 100;
						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
						int iDestRiverModifier = GetTradeConnectionRiverValueModifierTimes100(kTradeConnection, eYield, false);
						int iDestDiplomatModifier = GetTradeConnectionDiplomatModifierTimes100(kTradeConnection, eYield);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
						int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, false /*bAsOriginPlayer*/);
#endif

						iValue = iBaseValue;
						iValue += iYourBuildingBonus;
						iValue += iTheirBuildingBonus;
						iValue += iTraitBonus;

						// Cultural influence bump
						iValue += iInfluenceBoost;

						iModifier -= iDistanceModifier;
						iModifier += iDomainModifier;
						iModifier += iDestRiverModifier;
						iModifier += iDestDiplomatModifier;
#if defined(HH_MOD_API_TRADEROUTE_MODIFIERS)
						iModifier += iPolicyModifier;
#endif

						iValue *= iModifier;
						iValue /= 100;
					}
					break;
				case YIELD_CULTURE:
				case YIELD_FAITH:
				case YIELD_TOURISM:
				case YIELD_GOLDEN_AGE_POINTS:
				case YIELD_SCIENCE:
					{
						int iBaseValue = GetTradeConnectionBaseValueTimes100(kTradeConnection, eYield, bAsOriginPlayer);
						int iTraitBonus = GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);

						int iModifier = 100;

						int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
						if (eYield == YIELD_CULTURE || eYield == YIELD_SCIENCE)
						{
							iDistanceModifier = 0;
						}
						int iPolicyModifier = GetTradeConnectionPolicyModifierTimes100(kTradeConnection, eYield, false /*bAsOriginPlayer*/);
						iModifier -= iDistanceModifier;
						iModifier += iPolicyModifier;

						iValue = iBaseValue;
						iValue += iTraitBonus;

						iValue *= iModifier;
						iValue /= 100;						
					}
					break;
				case YIELD_FOOD:
				case YIELD_PRODUCTION:
					break;
				default:
					UNREACHABLE();
				}
			}
		}
		else
		{
			CvAssert(kTradeConnection.m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL);
			switch (kTradeConnection.m_eConnectionType)
			{
			case TRADE_CONNECTION_FOOD:
				if (eYield == YIELD_FOOD)
				{
					iValue = /*300 in CP, 600 in VP*/ GD_INT_GET(TRADE_ROUTE_BASE_FOOD_VALUE);

					iValue += GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					iValue += GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
					iValue += GC.getEraInfo(GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra())->getTradeRouteFoodBonusTimes100();
					iValue *= GC.getEraInfo(GC.getGame().getStartEra())->getGrowthPercent();
					iValue /= 100;

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

					if (GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsConquestOfTheWorld() && GET_PLAYER(kTradeConnection.m_eDestOwner).isGoldenAge())
					{
						iValue *= 2;
					}

					if (pOriginCity != NULL)
					{
						iValue += pOriginCity->GetYieldFromInternalTR(YIELD_FOOD) * 100;
					}

					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_MODIFIER);
			
					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER);
					}

					iValue *= iModifier;
					iValue /= 100;
				}
				break;
			case TRADE_CONNECTION_PRODUCTION:
				if (eYield == YIELD_PRODUCTION)
				{
					iValue = /*300 in CP, 600 in VP*/ GD_INT_GET(TRADE_ROUTE_BASE_PRODUCTION_VALUE);

					iValue += GetTradeConnectionPolicyValueTimes100(kTradeConnection, eYield);
					iValue += GetTradeConnectionOtherTraitValueTimes100(kTradeConnection, eYield, false);
					iValue += GC.getEraInfo(GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra())->getTradeRouteProductionBonusTimes100();
					iValue *= (GC.getEraInfo(GC.getGame().getStartEra())->getConstructPercent() + GC.getEraInfo(GC.getGame().getStartEra())->getTrainPercent()) / 2;
					iValue /= 100;

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

					if (GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsConquestOfTheWorld() && GET_PLAYER(kTradeConnection.m_eDestOwner).isGoldenAge())
					{
						iValue *= 2;
					}

					if (pOriginCity != NULL)
					{
						iValue += pOriginCity->GetYieldFromInternalTR(YIELD_PRODUCTION) * 100;
					}

					int iModifier = 100;
					int iDistanceModifier = GetTradeConnectionDistanceValueModifierTimes100(kTradeConnection);
					int iDomainModifier = GetTradeConnectionDomainValueModifierTimes100(kTradeConnection, eYield);
					iModifier -= iDistanceModifier;
					iModifier += iDomainModifier;
					iModifier += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_MODIFIER);

					if (pOriginCity != NULL && (pOriginCity == GET_PLAYER(kTradeConnection.m_eOriginOwner).getCapitalCity() || pOriginCity->GetCityReligions()->IsHolyCityAnyReligion()))
					{
						iModifier += GET_PLAYER(kTradeConnection.m_eOriginOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_INTERNAL_TRADE_CAPITAL_MODIFIER);
					}

					iValue *= iModifier;
					iValue /= 100;
				}
				break;
			case TRADE_CONNECTION_WONDER_RESOURCE:
			case TRADE_CONNECTION_GOLD_INTERNAL:
				break;
			case TRADE_CONNECTION_INTERNATIONAL:
				UNREACHABLE(); // International trade route types should not be available along this branch.
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
	const vector<int>& vConnections = pTrade->GetTradeConnectionsForPlayer(pCity->getOwner());
	for (uint ui = 0; ui < vConnections.size(); ui++)
	{
		//should not happen?
		if (pTrade->IsTradeRouteIndexEmpty( vConnections[ui] ))
			continue;

		const TradeConnection& connection = pTrade->GetTradeConnection( vConnections[ui] );

		if (connection.m_eOriginOwner == m_pPlayer->GetID())
		{
			if (connection.m_iOriginX == iCityX && connection.m_iOriginY == iCityY)
			{
				iResult += connection.m_aiOriginYields[eYield];
			}
		}
		
		if (connection.m_eDestOwner == m_pPlayer->GetID())
		{
			if (connection.m_iDestX == iCityX && connection.m_iDestY == iCityY)
			{
				iResult += connection.m_aiDestYields[eYield];
			}
		}
	}

	return iResult;
}

//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetAllTradeValueTimes100 (YieldTypes eYield) const
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
int CvPlayerTrade::GetAllTradeValueFromPlayerTimes100 (YieldTypes eYield, PlayerTypes ePlayer) const
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

//  --------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeGPTLostFromWarTimes100(PlayerTypes ePlayer) const
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iGold = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pTrade->IsTradeRouteIndexEmpty(ui))
		{
			continue;
		}

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID() && pConnection->m_eDestOwner == ePlayer)
		{
			iGold += pConnection->m_aiOriginYields[YIELD_GOLD];
		}

		if (pConnection->m_eDestOwner == m_pPlayer->GetID() && pConnection->m_eOriginOwner == ePlayer)
		{
			iGold += pConnection->m_aiDestYields[YIELD_GOLD];
		}
	}

	return iGold;
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::IsConnectedToPlayer(PlayerTypes eOtherPlayer) const
{
	return GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), eOtherPlayer);
}


//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CanCreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType, bool bIgnoreExisting, bool bCheckPath /* = true */) const
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

	return pTrade->CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, eConnectionType, bIgnoreExisting, bCheckPath);
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::CanCreateTradeRoute(PlayerTypes eOtherPlayer, DomainTypes eDomain)
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		int iCityLoop2 = 0;
		CvCity* pLoopCity2 = NULL;
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
bool CvPlayerTrade::CanCreateTradeRoute(DomainTypes eDomain) const
{
	CvGameTrade* pGameTrade = GC.getGame().GetGameTrade();

	int iCityLoop = 0;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
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
bool CvPlayerTrade::CreateTradeRoute(CvCity* pOriginCity, CvCity* pDestCity, DomainTypes eDomain, TradeConnectionType eConnectionType) const
{
	int plotsX[MAX_PLOTS_TO_DISPLAY];
	int plotsY[MAX_PLOTS_TO_DISPLAY];

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
			pOriginCity->UpdateAllNonPlotYields(false);
			pDestCity->UpdateAllNonPlotYields(false);
#endif
		}
	}

	if (MOD_API_ACHIEVEMENTS && m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
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

	return true;
}

//	--------------------------------------------------------------------------------
const TradeConnection* CvPlayerTrade::GetTradeConnection(CvCity* pOriginCity, CvCity* pDestCity) const
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

void CvPlayerTrade::UpdateTradeStats()
{
	if (m_tradeStats.iTurnSliceBuilt == GC.getGame().getTurnSlice())
		return;

	m_tradeStats.reset();

	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection& connection = pTrade->GetTradeConnection(ui);
		if (!connection.isValid())
			continue;

		if (connection.m_eOriginOwner == m_pPlayer->GetID() && GET_PLAYER(connection.m_eDestOwner).isMinorCiv())
			m_tradeStats.iMinorTRs++;

		if (connection.m_eOriginOwner == m_pPlayer->GetID() && connection.m_eDestOwner == m_pPlayer->GetID())
			m_tradeStats.iInternalTRs++;

		if (connection.m_eOriginOwner == m_pPlayer->GetID() && connection.m_eDestOwner != m_pPlayer->GetID())
			m_tradeStats.iInternationalTRsOut++;

		if (connection.m_eOriginOwner != m_pPlayer->GetID() && connection.m_eDestOwner == m_pPlayer->GetID())
			m_tradeStats.iInternationalTRsIn++;

		if (connection.m_eOriginOwner == m_pPlayer->GetID())
			m_tradeStats.nRoutesFromCity[connection.m_iOriginID]++;

		if (connection.m_eDestOwner == m_pPlayer->GetID())
			m_tradeStats.nRoutesToCity[connection.m_iDestID]++;
	}

	m_tradeStats.iTurnSliceBuilt = GC.getGame().getTurnSlice();
	}

//Returns the number of city state trade routes connected to a city
int CvPlayerTrade::GetNumberOfCityStateTradeRoutes()
{
	UpdateTradeStats();
	return m_tradeStats.iMinorTRs;
}

int CvPlayerTrade::GetNumberOfCityStateTradeRoutesFromCity(CvCity* pCity) const
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iNumConnections = 0;
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (pConnection->m_eOriginOwner == m_pPlayer->GetID())
		{
			if (pConnection->m_iOriginX == pCity->getX() && pConnection->m_iOriginY == pCity->getY())
			{
				if (GET_PLAYER(pConnection->m_eDestOwner).isMinorCiv())
				{
					iNumConnections++;
				}
			}
		}
	}

	return iNumConnections;
}

int CvPlayerTrade::GetNumberOfTradeRoutesFromCity(const CvCity* pCity)
{
	if (!pCity)
		return 0;

	UpdateTradeStats();
	return m_tradeStats.nRoutesFromCity[pCity->GetID()];
}

int CvPlayerTrade::GetNumberOfTradeRoutesCity(const CvCity* pCity)
{
	if (!pCity)
		return 0;

	UpdateTradeStats();
	return m_tradeStats.nRoutesToCity[pCity->GetID()] + m_tradeStats.nRoutesFromCity[pCity->GetID()];
}

bool CvPlayerTrade::IsCityAlreadyConnectedByTrade(CvCity* pOtherCity) const
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	for (uint ui = 0; ui < pTrade->GetNumTradeConnections(); ui++)
	{
		const TradeConnection* pConnection = &(pTrade->GetTradeConnection(ui));

		if (!pConnection)
			continue;

		if (pConnection->m_eOriginOwner != m_pPlayer->GetID())
			continue;

		if (pConnection->m_iDestX == pOtherCity->getX() && pConnection->m_iDestY == pOtherCity->getY())
			return true;
	}

	return false;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
//Returns the number of internal trade routes in your empire
int CvPlayerTrade::GetNumberOfInternalTradeRoutes()
{
	UpdateTradeStats();
	return m_tradeStats.iInternalTRs;
}

//Returns the number of international trade routes to other empires
int CvPlayerTrade::GetNumberOfInternationalTradeRoutes(bool bOutgoing)
{
	UpdateTradeStats();
	return bOutgoing ? m_tradeStats.iInternationalTRsOut : m_tradeStats.iInternationalTRsIn;
}

//Returns the number of trade routes in your empire
int CvPlayerTrade::GetNumberOfTradeRoutes()
{
	UpdateTradeStats();
	return m_tradeStats.iInternalTRs + m_tradeStats.iInternationalTRsOut;
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

		int iLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			int iDistance = ::plotDistance(pFromCity->getX(), pFromCity->getY(), pLoopCity->getX(), pLoopCity->getY());
			vPossibleTargets.push_back(pLoopCity, 1000 - iDistance);
		}
	}

	//this should put the closest cities first
	vPossibleTargets.StableSortItems();

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
				else if (!m_pPlayer->isHuman() && m_pPlayer->GetDiplomacyAI()->IsPlayerBadTheftTarget(pConnection->m_eOriginOwner, THEFT_TYPE_TRADE_ROUTE, pPlot))
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
				else if (!m_pPlayer->isHuman() && m_pPlayer->GetDiplomacyAI()->IsPlayerBadTheftTarget(pConnection->m_eOriginOwner, THEFT_TYPE_TRADE_ROUTE, pPlot))
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
	return static_cast<bool>(aiTradeConnectionIDs.size() > 0);
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
	return static_cast<bool>(aiTradeConnectionIDs.size() > 0);
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
	return static_cast<bool>(aiTradeConnectionIDs.size() > 0);
}

//	--------------------------------------------------------------------------------
bool CvPlayerTrade::PlunderTradeRoute(int iTradeConnectionID, CvUnit* pUnit)
{
	CvGameTrade* pTrade = GC.getGame().GetGameTrade();
	int iTradeConnectionIndex = pTrade->GetIndexFromID(iTradeConnectionID);

	CvAssertMsg(iTradeConnectionIndex >= 0, "iTradeConnectionIndex < 0");
	if (iTradeConnectionIndex < 0)
		return false;

	const TradeConnection* pTradeConnection = &(pTrade->GetTradeConnection(iTradeConnectionIndex));
	if (!pTradeConnection->isValid())
		return false;

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

	CvCity* pOriginCity = GET_PLAYER(eOwningPlayer).getCity(pTradeConnection->m_iOriginID);
	CvCity* pDestCity = GET_PLAYER(eDestPlayer).getCity(pTradeConnection->m_iDestID);
	CvAssertMsg(pOriginCity, "pOriginCity doesn't exist");
	CvAssertMsg(pDestCity, "pDestCity doesn't exist");

	// if the trade route was not broken
	if (!pTrade->ClearTradeRoute(iTradeConnectionIndex))
		return false;

#if defined(MOD_BALANCE_CORE)
	if(pOriginCity != NULL && pDestCity != NULL)
	{
		pOriginCity->UpdateAllNonPlotYields(false);
		pDestCity->UpdateAllNonPlotYields(false);
	}
#endif

	int iPlunderGoldValue = /*100*/ GD_INT_GET(TRADE_ROUTE_BASE_PLUNDER_GOLD);
	iPlunderGoldValue *= 100 + iDomainModifier;
	iPlunderGoldValue /= 100;

#if defined(MOD_BALANCE_CORE)
	iPlunderGoldValue *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
	int iEra = m_pPlayer->GetCurrentEra();
	if (iEra <= 0)
		iEra = 1;
	iPlunderGoldValue *= iEra;
	if(pUnit && pUnit->isHighSeaRaiderUnit())
	{
		iPlunderGoldValue *= 3;
	}
	iPlunderGoldValue /= 100;
#endif
	m_pPlayer->GetTreasury()->ChangeGold(iPlunderGoldValue);

	// do the floating popup
	if (GC.getGame().getActivePlayer() == m_pPlayer->GetID())
	{
		char text[256] = {0};
		sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iPlunderGoldValue);
		SHOW_PLOT_POPUP(pPlunderPlot, m_pPlayer->GetID(), text);

		CvString strBuffer;
		strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPlunderGoldValue, GC.getUnitInfo(GetTradeUnit(eDomain, m_pPlayer))->GetDescriptionKey());
		
		DLLUI->AddMessage(0, m_pPlayer->GetID(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);
	}

	// barbarians get a bonus unit out of the deal!
	if (pUnit->isBarbarian())
	{
		int iNumExtraUnits = /*1*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PLUNDER_TRADE_ROUTE_SPAWN);
		int iGameEra = GC.getGame().getCurrentEra();
		iNumExtraUnits += GC.getGame().isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PLUNDER_TRADE_ROUTE_SPAWN_CHILL) : 0;
		iNumExtraUnits += GC.getGame().isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PLUNDER_TRADE_ROUTE_SPAWN_RAGING) : 0;
		if (iGameEra > 0)
			iNumExtraUnits += iGameEra * /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PLUNDER_TRADE_ROUTE_SPAWN_PER_ERA) / 100;

		CvBarbarians::SpawnBarbarianUnits(pPlunderPlot, iNumExtraUnits, BARB_SPAWN_PLUNDERED_TRADE_ROUTE);
	}

	if (eOwningPlayer != NO_PLAYER && m_pPlayer->isMajorCiv() && GET_PLAYER(eOwningPlayer).isMajorCiv())
	{
		if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(eOwningTeam))
		{
			if (iPlunderGoldValue > 0)
				m_pPlayer->ApplyWarDamage(eOwningPlayer, iPlunderGoldValue);

			m_pPlayer->GetDiplomacyAI()->ChangeWarProgressScore(eOwningPlayer, /*10*/ GD_INT_GET(WAR_PROGRESS_PLUNDERED_TRADE_ROUTE));
			GET_PLAYER(eOwningPlayer).GetDiplomacyAI()->ChangeWarProgressScore(m_pPlayer->GetID(), /*-5*/ GD_INT_GET(WAR_PROGRESS_LOST_TRADE_ROUTE));

			// Diplo penalty with destination civ if not at war
			if (eOwningPlayer != eDestPlayer && !GET_TEAM(m_pPlayer->getTeam()).isAtWar(eDestTeam) && !GET_TEAM(eDestTeam).IsVassal(m_pPlayer->getTeam()))
			{
				GET_PLAYER(eDestPlayer).GetDiplomacyAI()->ChangeNumTradeRoutesPlundered(m_pPlayer->GetID(), 1);
			}
		}
		else if (m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar())
		{
			// Diplo penalty with owner
			if (pPlunderPlot->isVisible(eOwningTeam))
			{
				GET_PLAYER(eOwningPlayer).GetDiplomacyAI()->ChangeNumTradeRoutesPlundered(m_pPlayer->GetID(), 3);
			}
			// Diplo penalty with destination civ if not at war
			if (eOwningPlayer != eDestPlayer && pPlunderPlot->isVisible(eDestTeam) && !GET_TEAM(m_pPlayer->getTeam()).isAtWar(eDestTeam))
			{
				GET_PLAYER(eDestPlayer).GetDiplomacyAI()->ChangeNumTradeRoutesPlundered(m_pPlayer->GetID(), 1);
			}
		}
	}

	// do the notification stuff
	if (pOriginCity && pDestCity)
	{
		bool bHumanPlunderer = m_pPlayer->isHuman();
		
		// send notification to owner player
		CvNotifications* pNotifications = GET_PLAYER(eOwningPlayer).GetNotifications();
		if(pNotifications)
		{
			Localization::String strSummary;
			Localization::String strMessage;

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_UNIT_PLUNDERED_TRADER_SUMMARY");
			if (m_pPlayer->isBarbarian() || (!bHumanPlunderer && m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar() && !pPlunderPlot->isVisible(eOwningTeam)))
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
				if (m_pPlayer->isBarbarian() || (!bHumanPlunderer && !GET_TEAM(eDestTeam).isAtWar(m_pPlayer->getTeam()) && m_pPlayer->GetPlayerTraits()->IsCanPlunderWithoutWar() && !pPlunderPlot->isVisible(eDestTeam)))
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

	if (MOD_API_ACHIEVEMENTS && eDomain == DOMAIN_LAND && m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP2_28);
	}

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
			if(iPlotX != -1 && iPlotY != -1)
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
	// don't calculate trade routes for minors
	if (m_pPlayer->isMinorCiv())
		return;

	int iLongestRoute = 0;
	CvString strMsg;

	/*
	if (GC.getLogging() && GC.getAILogging())
	{
		strMsg.Format("%s,,%d,Finding the longest trade route", pOriginCity->getNameKey(), iMaxRange * SPath::getNormalizedDistanceBase());
		LogTradeMsg(strMsg);
	}
	*/

	// Check the route, but not the path
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
	{
		int iCity = 0;
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		for (CvCity* pDestCity = kLoopPlayer.firstCity(&iCity); pDestCity != NULL; pDestCity = kLoopPlayer.nextCity(&iCity))
		{
			if (CanCreateTradeRoute(pOriginCity, pDestCity, eDomain, TRADE_CONNECTION_INTERNATIONAL, false, false))
			{
				SPath outPath;
				int iLength = GC.getGame().GetGameTrade()->GetValidTradeRoutePathLength(pOriginCity, pDestCity, eDomain, &outPath);

				if (iLength <= 0)
					continue;

				if (iLength > iLongestRoute)
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						strMsg.Format("%s,%s,,New longest trade route,%d", pOriginCity->getNameKey(), pDestCity->getNameKey(), iLength);
						LogTradeMsg(strMsg);

						/*
						strMsg.Format("%s,%s,,Logging the whole path", pOriginCity->getNameKey(), pDestCity->getNameKey());
						LogTradeMsg(strMsg);
						for (std::vector<SPathNode>::iterator it = outPath.vPlots.begin(); it != outPath.vPlots.end(); ++it)
						{
							strMsg.Format(
								"%s,%s,,X:%d / Y:%d / Moves:%d / Turns:%d",
								pOriginCity->getNameKey(),
								pDestCity->getNameKey(),
								it->x,
								it->y,
								it->moves,
								it->turns
							);
							LogTradeMsg(strMsg);
						}
						*/
					}

					iLongestRoute = iLength;
					if (iLongestRoute >= iMaxRange * SPath::getNormalizedDistanceBase())
						break;
				}
			}
		}
	}
	pOriginCity->SetLongestPotentialTradeRoute(iLongestRoute == 0 ? iMaxRange : iLongestRoute, eDomain);
}
//	--------------------------------------------------------------------------------
int CvPlayerTrade::GetTradeRouteRange (DomainTypes eDomain, CvCity* pOriginCity) const
{
	int iRange = 0;
	int iBaseRange = 0;
	switch (eDomain)
	{
	case DOMAIN_SEA:
		iBaseRange = /*20*/ GD_INT_GET(TRADE_ROUTE_BASE_SEA_DISTANCE);
		break;
	case DOMAIN_LAND:
		iBaseRange = /*10*/ GD_INT_GET(TRADE_ROUTE_BASE_LAND_DISTANCE);
		break;
	default:
		UNREACHABLE(); // Not a trade domain.
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
	default:
		UNREACHABLE(); // Not a trade domain.
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
		default:
			UNREACHABLE(); // Not a trade domain.
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
int CvPlayerTrade::GetTradeRouteSpeed (DomainTypes eDomain) const
{
#if defined(MOD_TRADE_ROUTE_SCALING)
	UnitTypes eUnitType = GetTradeUnit(eDomain, m_pPlayer);
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
int CvPlayerTrade::GetTradeRouteTurnMod(CvCity* pOriginCity) const
{
	int iTurnChange = 0;

	const CvCivilizationInfo& kCivInfo = m_pPlayer->getCivilizationInfo();
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		bool bSameCity = pLoopCity == pOriginCity;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = NO_BUILDING;

			if (MOD_BUILDINGS_THOROUGH_PREREQUISITES)
			{
				eBuilding = pLoopCity->GetCityBuildings()->GetBuildingTypeFromClass((BuildingClassTypes)iI);
			}
			else
			{
				eBuilding = (BuildingTypes)kCivInfo.getCivilizationBuildings((BuildingClassTypes)iI);
			}
			if (eBuilding != NO_BUILDING)
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
						int iTurnMod = pBuildingEntry->GetTRTurnModGlobal();
						if (iTurnMod != 0)
						{
							iTurnChange += (iTurnMod * pLoopCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)pBuildingEntry->GetID()));
						}
						int iTurnModLocal = pBuildingEntry->GetTRTurnModLocal();
						if (iTurnModLocal != 0 && bSameCity)
						{
							iTurnChange += (iTurnModLocal * pLoopCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)pBuildingEntry->GetID()));
						}
					}
				}
			}
		}
	}
	if (abs(iTurnChange) > 100) {
		return 100 * (iTurnChange / abs(iTurnChange));
	}
	return iTurnChange;
}

//	--------------------------------------------------------------------------------
uint CvPlayerTrade::GetNumTradeRoutesPossible (void) const
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

	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		const std::vector<BuildingTypes>& vBuildings = pLoopCity->GetCityBuildings()->GetAllBuildingsHere();
		for (size_t i = 0; i < vBuildings.size(); i++)
		{
			CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(vBuildings[i]);
			if (pBuildingEntry)
			{
				if (pLoopCity->GetCityBuildings()->GetNumBuilding((BuildingTypes)pBuildingEntry->GetID()))
				{
					int iNumRouteBonus = pBuildingEntry->GetNumTradeRouteBonus();
					if (iNumRouteBonus != 0)
					{
						iNumRoutes += iNumRouteBonus * pLoopCity->GetCityBuildings()->GetNumBuilding(vBuildings[i]);
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

	int iExtraTradeRoutesPerXOwnedCities = m_pPlayer->GetPlayerTraits()->GetExtraTradeRoutesPerXOwnedCities();
	if (iExtraTradeRoutesPerXOwnedCities > 0) // someone might put a negative value, but per negative number of cities doesn't make sense, so we ignore those cases
	{
		int iNumCities = m_pPlayer->getNumCities();
		if (iNumCities > 0)
		{
			iNumRoutes += iNumCities / iExtraTradeRoutesPerXOwnedCities;
		}
	}

	int iExtraTradeRoutesPerXOwnedVassals = m_pPlayer->GetPlayerTraits()->GetExtraTradeRoutesPerXOwnedVassals();
	if (iExtraTradeRoutesPerXOwnedVassals > 0) // someone might put a negative value, but per negative number of cities doesn't make sense, so we ignore those cases
	{
		int iNumVassals = GET_TEAM(m_pPlayer->getTeam()).GetNumVassals();
		if (iNumVassals > 0)
		{
			iNumRoutes += iNumVassals / iExtraTradeRoutesPerXOwnedVassals;
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
int CvPlayerTrade::GetNumTradeUnits(bool bIncludeBeingBuilt) const
{
	int iReturnValue = 0;
	int iLoop = 0;

	// look for units on the map
	CvUnit* pLoopUnit = NULL;
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
		CvCity* pLoopCity = NULL;
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
int CvPlayerTrade::GetNumDifferentTradingPartners (void) const
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
int CvPlayerTrade::GetNumDifferentMajorCivTradingPartners(void) const
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
	int iTurnsUntilForget = /*30*/ GD_INT_GET(TRADE_ROUTE_PLUNDER_TURNS_COUNTER);
	iTurnsUntilForget = iTurnsUntilForget * GC.getGame().getGameSpeedInfo().getTradeRouteSpeedMod() / 100;

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
UnitTypes CvPlayerTrade::GetTradeUnit (DomainTypes eDomain, CvPlayer* pPlayer)
{
	UnitTypes eUnitType = NO_UNIT;
	if (eDomain == DOMAIN_LAND)
	{
		eUnitType = pPlayer->GetSpecificUnitType("UNITCLASS_CARAVAN");
	}
	else if (eDomain == DOMAIN_SEA)
	{
		eUnitType = pPlayer->GetSpecificUnitType("UNITCLASS_CARGO_SHIP");
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
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain, &GET_PLAYER(pConnection->m_eOriginOwner)))->GetDescription();
						}
						else
						{
							strLine = Localization::Lookup("TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV");
							strLine << GET_PLAYER(pConnection->m_eOriginOwner).getCivilizationAdjectiveKey();
							strLine << GC.getUnitInfo(GetTradeUnit(pConnection->m_eDomain, &GET_PLAYER(pConnection->m_eOriginOwner)))->GetDescription();
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

template<typename TradeConnectionWasPlunderedT, typename Visitor>
void TradeConnectionWasPlundered::Serialize(TradeConnectionWasPlunderedT& tradeConnectionWasPlundered, Visitor& visitor)
{
	visitor(tradeConnectionWasPlundered.m_kTradeConnection);
	visitor(tradeConnectionWasPlundered.m_iTurnPlundered);
}

FDataStream& operator<<(FDataStream& saveTo, const TradeConnectionWasPlundered& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	TradeConnectionWasPlundered::Serialize(readFrom, serialVisitor);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, TradeConnectionWasPlundered& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	TradeConnectionWasPlundered::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

template<typename PlayerTrade, typename Visitor>
void CvPlayerTrade::Serialize(PlayerTrade& playerTrade, Visitor& visitor)
{
	visitor(playerTrade.m_aRecentlyExpiredConnections);
	visitor(playerTrade.m_aTradeConnectionWasPlundered);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerTrade& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvPlayerTrade::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerTrade& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvPlayerTrade::Serialize(readFrom, serialVisitor);
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
	int iOriginCityLoop = 0;
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

			int iDestCityLoop = 0;
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
CvTradeAI::TRSortElement CvTradeAI::ScoreInternationalTR(const TradeConnection& kTradeConnection, bool bHaveTourism) const
{
	TRSortElement ret;
	ret.m_kTradeConnection = kTradeConnection;

	// don't evaluate other trade types
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL)
		return ret;

	// if this was recently plundered, 0 the score
	if (m_pPlayer->GetTrade()->CheckTradeConnectionWasPlundered(kTradeConnection))
		return ret;

	// don't send trade routes if we're about to declare war?
	if (m_pPlayer->getFirstOffensiveAIOperation(kTradeConnection.m_eDestOwner) != NULL)
		return ret;

	if (m_pPlayer->GetDiplomacyAI()->AvoidExchangesWithPlayer(kTradeConnection.m_eDestOwner, true))
		return ret;

	CvCity* pToCity = CvGameTrade::GetDestCity(kTradeConnection);
	CvCity* pFromCity = CvGameTrade::GetOriginCity(kTradeConnection);

	if (!pToCity || !pFromCity)
		return ret;

	if (pToCity->isUnderSiege() || pFromCity->isUnderSiege())
		return ret;

	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();
	CvPlayerTrade* pOtherPlayerTrade = GET_PLAYER(kTradeConnection.m_eDestOwner).GetTrade();

	// gold
	int iGoldAmount = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, true);

#if defined(MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	int iPlayerEra = MAX((int)m_pPlayer->GetCurrentEra(), 1);
	int iOtherPlayerEra = MAX((int)GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra(), 1);

	if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	{
		int iTraitGold = 0;
		
		iTraitGold += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_GOLD, true) * iPlayerEra;
		iTraitGold += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_GOLD, false) * iPlayerEra;
		iTraitGold += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_GOLD, true) * iPlayerEra;

		// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
		iGoldAmount += iTraitGold / 2;
	}
#endif

	//emphasize gold if we're in the red
	int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	if(iGPT < -1)
		iGoldAmount *= (int)sqrt((float)-iGPT);

	// if a city is impoverished, let's send trade routes from there
	if (MOD_BALANCE_VP)
	{
		int iPoverty = pFromCity->GetPoverty(false);
		if (iPoverty > 0)
		{
			iGoldAmount += iPoverty * 10;
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
					if (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetCivLowestInfluence(false) == kTradeConnection.m_eDestOwner)
					{
						iGoldAmount += pFromCity->GetLandTourismBonus() * 100;
					}
					else if (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTrend(kTradeConnection.m_eDestOwner) >= INFLUENCE_TREND_RISING)
					{
						iGoldAmount += pFromCity->GetLandTourismBonus() * 500;
					}
					else
					{
						iGoldAmount += pFromCity->GetLandTourismBonus() * 250;
					}
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
					if (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetCivLowestInfluence(false) == kTradeConnection.m_eDestOwner)
					{
						iGoldAmount += pFromCity->GetSeaTourismBonus() * 1000;
					}
					else if (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCulture()->GetInfluenceTrend(kTradeConnection.m_eDestOwner) >= INFLUENCE_TREND_RISING)
					{
						iGoldAmount += pFromCity->GetSeaTourismBonus() * 500;
					}
					else
					{
						iGoldAmount += pFromCity->GetSeaTourismBonus() * 250;
					}
				}
			}
		}
	}

	int iOtherGoldAmount = pOtherPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, false);

	if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
	{
		int iTraitGold = 0;

		iTraitGold += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_GOLD, true) * iOtherPlayerEra;
		iTraitGold += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_GOLD, false) * iOtherPlayerEra;

		// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
		iOtherGoldAmount += iTraitGold / 2;
	}

	if (GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
	{
		iOtherGoldAmount = 0;
	}

	//If we are friends with the player, let's not care about how much gold they make.
	if (m_pPlayer->GetDiplomacyAI()->GetCivApproach(kTradeConnection.m_eDestOwner) >= CIV_APPROACH_AFRAID)
	{
		iOtherGoldAmount /= 10;
	}
	else if (m_pPlayer->GetDiplomacyAI()->GetCivOpinion(kTradeConnection.m_eDestOwner) >= CIV_OPINION_NEUTRAL)
	{
		iOtherGoldAmount /= 10;
	}

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
		int iCeilTechDifference = iTechDifferenceP1fromP2 * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);

		iAdjustedTechDifferenceP1fromP2 = max(iCeilTechDifference, 1);

		//if we get extra yields from sending trade routes to foreign territory, let's consider that
		if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
		{
			int iTraitScience = 0;

			iTraitScience += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_SCIENCE, true); // no era scaling here, because it is done below
			iTraitScience += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_SCIENCE, false);
			iTraitScience += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_SCIENCE, true) * iPlayerEra;

			// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
			iAdjustedTechDifferenceP1fromP2 += iTraitScience / 2;
		}

		if (iAdjustedTechDifferenceP1fromP2 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedTechDifferenceP1fromP2 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}

		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedTechDifferenceP1fromP2 += iPolicyBump;
	}

	//if a city is illiterate, let's send trade routes from there (add based on amount of unhappiness).
	if (MOD_BALANCE_VP)
	{
		int iIlliteracy = pFromCity->GetIlliteracy(false);
		if (iIlliteracy > 0)
		{
			iAdjustedTechDifferenceP1fromP2 += iIlliteracy;
		}
	}

	int iTechDifferenceP2fromP1 = GC.getGame().GetGameTrade()->GetTechDifference(kTradeConnection.m_eDestOwner,   kTradeConnection.m_eOriginOwner);
	int iAdjustedTechDifferenceP2fromP1 = 0;
	if (iTechDifferenceP2fromP1 > 0)
	{
		int iCeilTechDifference = iTechDifferenceP2fromP1 * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);

		iAdjustedTechDifferenceP2fromP1 = max(iCeilTechDifference, 1);

		if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
		{
			int iTraitScience = 0;

			iTraitScience += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_SCIENCE, true); // no era scaling here, because it is done below
			iTraitScience += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_SCIENCE, false);

			// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
			iAdjustedTechDifferenceP2fromP1 += iTraitScience / 2;
		}

		if (iTechDifferenceP2fromP1 > 0 && (GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra() > 0))
		{
			iAdjustedTechDifferenceP2fromP1 *= GET_PLAYER(kTradeConnection.m_eDestOwner).GetCurrentEra();
		}

		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eOriginOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eDestOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedTechDifferenceP2fromP1 += iPolicyBump;
	}

	//If we are friends with the player, let's not care about how much science they make.
	if(m_pPlayer->GetDiplomacyAI()->GetCivApproach(kTradeConnection.m_eDestOwner) >= CIV_APPROACH_NEUTRAL)
	{
		iAdjustedTechDifferenceP2fromP1 /= 3;
	}
	else if(m_pPlayer->GetDiplomacyAI()->GetCivOpinion(kTradeConnection.m_eDestOwner) >= CIV_OPINION_NEUTRAL)
	{
		iAdjustedTechDifferenceP2fromP1 /= 3;
	}

	int iTechDelta = iAdjustedTechDifferenceP1fromP2 - iAdjustedTechDifferenceP2fromP1;

	// culture
	int iCultureDifferenceP1fromP2 = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eOriginOwner, kTradeConnection.m_eDestOwner);
	
	int iAdjustedCultureDifferenceP1fromP2 = 0;
	if (iCultureDifferenceP1fromP2 > 0)
	{
		int iCeilCultureDifference = iCultureDifferenceP1fromP2 * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
		iAdjustedCultureDifferenceP1fromP2 = max(iCeilCultureDifference, 1);

		//if we get extra yields from sending trade routes to foreign territory, let's consider that
		if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
		{
			int iTraitCulture = 0;

			iTraitCulture += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_CULTURE, true); // no era scaling here, because it is done below
			iTraitCulture += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_CULTURE, false);
			iTraitCulture += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_CULTURE, true) * iPlayerEra;

			// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
			iAdjustedCultureDifferenceP1fromP2 += iTraitCulture / 2;
		}

		if (iAdjustedCultureDifferenceP1fromP2 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedCultureDifferenceP1fromP2 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
			
		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedCultureDifferenceP1fromP2 += iPolicyBump;
	}

	//if a city is bored, let's send trade routes from there (add based on amount of unhappiness).
	if (MOD_BALANCE_VP)
	{
		int iBoredom = pFromCity->GetBoredom(false);
		if (iBoredom > 0)
		{
			iAdjustedCultureDifferenceP1fromP2 += iBoredom;
		}
	}

	int iCultureDifferenceP2fromP1 = GC.getGame().GetGameTrade()->GetPolicyDifference(kTradeConnection.m_eDestOwner, kTradeConnection.m_eOriginOwner);
	int iAdjustedCultureDifferenceP2fromP1 = 0;
	if (iCultureDifferenceP2fromP1 > 0)
	{
		int iCeilCultureDifference = iCultureDifferenceP2fromP1 * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
		iAdjustedCultureDifferenceP2fromP1 = max(iCeilCultureDifference, 1);

		if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
		{
			int iTraitCulture = 0;

			iTraitCulture += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_CULTURE, true); // no era scaling here, because it is done below
			iTraitCulture += GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(YIELD_CULTURE, false);

			// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
			iAdjustedCultureDifferenceP2fromP1 += iTraitCulture / 2;
		}

		if (iAdjustedCultureDifferenceP2fromP1 > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedCultureDifferenceP2fromP1 *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
				
		// Policy bump
		int iPolicyBump = GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() ? 0 : GET_PLAYER(kTradeConnection.m_eOriginOwner).GetExtraCultureandScienceTradeRoutes();

		iAdjustedCultureDifferenceP2fromP1 += iPolicyBump;
	}

	//If we are friends with the player, let's not care about how much science they make.
	if (m_pPlayer->GetDiplomacyAI()->GetCivApproach(kTradeConnection.m_eDestOwner) >= CIV_APPROACH_NEUTRAL)
	{
		iAdjustedCultureDifferenceP2fromP1 /= 3;
	}
	else if (m_pPlayer->GetDiplomacyAI()->GetCivOpinion(kTradeConnection.m_eDestOwner) >= CIV_OPINION_NEUTRAL)
	{
		iAdjustedCultureDifferenceP2fromP1 /= 3;
	}

	int iCultureDelta = iAdjustedCultureDifferenceP1fromP2 - iAdjustedCultureDifferenceP2fromP1;

	// religion
	ReligionTypes eOwnerStateReligion = m_pPlayer->GetReligions()->GetStateReligion();
	int iReligionDelta = 0;
	if (eOwnerStateReligion != NO_RELIGION)
	{
		ReligionTypes eToReligion = NO_RELIGION;
		int iToPressure = 0;
		ReligionTypes eFromReligion = NO_RELIGION;
		int iFromPressure = 0;
		bool bAnyFromCityPressure = pFromCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pToCity, eToReligion, iToPressure);
		bool bAnyToCityPressure = pToCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pFromCity, eFromReligion, iFromPressure);

		// Internally pressure is now 10 times greater than what is shown to user
		iToPressure /= /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
		iFromPressure /= /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);

		// if anyone is exerting pressure
		if (bAnyFromCityPressure || bAnyToCityPressure)
		{
			// "to" and "from" religions need to be different for us to care
			if (eToReligion != eFromReligion)
			{
				int iExistingToPressureAtFrom = pFromCity->GetCityReligions()->GetPressureAccumulated(eFromReligion);
				int iExistingGoodPressureAtTo = pToCity->GetCityReligions()->GetPressureAccumulated(eOwnerStateReligion);
				if (eToReligion != eOwnerStateReligion)
					iToPressure = 0;
				if (eFromReligion == eOwnerStateReligion)
					iFromPressure = 0;
				double dExistingPressureModFrom = sqrt(log((double)MAX(1, iExistingToPressureAtFrom + iFromPressure) / (double)MAX(1, iExistingToPressureAtFrom)) / log(2.));
				double dExistingPressureModTo = sqrt(log((double)MAX(1, iExistingGoodPressureAtTo + iToPressure) / (double)MAX(1, iExistingGoodPressureAtTo)) / log(2.));

				iReligionDelta += int(iToPressure * dExistingPressureModTo / /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER) + 0.5);
				iReligionDelta -= int(iFromPressure * dExistingPressureModFrom / /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER) + 0.5);
			}
		}
	}

	int iFlavorGold = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
	int iFlavorScience = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
	int iFlavorReligion = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
	int iFlavorCulture = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
	int iGoldScore = iGoldDelta / max(1, (10-iFlavorGold));
	int iScienceScore = (iTechDelta *  iFlavorScience);
	int iCultureScore = (iCultureDelta * iFlavorCulture);
	int iReligionScore = (iReligionDelta *  (iFlavorReligion / 2));

	//add it all up
	int iScore = iGoldScore + iScienceScore + iCultureScore + iReligionScore;

	if (GET_PLAYER(kTradeConnection.m_eDestOwner).isMajorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
	{
		iScore *= 5;
	}

	//abort if we're negative
	if(iScore <= 0)
		return ret;

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

		//If we get a bonus from sending trade routes to foreign territory, let's send an international route!
		if (MOD_TRAITS_YIELD_FROM_ROUTE_MOVEMENT_IN_FOREIGN_TERRITORY)
		{
			YieldTypes eYieldLoop = (YieldTypes)iJ;
			if (eYieldLoop != YIELD_GOLD && eYieldLoop != YIELD_SCIENCE && eYieldLoop != YIELD_CULTURE) // We already checked these yield types above with a more sophisticated routine
			{
				int iTempScore = 0;
				iTempScore += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(eYieldLoop, true);
				iTempScore += m_pPlayer->GetPlayerTraits()->GetYieldFromRouteMovementInForeignTerritory(eYieldLoop, false);

				// Era scaler here because the instant yield scales with era
				// We divide by 2 here, because the trade unit spends half of its time in foreign territory (rough estimate)
				iScore += iTempScore * iPlayerEra / 2;
			}
		}
	}

	//Check if we can siphon production from the target city
	if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && m_pPlayer->GetPlayerTraits()->IsTradeRouteProductionSiphon())
	{
		int iSiphonPercent = m_pPlayer->GetTradeRouteProductionSiphonPercent(false, &GET_PLAYER(kTradeConnection.m_eDestOwner));
		iSiphonPercent += m_pPlayer->GetTradeRouteProductionSiphonPercent(true, &GET_PLAYER(kTradeConnection.m_eDestOwner));

		//Choose cities with high production
		//AI cheats here, because they can directly check the target city's production
		//Multiply with 2 for a bit more weight here
		iScore += pToCity->getBaseYieldRate(YIELD_PRODUCTION) * 2 * iSiphonPercent / 100;
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

		//danger is hard to quantify for routes which be definition pass by a lot of invisible plots
		if (!pPlot->isVisible(m_pPlayer->getTeam()))
			iDangerSum += 1;

		if (pPlot->getTeam() && m_pPlayer->getTeam())
			iDangerSum += 10;

		if (pPlot->getTeam() != NO_TEAM && GET_TEAM(m_pPlayer->getTeam()).isAtWar(pPlot->getTeam()))
			iDangerSum += 100;

		if (m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pPlot))
			iDangerSum += 100;
	}

	int iEra = max(1, (int)m_pPlayer->GetCurrentEra()); // More international trade late game, please.
	iScore = (iScore * iEra) / iDangerSum;

	//Let's encourage TRs to feitorias.
	if (MOD_BALANCE_CORE_PORTUGAL_CHANGE && GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv() && GET_PLAYER(kTradeConnection.m_eDestOwner).GetMinorCivAI()->IsSiphoned(m_pPlayer->GetID()))
	{
		iScore *= 10;
	}

	// if we have any tourism and the destination owner is not a minor civ
	if (bHaveTourism && !GET_PLAYER(kTradeConnection.m_eDestOwner).isMinorCiv())
	{
		// if we're not connected to a player, double the value to that player
		if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner))
		{
			if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory())
			{
				iScore *= 5;
			}
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
			int iCityLoop = 0;
			CvCity* pLoopCity = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					if(pLoopCity->GetCityBuildings()->GetCityStateTradeRouteProductionModifier() > 0)
					{
						if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), kTradeConnection.m_eDestOwner))
						{
							iScore *= 25;
						}
					}
				}
			}
			if(m_pPlayer->GetEventTourismCS() > 0)
			{
				iScore *= (m_pPlayer->GetEventTourismCS() + m_pPlayer->GetEventTourismCS());
			}
		}
		if (m_pPlayer->GetCorporations()->CanCreateFranchiseInCity(pFromCity, pToCity))
		{
			CorporationTypes eCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();

			CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
			if (pkCorporationInfo != NULL)
			{
				int iGPYieldFromCorp = pFromCity->GetGPRateModifierPerXFranchises();
				if (iGPYieldFromCorp > 0)
				{
					iScore *= (iGPYieldFromCorp * 5);
				}
				BuildingClassTypes eOffice = pkCorporationInfo->GetOfficeBuildingClass();
				if (eOffice != NO_BUILDINGCLASS)
				{
					CvBuildingClassInfo* pInfo = GC.getBuildingClassInfo(eOffice);
					if (pInfo)
					{
						BuildingTypes eOfficeBuilding = NO_BUILDING;

						if (MOD_BUILDINGS_THOROUGH_PREREQUISITES)
						{
							if (pFromCity->HasBuildingClass(eOffice))
							{
								eOfficeBuilding = pFromCity->GetCityBuildings()->GetBuildingTypeFromClass(eOffice);
							}
							else
							{
								eOfficeBuilding = (BuildingTypes)GET_PLAYER(pFromCity->getOwner()).getCivilizationInfo().getCivilizationBuildings(eOffice);
							}
						}
						else
						{
							eOfficeBuilding = (BuildingTypes)GET_PLAYER(pFromCity->getOwner()).getCivilizationInfo().getCivilizationBuildings(eOffice);
						}
						if (eOfficeBuilding != NO_BUILDING)
						{
							CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eOfficeBuilding);
							for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
							{
								if (pkBuildingInfo->GetYieldPerFranchise(iYield) > 0)
								{
									iScore *= (pkBuildingInfo->GetYieldPerFranchise(iYield) * 5);
								}
							}
						}
					}
				}
				if (m_pPlayer->GetCulture()->GetInfluenceLevel(pToCity->getOwner()) >= INFLUENCE_LEVEL_POPULAR && m_pPlayer->GetCorporations()->GetCorporationFreeFranchiseAbovePopular() != 0)
				{
					iScore *= 10;
				}
			}
		}
		//Franchised? Do we get a bonus for sending TRs here again?
		else if (m_pPlayer->GetCorporations()->GetFoundedCorporation() != NO_CORPORATION && pToCity->IsHasFranchise(m_pPlayer->GetCorporations()->GetFoundedCorporation()))
		{
			if (pFromCity->IsHasOffice())
			{
				CorporationTypes eCorporation = m_pPlayer->GetCorporations()->GetFoundedCorporation();
				CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
				if (pkCorporationInfo != NULL)
				{
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
					{
						int iYieldFromCorp = pFromCity->GetTradeRouteCityMod((YieldTypes)iYield);
						if (iYieldFromCorp > 0)
						{
							iScore += (iYieldFromCorp * 50);
						}
						if (pkCorporationInfo->GetTradeRouteMod(iYield) > 0)
						{
							iScore += (pkCorporationInfo->GetTradeRouteMod(iYield) * 50);
						}
					}
				}
			}
		}
		if (pToCity->IsHasOffice())
		{
			int iFranchises = (GET_PLAYER(pToCity->getOwner()).GetCorporations()->GetNumFranchises() / 2);
			//Care less if we like this guy.
			if (m_pPlayer->GetDiplomacyAI()->GetCivApproach(kTradeConnection.m_eDestOwner) == CIV_APPROACH_FRIENDLY ||
				m_pPlayer->GetDiplomacyAI()->GetCivOpinion(kTradeConnection.m_eDestOwner) >= CIV_OPINION_FAVORABLE)
			{
				iFranchises /= 5;
			}

			iScore /= max(1, iFranchises);
		}	
	}
#endif
	if(m_pPlayer->IsAtWar())
	{
		iScore /= max(2, m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false));
	}

	ret.m_iScore = iScore;
	ret.m_iCultureScore = iCultureScore;
	ret.m_iGoldScore = iGoldScore;
	ret.m_iScienceScore = iScienceScore;
	ret.m_iReligionScore = iReligionScore;
	return ret;
}

/// Score Food TR
int CvTradeAI::ScoreInternalTR(const TradeConnection& kTradeConnection, const std::vector<CvCity*>& aTargetCityList)
{
	CvAssert(kTradeConnection.m_eConnectionType != TRADE_CONNECTION_INTERNATIONAL);

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
			iScore = ((pOriginCity->getBaseYieldRate(YIELD_FOOD) + pOriginCity->getPopulation()) - (pDestCity->getBaseYieldRate(YIELD_FOOD) + pDestCity->getPopulation()));
			if(pDestCity->GetCityCitizens()->GetFocusType() == CITY_AI_FOCUS_TYPE_FOOD)
				iScore *= 20;
			break;
		}
		case TRADE_CONNECTION_PRODUCTION:
		case TRADE_CONNECTION_WONDER_RESOURCE:
		{
			iScore = (pOriginCity->getBaseYieldRate(YIELD_PRODUCTION) - pDestCity->getBaseYieldRate(YIELD_PRODUCTION));
			if(pDestCity->GetCityCitizens()->GetFocusType() == CITY_AI_FOCUS_TYPE_PRODUCTION)
				iScore *= 20;
			BuildingTypes eBuilding = pDestCity->getProductionBuilding();
			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pBuildingInfo)
				{
					CvBuildingClassInfo* pInfo = GC.getBuildingClassInfo(pBuildingInfo->GetBuildingClassType());
					if (pInfo && pInfo->getMaxGlobalInstances() == 1)
					{
						iScore *= 20;
					}
				}
			}
			break;
		}
		case TRADE_CONNECTION_GOLD_INTERNAL:
			break;
		case TRADE_CONNECTION_INTERNATIONAL:
			UNREACHABLE(); // Not supposed to be scoring an international route.
	}

	//direct connection to capital via TR
	if (pDestCity->GetUnhappinessFromIsolation() > 0 && pOriginCity->isCapital())
	{
		iScore *= 2;
	}
	if (pOriginCity->GetUnhappinessFromIsolation() > 0 && pDestCity->isCapital())
	{
		iScore *= 2;
	}

	int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	if (iGPT <= 0)
	{
		iScore += iGPT * 10;
	}

	//Check if we can siphon production from the target city
	if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && m_pPlayer->GetPlayerTraits()->IsTradeRouteProductionSiphon())
	{
		int iSiphonPercent = m_pPlayer->GetTradeRouteProductionSiphonPercent(false, &GET_PLAYER(kTradeConnection.m_eDestOwner));

		//Choose cities with high production
		//Multiply with 2 for a bit more weight here
		iScore += pDestCity->getBaseYieldRate(YIELD_PRODUCTION) * 2 * iSiphonPercent / 100;
	}

	for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
	{
		if(m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner((YieldTypes)iYieldLoop) > 0)
		{
			iScore -= (m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner((YieldTypes)iYieldLoop)) * 10;
		}
		if(m_pPlayer->GetGoldInternalTrade() > 0)
		{
			iScore += m_pPlayer->GetGoldInternalTrade() * 10;
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

		//avoid fallout etc
		if (m_pPlayer->GetPlotDanger(*pPlot,true)>0)
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

	return iScore / (iDistance + iDangerSum);
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

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
/// Score gold internal trade route
CvTradeAI::TRSortElement CvTradeAI::ScoreGoldInternalTR(const TradeConnection& kTradeConnection) const
{
	TRSortElement ret;
	ret.m_kTradeConnection = kTradeConnection;

	// don't evaluate other trade types
	if (kTradeConnection.m_eConnectionType != TRADE_CONNECTION_GOLD_INTERNAL)
		return ret;

	// if this was recently plundered, 0 the score
	if (m_pPlayer->GetTrade()->CheckTradeConnectionWasPlundered(kTradeConnection))
		return ret;

	CvCity* pToCity = CvGameTrade::GetDestCity(kTradeConnection);
	CvCity* pFromCity = CvGameTrade::GetOriginCity(kTradeConnection);

	if (!pToCity || !pFromCity)
		return ret;

	if (pToCity->isUnderSiege() || pFromCity->isUnderSiege())
		return ret;

	CvPlayerTrade* pPlayerTrade = m_pPlayer->GetTrade();

	// gold
	int iGoldAmount = pPlayerTrade->GetTradeConnectionValueTimes100(kTradeConnection, YIELD_GOLD, true);

#if defined(MOD_BALANCE_CORE)
	int iGPT = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	if (iGPT <= 0)
	{
		iGoldAmount *= (iGPT * -2);
	}
#endif

	//if a city is impoverished, let's send trade routes from there (multiply based on amount of unhappiness).
	if (MOD_BALANCE_VP)
	{
		int iPoverty = pFromCity->GetPoverty(false);
		if (iPoverty > 0)
		{
			iGoldAmount *= iPoverty * 10;
		}
	}

	// science
	int iAdjustedScienceAmount = 0;
	int iScienceAmount = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA);
	if (iScienceAmount < 0)
	{
		iScienceAmount = 0;
	}
	else if (iScienceAmount > 0)
	{
		int iCeilScience = iScienceAmount * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);

		iAdjustedScienceAmount = max(iCeilScience, 1);
		if (iAdjustedScienceAmount > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedScienceAmount *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
	}

	//if a city is illiterate, let's send trade routes from there (add based on amount of unhappiness).
	if (MOD_BALANCE_VP)
	{
		int iIlliteracy = pFromCity->GetIlliteracy(false);
		if (iIlliteracy > 0)
		{
			iAdjustedScienceAmount *= iIlliteracy * 10;
		}
	}

	// culture
	int iAdjustedCultureAmount = 0;
	int iCultureAmount = /*1*/ GD_INT_GET(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA);
	if (iCultureAmount < 0)
	{
		iCultureAmount = 0;
	}
	else if (iCultureAmount > 0)
	{
		int iCeilCulture = iCultureAmount * 100 / /*200 in CP, 125 in VP*/ GD_INT_GET(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
		iAdjustedCultureAmount = max(iCeilCulture, 1);
		if (iAdjustedCultureAmount > 0 && (GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra() > 0))
		{
			iAdjustedCultureAmount *= GET_PLAYER(kTradeConnection.m_eOriginOwner).GetCurrentEra();
		}
	}

	//if a city is bored, let's send trade routes from there (add based on amount of unhappiness).
	if (MOD_BALANCE_VP)
	{
		int iBoredom = pFromCity->GetBoredom(false);
		if (iBoredom > 0)
		{
			iAdjustedCultureAmount *= iBoredom * 10;
		}
	}

	// religion
	ReligionTypes eOwnerStateReligion = m_pPlayer->GetReligions()->GetStateReligion();
	int iReligionDelta = 0;
	if (eOwnerStateReligion != NO_RELIGION)
	{
		ReligionTypes eToReligion = NO_RELIGION;
		int iToPressure = 0;
		ReligionTypes eFromReligion = NO_RELIGION;
		int iFromPressure = 0;
		bool bAnyFromCityPressure = pFromCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pToCity, eToReligion, iToPressure);
		bool bAnyToCityPressure = pToCity->GetCityReligions()->WouldExertTradeRoutePressureToward(pFromCity, eFromReligion, iFromPressure);

		// Internally pressure is now 10 times greater than what is shown to user
		iToPressure /= /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
		iFromPressure /= /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);

		// if anyone is exerting pressure
		if (bAnyFromCityPressure || bAnyToCityPressure)
		{
			// "to" and "from" religions need to be different for us to care
			if (eToReligion != eFromReligion)
			{
				int iExistingToPressureAtFrom = pFromCity->GetCityReligions()->GetPressureAccumulated(eFromReligion);
				int iExistingGoodPressureAtTo = pToCity->GetCityReligions()->GetPressureAccumulated(eOwnerStateReligion);
				if (eToReligion != eOwnerStateReligion)
					iToPressure = 0;
				if (eFromReligion == eOwnerStateReligion)
					iFromPressure = 0;
				double dExistingPressureModFrom = sqrt(log((double)MAX(1, iExistingToPressureAtFrom + iFromPressure) / (double)MAX(1, iExistingToPressureAtFrom)) / log(2.));
				double dExistingPressureModTo = sqrt(log((double)MAX(1, iExistingGoodPressureAtTo + iToPressure) / (double)MAX(1, iExistingGoodPressureAtTo)) / log(2.));

				iReligionDelta += int(iToPressure * dExistingPressureModTo / /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER) + 0.5);
				iReligionDelta -= int(iFromPressure * dExistingPressureModFrom / /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER) + 0.5);
			}
		}
	}

	int iFlavorGold = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
	int iFlavorScience = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
	int iFlavorReligion = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
	int iFlavorCulture = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
	int iGoldScore = iGoldAmount / max(1, (10 - iFlavorGold));
	int iScienceScore = (iAdjustedScienceAmount *  iFlavorScience);
	int iCultureScore = (iAdjustedCultureAmount * iFlavorCulture);
	int iReligionScore = (iReligionDelta *  (iFlavorReligion / 2));

	//add it all up
	int iScore = iGoldScore + iScienceScore + iCultureScore + iReligionScore;

	//abort if we're negative
	if (iScore <= 0)
		return ret;

	for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		for (uint uiPlotList = 0; uiPlotList < kTradeConnection.m_aPlotList.size(); uiPlotList++)
		{
			CvPlot* pPlot = GC.getMap().plot(kTradeConnection.m_aPlotList[uiPlotList].m_iX, kTradeConnection.m_aPlotList[uiPlotList].m_iY);
			CvAssertMsg(pPlot, "pPlot is null when trying to evaluate the list");
			if (pPlot == NULL)
			{
				continue;
			}
			if (pPlot->getOwner() != m_pPlayer->GetID())
			{
				continue;
			}
			if (pPlot->getTerrainType() == NO_TERRAIN)
			{
				continue;
			}
			iScore += (m_pPlayer->GetPlayerTraits()->GetTerrainYieldChange(pPlot->getTerrainType(), ((YieldTypes)iJ)) * 10);
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

		//danger is hard to quantify for routes which be definition pass by a lot of invisible plots
		if (!pPlot->isVisible(m_pPlayer->getTeam()))
			iDangerSum += 1;

		if (pPlot->getTeam() && m_pPlayer->getTeam())
			iDangerSum += 10;

		if (pPlot->getTeam() != NO_TEAM && GET_TEAM(m_pPlayer->getTeam()).isAtWar(pPlot->getTeam()))
			iDangerSum += 100;

		if (m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pPlot))
			iDangerSum += 100;
	}

	int iEra = max(1, (int)m_pPlayer->GetCurrentEra()); // More international trade late game, please.
	iScore = (iScore * iEra) / iDangerSum;

	//finally
	ret.m_iScore = iScore;
	ret.m_iCultureScore = iCultureScore;
	ret.m_iGoldScore = iGoldScore;
	ret.m_iScienceScore = iScienceScore;
	ret.m_iReligionScore = iReligionScore;
	return ret;
}
#endif

struct SortTR
{
	bool operator()(CvTradeAI::TRSortElement const& a, CvTradeAI::TRSortElement const& b) const
	{
		return a.m_iScore < b.m_iScore;
	}
};
#if defined(MOD_BALANCE_CORE)
struct SortTRHigh
{
	bool operator()(CvTradeAI::TRSortElement const& a, CvTradeAI::TRSortElement const& b) const
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
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	std::vector<TRSortElement> aGoldInternalSortedTR;
#endif

	// FOOD FOOD FOOD FOOD
	std::vector<CvCity*> apFoodTargetCities;
	if (m_pPlayer->GetHappiness() >= 0 || m_pPlayer->GetUnhappinessFromFamine() >= 0)
	{
		// - Find avg city size
		int iAvgFood = 0;
		int iCityLoop = 0;
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

	int iCityLoop = 0;
	for (CvCity* pCity = m_pPlayer->firstCity(&iCityLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if(pCity->isProductionSpaceshipPart())
		{
			apProductionTargetCities.push_back(pCity);
		}
		else if (m_pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory() && m_pPlayer->GetDiplomacyAI()->IsCloseToSpaceshipVictory())
		{
			// is this a city in which we want to build spaceship parts in the near future?
			vector<CvCity*> vCitiesForSpaceship = m_pPlayer->GetCoreCitiesForSpaceshipProduction();
			if (vCitiesForSpaceship.size() > 0)
			{
				if (find(vCitiesForSpaceship.begin(), vCitiesForSpaceship.end(), pCity) != vCitiesForSpaceship.end())
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
			int iCityLoop = 0;
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

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	{
		// GOLD GOLD GOLD GOLD (INTERNAL)
		aGoldInternalSortedTR.clear();
		int iBestScore = -1;
		for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
		{
			TRSortElement kElement = ScoreGoldInternalTR(aTradeConnectionList[ui]);
			if (kElement.m_iScore > iBestScore)
			{
				iBestScore = kElement.m_iScore;
				aGoldInternalSortedTR.push_back(kElement);
			}
		}
		std::stable_sort(aGoldInternalSortedTR.begin(), aGoldInternalSortedTR.end(), SortTR());

		// clear list
		aTradeConnectionList.clear();
	}
#endif

	//precalculate this, it's expensive
	bool bHaveTourism = (m_pPlayer->GetCulture()->GetTourism() / 100 > 0);

	// GOLD GOLD GOLD GOLD
	aGoldSortedTR.clear();
	int iBestScore = -1;
	for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
	{
		TRSortElement kElement = ScoreInternationalTR(aTradeConnectionList[ui], bHaveTourism);
		if (kElement.m_iScore > iBestScore)
		{
			iBestScore = kElement.m_iScore;
			aGoldSortedTR.push_back(kElement);
		}
	}
	std::stable_sort(aGoldSortedTR.begin(), aGoldSortedTR.end(), SortTR());

	// clear list
	aTradeConnectionList.clear();
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

#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	if (MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
	{
		for (uint ui = 0; ui < aGoldInternalSortedTR.size(); ui++)
		{
			if (aGoldInternalSortedTR[ui].m_iScore > 0)
			{
				aTotalList.push_back(aGoldInternalSortedTR[ui]);
			}
		}
	}
#endif

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
				default:
					UNREACHABLE(); // Not a trade domain.
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
#if defined(MOD_BALANCE_CORE) && defined(MOD_BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES)
				case TRADE_CONNECTION_GOLD_INTERNAL:
					strTRType = "gold internal";
					break;
#endif
				case TRADE_CONNECTION_INTERNATIONAL:
					strTRType = "international";
					break;
				}

				if (eConnectionType == TRADE_CONNECTION_INTERNATIONAL || eConnectionType == TRADE_CONNECTION_GOLD_INTERNAL)
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
				FILogFile* pLog = NULL;
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
	int iOriginCityLoop = 0;
	CvCity* pOriginLoopCity = NULL;
	for(pOriginLoopCity = m_pPlayer->firstCity(&iOriginCityLoop); pOriginLoopCity != NULL; pOriginLoopCity = m_pPlayer->nextCity(&iOriginCityLoop))
	{
		pOriginLoopCity->SetTradePrioritySea(0);
		pOriginLoopCity->SetTradePriorityLand(0);
	}

	//Let's store off values for 'good' trade unit cities to grab elsewhere.
	for (uint ui = 0; ui < aTradeConnectionList.size(); ui++)
	{
		CvCity* pCity = m_pPlayer->getCity(aTradeConnectionList[ui].m_iOriginID);
		if (!pCity)
			continue;

		// 100 is highest prio
		int iPrioPercent = ((aTradeConnectionList.size() - ui)*100)/aTradeConnectionList.size();

		if(aTradeConnectionList[ui].m_eDomain == DOMAIN_SEA && pCity->GetTradePrioritySea() < iPrioPercent)
		{
			pCity->SetTradePrioritySea(iPrioPercent);
		}
		else if(pCity->GetTradePriorityLand() < iPrioPercent)
		{
			pCity->SetTradePriorityLand(iPrioPercent);
		}
	}
}

template<typename TradeAI, typename Visitor>
void CvTradeAI::Serialize(TradeAI& tradeAI, Visitor& visitor)
{
	visitor(tradeAI.m_iRemovableValue);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvTradeAI& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvTradeAI::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvTradeAI& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvTradeAI::Serialize(readFrom, serialVisitor);
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
