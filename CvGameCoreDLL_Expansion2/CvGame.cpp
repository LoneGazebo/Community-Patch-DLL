/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvGame.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGlobals.h"
#include "CvMapGenerator.h"
#include "CvReplayMessage.h"
#include "CvInfos.h"
#include "CvReplayInfo.h"
#include "CvGameTextMgr.h"
#include "CvSiteEvaluationClasses.h"
#include "CvImprovementClasses.h"
#include "CvStartPositioner.h"
#include "CvTacticalAnalysisMap.h"
#include "CvGrandStrategyAI.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvNotifications.h"
#include "CvAdvisorCounsel.h"
#include "CvAdvisorRecommender.h"
#include "CvWorldBuilderMapLoader.h"
#include "CvTypes.h"

#include "cvStopWatch.h"
#include "CvUnitMission.h"

#include "CvDLLUtilDefines.h"
#include "CvAchievementUnlocker.h"

// interface uses
#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "FAutoVariableBase.h"
#include "CvStringUtils.h"
#include "CvBarbarians.h"
#include "CvGoodyHuts.h"

#include <sstream>

#include "FTempHeap.h"
#include "CvDiplomacyRequests.h"

#include "CvDllPlot.h"
#include "FFileSystem.h"

#include "CvInfosSerializationHelper.h"
#include "CvCityManager.h"

#if defined(MOD_BALANCE_CORE)
#include "CvPlayerManager.h"
#endif
#if defined(MOD_API_LUA_EXTENSIONS)
#include "CvDllContext.h"
#endif

// Public Functions...
// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_GLOBAL_IDS)
	int GetNextGlobalID() { return GC.getGame().GetNextGlobalID(); }
	int GetJonRand(int iRange) { return GC.getGame().getJonRandNum(iRange,"generic"); }
#endif

//------------------------------------------------------------------------------
// CvGame Version History
// Version 1 
//	 * CvGame save version reset for expansion pack 2.
//------------------------------------------------------------------------------
const int g_CurrentCvGameVersion = 1;

//some statistics
int gTactMovesCount[NUM_AI_TACTICAL_MOVES] = { 0 };
int gHomeMovesCount[NUM_AI_HOMELAND_MOVES] = { 0 };

CvGameInitialItemsOverrides::CvGameInitialItemsOverrides()
{
	//By default, all players and teams are granted all
	//free items.
	GrantInitialFreeTechsPerTeam.resize(MAX_TEAMS, true);
	GrantInitialGoldPerPlayer.resize(MAX_PLAYERS, true);
	GrantInitialCulturePerPlayer.resize(MAX_PLAYERS, true);
	ClearResearchQueuePerPlayer.resize(MAX_PLAYERS, true);
	GrantInitialUnitsPerPlayer.resize(MAX_PLAYERS, true);
}

//------------------------------------------------------------------------------
CvGame::CvGame() :
	m_jonRand("GameRng")
	, m_mapRand("PreGameRng")
	, m_endTurnTimer()
	, m_endTurnTimerSemaphore(0)
	, m_curTurnTimer()
	, m_timeSinceGameTurnStart()
	, m_fCurrentTurnTimerPauseDelta(0.f)
	, m_sentAutoMoves(false)
	, m_bForceEndingTurn(false)
	, m_pDiploResponseQuery(NULL)
	, m_bFOW(true)
	, m_bArchaeologyTriggered(false)
	, m_lastTurnAICivsProcessed(-1)
	, m_processPlayerAutoMoves(false)
{
	m_aiEndTurnMessagesReceived = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	m_aiRankPlayer = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);        // Ordered by rank...
	m_aiPlayerRank = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);        // Ordered by player ID...
	m_aiPlayerScore = FNEW(int[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);       // Ordered by player ID...
	m_aiRankTeam = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);						// Ordered by rank...
	m_aiTeamRank = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);						// Ordered by team ID...
	m_aiTeamScore = FNEW(int[MAX_TEAMS], c_eCiv5GameplayDLL, 0);						// Ordered by team ID...

	m_paiUnitCreatedCount = NULL;
	m_paiUnitClassCreatedCount = NULL;
	m_paiBuildingClassCreatedCount = NULL;
	m_paiProjectCreatedCount = NULL;
	m_paiVoteOutcome = NULL;
	m_aiSecretaryGeneralTimer = NULL;
	m_aiVoteTimer = NULL;
	m_aiDiploVote = NULL;
	m_aiVotesCast = NULL;
	m_aiPreviousVotesCast = NULL;
	m_aiNumVotesForTeam = NULL;
	m_aiTeamCompetitionWinnersScratchPad = NULL;
	m_aiGreatestMonopolyPlayer = NULL;

	m_pabSpecialUnitValid = NULL;

	m_ppaaiTeamVictoryRank = NULL;

	m_pSettlerSiteEvaluator = NULL;
	m_pStartSiteEvaluator = NULL;
	m_pStartPositioner = NULL;
	m_pGameReligions = NULL;
	m_pGameCulture = NULL;
	m_pGameLeagues = NULL;
	m_pGameTrade = NULL;

#if defined(MOD_BALANCE_CORE)
	m_pGameCorporations = NULL;
	m_pGameContracts = NULL;
	m_ppaiContractUnits = NULL;
#endif

	m_pAdvisorCounsel = NULL;
	m_pAdvisorRecommender = NULL;

	m_endTurnTimer.Start();
	m_endTurnTimer.Stop();

	reset(NO_HANDICAP, true);
}


//	--------------------------------------------------------------------------------
CvGame::~CvGame()
{
#ifdef VPDEBUG
	FILogFile* pLog=LOGFILEMGR.GetLog( "UnitMoveStats.txt", FILogFile::kDontTimeStamp );
	if (pLog)
	{
		pLog->Msg("#move and count @ turn %d\n",getGameTurn());
		for (int i = 0; i < NUM_AI_TACTICAL_MOVES; i++)
			pLog->Msg(CvString::format("%s:%d\n", tacticalMoveNames[i], gTactMovesCount[i]).c_str());
		for (int i = 0; i < NUM_AI_HOMELAND_MOVES; i++)
			pLog->Msg(CvString::format("%s:%d\n", homelandMoveNames[i], gHomeMovesCount[i]).c_str());
		pLog->Close();
	}
#endif

	uninit();

	SAFE_DELETE_ARRAY(m_aiEndTurnMessagesReceived);
	SAFE_DELETE_ARRAY(m_aiRankPlayer);
	SAFE_DELETE_ARRAY(m_aiPlayerRank);
	SAFE_DELETE_ARRAY(m_aiPlayerScore);
	SAFE_DELETE_ARRAY(m_aiRankTeam);
	SAFE_DELETE_ARRAY(m_aiTeamRank);
	SAFE_DELETE_ARRAY(m_aiTeamScore);
}

//	--------------------------------------------------------------------------------
void CvGame::init(HandicapTypes eHandicap)
{
	bool bValid;
	int iStartTurn;
	int iEstimateEndTurn;
	int iI;

	//--------------------------------
	// Init saved data
	reset(eHandicap);

	m_mapRand.init(CvPreGame::mapRandomSeed() % 73637381);
	m_jonRand.init(CvPreGame::syncRandomSeed() % 52319761);

	//--------------------------------
	// Verify pregame data

	//Validate game era. If we lack the era info for the current era, work backwards until we find a valid one.
	if(!GC.getEraInfo(getStartEra())){
		for(int eraIdx = ((int)CvPreGame::era())-1; eraIdx >= 0; --eraIdx){
			CvEraInfo* curEraInfo = GC.getEraInfo((EraTypes)eraIdx);
			if(curEraInfo){
				CvPreGame::setEra((EraTypes)eraIdx);
				break;
			}
		}
	}

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data

	// Turn off all MP options if it's a single player game
	GameTypes g = CvPreGame::gameType();
	if(g == GAME_SINGLE_PLAYER)
	{
		for(iI = 0; iI < NUM_MPOPTION_TYPES; ++iI)
		{
			setMPOption((MultiplayerOptionTypes)iI, false);
		}

		setOption(GAMEOPTION_SIMULTANEOUS_TURNS, false);
		setOption(GAMEOPTION_DYNAMIC_TURNS, false);
		setOption(GAMEOPTION_PITBOSS, false);
	}

	// If this is a hot seat game, simultaneous turns is always off
	if(isHotSeat() || isPbem())
	{
		setOption(GAMEOPTION_SIMULTANEOUS_TURNS, false);
		setOption(GAMEOPTION_DYNAMIC_TURNS, false);
		setOption(GAMEOPTION_PITBOSS, false);
	}

	if(isMPOption(MPOPTION_SHUFFLE_TEAMS))
	{
		int aiTeams[MAX_CIV_PLAYERS];

		int iNumPlayers = 0;
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if(CvPreGame::slotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				aiTeams[iNumPlayers] = CvPreGame::teamType((PlayerTypes)i);
				++iNumPlayers;
			}
		}

		for(int i = 0; i < iNumPlayers; i++)
		{
			int j = (getJonRandNum(iNumPlayers - i, NULL) + i);

			if(i != j)
			{
				int iTemp = aiTeams[i];
				aiTeams[i] = aiTeams[j];
				aiTeams[j] = iTemp;
			}
		}

		iNumPlayers = 0;
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if(CvPreGame::slotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				CvPreGame::setTeamType((PlayerTypes)i, (TeamTypes)aiTeams[iNumPlayers]);
				++iNumPlayers;
			}
		}
	}

	if(isOption(GAMEOPTION_LOCK_MODS))
	{
		if(isGameMultiPlayer())
		{
			setOption(GAMEOPTION_LOCK_MODS, false);
		}
		else
		{
			static const int iPasswordSize = 8;
			char szRandomPassword[iPasswordSize];
			for(int i = 0; i < iPasswordSize-1; i++)
			{
				szRandomPassword[i] = getJonRandNum(128, "Random Keyword");
			}
			szRandomPassword[iPasswordSize-1] = 0;

			CvString strRandomPassword = szRandomPassword;
			CvPreGame::setAdminPassword(strRandomPassword);
		}
	}

	const CvGameSpeedInfo& kGameSpeedInfo = getGameSpeedInfo();
	if(getGameTurn() == 0)
	{
		iStartTurn = 0;

		for(iI = 0; iI < kGameSpeedInfo.getNumTurnIncrements(); iI++)
		{
			iStartTurn += kGameSpeedInfo.getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
		}

		const CvEraInfo& kEraInfo = getStartEraInfo();

		iStartTurn *= kEraInfo.getStartPercent();
		iStartTurn /= 100;

		setGameTurn(iStartTurn);

		if (kEraInfo.isNoReligion())
		{
			CvPreGame::SetGameOption(GAMEOPTION_NO_RELIGION, true);
		}
	}

	setStartTurn(getGameTurn());

	iEstimateEndTurn = 0;

	for(iI = 0; iI < kGameSpeedInfo.getNumTurnIncrements(); iI++)
	{
		iEstimateEndTurn += kGameSpeedInfo.getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
	}

	setDefaultEstimateEndTurn(iEstimateEndTurn);

	if(getMaxTurns() == 0)
	{

		setEstimateEndTurn(iEstimateEndTurn);

		if(getEstimateEndTurn() > getGameTurn())
		{
			bValid = false;

			for(iI = 0; iI < GC.getNumVictoryInfos(); iI++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iI);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(isVictoryValid(eVictory))
					{
						if(pkVictoryInfo->isEndScore())
						{
							bValid = true;
							break;
						}
					}
				}

			}

			if(bValid)
			{
				setMaxTurns(getEstimateEndTurn() - getGameTurn());
			}
		}
	}
	else
	{
		setEstimateEndTurn(getGameTurn() + getMaxTurns());
	}

	setStartYear(GC.getSTART_YEAR());

	for(iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		SpecialUnitTypes eSpecialUnit = static_cast<SpecialUnitTypes>(iI);
		CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo(eSpecialUnit);
		if(pkSpecialUnitInfo)
		{
			if(pkSpecialUnitInfo->isValid())
			{
				makeSpecialUnitValid(eSpecialUnit);
			}
		}
	}

	if(isOption(GAMEOPTION_QUICK_COMBAT))
	{
		CvPreGame::setQuickCombat(true);
	}

	m_bArchaeologyTriggered = false;
	CvGoodyHuts::Reset();

	doUpdateCacheOnTurn();

#if defined(MOD_BALANCE_CORE_HAPPINESS)
	if(MOD_BALANCE_CORE_HAPPINESS)
	{
		updateGlobalAverage();
	}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	SetHighestPotential();
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	{
		DoBarbCountdown();
	}
#endif
}

//	--------------------------------------------------------------------------------
bool CvGame::init2()
{
	InitPlayers();

	CvGameInitialItemsOverrides kItemOverrides;
	if(!InitMap(kItemOverrides))
	{
		return false;
	}

	initDiplomacy();
	setInitialItems(kItemOverrides);

	if(CvPreGame::isWBMapScript() && !CvPreGame::mapNoPlayers())
	{
		CvWorldBuilderMapLoader::SetInitialItems(false);
	}

	CheckGenerateArchaeology();

	initScoreCalculation();
	setFinalInitialized(true);

#if defined(MOD_EVENTS_TERRAFORMING)
	if (MOD_EVENTS_TERRAFORMING) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingMap, TERRAFORMINGEVENT_LOAD, 0);
	}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		int iNumResourceInfos= GC.getNumResourceInfos();
		for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo)
			{
				GC.getMap().setNumResources(eResource);
			}
		}
	}
#endif

	return true;
}

//------------------------------------------------------------------------------
// Lua Hooks
// These are static functions to hook into Lua and relay information to the DLL.
//------------------------------------------------------------------------------
int WorldBuilderMapLoaderAddRandomItems(lua_State* L)
{
	return CvWorldBuilderMapLoader::AddRandomItems(L);
}
//------------------------------------------------------------------------------
int WorldBuilderMapLoaderLoadModData(lua_State* L)
{
	return CvWorldBuilderMapLoader::LoadModData(L);
}
//------------------------------------------------------------------------------
int WorldBuilderMapLoaderRunPostProcessScript(lua_State* L)
{
	return CvWorldBuilderMapLoader::RunPostProcessScript(L);
}

void SetAllPlotsVisible(TeamTypes eTeam)
{
	if (eTeam != NO_TEAM)
	{
		const int iNumInvisibleInfos = NUM_INVISIBLE_TYPES;
		for (int plotID = 0; plotID < GC.getMap().numPlots(); plotID++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(plotID);
			pLoopPlot->changeVisibilityCount(eTeam, pLoopPlot->getVisibilityCount(eTeam) + 1, NO_INVISIBLE, true, false);
			pLoopPlot->changeInvisibleVisibilityCountUnit(eTeam, pLoopPlot->getInvisibleVisibilityCountUnit(eTeam) + 1);
			for (int iJ = 0; iJ < iNumInvisibleInfos; iJ++)
			{
				pLoopPlot->changeInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ), pLoopPlot->getInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ)) + 1);
			}

			pLoopPlot->setRevealed(eTeam, true, false);
		}
	}
}

//------------------------------------------------------------------------------
bool CvGame::InitMap(CvGameInitialItemsOverrides& kGameInitialItemsOverrides)
{
	CvMap& kMap = GC.getMap();
	const bool bWBSave = CvPreGame::isWBMapScript();
	if(bWBSave)
	{
		const CvWorldBuilderMapLoaderMapInfo& kWBMapInfo =
		    CvWorldBuilderMapLoader::GetCurrentMapInfo();

		if(kWBMapInfo.uiWidth * kWBMapInfo.uiHeight != 0)
		{
			CvMapInitData kMapInitData;
			kMapInitData.m_bWrapX = kWBMapInfo.bWorldWrap;
			kMapInitData.m_bWrapY = false;
			kMapInitData.m_iGridW = kWBMapInfo.uiWidth;
			kMapInitData.m_iGridH = kWBMapInfo.uiHeight;
			kMap.init(&kMapInitData);

			CvBarbarians::MapInit(kMap.numPlots());

			CvWorldBuilderMapLoader::InitMap();
			CvWorldBuilderMapLoader::ValidateTerrain();

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if(pkScriptSystem != NULL)
			{
				lua_State* L = pkScriptSystem->CreateLuaThread("WorldBuilderMapLoader");
				if(L != NULL)
				{
					lua_cpcall(L, &WorldBuilderMapLoaderAddRandomItems, 0);
					lua_cpcall(L, &WorldBuilderMapLoaderLoadModData, 0);
					lua_cpcall(L, &WorldBuilderMapLoaderRunPostProcessScript, 0);

					pkScriptSystem->FreeLuaThread(L);
				}
			}
		}
		else
		{
			// Empty map...
			FAssertMsg(0, "Empty World Builder Map!");

			// Make the map at least 1 x 1 to avoid crashes
			CvMapInitData kMapInitData;
			kMapInitData.m_bWrapX = false;
			kMapInitData.m_bWrapY = false;
			kMapInitData.m_iGridW = 1;
			kMapInitData.m_iGridH = 1;
			kMap.init(&kMapInitData);

			CvBarbarians::MapInit(kMap.numPlots());

			CvWorldBuilderMapLoader::ValidateTerrain();
		}
	}
	else
	{
		const CvString& strMapName = CvPreGame::mapScriptName();

		CvMapGenerator* pGenerator = FNEW(CvMapGenerator(strMapName.c_str()), c_eMPoolTypeGame, 0);

		CvMapInitData kData;
		if(pGenerator->GetMapInitData(kData, CvPreGame::worldSize()))
			kMap.init(&kData);
		else
			kMap.init();

		CvBarbarians::MapInit(kMap.numPlots());

		pGenerator->GenerateRandomMap();
		pGenerator->GetGameInitialItemsOverrides(kGameInitialItemsOverrides);

		delete pGenerator;
	}

	CvBarbarians::MapInit(GC.getMap().numPlots());

	// Run this for all maps because a map should never crash the game on
	// load regardless of where that map came from.  (The map scripts are mod-able after all!!!)
	CvWorldBuilderMapLoader::ValidateCoast();

	// Update some cached values
	GC.getMap().updateAdjacency();

	// Set all the observer teams to be able to see all the plots
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			SetAllPlotsVisible( kPlayer.getTeam() );
		}
	}

	return true;
}

void PrintPlayerInfo(int iIndex)
{
	const char* slotStatus[] = { "open","computer","closed","taken","observer" };
	const char* slotClaim[] = { "unassigned","reserved","assigned" };

	CvString msg = CvString::format( "Slot %d is %s and %s. Player is %s and %s. Team is %s.\n", 
		iIndex, slotStatus[ CvPreGame::slotStatus((PlayerTypes)iIndex) ], slotClaim[ CvPreGame::slotClaim((PlayerTypes)iIndex) ],
		GET_PLAYER((PlayerTypes)iIndex).isMajorCiv() ? "major" : "minor",
		GET_PLAYER((PlayerTypes)iIndex).isAlive() ? "alive" : "dead",
		GET_TEAM( GET_PLAYER((PlayerTypes)iIndex).getTeam() ).isObserver() ? "observer" : "player"
		);
	OutputDebugString(msg.c_str());
};

//------------------------------------------------------------------------------
void CvGame::InitPlayers()
{
	PlayerColorTypes aePlayerColors[REALLY_MAX_PLAYERS];
	bool bValid;
	int iI, iJ, iK, iL;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		const TeamTypes eTeam(static_cast<TeamTypes>(iI));
		CvTeam& kTeam = GET_TEAM(eTeam);
		kTeam.init(eTeam);
	}

	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		aePlayerColors[iI] = NO_PLAYERCOLOR;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		const PlayerTypes eLoopPlayer(static_cast<PlayerTypes>(iI));
		aePlayerColors[iI] = CvPreGame::playerColor(eLoopPlayer);

		if(aePlayerColors[iI] == NO_PLAYERCOLOR)
		{
			SlotStatus eStatus = CvPreGame::slotStatus(eLoopPlayer);
			if(eStatus == SS_TAKEN || eStatus == SS_COMPUTER || eStatus == SS_OBSERVER)
			{
				CvCivilizationInfo* pCivilizationInfo = GC.getCivilizationInfo(CvPreGame::civilization(eLoopPlayer));
				aePlayerColors[iI] = ((PlayerColorTypes)(pCivilizationInfo->getDefaultPlayerColor()));
			}
		}
	}

	CivilizationTypes eBarbCiv = (CivilizationTypes)GC.getBARBARIAN_CIVILIZATION();
	CivilizationTypes eMinorCiv = (CivilizationTypes)GC.getMINOR_CIVILIZATION();

	CvCivilizationInfo* pBarbarianCivilizationInfo = GC.getCivilizationInfo(eBarbCiv);
	int barbarianPlayerColor = pBarbarianCivilizationInfo->getDefaultPlayerColor();

	const int iNumPlayerColorInfos = GC.GetNumPlayerColorInfos();
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if(aePlayerColors[iI] != NO_PLAYERCOLOR)
		{
			for(iJ = 0; iJ < iI; iJ++)
			{
				if(aePlayerColors[iI] == aePlayerColors[iJ])
				{
					for(iK = 0; iK < iNumPlayerColorInfos; iK++)
					{
						if(iK != barbarianPlayerColor)
						{
							bValid = true;

							for(iL = 0; iL < MAX_MAJOR_CIVS; iL++)
							{
								if(aePlayerColors[iL] == iK)
								{
									bValid = false;
									break;
								}
							}

							if(bValid)
							{
								aePlayerColors[iI] = ((PlayerColorTypes)iK);
							}
						}
					}

					break;
				}
			}
		}
	}

	int iNumMinors = CvPreGame::numMinorCivs();

	// TODO: this is needed till we have a screen to push this data
	if(iNumMinors < 0)
	{
		const CvWorldInfo& kWorldInfo = CvPreGame::worldInfo();
		iNumMinors = kWorldInfo.getDefaultMinorCivs();
	}

	PlayerTypes eMinorPlayer;

	// Players
	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		// init Barbarian slot
		if(iI == BARBARIAN_PLAYER)
		{
			CvPreGame::setTeamType(BARBARIAN_PLAYER, BARBARIAN_TEAM);
			CvPreGame::setSlotStatus(BARBARIAN_PLAYER, SS_COMPUTER);
			CvPreGame::setNetID(BARBARIAN_PLAYER, -1);
			CvPreGame::setHandicap(BARBARIAN_PLAYER, (HandicapTypes)GC.getBARBARIAN_HANDICAP());
			CvPreGame::setCivilization(BARBARIAN_PLAYER, eBarbCiv);
			CvPreGame::setLeaderHead(BARBARIAN_PLAYER, (LeaderHeadTypes)GC.getBARBARIAN_LEADER());
			CvPreGame::setPlayerColor(BARBARIAN_PLAYER, ((PlayerColorTypes)barbarianPlayerColor));
			CvPreGame::setMinorCiv(BARBARIAN_PLAYER, false);
		}
		// Major Civs
		else if(iI < MAX_MAJOR_CIVS)
		{
			CvPreGame::setPlayerColor((PlayerTypes)iI, aePlayerColors[iI]);
			// Make sure the AI has the proper handicap.
			if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_COMPUTER)
			{
				CvPreGame::setHandicap((PlayerTypes)iI, (HandicapTypes)GC.getAI_HANDICAP());
			}
#if defined(MOD_BALANCE_CORE_OBSERVER_CHANGES)
			//don't use a special observer team
#else
			else if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER)
			{//make all observers be on the observer team.
				CvPreGame::setTeamType((PlayerTypes)iI, OBSERVER_TEAM);
			}
#endif
		}
		// Minor civs
		else if(iI < MAX_CIV_PLAYERS)
		{
			eMinorPlayer = (PlayerTypes) iI;

			if(iI < MAX_MAJOR_CIVS + iNumMinors)
			{
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
				CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo(CvPreGame::minorCivType((PlayerTypes)(eMinorPlayer + (MAX_PREGAME_MAJOR_CIVS - MAX_MAJOR_CIVS))));
#else
				CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo(CvPreGame::minorCivType(eMinorPlayer));
#endif
				if(pMinorCivInfo)
				{
					CvPreGame::setSlotStatus(eMinorPlayer, SS_COMPUTER);
					CvPreGame::setNetID(eMinorPlayer, -1);
					CvPreGame::setHandicap(eMinorPlayer, (HandicapTypes)GC.getMINOR_CIV_HANDICAP());
					CvPreGame::setCivilization(eMinorPlayer, eMinorCiv);
					CvPreGame::setLeaderHead(eMinorPlayer, (LeaderHeadTypes)GC.getBARBARIAN_LEADER());
					CvPreGame::setPlayerColor(eMinorPlayer, (PlayerColorTypes)pMinorCivInfo->getDefaultPlayerColor());
					CvPreGame::setMinorCiv(eMinorPlayer, true);
					CvPreGame::setMinorCivType(eMinorPlayer, (MinorCivTypes)pMinorCivInfo->GetID());
				}
			}
			else
			{
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
				int testingMinor = GetAvailableMinorCivType() + (eMinorPlayer + (MAX_PREGAME_MAJOR_CIVS - MAX_MAJOR_CIVS));
				CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo((MinorCivTypes)testingMinor);
#else
				MinorCivTypes eAvailableMinor = GetAvailableMinorCivType();
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_FreeCitySelector, eMinorPlayer, eAvailableMinor);
				CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo(eAvailableMinor);
#endif
				if (pMinorCivInfo)
				{
					CvPreGame::setSlotStatus(eMinorPlayer, SS_CLOSED);
					CvPreGame::setNetID(eMinorPlayer, -1);
					CvPreGame::setHandicap(eMinorPlayer, (HandicapTypes)GC.getMINOR_CIV_HANDICAP());
					CvPreGame::setCivilization(eMinorPlayer, eMinorCiv);
					CvPreGame::setLeaderHead(eMinorPlayer, (LeaderHeadTypes)GC.getBARBARIAN_LEADER());
					CvPreGame::setPlayerColor(eMinorPlayer, (PlayerColorTypes)pMinorCivInfo->getDefaultPlayerColor());
					CvPreGame::setMinorCiv(eMinorPlayer, true);
					CvPreGame::setMinorCivType(eMinorPlayer, (MinorCivTypes)pMinorCivInfo->GetID());
				}
			}
		}
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		kPlayer.init(ePlayer);
	}

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PrintPlayerInfo(iI);
	}
}

//	--------------------------------------------------------------------------------
//
// Set initial items (units, techs, etc...)
//
void CvGame::setInitialItems(CvGameInitialItemsOverrides& kInitialItemOverrides)
{
	initFreeState(kInitialItemOverrides);

	if(CvPreGame::isWBMapScript())
		assignStartingPlots();

	// Adjust FLAVOR_GROWTH and FLAVOR_EXPANSION based on map size
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);

		if(kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			kPlayer.GetFlavorManager()->AdjustWeightsForMap();
		}
	}

	initFreeUnits(kInitialItemOverrides);

	m_iEarliestBarbarianReleaseTurn = getHandicapInfo().getEarliestBarbarianReleaseTurn() + GC.getGame().getJonRandNum(GC.getAI_TACTICAL_BARBARIAN_RELEASE_VARIATION(), "barb release");

	// What route type forms an industrial connection
	DoUpdateIndustrialRoute();

	bool bCanWorkWater = GC.getCAN_WORK_WATER_FROM_GAME_START();

	// Team Stuff
	TeamTypes eTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(bCanWorkWater)
		{
			GET_TEAM(eTeam).changeWaterWorkCount(1);
		}

		GET_TEAM(eTeam).DoUpdateBestRoute();
	}

	// Player Stuff
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(ePlayer).isAlive())
		{
			// Major Civ init
			if(!GET_PLAYER(ePlayer).isMinorCiv())
			{
				GET_PLAYER(ePlayer).GetDiplomacyAI()->DoInitializePersonality();
			}
			// Minor Civ init
			else
			{
				GET_PLAYER(ePlayer).GetMinorCivAI()->DoPickInitialItems();
			}

			// Set Policy Costs before game starts, or else it'll be 0 on the first turn and Players can get something with any amount!
			GET_PLAYER(ePlayer).DoUpdateNextPolicyCost();

			// To have an orientation which plots are relatively good or bad
			if (GET_PLAYER(ePlayer).isMajorCiv())
				GET_PLAYER(ePlayer).computeAveragePlotFoundValue();
		}
	}

	// Which Tech unlocks the Religion Race? (based on a CvBuildingEntry)
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(pkBuildingInfo->IsFoundsReligion())
			{
				const TechTypes eReligionTech = (TechTypes) pkBuildingInfo->GetPrereqAndTech();
				SetReligionTech(eReligionTech);
				break;
			}
		}
	}

	DoUpdateTotalReligionTechCost();
	DoCacheMapScoreMod();

	// Diplomacy Victory
	DoInitDiploVictory();

	LogGameState();
}

void CvGame::CheckGenerateArchaeology()
{
	bool bTriggered = false;

	// See if archaeological data should be triggered
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS && !bTriggered; iTeamLoop++)
	{
		const TeamTypes eTeam1 = (TeamTypes)iTeamLoop;
		CvTeam& kTeam1 = GET_TEAM(eTeam1);

		if (kTeam1.isAlive() && !kTeam1.isMinorCiv())
		{
			for (int iTech = 0; iTech < GC.getNumTechInfos() && !bTriggered; iTech++)
			{
				CvTechEntry *pkTech = GC.getTechInfo((TechTypes)iTech);
				if (pkTech)
				{
					if (kTeam1.GetTeamTechs()->HasTech((TechTypes)pkTech->GetID()))
					{
						if (pkTech->IsTriggersArchaeologicalSites())
						{
							GC.getGame().TriggerArchaeologySiteCreation(false /*bCheckInitialized*/);
							bTriggered = true;
						}
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGame::regenerateMap()
{
	int iI;

	if(CvPreGame::isWBMapScript())
	{
		return;
	}

	setFinalInitialized(false);

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).killUnits();
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).killCities();
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GC.getGame().GetGameDeals().DoCancelAllDealsWithPlayer((PlayerTypes) iI);
	}

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).setFoundedFirstCity(false);
		GET_PLAYER((PlayerTypes)iI).setStartingPlot(NULL);
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		GC.getMap().setRevealedPlots(((TeamTypes)iI), false);
	}

	GC.getMap().erasePlots();

	CvGameInitialItemsOverrides kOverrides;

	setInitialItems(kOverrides);
	CheckGenerateArchaeology();
	initScoreCalculation();
	setFinalInitialized(true);

	GC.getMap().setupGraphical();
	GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);

	GC.GetEngineUserInterface()->setCycleSelectionCounter(1);

	gDLL->AutoSave(true);
}


//	--------------------------------------------------------------------------------
void CvGame::DoGameStarted()
{
	// Are features clearable?
	BuildTypes eBuild;
	int iBuildLoop;
	bool bTempClearable;

	//consistency check for map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		pLoopPlot->Validate( GC.getMap() );
	}

	FeatureTypes eFeature;
	for(int iFeatureLoop = 0; iFeatureLoop < GC.getNumFeatureInfos(); iFeatureLoop++)
	{
		eFeature = (FeatureTypes) iFeatureLoop;

		bTempClearable = false;

		// Check unit build actions to see if any of them clear this feature
		for(iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
		{
			eBuild = (BuildTypes) iBuildLoop;
			CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);

			// Feature can be removed by this build
			if(NULL != pBuildInfo && pBuildInfo->isFeatureRemove(eFeature))
			{
				GC.getFeatureInfo(eFeature)->SetClearable(true);

				bTempClearable = true;
				break;
			}

			if(bTempClearable)
				break;
		}
	}

#if defined(MOD_BALANCE_CORE)
	CvPlayerManager::Refresh(false);
#endif
}



//	--------------------------------------------------------------------------------
void CvGame::uninit()
{
	CvGoodyHuts::Uninit();
	CvBarbarians::Uninit();

	SAFE_DELETE_ARRAY(m_paiUnitCreatedCount);
	SAFE_DELETE_ARRAY(m_paiUnitClassCreatedCount);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCreatedCount);
	SAFE_DELETE_ARRAY(m_paiProjectCreatedCount);
	SAFE_DELETE_ARRAY(m_paiVoteOutcome);
	SAFE_DELETE_ARRAY(m_aiSecretaryGeneralTimer);
	SAFE_DELETE_ARRAY(m_aiVoteTimer);
	SAFE_DELETE_ARRAY(m_aiDiploVote);
	SAFE_DELETE_ARRAY(m_aiVotesCast);
	SAFE_DELETE_ARRAY(m_aiPreviousVotesCast);
	SAFE_DELETE_ARRAY(m_aiNumVotesForTeam);
	SAFE_DELETE_ARRAY(m_aiTeamCompetitionWinnersScratchPad);
	SAFE_DELETE_ARRAY(m_aiGreatestMonopolyPlayer);

	SAFE_DELETE_ARRAY(m_pabSpecialUnitValid);

	if(m_ppaaiTeamVictoryRank != NULL)
	{
		for(int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiTeamVictoryRank[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiTeamVictoryRank);
	}
#if defined(MOD_BALANCE_CORE_JFD)
	if(m_ppaiContractUnits != NULL)
	{
		for(int iI = 0; iI < GC.getNumContractInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaiContractUnits[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaiContractUnits);
	}
#endif

	m_aszDestroyedCities.clear();
	m_aszGreatPeopleBorn.clear();

	m_mapRand.uninit();
	m_jonRand.uninit();

	clearReplayMessageMap();

	m_aPlotExtraYields.clear();
	m_aPlotExtraCosts.clear();

	SAFE_DELETE(m_pDiploResponseQuery);

	SAFE_DELETE(m_pSettlerSiteEvaluator);
	SAFE_DELETE(m_pStartSiteEvaluator);
	SAFE_DELETE(m_pStartPositioner);
	SAFE_DELETE(m_pGameReligions);
	SAFE_DELETE(m_pGameCulture);
	SAFE_DELETE(m_pGameLeagues);
	SAFE_DELETE(m_pGameTrade);

#if defined(MOD_BALANCE_CORE)
	SAFE_DELETE(m_pGameCorporations);
	SAFE_DELETE(m_pGameContracts);
#endif

	SAFE_DELETE(m_pAdvisorCounsel);
	SAFE_DELETE(m_pAdvisorRecommender);

	m_bForceEndingTurn = false;

	m_lastTurnAICivsProcessed = -1;
	m_processPlayerAutoMoves = false;
	m_iEndTurnMessagesSent = 0;
	m_iElapsedGameTurns = 0;
	m_iStartTurn = 0;
	m_iWinningTurn = 0;
	m_iStartYear = 0;
	m_iEstimateEndTurn = 0;
	m_iDefaultEstimateEndTurn = 0;
	m_iTurnSlice = 0;
	m_iCutoffSlice = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iTotalEconomicValue = 0;
	m_iHighestEconomicValue = 0;
	m_iNoNukesCount = 0;
	m_iNukesExploded = 0;
	m_iMaxPopulation = 0;
	m_iUnused1 = 0;
	m_iUnused2 = 0;
	m_iUnused3 = 0;
	m_iInitPopulation = 0;
	m_iInitLand = 0;
	m_iInitTech = 0;
	m_iInitWonders = 0;
	m_iAIAutoPlay = 0;
	m_iTotalReligionTechCost = 0;
	m_iCachedWorldReligionTechProgress = 0;
	m_iUnitedNationsCountdown = 0;
	m_iNumVictoryVotesTallied = 0;
	m_iNumVictoryVotesExpected = 0;
	m_iVotesNeededForDiploVictory = 0;
	m_iMapScoreMod = 0;

	m_uiInitialTime = 0;

	m_bScoreDirty = false;
	m_bCircumnavigated = false;
	m_eTechAstronomy = NO_TECH;
	m_bDebugMode = false;
	m_bDebugModeCache = false;
	m_bFOW = true;
	m_bFinalInitialized = false;
	m_eWaitDiploPlayer = NO_PLAYER;
	m_bPbemTurnSent = false;
	m_bHotPbemBetweenTurns = false;
	m_bPlayerOptionsSent = false;
	m_bNukesValid = false;
	m_bEndGameTechResearched = false;
	m_bTunerEverConnected = false;
	m_bTutorialEverAttacked = false;
	m_bStaticTutorialActive = false;
	m_bEverRightClickMoved = false;
	m_bCombatWarned = false;
	m_bArchaeologyTriggered = false;

	m_eHandicap = NO_HANDICAP;
	m_ePausePlayer = NO_PLAYER;
	m_eAIAutoPlayReturnPlayer = NO_PLAYER;
	m_eBestLandUnit = NO_UNIT;
	m_eWinner = NO_TEAM;
	m_eVictory = NO_VICTORY;
	m_eGameState = GAMESTATE_ON;
	m_eBestWondersPlayer = NO_PLAYER;
	m_eBestPoliciesPlayer = NO_PLAYER;
	m_eBestGreatPeoplePlayer = NO_PLAYER;
	m_eReligionTech = NO_TECH;
	m_eIndustrialRoute = NO_ROUTE;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	m_eTeamThatCircumnavigated = NO_TEAM;
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	m_bVictoryRandomization = false;
	m_iCultureAverage = 0;
	m_iScienceAverage = 0;
	m_iDefenseAverage = 0;
	m_iGoldAverage = 0;
	m_iGlobalTechAvg = 0;
	m_iGlobalPopulation = 0;
	m_iLastTurnCSSurrendered = 0;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	m_iLargestBasePotential = 0;
#endif

	m_strScriptData = "";
	m_iEarliestBarbarianReleaseTurn = 0;

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiEndTurnMessagesReceived[iI] = 0;
		m_aiRankPlayer[iI] = 0;
		m_aiPlayerRank[iI] = 0;
		m_aiPlayerScore[iI] = 0;
	}

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiRankTeam[iI] = 0;
		m_aiTeamRank[iI] = 0;
		m_aiTeamScore[iI] = 0;
	}

	m_iLastMouseoverUnitID = 0;

	CvCityManager::Shutdown();
}

//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvGame::reset(HandicapTypes eHandicap, bool bConstructorCall)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_fCurrentTurnTimerPauseDelta = 0.f;

	CvString strUTF8DatabasePath = gDLL->GetCacheFolderPath();
	strUTF8DatabasePath += "Civ5SavedGameDatabase.db";

	std::wstring wstrDatabasePath = CvStringUtils::FromUTF8ToUTF16(strUTF8DatabasePath);

	if(DeleteFileW(wstrDatabasePath.c_str()) == FALSE)
	{
		if(GetLastError() != ERROR_FILE_NOT_FOUND)
		{
			CvAssertMsg(false, "Warning! Cannot delete existing Civ5SavedGameDatabase! Does something have it opened?");
		}
	}

	Database::Connection db;
	if(db.Open(strUTF8DatabasePath.c_str(), Database::OPEN_CREATE | Database::OPEN_READWRITE | Database::OPEN_FULLMUTEX))
	{
		db.Execute("CREATE TABLE SimpleValues(Name TEXT Primary Key, Value VARIANT)");
	}
	else
	{
		CvAssertMsg(false, "Warning! Cannot create new Civ5SavedGameDatabase.");
	}


	m_eHandicap = eHandicap;

	if(!bConstructorCall)
	{
		
		CvAssertMsg(m_aiGreatestMonopolyPlayer == NULL, "about to leak memory, CvGame::m_aiGreatestMonopolyPlayer");
		m_aiGreatestMonopolyPlayer = FNEW(int[GC.getNumResourceInfos()], c_eCiv5GameplayDLL, 0);
		for (iI = 0; iI < GC.getNumResourceInfos(); iI++)
		{
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo((ResourceTypes)iI);
			if (!pkResourceInfo)
			{
				continue;
			}
			m_aiGreatestMonopolyPlayer[iI] = NO_PLAYER;
		}

		CvAssertMsg(m_paiUnitCreatedCount==NULL, "about to leak memory, CvGame::m_paiUnitCreatedCount");
		m_paiUnitCreatedCount = FNEW(int[GC.getNumUnitInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)iI);
			if(!pkUnitInfo)
			{
				continue;
			}
			m_paiUnitCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiUnitClassCreatedCount==NULL, "about to leak memory, CvGame::m_paiUnitClassCreatedCount");
		m_paiUnitClassCreatedCount = FNEW(int[GC.getNumUnitClassInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
			if(!pkUnitClassInfo)
			{
				continue;
			}
			m_paiUnitClassCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiBuildingClassCreatedCount==NULL, "about to leak memory, CvGame::m_paiBuildingClassCreatedCount");
		m_paiBuildingClassCreatedCount = FNEW(int[GC.getNumBuildingClassInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			m_paiBuildingClassCreatedCount[iI] = 0;
		}

		CvAssertMsg(m_paiProjectCreatedCount==NULL, "about to leak memory, CvGame::m_paiProjectCreatedCount");
		m_paiProjectCreatedCount = FNEW(int[GC.getNumProjectInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectCreatedCount[iI] = 0;
		}

		//antonjs: todo: remove unused UN and voting variables and allocations
		CvAssertMsg(0 < GC.getNumVoteInfos(), "GC.getNumVoteInfos() is not greater than zero in CvGame::reset");
		CvAssertMsg(m_paiVoteOutcome==NULL, "about to leak memory, CvGame::m_paiVoteOutcome");
		m_paiVoteOutcome = FNEW(PlayerVoteTypes[GC.getNumVoteInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			m_paiVoteOutcome[iI] = NO_PLAYER_VOTE;
		}

		CvAssertMsg(0 < GC.getNumVoteSourceInfos(), "GC.getNumVoteSourceInfos() is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiDiploVote==NULL, "about to leak memory, CvGame::m_aiDiploVote");
		m_aiDiploVote = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiDiploVote[iI] = 0;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiVotesCast==NULL, "about to leak memory, CvGame::m_aiVotesCast");
		m_aiVotesCast = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiVotesCast[iI] = NO_TEAM;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiPreviousVotesCast == NULL, "About to leak memory, CvGame::m_aiPreviousVotesCast. Please send Anton your save file and version.");
		m_aiPreviousVotesCast = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiPreviousVotesCast[iI] = NO_TEAM;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiNumVotesForTeam==NULL, "about to leak memory, CvGame::m_aiNumVotesForTeam");
		m_aiNumVotesForTeam = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiNumVotesForTeam[iI] = 0;
		}

		CvAssertMsg(0 < MAX_CIV_TEAMS, "MAX_CIV_TEAMS is not greater than zero in CvGame::reset");
		CvAssertMsg(m_aiTeamCompetitionWinnersScratchPad==NULL, "about to leak memory, CvGame::m_aiTeamCompetitionWinnersScratchPad");
		m_aiTeamCompetitionWinnersScratchPad = FNEW(int[MAX_CIV_TEAMS], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			m_aiTeamCompetitionWinnersScratchPad[iI] = 0;
		}

		CvAssertMsg(m_pabSpecialUnitValid==NULL, "about to leak memory, CvGame::m_pabSpecialUnitValid");
		m_pabSpecialUnitValid = FNEW(bool[GC.getNumSpecialUnitInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
		{
			m_pabSpecialUnitValid[iI] = false;
		}

		CvAssertMsg(m_aiSecretaryGeneralTimer==NULL, "about to leak memory, CvGame::m_aiSecretaryGeneralTimer");
		CvAssertMsg(m_aiVoteTimer==NULL, "about to leak memory, CvGame::m_aiVoteTimer");
		m_aiSecretaryGeneralTimer = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		m_aiVoteTimer = FNEW(int[GC.getNumVoteSourceInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiSecretaryGeneralTimer[iI] = 0;
			m_aiVoteTimer[iI] = 0;
		}

		CvAssertMsg(m_ppaaiTeamVictoryRank==NULL, "about to leak memory, CvGame::m_ppaaiTeamVictoryRank");
		m_ppaaiTeamVictoryRank = FNEW(int*[GC.getNumVictoryInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_ppaaiTeamVictoryRank[iI] = FNEW(int[GC.getNUM_VICTORY_POINT_AWARDS()], c_eCiv5GameplayDLL, 0);
			for(int iJ = 0; iJ < GC.getNUM_VICTORY_POINT_AWARDS(); iJ++)
			{
				m_ppaaiTeamVictoryRank[iI][iJ] = NO_TEAM;
			}
		}
#if defined(MOD_BALANCE_CORE_JFD)
		CvAssertMsg(m_ppaiContractUnits==NULL, "about to leak memory, CvGame::m_ppaiContractUnits");
		m_ppaiContractUnits = FNEW(int*[GC.getNumContractInfos()], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < GC.getNumContractInfos(); iI++)
		{
			m_ppaiContractUnits[iI] = FNEW(int[GC.getNumUnitInfos()], c_eCiv5GameplayDLL, 0);
			for(int iJ = 0; iJ <GC.getNumUnitInfos(); iJ++)
			{
				m_ppaiContractUnits[iI][iJ] = 0;
			}
		}
#endif

		CvAssertMsg(m_pSettlerSiteEvaluator==NULL, "about to leak memory, CvGame::m_pSettlerSiteEvaluator");
		m_pSettlerSiteEvaluator = FNEW(CvSiteEvaluatorForSettler, c_eCiv5GameplayDLL, 0);
		m_pSettlerSiteEvaluator->Init();

		CvAssertMsg(m_pStartSiteEvaluator==NULL, "about to leak memory, CvGame::m_pStartSiteEvaluator");
		m_pStartSiteEvaluator = FNEW(CvSiteEvaluatorForStart, c_eCiv5GameplayDLL, 0);
		m_pStartSiteEvaluator->Init();

		CvAssertMsg(m_pStartPositioner==NULL, "about to leak memory, CvGame::m_pStartPositioner");
		m_pStartPositioner = new CvStartPositioner(m_pStartSiteEvaluator);

		m_kGameDeals.Init();

		CvAssertMsg(m_pGameReligions==NULL, "about to leak memory, CvGame::m_pGameReligions");
		m_pGameReligions = FNEW(CvGameReligions, c_eCiv5GameplayDLL, 0);
		m_pGameReligions->Init();

#if defined(MOD_BALANCE_CORE)
		CvAssertMsg(m_pGameCorporations==NULL, "about to leak memory, CvGame::m_pGameCorporations");
		m_pGameCorporations = FNEW(CvGameCorporations, c_eCiv5GameplayDLL, 0);
		m_pGameCorporations->Init();

		CvAssertMsg(m_pGameContracts==NULL, "about to leak memory, CvGame::m_pGameContracts");
		m_pGameContracts = FNEW(CvGameContracts, c_eCiv5GameplayDLL, 0);
		m_pGameContracts->Init();
#endif

		CvAssertMsg(m_pGameCulture==NULL, "about to leak memory, CvGame::m_pGameCulture");
		m_pGameCulture = FNEW(CvGameCulture, c_eCiv5GameplayDLL, 0);

		CvAssertMsg(m_pGameLeagues==NULL, "about to leak memory, CvGame::m_pGameLeagues");
		m_pGameLeagues = FNEW(CvGameLeagues, c_eCiv5GameplayDLL, 0);
		m_pGameLeagues->Init();

		CvAssertMsg(m_pGameTrade==NULL, "about to leak memory, CvGame::m_pGameTrade");
		m_pGameTrade = FNEW(CvGameTrade, c_eCiv5GameplayDLL, 0);
		m_pGameTrade->Init();

		CvAssertMsg(m_pAdvisorCounsel==NULL, "about to leak memory, CvGame::m_pAdvisorCounsel");
		m_pAdvisorCounsel = FNEW(CvAdvisorCounsel, c_eCiv5GameplayDLL, 0);

		CvAssertMsg(m_pAdvisorRecommender==NULL, "about to leak memory, CvGame::m_pAdvisorRecommender");
		m_pAdvisorRecommender = FNEW(CvAdvisorRecommender, c_eCiv5GameplayDLL, 0);

		m_eTechAstronomy = (TechTypes)GC.getInfoTypeForString("TECH_ASTRONOMY");
	}

	m_mapRand.reset();
	m_jonRand.reset();

	m_iNumSessions = 1;

	m_AdvisorMessagesViewed.clear();

	CvCityManager::Reset();

#if defined(MOD_BALANCE_CORE_GLOBAL_IDS)
	m_iGlobalAssetCounterAllPreviousTurns = 1000; //0 is invalid
	m_iGlobalAssetCounterCurrentTurn = 0;
#endif
}

//	--------------------------------------------------------------------------------
/// Initial diplomacy State: right now this just has all teams meet themselves and sets them at war with the Barbs
void CvGame::initDiplomacy()
{
	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		const TeamTypes eTeamA = static_cast<TeamTypes>(iI);
		CvTeam& kTeamA = GET_TEAM(eTeamA);
		kTeamA.meet(eTeamA, false);

		if(kTeamA.isBarbarian())
		{
			for(int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
			{
				const TeamTypes eTeamB = static_cast<TeamTypes>(iJ);
				if(iI != iJ)
				{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					kTeamA.declareWar(eTeamB, false, kTeamA.getLeaderID());
#else
					kTeamA.declareWar(eTeamB);
#endif
				}
			}
		}

#if defined(MOD_BALANCE_CORE_OBSERVER_CHANGES)
		if(kTeamA.isObserver())
		{
			for(int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
			{
				const TeamTypes eTeamB = static_cast<TeamTypes>(iJ);
				if(iI != iJ)
					kTeamA.makeHasMet(eTeamB,true);
			}
		}
#endif

	}
}


//	--------------------------------------------------------------------------------
void CvGame::initFreeState(CvGameInitialItemsOverrides& kOverrides)
{
	for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iI);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo)
		{
			for(int iJ = 0; iJ < MAX_TEAMS; iJ++)
			{
				const TeamTypes eTeam = static_cast<TeamTypes>(iJ);
				const bool bGrantFreeTechs = kOverrides.GrantInitialFreeTechsPerTeam[eTeam];

				if(bGrantFreeTechs)
				{
					CvTeam& kTeam = GET_TEAM(eTeam);
					if(kTeam.isAlive())
					{
						// Skip if we already have it
						if (kTeam.GetTeamTechs()->HasTech(eTech))
						{
							continue;
						}

						bool bValid = false;

						if(!bValid)
						{
							if((getHandicapInfo().isFreeTechs(iI)) ||
							        (!(kTeam.isHuman())&& getHandicapInfo().isAIFreeTechs(iI)) ||
							        (pkTechInfo->GetEra() < getStartEra()))
							{
								bValid = true;
							}
						}

						if(!bValid)
						{
							for(int iK = 0; iK < MAX_PLAYERS; iK++)
							{
								CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iK);
								if(kPlayer.isAlive())
								{
									if(kPlayer.getTeam() == eTeam)
									{
										if(kPlayer.getCivilizationInfo().isCivilizationFreeTechs(iI))
										{
											bValid = true;
											break;
										}
									}
								}
							}
						}

						kTeam.setHasTech(eTech, bValid, NO_PLAYER, false, false);
						if(bValid && pkTechInfo->IsMapVisible())
						{
							GC.getMap().setRevealedPlots(eTeam, true, true);
							GC.getMap().updateDeferredFog();
						}
					}
				}
			}
		}
	}

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isAlive())
		{
			kPlayer.initFreeState(kOverrides);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvGame::initFreeUnits(CvGameInitialItemsOverrides& kOverrides)
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		if(kOverrides.GrantInitialUnitsPerPlayer[ePlayer])
		{
			if(kPlayer.isAlive())
			{
				if((kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE,false,false) == 0) && (kPlayer.getNumCities() == 0))
				{
					kPlayer.initFreeUnits(kOverrides);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGame::assignStartingPlots()
{
	 GetStartPositioner()->Run(countMajorCivsAlive());
}

#if defined(EXTERNAL_PAUSING)
bool ExternalPause()
{
	bool bPause = false;

	// wait for an external mutex if it exists to make it easier to see what the AI is doing
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, "TurnByTurn");
	if(hMutex != NULL)
	{
		if (::WaitForSingleObject(hMutex,0)==WAIT_OBJECT_0)
		{
			//we acquired the mutex, that means we can continue
			ReleaseMutex(hMutex);
		}
		else
		{
			//couldn't acquire it, we should pause
			bPause = true;
			//sleep a little bit for simple rate limiting
			Sleep(200);
		}
		//close the handle in any case
		CloseHandle(hMutex);
	}

	return bPause;
}
#endif

//	---------------------------------------------------------------------------
void CvGame::update()
{
	if(IsWaitingForBlockingInput())
	{
		if(!GC.GetEngineUserInterface()->isDiploActive())
		{
			GET_PLAYER(m_eWaitDiploPlayer).doTurnPostDiplomacy();
			SetWaitingForBlockingInput(NO_PLAYER);
		}
		else
		{
			return;
		}
	}

	// Send a Lua event at the start of the update
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "GameCoreUpdateBegin", args.get(), bResult);
		}
	}

	// if the game is single player, it's ok to block all processing until
	// the user selects an extended match or quits.
	if(getGameState() == GAMESTATE_OVER && !CvPreGame::isNetworkMultiplayerGame())
	{
		testExtendedGame();
	}
	else
	{
		// allow extended games in MP without blocking processing. The game
		// may be "over" for 1 player in a match with more than 2 players,
		// when the player is defeated, for example, but not over for the
		// rest of the players. It may also be over for everyone in the match
		// but they may still have units/cities (science/cultural victories)
		if(getGameState() == GAMESTATE_OVER && CvPreGame::isNetworkMultiplayerGame())
		{
			testExtendedGame();
		}

		{
			sendPlayerOptions();

			//this creates the initial autosave
			if(getTurnSlice() == 0 && !isPaused())
			{
				gDLL->AutoSave(true);
			}

#if defined(EXTERNAL_PAUSING)
			bool bExternalPause = ExternalPause();
#else
			bool bExternalPause = false;
#endif

			// If there are no active players, move on to the AI
			if ( !bExternalPause && getNumGameTurnActive()==0 )
			{
				if(gDLL->CanAdvanceTurn())
					doTurn();
			}

			if(!isPaused() && !bExternalPause)	// Check for paused again, the doTurn call might have called something that paused the game and we don't want an update to sneak through
			{
				updateScore();

				updateWar();

				updateMoves();

				if(!isPaused())	// And again, the player can change after the automoves and that can pause the game
				{
					updateTimers();

					UpdatePlayers(); // slewis added!

					testAlive();

					if((getAIAutoPlay() == 0) && !(gDLL->GetAutorun()) && GAMESTATE_EXTENDED != getGameState())
					{
						if(CvPreGame::slotStatus(getActivePlayer()) != SS_OBSERVER && !GET_PLAYER(getActivePlayer()).isAlive())
						{
							setGameState(GAMESTATE_OVER);
						}
					}

					CheckPlayerTurnDeactivate();

					changeTurnSlice(1);

#if defined(MOD_ACTIVE_DIPLOMACY)
					if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
					{
						// JdH: humans may have been activated, check for AI diplomacy
						CvDiplomacyRequests::DoAIMPDiplomacyWithHumans();
					}
#endif

					gDLL->FlushTurnReminders();
				}
			}

			PlayerTypes activePlayerID = getActivePlayer();
			const CvPlayer& activePlayer = GET_PLAYER(activePlayerID);
			if(NO_PLAYER != activePlayerID && activePlayer.getAdvancedStartPoints() >= 0 && !GC.GetEngineUserInterface()->isInAdvancedStart())
			{
				GC.GetEngineUserInterface()->setInAdvancedStart(true);
			}
		}
	}

	// Send a Lua event at the end of the update
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "GameCoreUpdateEnd", args.get(), bResult);
		}
	}
}

//	---------------------------------------------------------------------------------------------------------
//	Check to see if the player's turn should be deactivated.
//	This occurs when the player has set its EndTurn and its AutoMoves to true
//	and all activity has been completed.
void CvGame::CheckPlayerTurnDeactivate()
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if(kPlayer.isAlive() && kPlayer.isTurnActive())
		{
			if(kPlayer.isEndTurn() || (!kPlayer.isHuman() && !kPlayer.HasActiveDiplomacyRequests()))		// For some reason, AI players don't set EndTurn, why not?
			{
				if(kPlayer.hasProcessedAutoMoves())
				{
					bool bAutoMovesComplete = false;
					if(!(kPlayer.hasBusyUnitOrCity()))
					{
						bAutoMovesComplete = true;

						NET_MESSAGE_DEBUG_OSTR_ALWAYS("CheckPlayerTurnDeactivate() : auto-moves complete for " << kPlayer.getName());
					}
					else if(gDLL->HasReceivedTurnComplete(kPlayer.GetID()))
					{
						bAutoMovesComplete = true;
					}

					if(bAutoMovesComplete)
					{
						kPlayer.setTurnActive(false);

						// Activate the next player
						// This is not done if simultaneous turns is enabled (Networked MP).
						// In that case, the local human is (should be) the player we just deactivated the turn for
						// and the AI players will be activated all at once in CvGame::doTurn, once we have received
						// all the moves from the other human players
						AI_PERF_FORMAT("AI-perf.csv", ("CheckPlayerTurnDeactivate(), Turn %03d, %s: activate next player", GC.getGame().getElapsedGameTurns(), kPlayer.getName()));
						if(!kPlayer.isSimultaneousTurns())
						{
							if((isPbem() || isHotSeat()) && kPlayer.isHuman() && countHumanPlayersAlive() > 1)
							{
								setHotPbemBetweenTurns(true);
							}

							if(isSimultaneousTeamTurns())
							{
								if(!GET_TEAM(kPlayer.getTeam()).isTurnActive())
								{
									for(int iJ = (kPlayer.getTeam() + 1); iJ < MAX_TEAMS; iJ++)
									{
										CvTeam& kTeam = GET_TEAM((TeamTypes)iJ);
										if(kTeam.isAlive() && !kTeam.isSimultaneousTurns())
										{//this team is alive and also running sequential turns.  They're up next!
											kTeam.setTurnActive(true);
											resetTurnTimer(false);
											break;
										}
									}
								}
								else
								{
									CvString logOutput;
									logOutput.Format("CheckPlayerTurnDeactivate(): Next sequential player not set TurnActive due to player(%i)'s team(%i) being TurnActive.", kPlayer.GetID(), kPlayer.getTeam());
									gDLL->netMessageDebugLog(logOutput);
								}
							}
							else
							{
								if(!GC.GetEngineUserInterface()->isDiploActive())
								{
									if(!isHotSeat() || kPlayer.isAlive() || !kPlayer.isHuman())		// If it is a hotseat game and the player is human and is dead, don't advance the player, we want them to get the defeat screen
									{
										for(int iJ = (kPlayer.GetID() + 1); iJ < MAX_PLAYERS; iJ++)
										{
											CvPlayer& kNextPlayer = GET_PLAYER((PlayerTypes)iJ);
											if(kNextPlayer.isAlive() && !kNextPlayer.isSimultaneousTurns())
											{//the player is alive and also running sequential turns.  they're up!
												if(isPbem() && kNextPlayer.isHuman())
												{
													if(!getPbemTurnSent())
													{
														gDLL->sendPbemTurn((PlayerTypes)iJ);
													}
												}
												else
												{
													kNextPlayer.setTurnActive(true);
													resetTurnTimer(false);
												}
												break;
											}
										}
									}
								}
								else
								{
									// KWG: This doesn't actually do anything other than print to the debug log
									changeNumGameTurnActive(1, std::string("Because the diplo screen is blocking I am bumping this up for player ") + getName());
								}
							}
						}
					}
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------------------------------------
void CvGame::updateScore(bool bForce)
{
	if(!isScoreDirty() && !bForce)
		return;

	setScoreDirty(false);

	bool abPlayerScored[MAX_CIV_PLAYERS];
	bool abTeamScored[MAX_CIV_TEAMS];
	int iScore;
	int iBestScore;
	PlayerTypes eBestPlayer;
	TeamTypes eBestTeam;
	int iI, iJ, iK;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		abPlayerScored[iI] = false;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		iBestScore = MIN_INT;
		eBestPlayer = NO_PLAYER;

		for(iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			if(!abPlayerScored[iJ])
			{
				iScore = GET_PLAYER((PlayerTypes)iJ).GetScore(false);

				if(iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestPlayer = (PlayerTypes)iJ;
				}
			}
		}

		abPlayerScored[eBestPlayer] = true;

		setRankPlayer(iI, eBestPlayer);
		setPlayerRank(eBestPlayer, iI);
		setPlayerScore(eBestPlayer, iBestScore);

		CvPlayerAI& player = GET_PLAYER(eBestPlayer);

		unsigned int uiDataSetIndex = player.getReplayDataSetIndex("REPLAYDATASET_SCORE");
		player.setReplayDataValue(uiDataSetIndex, getGameTurn(), iBestScore);
	}

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		abTeamScored[iI] = false;
	}

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		iBestScore = MIN_INT;
		eBestTeam = NO_TEAM;

		for(iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
		{
			if(!abTeamScored[iJ])
			{
				iScore = GET_TEAM((TeamTypes) iJ).GetScore();

				for(iK = 0; iK < MAX_CIV_PLAYERS; iK++)
				{
					if(GET_PLAYER((PlayerTypes)iK).getTeam() == iJ)
					{
						iScore += getPlayerScore((PlayerTypes)iK);
					}
				}

				if(iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestTeam = (TeamTypes)iJ;
				}
			}
		}

		abTeamScored[eBestTeam] = true;

		setRankTeam(iI, eBestTeam);
		setTeamRank(eBestTeam, iI);
		setTeamScore(eBestTeam, iBestScore);
	}
}

//	--------------------------------------------------------------------------------
/// How does the size of the map affect how some of the score components are weighted?
int CvGame::GetMapScoreMod() const
{
	return m_iMapScoreMod;
}

//	--------------------------------------------------------------------------------
void CvGame::DoCacheMapScoreMod()
{
	// Seed with a default value in case someone's removed the Standard worldsize (shame on you!)
	int iBaseNumTiles = 4160;

	// Calculate "base" num tiles for the average map
	WorldSizeTypes eStandardWorld = (WorldSizeTypes) GC.getInfoTypeForString("WORLDSIZE_STANDARD", true);
	if(eStandardWorld == NO_WORLDSIZE)
	{
		Database::SingleResult kResult;
		CvWorldInfo kWorldInfo;
		const bool bResult = DB.SelectAt(kResult, "Worlds", eStandardWorld);
		DEBUG_VARIABLE(bResult);
		CvAssertMsg(bResult, "Cannot find world info.");
		kWorldInfo.CacheResult(kResult);

		iBaseNumTiles = kWorldInfo.getGridWidth();
		iBaseNumTiles *= kWorldInfo.getGridHeight();
	}

	int iNumTilesOnThisMap = GC.getMap().numPlots();

	int iScoreMod = 100 * iBaseNumTiles / iNumTilesOnThisMap;

	// If we're giving a bonus to score, reduce the value, so that one pop isn't worth 30 points or something crazy on really small maps
	if(iScoreMod > 100)
	{
		iScoreMod -= 100;
		iScoreMod /= 3;
		iScoreMod += 100;
	}

	m_iMapScoreMod = iScoreMod;
}


//	--------------------------------------------------------------------------------
void CvGame::updateCitySight(bool bIncrement)
{
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			kPlayer.updateCitySight(bIncrement);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGame::updateSelectionList()
{
	if(!GC.GetEngineUserInterface()->DoAutoUnitCycle())
	{
		return;
	}

	auto_ptr<ICvUnit1> pDllHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

	if((pkHeadSelectedUnit == NULL) || !(pkHeadSelectedUnit->ReadyToSelect()))
	{
		if(getGameTurn() == 0)
		{
			SelectSettler();	// Auto select the settler on turn 0, helps with multiple humans in the same game (Hot Seat)
		}
	}

	pDllHeadSelectedUnit.reset(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

	if((pkHeadSelectedUnit == NULL) || !(pkHeadSelectedUnit->ReadyToSelect()))
	{
		int iOriginalPlotIndex = GC.GetEngineUserInterface()->getOriginalPlotIndex();
		CvPlot* pkOriginalPlot = (iOriginalPlotIndex != -1)? GC.getMap().plotByIndex(iOriginalPlotIndex) : NULL;

		if((pkOriginalPlot == NULL) || !(cyclePlotUnits(pkOriginalPlot, true, true, GC.GetEngineUserInterface()->getOriginalPlotCount())))
		{
			auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
			CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
			if((pkSelectionPlot == NULL) || !(cyclePlotUnits(pkSelectionPlot, true, true)))
			{
				cycleUnits(true);
			}
		}

		pDllHeadSelectedUnit.reset(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		pkHeadSelectedUnit = GC.UnwrapUnitPointer(pDllHeadSelectedUnit.get());

		if(pkHeadSelectedUnit != NULL)
		{
			if(!(pkHeadSelectedUnit->ReadyToSelect()))
			{
				GC.GetEngineUserInterface()->ClearSelectionList();
			}
		}
	}
}

//	-----------------------------------------------------------------------------------------------
int s_unitMoveTurnSlice = 0;

bool CvGame::hasTurnTimerExpired(PlayerTypes playerID)
{//gameLoopUpdate - Indicates that we're updating the turn timer for the game loop update.  
 //					This forces the active player's turn to finish if her turn time has elapsed.
 //					We also reset the turn timer when ai processing is occurring.
 //					If false, we're simply querying the game for a player's turn timer status.
	bool gameTurnTimerExpired = false;
	bool isLocalPlayer = getActivePlayer() == playerID;
	if(isOption(GAMEOPTION_END_TURN_TIMER_ENABLED) && !isPaused() && GC.getGame().getGameState() == GAMESTATE_ON)
	{
		ICvUserInterface2* iface = GC.GetEngineUserInterface();
		if(getElapsedGameTurns() > 0)
		{
			if(isLocalPlayer && (!gDLL->allAICivsProcessedThisTurn() || !allUnitAIProcessed()))
			{//the turn timer doesn't doesn't start until all ai processing has been completed for this game turn.
				resetTurnTimer(true);

				//hold the turn timer at 0 seconds with 0% completion
				CvPreGame::setEndTurnTimerLength(0.0f);
				iface->updateEndTurnTimer(0.0f);
			}
			else
			{//turn timer is actively ticking.
				if(playerID == NO_PLAYER)
				{//can't do a turn timer check for an invalid player.
					return false;
				}
				CvPlayer& curPlayer = GET_PLAYER(playerID);

				// Has the turn expired?
				float gameTurnEnd = static_cast<float>(getMaxTurnLen());

				//NOTE:  These times exclude the time used for AI processing.
				//Time since the current player's turn started.  Used for measuring time for players in sequential turn mode.
				float timeSinceCurrentTurnStart = m_curTurnTimer.Peek() + m_fCurrentTurnTimerPauseDelta; 
				//Time since the game (year) turn started.  Used for measuring time for players in simultaneous turn mode.
				float timeSinceGameTurnStart = m_timeSinceGameTurnStart.Peek() + m_fCurrentTurnTimerPauseDelta; 
				
				float timeElapsed = (curPlayer.isSimultaneousTurns() ? timeSinceGameTurnStart : timeSinceCurrentTurnStart);
				if(curPlayer.isTurnActive())
				{//The timer is ticking for our turn
					if(timeElapsed > gameTurnEnd)
					{
						if(s_unitMoveTurnSlice == 0)
						{
							gameTurnTimerExpired = true;
						}
						else if(s_unitMoveTurnSlice + 10 < getTurnSlice())
						{
							gameTurnTimerExpired = true;
						}
					}
				}

				if((!curPlayer.isTurnActive() || gDLL->HasReceivedTurnComplete(playerID)) //Active player has finished their turn.
					&& getNumSequentialHumans() > 1)	//or sequential turn mode
				{//It's not our turn and there are sequential turn human players in the game.

					//In this case, the turn timer shows progress in terms of the max possible time until our next turn.
					//As such, timeElapsed has to be adjusted to be a value in terms of the max possible time.

					//determine number of players in the sequential turn sequence, not counting the active player.
					int playersInSeq = getNumSequentialHumans(playerID);

					//The max turn length is multiplied by the number of other human players in the sequential turn sequence.
					gameTurnEnd *= playersInSeq;

					float timePerPlayer = gameTurnEnd / playersInSeq; //time limit per human
					//count how many human players are left until us in the sequence.
					int humanTurnsUntilMe = countSeqHumanTurnsUntilPlayerTurn(playerID);
					int humanTurnsCompleted = playersInSeq - humanTurnsUntilMe;

					if(humanTurnsUntilMe)
					{//We're waiting on other sequential players
						timeElapsed =  timeSinceCurrentTurnStart + humanTurnsCompleted*timePerPlayer;
					}
					else
					{//All the other sequential players have finished.
					 //Either we're waiting on turn processing or on players who are playing simultaneous turns.

						//scale time to be that of the remaining possible time for the simultaneous players.
						//From the player's perspective, the timer will simply creep down for the remaining simultaneous turn time
						//rather than skipping straight to zero like it would by just tracking the sequential players' turn time.
						timeElapsed = timeSinceGameTurnStart + (humanTurnsCompleted-1)*timePerPlayer;
					}
				}

				if(isLocalPlayer)
				{//update the local end turn timer.
					CvPreGame::setEndTurnTimerLength(gameTurnEnd);
					iface->updateEndTurnTimer(timeElapsed / gameTurnEnd);
				}
			}
		}
		else if(isLocalPlayer){
			//hold the turn timer at 0 seconds with 0% completion
			CvPreGame::setEndTurnTimerLength(0.0f);

#if defined(MOD_EVENTS_RED_TURN)
			if (MOD_EVENTS_RED_TURN)
			// RED <<<<<
			{
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem)
				{	
					CvLuaArgsHandle args;

					args->Push(getActivePlayer());

					bool bResult;
					LuaSupport::CallHook(pkScriptSystem, "TurnComplete", args.get(), bResult);
				}
			}
			// RED >>>>>
#endif

			iface->updateEndTurnTimer(0.0f);
		}
	}

	return gameTurnTimerExpired;
}

//	-----------------------------------------------------------------------------------------------
void CvGame::TurnTimerSync(float fCurTurnTime, float fTurnStartTime)
{
	m_curTurnTimer.StartWithOffset(fCurTurnTime);
	m_timeSinceGameTurnStart.StartWithOffset(fTurnStartTime);
}

//	-----------------------------------------------------------------------------------------------
void CvGame::GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime)
{
	fCurTurnTime = m_curTurnTimer.Peek();
	fTurnStartTime = m_timeSinceGameTurnStart.Peek();
}

//	-----------------------------------------------------------------------------------------------
void CvGame::updateTestEndTurn()
{
	PlayerTypes activePlayerID = getActivePlayer();
	CvPlayer& activePlayer = GET_PLAYER(activePlayerID);

	ICvUserInterface2* pkIface = GC.GetEngineUserInterface();
	if(pkIface != NULL)
	{
		bool automaticallyEndTurns = (isGameMultiPlayer())? pkIface->IsMPAutoEndTurnEnabled() : pkIface->IsSPAutoEndTurnEnabled();
		if(automaticallyEndTurns && s_unitMoveTurnSlice != 0)
			automaticallyEndTurns = s_unitMoveTurnSlice + 10 < getTurnSlice();

		if(automaticallyEndTurns)
		{
			bool hasSelection = false;

			auto_ptr<ICvUnit1> pDllHeadSelectedUnit(pkIface->GetHeadSelectedUnit());
			if(pDllHeadSelectedUnit.get() != NULL)
			{
				hasSelection = pDllHeadSelectedUnit->IsSelected();
			}

			if(!hasSelection && !pkIface->waitingForRemotePlayers())
			{
				if(m_endTurnTimerSemaphore < 1)
				{
					if(pkIface->canEndTurn() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && !gDLL->HasSentTurnComplete())
					{
#if !defined(NO_ACHIEVEMENTS)
						activePlayer.GetPlayerAchievements().EndTurn();
#endif

#if defined(MOD_EVENTS_RED_TURN)
						if (MOD_EVENTS_RED_TURN)
						// RED <<<<<
						{
							ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
							if(pkScriptSystem)
							{	
								CvLuaArgsHandle args;

								args->Push(getActivePlayer());

								bool bResult;
								LuaSupport::CallHook(pkScriptSystem, "TurnComplete", args.get(), bResult);
							}
						}
						// RED >>>>>
#endif

						gDLL->sendTurnComplete();
#if !defined(NO_ACHIEVEMENTS)
						CvAchievementUnlocker::EndTurn();
#endif
						m_endTurnTimer.Start();
					}
				}
			}
		}
	}

	if(activePlayer.isTurnActive())
	{
		// check notifications
		EndTurnBlockingTypes eEndTurnBlockingType = NO_ENDTURN_BLOCKING_TYPE;
		int iNotificationIndex = -1;
		activePlayer.GetNotifications()->GetEndTurnBlockedType(eEndTurnBlockingType, iNotificationIndex);

		if(eEndTurnBlockingType == NO_ENDTURN_BLOCKING_TYPE)
		{
			// No notifications are blocking, check units/cities
			if(activePlayer.hasPromotableUnit() && !GC.getGame().isOption(GAMEOPTION_PROMOTION_SAVING))
			{
				eEndTurnBlockingType = ENDTURN_BLOCKING_UNIT_PROMOTION;
			}
			else if(activePlayer.hasReadyUnit())
			{
				const CvUnit* pUnit = activePlayer.GetFirstReadyUnit();
				CvAssertMsg(pUnit, "GetFirstReadyUnit is returning null");
				if(pUnit)
				{
					if(!pUnit->canHold(pUnit->plot()))
					{
						eEndTurnBlockingType = ENDTURN_BLOCKING_STACKED_UNITS;
					}
					else
					{
						eEndTurnBlockingType = ENDTURN_BLOCKING_UNITS;
					}
				}
			}
		}

		if(eEndTurnBlockingType == NO_ENDTURN_BLOCKING_TYPE)
		{
			if(!(activePlayer.hasBusyUnitOrCity()) && !(activePlayer.hasReadyUnit()))
			{
				// JAR  - Looks like popups are pretty much disabled at this point, this check will break
				// multiplayer games. Look at revision #27 to resurrect the old popup check code if/when
				// they are implemented again.
				if(!isGameMultiPlayer())
				{
					if((activePlayer.isOption(PLAYEROPTION_WAIT_END_TURN) && !isGameMultiPlayer()) || !(GC.GetEngineUserInterface()->isHasMovedUnit()) || isHotSeat() || isPbem())
					{
						GC.GetEngineUserInterface()->setCanEndTurn(true);
					}
				}
				else
				{
					if(activePlayer.hasAutoUnit() && !m_sentAutoMoves)
					{
						if(!(gDLL->shiftKey()))
						{
							gDLL->sendAutoMoves();
							m_sentAutoMoves = true;
						}
					}
					else
					{
						if((activePlayer.isOption(PLAYEROPTION_WAIT_END_TURN) && !isGameMultiPlayer()) || !(GC.GetEngineUserInterface()->isHasMovedUnit()) || isHotSeat() || isPbem())
						{
							GC.GetEngineUserInterface()->setCanEndTurn(true);
						}
						else
						{
							if(GC.GetEngineUserInterface()->getEndTurnCounter() > 0)
							{
								GC.GetEngineUserInterface()->changeEndTurnCounter(-1);
							}
							else
							{
								if(!gDLL->HasSentTurnComplete() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && pkIface && pkIface->IsMPAutoEndTurnEnabled())
								{
#if !defined(NO_ACHIEVEMENTS)
									activePlayer.GetPlayerAchievements().EndTurn();
#endif

#if defined(MOD_EVENTS_RED_TURN)
									if (MOD_EVENTS_RED_TURN)
									// RED <<<<<
									{
										ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
										if(pkScriptSystem)
										{	
											CvLuaArgsHandle args;

											args->Push(getActivePlayer());

											bool bResult;
											LuaSupport::CallHook(pkScriptSystem, "TurnComplete", args.get(), bResult);
										}
									}
									// RED >>>>>
#endif

									gDLL->sendTurnComplete();
#if !defined(NO_ACHIEVEMENTS)
									CvAchievementUnlocker::EndTurn();
#endif
								}

								GC.GetEngineUserInterface()->setEndTurnCounter(3); // XXX
								if(isGameMultiPlayer())
								{
									GC.GetEngineUserInterface()->setCanEndTurn(true);
									m_endTurnTimer.Start();
								}
							}
						}
					}
				}
			}
		}

		activePlayer.SetEndTurnBlocking(eEndTurnBlockingType, iNotificationIndex);
	}
}

//	--------------------------------------------------------------------------------
void CvGame::testExtendedGame()
{
	int iI;

	if(getGameState() != GAMESTATE_OVER)
	{
		return;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if(GET_PLAYER((PlayerTypes)iI).isExtendedGame())
				{
					setGameState(GAMESTATE_EXTENDED);
					break;
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
CvUnit* CvGame::getPlotUnit(CvPlot* pPlot, int iIndex)
{
	IDInfo* pUnitNode1;
	IDInfo* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iCount;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if(pPlot != NULL)
	{
		iCount = 0;

		for(iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while(pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(*pUnitNode1);
				pUnitNode1 = pPlot->nextUnitNode(pUnitNode1);

				if(!(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if(!(pLoopUnit1->isCargo()))
					{
						if((pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
						{
							if(iCount == iIndex)
							{
								return pLoopUnit1;
							}

							iCount++;

							if(pLoopUnit1->hasCargo())
							{
								pUnitNode2 = pPlot->headUnitNode();

								while(pUnitNode2 != NULL)
								{
									pLoopUnit2 = ::getUnit(*pUnitNode2);
									pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

									if(!(pLoopUnit2->isInvisible(activeTeam, true)))
									{
										if(pLoopUnit2->getTransportUnit() == pLoopUnit1)
										{
											if(iCount == iIndex)
											{
												return pLoopUnit2;
											}

											iCount++;
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

	return NULL;
}

//	--------------------------------------------------------------------------------
void CvGame::getPlotUnits(CvPlot* pPlot, std::vector<CvUnit*>& plotUnits)
{
	plotUnits.erase(plotUnits.begin(), plotUnits.end());

	IDInfo* pUnitNode1;
	IDInfo* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if(pPlot != NULL)
	{
		for(iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while(pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(*pUnitNode1);
				pUnitNode1 = pPlot->nextUnitNode(pUnitNode1);

				if(!(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if(!(pLoopUnit1->isCargo()))
					{
						if((pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
						{
							plotUnits.push_back(pLoopUnit1);

							if(pLoopUnit1->hasCargo())
							{
								pUnitNode2 = pPlot->headUnitNode();

								while(pUnitNode2 != NULL)
								{
									pLoopUnit2 = ::getUnit(*pUnitNode2);
									pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

									if(!(pLoopUnit2->isInvisible(activeTeam, true)))
									{
										if(pLoopUnit2->getTransportUnit() == pLoopUnit1)
										{
											plotUnits.push_back(pLoopUnit2);
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
}

//	--------------------------------------------------------------------------------
void CvGame::cycleCities(bool bForward, bool bAdd)
{
	CvCity* pSelectCity = NULL;

	auto_ptr<ICvCity1> pHeadSelectedCity(GC.GetEngineUserInterface()->getHeadSelectedCity());

	CvCity* pkHeadSelectedCity = GC.UnwrapCityPointer(pHeadSelectedCity.get());

	if((pkHeadSelectedCity != NULL) && ((pkHeadSelectedCity->getTeam() == getActiveTeam()) || isDebugMode()))
	{
		int iLoop = pkHeadSelectedCity->getIndex();

		iLoop += (bForward ? 1 : -1);

		CvCity* pLoopCity = pkHeadSelectedCity;
		do
		{
			pLoopCity = GET_PLAYER(pLoopCity->getOwner()).nextCity(pLoopCity, !bForward);

			if(pLoopCity == NULL)
			{
				pLoopCity = GET_PLAYER(pkHeadSelectedCity->getOwner()).firstCity(&iLoop, !bForward);
			}

			if((pLoopCity != NULL) && (pLoopCity != pkHeadSelectedCity) && !pLoopCity->IsPuppet())  // we don't want the player to be able to cycle to puppeted cities - it kind of defeats teh whole purpose
			{
				pSelectCity = pLoopCity;
			}

		}
		while((pLoopCity != pkHeadSelectedCity) && !pSelectCity);

	}
	else
	{
		int iLoop = 0;
		pSelectCity = GET_PLAYER(getActivePlayer()).firstCity(&iLoop, !bForward);
	}

	if(pSelectCity != NULL)
	{
		auto_ptr<ICvCity1> pDllSelectedCity = GC.WrapCityPointer(pSelectCity);
		if(bAdd)
		{
			GC.GetEngineUserInterface()->clearSelectedCities();
			GC.GetEngineUserInterface()->addSelectedCity(pDllSelectedCity.get());
		}
		else
		{
			GC.GetEngineUserInterface()->selectCity(pDllSelectedCity.get());
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGame::cycleUnits(bool bClear, bool bForward, bool bWorkers)
{
	CvUnit* pNextUnit;
	CvCity* pCycleCity = NULL;
	bool bWrap = false;
	bool bProcessed = false;
	PlayerTypes eActivePlayer = getActivePlayer();
	ICvUserInterface2* pUI = GC.GetEngineUserInterface();
	CvPlayerAI& theActivePlayer = GET_PLAYER(eActivePlayer);

	auto_ptr<ICvUnit1> pDllSelectedUnit(pUI->GetHeadSelectedUnit());
	CvUnit* pCycleUnit = GC.UnwrapUnitPointer(pDllSelectedUnit.get());

	if(pCycleUnit != NULL)
	{
		if(pCycleUnit->getOwner() != eActivePlayer)
		{
			pCycleUnit = NULL;
		}

		pNextUnit = theActivePlayer.GetUnitCycler().Cycle(pCycleUnit, bForward, bWorkers, &bWrap);

		if(bWrap)
		{
			if(theActivePlayer.hasAutoUnit())
			{
				gDLL->sendAutoMoves();
			}
		}
	}
	else
	{
		pNextUnit = GC.getMap().findUnit(0, 0, eActivePlayer, true, bWorkers);
	}

	if(pNextUnit != NULL && !bWrap)
	{
		CvAssert(pNextUnit->getOwner() == eActivePlayer);
		selectUnit(pNextUnit, bClear);
		bProcessed = true;
	}

	if(pNextUnit != NULL /*&& bWrap */&& !pCycleCity && !bProcessed)
	{
		CvAssert(pNextUnit->getOwner() == eActivePlayer);
		selectUnit(pNextUnit, bClear);
	}

	if(pNextUnit == NULL && pCycleUnit != NULL && pCycleUnit->getOwner() == eActivePlayer)
	{
		pUI->ClearSelectionList();
		pCycleUnit->plot()->updateCenterUnit();
	}

	pDllSelectedUnit.reset(pUI->GetHeadSelectedUnit());
	CvUnit* pCurrentSelectedUnit = GC.UnwrapUnitPointer(pDllSelectedUnit.get());
	if((pCycleUnit != pCurrentSelectedUnit) || ((pCycleUnit != NULL) && pCycleUnit->ReadyToSelect()) || pCycleCity)
	{
		pUI->lookAtSelectionPlot();
	}
}

//	--------------------------------------------------------------------------------
// Returns true if unit was cycled...
bool CvGame::cyclePlotUnits(CvPlot* pPlot, bool bForward, bool bAuto, int iCount)
{
	IDInfo* pUnitNode;
	CvUnit* pSelectedUnit;
	CvUnit* pLoopUnit = NULL;

	CvAssertMsg(iCount >= -1, "iCount expected to be >= -1");

	if(iCount == -1)
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);

			if(NULL != pLoopUnit && pLoopUnit->IsSelected())
			{
				break;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}
	else
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(*pUnitNode);

			if((iCount - 1) == 0)
			{
				break;
			}

			if(iCount > 0)
			{
				iCount--;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}

		if(pUnitNode == NULL)
		{
			pUnitNode = pPlot->tailUnitNode();

			if(pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(*pUnitNode);
			}
		}
	}

	if(pUnitNode != NULL)
	{
		pSelectedUnit = pLoopUnit;

		while(true)
		{
			if(bForward)
			{
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
				if(pUnitNode == NULL)
				{
					pUnitNode = pPlot->headUnitNode();
				}
			}
			else
			{
				pUnitNode = pPlot->prevUnitNode(pUnitNode);
				if(pUnitNode == NULL)
				{
					pUnitNode = pPlot->tailUnitNode();
				}
			}

			pLoopUnit = ::getUnit(*pUnitNode);

			if(iCount == -1)
			{
				if(pLoopUnit == pSelectedUnit)
				{
					break;
				}
			}

			if(NULL != pLoopUnit && pLoopUnit->getOwner() == getActivePlayer())
			{
				if(bAuto)
				{
					if(pLoopUnit->ReadyToSelect())
					{
						/*GC.GetEngineUserInterface()->*/selectUnit(pLoopUnit, true);
						return true;
					}
				}
				else
				{
					auto_ptr<ICvUnit1> pDllLoopUnit = GC.WrapUnitPointer(pLoopUnit);
					GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllLoopUnit.get(), true, false);
					return true;
				}
			}

			if(pLoopUnit == pSelectedUnit)
			{
				break;
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvGame::selectionListMove(CvPlot* pPlot, bool bShift)
{
	if(pPlot == NULL)
	{
		return;
	}

	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if((pkSelectedUnit == NULL) || (pkSelectedUnit->getOwner() != getActivePlayer()))
	{
		return;
	}

	//need to be careful, potential deadlock
	if (GET_PLAYER(pkSelectedUnit->getOwner()).isTurnActive())
	{
		if(pkSelectedUnit->CanSwapWithUnitHere(*pPlot))
		{
			selectionListGameNetMessage(GAMEMESSAGE_SWAP_UNITS, CvTypes::getMISSION_SWAP_UNITS(), pPlot->getX(), pPlot->getY(), 0, false, bShift);
		}
		else
		{
			selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, CvTypes::getMISSION_MOVE_TO(), pPlot->getX(), pPlot->getY(), 0, false, bShift);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)
{
	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if(pkSelectedUnit != NULL)
	{
		if(pkSelectedUnit->getOwner() == getActivePlayer() && !pSelectedUnit->IsBusy())
		{
			if(eMessage == GAMEMESSAGE_DO_COMMAND)
			{
				gDLL->sendDoCommand(pkSelectedUnit->GetID(), ((CommandTypes)iData2), iData3, iData4, bAlt);
			}
			else if((eMessage == GAMEMESSAGE_PUSH_MISSION) || (eMessage == GAMEMESSAGE_AUTO_MISSION))
			{
				if(eMessage == GAMEMESSAGE_PUSH_MISSION)
				{
					MissionTypes eMission = (MissionTypes)iData2;
					CvPlot* pPlot = GC.getMap().plot(iData3, iData4);
					if(pPlot && pkSelectedUnit->CanSwapWithUnitHere(*pPlot) && eMission != CvTypes::getMISSION_ROUTE_TO())
					{
						gDLL->sendSwapUnits(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
					}
					else
					{
#if defined(MOD_EVENTS_RED_COMBAT_MISSION)
						if (MOD_EVENTS_RED_COMBAT_MISSION)
						// RED <<<<<
						{
							ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
							if(pkScriptSystem && pPlot)
							{						
								CvLuaArgsHandle args;

								args->Push(pkSelectedUnit->getOwner());
								args->Push(pkSelectedUnit->GetID());
								args->Push(pPlot->getX());
								args->Push(pPlot->getY());
								args->Push(iData2);

								bool bResult;
								LuaSupport::CallHook(pkScriptSystem, "PushingMissionTo", args.get(), bResult);
							}
						}
						// RED >>>>>
#endif

						gDLL->sendPushMission(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
					}
				}
				else
				{
					gDLL->sendAutoMission(pkSelectedUnit->GetID());
				}
			}
			else if((eMessage == GAMEMESSAGE_SWAP_UNITS))
			{
				gDLL->sendSwapUnits(pkSelectedUnit->GetID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
			}
			else
			{
				CvAssert(false);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	const IDInfo* pSelectedCityNode;
	CvCity* pSelectedCity;

	pSelectedCityNode = GC.GetEngineUserInterface()->headSelectedCitiesNode();

	while(pSelectedCityNode != NULL)
	{
		pSelectedCity = ::getCity(*pSelectedCityNode);
		pSelectedCityNode = GC.GetEngineUserInterface()->nextSelectedCitiesNode(pSelectedCityNode);
		CvAssert(pSelectedCity);

		if(pSelectedCity != NULL)
		{
			if(pSelectedCity->getOwner() == getActivePlayer())
			{
				switch(eMessage)
				{
				case GAMEMESSAGE_PUSH_ORDER:
					cityPushOrder(pSelectedCity, ((OrderTypes)iData2), iData3, bAlt, bShift, bCtrl);
					break;

				case GAMEMESSAGE_POP_ORDER:
					if(pSelectedCity->getOrderQueueLength() >= 1)
					{
						gDLL->sendPopOrder(pSelectedCity->GetID(), iData2);
					}
					break;

				case GAMEMESSAGE_SWAP_ORDER:
					if(pSelectedCity->getOrderQueueLength() >= 2)
					{
						gDLL->sendSwapOrder(pSelectedCity->GetID(), iData2);
					}
					break;

				case GAMEMESSAGE_DO_TASK:
					gDLL->sendDoTask(pSelectedCity->GetID(), ((TaskTypes)iData2), iData3, iData4, bOption, bAlt, bShift, bCtrl);
					break;

				default:
					CvAssert(false);
					break;
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvGame::cityPushOrder(CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	CvAssert(pCity);
	if(!pCity) return;
	gDLL->sendPushOrder(pCity->GetID(), eOrder, iData, bAlt, bShift, bCtrl);
}

//	--------------------------------------------------------------------------------
void CvGame::CityPurchase(CvCity* pCity, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield)
{
	CvAssert(pCity);
	if(!pCity) return;

	// we're trying to buy a unit
	if(eUnitType >= 0)
	{
		// if there's a unit of the same type in the tile, BAIL!
		if(!pCity->CanPlaceUnitHere(eUnitType))
		{
			return;
		}
	}

	gDLL->sendPurchase(pCity->GetID(), eUnitType, eBuildingType, eProjectType, ePurchaseYield);
}

//	--------------------------------------------------------------------------------
void CvGame::selectUnit(CvUnit* pUnit, bool bClear, bool bToggle, bool bSound)
{
	auto_ptr<ICvUnit1> pOldSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkOldSelectedUnit = GC.UnwrapUnitPointer(pOldSelectedUnit.get());

	GC.GetEngineUserInterface()->clearSelectedCities();

	bool bGroup = true;
	if(bClear)
	{
		GC.GetEngineUserInterface()->ClearSelectionList();
		bGroup = false;
	}

	pUnit->IncrementFirstTimeSelected();

	auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
	GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllUnit.get(), true, bToggle, bGroup, bSound);

	gDLL->GameplayMinimapUnitSelect(pUnit->getX(), pUnit->getY());

	GC.GetEngineUserInterface()->makeSelectionListDirty();

	bool bHighlightTrade = false;
	int iRouteIndex = -1;
	if(GC.getGame().getActivePlayer() == pUnit->getOwner())
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
		if(pkOldSelectedUnit)
		{
			pkOldSelectedUnit->plot()->updateCenterUnit();
		}
		pUnit->plot()->updateCenterUnit();

		if (pUnit->isTrade())
		{
			iRouteIndex = GC.getGame().GetGameTrade()->GetIndexFromUnitID( pUnit->GetID(), pUnit->getOwner() );
			if (iRouteIndex != -1)
				bHighlightTrade = true;
		}
	}
	
	if (bHighlightTrade)
		gDLL->TradeVisuals_ActivatePopupRoute(iRouteIndex);
	else
		gDLL->TradeVisuals_DeactivatePopupRoute();
}
//	--------------------------------------------------------------------------------
static void IfTradeUnit_DisplayPopupTradeRoute(CvUnit *pUnit)
{
	int iRouteIndex;

	iRouteIndex = -1;
	if (pUnit && pUnit->isTrade())
	{
		iRouteIndex = GC.getGame().GetGameTrade()->GetIndexFromUnitID(pUnit->GetID(),pUnit->getOwner());
		if (iRouteIndex != -1) {
			gDLL->TradeVisuals_ActivatePopupRoute(iRouteIndex);
		}
	}	
}
void CvGame::mouseoverUnit(CvUnit *pUnit, bool bEnter)
{
	CvUnit *pkSelectedUnit;

	if (pUnit)
	{
		if (bEnter) {
			IfTradeUnit_DisplayPopupTradeRoute(pUnit);
			m_iLastMouseoverUnitID = pUnit->GetID();
		} else {
			if (pUnit->GetID() == m_iLastMouseoverUnitID) {
				gDLL->TradeVisuals_DeactivatePopupRoute();

				//reactivate selected unit's popup route
				auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
				pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());
				IfTradeUnit_DisplayPopupTradeRoute(pkSelectedUnit);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvGame::selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt)
{
	IDInfo* pUnitNode;
	CvPlot* pUnitPlot;
	bool bGroup;

	CvAssertMsg(pUnit != NULL, "pUnit == NULL unexpectedly");

	if(bAlt || bCtrl)
	{
		GC.GetEngineUserInterface()->clearSelectedCities();

		if(!bShift)
		{
			GC.GetEngineUserInterface()->ClearSelectionList();
			bGroup = true;
		}
		else
		{
			bGroup = true;
		}

		if(!pUnit) return;

		pUnitPlot = pUnit->plot();

		pUnitNode = pUnitPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
			pUnitNode = pUnitPlot->nextUnitNode(pUnitNode);

			if(NULL != pLoopUnit && pLoopUnit->getOwner() == getActivePlayer())
			{
				if(pLoopUnit->canMove())
				{
					CvPlayerAI* pOwnerPlayer = &(GET_PLAYER(pLoopUnit->getOwner()));
#if defined(MOD_BALANCE_CORE)
					if( !pOwnerPlayer->isSimultaneousTurns() || getGameTurn() - pLoopUnit->getLastMoveTurn() >= 1)
#else
					if( !pOwnerPlayer->isSimultaneousTurns() || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#endif
					{
						if(bAlt || (pLoopUnit->getUnitType() == pUnit->getUnitType()))
						{
							auto_ptr<ICvUnit1> pDllLoopUnit = GC.WrapUnitPointer(pLoopUnit);
							GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllLoopUnit.get(), true, false, bGroup, false, true);
						}
					}
				}
			}
		}
	}
	else
	{
		auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
		GC.GetEngineUserInterface()->selectUnit(pDllUnit.get(), !bShift, bShift, true);
	}
}


//	--------------------------------------------------------------------------------
void CvGame::selectAll(CvPlot* pPlot)
{
	CvUnit* pSelectUnit = NULL;

	if(pPlot != NULL)
	{
		CvUnit* pCenterUnit = pPlot->getCenterUnit();

		if((pCenterUnit != NULL) && (pCenterUnit->getOwner() == getActivePlayer()))
		{
			pSelectUnit = pCenterUnit;
		}
	}

	if(pSelectUnit != NULL)
	{
		auto_ptr<ICvUnit1> pDllSelectUnit = GC.WrapUnitPointer(pSelectUnit);
		GC.GetEngineUserInterface()->selectGroup(pDllSelectUnit.get(), false, false, true);
	}
}

//	--------------------------------------------------------------------------------
void CvGame::SelectSettler(void)
{
	CvUnit* pSettlerUnit = NULL;
	CvPlayerAI* pActivePlayer = &(GET_PLAYER(getActivePlayer()));

	CvUnit* pLoopUnit = NULL;
	int iUnitIndex;
	for(pLoopUnit = pActivePlayer->firstUnit(&iUnitIndex); pLoopUnit != NULL; pLoopUnit = pActivePlayer->nextUnit(&iUnitIndex))
	{
		if(pLoopUnit->isFound())
		{
			pSettlerUnit = pLoopUnit;
			break;
		}
	}

	if(pSettlerUnit && pSettlerUnit->ReadyToSelect())
	{
		selectUnit(pSettlerUnit, true, false, true);
	}
}


//	--------------------------------------------------------------------------------
bool CvGame::selectionListIgnoreBuildingDefense()
{
	bool bIgnoreBuilding = false;
	bool bAttackLandUnit = false;
	auto_ptr<ICvUnit1> pSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkSelectedUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

	if(pkSelectedUnit != NULL)
	{
		if(pkSelectedUnit->ignoreBuildingDefense())
		{
			bIgnoreBuilding = true;
		}

		if((pkSelectedUnit->getDomainType() == DOMAIN_LAND) && pkSelectedUnit->IsCanAttack())
		{
			bAttackLandUnit = true;
		}
	}

	if(!bIgnoreBuilding && !bAttackLandUnit)
	{
		const UnitTypes eBestLandUnit = getBestLandUnit();
		if(eBestLandUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eBestLandUnit);
			if(pkUnitInfo)
			{
				bIgnoreBuilding = pkUnitInfo->IsIgnoreBuildingDefense();
			}
		}
	}

	return bIgnoreBuilding;
}


//	--------------------------------------------------------------------------------
bool CvGame::canHandleAction(int iAction, CvPlot* pPlot, bool bTestVisible)
{
	CvPlot* pMissionPlot;
	bool bShift = gDLL->shiftKey();

	CvActionInfo* pActionInfo = GC.getActionInfo(iAction);
	CvAssert(pActionInfo != NULL);
	if(!pActionInfo) return false;

	if(pActionInfo->getControlType() != NO_CONTROL)
	{
		if(canDoControl((ControlTypes)(GC.getActionInfo(iAction)->getControlType())))
		{
			return true;
		}
	}

	if(GC.GetEngineUserInterface()->isCitySelection())
	{
		return false; // XXX hack!
	}

	auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
	CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());

	if(pkHeadSelectedUnit != NULL)
	{
		if(pkHeadSelectedUnit->getOwner() == getActivePlayer())
		{
			if(GET_PLAYER(pkHeadSelectedUnit->getOwner()).isSimultaneousTurns() || GET_PLAYER(pkHeadSelectedUnit->getOwner()).isTurnActive())
			{
				if(GC.getActionInfo(iAction)->getMissionType() != NO_MISSION)
				{
					if(pPlot != NULL)
					{
						pMissionPlot = pPlot;
					}
					else if(bShift)
					{
						pMissionPlot = pkHeadSelectedUnit->LastMissionPlot();
					}
					else
					{
						pMissionPlot = NULL;
					}

					if((pMissionPlot == NULL) || !(pMissionPlot->isVisible(pkHeadSelectedUnit->getTeam())))
					{
						pMissionPlot = pkHeadSelectedUnit->plot();
					}

					if(pkHeadSelectedUnit->CanStartMission(pActionInfo->getMissionType(), pActionInfo->getMissionData(), -1, pMissionPlot, bTestVisible))
					{
						return true;
					}
				}

				if(GC.getActionInfo(iAction)->getCommandType() != NO_COMMAND)
				{
					if(pkHeadSelectedUnit->canDoCommand(((CommandTypes)(pActionInfo->getCommandType())), pActionInfo->getCommandData(), -1, bTestVisible))
					{
						return true;
					}
				}

				if(GC.GetEngineUserInterface()->CanDoInterfaceMode((InterfaceModeTypes)pActionInfo->getInterfaceModeType(), bTestVisible))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
//	Handle an action initiated by the local human player
void CvGame::handleAction(int iAction)
{
	bool bAlt;
	bool bShift;
	bool bSkip;

	bAlt = gDLL->altKey();
	bShift = false;//gDLL->shiftKey();

	auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());

	if(!(canHandleAction(iAction)))
	{
		return;
	}

	// Control
	CvActionInfo* pkActionInfo = GC.getActionInfo(iAction);
	if(pkActionInfo->getControlType() != NO_CONTROL)
	{
		doControl((ControlTypes)(pkActionInfo->getControlType()));
	}

	// Interface Mode
	if(GC.GetEngineUserInterface()->CanDoInterfaceMode((InterfaceModeTypes)pkActionInfo->getInterfaceModeType()))
	{
		if(pHeadSelectedUnit.get() != NULL)
		{
			if(GC.getInterfaceModeInfo((InterfaceModeTypes)pkActionInfo->getInterfaceModeType())->getSelectAll())
			{
				GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, false, true);
			}
			else if(GC.getInterfaceModeInfo((InterfaceModeTypes)pkActionInfo->getInterfaceModeType())->getSelectType())
			{
				GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, true, false);
			}
		}

		GC.GetEngineUserInterface()->setInterfaceMode((InterfaceModeTypes)pkActionInfo->getInterfaceModeType());
	}

	// Mission
	int iMissionType = pkActionInfo->getMissionType();
	if(iMissionType != NO_MISSION)
	{
		bool bSkipMissionAdd = false;

		if(iMissionType == CvTypes::getMISSION_BUILD() || iMissionType == CvTypes::getMISSION_FOUND())
		{
			bool bShowConfirmPopup = false;
			// If we're going to build over an existing Improvement/Route, ask the player to confirm
			if(iMissionType == CvTypes::getMISSION_BUILD())
			{
				int iBuild = pkActionInfo->getMissionData();
				CvBuildInfo* pBuildInfo = GC.getBuildInfo((BuildTypes)iBuild);

				// Let them Repair, it's fine
				if(!pBuildInfo->isRepair())
				{
					// Let them build a Route, it's fine
					if(pBuildInfo->getRoute() == NO_ROUTE)
					{
						// Let them build over an improvement if the new improvement REQUIRES the old one
						CvImprovementEntry *pImprovementInfo = GC.getImprovementInfo((ImprovementTypes)pBuildInfo->getImprovement());
						if (!pImprovementInfo || !pImprovementInfo->IsRequiresImprovement())
						{
							if(pHeadSelectedUnit.get() != NULL)
							{
								CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
								CvPlot* pPlot = pkHeadSelectedUnit->plot();
								if(pPlot != NULL)
								{
									if(pPlot->getImprovementType() != NO_IMPROVEMENT && !(pPlot->getFeatureType() == FEATURE_FALLOUT && pBuildInfo->isFeatureRemove(FEATURE_FALLOUT)))
									{
										bShowConfirmPopup = true;
									}
								}
							}
						}
					}
				}
			}
			else
			if(iMissionType == CvTypes::getMISSION_FOUND())
			{
				if(pHeadSelectedUnit.get() != NULL)
				{
					CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
					CvPlot* pPlot = pkHeadSelectedUnit->plot();
					if(pPlot != NULL)
					{
						ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
						ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
						if (pPlot->getResourceType() == eArtifactResourceType || pPlot->getResourceType() == eHiddenArtifactResourceType)
						{
							bShowConfirmPopup = true;
						}
					}
				}
			}

			if(bShowConfirmPopup)
			{
				int iBuild = pkActionInfo->getMissionData();
				CvPopupInfo kPopupInfo(BUTTONPOPUP_CONFIRM_IMPROVEMENT_REBUILD, iAction, iBuild);
				kPopupInfo.bOption1 = bAlt;
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
				bSkipMissionAdd = true;		// Skip the mission add, the popup will do it
			}
		}
		else
		if (iMissionType == CvTypes::getMISSION_ESTABLISH_TRADE_ROUTE())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					GC.getGame().GetGameTrade()->InvalidateTradePathCache(pkHeadSelectedUnit->getOwner());
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_INTERNATIONAL_TRADE_ROUTE, pkHeadSelectedUnit->getOwner());
					kPopup.iData2 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;	// Skip no matter what, if there is no unit, there is no mission
		}
		else
		if (iMissionType == CvTypes::getMISSION_CHANGE_TRADE_UNIT_HOME_CITY())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_TRADE_UNIT_NEW_HOME);
					kPopup.iData1 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;	// Skip no matter what, if there is no unit, there is no mission
		}
		if (iMissionType == CvTypes::getMISSION_CHANGE_ADMIRAL_PORT())
		{
			if(pHeadSelectedUnit.get() != NULL)
			{
				CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
				if (pkHeadSelectedUnit)
				{
					CvPopupInfo kPopup(BUTTONPOPUP_CHOOSE_ADMIRAL_PORT);
					kPopup.iData1 = pkHeadSelectedUnit->getIndex();
					GC.GetEngineUserInterface()->AddPopup(kPopup);
				}
			}
			bSkipMissionAdd = true;	// Skip no matter what, if there is no unit, there is no mission
		}

		if (!bSkipMissionAdd)
		{
			selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, iMissionType, pkActionInfo->getMissionData(), -1, 0, false, bShift);
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
	}

	// Command
	if(pkActionInfo->getCommandType() != NO_COMMAND)
	{
		bSkip = false;

		if(!bSkip)
		{
			if(pkActionInfo->isConfirmCommand())
			{
				CvPopupInfo kPopupInfo(BUTTONPOPUP_CONFIRMCOMMAND, iAction);
				kPopupInfo.bOption1 = bAlt;
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
			}
			else
			{
				selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, pkActionInfo->getCommandType(), pkActionInfo->getCommandData(), -1, 0, bAlt);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvGame::canDoControl(ControlTypes eControl)
{
	switch(eControl)
	{
	case CONTROL_SELECTYUNITTYPE:
	case CONTROL_SELECTYUNITALL:
	case CONTROL_SELECT_HEALTHY:
	case CONTROL_SELECTCITY:
	case CONTROL_SELECTCAPITAL:
	case CONTROL_NEXTUNIT:
	case CONTROL_PREVUNIT:
	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
	case CONTROL_CYCLEWORKER:
	case CONTROL_LASTUNIT:
	case CONTROL_FORCEENDTURN:
	case CONTROL_AUTOMOVES:
	case CONTROL_SAVE_GROUP:
	case CONTROL_QUICK_SAVE:
	case CONTROL_QUICK_LOAD:
	case CONTROL_TURN_LOG:
		if(!GC.GetEngineUserInterface()->isFocused())
		{
			return true;
		}
		break;

	case CONTROL_PING:
	case CONTROL_YIELDS:
	case CONTROL_RESOURCE_ALL:
	case CONTROL_UNIT_ICONS:
	case CONTROL_SCORES:
	case CONTROL_OPTIONS_SCREEN:
	case CONTROL_DOMESTIC_SCREEN:
	case CONTROL_CIVILOPEDIA:
	case CONTROL_POLICIES_SCREEN:
	case CONTROL_FOREIGN_SCREEN:
	case CONTROL_MILITARY_SCREEN:
	case CONTROL_TECH_CHOOSER:
	case CONTROL_INFO:
	case CONTROL_SAVE_NORMAL:
	case CONTROL_ADVISOR_COUNSEL:
	case CONTROL_NEXTCITY:
	case CONTROL_PREVCITY:
	case CONTROL_RELIGION_OVERVIEW:
	case CONTROL_ESPIONAGE_OVERVIEW:
		return true;
		break;

	case CONTROL_VICTORY_SCREEN:
		if(getGameState() == GAMESTATE_ON)
		{
			return true;
		}
		break;


	case CONTROL_CENTERONSELECTION:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		if(pSelectionPlot.get() != NULL)
		{
			return true;
		}
	}
	break;

	case CONTROL_LOAD_GAME:
		if(!(isNetworkMultiPlayer()))
		{
			return true;
		}
		break;

	case CONTROL_RETIRE:
		if((getGameState() == GAMESTATE_ON) || isGameMultiPlayer())
		{
			if(GET_PLAYER(getActivePlayer()).isAlive())
			{
				if(isPbem() || isHotSeat())
				{
					if(!GET_PLAYER(getActivePlayer()).isEndTurn())
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
		break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if(GC.GetEngineUserInterface()->canEndTurn() && !GC.GetEngineUserInterface()->isFocused())
		{
			return true;
		}
		break;

	case CONTROL_TOGGLE_STRATEGIC_VIEW:
		GC.GetEngineUserInterface()->ToggleStrategicView();
		break;

	case CONTROL_RESTART_GAME:
		{
			if(!isGameMultiPlayer() && getGameTurn() == getStartTurn())
			{
				return true;
			}
		}
		break;

	default:
		CvAssertMsg(false, "eControl did not match any valid options");
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvGame::doControl(ControlTypes eControl)
{
	if(!canDoControl(eControl))
	{
		return;
	}

	switch(eControl)
	{
	case CONTROL_CENTERONSELECTION:
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_SELECTYUNITTYPE:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		if(pHeadSelectedUnit.get() != NULL)
		{
			GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, true, false);
		}
	}
	break;

	case CONTROL_SELECTYUNITALL:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		if(pHeadSelectedUnit.get() != NULL)
		{
			GC.GetEngineUserInterface()->selectGroup(pHeadSelectedUnit.get(), false, false, true);
		}
	}
	break;

	case CONTROL_SELECT_HEALTHY:
	{
		auto_ptr<ICvUnit1> pHeadSelectedUnit(GC.GetEngineUserInterface()->GetHeadSelectedUnit());
		CvUnit* pkHeadSelectedUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit.get());
		GC.GetEngineUserInterface()->ClearSelectionList();
		if(pkHeadSelectedUnit != NULL)
		{
			CvPlot* pHeadPlot = pkHeadSelectedUnit->plot();
			std::vector<CvUnit*> plotUnits;
			getPlotUnits(pHeadPlot, plotUnits);
			for(int iI = 0; iI < (int) plotUnits.size(); iI++)
			{
				CvUnit* pUnit = plotUnits[iI];

				if(pUnit->getOwner() == getActivePlayer())
				{
#if defined(MOD_BALANCE_CORE)
					if(!GET_PLAYER(pUnit->getOwner()).isSimultaneousTurns() || getGameTurn() - pUnit->getLastMoveTurn() >= 1)
#else
					if(!GET_PLAYER(pUnit->getOwner()).isSimultaneousTurns() || getTurnSlice() - pUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
#endif
					{
						if(pUnit->IsHurt())
						{
							auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
							GC.GetEngineUserInterface()->InsertIntoSelectionList(pDllUnit.get(), true, false, true, true, true);
						}
					}
				}
			}
		}
	}
	break;

	case CONTROL_SELECTCITY:
		if(GC.GetEngineUserInterface()->isCityScreenUp())
		{
			cycleCities();
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity();
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_SELECTCAPITAL:
	{
		CvCity* pCapitalCity = GET_PLAYER(getActivePlayer()).getCapitalCity();
		if(pCapitalCity != NULL)
		{
			auto_ptr<ICvCity1> pDllCapitalCity = GC.WrapCityPointer(pCapitalCity);
			GC.GetEngineUserInterface()->selectCity(pDllCapitalCity.get());
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
	}
	break;

	case CONTROL_NEXTCITY:
		if(GC.GetEngineUserInterface()->isCitySelection())
		{
			cycleCities(true, !(GC.GetEngineUserInterface()->isCityScreenUp()));
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity(true);
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_PREVCITY:
		if(GC.GetEngineUserInterface()->isCitySelection())
		{
			cycleCities(false, !(GC.GetEngineUserInterface()->isCityScreenUp()));
		}
		else
		{
			GC.GetEngineUserInterface()->selectLookAtCity(true);
		}
		GC.GetEngineUserInterface()->lookAtSelectionPlot();
		break;

	case CONTROL_NEXTUNIT:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
		if(pkSelectionPlot != NULL)
		{
			cyclePlotUnits(pkSelectionPlot);
		}
		break;
	}

	case CONTROL_PREVUNIT:
	{
		auto_ptr<ICvPlot1> pSelectionPlot(GC.GetEngineUserInterface()->getSelectionPlot());
		CvPlot* pkSelectionPlot = GC.UnwrapPlotPointer(pSelectionPlot.get());
		if(pkSelectionPlot != NULL)
		{
			cyclePlotUnits(pkSelectionPlot, false);
		}
		break;
	}

	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
		cycleUnits(true);
		break;

	case CONTROL_CYCLEWORKER:
		cycleUnits(true, true, true);
		break;

	case CONTROL_LASTUNIT:
	{
		ICvUserInterface2* UI = GC.GetEngineUserInterface();
		auto_ptr<ICvUnit1> pUnit(UI->getLastSelectedUnit());

		if(pUnit.get() != NULL)
		{
			UI->selectUnit(pUnit.get(), true);
			UI->lookAtSelectionPlot();
		}
		else
		{
			cycleUnits(true, false);
		}

		UI->setLastSelectedUnit(NULL);
	}
	break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if(GC.GetEngineUserInterface()->canEndTurn() && gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed())
		{
			CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());
#if defined(MOD_EVENTS_RED_TURN)
			if (MOD_EVENTS_RED_TURN)
			// RED <<<<<
			{
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem)
				{	
					CvLuaArgsHandle args;

					args->Push(getActivePlayer());

					bool bResult;
					LuaSupport::CallHook(pkScriptSystem, "TurnComplete", args.get(), bResult);
				}
			}
			// RED >>>>>
#endif

#if !defined(NO_ACHIEVEMENTS)
			kActivePlayer.GetPlayerAchievements().EndTurn();
#endif
			gDLL->sendTurnComplete();
#if !defined(NO_ACHIEVEMENTS)
			CvAchievementUnlocker::EndTurn();
#endif
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
		break;

	case CONTROL_FORCEENDTURN:
	{
		EndTurnBlockingTypes eBlock = GET_PLAYER(getActivePlayer()).GetEndTurnBlockingType();
		if(gDLL->allAICivsProcessedThisTurn() && allUnitAIProcessed() && (eBlock == NO_ENDTURN_BLOCKING_TYPE || eBlock == ENDTURN_BLOCKING_UNITS))
		{
#if !defined(NO_ACHIEVEMENTS)
			CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());
			kActivePlayer.GetPlayerAchievements().EndTurn();
#endif

#if defined(MOD_EVENTS_RED_TURN)
				if (MOD_EVENTS_RED_TURN)
				// RED <<<<<
				{
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if(pkScriptSystem)
					{	
						CvLuaArgsHandle args;

						args->Push(getActivePlayer());

						bool bResult;
						LuaSupport::CallHook(pkScriptSystem, "TurnComplete", args.get(), bResult);
					}
				}
				// RED >>>>>
#endif

			gDLL->sendTurnComplete();
#if !defined(NO_ACHIEVEMENTS)
			CvAchievementUnlocker::EndTurn();
#endif
			SetForceEndingTurn(true);
			GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_SELECTION);
		}
		break;
	}

	case CONTROL_AUTOMOVES:
		gDLL->sendAutoMoves();
		break;

	case CONTROL_PING:
		GC.GetEngineUserInterface()->setInterfaceMode(INTERFACEMODE_PING);
		break;

	case CONTROL_YIELDS:
		GC.GetEngineUserInterface()->toggleYieldVisibleMode();
		break;

	case CONTROL_RESOURCE_ALL:
		GC.GetEngineUserInterface()->toggleResourceVisibleMode();
		break;

	case CONTROL_UNIT_ICONS:
		break;

	case CONTROL_SCORES:
		break;

	case CONTROL_LOAD_GAME:
		gDLL->LoadGame();
		break;

	case CONTROL_OPTIONS_SCREEN:
		gDLL->GameplayOpenOptionsScreen();
		break;

	case CONTROL_RETIRE:
		if(!isGameMultiPlayer() || countHumanPlayersAlive() == 1)
		{
			setGameState(GAMESTATE_OVER);
			GC.GetEngineUserInterface()->setDirty(Soundtrack_DIRTY_BIT, true);
		}
		else
		{
			if(isNetworkMultiPlayer())
			{
				GC.GetEngineUserInterface()->exitingToMainMenu();
			}
		}
		break;

	case CONTROL_SAVE_GROUP:
		gDLL->SaveGame(SAVEGAME_GROUP);
		break;

	case CONTROL_SAVE_NORMAL:
		gDLL->SaveGame(SAVEGAME_NORMAL);
		break;

	case CONTROL_QUICK_SAVE:
		if(!(isNetworkMultiPlayer()))	// SP only!
		{
			gDLL->QuickSave();
		}
		break;

	case CONTROL_QUICK_LOAD:
		if(!(isNetworkMultiPlayer()))	// SP only!
		{
			gDLL->QuickLoad();
		}
		break;

	case CONTROL_CIVILOPEDIA:
		gDLL->GameplaySearchForPediaEntry("OPEN_VIA_HOTKEY");
		break;

	case CONTROL_POLICIES_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_CHOOSEPOLICY, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_FOREIGN_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_DIPLOMATIC_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_MILITARY_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_MILITARY_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_TECH_CHOOSER:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_TECH_TREE, getActivePlayer());

		// If the popup queue is empty, just show the tech tree, don't queue it up - otherwise, if we, say, go into the Pedia from here, it'll end up BEHIND the tech tree
		if(!GC.GetEngineUserInterface()->IsPopupQueueEmpty())
			kPopup.iData1 = 1;

		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_TURN_LOG:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_NOTIFICATION_LOG, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_DOMESTIC_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ECONOMIC_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_VICTORY_SCREEN:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_VICTORY_INFO, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_INFO:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_DEMOGRAPHICS, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_ADVISOR_COUNSEL:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ADVISOR_COUNSEL);
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_ESPIONAGE_OVERVIEW:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_ESPIONAGE_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_RELIGION_OVERVIEW:
	{
		CvPopupInfo kPopup(BUTTONPOPUP_RELIGION_OVERVIEW, getActivePlayer());
		kPopup.iData1 = 1;
		GC.GetEngineUserInterface()->AddPopup(kPopup);
	}
	break;

	case CONTROL_RESTART_GAME:
	{
		gDLL->RestartGame();
	}
	break;

	default:
		CvAssertMsg(false, "eControl did not match any valid options");
		break;
	}
}

//	--------------------------------------------------------------------------------
bool CvGame::IsForceEndingTurn() const
{
	return m_bForceEndingTurn;
}

//	--------------------------------------------------------------------------------
void CvGame::SetForceEndingTurn(bool bValue)
{
	m_bForceEndingTurn = bValue;
}

//	--------------------------------------------------------------------------------
int CvGame::getAdjustedPopulationPercent(VictoryTypes eVictory) const
{
	int iPopulation;
	int iBestPopulation;
	int iNextBestPopulation;
	int iI;

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
	{
		return 0;
	}

	if(pkVictoryInfo->getPopulationPercentLead() == 0)
	{
		return 0;
	}

	if(getTotalPopulation() == 0)
	{
		return 100;
	}

	iBestPopulation = 0;
	iNextBestPopulation = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			iPopulation = GET_TEAM((TeamTypes)iI).getTotalPopulation();

			if(iPopulation > iBestPopulation)
			{
				iNextBestPopulation = iBestPopulation;
				iBestPopulation = iPopulation;
			}
			else if(iPopulation > iNextBestPopulation)
			{
				iNextBestPopulation = iPopulation;
			}
		}
	}

	return std::min(100, (((iNextBestPopulation * 100) / getTotalPopulation()) + pkVictoryInfo->getPopulationPercentLead()));
}


//	--------------------------------------------------------------------------------
int CvGame::getProductionPerPopulation(HurryTypes eHurry)
{
	if(NO_HURRY == eHurry)
		return 0;

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
		return 0;

	return (pkHurryInfo->getProductionPerPopulation());
}


//	--------------------------------------------------------------------------------
int CvGame::getAdjustedLandPercent(VictoryTypes eVictory) const
{
	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
		return 0;

	if(pkVictoryInfo->getLandPercent() == 0)
	{
		return 0;
	}

	int iPercent = pkVictoryInfo->getLandPercent();

	iPercent -= (countCivTeamsEverAlive() * 2);

	return std::max(iPercent, pkVictoryInfo->getMinLandPercent());
}

//	--------------------------------------------------------------------------------
int CvGame::countCivPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countCivPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countCivTeamsAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countCivTeamsEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countHumanPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countHumanPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvGame::countSeqHumanTurnsUntilPlayerTurn( PlayerTypes playerID ) const
{//This function counts the number of sequential human player turns that remain before this player's turn.
	int humanTurnsUntilMe = 0;
	bool startCountingPlayers = false;
	CvPlayer& targetPlayer = GET_PLAYER(playerID);
	if(targetPlayer.isSimultaneousTurns())
	{//target player is playing simultaneous turns and is not actually in the sequential turn sequence.
		//Count every human player in sequential turn mode who is taking or hasn't taken their turn.
		for(int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer& kCurrentPlayer = GET_PLAYER((PlayerTypes)i);
			if(kCurrentPlayer.isHuman() 
				&& kCurrentPlayer.isAlive() 
				&& !kCurrentPlayer.isSimultaneousTurns())
			{//another human player who is playing sequential turns.
				if(kCurrentPlayer.isTurnActive())
				{//This player is currently playing their turn. Start counting human players after this point.
					startCountingPlayers = true;
					humanTurnsUntilMe++;
				}
				else if(startCountingPlayers)
				{//This is a human player who's before us in line.
					humanTurnsUntilMe++;
				}
			}
		}	
	}
	else
	{//target player is playing sequential turns.  
		//Our next turn will begin after every sequential player has finished this turn 
		//AND everyone ahead of us in the sequence has finished their turn for the NEXT turn.

		//Starting after us, count every player who's playing sequential turns.
		startCountingPlayers = false;
		int curPlayerIdx = (targetPlayer.GetID()+1)%MAX_PLAYERS;
		for(int i = 0; i < MAX_PLAYERS; curPlayerIdx = ++curPlayerIdx%MAX_PLAYERS, ++i)
		{
			CvPlayer& kCurrentPlayer = GET_PLAYER((PlayerTypes)curPlayerIdx);
			if(kCurrentPlayer.GetID() == targetPlayer.GetID())
			{//This is us.  We've looped back to ourself.  We're done.
				break;
			}
			else if(kCurrentPlayer.isHuman() 
				&& kCurrentPlayer.isAlive() 
				&& !kCurrentPlayer.isSimultaneousTurns())
			{//another human player who is playing sequential turns.
				if(kCurrentPlayer.isTurnActive())
				{//This player is currently playing their turn. Start counting human players after this point.
					startCountingPlayers = true;
					humanTurnsUntilMe++;
				}
				else if(startCountingPlayers)
				{//This is a human player who's before us in line.
					humanTurnsUntilMe++;
				}
			}
		}	
	}

	return humanTurnsUntilMe;
}

//	--------------------------------------------------------------------------------
int CvGame::countMajorCivsAlive() const
{
	int iCount = 0;

	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(!GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvGame::countMajorCivsEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			if(!GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvGame::countTotalCivPower()
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getPower();
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countTotalNukeUnits()
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getNumNukeUnits();
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvGame::countKnownTechNumTeams(TechTypes eTech)
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isEverAlive())
		{
			if(GET_TEAM((TeamTypes)iI).GetTeamTechs()->HasTech(eTech))
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvGame::goldenAgeLength() const
{
	int iLength;

	iLength = /*10*/ GC.getGOLDEN_AGE_LENGTH();

	iLength *= getGameSpeedInfo().getGoldenAgePercent();
	iLength /= 100;

	return iLength;
}

//	--------------------------------------------------------------------------------
int CvGame::victoryDelay(VictoryTypes eVictory) const
{
	CvAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	CvAssert(pkVictoryInfo);

	if(pkVictoryInfo == NULL)
		return 0;

	int iLength = pkVictoryInfo->getVictoryDelayTurns();

	iLength *= getGameSpeedInfo().getVictoryDelayPercent();
	iLength /= 100;

	return iLength;
}


//	--------------------------------------------------------------------------------
int CvGame::getImprovementUpgradeTimeMod(ImprovementTypes eImprovement, const CvPlot* pPlot) const
{
	int iTime = 100;

	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pPlot != NULL && NULL != pkImprovementInfo)
	{
		if(pPlot->isRiver())
		{
			if(pkImprovementInfo->GetRiverSideUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetRiverSideUpgradeMod();
				iTime /= 100;
			}
		}

		if(pPlot->isCoastalLand())
		{
			if(pkImprovementInfo->GetCoastalLandUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetCoastalLandUpgradeMod();
				iTime /= 100;
			}
		}

		if(pPlot->isHills())
		{
			if(pkImprovementInfo->GetHillsUpgradeMod() > 0)
			{
				iTime *= pkImprovementInfo->GetHillsUpgradeMod();
				iTime /= 100;
			}
		}
	}

	return iTime;
}

//	--------------------------------------------------------------------------------
int CvGame::getImprovementUpgradeTime(ImprovementTypes eImprovement, const CvPlot* pPlot) const
{
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	CvAssert(pkImprovementInfo);

	if(pkImprovementInfo == NULL)
		return 0;

	int iTime = pkImprovementInfo->GetUpgradeTime();

	iTime *= getImprovementUpgradeTimeMod(eImprovement, pPlot);
	iTime /= 100;

	iTime *= getGameSpeedInfo().getImprovementPercent();
	iTime /= 100;

	iTime *= getStartEraInfo().getImprovementPercent();
	iTime /= 100;

	return iTime;
}



//	--------------------------------------------------------------------------------
bool CvGame::canTrainNukes() const
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		if(GET_PLAYER(ePlayer).isAlive())
		{
			for(int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
			{
				const UnitTypes eUnit = static_cast<UnitTypes>(iJ);
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
				if(pkUnitInfo)
				{
					if(pkUnitInfo->GetNukeDamageLevel() != -1)
					{
						if(GET_PLAYER(ePlayer).canTrain(eUnit))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
EraTypes CvGame::getCurrentEra() const
{
	int iEra;
	int iCount;
	int iI;

	iEra = 0;
	iCount = 0;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && GET_TEAM((TeamTypes)iI).isMajorCiv())
		{
			iEra += GET_TEAM((TeamTypes)iI).GetCurrentEra();
			iCount++;
		}
	}

	if(iCount > 0)
	{
		return ((EraTypes)(iEra / iCount));
	}

	return NO_ERA;
}


//	--------------------------------------------------------------------------------
TeamTypes CvGame::getActiveTeam()
{
	PlayerTypes eActivePlayer = getActivePlayer();
	if(eActivePlayer == NO_PLAYER)
	{
		return NO_TEAM;
	}
	else
	{
		return (TeamTypes)GET_PLAYER(eActivePlayer).getTeam();
	}
}


//	--------------------------------------------------------------------------------
CivilizationTypes CvGame::getActiveCivilizationType()
{
	PlayerTypes eActivePlayer = getActivePlayer();
	if(eActivePlayer == NO_PLAYER)
	{
		return NO_CIVILIZATION;
	}
	else
	{
		return (CivilizationTypes)GET_PLAYER(eActivePlayer).getCivilizationType();
	}
}


#if defined(MOD_API_EXTENSIONS)
//	--------------------------------------------------------------------------------
bool CvGame::isReallyNetworkMultiPlayer() const
{
	return CvPreGame::isReallyNetworkMultiPlayer();
}
#endif

//	--------------------------------------------------------------------------------
bool CvGame::isNetworkMultiPlayer() const
{
	return CvPreGame::isNetworkMultiplayerGame();
}


//	--------------------------------------------------------------------------------
bool CvGame::isGameMultiPlayer() const
{
	return (isNetworkMultiPlayer() || isPbem() || isHotSeat());
}


//	--------------------------------------------------------------------------------
bool CvGame::isTeamGame() const
{
	CvAssert(countCivPlayersAlive() >= countCivTeamsAlive());
	return (countCivPlayersAlive() > countCivTeamsAlive());
}

//	--------------------------------------------------------------------------------
/// Return control to the user after an autoplay
void CvGame::ReviveActivePlayer()
{
	if(!(GET_PLAYER(getActivePlayer()).isAlive()))
	{
		setAIAutoPlay(0, m_eAIAutoPlayReturnPlayer);

		// If no player specified, returning as an observer
		if(m_eAIAutoPlayReturnPlayer == NO_PLAYER)
		{
			CvPreGame::setSlotClaim(getActivePlayer(), SLOTCLAIM_ASSIGNED);
			CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);
		}

		// Want to return as a specific player
		else
		{
			// Reset observer slot
			CvPreGame::setSlotClaim(getActivePlayer(), SLOTCLAIM_UNASSIGNED);
			CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);

			// Move the active player to the desired slot
			CvPreGame::setSlotStatus(m_eAIAutoPlayReturnPlayer, SS_TAKEN);
			setActivePlayer(m_eAIAutoPlayReturnPlayer, false /*bForceHotSeat*/, true /*bAutoplaySwitch*/);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvGame::getNumHumanPlayers()
{
	return CvPreGame::numHumans();
}


//	--------------------------------------------------------------------------------
int CvGame::GetNumMinorCivsEver()
{
	int iNumCivs = 0;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMinorLoop).isEverAlive())
		{
			iNumCivs++;
		}
	}

	return iNumCivs;
}

//	--------------------------------------------------------------------------------
int CvGame::GetNumMinorCivsAlive()
{
	int iNumCivs = 0;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMinorLoop).isAlive())
		{
			iNumCivs++;
		}
	}

	return iNumCivs;
}

//	--------------------------------------------------------------------------------
int CvGame::getNumHumansInHumanWars(PlayerTypes ignorePlayer)
{//returns the number of human players who are currently at war with other human players.
	int humansWarringHumans = 0;
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		const CvPlayer& curPlayer = GET_PLAYER((PlayerTypes)i);
		if(curPlayer.isAlive() 
			&& curPlayer.isHuman() 
			&& (ignorePlayer == NO_PLAYER || curPlayer.GetID() != ignorePlayer)  //ignore the ignore player
			&& GET_TEAM(curPlayer.getTeam()).isAtWarWithHumans())
		{
			++humansWarringHumans;
		}
	}
	return humansWarringHumans;
}

//	--------------------------------------------------------------------------------
int CvGame::getNumSequentialHumans(PlayerTypes ignorePlayer)
{//returns the number of human players who are playing sequential turns.
	int seqHumans = 0;
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		const CvPlayer& curPlayer = GET_PLAYER((PlayerTypes)i);
		if(curPlayer.isAlive() 
			&& curPlayer.isHuman() 
			&& !curPlayer.isSimultaneousTurns()
			&& (ignorePlayer == NO_PLAYER || curPlayer.GetID() != ignorePlayer))  //ignore the ignore player
		{
			++seqHumans;
		}
	}
	return seqHumans;
}

//	------------------------------------------------------------------------------------------------
int CvGame::getGameTurn() const
{
	return CvPreGame::gameTurn();
}

//	------------------------------------------------------------------------------------------------
void CvGame::setGameTurn(int iNewValue)
{
	if(getGameTurn() != iNewValue)
	{
		std::string turnMessage = std::string("Game Turn ") + FSerialization::toString(iNewValue) + std::string("\n");
		gDLL->netMessageDebugLog(turnMessage);

		CvPreGame::setGameTurn(iNewValue);
		CvAssert(getGameTurn() >= 0);

		setScoreDirty(true);

		GC.GetEngineUserInterface()->setDirty(TurnTimer_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		m_sentAutoMoves = false;
		gDLL->GameplayTurnChanged(iNewValue);
		endTurnTimerReset();
	}
}

//	------------------------------------------------------------------------------------------------
void CvGame::incrementGameTurn()
{
	setGameTurn(getGameTurn() + 1);
}


//	--------------------------------------------------------------------------------
int CvGame::getTurnYear(int iGameTurn)
{
	// moved the body of this method to Game Core Utils so we have access for other games than the current one (replay screen in HOF)
	return getTurnYearForGame(iGameTurn, getStartYear(), getCalendar(), getGameSpeedType());
}


//	--------------------------------------------------------------------------------
int CvGame::getGameTurnYear()
{
	return getTurnYear(getGameTurn());
}


//	--------------------------------------------------------------------------------
int CvGame::getElapsedGameTurns() const
{
	return m_iElapsedGameTurns;
}


//	--------------------------------------------------------------------------------
void CvGame::incrementElapsedGameTurns()
{
	m_iElapsedGameTurns++;
}


//	--------------------------------------------------------------------------------
int CvGame::getMaxTurns() const
{
	return CvPreGame::maxTurns();
}


//	--------------------------------------------------------------------------------
void CvGame::setMaxTurns(int iNewValue)
{
	CvPreGame::setMaxTurns(iNewValue);
	CvAssert(getMaxTurns() >= 0);
}


//	--------------------------------------------------------------------------------
void CvGame::changeMaxTurns(int iChange)
{
	setMaxTurns(getMaxTurns() + iChange);
}


//	--------------------------------------------------------------------------------
int CvGame::getMaxCityElimination() const
{
	return CvPreGame::maxCityElimination();
}


//	--------------------------------------------------------------------------------
void CvGame::setMaxCityElimination(int iNewValue)
{
	CvPreGame::setMaxCityElimination(iNewValue);
	CvAssert(getMaxCityElimination() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getNumAdvancedStartPoints() const
{
	return CvPreGame::advancedStartPoints();
}


//	--------------------------------------------------------------------------------
void CvGame::setNumAdvancedStartPoints(int iNewValue)
{
	CvPreGame::setAdvancedStartPoints(iNewValue);
	CvAssert(getNumAdvancedStartPoints() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getStartTurn() const
{
	return m_iStartTurn;
}

//	--------------------------------------------------------------------------------
void CvGame::setStartTurn(int iNewValue)
{
	m_iStartTurn = iNewValue;
}

//	--------------------------------------------------------------------------------
int CvGame::GetWinningTurn() const
{
	return m_iWinningTurn;
}

//	--------------------------------------------------------------------------------
void CvGame::SetWinningTurn(int iNewValue)
{
	m_iWinningTurn = iNewValue;
}


//	--------------------------------------------------------------------------------
int CvGame::getStartYear() const
{
	return m_iStartYear;
}


//	--------------------------------------------------------------------------------
void CvGame::setStartYear(int iNewValue)
{
	m_iStartYear = iNewValue;
}


//	--------------------------------------------------------------------------------
int CvGame::getEstimateEndTurn() const
{
	return m_iEstimateEndTurn;
}


//	--------------------------------------------------------------------------------
void CvGame::setEstimateEndTurn(int iNewValue)
{
	m_iEstimateEndTurn = iNewValue;
}

//	--------------------------------------------------------------------------------
int CvGame::getDefaultEstimateEndTurn() const
{
	return m_iDefaultEstimateEndTurn;
}


//	--------------------------------------------------------------------------------
void CvGame::setDefaultEstimateEndTurn(int iNewValue)
{
	m_iDefaultEstimateEndTurn = iNewValue;
}


//	--------------------------------------------------------------------------------
int CvGame::getTurnSlice() const
{
	return m_iTurnSlice;
}


//	--------------------------------------------------------------------------------
int CvGame::getMinutesPlayed() const
{
	return (getTurnSlice() / gDLL->getTurnsPerMinute());
}


//	--------------------------------------------------------------------------------
void CvGame::setTurnSlice(int iNewValue)
{
	m_iTurnSlice = iNewValue;
}


//	--------------------------------------------------------------------------------
void CvGame::changeTurnSlice(int iChange)
{
	setTurnSlice(getTurnSlice() + iChange);
}

//	--------------------------------------------------------------------------------
void CvGame::resetTurnTimer(bool resetGameTurnStart)
{
	m_curTurnTimer.Start();
	m_fCurrentTurnTimerPauseDelta = 0;
	if(resetGameTurnStart)
	{
		m_timeSinceGameTurnStart.Start();
	}
}

//	--------------------------------------------------------------------------------
int CvGame::getMaxTurnLen()
{//returns the amount of time players are being given for this turn.
	if(getPitbossTurnTime() != 0)
	{//manually set turn time.
		if(isPitboss())
		{// Turn time is in hours
			return (getPitbossTurnTime() * 3600);
		}
		else
		{
			return getPitbossTurnTime();
		}
	}
	else
	{
		int iMaxUnits = 0;
		int iMaxCities = 0;

		// Find out who has the most units and who has the most cities
		// Calculate the max turn time based on the max number of units and cities
		for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			if(GET_PLAYER((PlayerTypes)i).isAlive())
			{
				if(GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
				{
					iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
				}
				if(GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
				{
					iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
				}
			}
		}

		// Now return turn len based on base len and unit and city resources
		const CvTurnTimerInfo& kTurnTimer = CvPreGame::turnTimerInfo();
		int baseTurnTime = (kTurnTimer.getBaseTime() +
		        (kTurnTimer.getCityResource() * iMaxCities) +
		        (kTurnTimer.getUnitResource() * iMaxUnits));
		
		return baseTurnTime;
	}
}

//	--------------------------------------------------------------------------------
bool CvGame::IsStaticTutorialActive() const
{
	return m_bStaticTutorialActive;
}

//	--------------------------------------------------------------------------------
void CvGame::SetStaticTutorialActive(bool bStaticTutorialActive)
{
	m_bStaticTutorialActive = bStaticTutorialActive;
}

//	--------------------------------------------------------------------------------
bool CvGame::HasAdvisorMessageBeenSeen(const char* szAdvisorMessageName)
{
	std::tr1::hash<std::string> hasher;
	std::tr1::unordered_set<size_t>::iterator it = m_AdvisorMessagesViewed.find( hasher(szAdvisorMessageName) );
	return it != m_AdvisorMessagesViewed.end();
}

//	--------------------------------------------------------------------------------
void CvGame::SetAdvisorMessageHasBeenSeen(const char* szAdvisorMessageName, bool bSeen)
{
	std::tr1::hash<std::string> hasher;
	if(bSeen)
	{
		m_AdvisorMessagesViewed.insert( hasher(szAdvisorMessageName) );
	}
	else
	{
		m_AdvisorMessagesViewed.erase( hasher(szAdvisorMessageName) );
	}
}

//	--------------------------------------------------------------------------------
bool CvGame::IsCityScreenBlocked()
{
	return CvPreGame::IsCityScreenBlocked();
}

//	--------------------------------------------------------------------------------
bool CvGame::CanOpenCityScreen(PlayerTypes eOpener, CvCity* pCity)
{
	if(eOpener == pCity->getOwner())
	{
		return true;
	}
#ifdef AUI_GAME_OBSERVER_CAN_OPEN_CITIES
	else if (GET_PLAYER(eOpener).isObserver())
	{
		return true;
	}
#endif
	else if (!GET_PLAYER(pCity->getOwner()).isMinorCiv() && (GET_PLAYER(eOpener).GetEspionage()->HasEstablishedSurveillanceInCity(pCity) || GET_PLAYER(eOpener).GetEspionage()->IsAnySchmoozing(pCity)))
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvGame::getTargetScore() const
{
	return CvPreGame::targetScore();
}


//	--------------------------------------------------------------------------------
void CvGame::setTargetScore(int iNewValue)
{
	CvPreGame::setTargetScore(iNewValue);
	CvAssert(getTargetScore() >= 0);
}


//	--------------------------------------------------------------------------------
int CvGame::getNumGameTurnActive()
{
	int numActive = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive() && GET_PLAYER((PlayerTypes)i).isTurnActive())
		{
			++numActive;
		}
	}
	return numActive;
}


//	--------------------------------------------------------------------------------
int CvGame::countNumHumanGameTurnActive()
{
	int iCount;
	int iI;

	iCount = 0;

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			if(GET_PLAYER((PlayerTypes)iI).isTurnActive())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
void CvGame::changeNumGameTurnActive(int iChange, const std::string& why)
{
	char changeBuf[8] = {0};
	char activeBuf[8] = {0};
	sprintf_s(changeBuf, "%d", iChange);
	sprintf_s(activeBuf, "%d", getNumGameTurnActive());
	std::string output = "changeNumGameTurnActive(";
	output += changeBuf;
	output += ") m_iNumActive=";
	output += activeBuf;
	output += " : " + why;
	gDLL->netMessageDebugLog(output);
	CvAssert(getNumGameTurnActive() >= 0);
}


//	--------------------------------------------------------------------------------
int CvGame::getNumCities() const
{
	return m_iNumCities;
}


//	--------------------------------------------------------------------------------
int CvGame::getNumCivCities() const
{
	return (getNumCities() - GET_PLAYER(BARBARIAN_PLAYER).getNumCities());
}


//	--------------------------------------------------------------------------------
void CvGame::changeNumCities(int iChange)
{
	m_iNumCities = (m_iNumCities + iChange);
	CvAssert(getNumCities() >= 0);
}


//	--------------------------------------------------------------------------------
int CvGame::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


//	--------------------------------------------------------------------------------
void CvGame::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getTotalEconomicValue() const
{
	return m_iTotalEconomicValue;
}


//	--------------------------------------------------------------------------------
void CvGame::setTotalEconomicValue(int iChange)
{
	m_iTotalEconomicValue = iChange;
	CvAssert(getTotalEconomicValue() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getHighestEconomicValue() const
{
	return m_iHighestEconomicValue;
}


//	--------------------------------------------------------------------------------
void CvGame::setHighestEconomicValue(int iChange)
{
	m_iHighestEconomicValue = iChange;
	CvAssert(getHighestEconomicValue() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getNoNukesCount() const
{
	return m_iNoNukesCount;
}


//	--------------------------------------------------------------------------------
bool CvGame::isNoNukes() const
{
	return (getNoNukesCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvGame::changeNoNukesCount(int iChange)
{
	m_iNoNukesCount = (m_iNoNukesCount + iChange);
	CvAssert(getNoNukesCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvGame::getNukesExploded() const
{
	return m_iNukesExploded;
}


//	--------------------------------------------------------------------------------
void CvGame::changeNukesExploded(int iChange)
{
	m_iNukesExploded = (m_iNukesExploded + iChange);
}


//	--------------------------------------------------------------------------------
int CvGame::getMaxPopulation() const
{
	return m_iMaxPopulation;
}


//	--------------------------------------------------------------------------------
int CvGame::getInitPopulation() const
{
	return m_iInitPopulation;
}


//	--------------------------------------------------------------------------------
int CvGame::getInitLand() const
{
	return m_iInitLand;
}


//	--------------------------------------------------------------------------------
int CvGame::getInitTech() const
{
	return m_iInitTech;
}


//	--------------------------------------------------------------------------------
int CvGame::getInitWonders() const
{
	return m_iInitWonders;
}


//	--------------------------------------------------------------------------------
void CvGame::initScoreCalculation()
{
	// initialize score calculation
	int iMaxFood = 0;
	for(int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
		if(!pPlot->isWater() || pPlot->isAdjacentToLand(false))
		{
			iMaxFood += pPlot->calculateBestNatureYield(YIELD_FOOD, NO_PLAYER);
		}
	}
	m_iMaxPopulation = getPopulationScore(iMaxFood / std::max(1, GC.getFOOD_CONSUMPTION_PER_POPULATION()));
	if(NO_ERA != getStartEra())
	{
		const CvEraInfo& kStartEra = getStartEraInfo();
		int iNumSettlers = kStartEra.getStartingUnitMultiplier();
		m_iInitPopulation = getPopulationScore(iNumSettlers * (kStartEra.getFreePopulation() + 1));

		m_iInitLand = getLandPlotsScore(iNumSettlers *  AVG_CITY_PLOTS);
	}
	else
	{
		m_iInitPopulation = 0;
		m_iInitLand = 0;
	}

	m_iInitTech = 0;
	for(int i = 0; i < GC.getNumTechInfos(); i++)
	{
		const TechTypes eTech = static_cast<TechTypes>(i);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo)
		{
			if(pkTechInfo->GetEra() < getStartEra())
			{
				m_iInitTech += getTechScore(eTech);
			}
			else
			{
				// count all possible free techs as initial to lower the score from immediate retirement
				for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); iCiv++)
				{
					const CivilizationTypes eCivilization = static_cast<CivilizationTypes>(iCiv);
					CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
					if(pkCivilizationInfo)
					{
						if(pkCivilizationInfo->isPlayable())
						{
							if(pkCivilizationInfo->isCivilizationFreeTechs(i))
							{
								m_iInitTech += getTechScore(eTech);
								break;
							}
						}
					}
				}
			}
		}
	}
	m_iInitWonders = 0;
}


//	--------------------------------------------------------------------------------
int CvGame::getAIAutoPlay()
{
	return m_iAIAutoPlay;
}

//	--------------------------------------------------------------------------------
void CvGame::setAIAutoPlay(int iNewValue, PlayerTypes eReturnAsPlayer)
{
	int iOldValue;

	iOldValue = getAIAutoPlay();

	if(iOldValue != iNewValue)
	{
		m_iAIAutoPlay = std::max(0, iNewValue);
		m_eAIAutoPlayReturnPlayer = eReturnAsPlayer;

		if((iOldValue == 0) && (getAIAutoPlay() > 0))
		{
			int iObserver = NO_PLAYER;
			PlayerTypes activePlayer = CvPreGame::activePlayer();

			// Is active player already in an observer slot?
			if(CvPreGame::slotStatus(activePlayer) == SS_OBSERVER)
				return;

			for(int iI = 0; iI < MAX_PLAYERS; iI++)
				PrintPlayerInfo(iI);

			// Observer has to be in a major slot, the "higher" players don't get notifications etc, leads to a crash (see CvPlayer::Init())
			for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
			{
				// Found an observer slot
				if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER && (CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_UNASSIGNED || CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_RESERVED))
				{
					// Set current active player to a computer player
					CvPreGame::setSlotStatus(CvPreGame::activePlayer(), SS_COMPUTER);

					// Move the active player to the observer slot
					iObserver = iI;
					CvPreGame::setSlotClaim((PlayerTypes)iI, SLOTCLAIM_ASSIGNED);
					setActivePlayer((PlayerTypes)iI, false /*bForceHotSeat*/, true /*bAutoplaySwitch*/);

					break;
				}
			}

			if (iObserver==NO_PLAYER)
			{
				//try to find a closed slot. start from the end, so we reduce the chance 
				//to hit a killed player which might be revived. that would lead to problems.
				for(int iI = MAX_MAJOR_CIVS-1; iI > 0; iI--)
				{
					//open up a slot if required - it seems Really Advanced Setup does not leave observer slots but sets them all to closed
					if(CvPreGame::slotStatus((PlayerTypes)iI) == SS_CLOSED)
					{
						CvPreGame::setSlotStatus( (PlayerTypes)iI, SS_OBSERVER );
						GET_PLAYER((PlayerTypes)iI).init( (PlayerTypes)iI );
						//make sure the team flags are set correctly 
						GET_TEAM( GET_PLAYER((PlayerTypes)iI).getTeam() ).updateTeamStatus();
						//make sure the new player can see everything
						SetAllPlotsVisible( GET_PLAYER((PlayerTypes)iI).getTeam() );

						// Set current active player to a computer player
						CvPreGame::setSlotStatus(CvPreGame::activePlayer(), SS_COMPUTER);

						// Move the active player to the observer slot
						iObserver = iI;
						CvPreGame::setSlotClaim((PlayerTypes)iI, SLOTCLAIM_ASSIGNED);
						setActivePlayer((PlayerTypes)iI, false /*bForceHotSeat*/, true /*bAutoplaySwitch*/);
					}
				}
			}

			//if we didn't find a slot, replace the active player
			//don't know if that makes sense ...
			if (iObserver==NO_PLAYER)
			{
				GET_PLAYER(getActivePlayer()).killUnits();
				GET_PLAYER(getActivePlayer()).killCities();
				//make sure the new player can see everything
				SetAllPlotsVisible( GET_PLAYER(getActivePlayer()).getTeam() );
				CvPreGame::setSlotStatus(getActivePlayer(), SS_OBSERVER);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvGame::changeAIAutoPlay(int iChange)
{
	setAIAutoPlay(getAIAutoPlay() + iChange, m_eAIAutoPlayReturnPlayer);
}


//	--------------------------------------------------------------------------------
unsigned int CvGame::getInitialTime()
{
	return m_uiInitialTime;
}


//	--------------------------------------------------------------------------------
void CvGame::setInitialTime(unsigned int uiNewValue)
{
	m_uiInitialTime = uiNewValue;
}


//	--------------------------------------------------------------------------------
bool CvGame::isScoreDirty() const
{
	return m_bScoreDirty;
}


//	--------------------------------------------------------------------------------
void CvGame::setScoreDirty(bool bNewValue)
{
	m_bScoreDirty = bNewValue;
}

TechTypes CvGame::getOceanPassableTech() const
{
	return m_eTechAstronomy;
}

//	--------------------------------------------------------------------------------
bool CvGame::isCircumnavigated() const
{
	return m_bCircumnavigated;
}


//	--------------------------------------------------------------------------------
void CvGame::makeCircumnavigated()
{
	m_bCircumnavigated = true;
}

//	---------------------------------------------------------------------------
bool CvGame::circumnavigationAvailable() const
{
	if(isCircumnavigated())
	{
		return false;
	}

	CvMap& kMap = GC.getMap();

	if(!(kMap.isWrapX()) && !(kMap.isWrapY()))
	{
		return false;
	}

	if(kMap.getLandPlots() > ((kMap.numPlots() * 2) / 3))
	{
		return false;
	}

	return true;
}

//	---------------------------------------------------------------------------
/// Message from UI to gameplay about something that should happen with regards to diplomacy
void CvGame::DoFromUIDiploEvent(FromUIDiploEventTypes eEvent, PlayerTypes eAIPlayer, int iArg1, int iArg2)
{
#if defined(MOD_EVENTS_DIPLO_EVENTS)
	if (MOD_EVENTS_DIPLO_EVENTS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UiDiploEvent, eEvent, eAIPlayer, iArg1, iArg2);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(eEvent);
		args->Push(eAIPlayer);
		args->Push(iArg1);
		args->Push(iArg2);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UiDiploEvent", args.get(), bResult);
	}
#if defined(MOD_EVENTS_DIPLO_EVENTS)
	}
#endif

	gDLL->sendFromUIDiploEvent(eAIPlayer, eEvent, iArg1, iArg2);
}


//	--------------------------------------------------------------------------------
/// Set up diplo victory parameters
void CvGame::DoInitDiploVictory()
{
	DoUpdateDiploVictory();
}

//	--------------------------------------------------------------------------------
/// Update diplo victory parameters, such as how many votes are needed to win
void CvGame::DoUpdateDiploVictory()
{
	int iVotesForHost = 1;
	int iVotesPerCiv = 1;
	int iVotesPerCityState = 1;
	for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
	{
		LeagueSpecialSessionTypes e = (LeagueSpecialSessionTypes)i;
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(e);
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			if (pInfo->IsUnitedNations())
			{
				iVotesForHost = pInfo->GetHostDelegates();
				iVotesPerCiv = pInfo->GetCivDelegates();
				iVotesPerCityState = pInfo->GetCityStateDelegates();
			}
		}
	}

	float fCivsToCount = 0.0f;
	float fCityStatesToCount = 0.0f;
	for (uint i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes e = (PlayerTypes)i;
		CvPlayer* pPlayer = &GET_PLAYER(e);
		if (pPlayer != NULL && pPlayer->isEverAlive())
		{
			// Minor civ
			if (pPlayer->isMinorCiv())
			{
				// Bought out does not count (they are no longer in the pool of teams, cannot be liberated, etc.)
				if (!pPlayer->GetMinorCivAI()->IsBoughtOut())
				{
					if (pPlayer->isAlive())
					{
#if defined(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
						if(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
						{
							fCityStatesToCount += 1.34f;
						}
						else
						{
							fCityStatesToCount += 1.0f;
						}
#else
						fCityStatesToCount += 1.0f;
#endif
					}
					else
					{
#if defined(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
						if(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
						{
							fCityStatesToCount += 0.75f;
						}
						else
						{
							fCityStatesToCount += 0.5f;
						}
#else
						fCityStatesToCount += 0.5f;
#endif
					}
				}
			}
			// Major civ
			else
			{
				if (pPlayer->isAlive())
				{
#if defined(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					if(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					{
						fCivsToCount += 1.5f;
					}
					else
					{
						fCivsToCount += 1.0f;
					}
#endif
				}
				else
				{
#if defined(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					if(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					{
						fCivsToCount += 0.75f;
					}
					else
					{
						fCivsToCount += 0.5f;
					}
#else
					fCivsToCount += 0.5f;
#endif
				}
			}
		}
	}

	// Number of delegates needed to win increases the more civs and city-states there are in the game,
	// but these two scale differently since civs' delegates are harder to secure. These functions 
	// are based on a logarithmic regression.
	float fCivVotesPortion = (GC.getDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT() * (float)log(fCivsToCount)) + GC.getDIPLO_VICTORY_CIV_DELEGATES_CONSTANT();
	if (fCivVotesPortion < 0.0f)
	{
		fCivVotesPortion = 0.0f;
	}
	float fCityStateVotesPortion = (GC.getDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT() * (float)log(fCityStatesToCount)) + GC.getDIPLO_VICTORY_CS_DELEGATES_CONSTANT();
	if (fCityStateVotesPortion < 0.0f)
	{
		fCityStateVotesPortion = 0.0f;
	}

	int iVotesToWin = (int)floor(fCivVotesPortion + fCityStateVotesPortion);
	iVotesToWin = MAX(iVotesForHost + iVotesPerCiv + 1, iVotesToWin);
	iVotesToWin = MIN(iVotesForHost + (iVotesPerCiv * (int)fCivsToCount) + (iVotesPerCityState * (int)fCityStatesToCount), iVotesToWin);

	SetVotesNeededForDiploVictory(iVotesToWin);
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
/// How many votes are needed to win?
int CvGame::GetVotesNeededForDiploVictory() const
{
	return m_iVotesNeededForDiploVictory;
}

//	--------------------------------------------------------------------------------
/// How many votes are needed to win?
void CvGame::SetVotesNeededForDiploVictory(int iValue)
{
	if(iValue != GetVotesNeededForDiploVictory())
	{
		m_iVotesNeededForDiploVictory = iValue;
	}
}

//	--------------------------------------------------------------------------------
/// Is the UN active? (Diplo Victory)
bool CvGame::IsUnitedNationsActive()
{
	if (!GC.getGame().isOption(GAMEOPTION_NO_LEAGUES))
	{
		if (GetGameLeagues()->GetNumActiveLeagues() > 0)
		{
			CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				if (pLeague->IsUnitedNations())
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// United Nations diplo victory countdown
int CvGame::GetUnitedNationsCountdown() const
{
	return m_iUnitedNationsCountdown;
}

//	--------------------------------------------------------------------------------
/// United Nations diplo victory countdown
void CvGame::SetUnitedNationsCountdown(int iValue)
{
	if(iValue != GetUnitedNationsCountdown())
	{
		m_iUnitedNationsCountdown = iValue;
	}
}

//	--------------------------------------------------------------------------------
/// United Nations diplo victory countdown
void CvGame::ChangeUnitedNationsCountdown(int iChange)
{
	if(iChange != 0)
	{
		SetUnitedNationsCountdown(GetUnitedNationsCountdown() + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// How many diplo votes tallied
int CvGame::GetNumVictoryVotesTallied() const
{
	return m_iNumVictoryVotesTallied;
}

//	--------------------------------------------------------------------------------
/// How many diplo votes tallied
/// Preliminary votes will be counted, but will not trigger victory
void CvGame::SetNumVictoryVotesTallied(int iValue, bool /*bPreliminaryVote*/)
{
	m_iNumVictoryVotesTallied = iValue;

	// Vote completed?
	if(iValue > 0)
	{
		if(iValue == GetNumVictoryVotesExpected())
		{
		}
	}
}

//	--------------------------------------------------------------------------------
/// How many diplo votes tallied
/// Preliminary votes will be counted, but will not trigger victory
void CvGame::ChangeNumVictoryVotesTallied(int iChange, bool bPreliminaryVote)
{
	SetNumVictoryVotesTallied(GetNumVictoryVotesTallied() + iChange, bPreliminaryVote);
}

//	--------------------------------------------------------------------------------
/// How many diplo votes Expected
int CvGame::GetNumVictoryVotesExpected() const
{
	return m_iNumVictoryVotesExpected;
}

//	--------------------------------------------------------------------------------
/// How many diplo votes Expected
void CvGame::SetNumVictoryVotesExpected(int iValue)
{
	m_iNumVictoryVotesExpected = iValue;
}

//	--------------------------------------------------------------------------------
TeamTypes CvGame::GetVoteCast(TeamTypes eVotingTeam) const
{
	CvAssertMsg(eVotingTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	return (TeamTypes) m_aiVotesCast[eVotingTeam];
}

//	--------------------------------------------------------------------------------
/// Have eVotingTeam cast its UN vote for eVote
/// Preliminary votes will be counted, but will not trigger victory
void CvGame::SetVoteCast(TeamTypes eVotingTeam, TeamTypes eVote, bool bPreliminaryVote)
{
	CvAssertMsg(eVotingTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if(eVote != GetVoteCast(eVotingTeam))
	{
		m_aiVotesCast[eVotingTeam] = eVote;

		// Increment counts (unless we're resetting)
		if(eVote != NO_TEAM)
		{
			int iNumVotes = 1;

			ChangeNumVotesForTeam(eVote, iNumVotes);

			// Must call this last, as when enough votes are tallied the election ends immediately
			ChangeNumVictoryVotesTallied(1, bPreliminaryVote);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Get the last vote this team made, for record keeping
TeamTypes CvGame::GetPreviousVoteCast(TeamTypes eVotingTeam) const
{
	CvAssertMsg(eVotingTeam >= 0, "eVotingTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eVotingTeam is expected to be within maximum bounds (invalid Index)");
	if (eVotingTeam < 0 || eVotingTeam >= MAX_CIV_TEAMS) return NO_TEAM;

	return (TeamTypes) m_aiPreviousVotesCast[eVotingTeam];
}

//	--------------------------------------------------------------------------------
/// Set the last vote this team made, for record keeping
void CvGame::SetPreviousVoteCast(TeamTypes eVotingTeam, TeamTypes eVotingTarget)
{
	CvAssertMsg(eVotingTeam >= 0, "eVotingTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eVotingTeam < MAX_CIV_TEAMS, "eVotingTeam is expected to be within maximum bounds (invalid Index)");
	if (eVotingTeam < 0 || eVotingTeam >= MAX_CIV_TEAMS) return;

	if (eVotingTarget != GetPreviousVoteCast(eVotingTeam))
	{
		m_aiPreviousVotesCast[eVotingTeam] = eVotingTarget;
	}
}

//	--------------------------------------------------------------------------------
int CvGame::GetNumVotesForTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	return m_aiNumVotesForTeam[eTeam];
}

//	--------------------------------------------------------------------------------
void CvGame::SetNumVotesForTeam(TeamTypes eTeam, int iValue)
{
	CvAssertMsg(eTeam >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_CIV_TEAMS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if(iValue != GetNumVotesForTeam(eTeam))
	{
		m_aiNumVotesForTeam[eTeam] = iValue;
	}
}

//	--------------------------------------------------------------------------------
void CvGame::ChangeNumVotesForTeam(TeamTypes eTeam, int iChange)
{
	SetNumVotesForTeam(eTeam, GetNumVotesForTeam(eTeam) + iChange);
}


//	--------------------------------------------------------------------------------
Localization::String CvGame::GetDiploResponse(const char* szLeader, const char* szResponse, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2)
{
	//cvStopWatch stopWatch("GetDiploResponse");

	Localization::String response;

	if(m_pDiploResponseQuery == NULL)
	{
		//Directly reference Language_en_US is safe here since we're just looking for the tag
		//and not the actual text.
		const char* szSQL = "select Tag, Bias from Diplomacy_Responses, Language_en_US where (LeaderType = ? or LeaderType = 'GENERIC') and ResponseType = ? and Tag like Response";
		m_pDiploResponseQuery = new Database::Results();
		if(!GC.GetGameDatabase()->Execute(*m_pDiploResponseQuery, szSQL, strlen(szSQL)))
		{
			CvAssertMsg(false, "Failed to generate diplo response query.");
		}
	}

	//This is not the fastest thing out there...
	//The original design was to select a random text key (of uniform probability) from the set generated by the above SQL statement
	//(with the random selection actually part of the SQL statement)
	//
	//Jon had later decided that he wanted to specify a bias for each diplomacy response such that certain ones would
	//appear more frequently than others.
	//To implement this quickly, I convert the discrete distribution into a uniform distribution and select from that.
	//This implementation generates a ton of strings to store the text keys though and would benefit greatly from a "stack_string"
	//implementation.  For now though, it works, and the code is called so infrequently that it shouldn't be noticeable.
	//NOTE: Profiled on my machine to take 0.006965 seconds on average to complete.
	std::vector<string> probabilities;
	probabilities.reserve(512);

#if defined(MOD_NO_RANDOM_TEXT_CIVS)
    std::vector<int> biasList;

    Database::Results* tempDatabase;
    const char* callTemp = "select Tag, Bias from Diplomacy_Responses, Language_en_US where LeaderType = ? and ResponseType = ? and Tag like Response";
    tempDatabase = new Database::Results();
    if(!GC.GetGameDatabase()->Execute(*tempDatabase, callTemp, strlen(callTemp)))
    {
        CvAssertMsg(false, "Failed to generate diplo response query.");
    }

    tempDatabase->Bind(1, szLeader);
    tempDatabase->Bind(2, szResponse);

    int totbias=0;
    while(tempDatabase->Step())
    {
        const char* szTag = tempDatabase->GetText(0);
        int bias = tempDatabase->GetInt(1);
        totbias+=bias;
        biasList.push_back(totbias); 
        probabilities.push_back(szTag);
    }

    tempDatabase->Reset();
    delete tempDatabase; //Just to be safe

    int tempRand;
    unsigned int choice;

    if(!probabilities.empty())
    {
		tempRand = getSmallFakeRandNum(totbias, probabilities.size());
        for (choice=0; choice<biasList.size(); choice++){
            if(tempRand < biasList[choice]){
                break;
            }
        }
        response = Localization::Lookup(probabilities[choice].c_str());
        response << strOptionalKey1 << strOptionalKey2;
        return response;
    }

    m_pDiploResponseQuery->Bind(1, szLeader);
    m_pDiploResponseQuery->Bind(2, szResponse);

    totbias=0;
    biasList.clear(); //Just to be safe
    probabilities.clear(); //Just to be safe
    while(m_pDiploResponseQuery->Step())
    {
        const char* szTag = m_pDiploResponseQuery->GetText(0);
        int bias = m_pDiploResponseQuery->GetInt(1);
        totbias+=bias;
        biasList.push_back(totbias); 
        probabilities.push_back(szTag);
    }

    m_pDiploResponseQuery->Reset();


    if(!probabilities.empty())
    {
		tempRand = getSmallFakeRandNum(totbias, probabilities.size());
        for (choice=0; choice<biasList.size(); choice++){
            if(tempRand < biasList[choice]){
                break;
            }
        }
        response = Localization::Lookup(probabilities[choice].c_str());
        response << strOptionalKey1 << strOptionalKey2;
        return response;
    }
#else

	m_pDiploResponseQuery->Bind(1, szLeader);
	m_pDiploResponseQuery->Bind(2, szResponse);

	while(m_pDiploResponseQuery->Step())
	{
		const char* szTag = m_pDiploResponseQuery->GetText(0);
		int bias = m_pDiploResponseQuery->GetInt(1);
		for(int i = 0; i < bias; i++)
		{
			probabilities.push_back(szTag);
		}
	}

	m_pDiploResponseQuery->Reset();

	if(!probabilities.empty())
	{
		response = Localization::Lookup(probabilities[getAsyncRandNum(probabilities.size(), "Diplomacy Rand")].c_str());
		response << strOptionalKey1 << strOptionalKey2;
	}
#endif

	if(response.IsEmpty())
	{
		char szMessage[256];
		sprintf_s(szMessage, "Please send Jon this with your last 5 autosaves and what changelist # you're playing. Could not find diplomacy response. Leader - %s, Response - %s", szLeader, szResponse);
		CvAssertMsg(false, szMessage);
	}

	return response;
}


//	--------------------------------------------------------------------------------
bool CvGame::isDebugMode() const
{
	return m_bDebugModeCache;
}

//	--------------------------------------------------------------------------------
void CvGame::setFOW(bool bMode)
{
	m_bFOW = bMode;
}

//	--------------------------------------------------------------------------------
bool CvGame::getFOW()
{
	return m_bFOW;
}

//	--------------------------------------------------------------------------------
void CvGame::setDebugMode(bool bDebugMode)
{
	if(m_bDebugMode != bDebugMode)
		toggleDebugMode();
}

//	--------------------------------------------------------------------------------
void CvGame::toggleDebugMode()
{
	m_bDebugMode = ((m_bDebugMode) ? false : true);
	updateDebugModeCache();

	GC.getMap().updateVisibility();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(MinimapSection_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
void CvGame::updateDebugModeCache()
{
	m_bDebugModeCache = m_bDebugMode;
}

//	--------------------------------------------------------------------------------
int CvGame::getPitbossTurnTime() const
{
	return CvPreGame::pitBossTurnTime();
}

//	--------------------------------------------------------------------------------
void CvGame::setPitbossTurnTime(int iHours)
{
	CvPreGame::setPitBossTurnTime(iHours);
}


//	--------------------------------------------------------------------------------
bool CvGame::isHotSeat() const
{
	return CvPreGame::isHotSeat();
}

//	--------------------------------------------------------------------------------
bool CvGame::isPbem() const
{
	return CvPreGame::isPlayByEmail();
}

//	--------------------------------------------------------------------------------
bool CvGame::isPitboss() const
{
	return CvPreGame::isPitBoss();
}

//	--------------------------------------------------------------------------------
bool CvGame::isSimultaneousTeamTurns() const
{//When players are taking sequential turns, do they take them simultaneous with every member of their team?
 //WARNING:  This function doesn't indicate if a player is taking sequential turns or not.
	//		 Use CvPlayer::isSimultaneousTurns() to determine that.
	if(!isNetworkMultiPlayer())
	{
		return false;
	}

	if(!isOption(GAMEOPTION_DYNAMIC_TURNS) && isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{//truely simultaneous turn mode doesn't do this.
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvGame::isFinalInitialized() const
{
	return m_bFinalInitialized;
}


//	--------------------------------------------------------------------------------
void CvGame::setFinalInitialized(bool bNewValue)
{
	if(isFinalInitialized() != bNewValue)
	{
		m_bFinalInitialized = bNewValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvGame::getPbemTurnSent() const
{
	return m_bPbemTurnSent;
}


//	--------------------------------------------------------------------------------
void CvGame::setPbemTurnSent(bool bNewValue)
{
	m_bPbemTurnSent = bNewValue;
}


//	--------------------------------------------------------------------------------
bool CvGame::getHotPbemBetweenTurns() const
{
	return m_bHotPbemBetweenTurns;
}


//	--------------------------------------------------------------------------------
void CvGame::setHotPbemBetweenTurns(bool bNewValue)
{
	m_bHotPbemBetweenTurns = bNewValue;
}


//	--------------------------------------------------------------------------------
bool CvGame::isPlayerOptionsSent() const
{
	return m_bPlayerOptionsSent;
}


//	--------------------------------------------------------------------------------
void CvGame::sendPlayerOptions(bool bForce)
{
	if(getActivePlayer() == NO_PLAYER)
	{
		return;
	}

	if(!isPlayerOptionsSent() || bForce)
	{
		m_bPlayerOptionsSent = true;

		gDLL->BeginSendBundle();
		for(int iI = 0; iI < GC.getNumPlayerOptionInfos(); iI++)
		{
			const PlayerOptionTypes eOption = static_cast<PlayerOptionTypes>(iI);

			CvPlayerOptionInfo* pkInfo = GC.getPlayerOptionInfo(eOption);
			if (pkInfo)
			{
				uint uiID = FString::Hash( pkInfo->GetType() );
				gDLL->sendPlayerOption(static_cast<PlayerOptionTypes>(uiID), gDLL->getPlayerOption(static_cast<PlayerOptionTypes>(uiID)));
			}
		}
		gDLL->EndSendBundle();
	}
}


//	--------------------------------------------------------------------------------
PlayerTypes CvGame::getActivePlayer() const
{
	return CvPreGame::activePlayer();
}


//	--------------------------------------------------------------------------------
void CvGame::setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat, bool bAutoplaySwitch)
{
	PlayerTypes eOldActivePlayer = getActivePlayer();
	if(eOldActivePlayer != eNewValue)
	{
		int iActiveNetId = ((NO_PLAYER != eOldActivePlayer) ? GET_PLAYER(eOldActivePlayer).getNetID() : -1);
		CvPreGame::setActivePlayer(eNewValue);
		gDLL->GameplayActivePlayerChanged(eNewValue);

		if(GET_PLAYER(eNewValue).isHuman() && (isHotSeat() || isPbem() || bForceHotSeat))
		{
			if(isHotSeat())
			{
				GC.GetEngineUserInterface()->setDirty(TurnTimer_DIRTY_BIT, true);
				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
				m_sentAutoMoves = false;
				resetTurnTimer();
				endTurnTimerReset();
			}

			gDLL->getPassword(eNewValue);
			setHotPbemBetweenTurns(false);

			if(NO_PLAYER != eOldActivePlayer)
			{
				int iInactiveNetId = GET_PLAYER(eNewValue).getNetID();
				GET_PLAYER(eNewValue).setNetID(iActiveNetId);
				GET_PLAYER(eOldActivePlayer).setNetID(iInactiveNetId);
			}

			if(countHumanPlayersAlive() == 1 && isPbem())
			{
				// Nobody else left alive
				CvPreGame::setGameType(GAME_HOTSEAT_MULTIPLAYER);
			}
		}

		if(isHotSeat() || bForceHotSeat || bAutoplaySwitch)
		{
			sendPlayerOptions(true);
		}

		if(GC.IsGraphicsInitialized())
		{
			// Publish the player change first
			// Messages will be sent out by the updating of the fog and they do not indicate
			// the player the update is for, so listeners will want to get the player change message first
			gDLL->PublishActivePlayer(eNewValue, eOldActivePlayer);

			CvMap& theMap = GC.getMap();
			theMap.updateFog();
			theMap.updateVisibility();

			ICvUserInterface2* theUI = GC.GetEngineUserInterface();
			theUI->setCanEndTurn(false);

			theUI->clearSelectedCities();
			theUI->ClearSelectionList();

			theUI->setDirty(PercentButtons_DIRTY_BIT, true);
			theUI->setDirty(ResearchButtons_DIRTY_BIT, true);
			theUI->setDirty(GameData_DIRTY_BIT, true);
			theUI->setDirty(MinimapSection_DIRTY_BIT, true);
			theUI->setDirty(CityInfo_DIRTY_BIT, true);
			theUI->setDirty(UnitInfo_DIRTY_BIT, true);

			//theUI->setDirty(NationalBorders_DIRTY_BIT, true);
			theUI->setDirty(BlockadedPlots_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
const CvHandicapInfo& CvGame::getHandicapInfo() const
{
	static CvHandicapInfo emptyResult;
	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(getHandicapType());
	if(pkHandicapInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid handicap!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
		return emptyResult;
	}
	else
		return *pkHandicapInfo;
}

HandicapTypes CvGame::getHandicapType() const
{
	return m_eHandicap;
}

void CvGame::setHandicapType(HandicapTypes eHandicap)
{
	m_eHandicap = eHandicap;
}

//	-----------------------------------------------------------------------------------------------
PlayerTypes CvGame::getPausePlayer()
{
	return m_ePausePlayer;
}

//	-----------------------------------------------------------------------------------------------
bool CvGame::isPaused()
{
	return (getPausePlayer() != NO_PLAYER);
}

//	-----------------------------------------------------------------------------------------------
void CvGame::setPausePlayer(PlayerTypes eNewValue)
{
	if(!isNetworkMultiPlayer())
	{
		// If we're not in Network MP, if the game is paused the turn timer is too.
		if(isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
		{
			if(eNewValue != NO_PLAYER && m_ePausePlayer == NO_PLAYER)
			{
				m_fCurrentTurnTimerPauseDelta += m_curTurnTimer.Stop();
				m_timeSinceGameTurnStart.Stop();
			}
			else if(eNewValue == NO_PLAYER && m_ePausePlayer != NO_PLAYER)
			{
				m_timeSinceGameTurnStart.Start();
				m_curTurnTimer.Start();
			}
		}
	}

	m_ePausePlayer = eNewValue;
}

//	-----------------------------------------------------------------------------------------------
UnitTypes CvGame::getBestLandUnit()
{
	return m_eBestLandUnit;
}

//	--------------------------------------------------------------------------------
int CvGame::getBestLandUnitCombat()
{
	const UnitTypes eBestLandUnit = getBestLandUnit();
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eBestLandUnit);
	if(pkUnitInfo)
	{
		return std::max(1, pkUnitInfo->GetCombat());
	}

	return 1;
}


//	--------------------------------------------------------------------------------
void CvGame::setBestLandUnit(UnitTypes eNewValue)
{
	if(getBestLandUnit() != eNewValue)
	{
		m_eBestLandUnit = eNewValue;

		GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
int CvGame::GetFaithCost(CvUnitEntry *pkUnit) const
{
	int iRtnValue = pkUnit->GetFaithCost();
	iRtnValue *= getGameSpeedInfo().getTrainPercent();
	iRtnValue /= 1000;   // Normalize costs like purchase system does
	iRtnValue *= 10;

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
TeamTypes CvGame::getWinner() const
{
	return m_eWinner;
}

//	--------------------------------------------------------------------------------
bool CvGame::IsGameWon() const
{
	return (getWinner() != NO_TEAM);
}

//	--------------------------------------------------------------------------------
VictoryTypes CvGame::getVictory() const
{
	return m_eVictory;
}
#if defined(MOD_BALANCE_CORE)
bool CvGame::isVictoryRandomizationDone() const
{
	return m_bVictoryRandomization;
}
void CvGame::setVictoryRandomizationDone(bool bValue)
{
	m_bVictoryRandomization = bValue;
}
#endif


//	--------------------------------------------------------------------------------
void CvGame::setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	if((getWinner() != eNewWinner) || (getVictory() != eNewVictory))
	{
		m_eWinner = eNewWinner;
		m_eVictory = eNewVictory;
		SetWinningTurn(getElapsedGameTurns());

		// Reset UN countdown if necessary
		SetUnitedNationsCountdown(0);

		if(getVictory() != NO_VICTORY && !IsStaticTutorialActive())
		{
			CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(getVictory());
			CvAssert(pkVictoryInfo);
			if(pkVictoryInfo == NULL)
				return;

			const char* szVictoryTextKey = pkVictoryInfo->GetTextKey();

			if(getWinner() != NO_TEAM)
			{
				const PlayerTypes winningTeamLeaderID = GET_TEAM(getWinner()).getLeaderID();
				CvPlayerAI& kWinningTeamLeader = GET_PLAYER(winningTeamLeaderID);
				const char* szWinningTeamLeaderNameKey = kWinningTeamLeader.getNameKey();

				Localization::String localizedText = Localization::Lookup("TXT_KEY_GAME_WON");
				localizedText << GET_TEAM(getWinner()).getName().GetCString() << szVictoryTextKey;
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, winningTeamLeaderID, localizedText.toUTF8(), -1, -1);

				//Notify everyone of the victory
				localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_WINNER");
				localizedText << szWinningTeamLeaderNameKey << szVictoryTextKey;

				Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_WINNER");
				localizedSummary << szWinningTeamLeaderNameKey;

				for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
					PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
					CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
					CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
					if(pNotifications){
						pNotifications->Add(NOTIFICATION_VICTORY, localizedText.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
					}
				}

#if !defined(NO_ACHIEVEMENTS)
				//--Start Achievements
				//--Don't allow most in multiplayer so friends can't achieve-whore it up together
				if(!GC.getGame().isGameMultiPlayer() && kWinningTeamLeader.isHuman() && kWinningTeamLeader.isLocalPlayer())
				{
					const bool bUsingDLC1Scenario = gDLL->IsModActivated(CIV5_DLC_01_SCENARIO_MODID);
					const bool bUsingDLC2Scenario = gDLL->IsModActivated(CIV5_DLC_02_SCENARIO_MODID) || gDLL->IsModActivated(CIV5_COMPLETE_SCENARIO1_MODID);
					const bool bUsingDLC3Scenario = gDLL->IsModActivated(CIV5_DLC_03_SCENARIO_MODID);
					const bool bUsingDLC4Scenario = gDLL->IsModActivated(CIV5_DLC_04_SCENARIO_MODID);
					const bool bUsingDLC5Scenario = gDLL->IsModActivated(CIV5_DLC_05_SCENARIO_MODID);
					const bool bUsingDLC6Scenario = gDLL->IsModActivated(CIV5_DLC_06_SCENARIO_MODID);

					const bool bUsingXP1Scenario1 = gDLL->IsModActivated(CIV5_XP1_SCENARIO1_MODID);
					const bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
					const bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);

					const bool bUsingXP2Scenario1 = gDLL->IsModActivated(CIV5_XP2_SCENARIO1_MODID);
					const bool bUsingXP2Scenario2 = gDLL->IsModActivated(CIV5_XP2_SCENARIO2_MODID);

					const VictoryTypes eVictorySpaceRace = static_cast<VictoryTypes>(1);
					const VictoryTypes eVictoryDomination = static_cast<VictoryTypes>(2);
					const VictoryTypes eVictoryCultural = static_cast<VictoryTypes>(3);
					const VictoryTypes eVictoryDiplomatic = static_cast<VictoryTypes>(4);

					const PolicyBranchTypes ePolicyBranchFreedom = static_cast<PolicyBranchTypes>(9);
					const PolicyBranchTypes ePolicyBranchOrder = static_cast<PolicyBranchTypes>(10);
					const PolicyBranchTypes ePolicyBranchAutocracy = static_cast<PolicyBranchTypes>(11);

					const HandicapTypes eHandicapDeity = static_cast<HandicapTypes>(7);

					//Games Won Stat
					gDLL->IncrementSteamStat(ESTEAMSTAT_TOTAL_WINS);

					//Victory on Map Sizes
					WorldSizeTypes	winnerMapSize = GC.getMap().getWorldSize();
					switch(winnerMapSize)
					{
					case WORLDSIZE_DUEL:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_DUEL);
						break;
					case WORLDSIZE_TINY:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_TINY);
						break;
					case WORLDSIZE_SMALL:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_SMALL);
						break;
					case WORLDSIZE_STANDARD:
						if(!bUsingXP2Scenario2 && !bUsingXP2Scenario1)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_STANDARD);
						}
						break;
					case WORLDSIZE_LARGE:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_LARGE);
						break;
					case WORLDSIZE_HUGE:
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPSIZE_HUGE);
						break;
					default:
						OutputDebugString("Playing on some other kind of world size.");
					}

					//Victory on Map Types
					CvString winnerMapName = CvPreGame::mapScriptName();
					//OutputDebugString(winnerMapName);
					//OutputDebugString("\n");

					if(winnerMapName == "Assets\\Maps\\Continents.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_CONTINENTS);
					else if(winnerMapName == "Assets\\Maps\\Pangaea.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_PANGAEA);
					else if(winnerMapName == "Assets\\Maps\\Archipelago.lua")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_ARCHIPELAGO);
					else if(winnerMapName == "Assets\\Maps\\Earth_Duel.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Huge.Civ5Map"
						|| winnerMapName == "Assets\\Maps\\Earth_Large.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Small.Civ5Map"
						|| winnerMapName == "Assets\\Maps\\Earth_Standard.Civ5Map" || winnerMapName == "Assets\\Maps\\Earth_Tiny.Civ5Map")
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPTYPE_EARTH);
					else
						OutputDebugString("\n Playing some other map. \n\n");


					//Victory on Difficulty Levels
					HandicapTypes winnerHandicapType = getHandicapType();
					if(!bUsingXP2Scenario1 && !bUsingXP2Scenario2) 
					{
						switch(winnerHandicapType)
						{
						case 0:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_SETTLER);
							break;
						case 1:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_CHIEFTAIN);
							break;
						case 2:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_WARLORD);
							break;
						case 3:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_PRINCE);
							break;
						case 4:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_KING);
							break;
						case 5:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_EMPEROR);
							break;
						case 6:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_IMMORTAL);
							break;
						case 7:
							gDLL->UnlockAchievement(ACHIEVEMENT_DIFLEVEL_DEITY);
							break;
						default:
							OutputDebugString("Playing on some non-existant dif level.");
						}
					}

					//Different Victory Win Types
					if(!bUsingXP2Scenario1 && !bUsingXP2Scenario2)
					{
						switch(eNewVictory)
						{
						case 0:
							OutputDebugString("No current Achievement for a time victory");
							break;
						case 1:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_SPACE);
							break;
						case 2:
							{
								if(!bUsingXP1Scenario1)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_DOMINATION);
								}
							}
							break;
						case 3:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_CULTURE);
							break;
						case 4:
							gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_DIPLO);
							break;
						default:
							OutputDebugString("Your l33t victory skills allowed you to win in some other way.");
						}
					}


					//Victory with Specific Leaders
					CvString pLeader =  kWinningTeamLeader.getLeaderTypeKey();

					if(!bUsingDLC6Scenario && pLeader == "LEADER_ALEXANDER")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ALEXANDER);
					else if(pLeader == "LEADER_WASHINGTON" && !bUsingXP2Scenario1)
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_WASHINGTON);
					else if(!bUsingXP1Scenario3 && !bUsingDLC4Scenario && !bUsingXP2Scenario1 && pLeader == "LEADER_ELIZABETH")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ELIZABETH);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario3 && !bUsingDLC4Scenario && pLeader == "LEADER_NAPOLEON")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_NAPOLEON);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario3 && !bUsingDLC4Scenario && pLeader == "LEADER_BISMARCK")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_BISMARCK);
					else if(!bUsingXP1Scenario3 && pLeader == "LEADER_CATHERINE")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_CATHERINE);

						if(bUsingXP1Scenario1 && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_42);
						}
					}
					else if(!bUsingXP1Scenario2 && !bUsingXP2Scenario2 && pLeader == "LEADER_AUGUSTUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_CAESAR);
					else if(!bUsingDLC6Scenario && pLeader == "LEADER_RAMESSES")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_RAMESSES);
					else if(!bUsingXP1Scenario2 && !bUsingXP1Scenario1 && pLeader == "LEADER_ASKIA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ASKIA);
					else if(!bUsingXP1Scenario1 && !bUsingDLC6Scenario && pLeader == "LEADER_HARUN_AL_RASHID")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_HARUN);
					else if(!bUsingDLC6Scenario && pLeader == "LEADER_DARIUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_DARIUS);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_GANDHI")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_GANDHI);
						if(eNewVictory == 3 && kWinningTeamLeader.getNumCities() <= 3) //Bollywood
							gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_BOLLYWOOD);
					}
					else if(pLeader == "LEADER_RAMKHAMHAENG")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_RAMKHAMHAENG);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_WU_ZETIAN")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_WU);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_ODA_NOBUNAGA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ODA);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_HIAWATHA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_HIAWATHA);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_MONTEZUMA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_MONTEZUMA);
					else if(!bUsingXP1Scenario1 && !bUsingDLC6Scenario && pLeader == "LEADER_SULEIMAN")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SULEIMAN);
					else if(pLeader == "LEADER_NEBUCHADNEZZAR")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_NEBUCHADNEZZAR);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_GENGHIS_KHAN")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_GENGHIS);
					}
					else if(pLeader == "LEADER_ISABELLA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ISABELLA);
					else if(pLeader == "LEADER_PACHACUTI")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_PACHACUTI);
					else if(!bUsingDLC3Scenario && pLeader == "LEADER_KAMEHAMEHA")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_KAMEHAMEHA);
					else if(pLeader == "LEADER_HARALD")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_BLUETOOTH);
					else if(!bUsingDLC5Scenario && pLeader == "LEADER_SEJONG")
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SEJONG);
					else if(pLeader == "LEADER_MARIA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_01);
					else if(!bUsingXP1Scenario2 && pLeader == "LEADER_THEODORA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_02);
					else if(pLeader == "LEADER_DIDO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_03);
					else if(pLeader == "LEADER_BOUDICCA")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_04);

						if(bUsingXP1Scenario1 && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_41);
						}
					}
					else if(pLeader == "LEADER_SELASSIE")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_05);
					else if(pLeader == "LEADER_ATTILA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_06);
					else if(pLeader == "LEADER_PACAL")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_07);
					else if(pLeader == "LEADER_WILLIAM")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_08);
					else if(!bUsingXP1Scenario3 && pLeader == "LEADER_GUSTAVUS_ADOLPHUS")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_09);
					else if(pLeader == "LEADER_ASHURBANIPAL" )
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_01);
					else if(pLeader == "LEADER_PEDRO")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_02);

						//Diplomatic Victory
						if(eNewVictory == eVictoryDiplomatic)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_20);
						}
					}
					else if(pLeader == "LEADER_GAJAH_MADA" )
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_03);
					else if(pLeader == "LEADER_AHMAD_ALMANSUR")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_04);
					else if(pLeader == "LEADER_CASIMIR")
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_05);

						//Space Victory
						if(eNewVictory == eVictorySpaceRace)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_23);
						}
					}
					else if(pLeader == "LEADER_MARIA_I")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_06);
					else if(pLeader == "LEADER_POCATELLO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_07);
					else if(pLeader == "LEADER_ENRICO_DANDOLO")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_08);
					else if(pLeader == "LEADER_SHAKA")
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_09);
					else
						OutputDebugString("\nPlaying with a non-standard leader.\n");

					//One City
					if(kWinningTeamLeader.getNumCities() == 1)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_ONECITY);
					}

					//Uber Achievements for unlocking other achievements
					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_DUEL) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_TINY) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_SMALL) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_STANDARD) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_LARGE) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPSIZE_HUGE) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_ARCHIPELAGO) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_CONTINENTS) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_EARTH) &&  gDLL->IsAchievementUnlocked(ACHIEVEMENT_MAPTYPE_PANGAEA))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_MAPS_ALL);
					}
					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_CULTURE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_SPACE) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_DIPLO) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_VICTORY_DOMINATION))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_VICTORY_ALL);
					}
					if(gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_WASHINGTON) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ELIZABETH) && gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_NAPOLEON)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_BISMARCK)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_CATHERINE)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_CAESAR)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ALEXANDER)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_RAMESSES)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ASKIA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_HARUN)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_DARIUS)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_GANDHI)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_RAMKHAMHAENG)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_WU)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_ODA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_HIAWATHA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_MONTEZUMA)&& gDLL->IsAchievementUnlocked(ACHIEVEMENT_WIN_SULEIMAN))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_WIN_ALLBASELEADERS);
					}

					//Victory/Ideology Achievements
					const PolicyBranchTypes eBranch =kWinningTeamLeader.GetPlayerPolicies()->GetLateGamePolicyTree();
					if(eBranch != NO_POLICY_BRANCH_TYPE)
					{
						if(eNewVictory == eVictorySpaceRace)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_10);
							}
							else if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_11);
							}
						}
						else if(eNewVictory == eVictoryDomination)
						{
							if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_17);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_18);
							}
						}
						else if(eNewVictory == eVictoryCultural)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_12);
							}
							else if(eBranch == ePolicyBranchOrder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_13);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_14);
							}
						}
						else if(eNewVictory == eVictoryDiplomatic)
						{
							if(eBranch == ePolicyBranchFreedom)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_15);
							}
							else if(eBranch == ePolicyBranchAutocracy)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_16);
							}
						}
					}

					// World Congress related Achievements
					if (GetGameLeagues()->GetNumActiveLeagues() > 0)
					{
						CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
						if (pLeague)
						{
							// Diplomatic Victory without ever being host
							if (eNewVictory == eVictoryDiplomatic)
							{
								if (!pLeague->HasMemberEverBeenHost(kWinningTeamLeader.GetID()))
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP2_43);
								}
							}
						}
					}

					//Check for PSG
					CvAchievementUnlocker::Check_PSG();

					//DLC1 Scenario Win Achievements
					if(bUsingDLC1Scenario)
					{
						if(eNewVictory == 2)	//Only win by domination victory
						{
							CvString strHandicapType = this->getHandicapInfo().GetType();

							//All easier difficulty level achievements are unlocked when you beat it on a harder difficulty level.
							bool bBeatOnHarderDifficulty = false;

							if(strHandicapType == "HANDICAP_DEITY")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_DEITY);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_IMMORTAL")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_IMMORTAL);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_EMPEROR")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_EMPEROR);
								bBeatOnHarderDifficulty = true;
							}

							if(bBeatOnHarderDifficulty || strHandicapType == "HANDICAP_KING")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_KING);
								bBeatOnHarderDifficulty = true;
							}

							//Despite it's name, this achievement is for any difficulty.
							gDLL->UnlockAchievement(ACHIEVEMENT_WIN_SCENARIO_01_PRINCE_OR_BELOW);
						}
					}

					//DLC2 Scenario Win Achievements
					if(bUsingDLC2Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_SPAIN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_SPAIN);
						else if(strCivType == "CIVILIZATION_FRANCE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_FRANCE);
						else if(strCivType == "CIVILIZATION_ENGLAND")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_ENGLAND);
						else if(strCivType == "CIVILIZATION_INCA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_INCA);
						else if(strCivType == "CIVILIZATION_AZTEC")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_AZTECS);
						else if(strCivType == "CIVILIZATION_IROQUOIS")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_WIN_IROQUOIS);
					}

					//DLC3 Scenario Win Achievements
					if(bUsingDLC3Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_POLYNESIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_HIVA);
						else if(strCivType == "CIVILIZATION_IROQUOIS")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_TAHITI);
						else if(strCivType == "CIVILIZATION_INDIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_SAMOA);
						else if(strCivType == "CIVILIZATION_AZTEC")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_03_WIN_TONGA);
					}

					//DLC4 Scenario Win Achievements
					if(bUsingDLC4Scenario)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_DENMARK")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_DENMARK);
						else if(strCivType == "CIVILIZATION_ENGLAND")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_ENGLAND);
						else if(strCivType == "CIVILIZATION_GERMANY")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_NORWAY);
						else if(strCivType == "CIVILIZATION_FRANCE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_NORMANDY);

						switch(winnerHandicapType)
						{
						case 5:	//	Win scenario on Emperor (any civ)  YOU! The Conqueror
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_EMPEROR);
							break;
						case 6:	//	Win scenario on Immortal (any civ)  Surviving Domesday
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_IMMORTAL);
							break;
						case 7:	//	Win scenario on Deity (any civ)  Surviving Ragnarok
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_WIN_DEITY);
							break;
						}
					}

					//DLC5 Scenario Win Achievements
					if(bUsingDLC5Scenario)
					{
						// Civilization
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_JAPAN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_JAPAN);
						else if(strCivType == "CIVILIZATION_KOREA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_KOREA);
						else if(strCivType == "CIVILIZATION_CHINA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_CHINA);
						else if(strCivType == "CIVILIZATION_MONGOL")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_MANCHU);

						// Difficulty
						switch(winnerHandicapType)
						{
						case 5: // Emperor
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_EMPEROR);
							break;
						case 6: // Immortal
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_IMMORTAL);
							break;
						case 7: // Deity
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_DEITY);
							break;
						}

						// Win in less than 100 turns
						if(getGameTurn() >= 0 && getGameTurn() < 100)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_WIN_100TURNS);
						}

					}

					//DLC6 Scenario Win Achievements
					if(bUsingDLC6Scenario)
					{
						// Civilization
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_OTTOMAN")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_HITTITES);
						else if(strCivType == "CIVILIZATION_GREECE")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_GREECE);
						else if(strCivType == "CIVILIZATION_ARABIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_SUMER);
						else if(strCivType == "CIVILIZATION_EGYPT")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_EGYPT);
						else if(strCivType == "CIVILIZATION_PERSIA")
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_PERSIA);

						// Difficulty
						switch(winnerHandicapType)
						{
						case 3: // Prince
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_PRINCE);
							break;
						case 4: // King
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_KING);
							break;
						case 5:	// Emperor
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_EMPEROR);
							break;
						case 6:	// Immortal
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_IMMORTAL);
							break;
						case 7:	// Deity
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_06_WIN_DEITY);
							break;
						}

					}

					if(bUsingXP1Scenario1)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_21);

						if(pLeader == "LEADER_HARUN_AL_RASHID" && winnerHandicapType >= 5)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_43);
						}
					}

					if(bUsingXP1Scenario2)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_22);

						if(winnerHandicapType >= 7) //Deity or higher
						{
							if(pLeader == "LEADER_THEODORA")
							{
								typedef std::pair<int,int> Location;
								typedef std::tr1::array<Location, 27> OriginalCitiesArray;
								OriginalCitiesArray OriginalCities = {
									Location(65,6), // Alexandria
									Location(48,7), // Cyrene
									Location(71,9),  // Caesarea
									Location(79,13), // Palmyra
									Location(74,14), // Damascus
									Location(49,18), // Mistra
									Location(68,20), // Seleucia
									Location(73,20), // Antioch
									Location(63,21), // Attalia
									Location(76,21), // Edessa
									Location(51,22), // Athens
									Location(47,23), // Nicopolis
									Location(57,23), // Ephesus
									Location(67,24), // Iconium
									Location(75,26), // Melitene
									Location(63,27), // Dorylaeum
									Location(50,28), // Thessalonica
									Location(58,29), // Constantinople
									Location(45,30), // Dyrrachium
									Location(71,30), // Amasia
									Location(54,31), // Adrianople
									Location(64,32), // Amastris
									Location(78,32), // Theodosiopolis
									Location(49,34), // Naissus
									Location(69,34), // Sinope
									Location(75,34), // Trebizond
									Location(45,35), // Sirmium	

								};					

								CvMap& kMap = GC.getMap();
								const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
								bool bHasAllCities = true;

								//Test if we still own each city.
								for(OriginalCitiesArray::iterator it = OriginalCities.begin(); it != OriginalCities.end(); ++it)
								{
									CvPlot* pkPlot = kMap.plot(it->first, it->second);
									if(pkPlot != NULL)
									{
										CvCity* pkCity = pkPlot->getPlotCity();
										if(pkCity != NULL)
										{
											if(pkCity->getOwner() == eActivePlayer)
											{
												continue;
											}
										}
									}

									bHasAllCities = false;
								}

								if(bHasAllCities)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_50);
								}
							}
							else if(pLeader == "LEADER_AUGUSTUS")
							{
								typedef std::pair<int,int> Location;
								typedef std::tr1::array<Location, 29> OriginalCitiesArray;
								OriginalCitiesArray OriginalCities = {
									Location(10,16), // Russadir
									Location(15,18), // Iol Caesarea
									Location(30,18), // Carthage
									Location(37,19), // Syracuse
									Location(4,23),  // Gades
									Location(13,25), // Carthago Nova
									Location(37,27), // Neapoli
									Location(41,27), // Brundisium
									Location(1,29),  // Olisipo
									Location(6,29),  // Emerita Augusta
									Location(15,30), // Tarraco
									Location(33,31), // Rome
									Location(14,34), // Caesaraugusta
									Location(20,36), // Narbo
									Location(24,36), // Massilia
									Location(40,36), // Salonae
									Location(28,37), // Genua
									Location(34,37), // Ravenna
									Location(3,39),  // Brigantium
									Location(30,40), // Milan
									Location(16,42), // Mediolanum
									Location(22,42), // Lundunum
									Location(32,45), // Casta Regina
									Location(41,45), // Carnutum
									Location(14,47), // Portus Namnetum
									Location(21,48), // Lutetia
									Location(26,49), // Trier
									Location(17,52), // Coriallum
									Location(22,53)  // Gesoriacum
								};

								CvMap& kMap = GC.getMap();
								const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
								bool bHasAllCities = true;

								//Test if we still own each city.
								for(OriginalCitiesArray::iterator it = OriginalCities.begin(); it != OriginalCities.end(); ++it)
								{
									CvPlot* pkPlot = kMap.plot(it->first, it->second);
									if(pkPlot != NULL)
									{
										CvCity* pkCity = pkPlot->getPlotCity();
										if(pkCity != NULL)
										{
											if(pkCity->getOwner() == eActivePlayer)
											{
												continue;
											}
										}
									}

									bHasAllCities = false;
								}

								if(bHasAllCities)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_50);
								}
							}
						}
					}

					if(bUsingXP1Scenario3)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_23);
					}


					if(bUsingXP2Scenario1)
					{
						CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_AMERICA")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_56);
						}
						else
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_57);
						}

						// Difficulty
						if (winnerHandicapType == eHandicapDeity)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_58);
						}
					}

					if(bUsingXP2Scenario2)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_49);

						// Difficulty
						if (winnerHandicapType == eHandicapDeity)
						{
							CvString strCivType = kWinningTeamLeader.getCivilizationInfo().GetType();
							if(strCivType == "CIVILIZATION_AMERICA")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_50);
							}
							else if(strCivType == "CIVILIZATION_OTTOMAN")
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_51);
							}
						}
					}
				}
				//Win any multiplayer game
				if(GC.getGame().isGameMultiPlayer() && kWinningTeamLeader.isHuman() && (GET_PLAYER(GC.getGame().getActivePlayer()).GetID() == kWinningTeamLeader.GetID()))
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_WIN_MULTIPLAYER);
				}
#endif

			}

			if((getAIAutoPlay() > 0) || gDLL->GetAutorun())
			{
				setGameState(GAMESTATE_EXTENDED);
			}
			else
			{
				setGameState(GAMESTATE_OVER);
			}
		}

		GC.GetEngineUserInterface()->setDirty(Center_DIRTY_BIT, true);

		GC.GetEngineUserInterface()->setDirty(Soundtrack_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
// Check last slot to see if there is still a Victory slot open
bool CvGame::isVictoryAvailable(VictoryTypes eVictory) const
{
	if(getTeamVictoryRank(eVictory, GC.getNUM_VICTORY_POINT_AWARDS() - 1) == NO_TEAM)
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// What's the next victory slot available to be won?
int CvGame::GetNextAvailableVictoryCompetitionRank(VictoryTypes eVictory) const
{
	for(int iRankLoop = 0; iRankLoop < GC.getNUM_VICTORY_POINT_AWARDS(); iRankLoop++)
	{
		if(getTeamVictoryRank(eVictory, iRankLoop) == NO_TEAM)
		{
			return iRankLoop;
		}
	}

	return -1;
}


//	--------------------------------------------------------------------------------
void CvGame::DoPlaceTeamInVictoryCompetition(VictoryTypes eNewVictory, TeamTypes eTeam)
{
	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eNewVictory);
	if(pkVictoryInfo == NULL)
		return;

	// Is there a slot open for this new Team?
	if(isVictoryAvailable(eNewVictory))
	{
		CvTeam& kTeam = GET_TEAM(eTeam);

		// Loop through and find first available slot
		for(int iSlotLoop = 0; iSlotLoop < GC.getNUM_VICTORY_POINT_AWARDS(); iSlotLoop++)
		{
			if(getTeamVictoryRank(eNewVictory, iSlotLoop) == NO_TEAM)
			{
				setTeamVictoryRank(eNewVictory, iSlotLoop, eTeam);
				int iNumPoints = pkVictoryInfo->GetVictoryPointAward(iSlotLoop);
				kTeam.changeVictoryPoints(iNumPoints);
				kTeam.setVictoryAchieved(eNewVictory, true);

				Localization::String youWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_YOU");
				Localization::String youWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_YOU");
				Localization::String someoneWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_SOMEBODY");
				Localization::String someoneWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_SOMEBODY");
				Localization::String unmetWonInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_VICTORY_RACE_WON_UNMET");
				Localization::String unmetWonSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_VICTORY_RACE_WON_UNMET");

				for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
					PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
					CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
					CvNotifications* pNotification = kCurNotifyPlayer.GetNotifications();
					if(pNotification)
					{
						Localization::String localizedText;
						Localization::String localizedSummary;

						const char* szVictoryTextKey = pkVictoryInfo->GetTextKey();
						// Active Team
						if(eTeam == kCurNotifyPlayer.getTeam())
						{
							localizedText = youWonInfo;
							localizedText << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = youWonSummary;
							localizedSummary << iSlotLoop+1 << szVictoryTextKey;
						}
						// Met Team
						else if(GET_TEAM(kCurNotifyPlayer.getTeam()).isHasMet(eTeam))
						{
							const char* szTeamLeaderNameKey = GET_PLAYER(kTeam.getLeaderID()).getNameKey();

							localizedText = someoneWonInfo;
							localizedText << szTeamLeaderNameKey << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = someoneWonSummary;
							localizedSummary << szTeamLeaderNameKey << iSlotLoop+1 << szVictoryTextKey;
						}
						// Unmet Team
						else
						{
							localizedText = unmetWonInfo;
							localizedText << iSlotLoop+1 << szVictoryTextKey << iNumPoints;
							localizedSummary = unmetWonSummary;
							localizedSummary << iSlotLoop+1 << szVictoryTextKey;
						}
						pNotification->Add(NOTIFICATION_VICTORY, localizedText.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
					}
				}

				break;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
TeamTypes CvGame::getTeamVictoryRank(VictoryTypes eNewVictory, int iRank) const
{
	CvAssert(iRank >= 0);
	CvAssert(iRank < GC.getNUM_VICTORY_POINT_AWARDS());

	return (TeamTypes) m_ppaaiTeamVictoryRank[eNewVictory][iRank];
}


//	--------------------------------------------------------------------------------
void CvGame::setTeamVictoryRank(VictoryTypes eNewVictory, int iRank, TeamTypes eTeam)
{
	CvAssert(iRank >= 0);
	CvAssert(iRank < GC.getNUM_VICTORY_POINT_AWARDS());

	m_ppaaiTeamVictoryRank[eNewVictory][iRank] = eTeam;
}

//	--------------------------------------------------------------------------------
/// Returns the Average Military Might of all Players in the game
int CvGame::GetWorldMilitaryStrengthAverage(PlayerTypes ePlayer, bool bIncludeMe, bool bIncludeOnlyKnown)
{
	int iWorldMilitaryStrength = 0;
	int iNumAlivePlayers = 0;

	// Look at our military strength relative to everyone else in the world
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
		{
			if(bIncludeMe || iMajorLoop != ePlayer)
			{
				if(!bIncludeOnlyKnown || (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam())))
				{
					iNumAlivePlayers++;
					iWorldMilitaryStrength += GET_PLAYER((PlayerTypes) iMajorLoop).GetMilitaryMight();
				}
			}
		}
	}

	if(iNumAlivePlayers > 0)
	{
		iWorldMilitaryStrength /= iNumAlivePlayers;
	}

	return iWorldMilitaryStrength;
}

//	--------------------------------------------------------------------------------
/// Returns the cached Total Research cost for the Religion Victory Competition
int CvGame::GetTotalReligionTechCost() const
{
	return m_iTotalReligionTechCost;
}

//	--------------------------------------------------------------------------------
/// Updates the cached Total Research cost for the Religion Victory Competition
void CvGame::DoUpdateTotalReligionTechCost()
{
	TechTypes ePrereqTech = GetReligionTech();

	// If it's been removed, then we're done here
	if(ePrereqTech == NO_TECH)
	{
		m_iTotalReligionTechCost = -1;
		return;
	}

	if(ePrereqTech != NO_TECH)
	{
		m_iTotalReligionTechCost = GetResearchLeftToTech(NO_TEAM, ePrereqTech);
	}
}

//	--------------------------------------------------------------------------------
/// Returns the cached World Average Tech Progress towards the Religion Victory Competition
int CvGame::GetCachedWorldReligionTechProgress() const
{
	return m_iCachedWorldReligionTechProgress;
}

//	--------------------------------------------------------------------------------
/// Updates the cached World Average Tech Progress towards the Religion Victory Competition
void CvGame::DoUpdateCachedWorldReligionTechProgress()
{
	TechTypes ePrereqTech = GetReligionTech();

	// If it's been removed, then we're done here
	if(ePrereqTech == NO_TECH)
	{
		m_iCachedWorldReligionTechProgress = -1;
		return;
	}

	TeamTypes eTeam;
	int iNumTeams = 0;

	int iResearchLeftToReligionTech = 0;

	// Look at every Team's progress
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eTeam = (TeamTypes) iTeamLoop;

		if(GET_TEAM(eTeam).isAlive() && !GET_TEAM(eTeam).isMinorCiv())
		{
			iNumTeams++;
			iResearchLeftToReligionTech += GetResearchLeftToReligionTech(eTeam);
		}
	}

	CvAssert(iNumTeams > 0);
	m_iCachedWorldReligionTechProgress = iResearchLeftToReligionTech / iNumTeams;
}

//	--------------------------------------------------------------------------------
/// What Tech unlocks the Religion Race?
TechTypes CvGame::GetReligionTech() const
{
	return m_eReligionTech;
}

//	--------------------------------------------------------------------------------
/// Sets what Tech unlocks the Religion Race
void CvGame::SetReligionTech(TechTypes eTech)
{
	// This function should only be called once under normal circumstances
	CvAssert(m_eReligionTech == NO_TECH);

	m_eReligionTech = eTech;
}

//	--------------------------------------------------------------------------------
/// How much Research is left to get to the Tech which unlocks the Religious Race
int CvGame::GetResearchLeftToReligionTech(TeamTypes eTeam)
{
	TechTypes eReligionTech = GetReligionTech();

	// Didn't find anything
	if(eReligionTech == NO_TECH)
	{
		return -1;
	}

	int iResearchLeft = GetResearchLeftToTech(eTeam, eReligionTech);

	return iResearchLeft;
}


//	--------------------------------------------------------------------------------
/// Recursive function to see how much Research is left to get to a Tech
int CvGame::GetResearchLeftToTech(TeamTypes eTeam, TechTypes eTech)
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);

	//THIS SHOULD NEVER HAPPEN!
	if(pkTechInfo == NULL)
		return 0;

	// Base Case - first Prereq AND Tech does not exist
	if(pkTechInfo->GetPrereqAndTechs(0) == NO_TECH)
	{
		return pkTechInfo->GetResearchCost();
	}

	// Another base case! - Team already has tech
	if(eTeam != NO_TEAM && GET_TEAM(eTeam).GetTeamTechs()->HasTech(eTech))
	{
		return 0;
	}

	int iPrereqTechCost = 0;
	TechTypes ePreReq;

	for(int i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes) pkTechInfo->GetPrereqAndTechs(i);

		if(ePreReq != NO_TECH)
		{
			iPrereqTechCost += GetResearchLeftToTech(eTeam, ePreReq);
		}
	}

	return pkTechInfo->GetResearchCost() + iPrereqTechCost;
}

//	--------------------------------------------------------------------------------
GameStateTypes CvGame::getGameState()
{
	return m_eGameState;
}


//	--------------------------------------------------------------------------------
void CvGame::setGameState(GameStateTypes eNewValue)
{
	if(getGameState() != eNewValue)
	{
		m_eGameState = eNewValue;

#if !defined(NO_ACHIEVEMENTS)
		if(eNewValue == GAMESTATE_OVER || eNewValue == GAMESTATE_EXTENDED)
		{
			if (!isGameMultiPlayer())
			{
				if (GetGameLeagues()->GetNumActiveLeagues() > 0)
				{
					CvLeague* pLeague = GetGameLeagues()->GetActiveLeague();
					if (pLeague)
					{
						if (pLeague->HasMemberAlwaysBeenHost(getActivePlayer()))
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_41);
						}
					}
				}
			}
		}
#endif

		if(eNewValue == GAMESTATE_OVER)
		{

#if !defined(NO_ACHIEVEMENTS)
			if(!isGameMultiPlayer())
			{
				bool bLocalPlayerLost = true;

				const TeamTypes eWinningTeam = getWinner();
				if(eWinningTeam != NO_TEAM)
				{
					CvPlayerAI& kLocalPlayer = GET_PLAYER(getActivePlayer());
					if(eWinningTeam == kLocalPlayer.getTeam())
						bLocalPlayerLost = false;
				}
				if(bLocalPlayerLost)
				{
					//Handle any local losing achievements here.

					//Are we playing DLC_01 - Mongol Scenario??
					bool bUsingDLC1MongolScenario = gDLL->IsModActivated(CIV5_DLC_01_SCENARIO_MODID);
					if(bUsingDLC1MongolScenario && getGameTurn() == 100)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_LOSE_SCENARIO_01);
					}

					//DLC_05 - Achievement for losing Invasion of Korea Scenario as Korea
					bool bUsingDLC5Scenario = gDLL->IsModActivated(CIV5_DLC_05_SCENARIO_MODID);
					if(bUsingDLC5Scenario) // && getGameTurn() == 100)
					{
						CvPlayerAI& kLocalPlayer = GET_PLAYER(getActivePlayer());
						CvString strCivType = kLocalPlayer.getCivilizationInfo().GetType();
						if(strCivType == "CIVILIZATION_KOREA")
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_05_LOSE_KOREA);
						}
					}
				}
			}
#endif

			//Write out time spent playing.
			long iHours = getMinutesPlayed() / 60;
			long iMinutes = getMinutesPlayed() % 60;

			for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
			{
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
				if(player.isHuman())
				{
					addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, (PlayerTypes)iI, GetLocalizedText("TXT_KEY_MISC_TIME_SPENT", iHours, iMinutes));

				}
			}

			saveReplay();
			showEndGameSequence();
		}

		GC.GetEngineUserInterface()->setDirty(Cursor_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
const CvGameSpeedInfo& CvGame::getGameSpeedInfo() const
{
	static CvGameSpeedInfo emptyResult;
	CvGameSpeedInfo* pkGameSpeedInfo = GC.getGameSpeedInfo(getGameSpeedType());
	if(pkGameSpeedInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid game speed!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);

		return emptyResult;
	}
	else
		return *pkGameSpeedInfo;
}

//	--------------------------------------------------------------------------------
GameSpeedTypes CvGame::getGameSpeedType() const
{
	return CvPreGame::gameSpeed();
}

//	--------------------------------------------------------------------------------
const CvEraInfo& CvGame::getStartEraInfo() const
{
	static CvEraInfo emptyResult;

	CvEraInfo* pkStartEraInfo = GC.getEraInfo(getStartEra());
	if(pkStartEraInfo == NULL)
	{
		const char* szError = "ERROR: Game does not contain valid start era!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);

		return emptyResult;
	}
	else
		return *pkStartEraInfo;
}

//	--------------------------------------------------------------------------------
EraTypes CvGame::getStartEra() const
{
	return CvPreGame::era();
}

//	--------------------------------------------------------------------------------
CalendarTypes CvGame::getCalendar() const
{
	return CvPreGame::calendar();
}


//	--------------------------------------------------------------------------------
int CvGame::getEndTurnMessagesReceived(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEndTurnMessagesReceived[iIndex];
}


//	--------------------------------------------------------------------------------
void CvGame::incrementEndTurnMessagesReceived(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEndTurnMessagesReceived[iIndex]++;
}


//	--------------------------------------------------------------------------------
PlayerTypes CvGame::getRankPlayer(int iRank)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");
	return (PlayerTypes)m_aiRankPlayer[iRank];
}


//	--------------------------------------------------------------------------------
void CvGame::setRankPlayer(int iRank, PlayerTypes ePlayer)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");

	if(getRankPlayer(iRank) != ePlayer)
	{
		m_aiRankPlayer[iRank] = ePlayer;
	}
}


//	--------------------------------------------------------------------------------
int CvGame::getPlayerRank(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerRank[ePlayer];
}


//	--------------------------------------------------------------------------------
void CvGame::setPlayerRank(PlayerTypes ePlayer, int iRank)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	m_aiPlayerRank[ePlayer] = iRank;
	CvAssert(getPlayerRank(ePlayer) >= 0);
}


//	--------------------------------------------------------------------------------
int CvGame::getPlayerScore(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerScore[ePlayer];
}


//	--------------------------------------------------------------------------------
void CvGame::setPlayerScore(PlayerTypes ePlayer, int iScore)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(getPlayerScore(ePlayer) != iScore)
	{
		m_aiPlayerScore[ePlayer] = iScore;
		CvAssert(getPlayerScore(ePlayer) >= 0);

		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
TeamTypes CvGame::getRankTeam(int iRank)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");
	return (TeamTypes)m_aiRankTeam[iRank];
}


//	--------------------------------------------------------------------------------
void CvGame::setRankTeam(int iRank, TeamTypes eTeam)
{
	CvAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	CvAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");

	if(getRankTeam(iRank) != eTeam)
	{
		m_aiRankTeam[iRank] = eTeam;
	}
}


//	--------------------------------------------------------------------------------
int CvGame::getTeamRank(TeamTypes eTeam)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamRank[eTeam];
}


//	--------------------------------------------------------------------------------
void CvGame::setTeamRank(TeamTypes eTeam, int iRank)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamRank[eTeam] = iRank;
	CvAssert(getTeamRank(eTeam) >= 0);
}


//	--------------------------------------------------------------------------------
int CvGame::getTeamScore(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamScore[eTeam];
}


//	--------------------------------------------------------------------------------
void CvGame::setTeamScore(TeamTypes eTeam, int iScore)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamScore[eTeam] = iScore;
	CvAssert(getTeamScore(eTeam) >= 0);
}


//	--------------------------------------------------------------------------------
bool CvGame::isOption(GameOptionTypes eIndex) const
{
	int i = 0;
	CvPreGame::GetGameOption(eIndex, i);

	return (i == 1);
}

//	--------------------------------------------------------------------------------
bool CvGame::isOption(const char* pszOption) const
{
	int i = 0;
	CvPreGame::GetGameOption(pszOption, i);

	return (i == 1);
}

//	--------------------------------------------------------------------------------
void CvGame::setOption(GameOptionTypes eIndex, bool bEnabled)
{
	CvPreGame::SetGameOption(eIndex, (int)bEnabled);
}

//	--------------------------------------------------------------------------------
void CvGame::setOption(const char* pszOption, bool bEnabled)
{
	CvPreGame::SetGameOption(pszOption, (int)bEnabled);
}


//	--------------------------------------------------------------------------------
bool CvGame::isMPOption(MultiplayerOptionTypes eIndex) const
{
	return CvPreGame::multiplayerOptionEnabled(eIndex);
}


//	--------------------------------------------------------------------------------
void CvGame::setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled)
{
	CvPreGame::setMultiplayerOption(eIndex, bEnabled);
}

//	--------------------------------------------------------------------------------
int CvGame::getUnitCreatedCount(UnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCreatedCount[eIndex];
}


//	--------------------------------------------------------------------------------
void CvGame::incrementUnitCreatedCount(UnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitCreatedCount[eIndex]++;
}


//	--------------------------------------------------------------------------------
int CvGame::getUnitClassCreatedCount(UnitClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCreatedCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvGame::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eIndex);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if(!isWorldUnitClass(eIndex))
	{
		return false;
	}

	CvAssertMsg(getUnitClassCreatedCount(eIndex) <= pkUnitClassInfo->getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getUnitClassCreatedCount(eIndex) + iExtra) >= pkUnitClassInfo->getMaxGlobalInstances());
}


//	--------------------------------------------------------------------------------
void CvGame::incrementUnitClassCreatedCount(UnitClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCreatedCount[eIndex]++;
}


//	--------------------------------------------------------------------------------
int CvGame::getBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCreatedCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvGame::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
	{
		CvAssertMsg(false, "BuildingClassInfo does not exist for type.  NOT GOOD");
		return false;
	}


	if(!isWorldWonderClass(*pkBuildingClassInfo))
	{
		return false;
	}

	CvAssertMsg(getBuildingClassCreatedCount(eIndex) <= pkBuildingClassInfo->getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getBuildingClassCreatedCount(eIndex) + iExtra) >= pkBuildingClassInfo->getMaxGlobalInstances());
}


//	--------------------------------------------------------------------------------
void CvGame::incrementBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCreatedCount[eIndex]++;
}

//	--------------------------------------------------------------------------------
void CvGame::decrementBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCreatedCount[eIndex]--;
}


//	--------------------------------------------------------------------------------
int CvGame::getProjectCreatedCount(ProjectTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectCreatedCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvGame::isProjectMaxedOut(ProjectTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(!isWorldProject(eIndex))
	{
		return false;
	}

	CvAssertMsg(getProjectCreatedCount(eIndex) <= GC.getProjectInfo(eIndex)->GetMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getProjectCreatedCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex)->GetMaxGlobalInstances());
}


//	--------------------------------------------------------------------------------
void CvGame::incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectCreatedCount[eIndex] += iExtra;
}

//	--------------------------------------------------------------------------------
bool CvGame::isVictoryValid(VictoryTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return CvPreGame::isVictory(eIndex);
}

//	--------------------------------------------------------------------------------
void CvGame::setVictoryValid(VictoryTypes eIndex, bool bValid)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvPreGame::setVictory(eIndex, bValid);
}

//	--------------------------------------------------------------------------------
bool CvGame::areNoVictoriesValid() const
{
	bool bRtnValue = true;

	for(int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		VictoryTypes eVictory = static_cast<VictoryTypes>(iI);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
		if(pkVictoryInfo)
		{
			if(isVictoryValid(eVictory))
			{
				bRtnValue = false;
				break;
			}
		}
	}

	return bRtnValue;
}

//	--------------------------------------------------------------------------------
bool CvGame::isSpecialUnitValid(SpecialUnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabSpecialUnitValid[eIndex];
}


//	--------------------------------------------------------------------------------
void CvGame::makeSpecialUnitValid(SpecialUnitTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabSpecialUnitValid[eIndex] = true;
}


//	--------------------------------------------------------------------------------
bool CvGame::isNukesValid() const
{
	return m_bNukesValid;
}


//	--------------------------------------------------------------------------------
void CvGame::makeNukesValid(bool bValid)
{
	m_bNukesValid = bValid;
}

//	--------------------------------------------------------------------------------
bool CvGame::isInAdvancedStart() const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if((GET_PLAYER((PlayerTypes)iPlayer).getAdvancedStartPoints() >= 0) && GET_PLAYER((PlayerTypes)iPlayer).isHuman())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
const CvString& CvGame::getName()
{
	return CvPreGame::gameName();
}


//	--------------------------------------------------------------------------------
void CvGame::setName(const char* szName)
{
	CvPreGame::setGameName(szName);
}

//	--------------------------------------------------------------------------------
bool CvGame::isDestroyedCityName(CvString& szName) const
{
	std::tr1::hash<std::string> hasher;
	std::vector<size_t>::const_iterator it = std::find(m_aszDestroyedCities.begin(), m_aszDestroyedCities.end(), hasher(szName.c_str()));
	return it != m_aszDestroyedCities.end();
}

//	--------------------------------------------------------------------------------
void CvGame::addDestroyedCityName(const CvString& szName)
{
	std::tr1::hash<std::string> hasher;
	m_aszDestroyedCities.push_back( hasher(szName.c_str()) );
}

//	--------------------------------------------------------------------------------
bool CvGame::isGreatPersonBorn(CvString& szName) const
{
	std::tr1::hash<std::string> hasher;
	std::vector<size_t>::const_iterator it = std::find(m_aszGreatPeopleBorn.begin(), m_aszGreatPeopleBorn.end(), hasher(szName.c_str()));
	return it != m_aszGreatPeopleBorn.end();
}

//	--------------------------------------------------------------------------------
void CvGame::addGreatPersonBornName(const CvString& szName)
{
	std::tr1::hash<std::string> hasher;
	m_aszGreatPeopleBorn.push_back( hasher(szName.c_str()) );
}

#if defined(MOD_API_EXTENSIONS)
//	--------------------------------------------------------------------------------
void CvGame::removeGreatPersonBornName(const CvString& szName)
{
	std::tr1::hash<std::string> hasher;
	m_aszGreatPeopleBorn.erase(std::remove(m_aszGreatPeopleBorn.begin(), m_aszGreatPeopleBorn.end(), hasher(szName.c_str())), m_aszGreatPeopleBorn.end());
}
#endif

// Protected Functions...

//	--------------------------------------------------------------------------------
void CvGame::doTurn()
{
#if defined(MOD_BALANCE_CORE) && defined(MOD_UNIT_KILL_STATS)
	GC.getMap().DoKillCountDecay();
#endif

	//create an autosave at turn start
	if(!isNetworkMultiPlayer())
		gDLL->AutoSave(false, false);

	// END OF TURN

	//We reset the turn timer now so that we know that the turn timer has been reset at least once for
	//this turn.  CvGameController::Update() will continue to reset the timer if there is prolonged ai processing.
	resetTurnTimer(true);

	m_processPlayerAutoMoves = false; // Starts out as false and gets set to true in updateMoves but was never getting set back to false if MP simultaneous turns with no AI.

	// If player unit cycling has been canceled for this turn, set it back to normal for the next
	GC.GetEngineUserInterface()->setNoSelectionListCycle(false);

	gDLL->DoTurn();

#if defined(MOD_ACTIVE_DIPLOMACY)
	// Dodgy business to cleanup all the completed requests from last turn. Any still here should just be ones that were processed on other clients anyway.
	if (MOD_ACTIVE_DIPLOMACY)
	{
		for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
			CvAssertMsg((kPlayer.isLocalPlayer() && !kPlayer.GetDiplomacyRequests()->HasPendingRequests()) || !kPlayer.isLocalPlayer(), "Clearing requests, expected local player to be empty.");
			kPlayer.GetDiplomacyRequests()->ClearAllRequests();
		}
	}
#endif

	doUpdateCacheOnTurn();

	DoUpdateCachedWorldReligionTechProgress();

	updateScore();

	m_kGameDeals.DoTurn();

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			GET_TEAM((TeamTypes)iI).doTurn();
		}
	}

	GC.getMap().doTurn();

	GC.GetEngineUserInterface()->doTurn();

	GetGameReligions()->DoTurn();
	GetGameTrade()->DoTurn();
	GetGameLeagues()->DoTurn();
	GetGameCulture()->DoTurn();

#if defined(MOD_BALANCE_CORE)
	GetGameCorporations()->DoTurn();
	GetGameContracts()->DoTurn();

	for (int iLoop = 0; iLoop < GC.getNumResourceInfos(); iLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource && pkResource->isMonopoly())
		{
			UpdateGreatestPlayerResourceMonopoly(eResource);
			for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					GET_PLAYER((PlayerTypes)iI).CheckForMonopoly(eResource);
				}
			}
		}
	}
	UpdateGreatestPlayerResourceMonopoly();
#endif

#if defined(MOD_BALANCE_CORE_HAPPINESS)
	if(MOD_BALANCE_CORE_HAPPINESS)
	{
		updateGlobalAverage();
	}
	updateEconomicTotal();
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	SetHighestPotential();
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	{
		DoBarbCountdown();
	}
#endif
	GC.GetEngineUserInterface()->setCanEndTurn(false);
	GC.GetEngineUserInterface()->setHasMovedUnit(false);

	if(getAIAutoPlay() > 0)
	{
		changeAIAutoPlay(-1);

		if(getAIAutoPlay() == 0)
		{
			ReviveActivePlayer();
		}
	}

#if defined(MOD_BALANCE_CORE_GLOBAL_IDS)
	RollOverAssetCounter();
#endif

	//-------------------------------------------------------------
	// old turn ends here, new turn starts
	//-------------------------------------------------------------

	OutputDebugString(CvString::format("Turn\t%03i\tTime\t%012u\tThread\t%d\n", getGameTurn(), GetTickCount(), GetCurrentThreadId()));
	incrementGameTurn();
	incrementElapsedGameTurns();
	gDLL->PublishNewGameTurn(getGameTurn());

	if(isOption(GAMEOPTION_DYNAMIC_TURNS))
	{// update turn mode for dynamic turn mode.
		for(int teamIdx = 0; teamIdx < MAX_TEAMS; ++teamIdx)
		{
			CvTeam& curTeam = GET_TEAM((TeamTypes)teamIdx);
			curTeam.setDynamicTurnsSimultMode(!curTeam.isHuman() || !curTeam.isAtWarWithHumans());
		}
	}

	// Configure turn active status for the beginning of the new turn.
	if(isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{// In multi-player with simultaneous turns, we activate all of the AI players
	 // at the same time.  The human players who are playing simultaneous turns will be activated in updateMoves after all
	 // the AI players are processed.
		int aiShuffle[MAX_PLAYERS];
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
			aiShuffle[iI] = iI;

		//use the pre-game RNG here
		shuffleArray(aiShuffle, MAX_PLAYERS, getMapRand());

		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			int iLoopPlayer = aiShuffle[iI];
			CvPlayer& player = GET_PLAYER((PlayerTypes)iLoopPlayer);
			// activate AI here, when they are done, activate human players in
			// updateMoves
			if(player.isAlive() && !player.isHuman())
			{
				player.setTurnActive(true);
			}
		}
	}

	if(isSimultaneousTeamTurns())
	{//We're doing simultaneous team turns, activate the first team in sequence.
		for(int iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if(kTeam.isAlive() && !kTeam.isSimultaneousTurns()) 
			{
				kTeam.setTurnActive(true);
				break;
			}
		}
	}
	else if(!isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{// player sequential turns.
		// Sequential turns.  Activate the first player we find from the start, human or AI, who wants a sequential turn.
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() 
				&& !GET_PLAYER((PlayerTypes)iI).isSimultaneousTurns()) //we don't want to be a person who's doing a simultaneous turn for dynamic turn mode.
			{
				if(isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					if(iI == getActivePlayer())
					{
						// Nobody else left alive
						CvPreGame::setGameType(GAME_HOTSEAT_MULTIPLAYER);
						GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					}
					else if(!getPbemTurnSent())
					{
						gDLL->sendPbemTurn((PlayerTypes)iI);
					}
				}
				else
				{
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					CvAssert(getNumGameTurnActive() == 1);
				}

				break;
			}
		}
	}

#if defined(MOD_BALANCE_CORE)
	if (isOption(GAMEOPTION_RANDOM_VICTORY))
	{
		doVictoryRandomization();
	}
#endif
	// Victory stuff
	testVictory();

#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		// JdH: humans may have been activated, check for AI diplomacy
		CvDiplomacyRequests::DoAIMPDiplomacyWithHumans();
	}
#endif

	// Who's Winning
	if(GET_PLAYER(getActivePlayer()).isAlive() && !IsStaticTutorialActive())
	{
		// Don't show this stuff in MP
#if defined(MOD_API_EXTENSIONS)
		if(!isReallyNetworkMultiPlayer() && !isPbem() && !isHotSeat())
#else
		if(!isGameMultiPlayer())
#endif
		{
			int iTurnFrequency = /*25*/ GC.getPROGRESS_POPUP_TURN_FREQUENCY();

			// This isn't exactly appropriate, but it'll do
			iTurnFrequency *= getGameSpeedInfo().getTrainPercent();
			iTurnFrequency /= 100;

			if(getElapsedGameTurns() % iTurnFrequency == 0)
			{
				// This popup his the sync rand, so beware
				CvPopupInfo kPopupInfo(BUTTONPOPUP_WHOS_WINNING);
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
			}
		}
	}

	LogGameState();

	//autosave after doing a turn
	if (isNetworkMultiPlayer())
		gDLL->AutoSave(false, false);
}

//	--------------------------------------------------------------------------------
ImprovementTypes CvGame::GetBarbarianCampImprovementType()
{
	return (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();
}


//	--------------------------------------------------------------------------------
void CvGame::SetBarbarianReleaseTurn(int iValue)
{
	m_iEarliestBarbarianReleaseTurn = iValue;
}


void DumpUnitInfo(CvUnitEntry* pkUnitInfo)
{
	int iUnitEra = 0;
	TechTypes ePrereqTech = (TechTypes)pkUnitInfo->GetPrereqAndTech();
	if (ePrereqTech != NO_TECH)
	{
		CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
		if (pkTechInfo)
			iUnitEra = pkTechInfo->GetEra();
	}

	CvString strAIType;
	getUnitAIString(strAIType,pkUnitInfo->GetDefaultUnitAIType());

	OutputDebugString( CvString::format("Unit,%d,Name,%s,AIType,%d,%s,era,%d,cs,%d,rcs,%d,moves,%d,cost,%d\n",
		pkUnitInfo->GetID(),pkUnitInfo->GetType(),pkUnitInfo->GetDefaultUnitAIType(),strAIType.c_str(),
		iUnitEra,pkUnitInfo->GetCombat(),pkUnitInfo->GetRangedCombat(),pkUnitInfo->GetMoves(),pkUnitInfo->GetProductionCost()).c_str() );
}

//	--------------------------------------------------------------------------------
/// Determine a random Unit type
UnitTypes CvGame::GetRandomSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged)
{
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	int iValue = 0;
	int iBonusValue;

	// Loop through all Unit Classes
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		bool bValid = false;
		const UnitTypes eLoopUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);

		if(pkUnitInfo != NULL)
		{
			//DumpUnitInfo(pkUnitInfo);

			iBonusValue = 0;

			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)pkUnitInfo->GetUnitClassType());
			if(pkUnitClassInfo)
			{
				// If this is NOT a UU, add extra value so that the default unit is more likely to get picked
				if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
					iBonusValue += 700;
				else if(!bIncludeUUs)
					continue;
			}

			bValid = (pkUnitInfo->GetCombat() > 0);
			if(bValid)
			{
				// Unit has combat strength, make sure it isn't only defensive (and with no ranged combat ability)
				if(pkUnitInfo->GetRange() == 0)
				{
					for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
					{
						const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo)
						{
							if(pkUnitInfo->GetFreePromotions(iLoop))
							{
								if(pkPromotionInfo->IsOnlyDefensive())
								{
									bValid = false;
									break;
								}
							}
						}
					}
				}
			}

			if(!bValid)
				continue;

			// Avoid Recon units
			if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
				continue;

			// No Ranged units?
			if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
				continue;

			// Must be land Unit
			if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
				continue;

#if defined(MOD_BUGFIX_NO_HOVERING_REBELS)
			// Must NOT be a hovering unit
			for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				if(pkUnitInfo->GetFreePromotions(iI))
				{
					if(GC.getPromotionInfo((PromotionTypes) iI)->IsHoveringUnit())
						continue;
				}
			}
#endif

			// Must be able to train this thing
			if(!GET_PLAYER(ePlayer).canTrain(eLoopUnit, false, false, false, /*bIgnoreUniqueUnitStatus*/ true))
				continue;

			// Random weighting
			iValue = (1 + GC.getGame().getSmallFakeRandNum(10, iUnitLoop)) * 100;
			iValue += iBonusValue;

			if(iValue > iBestValue)
			{
				eBestUnit = eLoopUnit;
				iBestValue = iValue;
			}
		}
	}

	return eBestUnit;
}

//	--------------------------------------------------------------------------------
/// Pick a random a Unit type that is ranked by unit power and restricted to units available to ePlayer's technology
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
UnitTypes CvGame::GetCompetitiveSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged, bool bIncludeShips, bool bNoResource, bool bIncludeOwnUUsOnly)
#else
UnitTypes CvGame::GetCompetitiveSpawnUnitType(PlayerTypes ePlayer, bool bIncludeUUs, bool bIncludeRanged)
#endif
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvWeightedVector<UnitTypes, SAFE_ESTIMATE_NUM_UNITS, true> veUnitRankings;

	// Loop through all Unit Classes
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eLoopUnit = (UnitTypes) iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if(pkUnitInfo == NULL)
		{
			continue;
		}

		if (pkUnitInfo->GetCombat() <= 0)
			continue;

		bool bValid = true;

		if(bNoResource)
		{
			int iNumResourceInfos= GC.getNumResourceInfos();
			for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
			{
				const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo)
				{
					if(pkUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
					{
						bValid = false;
						break;
					}
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
					if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS && pkUnitInfo->GetResourceQuantityTotal(eResource) > 0)
					{
						bValid = false;
						break;
					}
#endif
				}
			}
		}
		if (pkUnitInfo->GetResourceType() != NO_RESOURCE)
		{
			continue;
		}

		// Unit has combat strength, make sure it isn't only defensive (and with no ranged combat ability)
		if(bValid && pkUnitInfo->GetRange() == 0)
		{
			for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
			{
				const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkPromotionInfo)
				{
					if(pkUnitInfo->GetFreePromotions(iPromotionLoop))
					{
						if(pkPromotionInfo->IsOnlyDefensive())
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}
		if(!bValid)
			continue;

		UnitClassTypes eLoopUnitClass = (UnitClassTypes) pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eLoopUnitClass);

		if(pkUnitClassInfo == NULL)
		{
			CvAssertMsg(false, "UnitClassInfo is NULL. Please send Anton your save file and version.");
			continue;
		}

		// Default unit
		if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
		{
			// Allowed, do nothing
		}
		// Unique unit
		else
		{
			if(!bIncludeUUs)
			{
				continue;
			}
#if defined(MOD_BALANCE_CORE)
			//Skip own player if to include own UUs.
			else if(bIncludeOwnUUsOnly && ((UnitTypes)GET_PLAYER(ePlayer).getCivilizationInfo().getCivilizationUnits(eLoopUnitClass) != eLoopUnit))
			{
				continue;
			}
#endif
			else
			{
				// Cannot be a UU from a civ that is in our game
				for(int iMajorLoop = 0; iMajorLoop < MAX_PLAYERS; iMajorLoop++)  // MAX_PLAYERS so that we look at Barbarian UUs (ie. Brute) as well
				{
					PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
					if(GET_PLAYER(eMajorLoop).isAlive())
					{

						UnitTypes eUniqueUnitInGame = (UnitTypes) GET_PLAYER(eMajorLoop).getCivilizationInfo().getCivilizationUnits(eLoopUnitClass);
						if(eLoopUnit == eUniqueUnitInGame)
						{
							bValid = false;
							break;
						}
					}
				}
				if(!bValid)
					continue;
			}
		}

		// Avoid Recon units
		if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
			continue;

		// No Ranged units?
		if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
			continue;

		// Must be land Unit
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
		if(!(pkUnitInfo->GetDomainType() == DOMAIN_LAND || (pkUnitInfo->GetDomainType() == DOMAIN_SEA && bIncludeShips && (GC.getMap().GetAIMapHint() & ciMapHint_NavalOffshore) != 0)))
#else
		if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
#endif
			continue;

		// Must be able to train this thing
		if(!GET_PLAYER(ePlayer).canTrain(eLoopUnit, false, false, false, /*bIgnoreUniqueUnitStatus*/ true))
			continue;

		veUnitRankings.push_back(eLoopUnit, pkUnitInfo->GetPower());
	}

	// Choose from weighted unit types
	veUnitRankings.SortItems();
	int iNumChoices = GC.getUNIT_SPAWN_NUM_CHOICES();
	RandomNumberDelegate randFn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	UnitTypes eChosenUnit = veUnitRankings.ChooseFromTopChoices(iNumChoices, &randFn, "Choosing competitive unit from top choices");

	return eChosenUnit;
}

#if defined(MOD_GLOBAL_CS_GIFTS)
//	--------------------------------------------------------------------------------
/// Pick a random a Unit type that is ranked by unit power and restricted to recon units available to ePlayer's technology
#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
UnitTypes CvGame::GetCsGiftSpawnUnitType(PlayerTypes ePlayer, bool bIncludeShips)
#else
UnitTypes CvGame::GetCsGiftSpawnUnitType(PlayerTypes ePlayer)
#endif
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	vector<OptionWithScore<UnitTypes>> veUnitRankings;

	// Loop through all Unit Classes
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++) {
		const UnitTypes eLoopUnit = (UnitTypes) iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if (pkUnitInfo == NULL) 
			continue;
		
		bool bValid = (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true));
		bValid = bValid || (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true));
		bValid = bValid || (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED", true));
		bValid = bValid || (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_HELICOPTER", true));

#if defined(MOD_GLOBAL_CS_GIFT_SHIPS)
		// Exclude carrier units
		if (pkUnitInfo->GetSpecialCargo() == (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER")) 
			continue;

		// Include shipping
		bValid = bValid || (bIncludeShips && (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_NAVALMELEE", true)));
		bValid = bValid || (bIncludeShips && (pkUnitInfo->GetUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_NAVALRANGED", true)));
#endif

		if (!bValid)
			continue;

		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes) pkUnitInfo->GetUnitClassType());
		if (pkUnitClassInfo == NULL) 
			continue;

		// Exclude unique units
		if (eLoopUnit != pkUnitClassInfo->getDefaultUnitIndex()) 
			continue;

		// Must be able to train this thing
		if (!kPlayer.canTrain(eLoopUnit, false, false, false, /*bIgnoreUniqueUnitStatus*/ true)) 
			continue;

		// CUSTOMLOG("CS Gift considering unit type %i", eLoopUnit);
		veUnitRankings.push_back( OptionWithScore<UnitTypes>(eLoopUnit, pkUnitInfo->GetPower()));
	}

	// Choose from weighted unit types
	return PseudoRandomChoiceByWeight(veUnitRankings, NO_UNIT, GC.getUNIT_SPAWN_NUM_CHOICES(), kPlayer.getNumUnits() + kPlayer.GetTreasury()->GetLifetimeGrossGold());
}
#endif
#if defined(MOD_BALANCE_CORE)
bool CvGame::DoSpawnUnitsAroundTargetCity(PlayerTypes ePlayer, CvCity* pCity, int iNumber, bool bIncludeUUs, bool bIncludeShips, bool bNoResource, bool bIncludeOwnUUsOnly)
{
	if(pCity == NULL)
		return false;

	if(ePlayer == BARBARIAN_PLAYER)
	{
		SetBarbarianReleaseTurn(0);
	}

	if(iNumber <= 0)
		return false;

	int iBestPlot = -1;
	int iBestPlotWeight = -1;
	CvCityCitizens* pCitizens = pCity->GetCityCitizens();

	// Start at 1, since ID 0 is the city plot itself
	for(int iPlotLoop = 1; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		CvPlot *pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);
		// Could be outside of the map ...
		if(!pPlot)		
			continue;

		// Can't be impassable
		if(!pPlot->isValidMovePlot(pCity->getOwner()))
			continue;

		// Can't be water
		if(pPlot->isWater())
			continue;

		// Don't pick plots that aren't ours
		if(pPlot->getOwner() != pCity->getOwner())
			continue;

		// Don't place on a plot where a unit is already standing
		if(pPlot->getNumUnits() > 0)
			continue;

		int iTempWeight = getSmallFakeRandNum(10, GET_PLAYER(pCity->getOwner()).GetMilitaryMight()+pPlot->GetPlotIndex());

		// Add weight if there's an improvement here!
		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iTempWeight += 4;

			// If there's also a resource, even more weight!
			if(pPlot->getResourceType(pCity->getTeam()) != NO_RESOURCE)
				iTempWeight += 3;
		}

		// Add weight if there's a defensive bonus for this plot
		if(pPlot->defenseModifier(BARBARIAN_TEAM, false, false))
			iTempWeight += 4;

		if(iTempWeight > iBestPlotWeight)
		{
			iBestPlotWeight = iTempWeight;
			iBestPlot = iPlotLoop;
		}
	}

	bool bUnitCreated = false;
	// Found valid plot
	if(iBestPlot != -1)
	{
		CvPlot* pPlot = pCitizens->GetCityPlotFromIndex(iBestPlot);

		// Pick a unit type - should give us more melee than ranged
		UnitTypes eUnit = GetCompetitiveSpawnUnitType(ePlayer, /*bIncludeUUs*/ bIncludeUUs, /*bIncludeRanged*/ true, bIncludeShips, bNoResource, bIncludeOwnUUsOnly);
		UnitTypes emUnit = GetCompetitiveSpawnUnitType(ePlayer, /*bIncludeUUs*/ bIncludeUUs, /*bIncludeRanged*/ false, bIncludeShips, bNoResource, bIncludeOwnUUsOnly);

		CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
		if (pkInfo)
		{
			CvUnitEntry* eUnitEntry = GC.getUnitInfo(eUnit);
			
			if (pkInfo->isCivilizationUnitOverridden(eUnitEntry->GetUnitClassType()))
			{
				UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(eUnitEntry->GetUnitClassType()));
				if (eCivilizationUnit != NO_UNIT)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
					if (pkUnitEntry)
						eUnit = eCivilizationUnit;
				}
			}

			CvUnitEntry* emUnitEntry = GC.getUnitInfo(emUnit);

			if (pkInfo->isCivilizationUnitOverridden(emUnitEntry->GetUnitClassType()))
			{
				UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(emUnitEntry->GetUnitClassType()));
				if (eCivilizationUnit != NO_UNIT)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
					if (pkUnitEntry)
						emUnit = eCivilizationUnit;
				}
			}
		}

		CvUnit* pstartUnit = GET_PLAYER(ePlayer).initUnit(emUnit, pPlot->getX(), pPlot->getY());
		CvAssert(pstartUnit);
		if (pstartUnit)
		{
			if (!pstartUnit->jumpToNearestValidPlotWithinRange(3) || !pstartUnit->IsCombatUnit())
			{
				pstartUnit->kill(false);		// Could not find a spot!
			}
			else
			{
				pstartUnit->finishMoves();
				if(ePlayer != BARBARIAN_PLAYER)
				{
					pCity->addProductionExperience(pstartUnit);
				}
				bUnitCreated = true;
			}
		}
		iNumber--;	// Reduce the count since we just added the seed rebel
		if(iNumber > 0)
		{
			// Loop until all rebels are placed
			do
			{
				iNumber--;

				// Init unit
				CvUnit* pmUnit = GET_PLAYER(ePlayer).initUnit(emUnit, pPlot->getX(), pPlot->getY());
				CvAssert(pmUnit);
				if (pmUnit)
				{
					if (!pmUnit->jumpToNearestValidPlotWithinRange(3) || !pmUnit->IsCombatUnit())
					{
						pmUnit->kill(false);		// Could not find a spot!
					}
					else
					{
						pmUnit->finishMoves();
						if(ePlayer != BARBARIAN_PLAYER)
						{
							pCity->addProductionExperience(pmUnit);
						}
						bUnitCreated = true;
					}
				}
				if(iNumber > 0)
				{
					iNumber--;
					// Init unit
					CvUnit* pUnit = GET_PLAYER(ePlayer).initUnit(eUnit, pPlot->getX(), pPlot->getY());
					CvAssert(pUnit);
					if (pUnit)
					{
						if (!pUnit->jumpToNearestValidPlotWithinRange(3) || !pUnit->IsCombatUnit())
						{
							pUnit->kill(false);		// Could not find a spot!
						}
						else
						{
							pUnit->finishMoves();
							if(ePlayer != BARBARIAN_PLAYER)
							{
								pCity->addProductionExperience(pUnit);
							}
							bUnitCreated = true;
						}
					}
				}
			}
			while(iNumber > 0);
		}
	}
	return bUnitCreated;
}
#endif
//	--------------------------------------------------------------------------------
UnitTypes CvGame::GetRandomUniqueUnitType(bool bIncludeCivsInGame, bool bIncludeStartEra, bool bIncludeOldEras, bool bIncludeRanged, bool bCoastal)
{
	// Find the unique units that have already been assigned
	int iRandomSeed = 0;
	std::set<UnitTypes> setUniquesAlreadyAssigned;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
		if(pMinorLoop && pMinorLoop->isEverAlive())
		{
			iRandomSeed += pMinorLoop->GetTreasury()->GetLifetimeGrossGold(); //needed later
			UnitTypes eUniqueUnit = pMinorLoop->GetMinorCivAI()->GetUniqueUnit();
			if(eUniqueUnit != NO_UNIT)
			{
				setUniquesAlreadyAssigned.insert(eUniqueUnit);
			}
		}
	}
	
	vector<OptionWithScore<UnitTypes>> veUnitRankings;

	// Loop through all Unit Classes
	for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eLoopUnit = (UnitTypes) iUnitLoop;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if(pkUnitInfo == NULL)
		{
			continue;
		}

#if defined(MOD_GLOBAL_EXCLUDE_FROM_GIFTS)
		if (MOD_GLOBAL_EXCLUDE_FROM_GIFTS) {
			if (pkUnitInfo->IsNoMinorGifts()) {
				continue;
			}
		}
#endif

		bool bValid = (pkUnitInfo->GetCombat() > 0);

		// Unit has combat strength, make sure it isn't only defensive (and with no ranged combat ability)
		if(bValid && pkUnitInfo->GetRange() == 0)
		{
			for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
			{
				const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkPromotionInfo)
				{
					if(pkUnitInfo->GetFreePromotions(iPromotionLoop))
					{
						if(pkPromotionInfo->IsOnlyDefensive())
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}
		if(!bValid)
			continue;

		UnitClassTypes eLoopUnitClass = (UnitClassTypes) pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eLoopUnitClass);

		if(pkUnitClassInfo == NULL)
		{
			CvAssertMsg(false, "UnitClassInfo is NULL. Please send Anton your save file and version.");
			continue;
		}

		// We only want unique units
		if(eLoopUnit == pkUnitClassInfo->getDefaultUnitIndex())
#if defined(MOD_BALANCE_CORE_MINOR_CIV_GIFT)
			//Unless they are minor civ gifts only.
			if(MOD_BALANCE_CORE_MINOR_CIV_GIFT && !pkUnitInfo->IsMinorCivGift())
			{
#endif
			continue;
#if defined(MOD_BALANCE_CORE)
			}
#endif

		// Is it a unique unit from a civ that is in our game?
		if (!bIncludeCivsInGame)
		{
			for(int iMajorLoop = 0; iMajorLoop < MAX_PLAYERS; iMajorLoop++)  // MAX_PLAYERS so that we look at Barbarian UUs (ie. Brute) as well
			{
				PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
				if(GET_PLAYER(eMajorLoop).isAlive())
				{
					UnitTypes eUniqueUnitInGame = (UnitTypes) GET_PLAYER(eMajorLoop).getCivilizationInfo().getCivilizationUnits(eLoopUnitClass);
					if(eLoopUnit == eUniqueUnitInGame)
					{
						bValid = false;
						break;
					}
				}
			}
		}
		if(!bValid)
			continue;

		//Not valid?
		if (pkUnitInfo->IsInvalidMinorCivGift())
			continue;

		// Avoid Recon units
		if(pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE)
			continue;

		// No Ranged units?
		if(!bIncludeRanged && pkUnitInfo->GetRangedCombat() > 0)
			continue;

		if(!bCoastal)
		{
		// Must be land Unit
		if(pkUnitInfo->GetDomainType() != DOMAIN_LAND)
			continue;
		}

		if(pkUnitInfo->GetDomainType() == DOMAIN_AIR)
			continue;

		// Technology level
		TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
		EraTypes ePrereqEra = NO_ERA;
		if (ePrereqTech != NO_TECH)
		{
			CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
			CvAssertMsg(pkTechInfo, "Tech info not found when picking unique unit for minor civ. Please send Anton your save file and version!");
			if (pkTechInfo)
			{
				ePrereqEra = (EraTypes) pkTechInfo->GetEra();
			}
		}

		if (ePrereqEra == getStartEra())
		{
			if (!bIncludeStartEra)
				continue;
		}
		else if (ePrereqEra < getStartEra()) // Assumption: NO_ERA < 0
		{
			if (!bIncludeOldEras)
				continue;
		}

		// Is this Unit already assigned to another minor civ?
		if (setUniquesAlreadyAssigned.count(eLoopUnit) > 0)
			continue;

		int iRandom = getSmallFakeRandNum(5, eLoopUnit + iUnitLoop);

		//Weight minor civ gift units higher, so they're more likely to spawn each game.
		if (pkUnitInfo->IsMinorCivGift())
			iRandom *= 2;

		veUnitRankings.push_back( OptionWithScore<UnitTypes>(eLoopUnit, iRandom));
	}

	return PseudoRandomChoiceByWeight(veUnitRankings, NO_UNIT, GC.getUNIT_SPAWN_NUM_CHOICES(), iRandomSeed);
}

//	--------------------------------------------------------------------------------
void CvGame::updateWar()
{
	int iI, iJ;

	if(isOption(GAMEOPTION_ALWAYS_WAR))
	{
		for(iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& teamI = GET_TEAM((TeamTypes)iI);
			if(teamI.isHuman())
			{
				if(teamI.isAlive())
				{
					for(iJ = 0; iJ < MAX_TEAMS; iJ++)
					{
						CvTeam& teamJ = GET_TEAM((TeamTypes)iJ);
						if(!(teamJ.isHuman()))
						{
							if(teamJ.isAlive())
							{
								if(iI != iJ)
								{
									if(teamI.isHasMet((TeamTypes)iJ))
									{
										if(!atWar(((TeamTypes)iI), ((TeamTypes)iJ)))
										{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
											teamI.declareWar(((TeamTypes)iJ), false, teamI.getLeaderID());
#else
											teamI.declareWar(((TeamTypes)iJ));
#endif
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
}

//	-----------------------------------------------------------------------------------------------
void CvGame::updateMoves()
{
	CvUnit* pLoopUnit = NULL;
	int iLoop;
	int iI;

	// Process all AI first, then process players.
	// Processing of the AI 'first' only occurs when the AI are activated first
	// in doTurn, when MPSIMULTANEOUS_TURNS is set.  If the turns are sequential,
	// only one human or AI is active at one time and this will process them in order.
	FStaticVector<PlayerTypes, MAX_PLAYERS, true, c_eCiv5GameplayDLL, 0> playersToProcess;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if(player.isAlive() && player.isTurnActive() && !player.isHuman())
		{
			playersToProcess.push_back(static_cast<PlayerTypes>(iI));
			m_processPlayerAutoMoves = false;
			// Notice the break.  Even if there is more than one AI with an active turn, we do them sequentially.
			break;
		}
	}


	int currentTurn = getGameTurn();
	bool activatePlayers = playersToProcess.empty() && m_lastTurnAICivsProcessed != currentTurn;

#if defined(MOD_BUGFIX_AI_DOUBLE_TURN_MP_LOAD)
	m_firstActivationOfPlayersAfterLoad = activatePlayers && m_lastTurnAICivsProcessed == -1;
#endif
	// If no AI with an active turn, check humans.
	if(playersToProcess.empty())
	{
		SetLastTurnAICivsProcessed();
		if(gDLL->allAICivsProcessedThisTurn())
		{//everyone is finished processing the AI civs.
			PlayerTypes eActivePlayer = getActivePlayer();
			if(eActivePlayer != NO_PLAYER && CvPreGame::slotStatus(eActivePlayer) == SS_OBSERVER)
			{//if the active player is an observer, send a turn complete so we don't hold up the game.
				//We wait until allAICivsProcessedThisTurn to prevent a race condition where an observer could send turn complete,
				//before all clients have cleared the netbarrier locally.
#if !defined(NO_ACHIEVEMENTS)
				CvPlayer& kActivePlayer = GET_PLAYER(eActivePlayer);
				kActivePlayer.GetPlayerAchievements().EndTurn();
#endif
				gDLL->sendTurnComplete();
#if !defined(NO_ACHIEVEMENTS)
				CvAchievementUnlocker::EndTurn();
#endif
			}

			if(!m_processPlayerAutoMoves)
			{
				if(!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
				{//fully simultaneous turns.
					// All humans must be ready for auto moves
					bool readyForAutoMoves = true;
					for(iI = 0; iI < MAX_PLAYERS; iI++)
					{
						CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
						if(player.isHuman() && !player.isObserver() && !player.isAutoMoves())
							readyForAutoMoves = false;
					}
					m_processPlayerAutoMoves = readyForAutoMoves;
				}
				else
					m_processPlayerAutoMoves = true;
			}

			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);

				player.checkInitialTurnAIProcessed();
				if(player.isTurnActive() && player.isHuman())
				{
					playersToProcess.push_back(static_cast<PlayerTypes>(iI));
				}
			}
		}
	}

	FStaticVector<PlayerTypes, MAX_PLAYERS, true, c_eCiv5GameplayDLL, 0>::const_iterator i;

	for(i = playersToProcess.begin(); i != playersToProcess.end(); ++i)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)*i);

		int iReadyUnitsBeforeMoves = player.GetCountReadyUnits();

		if(player.isAlive())
		{
			bool needsAIUpdate = player.hasUnitsThatNeedAIUpdate();
			if(player.isTurnActive() || needsAIUpdate)
			{
				if(!(player.isAutoMoves()) || needsAIUpdate)
				{
					if(needsAIUpdate || !player.isHuman())
					{
						// ------- this is where the important stuff happens! --------------
						player.AI_unitUpdate();
						NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.AI_unitUpdate() called for player " << player.GetID() << " " << player.getName()); 
					}

					int iReadyUnitsNow = player.GetCountReadyUnits();

					// Was a move completed, if so save off which turn slice this was
					if(iReadyUnitsNow < iReadyUnitsBeforeMoves)
					{
						player.SetLastSliceMoved(m_iTurnSlice);
					}

					if(!(player.isHuman()) && !(player.hasBusyUnitOrCity()))
					{
						if(iReadyUnitsNow == 0)
						{
							player.setAutoMoves(true);
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.setAutoMoves(true) called for player " << player.GetID() << " " << player.getName()); 
						}
						else
						{
							const CvUnit* pReadyUnit = player.GetFirstReadyUnit();
#if defined(MOD_BALANCE_CORE)
							if (pReadyUnit) //there was a hang with a queued attack on autoplay so setAutoMoves(true) was never called
#else
							if(pReadyUnit && !player.GetTacticalAI()->IsInQueuedAttack(pReadyUnit))
#endif
							{
								int iWaitTime = 1000;
								if(!isNetworkMultiPlayer())
								{
									iWaitTime = 10;
								}
								if(m_iTurnSlice - player.GetLastSliceMoved() > iWaitTime)
								{
									CvUnitEntry* entry = GC.getUnitInfo(pReadyUnit->getUnitType());
									if(entry)
									{
										CvString strTemp = entry->GetDescription();
										CvString szAssertMessage;
										szAssertMessage.Format(
										    "GAME HANG - Please show Ed and send save. Stuck units will have their turn ended so game can advance. [DETAILS: Player %i %s. First stuck unit is %s at (%d, %d)]",
										    player.GetID(), player.getName(), strTemp.GetCString(), pReadyUnit->getX(), pReadyUnit->getY());
										CvAssertMsg(false, szAssertMessage);
										NET_MESSAGE_DEBUG_OSTR_ALWAYS(szAssertMessage);
									}
									player.EndTurnsForReadyUnits();
								}
							}
						}
					}
				}

				if(player.isAutoMoves() && (!player.isHuman() || m_processPlayerAutoMoves))
				{
					bool bRepeatAutomoves = false;
					int iRepeatPassCount = 2;	// Prevent getting stuck in a loop
					do
					{
						for(pLoopUnit = player.firstUnit(&iLoop); pLoopUnit; pLoopUnit = player.nextUnit(&iLoop))
						{
							CvString tempString;
							getMissionAIString(tempString, pLoopUnit->GetMissionAIType());
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player " << player.GetID() << " " << player.getName()
																							<< " running AutoMission (" << tempString << ") on " 
																							<< pLoopUnit->getName() << " id=" << pLoopUnit->GetID());

							pLoopUnit->AutoMission();

							// Does the unit still have movement points left over?
							if(player.isHuman() && CvUnitMission::HasCompletedMoveMission(pLoopUnit) && pLoopUnit->canMove() && !pLoopUnit->IsDoingPartialMove() && !pLoopUnit->IsAutomated())
							{
								if(player.isEndTurn())
								{
									bRepeatAutomoves = true;	// Do another pass.
									NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player " << player.GetID() << " " << player.getName()
																									<< " AutoMission did not use up all movement points for " 
																									<< pLoopUnit->getName() << " id=" << pLoopUnit->GetID());

									if(player.isLocalPlayer() && gDLL->sendTurnUnready())
										player.setEndTurn(false);
								}
							}

							// slewis sez:

							// This is a short-term solution to a problem where a unit with an auto-mission (a queued, multi-turn) move order cannot reach its destination, but
							//  does not re-enter the "need order" list because this code is processed at the end of turns. The result is that the player could easily "miss" moving
							//  the unit this turn because it displays "next turn" rather than "skip unit turn" and the unit is not added to the "needs orders" list.
							// To correctly fix this problem, we would need some way to determine if any of the auto-missions are invalid before the player can end the turn and
							//  activate the units that have a problem.
							// The problem with evaluating this is that, with one unit per tile, we don't know what is a valid move until other units have moved.
							// (For example, if one unit was to follow another, we would want the unit in the lead to move first and then have the following unit move, in order
							//  to prevent the following unit from constantly waking up because it can't move into the next tile. This is currently not supported.)

							// This short-term solution will reactivate a unit after the player clicks "next turn". It will appear strange, because the player will be asked to move
							// a unit after they clicked "next turn", but it is to give the player a chance to move all of their units.

							// jrandall sez: In MP matches, let's not OOS or stall the game.
							if(!isNetworkMultiPlayer() && !isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
							{
								if(pLoopUnit && player.isEndTurn() && pLoopUnit->GetLengthMissionQueue() == 0 && pLoopUnit->GetActivityType() == ACTIVITY_AWAKE && pLoopUnit->canMove() && !pLoopUnit->IsDoingPartialMove() && !pLoopUnit->IsAutomated())
								{
									if(IsForceEndingTurn())
									{
										SetForceEndingTurn(false);
									}
									else
									{
										CvAssertMsg(GC.getGame().getActivePlayer() == player.GetID(), "slewis - We should not need to resolve ambiguous end turns for the AI or remotely.");
										if(player.isLocalPlayer() && gDLL->sendTurnUnready())
											player.setEndTurn(false);
									}
								}
							}
						}
					}
					while(bRepeatAutomoves && iRepeatPassCount--);

					// slewis - I changed this to only be the AI because human players should have the tools to deal with this now
					if(!player.isHuman())
					{
						for(pLoopUnit = player.firstUnit(&iLoop); pLoopUnit; pLoopUnit = player.nextUnit(&iLoop))
						{
							bool bMoveMe  = false;
							IDInfo* pUnitNodeInner = pLoopUnit->plot()->headUnitNode();
							while(pUnitNodeInner != NULL && !bMoveMe)
							{
								CvUnit* pLoopUnitInner = ::getUnit(*pUnitNodeInner);
								if(pLoopUnitInner && pLoopUnit != pLoopUnitInner)
								{
									if(pLoopUnit->getOwner() == pLoopUnitInner->getOwner())	// Could be a dying Unit from another player here
									{
										if (!pLoopUnit->canEndTurnAtPlot(pLoopUnit->plot()))
										{
											if(pLoopUnitInner->IsFortified() && !pLoopUnit->IsFortified())
											{
												bMoveMe = true;
											}
										}
									}
								}
								pUnitNodeInner = pLoopUnit->plot()->nextUnitNode(pUnitNodeInner);
							}

							if(bMoveMe)
							{
								if (!pLoopUnit->jumpToNearestValidPlotWithinRange(1))
									pLoopUnit->kill(false);
								break;
							}

							pLoopUnit->doDelayedDeath();
						}
					}

					// If we completed the processing of the auto-moves, flag it.
					if(player.isEndTurn() || !player.isHuman())
					{
						player.setProcessedAutoMoves(true);
					}
				}

				// KWG: This code should go into CheckPlayerTurnDeactivate
				if(!player.isEndTurn() && gDLL->HasReceivedTurnComplete(player.GetID()) && player.isHuman() /* && (isNetworkMultiPlayer() || (!isNetworkMultiPlayer() && player.GetID() != getActivePlayer())) */)
				{
					if(!player.hasBusyUnitOrCity())
					{
						player.setEndTurn(true);
						if(player.isEndTurn())
						{//If the player's turn ended, indicate it in the log.  We only do so when the end turn state has changed to prevent useless log spamming in multiplayer. 
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("UpdateMoves() : player.setEndTurn(true) called for player " << player.GetID() << " " << player.getName());
						}
					}
					else
					{
						if(!player.hasBusyUnitUpdatesRemaining())
						{
							NET_MESSAGE_DEBUG_OSTR_ALWAYS("Received turn complete for player "  << player.GetID() << " " << player.getName() << " but there is a busy unit. Forcing the turn to advance");
							player.setEndTurn(true);
						}
					}
				}
			}
		}
	}

	if(activatePlayers)
	{
		if (isOption(GAMEOPTION_DYNAMIC_TURNS) || isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
		{//Activate human players who are playing simultaneous turns now that we've finished moves for the AI.
			// KWG: This code should go into CheckPlayerTurnDeactivate
			for(iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
				if(!player.isTurnActive() && player.isHuman() && player.isAlive() && player.isSimultaneousTurns())
				{
					player.setTurnActive(true);
				}
			}
		}
	}
}

//	-----------------------------------------------------------------------------------------------
void CvGame::updateTimers()
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
			kPlayer.updateTimers();
		}
	}
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(!GC.getGame().isReallyNetworkMultiPlayer() || !MOD_ACTIVE_DIPLOMACY)
	{
		if(isHotSeat())
		{
			// For Hot Seat, all the AIs will get a chance to do diplomacy with the active human player
			PlayerTypes eActivePlayer = getActivePlayer();
			if(eActivePlayer != NO_PLAYER)
			{
				CvPlayer& kActivePlayer = GET_PLAYER(eActivePlayer);
				if(kActivePlayer.isAlive() && kActivePlayer.isHuman() && kActivePlayer.isTurnActive())
					CvDiplomacyRequests::DoAIDiplomacy(eActivePlayer);
			}
		}
	}
#endif
}

//	-----------------------------------------------------------------------------------------------
void CvGame::UpdatePlayers()
{
	int numActive = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		// slewis - should it check for active turn?
		if(kPlayer.isAlive() && kPlayer.isTurnActive())
		{
			kPlayer.UpdateNotifications();
			++numActive;
		}
	}
	CvAssert(numActive == getNumGameTurnActive());
}

//	-----------------------------------------------------------------------------------------------
void CvGame::testAlive()
{
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).verifyAlive();
	}
}

bool CvGame::testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore) const
{
	CvAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());
	CvAssert(eTeam >=0 && eTeam < MAX_CIV_TEAMS);
	CvAssert(GET_TEAM(eTeam).isAlive());

	CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
	if(pkVictoryInfo == NULL)
	{
		return false;
	}

	// Has the player already achieved this victory?
	if(GET_TEAM(eTeam).isVictoryAchieved(eVictory))
	{
		return false;
	}

	bool bValid = isVictoryValid(eVictory);
	if(pbEndScore)
	{
		*pbEndScore = false;
	}

	// Can't end the game unless a certain number of turns has already passed (ignore this on Debug Micro Map because it's only for testing)
	if(getElapsedGameTurns() <= /*10*/ GC.getMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY() && (GC.getMap().getWorldSize() != WORLDSIZE_DEBUG))
	{
		return false;
	}

	// End Score
	if(bValid)
	{
		if(pkVictoryInfo->isEndScore())
		{
			if(pbEndScore)
			{
				*pbEndScore = true;
			}

			if(getMaxTurns() == 0)
			{
				bValid = false;
			}
			else if(getElapsedGameTurns() < getMaxTurns())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}

	// Target Score
	if(bValid)
	{
		if(pkVictoryInfo->isTargetScore())
		{
			if(getTargetScore() == 0)
			{
				bValid = false;
			}
			else if(getTeamScore(eTeam) < getTargetScore())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}

	// Conquest
	if(bValid)
	{
		if(pkVictoryInfo->isConquest())
		{
			if(GET_TEAM(eTeam).getNumCities() == 0)
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for(int iK = 0; iK < MAX_CIV_TEAMS; iK++)
				{
					if(GET_TEAM((TeamTypes)iK).isAlive())
					{
						if(iK != eTeam)
						{
							if(GET_TEAM((TeamTypes)iK).getNumCities() > 0)
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if(bFound)
				{
					bValid = false;
				}
			}
		}
	}

	// Diplomacy Victory
	if(bValid)
	{
		if(pkVictoryInfo->isDiploVote())
		{
			bValid = false;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
				if (GET_PLAYER(ePlayerLoop).getTeam() == eTeam)
				{
					if (m_pGameLeagues->GetDiplomaticVictor() == ePlayerLoop)
					{
						bValid = true;
					}
				}
			}
		}
	}

	// Culture victory
	if(bValid)
	{
		if(pkVictoryInfo->isInfluential())
		{
			// See if all players on this team have influential culture with all other players (still alive)
			bValid = false;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
				if (kPlayer.isAlive())
				{
#if defined(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					if(MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
					{
						if(kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree() == NO_POLICY_BRANCH_TYPE)
							continue;

						if(kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE)
						{
							if(kPlayer.GetCulture()->GetPublicOpinionType() > PUBLIC_OPINION_CONTENT)
								continue;
						}

						ProjectTypes eUtopia = (ProjectTypes)GC.getInfoTypeForString("PROJECT_UTOPIA_PROJECT", true);
						if (eUtopia != NO_PROJECT)
						{
							if (GET_TEAM(kPlayer.getTeam()).getProjectCount(eUtopia) <= 0)
								continue;
						}
					}
#endif
					if (kPlayer.getTeam() == eTeam)
					{
						if (kPlayer.GetCulture()->GetNumCivsInfluentialOn() >= m_pGameCulture->GetNumCivsInfluentialForWin())
						{
							// Not enough civs for a win
							bValid = true;
							break;
						}
					}
				}
			}		
		}
	}

	// Religion in all Cities
	if(bValid)
	{
		if(pkVictoryInfo->IsReligionInAllCities())
		{
			bool bReligionInAllCities = true;

			CvCity* pLoopCity;
			int iLoop;

			PlayerTypes eLoopPlayer;

			// See if all players on this team have their State Religion in their Cities
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(eLoopPlayer).isAlive())
				{
					if(GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
					{
						for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoop))
						{
							// Any Cities WITHOUT State Religion?
							if(pLoopCity->GetPlayersReligion() != eLoopPlayer)
							{
								bReligionInAllCities = false;
								break;
							}
						}

						if(!bReligionInAllCities)
						{
							break;
						}
					}
				}
			}

			if(!bReligionInAllCities)
			{
				bValid = false;
			}
		}
	}

	// FindAllNaturalWonders
	if(bValid)
	{
		if(pkVictoryInfo->IsFindAllNaturalWonders())
		{
			int iWorldNumNaturalWonders = GC.getMap().GetNumNaturalWonders();

			if(iWorldNumNaturalWonders == 0 || GET_TEAM(eTeam).GetNumNaturalWondersDiscovered() < iWorldNumNaturalWonders)
			{
				bValid = false;
			}
		}
	}

	// Population Percent
	if(bValid)
	{
		if(getAdjustedPopulationPercent(eVictory) > 0)
		{
			if(100 * GET_TEAM(eTeam).getTotalPopulation() < getTotalPopulation() * getAdjustedPopulationPercent(eVictory))
			{
				bValid = false;
			}
		}
	}

	// Land Percent
	if(bValid)
	{
		if(getAdjustedLandPercent(eVictory) > 0)
		{
			if(100 * GET_TEAM(eTeam).getTotalLand() < GC.getMap().getLandPlots() * getAdjustedLandPercent(eVictory))
			{
				bValid = false;
			}
		}
	}

	// Buildings
	if(bValid)
	{
		for(int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
		{
			BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iK);
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(pkBuildingClassInfo)
			{
				if(pkBuildingClassInfo->getVictoryThreshold(eVictory) > GET_TEAM(eTeam).getBuildingClassCount(eBuildingClass))
				{
					bValid = false;
					break;
				}
			}

		}
	}

	// Projects
	if(bValid)
	{
		for(int iK = 0; iK < GC.getNumProjectInfos(); iK++)
		{
			const ProjectTypes eProject = static_cast<ProjectTypes>(iK);
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
			if(pkProjectInfo)
			{
				if(pkProjectInfo->GetVictoryMinThreshold(eVictory) > GET_TEAM(eTeam).getProjectCount(eProject))
				{
					bValid = false;
					break;
				}
			}
		}
	}

	return bValid;
}

//	---------------------------------------------------------------------------
void CvGame::testVictory()
{
	bool bEndScore = false;

	// Send a game event to allow a Lua script to set the victory state
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GameCoreTestVictory", args.get(), bResult);
	}

	if(getVictory() != NO_VICTORY)
	{
		return;
	}

	if(getGameState() == GAMESTATE_EXTENDED)
	{
		return;
	}

	updateScore();

	bool bEndGame = false;
#if defined(MOD_BALANCE_CORE)
	bool bIsDomination = false;
	bool bIsScore = false;
#endif

	std::vector<std::vector<int> > aaiGameWinners;
	int iTeamLoop = 0;
	int iVictoryLoop = 0;

	int iNumCompetitionWinners = 0;
	for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		m_aiTeamCompetitionWinnersScratchPad[iTeamLoop] = 0;
	}

	// Look at each Victory Competition
	for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
	{
		const VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
		if(pkVictoryInfo == NULL)
			continue;

		for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeamLoop);
			if(kLoopTeam.isAlive())
			{
				if(!(kLoopTeam.isMinorCiv()))
				{
					if(testVictory(eVictory, (TeamTypes)iTeamLoop, &bEndScore))
					{
						// Some Victories win the game for the player who accomplishes it.  If this happens for multiple teams in the same turn we have to break the tie
						if(pkVictoryInfo->IsWinsGame())
						{
							std::vector<int> aWinner;
							aWinner.push_back(iTeamLoop);
							aWinner.push_back(iVictoryLoop);
							aaiGameWinners.push_back(aWinner);

							bEndGame = true;
#if defined(MOD_BALANCE_CORE)
							if (pkVictoryInfo->isConquest())
							{
								bIsDomination = true;
							}
#endif
						}
						// Non game-ending Competition winner placement
						else
						{
							m_aiTeamCompetitionWinnersScratchPad[iNumCompetitionWinners] = iTeamLoop;
							iNumCompetitionWinners++;
						}
					}
				}
			}
		}

		// Any (non game-ending) Competition placers?
		if(iNumCompetitionWinners > 0)
		{
			int iRand;

			do
			{
				if(isVictoryAvailable(eVictory))
				{
					iRand = GC.getGame().getSmallFakeRandNum(iNumCompetitionWinners, iNumCompetitionWinners + iVictoryLoop);
					iTeamLoop = m_aiTeamCompetitionWinnersScratchPad[iRand];

					DoPlaceTeamInVictoryCompetition(eVictory, (TeamTypes) iTeamLoop);

					iNumCompetitionWinners--;
				}
				else
				{
					iNumCompetitionWinners = 0;
				}
			}
			while(iNumCompetitionWinners > 0);
		}
	}

	// Game could have been set to ending already by an insta-win victory, or we might have hit the end of time manually
	if(!bEndGame)
	{
		bEndGame = IsEndGameTechResearched();

		if(bEndGame)
		{
			VictoryTypes eScoreVictory = NO_VICTORY;
			for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(pkVictoryInfo->isTargetScore())
					{
						eScoreVictory = eVictory;
						break;
					}
				}
			}

			aaiGameWinners.clear();

			// Find out who is in the lead with VPs
			int iBestVPNum = 0;
			int iVPs;
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).getVictoryPoints();

				if(iVPs > iBestVPNum)
				{
					iBestVPNum = iVPs;
				}
			}

			// Now that we know what the highest is, see if any players are tied
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).getVictoryPoints();

				if(iVPs == iBestVPNum)
				{
					std::vector<int> aWinner;
					aWinner.push_back(iTeamLoop);
					aWinner.push_back(eScoreVictory);
					aaiGameWinners.push_back(aWinner);
				}
			}
		}
	}

	if(!bEndGame)
	{
		bEndGame = (getMaxTurns() > 0 && getElapsedGameTurns() >= getMaxTurns());

		if(bEndGame)
		{
			VictoryTypes eScoreVictory = NO_VICTORY;
			for(iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
			{
				VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
				CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
				if(pkVictoryInfo)
				{
					if(pkVictoryInfo->isEndScore())
					{
						eScoreVictory = eVictory;
#if defined(MOD_BALANCE_CORE)
						bIsScore = true;
#endif
						break;
					}
				}
			}

			aaiGameWinners.clear();

			// Find out who is in the lead with VPs
			int iBestVPNum = 0;
			int iVPs;
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).GetScore();

				if(iVPs > iBestVPNum)
				{
					iBestVPNum = iVPs;
				}
			}

			// Now that we know what the highest is, see if any players are tied
			for(iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				iVPs = GET_TEAM((TeamTypes) iTeamLoop).GetScore();

				if(iVPs == iBestVPNum)
				{
					std::vector<int> aWinner;
					aWinner.push_back(iTeamLoop);
					aWinner.push_back(eScoreVictory);
					aaiGameWinners.push_back(aWinner);
				}
			}
		}
	}

#if defined(MOD_BALANCE_CORE)
	//Not already defined? Skip!
	if (isOption(GAMEOPTION_RANDOM_VICTORY) && !isVictoryRandomizationDone() && !bIsDomination && !bIsScore)
	{
		aaiGameWinners.clear();
		bEndGame = false;
	}
#endif

	// Two things can set this to true: either someone has finished an insta-win victory, or the game-ending tech has been researched and we're now tallying VPs
	if(bEndGame && !aaiGameWinners.empty())
	{
		int iWinner = getSmallFakeRandNum(aaiGameWinners.size(), aaiGameWinners.size());
		CUSTOMLOG("Calling setWinner from testVictory: %i, %i", aaiGameWinners[iWinner][0], aaiGameWinners[iWinner][1]);
		setWinner(((TeamTypes)aaiGameWinners[iWinner][0]), ((VictoryTypes)aaiGameWinners[iWinner][1]));
	}

	if(getVictory() == NO_VICTORY)
	{
		if(getMaxTurns() > 0)
		{
			if(getElapsedGameTurns() >= getMaxTurns())
			{
				if(!bEndScore)
				{
					if((getAIAutoPlay() > 0) || gDLL->GetAutorun())
					{
						setGameState(GAMESTATE_EXTENDED);
					}
					else
					{
						setGameState(GAMESTATE_OVER);
					}
				}
			}
		}
	}
}

#if defined(MOD_BALANCE_CORE)
void CvGame::doVictoryRandomization()
{
	//Already done?
	if (isVictoryRandomizationDone())
		return;

	//no one in atomic era?
	bool bFinalEra = false;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && !GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			if (GET_PLAYER(eLoopPlayer).GetCurrentEra() >= (EraTypes) GC.getInfoTypeForString("ERA_POSTMODERN"))
			{
				bFinalEra = true;
			}
		}
	}
	if (!bFinalEra)
	{
		return;
	}
	//Okay, let's do this!

	VictoryTypes eBestVictory = NO_VICTORY;
	int iBestVictoryScore = 0;
	// Look at each Victory Competition
	for (int iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
	{
		const VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
		CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
		if (pkVictoryInfo == NULL)
			continue;

		//Skip score, always valid.
		if (pkVictoryInfo->isEndScore())
			continue;

		//Skip conquest, always valid.
		if (pkVictoryInfo->isConquest())
			continue;

		int iScore = getSmallFakeRandNum(100, GC.getGame().GetGameCulture()->GetNumGreatWorks()+ iVictoryLoop);
		
		if (pkVictoryInfo->isDiploVote())
		{
			iScore += GC.getGame().GetNumMinorCivsAlive() * 3;
		}
		else if (pkVictoryInfo->isInfluential())
		{
			iScore += GC.getGame().GetGameCulture()->GetNumGreatWorks() / 5;
		}
		else if (eVictory == (VictoryTypes)GC.getInfoTypeForString("VICTORY_SPACE_RACE", true))
		{
			for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
			{
				TeamTypes eTeam = (TeamTypes)iTeamLoop;
				if (eTeam == NO_TEAM)
					continue;

				if (GET_TEAM(eTeam).isMinorCiv() || GET_TEAM(eTeam).isBarbarian())
					continue;

				iScore += (GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown() / 20);
			}
		}
		else
		{
			continue;
		}
		if (iScore > iBestVictoryScore)
		{
			iBestVictoryScore = iScore;
			eBestVictory = eVictory;
		}
	}

	if (eBestVictory != NO_VICTORY)
	{
		CvVictoryInfo* pkBestVictoryInfo = GC.getVictoryInfo(eBestVictory);
		if (pkBestVictoryInfo == NULL)
			return;

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strOutput;

			CvString playerName;
			CvString otherPlayerName;
			CvString strMinorString;
			CvString strDesc;
			CvString strLogName;
			CvString strTemp;

			strLogName = "RandomVictory_Log.csv";

			FILogFile* pLog;
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

			// Get the leading info for this line
			strOutput.Format("%03d", GC.getGame().getElapsedGameTurns());

			strTemp.Format("Random Victory Chosen: %s", pkBestVictoryInfo->GetDescription());
			strOutput += ", " + strTemp;

			pLog->Msg(strOutput);
		}

		// Look at each Victory Competition
		for (int iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
		{
			const VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
			CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
			if (pkVictoryInfo == NULL)
				continue;

			if (!isVictoryValid(eVictory))
				continue;

			//Skip conquest, always valid.
			if (pkVictoryInfo->isConquest())
				continue;

			//Skip score, always valid.
			if (pkVictoryInfo->isEndScore())
				continue;

			//Skip our best selection.
			if (eBestVictory == eVictory)
				continue;

			//Disable all victories
			setVictoryValid(eVictory, false);
		}

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;
			if (ePlayer == NO_PLAYER)
				continue;

			if (GET_PLAYER(ePlayer).isHuman() && GC.getGame().getActivePlayer() == ePlayer)
			{
				if (pkBestVictoryInfo->isDiploVote())
				{
					CvPopupInfo kPopupInfo(BUTTONPOPUP_MODDER_4, 1);
					DLLUI->AddPopup(kPopupInfo);
				}
				else if (pkBestVictoryInfo->isInfluential())
				{
					CvPopupInfo kPopupInfo(BUTTONPOPUP_MODDER_4, 2);
					DLLUI->AddPopup(kPopupInfo);
				}
				else if (eBestVictory == (VictoryTypes)GC.getInfoTypeForString("VICTORY_SPACE_RACE", true))
				{
					CvPopupInfo kPopupInfo(BUTTONPOPUP_MODDER_4, 3);
					DLLUI->AddPopup(kPopupInfo);
				}
			}
		}

		setVictoryRandomizationDone(true);
	}
	
}
#endif

//	--------------------------------------------------------------------------------
CvRandom& CvGame::getMapRand()
{
	return m_mapRand;
}


//	--------------------------------------------------------------------------------
int CvGame::getMapRandNum(int iNum, const char* pszLog)
{
#if defined(MOD_BUGFIX_RANDOM)
	if (iNum > 0)
		return m_mapRand.get(iNum, pszLog);

	return (int)m_mapRand.get(-iNum, pszLog)*(-1);
#else
	return m_mapRand.get(iNum, pszLog);
#endif
}


//	--------------------------------------------------------------------------------
CvRandom& CvGame::getJonRand()
{
	return m_jonRand;
}


//	--------------------------------------------------------------------------------
/// Get a synchronous random number in the range of 0...iNum-1
/// Allows for logging.
int CvGame::getJonRandNum(int iNum, const char* pszLog)
{
#if defined(MOD_BUGFIX_RANDOM)
	if (iNum > 0)
		return m_jonRand.get(iNum, pszLog);

	return (int)m_jonRand.get(-iNum, pszLog)*(-1);
#else
	return m_jonRand.get(iNum, pszLog);
#endif
}

//	--------------------------------------------------------------------------------
/// Get a synchronous random number in the range of 0...iNum-1
/// Allows for logging.
// Unfortunately we need to name the method differently so that the non-va one can still exist without
// causing ambiguous call errors.  The non VA one is needed for use as a delegate
int CvGame::getJonRandNumVA(int iNum, const char* pszLog, ...)
{
	if (pszLog)
	{
		const size_t uiOutputSize = 512;
		char szOutput[uiOutputSize];

		va_list vl;
		va_start(vl, pszLog);
		vsprintf_s(szOutput, uiOutputSize, pszLog, vl);
		va_end(vl);

		return getJonRandNum(iNum, szOutput);
	}
	else
		return getJonRandNum(iNum, NULL);
}

//	--------------------------------------------------------------------------------
/// Get an asynchronous random number in the range of 0...iNum-1
/// This should only be called by operations that will not effect gameplay!
int CvGame::getAsyncRandNum(int iNum, const char* pszLog)
{
#if defined(MOD_BUGFIX_RANDOM)
	if (iNum > 0)
		return GC.getASyncRand().get(iNum, pszLog);

	return (int)GC.getASyncRand().get(-iNum, pszLog)*(-1);
#else
	return GC.getASyncRand().get(iNum, pszLog);
#endif
}

#if defined(MOD_CORE_REDUCE_RANDOMNESS)
//	--------------------------------------------------------------------------------
// Get a fake random number which depends only on game state
// for small numbers (e.g. direction rolls) this should be good enough
// most importantly, it should reduce desyncs in multiplayer

// Robert Jenkins method
unsigned long hash32(unsigned long a)
{
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return a;
}

static unsigned long giLastState = 0;

int CvGame::getSmallFakeRandNum(int iNum, const CvPlot& input)
{
	unsigned long iState = input.getX()*17 + input.getY()*23 + getGameTurn()*3;

	/*
	//safety check
	if (iState == giLastState)
		OutputDebugString("warning rng seed repeated\n");
	giLastState = iState;
	*/
	
	int iResult = 0;
	if (iNum > 0)
		iResult = hash32(iState) % iNum;
	else if (iNum < 0)
		iResult = -int(hash32(iState) % (-iNum));

	/*
	FILogFile* pLog = LOGFILEMGR.GetLog("FakeRandCalls1.csv", FILogFile::kDontTimeStamp);
	if (pLog)
	{
		char szOut[1024] = { 0 };
		sprintf_s(szOut, "turn %d, max %d, res %d, seed (%d:%d)\n", getGameTurn(), iNum, iResult, input.getX(), input.getY());
		pLog->Msg(szOut);
	}
	*/

	return iResult;
}

int CvGame::getSmallFakeRandNum(int iNum, int iExtraSeed)
{
	unsigned long iState = getGameTurn() + abs(iExtraSeed);

	/*
	//safety check
	if (iState == giLastState)
		OutputDebugString("warning rng seed repeated\n");
	giLastState = iState;
	*/

	int iResult = 0;
	if (iNum > 0)
		iResult = hash32(iState) % iNum;
	else if (iNum < 0)
		iResult = -int(hash32(iState) % (-iNum));

	/*
	FILogFile* pLog = LOGFILEMGR.GetLog("FakeRandCalls2.csv", FILogFile::kDontTimeStamp);
	if (pLog)
	{
		char szOut[1024] = { 0 };
		sprintf_s(szOut, "turn %d, max %d, res %d, seed %d\n", getGameTurn(), iNum, iResult, iExtraSeed);
		pLog->Msg(szOut);
	}
	*/

	return iResult;
}

#endif

//	--------------------------------------------------------------------------------
int CvGame::calculateSyncChecksum()
{
	CvUnit* pLoopUnit;
#if defined(MOD_BALANCE_CORE)
	unsigned int iMultiplier;
	unsigned long long iValue;
	int iLoop;
	int iI, iJ;
#else
	int iMultiplier;
	int iValue;
	int iLoop;
	int iI, iJ;
#endif

	iValue = 0;

	iValue += getMapRand().getSeed();
	iValue += getJonRand().getSeed();

	iValue += getNumCities();
	iValue += getTotalPopulation();

	iValue += GC.getMap().getOwnedPlots();
	iValue += GC.getMap().getNumAreas();

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			iMultiplier = getPlayerScore((PlayerTypes)iI);

			switch(getTurnSlice() % 4)
			{
			case 0:
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalPopulation() * 543271);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalLand() * 327382);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).GetTreasury()->GetGold() * 107564);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getPower() * 135647);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumCities() * 436432);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumUnits() * 324111);
				break;

			case 1:
				for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).calculateTotalYield((YieldTypes)iJ) * 432754);
				}
				break;

			case 2:
				for(iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getImprovementCount((ImprovementTypes)iJ) * 883422);
				}

				for(iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iJ);
					if(!pkBuildingClassInfo)
					{
						continue;
					}

					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBuildingClassCountPlusMaking((BuildingClassTypes)iJ) * 954531);
				}

				for(iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
				{
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iJ);
					if(!pkUnitClassInfo)
					{
						continue;
					}

					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getUnitClassCountPlusMaking((UnitClassTypes)iJ) * 754843);
				}
				break;

			case 3:
				for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
				{
					iMultiplier += (pLoopUnit->getX() * 876543);
					iMultiplier += (pLoopUnit->getY() * 985310);
					iMultiplier += (pLoopUnit->getDamage() * 736373);
#if defined(MOD_UNITS_XP_TIMES_100)
					iMultiplier += ((pLoopUnit->getExperienceTimes100() / 100) * 820622);
#else
					iMultiplier += (pLoopUnit->getExperience() * 820622);
#endif
					iMultiplier += (pLoopUnit->getLevel() * 367291);
				}
				break;
			}

			if(iMultiplier != 0)
			{
				iValue *= iMultiplier;
			}
		}
	}

#if defined(MOD_CORE_DEBUGGING)
	debugSyncChecksum();
#endif

	return iValue & 0xFFFFFFFF;
}

#if defined(MOD_BALANCE_CORE)
void CvGame::debugSyncChecksum()
{
	CvString fname = CvString::format( "AssetsTurn%03d.txt", GC.getGame().getGameTurn() );
	FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
	if (!pLog)
		return;

	pLog->Msg( "----global-----\n" );
	pLog->Msg( CvString::format("MapRandSeed: %I64u\n",getMapRand().getSeed()).c_str() );
	pLog->Msg( CvString::format("GameRandSeed: %I64u\n",getJonRand().getSeed()).c_str() );
	pLog->Msg( CvString::format("NCities: %d\n",getNumCities()).c_str() );
	pLog->Msg( CvString::format("TotalPop: %d\n",getTotalPopulation()).c_str() );
	pLog->Msg( CvString::format("OwnedPlots: %d\n",GC.getMap().getOwnedPlots()).c_str() );
	pLog->Msg( CvString::format("MapAreas: %d\n",GC.getMap().getNumAreas()).c_str() );

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			pLog->Msg( CvString::format("------ Player %02d : %s\n",iI,GET_PLAYER((PlayerTypes)iI).getCivilizationAdjective()).c_str() );

			pLog->Msg( CvString::format("Score: %d\n",getPlayerScore((PlayerTypes)iI)).c_str() );

			pLog->Msg( CvString::format("pop: %d\n",GET_PLAYER((PlayerTypes)iI).getTotalPopulation()).c_str() );
			pLog->Msg( CvString::format("land: %d\n",GET_PLAYER((PlayerTypes)iI).getTotalLand()).c_str() );
			pLog->Msg( CvString::format("gold: %d\n",GET_PLAYER((PlayerTypes)iI).GetTreasury()->GetGold()).c_str() );
			pLog->Msg( CvString::format("power: %d\n",GET_PLAYER((PlayerTypes)iI).getPower()).c_str() );
			pLog->Msg( CvString::format("cities: %d\n",GET_PLAYER((PlayerTypes)iI).getNumCities()).c_str() );
			pLog->Msg( CvString::format("units: %d\n",GET_PLAYER((PlayerTypes)iI).getNumUnits()).c_str() );

			for(int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				pLog->Msg( CvString::format("yield type %d: %d\n",iJ,GET_PLAYER((PlayerTypes)iI).calculateTotalYield((YieldTypes)iJ)).c_str() );

			for(int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				pLog->Msg( CvString::format("improvement type %d: %d\n",iJ,GET_PLAYER((PlayerTypes)iI).getImprovementCount((ImprovementTypes)iJ)).c_str() );

			for(int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
			{
				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iJ);
				if(!pkBuildingClassInfo)
					continue;

				int iCount = GET_PLAYER((PlayerTypes)iI).getBuildingClassCountPlusMaking((BuildingClassTypes)iJ);
				if (iCount>0)
					pLog->Msg( CvString::format("building type %d: %d\n",iJ,iCount).c_str() );
			}

			for(int iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
			{
				CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iJ);
				if(!pkUnitClassInfo)
					continue;

				int iCount = GET_PLAYER((PlayerTypes)iI).getUnitClassCountPlusMaking((UnitClassTypes)iJ);
				if (iCount>0)
					pLog->Msg( CvString::format("unit type %d: %d\n",iJ,iCount).c_str() );
			}

			int iLoop=0;
			for(CvUnit* pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
				pLog->Msg( CvString::format("unit %d: x %02d, y %02d, damage %02d, exp %02d, level %02d\n",
#if defined(MOD_UNITS_XP_TIMES_100)
					iLoop, pLoopUnit->getX(), pLoopUnit->getY(), pLoopUnit->getDamage(), (pLoopUnit->getExperienceTimes100() / 100), pLoopUnit->getLevel() ).c_str() );
#else
					iLoop, pLoopUnit->getX(), pLoopUnit->getY(), pLoopUnit->getDamage(), pLoopUnit->getExperience(), pLoopUnit->getLevel() ).c_str() );
#endif
		}
	}

	pLog->Close();
}

#endif

//	--------------------------------------------------------------------------------
int CvGame::calculateOptionsChecksum()
{
	int iValue;
	int iI, iJ;

	iValue = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		for(iJ = 0; iJ < GC.getNumPlayerOptionInfos(); iJ++)
		{
			CvPlayerOptionInfo* pkInfo = GC.getPlayerOptionInfo((PlayerOptionTypes)iJ);
			if (pkInfo)
			{
				uint uiID = FString::Hash( pkInfo->GetType() );
				if(kPlayer.isOption((PlayerOptionTypes)uiID))
				{
					iValue += (iI * 943097);
					iValue += (iJ * 281541);
				}
			}
		}
	}

	return iValue;
}


//	--------------------------------------------------------------------------------
void CvGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, const CvString& pszText, int iPlotX, int iPlotY)
{
	int iGameTurn = getGameTurn();

	//If this is a plot-related message, search for any previously created messages that match this one and just add the plot.
	if(iPlotX != -1 || iPlotY != -1)
	{
		for(ReplayMessageList::iterator it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
		{
			CvReplayMessage& msg = (*it);
			if(msg.getType() == eType && msg.getTurn() == iGameTurn && msg.getPlayer() == ePlayer && msg.getText() == pszText)
			{
				msg.addPlot(iPlotX, iPlotY);
				return;
			}
		}
	}

	CvReplayMessage message(iGameTurn, eType, ePlayer);
	message.addPlot(iPlotX, iPlotY);
	message.setText(pszText);
	m_listReplayMessages.push_back(message);
}

//	--------------------------------------------------------------------------------
void CvGame::clearReplayMessageMap()
{
	m_listReplayMessages.clear();
}

//	--------------------------------------------------------------------------------
uint CvGame::getNumReplayMessages() const
{
	return m_listReplayMessages.size();
}

//	--------------------------------------------------------------------------------
int CvGame::CalculateMedianNumCities()
{
	vector<int> v;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		const CvPlayer& kPlayer = GET_PLAYER(eLoopPlayer);
		
		if (kPlayer.isMajorCiv() && kPlayer.isAlive() && kPlayer.getNumCities() > 0)
			v.push_back(kPlayer.getNumCities());
	}

	if (v.empty())
		return 0;

	std::sort(v.begin(), v.end());

	return v[v.size()/2];
}

//	--------------------------------------------------------------------------------
int CvGame::CalculateMedianNumPlots()
{
	vector<int> v;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		const CvPlayer& kPlayer = GET_PLAYER(eLoopPlayer);
		
		if (kPlayer.isMajorCiv() && kPlayer.isAlive() && kPlayer.getNumCities() > 0)
			v.push_back(kPlayer.getTotalLand());
	}

	if (v.empty())
		return 0;

	std::sort(v.begin(), v.end());

	return v[v.size()/2];
}

//	--------------------------------------------------------------------------------
int CvGame::CalculateMedianNumWondersConstructed()
{
	vector<int> v;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		const CvPlayer& kPlayer = GET_PLAYER(eLoopPlayer);

		if (kPlayer.isMajorCiv() && kPlayer.GetWondersConstructed() > 0)
			v.push_back(kPlayer.GetWondersConstructed());
	}

	if (v.empty())
		return 0;

	std::sort(v.begin(), v.end());

	return v[v.size()/2];
}


//	--------------------------------------------------------------------------------

#if defined(MOD_BALANCE_CORE_HAPPINESS)
void CvGame::updateEconomicTotal()
{
	CvCity* pLoopCity;
	int iCityLoop;
	PlayerTypes eLoopPlayer;

	int iTotalEconomicValue = 0;
	int iHighestVal = 0;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			for (pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				if (pLoopCity != NULL)
				{
					int iVal = pLoopCity->getEconomicValue(pLoopCity->getOwner());
					if (iVal > iHighestVal)
						iHighestVal = iVal;

					iTotalEconomicValue += iVal;
				}
			}
		}
	}
	setTotalEconomicValue(iTotalEconomicValue);
	setHighestEconomicValue(iHighestVal);
}
//	--------------------------------------------------------------------------------
void CvGame::updateGlobalAverage()
{
	CvCity* pLoopCity;
	int iCityLoop;
	PlayerTypes eLoopPlayer;
	int iPopulation, iTotalPopulation = 0;
	int iCultureYield = 0;
	int iScienceYield = 0;
	int iDefenseYield = 0;
	int iGoldYield = 0;

	std::vector<float> vfCultureYield;
	std::vector<float> vfScienceYield;
	std::vector<float> vfDefenseYield;
	std::vector<float> vfGoldYield;

	std::vector<int> viTechMedian;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && !GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			if (GET_PLAYER(eLoopPlayer).getNumCities() > 0)
			{
				int iTechProgress = GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();

				viTechMedian.push_back(iTechProgress);
			}

			for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				if(pLoopCity != NULL)
				{
					if (pLoopCity->IsPuppet() || pLoopCity->IsRazing() || pLoopCity->IsResistance())
						continue;

					iPopulation = pLoopCity->getPopulation();
					
					iTotalPopulation += iPopulation;

					//Uncultured
					iCultureYield = pLoopCity->getJONSCulturePerTurn() * 100;
					float iCultureAvg = iCultureYield / (float)iPopulation;
					vfCultureYield.push_back((float)iCultureAvg);

					//Illiteracy
					iScienceYield = pLoopCity->getYieldRateTimes100(YIELD_SCIENCE, true);
					float iScienceAvg = iScienceYield / (float)iPopulation;
					vfScienceYield.push_back((float)iScienceAvg);
					
					//Disorder
					iDefenseYield = (pLoopCity->getYieldRateTimes100(YIELD_FOOD, true) + pLoopCity->getYieldRateTimes100(YIELD_PRODUCTION, true)) / 2;
					float iDefenseAvg = iDefenseYield / (float)iPopulation;
					vfDefenseYield.push_back((float)iDefenseAvg);

					//Poverty
					iGoldYield = pLoopCity->getYieldRateTimes100(YIELD_GOLD, true);
					float iGoldAvg = iGoldYield / (float)iPopulation;
					vfGoldYield.push_back((float)iGoldAvg);
				}		
			}
		}
	}
	//Cannot define median if calculations are at zero.
	if (vfCultureYield.empty() || vfScienceYield.empty() || vfDefenseYield.empty() || vfGoldYield.empty() || viTechMedian.empty())
	{	
		return;
	}
	
	//Select n-th percentile of each category
	size_t n = (vfCultureYield.size() * GC.getBALANCE_HAPPINESS_THRESHOLD_PERCENTILE()) / 100;
	
	size_t nt = (viTechMedian.size() * GC.getBALANCE_HAPPINESS_THRESHOLD_PERCENTILE()) / 100;

	//Find it ...
	std::nth_element(vfCultureYield.begin(), vfCultureYield.begin()+n, vfCultureYield.end());
	std::nth_element(vfScienceYield.begin(), vfScienceYield.begin()+n, vfScienceYield.end());
	std::nth_element(vfDefenseYield.begin(), vfDefenseYield.begin()+n, vfDefenseYield.end());
	std::nth_element(vfGoldYield.begin(), vfGoldYield.begin()+n, vfGoldYield.end());
	std::nth_element(viTechMedian.begin(), viTechMedian.begin() + nt, viTechMedian.end());
	
	//And set it.
	SetCultureAverage((int)vfCultureYield[n]);
	SetScienceAverage((int)vfScienceYield[n]);
	SetDefenseAverage((int)vfDefenseYield[n]);
	SetGoldAverage((int)vfGoldYield[n]);
	SetGlobalPopulation(iTotalPopulation);

	DoGlobalAvgLogging();

	if ((int)viTechMedian[nt] > m_iGlobalTechAvg)
		m_iGlobalTechAvg = (int)viTechMedian[nt];
}
//	--------------------------------------------------------------------------------
void CvGame::SetCultureAverage(int iValue)
{
	float fAlpha = 0.65f;
	int iAverage = int(0.5f + (iValue * fAlpha) + (GetCultureAverage() * ( 1 - fAlpha)));
	m_iCultureAverage = iAverage;
}
//	--------------------------------------------------------------------------------
void CvGame::SetScienceAverage(int iValue)
{
	float fAlpha = 0.65f;
	int iAverage = int(0.5f + (iValue * fAlpha) + (GetScienceAverage() * ( 1 - fAlpha)));
	m_iScienceAverage = iAverage;
}
//	--------------------------------------------------------------------------------
void CvGame::SetDefenseAverage(int iValue)
{
	float fAlpha = 0.65f;
	int iAverage = int(0.5f + (iValue * fAlpha) + (GetDefenseAverage() * ( 1 - fAlpha)));
	m_iDefenseAverage = iAverage;
}
//	--------------------------------------------------------------------------------
void CvGame::SetGoldAverage(int iValue)
{
	float fAlpha = 0.75f;
	int iAverage = int(0.5f + (iValue * fAlpha) + (GetGoldAverage() * ( 1 - fAlpha)));
	m_iGoldAverage = iAverage;
}
//	--------------------------------------------------------------------------------
void CvGame::SetGlobalPopulation(int iValue)
{
	if(GetGlobalPopulation() != iValue)
	m_iGlobalPopulation = iValue;
}
//	--------------------------------------------------------------------------------
int CvGame::GetCultureAverage() const
{
	return m_iCultureAverage;
}
int CvGame::GetScienceAverage() const
{
	return m_iScienceAverage;
}
int CvGame::GetDefenseAverage() const
{
	return m_iDefenseAverage;
}	
int CvGame::GetGoldAverage() const
{
	return m_iGoldAverage;
}
int CvGame::GetGlobalPopulation() const
{
	return m_iGlobalPopulation;
}
int CvGame::GetGlobalTechAvg() const
{
	return m_iGlobalTechAvg;
}

void CvGame::DoGlobalAvgLogging()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutput;
		CvString strTemp;

		CvString strLogName = "GlobalMedian_Log.csv";
		FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		strOutput.Format("Turn: %03d", GC.getGame().getElapsedGameTurns());
		strTemp.Format("Food/Production: %d", m_iDefenseAverage);
		strOutput += ", " + strTemp;

		strTemp.Format("Gold: %d", m_iGoldAverage);
		strOutput += ", " + strTemp;

		strTemp.Format("Science: %d", m_iScienceAverage);
		strOutput += ", " + strTemp;

		strTemp.Format("Culture: %d", m_iCultureAverage);
		strOutput += ", " + strTemp;
		
		pLog->Msg(strOutput);
	}
}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
void CvGame::SetHighestPotential()
{
	m_iLargestBasePotential = 0;

	// first pass to get the largest base potential available
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive())
		{
			CvPlayer& kLoopPlayer = GET_PLAYER(eLoopPlayer);

			if(!kLoopPlayer.isAlive() || kLoopPlayer.isBarbarian() || kLoopPlayer.isMinorCiv())
			{
				continue;
			}

			int iLoop = 0;
			for(CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{				
				int iPotential = kLoopPlayer.GetEspionage()->CalcPerTurn( pLoopCity->isCapital() ? SPY_STATE_GATHERING_INTEL : SPY_STATE_PREPARING_HEIST, pLoopCity, -1);

				if (iPotential > m_iLargestBasePotential)
				{
					m_iLargestBasePotential = iPotential;
				}
			}
		}
	}
	// second pass to set the base potential for each city
	if(m_iLargestBasePotential > 0)
	{
		for(int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if(!kLoopPlayer.isAlive() || kLoopPlayer.isBarbarian() || kLoopPlayer.isMinorCiv())
			{
				continue;
			}

			//Check for spies - this determines if we need to do any updates or anything.
			bool bNotify = false;
			for(int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
			{
				PlayerTypes eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;
				if(eLoopPlayer2 != NO_PLAYER && GET_PLAYER(eLoopPlayer2).isAlive())
				{
					CvPlayer& kLoopPlayer2 = GET_PLAYER(eLoopPlayer2);

					if((kLoopPlayer.GetID() == kLoopPlayer2.GetID()) || !kLoopPlayer2.isAlive() || kLoopPlayer2.isBarbarian() || kLoopPlayer2.isMinorCiv())
					{
						continue;
					}
					if(GET_TEAM(kLoopPlayer.getTeam()).isHasMet(kLoopPlayer2.getTeam()))
					{
						if(kLoopPlayer2.GetEspionage()->GetNumAliveSpies() > 0)
						{
							bNotify = true;
							break;
						}
					}
				}
			}

			int iLoop = 0;
			for(CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{				
				int iPotential = kLoopPlayer.GetEspionage()->CalcPerTurn( pLoopCity->isCapital() ? SPY_STATE_GATHERING_INTEL : SPY_STATE_PREPARING_HEIST, pLoopCity, -1);
				pLoopCity->SetEspionageRanking(iPotential, bNotify);
			}
		}
	}
}
void CvGame::DoBarbCountdown()
{
	int iOtherMinorLoop;
	PlayerTypes eOtherMinor;
	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		// Another Minor
		if(!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;

		// They not alive!
		if(!GET_TEAM(eLoopTeam).isAlive())
			continue;

		for(iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;

			if(eOtherMinor == NO_PLAYER)
				continue;

			// Other minor is on this team
			if(GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				if(GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetTurnsSinceRebellion() > 0)
				{
					int iRebellionSpawn = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetTurnsSinceRebellion();

					GET_PLAYER(eOtherMinor).GetMinorCivAI()->ChangeTurnsSinceRebellion(-1);

					//Rebel Spawn - once every 4 turns
					if (iRebellionSpawn == /*20*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 100) / 100)
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
					else if(iRebellionSpawn == /*16*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 80) / 100)
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
					else if(iRebellionSpawn == /*12*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 60) / 100)
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
					else if(iRebellionSpawn == /*8*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 40) / 100)
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
					else if(iRebellionSpawn == /*4*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 20) / 100)
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
				}
			}
		}
	}
}
void CvGame::SetLastTurnCSAnnexed(int iValue)
{
	m_iLastTurnCSSurrendered = iValue;
}
int CvGame::GetLastTurnCSAnnexed()
{
	return m_iLastTurnCSSurrendered;
}
#endif
//	--------------------------------------------------------------------------------
const CvReplayMessage* CvGame::getReplayMessage(uint i) const
{
	if(i < m_listReplayMessages.size())
	{
		return &(m_listReplayMessages[i]);
	}

	return NULL;
}

// Private Functions...

//	--------------------------------------------------------------------------------
void CvGame::Read(FDataStream& kStream)
{
	int iI;

	reset(NO_HANDICAP);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iEndTurnMessagesSent;
	kStream >> m_iElapsedGameTurns;
	kStream >> m_iStartTurn;
	kStream >> m_iWinningTurn;
	kStream >> m_iStartYear;
	kStream >> m_iEstimateEndTurn;
	kStream >> m_iDefaultEstimateEndTurn;
	kStream >> m_iTurnSlice;
	kStream >> m_iCutoffSlice;
	kStream >> m_iNumCities;
	kStream >> m_iTotalPopulation;
	kStream >> m_iTotalEconomicValue;
	kStream >> m_iHighestEconomicValue;
	kStream >> m_iNoNukesCount;
	kStream >> m_iNukesExploded;
	kStream >> m_iMaxPopulation;
#if defined(MOD_BALANCE_CORE_GLOBAL_IDS)
	kStream >> m_iGlobalAssetCounterAllPreviousTurns;
	kStream >> m_iGlobalAssetCounterCurrentTurn;
#endif
	kStream >> m_iUnused1;
	kStream >> m_iUnused2;
	kStream >> m_iUnused3;
	kStream >> m_iInitPopulation;
	kStream >> m_iInitLand;
	kStream >> m_iInitTech;
	kStream >> m_iInitWonders;
	kStream >> m_iAIAutoPlay;

	kStream >> m_iTotalReligionTechCost;
	kStream >> m_iCachedWorldReligionTechProgress;
	kStream >> m_iUnitedNationsCountdown;
	kStream >> m_iNumVictoryVotesTallied;
	kStream >> m_iNumVictoryVotesExpected;
	kStream >> m_iVotesNeededForDiploVictory;
	kStream >> m_iMapScoreMod;

	// m_uiInitialTime not saved

	kStream >> m_bScoreDirty;
	kStream >> m_bCircumnavigated;
	// m_bDebugMode not saved
	kStream >> m_bFinalInitialized;
	// m_bPbemTurnSent not saved
	kStream >> m_bHotPbemBetweenTurns;
	// m_bPlayerOptionsSent not saved
	kStream >> m_bNukesValid;
	kStream >> m_bEndGameTechResearched;
	kStream >> m_bTunerEverConnected;
	kStream >> m_bTutorialEverAttacked;
	kStream >> m_bStaticTutorialActive;
	kStream >> m_bEverRightClickMoved;
	kStream >> m_AdvisorMessagesViewed;
	kStream >> m_eHandicap;
	kStream >> m_ePausePlayer;
	kStream >> m_eAIAutoPlayReturnPlayer;
	kStream >> m_eBestLandUnit;
	kStream >> m_eWinner;
	kStream >> m_eVictory;
	kStream >> m_eGameState;
	if(m_eGameState == GAMESTATE_OVER)
	{
		m_eGameState = GAMESTATE_EXTENDED;
	}
	kStream >> m_eBestWondersPlayer;
	kStream >> m_eBestPoliciesPlayer;
	kStream >> m_eBestGreatPeoplePlayer;
	kStream >> m_eReligionTech;
	kStream >> m_eIndustrialRoute;

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	MOD_SERIALIZE_READ(39, kStream, m_eTeamThatCircumnavigated, NO_TEAM);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	MOD_SERIALIZE_READ(55, kStream, m_bVictoryRandomization, false);
	MOD_SERIALIZE_READ(55, kStream, m_iCultureAverage, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iScienceAverage, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iDefenseAverage, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iGoldAverage, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iGlobalTechAvg, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iGlobalPopulation, 0);
	MOD_SERIALIZE_READ(55, kStream, m_iLastTurnCSSurrendered, 0);

	ArrayWrapper<int> wrapm_aiGreatestMonopolyPlayer(GC.getNumResourceInfos(), m_aiGreatestMonopolyPlayer);
	kStream >> wrapm_aiGreatestMonopolyPlayer;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	MOD_SERIALIZE_READ(66, kStream, m_iLargestBasePotential, 0);
#endif

	kStream >> m_strScriptData;

	ArrayWrapper<int> wrapm_aiEndTurnMessagesReceived(MAX_PLAYERS, m_aiEndTurnMessagesReceived);
	kStream >> wrapm_aiEndTurnMessagesReceived;

	ArrayWrapper<int> wrapm_aiRankPlayer(MAX_PLAYERS, m_aiRankPlayer);
	kStream >> wrapm_aiRankPlayer;

	ArrayWrapper<int> wrapm_aiPlayerRank(MAX_PLAYERS, m_aiPlayerRank);
	kStream >> wrapm_aiPlayerRank;

	ArrayWrapper<int> wrapm_aiPlayerScore(MAX_PLAYERS, m_aiPlayerScore);
	kStream >> wrapm_aiPlayerScore;

	ArrayWrapper<int> wrapm_aiRankTeam(MAX_TEAMS, m_aiRankTeam);
	kStream >> wrapm_aiRankTeam;

	ArrayWrapper<int> wrapm_aiTeamRank(MAX_TEAMS, m_aiTeamRank);
	kStream >> wrapm_aiTeamRank;

	ArrayWrapper<int> wrapm_aiTeamScore(MAX_TEAMS, m_aiTeamScore);
	kStream >> wrapm_aiTeamScore;

	UnitArrayHelpers::Read(kStream, m_paiUnitCreatedCount);
	UnitClassArrayHelpers::Read(kStream, m_paiUnitClassCreatedCount);
	BuildingClassArrayHelpers::Read(kStream, m_paiBuildingClassCreatedCount);

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiProjectCreatedCount, GC.getNumProjectInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiVoteOutcome, GC.getNumVoteInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSecretaryGeneralTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiVoteTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiDiploVote, GC.getNumVoteSourceInfos());

	ArrayWrapper<int> wrapm_aiVotesCast(MAX_CIV_TEAMS, m_aiVotesCast);
	kStream >> wrapm_aiVotesCast;

	ArrayWrapper<int> wrapm_aiPreviousVotesCast(MAX_CIV_TEAMS, m_aiPreviousVotesCast);
	kStream >> wrapm_aiPreviousVotesCast;

	ArrayWrapper<int> wrapm_aiNumVotesForTeam(MAX_CIV_TEAMS, m_aiNumVotesForTeam);
	kStream >> wrapm_aiNumVotesForTeam;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabSpecialUnitValid, GC.getNumSpecialUnitInfos());

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_ppaaiTeamVictoryRank, GC.getNUM_VICTORY_POINT_AWARDS(), GC.getNumVictoryInfos());
#if defined(MOD_BALANCE_CORE_JFD)
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_ppaiContractUnits, GC.getNumUnitInfos(), GC.getNumContractInfos());
#endif
	kStream >> m_aszDestroyedCities;
	kStream >> m_aszGreatPeopleBorn;

	m_mapRand.read(kStream);
	m_jonRand.read(kStream);
	{
		clearReplayMessageMap();

		unsigned int uiReplayMessageVersion = 1;
		int iSize = 0;

		kStream >> uiReplayMessageVersion;

		kStream >> iSize;
		for(int i = 0; i < iSize; i++)
		{
			CvReplayMessage message;
			message.read(kStream, uiReplayMessageVersion);
			m_listReplayMessages.push_back(message);
		}

	}

	kStream >> m_iNumSessions;
	if(!isNetworkMultiPlayer())
	{
		++m_iNumSessions;
	}

	kStream >> m_aPlotExtraYields;
	kStream >> m_aPlotExtraCosts;


	// Get the active player information from the initialization structure
	if(!isGameMultiPlayer())
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				setActivePlayer((PlayerTypes)iI);
				break;
			}
		}
		Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_RELOAD");
		localizedText << m_iNumSessions;
		addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getActivePlayer(), localizedText.toUTF8());
	}

	if(isOption(GAMEOPTION_NEW_RANDOM_SEED))
	{
		if(!isNetworkMultiPlayer())
		{
			m_jonRand.reseed(timeGetTime());
		}
	}

	kStream >> m_bArchaeologyTriggered;

	kStream >> m_iEarliestBarbarianReleaseTurn;
	kStream >> m_kGameDeals;
	kStream >> *m_pGameReligions;
	kStream >> *m_pGameCulture;
	kStream >> *m_pGameLeagues;
	kStream >> *m_pGameTrade;

#if defined(MOD_BALANCE_CORE)
	// Don't even try to worry about save compatibility
	kStream >> *m_pGameCorporations;
	kStream >> *m_pGameContracts;
#endif

	unsigned int lSize = 0;
	kStream >> lSize;
	if(lSize > 0)
	{
		//Deserialize the embedded SQLite database file.
		CvString strUTF8DatabasePath = gDLL->GetCacheFolderPath();
		strUTF8DatabasePath += "Civ5SavedGameDatabase.db";

		// Need to Convert the UTF-8 string into a wide character string.
		std::wstring wstrDatabasePath = CvStringUtils::FromUTF8ToUTF16(strUTF8DatabasePath);

		FIFile* pkFile = FFILESYSTEM.Create(wstrDatabasePath.c_str(), FIFile::modeWrite);
		if (pkFile != NULL)
		{
			byte* szBuffer = GetTempHeap()->Allocate(sizeof(char) * lSize);
			ZeroMemory((void*)szBuffer, lSize);

			kStream.ReadIt(lSize, szBuffer);

			pkFile->Write(szBuffer, lSize);

			pkFile->Close();

			GetTempHeap()->DeAllocate(szBuffer);
		}
		else
		{
			CvAssertMsg(false, "Cannot open Civ5SavedGameDatabase.db for write! Does something have this opened?");
		}
	}

	//when loading from file, we need to reset m_lastTurnAICivsProcessed 
	//so that updateMoves() can turn active players after loading an autosave in simultaneous turns multiplayer.
	m_lastTurnAICivsProcessed = -1;

	// used to be a static in updateMoves but made a member due to it not being re-inited and maybe causing issues when loading after an exit-to-menu. Not serialized - I wasn't willing to think about the implications.
	m_processPlayerAutoMoves = false;
}

//	---------------------------------------------------------------------------
void CvGame::ReadSupportingClassData(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;	
	MOD_SERIALIZE_INIT_READ(kStream);

	CvBarbarians::Read(kStream, uiVersion);
	CvGoodyHuts::Read(kStream, uiVersion);
}

//	--------------------------------------------------------------------------------
void CvGame::Write(FDataStream& kStream) const
{
	// Current version number
	kStream << g_CurrentCvGameVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iEndTurnMessagesSent;
	kStream << m_iElapsedGameTurns;
	kStream << m_iStartTurn;
	kStream << m_iWinningTurn;
	kStream << m_iStartYear;
	kStream << m_iEstimateEndTurn;
	kStream << m_iDefaultEstimateEndTurn;
	kStream << m_iTurnSlice;
	kStream << m_iCutoffSlice;
	kStream << m_iNumCities;
	kStream << m_iTotalPopulation;
	kStream << m_iTotalEconomicValue;
	kStream << m_iHighestEconomicValue;
	kStream << m_iNoNukesCount;
	kStream << m_iNukesExploded;
	kStream << m_iMaxPopulation;
#if defined(MOD_BALANCE_CORE_GLOBAL_IDS)
	kStream << m_iGlobalAssetCounterAllPreviousTurns;
	kStream << m_iGlobalAssetCounterCurrentTurn;
#endif
	kStream << m_iUnused1;
	kStream << m_iUnused2;
	kStream << m_iUnused3;
	kStream << m_iInitPopulation;
	kStream << m_iInitLand;
	kStream << m_iInitTech;
	kStream << m_iInitWonders;
	kStream << m_iAIAutoPlay;
	kStream << m_iTotalReligionTechCost;
	kStream << m_iCachedWorldReligionTechProgress;
	kStream << m_iUnitedNationsCountdown;
	kStream << m_iNumVictoryVotesTallied;
	kStream << m_iNumVictoryVotesExpected;
	kStream << m_iVotesNeededForDiploVictory;
	kStream << m_iMapScoreMod;

	// m_uiInitialTime not saved

	kStream << m_bScoreDirty;
	kStream << m_bCircumnavigated;
	// m_bDebugMode not saved
	kStream << m_bFinalInitialized;
	// m_bPbemTurnSent not saved
	kStream << m_bHotPbemBetweenTurns;
	// m_bPlayerOptionsSent not saved
	kStream << m_bNukesValid;
	kStream << m_bEndGameTechResearched;
	kStream << TunerEverConnected();
	kStream << m_bTutorialEverAttacked;
	kStream << m_bStaticTutorialActive;
	kStream << m_bEverRightClickMoved;
	kStream << m_AdvisorMessagesViewed;

	kStream << m_eHandicap;
	kStream << m_ePausePlayer;
	kStream << m_eAIAutoPlayReturnPlayer;
	kStream << m_eBestLandUnit;
	kStream << m_eWinner;
	kStream << m_eVictory;
	kStream << m_eGameState;
	kStream << m_eBestWondersPlayer;
	kStream << m_eBestPoliciesPlayer;
	kStream << m_eBestGreatPeoplePlayer;
	kStream << m_eReligionTech;
	kStream << m_eIndustrialRoute;

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	MOD_SERIALIZE_WRITE(kStream, m_eTeamThatCircumnavigated);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	MOD_SERIALIZE_WRITE(kStream, m_bVictoryRandomization);
	MOD_SERIALIZE_WRITE(kStream, m_iCultureAverage);
	MOD_SERIALIZE_WRITE(kStream, m_iScienceAverage);
	MOD_SERIALIZE_WRITE(kStream, m_iDefenseAverage);
	MOD_SERIALIZE_WRITE(kStream, m_iGoldAverage);
	MOD_SERIALIZE_WRITE(kStream, m_iGlobalTechAvg);
	MOD_SERIALIZE_WRITE(kStream, m_iGlobalPopulation);
	MOD_SERIALIZE_WRITE(kStream, m_iLastTurnCSSurrendered);
	kStream << ArrayWrapper<int>(GC.getNumResourceInfos(), m_aiGreatestMonopolyPlayer);
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	MOD_SERIALIZE_WRITE(kStream, m_iLargestBasePotential);
#endif



	kStream << m_strScriptData;

	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiEndTurnMessagesReceived);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiRankPlayer);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiPlayerRank);
	kStream << ArrayWrapper<int>(MAX_PLAYERS, m_aiPlayerScore);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiRankTeam);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiTeamRank);
	kStream << ArrayWrapper<int>(MAX_TEAMS, m_aiTeamScore);

	UnitArrayHelpers::Write(kStream, m_paiUnitCreatedCount, GC.getNumUnitInfos());

	UnitClassArrayHelpers::Write(kStream, m_paiUnitClassCreatedCount, GC.getNumUnitClassInfos());
	BuildingClassArrayHelpers::Write(kStream, m_paiBuildingClassCreatedCount, GC.getNumBuildingClassInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<ProjectTypes, int>(kStream, m_paiProjectCreatedCount, GC.getNumProjectInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteTypes, PlayerVoteTypes>(kStream, m_paiVoteOutcome, GC.getNumVoteInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiSecretaryGeneralTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiVoteTimer, GC.getNumVoteSourceInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<VoteSourceTypes, int>(kStream, m_aiDiploVote, GC.getNumVoteSourceInfos());

	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiVotesCast);
	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiPreviousVotesCast);
	kStream << ArrayWrapper<int>(MAX_CIV_TEAMS, m_aiNumVotesForTeam);

	CvInfosSerializationHelper::WriteHashedDataArray<SpecialUnitTypes, bool>(kStream, m_pabSpecialUnitValid, GC.getNumSpecialUnitInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<VictoryTypes, int>(kStream, m_ppaaiTeamVictoryRank, GC.getNUM_VICTORY_POINT_AWARDS(), GC.getNumSpecialUnitInfos());
#if defined(MOD_BALANCE_CORE_JFD)
	CvInfosSerializationHelper::WriteHashedDataArray<ContractTypes, int>(kStream, m_ppaiContractUnits,  GC.getNumUnitInfos(), GC.getNumContractInfos());
#endif
	kStream << m_aszDestroyedCities;
	kStream << m_aszGreatPeopleBorn;

	m_mapRand.write(kStream);
	m_jonRand.write(kStream);

	const int iSize = m_listReplayMessages.size();
	kStream << CvReplayMessage::Version();
	kStream << iSize;

	ReplayMessageList::const_iterator it;
	for(it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
	{
		(*it).write(kStream);
	}

	kStream << m_iNumSessions;

	kStream << m_aPlotExtraYields;
	kStream << m_aPlotExtraCosts;

	kStream << m_bArchaeologyTriggered;

	kStream << m_iEarliestBarbarianReleaseTurn;

	kStream << m_kGameDeals;
	kStream << *m_pGameReligions;
	kStream << *m_pGameCulture;
	kStream << *m_pGameLeagues;
	kStream << *m_pGameTrade;

#if defined(MOD_BALANCE_CORE)
	kStream << *m_pGameCorporations;
	kStream << *m_pGameContracts;
#endif

	//In Version 8, Serialize Saved Game database
	CvString strPath = gDLL->GetCacheFolderPath();
	strPath += "Civ5SavedGameDatabase.db";

	//Need to Convert the UTF-8 string into a wide character string so windows can open this file.
	wchar_t savePath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_UTF8, 0, strPath.c_str(), -1, savePath, MAX_PATH);

	HANDLE hFile = CreateFileW(savePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		if(dwSize != INVALID_FILE_SIZE)
		{
			byte* szBuffer = GetTempHeap()->Allocate(sizeof(char) * dwSize);
			ZeroMemory((void*)szBuffer, dwSize);

			DWORD dwBytesRead = 0;
			if(ReadFile(hFile, szBuffer, dwSize, &dwBytesRead, NULL) == TRUE)
			{
				//Serialize out the file size first.
				kStream << dwBytesRead;
				kStream.WriteIt(dwBytesRead, szBuffer);
			}

			GetTempHeap()->DeAllocate(szBuffer);
		}
		else
		{
			CvAssertMsg(false, "Saved game database exists, but could not get file size???");
		}
	
		if(CloseHandle(hFile) == FALSE)
		{
			CvAssertMsg(false, "Could not close file handle to saved game database!");
		}
	}
	else
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
		if(GetFileAttributesExW(savePath, GetFileExInfoStandard, &fileAttributes) != INVALID_FILE_ATTRIBUTES)
		{
			CvAssertMsg(false,"Saved game database exists, but could not open it!");
		}

		long nilSize = 0;
		kStream << nilSize;
	}
}

//	---------------------------------------------------------------------------
void CvGame::WriteSupportingClassData(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	CvBarbarians::Write(kStream);
	CvGoodyHuts::Write(kStream);
}

//	--------------------------------------------------------------------------------
void CvGame::writeReplay(FDataStream& kStream)
{
	CvReplayInfo replayInfo;
	replayInfo.createInfo();
	replayInfo.write(kStream);
}

//	-----------------------------------------------------------------------------------------------
void CvGame::saveReplay()
{
	gDLL->saveReplay();

	CvPlayerAI& activePlayer = GET_PLAYER(getActivePlayer());

	bool playerTeamWon = (getActiveTeam() == getWinner());
	int score = activePlayer.GetScore(true, playerTeamWon);

	if(!isHotSeat())
	{
		gDLL->RecordVictoryInformation(score);
		gDLL->RecordLeaderboardScore(score);
	}
}
//	-----------------------------------------------------------------------------------------------

void CvGame::showEndGameSequence()
{

	GC.GetEngineUserInterface()->OpenEndGameMenu();
}

//	--------------------------------------------------------------------------------
void CvGame::addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv)
{
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
	CvCivilizationInfo* pkBarbarianCivInfo = GC.getCivilizationInfo(static_cast<CivilizationTypes>(GC.getBARBARIAN_CIVILIZATION()));

	if(pkCivilizationInfo == NULL || pkBarbarianCivInfo == NULL)
	{
		//Should never happen.
		CvAssert(false);
		return;
	}

	PlayerColorTypes eColor = (PlayerColorTypes)pkCivilizationInfo->getDefaultPlayerColor();

	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if(eColor == NO_PLAYERCOLOR || GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor)
		{
			for(int iK = 0; iK < GC.GetNumPlayerColorInfos(); iK++)
			{
				const PlayerColorTypes ePlayerColor = static_cast<PlayerColorTypes>(iK);
				CvPlayerColorInfo* pkPlayerColorInfo = GC.GetPlayerColorInfo(ePlayerColor);
				if(pkPlayerColorInfo)
				{
					if(iK != pkBarbarianCivInfo->getDefaultPlayerColor())
					{
						bool bValid = true;

						for(int iL = 0; iL < MAX_CIV_PLAYERS; iL++)
						{
							if(GET_PLAYER((PlayerTypes)iL).getPlayerColor() == ePlayerColor)
							{
								bValid = false;
								break;
							}
						}

						if(bValid)
						{
							eColor = ePlayerColor;
							iI = MAX_CIV_PLAYERS;
							break;
						}
					}
				}
			}
		}
	}

	CvPreGame::setLeaderHead(eNewPlayer, eLeader);
	CvPreGame::setCivilization(eNewPlayer, eCiv);
	CvPreGame::setSlotStatus(eNewPlayer, SS_COMPUTER);
	CvPreGame::setPlayerColor(eNewPlayer, eColor);
	GET_PLAYER(eNewPlayer).init(eNewPlayer);
}

//	--------------------------------------------------------------------------------
int CvGame::getPlotExtraYield(int iX, int iY, YieldTypes eYield) const
{
	for(std::vector<PlotExtraYield>::const_iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_aeExtraYield[eYield];
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	bool bFound = false;

	for(std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_aeExtraYield[eYield] += iExtraYield;
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		PlotExtraYield kExtraYield;
		kExtraYield.m_iX = iX;
		kExtraYield.m_iY = iY;
		for(int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if(eYield == i)
			{
				kExtraYield.m_aeExtraYield.push_back(iExtraYield);
			}
			else
			{
				kExtraYield.m_aeExtraYield.push_back(0);
			}
		}
		m_aPlotExtraYields.push_back(kExtraYield);
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL != pPlot)
	{
		pPlot->updateYield();
	}
}

//	--------------------------------------------------------------------------------
void CvGame::removePlotExtraYield(int iX, int iY)
{
	for(std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraYields.erase(it);
			break;
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL != pPlot)
	{
		pPlot->updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvGame::getPlotExtraCost(int iX, int iY) const
{
	for(std::vector<PlotExtraCost>::const_iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_iCost;
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvGame::changePlotExtraCost(int iX, int iY, int iCost)
{
	bool bFound = false;

	for(std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_iCost += iCost;
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		PlotExtraCost kExtraCost;
		kExtraCost.m_iX = iX;
		kExtraCost.m_iY = iY;
		kExtraCost.m_iCost = iCost;
		m_aPlotExtraCosts.push_back(kExtraCost);
	}
}

//	--------------------------------------------------------------------------------
void CvGame::removePlotExtraCost(int iX, int iY)
{
	for(std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraCosts.erase(it);
			break;
		}
	}
}


// CACHE: cache frequently used values
///////////////////////////////////////

//	--------------------------------------------------------------------------------
void CvGame::doUpdateCacheOnTurn()
{
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for(int iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		pLoopPlot->updateWaterFlags();
	}
}

//	--------------------------------------------------------------------------------
bool CvGame::isCivEverActive(CivilizationTypes eCivilization) const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kLoopPlayer.isEverAlive())
		{
			if(kLoopPlayer.getCivilizationType() == eCivilization)
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvGame::isLeaderEverActive(LeaderHeadTypes eLeader) const
{
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if(kLoopPlayer.isEverAlive())
		{
			if(kLoopPlayer.getLeaderType() == eLeader)
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvGame::isUnitEverActive(UnitTypes eUnit) const
{
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
		return false;

	for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
	{
		const CivilizationTypes eCiv = static_cast<CivilizationTypes>(iCiv);
		CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
		if(pkCivilizationInfo)
		{
			if(isCivEverActive(eCiv))
			{
				if(eUnit == pkCivilizationInfo->getCivilizationUnits(pkUnitInfo->GetUnitClassType()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvGame::isBuildingEverActive(BuildingTypes eBuilding) const
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo)
	{
		for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
		{
			const CivilizationTypes eCivilization = static_cast<CivilizationTypes>(iCiv);
			CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
			if(pkCivilizationInfo)
			{
				if(isCivEverActive(eCivilization))
				{
					if(eBuilding == pkCivilizationInfo->getCivilizationBuildings(pkBuildingInfo->GetBuildingClassType()))
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
/// What route type forms an industrial connection?
RouteTypes CvGame::GetIndustrialRoute() const
{
	return m_eIndustrialRoute;
}

//	--------------------------------------------------------------------------------
/// What route type forms an industrial connection?
void CvGame::DoUpdateIndustrialRoute()
{
	RouteTypes eIndustrialRoute = NO_ROUTE;

	for(int iRouteLoop = 0; iRouteLoop < GC.getNumRouteInfos(); iRouteLoop++)
	{
		const RouteTypes eRoute = static_cast<RouteTypes>(iRouteLoop);
		CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
		if(pkRouteInfo)
		{
			if(pkRouteInfo->IsIndustrial())
			{
				eIndustrialRoute = eRoute;
				break;
			}
		}
	}

	m_eIndustrialRoute = eIndustrialRoute;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
TeamTypes CvGame::GetTeamThatCircumnavigated() const
{
	return (TeamTypes) m_eTeamThatCircumnavigated;
}

//	--------------------------------------------------------------------------------
void CvGame::SetTeamThatCircumnavigated(TeamTypes eNewValue)
{
	m_eTeamThatCircumnavigated = eNewValue;
}
#endif

//	--------------------------------------------------------------------------------
CvSiteEvaluatorForSettler* CvGame::GetSettlerSiteEvaluator()
{
	return m_pSettlerSiteEvaluator;
}

//	--------------------------------------------------------------------------------
CvSiteEvaluatorForStart* CvGame::GetStartSiteEvaluator()
{
	return m_pStartSiteEvaluator;
}

//	--------------------------------------------------------------------------------
IStartPositioner* CvGame::GetStartPositioner()
{
	return m_pStartPositioner;
}

//	--------------------------------------------------------------------------------
CvGameDeals& CvGame::GetGameDeals()
{
	return m_kGameDeals;
}

//	--------------------------------------------------------------------------------
CvGameReligions* CvGame::GetGameReligions()
{
	return m_pGameReligions;
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
CvGameCorporations* CvGame::GetGameCorporations()
{
	return m_pGameCorporations;
}
//	--------------------------------------------------------------------------------
CvGameContracts* CvGame::GetGameContracts()
{
	return m_pGameContracts;
}
#endif

//	--------------------------------------------------------------------------------
CvGameCulture* CvGame::GetGameCulture()
{
	return m_pGameCulture;
}

//	--------------------------------------------------------------------------------
CvGameLeagues* CvGame::GetGameLeagues()
{
	return m_pGameLeagues;
}

//	--------------------------------------------------------------------------------
CvGameTrade* CvGame::GetGameTrade()
{
	return m_pGameTrade;
}

#if defined(MOD_API_LUA_EXTENSIONS)
//	--------------------------------------------------------------------------------
CvString CvGame::getDllGuid() const
{
	CvString szDllGuid = "";
	
	GUID guid = CvDllGameContext::GetSingleton()->GetDLLGUID();
	unsigned long d1 = guid.Data1;
	unsigned short d2 = guid.Data2;
	unsigned short d3 = guid.Data3;
	unsigned char* d4 = guid.Data4;

	CvString::format(szDllGuid, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", d1, d2, d3, d4[0], d4[1], d4[2], d4[3], d4[4], d4[5], d4[6], d4[7]);

	return szDllGuid;
}
#endif

//	--------------------------------------------------------------------------------
CvAdvisorCounsel* CvGame::GetAdvisorCounsel()
{
	return m_pAdvisorCounsel;
}

//	--------------------------------------------------------------------------------
CvAdvisorRecommender* CvGame::GetAdvisorRecommender()
{
	return m_pAdvisorRecommender;
}

//	--------------------------------------------------------------------------------
int CvGame::GetTurnsBetweenMinorCivElections()
{
	int iTurnsBetweenCityStateElections = GC.getESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS();
	iTurnsBetweenCityStateElections *= GC.getGame().getGameSpeedInfo().getMinorCivElectionFreqMod();
	iTurnsBetweenCityStateElections /= 100;
	return iTurnsBetweenCityStateElections;
}

//	--------------------------------------------------------------------------------
int CvGame::GetTurnsUntilMinorCivElection()
{
	int iMod = 0;
	int iTurnsBetweenCityStateElections = GetTurnsBetweenMinorCivElections();

	if(iTurnsBetweenCityStateElections != 0)
	{
		iMod = getGameTurn() % iTurnsBetweenCityStateElections;
	}
	if(iMod == 0)
	{
		return 0;
	}
	else
	{
		return iTurnsBetweenCityStateElections - iMod;
	}
}

//------------------------------------------------------------------------------
/// Get the action associated with the supplied key.
/// This will also test to see if the action can actually be done.
/// Returns: the action info index or -1.
int CvGame::GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
	int i;
	int iActionIndex = -1;
	int iPriority = -1;


	for(i=0; i<GC.getNumActionInfos(); i++)
	{
		CvActionInfo& thisActionInfo = *GC.getActionInfo(i);
		if((((thisActionInfo.getHotKeyVal() == iKeyStroke) &&
		        (thisActionInfo.getHotKeyPriority() > iPriority) &&
		        (bAlt == thisActionInfo.isAltDown()) &&
		        (bShift == thisActionInfo.isShiftDown()) &&
		        (bCtrl == thisActionInfo.isCtrlDown())
		    )
		        ||
		        ((thisActionInfo.getHotKeyValAlt() == iKeyStroke) &&
		         (thisActionInfo.getHotKeyPriorityAlt() > iPriority) &&
		         (bAlt == thisActionInfo.isAltDownAlt()) &&
		         (bShift == thisActionInfo.isShiftDownAlt()) &&
		         (bCtrl == thisActionInfo.isCtrlDownAlt())
		        )
		   )
		        &&
		        (canHandleAction(i))
		  )
		{
			iPriority = thisActionInfo.getHotKeyPriority();
			iActionIndex = i;
		}
	}

	return iActionIndex;
}

//------------------------------------------------------------------------------
/// Get the action associated with the supplied key.
/// This will NOT test to see if the action can actually be done.
/// Returns: the action info index or -1.
int CvGame::IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
	int i;
	int iActionIndex = -1;
	int iPriority = -1;


	for(i=0; i<GC.getNumActionInfos(); i++)
	{
		CvActionInfo& thisActionInfo = *GC.getActionInfo(i);
		if((((thisActionInfo.getHotKeyVal() == iKeyStroke) &&
			(thisActionInfo.getHotKeyPriority() > iPriority) &&
			(bAlt == thisActionInfo.isAltDown()) &&
			(bShift == thisActionInfo.isShiftDown()) &&
			(bCtrl == thisActionInfo.isCtrlDown())
			)
			||
			((thisActionInfo.getHotKeyValAlt() == iKeyStroke) &&
			(thisActionInfo.getHotKeyPriorityAlt() > iPriority) &&
			(bAlt == thisActionInfo.isAltDownAlt()) &&
			(bShift == thisActionInfo.isShiftDownAlt()) &&
			(bCtrl == thisActionInfo.isCtrlDownAlt())
			)
			)			
			)
		{
			iPriority = thisActionInfo.getHotKeyPriority();
			iActionIndex = i;
		}
	}

	return iActionIndex;
}

//------------------------------------------------------------------------------
void CvGame::endTurnTimerSemaphoreIncrement()
{
	++m_endTurnTimerSemaphore;
}

//	--------------------------------------------------------------------------------
void CvGame::endTurnTimerSemaphoreDecrement()
{
	--m_endTurnTimerSemaphore;
	if(m_endTurnTimerSemaphore <= 0)
	{
		m_endTurnTimerSemaphore = 0;
		m_endTurnTimer.Start();
	}
}

//	--------------------------------------------------------------------------------
void CvGame::endTurnTimerReset()
{
	m_endTurnTimerSemaphore = 0;
	m_endTurnTimer.Start();
}

//	--------------------------------------------------------------------------------
/// Called when a major changes its protection status towards a minor
void CvGame::DoMinorPledgeProtection(PlayerTypes eMajor, PlayerTypes eMinor, bool bProtect, bool bPledgeNowBroken)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	if(bProtect)
	{
		CvAssertMsg(GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorProtect(eMajor), "eMajor is not allowed to protect this minor! Please send Anton your save file and version.");
	}

	gDLL->sendMinorPledgeProtection(eMajor, eMinor, bProtect, bPledgeNowBroken);
}

//	--------------------------------------------------------------------------------
/// Amount of Gold being gifted to the Minor by the active player
void CvGame::DoMinorGiftGold(PlayerTypes eMinor, int iNumGold)
{
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorGiftGold(eMinor, iNumGold);
}

//	--------------------------------------------------------------------------------
/// Do the action of a major gifting a tile improvement to a minor's plot, to improve its resource
void CvGame::DoMinorGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorGiftTileImprovement(eMajor, eMinor, iPlotX, iPlotY);
}

//	--------------------------------------------------------------------------------
/// Do the action of a major bullying gold from a minor
/// Demanded gold and a calculated bully metric are not provided (ex. from Lua, Player UI), so calculate them here
void CvGame::DoMinorBullyGold(PlayerTypes eBully, PlayerTypes eMinor)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	int iGold = GET_PLAYER(eMinor).GetMinorCivAI()->GetBullyGoldAmount(eBully);
	if (iGold <= 0)
		return;

	gDLL->sendMinorBullyGold(eBully, eMinor, iGold);
}

//	--------------------------------------------------------------------------------
/// Do the action of a major bullying a unit from a minor
void CvGame::DoMinorBullyUnit(PlayerTypes eBully, PlayerTypes eMinor)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

#if defined(MOD_BALANCE_CORE)
	UnitClassTypes eUnitClassType = GET_PLAYER(eMinor).GetMinorCivAI()->GetBullyUnit();
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClassType);
	UnitTypes eUnitType = NO_UNIT;
	if(pkUnitClassInfo != NULL)
	{
		eUnitType = ((UnitTypes)(GET_PLAYER(eMinor).getCivilizationInfo().getCivilizationUnits((int)eUnitClassType)));
	}
	if(eUnitType != NO_UNIT)
	{
#else
	UnitTypes eUnitType = (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER"); //antonjs: todo: XML/function
#endif

	gDLL->sendMinorBullyUnit(eBully, eMinor, eUnitType);
#if defined(MOD_BALANCE_CORE)
	}
#endif
}

//	--------------------------------------------------------------------------------
/// Do the action of a major buying out a minor and acquiring it
void CvGame::DoMinorBuyout(PlayerTypes eMajor, PlayerTypes eMinor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	gDLL->sendMinorBuyout(eMajor, eMinor);
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
/// Do the action of a major buying out a minor and marrying it
void CvGame::DoMinorMarriage(PlayerTypes eMajor, PlayerTypes eMinor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is not in expected range (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is not in expected range (invalid Index)");

	GET_PLAYER(eMinor).GetMinorCivAI()->DoMarriage(eMajor);
}
#endif



//	--------------------------------------------------------------------------------
/// Notification letting all non-party players know that two teams made a Research Agreement.  This is in CvGame because we only want it called once, and if it were in CvDealClasses it would be called twice, or have to be special-cased, so we'll special-case it here instead
void CvGame::DoResearchAgreementNotification(TeamTypes eTeam1, TeamTypes eTeam2)
{
	// Notify all non-parties that these civs made a research agreement.
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

		// Don't show notification if WE'RE the ones in the deal
		if(eCurNotifyTeam != eTeam1 && eCurNotifyTeam != eTeam2)
		{
			CvTeam* pCurTeam = &GET_TEAM(eCurNotifyTeam);

			if(pCurTeam->isHasMet(eTeam1) && pCurTeam->isHasMet(eTeam2))
			{
				CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
				if(pNotifications)
				{
					const char* strLeaderName = GET_PLAYER(GET_TEAM(eTeam1).getLeaderID()).getCivilizationShortDescriptionKey();
					const char* strOtherLeaderName = GET_PLAYER(GET_TEAM(eTeam2).getLeaderID()).getCivilizationShortDescriptionKey();

					Localization::String strText;
					strText = Localization::Lookup("TXT_KEY_NOTIFICATION_RESEARCH_AGREEMENT");
					strText << strLeaderName << strOtherLeaderName;

					Localization::String strSummary;
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_RESEARCH_AGREEMENT");

					pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvGame::GetResearchAgreementCost(PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	CvAssertMsg(ePlayer1 > NO_PLAYER, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer1 <= MAX_MAJOR_CIVS, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer2 > NO_PLAYER, "Invalid player. Please show Jon this.");
	CvAssertMsg(ePlayer2 <= MAX_MAJOR_CIVS, "Invalid player. Please show Jon this.");

	EraTypes ePlayer1Era = GET_TEAM(GET_PLAYER(ePlayer1).getTeam()).GetCurrentEra();
	EraTypes ePlayer2Era = GET_TEAM(GET_PLAYER(ePlayer2).getTeam()).GetCurrentEra();
	EraTypes eHighestEra = max(ePlayer1Era, ePlayer2Era);

	int iCost = GC.getEraInfo(eHighestEra)->getResearchAgreementCost();

	iCost *= getGameSpeedInfo().getGoldPercent();
	iCost /= 100;

	return iCost;
}


//	--------------------------------------------------------------------------------
/// See if someone has won a conquest Victory
/// slewis, 10.1.12 - changing conquest so that a player has to hold all capitals
void CvGame::DoTestConquestVictory()
{
	TeamTypes eTeamWhoWon = NO_TEAM;

	PlayerTypes eLoopPlayer;

	// find out how many original capitals there are
	int iNumOriginalCapitals = 0;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			int iCityLoop;
			CvCity* pLoopCity = NULL;
			for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				if (pLoopCity->getOriginalOwner() < MAX_MAJOR_CIVS && pLoopCity->IsOriginalCapital())
				{
					iNumOriginalCapitals += 1;
				}
			}
		}
	}

	CvAssertMsg(iNumOriginalCapitals > 0, "No one controls an original capital. This is wrong.");

	// find out if any team controls all the capitals
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		int iNumCapitalsControlled = 0;
		if(GET_TEAM((TeamTypes)iTeamLoop).isAlive())
		{
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == (TeamTypes)iTeamLoop)
				{
					int iCityLoop;
					CvCity* pLoopCity = NULL;
					for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
					{
						if (pLoopCity->getOriginalOwner() < MAX_MAJOR_CIVS && pLoopCity->IsOriginalCapital())
						{
							iNumCapitalsControlled += 1;
						}
					}
				}
			}

			if (iNumCapitalsControlled == iNumOriginalCapitals)
			{
				eTeamWhoWon = (TeamTypes)iTeamLoop;
				break;
			}
		}
	}

	// If we got here then only one player remains alive!
	if (eTeamWhoWon != NO_TEAM)
	{
		for(int iVictoryLoop = 0; iVictoryLoop < GC.getNumVictoryInfos(); iVictoryLoop++)
		{
			VictoryTypes eVictory = static_cast<VictoryTypes>(iVictoryLoop);
			CvVictoryInfo* pkVictoryInfo = GC.getVictoryInfo(eVictory);
			if(pkVictoryInfo)
			{
				if(pkVictoryInfo->isConquest() && isVictoryValid(eVictory))
				{
					CUSTOMLOG("Calling setWinner from DoTestConquestVictory: %i, %i", eTeamWhoWon, eVictory);
					setWinner(eTeamWhoWon, eVictory);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Player leading with Wonders
PlayerTypes CvGame::GetBestWondersPlayer()
{
	return m_eBestWondersPlayer;
}

//	--------------------------------------------------------------------------------
/// Set Player leading with Wonders
void CvGame::SetBestWondersPlayer(PlayerTypes ePlayer, int iWonderCount)
{
	int iVictoryPointChange = /*5*/ GC.getZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS();

	// Remove VPs from old player's team
	if(GetBestWondersPlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestWondersPlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestWondersPlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);

		// Notify everyone of this change.
		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer;
				CvString strSummary;

				// current player now has the most Wonders
				if(kCurNotifyPlayer.GetID() == ePlayer)
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_YOU", iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_YOU", iVictoryPointChange, iWonderCount+1);
				}
				// Unmet player
				else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_UNMET", iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_UNMET", iVictoryPointChange, iWonderCount+1);
				}
				// Player we've met
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_WONDERS_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iWonderCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_WONDERS_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey());
				}

				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Player leading with Policies
PlayerTypes CvGame::GetBestPoliciesPlayer()
{
	return m_eBestPoliciesPlayer;
}

//	--------------------------------------------------------------------------------
/// Set Player leading with Policies
void CvGame::SetBestPoliciesPlayer(PlayerTypes ePlayer, int iPolicyCount)
{
	int iVictoryPointChange = /*5*/ GC.getZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS();

	// Remove VPs from old player's team
	if(GetBestPoliciesPlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestPoliciesPlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestPoliciesPlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		CvString strBuffer;
		CvString strSummary;

		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);

		//Notify everyone
		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

			// This player has the most Policies
			if(eNotifyPlayer == ePlayer)
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_YOU", iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_YOU", iVictoryPointChange, iPolicyCount+1);
			}
			// Unmet player
			else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_UNMET", iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_UNMET", iVictoryPointChange, iPolicyCount+1);
			}
			// player met
			else
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_POLICIES_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iPolicyCount+1);
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_POLICIES_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iPolicyCount+1);
			}

			CvNotifications* pNotifications = kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Player leading with GreatPeople
PlayerTypes CvGame::GetBestGreatPeoplePlayer()
{
	return m_eBestGreatPeoplePlayer;
}

//	--------------------------------------------------------------------------------
/// Set Player leading with GreatPeople
void CvGame::SetBestGreatPeoplePlayer(PlayerTypes ePlayer, int iGreatPeopleCount)
{
	int iVictoryPointChange = /*5*/ GC.getZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS();

	// Remove VPs from old player's team
	if(GetBestGreatPeoplePlayer() != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(GetBestGreatPeoplePlayer()).getTeam()).changeVictoryPoints(-iVictoryPointChange);
	}

	m_eBestGreatPeoplePlayer = ePlayer;

	if(ePlayer != NO_PLAYER)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).changeVictoryPoints(iVictoryPointChange);

		for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
			PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
			CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
			TeamTypes eCurNotifyTeam = kCurNotifyPlayer.getTeam();

			CvNotifications* pNotifications =kCurNotifyPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer;
				CvString strSummary;

				// Active Player now has the most GreatPeople
				if(eNotifyPlayer == ePlayer)
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_YOU", iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_YOU", iVictoryPointChange, iGreatPeopleCount+1);
				}
				// Unmet player
				else if(!GET_TEAM(eCurNotifyTeam).isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_UNMET", iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_UNMET", iVictoryPointChange, iGreatPeopleCount+1);
				}
				// Player we've met
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BEST_IN_GREAT_PEOPLE_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iGreatPeopleCount+1);
					strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_BEST_IN_GREAT_PEOPLE_ANOTHER", GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey(), iVictoryPointChange, iGreatPeopleCount+1);
				}

				pNotifications->Add(NOTIFICATION_VICTORY, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Has a player researched a Tech which ends the game?
bool CvGame::IsEndGameTechResearched() const
{
	return m_bEndGameTechResearched;
}

//	--------------------------------------------------------------------------------
/// Sets whether or not a player has researched a Tech which ends the game
void CvGame::SetEndGameTechResearched(bool bValue)
{
	m_bEndGameTechResearched = bValue;
}

//	--------------------------------------------------------------------------------
bool CvGame::TunerEverConnected() const
{
	return m_bTunerEverConnected || gDLL->TunerEverConnected();
}

//	--------------------------------------------------------------------------------
bool CvGame::IsEverAttackedTutorial() const
{
	return m_bTutorialEverAttacked;
}

//	--------------------------------------------------------------------------------
void CvGame::SetEverAttackedTutorial(bool bValue)
{
	m_bTutorialEverAttacked = bValue;
}

//	--------------------------------------------------------------------------------
bool CvGame::IsEverRightClickMoved() const
{
	return m_bEverRightClickMoved;
}

//	--------------------------------------------------------------------------------
void CvGame::SetEverRightClickMoved(bool bValue)
{
	m_bEverRightClickMoved = bValue;
}

//	--------------------------------------------------------------------------------
bool CvGame::IsCombatWarned() const
{
	return m_bCombatWarned;
}

//	--------------------------------------------------------------------------------
void CvGame::SetCombatWarned(bool bValue)
{
	m_bCombatWarned = bValue;
}

//	--------------------------------------------------------------------------------
/// Shortcut for generating production mod tool tip help
void CvGame::BuildProdModHelpText(CvString* toolTipSink, const char* strTextKey, int iMod, const char* strExtraKey) const
{
	if(iMod != 0 && toolTipSink != NULL)
	{
		Localization::String localizedText = Localization::Lookup(strTextKey);
		localizedText << iMod;

		if(strExtraKey)
		{
			std::string extraKey(strExtraKey);
			if(!extraKey.empty())
				localizedText << strExtraKey;

			const char* const localized = localizedText.toUTF8();
			if(localized)
				(*toolTipSink) += localized;
		}
	}
}

//	--------------------------------------------------------------------------------
/// Shortcut for generating production mod tool tip help
void CvGame::BuildCannotPerformActionHelpText(CvString* toolTipSink, const char* strTextKey, const char* strExtraKey1, const char* strExtraKey2, int iValue) const
{
	if(toolTipSink != NULL)
	{
		Localization::String localizedText = Localization::Lookup(strTextKey);

		if(iValue != -666)
			localizedText << iValue;

		if(strExtraKey1)
		{
			std::string extraKey1(strExtraKey1);
			if(!extraKey1.empty())
				localizedText << strExtraKey1;
		}

		if(strExtraKey2)
		{
			std::string extraKey2(strExtraKey2);
			if(!extraKey2.empty())
				localizedText << strExtraKey2;
		}

		const char* const localized = localizedText.toUTF8();
		if(localized)
			(*toolTipSink) += localized;
	}
}

//	--------------------------------------------------------------------------------
void CvGame::LogGameState(bool bLogHeaders)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutput;

		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strTemp;

		CvString strLogName = "WorldState_Log.csv";
		FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		AIGrandStrategyTypes eGrandStrategy;
		int iGSConquest = 0;
		int iGSSpaceship = 0;
		int iGSUN = 0;
		int iGSCulture = 0;

		int iAlly = 0;
		int iFriend = 0;
		int iFavorable = 0;
		int iNeutral = 0;
		int iCompetitor = 0;
		int iEnemy = 0;
		int iUnforgivable = 0;

		int iMajorWar = 0;
		int iMajorHostile = 0;
		int iMajorDeceptive = 0;
		int iMajorGuarded = 0;
		int iMajorAfraid = 0;
		int iMajorFriendly = 0;
		int iMajorNeutral = 0;

		int iMinorIgnore = 0;
		int iMinorFriendly = 0;
		int iMinorProtective = 0;
		int iMinorConquest = 0;
		int iMinorBully = 0;

		int iPlayerLoop2;
		PlayerTypes eLoopPlayer2;

		// Loop through all Players
		PlayerTypes eLoopPlayer;
		CvPlayer* pPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			pPlayer = &GET_PLAYER(eLoopPlayer);

			if(pPlayer->isAlive())
			{
				eGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();

				if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
				{
					iGSConquest++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
				{
					iGSSpaceship++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
				{
					iGSUN++;
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
				{
					iGSCulture++;
				}

				// Loop through all players
				for(iPlayerLoop2 = 0; iPlayerLoop2 < MAX_CIV_PLAYERS; iPlayerLoop2++)
				{
					eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;

					if(GET_PLAYER(eLoopPlayer2).isAlive())
					{
						// Major
						if(eLoopPlayer2 < MAX_MAJOR_CIVS)
						{
							switch(pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(eLoopPlayer2))
							{
							case MAJOR_CIV_OPINION_ALLY:
								iAlly++;
								break;
							case MAJOR_CIV_OPINION_FRIEND:
								iFriend++;
								break;
							case MAJOR_CIV_OPINION_FAVORABLE:
								iFavorable++;
								break;
							case MAJOR_CIV_OPINION_NEUTRAL:
								iNeutral++;
								break;
							case MAJOR_CIV_OPINION_COMPETITOR:
								iCompetitor++;
								break;
							case MAJOR_CIV_OPINION_ENEMY:
								iEnemy++;
								break;
							case MAJOR_CIV_OPINION_UNFORGIVABLE:
								iUnforgivable++;
								break;
							}

							switch(pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer2, false))
							{
							case MAJOR_CIV_APPROACH_WAR:
								iMajorWar++;
								break;
							case MAJOR_CIV_APPROACH_HOSTILE:
								iMajorHostile++;
								break;
							case MAJOR_CIV_APPROACH_DECEPTIVE:
								iMajorDeceptive++;
								break;
							case MAJOR_CIV_APPROACH_GUARDED:
								iMajorGuarded++;
								break;
							case MAJOR_CIV_APPROACH_AFRAID:
								iMajorAfraid++;
								break;
							case MAJOR_CIV_APPROACH_FRIENDLY:
								iMajorFriendly++;
								break;
							case MAJOR_CIV_APPROACH_NEUTRAL:
								iMajorNeutral++;
								break;
							}
						}
						// Minor
						else
						{
							switch(pPlayer->GetDiplomacyAI()->GetMinorCivApproach(eLoopPlayer2))
							{
							case MINOR_CIV_APPROACH_IGNORE:
								iMinorIgnore++;
								break;
							case MINOR_CIV_APPROACH_FRIENDLY:
								iMinorFriendly++;
								break;
							case MINOR_CIV_APPROACH_PROTECTIVE:
								iMinorProtective++;
								break;
							case MINOR_CIV_APPROACH_CONQUEST:
								iMinorConquest++;
								break;
							case MINOR_CIV_APPROACH_BULLY:
								iMinorBully++;
								break;
							}
						}
					}
				}
			}
		}

		bool bFirstTurn = bLogHeaders || getElapsedGameTurns() == 0;

		// Grand Strategies
		if(bFirstTurn)
		{
			strOutput = "Turn";
			strOutput += ", Conquest";
			strOutput += ", Spaceship";
			strOutput += ", Diplo";
			strOutput += ", Culture";
		}
		else
		{
			strOutput.Format("%03d", GC.getGame().getElapsedGameTurns());
			strTemp.Format("%d", iGSConquest);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSSpaceship);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSUN);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iGSCulture);
			strOutput += ", " + strTemp;
		}

		// Major Approaches
		if(bFirstTurn)
		{
			strOutput += ", Ally";
			strOutput += ", Friend";
			strOutput += ", Favorable";
			strOutput += ", Neutral";
			strOutput += ", Competitor";
			strOutput += ", Enemy";
			strOutput += ", Unforgivable";
		}
		else
		{
			strTemp.Format("%d", iAlly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iFriend);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iFavorable);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iNeutral);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iCompetitor);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iEnemy);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iUnforgivable);
			strOutput += ", " + strTemp;
		}

		// Major Approaches
		if(bFirstTurn)
		{
			strOutput += ", War";
			strOutput += ", Hostile";
			strOutput += ", Deceptive";
			strOutput += ", Guarded";
			strOutput += ", Afraid";
			strOutput += ", Friendly";
			strOutput += ", Neutral";
		}
		else
		{
			strTemp.Format("%d", iMajorWar);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorHostile);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorDeceptive);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorGuarded);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorAfraid);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorFriendly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMajorNeutral);
			strOutput += ", " + strTemp;
		}

		// Minor Approaches
		if(bFirstTurn)
		{
			strOutput += ", Ignore";
			strOutput += ", Friendly";
			strOutput += ", Protective";
			strOutput += ", Conquest";
			strOutput += ", Bully";
		}
		else
		{
			strTemp.Format("%d", iMinorIgnore);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorFriendly);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorProtective);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorConquest);
			strOutput += ", " + strTemp;
			strTemp.Format("%d", iMinorBully);
			strOutput += ", " + strTemp;
		}

#if defined(MOD_BALANCE_CORE_HAPPINESS)
		if(MOD_BALANCE_CORE_HAPPINESS)
		{
			if(bFirstTurn)
			{
				strOutput += ", CultureAvg";
				strOutput += ", ScienceAvg";
				strOutput += ", DefenseAvg";
				strOutput += ", GoldAvg";
			}
			else
			{
				strTemp.Format("%d", GC.getGame().GetCultureAverage());
				strOutput += ", " + strTemp;
				strTemp.Format("%d", GC.getGame().GetScienceAverage());
				strOutput += ", " + strTemp;
				strTemp.Format("%d", GC.getGame().GetDefenseAverage());
				strOutput += ", " + strTemp;
				strTemp.Format("%d", GC.getGame().GetGoldAverage());
				strOutput += ", " + strTemp;
			}
		}
#endif

		pLog->Msg(strOutput);
	}
}

//	------------------------------------------------------------------------------------------------
void CvGame::unitIsMoving()
{
	s_unitMoveTurnSlice = getTurnSlice();
}

//	------------------------------------------------------------------------------------------------
bool CvGame::allUnitAIProcessed() const
{
	int i = 0;
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		const CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));
		if(player.isTurnActive() && player.hasUnitsThatNeedAIUpdate())
			return false;
	}
	return true;
}

//	--------------------------------------------------------------------------------
/// How long are deals in this game (based on game speed)
int CvGame::GetDealDuration()
{
	return getGameSpeedInfo().GetDealDuration();
}

int CvGame::GetRelationshipDuration()
{
	return getGameSpeedInfo().getRelationshipDuration();
}

//	--------------------------------------------------------------------------------
int CvGame::GetPeaceDuration()
{
	return getGameSpeedInfo().getPeaceDealDuration();
}

//	--------------------------------------------------------------------------------
bool CvGame::IsArchaeologyTriggered() const
{
	return m_bArchaeologyTriggered;
}

//	--------------------------------------------------------------------------------
void CvGame::TriggerArchaeologySiteCreation(bool bCheckInitialized)
{
	if (!m_bArchaeologyTriggered)
	{
		if (!bCheckInitialized || isFinalInitialized())
		{
			SpawnArchaeologySitesHistorically();
			m_bArchaeologyTriggered = true;
		}
	}
}


//	--------------------------------------------------------------------------------
int CalculateDigSiteWeight(int iIndex, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& inputData, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& chosenDigSites)
{
	CvMap& theMap = GC.getMap();
	int iGridWidth = theMap.getGridWidth();
	int iBaseWeight = 0;
	if (chosenDigSites[iIndex].m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS) // if we have not already chosen this spot for a dig site
	{
		iBaseWeight = inputData[iIndex].m_eArtifactType + 1;
		iBaseWeight *= (10 - inputData[iIndex].m_eEra);

		int iPlotX = iIndex % iGridWidth;
		int iPlotY = iIndex / iGridWidth;

		CvPlot* pPlot = theMap.plotByIndexUnchecked(iIndex);

		// zero this value if this plot has a resource, water, ice, mountain, or natural wonder
		if (pPlot->getResourceType() != NO_RESOURCE || pPlot->isWater() || !pPlot->isValidMovePlot(NO_PLAYER) || pPlot->IsNaturalWonder())
			iBaseWeight = 0;

		// if this tile cannot be improved, zero it out
		if (iBaseWeight && pPlot->getFeatureType() != NO_FEATURE)
		{
			if (GC.getFeatureInfo(pPlot->getFeatureType())->isNoImprovement())
			{
				iBaseWeight = 0;
			}
		}

		// if this tile has a GP improvement, zero it out
		if (iBaseWeight && pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType())->IsCreatedByGreatPerson())
			{
				iBaseWeight = 0;
			}
		}

		if (iBaseWeight > 0)
		{
			// add a small random factor
			iBaseWeight += 10 + GC.getGame().getSmallFakeRandNum(10, iBaseWeight);

			// increase the value if unowned
			iBaseWeight *= (pPlot->getOwner() == NO_PLAYER) ? 9 : 8;
			iBaseWeight /= 8;

			// lower the value if owned by a major
			iBaseWeight *= (pPlot->getOwner() > NO_PLAYER && pPlot->getOwner() < MAX_MAJOR_CIVS) ? 11 : 12;
			iBaseWeight /= 12;

			// lower the value if tile has been improved
			iBaseWeight *= (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getRouteType() != NO_ROUTE) ? 7 : 8;
			iBaseWeight /= 8;

			// lower the value if tile has a city
			iBaseWeight *= (pPlot->isCity()) ? 1 : 5;
			iBaseWeight /= 5;

			// increase the value if in thematic terrain (desert, jungle, or small island)
			iBaseWeight *= (pPlot->getTerrainType() == TERRAIN_DESERT) ? 3 : 2;
			iBaseWeight *= (pPlot->getFeatureType() == FEATURE_JUNGLE) ? 3 : 2;
			CvArea* pArea = theMap.getArea(pPlot->getArea());
			iBaseWeight *= (pArea->getNumTiles() <= 4) ? 3 : 2;

			// lower the value by number of neighbors
			int iDivisor = 1;
			// lower the value if there is at least one nearby site (say, 3 tiles distance)
			int iRange = 3;
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
						}
					}
				}
			}
			iRange = 2;
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
						}
					}
				}
			}
			iRange = 1;
			for (int iDX = -iRange; iDX <= iRange; iDX++)
			{
				for (int iDY = -iRange; iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
					if (pLoopPlot)
					{
						if (chosenDigSites[pLoopPlot->GetPlotIndex()].m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
						{
							iDivisor++;
						}
					}
				}
			}
			iBaseWeight /= iDivisor;
		}
	}
	return iBaseWeight;
}


//	--------------------------------------------------------------------------------
void CalculateDigSiteWeights(int iGridSize, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& inputData, FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0>& chosenDigSites, FFastVector<int, true, c_eCiv5GameplayDLL, 0>& currentWeights)
{
	CvAssertMsg(NO_GREAT_WORK_ARTIFACT_CLASS == 0, "Value of NO_ARTIFACT has changed");
	for (int i = 0; i < iGridSize; i++)
	{
		currentWeights[i] = CalculateDigSiteWeight(i, inputData, chosenDigSites);
	}
}

//	--------------------------------------------------------------------------------
int CvGame::GetNumArchaeologySites() const
{
	if (!IsArchaeologyTriggered())
	{
		return -1;
	}

	int iRtnValue = 0;
	int iPlotLoop;
	CvPlot *pPlot;
	for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->getResourceType() == GC.getARTIFACT_RESOURCE())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}
//	--------------------------------------------------------------------------------
int CvGame::GetNumHiddenArchaeologySites() const
{
	if (!IsArchaeologyTriggered())
	{
		return -1;
	}

	int iRtnValue = 0;
	int iPlotLoop;
	CvPlot *pPlot;
	for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->getResourceType() == GC.getHIDDEN_ARTIFACT_RESOURCE())
		{
			iRtnValue++;
		}
	}
	return iRtnValue;
}

//	--------------------------------------------------------------------------------
PlayerTypes GetRandomMajorPlayer(CvPlot* pPlot)
{
	std::vector<PlayerTypes>tempPlayers;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;

		if (ePlayer == NO_PLAYER)
			continue;

		if (!GET_PLAYER(ePlayer).isEverAlive())
			continue;

		tempPlayers.push_back(ePlayer);
	}


	int iValue = GC.getGame().getSmallFakeRandNum(tempPlayers.size(), *pPlot);

	PlayerTypes ePlayer = (PlayerTypes)tempPlayers[iValue];

	if (ePlayer == NO_PLAYER)
		return BARBARIAN_PLAYER;

	return ePlayer;
}


//	--------------------------------------------------------------------------------
PlayerTypes GetRandomPlayer(CvPlot* pPlot)
{
	std::vector<PlayerTypes>tempPlayers;
	for (int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;

		if (ePlayer == NO_PLAYER)
			continue;

		if (!GET_PLAYER(ePlayer).isEverAlive())
			continue;

		tempPlayers.push_back(ePlayer);
	}


	int iValue = GC.getGame().getSmallFakeRandNum(tempPlayers.size(), *pPlot);

	PlayerTypes ePlayer = (PlayerTypes)tempPlayers[iValue];

	if (ePlayer == NO_PLAYER)
		return BARBARIAN_PLAYER;

	return ePlayer;
}


void CvGame::PopulateDigSite(CvPlot& kPlot, EraTypes eEra, GreatWorkArtifactClass eArtifact)
{
	CvMap& theMap = GC.getMap();
	CvArchaeologyData digSite;

	const int iPlotX = kPlot.getX();
	const int iPlotY = kPlot.getY();

	eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);
	digSite.m_eArtifactType = eArtifact;
	digSite.m_eEra = eEra;

	// find nearest city (preferably on same area)
	CvCity* pNearestCity = theMap.findCity(iPlotX, iPlotY, NO_PLAYER, NO_TEAM, true /* bSameArea */);
	pNearestCity = pNearestCity ? pNearestCity : theMap.findCity(iPlotX, iPlotY, NO_PLAYER, NO_TEAM, false /* bSameArea */); // expand search if we need to
	if (pNearestCity)
	{
		digSite.m_ePlayer1 = pNearestCity->getOriginalOwner();
	}
	else //  we can't find a nearby city (likely a late era start)
	{
		// look for nearby units
		CvUnit* pUnit = theMap.findUnit(iPlotX, iPlotY);
		if (pUnit)
		{
			digSite.m_ePlayer1 = pUnit->GetOriginalOwner();
		}
		else
		{
			// look for the start location if it exists
			PlayerTypes thisPlayer;
			if (theMap.findNearestStartPlot(iPlotX, iPlotY, thisPlayer))
			{
				digSite.m_ePlayer1 = thisPlayer;
			}
			else // just make something up
			{
				PlayerTypes ePlayer2 = GetRandomMajorPlayer(&kPlot);
				digSite.m_ePlayer1 = ePlayer2 == NO_PLAYER ? BARBARIAN_PLAYER : ePlayer2;
			}
		}
	}

	if (eArtifact == CvTypes::getARTIFACT_BATTLE_MELEE() || eArtifact == CvTypes::getARTIFACT_BATTLE_RANGED() || eArtifact == CvTypes::getARTIFACT_RAZED_CITY())
	{
		PlayerTypes ePlayer2 = GetRandomPlayer(&kPlot);
		digSite.m_ePlayer2 = ePlayer2 == NULL ? BARBARIAN_PLAYER : ePlayer2;
	}

	kPlot.AddArchaeologicalRecord(digSite.m_eArtifactType, digSite.m_eEra, digSite.m_ePlayer1, digSite.m_ePlayer2);
}
//	--------------------------------------------------------------------------------
void CvGame::SpawnArchaeologySitesHistorically()
{
	CvMap& theMap = GC.getMap();
	const int iGridWidth = theMap.getGridWidth();

	// we should now have a map of the dig sites
	// turn this map into set of RESOURCE_ARTIFACTS
	const ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
	const ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());

	const size_t aRandomArtifactsCount = 7;
	GreatWorkArtifactClass aRandomArtifacts[aRandomArtifactsCount] = { 
		CvTypes::getARTIFACT_ANCIENT_RUIN(), 
		CvTypes::getARTIFACT_ANCIENT_RUIN(), 
		CvTypes::getARTIFACT_RAZED_CITY(), 
		CvTypes::getARTIFACT_BARBARIAN_CAMP(), 
		CvTypes::getARTIFACT_BARBARIAN_CAMP(), 
		CvTypes::getARTIFACT_BATTLE_MELEE(), 
		CvTypes::getARTIFACT_BATTLE_RANGED() 
	};

	// find how many dig sites we need to create
	const int iNumMajorCivs = countMajorCivsEverAlive();
	const int iMinDigSites = GC.getMIN_DIG_SITES_PER_MAJOR_CIV() * iNumMajorCivs; //todo: parameterize this
	const int iMaxDigSites = GC.getMAX_DIG_SITES_PER_MAJOR_CIV() * iNumMajorCivs; //todo: parameterize this
	const int iIdealNumDigSites = iMinDigSites + getJonRandNum(iMaxDigSites - iMinDigSites, "dig sites");

	// find the highest era any player has gotten to
	EraTypes eHighestEra = NO_ERA;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Player not ever alive
		if(!GET_PLAYER(eLoopPlayer).isEverAlive())
			continue;

		if (GET_PLAYER(eLoopPlayer).GetCurrentEra() > eHighestEra)
		{
			eHighestEra = GET_PLAYER(eLoopPlayer).GetCurrentEra();
		}
	}

	CvWeightedVector<int, 64, true> eEraWeights;
	eEraWeights.clear();
	int iMaxEraWeight = 0;
	for (int i=0; i < static_cast<int>(eHighestEra); i++)
	{
		int iWeight = static_cast<int>(eHighestEra) - i;
		eEraWeights.push_back(i,iWeight);
		iMaxEraWeight += iWeight;
	}

	RandomNumberDelegate fcn;
	fcn = MakeDelegate(this, &CvGame::getJonRandNum);

	// find out how many dig sites we have now
	int iHowManyChosenDigSites = 0;

	// fill the historical buffer with the archaeological data
	FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0> historicalDigSites;
	FFastVector<CvArchaeologyData, true, c_eCiv5GameplayDLL, 0> scratchDigSites;
	int iGridSize = theMap.numPlots();
	CvAssertMsg(iGridSize > 0, "iGridSize is zero");
	historicalDigSites.resize(iGridSize);
	scratchDigSites.resize(iGridSize);
	for (int i = 0; i < iGridSize; i++)
	{
		scratchDigSites[i].m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
		scratchDigSites[i].m_eEra = NO_ERA;
		scratchDigSites[i].m_ePlayer1 = NO_PLAYER;
		scratchDigSites[i].m_ePlayer2 = NO_PLAYER;

		CvPlot* pPlot = theMap.plotByIndexUnchecked(i);
		const ResourceTypes eResource = pPlot->getResourceType();
		if (pPlot->isWater() || !pPlot->isValidMovePlot(BARBARIAN_PLAYER))
		{
			historicalDigSites[i].m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
			historicalDigSites[i].m_eEra = NO_ERA;
			historicalDigSites[i].m_ePlayer1 = NO_PLAYER;
			historicalDigSites[i].m_ePlayer2 = NO_PLAYER;

			//Cannot be an antiquity site if we cannot generate an artifact.
			if(eResource == eArtifactResourceType || eResource == eHiddenArtifactResourceType)
			{
				pPlot->setResourceType(NO_RESOURCE, 0, true);
			}
		}
		else
		{
			//If this plot is already marked as an antiquity site, ensure it's populated.
			if(eResource == eArtifactResourceType || eResource == eHiddenArtifactResourceType)
			{
				if(pPlot->GetArchaeologicalRecord().m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
				{
					// pick an era before this one			
					EraTypes eEra = static_cast<EraTypes>(eEraWeights.ChooseByWeight(&fcn, "Choosing an era by weight"));
					eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);

					// pick a type of artifact
					GreatWorkArtifactClass eArtifact = aRandomArtifacts[getSmallFakeRandNum(aRandomArtifactsCount, *pPlot)];

					PopulateDigSite(*pPlot, eEra, eArtifact);

					//Record in scratch space for weights.
					scratchDigSites[i] = pPlot->GetArchaeologicalRecord();
				}

				iHowManyChosenDigSites++;
			}

			historicalDigSites[i] = pPlot->GetArchaeologicalRecord();
		}
	}

	// calculate initial weights
	FFastVector<int, true, c_eCiv5GameplayDLL, 0> digSiteWeights;
	digSiteWeights.resize(iGridSize);
	for (int i = 0; i < iGridSize; i++)
	{
		digSiteWeights[i] = 0;
	}
	CalculateDigSiteWeights(iGridSize, historicalDigSites, scratchDigSites, digSiteWeights);

	// build a weight vector
	static CvWeightedVector<int, 128*80, true> aDigSiteWeights; // size of a HUGE world
	aDigSiteWeights.resize(iGridSize);

	vector<GreatWorkType> aWorksWriting;
	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		Database::Results kQuery;
		if(db->Execute(kQuery, "SELECT ID from GreatWorks WHERE ArchaeologyOnly = '1'"))
		{
			while(kQuery.Step())
			{
				const GreatWorkType eWork = static_cast<GreatWorkType>(kQuery.GetInt(0));
				aWorksWriting.push_back(eWork);
			}
		}
	}

	int iApproxNumHiddenSites = iIdealNumDigSites * GC.getPERCENT_SITES_HIDDEN() / 100;
	int iNumDesiredWritingSites = iApproxNumHiddenSites * GC.getPERCENT_HIDDEN_SITES_WRITING() / 100;
	int iNumWritingSites = min((int)aWorksWriting.size(), iNumDesiredWritingSites);

	// while we are not in the proper range of number of dig sites
	while (iHowManyChosenDigSites < iIdealNumDigSites)
	{
		// populate a weight vector
		aDigSiteWeights.clear();
		for (int i = 0; i < iGridSize; i++)
		{
			if (digSiteWeights[i] > 0)
			{
				aDigSiteWeights.push_back(i, digSiteWeights[i]);
			}
		}

		// sort the weight vector
		aDigSiteWeights.SortItems();

		// add the best dig site
		int iBestSite = aDigSiteWeights.GetElement(0);
		CvPlot* pPlot = theMap.plotByIndexUnchecked(iBestSite);

		// Hidden site?
		bool bHiddenSite = GC.getGame().getSmallFakeRandNum(100, *pPlot)  < GC.getPERCENT_SITES_HIDDEN();
		if (bHiddenSite)
		{
			pPlot->setResourceType(eHiddenArtifactResourceType, 1);
		}
		else
		{
			pPlot->setResourceType(eArtifactResourceType, 1);
		}

		// if this is not a historical dig site
		if (scratchDigSites[iBestSite].m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
		{
			// fake the historical data
			// pick an era before this one			
			EraTypes eEra = static_cast<EraTypes>(eEraWeights.ChooseByWeight(&fcn, "Choosing an era by weight"));
			eEra = eEra > static_cast<EraTypes>(0) ? eEra : static_cast<EraTypes>(0);

			// pick a type of artifact
			GreatWorkArtifactClass eArtifact;
			eArtifact = aRandomArtifacts[getSmallFakeRandNum(aRandomArtifactsCount, *pPlot)];

			PopulateDigSite(*pPlot, eEra, eArtifact);
		}

		// If this is a hidden slot getting a writing, override a few things
		if (bHiddenSite && iNumWritingSites > 0)
		{
			// First change the type
			pPlot->SetArtifactType(CvTypes::getARTIFACT_WRITING());

			// Then get a writing and set it
			int iIndex = getSmallFakeRandNum(aWorksWriting.size(), aWorksWriting.size()+pPlot->GetPlotIndex());
			GreatWorkType eWrittenGreatWork = aWorksWriting[iIndex];
			pPlot->SetArtifactGreatWork((GreatWorkType)eWrittenGreatWork);

			// Erase that writing from future consideration
			vector<GreatWorkType>::const_iterator it = std::find (aWorksWriting.begin(), aWorksWriting.end(), eWrittenGreatWork);
			aWorksWriting.erase(it);

			// One less writing to give out
			iNumWritingSites--;
		}

		scratchDigSites[iBestSite] = pPlot->GetArchaeologicalRecord();

		iHowManyChosenDigSites++;

		// recalculate weights near the chosen dig site (the rest of the world should still be fine)
		const int iRange = 3;
		int iPlotX = iBestSite % iGridWidth;
		int iPlotY = iBestSite / iGridWidth;
		for (int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iDX, iDY, iRange);
				if (pLoopPlot)
				{
					int iIndex = pLoopPlot->GetPlotIndex();
					digSiteWeights[iIndex] = CalculateDigSiteWeight(iIndex, historicalDigSites, scratchDigSites);				
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
CombatPredictionTypes CvGame::GetCombatPrediction(const CvUnit* pAttackingUnit, const CvUnit* pDefendingUnit)
{
	if(!pAttackingUnit || !pDefendingUnit)
	{
		return NO_COMBAT_PREDICTION;
	}

	CombatPredictionTypes ePrediction = NO_COMBAT_PREDICTION;

	if(pAttackingUnit->isRanged())
	{
		return COMBAT_PREDICTION_RANGED;
	}

	CvPlot* pFromPlot = pAttackingUnit->plot();
	CvPlot* pToPlot = pDefendingUnit->plot();

	int iAttackingStrength = pAttackingUnit->GetMaxAttackStrength(pFromPlot, pToPlot, pDefendingUnit);
	if(iAttackingStrength == 0)
	{
		return NO_COMBAT_PREDICTION;
	}

	int iDefenderStrength = pDefendingUnit->GetMaxDefenseStrength(pToPlot, pAttackingUnit, pFromPlot, false);

	//iMyDamageInflicted = pMyUnit:GetCombatDamage(iMyStrength, iTheirStrength, pMyUnit:GetDamage() + iTheirFireSupportCombatDamage, false, false, false);
	int iAttackingDamageInflicted = pAttackingUnit->getCombatDamage(iAttackingStrength, iDefenderStrength, false, false, false);
	int iDefenderDamageInflicted  = pDefendingUnit->getCombatDamage(iDefenderStrength, iAttackingStrength, false, false, false);
	//iTheirDamageInflicted = iTheirDamageInflicted + iTheirFireSupportCombatDamage;

#if defined(MOD_UNITS_MAX_HP)
	int iAttackerMaxHitPoints = pAttackingUnit->GetMaxHitPoints();
	int iDefenderMaxHitPoints = pDefendingUnit->GetMaxHitPoints();

	if (iAttackingDamageInflicted > iDefenderMaxHitPoints)
	{
		iAttackingDamageInflicted = iDefenderMaxHitPoints;
	}
	if (iDefenderDamageInflicted > iAttackerMaxHitPoints)
	{
		iDefenderDamageInflicted = iAttackerMaxHitPoints;
	}

	bool bAttackerDies = (pAttackingUnit->getDamage() + iDefenderDamageInflicted >= iAttackerMaxHitPoints);
	bool bDefenderDies = (pDefendingUnit->getDamage() + iAttackingDamageInflicted >= iDefenderMaxHitPoints);
#else
	int iMaxUnitHitPoints = GC.getMAX_HIT_POINTS();
	if (iAttackingDamageInflicted > iMaxUnitHitPoints)
	{
		iAttackingDamageInflicted = iMaxUnitHitPoints;
	}
	if (iDefenderDamageInflicted > iMaxUnitHitPoints)
	{
		iDefenderDamageInflicted = iMaxUnitHitPoints;
	}

	bool bAttackerDies = false;
	bool bDefenderDies = false;

	if (pAttackingUnit->getDamage() + iDefenderDamageInflicted >= iMaxUnitHitPoints)
	{
		bAttackerDies = true;
	}

	if (pDefendingUnit->getDamage() + iAttackingDamageInflicted >= iMaxUnitHitPoints)
	{
		bDefenderDies = true;
	}
#endif

	if(bAttackerDies && bDefenderDies)
	{
		ePrediction = COMBAT_PREDICTION_STALEMATE;
	}
	else if(bAttackerDies)
	{
		ePrediction = COMBAT_PREDICTION_TOTAL_DEFEAT;
	}
	else if(bDefenderDies)
	{
		ePrediction = COMBAT_PREDICTION_TOTAL_VICTORY;
	}
	else if(iAttackingDamageInflicted - iDefenderDamageInflicted > 30)
	{
		ePrediction = COMBAT_PREDICTION_MAJOR_VICTORY;
	}
	else if(iAttackingDamageInflicted > iDefenderDamageInflicted)
	{
		ePrediction = COMBAT_PREDICTION_SMALL_VICTORY;
	}
	else if(iDefenderDamageInflicted - iAttackingDamageInflicted > 30)
	{
		ePrediction = COMBAT_PREDICTION_MAJOR_DEFEAT;
	}
	else if(iAttackingDamageInflicted < iDefenderDamageInflicted)
	{
		ePrediction = COMBAT_PREDICTION_SMALL_DEFEAT;
	}
	else
	{
		ePrediction = COMBAT_PREDICTION_STALEMATE;
	}

	return ePrediction;
}

void CvGame::SetClosestCityMapDirty()
{
	m_cityDistanceTurns.SetDirty();
	m_cityDistancePlots.SetDirty();

	//debugging
	if (false)
	{
		CvString fname = CvString::format("CityDistance%03d.txt",getGameTurn());
		FILogFile* pLog = LOGFILEMGR.GetLog(fname.c_str(), FILogFile::kDontTimeStamp);
		if (pLog)
		{
			pLog->Msg("#x,y,water,plot dist,plot city,plot owner,turn dist,turn city,turn owner\n");
			for (int i = 0; i < GC.getMap().numPlots(); i++)
			{
				CvPlot* pPlot = GC.getMap().plotByIndex(i);
				int iDP = m_cityDistancePlots.GetDistance(*pPlot,false,NO_PLAYER);
				int iCP = m_cityDistancePlots.GetFeatureId(*pPlot,false,NO_PLAYER);
				int iOP = m_cityDistancePlots.GetFeatureOwner(*pPlot,false,NO_PLAYER);
				int iDT = m_cityDistanceTurns.GetDistance(*pPlot,false,NO_PLAYER);
				int iCT = m_cityDistanceTurns.GetFeatureId(*pPlot,false,NO_PLAYER);
				int iOT = m_cityDistanceTurns.GetFeatureOwner(*pPlot,false,NO_PLAYER);

				CvString dump = CvString::format("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					pPlot->getX(), pPlot->getY(), pPlot->isWater() ? 1 : 0, iDP, iCP, iOP, iDT, iCT, iOT);

				pLog->Msg(dump.c_str());
			}
		}
	}
}

int CvGame::GetClosestCityDistanceInTurns( const CvPlot* pPlot, PlayerTypes ePlayer )
{
	if (!pPlot)
		return INT_MAX;

	return m_cityDistanceTurns.GetDistance(*pPlot, false, ePlayer);
}

CvCity* CvGame::GetClosestCityByEstimatedTurns( const CvPlot* pPlot, PlayerTypes ePlayer )
{
	if (!pPlot)
		return NULL;

	int owner = m_cityDistanceTurns.GetFeatureOwner(*pPlot, false, ePlayer);
	int id = m_cityDistanceTurns.GetFeatureId(*pPlot, false, ePlayer);
	if (owner!=NO_PLAYER)
		return GET_PLAYER((PlayerTypes)owner).getCity(id);
	else
		return NULL;
}

int CvGame::GetClosestCityDistanceInTurns( const CvPlot* pPlot, bool bMajorOnly )
{
	if (!pPlot)
		return INT_MAX;

	return m_cityDistanceTurns.GetDistance(*pPlot, bMajorOnly, NO_PLAYER);
}

CvCity* CvGame::GetClosestCityByEstimatedTurns( const CvPlot* pPlot, bool bMajorOnly )
{
	if (!pPlot)
		return NULL;

	int owner = m_cityDistanceTurns.GetFeatureOwner(*pPlot, bMajorOnly, NO_PLAYER);
	int id = m_cityDistanceTurns.GetFeatureId(*pPlot, bMajorOnly, NO_PLAYER);
	if (owner!=NO_PLAYER)
		return GET_PLAYER((PlayerTypes)owner).getCity(id);
	else
		return NULL;
}

int CvGame::GetClosestCityDistanceInPlots( const CvPlot* pPlot, PlayerTypes ePlayer )
{
	if (!pPlot)
		return INT_MAX;

	return m_cityDistancePlots.GetDistance(*pPlot, false, ePlayer);
}

CvCity* CvGame::GetClosestCityByPlots( const CvPlot* pPlot, PlayerTypes ePlayer )
{
	if (!pPlot)
		return NULL;

	int owner = m_cityDistancePlots.GetFeatureOwner(*pPlot, false, ePlayer);
	int id = m_cityDistancePlots.GetFeatureId(*pPlot, false, ePlayer);
	if (owner!=NO_PLAYER)
		return GET_PLAYER((PlayerTypes)owner).getCity(id);
	else
		return NULL;
}

int CvGame::GetClosestCityDistanceInPlots( const CvPlot* pPlot, bool bMajorOnly )
{
	if (!pPlot)
		return INT_MAX;

	return m_cityDistancePlots.GetDistance(*pPlot, bMajorOnly, NO_PLAYER);
}

CvCity* CvGame::GetClosestCityByPlots( const CvPlot* pPlot, bool bMajorOnly )
{
	if (!pPlot)
		return NULL;

	int owner = m_cityDistancePlots.GetFeatureOwner(*pPlot, bMajorOnly, NO_PLAYER);
	int id = m_cityDistancePlots.GetFeatureId(*pPlot, bMajorOnly, NO_PLAYER);
	if (owner!=NO_PLAYER)
		return GET_PLAYER((PlayerTypes)owner).getCity(id);
	else
		return NULL;
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// Convert from city population to discrete size
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

//	--------------------------------------------------------------------------------
//Function to determine city size from city population
unsigned int CvGame::GetVariableCitySizeFromPopulation(unsigned int nPopulation)
{
	//fibonacci
	const unsigned int aiSizes[10] = { 1, 2, 3, 5, 8, 13, 21, 34, 55, UINT_MAX };
	for(unsigned int i = 0; i < 10; ++i)
	{
		if(nPopulation < aiSizes[i])
		{
			return i;
		}
	}
	return 4;
};

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

//	--------------------------------------------------------------------------------
void CvGame::NetMessageStaticsReset()
{//The net message system reset its static variables.  
}

//	--------------------------------------------------------------------------------
void CvGame::SetLastTurnAICivsProcessed()
{
	if(m_lastTurnAICivsProcessed != getGameTurn()){
		gDLL->SendAICivsProcessed();
		m_lastTurnAICivsProcessed = getGameTurn();
	}
}

#if defined(MOD_API_EXTENSIONS)
bool CvGame::AnyoneHasBelief(BeliefTypes iBeliefType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasBelief(iBeliefType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasBuilding(BuildingTypes iBuildingType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasBuilding(iBuildingType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasBuildingClass(BuildingClassTypes iBuildingClassType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasBuildingClass(iBuildingClassType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasAnyWonder() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasAnyWonder()) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasWonder(BuildingTypes iBuildingType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasWonder(iBuildingType)) {
			return true;
		}
	}

	return false;
}

int CvGame::GetCivilizationPlayer(CivilizationTypes iCivilizationType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.IsCivilization(iCivilizationType)) {
			return i;
		}
	}

	return NO_PLAYER;
}

bool CvGame::AnyoneIsInEra(EraTypes iEraType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.IsInEra(iEraType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasReachedEra(EraTypes iEraType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasReachedEra(iEraType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasAnyNaturalWonder() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasAnyNaturalWonder()) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasNaturalWonder(FeatureTypes iFeatureType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasNaturalWonder(iFeatureType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasPolicy(PolicyTypes iPolicyType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasPolicy(iPolicyType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasTenet(PolicyTypes iPolicyType) const
{
	return AnyoneHasPolicy(iPolicyType);
}

bool CvGame::AnyoneHasPolicyBranch(PolicyBranchTypes iPolicyBranchType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasPolicyBranch(iPolicyBranchType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasIdeology(PolicyBranchTypes iPolicyBranchType) const
{
	return AnyoneHasPolicyBranch(iPolicyBranchType);
}

bool CvGame::AnyoneHasProject(ProjectTypes iProjectType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasProject(iProjectType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasPantheon() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasPantheon()) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasAnyReligion() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasAnyReligion()) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasReligion(ReligionTypes iReligionType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasReligion(iReligionType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::IsResolutionPassed(ResolutionTypes iResolutionType, int iChoice) const
{
	CvGameLeagues* pGameLeagues = GC.getGame().GetGameLeagues();

	if (pGameLeagues->GetNumActiveLeagues() > 0) {
		CvLeague* pLeague = pGameLeagues->GetActiveLeague();
		if (pLeague) {
			return pLeague->IsActiveResolution(iResolutionType, iChoice);
		}
	}

	return false;
}

bool CvGame::AnyoneHasTech(TechTypes iTechType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasTech(iTechType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasUnit(UnitTypes iUnitType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasUnit(iUnitType)) {
			return true;
		}
	}

	return false;
}

bool CvGame::AnyoneHasUnitClass(UnitClassTypes iUnitClassType) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(i));

		if(player.isAlive() && player.HasUnitClass(iUnitClassType)) {
			return true;
		}
	}

	return false;
}
#endif
#if defined(MOD_BALANCE_CORE_JFD)	
void CvGame::SetContractUnits(ContractTypes eContract, UnitTypes eUnit, int iValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eContract > -1 && eContract < GC.getNumContractInfos(), "Invalid eContract index.");
	CvAssertMsg(eUnit > -1 && eUnit < GC.getNumUnitInfos(), "Invalid eUnit index.");

	if(m_ppaiContractUnits[eContract][eUnit] != iValue)
	{
		m_ppaiContractUnits[eContract][eUnit] = iValue;
	}
}
int CvGame::GetContractUnits(ContractTypes eContract, UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eContract > -1 && eContract < GC.getNumContractInfos(), "Invalid eContract index.");
	CvAssertMsg(eUnit > -1 && eUnit < GC.getNumUnitInfos(), "Invalid eUnit index.");

	return m_ppaiContractUnits[eContract][eUnit];
}
#endif
#if defined(MOD_BALANCE_CORE)

PlayerTypes CvGame::GetCorporationFounder(CorporationTypes eCorporation) const
{
	CvCorporation* pCorporation = m_pGameCorporations->GetCorporation(eCorporation);
	if (pCorporation == NULL)
		return NO_PLAYER;

	return pCorporation->m_eFounder;
}

int CvGame::GetNumCorporationsFounded() const
{
	return m_pGameCorporations->GetNumActiveCorporations();
}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
void CvGame::UpdateGreatestPlayerResourceMonopoly(ResourceTypes eTestResource)
{
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes)iResourceLoop;

		//got a specific one in mind?
		if (eTestResource != NO_RESOURCE && eResource != eTestResource)
			continue;

		const CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource == NULL)
			continue;
		if (!pkResource->isMonopoly())
			continue;

		m_aiGreatestMonopolyPlayer[eResource] = NO_PLAYER;

		PlayerTypes eGreatestMonopolyPlayer = NO_PLAYER;
		PlayerTypes eLoopPlayer;
		int iMax = 0;
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			eLoopPlayer = (PlayerTypes)iLoopPlayer;

			if (!GET_PLAYER(eLoopPlayer).isAlive()) continue;

			int iMonopolyPercent = GET_PLAYER(eLoopPlayer).GetMonopolyPercent(eResource);
			if (iMonopolyPercent > iMax)
			{
				iMax = iMonopolyPercent;
				eGreatestMonopolyPlayer = eLoopPlayer;
			}
		}
		m_aiGreatestMonopolyPlayer[eResource] = eGreatestMonopolyPlayer;
	}
}

int CvGame::GetGreatestPlayerResourceMonopoly(ResourceTypes eResource) const
{
	return m_aiGreatestMonopolyPlayer[eResource];
}

int CvGame::GetGreatestPlayerResourceMonopolyValue(ResourceTypes eResource) const
{
	PlayerTypes eGreatestPlayer = (PlayerTypes)m_aiGreatestMonopolyPlayer[eResource];
	if (eGreatestPlayer == NO_PLAYER)
		return 0;

	return GET_PLAYER(eGreatestPlayer).GetMonopolyPercent(eResource);
}
#endif

PlayerTypes CvGame::GetPotentialFreeCityPlayer(CvCity* pCity)
{
	if (pCity != NULL)
	{
		for (int i = MAX_MAJOR_CIVS; i < MAX_CIV_PLAYERS; i++)
		{
			PlayerTypes ePlayer = (PlayerTypes)i;

			if (GET_PLAYER(ePlayer).isAlive())
				continue;

			if (pCity->GetNumTimesOwned(ePlayer) <= 0)
				continue;

				return ePlayer;
		}
	}

	for(int i = MAX_MAJOR_CIVS; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;

		if (GET_PLAYER(ePlayer).isEverAlive() || GET_PLAYER(ePlayer).isObserver())
			continue;

		return ePlayer;
	}

	return NO_PLAYER;
}

TeamTypes CvGame::GetPotentialFreeCityTeam(CvCity* pCity)
{
	if (pCity != NULL)
	{
		for (int i = 0; i < MAX_TEAMS; i++)
		{
			TeamTypes eTeam = (TeamTypes)i;

			if (GET_TEAM(eTeam).isAlive())
				continue;

			const std::vector<PlayerTypes>& teammates = GET_TEAM(eTeam).getPlayers();
			for (size_t i = 0; i < teammates.size(); ++i)
			{
				CvPlayer& player = GET_PLAYER(teammates[i]);
				if (pCity->GetNumTimesOwned(player.GetID()) <= 0)
					continue;

				return eTeam;
			}
		}
	}
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		TeamTypes eTeam = (TeamTypes)i;

		if (GET_TEAM(eTeam).isEverAlive() || GET_TEAM(eTeam).isObserver())
			continue;

		return eTeam;
	}
	return NO_TEAM;
}

MinorCivTypes CvGame::GetAvailableMinorCivType()
{
	for (int i = 0; i < GC.getNumMinorCivInfos(); i++)
	{
		CvMinorCivInfo* pkCivilization = GC.getMinorCivInfo((MinorCivTypes)i);
		if (pkCivilization == NULL)
			continue;

		bool bad = false;
		for(int j = MAX_MAJOR_CIVS; j < MAX_CIV_PLAYERS; j++)
		{
			PlayerTypes eMinor = (PlayerTypes)j;
			if (CvPreGame::minorCivType(eMinor) == (MinorCivTypes)i)
			{
				bad = true;
				break;
			}
		}
		if (bad)
			continue;

		return (MinorCivTypes)i;
	}
	return NO_MINORCIV;
}

bool CvGame::CreateFreeCityPlayer(CvCity* pStartingCity, bool bJustChecking)
{
	if (pStartingCity == NULL)
		return false;

	PlayerTypes eNewPlayer = GetPotentialFreeCityPlayer(pStartingCity);
	TeamTypes eNewTeam = GetPotentialFreeCityTeam(pStartingCity);
	if (eNewPlayer == NO_PLAYER || eNewTeam == NO_TEAM)
		return false;

	const TeamTypes eTeam(static_cast<TeamTypes>(eNewTeam));
	CvTeam& kTeam = GET_TEAM(eTeam);

	MinorCivTypes eNewType = CvPreGame::minorCivType(eNewPlayer);

	if (eNewType == NO_MINORCIV)
		return false;

	CvMinorCivInfo* pMinorCivInfo = GC.getMinorCivInfo(eNewType);

	if (!pMinorCivInfo)
		return false;

	if (bJustChecking)
		return true;

	CvString strCityName = pStartingCity->getNameKey();

	CvPreGame::setSlotStatus(eNewPlayer, SS_COMPUTER);

	CvPlayerAI& kPlayer = GET_PLAYER(eNewPlayer);

	kTeam.init(eTeam);
	kPlayer.init(eNewPlayer);
	kPlayer.gameStartInit();
	kPlayer.setAlive(true, false);
	kTeam.updateTeamStatus();
	initDiplomacy();

	kPlayer.GetMinorCivAI()->DoPickInitialItems();
	
	// get the plot before transferring ownership
	CvPlot *pPlot = pStartingCity->plot();

	// setStartingPlot needs to be before acquireCity, otherwise if this is a maritime city
	// state, it will crash when the game tries to use the starting plot to look for the
	// closest city to grant the bonus food for the civ first meeting this city state, before
	// this function has finished executing.
	kPlayer.setStartingPlot(pPlot);
	CvCity* pNewCity = kPlayer.acquireCity(pStartingCity, false/*bConquest*/, true/*bGift*/);
	kPlayer.setFoundedFirstCity(true);
	kPlayer.setCapitalCity(pNewCity);
	pStartingCity = NULL; //no longer valid
	//we have to set this here!

	if (strCityName != "")
	{
		CvString localizedText = "TXT_KEY_FREE_CITY";
		pNewCity->setName(strCityName, false, true);
	}

	pNewCity->ChangeNumTimesOwned(eNewPlayer, 1);
	pNewCity->SetOccupied(false);

	if (!pNewCity->IsNoOccupiedUnhappiness())
		pNewCity->ChangeNoOccupiedUnhappinessCount(1);

	kPlayer.GetMinorCivAI()->SetTurnLiberated(getGameTurn());

	//update our techs!
	GET_TEAM(kPlayer.getTeam()).DoMinorCivTech();

	DoSpawnUnitsAroundTargetCity(eNewPlayer, pNewCity, GC.getGame().getCurrentEra() + 2, false, true, false, false);

	// Move Units from player that don't belong here
	if (pPlot->getNumUnits() > 0)
	{
		// Get the current list of units because we will possibly be moving them out of the plot's list
		IDInfoVector currentUnits;
		if (pPlot->getUnits(&currentUnits) > 0)
		{
			for (IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
			{
				CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

				if (pLoopUnit && pLoopUnit->getOwner() != kPlayer.GetID())
				{
					pLoopUnit->finishMoves();
					if (!pLoopUnit->jumpToNearestValidPlot())
						pLoopUnit->kill(false);
				}
			}
		}
	}
	return true;
}
#endif
#if defined(MOD_BUGFIX_AI_DOUBLE_TURN_MP_LOAD)
bool CvGame::isFirstActivationOfPlayersAfterLoad()
{
	return m_firstActivationOfPlayersAfterLoad;
}
#endif
