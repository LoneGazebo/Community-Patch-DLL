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

#include "CvDllUnit.h"

#include "LintFree.h"

// statics
CvTeam* CvTeam::m_aTeams = NULL;

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
	m_pavProjectArtTypes = NULL;
	m_paiProjectMaking = NULL;
	m_paiUnitClassCount = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_pTeamTechs = FNEW(CvTeamTechs, c_eCiv5GameplayDLL, 0);

	m_ppaaiImprovementYieldChange = NULL;
	m_ppaaiImprovementNoFreshWaterYieldChange = NULL;
	m_ppaaiImprovementFreshWaterYieldChange = NULL;

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

	m_paiRouteChange = NULL;
	m_paiBuildTimeChange = NULL;
	m_paiProjectCount = NULL;
	m_paiProjectDefaultArtTypes = NULL;
	m_pavProjectArtTypes = NULL;
	m_paiProjectMaking = NULL;
	m_paiUnitClassCount = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_ppaaiImprovementYieldChange = NULL;
	m_ppaaiImprovementNoFreshWaterYieldChange = NULL;
	m_ppaaiImprovementFreshWaterYieldChange = NULL;

	m_pTeamTechs->Uninit();

	m_iNumMembers = 0;
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
	m_iTradeAgreementTradingAllowedCount = 0;
	m_iPermanentAllianceTradingCount = 0;
	m_iBridgeBuildingCount = 0;
	m_iWaterWorkCount = 0;
	m_iRiverTradeCount = 0;
	m_iBorderObstacleCount = 0;
	m_iVictoryPoints = 0;
	m_iEmbarkedExtraMoves = 0;
	m_iCanEmbarkCount = 0;
	m_iDefensiveEmbarkCount = 0;
	m_iEmbarkedAllWaterPassageCount = 0;
	m_iNumNaturalWondersDiscovered = 0;
	m_iBestPossibleRoute = NO_ROUTE;
	m_iNumMinorCivsAttacked = 0;

	m_bMapCentering = false;
	m_bHasBrokenPeaceTreaty = false;
	m_bHomeOfUnitedNations = false;

	m_bBrokenMilitaryPromise = false;
	m_bBrokenExpansionPromise = false;
	m_bBrokenBorderPromise = false;
	m_bBrokenCityStatePromise = false;

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
		m_abHasMet[i] = false;
		m_abAtWar[i] = false;
		m_abPermanentWarPeace[i] = false;
		m_abEmbassy[i] = false;
		m_abOpenBorders[i] = false;
		m_abDefensivePact[i] = false;
		m_abResearchAgreement[i] = false;
		m_abTradeAgreement[i] = false;
		m_abForcePeace[i] = false;
	}

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		m_abHasFoundPlayersTerritory[i] = false;
	}

	m_eID = NO_TEAM;
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
		int numTerrainInfos = GC.getNumTerrainInfos();
		int numImprovementInfos = GC.getNumImprovementInfos();

		//Perform batch allocation
		AllocData aData[] =
		{
			{&m_aiForceTeamVoteEligibilityCount,	numVoteSourceInfos, 0},

			{&m_abCanLaunch,						numVictoryInfos, 0},
			{&m_abVictoryAchieved,					numVictoryInfos, 0},
			{&m_abSmallAwardAchieved,				numSmallAwardInfos, 0},

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

		for(int i = 0; i < MAX_TEAMS; i++)
		{
			m_aiTurnTeamMet[i] = -1;
			m_aiNumTurnsAtWar[i] = 0;
			m_aiNumTurnsLockedIntoWar[i] = 0;
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
		m_pavProjectArtTypes = FNEW(std::vector<int> [GC.getNumProjectInfos()], c_eCiv5GameplayDLL, 0);
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
						DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_THEIRALLIANCE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")*/);
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
					declareWar(((TeamTypes)iI));
				}
				else if(isAtWar((TeamTypes)iI))
				{
					GET_TEAM(eTeam).declareWar(((TeamTypes)iI));
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

		for(iJ = 0; iJ < iNumInvisibleInfos; iJ++)
		{
			pLoopPlot->changeInvisibleVisibilityCount(GetID(), ((InvisibleTypes)iJ), pLoopPlot->getInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ)));
		}

		if(pLoopPlot->isRevealed(eTeam))
		{
			pLoopPlot->setRevealed(GetID(), true, false, eTeam);
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
												kOtherPlayer.processBuilding(eBuilding, pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding), /*bFirst*/ false, pLoopCity->area());
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

			GC.getGame().DoDiplomacyVictoryVote(/*bPreliminaryVote*/ true); // Also sets the countdown for the first, real election
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
	int iI;

	CvAssertMsg(isAlive(), "isAlive is expected to be true");

	// Barbarians get all Techs that 3/4 of alive players get
	if(isBarbarian())
	{
		DoBarbarianTech();
	}
	// NOT barbs
	else
	{
		// War counter
		TeamTypes eTeam;
		int iTeamLoop;
		for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eTeam = (TeamTypes) iTeamLoop;

			if(!GET_TEAM(eTeam).isBarbarian())
			{
				if(isAtWar(eTeam))
					ChangeNumTurnsAtWar(eTeam, 1);
				else
					SetNumTurnsAtWar(eTeam, 0);
			}

			if(GetNumTurnsLockedIntoWar(eTeam) > 0)
				ChangeNumTurnsLockedIntoWar(eTeam, -1);
		}
	}

	// City States also get all Techs that 3/4 of alive players get
	if(isMinorCiv())
	{
		DoMinorCivTech();
	}

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		GetTeamTechs()->SetNoTradeTech(((TechTypes)iI), false);
	}

	DoTestSmallAwards();

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

	// 75% of majors (rounded down) need the tech for the Barbs to get it
	int iTechPercent = /*75*/ GC.getBARBARIAN_TECH_PERCENT();
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

	// 40% of majors (rounded down) need the tech for the Minors to get it
	int iTechPercent = /*40*/ GC.getMINOR_CIV_TECH_PERCENT();
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
	if(eTeam == NO_TEAM) return false;

	if(GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
	{
		return false;
	}

	if(GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE))
	{
		return false;
	}

	if(GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return false;
	}

	if(eTeam == GetID())
	{
		return false;
	}

	if(isPermanentWarPeace(eTeam) || GET_TEAM(eTeam).isPermanentWarPeace(GetID()))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvTeam::canDeclareWar(TeamTypes eTeam) const
{
	if(eTeam == GetID())
	{
		return false;
	}

	if(!(isAlive()) || !(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if(isAtWar(eTeam))
	{
		return false;
	}

	if(!isHasMet(eTeam))
	{
		return false;
	}

	if(isForcePeace(eTeam))
	{
		return false;
	}

	if(!canChangeWarPeace(eTeam))
	{
		return false;
	}

	if(GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE))
	{
		return false;
	}

	// First, obtain the Lua script system.
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		// Construct and push in some event arguments.
		CvLuaArgsHandle args(2);
		args->Push(GetID());
		args->Push(eTeam);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanDeclareWar", args.get(), bResult))
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

//	-----------------------------------------------------------------------------------------------
void CvTeam::declareWar(TeamTypes eTeam, bool bDefensivePact)
{
	DoDeclareWar(eTeam, bDefensivePact);

	CvPlayerManager::RefreshDangerPlots();
}

//	-----------------------------------------------------------------------------------------------
void CvTeam::DoDeclareWar(TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact)
{
	Localization::String locString;
	int iI;

	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");

	if(isAtWar(eTeam))
	{
		return;
	}

	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");
	if(!isBarbarian())
	{
		// Since we declared war, all of OUR Defensive Pacts are nullified
		cancelDefensivePacts();

		if (!bDefensivePact)
		{
			for(int iAttackingPlayer = 0; iAttackingPlayer < MAX_MAJOR_CIVS; iAttackingPlayer++)
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
							// Forget any of that liberation crud!
							int iNumCitiesLiberated = kDefendingPlayer.GetDiplomacyAI()->GetNumCitiesLiberated(eAttackingPlayer);
							kDefendingPlayer.GetDiplomacyAI()->ChangeNumCitiesLiberated(eAttackingPlayer, -iNumCitiesLiberated);
						}
					}
				}
			}
		}

		// Auto War for Defensive Pacts
		for(iI = 0; iI < MAX_TEAMS; iI++)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
			{
				if(GET_TEAM((TeamTypes)iI).IsHasDefensivePact(eTeam))
				{
					GET_TEAM((TeamTypes)iI).DoDeclareWar(GetID(), /*bDefensivePact*/ true);
				}
			}
		}
	}

	// Cancel Trade Deals
	if(!isBarbarian())
	{
		GC.getGame().GetGameDeals()->DoCancelDealsBetweenTeams(GetID(), eTeam);
		CloseEmbassyAtTeam(eTeam);
		GET_TEAM(eTeam).CloseEmbassyAtTeam(m_eID);
	}

	// Bump Units out of places they shouldn't be
	GC.getMap().verifyUnitValidPlot();

	setAtWar(eTeam, true);
	GET_TEAM(eTeam).setAtWar(GetID(), true);


	// One shot things
	DoNowAtWarOrPeace(eTeam, true);
	GET_TEAM(eTeam).DoNowAtWarOrPeace(GetID(), true);

	// Meet the team if we haven't already
	meet(eTeam, false);

	// Update the ATTACKED players' Diplo AI
	if(!isBarbarian())
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive() && kPlayer.getTeam() == eTeam)
				kPlayer.GetDiplomacyAI()->DoSomeoneDeclaredWarOnMe(GetID());
		}

		// If we've made a peace treaty before, this is bad news (no minors though)
		if(!GET_TEAM(eTeam).isMinorCiv())
		{
			int iPeaceTreatyTurn = GetTurnMadePeaceTreatyWithTeam(eTeam);
			if(iPeaceTreatyTurn != -1)
			{
				int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
				if (iTurnsSincePeace < GC.getPEACE_TREATY_LENGTH())
				{
					SetHasBrokenPeaceTreaty(true);
				}
			}
		}
	}

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
			{
				PlayerTypes ePlayer;
				for(iI = 0; iI < MAX_PLAYERS; iI++)
				{
					ePlayer = (PlayerTypes) iI;

					if(GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).GetNotifications())
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
									DLLUI->AddMessage(0, (ePlayer), true, GC.getEVENT_MESSAGE_TIME(), locString.toUTF8()/*, "AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")*/);
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
							else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam))
							{
								locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_DECLARED_WAR");
								locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
								GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_WAR, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), eTeam);
							}
						}
					}
				}
			}

			locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_DECLARES_WAR");
			locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), locString.toUTF8(), -1, -1);
		}
	}

	int iMinorCivLoop;
	int iMajorCivLoop;

	// Minor Civs declaring war
	if(isMinorCiv())
	{

	}
	// Major is declaring War
	else
	{
		int iMajorCivLoop2;

		// Update what every Major Civ sees
		for(iMajorCivLoop = 0; iMajorCivLoop < MAX_MAJOR_CIVS; iMajorCivLoop++)
		{
			if(GET_PLAYER((PlayerTypes) iMajorCivLoop).getTeam() == GetID())
			{
				if(GET_PLAYER((PlayerTypes) iMajorCivLoop).isAlive())
				{
					// Alter Grand Strategy stats for all Majors in the game whove met this declaring war Major
					for(iMajorCivLoop2 = 0; iMajorCivLoop2 < MAX_MAJOR_CIVS; iMajorCivLoop2++)
					{
						// I don't care if it's me
						if(iMajorCivLoop != iMajorCivLoop2)
						{
							// Have I actually met this player declaring war?
							if(GET_TEAM(GET_PLAYER((PlayerTypes) iMajorCivLoop2).getTeam()).isHasMet(GET_PLAYER((PlayerTypes) iMajorCivLoop).getTeam()))
							{
								GET_PLAYER((PlayerTypes) iMajorCivLoop2).GetDiplomacyAI()->DoPlayerDeclaredWarOnSomeone((PlayerTypes) iMajorCivLoop, eTeam);

								if(!bDefensivePact)
								{
									// Major declaring war on Minor
									if(GET_TEAM(eTeam).isMinorCiv())
									{
										GET_PLAYER((PlayerTypes) iMajorCivLoop2).GetDiplomacyAI()->ChangeOtherPlayerNumMinorsAttacked((PlayerTypes) iMajorCivLoop, 1);
									}
									// Major declaring war on Major
									else
									{
										GET_PLAYER((PlayerTypes) iMajorCivLoop2).GetDiplomacyAI()->ChangeOtherPlayerNumMajorsAttacked((PlayerTypes) iMajorCivLoop, 1, eTeam);
									}
								}
							}
						}
					}

					// Declaring war on Minor
					for(iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
					{
						// Now loop through all players on this team to nullify Quests for them
						if(GET_PLAYER((PlayerTypes) iMinorCivLoop).getTeam() == eTeam)
						{
							if(GET_PLAYER((PlayerTypes) iMinorCivLoop).isAlive())
							{
								// Increment # of Minors this player has attacked - note that this will be called EACH time a team declares war on a Minor,
								// even the same Minor multiple times.  The current design assumes that once a player is at war with a Minor it's forever, so this is fine
								//antonjs: consider: this statement is no longer valid, since current design allows peace to be made; update the implementation
								if(!isMinorCiv() && !bDefensivePact)
								{
									ChangeNumMinorCivsAttacked(1);

									GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->DoTeamDeclaredWarOnMe(GetID());

									// Hand out "we were attacked!" quest
									GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->DoLaunchWarWithMajorQuest(GetID());
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
void CvTeam::DoNowAtWarOrPeace(TeamTypes eTeam, bool bWar)
{
	// Major is at war with a minor
	if(isMinorCiv())
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;

			if(GET_PLAYER(eMinor).getTeam() == GetID())
			{
				if(GET_PLAYER(eMinor).isAlive())
				{
					if(bWar)
						GET_PLAYER(eMinor).GetMinorCivAI()->DoNowAtWarWithTeam(eTeam);
					else
						GET_PLAYER(eMinor).GetMinorCivAI()->DoNowPeaceWithTeam(eTeam);
				}
			}
		}
	}

	if(bWar)
	{
		PlayerTypes eMinor;
		PlayerTypes ePlayer;

		// Loop through players on this team
		for(int iPlayerCivLoop = 0; iPlayerCivLoop < MAX_MAJOR_CIVS; iPlayerCivLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerCivLoop;

			if(!GET_PLAYER(ePlayer).isAlive())
				continue;

			if(GET_PLAYER(ePlayer).getTeam() != GetID())
				continue;

			GET_PLAYER(ePlayer).SetDangerPlotsDirty();
			GET_PLAYER(ePlayer).UpdateReligion();

			// ******************************
			// Our minor civ allies declare war on eTeam
			// ******************************

			FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veMinorAllies;
			for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
			{
				eMinor = (PlayerTypes) iMinorCivLoop;

				// Must be alive
				if(!GET_PLAYER(eMinor).isAlive())
					continue;

				if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
				{
					// Don't declare war on self! (just in case)
					if(GET_PLAYER(eMinor).getTeam() != eTeam)
					{
						// Match war state
						GET_TEAM(GET_PLAYER(eMinor).getTeam()).DoDeclareWar(eTeam, /*bDefensivePact*/ false, /*bMinorAllyPact*/ true);

						// Add to vector for notification sent out
						veMinorAllies.push_back(eMinor);
					}
				}
			}

			// Notifications about minor allies that join the war against a major civ
			if(!veMinorAllies.empty())
			{
				if(!GET_TEAM(eTeam).isMinorCiv())
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
						eMinor = veMinorAllies[iMinorCivLoop];
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
}

//	------------------------------------------------------------------------------------------------
void CvTeam::makePeace(TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification)
{
	DoMakePeace(eTeam, bBumpUnits, bSuppressNotification);
}

//	------------------------------------------------------------------------------------------------
//	The make peace handler, can be called recursively
void CvTeam::DoMakePeace(TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification)
{
	CvString strBuffer;
	int iI;

	CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	CvAssertMsg(eTeam != GetID(), "eTeam is not expected to be equal with GetID()");

	if(isAtWar(eTeam))
	{
		setAtWar(eTeam, false);
		GET_TEAM(eTeam).setAtWar(GetID(), false);

		// One shot things
		DoNowAtWarOrPeace(eTeam, false);
		GET_TEAM(eTeam).DoNowAtWarOrPeace(GetID(), false);

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

					FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL, 0> veMinorAllies;

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
									GET_TEAM(GET_PLAYER(eOurMinor).getTeam()).DoMakePeace(eTeamWeMadePeaceWith, /*bBumpUnits*/ true, /*bSuppressNotification*/ true);
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
				if(pOurPlayer->getTeam() == GetID())
				{
					pOurPlayer->GetDiplomacyAI()->DoWeMadePeaceWithSomeone(eTeam);
					pOurPlayer->GetMilitaryAI()->LogPeace(eTeam);	// This is not quite correct, but it'll work well enough for AI testing
				}
				// Their Team
				else if(pOurPlayer->getTeam() == eTeam)
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
			PlayerTypes ePlayer;
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				ePlayer = (PlayerTypes) iI;

				if(GET_PLAYER(ePlayer).isAlive())
				{
					if(GET_PLAYER(ePlayer).getTeam() == GetID())
					{
						if(GET_PLAYER(ePlayer).GetNotifications())
						{
							locString = Localization::Lookup("TXT_KEY_MISC_YOU_MADE_PEACE_WITH");
							locString << GET_TEAM(eTeam).getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID());
						}
					}
					else if(GET_PLAYER(ePlayer).getTeam() == eTeam)
					{
						if(GET_PLAYER(ePlayer).GetNotifications())
						{
							locString = Localization::Lookup("TXT_KEY_MISC_YOU_MADE_PEACE_WITH");
							locString << getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE_ACTIVE_PLAYER, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID());
						}
					}
					else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetID()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eTeam))
					{
						if(GET_PLAYER(ePlayer).GetNotifications())
						{
							locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_MADE_PEACE");
							locString << getName().GetCString() << GET_TEAM(eTeam).getName().GetCString();
							GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_PEACE, locString.toUTF8(), locString.toUTF8(), -1, -1, this->getLeaderID(), eTeam);
						}
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
		CvWeightedVector<TeamTypes, MAX_CIV_TEAMS, true> veVoteCandidates;
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
					int iOpinion = GET_PLAYER(getLeaderID()).GetDiplomacyAI()->GetMajorCivOpinionWeight(eLeaderLoop);
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
					if (GET_TEAM(eTeamLoop).GetLiberatedByTeam() != GetID())
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
					if (GET_TEAM(eTeamLoop).GetLiberatedByTeam() == GetID())
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
			int iVotesFromUN = /*1*/ GC.getOWN_UNITED_NATIONS_VOTE_BONUS();
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
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvTeam::isObserver() const
{
	int iI;
	for(iI = 0; iI < MAX_PLAYERS; iI++){
		CvPlayer &player = GET_PLAYER((PlayerTypes)iI);
		if(player.getTeam() == GetID() && player.isObserver()){
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


//	--------------------------------------------------------------------------------
bool CvTeam::isMinorCiv() const
{
	bool bValid;
	int iI;

	bValid = false;

	for(iI = MAX_MAJOR_CIVS; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			if(kPlayer.getTeam() == GetID())
			{
				if(kPlayer.isMinorCiv())
				{
					bValid = true;
					break;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return bValid;
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
	if(GetNumMinorCivsAttacked() != iValue)
	{
		m_iNumMinorCivsAttacked = iValue;
	}
}

//	--------------------------------------------------------------------------------
/// Changes the number of Minor Civs this player has declared war on
void CvTeam::ChangeNumMinorCivsAttacked(int iChange)
{
	if(iChange != 0)
		SetNumMinorCivsAttacked(GetNumMinorCivsAttacked() + iChange);
}

//	--------------------------------------------------------------------------------
/// Has this Player been roughing up Minors enough to worry them?
bool CvTeam::IsMinorCivAggressor() const
{
	// Player has attacked enough Minors that they're getting antsy
	if(GetNumMinorCivsAttacked() >= /*2*/ GC.getMINOR_CIV_AGGRESSOR_THRESHOLD())
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
/// Has this player attacked enough Minors to be considered a world threat? (Minors band together and declare war)
bool CvTeam::IsMinorCivWarmonger() const
{
	// Player has attacked enough Minors that an Alliance has formed
	if(GetNumMinorCivsAttacked() >= /*4*/ GC.getMINOR_CIV_WARMONGER_THRESHOLD())
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
// Some diplo stuff
bool CvTeam::IsBrokenMilitaryPromise() const
{
	return m_bBrokenMilitaryPromise;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetBrokenMilitaryPromise(bool bValue)
{
	if(IsBrokenMilitaryPromise() != bValue)
		m_bBrokenMilitaryPromise = bValue;
}

//	--------------------------------------------------------------------------------
bool CvTeam::IsBrokenExpansionPromise() const
{
	return m_bBrokenExpansionPromise;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetBrokenExpansionPromise(bool bValue)
{
	if(IsBrokenExpansionPromise() != bValue)
		m_bBrokenExpansionPromise = bValue;
}

//	--------------------------------------------------------------------------------
bool CvTeam::IsBrokenBorderPromise() const
{
	return m_bBrokenBorderPromise;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetBrokenBorderPromise(bool bValue)
{
	if(IsBrokenBorderPromise() != bValue)
		m_bBrokenBorderPromise = bValue;
}

//	--------------------------------------------------------------------------------
// Broke a promise to not attack a city-state?
bool CvTeam::IsBrokenCityStatePromise() const
{
	return m_bBrokenCityStatePromise;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetBrokenCityStatePromise(bool bValue)
{
	if(IsBrokenCityStatePromise() != bValue)
		m_bBrokenCityStatePromise = bValue;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvTeam::getLeaderID() const
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.isAlive())
		{
			if(thisPlayer.getTeam() == m_eID)
			{
				return ((PlayerTypes)iI);
			}
		}
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if(thisPlayer.getTeam() == m_eID)
		{
			return ((PlayerTypes)iI);
		}
	}

	return NO_PLAYER;
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
		return ((HandicapTypes)(GC.getSTANDARD_HANDICAP()));
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
	return m_iNumMembers;
}


//	--------------------------------------------------------------------------------
void CvTeam::changeNumMembers(int iChange)
{
	m_iNumMembers = (m_iNumMembers + iChange);
	CvAssert(getNumMembers() >= 0);
}


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


//	--------------------------------------------------------------------------------
int CvTeam::GetTradeAgreementTradingAllowedCount() const
{
	return m_iTradeAgreementTradingAllowedCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::IsTradeAgreementTradingAllowed() const
{
	return (GetTradeAgreementTradingAllowedCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvTeam::ChangeTradeAgreementTradingAllowedCount(int iChange)
{
	m_iTradeAgreementTradingAllowedCount = (m_iTradeAgreementTradingAllowedCount + iChange);
	CvAssert(GetTradeAgreementTradingAllowedCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvTeam::getPermanentAllianceTradingCount() const
{
	return m_iPermanentAllianceTradingCount;
}


//	--------------------------------------------------------------------------------
bool CvTeam::isPermanentAllianceTrading() const
{
	return false;
}


//	--------------------------------------------------------------------------------
void CvTeam::changePermanentAllianceTradingCount(int iChange)
{
	m_iPermanentAllianceTradingCount = (m_iPermanentAllianceTradingCount + iChange);
	CvAssert(getPermanentAllianceTradingCount() >= 0);
}

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
				if(GetCurrentEra() >= GC.getLAST_BRIDGE_ART_ERA())
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
					for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
					{
						// Land Unit
						if(pLoopUnit->getDomainType() == DOMAIN_LAND)
						{
							// If we're in friendly territory and we can embark, give the promotion for free
							if(pLoopUnit->plot()->IsFriendlyTerritory((PlayerTypes)iI))
							{
								// Civilian unit or the unit can acquire this promotion
								PromotionTypes ePromotionEmbarkation = GET_PLAYER((PlayerTypes)iI).GetEmbarkationPromotion();
								if(!pLoopUnit->IsCombatUnit() || ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, pLoopUnit->getUnitType()))
									pLoopUnit->setHasPromotion(ePromotionEmbarkation, true);
							}
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
							if(pLoopUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_EMBARKATION()))
							{
								pLoopUnit->setHasPromotion((PromotionTypes)GC.getPROMOTION_EMBARKATION(), false);
								pLoopUnit->setHasPromotion(ePromotionDefensiveEmbarkation, true);
							}
							// Could be cleaner if add "allwater defensive promotion".  Luckily for now the only way you can get
							// both is in the Polynesia scenario and this works for that
							if(pLoopUnit->isHasPromotion((PromotionTypes)GC.getPROMOTION_ALLWATER_EMBARKATION()))
							{
								pLoopUnit->setHasPromotion((PromotionTypes)GC.getPROMOTION_ALLWATER_EMBARKATION(), false);
								pLoopUnit->setHasPromotion(ePromotionDefensiveEmbarkation, true);
							}
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
	int iI;

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
					declareWar(eIndex);
				}
			}
		}

		int iMyPlayersLoop;
		PlayerTypes eMyPlayer;

		int iTheirPlayersLoop;
		PlayerTypes eTheirPlayer;

		int iThirdPlayersLoop;
		PlayerTypes eThirdPlayer;

		// First Contact in Diplo AI (Civ 5)
		for(iMyPlayersLoop = 0; iMyPlayersLoop < MAX_CIV_PLAYERS; iMyPlayersLoop++)
		{
			eMyPlayer = (PlayerTypes) iMyPlayersLoop;

			if(GET_PLAYER(eMyPlayer).isAlive())
			{
				if(GET_PLAYER(eMyPlayer).getTeam() == GetID())
				{
					// Now loop through players on Their team
					for(iTheirPlayersLoop = 0; iTheirPlayersLoop < MAX_CIV_PLAYERS; iTheirPlayersLoop++)
					{
						eTheirPlayer = (PlayerTypes) iTheirPlayersLoop;

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
									if(!isMinorCiv())
									{
										GET_PLAYER(eMyPlayer).GetDiplomacyAI()->DoFirstContact(eTheirPlayer);
									}

									// THIRD party loop - let everyone else know that someone met someone!
									for(iThirdPlayersLoop = 0; iThirdPlayersLoop < MAX_CIV_PLAYERS; iThirdPlayersLoop++)
									{
										eThirdPlayer = (PlayerTypes) iThirdPlayersLoop;

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
			for(iI = 0; iI < MAX_PLAYERS; iI++)
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
			if(pCap)
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
			if(!isMinorCiv())
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
bool CvTeam::isAtWar(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abAtWar[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::setAtWar(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex != GetID() || bNewValue == false, "Team is setting war with itself!");
	if(eIndex != GetID() || bNewValue == false)
		m_abAtWar[eIndex] = bNewValue;

	gDLL->GameplayWarStateChanged(GetID(), eIndex, bNewValue);

#ifndef FINAL_RELEASE
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iMinorLoop);
		if(kPlayer.getTeam() == eIndex && kPlayer.isAlive())
			CvAssertMsg(GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->GetAlly() != getLeaderID(), "Major civ is now at war with a minor it is allied with! This is dumb and bad. If you didn't do this on purpose, please send Jon this along with your last 5 autosaves and a changelist #.");
	}
#endif
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
	CvAssertMsg(eTeam >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(iValue >= 0, "Num turns locked into war should always be 0 or greater. Please show Jon this and send your last 5 autosaves and what changelist # you were playing.");
	CvAssertMsg(eTeam != GetID() || iValue == 0, "Team is setting locked war turns with itself!");
	if(eTeam != GetID() || iValue == 0)
		m_aiNumTurnsLockedIntoWar[eTeam] = iValue;
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
bool CvTeam::IsHasBrokenPeaceTreaty() const
{
	return m_bHasBrokenPeaceTreaty;
}

//	--------------------------------------------------------------------------------
void CvTeam::SetHasBrokenPeaceTreaty(bool bValue)
{
	m_bHasBrokenPeaceTreaty = bValue;
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
	SetHasResearchAgreement(m_eID, false);
	GET_TEAM(eIndex).SetHasResearchAgreement(m_eID, false);
}

//	--------------------------------------------------------------------------------
bool CvTeam::HasEmbassyAtTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

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
bool CvTeam::IsAllowsOpenBordersToTeam(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(GetLiberatedByTeam() == eIndex && !isAtWar(eIndex))
	{
		return true;
	}

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

	if(IsHasDefensivePact(eIndex) != bNewValue)
	{
		m_abDefensivePact[eIndex] = bNewValue;

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}

		if(bNewValue && !GET_TEAM(eIndex).IsHasDefensivePact(GetID()))
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
bool CvTeam::IsHasTradeAgreement(TeamTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abTradeAgreement[eIndex];
}


//	--------------------------------------------------------------------------------
void CvTeam::SetHasTradeAgreement(TeamTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsHasTradeAgreement(eIndex) != bNewValue)
	{
		m_abTradeAgreement[eIndex] = bNewValue;

		if((GetID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			DLLUI->setDirty(Score_DIRTY_BIT, true);
		}

		if(bNewValue && !GET_TEAM(eIndex).IsHasTradeAgreement(GetID()))
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_PLAYERS_SIGN_TRADE_AGREEMENT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
		}
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

	if(iBestRouteValue > -1)
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
	CvString strBuffer;
	bool bChangeProduction;
	int iOldProjectCount;
	int iI, iJ;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
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
				CvString strSomeoneCompletesProject = GetLocalizedText("TXT_KEY_MISC_COMPLETES_PROJECT", getName().GetCString(), pkProject->GetTextKey());
				CvString strSomeoneCompletedProject = GetLocalizedText("TXT_KEY_MISC_SOMEONE_HAS_COMPLETED", getName().GetCString(), pkProject->GetTextKey());
				CvString strUnknownCompletesProject = GetLocalizedText("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN", pkProject->GetTextKey());

				const PlayerTypes eTeamLeader = getLeaderID();
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eTeamLeader, strSomeoneCompletesProject);

				CvPlayerAI& playerWhoLeadsTeam = GET_PLAYER(eTeamLeader);
				CvCity* pLeadersCapital = playerWhoLeadsTeam.getCapitalCity();


				for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
				{
					const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
					CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

					if(kPlayer.isAlive())
					{
						if(isHasMet(kPlayer.getTeam()))
						{
							if(ePlayer == GC.getGame().getActivePlayer())
							{
								DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), strSomeoneCompletedProject);
							}
							CvNotifications* pNotifications = kPlayer.GetNotifications();
							pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strSomeoneCompletedProject, strSomeoneCompletedProject, pLeadersCapital->getX(), pLeadersCapital->getY(), eIndex, playerWhoLeadsTeam.GetID());
						}
						else
						{
							if(ePlayer == GC.getGame().getActivePlayer())
							{
								DLLUI->AddCityMessage(0, pLeadersCapital->GetIDInfo(), ePlayer, false, GC.getEVENT_MESSAGE_TIME(), strUnknownCompletesProject);
							}
							CvNotifications* pNotifications = kPlayer.GetNotifications();
							pNotifications->Add(NOTIFICATION_PROJECT_COMPLETED, strUnknownCompletesProject, strUnknownCompletesProject, -1, -1, eIndex, NO_PLAYER);
						}
					}
				}
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
								if((YieldTypes)k == YIELD_CULTURE)
								{
									pLoopCity->ChangeJONSCulturePerTurnFromBuildings(thisBuildingEntry->GetTechEnhancedYieldChange(k) * iChange);
								}
								else if((YieldTypes)k == YIELD_FAITH)
								{
									pLoopCity->ChangeFaithPerTurnFromBuildings(thisBuildingEntry->GetTechEnhancedYieldChange(k) * iChange);
								}
								else
								{
									pLoopCity->ChangeBaseYieldRateFromBuildings(((YieldTypes)k), thisBuildingEntry->GetTechEnhancedYieldChange(k) * iChange);
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
						DLLUI->AddMessage(0, ((PlayerTypes)iJ), false, GC.getEVENT_MESSAGE_TIME(), strBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MAJOR_EVENT);
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
				DLLUI->AddMessage(0, ePlayer, false, (bSound ? GC.getEVENT_MESSAGE_TIME() : -1), strBuffer/*, (bSound ? GC.getTechInfo(eIndex)->GetSoundMP() : NULL), MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_TECH_TEXT")*/);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce)
{
	CvCity* pCapitalCity;
	CvCity* pCity;
	CvPlot* pLoopPlot;
	CvString strBuffer;
	UnitTypes eFreeUnit;
	bool bFirstResource;
	int iI;

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
		CvPlayerAI& kResearchingPlayer = GET_PLAYER(ePlayer);

		if(	GC.getGame().getActivePlayer() == ePlayer &&
			strcmp(pkTechInfo->GetType(), "TECH_SATELLITES") == 0 &&
			strcmp(kResearchingPlayer.getCivilizationTypeKey(), "CIVILIZATION_HUNS") == 0 &&
			strcmp(GC.getMap().getWorldInfo().GetType(), "WORLDSIZE_HUGE") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_30);
		}

		if(pkTechInfo->IsRepeat())
		{
			GetTeamTechs()->IncrementTechCount(eIndex);

			GetTeamTechs()->SetResearchProgress(eIndex, 0, ePlayer);

			// Repeating techs are good for score!
			int iScoreChange = /*10*/ GC.getSCORE_FUTURE_TECH_MULTIPLIER();
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					GET_PLAYER((PlayerTypes)iI).ChangeScoreFromFutureTech(iScoreChange);
				}
			}

			// report event to Python
			////gDLL->getEventReporterIFace()->techAcquired(eIndex, GetID(), ePlayer, bAnnounce && 1 == GetTeamTechs()->GetTechCount(eIndex));

			if(1 == GetTeamTechs()->GetTechCount(eIndex))
			{
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

			// Tech progress affects city strength, so update
			CvCity* pLoopCity;
			int iLoop;

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					for(pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
					{
						pLoopCity->updateStrengthValue();
					}
				}
			}

			ResourceTypes eResource = NO_RESOURCE;
			NotificationTypes eNotificationType = NO_NOTIFICATION_TYPE;

			for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

				if(pLoopPlot->getResourceType() != NO_RESOURCE)
				{
					if(bNewValue)
					{
						// Resource Reveal
						if(GC.getResourceInfo(pLoopPlot->getResourceType())->getTechReveal() == eIndex)
						{
							// update the resources
							if(pLoopPlot->isRevealed(m_eID))
							{
								if(pLoopPlot->getResourceType(m_eID) != NO_RESOURCE)
								{
									// Create resource link
									if(pLoopPlot->isOwned())
										pLoopPlot->DoFindCityToLinkResourceTo();

									// Only update for active team
									if(GetID() == GC.getGame().getActiveTeam())
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
												eResource = pLoopPlot->getResourceType();
												strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RESOURCE", GC.getResourceInfo(eResource)->GetTextKey());
												CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RESOURCE", GC.getResourceInfo(eResource)->GetTextKey());

												switch(GC.getResourceInfo(eResource)->getResourceUsage())
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
						if(GC.getResourceInfo(pLoopPlot->getResourceType())->getTechCityTrade() == eIndex)
						{
							if(pLoopPlot->isCity() || pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
							{
								// Appropriate Improvement on this Plot?
								if(pLoopPlot->isCity() || GC.getImprovementInfo(pLoopPlot->getImprovementType())->IsImprovementResourceTrade(pLoopPlot->getResourceType()))
								{
									for(iI = 0; iI < MAX_PLAYERS; iI++)
									{
										if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID() && pLoopPlot->getOwner() == (PlayerTypes)iI)
										{
											// We now have a new Tech
											if(bNewValue)
											{
												// slewis - added in so resources wouldn't be double counted when the minor civ researches the technology
												if (!(GET_PLAYER((PlayerTypes)iI).isMinorCiv() && pLoopPlot->IsImprovedByGiftFromMajor()))
												{
													GET_PLAYER((PlayerTypes)iI).changeNumResourceTotal(pLoopPlot->getResourceType(), pLoopPlot->getNumResourceForPlayer((PlayerTypes)iI));
												}

												// Reconnect resource link
												if(pLoopPlot->GetResourceLinkedCity() != NULL)
												{
													pLoopPlot->SetResourceLinkedCityActive(true);
												}
												// Create resource link
												else
												{
													pLoopPlot->DoFindCityToLinkResourceTo();
												}
											}
											// Removing Tech
											else
											{
												GET_PLAYER((PlayerTypes)iI).changeNumResourceTotal(pLoopPlot->getResourceType(), -pLoopPlot->getNumResourceForPlayer((PlayerTypes)iI));

												// Disconnect resource link
												if(pLoopPlot->GetResourceLinkedCity() != NULL)
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
		}

		processTech(eIndex, ((bNewValue) ? 1 : -1));

		if(GetTeamTechs()->HasTech(eIndex))
		{
			EraTypes eThisTechsEra = (EraTypes) pkTechInfo->GetEra();

			// Era change for this Team?
			if(GetCurrentEra() < eThisTechsEra)
			{
				EraTypes eNewEra = (EraTypes) pkTechInfo->GetEra();
				//int iEraChange = eNewEra - GetCurrentEra();

				//int iVictoryPointChange = iEraChange * /*1*/ GC.getVICTORY_POINTS_PER_ERA();
				//changeVictoryPoints(iVictoryPointChange);

				//strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ERA_VICTORY_POINTS", iVictoryPointChange);
				//CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ERA_VICTORY_POINTS");
				//AddNotification(NOTIFICATION_VICTORY, strBuffer, strSummary);

				SetCurrentEra(eNewEra);
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

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == GetID())
				{
					if(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->IsResearchingTech(eIndex))
					{
						GET_PLAYER((PlayerTypes)iI).popResearch(eIndex);
					}

					// notify the player they now have the tech, if they want to make immediate changes
					//GET_PLAYER((PlayerTypes)iI).AI_nowHasTech(eIndex);

					GET_PLAYER((PlayerTypes)iI).invalidateYieldRankCache();
				}
			}

			if(bFirst)
			{
				if(GC.getGame().countKnownTechNumTeams(eIndex) == 1)
				{
					if(pkTechInfo->GetFirstFreeUnitClass() != NO_UNITCLASS)
					{
						bFirstResource = true;

						eFreeUnit = ((UnitTypes)(GET_PLAYER(ePlayer).getCivilizationInfo().getCivilizationUnits(GC.getTechInfo(eIndex)->GetFirstFreeUnitClass())));

						if(eFreeUnit != NULL)
						{
							pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

							if(pCapitalCity != NULL)
							{
								pCapitalCity->GetCityCitizens()->DoSpawnGreatPerson(eFreeUnit, true, false);
							}
						}
					}

					if(pkTechInfo->GetFirstFreeTechs() > 0)
					{
						bFirstResource = true;

						if(!isHuman())
						{
							for(iI = 0; iI < pkTechInfo->GetFirstFreeTechs(); iI++)
							{
								GET_PLAYER(ePlayer).AI_chooseFreeTech();
							}
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_FIRST_TECH_CHOOSE_FREE", pkTechInfo->GetTextKey());
							GET_PLAYER(ePlayer).chooseTech(GC.getTechInfo(eIndex)->GetFirstFreeTechs(), strBuffer.GetCString());
						}

						for(iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if(GET_PLAYER((PlayerTypes)iI).isAlive() && GC.getGame().getActivePlayer())
							{
								if(isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getNameKey(), pkTechInfo->GetTextKey());
								}
								else
								{
									strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_FIRST_TO_TECH", GC.getTechInfo(eIndex)->GetTextKey());
								}
								DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")*/);
							}
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getName(), GC.getTechInfo(eIndex)->GetTextKey());
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, ePlayer, strBuffer, -1, -1);
					}

					if(bFirstResource)
					{
						for(iI = 0; iI < MAX_PLAYERS; iI++)
						{
							CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
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
					for(iI = 0; iI < GC.getMap().numPlots(); iI++)
					{
						pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

						if(pLoopPlot->getTeam() == GetID() && pLoopPlot->getOwner() == GC.getGame().getActivePlayer())
						{
							ResourceTypes eResource = pLoopPlot->getResourceType();

							if(eResource != NO_RESOURCE)
							{
								if(GC.getResourceInfo(eResource)->getTechReveal() == eIndex && !isForceRevealedResource(eResource))
								{
									pCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), NO_PLAYER, GetID(), false);

									if(pCity != NULL)
									{
										strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_DISCOVERED_RESOURCE", GC.getResourceInfo(eResource)->GetTextKey(), pCity->getNameKey());
										DLLUI->AddPlotMessage(0, pLoopPlot->GetPlotIndex(), pLoopPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_DISCOVERRESOURCE", MESSAGE_TYPE_INFO, GC.getResourceInfo(eResource)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopPlot->getX(), pLoopPlot->getY(), true, true*/);
									}
								}
							}
						}
					}

					// Cities demand a Resource that's been newly revealed
					CvCity* pLoopCity;
					int iLoop;
					PlayerTypes eLoopPlayer;
					ResourceTypes eResourceDemanded;

					// Check all players on this team
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(eLoopPlayer).getTeam() == GetID() && eLoopPlayer == GC.getGame().getActivePlayer())
						{
							// Look at all Cities
							for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
							{
								eResourceDemanded = pLoopCity->GetResourceDemanded();

								if(eResourceDemanded != NO_RESOURCE)
								{
									// Resource being demanded is revealed by this Tech
									if(GC.getResourceInfo(eResourceDemanded)->getTechReveal() == eIndex)
									{
										strBuffer = GetLocalizedText("TXT_KEY_MISC_RESOURCE_DISCOVERED_CITY_DEMANDS", GC.getResourceInfo(eResourceDemanded)->GetTextKey(), pLoopCity->getNameKey());
										DLLUI->AddCityMessage(0, pLoopCity->GetIDInfo(), eLoopPlayer, false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_DISCOVERRESOURCE", MESSAGE_TYPE_INFO, GC.getResourceInfo(eResourceDemanded)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopCity->getX(), pLoopCity->getY(), true, true*/);
									}
								}
							}
						}
					}
				}
			}

			for(iI = 0; iI < MAX_TEAMS; iI++)
			{
				if(GET_TEAM((TeamTypes)iI).isAlive())
				{
					if(iI != GetID())
					{
						GET_TEAM((TeamTypes)iI).updateTechShare(eIndex);
					}
				}
			}

			// Cities that are owed a culture building are granted a culture building
			CvCity* pLoopCity;
			int iLoop;
			PlayerTypes eLoopPlayer;

			// Check all players on this team
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(eLoopPlayer).getTeam() == GetID())
				{
					// Look at all Cities
					for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
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
				GET_PLAYER((PlayerTypes) iPlayerLoop).DoUpdateHappiness();
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
				if(bDontShowRewardPopup || GC.getGame().isNetworkMultiPlayer())
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
		int iFreeMoves = GC.getCIRCUMNAVIGATE_FREE_MOVES();
		if(iFreeMoves != 0)
			changeExtraMoves(DOMAIN_SEA, GC.getCIRCUMNAVIGATE_FREE_MOVES());

		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive())
			{
				if(eTeamID == kPlayer.getTeam())
				{
					if(!kGame.isGameMultiPlayer() && kPlayer.isHuman())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_ROUND_WORLD);
					}
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
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer);
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), strBuffer, -1, -1);
	}
}

//	--------------------------------------------------------------------------------
void CvTeam::processTech(TechTypes eTech, int iChange)
{
	CvCity* pCity;
	CvPlot* pLoopPlot;
	ResourceTypes eResource;
	int iI, iJ;

	CvTechEntry* pTech = GC.getTechInfo(eTech);

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
		ChangeResearchAgreementTradingAllowedCount(iChange);
	}

	if(pTech->IsTradeAgreementTradingAllowed())
	{
		ChangeTradeAgreementTradingAllowedCount(iChange);
	}

	if(pTech->IsPermanentAllianceTrading())
	{
		changePermanentAllianceTradingCount(iChange);
	}

	if(pTech->IsBridgeBuilding())
	{
		changeBridgeBuildingCount(iChange);
	}

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

			// Free promotion from this tech?
			for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
			{
				PromotionTypes ePromotion = (PromotionTypes) iPromotion;
				if(pTech->IsFreePromotion(ePromotion))
					kPlayer.ChangeFreePromotionCount(ePromotion, iChange);
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
				CvCivilizationInfo& playerCivilization = kPlayer.getCivilizationInfo();
				eLoopUnit = (UnitTypes)playerCivilization.getCivilizationUnits(iUnitClass);
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
		}
	}

	for(iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		eResource = pLoopPlot->getResourceType();

		if(eResource != NO_RESOURCE)
		{
			if(GC.getResourceInfo(eResource)->getTechReveal() == eTech)
			{
				pLoopPlot->updateYield();
				if(pLoopPlot->isRevealed(m_eID))
				{
					pLoopPlot->setLayoutDirty(true);
				}
			}
		}
	}

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
				if(IsHasDefensivePact(eTeam))
				{
					SetHasDefensivePact(eTeam, false);
					kTeam.SetHasDefensivePact(GetID(), false);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvTeam::isFriendlyTerritory(TeamTypes eTeam) const
{
	if(eTeam == NO_TEAM)
	{
		return false;
	}

	if(eTeam == GetID())
	{
		return true;
	}

	return false;
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
					if (!GC.getGame().isNetworkMultiPlayer() && isHuman() && GetID() == GC.getGame().getActiveTeam()){
						CvPopupInfo kPopupInfo(BUTTONPOPUP_NEW_ERA, eNewValue);
						DLLUI->AddPopup(kPopupInfo);
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
						if (kPlayer.GetEspionage()->GetNumSpies() == 0)
						{
							int iNumTraitSpies = kPlayer.GetPlayerTraits()->GetExtraSpies();
							for (int i = 0; i < iNumTraitSpies; i++)
							{
								kPlayer.GetEspionage()->CreateSpy();
							}
						}

						for(int i = 0; i < pEraInfo->getSpiesGrantedForPlayer(); i++)
						{
							kPlayer.GetEspionage()->CreateSpy();
						}
					}
				}
			}

			if(pEraInfo->getSpiesGrantedForEveryone() > 0)
			{
				// check to see if anyone else has reached or surpassed this era yet
				bool bAlreadyProvided = false;
				PlayerTypes ePlayer;
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
							if (kPlayer.GetEspionage()->GetNumSpies() == 0)
							{
								int iNumTraitSpies = kPlayer.GetPlayerTraits()->GetExtraSpies();
								for (int iIndex = 0; iIndex < iNumTraitSpies; iIndex++)
								{
									kPlayer.GetEspionage()->CreateSpy();
								}
							}
							kPlayer.GetEspionage()->CreateSpy();
						}
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

		PlayerTypes ePlayer;

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			ePlayer = (PlayerTypes) iPlayerLoop;

			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == GetID())
			{
				gDLL->GameplayEraChanged(ePlayer, eNewValue);
			}
		}

		if(GC.getGame().getActiveTeam() == GetID() && isBridgeBuilding() && eNewValue >= GC.getLAST_BRIDGE_ART_ERA())
		{
			gDLL->GameplayBridgeChanged(true, 1);
		}

		if(GetID() == GC.getGame().getActiveTeam())
		{
			DLLUI->setDirty(Soundtrack_DIRTY_BIT, true);
		}
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
void CvTeam::Read(FDataStream& kStream)
{
	// Init data before load
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iNumMembers;
	kStream >> m_iAliveCount;
	kStream >> m_iEverAliveCount;
	kStream >> m_iNumCities;
	kStream >> m_iTotalPopulation;
	kStream >> m_iTotalLand;
	kStream >> m_iNukeInterception;
	kStream >> m_iExtraWaterSeeFromCount;
	kStream >> m_iMapTradingCount;
	kStream >> m_iTechTradingCount;
	kStream >> m_iGoldTradingCount;
	if(uiVersion >= 12)
	{
		kStream >> m_iAllowEmbassyTradingAllowedCount;
	}
	kStream >> m_iOpenBordersTradingAllowedCount;
	kStream >> m_iDefensivePactTradingAllowedCount;
	kStream >> m_iResearchAgreementTradingAllowedCount;
	kStream >> m_iTradeAgreementTradingAllowedCount;
	kStream >> m_iPermanentAllianceTradingCount;
	kStream >> m_iBridgeBuildingCount;
	kStream >> m_iWaterWorkCount;
	kStream >> m_iRiverTradeCount;
	kStream >> m_iBorderObstacleCount;
	kStream >> m_iVictoryPoints;
	kStream >> m_iEmbarkedExtraMoves;
	kStream >> m_iCanEmbarkCount;
	if(uiVersion >= 8)
	{
		kStream >> m_iDefensiveEmbarkCount;
	}
	else
	{
		m_iDefensiveEmbarkCount = 0;
	}
	kStream >> m_iEmbarkedAllWaterPassageCount;
	kStream >> m_iNumNaturalWondersDiscovered;
	kStream >> m_iBestPossibleRoute;
	kStream >> m_iNumMinorCivsAttacked;

	kStream >> m_bMapCentering;
	kStream >> m_bHasBrokenPeaceTreaty;
	kStream >> m_bHomeOfUnitedNations;

	kStream >> m_bBrokenMilitaryPromise;
	kStream >> m_bBrokenExpansionPromise;
	kStream >> m_bBrokenBorderPromise;
	kStream >> m_bBrokenCityStatePromise;

	kStream >> m_eID;

	kStream >> m_eCurrentEra;
	kStream >> m_eLiberatedByTeam;
	kStream >> m_eKilledByTeam;

	ArrayWrapper<int> kTechShareCountWrapper(MAX_TEAMS, &m_aiTechShareCount[0]);
	kStream >> kTechShareCountWrapper;

	ArrayWrapper<int> kNumTurnsAtWarWrapper(MAX_TEAMS, &m_aiNumTurnsAtWar[0]);
	kStream >> kNumTurnsAtWarWrapper;

	if(uiVersion >= 3)
	{
		ArrayWrapper<int> kNumTurnsLockedIntoWarWrapper(MAX_TEAMS, &m_aiNumTurnsLockedIntoWar[0]);
		kStream >> kNumTurnsLockedIntoWarWrapper;
	}
	else
	{
		for(int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
			m_aiNumTurnsLockedIntoWar[iTeamLoop] = 0;
	}

	if(uiVersion >= 13)
	{
		ArrayWrapper<int> kExtraMovesWrapper(NUM_DOMAIN_TYPES, &m_aiExtraMoves[0]);
		kStream >> kExtraMovesWrapper;
	}
	else
	{
		int iOldNumDomainTypes = 4; // Before version 13, NUM_DOMAIN_TYPES was 4
		ArrayWrapper<int> kExtraMovesWrapper(iOldNumDomainTypes, &m_aiExtraMoves[0]);
		kStream >> kExtraMovesWrapper;

		// Set any new DomainTypes to 0
		for (int iI = iOldNumDomainTypes; iI < NUM_DOMAIN_TYPES; iI++)
		{
			m_aiExtraMoves[iI] = 0;
		}
	}

	ArrayWrapper<int> kForceTeamVoteWrapper(GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	kStream >> kForceTeamVoteWrapper;

	ArrayWrapper<int> kTurnMadePeaceWrapper(MAX_TEAMS, &m_paiTurnMadePeaceTreatyWithTeam[0]);
	kStream >> kTurnMadePeaceWrapper;

	ArrayWrapper<bool> kHasMetWrapper(MAX_TEAMS, &m_abHasMet[0]);
	kStream >> kHasMetWrapper;

	ArrayWrapper<bool> kHasFoundPlayerWrapper(MAX_PLAYERS, &m_abHasFoundPlayersTerritory[0]);
	kStream >> kHasFoundPlayerWrapper;

	ArrayWrapper<bool> kAtWarWrapper(MAX_TEAMS, &m_abAtWar[0]);
	kStream >> kAtWarWrapper;

	ArrayWrapper<bool> kPermanentWarWrapper(MAX_TEAMS, &m_abPermanentWarPeace[0]);
	kStream >> kPermanentWarWrapper;

	if(uiVersion >= 12)
	{
		ArrayWrapper<bool> kEmbassyWrapper(MAX_TEAMS, &m_abEmbassy[0]);
		kStream >> kEmbassyWrapper;
	}
	else
	{
		for(int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
		{
			m_abEmbassy[iTeamLoop] = false;
		}
	}

	ArrayWrapper<bool> kOpenBordersWrapper(MAX_TEAMS, &m_abOpenBorders[0]);
	kStream >> kOpenBordersWrapper;

	ArrayWrapper<bool> kDefensivePactWrapper(MAX_TEAMS, &m_abDefensivePact[0]);
	kStream >> kDefensivePactWrapper;

	ArrayWrapper<bool> kResearchAgreementWrapper(MAX_TEAMS, &m_abResearchAgreement[0]);
	kStream >> kResearchAgreementWrapper;

	ArrayWrapper<bool> kTradeAgreementWrapper(MAX_TEAMS, &m_abTradeAgreement[0]);
	kStream >> kTradeAgreementWrapper;

	ArrayWrapper<bool> kForcePeaceWrapper(MAX_TEAMS, &m_abForcePeace[0]);
	kStream >> kForcePeaceWrapper;

	ArrayWrapper<bool> kCanLaunchWrapper(GC.getNumVictoryInfos(), m_abCanLaunch);
	kStream >> kCanLaunchWrapper;

	ArrayWrapper<bool> kVictoryAchievedWrapper(GC.getNumVictoryInfos(), m_abVictoryAchieved);
	kStream >> kVictoryAchievedWrapper;

	ArrayWrapper<bool> kSmallAwardAchievedWrapper(GC.getNumSmallAwardInfos(), m_abSmallAwardAchieved);
	kStream >> kSmallAwardAchievedWrapper;

	ArrayWrapper<int> kRouteChangeWrapper(GC.getNumRouteInfos(), m_paiRouteChange);
	kStream >> kRouteChangeWrapper;

	if (uiVersion >= 14)
	{
		ArrayWrapper<int> kBuildTimeChangeWrapper(GC.getNumBuildInfos(), &m_paiBuildTimeChange[0]);
		kStream >> kBuildTimeChangeWrapper;
	}
	else
	{
		for (int i = 0; i < GC.getNumBuildInfos(); i++)
		{
			m_paiBuildTimeChange[i] = 0;
		}
	}

	ArrayWrapper<int> kProjectCountWrapper(GC.getNumProjectInfos(), m_paiProjectCount);
	kStream >> kProjectCountWrapper;

	ArrayWrapper<int> kProjectArtWrapper(GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);
	kStream >> kProjectArtWrapper;

	//project art types
	for(int i=0; i<GC.getNumProjectInfos(); i++)
	{
		int temp;
		for(int j=0; j<m_paiProjectCount[i]; j++)
		{
			kStream >> temp;
			m_pavProjectArtTypes[i].push_back(temp);
		}
	}

	ArrayWrapper<int> kProjectMakingWrapper(GC.getNumProjectInfos(), m_paiProjectMaking);
	kStream >> kProjectMakingWrapper;

	if(uiVersion >= 11)
	{
		UnitClassArrayHelpers::Read(kStream, m_paiUnitClassCount);
	}
	else
	{
		ArrayWrapper<int> kUnitClassCountWrapper(60, m_paiUnitClassCount);
		kStream >> kUnitClassCountWrapper;
	}

	if(uiVersion >= 7)
	{
		BuildingClassArrayHelpers::Read(kStream, m_paiBuildingClassCount);
	}
	else
	{
		ArrayWrapper<int> kBuilderClassCountWrapper(79, m_paiBuildingClassCount);
		kStream >> kBuilderClassCountWrapper;
	}

	if(uiVersion >= 4)
	{
		BuildingArrayHelpers::Read(kStream, m_paiObsoleteBuildingCount);
	}
	else
	{
		ArrayWrapper<int> kObsoleteBuilderCountWrapper(89, m_paiObsoleteBuildingCount);
		kStream >> kObsoleteBuilderCountWrapper;
	}

	ArrayWrapper<int> kTerrainTradeCount(GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	kStream >> kTerrainTradeCount;

	ArrayWrapper<int> kVictoryCountdownWrapper(GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	kStream >> kVictoryCountdownWrapper;

	ArrayWrapper<int> kTurnTeamMetWrapper(MAX_CIV_TEAMS, &m_aiTurnTeamMet[0]);
	kStream >> kTurnTeamMetWrapper;

	m_pTeamTechs->Read(kStream);

	if(uiVersion >= 10)
	{
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementYieldChange, NUM_YIELD_TYPES);
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementNoFreshWaterYieldChange, NUM_YIELD_TYPES);
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementFreshWaterYieldChange, NUM_YIELD_TYPES);
	}
	else if(uiVersion >= 6)
	{
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementYieldChange, 4);
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementNoFreshWaterYieldChange, 4);
		ImprovementArrayHelpers::ReadYieldArray(kStream, m_ppaaiImprovementFreshWaterYieldChange, 4);
	}
	else
	{
		for(int i = 0; i < 20; ++i)
		{
			ArrayWrapper<int> kImprovementYieldChangeWrapper(4, m_ppaaiImprovementYieldChange[i]);
			kStream >> kImprovementYieldChangeWrapper;

			if(uiVersion >= 2)
			{
				ArrayWrapper<int> kImprovementNoFreshWaterWrapper(4, m_ppaaiImprovementNoFreshWaterYieldChange[i]);
				kStream >> kImprovementNoFreshWaterWrapper;
			}
			else
			{
				for(int iJ = 0; iJ < 4; ++iJ)
				{
					m_ppaaiImprovementNoFreshWaterYieldChange[i][iJ] = 0;
				}
			}

			ArrayWrapper<int> ImprovementFreshWaterWrapper(4, m_ppaaiImprovementFreshWaterYieldChange[i]);
			kStream >> ImprovementFreshWaterWrapper;
		}
	}

	if(uiVersion >= 9)
		CvInfosSerializationHelper::ReadHashedTypeArray(kStream, m_aeRevealedResources);
	else
		CvInfosSerializationHelper::ReadV0TypeArray(kStream, m_aeRevealedResources, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));

	// Fix bad 'at war' flags where we are at war with ourselves.  Not a good thing.
	if(m_eID >= 0 && m_eID < MAX_TEAMS)
	{
		m_abAtWar[m_eID] = false;
		m_aiNumTurnsAtWar[m_eID] = 0;
	}
}


//	--------------------------------------------------------------------------------
void CvTeam::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 14;
	kStream << uiVersion;

	kStream << m_iNumMembers;
	kStream << m_iAliveCount;
	kStream << m_iEverAliveCount;
	kStream << m_iNumCities;
	kStream << m_iTotalPopulation;
	kStream << m_iTotalLand;
	kStream << m_iNukeInterception;
	kStream << m_iExtraWaterSeeFromCount;
	kStream << m_iMapTradingCount;
	kStream << m_iTechTradingCount;
	kStream << m_iGoldTradingCount;
	kStream << m_iAllowEmbassyTradingAllowedCount;
	kStream << m_iOpenBordersTradingAllowedCount;
	kStream << m_iDefensivePactTradingAllowedCount;
	kStream << m_iResearchAgreementTradingAllowedCount;
	kStream << m_iTradeAgreementTradingAllowedCount;
	kStream << m_iPermanentAllianceTradingCount;
	kStream << m_iBridgeBuildingCount;
	kStream << m_iWaterWorkCount;
	kStream << m_iRiverTradeCount;
	kStream << m_iBorderObstacleCount;
	kStream << m_iVictoryPoints;
	kStream << m_iEmbarkedExtraMoves;
	kStream << m_iCanEmbarkCount;
	kStream << m_iDefensiveEmbarkCount;
	kStream << m_iEmbarkedAllWaterPassageCount;
	kStream << m_iNumNaturalWondersDiscovered;
	kStream << m_iBestPossibleRoute;
	kStream << m_iNumMinorCivsAttacked;

	kStream << m_bMapCentering;
	kStream << m_bHasBrokenPeaceTreaty;
	kStream << m_bHomeOfUnitedNations;

	kStream << m_bBrokenMilitaryPromise;
	kStream << m_bBrokenExpansionPromise;
	kStream << m_bBrokenBorderPromise;
	kStream << m_bBrokenCityStatePromise;

	kStream << m_eID;

	kStream << m_eCurrentEra;
	kStream << m_eLiberatedByTeam;
	kStream << m_eKilledByTeam;

	kStream << ArrayWrapperConst<int>(MAX_TEAMS, &m_aiTechShareCount[0]);
	kStream << ArrayWrapperConst<int>(MAX_TEAMS, &m_aiNumTurnsAtWar[0]);
	kStream << ArrayWrapperConst<int>(MAX_TEAMS, &m_aiNumTurnsLockedIntoWar[0]);
	kStream << ArrayWrapperConst<int>(NUM_DOMAIN_TYPES, &m_aiExtraMoves[0]);
	kStream << ArrayWrapperConst<int>(GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	kStream << ArrayWrapperConst<int>(MAX_TEAMS, &m_paiTurnMadePeaceTreatyWithTeam[0]);

	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abHasMet[0]);
	kStream << ArrayWrapperConst<bool>(MAX_PLAYERS, &m_abHasFoundPlayersTerritory[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abAtWar[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abPermanentWarPeace[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abEmbassy[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abOpenBorders[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abDefensivePact[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abResearchAgreement[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abTradeAgreement[0]);
	kStream << ArrayWrapperConst<bool>(MAX_TEAMS, &m_abForcePeace[0]);
	kStream << ArrayWrapperConst<bool>(GC.getNumVictoryInfos(), m_abCanLaunch);
	kStream << ArrayWrapperConst<bool>(GC.getNumVictoryInfos(), m_abVictoryAchieved);
	kStream << ArrayWrapperConst<bool>(GC.getNumSmallAwardInfos(), m_abSmallAwardAchieved);

	kStream << ArrayWrapperConst<int>(GC.getNumRouteInfos(), m_paiRouteChange);
	kStream << ArrayWrapperConst<int>(GC.getNumBuildInfos(), m_paiBuildTimeChange);
	kStream << ArrayWrapperConst<int>(GC.getNumProjectInfos(), m_paiProjectCount);
	kStream << ArrayWrapperConst<int>(GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);

	//project art types
	for(int i=0; i<GC.getNumProjectInfos(); i++)
	{
		for(int j=0; j<m_paiProjectCount[i]; j++)
			kStream << m_pavProjectArtTypes[i][j];
	}

	kStream << ArrayWrapperConst<int>(GC.getNumProjectInfos(), m_paiProjectMaking);

	UnitClassArrayHelpers::Write(kStream, m_paiUnitClassCount, GC.getNumUnitClassInfos());
	BuildingClassArrayHelpers::Write(kStream, m_paiBuildingClassCount, GC.getNumBuildingClassInfos());
	BuildingArrayHelpers::Write(kStream, m_paiObsoleteBuildingCount, GC.getNumBuildingInfos());

	kStream << ArrayWrapperConst<int>(GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	kStream << ArrayWrapperConst<int>(GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	kStream << ArrayWrapperConst<int>(MAX_CIV_TEAMS, &m_aiTurnTeamMet[0]);

	m_pTeamTechs->Write(kStream);

	int iNumImprovements = GC.getNumImprovementInfos();
	ImprovementArrayHelpers::WriteYieldArray(kStream, m_ppaaiImprovementYieldChange, iNumImprovements);
	ImprovementArrayHelpers::WriteYieldArray(kStream, m_ppaaiImprovementNoFreshWaterYieldChange, iNumImprovements);
	ImprovementArrayHelpers::WriteYieldArray(kStream, m_ppaaiImprovementFreshWaterYieldChange, iNumImprovements);

	CvInfosSerializationHelper::WriteHashedTypeArray(kStream, m_aeRevealedResources);
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
