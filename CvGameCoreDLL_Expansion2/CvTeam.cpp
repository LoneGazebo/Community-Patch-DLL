/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvPlayerAI.h"
#include "CvDefines.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvGlobals.h"
#include "CvTeam.h"
#include "CvPlayerAI.h"
#include "CvEspionageClasses.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeam.h"
#include "CvTechAI.h"
#include "ICvDLLUserInterface.h"
#include "CvInfos.h"
#include "CvMinorCivAI.h"
#include "CvGrandStrategyAI.h"
#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"
#include "CvImprovementClasses.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "CvInfosSerializationHelper.h"
#include "CvPlayerManager.h"
#include "CvCitySpecializationAI.h"
#include "CvEnumMapSerialization.h"

#include "CvDllUnit.h"

#include "LintFree.h"

// statics
CvTeam* CvTeam::m_aTeams = NULL;

inline CvTeam& CvTeam::getTeam(TeamTypes eTeam)
{
	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is not assigned a valid value");

	if (eTeam == NO_TEAM || eTeam >= MAX_TEAMS)
		eTeam = BARBARIAN_TEAM;

	return m_aTeams[eTeam];
}

//	--------------------------------------------------------------------------------
void CvTeam::initStatics()
{
	m_aTeams = FNEW(CvTeam[REALLY_MAX_TEAMS], c_eCiv5GameplayDLL, 0);

	for(int i = 0; i < REALLY_MAX_TEAMS; ++i)
	{
		m_aTeams[i].m_eID = (TeamTypes)i;
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::freeStatics()
{
	delete[] m_aTeams;
	m_aTeams = NULL;
}

// Public Functions...
//	--------------------------------------------------------------------------------
CvTeam::CvTeam()
{
	m_abCanLaunch = NULL;
	m_abVictoryAchieved = NULL;
	m_abSmallAwardAchieved = NULL;

	m_paiRouteChange = NULL;
	m_paiBuildTimeChange = NULL;
	m_paiProjectCount = NULL;
	m_paiProjectDefaultArtTypes = NULL;
	m_paiProjectMaking = NULL;
	m_paiUnitClassCount = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_pTeamTechs = FNEW(CvTeamTechs, c_eCiv5GameplayDLL, 0);

	m_ppaaiFeatureYieldChange = NULL;
	m_ppaaiTerrainYieldChange = NULL;
	m_paiTradeRouteDomainExtraRange = NULL;

	m_ppaaiImprovementYieldChange = NULL;
	m_ppaaiImprovementNoFreshWaterYieldChange = NULL;
	m_ppaaiImprovementFreshWaterYieldChange = NULL;

	m_pabTradeTech = NULL;

	reset((TeamTypes)0, true);
}


//	--------------------------------------------------------------------------------
CvTeam::~CvTeam()
{
	uninit();

	delete m_pTeamTechs;
}


//	--------------------------------------------------------------------------------
void CvTeam::init(TeamTypes eID)
{
	//--------------------------------
	// Init saved data
	reset(eID);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


//	--------------------------------------------------------------------------------
void CvTeam::uninit()
{

	//Free the batch-allocated memory
	m_BatchData.Free();

	//Set the ptrs to null
	m_abCanLaunch = NULL;
	m_abVictoryAchieved = NULL;
	m_abSmallAwardAchieved = NULL;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_pabTradeTech = NULL;
#endif

	m_paiRouteChange = NULL;
	m_paiBuildTimeChange = NULL;
	m_paiProjectCount = NULL;
	m_paiProjectDefaultArtTypes = NULL;
	m_pavProjectArtTypes.uninit();
	m_paiProjectMaking = NULL;
	m_paiUnitClassCount = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_paiTradeRouteDomainExtraRange = NULL;
	m_ppaaiFeatureYieldChange = NULL;
	m_ppaaiTerrainYieldChange = NULL;

	m_ppaaiImprovementYieldChange = NULL;
	m_ppaaiImprovementNoFreshWaterYieldChange = NULL;
	m_ppaaiImprovementFreshWaterYieldChange = NULL;

	m_pTeamTechs->Uninit();

	m_iAliveCount = 0;
	m_iEverAliveCount = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iNukeInterception = 0;
	m_iExtraWaterSeeFromCount = 0;
	m_iMapTradingCount = 0;
	m_iTechTradingCount = 0;
	m_iGoldTradingCount = 0;
	m_iAllowEmbassyTradingAllowedCount = 0;
	m_iOpenBordersTradingAllowedCount = 0;
	m_iDefensivePactTradingAllowedCount = 0;
	m_iResearchAgreementTradingAllowedCount = 0;
#if defined(MOD_TECHS_CITY_WORKING)
	m_iCityWorkingChange = 0;
#endif
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	m_iCityAutomatonWorkersChange = 0;
#endif
	m_iBridgeBuildingCount = 0;
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	m_iCityLessEmbarkCost = 0;
	m_iCityNoEmbarkCost = 0;
#endif
	m_iWaterWorkCount = 0;
	m_iRiverTradeCount = 0;
	m_iBorderObstacleCount = 0;
	m_iVictoryPoints = 0;
	m_iEmbarkedExtraMoves = 0;
	m_iCanEmbarkCount = 0;
	m_iDefensiveEmbarkCount = 0;
	m_iEmbarkedAllWaterPassageCount = 0;
	m_iNumNaturalWondersDiscovered = 0;
	m_iNumLandmarksBuilt = 0;
	m_iBestPossibleRoute = NO_ROUTE;
	m_iNumMinorCivsAttacked = 0;

	m_bMapCentering = false;
	m_bHomeOfUnitedNations = false;
	m_bHasTechForWorldCongress = false;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_iVassalageTradingAllowedCount = 0;
#endif

	m_eCurrentEra = ((EraTypes) 0);

	m_eLiberatedByTeam = NO_TEAM;
	m_eKilledByTeam = NO_TEAM;

	for(int i = 0; i < NUM_DOMAIN_TYPES; i++)
	{
		m_aiExtraMoves[i] = 0;
	}

	for(int i = 0; i < MAX_TEAMS; i++)
	{
		m_aiTechShareCount[i] = 0;
		m_paiTurnMadePeaceTreatyWithTeam[i] = -1;
		m_aiIgnoreWarningCount[i] = 0;
		m_abHasMet[i] = false;
		m_abAtWar[i] = false;
		m_abAggressorPacifier[i] = false;
		m_abPermanentWarPeace[i] = false;
		m_abEmbassy[i] = false;
		m_abOpenBorders[i] = false;
		m_abDefensivePact[i] = false;
		m_abResearchAgreement[i] = false;
		m_abForcePeace[i] = false;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		m_aiNumTurnsSinceVassalEnded[i] = -1;
#endif
	}

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		m_abHasFoundPlayersTerritory[i] = false;
	}

	m_eID = NO_TEAM;

#if defined(MOD_BALANCE_CORE)
	m_members.clear();
	m_bIsMinorTeam = false;
	m_bIsObserverTeam = false;
	m_iCorporationsEnabledCount = 0;
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsVoluntaryVassal = false;
	m_iNumTurnsIsVassal = -1;
	m_iNumCitiesWhenVassalMade = 0;
	m_iTotalPopulationWhenVassalMade = 0;
	m_eMaster = NO_TEAM;

	for(int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		m_aiNumTurnsSinceVassalTaxSet[i] = -1;
		m_aiVassalTax[i] = 0;
	}
#endif
}


//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvTeam::reset(TeamTypes eID, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_eID = eID;

	if(!bConstructorCall)
	{
		//Collect sizes
		int numVoteSourceInfos = GC.getNumVoteSourceInfos();
		int numVictoryInfos = GC.getNumVictoryInfos();
		int numSmallAwardInfos = GC.getNumSmallAwardInfos();
		int numRouteInfos = GC.getNumRouteInfos();
		int numBuildInfos = GC.getNumBuildInfos();
		int numProjectInfos = GC.getNumProjectInfos();
		int numUnitClassInfos = GC.getNumUnitClassInfos();
		int numBuildingClassInfos = GC.getNumBuildingClassInfos();
		int numBuildingInfos = GC.getNumBuildingInfos();
		int numFeatureInfos = GC.getNumFeatureInfos();
		int numDomainInfos = NUM_DOMAIN_TYPES;
		int numTerrainInfos = GC.getNumTerrainInfos();
		int numImprovementInfos = GC.getNumImprovementInfos();
		int numTechInfos = GC.getNumTechInfos();

		//Perform batch allocation
		AllocData aData[] =
		{
			{&m_aiForceTeamVoteEligibilityCount,	numVoteSourceInfos, 0},

			{&m_abCanLaunch,						numVictoryInfos, 0},
			{&m_abVictoryAchieved,					numVictoryInfos, 0},
			{&m_abSmallAwardAchieved,				numSmallAwardInfos, 0},
			{&m_pabTradeTech,						numTechInfos, 0},

			{&m_paiRouteChange,						numRouteInfos, 0},
			{&m_paiBuildTimeChange,					numBuildInfos, 0},

			{&m_paiProjectCount,					numProjectInfos, 0},
			{&m_paiProjectDefaultArtTypes,			numProjectInfos, 0},
			{&m_paiProjectMaking,					numProjectInfos, 0},

			{&m_paiUnitClassCount,					numUnitClassInfos, 0},
			{&m_paiBuildingClassCount,				numBuildingClassInfos, 0},
			{&m_paiObsoleteBuildingCount,			numBuildingInfos, 0},
			{&m_paiTerrainTradeCount,				numTerrainInfos, 0},
			{&m_aiVictoryCountdown,					numVictoryInfos, 0},

			{&m_paiTradeRouteDomainExtraRange,		numDomainInfos, 0},
			// If adding more entries into this strucure, you also need to update CvTeamData in Cvteam.h to match
			{&m_ppaaiFeatureYieldChange,			numFeatureInfos, NUM_YIELD_TYPES},
			{&m_ppaaiTerrainYieldChange,			numTerrainInfos, NUM_YIELD_TYPES},

			{&m_ppaaiImprovementYieldChange,		numImprovementInfos, NUM_YIELD_TYPES},
			{&m_ppaaiImprovementNoFreshWaterYieldChange,numImprovementInfos, NUM_YIELD_TYPES},
			{&m_ppaaiImprovementFreshWaterYieldChange,numImprovementInfos, NUM_YIELD_TYPES}
		};
		m_BatchData.Alloc(aData);

		//Init the data - it is valid now
		for(int i = 0; i < numVoteSourceInfos; i++)
		{
			m_aiForceTeamVoteEligibilityCount[i] = 0;
		}
		for(int i = 0; i < numVictoryInfos; i++)
		{
			m_abCanLaunch[i] = false;
			m_abVictoryAchieved[i] = false;
			m_aiVictoryCountdown[i] = -1;
		}
		for(int i = 0; i < numSmallAwardInfos; i++)
		{
			m_abSmallAwardAchieved[i] = false;
		}
		for(int i = 0; i < numRouteInfos; i++)
		{
			m_paiRouteChange[i] = 0;
		}
		for (int i = 0; i < numBuildInfos; i++)
		{
			m_paiBuildTimeChange[i] = 0;
		}
		for(int i = 0; i < numProjectInfos; i++)
		{
			m_paiProjectCount[i] = 0;
			m_paiProjectDefaultArtTypes[i] = 0;
			m_paiProjectMaking[i] = 0;
		}
		for(int i = 0; i < numUnitClassInfos; i++)
		{
			m_paiUnitClassCount[i] = 0;
		}
		for(int i = 0; i < numBuildingClassInfos; i++)
		{
			m_paiBuildingClassCount[i] = 0;
		}
		for(int i = 0; i < numBuildingInfos; i++)
		{
			m_paiObsoleteBuildingCount[i] = 0;
		}
		for(int i = 0; i < numTerrainInfos; i++)
		{
			m_paiTerrainTradeCount[i] = 0;
		}
		for(int i = 0; i < numTechInfos; i++)
		{
			m_pabTradeTech[i] = false;
		}

		for(int i = 0; i < MAX_TEAMS; i++)
		{
			m_aiTurnTeamMet[i] = -1;
			m_aiNumTurnsAtWar[i] = 0;
			m_aiNumTurnsLockedIntoWar[i] = 0;
		}

		for (int i = 0; i < numDomainInfos; i++)
		{
			m_paiTradeRouteDomainExtraRange[i] = 0;
		}
		for(int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			for(int i = 0; i < numFeatureInfos; i++)
			{
				m_ppaaiFeatureYieldChange[i][j] = 0;
			}

			for(int i = 0; i < numTerrainInfos; i++)
			{
				m_ppaaiTerrainYieldChange[i][j] = 0;
			}
		}

		for(int i = 0; i < numImprovementInfos; i++)
		{
			for(int j = 0; j < NUM_YIELD_TYPES; j++)
			{
				m_ppaaiImprovementYieldChange[i][j] = 0;
				m_ppaaiImprovementNoFreshWaterYieldChange[i][j] = 0;
				m_ppaaiImprovementFreshWaterYieldChange[i][j] = 0;
			}
		}

		m_pTeamTechs->Init(GC.GetGameTechs(), this);
		m_pavProjectArtTypes.init();
		m_aeRevealedResources.clear();
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::addTeam(TeamTypes eTeam)
{
	CvPlot* pLoopPlot;
	CvString strBuffer;
	int iI, iJ;

	CvAssert(eTeam != NO_TEAM);
	CvAssert(eTeam != GetID());

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if((GET_PLAYER((PlayerTypes)iI).getTeam() != GetID()) && (GET_PLAYER((PlayerTypes)iI).getTeam() != eTeam))
			{
				if(GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
				{
					if(iI == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
						DLLUI->AddMessage(0, ((PlayerTypes)iI), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_THEIRALLIANCE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")*/);
					}
				}
			}
		}
	}

	strBuffer = GetLocalizedText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);

	// To be replaced with new diplomatic code

	shareItems(eTeam);
	GET_TEAM(eTeam).shareItems(GetID());

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
				{
					meet(((TeamTypes)iI), false);
				}
				else if(isHasMet((TeamTypes)iI))
				{
					GET_TEAM(eTeam).meet((TeamTypes)iI, false);
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
				{
					declareWar(((TeamTypes)iI), false, getLeaderID());
				}
				else if(isAtWar((TeamTypes)iI))
				{
					GET_TEAM(eTeam).declareWar(((TeamTypes)iI), false, GET_TEAM(eTeam).getLeaderID());
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iI))
				{
					setPermanentWarPeace(((TeamTypes)iI), true);
				}
				else if(isPermanentWarPeace((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setPermanentWarPeace(((TeamTypes)iI), true);
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).HasEmbassyAtTeam((TeamTypes)iI))
				{
					SetHasEmbassyAtTeam(((TeamTypes)iI), true);
				}
				else if(HasEmbassyAtTeam((TeamTypes)iI))
				{
					GET_TEAM(eTeam).SetHasEmbassyAtTeam(((TeamTypes)iI), true);
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).IsAllowsOpenBordersToTeam((TeamTypes)iI))
				{
					SetAllowsOpenBordersToTeam(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).SetAllowsOpenBordersToTeam(GetID(), true);
				}
				else if(IsAllowsOpenBordersToTeam((TeamTypes)iI))
				{
					GET_TEAM(eTeam).SetAllowsOpenBordersToTeam(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).SetAllowsOpenBordersToTeam(eTeam, true);
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).IsHasDefensivePact((TeamTypes)iI))
				{
					SetHasDefensivePact(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).SetHasDefensivePact(GetID(), true);
				}
				else if(IsHasDefensivePact((TeamTypes)iI))
				{
					GET_TEAM(eTeam).SetHasDefensivePact(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).SetHasDefensivePact(eTeam, true);
				}
			}
		}
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if((iI != GetID()) && (iI != eTeam))
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM(eTeam).isForcePeace((TeamTypes)iI))
				{
					setForcePeace(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setForcePeace(GetID(), true);
				}
				else if(isForcePeace((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setForcePeace(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setForcePeace(eTeam, true);
				}
			}
		}
	}

	shareCounters(eTeam);
	GET_TEAM(eTeam).shareCounters(GetID());

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			GET_PLAYER((PlayerTypes)iI).setTeam(GetID());
		}
	}

	const int iNumInvisibleInfos = NUM_INVISIBLE_TYPES;
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		pLoopPlot->changeVisibilityCount(GetID(), pLoopPlot->getVisibilityCount(eTeam), NO_INVISIBLE, true, false);
		pLoopPlot->changeInvisibleVisibilityCountUnit(GetID(), pLoopPlot->getInvisibleVisibilityCountUnit(eTeam));
		for(iJ = 0; iJ < iNumInvisibleInfos; iJ++)
		{
			pLoopPlot->changeInvisibleVisibilityCount(GetID(), ((InvisibleTypes)iJ), pLoopPlot->getInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ)));
		}

		if(pLoopPlot->isRevealed(eTeam))
		{
			pLoopPlot->setRevealed(GetID(), true, NULL, false, eTeam);
		}
	}

	GC.getGame().updateScore(true);
}


//	--------------------------------------------------------------------------------
void CvTeam::shareItems(TeamTypes eTeam)
{
	CvCity* pLoopCity;
	int iLoop;
	int iI, iJ, iK;

	CvAssert(eTeam != NO_TEAM);
	CvAssert(eTeam != GetID());

	CvTeam& kTeam = GET_TEAM(eTeam);

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if(kTeam.GetTeamTechs()->HasTech((TechTypes)iI))
		{
			setHasTech(((TechTypes)iI), true, NO_PLAYER, true, false);
		}
	}

	for(iI = 0; iI < GC.getNumResourceInfos(); ++iI)
	{
		if(kTeam.isForceRevealedResource((ResourceTypes)iI))
		{
			setForceRevealedResource((ResourceTypes)iI, true);
		}
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(static_cast<PlayerTypes>(iI));
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == eTeam)
			{
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					for(iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
					{
						const BuildingTypes eBuilding = static_cast<BuildingTypes>(iJ);
						CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
						if(pkBuildingInfo)
						{
							if(pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
							{
								if(!isObsoleteBuilding(eBuilding))
								{
									if(pkBuildingInfo->IsTeamShare())
									{
										for(iK = 0; iK < MAX_PLAYERS; iK++)
										{
											CvPlayerAI& kOtherPlayer = GET_PLAYER(static_cast<PlayerTypes>(iK));
											if(kOtherPlayer.isAlive() && kOtherPlayer.getTeam() == m_eID)
											{
												CvArea* pArea = pLoopCity->plot()->area();
												kOtherPlayer.processBuilding(eBuilding, pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding), /*bFirst*/ false, pArea);
											}
										}
									}

									processBuilding(eBuilding, pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding), /*bFirst*/ false);
								}
							}
						}
					}
				}
			}
		}
	}

}


//	--------------------------------------------------------------------------------
void CvTeam::shareCounters(TeamTypes eTeam)
{
	int iI;

	for(iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		int iExtraProjects = GET_TEAM(eTeam).getProjectCount((ProjectTypes)iI) - getProjectCount((ProjectTypes)iI);
		if(iExtraProjects > 0)
		{
			changeProjectCount((ProjectTypes)iI, iExtraProjects);
			GC.getGame().incrementProjectCreatedCount((ProjectTypes)iI, -iExtraProjects);
		}

		changeProjectMaking(((ProjectTypes)iI), GET_TEAM(eTeam).getProjectMaking((ProjectTypes)iI));
	}

	for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
		if(!pkUnitClassInfo)
		{
			continue;
		}

		changeUnitClassCount(((UnitClassTypes)iI), GET_TEAM(eTeam).getUnitClassCount((UnitClassTypes)iI));
	}

	for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		changeBuildingClassCount(((BuildingClassTypes)iI), GET_TEAM(eTeam).getBuildingClassCount((BuildingClassTypes)iI));
	}

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if(GET_TEAM(eTeam).GetTeamTechs()->GetResearchProgress((TechTypes)iI) > GetTeamTechs()->GetResearchProgress((TechTypes)iI))
		{
			GetTeamTechs()->SetResearchProgress(((TechTypes)iI), GET_TEAM(eTeam).GetTeamTechs()->GetResearchProgress((TechTypes)iI), getLeaderID());
		}

		if(GET_TEAM(eTeam).GetTeamTechs()->IsNoTradeTech((TechTypes)iI))
		{
			GetTeamTechs()->SetNoTradeTech((TechTypes)iI, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst)
{
	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pBuildingInfo == NULL)
		return;

	// One-shot items
	if(bFirst && iChange > 0)
	{
		// Diplo victory
		if(pBuildingInfo->IsDiplomaticVoting())
		{
			SetHomeOfUnitedNations(true);
			GC.getGame().GetGameLeagues()->DoUnitedNationsBuilt(getLeaderID());
		}
	}

	changeVictoryPoints((pBuildingInfo->GetVictoryPoints()) * iChange);

	if((pBuildingInfo->GetTechShare() > 0) && (pBuildingInfo->GetTechShare() <= MAX_TEAMS))
	{
		changeTechShareCount((pBuildingInfo->GetTechShare() - 1), iChange);
	}

	if(pBuildingInfo->IsBorderObstacle())
	{
		changeBorderObstacleCount(pBuildingInfo->IsBorderObstacle() * iChange);
	}

	if(pBuildingInfo->IsMapCentering())
	{
		if(iChange > 0)
		{
			setMapCentering(true);
		}
	}

	// Effects in every City on this Team
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
		if(kPlayer.getTeam() == m_eID && kPlayer.isAlive())
		{
			CvCity* pLoopCity;
			int iLoop;

			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				pLoopCity->GetCityBuildings()->ChangeBuildingDefenseMod(pBuildingInfo->GetGlobalDefenseModifier() * iChange);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::doTurn()
{
	CvAssertMsg(isAlive(), "isAlive is expected to be true");

	// Barbarians get all Techs that 3/4 of alive players get
	if (isBarbarian())
	{
		DoBarbarianTech();
	}
	// NOT barbs
	else
	{
		// War counter
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eTeam = (TeamTypes) iTeamLoop;

			if (!GET_TEAM(eTeam).isBarbarian())
			{
				if (isAtWar(eTeam))
					ChangeNumTurnsAtWar(eTeam, 1);
				else
					SetNumTurnsAtWar(eTeam, 0);
			}

			if (GetNumTurnsLockedIntoWar(eTeam) > 0)
				ChangeNumTurnsLockedIntoWar(eTeam, -1);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			if (MOD_DIPLOMACY_CIV4_FEATURES)
			{
				// We are their vassal
				if (IsVassal(eTeam)) 
				{
					ChangeNumTurnsIsVassal(1);

					// Get players in vassal team
					for (std::vector<PlayerTypes>::const_iterator vassal = getPlayers().begin(); vassal != getPlayers().end(); ++vassal)
					{
						int iGrossGold = GET_PLAYER(*vassal).GetTreasury()->CalculateGrossGoldTimes100();
						int iTaxedGold = GET_PLAYER(*vassal).GetTreasury()->GetExpensePerTurnFromVassalTaxesTimes100();
					
						// Need master players too
						for (std::vector<PlayerTypes>::const_iterator master = GET_TEAM(eTeam).getPlayers().begin(); master != GET_TEAM(eTeam).getPlayers().end(); ++master)
						{
							GET_PLAYER(*vassal).GetDiplomacyAI()->ChangeVassalGoldPerTurnTaxedSinceVassalStarted(*master, iTaxedGold);
							GET_PLAYER(*vassal).GetDiplomacyAI()->ChangeVassalGoldPerTurnCollectedSinceVassalStarted(*master, iGrossGold);
						}
					}
				}

				// We are their master
				if (GET_TEAM(eTeam).GetMaster() == GetID()) 
				{
					// Push a notification to team members if we can liberate a vassal this turn
					if (CanLiberateVassal(eTeam) && GET_TEAM(eTeam).GetNumTurnsIsVassal() == GC.getGame().getGameSpeedInfo().getMinimumVassalLiberateTurns())
					{
						Localization::String summaryString = Localization::Lookup("TXT_KEY_MISC_VASSAL_LIBERATION_POSSIBLE_SUMMARY");
						Localization::String descString = Localization::Lookup("TXT_KEY_MISC_VASSAL_LIBERATION_POSSIBLE");
						descString << GET_TEAM(eTeam).getName();
						AddNotification(NOTIFICATION_PEACE_ACTIVE_PLAYER, descString.toUTF8(), summaryString.toUTF8(), -1, -1, GET_TEAM(eTeam).getLeaderID());
					}

					// Get players in vassal team
					for (std::vector<PlayerTypes>::const_iterator iI = GET_TEAM(eTeam).getPlayers().begin(); iI != GET_TEAM(eTeam).getPlayers().end(); ++iI)
					{
						// We set a vassal tax sometime
						if (GetNumTurnsSinceVassalTaxSet(*iI) > -1)
						{
							ChangeNumTurnsSinceVassalTaxSet(*iI, 1);

							// Push a notification to team members if we can set vassal tax this turn
							if (CanSetVassalTax(*iI) && GetNumTurnsSinceVassalTaxSet(*iI) == GC.getGame().getGameSpeedInfo().getMinimumVassalTaxTurns())
							{
								Localization::String locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAXES_AVAILABLE");
								locString << GET_PLAYER(*iI).getName();
								AddNotification(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, *iI);
							}
						}
					}
				}

				// increment vassal ended if we're not a vassal of eTeam
				if (GetNumTurnsSinceVassalEnded(eTeam) > -1)
					ChangeNumTurnsSinceVassalEnded(eTeam, 1);
			}
#endif
		}
	}

	// City States also get all Techs that 3/4 of alive players get
	if (isMinorCiv())
	{
		DoMinorCivTech();
	}

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		GetTeamTechs()->SetNoTradeTech(((TechTypes)iI), false);
	}
#if !defined(MOD_BALANCE_CORE)
	DoTestSmallAwards();
#endif

	testCircumnavigated();

#ifndef FINAL_RELEASE
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		if(!isAtWar(GET_PLAYER((PlayerTypes) iMinorLoop).getTeam()))
			continue;

		CvAssertMsg(GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->GetAlly() != getLeaderID(), "Major civ is now at war with a minor it is allied with! This is dumb and bad. Please send Jon this along with your last 5 autosaves and a changelist #.");
	}
#endif
}

//	--------------------------------------------------------------------------------
/// Barbarian Tech Progress
void CvTeam::DoBarbarianTech()
{
	int iCount;

	TechTypes eTech;
	TeamTypes eTeam;

	int iPossibleCount = 0;

	// See how many majors are still around
	int iTeamLoop;
	for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eTeam).isAlive())
		{
			if(!GET_TEAM(eTeam).isMinorCiv())
			{
				iPossibleCount++;
			}
		}
	}

	CvAssertMsg(iPossibleCount > 0, "Zero possible players? Uhhh...");

	// x% of majors (rounded down) need the tech for the Barbs to get it
	int iTechPercent = /*75 in CP, 80 in VP*/ GD_INT_GET(BARBARIAN_TECH_PERCENT);
	int iTeamsNeeded = max(1, iPossibleCount * iTechPercent / 100);

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		eTech = (TechTypes) iTechLoop;

		if(!GetTeamTechs()->HasTech(eTech))
		{
			iCount = 0;

			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				eTeam = (TeamTypes) iTeamLoop;

				if(GET_TEAM(eTeam).isAlive())
				{
					// Only look at Majors, as the City States will lag behind
					if(!GET_TEAM(eTeam).isMinorCiv())
					{
						if(GET_TEAM(eTeam).GetTeamTechs()->HasTech(eTech))
						{
							iCount++;
						}
					}
				}
			}

			// Do enough majors have this tech?
			if(iCount >= iTeamsNeeded)
			{
				setHasTech(eTech, true, getLeaderID(), false, false);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Minor Civ Tech Progress
void CvTeam::DoMinorCivTech()
{
	int iCount;

	TechTypes eTech;
	TeamTypes eTeam;

	int iPossibleCount = 0;

	// See how many majors are still around
	int iTeamLoop;
	for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eTeam).isAlive())
		{
			if(!GET_TEAM(eTeam).isMinorCiv())
			{
				iPossibleCount++;
			}
		}
	}

	CvAssertMsg(iPossibleCount > 0, "Zero possible players? Uhhh...");

	// x% of majors (rounded down) need the tech for the Minors to get it
	int iTechPercent = /*40 in CP, 60 in VP*/ GD_INT_GET(MINOR_CIV_TECH_PERCENT);
	int iTeamsNeeded = max(1, iPossibleCount * iTechPercent / 100);

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		eTech = (TechTypes) iTechLoop;

		if(!GetTeamTechs()->HasTech(eTech))
		{
			iCount = 0;

			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				eTeam = (TeamTypes) iTeamLoop;

				if(GET_TEAM(eTeam).isAlive())
				{
					// Only look at Majors
					if(!GET_TEAM(eTeam).isMinorCiv())
					{
						if(GET_TEAM(eTeam).GetTeamTechs()->HasTech(eTech))
						{
							iCount++;
						}
					}
				}
			}

			// Do enough majors have this tech?
			if(iCount >= iTeamsNeeded)
			{
				setHasTech(eTech, true, getLeaderID(), false, false);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::updateYield()
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
			{
				GET_PLAYER((PlayerTypes)iI).updateYield();
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::canChangeWarPeace(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM || eTeam == GetID()) 
	{
		return false;
	}

	if (GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE) || GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) || GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE))
	{
		return false;
	}

	// Vassals have no control over war/peace
	if (IsVassalOfSomeone())
	{
		return false;
	}

	if (GetNumTurnsLockedIntoWar(eTeam) > 0 || GET_TEAM(eTeam).GetNumTurnsLockedIntoWar(GetID()) > 0)
	{
		return false;
	}

	if (isPermanentWarPeace(eTeam) || GET_TEAM(eTeam).isPermanentWarPeace(GetID()))
	{
		return false;
	}

	return true;
}

#if defined(MOD_BALANCE_CORE)
void CvTeam::ClearWarDeclarationCache()
{
	m_cacheCanDeclareWar.clear();
}
int CvTeam::getCorporationsEnabledCount() const
{
	return m_iCorporationsEnabledCount;
}
bool CvTeam::IsCorporationsEnabled() const
{
	return m_iCorporationsEnabledCount > 0;
}
void CvTeam::changeCorporationsEnabledCount(int iChange)
{
	if(iChange > 0 && m_iCorporationsEnabledCount == 0)
	{
		PlayerTypes ePlayer;
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			ePlayer = (PlayerTypes) iI;

			if(GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).GetNotifications())
			{
				// Players on team that declared
				if(GET_PLAYER(ePlayer).getTeam() == GetID())
				{
					if(ePlayer == GC.getGame().getActivePlayer())
					{
						Localization::String strTemp = Localization::Lookup("TXT_KEY_TECH_CORP_ENABLED");
						Localization::String strSummary = Localization::Lookup("TXT_KEY_TECH_CORP_ENABLED_S");
						GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_GENERIC, strTemp.toUTF8(), strSummary.toUTF8(), -1, -1, ePlayer);
					}
				}
			}
		}
	}
	m_iCorporationsEnabledCount = m_iCorporationsEnabledCount + iChange;
	CvAssert(getCorporationsEnabledCount() >= 0);
}
#endif

//	--------------------------------------------------------------------------------
bool CvTeam::canDeclareWar(TeamTypes eTeam, PlayerTypes eOriginatingPlayer)
{
	if (eTeam == GetID())
	{
		return false;
	}

	if (!isAlive() || !GET_TEAM(eTeam).isAlive())
	{
		return false;
	}

	if (isAtWar(eTeam))
	{
		return false;
	}

	if (!isHasMet(eTeam))
	{
		return false;
	}

	if (isForcePeace(eTeam))
	{
		return false;
	}

	if (!canChangeWarPeace(eTeam))
	{
		return false;
	}

	if (GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE))
	{
		return false;
	}

	if (IsVassalOfSomeone() && GetMaster() != eTeam)
	{
		return false;
	}

	if (!GET_PLAYER(eOriginatingPlayer).isHuman())
	{
		if (GC.getGame().IsAIPassiveMode())
		{
			return false;
		}
		else if (GC.getGame().IsAIPassiveTowardsHumans())
		{
			if (GET_TEAM(eTeam).isHuman())
			{
				return false;
			}
		}
	}

	// Also check Defensive Pacts/Vassals
	for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;
		if (eLoopTeam != NO_TEAM && eLoopTeam != GetID() && eLoopTeam != eTeam && (GET_TEAM(eTeam).IsHasDefensivePact(eLoopTeam) || GET_TEAM(eTeam).IsVassal(eLoopTeam) || GET_TEAM(eLoopTeam).IsVassal(eTeam)))
		{
			if (GET_TEAM(eTeam).isHuman() && !GET_PLAYER(eOriginatingPlayer).isHuman() && GC.getGame().IsAIPassiveTowardsHumans())
				return false;

			// Exploit prevention: Can't bypass a Peace Treaty!
			if (isForcePeace(eLoopTeam) && /*exclude the vassal case for Peace Treaties*/ !GET_TEAM(eLoopTeam).IsVassal(eTeam))
				return false;
		}
	}

	if (MOD_EVENTS_WAR_AND_PEACE) 
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_IsAbleToDeclareWar, eOriginatingPlayer, eTeam) == GAMEEVENTRETURN_FALSE) 
		{
			return false;
		}

		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanDeclareWar, eOriginatingPlayer, eTeam) == GAMEEVENTRETURN_FALSE) 
		{
			return false;
		}
	}

	// First, obtain the Lua script system.
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		// Construct and push in some event arguments.
		CvLuaArgsHandle args(2);
		args->Push(GetID());
		args->Push(eTeam);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "CanDeclareWar", args.get(), bResult))
		{
			// Check the result.
			if (!bResult)
				return false;
		}
	}

	return true;
}

//	-----------------------------------------------------------------------------------------------
void CvTeam::declareWar(TeamTypes eTeam, bool bDefensivePact, PlayerTypes eOriginatingPlayer)
{
	DoDeclareWar(eOriginatingPlayer, true, eTeam, bDefensivePact);

	CvPlayerManager::Refresh(true);
}

//	-----------------------------------------------------------------------------------------------
void CvTeam::DoDeclareWar(PlayerTypes eOriginatingPlayer, bool bAggressor, TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact)
{
	Localization::String locString;
	int iI;

	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");

	// War declarations on vassals are redirected to the master
	if (!isMinorCiv())
	{
		if (GET_TEAM(eTeam).GetMaster() != NO_TEAM && eTeam != GetID() && GET_TEAM(eTeam).GetMaster() != GetID())
		{
			if (!isAtWar(GET_TEAM(eTeam).GetMaster()))
			{
				DoDeclareWar(eOriginatingPlayer, bAggressor, GET_TEAM(eTeam).GetMaster(), bDefensivePact);
				return;
			}
		}
	}

	//is also catches the barbarians ...
	if (isAtWar(eTeam) || GET_TEAM(eTeam).isAtWar(GetID()))
		return;

#if defined(MOD_EVENTS_WAR_AND_PEACE)
	if (MOD_EVENTS_WAR_AND_PEACE)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_DeclareWar, eOriginatingPlayer, eTeam, bAggressor);
	}
	else
	{
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(GetID());
			args->Push(eTeam);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "DeclareWar", args.get(), bResult);
		}
#if defined(MOD_EVENTS_WAR_AND_PEACE)
	}
#endif

	// anyone who WANTED to declare war becomes aggressive now
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if (GET_PLAYER(eLoopPlayer).getTeam() == m_eID && GET_PLAYER(eLoopPlayer).isAlive())
		{
			for (int iTargetLoop = 0; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
			{
				PlayerTypes eLoopTarget = (PlayerTypes) iTargetLoop;
				if (GET_PLAYER(eLoopTarget).getTeam() == eTeam && GET_PLAYER(eLoopTarget).isAlive())
				{
					if (GET_PLAYER(eLoopPlayer).isMajorCiv())
					{
						CvDiplomacyAI* pDiplo = GET_PLAYER(eLoopPlayer).GetDiplomacyAI();
						pDiplo->SetWarProgressScore(eLoopTarget, /*100*/ GD_INT_GET(WAR_PROGRESS_INITIAL_VALUE));

						if (bAggressor && !bDefensivePact)
						{
							pDiplo->SetAggressor(eLoopTarget, true);
						}
						else
						{
							if (GET_PLAYER(eLoopTarget).isMajorCiv())
							{
								bool bHaveOffensiveOperation = GET_PLAYER(eLoopPlayer).HasAnyOffensiveOperationsAgainstPlayer(eLoopTarget);
								bool bWarApproach = !GET_PLAYER(eLoopPlayer).isHuman() && pDiplo->GetCivApproach(eLoopTarget) == CIV_APPROACH_WAR;

								if (bHaveOffensiveOperation || bWarApproach || pDiplo->IsWantsSneakAttack(eLoopTarget) || pDiplo->IsArmyInPlaceForAttack(eLoopTarget) || pDiplo->GetGlobalCoopWarAgainstState(eLoopTarget) >= COOP_WAR_STATE_PREPARING)
								{
									pDiplo->SetAggressor(eLoopTarget, true);
								}
							}
							else if (GET_PLAYER(eLoopTarget).isMinorCiv())
							{
								bool bHaveOperation = GET_PLAYER(eLoopPlayer).HasAnyOffensiveOperationsAgainstPlayer(eLoopTarget);
								bool bWarApproach = !GET_PLAYER(eLoopPlayer).isHuman() && pDiplo->GetCivApproach(eLoopTarget) == CIV_APPROACH_WAR;

								if (bHaveOperation || bWarApproach || pDiplo->IsArmyInPlaceForAttack(eLoopTarget))
								{
									pDiplo->SetAggressor(eLoopTarget, true);
								}
							}
						}
					}

					if (GET_PLAYER(eLoopTarget).isMajorCiv())
					{
						CvDiplomacyAI* pDiplo = GET_PLAYER(eLoopTarget).GetDiplomacyAI();
						pDiplo->SetWarProgressScore(eLoopPlayer, /*100*/ GD_INT_GET(WAR_PROGRESS_INITIAL_VALUE));

						if (GET_PLAYER(eLoopPlayer).isMajorCiv())
						{
							bool bHaveOffensiveOperation = GET_PLAYER(eLoopTarget).HasAnyOffensiveOperationsAgainstPlayer(eLoopPlayer);
							bool bWarApproach = !GET_PLAYER(eLoopTarget).isHuman() && pDiplo->GetCivApproach(eLoopPlayer) == CIV_APPROACH_WAR;

							if (bHaveOffensiveOperation || bWarApproach || pDiplo->IsWantsSneakAttack(eLoopPlayer) || pDiplo->IsArmyInPlaceForAttack(eLoopPlayer) || pDiplo->GetGlobalCoopWarAgainstState(eLoopPlayer) >= COOP_WAR_STATE_PREPARING)
							{
								pDiplo->SetAggressor(eLoopPlayer, true);
							}
						}
						else if (GET_PLAYER(eLoopPlayer).isMinorCiv() && pDiplo->GetCivApproach(eLoopPlayer) == CIV_APPROACH_WAR)
						{
							bool bHaveOperation = GET_PLAYER(eLoopTarget).HasAnyOffensiveOperationsAgainstPlayer(eLoopPlayer);
							bool bWarApproach = !GET_PLAYER(eLoopTarget).isHuman() && pDiplo->GetCivApproach(eLoopPlayer) == CIV_APPROACH_WAR;

							if (bHaveOperation || bWarApproach || pDiplo->IsArmyInPlaceForAttack(eLoopPlayer))
							{
								pDiplo->SetAggressor(eLoopPlayer, true);
							}
						}
					}
				}
			}
		}
	}

	//first cancel open borders and other diplomatic agreements
	GET_TEAM(eTeam).SetAllowsOpenBordersToTeam(m_eID, false);
	SetAllowsOpenBordersToTeam(eTeam, false);
	GC.getGame().GetGameDeals().DoCancelDealsBetweenTeams(GetID(), eTeam);
	CloseEmbassyAtTeam(eTeam);
	GET_TEAM(eTeam).CloseEmbassyAtTeam(m_eID);
	CancelResearchAgreement(eTeam);
	GET_TEAM(eTeam).CancelResearchAgreement(m_eID);
	EvacuateDiplomatsAtTeam(eTeam);
	GET_TEAM(eTeam).EvacuateDiplomatsAtTeam(m_eID);
	if (MOD_BALANCE_DEFENSIVE_PACTS_AGGRESSION_ONLY && IsHasDefensivePact(eTeam))
	{
		SetHasDefensivePact(eTeam, false);
		GET_TEAM(eTeam).SetHasDefensivePact(GetID(), false);
	}

	//Diplo Stuff ONLY triggers if we were the aggressor AND this wasn't a defensive pact/vassal (C4DF)
	if (!bDefensivePact && bAggressor && !GET_TEAM(eTeam).isMinorCiv())
	{
		cancelDefensivePacts();

		if (GET_TEAM(eTeam).GetLiberatedByTeam() == m_eID)
		{
			GET_TEAM(eTeam).SetLiberatedByTeam(NO_TEAM);
		}
	}

	GC.getGame().GetGameTrade()->DoAutoWarPlundering(m_eID, eTeam);
	GC.getGame().GetGameTrade()->CancelTradeBetweenTeams(m_eID, eTeam);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES)
	{
		// We declared war on our vassal!
		if (GET_TEAM(eTeam).GetMaster() == GetID())
		{
			// this guy is no longer our vassal
			GET_TEAM(eTeam).DoEndVassal(GetID(), true, false);

			PlayerTypes eLoopPlayer;
			PlayerTypes eLoopPlayer2;
			PlayerTypes eThirdParty;

			// Loop through all (living) players on this team
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
				{
					// Loop through all (living) players on the attacker's team - diplo penalty!
					for (int iPlayerLoop2 = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer2 = (PlayerTypes)iPlayerLoop2;

						if (GET_PLAYER(eLoopPlayer2).isAlive() && GET_PLAYER(eLoopPlayer2).getTeam() == GetID())
						{
							GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetPlayerBrokenVassalAgreement(eLoopPlayer2, true);
							GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eLoopPlayer2, 300);

							// Friends of the vassal - penalty to recent assistance!
							for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
							{
								eThirdParty = (PlayerTypes)iThirdPartyLoop;

								if (eThirdParty == eLoopPlayer || eThirdParty == eLoopPlayer2 || GET_PLAYER(eThirdParty).getTeam() == GET_PLAYER(eLoopPlayer2).getTeam())
									continue;

								if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eThirdParty) && GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsPlayerValid(eThirdParty) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsDoFAccepted(eThirdParty))
								{
									GET_PLAYER(eThirdParty).GetDiplomacyAI()->ChangeRecentAssistValue(eLoopPlayer2, 300);
								}
							}
						}
					}
				}
			}
		}
	}
#endif	

#if defined(MOD_GLOBAL_STACKING_RULES)
	// Bump Units out of places they shouldn't be
	GC.getMap().verifyUnitValidPlot();
#endif

	// Set initial war counters for all players
	for (int iLoop = 0; iLoop < MAX_PLAYERS; iLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iLoop;

		if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == GetID())
		{
			for (int iLoop2 = 0; iLoop2 < MAX_PLAYERS; iLoop2++)
			{
				PlayerTypes eLoopPlayer2 = (PlayerTypes) iLoop2;

				if (GET_PLAYER(eLoopPlayer2).isAlive() && GET_PLAYER(eLoopPlayer2).getTeam() == eTeam)
				{
					GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsAtWar(eLoopPlayer2, 0);
					GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsSinceCityCapture(eLoopPlayer2, 0);
					GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsAtPeace(eLoopPlayer2, 0);
					GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsAtWar(eLoopPlayer, 0);
					GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsSinceCityCapture(eLoopPlayer, 0);
					GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsAtPeace(eLoopPlayer, 0);
				}
			}
		}
	}

	setAtWar(eTeam, true, bAggressor);
	GET_TEAM(eTeam).setAtWar(GetID(), true, !bAggressor);

	// If any coop wars were preparing against either side, initiate them now.
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvDiplomacyAI* pDiplo = GET_PLAYER(eLoopPlayer).GetDiplomacyAI();

		if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).IsVassalOfSomeone())
		{
			if (GET_PLAYER(eLoopPlayer).getTeam() == GetID())
			{
				for (int iDefenderLoop = 0; iDefenderLoop < MAX_MAJOR_CIVS; iDefenderLoop++)
				{
					PlayerTypes eLoopDefender = (PlayerTypes) iDefenderLoop;

					if (pDiplo->IsPlayerValid(eLoopDefender) && GET_PLAYER(eLoopDefender).getTeam() == eTeam)
					{
						for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
						{
							PlayerTypes eThirdParty = (PlayerTypes) iThirdPartyLoop;

							if (eThirdParty == eLoopDefender || eThirdParty == eLoopPlayer || GET_PLAYER(eThirdParty).getTeam() == eTeam)
								continue;

							if (GET_PLAYER(eThirdParty).IsAtWarWith(eLoopPlayer))
								continue;

							if (!pDiplo->IsPlayerValid(eThirdParty, true))
								continue;

							// Make sure no vassals are involved
							if (GET_PLAYER(eLoopDefender).IsVassalOfSomeone() || GET_PLAYER(eThirdParty).IsVassalOfSomeone())
								continue;

							if (pDiplo->GetCoopWarState(eThirdParty, eLoopDefender) == COOP_WAR_STATE_PREPARING)
							{
								if (pDiplo->CanStartCoopWar(eThirdParty, eLoopDefender) || (pDiplo->IsAtWar(eLoopDefender) && GET_PLAYER(eThirdParty).IsAtWarWith(eLoopDefender)))
								{
									pDiplo->DoStartCoopWar(eThirdParty, eLoopDefender);
								}
								else
								{
									CvNotifications* pNotify = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotify)
									{
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_INVALID_TARGET");
										strText << GET_PLAYER(eLoopDefender).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eThirdParty).getCivilizationShortDescriptionKey();
										pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
									pNotify = GET_PLAYER(eThirdParty).GetNotifications();
									if (pNotify)
									{
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_INVALID_TARGET");
										strText << GET_PLAYER(eLoopDefender).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eLoopPlayer).getCivilizationShortDescriptionKey();
										pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}

									pDiplo->SetCoopWarState(eThirdParty, eLoopDefender, NO_COOP_WAR_STATE);
									GET_PLAYER(eThirdParty).GetDiplomacyAI()->SetCoopWarState(eLoopPlayer, eLoopDefender, NO_COOP_WAR_STATE);
								}
							}
						}
					}
				}
			}
			else if (GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
			{
				for (int iAttackerLoop = 0; iAttackerLoop < MAX_MAJOR_CIVS; iAttackerLoop++)
				{
					PlayerTypes eLoopAttacker = (PlayerTypes) iAttackerLoop;

					if (pDiplo->IsPlayerValid(eLoopAttacker) && GET_PLAYER(eLoopAttacker).getTeam() == GetID())
					{
						for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
						{
							PlayerTypes eThirdParty = (PlayerTypes) iThirdPartyLoop;

							if (eThirdParty == eLoopAttacker || eThirdParty == eLoopPlayer || GET_PLAYER(eThirdParty).getTeam() == GetID())
								continue;

							if (GET_PLAYER(eThirdParty).IsAtWarWith(eLoopPlayer))
								continue;

							if (!pDiplo->IsPlayerValid(eThirdParty, true))
								continue;

							// Make sure no vassals are involved
							if (GET_PLAYER(eLoopAttacker).IsVassalOfSomeone() || GET_PLAYER(eThirdParty).IsVassalOfSomeone())
								continue;

							if (pDiplo->GetCoopWarState(eThirdParty, eLoopAttacker) == COOP_WAR_STATE_PREPARING)
							{
								if (pDiplo->CanStartCoopWar(eThirdParty, eLoopAttacker) || (pDiplo->IsAtWar(eLoopAttacker) && GET_PLAYER(eThirdParty).IsAtWarWith(eLoopAttacker)))
								{
									pDiplo->DoStartCoopWar(eThirdParty, eLoopAttacker);
								}
								else
								{
									CvNotifications* pNotify = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotify)
									{
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_INVALID_TARGET");
										strText << GET_PLAYER(eLoopAttacker).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eThirdParty).getCivilizationShortDescriptionKey();
										pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
									pNotify = GET_PLAYER(eThirdParty).GetNotifications();
									if (pNotify)
									{
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_INVALID_TARGET");
										strText << GET_PLAYER(eLoopAttacker).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eLoopPlayer).getCivilizationShortDescriptionKey();
										pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}

									pDiplo->SetCoopWarState(eThirdParty, eLoopAttacker, NO_COOP_WAR_STATE);
									GET_PLAYER(eThirdParty).GetDiplomacyAI()->SetCoopWarState(eLoopPlayer, eLoopAttacker, NO_COOP_WAR_STATE);									
								}
							}
						}
					}
				}
			}
		}
	}

	for (int iAttackingPlayer = 0; iAttackingPlayer < MAX_MAJOR_CIVS; iAttackingPlayer++)
	{
		PlayerTypes eAttackingPlayer = (PlayerTypes)iAttackingPlayer;
		CvPlayerAI& kAttackingPlayer = GET_PLAYER(eAttackingPlayer);
		if(kAttackingPlayer.isAlive() && kAttackingPlayer.getTeam() == GetID())
		{
			for (int iDefendingPlayer = 0; iDefendingPlayer < MAX_MAJOR_CIVS; iDefendingPlayer++)
			{
				PlayerTypes eDefendingPlayer = (PlayerTypes)iDefendingPlayer;
				CvPlayerAI& kDefendingPlayer = GET_PLAYER(eDefendingPlayer);
				if(kDefendingPlayer.isAlive() && kDefendingPlayer.getTeam() == eTeam)
				{
					//Setup our defenses!
					if(!kAttackingPlayer.isHuman())
					{
						kAttackingPlayer.GetMilitaryAI()->SetupInstantDefenses(eDefendingPlayer);
					}
					if(!kDefendingPlayer.isHuman())
					{
						kDefendingPlayer.GetMilitaryAI()->SetupInstantDefenses(eAttackingPlayer);
					}

					// Update City Specializations.
					kDefendingPlayer.GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
#if defined(MOD_BALANCE_CORE)
					//Do a golden age on war if we can
					if(kAttackingPlayer.GetPlayerTraits()->IsGoldenAgeOnWar())
					{
						kAttackingPlayer.changeGoldenAgeTurns(kAttackingPlayer.getGoldenAgeLength(), kAttackingPlayer.GetGoldenAgeProgressMeter());
					}
					if(kDefendingPlayer.GetPlayerTraits()->IsGoldenAgeOnWar())
					{
						kDefendingPlayer.changeGoldenAgeTurns(kDefendingPlayer.getGoldenAgeLength(), kDefendingPlayer.GetGoldenAgeProgressMeter());
					}
					// Best unit on an improvement DOW?
					if(kAttackingPlayer.GetPlayerTraits()->IsBestUnitSpawnOnImprovementDOW())
					{
						CvCity* pLoopCity;
						int iLoop;
						for(pLoopCity = kAttackingPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kAttackingPlayer.nextCity(&iLoop))
						{
							kAttackingPlayer.GetPlayerTraits()->SpawnBestUnitsOnImprovementDOW(pLoopCity);
						}
					}
					if(kDefendingPlayer.GetPlayerTraits()->IsBestUnitSpawnOnImprovementDOW())
					{
						CvCity* pLoopCity;
						int iLoop;
						for(pLoopCity = kDefendingPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kDefendingPlayer.nextCity(&iLoop))
						{
							kDefendingPlayer.GetPlayerTraits()->SpawnBestUnitsOnImprovementDOW(pLoopCity);
						}
					}
					// Get a free unit on DOW?
					for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
					{
						const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
						CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
						if(pkUnitClassInfo)
						{
							CvPlot* pNewUnitPlot;
							UnitTypes eLoopUnit;
							int iDefaultAI;
							int iUnitAttackerClass = kAttackingPlayer.GetPlayerTraits()->GetFreeUnitClassesDOW(eUnitClass);
							int iUnitDefenderClass = kDefendingPlayer.GetPlayerTraits()->GetFreeUnitClassesDOW(eUnitClass);
							for(int iJ = 0; iJ < iUnitAttackerClass != NULL; iJ++)
							{
								eLoopUnit = kAttackingPlayer.GetSpecificUnitType(eUnitClass);
								iDefaultAI = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetDefaultUnitAIType();
								bool bWarOnly = GC.GetGameUnits()->GetEntry(eLoopUnit)->IsWarOnly();
								bool bCombat = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetCombat() > 0;
								bool bPrereqTech = GET_TEAM(kAttackingPlayer.getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.GetGameUnits()->GetEntry(eLoopUnit)->GetPrereqAndTech());
								if(!bCombat)
								{
									pNewUnitPlot = kAttackingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
								}
								else if(bWarOnly && bPrereqTech)
								{
									pNewUnitPlot = kAttackingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
								}
								else
								{
									if(kAttackingPlayer.canTrainUnit(eLoopUnit, false, false, true))
									{
										pNewUnitPlot = kAttackingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
									}
								}
							}
							for(int iK = 0; iK < iUnitDefenderClass != NULL; iK++)
							{
								eLoopUnit = kDefendingPlayer.GetSpecificUnitType(eUnitClass);
								iDefaultAI = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetDefaultUnitAIType();
								bool bWarOnly = GC.GetGameUnits()->GetEntry(eLoopUnit)->IsWarOnly();
								bool bCombat = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetCombat() > 0;
								bool bPrereqTech = GET_TEAM(kDefendingPlayer.getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.GetGameUnits()->GetEntry(eLoopUnit)->GetPrereqAndTech());
								if(!bCombat)
								{
									pNewUnitPlot = kDefendingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
								}
								else if(bWarOnly && bPrereqTech)
								{
									pNewUnitPlot = kDefendingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
								}
								else
								{
									if(kDefendingPlayer.canTrainUnit(eLoopUnit, false, false, true))
									{
										pNewUnitPlot = kDefendingPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
									}
								}
							}
						}
					}
#endif
				}
			}
		}
	}

	//are we an aggressor?
	//These only trigger on the 'first' major declaration - no chain declarations.
	if (bAggressor && !bDefensivePact)
	{
		//DPs and Vassal civs - we have to DOW them as well!
		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (!GET_TEAM((TeamTypes)iI).isAlive())
				continue;
			if (GET_TEAM((TeamTypes)iI).isMinorCiv())
				continue;

			//Defensive pacts and vassals trigger here.
			if (GET_TEAM((TeamTypes)iI).IsHasDefensivePact(eTeam) || GET_TEAM((TeamTypes)iI).IsVassal(eTeam))
			{
				GET_TEAM(GetID()).DoDeclareWar(eOriginatingPlayer, true, (TeamTypes)iI, /*bDefensivePact*/ true);
			}
		}
	}

	//Secondary civs that will declare war along with us
	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (!GET_TEAM((TeamTypes)iI).isAlive())
			continue;
		if (GET_TEAM((TeamTypes)iI).isMinorCiv())
			continue;
				
		if (MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM((TeamTypes)iI).IsVassal(GetID()))
		{
			GET_TEAM((TeamTypes)iI).DoDeclareWar(eOriginatingPlayer, bAggressor, eTeam, /*bDefensivePact*/ true);
		}
	}

	//Secondary check for vassals.
	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (!GET_TEAM((TeamTypes)iI).isAlive())
			continue;

		if (GET_TEAM((TeamTypes)iI).isMinorCiv())
			continue;

		if (MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM((TeamTypes)iI).IsVassal(eTeam))
		{
			GET_TEAM((TeamTypes)iI).DoDeclareWar(eOriginatingPlayer, bAggressor, GetID(), /*bDefensivePact*/ true);
		}

		if (MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM((TeamTypes)iI).IsVassal(GetID()))
		{
			GET_TEAM((TeamTypes)iI).DoDeclareWar(eOriginatingPlayer, bAggressor, eTeam, /*bDefensivePact*/ true);
		}
	}

	// One shot things
	DoNowAtWarOrPeace(eTeam, true);
	GET_TEAM(eTeam).DoNowAtWarOrPeace(GetID(), true);

	// Meet the team if we haven't already
	meet(eTeam, false);

	// Update interface stuff
	if((GetID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
	{
		DLLUI->setDirty(Score_DIRTY_BIT, true);
		DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	}

	if(GC.getGame().isFinalInitialized())
	{
		// Message everyone about what happened
		if(!isBarbarian() && !(GET_TEAM(eTeam).isBarbarian()))
		{
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iI;

				if((GET_PLAYER(ePlayer).isAlive() || GET_PLAYER(ePlayer).isObserver()) && GET_PLAYER(ePlayer).GetNotifications())
				{
					// If this declaration is a minor following a major's declaration, don't send out these individual notifications
					if(!bMinorAllyPact)
					{
						// Players on team that declared
						if(GET_PLAYER(ePlayer).getTeam() == GetID())
						{
							if(ePlayer == GC.getGame().getActivePlayer())
							{
								locString = Localization::Lookup("TXT_KEY_MISC_YOU_DECLARED_WAR_ON");
								locString << GET_TEAM(eTeam).getName().GetCString();
								DLLUI->AddMessage(0, (ePlayer), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), locString.toUTF8()/*, "AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")*/);
							}
						}
						// Players on team that got declared on
						else if(GET_PLAYER(ePlayer).getTeam() == eTeam)
						{
							locString = Localization::Lookup("TXT_KEY_MISC_DECLARED_WAR_ON_YOU");
							locString << getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_WAR_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID());
						}
						// Players that are on neither team, but know both parties
						else if((GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam)) || GET_PLAYER(ePlayer).isObserver())
						{
							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_DECLARED_WAR");
							locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_WAR, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), eTeam);
						}
					}
				}
			}

			locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_DECLARES_WAR");
			locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), locString.toUTF8(), -1, -1);
		}
	}

	// Major is declaring War
	if (isMajorCiv())
	{
		// Update what every Major Civ sees
		for (int iMajorCivLoop = 0; iMajorCivLoop < MAX_MAJOR_CIVS; iMajorCivLoop++)
		{
			PlayerTypes eAttacker = (PlayerTypes) iMajorCivLoop;

			if (GET_PLAYER(eAttacker).getTeam() == GetID() && GET_PLAYER(eAttacker).isAlive())
			{
				// All other major civs should take note of this war declaration
				for (int iMajorCivLoop2 = 0; iMajorCivLoop2 < MAX_MAJOR_CIVS; iMajorCivLoop2++)
				{
					PlayerTypes eMajor = (PlayerTypes) iMajorCivLoop2;

					// If we've met the player, update Diplo stuff
					if (GET_PLAYER(eMajor).getTeam() != GetID() && GET_PLAYER(eMajor).GetDiplomacyAI()->IsHasMet(eAttacker, /*bMyTeamIsValid*/ true))
					{
						GET_PLAYER(eMajor).GetDiplomacyAI()->DoPlayerDeclaredWarOnSomeone(eAttacker, eTeam, bDefensivePact);
					}
				}
			}
		}
		// Second loop, reevaluate diplomacy
		for (int iMajorCivLoop = 0; iMajorCivLoop < MAX_MAJOR_CIVS; iMajorCivLoop++)
		{
			PlayerTypes eMajor = (PlayerTypes) iMajorCivLoop;
			TeamTypes eMajorTeam = GET_PLAYER(eMajor).getTeam();

			// On the attacking team
			if (eMajorTeam == GetID())
			{
				vector<PlayerTypes> v = GET_PLAYER(eMajor).GetDiplomacyAI()->GetAllValidMajorCivs();
				GET_PLAYER(eMajor).GetDiplomacyAI()->DoReevaluatePlayers(v, true);
			}
			// Has met the attacking team
			else if (isHasMet(eMajorTeam))
			{
				vector<PlayerTypes> v = getPlayers();
				GET_PLAYER(eMajor).GetDiplomacyAI()->DoReevaluatePlayers(v, true);
			}
		}

		// Declaring war on Minor
		for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;

			// Now loop through all players on this team to nullify Quests for them
			if (GET_PLAYER(eMinor).getTeam() == eTeam && GET_PLAYER(eMinor).isAlive())
			{
				// Increment # of Minors this player has attacked - note that this will be called EACH time a team declares war on a Minor,
				// even the same Minor multiple times.  The current design assumes that once a player is at war with a Minor it's forever, so this is fine
				//antonjs: consider: this statement is no longer valid, since current design allows peace to be made; update the implementation
				if (!isMinorCiv() && !bDefensivePact && bAggressor && isAtWar(eTeam))
				{
					ChangeNumMinorCivsAttacked(1);
					GET_PLAYER((PlayerTypes)iMinorCivLoop).GetMinorCivAI()->DoTeamDeclaredWarOnMe(GetID());
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::DoNowAtWarOrPeace(TeamTypes eTeam, bool bWar)
{
#if defined(MOD_BALANCE_CORE)
	ClearWarDeclarationCache();
#endif

	// Major is at war with a minor
	if(isMinorCiv())
	{
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;

			if(GET_PLAYER(eMinor).getTeam() == GetID())
			{
				if(GET_PLAYER(eMinor).isAlive())
				{
					if (bWar)
					{
						if(GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() != NO_PLAYER)
						{
							if(GET_TEAM(eTeam).isAtWar(GET_PLAYER(GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly()).getTeam()))
							{
								GET_PLAYER(eMinor).GetMinorCivAI()->DoNowAtWarWithTeam(eTeam);
							}
						}
					}

					else
						GET_PLAYER(eMinor).GetMinorCivAI()->DoNowPeaceWithTeam(eTeam);
				}
			}
		}
	}

	if(bWar)
	{
		// Loop through players on this team
		for(int iPlayerCivLoop = 0; iPlayerCivLoop < MAX_MAJOR_CIVS; iPlayerCivLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes) iPlayerCivLoop;

			if(!GET_PLAYER(ePlayer).isAlive())
				continue;

			if(GET_PLAYER(ePlayer).getTeam() != GetID())
				continue;

			GET_PLAYER(ePlayer).SetDangerPlotsDirty();
			GET_PLAYER(ePlayer).UpdateReligion();

			// ******************************
			// Our minor civ allies declare war on eTeam
			// ******************************

			vector<PlayerTypes> veMinorAllies;
			for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;

				// Must be alive
				if (!GET_PLAYER(eMinor).isAlive())
					continue;

				if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
				{
					// Don't declare war on self! (just in case)
					if (GET_PLAYER(eMinor).getTeam() != eTeam)
					{
						// Match war state
						if (GET_TEAM(eTeam).isAtWar(GET_PLAYER(ePlayer).getTeam()))
						{
							GET_TEAM(GET_PLAYER(eMinor).getTeam()).DoDeclareWar(eMinor, false, eTeam, /*bDefensivePact*/ false, /*bMinorAllyPact*/ true);
						}

						// Add to vector for notification sent out
						veMinorAllies.push_back(eMinor);
					}
				}
			}

			// Notifications about minor allies that join the war against a major civ
			if (!veMinorAllies.empty())
			{
				if (!GET_TEAM(eTeam).isMinorCiv())
				{
					Localization::String strTemp;

					// Notification for us...allies got our back!
					Localization::String strOurAlliesSummary = Localization::Lookup("TXT_KEY_MISC_YOUR_MINOR_ALLIES_DECLARED_WAR_SUMMARY");
					strTemp = Localization::Lookup("TXT_KEY_MISC_YOUR_MINOR_ALLIES_DECLARED_WAR");
					strTemp << GET_TEAM(eTeam).getName().GetCString();
					CvString strOurAlliesMessage = strTemp.toUTF8();

					// Notification for players on the other team
					Localization::String strTheirEnemiesSummary = Localization::Lookup("TXT_KEY_MISC_MINOR_ALLIES_DECLARED_WAR_ON_YOU_SUMMARY");
					strTemp = Localization::Lookup("TXT_KEY_MISC_MINOR_ALLIES_DECLARED_WAR_ON_YOU");
					strTemp << getName().GetCString();
					CvString strTheirEnemiesMessage = strTemp.toUTF8();

					for(uint iMinorCivLoop = 0; iMinorCivLoop < veMinorAllies.size(); iMinorCivLoop++)
					{
						PlayerTypes eMinor = veMinorAllies[iMinorCivLoop];
						strTemp = Localization::Lookup(GET_TEAM(GET_PLAYER(eMinor).getTeam()).getName().GetCString());
						strOurAlliesMessage = strOurAlliesMessage + "[NEWLINE]" + strTemp.toUTF8();
						strTheirEnemiesMessage = strTheirEnemiesMessage + "[NEWLINE]" + strTemp.toUTF8();
					}

					GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_WAR, strOurAlliesMessage, strOurAlliesSummary.toUTF8(), -1, -1, veMinorAllies.front(), eTeam);

					for(int iOtherPlayerCivLoop = 0; iOtherPlayerCivLoop < MAX_MAJOR_CIVS; iOtherPlayerCivLoop++)
					{
						PlayerTypes eOtherPlayer = (PlayerTypes) iOtherPlayerCivLoop;

						if(!GET_PLAYER(eOtherPlayer).isAlive())
							continue;

						if(GET_PLAYER(eOtherPlayer).getTeam() != eTeam)
							continue;

						GET_PLAYER(eOtherPlayer).GetNotifications()->Add(NOTIFICATION_WAR_ACTIVE_PLAYER, strTheirEnemiesMessage, strTheirEnemiesSummary.toUTF8(), -1, -1, veMinorAllies.front());
					}

				}
			}
		}
	}

	CvPlayerManager::Refresh(true);
}

//	------------------------------------------------------------------------------------------------
void CvTeam::makePeace(TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification, PlayerTypes eOriginatingPlayer)
{
	DoMakePeace(eOriginatingPlayer, true, eTeam, bBumpUnits, bSuppressNotification);
}

//	------------------------------------------------------------------------------------------------
//	The make peace handler, can be called recursively
void CvTeam::DoMakePeace(PlayerTypes eOriginatingPlayer, bool bPacifier, TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification)
{
	CvString strBuffer;

	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");

	if(isAtWar(eTeam))
	{
		setAtWar(eTeam, false, bPacifier);
		GET_TEAM(eTeam).setAtWar(GetID(), false, !bPacifier);

		// Set initial peace counters for all players
		for (int iLoop = 0; iLoop < MAX_PLAYERS; iLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iLoop;

			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == GetID())
			{
				for (int iLoop2 = 0; iLoop2 < MAX_PLAYERS; iLoop2++)
				{
					PlayerTypes eLoopPlayer2 = (PlayerTypes) iLoop2;

					if (GET_PLAYER(eLoopPlayer2).isAlive() && GET_PLAYER(eLoopPlayer2).getTeam() == eTeam)
					{
						GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsAtWar(eLoopPlayer2, 0);
						GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsSinceCityCapture(eLoopPlayer2, 0);
						GET_PLAYER(eLoopPlayer).SetPlayerNumTurnsAtPeace(eLoopPlayer2, 0);
						GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsAtWar(eLoopPlayer, 0);
						GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsSinceCityCapture(eLoopPlayer, 0);
						GET_PLAYER(eLoopPlayer2).SetPlayerNumTurnsAtPeace(eLoopPlayer, 0);
					}
				}
			}
		}

#if defined(MOD_BALANCE_CORE)
		//Secondary major declarations
		for(int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(MOD_DIPLOMACY_CIV4_FEATURES)
				{
					//Are we a vassal of the from player?
					if(GET_TEAM((TeamTypes)iI).IsVassal(GetID()))
					{
						GET_TEAM((TeamTypes)iI).DoMakePeace(eOriginatingPlayer, true, eTeam, true, false);
					}
					//Are we a vassal of the to player?
					else if(GET_TEAM((TeamTypes)iI).IsVassal(eTeam))
					{
						GET_TEAM((TeamTypes)iI).DoMakePeace(eOriginatingPlayer, true, GetID(), true, false);
					}
				}
				if(GET_TEAM((TeamTypes)iI).isMinorCiv())
				{
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
						if(kPlayer.getTeam() == (TeamTypes)iI && kPlayer.isAlive())
						{
							for(int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_CIV_PLAYERS; iPlayerLoop2++)
							{
								CvPlayerAI& kPlayer2 = GET_PLAYER((PlayerTypes) iPlayerLoop2);
								if(kPlayer2.getTeam() == GetID() && kPlayer.isAlive())
								{
									if(kPlayer.GetMinorCivAI()->IsAllies((PlayerTypes)iPlayerLoop2))
									{
										GET_TEAM((TeamTypes)iI).DoMakePeace((PlayerTypes)iPlayerLoop, true, eTeam, true, false);
									}
								}
								else if(kPlayer2.getTeam() == eTeam && kPlayer.isAlive())
								{
									if(kPlayer.GetMinorCivAI()->IsAllies((PlayerTypes)iPlayerLoop2))
									{
										GET_TEAM((TeamTypes)iI).DoMakePeace((PlayerTypes)iPlayerLoop, true, GetID(), true, false);
									}
								}
							}
						}
					}
				}
			}
		}
#endif

#if defined(MOD_EVENTS_WAR_AND_PEACE)
		if (MOD_EVENTS_WAR_AND_PEACE) 
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MakePeace, eOriginatingPlayer, eTeam, bPacifier);
		}
#else
		else 
		{
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(GetID());
				args->Push(eTeam);

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "MakePeace", args.get(), bResult);
			}
		}
#endif

		// One shot things
		DoNowAtWarOrPeace(eTeam, false);
		GET_TEAM(eTeam).DoNowAtWarOrPeace(GetID(), false);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if(MOD_DIPLOMACY_CIV4_FEATURES)
		{
			DoUpdateVassalWarPeaceRelationships();
		}
#endif

		// Move Units that shouldn't be in each others' territory any more
		if(bBumpUnits)
		{
			GC.getMap().verifyUnitValidPlot();
		}

		// Both of us have now made a peace treaty.  Keep track of this in case either one breaks the agreement
		int iCurrentTurn = GC.getGame().getElapsedGameTurns();
		SetTurnMadePeaceTreatyWithTeam(eTeam, iCurrentTurn);
		GET_TEAM(eTeam).SetTurnMadePeaceTreatyWithTeam(GetID(), iCurrentTurn);

		TeamTypes eTeamWeMadePeaceWith = eTeam;

		if(!isMinorCiv())
		{
			// Made peace with a minor - see if we have allied minors which should also make peace
			if(GET_TEAM(eTeamWeMadePeaceWith).isMinorCiv())
			{
				PlayerTypes eOurMinor;
				int iMinorLoop;

				PlayerTypes eOurPlayer;
				int iPlayerLoop;

				PlayerTypes eThirdParty;
				int iThirdPartyLoop;

				PlayerTypes eMakingPeaceWithMinor;
				int iMakingPeaceWithMinorLoop;

				bool bPeaceBlocked;

				// Loop through all players to see if they're on our team
				for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					eOurPlayer = (PlayerTypes) iPlayerLoop;

					// Not on this team
					if(GET_PLAYER(eOurPlayer).getTeam() != GetID())
						continue;

					vector<PlayerTypes> veMinorAllies;

					// Loop through minors to see if they're allied with us
					for(iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
					{
						eOurMinor = (PlayerTypes) iMinorLoop;

						// Minor not alive
						if(!GET_PLAYER(eOurMinor).isAlive())
							continue;

						// Allied with us
						if(GET_PLAYER(eOurMinor).GetMinorCivAI()->GetAlly() == eOurPlayer)
						{
							bPeaceBlocked = false;

							// Now... see if there's another major allied to the minor we just made peace with, which would block OUR allied minor from making peace with him
							for(iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
							{
								eThirdParty = (PlayerTypes) iThirdPartyLoop;

								for(iMakingPeaceWithMinorLoop = MAX_MAJOR_CIVS; iMakingPeaceWithMinorLoop < MAX_CIV_PLAYERS; iMakingPeaceWithMinorLoop++)
								{
									eMakingPeaceWithMinor = (PlayerTypes) iMakingPeaceWithMinorLoop;

									// Not on the team we're making peace with
									if(!GET_PLAYER(eMakingPeaceWithMinor).getTeam() == eTeamWeMadePeaceWith)
										continue;

									// Minor not alive
									if(!GET_PLAYER(eMakingPeaceWithMinor).isAlive())
										continue;

									if(GET_PLAYER(eMakingPeaceWithMinor).GetMinorCivAI()->GetAlly() == eThirdParty)
									{
										if(GET_TEAM(GET_PLAYER(eThirdParty).getTeam()).isAtWar(GET_PLAYER(eOurMinor).getTeam()))
										{
											bPeaceBlocked = true;
											break;
										}
									}
								}
							}

							// Not at permanent war with this team
							if(!bPeaceBlocked)
							{
								if(!GET_PLAYER(eOurMinor).GetMinorCivAI()->IsPermanentWar(eTeamWeMadePeaceWith))
								{
									GET_TEAM(GET_PLAYER(eOurMinor).getTeam()).DoMakePeace(eOurMinor, bPacifier, eTeamWeMadePeaceWith, /*bBumpUnits*/ true, /*bSuppressNotification*/ true);
									veMinorAllies.push_back(eOurMinor);
								}
							}
						}
					}

					// Send out a notification to us telling which of our minor allies made peace
					if(!veMinorAllies.empty())
					{
						Localization::String strTemp = Localization::Lookup("TXT_KEY_MISC_YOUR_MINOR_ALLIES_MADE_PEACE");
						Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_YOUR_MINOR_ALLIES_MADE_PEACE_SUMMARY");
						strTemp << GET_TEAM(eTeam).getName().GetCString();
						CvString strMessage = strTemp.toUTF8();

						for(uint iMinorCivLoop = 0; iMinorCivLoop < veMinorAllies.size(); iMinorCivLoop++)
						{
							PlayerTypes eMinor = veMinorAllies[iMinorCivLoop];
							strTemp = Localization::Lookup(GET_TEAM(GET_PLAYER(eMinor).getTeam()).getName().GetCString());
							strMessage = strMessage + "[NEWLINE]" + strTemp.toUTF8();
						}

						if(GET_PLAYER(eOurPlayer).GetNotifications())
							GET_PLAYER(eOurPlayer).GetNotifications()->Add(NOTIFICATION_PEACE, strMessage, strSummary.toUTF8(), -1, -1, veMinorAllies.front(), eTeam);
					}
				}
			}
		}

		// Update Interface
		if((GetID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
			DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
		}

		// What does it mean when we make peace
		CvPlayer* pOurPlayer;
		PlayerTypes eOurPlayer;
		for(int iOurPlayerLoop = 0; iOurPlayerLoop < MAX_CIV_PLAYERS; iOurPlayerLoop++)
		{
			eOurPlayer = (PlayerTypes) iOurPlayerLoop;
			pOurPlayer = &GET_PLAYER(eOurPlayer);

			if(pOurPlayer->isAlive())
			{
				// Our Team
				if (pOurPlayer->getTeam() == GetID())
				{
					pOurPlayer->GetDiplomacyAI()->DoWeMadePeaceWithSomeone(eTeam);
					pOurPlayer->GetMilitaryAI()->LogPeace(eTeam);	// This is not quite correct, but it'll work well enough for AI testing
				}
				// Their Team
				else if (pOurPlayer->getTeam() == eTeam)
				{
					pOurPlayer->GetDiplomacyAI()->DoWeMadePeaceWithSomeone(GetID());
					pOurPlayer->GetMilitaryAI()->LogPeace(GetID());	// This is not quite correct, but it'll work well enough for AI testing
				}
			}
		}

		Localization::String locString;

		// Text stuff
		if(!bSuppressNotification)
		{
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iI;

				if ((GET_PLAYER(ePlayer).isAlive() || GET_PLAYER(ePlayer).isObserver()) && GET_PLAYER(ePlayer).GetNotifications())
				{
					if (GET_PLAYER(ePlayer).getTeam() == GetID())
					{
						locString = Localization::Lookup("TXT_KEY_MISC_YOU_MADE_PEACE_WITH");
						locString << GET_TEAM(eTeam).getName().GetCString();
						GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID());
					}
					else if (GET_PLAYER(ePlayer).getTeam() == eTeam)
					{
						locString = Localization::Lookup("TXT_KEY_MISC_YOU_MADE_PEACE_WITH");
						locString << getName().GetCString();
						GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID());
					}
					else if ((GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam)) || GET_PLAYER(ePlayer).isObserver())
					{
						locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_MADE_PEACE");
						locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
						GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), eTeam);
					}
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::meet(TeamTypes eTeam, bool bSuppressMessages)
{
	//can happen with observer?
	if(!isAlive())
		return;

	if(!isHasMet(eTeam))
	{
		makeHasMet(eTeam, bSuppressMessages);
		GET_TEAM(eTeam).makeHasMet(GetID(), bSuppressMessages);

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args(2);
			args->Push(eTeam);
			args->Push(GetID());

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "TeamMeet", args.get(), bResult);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getPower() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kLoopPlayer.isAlive())
		{
			if(kLoopPlayer.getTeam() == GetID())
			{
				iCount += kLoopPlayer.getPower();
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getDefensivePower() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if(kLoopTeam.isAlive())
		{
			if(GetID() == iI || IsHasDefensivePact((TeamTypes)iI))
			{
				iCount += kLoopTeam.getPower();
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getEnemyPower() const
{
	int iCount = 0;

	for(int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if(kLoopTeam.isAlive())
		{
			if(GetID() != iI && isAtWar((TeamTypes)iI))
			{
				iCount += kLoopTeam.getPower();
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getNumNukeUnits() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kLoopPlayer.isAlive())
		{
			if(kLoopPlayer.getTeam() == GetID())
			{
				iCount += kLoopPlayer.getNumNukeUnits();
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
/// Who will this team vote for!
TeamTypes CvTeam::GetTeamVotingForInDiplo() const
{
	TeamTypes eVoteTeam = NO_TEAM;
	
	if(isBarbarian())
	{
		// Barbarians do not vote!
		CvAssertMsg(false, "Barbarian team should not be voting for diplo victory. Please send Anton your save file and verison.");
	}
	else if(isMinorCiv())
	{
		// Minor civs vote for their favored nation (liberator or ally)
		if(GetLiberatedByTeam() != NO_TEAM && GET_TEAM(GetLiberatedByTeam()).isAlive())
		{
			eVoteTeam = GetLiberatedByTeam();
		}
		else
		{
			PlayerTypes eAlly = GET_PLAYER(getLeaderID()).GetMinorCivAI()->GetAlly();
			if(eAlly != NO_PLAYER && GET_PLAYER(eAlly).isAlive())
				eVoteTeam = GET_PLAYER(eAlly).getTeam();
		}
	}
	else
	{
		// Major civs vote for other majors based on opinion weight (our team leader towards their team leader)
		CvWeightedVector<TeamTypes> veVoteCandidates;
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eTeamLoop = (TeamTypes) iTeamLoop;
			if (GetID() != eTeamLoop && GET_TEAM(eTeamLoop).isAlive() && !GET_TEAM(eTeamLoop).isMinorCiv() && !GET_TEAM(eTeamLoop).isBarbarian() && isHasMet(eTeamLoop))
			{
				PlayerTypes eLeaderLoop = (PlayerTypes) GET_TEAM(eTeamLoop).getLeaderID();
				CvAssertMsg(getLeaderID() != NO_PLAYER, "Our team leader ID should not be NO_PLAYER. Please send Anton your save file and version.");
				CvAssertMsg(eLeaderLoop != NO_PLAYER, "Other team leader ID should not be NO_PLAYER. Please send Anton your save file and version.");
				if (getLeaderID() != NO_PLAYER && eLeaderLoop != NO_PLAYER)
				{
					int kiBase = 10000;
					
					// What is our leader's opinion of the other team's leader? Remember, bad opinion is positive, good opinion is negative.
					int iOpinion = GET_PLAYER(getLeaderID()).GetDiplomacyAI()->CalculateCivOpinionWeight(eLeaderLoop);
					if (isAtWar(eTeamLoop))
						iOpinion = kiBase; // Don't vote for someone we are at war with, if we can help it

					// Weight cannot be negative. This calculation makes good opinions > 10000 and bad opinions < 10000.
					int iWeight = kiBase - iOpinion;
					if (iWeight < 0)
						iWeight = 0;

					veVoteCandidates.push_back(eTeamLoop, iWeight);
				}
			}
		}

		if (veVoteCandidates.size() > 0)
		{
			// Our most favored other team ends up at the top after sorting
			veVoteCandidates.SortItems();
			int iTopWeight = veVoteCandidates.GetWeight(0);

			// If there is a tie at the top, choose randomly from those that tied
			int iNumAtTop = 0;
			for (int iIndex = 0; iIndex < veVoteCandidates.size(); iIndex++)
			{
				int iWeight = veVoteCandidates.GetWeight(iIndex);
				CvAssertMsg(iWeight <= iTopWeight, "Vote opinion weight should not be higher than the top team's weight! Please send Anton your save file and version.");
				if (iWeight >= iTopWeight)
				{
					iNumAtTop++;
				}
				else
				{
					break;
				}
			}
			CvAssertMsg(iNumAtTop > 0, "Should have at least one vote candidate at the top of the list for consideration. Please send Anton your save file and version.");
			CvAssertMsg(iNumAtTop <= veVoteCandidates.size(), "Should not have more top vote candidates than there are total candidates. Please send Anton your save file and version.");
			if (iNumAtTop > 0 && iNumAtTop <= veVoteCandidates.size())
			{
				RandomNumberDelegate randFn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
				eVoteTeam = veVoteCandidates.ChooseFromTopChoices(iNumAtTop, &randFn, "Tie for most favored other team to vote for. Rolling to choose.");
			}
		}
	}

	// If all else fails, vote for ourselves
	if (eVoteTeam == NO_TEAM)
		eVoteTeam = GetID();

	return eVoteTeam;
}

//	--------------------------------------------------------------------------------
/// How many votes are we likely to get from City-State allies in the upcoming election?
int CvTeam::GetProjectedVotesFromMinorAllies() const
{
	int iVotes = 0;
	if (isAlive())
	{
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eTeamLoop = (TeamTypes) iTeamLoop;
			CvTeam* pTeamLoop = &GET_TEAM(eTeamLoop);
			if (pTeamLoop->isAlive() && pTeamLoop->isMinorCiv() && !pTeamLoop->isBarbarian())
			{
				// Minor civ team votes are definite things, given the situation doesn't change
				if (GET_TEAM(eTeamLoop).GetTeamVotingForInDiplo() == GetID())
				{
					// Liberated minors are handled elsewhere				
					if (GetLiberatedByTeam() != GetID())
					{
						iVotes++;
					}
				}
			}
		}
	}

	return iVotes;
}

//	--------------------------------------------------------------------------------
/// How many votes are we likely to get from City-States we have liberated in the upcoming election?
int CvTeam::GetProjectedVotesFromLiberatedMinors() const
{
	int iVotes = 0;
	if (isAlive())
	{
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eTeamLoop = (TeamTypes) iTeamLoop;
			CvTeam* pTeamLoop = &GET_TEAM(eTeamLoop);
			if (pTeamLoop->isAlive() && pTeamLoop->isMinorCiv() && !pTeamLoop->isBarbarian())
			{
				// Minor civ team votes are definite things, given the situation doesn't change
				if (GET_TEAM(eTeamLoop).GetTeamVotingForInDiplo() == GetID())
				{
					// Liberated by us?
					if (GetLiberatedByTeam() == GetID())
					{
						iVotes++;
					}
				}
			}
		}
	}

	return iVotes;
}

//	--------------------------------------------------------------------------------
/// How many votes are we likely to get from major Civilizations in the upcoming election?
int CvTeam::GetProjectedVotesFromCivs() const
{
	int iVotes = 0;

	if (isAlive())
	{
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eTeamLoop = (TeamTypes) iTeamLoop;
			CvTeam* pTeamLoop = &GET_TEAM(eTeamLoop);
			if (pTeamLoop->isAlive() && !pTeamLoop->isMinorCiv() && !pTeamLoop->isBarbarian())
			{
				// Did they vote for us last time?
				if (GC.getGame().GetPreviousVoteCast(eTeamLoop) == GetID())
					iVotes++;
			}
		}
	}

	return iVotes;
}

//	--------------------------------------------------------------------------------
/// How many total votes are we likely to get in the upcoming election?
int CvTeam::GetTotalProjectedVotes() const
{
	int iVotes = 0;

	if (isAlive())
	{
		// Majors
		iVotes += GetProjectedVotesFromCivs();

		// Minor Allies
		iVotes += GetProjectedVotesFromMinorAllies();

		// Liberated Minors
		iVotes += GetProjectedVotesFromLiberatedMinors();

		// UN
		if (IsHomeOfUnitedNations())
		{
			int iVotesFromUN = /*1*/ GD_INT_GET(OWN_UNITED_NATIONS_VOTE_BONUS);
			iVotes += iVotesFromUN;
		}
	}

	return iVotes;
}

//	--------------------------------------------------------------------------------
/// How many votes should we get?
/// DEPRECATED: Use GetTotalProjectedVotes() instead.  Votes are no longer perfectly known ahead of time.
int CvTeam::GetTotalSecuredVotes() const
{
	return GetTotalProjectedVotes();
}

//	--------------------------------------------------------------------------------
int CvTeam::getAtWarCount(bool bIgnoreMinors) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
			{
				if(isAtWar((TeamTypes)iI))
				{
					CvAssert(iI != GetID());
					iCount++;
				}
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getHasMetCivCount(bool bIgnoreMinors) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(iI != GetID())
			{
				if(!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
				{
					if(isHasMet((TeamTypes)iI))
					{
						CvAssert(iI != GetID());
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::hasMetHuman() const
{
	int iI;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(iI != GetID())
			{
				if(GET_TEAM((TeamTypes)iI).isHuman())
				{
					if(isHasMet((TeamTypes)iI))
					{
						CvAssert(iI != GetID());
						return true;
					}
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvTeam::getDefensivePactCount(TeamTypes eTeam) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(iI != m_eID)
			{
				if(IsHasDefensivePact((TeamTypes)iI))
				{
					if(NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvTeam::getUnitClassMaking(UnitClassTypes eUnitClass) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == m_eID)
			{
				iCount += kPlayer.getUnitClassMaking(eUnitClass);
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}


//	--------------------------------------------------------------------------------
int CvTeam::getBuildingClassMaking(BuildingClassTypes eBuildingClass) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == m_eID)
			{
				iCount += kPlayer.getBuildingClassMaking(eBuildingClass);
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}

//	--------------------------------------------------------------------------------
int CvTeam::countNumUnitsByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == m_eID)
			{
				iCount += pArea->getUnitsPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::countNumCitiesByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == m_eID)
			{
				iCount += pArea->getCitiesPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::countTotalPopulationByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == m_eID)
			{
				iCount += pArea->getPopulationPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::countEnemyDangerByArea(CvArea* pArea) const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->area() == pArea)
			{
				if(pLoopPlot->getTeam() == GetID())
				{
					iCount += pLoopPlot->plotCount(PUF_canDefendEnemy, getLeaderID(), false, NO_PLAYER, NO_TEAM, PUF_isVisible, getLeaderID());
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
CvTeamTechs* CvTeam::GetTeamTechs() const
{
	return m_pTeamTechs;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isHuman() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iI;
		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(ePlayer).getTeam() == GetID())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isBarbarian() const
{
	return (m_eID == BARBARIAN_TEAM);
}

bool CvTeam::isMinorCiv() const
{
	return m_bIsMinorTeam;
}

bool CvTeam::isObserver() const
{
	return m_bIsObserverTeam;
}

void CvTeam::updateTeamStatus()
{
	m_bIsMinorTeam = false;
	m_bIsObserverTeam = false;

	//one member determines the result
	for(std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
	{
		if (CvPreGame::isMinorCiv(*iI))
		{
			m_bIsMinorTeam = true;
		}

		CvPlayer& kPlayer = GET_PLAYER(*iI);
		if (kPlayer.isObserver())
		{
			m_bIsObserverTeam = true;
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::isMajorCiv() const
{
	return !(isMinorCiv() || isBarbarian() || isObserver());
}

//	--------------------------------------------------------------------------------
/// The number of Minor Civs this player has declared war on
int CvTeam::GetNumMinorCivsAttacked() const
{
	return m_iNumMinorCivsAttacked;
}

//	--------------------------------------------------------------------------------
/// Sets the number of Minor Civs this player has declared war on
void CvTeam::SetNumMinorCivsAttacked(int iValue)
{
	m_iNumMinorCivsAttacked = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes the number of Minor Civs this player has declared war on
void CvTeam::ChangeNumMinorCivsAttacked(int iChange)
{
	SetNumMinorCivsAttacked(GetNumMinorCivsAttacked() + iChange);
}

//	--------------------------------------------------------------------------------
/// Has this Player been roughing up Minors enough to worry them?
bool CvTeam::IsMinorCivAggressor() const
{
	// Player has attacked enough Minors that they're getting antsy
	if (GetNumMinorCivsAttacked() >= /*2*/ GD_INT_GET(MINOR_CIV_AGGRESSOR_THRESHOLD))
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
/// Has this player attacked enough Minors to be considered a world threat? (Minors band together and declare war)
bool CvTeam::IsMinorCivWarmonger() const
{
	// Player has attacked enough Minors that an Alliance has formed
	if (GetNumMinorCivsAttacked() >= /*4*/ GD_INT_GET(MINOR_CIV_WARMONGER_THRESHOLD))
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvTeam::getLeaderID() const
{
	// If there are no members on this team, return NO_PLAYER
	// This is dangerous - the return value is not checked in many places
	if (m_members.empty())
		return NO_PLAYER;

	for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
	{
		CvPlayer& thisPlayer = GET_PLAYER(*iI);
		if (thisPlayer.isAlive())
			return thisPlayer.GetID();
	}

	// If no member is alive, return the first
	// This is dangerous - the return value is not checked in many places
	return m_members.front();
}


//	--------------------------------------------------------------------------------
PlayerTypes CvTeam::getSecretaryID() const
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if(GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					return ((PlayerTypes)iI);
				}
			}
		}
	}

	return getLeaderID();
}


//	--------------------------------------------------------------------------------
HandicapTypes CvTeam::getHandicapType() const
{
	int iGameHandicap;
	int iCount;
	int iI;

	iGameHandicap = 0;
	iCount = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isAlive())
		{
			if(thisPlayer.getTeam() == m_eID)
			{
				iGameHandicap += GET_PLAYER((PlayerTypes)iI).getHandicapType();
				iCount++;
			}
		}
	}

	if(iCount > 0)
	{
		CvAssertMsg((iGameHandicap / iCount) >= 0, "(iGameHandicap / iCount) is expected to be non-negative (invalid Index)");
		return ((HandicapTypes)(iGameHandicap / iCount));
	}
	else
	{
		return ((HandicapTypes)(GD_INT_GET(STANDARD_HANDICAP)));
	}
}


//	--------------------------------------------------------------------------------
CvString CvTeam::getName() const
{
	CvString strBuffer;
	bool bFirst = true;
	const TeamTypes eID(GetID());

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(iI));
		CvPlayerAI& kPlayer(GET_PLAYER(ePlayer));

		if(kPlayer.isAlive() || kPlayer.isBeingResurrected())
		{
			if(kPlayer.getTeam() == eID)
			{
				if(!bFirst)
				{
					strBuffer += "/";
				}

				strBuffer += kPlayer.getName();

				bFirst = false;
			}
		}
	}

	return strBuffer;
}

//	--------------------------------------------------------------------------------
CvString CvTeam::getNameKey() const
{
	CvString strBuffer;
	bool bFirst = true;
	const TeamTypes eID(GetID());
	bool bOneManTeam = (this->getNumMembers() == 1);


	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(iI));
		CvPlayerAI& kPlayer(GET_PLAYER(ePlayer));

		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == eID)
			{
				if(!bFirst)
				{
					strBuffer += "/";
				}
				
				if(bOneManTeam)
					strBuffer += kPlayer.getNameKey();
				else
					strBuffer += kPlayer.getName();

				bFirst = false;
			}
		}
	}

	return strBuffer;
}


//	--------------------------------------------------------------------------------
int CvTeam::getNumMembers() const
{
	return int(m_members.size());
}

#if defined(MOD_BALANCE_CORE)
bool CvTeam::addPlayer(PlayerTypes eID)
{
	if (eID==NO_PLAYER)
		return false;

	bool bPlayerAdded = false;

	if ( std::find( m_members.begin(), m_members.end(), eID ) == m_members.end() )
	{
		m_members.push_back(eID);
		bPlayerAdded = true;
	}

	updateTeamStatus();
	return bPlayerAdded;
}

void CvTeam::removePlayer(PlayerTypes eID)
{
	std::vector<PlayerTypes>::iterator pos = std::find( m_members.begin(), m_members.end(), eID );
	if ( pos != m_members.end() )
		m_members.erase(pos);

	updateTeamStatus();
}

const std::vector<PlayerTypes>& CvTeam::getPlayers()
{
	return m_members;
}

bool CvTeam::isMember(PlayerTypes eID) const
{
	std::vector<PlayerTypes>::const_iterator pos = std::find( m_members.begin(), m_members.end(), eID );
	return ( pos != m_members.end() );
}

#endif

//	--------------------------------------------------------------------------------
int CvTeam::getAliveCount() const
{
	return m_iAliveCount;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeAliveCount(int iChange)
{
	m_iAliveCount = (m_iAliveCount + iChange);
	CvAssert(getAliveCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getEverAliveCount() const
{
	return m_iEverAliveCount;
}


//	--------------------------------------------------------------------------------
int CvTeam::isEverAlive() const
{
	return (getEverAliveCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeEverAliveCount(int iChange)
{
	m_iEverAliveCount = (m_iEverAliveCount + iChange);
	CvAssert(getEverAliveCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getNumCities() const
{
	return m_iNumCities;
}


//	--------------------------------------------------------------------------------
void CvTeam::changeNumCities(int iChange)
{
	m_iNumCities = (m_iNumCities + iChange);
	CvAssert(getNumCities() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getTotalPopulation() const
{
	return (m_iTotalPopulation);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getTotalLand() const
{
	return (m_iTotalLand);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	CvAssert(getTotalLand() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getNukeInterception() const
{
	return m_iNukeInterception;
}


//	--------------------------------------------------------------------------------
void CvTeam::changeNukeInterception(int iChange)
{
	m_iNukeInterception = (m_iNukeInterception + iChange);
	CvAssert(getNukeInterception() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const
{
	return m_aiForceTeamVoteEligibilityCount[eVoteSource];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const
{
	return ((getForceTeamVoteEligibilityCount(eVoteSource) > 0) && !isMinorCiv());
}


//	--------------------------------------------------------------------------------
void CvTeam::changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange)
{
	m_aiForceTeamVoteEligibilityCount[eVoteSource] += iChange;
	CvAssert(getForceTeamVoteEligibilityCount(eVoteSource) >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getExtraWaterSeeFromCount() const
{
	return m_iExtraWaterSeeFromCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isExtraWaterSeeFrom() const
{
	return (getExtraWaterSeeFromCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeExtraWaterSeeFromCount(int iChange)
{
	if(iChange != 0)
	{
		GC.getMap().updateSight(false);

		m_iExtraWaterSeeFromCount = (m_iExtraWaterSeeFromCount + iChange);
		CvAssert(getExtraWaterSeeFromCount() >= 0);

		GC.getMap().updateSight(true);
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getMapTradingCount() const
{
	return m_iMapTradingCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isMapTrading()	const
{
	return (getMapTradingCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeMapTradingCount(int iChange)
{
	m_iMapTradingCount = (m_iMapTradingCount + iChange);
	CvAssert(getMapTradingCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getTechTradingCount() const
{
	return m_iTechTradingCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isTechTrading() const
{
	return (getTechTradingCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeTechTradingCount(int iChange)
{
	m_iTechTradingCount = (m_iTechTradingCount + iChange);
	CvAssert(getTechTradingCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getGoldTradingCount() const
{
	return m_iGoldTradingCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isGoldTrading() const
{
	return (getGoldTradingCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeGoldTradingCount(int iChange)
{
	m_iGoldTradingCount = (m_iGoldTradingCount + iChange);
	CvAssert(getGoldTradingCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvTeam::HavePolicyInTeam(PolicyTypes ePolicy)
{
	const TeamTypes eID(GetID());

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer(static_cast<PlayerTypes>(iI));
		CvPlayerAI& kPlayer(GET_PLAYER(ePlayer));

		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == eID)
			{
				if (kPlayer.GetPlayerPolicies()->HasPolicy(ePolicy))
				{
					return true;
				}
			}
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
int CvTeam::getAllowEmbassyTradingAllowedCount() const
{
	return m_iAllowEmbassyTradingAllowedCount;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isAllowEmbassyTradingAllowed() const
{
	return (getAllowEmbassyTradingAllowedCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvTeam::changeAllowEmbassyTradingAllowedCount(int iChange)
{
	m_iAllowEmbassyTradingAllowedCount = (m_iAllowEmbassyTradingAllowedCount + iChange);
	CvAssert(getAllowEmbassyTradingAllowedCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getOpenBordersTradingAllowedCount() const
{
	return m_iOpenBordersTradingAllowedCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isOpenBordersTradingAllowed() const
{
	return (getOpenBordersTradingAllowedCount() > 0);
}

//	--------------------------------------------------------------------------------
bool CvTeam::isOpenBordersTradingAllowedWithTeam(TeamTypes eTeam) const
{
	return HasEmbassyAtTeam(eTeam) && isOpenBordersTradingAllowed();
}

//	--------------------------------------------------------------------------------
void CvTeam::changeOpenBordersTradingAllowedCount(int iChange)
{
	m_iOpenBordersTradingAllowedCount = (m_iOpenBordersTradingAllowedCount + iChange);
	CvAssert(getOpenBordersTradingAllowedCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getDefensivePactTradingAllowedCount() const
{
	return m_iDefensivePactTradingAllowedCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isDefensivePactTradingAllowed() const
{
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if(IsVassalOfSomeone())
		{
			return false;
		}
	}
#endif
	return (getDefensivePactTradingAllowedCount() > 0);
}

//	--------------------------------------------------------------------------------
bool CvTeam::isDefensivePactTradingAllowedWithTeam(TeamTypes eTeam) const
{
	return HasEmbassyAtTeam(eTeam) && GET_TEAM(eTeam).HasEmbassyAtTeam(m_eID) && isDefensivePactTradingAllowed();
}

//	--------------------------------------------------------------------------------
void CvTeam::changeDefensivePactTradingAllowedCount(int iChange)
{
	m_iDefensivePactTradingAllowedCount = (m_iDefensivePactTradingAllowedCount + iChange);
	CvAssert(getDefensivePactTradingAllowedCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::GetResearchAgreementTradingAllowedCount() const
{
	return m_iResearchAgreementTradingAllowedCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::IsResearchAgreementTradingAllowed() const
{
	return (GetResearchAgreementTradingAllowedCount() > 0);
}

//	--------------------------------------------------------------------------------
bool CvTeam::IsResearchAgreementTradingAllowedWithTeam(TeamTypes eTeam) const
{
	return HasEmbassyAtTeam(eTeam) && IsResearchAgreementTradingAllowed();
}

//	--------------------------------------------------------------------------------
void CvTeam::ChangeResearchAgreementTradingAllowedCount(int iChange)
{
	m_iResearchAgreementTradingAllowedCount = (m_iResearchAgreementTradingAllowedCount + iChange);
	CvAssert(GetResearchAgreementTradingAllowedCount() >= 0);
}

#if defined(MOD_TECHS_CITY_WORKING)
//	--------------------------------------------------------------------------------
int CvTeam::GetCityWorkingChange() const
{
	return m_iCityWorkingChange;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isCityWorkingChange()	const
{
	return (GetCityWorkingChange() != 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeCityWorkingChange(int iChange)
{
	if(iChange != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.isAlive()) {
				if (kLoopPlayer.getTeam() == GetID()) {
					CvCity* pLoopCity;
					int iLoop;
		
					for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop)) {
						int iOldPlots = pLoopCity->GetNumWorkablePlots();
						int iNewPlots = pLoopCity->GetNumWorkablePlots(iChange);
			
						for (int iI = std::min(iOldPlots, iNewPlots); iI < std::max(iOldPlots, iNewPlots); ++iI) {
							CvPlot* pLoopPlot = iterateRingPlots(pLoopCity->getX(), pLoopCity->getY(), iI);

							if (pLoopPlot) {
								pLoopPlot->changeCityRadiusCount(iChange);
								pLoopPlot->changePlayerCityRadiusCount(kLoopPlayer.GetID(), iChange);
							}
						}
					}
				}
			}
		}

		m_iCityWorkingChange = (m_iCityWorkingChange + iChange);
	}
}
#endif

#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
//	--------------------------------------------------------------------------------
int CvTeam::GetCityAutomatonWorkersChange() const
{
	return m_iCityAutomatonWorkersChange;
}
 //	--------------------------------------------------------------------------------
bool CvTeam::isCityAutomatonWorkersChange()	const
{
	return (GetCityAutomatonWorkersChange() != 0);
}

//	--------------------------------------------------------------------------------
void CvTeam::changeCityAutomatonWorkersChange(int iChange)
{
	if (iChange != 0) {
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.isAlive()) {
				if (kLoopPlayer.getTeam() == GetID()) {
					CvCity* pLoopCity;
					int iLoop;
		
					for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop)) {
						pLoopCity->changeAutomatons(iChange);
					}
				}
			}
		}
		m_iCityAutomatonWorkersChange = (m_iCityAutomatonWorkersChange + iChange);
	}
}
#endif
	
//	--------------------------------------------------------------------------------
int CvTeam::getBridgeBuildingCount() const
{
	return m_iBridgeBuildingCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isBridgeBuilding()	const
{
	return (getBridgeBuildingCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeBridgeBuildingCount(int iChange)
{
	if(iChange != 0)
	{
		m_iBridgeBuildingCount = (m_iBridgeBuildingCount + iChange);
		CvAssert(getBridgeBuildingCount() >= 0);

		if(m_eID == GC.getGame().getActiveTeam())
		{
			if(GC.IsGraphicsInitialized())
			{
				if(GetCurrentEra() >= GD_INT_GET(LAST_BRIDGE_ART_ERA))
				{
					gDLL->GameplayBridgeChanged(true, 1);
				}
				else
				{
					gDLL->GameplayBridgeChanged(true, 0);
				}
			}
		}
	}
}
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
//	--------------------------------------------------------------------------------
int CvTeam::getCityLessEmbarkCost() const
{
	return m_iCityLessEmbarkCost;
}
//	--------------------------------------------------------------------------------
bool CvTeam::isCityLessEmbarkCost()	const
{
	return (getCityLessEmbarkCost() > 0);
}
//	--------------------------------------------------------------------------------
void CvTeam::changeCityLessEmbarkCost(int iChange)
{
	if(iChange != 0)
	{
		m_iCityLessEmbarkCost = (m_iCityLessEmbarkCost + iChange);
		CvAssert(getCityLessEmbarkCost() >= 0);
	}
}
//	--------------------------------------------------------------------------------
int CvTeam::getCityNoEmbarkCost() const
{
	return m_iCityNoEmbarkCost;
}
//	--------------------------------------------------------------------------------
bool CvTeam::isCityNoEmbarkCost()	const
{
	return (getCityNoEmbarkCost() > 0);
}
//	--------------------------------------------------------------------------------
void CvTeam::changeCityNoEmbarkCost(int iChange)
{
	if(iChange != 0)
	{
		m_iCityNoEmbarkCost = (m_iCityNoEmbarkCost + iChange);
		CvAssert(getCityNoEmbarkCost() >= 0);
	}
}
#endif
//	--------------------------------------------------------------------------------
int CvTeam::getWaterWorkCount() const
{
	return m_iWaterWorkCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isWaterWork() const
{
	return (getWaterWorkCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeWaterWorkCount(int iChange)
{
	if(iChange != 0)
	{
		m_iWaterWorkCount = (m_iWaterWorkCount + iChange);
		CvAssert(getWaterWorkCount() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getBorderObstacleCount() const
{
	return m_iBorderObstacleCount;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isBorderObstacle() const
{
	return (getBorderObstacleCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvTeam::changeBorderObstacleCount(int iChange)
{
	if(iChange != 0)
	{
		m_iBorderObstacleCount = (m_iBorderObstacleCount + iChange);
		CvAssert(getBorderObstacleCount() >= 0);
	}
}


//	--------------------------------------------------------------------------------
bool CvTeam::isMapCentering() const
{
	return m_bMapCentering;
}


//	--------------------------------------------------------------------------------
void CvTeam::setMapCentering(bool bNewValue)
{
	if(isMapCentering() != bNewValue)
	{
		m_bMapCentering = bNewValue;

		if(GetID() == GC.getGame().getActiveTeam())
		{
			DLLUI->setDirty(MinimapSection_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
TeamTypes CvTeam::GetID() const
{
	return m_eID;
}


//	--------------------------------------------------------------------------------
int CvTeam::getTechShareCount(int iIndex) const
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");
	if(iIndex < 0 || iIndex >= MAX_TEAMS) return 0; // as set in reset()
	return m_aiTechShareCount[iIndex];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isTechShare(int iIndex) const
{
	return (getTechShareCount(iIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeTechShareCount(int iIndex, int iChange)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");
	if(iIndex < 0 || iIndex >= MAX_TEAMS) return;
	if(iChange != 0)
	{
		m_aiTechShareCount[iIndex] = (m_aiTechShareCount[iIndex] + iChange);
		CvAssert(getTechShareCount(iIndex) >= 0);

		if(isTechShare(iIndex))
		{
			updateTechShare();
		}
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getExtraMoves(DomainTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraMoves[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::changeExtraMoves(DomainTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiExtraMoves[eIndex] = (m_aiExtraMoves[eIndex] + iChange);
	CvAssert(getExtraMoves(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
bool CvTeam::canEmbark() const
{
	return m_iCanEmbarkCount > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvTeam::getCanEmbarkCount() const
{
	return m_iCanEmbarkCount;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeCanEmbarkCount(int iChange)
{
	if(iChange != 0)
	{
		m_iCanEmbarkCount += iChange;

		if(canEmbark())
		{
			int iLoop;
			CvUnit* pLoopUnit;

			// Give embarkation promotion to all civilians, because they have no way to earn it later
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					PromotionTypes ePromotionEmbarkation = GET_PLAYER((PlayerTypes)iI).GetEmbarkationPromotion();
					for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
					{
						// Land Unit
						if(pLoopUnit->getDomainType() == DOMAIN_LAND)
						{
							// Civilian unit or the unit can acquire this promotion
							if(!pLoopUnit->IsCombatUnit() || ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, pLoopUnit->getUnitType()))
								pLoopUnit->setHasPromotion(ePromotionEmbarkation, true);
						}
					}
				}
			}
		}
	}

	CvAssert(getCanEmbarkCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvTeam::canDefensiveEmbark() const
{
	return m_iDefensiveEmbarkCount > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvTeam::getDefensiveEmbarkCount() const
{
	return m_iDefensiveEmbarkCount;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeDefensiveEmbarkCount(int iChange)
{
	if(iChange != 0)
	{
		m_iDefensiveEmbarkCount += iChange;

		if(canDefensiveEmbark())
		{
			int iLoop;
			CvUnit* pLoopUnit;

			// Give embarkation promotion to all units who can currently embark
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					PromotionTypes ePromotionDefensiveEmbarkation = GET_PLAYER((PlayerTypes)iI).GetEmbarkationPromotion();

					for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
					{
						// Land Unit
						if(pLoopUnit->getDomainType() == DOMAIN_LAND)
						{
							// Take away old promotion and give the new one instead
							if(pLoopUnit->isHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_EMBARKATION)))
							{
								pLoopUnit->setHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_EMBARKATION), false);
								pLoopUnit->setHasPromotion(ePromotionDefensiveEmbarkation, true);
							}
							// Could be cleaner if add "allwater defensive promotion".  Luckily for now the only way you can get
							// both is in the Polynesia scenario and this works for that
							if(pLoopUnit->isHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_ALLWATER_EMBARKATION)))
							{
								pLoopUnit->setHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_ALLWATER_EMBARKATION), false);
								pLoopUnit->setHasPromotion(ePromotionDefensiveEmbarkation, true);
							}
							
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
							if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && GD_INT_GET(PROMOTION_DEEPWATER_EMBARKATION) != -1) {
								// If the unit has Deep Water Embarkation, change it to Defensive Deep Water Embarkation
								// This is very unlikely to happen in reality as it implies the player got the helicopter BEFORE the embarkation tech!!!
								if(pLoopUnit->isHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_DEEPWATER_EMBARKATION)))
								{
									pLoopUnit->setHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_DEEPWATER_EMBARKATION), false);
									pLoopUnit->setHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_DEFENSIVE_DEEPWATER_EMBARKATION), true);
								}
							}
#endif
						}
					}
				}
			}
		}
	}
	CvAssert(getDefensiveEmbarkCount() >= 0);
}

//	--------------------------------------------------------------------------------
void CvTeam::UpdateEmbarkGraphics()
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			int iLoop;
			for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				// Land Unit
				if(pLoopUnit->getDomainType() == DOMAIN_LAND)
				{
					// At sea so its graphic look needs to change too?
					if(pLoopUnit->isEmbarked())
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitEmbark(pDllUnit.get(), true);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::canEmbarkAllWaterPassage() const
{
	return m_iEmbarkedAllWaterPassageCount > 0 ? true : false;
}

//	--------------------------------------------------------------------------------
int CvTeam::getEmbarkedAllWaterPassage() const
{
	return m_iEmbarkedAllWaterPassageCount;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeEmbarkedAllWaterPassage(int iChange)
{
	if(iChange != 0)
	{
		m_iEmbarkedAllWaterPassageCount += iChange;
	}
	CvAssert(getEmbarkedAllWaterPassage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvTeam::GetNumNaturalWondersDiscovered() const
{
	return m_iNumNaturalWondersDiscovered;
}

//	--------------------------------------------------------------------------------
void CvTeam::ChangeNumNaturalWondersDiscovered(int iChange)
{
	if(iChange != 0)
	{
		m_iNumNaturalWondersDiscovered += iChange;
	}
	CvAssert(GetNumNaturalWondersDiscovered() >= 0);
}

//	--------------------------------------------------------------------------------
int CvTeam::GetNumLandmarksBuilt() const
{
	return m_iNumLandmarksBuilt;
}

//	--------------------------------------------------------------------------------
void CvTeam::ChangeNumLandmarksBuilt(int iChange)
{
	if (iChange != 0)
	{
		m_iNumLandmarksBuilt += iChange;
	}
	CvAssert(GetNumLandmarksBuilt() >= 0);
}

//	--------------------------------------------------------------------------------
int CvTeam::getEmbarkedExtraMoves() const
{
	return m_iEmbarkedExtraMoves;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeEmbarkedExtraMoves(int iChange)
{
	m_iEmbarkedExtraMoves = (m_iEmbarkedExtraMoves + iChange);
}


//	--------------------------------------------------------------------------------
bool CvTeam::isHasMet(TeamTypes eIndex)	const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abHasMet[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::makeHasMet(TeamTypes eIndex, bool bSuppressMessages)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(!isHasMet(eIndex))
	{
		m_abHasMet[eIndex] = true;

		SetTurnTeamMet(eIndex, GC.getGame().getGameTurn());

		updateTechShare();

		if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
		{
			if(isHuman())
			{
				if(GetID() != eIndex)
				{
					declareWar(eIndex, false, getLeaderID());
				}
			}
		}

		// First Contact in Diplo AI (Civ 5)
		for(int iMyPlayersLoop = 0; iMyPlayersLoop < MAX_CIV_PLAYERS; iMyPlayersLoop++)
		{
			PlayerTypes eMyPlayer = (PlayerTypes) iMyPlayersLoop;

			if(GET_PLAYER(eMyPlayer).isAlive())
			{
				if(GET_PLAYER(eMyPlayer).getTeam() == GetID())
				{
					// Now loop through players on Their team
					for(int iTheirPlayersLoop = 0; iTheirPlayersLoop < MAX_CIV_PLAYERS; iTheirPlayersLoop++)
					{
						PlayerTypes eTheirPlayer = (PlayerTypes) iTheirPlayersLoop;

						// Don't calculate proximity to oneself!
						if(eMyPlayer != eTheirPlayer)
						{
							if(GET_PLAYER(eTheirPlayer).isAlive())
							{
								if(GET_PLAYER(eTheirPlayer).getTeam() == eIndex)
								{
									// Begin contact stuff here

									// Update Proximity between players
									GET_PLAYER(eMyPlayer).DoUpdateProximityToPlayer(eTheirPlayer);
									GET_PLAYER(eTheirPlayer).DoUpdateProximityToPlayer(eMyPlayer);

									// First contact Diplo changes (no Minors)
									if(isMajorCiv())
									{
										GET_PLAYER(eMyPlayer).GetDiplomacyAI()->DoFirstContact(eTheirPlayer);
									}

									// THIRD party loop - let everyone else know that someone met someone!
									for(int iThirdPlayersLoop = 0; iThirdPlayersLoop < MAX_CIV_PLAYERS; iThirdPlayersLoop++)
									{
										PlayerTypes eThirdPlayer = (PlayerTypes) iThirdPlayersLoop;

										if(GET_PLAYER(eThirdPlayer).isAlive())
										{
											// Don't notify diplo AI if we're the one meeting or the one being met
											if(eThirdPlayer != eMyPlayer && eThirdPlayer != eTheirPlayer)
											{
												GET_PLAYER(eThirdPlayer).GetDiplomacyAI()->DoPlayerMetSomeone(eMyPlayer, eTheirPlayer);
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

		if(GET_TEAM(eIndex).isHuman())
		{
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
					{
						if(!(GET_PLAYER((PlayerTypes)iI).isHuman()))
						{
							GET_PLAYER((PlayerTypes)iI).clearResearchQueue();
							//GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
						}
					}
				}
			}
		}

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}

		if(GET_TEAM(eIndex).isMinorCiv())
		{
			int iCapitalX = -1;
			int iCapitalY = -1;
			int iCapitalID = -1;

			// Minor reveals his capital to the player so that he can click on the City to contact
			CvCity* pCap = GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).getCapitalCity();
			if(pCap != NULL)
			{
				iCapitalX  = pCap->getX();
				iCapitalY  = pCap->getY();
				iCapitalID = pCap->GetID();
				CvPlot* pCapPlot = pCap->plot();
				if(pCapPlot)
				{
					pCapPlot->setRevealed(GetID(), true);
					GC.getMap().updateDeferredFog();
				}
			}

			// First contact with major stuff
			if(isMajorCiv())
			{
				GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).GetMinorCivAI()->DoFirstContactWithMajor(GetID(), /*bSuppressMessages*/ isAtWar(eIndex));
			}

			if(!isAtWar(eIndex))
			{
				// Notify the Team that they met someone
				if(!bSuppressMessages)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_MET_MINOR_CIV", GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).getNameKey());
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_MET_MINOR_CIV", GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).getNameKey());

					AddNotification(NOTIFICATION_MET_MINOR, strBuffer, strSummary, iCapitalX, iCapitalY, iCapitalID);
				}
			}
		}

		// Report event
		gDLL->GameplayMetTeam(GetID(), eIndex);
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::GetTurnsSinceMeetingTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return GC.getGame().getGameTurn() - m_aiTurnTeamMet[eTeam];
}

//	--------------------------------------------------------------------------------
int CvTeam::GetTurnTeamMet(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiTurnTeamMet[eTeam];
}

//	--------------------------------------------------------------------------------
void CvTeam::SetTurnTeamMet(TeamTypes eTeam, int iTurn)
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiTurnTeamMet[eTeam] = iTurn;
}

//	--------------------------------------------------------------------------------
/// Have we seen ePlayer's territory before?
bool CvTeam::IsHasFoundPlayersTerritory(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_abHasFoundPlayersTerritory[ePlayer];
}

//	--------------------------------------------------------------------------------
/// Sets us to have seen ePlayer's territory before
bool CvTeam::SetHasFoundPlayersTerritory(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(IsHasFoundPlayersTerritory(ePlayer) != bValue)
	{
		m_abHasFoundPlayersTerritory[ePlayer] = bValue;
		return true;
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isAggressor(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (isAtWar(eIndex) && m_abAggressorPacifier[eIndex]);
}

//	--------------------------------------------------------------------------------
bool CvTeam::isPacifier(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (!isAtWar(eIndex) && m_abAggressorPacifier[eIndex]);
}

//	--------------------------------------------------------------------------------
bool CvTeam::isAtWar(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abAtWar[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::setAtWar(TeamTypes eIndex, bool bNewValue, bool bAggressorPacifier)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex != GetID() || bNewValue == false, "Team is setting war with itself!");
	if (eIndex != GetID() || bNewValue == false)
	{
		m_abAggressorPacifier[eIndex] = bAggressorPacifier;
		m_abAtWar[eIndex] = bNewValue;
#if defined(MOD_BALANCE_CORE)
		for (int iAttackingPlayer = 0; iAttackingPlayer < MAX_MAJOR_CIVS; iAttackingPlayer++)
		{
			PlayerTypes eAttackingPlayer = (PlayerTypes)iAttackingPlayer;
			CvPlayerAI& kAttackingPlayer = GET_PLAYER(eAttackingPlayer);
			if(kAttackingPlayer.isAlive() && kAttackingPlayer.getTeam() == GetID())
			{
				for (int iDefendingPlayer = 0; iDefendingPlayer < MAX_MAJOR_CIVS; iDefendingPlayer++)
				{
					PlayerTypes eDefendingPlayer = (PlayerTypes)iDefendingPlayer;
					CvPlayerAI& kDefendingPlayer = GET_PLAYER(eDefendingPlayer);
					if(kDefendingPlayer.isAlive() && kDefendingPlayer.getTeam() == eIndex)
					{
						kAttackingPlayer.recomputeGreatPeopleModifiers();
						kDefendingPlayer.recomputeGreatPeopleModifiers();
					}
				}
			}
		}
#endif
	}

#if defined(MOD_BALANCE_CORE)
	//Check for bad units, and capture them!
	vector<CvUnitCaptureDefinition> kCaptureUnitList;

	vector<PlayerTypes> vOurTeam = getPlayers();
	for(size_t i=0; i<vOurTeam.size(); i++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(vOurTeam[i]);
		if(kPlayer.isAlive())
		{
			int iLoop = 0;
			CvUnit* pLoopUnit = NULL; //for some stupid reason createCaptureUnit is a member of CvUnit and not CvPlayer
			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if(pLoopUnit->IsCombatUnit())
				{
					for(int iUnitLoop = 0; iUnitLoop < pLoopUnit->plot()->getNumUnits(); iUnitLoop++)
					{
						CvUnit* pPotentialCaptureUnit = pLoopUnit->plot()->getUnitByIndex(iUnitLoop);

						if( pPotentialCaptureUnit && 
							pPotentialCaptureUnit->getTeam()==eIndex && //only from the right team
							!pPotentialCaptureUnit->IsCombatUnit() && 
							!pPotentialCaptureUnit->isEmbarked() && 
							!pPotentialCaptureUnit->isDelayedDeath() ) //can only capture once!
						{
							if(pPotentialCaptureUnit->getCaptureUnitType(GET_PLAYER(pPotentialCaptureUnit->getOwner()).getCivilizationType()) != NO_UNIT)
							{
								CvUnitCaptureDefinition kCaptureDef;
								if (pPotentialCaptureUnit->getCaptureDefinition(&kCaptureDef, kPlayer.GetID()))
								{
									bool bAlreadyCaptured = false;
									for (uint uiCaptureIndex = 0; uiCaptureIndex < kCaptureUnitList.size(); ++uiCaptureIndex)
									{
										if (kCaptureUnitList[uiCaptureIndex].iUnitID == kCaptureDef.iUnitID)
										{
											bAlreadyCaptured = true;
											break;
										}
									}
									if (!bAlreadyCaptured)
									{
										kCaptureUnitList.push_back(kCaptureDef);
									}
									pPotentialCaptureUnit->setCapturingPlayer(NO_PLAYER);	// Make absolutely sure this is not valid so the kill does not do the capture.
								}
							}

							//be careful here, it's possible we're about to kill a civilian which is right now executing a mission causing this war state change
							pPotentialCaptureUnit->kill(true, kPlayer.GetID());
						}
					}
				}
			}

			// Create any units we captured, now that we own the destination
			for(uint uiCaptureIndex = 0; uiCaptureIndex < kCaptureUnitList.size(); ++uiCaptureIndex)
			{
				pLoopUnit->createCaptureUnit(kCaptureUnitList[uiCaptureIndex], true);
			}
		}
	}
#endif

	gDLL->GameplayWarStateChanged(GetID(), eIndex, bNewValue);
}

//	--------------------------------------------------------------------------------
/// Do we have a common enemy with this other team?
bool CvTeam::HasCommonEnemy(TeamTypes eOtherTeam) const
{
	CvTeam& kOtherTeam = GET_TEAM(eOtherTeam);
	int iI;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		CvPlayerAI& kEnemyPlayer = GET_PLAYER((PlayerTypes)iI);
		TeamTypes eEnemyTeam = kEnemyPlayer.getTeam();
		if(eEnemyTeam != m_eID && eEnemyTeam != eOtherTeam)
		{
			if(kEnemyPlayer.isAlive())
			{
				if(kOtherTeam.isAtWar(eEnemyTeam) && isAtWar(eEnemyTeam))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// How long have we been at war with eTeam?
int CvTeam::GetNumTurnsAtWar(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumTurnsAtWar[eTeam];
}

//	--------------------------------------------------------------------------------
/// Sets how long have we been at war with eTeam
void CvTeam::SetNumTurnsAtWar(TeamTypes eTeam, int iValue)
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eTeam != GetID() || iValue == 0, "Team is setting war turns with itself!");
	if(eTeam != GetID() || iValue == 0)
		m_aiNumTurnsAtWar[eTeam] = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes how long have we been at war with eTeam
void CvTeam::ChangeNumTurnsAtWar(TeamTypes eTeam, int iChange)
{
	SetNumTurnsAtWar(eTeam, GetNumTurnsAtWar(eTeam) + iChange);
}

//	--------------------------------------------------------------------------------
/// How long are we locked into a war with eTeam?
int CvTeam::GetNumTurnsLockedIntoWar(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumTurnsLockedIntoWar[eTeam];
}

//	--------------------------------------------------------------------------------
/// How long are we locked into a war with eTeam?
void CvTeam::SetNumTurnsLockedIntoWar(TeamTypes eTeam, int iValue)
{
	if (eTeam == GetID())
		return;

	if (iValue <= 0)
	{
		m_aiNumTurnsLockedIntoWar[eTeam] = 0;
	}
	else
	{
		int iCurrentValue = m_aiNumTurnsLockedIntoWar[eTeam];
		if ((iCurrentValue <= 0) || (iValue < iCurrentValue)) // Don't allow adding more locked turns if already locked at war! Prevents happiness exploit.
		{
			m_aiNumTurnsLockedIntoWar[eTeam] = iValue;
		}
	}
}

//	--------------------------------------------------------------------------------
/// How long are we locked into a war with eTeam?
void CvTeam::ChangeNumTurnsLockedIntoWar(TeamTypes eTeam, int iChange)
{
	SetNumTurnsLockedIntoWar(eTeam, GetNumTurnsLockedIntoWar(eTeam) + iChange);
}

//	--------------------------------------------------------------------------------
int CvTeam::GetTurnMadePeaceTreatyWithTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTurnMadePeaceTreatyWithTeam[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::SetTurnMadePeaceTreatyWithTeam(TeamTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiTurnMadePeaceTreatyWithTeam[eIndex] = iNewValue;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isPermanentWarPeace(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abPermanentWarPeace[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::setPermanentWarPeace(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abPermanentWarPeace[eIndex] = bNewValue;
}

//	--------------------------------------------------------------------------------
TeamTypes CvTeam::GetLiberatedByTeam() const
{
	return m_eLiberatedByTeam;
}


//	--------------------------------------------------------------------------------
void CvTeam::SetLiberatedByTeam(TeamTypes eIndex)
{
	if(GetLiberatedByTeam() != eIndex)
	{
		m_eLiberatedByTeam = eIndex;
	}
}


//	--------------------------------------------------------------------------------
TeamTypes CvTeam::GetKilledByTeam() const
{
	return m_eKilledByTeam;
}


//	--------------------------------------------------------------------------------
void CvTeam::SetKilledByTeam(TeamTypes eIndex)
{
	if(GetKilledByTeam() != eIndex)
	{
		m_eKilledByTeam = eIndex;
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::CloseEmbassyAtTeam(TeamTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	SetHasEmbassyAtTeam(eIndex, false);
	SetAllowsOpenBordersToTeam(m_eID, false);
	SetHasDefensivePact(m_eID, false);
	GET_TEAM(eIndex).SetHasDefensivePact(m_eID, false);

	//SetHasResearchAgreement(m_eID, false);
	//GET_TEAM(eIndex).SetHasResearchAgreement(m_eID, false);
}

//	--------------------------------------------------------------------------------
bool CvTeam::HasEmbassyAtTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	
	if ((GetLiberatedByTeam() == eIndex || GET_TEAM(eIndex).GetLiberatedByTeam() == m_eID) && !isAtWar(eIndex))
	{
		return true;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if (GetMaster() == eIndex)
		{
			return true;
		}
		if (GET_TEAM(eIndex).GetMaster() == m_eID)
		{
			return true;
		}
	}
#endif

	return m_abEmbassy[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::SetHasEmbassyAtTeam(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(HasEmbassyAtTeam(eIndex) != bNewValue)
	{
		m_abEmbassy[eIndex] = bNewValue;
		bool bRevealPlots = false;

		// find capital of team
		// reveal hexes around capital
		for(int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iPlayer;
			if(GET_PLAYER(ePlayer).getTeam() == eIndex)
			{
				CvCity* pCity = GET_PLAYER(ePlayer).getCapitalCity();
				if(pCity)
				{
					CvPlot* pPlot = pCity->plot();
					CvAssertMsg(pPlot, "Capital city lacks plot? How'd that happen?");
					if(pPlot)
					{
						const int iPopRange = 2;
						for(int iDX = -(iPopRange); iDX <= iPopRange; iDX++)
						{
							for(int iDY = -(iPopRange); iDY <= iPopRange; iDY++)
							{
								CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iPopRange);
								if(pLoopPlot == NULL)
								{
									continue;
								}

								pLoopPlot->setRevealed(GetID(), true);
							}
						}
						bRevealPlots = true;
					}
				}
			}
		}

		if(bRevealPlots)
		{
			GC.getMap().updateDeferredFog();
		}

		GC.getMap().verifyUnitValidPlot();

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}

		// logging
		for (uint uiMyPlayer = 0; uiMyPlayer < MAX_MAJOR_CIVS; uiMyPlayer++)
		{
			PlayerTypes eMyPlayer = (PlayerTypes)uiMyPlayer;
			if (GET_PLAYER(eMyPlayer).getTeam() == m_eID)
			{
				for (uint uiOtherPlayer = 0; uiOtherPlayer < MAX_MAJOR_CIVS; uiOtherPlayer++)
				{
					PlayerTypes eOtherPlayer = (PlayerTypes)uiOtherPlayer;
					CvPlayer& kOtherPlayer = GET_PLAYER(eOtherPlayer);
					if (kOtherPlayer.getTeam() == eIndex && kOtherPlayer.isAlive())
					{
						if (bNewValue)
						{
							GET_PLAYER(eMyPlayer).GetDiplomacyAI()->LogOpenEmbassy(eOtherPlayer);
						}
						else
						{
							GET_PLAYER(eMyPlayer).GetDiplomacyAI()->LogCloseEmbassy(eOtherPlayer);
						}
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::HasSpyAtTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	for (uint uiMyPlayer = 0; uiMyPlayer < MAX_MAJOR_CIVS; uiMyPlayer++)
	{
		CvPlayer& kMyPlayer = GET_PLAYER((PlayerTypes)uiMyPlayer);
		if (kMyPlayer.getTeam() == m_eID)
		{
			for (uint uiOtherPlayer = 0; uiOtherPlayer < MAX_MAJOR_CIVS; uiOtherPlayer++)
			{
				CvPlayer& kOtherPlayer = GET_PLAYER((PlayerTypes)uiOtherPlayer);
				if (kOtherPlayer.getTeam() == eIndex)
				{
					if (kMyPlayer.GetEspionage()->GetSpyIndexInCity(kOtherPlayer.getCapitalCity()) != -1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvTeam::EvacuateDiplomatsAtTeam(TeamTypes eIndex)
{
	for (int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++)
	{
		PlayerTypes ePlayer1 = (PlayerTypes)iPlayer;
		if (GET_PLAYER(ePlayer1).getTeam() == m_eID)
		{
			for (int iPlayer2 = 0; iPlayer2 < MAX_MAJOR_CIVS; iPlayer2++)
			{
				PlayerTypes ePlayer2 = (PlayerTypes)iPlayer2;
				if (GET_PLAYER(ePlayer2).getTeam() != eIndex)
				{
					continue;
				}

				if (GET_PLAYER(ePlayer1).GetEspionage()->IsMyDiplomatVisitingThem(ePlayer2, true))
				{
					CvCity* pCapitalCity = GET_PLAYER(ePlayer2).getCapitalCity();
					int iSpyIndex = GET_PLAYER(ePlayer1).GetEspionage()->GetSpyIndexInCity(pCapitalCity);
					if (iSpyIndex != -1)
					{
						CvNotifications* pNotifications = GET_PLAYER(ePlayer1).GetNotifications();
						if(pNotifications)
						{
							CvPlayerEspionage* pEspionage = GET_PLAYER(ePlayer1).GetEspionage();
							CvSpyRank eSpyRank = pEspionage->m_aSpyList[iSpyIndex].m_eRank;
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMAT_EJECTED");
							Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMAT_EJECTED_TT");
							strNotification << pEspionage->GetSpyRankName(eSpyRank);
							strNotification << pEspionage->m_aSpyList[iSpyIndex].GetSpyName(&GET_PLAYER(ePlayer1));
							strNotification << pCapitalCity->getNameKey();
							pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
						GET_PLAYER(ePlayer1).GetEspionage()->MoveSpyTo(NULL, iSpyIndex, false);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::IsAllowsOpenBordersToTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(GetLiberatedByTeam() == eIndex && !isAtWar(eIndex))
	{
		return true;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && GetMaster() == eIndex)
	{
		return true;
	}
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
	//we always have open borders with ourselves
	if (GetID() == eIndex)
		return true;
#endif

	return m_abOpenBorders[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::SetAllowsOpenBordersToTeam(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsAllowsOpenBordersToTeam(eIndex) != bNewValue)
	{
		m_abOpenBorders[eIndex] = bNewValue;

		GC.getMap().verifyUnitValidPlot();

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvTeam::IsHasDefensivePact(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abDefensivePact[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::SetHasDefensivePact(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	m_abDefensivePact[eIndex] = bNewValue;

	if ((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
	{
		DLLUI->setDirty(Score_DIRTY_BIT, true);
	}

	if (bNewValue)
	{
		// Cancel coop war plans
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			CvDiplomacyAI* pDiplo = GET_PLAYER(eLoopPlayer).GetDiplomacyAI();

			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == GetID())
			{
				for (int iDPLoop = 0; iDPLoop < MAX_MAJOR_CIVS; iDPLoop++)
				{
					PlayerTypes eDPLoopPlayer = (PlayerTypes) iDPLoop;

					if (GET_PLAYER(eDPLoopPlayer).isAlive() && GET_PLAYER(eDPLoopPlayer).getTeam() == eIndex)
					{
						for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
						{
							PlayerTypes eThirdParty = (PlayerTypes) iThirdPartyLoop;

							if (eThirdParty == eLoopPlayer || GET_PLAYER(eThirdParty).getTeam() == GET_PLAYER(eDPLoopPlayer).getTeam())
								continue;

							if (pDiplo->IsPlayerValid(eThirdParty, true) && pDiplo->GetCoopWarState(eThirdParty, eDPLoopPlayer) == COOP_WAR_STATE_PREPARING)
							{
								GET_PLAYER(eThirdParty).GetDiplomacyAI()->SetPlayerBrokenCoopWarPromise(eLoopPlayer, true);
								GET_PLAYER(eThirdParty).GetDiplomacyAI()->ChangeCoopWarScore(eLoopPlayer, -2);
								GET_PLAYER(eThirdParty).GetDiplomacyAI()->ChangeRecentAssistValue(eLoopPlayer, 300);

								CvNotifications* pNotify = GET_PLAYER(eLoopPlayer).GetNotifications();
								if (pNotify)
								{
									Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
									Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_DP");
									strText << GET_PLAYER(eDPLoopPlayer).getCivilizationShortDescriptionKey();
									strText << GET_PLAYER(eThirdParty).getCivilizationShortDescriptionKey();
									pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
								}
								pNotify = GET_PLAYER(eThirdParty).GetNotifications();
								if (pNotify)
								{
									Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_S");
									Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_COOP_WAR_BROKEN_DP");
									strText << GET_PLAYER(eDPLoopPlayer).getCivilizationShortDescriptionKey();
									strText << GET_PLAYER(eLoopPlayer).getCivilizationShortDescriptionKey();
									pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
								}

								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetCoopWarState(eThirdParty, eDPLoopPlayer, NO_COOP_WAR_STATE);
								GET_PLAYER(eThirdParty).GetDiplomacyAI()->SetCoopWarState(eLoopPlayer, eDPLoopPlayer, NO_COOP_WAR_STATE);
							}
						}
					}
				}
			}
		}

		if (!GET_TEAM(eIndex).IsHasDefensivePact(GetID()))
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_PLAYERS_SIGN_DEFENSIVE_PACT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::GetTotalNumResearchAgreements() const
{
	int iCount = 0;

	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eLoopTeam).isAlive())
		{
			if(IsHasResearchAgreement(eLoopTeam))
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
bool CvTeam::IsHasResearchAgreement(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abResearchAgreement[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::SetHasResearchAgreement(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsHasResearchAgreement(eIndex) != bNewValue)
	{
		m_abResearchAgreement[eIndex] = bNewValue;

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}

		if(bNewValue && !GET_TEAM(eIndex).IsHasResearchAgreement(GetID()))
		{
			const char* strLeaderName = GET_PLAYER(getLeaderID()).getCivilizationShortDescriptionKey();
			const char* strOtherLeaderName = GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).getCivilizationShortDescriptionKey();

			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_RESEARCH_AGREEMENT", strLeaderName, strOtherLeaderName);
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::CancelResearchAgreement(TeamTypes eIndex)
{
	if (IsHasResearchAgreement(eIndex))
	{
		m_abResearchAgreement[eIndex] = false;
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::isForcePeace(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abForcePeace[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::setForcePeace(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abForcePeace[eIndex] = bNewValue;
}


//	--------------------------------------------------------------------------------
int CvTeam::getRouteChange(RouteTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiRouteChange[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::changeRouteChange(RouteTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiRouteChange[eIndex] = (m_paiRouteChange[eIndex] + iChange);
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvTeam::getTradeRouteDomainExtraRange(DomainTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTradeRouteDomainExtraRange[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::changeTradeRouteDomainExtraRange(DomainTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiTradeRouteDomainExtraRange[eIndex] = (m_paiTradeRouteDomainExtraRange[eIndex] + iChange);
}
#endif

//	--------------------------------------------------------------------------------
int CvTeam::getBuildTimeChange(BuildTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildTimeChange[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::changeBuildTimeChange(BuildTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	if (iChange != 0)
	{
		m_paiBuildTimeChange[eIndex] = (m_paiBuildTimeChange[eIndex] + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// What's the best route this team can build?
RouteTypes CvTeam::GetBestPossibleRoute()
{
	return (RouteTypes) m_iBestPossibleRoute;
}

//	--------------------------------------------------------------------------------
/// Sets what the best route this team can build is
void CvTeam::SetBestPossibleRoute(RouteTypes eRoute)
{
	CvAssertMsg(eRoute >= NO_ROUTE, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eRoute < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(GetBestPossibleRoute() != eRoute)
	{
		m_iBestPossibleRoute = eRoute;
	}
}

//	--------------------------------------------------------------------------------
/// Figures out what the best available route is
void CvTeam::DoUpdateBestRoute()
{
	RouteTypes eBestRoute = NO_ROUTE;
	int iBestRouteValue = 0;

	// Loop through all builds to see if it allows us to build a Route
	for(int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iBuildLoop);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			const RouteTypes eRoute = (RouteTypes) pkBuildInfo->getRoute();
			if(eRoute != NO_ROUTE)
			{
				int iRouteValue = -1;

				// If this route requires a tech, it's value is the cost of the tech
				const TechTypes eTech = (TechTypes) pkBuildInfo->getTechPrereq();
				CvTechEntry* pkTechInfo = (eTech != NO_TECH)? GC.getTechInfo(eTech) : NULL;
				if(pkTechInfo)
				{
					if(GetTeamTechs()->HasTech(eTech))
					{
						iRouteValue = pkTechInfo->GetResearchCost();
					}
				}
				// Route requires no tech, so it's value is simply 1
				else
				{
					iRouteValue = 1;
				}

				// Most valuable route found so far? (note we use greater than OR equal to so that we'll pick up the LAST entry in Builds - this is most likely to be the most advanced if there are ties)
				if(iRouteValue >= iBestRouteValue)
				{
					iBestRouteValue = iRouteValue;
					eBestRoute = eRoute;
				}
			}
		}
	}

	if(eBestRoute > NO_ROUTE)
	{
		SetBestPossibleRoute(eBestRoute);
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getProjectCount(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectCount[eIndex];
}

//	--------------------------------------------------------------------------------
int CvTeam::getProjectDefaultArtType(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectDefaultArtTypes[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::setProjectDefaultArtType(ProjectTypes eIndex, int value)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectDefaultArtTypes[eIndex] = value;
}

//	--------------------------------------------------------------------------------
int CvTeam::getProjectArtType(ProjectTypes eIndex, int number) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(number >= 0, "number is expected to be non-negative (invalid Index)");
	CvAssertMsg(number < getProjectCount(eIndex), "number is expected to be within maximum bounds (invalid Index)");
	return m_pavProjectArtTypes[eIndex][number];
}

//	--------------------------------------------------------------------------------
void CvTeam::setProjectArtType(ProjectTypes eIndex, int number, int value)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(number >= 0, "number is expected to be non-negative (invalid Index)");
	CvAssertMsg(number < getProjectCount(eIndex), "number is expected to be within maximum bounds (invalid Index)");
	m_pavProjectArtTypes[eIndex][number] = value;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(!isTeamProject(eIndex))
	{
		return false;
	}

	CvAssertMsg(getProjectCount(eIndex) <= GC.getProjectInfo(eIndex)->GetMaxTeamInstances(), "Current Project count is expected to not exceed the maximum number of instances for this project");

	return ((getProjectCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex)->GetMaxTeamInstances());
}

//	--------------------------------------------------------------------------------
bool CvTeam::isProjectAndArtMaxedOut(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(getProjectCount(eIndex) >= GC.getProjectInfo(eIndex)->GetMaxTeamInstances())
	{
		int count = getProjectCount(eIndex);
		for(int i=0; i<count; i++)
		{
			if(getProjectArtType(eIndex, i) == -1) //undefined
				return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::finalizeProjectArtTypes()
{
	//loop through each project and fill in default art values
	for(int i=0; i<GC.getNumProjectInfos(); i++)
	{
		ProjectTypes projectType = (ProjectTypes) i;
		int projectCount = getProjectCount(projectType);
		for(int j=0; j<projectCount; j++)
		{
			int projectArtType = getProjectArtType(projectType, j);
			if(projectArtType == -1) //undefined
			{
				int defaultArtType = getProjectDefaultArtType(projectType);
				setProjectArtType(projectType, j, defaultArtType);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::changeProjectCount(ProjectTypes eIndex, int iChange)
{
	bool bChangeProduction;
	int iOldProjectCount;
	int iI, iJ;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
#if defined(MOD_GLOBAL_NO_CONQUERED_SPACESHIPS)
		if (MOD_GLOBAL_NO_CONQUERED_SPACESHIPS && iChange < 0) {
			CUSTOMLOG("Disassembling project %d by %d", (int) eIndex, iChange);
		}
#endif
		GC.getGame().incrementProjectCreatedCount(eIndex, iChange);

		iOldProjectCount = getProjectCount(eIndex);

		m_paiProjectCount[eIndex] = (m_paiProjectCount[eIndex] + iChange);
		CvAssert(getProjectCount(eIndex) >= 0);

		//adjust default art types
		if(iChange >= 0)
		{
			//multiplayer games default to first art type
			int defaultType = -1;
			if(GC.getGame().isNetworkMultiPlayer())
				defaultType = getProjectDefaultArtType(eIndex);

			for(int i=0; i<iChange; i++)
				m_pavProjectArtTypes[eIndex].push_back(defaultType);
		}
		else
		{
			for(int i=0; i<-iChange; i++)
				m_pavProjectArtTypes[eIndex].pop_back();
		}
		CvAssertMsg(getProjectCount(eIndex) == (int)m_pavProjectArtTypes[eIndex].size(), "[Jason] Unbalanced project art types.");

		CvProjectEntry* pkProject = GC.getProjectInfo(eIndex);

		changeNukeInterception(pkProject->GetNukeInterception() * iChange);

		if((pkProject->GetTechShare() > 0) && (pkProject->GetTechShare() <= MAX_TEAMS))
		{
			changeTechShareCount((pkProject->GetTechShare() - 1), iChange);
		}

		for(int iVictory = 0; iVictory < GC.getNumVictoryInfos(); ++iVictory)
		{
			if(pkProject->GetVictoryThreshold(iVictory) > 0)
			{
				m_abCanLaunch[iVictory] = GC.getGame().testVictory((VictoryTypes)iVictory, GetID());
				GC.getGame().testVictory();
			}
		}

		if(iChange > 0)
		{
			if(pkProject->GetEveryoneSpecialUnit() != NO_SPECIALUNIT)
			{
				GC.getGame().makeSpecialUnitValid((SpecialUnitTypes)(pkProject->GetEveryoneSpecialUnit()));
			}

			if(pkProject->IsAllowsNukes())
			{
				GC.getGame().makeNukesValid(true);
			}
#if defined(MOD_BALANCE_CORE)
			bool bFirst = true;
			if(!pkProject->IsSpaceship())
			{
				for(int iK = 0; iK < MAX_TEAMS; iK++)
				{
					const TeamTypes eLoopTeam = static_cast<TeamTypes>(iK);
					CvTeam& kLoopTeam = GET_TEAM(eLoopTeam);
					if(kLoopTeam.isAlive() && !kLoopTeam.isMinorCiv())
					{
						if(eLoopTeam != GetID())
						{
							if(kLoopTeam.getProjectCount(eIndex) > 0)
							{
								bFirst = false;
								break;
							}
						}
					}
				}
			}
			if(bFirst)
			{
				BuildingClassTypes eBuildingClass = pkProject->GetFreeBuilding();
				if(eBuildingClass != NO_BUILDINGCLASS)
				{
					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if(pkBuildingClassInfo)
					{
						for(int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							if(GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if(GET_PLAYER((PlayerTypes)iJ).getTeam() == GetID())
								{
									BuildingTypes eBuilding = (BuildingTypes) GET_PLAYER((PlayerTypes)iJ).getCivilizationInfo().getCivilizationBuildings(eBuildingClass);

									CvCity* pCapital = GET_PLAYER((PlayerTypes)iJ).getCapitalCity();

									if(pCapital == NULL)
										continue;

									bool bRome = GET_PLAYER((PlayerTypes)iJ).GetPlayerTraits()->IsKeepConqueredBuildings();
									if ((MOD_BUILDINGS_THOROUGH_PREREQUISITES || bRome) && pCapital->HasBuildingClass(eBuildingClass))
									{
										eBuilding = pCapital->GetCityBuildings()->GetBuildingTypeFromClass(eBuildingClass);
									}

									CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo(eBuilding);
									if (pBuildingEntry == NULL)
										continue;

									pCapital->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
									pCapital->GetCityBuildings()->SetNumFreeBuilding(eBuilding, 1);
								}
							}
						}
					}
				}
				PolicyTypes ePolicy = pkProject->GetFreePolicy();
				if(ePolicy != NO_POLICY)
				{
					for(int iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if(GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							if(GET_PLAYER((PlayerTypes)iJ).getTeam() == GetID())
							{
								GET_PLAYER((PlayerTypes)iJ).setHasPolicy(ePolicy, true);
							}
						}
					}
				}	
			}
#endif

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
					{
						if(!(GET_PLAYER((PlayerTypes)iI).isHuman()))
						{
							bChangeProduction = false;

							for(iJ = 0; iJ < GC.getNumProjectInfos(); iJ++)
							{
								if((getProjectCount(eIndex) >= GC.getProjectInfo((ProjectTypes)iJ)->GetProjectsNeeded(eIndex)) &&
								        (iOldProjectCount < GC.getProjectInfo((ProjectTypes)iJ)->GetProjectsNeeded(eIndex)))
								{
									bChangeProduction = true;
									break;
								}
							}

							if(bChangeProduction)
							{
								//GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
							}
						}
					}
				}
			}

			if(GC.getGame().isFinalInitialized())
			{
#if defined(MOD_BALANCE_CORE)
				if (!pkProject->IsRepeatable())
				{
					if (bFirst)
					{
						CvString strSomeoneCompletesProject = GetLocalizedText("TXT_KEY_MISC_COMPLETES_PROJECT_FIRST", getName().GetCString(), pkProject->GetTextKey());
						CvString strSomeoneCompletedProject = GetLocalizedText("TXT_KEY_MISC_SOMEONE_HAS_COMPLETED_FIRST", getName().GetCString(), pkProject->GetTextKey());
						CvString strUnknownCompletesProject = GetLocalizedText("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN_FIRST", pkProject->GetTextKey());

						const PlayerTypes eTeamLeader = getLeaderID();
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eTeamLeader, strSomeoneCompletesProject);
						CvPlayerAI& playerWhoLeadsTeam = GET_PLAYER(eTeamLeader);
						CvCity* pLeadersCapital = playerWhoLeadsTeam.getCapitalCity();
						if (pLeadersCapital)
						{
							for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
							{
								const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
								CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

								if (kPlayer.isAlive())
								{
									if (isHasMet(kPlayer.getTeam()))
									{
										if (ePlayer == GC.getGame().getActivePlayer())
										{
											DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strSomeoneCompletedProject);
										}
										CvNotifications* pNotifications = kPlayer.GetNotifications();
										pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strSomeoneCompletedProject, strSomeoneCompletedProject, pLeadersCapital->getX(), pLeadersCapital->getY(), eIndex, playerWhoLeadsTeam.GetID());
									}
									else
									{
										if (ePlayer == GC.getGame().getActivePlayer())
										{
											DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strUnknownCompletesProject);
										}
										CvNotifications* pNotifications = kPlayer.GetNotifications();
										pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strUnknownCompletesProject, strUnknownCompletesProject, -1, -1, eIndex, NO_PLAYER);
									}
								}
							}
						}
					}
					else
					{
#endif
						CvString strSomeoneCompletesProject = GetLocalizedText("TXT_KEY_MISC_COMPLETES_PROJECT", getName().GetCString(), pkProject->GetTextKey());
						CvString strSomeoneCompletedProject = GetLocalizedText("TXT_KEY_MISC_SOMEONE_HAS_COMPLETED", getName().GetCString(), pkProject->GetTextKey());
						CvString strUnknownCompletesProject = GetLocalizedText("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN", pkProject->GetTextKey());

						const PlayerTypes eTeamLeader = getLeaderID();
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eTeamLeader, strSomeoneCompletesProject);

						CvPlayerAI& playerWhoLeadsTeam = GET_PLAYER(eTeamLeader);
						CvCity* pLeadersCapital = playerWhoLeadsTeam.getCapitalCity();

						if (pLeadersCapital)
						{
							for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
							{
								const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
								CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

								if (kPlayer.isAlive())
								{
									if (isHasMet(kPlayer.getTeam()))
									{
										if (ePlayer == GC.getGame().getActivePlayer())
										{
											DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strSomeoneCompletedProject);
										}
										CvNotifications* pNotifications = kPlayer.GetNotifications();
										pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strSomeoneCompletedProject, strSomeoneCompletedProject, pLeadersCapital->getX(), pLeadersCapital->getY(), eIndex, playerWhoLeadsTeam.GetID());
									}
									else
									{
										if (ePlayer == GC.getGame().getActivePlayer())
										{
											DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strUnknownCompletesProject);
										}
										CvNotifications* pNotifications = kPlayer.GetNotifications();
										pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strUnknownCompletesProject, strUnknownCompletesProject, -1, -1, eIndex, NO_PLAYER);
									}
								}
							}
						}
#if defined(MOD_BALANCE_CORE)
					}
				}
#endif
			}
		}
	}

	// Update the amount of a Resource used up by this Project
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		if(GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
				if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
				{
					kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, iChange * GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop));
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getProjectMaking(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectMaking[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::changeProjectMaking(ProjectTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectMaking[eIndex] = (m_paiProjectMaking[eIndex] + iChange);
	CvAssert(getProjectMaking(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getUnitClassCount(UnitClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eIndex);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if(!isTeamUnitClass(eIndex))
	{
		return false;
	}

	CvAssertMsg(getUnitClassCount(eIndex) <= pkUnitClassInfo->getMaxTeamInstances(), "The current unit class count is expected not to exceed the maximum number of instances allowed for this team");

	return ((getUnitClassCount(eIndex) + iExtra) >= pkUnitClassInfo->getMaxTeamInstances());
}


//	--------------------------------------------------------------------------------
void CvTeam::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCount[eIndex] = (m_paiUnitClassCount[eIndex] + iChange);
	CvAssert(getUnitClassCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
	{
		CvAssertMsg(false, "Could not find BuildingClassInfo for BuildingClassType.");
		return false;
	}

	if(!isTeamWonderClass(*pkBuildingClassInfo))
	{
		return false;
	}

	CvAssertMsg(getBuildingClassCount(eIndex) <= pkBuildingClassInfo->getMaxTeamInstances(), "The current building class count is expected not to exceed the maximum number of instances allowed for this team");

	return ((getBuildingClassCount(eIndex) + iExtra) >= pkBuildingClassInfo->getMaxTeamInstances());
}


//	--------------------------------------------------------------------------------
void CvTeam::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCount[eIndex] = (m_paiBuildingClassCount[eIndex] + iChange);
	CvAssert(getBuildingClassCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getObsoleteBuildingCount(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiObsoleteBuildingCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isObsoleteBuilding(BuildingTypes eIndex) const
{
	return (getObsoleteBuildingCount(eIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	bool bOldObsoleteBuilding;
	int iLoop;
	int iI;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		bOldObsoleteBuilding = isObsoleteBuilding(eIndex);

		m_paiObsoleteBuildingCount[eIndex] = (m_paiObsoleteBuildingCount[eIndex] + iChange);
		CvAssert(getObsoleteBuildingCount(eIndex) >= 0);

		if(bOldObsoleteBuilding != isObsoleteBuilding(eIndex))
		{
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
					{
						for(pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
						{
							if(pLoopCity->GetCityBuildings()->GetNumBuilding(eIndex) > 0)
							{
								pLoopCity->processBuilding(eIndex, ((isObsoleteBuilding(eIndex)) ? -pLoopCity->GetCityBuildings()->GetNumBuilding(eIndex) : pLoopCity->GetCityBuildings()->GetNumBuilding(eIndex)), /*bFirst*/ false, /*bObsolete*/ true);
							}
						}
					}
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::enhanceBuilding(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingEntry* thisBuildingEntry = GC.getBuildingInfo(eIndex);
	if(thisBuildingEntry == NULL)
		return;

	if(iChange != 0)
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER(static_cast<PlayerTypes>(i));

			if(kPlayer.isAlive())
			{
				if(kPlayer.getTeam() == GetID())
				{
					for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
					{
						if(pLoopCity->GetCityBuildings()->GetNumBuilding(eIndex) > 0)
						{
							for(int k = 0; k < NUM_YIELD_TYPES; k++)
							{
								int iEnhancedYield = thisBuildingEntry->GetTechEnhancedYieldChange(k) * pLoopCity->GetCityBuildings()->GetNumBuilding(eIndex);
								pLoopCity->ChangeBaseYieldRateFromBuildings(((YieldTypes)k), iEnhancedYield * iChange);
							}
						}
					}
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getTerrainTradeCount(TerrainTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTerrainTradeCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvTeam::isTerrainTrade(TerrainTypes eIndex) const
{
	return (getTerrainTradeCount(eIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::changeTerrainTradeCount(TerrainTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_paiTerrainTradeCount[eIndex] = (m_paiTerrainTradeCount[eIndex] + iChange);
		CvAssert(getTerrainTradeCount(eIndex) >= 0);
	}
}

//	--------------------------------------------------------------------------------
/// Does this team have the UN?
bool CvTeam::IsHomeOfUnitedNations() const
{
	return m_bHomeOfUnitedNations;
}

//	--------------------------------------------------------------------------------
/// Does this team have the UN?
void CvTeam::SetHomeOfUnitedNations(bool bValue)
{
	if(bValue != IsHomeOfUnitedNations())
	{
		m_bHomeOfUnitedNations = bValue;
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getVictoryCountdown(VictoryTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiVictoryCountdown[eIndex];
}

//	--------------------------------------------------------------------------------
void CvTeam::setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiVictoryCountdown[eIndex] = iTurnsLeft;
}


//	--------------------------------------------------------------------------------
void CvTeam::changeVictoryCountdown(VictoryTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_aiVictoryCountdown[eIndex] += iChange;
		CvAssert(m_aiVictoryCountdown[eIndex] >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getVictoryDelay(VictoryTypes eVictory) const
{
	int iExtraDelayPercent = 0;
	for(int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
	{
		CvProjectEntry* pkProject = GC.getProjectInfo((ProjectTypes)iProject);
		int iCount = getProjectCount((ProjectTypes)iProject);

		if(iCount < pkProject->GetVictoryMinThreshold(eVictory))
		{
			CvAssert(false);
			return -1;
		}

		if(iCount < pkProject->GetVictoryThreshold(eVictory))
		{
			iExtraDelayPercent += ((pkProject->GetVictoryThreshold(eVictory)  - iCount) * pkProject->GetVictoryDelayPercent()) / pkProject->GetVictoryThreshold(eVictory);
		}
	}

	return (GC.getGame().victoryDelay(eVictory)  * (100 + iExtraDelayPercent)) / 100;
}

//	--------------------------------------------------------------------------------
void CvTeam::setCanLaunch(VictoryTypes eVictory, bool bCan)
{
	m_abCanLaunch[eVictory] = bCan;
}

//	--------------------------------------------------------------------------------
bool CvTeam::canLaunch(VictoryTypes eVictory) const
{
	return m_abCanLaunch[eVictory];
}

//	--------------------------------------------------------------------------------
void CvTeam::resetVictoryProgress()
{
	for(int iI = 0; iI < GC.getNumVictoryInfos(); ++iI)
	{
		VictoryTypes eVictory = static_cast<VictoryTypes>(iI);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo((VictoryTypes)iI);
		if(pkVictoryInfo)
		{
			if(getVictoryCountdown(eVictory) >= 0 && GC.getGame().getGameState() == GAMESTATE_ON)
			{
				setVictoryCountdown(eVictory, -1);

				for(int iK = 0; iK < GC.getNumProjectInfos(); iK++)
				{
					ProjectTypes eProject = static_cast<ProjectTypes>(iK);
					CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
					if(pkProjectInfo)
					{
						if(pkProjectInfo->GetVictoryMinThreshold(eVictory) > 0)
						{
							changeProjectCount(eProject, -getProjectCount(eProject));
						}
					}
				}

				CvString strBuffer = GetLocalizedText("TXT_KEY_VICTORY_RESET", getName().GetCString(), pkVictoryInfo->GetTextKey());

				for(int iJ = 0; iJ < MAX_PLAYERS; ++iJ)
				{
					if(GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						DLLUI->AddMessage(0, ((PlayerTypes)iJ), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MAJOR_EVENT);
					}
				}

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvTeam::isVictoryAchieved(VictoryTypes eVictory) const
{
	return m_abVictoryAchieved[eVictory];
}

//	--------------------------------------------------------------------------------
void CvTeam::setVictoryAchieved(VictoryTypes eVictory, bool bValue)
{
	if(m_abVictoryAchieved[eVictory] != bValue)
	{
		m_abVictoryAchieved[eVictory] = bValue;
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getVictoryPoints() const
{
	return m_iVictoryPoints;
}

//	--------------------------------------------------------------------------------
int CvTeam::GetScore() const
{
	int iScore = 0;

	// Sum up score for all players
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).getTeam() == GetID())
		{
			iScore += GET_PLAYER(ePlayer).GetScore();
		}
	}

	return iScore;
}

//	--------------------------------------------------------------------------------
void CvTeam::changeVictoryPoints(int iChange)
{
	if(iChange != 0)
	{
		m_iVictoryPoints += iChange;
	}

	// Right now there's no reason VPs should ever be negative
	CvAssert(m_iVictoryPoints >= 0);
}

//	--------------------------------------------------------------------------------
/// See if there are any Small Awards we've just accomplished
void CvTeam::DoTestSmallAwards()
{
	if(!isAlive() || isMinorCiv())
	{
		return;
	}

	int iPlayerLoop;
	PlayerTypes ePlayer;

	CvCity* pLoopCity;
	int iCityLoop;

	int iAwardRequirement;

	bool bShouldShowNotification;
	int iNotificationData;
	int iNotificationX, iNotificationY;

	for(int iSmallAwardLoop = 0; iSmallAwardLoop < GC.getNumSmallAwardInfos(); iSmallAwardLoop++)
	{
		SmallAwardTypes eAward = static_cast<SmallAwardTypes>(iSmallAwardLoop);
		CvSmallAwardInfo* pkSmallAwardInfo = GC.getSmallAwardInfo(eAward);
		if(pkSmallAwardInfo)
		{
			bShouldShowNotification = false;
			iNotificationX = -1;
			iNotificationY = -1;

			if(!IsSmallAwardAchieved(eAward))
			{
				// Cities
				iAwardRequirement = GC.getSmallAwardInfo(eAward)->GetNumCities();
				if(iAwardRequirement > 0)
				{
					for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(ePlayer).getTeam() == GetID())
						{
							if(GET_PLAYER(ePlayer).getNumCities() >= iAwardRequirement)
							{
								SetSmallAwardAchieved(eAward, true);
								changeVictoryPoints(GC.getSmallAwardInfo(eAward)->GetNumVictoryPoints());

								bShouldShowNotification = true;
								iNotificationData = iAwardRequirement;

								if(GET_PLAYER(ePlayer).getCapitalCity() != NULL)
								{
									iNotificationX = GET_PLAYER(ePlayer).getCapitalCity()->getX();
									iNotificationY = GET_PLAYER(ePlayer).getCapitalCity()->getY();
								}
							}
						}
					}
				}

				// Population
				iAwardRequirement = pkSmallAwardInfo->GetCityPopulation();
				if(iAwardRequirement > 0)
				{
					for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(ePlayer).getTeam() == GetID())
						{
							// Check all Cities
							for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
							{
								if(pLoopCity->getPopulation() >= iAwardRequirement)
								{
									SetSmallAwardAchieved(eAward, true);
									changeVictoryPoints(pkSmallAwardInfo->GetNumVictoryPoints());

									bShouldShowNotification = true;
									iNotificationData = iAwardRequirement;
									iNotificationX = pLoopCity->getX();
									iNotificationY = pLoopCity->getY();
								}
							}
						}
					}
				}

				// Give notification to all players on this team
				if(bShouldShowNotification)
				{
					CvString strBuffer;
					CvString strSummary;
					for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(ePlayer).getTeam() == GetID())
						{
							CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
							if(pNotifications)
							{
								if(getNumMembers() == 1)
								{
									strBuffer = GetLocalizedText(pkSmallAwardInfo->GetNotificationString(), iNotificationData, pkSmallAwardInfo->GetNumVictoryPoints());
									strSummary = GetLocalizedText(pkSmallAwardInfo->GetDescription());
								}
								// Team game, different string
								else
								{
									strBuffer = GetLocalizedText(pkSmallAwardInfo->GetTeamNotificationString(), iNotificationData, pkSmallAwardInfo->GetNumVictoryPoints());
									strSummary = GetLocalizedText(pkSmallAwardInfo->GetDescription());
								}
								pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, iNotificationX, iNotificationY, -1);
							}
						}
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Have we achieved a Small Award already?
bool CvTeam::IsSmallAwardAchieved(SmallAwardTypes eAward) const
{
	FAssert(eAward >= 0);
	FAssert(eAward < GC.getNumSmallAwardInfos());

	return m_abSmallAwardAchieved[eAward];
}

//	--------------------------------------------------------------------------------
/// Sets if we have achieved a Small Award
void CvTeam::SetSmallAwardAchieved(SmallAwardTypes eAward, bool bValue)
{
	FAssert(eAward >= 0);
	FAssert(eAward < GC.getNumSmallAwardInfos());

	m_abSmallAwardAchieved[eAward] = bValue;
}

//	--------------------------------------------------------------------------------
void CvTeam::announceTechToPlayers(TechTypes eIndex, bool bPartial)
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eIndex);
	if(pkTechInfo == NULL)
		return;

	const char* szTechTextKey = pkTechInfo->GetTextKey();

	bool bSound = (GC.getGame().isNetworkMultiPlayer() && !bPartial);
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			if(ePlayer == GC.getGame().getActivePlayer())
			{
				CvString strBuffer = GetLocalizedText((bPartial ? "TXT_KEY_MISC_PROGRESS_TOWARDS_TECH" : "TXT_KEY_MISC_YOU_DISCOVERED_TECH"), szTechTextKey);
				DLLUI->AddMessage(0, ePlayer, false, (bSound ? /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME) : -1), strBuffer/*, (bSound ? GC.getTechInfo(eIndex)->GetSoundMP() : NULL), MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_TECH_TEXT")*/);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
void CvTeam::setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce, bool bNoBonus)
#else
void CvTeam::setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce)
#endif
{
	CvCity* pCapitalCity;
	CvCity* pCity;
	CvString strBuffer;
	UnitTypes eFreeUnit;
	bool bFirstResource;
	
	if(eIndex == NO_TECH)
	{
		return;
	}

	CvTechEntry* pkTechInfo = GC.getTechInfo(eIndex);
	if(pkTechInfo == NULL)
	{
		return;
	}

	if(ePlayer == NO_PLAYER)
	{
		ePlayer = getLeaderID();
	}

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(ePlayer == NO_PLAYER)
	{
		return;
	}

	if(GetTeamTechs()->HasTech(eIndex) != bNewValue)
	{
#if defined(MOD_API_ACHIEVEMENTS)
		CvPlayerAI& kResearchingPlayer = GET_PLAYER(ePlayer);

		if(	GC.getGame().getActivePlayer() == ePlayer &&
			strcmp(pkTechInfo->GetType(), "TECH_SATELLITES") == 0 &&
			strcmp(kResearchingPlayer.getCivilizationTypeKey(), "CIVILIZATION_HUNS") == 0 &&
			strcmp(GC.getMap().getWorldInfo().GetType(), "WORLDSIZE_HUGE") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_30);
		}
#endif

		if(pkTechInfo->IsRepeat())
		{
			processTech(eIndex, 1);
			GetTeamTechs()->IncrementTechCount(eIndex);

			GetTeamTechs()->SetResearchProgress(eIndex, 0, ePlayer);

			// Repeating techs are good for score!
			int iScoreChange = /*10*/ GD_INT_GET(SCORE_FUTURE_TECH_MULTIPLIER);
			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
				CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if(kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == GetID())
				{
					kLoopPlayer.ChangeScoreFromFutureTech(iScoreChange);
				}
			}

			// report event to Python
			////gDLL->getEventReporterIFace()->techAcquired(eIndex, GetID(), ePlayer, bAnnounce && 1 == GetTeamTechs()->GetTechCount(eIndex));

			if(1 == GetTeamTechs()->GetTechCount(eIndex))
			{
				if (bNewValue)
					GetTeamTechs()->ChangeNumTechsKnown(1);
				else
					GetTeamTechs()->ChangeNumTechsKnown(-1);

				if(bAnnounce)
				{
					if(GC.getGame().isFinalInitialized())
					{
						announceTechToPlayers(eIndex);
					}
				}
			}
		}
		else
		{
			GetTeamTechs()->SetHasTech(eIndex, bNewValue);

			if (bNewValue)
				GetTeamTechs()->ChangeNumTechsKnown(1);
			else
				GetTeamTechs()->ChangeNumTechsKnown(-1);

#if defined(MOD_BALANCE_CORE)
			if (bNewValue)
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
					CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
					if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == GetID())
					{
						if(kLoopPlayer.GetPlayerTraits()->GetFreePolicyPerXTechs() > 0)
						{
							int iRemainder = (GetTeamTechs()->GetNumTechsKnown() % kLoopPlayer.GetPlayerTraits()->GetFreePolicyPerXTechs());
							if (iRemainder == 0)
							{
								kLoopPlayer.ChangeNumFreePolicies(1);
							}
						}
					}
				}
			}
#endif

			// Tech progress affects city strength, so update
			CvCity* pLoopCity;
			int iLoop;

			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
				CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if(kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == GetID())
				{
					for(pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						pLoopCity->updateStrengthValue();
#if defined(MOD_BALANCE_CORE)
						pLoopCity->GetCityCitizens()->SetDirty(true);
#endif
					}
				}
			}

			NotificationTypes eNotificationType = NO_NOTIFICATION_TYPE;

			const bool bIsActiveTeam = (GetID() == GC.getGame().getActiveTeam());
			const int iNumPlots = GC.getMap().numPlots();
			for(int iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
#if defined(MOD_BALANCE_CORE)
				if(pLoopPlot != NULL)
				{
					pLoopPlot->updateImpassable(m_eID);
				}
#endif
				const ResourceTypes eResource = pLoopPlot->getResourceType();
				if(eResource != NO_RESOURCE)
				{
					CvResourceInfo* pResourceInfo = GC.getResourceInfo(eResource);
					CvAssert(pResourceInfo);

					if(bNewValue)
					{
						TechTypes eDefaultTech = (TechTypes)pResourceInfo->getTechReveal();
						PolicyTypes ePolicyReveal = (PolicyTypes)pResourceInfo->getPolicyReveal();
						bool bRevealTech = false;
						bool bRevealed = false;

						for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
						{
							const PlayerTypes ePlayer = (PlayerTypes)*iI;
							CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

							if (pPlayer && pPlayer->isAlive())
							{
								TechTypes eTechReveal = eDefaultTech;

#if defined(MOD_BALANCE_CORE)
								if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
								{
									TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechReveal;
									if (eAltTech != NO_TECH)
									{
										eTechReveal = eAltTech;
									}
								}
#endif
								if (pPlayer->HasPolicy(ePolicyReveal) || (eTechReveal != eIndex && GetTeamTechs()->HasTech(eTechReveal)))
								{
									bRevealed = true;
									break;
								}
								else if (!bRevealTech && eTechReveal == eIndex)
								{
									bRevealTech = true;
								}
							}
						}
						// Resource Reveal
						if(bRevealTech && !bRevealed)
						{
							// update the resources
							if(pLoopPlot->isRevealed(m_eID))
							{
								if(pLoopPlot->getResourceType(m_eID) != NO_RESOURCE)
								{
									// Only update for active team
									if(bIsActiveTeam)
									{
										// If we've force revealed the resource for this team, then don't send another event
										if(!pLoopPlot->IsResourceForceReveal(GetID()))
										{
											pLoopPlot->setLayoutDirty(true);
										}

										// Notify the player that owns this Plot
										if(pLoopPlot->getOwner() == GC.getGame().getActivePlayer() && pLoopPlot->getTeam() == GetID())
										{
											if(!CvPreGame::loadWBScenario() || GC.getGame().getGameTurn() > 0)
											{
												strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RESOURCE", pResourceInfo->GetTextKey());
									
												CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RESOURCE", pResourceInfo->GetTextKey());

												switch(pResourceInfo->getResourceUsage())
												{
												case RESOURCEUSAGE_LUXURY:
													eNotificationType = NOTIFICATION_DISCOVERED_LUXURY_RESOURCE;
													break;
												case RESOURCEUSAGE_STRATEGIC:
													eNotificationType = NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE;
													break;
												case RESOURCEUSAGE_BONUS:
													eNotificationType = NOTIFICATION_DISCOVERED_BONUS_RESOURCE;
													break;
												}

												CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
												if(pNotifications)
												{
													pNotifications->Add(eNotificationType, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), eResource);
												}
											}
										}
									}
								}
							}
						}
					}
					// Resource Connection
					if(pLoopPlot->getTeam() == GetID())
					{
						// Check if this tech unlocks city trade for any of a team's players, and check if the resource has already been unlocked
						bool bUnlocksResource = false;
						bool bResourceUnlocked = false;
						TechTypes eDefaultTech = (TechTypes)pResourceInfo->getTechCityTrade();
						for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
						{
							const PlayerTypes ePlayer = (PlayerTypes)*iI;
							CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

							if (pPlayer && pPlayer->isAlive())
							{
								TechTypes eTech = eDefaultTech;
#if defined(MOD_BALANCE_CORE)
								if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
								{
									TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechCityTrade;
									if (eAltTech != NO_TECH)
									{
										eTech = eAltTech;
									}
								}
#endif
								// Has this resource been unlocked by another tech?
								if (eTech != eIndex && GetTeamTechs()->HasTech(eTech))
								{
									bResourceUnlocked = true;
									break; // Resource already unlocked, so we're stopping the loop
								}
								// If the resource is still locked, will eIndex unlock our resource?
								else if (!bUnlocksResource && eTech == eIndex)
								{
									bUnlocksResource = true;
								}
							}
						}

						if(!bResourceUnlocked && bUnlocksResource)
						{
							// Appropriate Improvement on this Plot?
							if (pLoopPlot->isCity() || (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType())->IsConnectsResource(eResource)))
							{
								for (int iI = 0; iI < MAX_PLAYERS; iI++)
								{
									const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
									CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
									if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == GetID() && pLoopPlot->getOwner() == eLoopPlayer)
									{
										// We now have a new Tech
										if (bNewValue)
										{
											// slewis - added in so resources wouldn't be double counted when the minor civ researches the technology
											if (!(kLoopPlayer.isMinorCiv() && pLoopPlot->IsImprovedByGiftFromMajor()))
											{
												kLoopPlayer.changeNumResourceTotal(eResource, pLoopPlot->getNumResourceForPlayer(eLoopPlayer));
											}

											// Reconnect resource link
											if (pLoopPlot->getEffectiveOwningCity() != NULL)
											{
												pLoopPlot->SetResourceLinkedCityActive(true);
											}
										}
										// Removing Tech
										else
										{
											kLoopPlayer.changeNumResourceTotal(eResource, -pLoopPlot->getNumResourceForPlayer(eLoopPlayer));

											// Disconnect resource link
											if (pLoopPlot->getEffectiveOwningCity() != NULL)
												pLoopPlot->SetResourceLinkedCityActive(false);
										}
									}
								}
							}
						}
					}
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		processTech(eIndex, ((bNewValue) ? 1 : -1), bNoBonus);
#else
		processTech(eIndex, ((bNewValue) ? 1 : -1));
#endif

		//Antiquity site notifications.
		//Notifications for Artifacts and Hidden Artifacts have to come AFTER processTech because they may not have been spawned yet.
		if(!CvPreGame::loadWBScenario() || GC.getGame().getGameTurn() > 0)
		{
			bool bTechRevealsArtifacts = false;
			bool bTechRevealsHiddenArtifacts = false;
			bool bArtifactRevealed = false;
			bool bHiddenArtifactRevealed = false;

			ResourceTypes eArtifactResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ARTIFACTS", true);
			CvResourceInfo* pArtifactResource = NULL;
			if(eArtifactResource != NO_RESOURCE)
			{
				pArtifactResource = GC.getResourceInfo(eArtifactResource);
				TechTypes eDefaultTech = (TechTypes)pArtifactResource->getTechReveal();
				PolicyTypes eRevealPolicy = (PolicyTypes)pArtifactResource->getPolicyReveal();

				for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
				{
					const PlayerTypes ePlayer = (PlayerTypes)*iI;
					CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

					if (pPlayer && pPlayer->isAlive())
					{
						TechTypes eTechReveal = eDefaultTech;

#if defined(MOD_BALANCE_CORE)
						if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
						{
							TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eArtifactResource).m_eTechReveal;
							if (eAltTech != NO_TECH)
							{
								eTechReveal = eAltTech;
							}
						}
#endif
						if (pPlayer->HasPolicy(eRevealPolicy) || (eTechReveal != eIndex && GetTeamTechs()->HasTech(eTechReveal)))
						{
							bArtifactRevealed = true;
							break;
						}
						else if (!bTechRevealsArtifacts && eTechReveal == eIndex)
						{
							bTechRevealsArtifacts = true;
						}
					}
				}
			}

			ResourceTypes eHiddenArtifactResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_HIDDEN_ARTIFACTS", true);
			CvResourceInfo* pHiddenArtifactResource = NULL;
			if(eHiddenArtifactResource != NO_RESOURCE)
			{
				pHiddenArtifactResource = GC.getResourceInfo(eHiddenArtifactResource);
				TechTypes eDefaultTech = (TechTypes)pHiddenArtifactResource->getTechReveal();
				PolicyTypes eRevealPolicy = (PolicyTypes)pHiddenArtifactResource->getPolicyReveal();

				for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
				{
					const PlayerTypes ePlayer = (PlayerTypes)*iI;
					CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

					if (pPlayer && pPlayer->isAlive())
					{
						TechTypes eTechReveal = eDefaultTech;

#if defined(MOD_BALANCE_CORE)
						if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
						{
							TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eHiddenArtifactResource).m_eTechReveal;
							if (eAltTech != NO_TECH)
							{
								eTechReveal = eAltTech;
							}
						}
#endif
						if (pPlayer->HasPolicy(eRevealPolicy) || (eTechReveal != eIndex && GetTeamTechs()->HasTech(eTechReveal)))
						{
							bHiddenArtifactRevealed = true;
							break;
						}
						else if (!bTechRevealsHiddenArtifacts && eTechReveal == eIndex)
						{
							bTechRevealsHiddenArtifacts = true;
						}
					}
				}
			}

			if((bTechRevealsArtifacts && !bArtifactRevealed) || (bTechRevealsHiddenArtifacts && !bHiddenArtifactRevealed))
			{
				const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
				const int iNumPlots = GC.getMap().numPlots();
				for(int iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
				{
					CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

					if(pLoopPlot->getOwner() == eActivePlayer && pLoopPlot->getTeam() == m_eID && pLoopPlot->isRevealed(m_eID))
					{
						const ResourceTypes ePlotResource = pLoopPlot->getResourceType(m_eID);
						if( (bTechRevealsArtifacts && ePlotResource == eArtifactResource) || 
							(bTechRevealsHiddenArtifacts && ePlotResource == eHiddenArtifactResource))
						{
							CvString strSummary;
					//		CvString strBuffer;
							ResourceUsageTypes eResourceUsage = RESOURCEUSAGE_LUXURY;
							NotificationTypes eNotificationType = NO_NOTIFICATION_TYPE;

							if(ePlotResource == eArtifactResource)
							{
								strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_ARTIFACTS");
								strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RESOURCE", pArtifactResource->GetTextKey());
								eResourceUsage = pArtifactResource->getResourceUsage();
							}
							else if(ePlotResource == eHiddenArtifactResource)
							{
								strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_HIDDEN_ARTIFACTS");
								strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RESOURCE", pHiddenArtifactResource->GetTextKey());
								eResourceUsage = pHiddenArtifactResource->getResourceUsage();
							}

							switch(eResourceUsage)
							{
							case RESOURCEUSAGE_LUXURY:
								eNotificationType = NOTIFICATION_DISCOVERED_LUXURY_RESOURCE;
								break;
							case RESOURCEUSAGE_STRATEGIC:
								eNotificationType = NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE;
								break;
							case RESOURCEUSAGE_BONUS:
								eNotificationType = NOTIFICATION_DISCOVERED_BONUS_RESOURCE;
								break;
							}

							CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
							if(pNotifications)
							{
								pNotifications->Add(eNotificationType, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), ePlotResource);
							}
						}
					}
				}
			}

		}

		if(GetTeamTechs()->HasTech(eIndex))
		{
			EraTypes eThisTechsEra = (EraTypes) pkTechInfo->GetEra();

			// Era change for this Team?
			if(GetCurrentEra() < eThisTechsEra)
			{
				EraTypes eNewEra = (EraTypes) pkTechInfo->GetEra();

				SetCurrentEra(eNewEra);
				if (isMajorCiv())
				{
					GC.getGame().UpdateGameEra();
				}
#if defined(MOD_BALANCE_CORE)
				if(!bNoBonus)
				{
					for(int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
						CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
						if(kPlayer.isAlive() && kPlayer.getTeam() == GetID() && kPlayer.isMajorCiv())
						{
							int iTourism = GET_PLAYER(eLoopPlayer).GetHistoricEventTourism(HISTORIC_EVENT_ERA);
							GET_PLAYER(eLoopPlayer).ChangeNumHistoricEvents(HISTORIC_EVENT_ERA, 1);
							// Culture boost based on previous turns
							if(iTourism > 0)
							{
								GET_PLAYER(eLoopPlayer).GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
								if(eLoopPlayer == GC.getGame().getActivePlayer())
								{
									CvCity* pCity = GET_PLAYER(eLoopPlayer).getCapitalCity();
									if(pCity != NULL)
									{
										char text[256] = {0};
				
										sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
										SHOW_PLOT_POPUP(pCity->plot(), eLoopPlayer, text);
										CvNotifications* pNotification = GET_PLAYER(eLoopPlayer).GetNotifications();
										if(pNotification)
										{
											CvString strMessage;
											CvString strSummary;
											strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_ERA", iTourism);
											strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
											pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, pCity->getX(), pCity->getY(), eLoopPlayer);
										}
									}
								}
							}		
							if (GET_PLAYER(eLoopPlayer).GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
							{
								bool bChange = false;
								// Look at all Cities
								int iLoop;
								for (CvCity* pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
								{
									for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
									{
										//We leave science alone, it's used for pop science in vanilla.
										if ((YieldTypes)iJ == YIELD_SCIENCE)
											continue;

										if (GET_PLAYER(eLoopPlayer).GetPlayerTraits()->GetPermanentYieldChangeWLTKD((YieldTypes)iJ) <= 0)
											continue;

										int iValue = pLoopCity->GetBaseYieldRateFromMisc((YieldTypes)iJ);
										iValue *= 50;
										iValue /= 100;
										pLoopCity->ChangeBaseYieldRateFromMisc((YieldTypes)iJ, -iValue);

										if (pLoopCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
										{
											CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iJ);
											if (pYieldInfo)
											{
												char text[256] = { 0 };
												CvString yieldString = "";
												yieldString.Format("%s+%%d[ENDCOLOR] %s", pYieldInfo->getColorString(), pYieldInfo->getIconString());
												sprintf_s(text, yieldString, -iValue);
												SHOW_PLOT_POPUP(pLoopCity->plot(), NO_PLAYER,  text);
											}
										}
										bChange = true;
									}
								}
								if (bChange)
								{
									CvNotifications* pNotification = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotification)
									{
										CvString strMessage;
										CvString strSummary;
										strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_ERA_CHANGE");
										strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CITY_SUMMARY_WLTKD_UA_ERA_CHANGE");
										pNotification->Add(NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER, strMessage, strSummary, -1, -1, eLoopPlayer);
									}
								}
							}
						}
					}
				}
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
				if(!bNoBonus)
				{
					for(int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
						CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
						if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
						{
							kPlayer.doInstantYield(INSTANT_YIELD_TYPE_ERA_UNLOCK, false, NO_GREATPERSON, NO_BUILDING, 0 , true);
						}
					}
				}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				if (MOD_DIPLOMACY_CIV4_FEATURES && !bNoBonus && GetNumVassals() > 0)
				{
					// Check all players on this team
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
						CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);

						if (kPlayer.isAlive() && kPlayer.getTeam() == GetID())
						{
							kPlayer.SetVassalLevy(true);
						}
					}
				}
#endif
			}
			// DON'T enter a new era with this tech
			else
			{
				// If we have all the techs in the earlier eras, then we're in that era
				bool bHasAllEarlierTechs = true;
				for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
				{
					const TechTypes eLoopTech = static_cast<TechTypes>(iTechLoop);
					CvTechEntry* pkLoopTech = GC.getTechInfo(eLoopTech);
					if(pkLoopTech)
					{
						// Only look at techs prior to our era - there's no need to go further
						if(pkLoopTech->GetEra() <= GetCurrentEra())
						{
							if(!GetTeamTechs()->HasTech(eLoopTech))
							{
								bHasAllEarlierTechs = false;
								break;
							}
						}
					}
				}

				if(bHasAllEarlierTechs)
				{
					const EraTypes currentEra = GetCurrentEra();
					const int iNextEra = static_cast<int>(currentEra) + 1;
					if(iNextEra < GC.getNumEraInfos())
					{
						const EraTypes nextEra = static_cast<EraTypes>(iNextEra);
						const CvEraInfo* pkEraInfo = GC.getEraInfo(nextEra);
						if(pkEraInfo != NULL)
						{
							SetCurrentEra(nextEra);
							if (isMajorCiv())
							{
								GC.getGame().UpdateGameEra();
							}
#if defined(MOD_BALANCE_CORE)
							if(!bNoBonus)
							{
								for(int iI = 0; iI < MAX_PLAYERS; iI++)
								{
									const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
									CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
									if(kPlayer.isAlive() && kPlayer.getTeam() == GetID() && kPlayer.isMajorCiv())
									{
										int iTourism = GET_PLAYER(eLoopPlayer).GetHistoricEventTourism(HISTORIC_EVENT_ERA);
										GET_PLAYER(eLoopPlayer).ChangeNumHistoricEvents(HISTORIC_EVENT_ERA, 1);
										// Culture boost based on previous turns
										if(iTourism > 0)
										{
											GET_PLAYER(eLoopPlayer).GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
											if(eLoopPlayer == GC.getGame().getActivePlayer())
											{
												CvCity* pCity = GET_PLAYER(eLoopPlayer).getCapitalCity();
												if(pCity != NULL)
												{
													char text[256] = {0};
							
													sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
													SHOW_PLOT_POPUP(pCity->plot(), eLoopPlayer, text);
													CvNotifications* pNotification = GET_PLAYER(eLoopPlayer).GetNotifications();
													if(pNotification)
													{
														CvString strMessage;
														CvString strSummary;
														strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_ERA", iTourism);
														strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
														pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, pCity->getX(), pCity->getY(), eLoopPlayer);
													}
												}
											}
										}
										if (GET_PLAYER(eLoopPlayer).GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
										{
											bool bChange = false;
											// Look at all Cities
											int iLoop;
											for (CvCity* pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
											{
												for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
												{
													//We leave science alone, it's used for pop science in vanilla.
													if ((YieldTypes)iJ == YIELD_SCIENCE)
														continue;

													if (GET_PLAYER(eLoopPlayer).GetPlayerTraits()->GetPermanentYieldChangeWLTKD((YieldTypes)iJ) <= 0)
														continue;

													int iValue = pLoopCity->GetBaseYieldRateFromMisc((YieldTypes)iJ);
													iValue *= 50;
													iValue /= 100;
													pLoopCity->ChangeBaseYieldRateFromMisc((YieldTypes)iJ, -iValue);

													if (pLoopCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
													{
														CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iJ);
														if (pYieldInfo)
														{
															char text[256] = { 0 };
															CvString yieldString = "";
															yieldString.Format("%s+%%d[ENDCOLOR] %s", pYieldInfo->getColorString(), pYieldInfo->getIconString());
															sprintf_s(text, yieldString, -iValue);
															SHOW_PLOT_POPUP(pLoopCity->plot(), NO_PLAYER, text);
														}
													}
													bChange = true;
												}
											}
											if (bChange)
											{
												CvNotifications* pNotification = GET_PLAYER(eLoopPlayer).GetNotifications();
												if (pNotification)
												{
													CvString strMessage;
													CvString strSummary;
													strMessage = GetLocalizedText("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_ERA_CHANGE");
													strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CITY_SUMMARY_WLTKD_UA_ERA_CHANGE");
													pNotification->Add(NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER, strMessage, strSummary, -1, -1, eLoopPlayer);
												}
											}
										}
									}
								}
							}
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
							if(!bNoBonus)
							{
								for(int iI = 0; iI < MAX_PLAYERS; iI++)
								{
									const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
									CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
									if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
									{
										kPlayer.doInstantYield(INSTANT_YIELD_TYPE_ERA_UNLOCK, false, NO_GREATPERSON, NO_BUILDING, 0, true);
									}
								}
							}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
							if (MOD_DIPLOMACY_CIV4_FEATURES && !bNoBonus && GetNumVassals() > 0)
							{
								// Check all players on this team
								for (int iI = 0; iI < MAX_PLAYERS; iI++)
								{
									const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
									CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);

									if (kPlayer.isAlive() && kPlayer.getTeam() == GetID())
									{
										kPlayer.SetVassalLevy(true);
									}
								}
							}
#endif
						}
					}

				}
			}

			if(pkTechInfo->IsMapVisible())
			{
				GC.getMap().setRevealedPlots(GetID(), true, true);
				GC.getMap().updateDeferredFog();
			}

			DoUpdateBestRoute();

			// report event to Python, along with some other key state
			//////gDLL->getEventReporterIFace()->techAcquired(eIndex, GetID(), ePlayer, bAnnounce);

			bFirstResource = false;

			for(int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
				CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
				if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
				{
					if(kPlayer.GetPlayerTechs()->IsResearchingTech(eIndex))
					{
						kPlayer.popResearch(eIndex);
						kPlayer.GetPlayerTechs()->GetTechAI()->LogResearchCompleted(eIndex);
					}

					// notify the player they now have the tech, if they want to make immediate changes
					//GET_PLAYER((PlayerTypes)iI).AI_nowHasTech(eIndex);

					kPlayer.invalidateYieldRankCache();
				}
			}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			if(MOD_DIPLOMACY_CIV4_FEATURES && GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
			{
				SetTradeTech(eIndex, true);
			}
#endif

			if(bFirst)
			{
				if(GC.getGame().countKnownTechNumTeams(eIndex) == 1)
				{
					if (pkTechInfo->GetFirstFreeUnitClass() != NO_UNITCLASS)
					{
						bFirstResource = true;

						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
							CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
							if (kPlayer.isAlive() && kPlayer.getTeam() == GetID())
							{
								eFreeUnit = ((UnitTypes)(GET_PLAYER(eLoopPlayer).GetSpecificUnitType((UnitClassTypes)GC.getTechInfo(eIndex)->GetFirstFreeUnitClass())));

								if (eFreeUnit != NULL)
								{
									pCapitalCity = GET_PLAYER(eLoopPlayer).getCapitalCity();

									if (pCapitalCity != NULL)
									{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
										pCapitalCity->GetCityCitizens()->DoSpawnGreatPerson(eFreeUnit, true, false, MOD_GLOBAL_TRULY_FREE_GP);
#else
										pCapitalCity->GetCityCitizens()->DoSpawnGreatPerson(eFreeUnit, true, false);
#endif
									}
								}
							}
						}
					}

					if(pkTechInfo->GetFirstFreeTechs() > 0)
					{
						bFirstResource = true;
						
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
							CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
							if (kPlayer.isAlive() && kPlayer.getTeam() == GetID())
							{

								if (!kPlayer.isHuman())
								{
									for (int iI = 0; iI < pkTechInfo->GetFirstFreeTechs(); iI++)
									{
										kPlayer.AI_chooseFreeTech();
									}
								}
								else
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_FIRST_TECH_CHOOSE_FREE", pkTechInfo->GetTextKey());
									kPlayer.chooseTech(GC.getTechInfo(eIndex)->GetFirstFreeTechs(), strBuffer.GetCString());
								}
							}
						}

						for(int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
							CvPlayerAI& kLoopPLayer = GET_PLAYER(eLoopPlayer);
							if(kLoopPLayer.isAlive() && GC.getGame().getActivePlayer())
							{
								if(isHasMet(kLoopPLayer.getTeam()))
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getNameKey(), pkTechInfo->GetTextKey());
								}
								else
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_FIRST_TO_TECH", GC.getTechInfo(eIndex)->GetTextKey());
								}
								DLLUI->AddMessage(0, eLoopPlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")*/);
							}
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getName(), GC.getTechInfo(eIndex)->GetTextKey());
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, ePlayer, strBuffer, -1, -1);
					}

					if(bFirstResource)
					{
						for(int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							const PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iI);
							CvPlayerAI& kPlayer = GET_PLAYER(eLoopPlayer);
							if(kPlayer.isAlive())
							{
								if(!(kPlayer.isHuman()))
								{
									if(kPlayer.GetPlayerTechs()->IsResearchingTech(eIndex))
									{
										kPlayer.clearResearchQueue();
									}
								}
							}
						}
					}
				}
			}

			if(bAnnounce)
			{
				if(GC.getGame().isFinalInitialized()/* && !(gDLL->GetWorldBuilderMode())*/)
				{
					announceTechToPlayers(eIndex);

					// Resources discovered in a player's territory
					for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
					{
						CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

						if(pLoopPlot->getTeam() == GetID() && pLoopPlot->getOwner() == GC.getGame().getActivePlayer())
						{
							ResourceTypes eResource = pLoopPlot->getResourceType();

							if(eResource != NO_RESOURCE)
							{
								TechTypes eDefaultTech = (TechTypes)GC.getResourceInfo(eResource)->getTechReveal();
								PolicyTypes eRevealPolicy = (PolicyTypes)GC.getResourceInfo(eResource)->getPolicyReveal();
								bool bRevealed = false;
								bool bReveals = false;

								for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
								{
									const PlayerTypes ePlayer = (PlayerTypes)*iI;
									CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

									if (pPlayer && pPlayer->isAlive())
									{
										TechTypes eTechReveal = eDefaultTech;

#if defined(MOD_BALANCE_CORE)
										if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
										{
											TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechReveal;
											if (eAltTech != NO_TECH)
											{
												eTechReveal = eAltTech;
											}
										}
#endif
										if (pPlayer->HasPolicy(eRevealPolicy) || (eTechReveal != eIndex && GetTeamTechs()->HasTech(eTechReveal)))
										{
											bRevealed = true;
											break;
										}
										else if (!bReveals && eTechReveal == eIndex)
										{
											bReveals = true;
										}
									}
								}

								if(bReveals && !bRevealed && !isForceRevealedResource(eResource))
								{
									pCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), NO_PLAYER, GetID(), false);

									if(pCity != NULL)
									{
										CvResourceInfo* pResourceInfo = GC.getResourceInfo(eResource);

										if(strcmp(pResourceInfo->GetType(), "RESOURCE_ARTIFACTS") == 0)
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_DISCOVERED_ARTIFACTS_NEAR", pCity->getNameKey());
										}
										else if(strcmp(pResourceInfo->GetType(), "RESOURCE_HIDDEN_ARTIFACTS") == 0)
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_DISCOVERED_HIDDEN_ARTIFACTS_NEAR", pCity->getNameKey());
										}
										else
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_DISCOVERED_RESOURCE", pResourceInfo->GetTextKey(), pCity->getNameKey());
										}

										DLLUI->AddPlotMessage(0, pLoopPlot->GetPlotIndex(), pLoopPlot->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_DISCOVERRESOURCE", MESSAGE_TYPE_INFO, GC.getResourceInfo(eResource)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopPlot->getX(), pLoopPlot->getY(), true, true*/);
									}
								}
							}
						}
					}

					// Cities demand a Resource that's been newly revealed
					CvCity* pLoopCity;
					int iLoop;
					PlayerTypes eLoopPlayer = GC.getGame().getActivePlayer();
					ResourceTypes eResourceDemanded;

					// Check all players on this team
					if(GET_PLAYER(eLoopPlayer).getTeam() == GetID())
					{
						// Look at all Cities
						for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
						{
							eResourceDemanded = pLoopCity->GetResourceDemanded();

							if(eResourceDemanded != NO_RESOURCE)
							{
								// Resource being demanded is revealed by this Tech
								TechTypes eDefaultTech = (TechTypes)GC.getResourceInfo(eResourceDemanded)->getTechReveal();
								PolicyTypes eRevealPolicy = (PolicyTypes)GC.getResourceInfo(eResourceDemanded)->getPolicyReveal();
								bool bRevealed = false;
								bool bReveals = false;

								for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
								{
									const PlayerTypes ePlayer = (PlayerTypes)*iI;
									CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

									if (pPlayer && pPlayer->isAlive())
									{
										TechTypes eTechReveal = eDefaultTech;

#if defined(MOD_BALANCE_CORE)
										if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
										{
											TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResourceDemanded).m_eTechReveal;
											if (eAltTech != NO_TECH)
											{
												eTechReveal = eAltTech;
											}
										}
#endif
										if (pPlayer->HasPolicy(eRevealPolicy) || (eTechReveal != eIndex && GetTeamTechs()->HasTech(eTechReveal)))
										{
											bRevealed = true;
											break;
										}
										else if (!bReveals && eTechReveal == eIndex)
										{
											bReveals = true;
										}
									}
								}

								if (bReveals && !bRevealed)
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_RESOURCE_DISCOVERED_CITY_DEMANDS", GC.getResourceInfo(eResourceDemanded)->GetTextKey(), pLoopCity->getNameKey());
									DLLUI->AddCityMessage(0, pLoopCity->GetIDInfo(), eLoopPlayer, false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_DISCOVERRESOURCE", MESSAGE_TYPE_INFO, GC.getResourceInfo(eResourceDemanded)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopCity->getX(), pLoopCity->getY(), true, true*/);
								}
							}
						}
					}
				}
			}

			for(int iI = 0; iI < MAX_TEAMS; iI++)
			{
				const TeamTypes eLoopTeam = static_cast<TeamTypes>(iI);
				CvTeam& kLoopTeam = GET_TEAM(eLoopTeam);
				if(kLoopTeam.isAlive())
				{
					if(eLoopTeam != GetID())
					{
						kLoopTeam.updateTechShare(eIndex);
					}
				}
			}

			// Cities that are owed a culture building are granted a culture building
			int iLoop;

			// Check all players on this team
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(eLoopPlayer).getTeam() == GetID())
				{
					// Look at all Cities
					for (CvCity* pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
					{
						if (pLoopCity->IsOwedCultureBuilding())
						{
							BuildingTypes eFreeCultureBuilding = pLoopCity->ChooseFreeCultureBuilding();
							if (eFreeCultureBuilding != NO_BUILDING)
							{
								pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eFreeCultureBuilding, 1);
								pLoopCity->SetOwedCultureBuilding(false);
							}
						}

						if (pLoopCity->IsOwedFoodBuilding())
						{
							BuildingTypes eFreeFoodBuilding = pLoopCity->ChooseFreeFoodBuilding();
							if (eFreeFoodBuilding != NO_BUILDING)
							{
								pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eFreeFoodBuilding, 1);
								pLoopCity->SetOwedFoodBuilding(false);
							}
						}
					}
				}
			}
		}

		if(bNewValue)
		{
			if(bAnnounce)
			{
				if(GC.getGame().isFinalInitialized())
				{
					CvAssert(ePlayer != NO_PLAYER);
					if(GET_PLAYER(ePlayer).isHuman())
					{
						if(GET_PLAYER(ePlayer).GetPlayerTechs()->IsResearch() && (GET_PLAYER(ePlayer).GetPlayerTechs()->GetCurrentResearch() == NO_TECH))
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_WHAT_TO_RESEARCH_NEXT");
							GET_PLAYER(ePlayer).chooseTech(0, strBuffer, eIndex);
						}
					}
				}
			}

			// espionage logic. Whenever someone gains a tech, reset the stealable techs for all other leaders
			if(!GET_PLAYER(ePlayer).isMinorCiv() && !GET_PLAYER(ePlayer).isBarbarian())
			{
				CvPlayerEspionage* pEspionage = GET_PLAYER(ePlayer).GetEspionage();
				if(pEspionage)
				{
					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						pEspionage->BuildStealableTechList((PlayerTypes)ui);

						// if the player is out of techs to steal, set their number of stealable techs to zero
						if(pEspionage->m_aaPlayerStealableTechList[ui].size() == 0)
						{
							pEspionage->m_aiNumTechsToStealList[ui] = 0;
						}
					}
				}
			}
		}

		// Update Happiness for all players
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			if(GET_PLAYER((PlayerTypes)iPlayerLoop).isAlive() && GET_PLAYER((PlayerTypes) iPlayerLoop).getTeam() == GetID())
			{
				GET_PLAYER((PlayerTypes) iPlayerLoop).CalculateNetHappiness();
			}
		}

		if(GC.getGame().isFinalInitialized())
		{
			if(GetID() == GC.getGame().getActiveTeam())
			{
				DLLUI->setDirty(MiscButtons_DIRTY_BIT, true);
				DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
				DLLUI->setDirty(ResearchButtons_DIRTY_BIT, true);
			}

			if(eIndex != NO_TECH && bNewValue)
			{
				bool bDontShowRewardPopup = DLLUI->IsOptionNoRewardPopups();

				// Notification in MP games
				if(bDontShowRewardPopup || GC.getGame().isReallyNetworkMultiPlayer())
				{
					Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOU_DISCOVERED_TECH");
					localizedText << pkTechInfo->GetTextKey();
					AddNotification(NOTIFICATION_TECH_AWARD, localizedText.toUTF8(), localizedText.toUTF8(), -1, -1, 0, (int) eIndex);
				}
				// Popup in SP games
				else if(GetID() == GC.getGame().getActiveTeam())
				{
					CvPopupInfo kPopup(BUTTONPOPUP_TECH_AWARD, GC.getGame().getActivePlayer(), 0, eIndex);
					//kPopup.setText(localizedText.toUTF8());
					DLLUI->AddPopup(kPopup);
				}
			}
		}

		if(bNewValue)
		{
			gDLL->GameplayTechAcquired(GetID(), eIndex);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumFeatureInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiFeatureYieldChange[eIndex1][eIndex2];
}

//	--------------------------------------------------------------------------------
void CvTeam::changeFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumFeatureInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_ppaaiFeatureYieldChange[eIndex1][eIndex2] = (m_ppaaiFeatureYieldChange[eIndex1][eIndex2] + iChange);
		CvAssert(getFeatureYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumTerrainInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiTerrainYieldChange[eIndex1][eIndex2];
}

//	--------------------------------------------------------------------------------
void CvTeam::changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumTerrainInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_ppaaiTerrainYieldChange[eIndex1][eIndex2] = (m_ppaaiTerrainYieldChange[eIndex1][eIndex2] + iChange);
		CvAssert(getTerrainYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


//	--------------------------------------------------------------------------------
void CvTeam::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		CvAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementNoFreshWaterYieldChange[eIndex1][eIndex2];
}


//	--------------------------------------------------------------------------------
void CvTeam::changeImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_ppaaiImprovementNoFreshWaterYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementNoFreshWaterYieldChange[eIndex1][eIndex2] + iChange);
		CvAssert(getImprovementNoFreshWaterYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}


//	--------------------------------------------------------------------------------
int CvTeam::getImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementFreshWaterYieldChange[eIndex1][eIndex2];
}


//	--------------------------------------------------------------------------------
void CvTeam::changeImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_ppaaiImprovementFreshWaterYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementFreshWaterYieldChange[eIndex1][eIndex2] + iChange);
		CvAssert(getImprovementFreshWaterYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

// Protected Functions...

//	--------------------------------------------------------------------------------
void CvTeam::updateTechShare(TechTypes eTech)
{
	int iBestShare;
	int iCount;
	int iI;

	if(GetTeamTechs()->HasTech(eTech))
	{
		return;
	}

	iBestShare = numeric_limits<int>::max();

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(isTechShare(iI))
		{
			iBestShare = std::min(iBestShare, (iI + 1));
		}
	}

	if(iBestShare != INT_MAX)
	{
		iCount = 0;

		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM((TeamTypes)iI).GetTeamTechs()->HasTech(eTech))
				{
					if(isHasMet((TeamTypes)iI))
					{
						CvAssertMsg(iI != GetID(), "iI is not expected to be equal with GetID()");
						iCount++;
					}
				}
			}
		}

		if(iCount >= iBestShare)
		{
			setHasTech(eTech, true, NO_PLAYER, true, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::updateTechShare()
{
	int iI;

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		updateTechShare((TechTypes)iI);
	}
}

//	---------------------------------------------------------------------------
void CvTeam::testCircumnavigated()
{
	CvPlot* pPlot;
	CvString strBuffer;
	bool bFoundVisible;
	int iX, iY;

	if(isBarbarian())
	{
		return;
	}

	CvGame& kGame = GC.getGame();
	if(!kGame.circumnavigationAvailable())
	{
		return;
	}

	CvMap& kMap = GC.getMap();

	// Test one of the wrapping axises.  This is brute force and not exactly "correct".
	// It does not test for continuity and is not able to test whether the plot was revealed by exploration or
	// some other method such as gifted "maps".
	TeamTypes eTeamID = GetID();
	if(kMap.isWrapX())
	{
		for(iX = kMap.getGridWidth(); iX--;)
		{
			bFoundVisible = false;

			for(iY = kMap.getGridHeight(); iY--;)
			{
				pPlot = kMap.plotUnchecked(iX, iY);

				if(pPlot->isRevealed(eTeamID))
				{
					bFoundVisible = true;
					break;
				}
			}

			if(!bFoundVisible)
			{
				return;
			}
		}
	}
	else if(kMap.isWrapY())
	{
		for(iY = kMap.getGridHeight(); iY--;)
		{
			bFoundVisible = false;

			for(iX = kMap.getGridWidth(); iX--;)
			{
				pPlot = kMap.plotUnchecked(iX, iY);

				if(pPlot->isRevealed(GetID()))
				{
					bFoundVisible = true;
					break;
				}
			}

			if(!bFoundVisible)
			{
				return;
			}
		}
	}

	kGame.makeCircumnavigated();

	int iActivePlayerID = GC.getGame().getActivePlayer();

	if(kGame.getElapsedGameTurns() > 0)
	{
		int iFreeMoves = /*0*/ GD_INT_GET(CIRCUMNAVIGATE_FREE_MOVES);
		if (iFreeMoves != 0)
			changeExtraMoves(DOMAIN_SEA, iFreeMoves);

		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive())
			{
				if(eTeamID == kPlayer.getTeam())
				{
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
					GC.getGame().SetTeamThatCircumnavigated(eTeamID);
#endif

#if defined(MOD_API_ACHIEVEMENTS)
					if(!kGame.isGameMultiPlayer() && kPlayer.isHuman())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_ROUND_WORLD);
					}
#endif
				}

				if(iActivePlayerID == iI)
				{
					if(eTeamID == kPlayer.getTeam())
					{
						if(iFreeMoves == 0)
							// Not the most ideal solution, but if the free moves are 0, use the string designed for telling about another team circling the globe
							strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", kPlayer.getName());
						else
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CIRC_GLOBE", iFreeMoves);
					}
					else if(isHasMet(kPlayer.getTeam()))
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_CIRC_GLOBE");
					}
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);

#if defined(MOD_EVENTS_CIRCUMNAVIGATION)
					if (MOD_EVENTS_CIRCUMNAVIGATION) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_CircumnavigatedGlobe, eTeamID);
						
						// Notifications should now be sent via the event
						// CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CIRC_GLOBE");
						// AddNotification(NOTIFICATION_GENERIC, strBuffer, strSummary, -1, -1, -1);
					} else {
#endif
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem)
					{
						CvLuaArgsHandle args;
						args->Push(eTeamID);

						bool bResult = false;
						LuaSupport::CallHook(pkScriptSystem, "CircumnavigatedGlobe", args.get(), bResult);
					}
#if defined(MOD_EVENTS_CIRCUMNAVIGATION)
					}
#endif
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
	}
}

//	--------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
void CvTeam::processTech(TechTypes eTech, int iChange, bool bNoBonus)
#else
void CvTeam::processTech(TechTypes eTech, int iChange)
#endif
{
	CvCity* pCity;
	CvPlot* pLoopPlot;
	ResourceTypes eResource;
	int iI, iJ;

	CvTechEntry* pTech = GC.getTechInfo(eTech);

	if (pTech->IsAllowsWorldCongress())
	{
		SetHasTechForWorldCongress(true);
	}

	if(pTech->IsTriggersArchaeologicalSites())
	{
		GC.getGame().TriggerArchaeologySiteCreation(true /*bCheckInitialized*/);
	}

	if(pTech->IsExtraWaterSeeFrom())
	{
		changeExtraWaterSeeFromCount(iChange);
	}

	if(pTech->IsMapCentering())
	{
		if(iChange > 0)
		{
			setMapCentering(true);
		}
	}

	if(pTech->IsMapTrading())
	{
		changeMapTradingCount(iChange);
	}

	if(pTech->IsTechTrading())
	{
		changeTechTradingCount(iChange);
	}

	if(pTech->IsGoldTrading())
	{
		changeGoldTradingCount(iChange);
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && pTech->IsVassalageTradingAllowed())
	{
		changeVassalageTradingAllowedCount(iChange);
	}

#endif

#if defined(MOD_BALANCE_CORE)
	if (pTech->IsCorporationsEnabled())
	{
		changeCorporationsEnabledCount(iChange);
	}
#endif

	if(pTech->IsAllowEmbassyTradingAllowed())
	{
		changeAllowEmbassyTradingAllowedCount(iChange);
	}

	if(pTech->IsOpenBordersTradingAllowed())
	{
		changeOpenBordersTradingAllowedCount(iChange);
	}

	if(pTech->IsDefensivePactTradingAllowed())
	{
		changeDefensivePactTradingAllowedCount(iChange);
	}
	if(pTech->IsResearchAgreementTradingAllowed())
	{
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if(MOD_DIPLOMACY_CIV4_FEATURES)
		{
			if(GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS))
			{
				ChangeResearchAgreementTradingAllowedCount(iChange);
			}
		}
		else
		{
			ChangeResearchAgreementTradingAllowedCount(iChange);
		}
#endif
	}

#if defined(MOD_TECHS_CITY_WORKING)
	if(pTech->GetCityWorkingChange() != 0)
	{
		changeCityWorkingChange(pTech->GetCityWorkingChange() * iChange);
	}
#endif
	
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	if(pTech->GetCityAutomatonWorkersChange() != 0)
	{
		changeCityAutomatonWorkersChange(pTech->GetCityAutomatonWorkersChange() * iChange);
	}
#endif

	if(pTech->IsBridgeBuilding())
	{
		changeBridgeBuildingCount(iChange);
	}
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	if(pTech->IsCityLessEmbarkCost())
	{
		changeCityLessEmbarkCost(iChange);
	}
	if(pTech->IsCityNoEmbarkCost())
	{
		changeCityNoEmbarkCost(iChange);
	}
#endif

	if(pTech->IsWaterWork())
	{
		changeWaterWorkCount(iChange);
	}

	if(pTech->IsAllowsEmbarking())
	{
		changeCanEmbarkCount(iChange);
	}

	if(pTech->IsAllowsDefensiveEmbarking())
	{
		changeDefensiveEmbarkCount(iChange);
	}

	if(pTech->IsEmbarkedAllWaterPassage())
	{
		changeEmbarkedAllWaterPassage(iChange);
	}

	if(pTech->GetEmbarkedMoveChange() != 0)
	{
		changeEmbarkedExtraMoves(pTech->GetEmbarkedMoveChange() * iChange);
	}

	for(iI = 0; iI < GC.getNumRouteInfos(); iI++)
	{
		changeRouteChange(((RouteTypes)iI), (GC.getRouteInfo((RouteTypes) iI)->getTechMovementChange(eTech) * iChange));
	}
#if defined(MOD_BALANCE_CORE)
	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeTradeRouteDomainExtraRange((DomainTypes)iI, (pTech->GetTradeRouteDomainExtraRange((DomainTypes)iI) * iChange));
	}
#endif

	for (int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)i);
		if (pkBuildInfo != NULL)
			changeBuildTimeChange(((BuildTypes)i), (pkBuildInfo->getTechTimeChange(eTech) * iChange));
	}

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeExtraMoves(((DomainTypes)iI), (pTech->GetDomainExtraMoves(iI) * iChange));
	}

	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo((BuildingTypes) iI);
		if(pBuildingEntry)
		{
			if(pBuildingEntry->GetObsoleteTech() == eTech)
			{
				changeObsoleteBuildingCount(((BuildingTypes)iI), iChange);
			}
			if(pBuildingEntry->GetEnhancedYieldTech() == eTech)
			{
				enhanceBuilding(((BuildingTypes)iI), iChange);
			}

			if (pBuildingEntry->GetPrereqAndTech() == eTech && isWorldWonderClass(pBuildingEntry->GetBuildingClassInfo()))
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iJ);
					if (kPlayer.getTeam() == GetID() && kPlayer.isAlive())
					{
						// Reset city specialization
						kPlayer.GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL);
					}
				}
			}
		}
	}

	for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		CvFeatureInfo* pFeatureEntry = GC.getFeatureInfo((FeatureTypes)iI);
		if(pFeatureEntry)
		{
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeFeatureYieldChange(((FeatureTypes)iI), ((YieldTypes)iJ), (pFeatureEntry->GetTechYieldChanges(eTech, iJ) * iChange));
			}
		}
	}

	for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		CvTerrainInfo* pTerrainEntry = GC.getTerrainInfo((TerrainTypes)iI);
		if(pTerrainEntry)
		{
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeTerrainYieldChange(((TerrainTypes)iI), ((YieldTypes)iJ), (pTerrainEntry->GetTechYieldChanges(eTech, iJ) * iChange));
			}
		}
	}

	for(iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		CvImprovementEntry* pImprovementEntry = GC.getImprovementInfo((ImprovementTypes)iI);
		if(pImprovementEntry)
		{
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (pImprovementEntry->GetTechYieldChanges(eTech, iJ) * iChange));
				changeImprovementNoFreshWaterYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (pImprovementEntry->GetTechNoFreshWaterYieldChanges(eTech, iJ) * iChange));
				changeImprovementFreshWaterYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (pImprovementEntry->GetTechFreshWaterYieldChanges(eTech, iJ) * iChange));
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.getTeam() == GetID() && kPlayer.isEverAlive())
		{
			for(int iL = 0; iL < GC.getNumSpecialistInfos(); iL++)
			{
				CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo((SpecialistTypes)iL);
				if(pSpecialistInfo)
				{
					for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						kPlayer.changeSpecialistExtraYield(((SpecialistTypes)iL), ((YieldTypes)iJ), (pTech->GetTechYieldChanges((SpecialistTypes)iL, (YieldTypes)iJ) * iChange));
					}
				}
			}
		}
	}
#endif
	CvPlot* pNewUnitPlot;
	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.getTeam() == GetID() && kPlayer.isEverAlive())
		{
			kPlayer.changeFeatureProductionModifier(pTech->GetFeatureProductionModifier() * iChange);
			kPlayer.changeUnitFortificationModifier(pTech->GetUnitFortificationModifier() * iChange);
			kPlayer.changeUnitBaseHealModifier(pTech->GetUnitBaseHealModifier() * iChange);
			kPlayer.changeWorkerSpeedModifier(pTech->GetWorkerSpeedModifier() * iChange);
			kPlayer.ChangeInfluenceSpreadModifier(pTech->GetInfluenceSpreadModifier() * iChange);
			kPlayer.ChangeExtraVotesPerDiplomat(pTech->GetExtraVotesPerDiplomat() * iChange);

			if (kPlayer.isMajorCiv())
			{
				for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
				{
					PlayerTypes eMinor = (PlayerTypes) iMinorLoop;

					if (eMinor != NO_PLAYER && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv() && GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() == (PlayerTypes)iI)
					{
						GET_PLAYER(eMinor).GetMinorCivAI()->DoUpdateAlliesResourceBonus((PlayerTypes)iI, (PlayerTypes)iI);
					}
				}
			}

			// Free promotion from this tech?
			for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
			{
				PromotionTypes ePromotion = (PromotionTypes) iPromotion;
#if defined(MOD_BALANCE_CORE)
				if(pTech->IsFreePromotion(ePromotion))
				{
					kPlayer.ChangeFreePromotionCount(ePromotion, iChange);

					//For civilians
					int iLoop;
					CvUnit* pLoopUnit;
					// Loop through existing units, because they have no way to earn it later
					for(int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
						{
							for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
							{
								// If we're in friendly territory and we can embark, give the promotion for free
								if(pLoopUnit->plot()->IsFriendlyTerritory((PlayerTypes)iI))
								{
									// Civilian unit or the unit can acquire this promotion
									if(IsPromotionValidForUnitCombatType(ePromotion, pLoopUnit->getUnitType()) || IsPromotionValidForCivilianUnitType(ePromotion, pLoopUnit->getUnitType()))
									{
										pLoopUnit->setHasPromotion(ePromotion, true);
									}
								}
							}
						}
					}
				}
#else
				if(pTech->IsFreePromotion(ePromotion))
					kPlayer.ChangeFreePromotionCount(ePromotion, iChange);
#endif
			}

			// Update our traits (some may have become obsolete)
			kPlayer.GetPlayerTraits()->Reset();
			kPlayer.GetPlayerTraits()->InitPlayerTraits();
			kPlayer.recomputePolicyCostModifier();

			// Does our trait give us a new unit when we reach this tech?
			UnitTypes eLoopUnit;
			int iDefaultAI;
			int iUnitClass = kPlayer.GetPlayerTraits()->GetFirstFreeUnit(eTech);
			while(iUnitClass != NO_UNITCLASS)
			{
				eLoopUnit = kPlayer.GetSpecificUnitType((UnitClassTypes)iUnitClass);
				iDefaultAI = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetDefaultUnitAIType();
				pNewUnitPlot = kPlayer.addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);

				// Notification below only tells user if their civ gets a Great Person from their trait.  But trait code allows ANY unit to be received from reaching a tech.
				// So in the future we should use a different notification ("you received a new unit from your trait since you reached this tech") instead.
				SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
				if(pNewUnitPlot && kPlayer.GetNotifications() && GC.GetGameUnits()->GetEntry(eLoopUnit)->GetSpecialUnitType() == eSpecialUnitGreatPerson)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
					kPlayer.GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), pNewUnitPlot->getX(), pNewUnitPlot->getY(), eLoopUnit);
				}

				// Another?
				iUnitClass = kPlayer.GetPlayerTraits()->GetNextFreeUnit();
			}
#if defined(MOD_BALANCE_CORE)
			int iLoop;
			CvUnit* pLoopUnit;
			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if(pLoopUnit->isFreeUpgrade() || kPlayer.GetPlayerTraits()->IsFreeUpgrade())
				{
					UnitTypes eUpgradeUnit = pLoopUnit->GetUpgradeUnitType();
					if(eUpgradeUnit != NO_UNIT && kPlayer.canTrainUnit(eUpgradeUnit, false, false, true))
					{
						pLoopUnit->DoUpgrade(true);
					}
				}
				if(kPlayer.GetPlayerTraits()->IsFreeZuluPikemanToImpi())
				{
					UnitClassTypes ePikemanClass = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_PIKEMAN");
					UnitTypes eZuluImpi = (UnitTypes)GC.getInfoTypeForString("UNIT_ZULU_IMPI");
					if(pLoopUnit != NULL && pLoopUnit->getUnitClassType() == ePikemanClass && kPlayer.canTrainUnit(eZuluImpi, false, false, true))
					{
						CvUnitEntry* pkcUnitEntry = GC.getUnitInfo(eZuluImpi);
						if(pkcUnitEntry)
						{
							UnitAITypes eZuluImpiAI = pkcUnitEntry->GetDefaultUnitAIType();
							CvUnit* pZuluImpi = kPlayer.initUnit(eZuluImpi, pLoopUnit->getX(), pLoopUnit->getY(), eZuluImpiAI);
							pZuluImpi->convert(pLoopUnit, true);
						}
					}
				}
			}
			if(kPlayer.getCapitalCity() != NULL)
			{
				//Free Happiness
				if (pTech->GetHappiness() != 0)
				{
					int iLoop;
					CvCity* pLoopCity;
					for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
					{
						pLoopCity->ChangeUnmoddedHappinessFromBuildings(pTech->GetHappiness());
					}
				}

				//Free building in capital unlocked via tech?
				if(kPlayer.GetPlayerTraits()->GetCapitalFreeBuildingPrereqTech() == eTech)
				{
					BuildingTypes eFreeCapitalBuilding = kPlayer.GetPlayerTraits()->GetFreeCapitalBuilding();
					if(eFreeCapitalBuilding != NO_BUILDING)
					{
						if(kPlayer.getCapitalCity()->GetCityBuildings()->GetNumRealBuilding(eFreeCapitalBuilding) > 0)
						{
							kPlayer.getCapitalCity()->GetCityBuildings()->SetNumRealBuilding(eFreeCapitalBuilding, 0);
							int iProductionValue = kPlayer.getCapitalCity()->getProductionNeeded(eFreeCapitalBuilding);
							kPlayer.doInstantYield(INSTANT_YIELD_TYPE_REFUND, false, NO_GREATPERSON, NO_BUILDING, iProductionValue, false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity());
						}
						kPlayer.getCapitalCity()->GetCityBuildings()->SetNumFreeBuilding(eFreeCapitalBuilding, 1);
					}
				}
			}
#endif
#if defined(MOD_BALANCE_CORE)
			// Free buildings (once unlocked via tech)
			CvCity* pLoopCity;
			const CvCivilizationInfo& thisCiv = kPlayer.getCivilizationInfo();
			if(kPlayer.GetPlayerTraits()->GetFreeBuildingPrereqTech() == eTech)
			{
				for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
				{
					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
					if(!pkBuildingClassInfo)
					{
						continue;
					}
					
					if (kPlayer.GetNumCitiesFreeChosenBuilding((BuildingClassTypes)iI) > 0 || kPlayer.IsFreeChosenBuildingNewCity((BuildingClassTypes)iI) || kPlayer.IsFreeBuildingAllCity((BuildingClassTypes)iI))
					{
						BuildingTypes eBuilding = ((BuildingTypes)(thisCiv.getCivilizationBuildings((BuildingClassTypes)iI)));

						if(eBuilding != NO_BUILDING)
						{
							CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
							if(pkBuildingInfo)
							{
								int iLoop;
								for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
								{
									bool bHasBuildingClass = pLoopCity->HasBuildingClass((BuildingClassTypes)iI);
									BuildingTypes eReplacedBuilding = eBuilding;
									if (MOD_BUILDINGS_THOROUGH_PREREQUISITES || kPlayer.GetPlayerTraits()->IsKeepConqueredBuildings())
									{
										if (bHasBuildingClass)
										{
											eReplacedBuilding = pLoopCity->GetCityBuildings()->GetBuildingTypeFromClass((BuildingClassTypes)iI);
										}
									}
									if(pLoopCity->isValidBuildingLocation(eBuilding) || (eBuilding != eReplacedBuilding && pLoopCity->isValidBuildingLocation(eReplacedBuilding)))
									{
										if (kPlayer.GetNumCitiesFreeChosenBuilding((BuildingClassTypes)iI) > 0 || kPlayer.IsFreeChosenBuildingNewCity((BuildingClassTypes)iI) || kPlayer.IsFreeBuildingAllCity((BuildingClassTypes)iI))
										{
											if (eBuilding != eReplacedBuilding)
											{
												if (pLoopCity->GetCityBuildings()->GetNumRealBuilding(eReplacedBuilding) > 0)
												{
													pLoopCity->GetCityBuildings()->SetNumRealBuilding(eReplacedBuilding, 0);
													int iProductionValue = pLoopCity->getProductionNeeded(eReplacedBuilding);
													kPlayer.doInstantYield(INSTANT_YIELD_TYPE_REFUND, false, NO_GREATPERSON, NO_BUILDING, iProductionValue, false, NO_PLAYER, NULL, false, pLoopCity);
												}
												if (pLoopCity->GetCityBuildings()->GetNumFreeBuilding(eReplacedBuilding) <= 0)
												{
													pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eReplacedBuilding, 1);
												}
												if (pLoopCity->GetCityBuildings()->GetNumFreeBuilding(eReplacedBuilding) > 0)
												{
													kPlayer.ChangeNumCitiesFreeChosenBuilding((BuildingClassTypes)iI, -1);
												}
												if (pLoopCity->getFirstBuildingOrder(eReplacedBuilding) == 0)
												{
													pLoopCity->clearOrderQueue();
													pLoopCity->chooseProduction();
													// Send a notification to the user that what they were building was given to them, and they need to produce something else.
												}
											}
											else
											{
												if (pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
												{
													pLoopCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
												}
												if (pLoopCity->GetCityBuildings()->GetNumFreeBuilding(eBuilding) <= 0)
												{
													pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eBuilding, 1);
												}
												if (pLoopCity->GetCityBuildings()->GetNumFreeBuilding(eBuilding) > 0)
												{
													kPlayer.ChangeNumCitiesFreeChosenBuilding((BuildingClassTypes)iI, -1);
												}
												if (pLoopCity->getFirstBuildingOrder(eBuilding) == 0)
												{
													pLoopCity->clearOrderQueue();
													pLoopCity->chooseProduction();
													// Send a notification to the user that what they were building was given to them, and they need to produce something else.
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
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
			if(!bNoBonus)
			{
				// call one for era scaling, another for no era scaliing
				kPlayer.doInstantYield(INSTANT_YIELD_TYPE_TECH, false, NO_GREATPERSON, NO_BUILDING, 0, true);
				kPlayer.doInstantYield(INSTANT_YIELD_TYPE_TECH, false, NO_GREATPERSON, NO_BUILDING, 0, false);
			}
#endif
#if defined(MOD_BALANCE_CORE)
			int iLoop2;
			for(CvCity* pLoopCity2 = kPlayer.firstCity(&iLoop2); pLoopCity2 != NULL; pLoopCity2 = kPlayer.nextCity(&iLoop2))
				pLoopCity2->UpdateAllNonPlotYields(false);
#endif
		}
	}

	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		eResource = pLoopPlot->getResourceType();

		if(eResource != NO_RESOURCE)
		{
			TechTypes eDefaultTech = (TechTypes)GC.getResourceInfo(eResource)->getTechReveal();
			PolicyTypes eRevealPolicy = (PolicyTypes)GC.getResourceInfo(eResource)->getPolicyReveal();
			bool bRevealTech = false;
			bool bRevealed = false;

			for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
			{
				const PlayerTypes ePlayer = (PlayerTypes)*iI;
				CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

				if (pPlayer && pPlayer->isAlive())
				{
					TechTypes eRevealTech = eDefaultTech;
					
#if defined(MOD_BALANCE_CORE)
					if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
					{
						TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechReveal;
						if (eAltTech != NO_TECH)
						{
							eRevealTech = eAltTech;
						}
					}
#endif
					if ((eRevealPolicy == NO_POLICY || (eRevealPolicy != NO_POLICY && pPlayer->HasPolicy(eRevealPolicy))) && (eRevealTech == NO_TECH || (eRevealTech != eTech && GetTeamTechs()->HasTech(eRevealTech))))
					{
						bRevealed = true;
						break;
					}
					else if (!bRevealTech && eRevealTech == eTech)
					{
						bRevealTech = true;
					}
				}
			}

			if(bRevealTech && !bRevealed)
			{
				pLoopPlot->updateYield();
				if(pLoopPlot->isRevealed(m_eID))
				{
					pLoopPlot->setLayoutDirty(true);
				}
			}
		}
	}

	//important - otherwise the update below will not change anything
	DoUpdateBestRoute();

	for(iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		CvBuildInfo* pBuildInfo = GC.getBuildInfo((BuildTypes) iI);
		if(pBuildInfo && pBuildInfo->getTechPrereq() == eTech)
		{
			if(pBuildInfo->getRoute() != NO_ROUTE)
			{
				for(iJ = 0; iJ < GC.getMap().numPlots(); iJ++)
				{
					pLoopPlot = GC.getMap().plotByIndexUnchecked(iJ);

					pCity = pLoopPlot->getPlotCity();

					if(pCity != NULL)
					{
						if(pCity->getTeam() == GetID())
						{
							pLoopPlot->updateCityRoute();
						}
					}
				}
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args(3);
		args->Push(GetID());
		args->Push(eTech);
		args->Push(iChange);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "TeamTechResearched", args.get(), bResult);
	}

	// End game!
	if(pTech->IsEndsGame())
	{
		GC.getGame().SetEndGameTechResearched(true);
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::cancelDefensivePacts()
{
	int iI;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		TeamTypes eTeam = (TeamTypes)iI;
		CvTeam& kTeam = GET_TEAM(eTeam);

		if(eTeam != GetID())
		{
			if(kTeam.isAlive())
			{
				if (IsHasDefensivePact(eTeam))
				{
					if (!MOD_BALANCE_DEFENSIVE_PACTS_AGGRESSION_ONLY)
					{
						SetHasDefensivePact(eTeam, false);
						kTeam.SetHasDefensivePact(GetID(), false);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::isAtWarWithHumans() const
{//are we currently fighting a war with a human controlled civ?
	for(int oppTeamIdx = 0; oppTeamIdx < MAX_TEAMS; ++oppTeamIdx)
	{
		CvTeam& oppTeam = GET_TEAM((TeamTypes)oppTeamIdx);
		if(oppTeam.isAlive() && isAtWar((TeamTypes)oppTeamIdx) && oppTeam.isHuman())
		{//we're at war with a human team
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isSimultaneousTurns() const
{//determine if this team is running simultaneous turns or not.
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			//just use the status of the first player we find for this team.
			return kPlayer.isSimultaneousTurns();
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvTeam::setForceRevealedResource(ResourceTypes eResource, bool bRevealed)
{
	if(isForceRevealedResource(eResource) == bRevealed)
	{
		return;
	}

	if(bRevealed)
	{
		m_aeRevealedResources.push_back(eResource);
	}
	else
	{
		std::vector<ResourceTypes>::iterator it;

		for(it = m_aeRevealedResources.begin(); it != m_aeRevealedResources.end(); ++it)
		{
			if(*it == eResource)
			{
				m_aeRevealedResources.erase(it);
				break;
			}
		}
	}

	for(int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pLoopPlot->getResourceType() == eResource)
		{
			pLoopPlot->updateYield();
			pLoopPlot->setLayoutDirty(true);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::isForceRevealedResource(ResourceTypes eResource) const
{
	std::vector<ResourceTypes>::const_iterator it;

	for(it = m_aeRevealedResources.begin(); it != m_aeRevealedResources.end(); ++it)
	{
		if(*it == eResource)
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Can we use this resource?
bool CvTeam::IsResourceObsolete(ResourceTypes eResource)
{
	CvResourceInfo* pResource = GC.getResourceInfo(eResource);

	if(pResource == NULL)
		return false;

	// If this is a luxury or bonus resource it doesn't go obsolete
	if(pResource->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
		return false;

	EraTypes eObsoleteEra = (EraTypes) pResource->getAIStopTradingEra();

	// AI will always trade for this
	if(eObsoleteEra == NO_ERA)
		return false;

	// Not obsolete yet
	if(GetCurrentEra() < eObsoleteEra)
		return false;

	return true;
}

//	--------------------------------------------------------------------------------
/// Is this resource city tradeable for us?
bool CvTeam::IsResourceCityTradeable(ResourceTypes eResource) const
{
	CvResourceInfo* pResource = GC.getResourceInfo(eResource);
	if (pResource)
	{
		TechTypes eDefaultTech = (TechTypes)pResource->getTechCityTrade();
		for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
		{
			const PlayerTypes ePlayer = (PlayerTypes)*iI;
			CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

			if (pPlayer && pPlayer->isAlive())
			{
				TechTypes eTech = eDefaultTech;
#if defined(MOD_BALANCE_CORE)
				if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
				{
					TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechCityTrade;
					if (eAltTech != NO_TECH)
					{
						eTech = eAltTech;
					}
				}
#endif

				if (eTech == NO_TECH || GetTeamTechs()->HasTech(eTech))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Is this resource revealed for us?
bool CvTeam::IsResourceRevealed(ResourceTypes eResource) const
{
	CvResourceInfo* pResource = GC.getResourceInfo(eResource);
	if (pResource)
	{
		TechTypes eDefaultTech = (TechTypes)pResource->getTechReveal();
		PolicyTypes ePolicy = (PolicyTypes)pResource->getPolicyReveal();
		for (std::vector<PlayerTypes>::const_iterator iI = m_members.begin(); iI != m_members.end(); ++iI)
		{
			const PlayerTypes ePlayer = (PlayerTypes)*iI;
			CvPlayer* pPlayer = &GET_PLAYER(ePlayer);

			if (pPlayer && pPlayer->isAlive())
			{
				TechTypes eTech = eDefaultTech;
#if defined(MOD_BALANCE_CORE)
				if (pPlayer->GetPlayerTraits()->IsAlternateResourceTechs())
				{
					TechTypes eAltTech = pPlayer->GetPlayerTraits()->GetAlternateResourceTechs(eResource).m_eTechReveal;
					if (eAltTech != NO_TECH)
					{
						eTech = eAltTech;
					}
				}
#endif

				if ((eTech == NO_TECH || GetTeamTechs()->HasTech(eTech)) && (ePolicy == NO_POLICY || pPlayer->HasPolicy(ePolicy)))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Do we have the technology for founding a World Congress?
bool CvTeam::HasTechForWorldCongress() const
{
	return m_bHasTechForWorldCongress;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetHasTechForWorldCongress(bool bValue)
{
	m_bHasTechForWorldCongress = bValue;
}

//	--------------------------------------------------------------------------------
/// What Era are we in?
EraTypes CvTeam::GetCurrentEra() const
{
	return m_eCurrentEra;
}

//	--------------------------------------------------------------------------------
/// Sets what Era we're in
void CvTeam::SetCurrentEra(EraTypes eNewValue)
{
	CvPlot* pLoopPlot;
	int iI;

	if(GetCurrentEra() != eNewValue)
	{
#if defined(MOD_EVENTS_NEW_ERA)
		// check to see if anyone else has reached or surpassed this era yet
		bool bAlreadyProvided = false;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
			if (kPlayer.isEverAlive() && !kPlayer.isBarbarian() && !kPlayer.isMinorCiv())
			{
				if(GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eNewValue)
				{
					bAlreadyProvided = true;
					break;
				}
			}
		}
#endif
#if defined(MOD_BALANCE_CORE_JFD)
		if(MOD_BALANCE_CORE_JFD && isHuman())
		{
			GC.getGame().GetGameContracts()->DoUpdateContracts();
		}
#endif

		if(!isMinorCiv())
		{
			if(GC.getGame().isFinalInitialized())
			{
				bool bMinorBonusesChanged;
				bool bTemp;
				Localization::String strMessage;
				Localization::String strSummary;

				
				if(!isBarbarian() && (eNewValue != GC.getGame().getStartEra())){
					//Era Popup
					if (!GC.getGame().isReallyNetworkMultiPlayer() && isHuman() && GetID() == GC.getGame().getActiveTeam())
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_NEW_ERA, eNewValue);
						DLLUI->AddPopup(kPopupInfo);
					}

					for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
					{
						CvFeatureInfo* pFeatureEntry = GC.getFeatureInfo((FeatureTypes)iI);
						if(pFeatureEntry)
						{
							for(int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								int iYield = pFeatureEntry->GetEraYieldChanges(iJ);
								if(iYield > 0)
								{
									changeFeatureYieldChange(((FeatureTypes)iI), ((YieldTypes)iJ), iYield);
								}
							}
						}
					}

					//Notify Everyone
					for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
						PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
						CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
						CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
						if(pNotifications && 
							(kCurNotifyPlayer.getTeam() != GetID() || GC.getGame().isNetworkMultiPlayer()) &&
							kCurNotifyPlayer.isAlive()){
							strMessage = Localization::Lookup("TXT_KEY_NTFN_PLAYER_ERA");

							CvEraInfo* pkEraInfo = GC.getEraInfo(eNewValue);
							const char* szEraTextKey = pkEraInfo->GetTextKey();

							// Notify player has met this team
							if(GET_TEAM(kCurNotifyPlayer.getTeam()).isHasMet(GetID()))
							{
								CvPlayerAI& player = GET_PLAYER(getLeaderID());
								if(GC.getGame().isGameMultiPlayer() && player.isHuman())
									strMessage << player.getNickName() << szEraTextKey;
								else
									strMessage << player.getName() << szEraTextKey;
							}

							// Has not met this team
							else
							{
								Localization::String unmetPlayer = Localization::Lookup("TXT_KEY_UNMET_PLAYER");
								strMessage << unmetPlayer << szEraTextKey;
							}

							pNotifications->Add(NOTIFICATION_OTHER_PLAYER_NEW_ERA, strMessage.toUTF8(), strMessage.toUTF8(), -1, -1, -1);
						}
					}
				}

				// Apply Minor Civ changes BEFORE setting the new era
				PlayerTypes eLoopMinor;
				int iMinorLoop;

				PlayerTypes eLoopPlayer;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;

					// Must be on this team
					if(GET_PLAYER(eLoopPlayer).getTeam() != GetID())
						continue;

					// Player not ever alive
					if(!GET_PLAYER(eLoopPlayer).isEverAlive())
						continue;

					bMinorBonusesChanged = false;

					for(iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
					{
						eLoopMinor = (PlayerTypes) iMinorLoop;

						// Minor not alive
						if(!GET_PLAYER(eLoopMinor).isAlive())
							continue;

						bTemp = GET_PLAYER(eLoopMinor).GetMinorCivAI()->DoMajorCivEraChange(eLoopPlayer, eNewValue);

						if(bTemp)
							bMinorBonusesChanged = true;
					}

					if(bMinorBonusesChanged)
					{
						if(GET_PLAYER(eLoopPlayer).GetNotifications())
						{
							strMessage = Localization::Lookup("TXT_KEY_NTFN_MINOR_BONUSES_CHANGED");
							strSummary = Localization::Lookup("TXT_KEY_NTFN_MINOR_BONUSES_CHANGED_SM");
							GET_PLAYER(eLoopPlayer).GetNotifications()->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
				}
			}

			CvEraInfo* pEraInfo = GC.getEraInfo(eNewValue);
			if(pEraInfo->getSpiesGrantedForPlayer() > 0)
			{
				PlayerTypes ePlayer;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					ePlayer = (PlayerTypes) iPlayerLoop;
					CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
					if(kPlayer.isAlive() && kPlayer.getTeam() == GetID() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
					{
						// provide the player with the extra spies according to their trait when they get their first spy
#if defined(MOD_BALANCE_CORE)
						if (kPlayer.GetEspionage()->GetNumSpies() <= kPlayer.GetFreeSpy())
#else
						if (kPlayer.GetEspionage()->GetNumSpies() == 0)
#endif
						{
							int iNumTraitSpies = kPlayer.GetPlayerTraits()->GetExtraSpies();
#if defined(MOD_BALANCE_CORE_SPIES)
							if(MOD_BALANCE_CORE_SPIES && iNumTraitSpies > 0)
							{
								//Optional: Additional Trait Spies scaled for the number of City-States in the game.
								int iNumMinor = ((GC.getGame().GetNumMinorCivsEver(true) * /*10*/ GD_INT_GET(BALANCE_SPY_TO_MINOR_RATIO)) / 100);
								if((iNumMinor - 1) > 0)
								{
									iNumMinor = iNumMinor - 1;
									for(int i = 0; i < iNumMinor; i++)
									{
										kPlayer.GetEspionage()->CreateSpy();
									}
								}			
							}
#endif
							for (int i = 0; i < iNumTraitSpies; i++)
							{
								kPlayer.GetEspionage()->CreateSpy();
							}
						}

						for(int i = 0; i < pEraInfo->getSpiesGrantedForPlayer(); i++)
						{
							kPlayer.GetEspionage()->CreateSpy();
						}
#if defined(MOD_BALANCE_CORE_SPIES)
						if(MOD_BALANCE_CORE_SPIES){
							//Optional: Spies scaled for the number of City-States in the game.
							int iNumMinor = ((GC.getGame().GetNumMinorCivsEver(true) * /*10*/ GD_INT_GET(BALANCE_SPY_TO_MINOR_RATIO)) / 100);
							if((iNumMinor - 1) > 0)
							{
								iNumMinor = iNumMinor - 1;
								for(int i = 0; i < iNumMinor; i++)
								{
									kPlayer.GetEspionage()->CreateSpy();
								}
							}			
						}
#endif
					}
				}
			}

			if(pEraInfo->getSpiesGrantedForEveryone() > 0)
			{
#if !defined(MOD_EVENTS_NEW_ERA)
				// check to see if anyone else has reached or surpassed this era yet
				bool bAlreadyProvided = false;
#endif
				PlayerTypes ePlayer;
#if !defined(MOD_EVENTS_NEW_ERA)
				TeamTypes eTeam;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					ePlayer = (PlayerTypes) iPlayerLoop;
					CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
					if (kPlayer.isEverAlive() && !kPlayer.isBarbarian() && !kPlayer.isMinorCiv())
					{
						eTeam = kPlayer.getTeam();

						if(GET_TEAM(eTeam).GetCurrentEra() >= eNewValue)
						{
							bAlreadyProvided = true;
							break;
						}
					}
				}
#endif

				if(!bAlreadyProvided)
				{
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;
						CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
						if(!kPlayer.isAlive() || kPlayer.isBarbarian() || kPlayer.isMinorCiv())
						{
							continue;
						}

						for(int i = 0; i < pEraInfo->getSpiesGrantedForEveryone(); i++)
						{
							// provide the player with the extra spies according to their trait when they get their first spy
#if defined(MOD_BALANCE_CORE)
							if (kPlayer.GetEspionage()->GetNumSpies() <= kPlayer.GetFreeSpy())
#else
							if (kPlayer.GetEspionage()->GetNumSpies() == 0)
#endif
							{
								int iNumTraitSpies = kPlayer.GetPlayerTraits()->GetExtraSpies();
								for (int iIndex = 0; iIndex < iNumTraitSpies; iIndex++)
								{
									kPlayer.GetEspionage()->CreateSpy();
								}
							}
							kPlayer.GetEspionage()->CreateSpy();
#if defined(MOD_BALANCE_CORE_SPIES)
							if(MOD_BALANCE_CORE_SPIES)
							{
								//Optional: Spies scaled for the number of City-States in the game.
								int iNumMinor = ((GC.getGame().GetNumMinorCivsEver(true) * /*10*/ GD_INT_GET(BALANCE_SPY_TO_MINOR_RATIO)) / 100);
								if((iNumMinor - 1) > 0)
								{
									iNumMinor = iNumMinor - 1;
									for(int i = 0; i < iNumMinor; i++)
									{
										kPlayer.GetEspionage()->CreateSpy();
									}
								}			
							}
#endif

						}
					}
				}
			}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			if(MOD_DIPLOMACY_CIV4_FEATURES && pEraInfo->getVassalageEnabled())
			{
				if(!GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
				{
					changeVassalageTradingAllowedCount(1);

					// Send notification to team
					PlayerTypes ePlayer;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;
						if(GET_PLAYER(ePlayer).getTeam() != GetID())
						{
							continue;
						}

						if(GET_PLAYER(ePlayer).GetID() == GC.getGame().getActivePlayer())
						{
							CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
							if(pNotifications)
							{
								CvEraInfo* pkEraInfo = GC.getEraInfo(eNewValue) ;
								const char* szEra = pkEraInfo->GetTextKey();
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_VASSALAGE_ALLOWED", szEra);
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_VASSALAGE_ALLOWED");
								pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, -1, -1, 0);
							}
						}
					}
				}
			}
#endif
		}
		// Trait to provide free policies on era change?
		PlayerTypes ePlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == GetID() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
			{
				int iNumFreePolicies = kPlayer.GetPlayerTraits()->GetFreeSocialPoliciesPerEra() > 0;
				if (iNumFreePolicies > 0)
				{
					if (kPlayer.GetPlayerTraits()->IsOddEraScaler())
					{
						if (((eNewValue % 2) != 0))
						{
							if ((GC.getLogging() && GC.getAILogging()))
							{
								CvString strLogString;
								strLogString.Format("CBP - Poland got their odd scaler in era %d", (int)eNewValue);
								kPlayer.GetHomelandAI()->LogHomelandMessage(strLogString);
							}

							kPlayer.ChangeNumFreePolicies(iNumFreePolicies);
						}
					}
					else
					{
						kPlayer.ChangeNumFreePolicies(iNumFreePolicies);
					}
				}
			}
		}

		m_eCurrentEra = eNewValue;

		if(GC.getGame().getActiveTeam() != NO_TEAM)
		{
			for(iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

				if(pLoopPlot->getRevealedImprovementType(GC.getGame().getActiveTeam(), true) != NO_IMPROVEMENT)
				{
					if((pLoopPlot->getTeam() == GetID()) ||
					        (!pLoopPlot->isOwned() && GetID() == GC.getGame().getActiveTeam()))
					{
						pLoopPlot->setLayoutDirty(true);
					}
				}
			}
		}

		// need to map gameplay eras to the artwork eras
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;

			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
			{
				gDLL->GameplayEraChanged(ePlayer, eNewValue);
			}
		}

		if(GC.getGame().getActiveTeam() == GetID() && isBridgeBuilding() && eNewValue >= GD_INT_GET(LAST_BRIDGE_ART_ERA))
		{
			gDLL->GameplayBridgeChanged(true, 1);
		}

		if(GetID() == GC.getGame().getActiveTeam())
		{
			DLLUI->setDirty(Soundtrack_DIRTY_BIT, true);
		}
#if defined(MOD_BALANCE_CORE)
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
			{
				int iLoop;
				for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{
					if(pLoopUnit->isUnitEraUpgrade())
					{
						if((GC.getUnitInfo(pLoopUnit->getUnitType())->GetEraCombatStrength(eNewValue) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eNewValue))
						{
							pLoopUnit->SetBaseCombatStrength(GC.getUnitInfo(pLoopUnit->getUnitType())->GetEraCombatStrength(eNewValue));
						}
						UnitCombatTypes eUnitCombatClass;
						for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
						{
							eUnitCombatClass = (UnitCombatTypes) iI;
							if((GC.getUnitInfo(pLoopUnit->getUnitType())->GetUnitNewEraCombatType(eUnitCombatClass, eNewValue) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eNewValue))
							{
								pLoopUnit->setUnitCombatType(eUnitCombatClass);
							}
						}
						PromotionTypes ePromotion;
						for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
						{
							ePromotion = (PromotionTypes) iI;
							if((GC.getUnitInfo(pLoopUnit->getUnitType())->GetUnitNewEraPromotions(ePromotion, eNewValue) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eNewValue))
							{
								pLoopUnit->setHasPromotion(ePromotion, true);
							}
						}
					}
				}

				//specialist food consumption changed, set all cities dirty
				for(CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
					pLoopCity->GetCityCitizens()->SetDirty(true);
			}
		}
#endif
#if defined(MOD_BALANCE_CORE)
		updateYield();
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if (ePlayer != NO_PLAYER && kPlayer.isMinorCiv() && kPlayer.isAlive())
			{
				for (int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
				{
					PlayerTypes eActivePlayer = (PlayerTypes)iPlayerLoop2;
					CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);
					if (eActivePlayer != NO_PLAYER && kActivePlayer.isAlive() && kActivePlayer.isMajorCiv() && kActivePlayer.getTeam() == GetID())
					{
						kPlayer.GetMinorCivAI()->RecalculateRewards(eActivePlayer);
					}
				}
			}
		}
#endif
#if defined(MOD_EVENTS_NEW_ERA)
		if (MOD_EVENTS_NEW_ERA && GetCurrentEra() != GC.getGame().getStartEra()) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TeamSetEra, GetID(), GetCurrentEra(), ((GetID() < MAX_MAJOR_CIVS) && !bAlreadyProvided));
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(GetID());
			args->Push(GetCurrentEra());
			
			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "TeamSetEra", args.get(), bResult);
		}
#if defined(MOD_EVENTS_NEW_ERA)
		}
#endif
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::countNumHumanGameTurnActive() const
{
	int iCount = 0;

	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);

		if(kLoopPlayer.isHuman() && kLoopPlayer.getTeam() == GetID())
		{
			if(kLoopPlayer.isTurnActive())
			{
				++iCount;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
void CvTeam::setTurnActive(bool bNewValue, bool bDoTurn)
{
	CvAssert(GC.getGame().isSimultaneousTeamTurns());

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			kPlayer.setTurnActive(bNewValue, bDoTurn);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvTeam::isTurnActive() const
{
	CvAssert(GC.getGame().isSimultaneousTeamTurns());

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			if(kPlayer.isTurnActive())
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvTeam::PushIgnoreWarning (TeamTypes eTeam)
{
	CvAssertMsg(eTeam != NO_TEAM, "PushIgnoreWarning got NO_TEAM passed to it");
	if (eTeam == NO_TEAM)
	{
		return;
	}

	m_aiIgnoreWarningCount[eTeam] += 1;
}

//	--------------------------------------------------------------------------------
void CvTeam::PopIgnoreWarning (TeamTypes eTeam)
{
	CvAssertMsg(eTeam != NO_TEAM, "PopIgnoreWarning got NO_TEAM passed to it");
	if (eTeam == NO_TEAM)
	{
		return;
	}

	CvAssertMsg(m_aiIgnoreWarningCount[eTeam] > 0, "Trying to pop when there's nothing to be popped");
	if (m_aiIgnoreWarningCount[eTeam] > 0)
	{
		m_aiIgnoreWarningCount[eTeam] -= 1;
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::GetIgnoreWarningCount (TeamTypes eTeam)
{
	CvAssertMsg(eTeam != NO_TEAM, "GetIgnoreWarningCount got NO_TEAM passed to it");
	if (eTeam == NO_TEAM)
	{
		return -1;
	}

	return m_aiIgnoreWarningCount[eTeam];
}

//	--------------------------------------------------------------------------------
void CvTeam::setDynamicTurnsSimultMode(bool simultaneousTurns)
{//set DynamicTurnsSimultMode for every player on this team.
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
		{
			kPlayer.setDynamicTurnsSimultMode(simultaneousTurns);
		}
	}
}

//	--------------------------------------------------------------------------------
template<typename Team, typename Visitor>
void CvTeam::Serialize(Team& team, Visitor& visitor)
{
	visitor(team.m_members);

	visitor(team.m_iAliveCount);
	visitor(team.m_iEverAliveCount);
	visitor(team.m_iNumCities);
	visitor(team.m_iTotalPopulation);
	visitor(team.m_iTotalLand);
	visitor(team.m_iNukeInterception);
	visitor(team.m_iExtraWaterSeeFromCount);
	visitor(team.m_iMapTradingCount);
	visitor(team.m_iTechTradingCount);
	visitor(team.m_iGoldTradingCount);
	visitor(team.m_iAllowEmbassyTradingAllowedCount);
	visitor(team.m_iOpenBordersTradingAllowedCount);
	visitor(team.m_iDefensivePactTradingAllowedCount);
	visitor(team.m_iResearchAgreementTradingAllowedCount);
	visitor(team.m_iCityWorkingChange);
	visitor(team.m_iCityAutomatonWorkersChange);
	visitor(team.m_iBridgeBuildingCount);
	visitor(team.m_iCityLessEmbarkCost);
	visitor(team.m_iCityNoEmbarkCost);

	visitor(team.m_iWaterWorkCount);
	visitor(team.m_iRiverTradeCount);
	visitor(team.m_iBorderObstacleCount);
	visitor(team.m_iVictoryPoints);
	visitor(team.m_iEmbarkedExtraMoves);
	visitor(team.m_iCanEmbarkCount);
	visitor(team.m_iDefensiveEmbarkCount);
	visitor(team.m_iEmbarkedAllWaterPassageCount);
	visitor(team.m_iNumNaturalWondersDiscovered);
	visitor(team.m_iNumLandmarksBuilt);
	visitor(team.m_iBestPossibleRoute);
	visitor(team.m_iNumMinorCivsAttacked);

	visitor(team.m_bMapCentering);
	visitor(team.m_bHomeOfUnitedNations);
	visitor(team.m_bHasTechForWorldCongress);

	visitor(team.m_eID);

	visitor(team.m_eCurrentEra);
	visitor(team.m_eLiberatedByTeam);
	visitor(team.m_eKilledByTeam);

	visitor(team.m_aiTechShareCount);
	visitor(team.m_aiNumTurnsAtWar);
	visitor(team.m_aiNumTurnsLockedIntoWar);
	visitor(team.m_aiExtraMoves);

	visitor(MakeConstSpan(team.m_aiForceTeamVoteEligibilityCount, GC.getNumVoteSourceInfos()));

	visitor(team.m_paiTurnMadePeaceTreatyWithTeam);
	visitor(team.m_aiIgnoreWarningCount);

	visitor(team.m_abHasMet);
	visitor(team.m_abHasFoundPlayersTerritory);
	visitor(team.m_abAtWar);
	visitor(team.m_abAggressorPacifier);
	visitor(team.m_abPermanentWarPeace);
	visitor(team.m_abEmbassy);
	visitor(team.m_abOpenBorders);
	visitor(team.m_abDefensivePact);
	visitor(team.m_abResearchAgreement);
	visitor(team.m_abForcePeace);

	visitor(MakeConstSpan(team.m_abCanLaunch, GC.getNumVictoryInfos()));
	visitor(MakeConstSpan(team.m_abVictoryAchieved, GC.getNumVictoryInfos()));
	visitor(MakeConstSpan(team.m_abSmallAwardAchieved, GC.getNumSmallAwardInfos()));
	visitor(MakeConstSpan(team.m_pabTradeTech, GC.getNumTechInfos()));

	visitor(MakeConstSpan(team.m_paiRouteChange, GC.getNumRouteInfos()));
	visitor(MakeConstSpan(team.m_paiBuildTimeChange, GC.getNumBuildInfos()));
	visitor(MakeConstSpan(team.m_paiProjectCount, GC.getNumProjectInfos()));
	visitor(MakeConstSpan(team.m_paiProjectDefaultArtTypes, GC.getNumProjectInfos()));

	visitor(team.m_pavProjectArtTypes);

	visitor(MakeConstSpan(team.m_paiProjectMaking, GC.getNumProjectInfos()));

	visitor(MakeConstSpan(team.m_paiUnitClassCount, GC.getNumUnitClassInfos()));

	visitor(MakeConstSpan(team.m_paiBuildingClassCount, GC.getNumBuildingClassInfos()));
	visitor(MakeConstSpan(team.m_paiObsoleteBuildingCount, GC.getNumBuildingInfos()));

	visitor(MakeConstSpan(team.m_paiTerrainTradeCount, GC.getNumTerrainInfos()));
	visitor(MakeConstSpan(team.m_aiVictoryCountdown, GC.getNumVictoryInfos()));

	visitor(team.m_aiTurnTeamMet);

	visitor(*team.m_pTeamTechs);

	visitor(MakeConstSpan(team.m_paiTradeRouteDomainExtraRange, NUM_DOMAIN_TYPES));
	for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
	{
		visitor(MakeConstSpan(team.m_ppaaiFeatureYieldChange[i], NUM_YIELD_TYPES));
	}
	for (int i = 0; i < GC.getNumTerrainInfos(); ++i)
	{
		visitor(MakeConstSpan(team.m_ppaaiTerrainYieldChange[i], NUM_YIELD_TYPES));
	}

	for (int i = 0; i < GC.getNumImprovementInfos(); ++i)
	{
		visitor(MakeConstSpan(team.m_ppaaiImprovementYieldChange[i], NUM_YIELD_TYPES));
		visitor(MakeConstSpan(team.m_ppaaiImprovementNoFreshWaterYieldChange[i], NUM_YIELD_TYPES));
		visitor(MakeConstSpan(team.m_ppaaiImprovementFreshWaterYieldChange[i], NUM_YIELD_TYPES));
	}

	visitor(team.m_aeRevealedResources);

	visitor(team.m_iVassalageTradingAllowedCount);
	visitor(team.m_eMaster);
	visitor(team.m_bIsVoluntaryVassal);
	visitor(team.m_iNumTurnsIsVassal);
	visitor(team.m_iNumCitiesWhenVassalMade);
	visitor(team.m_iTotalPopulationWhenVassalMade);
	visitor(team.m_aiNumTurnsSinceVassalEnded);
	visitor(team.m_aiNumTurnsSinceVassalTaxSet);
	visitor(team.m_aiVassalTax);
	visitor(team.m_iCorporationsEnabledCount);

	visitor(team.m_abAtWar);
	visitor(team.m_aiNumTurnsAtWar);
	visitor(team.m_abAggressorPacifier);
}

//	--------------------------------------------------------------------------------
void CvTeam::Read(FDataStream& kStream)
{
	// Init data before load
	reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);

	DoUpdateBestRoute();
}


//	--------------------------------------------------------------------------------
void CvTeam::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

// CACHE: cache frequently used values
///////////////////////////////////////

//	--------------------------------------------------------------------------------
/// Wrapper for giving Players on this Team a notification message
void CvTeam::AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData)
{
	PlayerTypes eLoopPlayer;

	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		eLoopPlayer = (PlayerTypes) iI;
		CvPlayer& loopPlayer = GET_PLAYER(eLoopPlayer);

		if(!loopPlayer.isAlive())
			continue;

		if(loopPlayer.getTeam() != GetID())
			continue;

		if(!loopPlayer.GetNotifications())
			continue;

		loopPlayer.GetNotifications()->Add(eNotificationType, strMessage, strSummary, iX, iY, iGameDataIndex, iExtraGameData);
	}
}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
//  Acquire the map (territory or world) of eTeam
void CvTeam::AcquireMap(TeamTypes eTeam, bool bTerritoryOnly)
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	CvMap& kMap = GC.getMap();
	CvPlot* pPlot;
	int iI;

	// Loop through every plot on the map
	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pPlot = kMap.plotByIndexUnchecked(iI);

		if(pPlot == NULL)
		{
			continue;
		}

		// Plot must be revealed by eTeam
		if(!pPlot->isRevealed(eTeam))
		{
			continue;
		}

		// If this is a territory only map, the plot must be adjacent to or owned by a plot owned by eTeam
		if(bTerritoryOnly)
		{
			if(pPlot->getTeam() != eTeam || !pPlot->isAdjacentTeam(eTeam))
			{
				continue;
			}
		}

		pPlot->setRevealed(GetID(), true);
	}

	GC.getMap().updateDeferredFog();
	GC.getMap().verifyUnitValidPlot();

	if((GetID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
	{
		DLLUI->setDirty(Score_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
int CvTeam::getVassalageTradingAllowedCount() const
{
	return m_iVassalageTradingAllowedCount;
}
//	--------------------------------------------------------------------------------
bool CvTeam::IsVassalageTradingAllowed() const
{
	return (getVassalageTradingAllowedCount() > 0);
}
//	--------------------------------------------------------------------------------
void CvTeam::changeVassalageTradingAllowedCount(int iChange)
{
	m_iVassalageTradingAllowedCount = (m_iVassalageTradingAllowedCount + iChange);
	CvAssert(getVassalageTradingAllowedCount() >= 0);
}
//	--------------------------------------------------------------------------------
// Find out who we're a vassal of
TeamTypes CvTeam::GetMaster() const
{
	return m_eMaster;
}
//	--------------------------------------------------------------------------------
// We're a vassal of somebody (doesn't matter who)
bool CvTeam::IsVassalOfSomeone() const
{
	return m_eMaster!=NO_TEAM;
}
//	--------------------------------------------------------------------------------
bool CvTeam::IsVoluntaryVassal(TeamTypes eIndex) const
{
	return m_bIsVoluntaryVassal && IsVassal(eIndex);
}
//	--------------------------------------------------------------------------------
// Are we a vassal of eIndex?
bool CvTeam::IsVassal(TeamTypes eIndex) const
{
	return eIndex!=NO_TEAM && eIndex==m_eMaster;
}
//	--------------------------------------------------------------------------------
void CvTeam::setVassal(TeamTypes eIndex, bool bNewValue, bool bVoluntary)
{
	//can't be our own master
	if(eIndex==GetID())
		return;

	m_eMaster = bNewValue ? (TeamTypes) eIndex : NO_TEAM;
	m_bIsVoluntaryVassal = bNewValue ? bVoluntary : false;
}

//	-----------------------------------------------------------------------------------------------
// Are we (the master) able to end our vassalage with all of our vassals?
bool CvTeam::canEndAllVassal()
{
	if (GetNumVassals() <= 0)
		return false;

	// Go through every major.
	for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

		if (!GET_TEAM(eLoopTeam).isAlive() || !GET_TEAM(eLoopTeam).isMajorCiv())
			continue;

		// Is eLoopTeam our vassal?
		if (!GET_TEAM(eLoopTeam).IsVassal(GetID()))
			continue;

		// Too soon to end our vassalage with ePlayer
		if (GET_TEAM(eLoopTeam).GetNumTurnsIsVassal() < GC.getGame().getGameSpeedInfo().getMinimumVassalLiberateTurns())
		{
			return false;
		}
	}

	return true;
}

// Can we end our vassalage with eTeam?
bool CvTeam::canEndVassal(TeamTypes eTeam) const
{
	if(eTeam == NO_TEAM) return false;
	
	// Can't end a vassalage if we're not the vassal of eTeam.
	if(!IsVassal(eTeam))
	{
		return false;
	}

	//blocks capitulated vassal rebellion?
	if (!IsVoluntaryVassal(eTeam))
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
			{
				if (GET_PLAYER(eLoopPlayer).IsVassalsNoRebel())
					return false;
			}
		}
	}

	// can't end vassalage with ourselves
	if(eTeam == GetID())
	{
		return false;
	}

	if(!isAlive())
		return false;

	if(!GET_TEAM(eTeam).isAlive())
		return true;

	// Too soon to end our vassalage with ePlayer
	int iMinTurns = IsVoluntaryVassal(eTeam) ? /*10*/ GC.getGame().getGameSpeedInfo().getMinimumVoluntaryVassalTurns() : /*50*/ GC.getGame().getGameSpeedInfo().getMinimumVassalTurns();

	if (GetNumTurnsIsVassal() < iMinTurns)
	{
		return false;
	}

	// We're the voluntary vassal of eTeam and it's not too early to end vassalage - we're not bound by the 50% rules
	if (IsVoluntaryVassal(eTeam))
	{
		return true;
	}

	// Number of cities
	bool bAbleToEndVassalage = false;

	int iCityPercent = 0;
	int iPopPercent = 0;

	if(getNumCitiesWhenVassalMade() > 0 && getTotalPopulationWhenVassalMade() > 0)
	{
		iCityPercent = getNumCities() * 100 / getNumCitiesWhenVassalMade();
		iPopPercent = getTotalPopulation() * 100 / getTotalPopulationWhenVassalMade();
	}
	// No cities when vassalage was made (what?) - can end vassalage if we have cities and population
	else
	{
		return (getNumCities() > 0 && getTotalPopulation() > 0);
	}

	int iMasterCities = GET_TEAM(eTeam).getNumCities();
	int iMasterPopulation = GET_TEAM(eTeam).getTotalPopulation();

	// Master is dead???
	if(iMasterCities == 0 || iMasterPopulation == 0)
		return true;

	// We have lost cities so that we are 75% less than what we started with (Master is not protecting us)
	if (iCityPercent <= /*75*/ GD_INT_GET(VASSALAGE_VASSAL_LOST_CITIES_THRESHOLD))
	{
		bAbleToEndVassalage = true;
	}
	// We have more than 300% of our original population (We don't need his protection anymore)
	else if (iPopPercent >= /*300*/ GD_INT_GET(VASSALAGE_VASSAL_POPULATION_THRESHOLD))
	{
		bAbleToEndVassalage = true;
	}
	// We have 60% or more of the Master's population OR cities
	else if (getNumCities() * 100 / iMasterCities >= /*60*/ GD_INT_GET(VASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD) &&
			getTotalPopulation() * 100 / iMasterPopulation >= /*60*/ GD_INT_GET(VASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD))
	{
		bAbleToEndVassalage = true;
	}

	return bAbleToEndVassalage;
}
//	-----------------------------------------------------------------------------------------------
//  We are no longer the vassal of eTeam
void CvTeam::DoEndVassal(TeamTypes eTeam, bool bPeaceful, bool bSuppressNotification)
{
	CvString strBuffer;
	int iI;
	
	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID");
	
	if(!IsVassal(eTeam))
		return;

	// What does it mean when we break vassalage
	CvPlayer* pOurPlayer;
	PlayerTypes eOurPlayer;
	for(int iOurPlayerLoop = 0; iOurPlayerLoop < MAX_CIV_PLAYERS; iOurPlayerLoop++)
	{
		eOurPlayer = (PlayerTypes) iOurPlayerLoop;
		pOurPlayer = &GET_PLAYER(eOurPlayer);
			
		if(pOurPlayer->isAlive())
		{
			// Our Team
			if(pOurPlayer->getTeam() == GetID())
			{
				pOurPlayer->GetDiplomacyAI()->DoWeEndedVassalageWithSomeone(eTeam);
				GET_TEAM(eTeam).SetVassalTax(eOurPlayer, 0);
				GET_TEAM(eTeam).SetNumTurnsSinceVassalTaxSet(eOurPlayer, -1);
			}
		}
	}

	//Break open borders
	SetAllowsOpenBordersToTeam(eTeam, false);
	GET_TEAM(eTeam).SetAllowsOpenBordersToTeam(GetID(), false);

	setVassal(eTeam, false);

	// reset counters
	SetNumTurnsSinceVassalEnded(eTeam, 0);
	SetNumTurnsIsVassal(-1);

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateConquestStats();
	}

	// Not peaceful end of vassalage? Declare war!
	if(!bPeaceful)
	{
		declareWar(eTeam);
	}

	// Update war/peace relationships for all of eTeam's vassals
	for(int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		if(GET_TEAM((TeamTypes)iTeamLoop).GetMaster() == eTeam)
		{
			GET_TEAM((TeamTypes)iTeamLoop).DoUpdateVassalWarPeaceRelationships();
		}
	}

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		// Update Happiness for all players
		if(GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == GetID())
		{
			GET_PLAYER(eLoopPlayer).CalculateNetHappiness();
		}
	}

	vector<PlayerTypes> vOurTeam = getPlayers();
	for(size_t i=0; i<vOurTeam.size(); i++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(vOurTeam[i]);
		if (kPlayer.isAlive())
		{
			vector<PlayerTypes> v = kPlayer.GetDiplomacyAI()->GetAllValidMajorCivs();
			kPlayer.GetDiplomacyAI()->DoReevaluatePlayers(v, false, true, true);
		}
	}

	// Send out notifications to everyone
	if (bSuppressNotification)
		return;

	Localization::String locString, summaryString;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iI;

		if (!GET_PLAYER(ePlayer).GetNotifications())
			continue;

		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isHuman())
		{
			// Player is no longer the vassal
			if (GET_PLAYER(ePlayer).getTeam() == GetID())
			{
				if (bPeaceful) 
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_FROM_YOU_PEACEFUL");
					locString << GET_TEAM(eTeam).getName().GetCString();
				}
				else 
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_FROM_YOU");
					locString << GET_TEAM(eTeam).getName().GetCString();
				}

				summaryString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_FROM_YOU_SUMMARY");
				summaryString << GET_TEAM(eTeam).getName().GetCString();
				GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), summaryString.toUTF8(), -1, -1, GET_TEAM(eTeam).getLeaderID());
			}
			// Player is no longer the master
			else if (GET_PLAYER(ePlayer).getTeam() == eTeam)
			{
				if (bPeaceful) 
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_WITH_YOU_PEACEFUL");
					locString << getName().GetCString();
				}
				else 
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_WITH_YOU");
					locString << getName().GetCString();
				}

				summaryString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_ENDED_WITH_YOU_SUMMARY");
				summaryString << getName().GetCString();
				GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), summaryString.toUTF8(), -1, -1, this->getLeaderID());
			}
			// Unrelated players
			else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam))
			{
				locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_ENDED_VASSALAGE");
				locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
				GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE, locString.toUTF8(), locString.toUTF8(), -1, -1, GET_TEAM(eTeam).getLeaderID(), this->getLeaderID());
			}
		}
		else if (GET_PLAYER(ePlayer).isObserver())
		{
			locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_ENDED_VASSALAGE");
			locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
			GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE, locString.toUTF8(), locString.toUTF8(), -1, -1, GET_TEAM(eTeam).getLeaderID(), this->getLeaderID());
		}
	}

	strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_ENDED_VASSALAGE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
}

//	----------------------------------------------------------------------------------------------
// We liberate eTeam, if we can
void CvTeam::DoLiberateVassal(TeamTypes eTeam)
{
	if (!CanLiberateVassal(eTeam))
		return;

	// End our vassalage peacefully
	GET_TEAM(eTeam).DoEndVassal(GetID(), true, false);

	// Find our vassals
	for (int iVassalPlayer = 0; iVassalPlayer < MAX_MAJOR_CIVS; iVassalPlayer++)
	{
		PlayerTypes eVassalPlayer = (PlayerTypes) iVassalPlayer;
		if (GET_PLAYER(eVassalPlayer).isAlive() && GET_PLAYER(eVassalPlayer).getTeam() == eTeam)
		{
			GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->DoLiberatedFromVassalage(GetID(), false);
		}
	}
}
//	----------------------------------------------------------------------------------------------
// Update vassal war/peace relationships for one team
void CvTeam::DoUpdateVassalWarPeaceRelationships()
{
	TeamTypes eMaster = GetMaster();

	// If I'm not alive I can't declare war!
	if (!isAlive())
	{
		return;
	}

	// Have to be someone's vassal
	if (eMaster == NO_TEAM)
	{
		return;
	}

	// Never at war with Master
	if (isAtWar(eMaster))
	{
		makePeace(eMaster, true, false, getLeaderID());
	}

	for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		TeamTypes eTeam = (TeamTypes) iTeamLoop;

		if (eTeam == NO_TEAM || eTeam == eMaster || GET_TEAM(eTeam).isBarbarian())
		{
			continue;
		}

		// Master at war with eTeam?
		if(GET_TEAM(eMaster).isAtWar(eTeam))
		{
			if (!isAtWar(eTeam))
			{
				DoDeclareWar(getLeaderID(), false, eTeam, false);
			}
		}
		// Not at war
		else
		{
			if (isAtWar(eTeam))
			{
				DoMakePeace(getLeaderID(), GET_TEAM(eMaster).isPacifier(eTeam), eTeam, true);
			}
		}
	}
}
//	----------------------------------------------------------------------------------------------
// Can we become eTeam's vassal?
bool CvTeam::canBecomeVassal(TeamTypes eTeam, bool bIgnoreAlreadyVassal) const
{
	// Can't become a vassal of nobody
	if(eTeam == NO_TEAM)
	{
		return false;
	}

	// Can't make ourselves a vassal.
	if(eTeam == GetID())
	{
		return false;
	}

	// Vassalage is disabled...
	if(GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
	{
		return false;
	}

	// Human Vassalage isn't enabled...
	if(!GC.getGame().isOption(GAMEOPTION_HUMAN_VASSALS) && isHuman())
	{
		return false;
	}

	// Have to have met eTeam
	if(!isHasMet(eTeam))
	{
		return false;
	}

	// Someone's not alive.
	if(!isAlive() || !(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	// Ignore minors
	if(isMinorCiv() || isBarbarian())
	{
		return false;
	}

	// Master civ needs to be able to create vassals
	if(!GET_TEAM(eTeam).IsVassalageTradingAllowed())
		return false;

	// We are a vassal, and not ignoring it
	if(!bIgnoreAlreadyVassal && IsVassalOfSomeone())
		return false;

	// Master is a vassal, he can't get his own vassals
	if(GET_TEAM(eTeam).IsVassalOfSomeone())
		return false;

	// Too early for us to become eTeam's vassal
	if(IsTooSoonForVassal(eTeam))
		return false;

	// I don't have cities or population (prevents crash)
	if(getNumCities() <= 0 || getTotalPopulation() <= 0)
		return false;

	// Check Lua to see if an event has been registered to prevent vassals
	// Create an event like follows: GameEvents.CanMakeVassal.Add(function(eMasterTeam, eVassalTeam) ...)

	// First, obtain the Lua script system.
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		// Construct and push in some event arguments.
		CvLuaArgsHandle args(2);
		args->Push(eTeam);
		args->Push(GetID());

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if (LuaSupport::CallTestAll(pkScriptSystem, "CanMakeVassal", args.get(), bResult))
		{
			// Check the result.
			if (!bResult)
				return false;
		}
	}

	return true;
}
//	-----------------------------------------------------------------------------------------------
// Can we liberate our vassal eTeam?
bool CvTeam::CanLiberateVassal(TeamTypes eTeam) const
{
	// Must be a valid team
	if(eTeam == NO_TEAM)
		return false;

	// nope, can't liberate ourselves
	if(eTeam == GetID())
		return false;

	// must be alive
	if(!GET_TEAM(eTeam).isAlive())
		return false;

	// Vassalage is disabled...
	if(GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
		return false;

	// Must be a vassal of ours
	if(!GET_TEAM(eTeam).IsVassal(GetID()))
		return false;

	// Must have been our vassal for a certain time
	if(GET_TEAM(eTeam).GetNumTurnsIsVassal() < GC.getGame().getGameSpeedInfo().getMinimumVassalLiberateTurns())
		return false;

	return true;
}
//	-----------------------------------------------------------------------------------------------
bool CvTeam::CanMakeVassal(TeamTypes eTeam, bool bIgnoreAlreadyVassal) const
{
	return GET_TEAM(eTeam).canBecomeVassal(GetID(), bIgnoreAlreadyVassal);
}
//	-----------------------------------------------------------------------------------------------
// We become the new Vassal of eTeam
void CvTeam::DoBecomeVassal(TeamTypes eTeam, bool bVoluntary, PlayerTypes eOriginatingMaster)
{
	Localization::String locString, summaryString;

	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");

	// we must be able to become their vassal
	if(!canBecomeVassal(eTeam, /*bIgnoreAlreadyVassal*/ true))
		return;

	// Immediately cancel all Defensive Pacts!
	for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		TeamTypes eOtherTeam = (TeamTypes) iTeamLoop;
		SetHasDefensivePact(eOtherTeam, false);
		GET_TEAM(eOtherTeam).SetHasDefensivePact(GetID(), false);
	}

	// If this is voluntary vassalage, master declares war on everyone the vassal is at war with (prior to updating relationships)
	if (bVoluntary)
	{
		for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
		{
			TeamTypes eOtherTeam = (TeamTypes) iTeamLoop;

			if (eOtherTeam != eTeam && eOtherTeam != GetID() && GET_TEAM(eOtherTeam).isAlive())
			{
				if (isAtWar(eOtherTeam) && !GET_TEAM(eTeam).isAtWar(eOtherTeam))
				{
					if (eOriginatingMaster != NO_PLAYER)
						GET_TEAM(eTeam).declareWar(eOtherTeam, false, eOriginatingMaster);
					else
						GET_TEAM(eTeam).declareWar(eOtherTeam, false);
				}
			}
		}
	}

	std::vector<PlayerTypes> aMasterTeam;
	std::vector<PlayerTypes> aVassalTeam;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if (GET_PLAYER(eLoopPlayer).isAlive())
		{
			if(GET_PLAYER(eLoopPlayer).getTeam() == GetID()) aVassalTeam.push_back(eLoopPlayer);
			if(GET_PLAYER(eLoopPlayer).getTeam() == eTeam) aMasterTeam.push_back(eLoopPlayer);
		}
	}

	// Let's check if we have a vassal already, if so they are liberated
	for (int iOtherTeamLoop = 0; iOtherTeamLoop < MAX_TEAMS; iOtherTeamLoop++)
	{
		TeamTypes eOtherTeam = (TeamTypes) iOtherTeamLoop;
		
		if (GET_TEAM(eOtherTeam).isAlive() && eOtherTeam != eTeam && GET_TEAM(eOtherTeam).IsVassal(GetID()))
		{
			bool bWasVoluntary = GET_TEAM(eOtherTeam).IsVoluntaryVassal(GetID());
			GET_TEAM(eOtherTeam).DoEndVassal(GetID(), true, true); // they are no longer our vassal

			// Put these guys at peace with everyone
			if (!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) && !GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
			{
				for (int iThirdTeamLoop = 0; iThirdTeamLoop < MAX_TEAMS; iThirdTeamLoop++)
				{
					TeamTypes eThirdTeam = (TeamTypes) iThirdTeamLoop;

					if (eThirdTeam == m_eID)
						continue;
					if (!GET_TEAM(eThirdTeam).isAlive())
						continue;
					if (GET_TEAM(eThirdTeam).isBarbarian())
						continue;
					if (GET_TEAM(eThirdTeam).isMinorCiv())
					{
						if (GET_PLAYER(GET_TEAM(eThirdTeam).getLeaderID()).isMinorCiv() && GET_PLAYER(GET_TEAM(eThirdTeam).getLeaderID()).GetMinorCivAI()->IsPermanentWar(eOtherTeam))
							continue;
					}

					if (GET_TEAM(eThirdTeam).isAtWar(eOtherTeam))
					{
						GET_TEAM(eOtherTeam).makePeace(eThirdTeam, true, true, GET_TEAM(eOtherTeam).getLeaderID());
					}
				}
			}
			if (!bWasVoluntary && !GET_TEAM(eOtherTeam).isAtWar(eTeam)) // Diplo bonus for liberating a capitulated vassal
			{
				for (int iVassalPlayer = 0; iVassalPlayer < MAX_MAJOR_CIVS; iVassalPlayer++)
				{
					PlayerTypes eVassalPlayer = (PlayerTypes) iVassalPlayer;
					if (GET_PLAYER(eVassalPlayer).isAlive() && GET_PLAYER(eVassalPlayer).getTeam() == eOtherTeam)
					{
						GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->DoLiberatedFromVassalage(eTeam, true);
					}
				}
			}
			else if (bWasVoluntary) // Diplo penalty for forcibly liberating a voluntary vassal
			{
				for (int iVassalPlayer = 0; iVassalPlayer < MAX_MAJOR_CIVS; iVassalPlayer++)
				{
					PlayerTypes eVassalPlayer = (PlayerTypes) iVassalPlayer;
					if (GET_PLAYER(eVassalPlayer).isAlive() && GET_PLAYER(eVassalPlayer).getTeam() == eOtherTeam)
					{
						for (int iNewMasterPlayer = 0; iNewMasterPlayer < MAX_MAJOR_CIVS; iNewMasterPlayer++)
						{
							PlayerTypes eNewMaster = (PlayerTypes) iNewMasterPlayer;
							if (GET_PLAYER(eNewMaster).isAlive() && GET_PLAYER(eNewMaster).getTeam() == eTeam)
							{
								GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eNewMaster, 300);
							}
						}
					}
				}
			}
		}
	}

	setVassal(eTeam, true, bVoluntary);	// We become the vassal of eTeam
	GET_TEAM(eTeam).AcquireMap(GetID(), /*bTerritoryOnly*/ true);	// eTeam acquires our territory map

	// Let's save some stuff
	setNumCitiesWhenVassalMade(getNumCities());
	setTotalPopulationWhenVassalMade(getTotalPopulation());

	// reset counters
	SetNumTurnsSinceVassalEnded(eTeam,-1);
	SetNumTurnsIsVassal(0);

	// If we haven't met the guy, meet him
	if (!isHasMet(eTeam))
		meet(eTeam, true);

	//Set open borders
	SetAllowsOpenBordersToTeam(eTeam, true);

	// Update war/peace relationships for all of eTeam's vassals
	for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		if (GET_TEAM((TeamTypes)iTeamLoop).GetMaster() == eTeam)
		{
			GET_TEAM((TeamTypes)iTeamLoop).DoUpdateVassalWarPeaceRelationships();
		}
	}

	for (std::vector<PlayerTypes>::iterator it = aVassalTeam.begin(); it != aVassalTeam.end(); it++)
	{
		// Notify DiploAI that we are now eTeam's vassal
		GET_PLAYER(*it).GetDiplomacyAI()->DoWeMadeVassalageWithSomeone(eTeam, bVoluntary);
		
		// Update Happiness for all players
		GET_PLAYER(*it).CalculateNetHappiness();

		// Send notification to master to set taxes for player
		Localization::String locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAXES_AVAILABLE");
		locString << GET_PLAYER(*it).getName();
		GET_TEAM(eTeam).AddNotification(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, *it);
	}

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		TeamTypes eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

		if (!GET_PLAYER(eLoopPlayer).isAlive() || GET_PLAYER(eLoopPlayer).isHuman())
			continue;

		if (eLoopTeam == GetID())
		{
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateCompetingForVictory();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateRecklessExpanders();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateWonderSpammers();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateTechBlockLevels();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdatePolicyBlockLevels();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateVictoryDisputeLevels();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateVictoryBlockLevels();
			GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateConquestStats();
		}

		// AI needs to reevaluate all players (reprioritizes friendships and prevents exceeding the Defensive Pact limit)
		vector<PlayerTypes> v = GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetAllValidMajorCivs();
		GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoReevaluatePlayers(v, !bVoluntary, true);
	}

	if(GC.getGame().isFinalInitialized())
	{
		// Message everyone about what happened
		if(!isBarbarian() && !(GET_TEAM(eTeam).isBarbarian()))
		{
			{
				for(int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					PlayerTypes ePlayer = (PlayerTypes) iI;

					if((GET_PLAYER(ePlayer).isAlive() || GET_PLAYER(ePlayer).isObserver()) && GET_PLAYER(ePlayer).GetNotifications())
					{
						// Players that now have the Vassal
						if(GET_PLAYER(ePlayer).getTeam() == eTeam)
						{
							if(bVoluntary) {
								summaryString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_NOW_VASSAL_YOU_VOLUNTARY_SUMMARY");
								summaryString << getName().GetCString();

								locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_NOW_VASSAL_YOU_VOLUNTARY");
								locString << getName().GetCString();
							}
							else {
								summaryString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_NOW_VASSAL_YOU_SUMMARY");
								summaryString << getName().GetCString();

								locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_NOW_VASSAL_YOU");
								locString << getName().GetCString();
							}
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), summaryString.toUTF8(), -1, -1, this->getLeaderID());
						}
						// Players that are the vassal
						else if(GET_PLAYER(ePlayer).getTeam() == GetID())
						{
							summaryString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_YOU_NOW_VASSAL_SUMMARY");
							summaryString << GET_TEAM(eTeam).getName().GetCString();

							locString = Localization::Lookup("TXT_KEY_MISC_VASSALAGE_YOU_NOW_VASSAL");
							locString << GET_TEAM(eTeam).getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), summaryString.toUTF8(), -1, -1, GET_TEAM(eTeam).getLeaderID());
						}
						// Players that are on neither team, but know both parties
						else if((GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam)) || GET_PLAYER(ePlayer).isObserver())
						{
							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_NOW_VASSAL");
							locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), eTeam);
						}
						// Players that know the Master
						else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam))
						{
							summaryString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_NOW_VASSAL_UNKNOWN_VASSAL_SUMMARY");
							summaryString << GET_TEAM(eTeam).getName().GetCString();

							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_NOW_VASSAL_UNKNOWN_VASSAL");
							locString << GET_TEAM(eTeam).getName().GetCString() << Localization::Lookup("TXT_KEY_UNMET_PLAYER");
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), summaryString.toUTF8(), -1, -1, -1, eTeam);
						}
						// Players that know the Vassal
						else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()))
						{
							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_NOW_VASSAL_UNKNOWN_MASTER");
							locString << GET_TEAM(GetID()).getName().GetCString() << Localization::Lookup("TXT_KEY_UNMET_PLAYER");
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), -1);
						}
						// Players that know no one
						else
						{
							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_ACQUIRE_NEW_VASSAL");
							locString << Localization::Lookup("TXT_KEY_UNMET_PLAYER");
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, -1, -1);
						}
					}
				}
			}

			locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_MAKES_VASSAL");
			locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), locString.toUTF8(), -1, -1);
		}
	}
}

//	--------------------------------------------------------------------------------
//	Are we locked into a war with eOtherTeam because our Master is at war with him?
bool CvTeam::IsVassalLockedIntoWar(TeamTypes eOtherTeam) const
{
	if (IsVassalOfSomeone() && GET_TEAM(GetMaster()).isAtWar(eOtherTeam))
		return true;

	return false;
}
//	--------------------------------------------------------------------------------
int CvTeam::getNumCitiesWhenVassalMade() const
{
	return m_iNumCitiesWhenVassalMade;
}
//	--------------------------------------------------------------------------------
void CvTeam::setNumCitiesWhenVassalMade(int iValue)
{
	m_iNumCitiesWhenVassalMade = iValue;
}
//	--------------------------------------------------------------------------------
int CvTeam::getTotalPopulationWhenVassalMade() const
{
	return m_iTotalPopulationWhenVassalMade;
}
//	--------------------------------------------------------------------------------
void CvTeam::setTotalPopulationWhenVassalMade(int iValue)
{
	m_iTotalPopulationWhenVassalMade = iValue;
}
//	--------------------------------------------------------------------------------
int CvTeam::GetNumTurnsIsVassal() const
{
	return m_iNumTurnsIsVassal;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetNumTurnsIsVassal(int iValue)
{
	m_iNumTurnsIsVassal = iValue;
}

//	--------------------------------------------------------------------------------
void CvTeam::ChangeNumTurnsIsVassal(int iChange)
{
	SetNumTurnsIsVassal(GetNumTurnsIsVassal() + iChange);
}
//	--------------------------------------------------------------------------------
int CvTeam::GetNumTurnsSinceVassalEnded(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumTurnsSinceVassalEnded[eTeam];
}

//	--------------------------------------------------------------------------------
void CvTeam::SetNumTurnsSinceVassalEnded(TeamTypes eTeam, int iValue)
{
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eTeam != GetID(), "Team is setting vassal ended turns with itself!");
	if(eTeam != GetID() || iValue == 0)
		m_aiNumTurnsSinceVassalEnded[eTeam] = iValue;
}

//	--------------------------------------------------------------------------------
void CvTeam::ChangeNumTurnsSinceVassalEnded(TeamTypes eTeam, int iChange)
{
	SetNumTurnsSinceVassalEnded(eTeam, GetNumTurnsSinceVassalEnded(eTeam) + iChange);
}
//	--------------------------------------------------------------------------------
// Can we tax ePlayer right now?
bool CvTeam::CanSetVassalTax(PlayerTypes ePlayer) const
{
	if (!MOD_DIPLOMACY_CIV4_FEATURES || GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
		return false;

	if (!isAlive() || !GET_PLAYER(ePlayer).isAlive() || !GET_PLAYER(ePlayer).isMajorCiv())
		return false;

	// They must be our vassal
	if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsVassal(GetID()))
		return false;

	// Can't set tax too fast (if it was already set)
	if(GetNumTurnsSinceVassalTaxSet(ePlayer) > -1 && 
		GetNumTurnsSinceVassalTaxSet(ePlayer) < GC.getGame().getGameSpeedInfo().getMinimumVassalTaxTurns())
		return false;

	return true;
}
//	--------------------------------------------------------------------------------
// We apply a vassal tax to ePlayer
void CvTeam::DoApplyVassalTax(PlayerTypes ePlayer, int iPercent)
{
	if (!CanSetVassalTax(ePlayer))
		return;

	iPercent = max(iPercent, /*0*/ GD_INT_GET(VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM));
	iPercent = min(iPercent, /*25*/ GD_INT_GET(VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM));

	int iCurrentTaxRate = GetVassalTax(ePlayer);
	
	SetNumTurnsSinceVassalTaxSet(ePlayer, 0);
	SetVassalTax(ePlayer, iPercent);

	// Note: using EspionageScreen dirty for this.
	GC.GetEngineUserInterface()->setDirty(EspionageScreen_DIRTY_BIT, true);

	// notify diplo AI if there was some change		
	if(iPercent != iCurrentTaxRate)
	{
		GET_PLAYER(ePlayer).GetDiplomacyAI()->DoVassalTaxChanged(GetID(), (iPercent < iCurrentTaxRate));	

		// send a notification if there was some change
		Localization::String locString, summaryString;
		if(iPercent > iCurrentTaxRate)
		{
			locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_INCREASED");
			summaryString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_INCREASED_SUMMARY");
		}
		else
		{
			locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_DECREASED");
			summaryString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_DECREASED_SUMMARY");
		}

		locString << getName().GetCString() << iCurrentTaxRate << iPercent;

		GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_GENERIC, locString.toUTF8(), summaryString.toUTF8(), -1, -1, this->getLeaderID());
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::SetVassalTax(PlayerTypes ePlayer, int iPercent)
{
	CvAssertMsg(ePlayer >= 0, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	
	m_aiVassalTax[ePlayer] = iPercent;
}
//	--------------------------------------------------------------------------------
int CvTeam::GetVassalTax(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "TEAM: VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "TEAM: VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	
	return m_aiVassalTax[ePlayer];
}
//	--------------------------------------------------------------------------------
int CvTeam::GetNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_aiNumTurnsSinceVassalTaxSet[ePlayer];
}
//	--------------------------------------------------------------------------------
void CvTeam::ChangeNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(iChange != 0)
		SetNumTurnsSinceVassalTaxSet(ePlayer, GetNumTurnsSinceVassalTaxSet(ePlayer) + iChange);
}
//	--------------------------------------------------------------------------------
void CvTeam::SetNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "SET VASSAL TAX! Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_aiNumTurnsSinceVassalTaxSet[ePlayer] = iValue;
}
//	--------------------------------------------------------------------------------
// Is it too soon for us to become eTeam's vassal?
bool CvTeam::IsTooSoonForVassal(TeamTypes eTeam) const
{
	// a value of -1 means we haven't made a vassal yet
	if(GetNumTurnsSinceVassalEnded(eTeam) < GC.getGame().getGameSpeedInfo().getNumTurnsBetweenVassals() &&
		GetNumTurnsSinceVassalEnded(eTeam) > -1)
	{
		return true;
	}

	return false;
}
//	--------------------------------------------------------------------------------
/// How many vassals do we have?
int CvTeam::GetNumVassals()
{
	CvAssertMsg(GetID() >= 0, "TeamID is expected to be non-negative (invalid Index)");
	CvAssertMsg(GetID() < MAX_TEAMS, "TeamID is expected to be within maximum bounds (invalid Index)");

	if(GET_TEAM(GetID()).isMinorCiv() || GET_TEAM(GetID()).isBarbarian())
		return 0;

	int iVassals = 0;

	TeamTypes eTeamLoop;
	for(int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		eTeamLoop = (TeamTypes) iTeamLoop;
		if(eTeamLoop == GetID())
			continue;

		// eTeamLoop vassal of us?
		if(GET_TEAM(eTeamLoop).IsVassal(GetID()))
		{
			iVassals++;
		}
	}

	return iVassals;
}

//	--------------------------------------------------------------------------------
/// Can we trade this tech?
bool CvTeam::IsTradeTech(TechTypes eTech) const
{
	FAssert(eTech >= 0);
	FAssert(eTech < GC.getNumTechInfos());

	return m_pabTradeTech[eTech];
}
//	--------------------------------------------------------------------------------
/// Sets if we can trade this tech
void CvTeam::SetTradeTech(TechTypes eTech, bool bValue)
{
	FAssert(eTech >= 0);
	FAssert(eTech < GC.getNumTechInfos());

	m_pabTradeTech[eTech] = bValue;
}

int CvTeam::GetSSProjectCount()
{
	ProjectTypes ApolloProgram = (ProjectTypes)GD_INT_GET(SPACE_RACE_TRIGGER_PROJECT);
	ProjectTypes capsuleID = (ProjectTypes)GD_INT_GET(SPACESHIP_CAPSULE);
	ProjectTypes boosterID = (ProjectTypes)GD_INT_GET(SPACESHIP_BOOSTER);
	ProjectTypes stasisID = (ProjectTypes)GD_INT_GET(SPACESHIP_STASIS);
	ProjectTypes engineID = (ProjectTypes)GD_INT_GET(SPACESHIP_ENGINE);

	int iTotal = 0;
	iTotal += getProjectCount((ProjectTypes)ApolloProgram) + getProjectMaking((ProjectTypes)ApolloProgram);
	iTotal += getProjectCount((ProjectTypes)capsuleID) + getProjectMaking((ProjectTypes)capsuleID);
	iTotal += getProjectCount((ProjectTypes)boosterID) + getProjectMaking((ProjectTypes)boosterID);
	iTotal += getProjectCount((ProjectTypes)stasisID) + getProjectMaking((ProjectTypes)stasisID);
	iTotal += getProjectCount((ProjectTypes)engineID) + getProjectMaking((ProjectTypes)engineID);

	return iTotal;
}
#endif
