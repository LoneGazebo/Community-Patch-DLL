/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvWorldBuilderMapLoader.h"
#include "CvGameCoreUtils.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvTypes.h"

#define CV_WORLD_BUILDER_MAP_IN_GAME
#include "CvWorldBuilderMap.h"
#include "CvWorldBuilderMapTypeDesc.h"

#include "LintFree.h"

// I know it looks weird to have these as static non-members.
// The reason it's set up this was is to avoid an h file dependency
// on CvWorldBuilderMap.
static CvWorldBuilderMap sg_kSave;
static CvWorldBuilderMapLoaderMapInfo sg_kMapInfo;
static CvWorldBuilderMapTypeDesc sg_kMapTypeDesc;
static uint sg_auiPlayerSlots[MAX_CIV_PLAYERS];

void InitTypeDesc()
{
	//static bool bFirst = true;
	//if( bFirst ) // Do this every time as mod data may have changed.
	{
		sg_kMapTypeDesc.m_kTerrainTypes.Clear();
		const int iTerrainTypeCount = GC.getNumTerrainInfos();
		for(int i = 0; i < iTerrainTypeCount; ++i)
		{
			const CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo((TerrainTypes)i);
			if(pkTerrainInfo != NULL)
			{
				sg_kMapTypeDesc.m_kTerrainTypes.Add( pkTerrainInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kTerrainTypes.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kFeatures.Clear();
		const int iFeatureCount = GC.getNumFeatureInfos();
		for(int i = 0; i < iFeatureCount; ++i)
		{
			const CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo((FeatureTypes)i);
			if(pkFeatureInfo != NULL)
			{
				sg_kMapTypeDesc.m_kFeatures.Add( pkFeatureInfo->GetType() );
				sg_kMapTypeDesc.m_kNaturalWonders.Add( pkFeatureInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kFeatures.Add( NULL );
				sg_kMapTypeDesc.m_kNaturalWonders.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kResources.Clear();
		const int iResourceCount = GC.getNumResourceInfos();
		for(int i = 0; i < iResourceCount; ++i)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo((ResourceTypes)i);
			if(pkResourceInfo != NULL)
			{
				sg_kMapTypeDesc.m_kResources.Add( pkResourceInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kResources.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kImprovements.Clear();
		const int iImprovementCount = GC.getNumImprovementInfos();
		for(int i = 0; i < iImprovementCount; ++i)
		{
			const CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo((ImprovementTypes)i);
			if(pkImprovementInfo != NULL)
			{
				sg_kMapTypeDesc.m_kImprovements.Add( pkImprovementInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kImprovements.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kUnits.Clear();
		const int iUnitCount = GC.getNumUnitInfos();
		for(int i = 0; i < iUnitCount; ++i)
		{
			const CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)i);
			if(pkUnitInfo != NULL)
			{
				sg_kMapTypeDesc.m_kUnits.Add( pkUnitInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kUnits.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kTechs.Clear();
		const int iTechCount = GC.getNumTechInfos();
		for(int i = 0; i < iTechCount; ++i)
		{
			const CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes)i);
			if(pkTechInfo != NULL)
			{
				sg_kMapTypeDesc.m_kTechs.Add( pkTechInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kTechs.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kPolicies.Clear();
		const int iPolicyCount = GC.getNumPolicyInfos();
		for(int i = 0; i < iPolicyCount; ++i)
		{
			const CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo((PolicyTypes)i);
			if(pkPolicyInfo != NULL)
			{
				sg_kMapTypeDesc.m_kPolicies.Add( pkPolicyInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kPolicies.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kBuildingTypes.Clear();
		const int iBuildingCount = GC.getNumBuildingInfos();
		for(int i = 0; i < iBuildingCount; ++i)
		{
			const CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo((BuildingTypes)i);
			if(pkBuildingInfo != NULL)
			{
				sg_kMapTypeDesc.m_kBuildingTypes.Add( pkBuildingInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kBuildingTypes.Add( NULL );
			}
		}

		sg_kMapTypeDesc.m_kUnitPromotions.Clear();
		const int iPromotionCount = GC.getNumPromotionInfos();
		for(int i = 0; i < iPromotionCount; ++i)
		{
			const CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo((PromotionTypes)i);
			if(pkPromotionInfo != NULL)
			{
				sg_kMapTypeDesc.m_kUnitPromotions.Add( pkPromotionInfo->GetType() );
			}
			else
			{
				sg_kMapTypeDesc.m_kUnitPromotions.Add( NULL );
			}
		}

		//bFirst = false;
	}
}

const CvWorldBuilderMapLoaderMapInfo& CvWorldBuilderMapLoader::GetCurrentMapInfo()
{
	return sg_kMapInfo;
}

bool CvWorldBuilderMapLoader::Preload(const wchar_t* wszFilename, bool bScenario)
{
	InitTypeDesc();

	// Re-initialize map info
	new(&sg_kMapInfo)CvWorldBuilderMapLoaderMapInfo();

	if(sg_kSave.Load(wszFilename, sg_kMapTypeDesc))
	{
		const uint uiPlotCount = sg_kSave.GetPlotCount();

		if(!bScenario)
		{
			// Clear out all of the scenario data (except improvements)
			sg_kSave.ClearScenarioData(true);

			// Remove all improvements that aren't goodies
			for(uint i = 0; i < uiPlotCount; ++i)
			{
				CvWorldBuilderMap::PlotScenarioData& kPlot = sg_kSave.GetPlotScenarioData(i);
				if(kPlot.m_byImprovement != CvWorldBuilderMap::PlotScenarioData::InvalidImprovement)
				{
					ImprovementTypes eType = (ImprovementTypes)kPlot.m_byImprovement;
					const CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eType);
					if(pkImprovement == NULL || !pkImprovement->IsGoody())
						kPlot.m_byImprovement = CvWorldBuilderMap::PlotScenarioData::InvalidImprovement;
				}
			}
		}

		sg_kMapInfo.uiWidth = sg_kSave.GetWidth();
		sg_kMapInfo.uiHeight = sg_kSave.GetHeight();

		sg_kMapInfo.uiPlayers = sg_kSave.GetPlayerCount();
		sg_kMapInfo.uiCityStates = sg_kSave.GetCityStateCount();

		sg_kMapInfo.uiMajorCivStartingPositions = 0;
		sg_kMapInfo.uiMinorCivStartingPositions = 0;

		sg_kMapInfo.bWorldWrap = sg_kSave.GetFlag(CvWorldBuilderMap::FLAG_WORLD_WRAP);
		sg_kMapInfo.bRandomResources = sg_kSave.GetFlag(CvWorldBuilderMap::FLAG_RANDOM_RECOURCES);

		for(uint i = 0; i < uiPlotCount; ++i)
		{
			const CvWorldBuilderMap::PlotMapData& kPlotData = sg_kSave.GetPlotData(i);
			if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::START_POS_MAJOR))
				sg_kMapInfo.uiMajorCivStartingPositions++;
			else if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::START_POS_MINOR))
				sg_kMapInfo.uiMinorCivStartingPositions++;
		}

		const uint uiPlayerCount = sg_kSave.GetPlayerCount();
		if(uiPlayerCount > 0)
		{
			for(uint i = 0; i < uiPlayerCount; ++i)
			{
				const uint uiNew = sg_auiPlayerSlots[i];
				if(uiNew != i)
				{
					if(uiNew < uiPlayerCount)
					{
						sg_kSave.SwapPlayers(uiNew, i);

						for(uint j = i + 1; j < uiPlayerCount; ++j)
						{
							if(sg_auiPlayerSlots[j] == i)
							{
								sg_auiPlayerSlots[j] = uiNew;
								break;
							}
						}
					}
				}
			}
		}

		//ResetPlayerSlots();

		return true;
	}
	else
	{
		return false;
	}
}

void CvWorldBuilderMapLoader::SetupGameOptions()
{
	const int iGameOptionCount = GC.getNumGameOptionInfos();
	for(int i = 0; i < iGameOptionCount; ++i)
	{
		const GameOptionTypes eOption = (GameOptionTypes)i;
		const CvGameOptionInfo* pkOption = GC.getGameOptionInfo(eOption);
		if(pkOption != NULL)
		{
			const char* szOption = pkOption->GetType();
			const bool bEnabled = sg_kSave.m_kGameOptions.Contains(szOption);
			GC.getGame().setOption(szOption, bEnabled);
		}
	}

	const int iVictoryTypeCount = GC.getNumVictoryInfos();
	for(int i = 0; i < iVictoryTypeCount; ++i)
	{
		const VictoryTypes eVictoryType = (VictoryTypes)i;
		const CvVictoryInfo* pkVictoryType = GC.getVictoryInfo(eVictoryType);
		if(pkVictoryType != NULL)
		{
			const char* szVictoryType = pkVictoryType->GetType();
			const bool bValid = sg_kSave.m_kVictoryTypes.Contains(szVictoryType);
			GC.getGame().setVictoryValid(eVictoryType, bValid);
		}
	}

	CvPreGame::setMaxTurns(sg_kSave.GetMaxTurns());

	GC.getGame().setStartYear(sg_kSave.GetStartYear());
	CvPreGame::setGameSpeed(sg_kSave.GetDefaultGameSpeed());
}

void CvWorldBuilderMapLoader::SetupPlayers()
{
	const uint uiPlayerCount = std::min(sg_kSave.GetPlayerCount(), (byte)MAX_CIV_PLAYERS);
	for(uint i = 0; i < uiPlayerCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)i;
		const SlotStatus eStatus = CvPreGame::slotStatus(ePlayer);
		if(eStatus != SS_TAKEN && eStatus != SS_COMPUTER)
			CvPreGame::setSlotStatus(ePlayer, SS_COMPUTER);

		const CvWorldBuilderMap::Player& kPlayer = sg_kSave.GetPlayer(i);

		CivilizationTypes eCivType = NO_CIVILIZATION;
		for(int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
		{
			CvCivilizationInfo* pkCivilization = GC.getCivilizationInfo((CivilizationTypes)iCiv);
			if(pkCivilization != NULL && strcmp(kPlayer.m_szCivType, pkCivilization->GetType()) == 0)
			{
				eCivType = (CivilizationTypes)iCiv;
				break;
			}
		}

		CvPreGame::setCivilization(ePlayer, eCivType);

		PlayerColorTypes eColorType = NO_PLAYERCOLOR;
		for(int iColor = 0; iColor < GC.GetNumPlayerColorInfos(); ++iColor)
		{
			CvPlayerColorInfo* pkColor = GC.GetPlayerColorInfo((PlayerColorTypes)iColor);
			if(pkColor != NULL && strcmp(kPlayer.m_szTeamColor, pkColor->GetType()) == 0)
			{
				eColorType = (PlayerColorTypes)iColor;
				break;
			}
		}

		CvPreGame::setPlayerColor(ePlayer, eColorType);

		if(!(eStatus == SS_TAKEN && CvPreGame::overrideScenarioHandicap()))
		{
			HandicapTypes eHandicap = NO_HANDICAP;
			for(int iHandicap = 0; iHandicap < GC.getNumHandicapInfos(); ++iHandicap)
			{
				CvHandicapInfo* pkHandicap = GC.getHandicapInfo((HandicapTypes)iHandicap);
				if(pkHandicap != NULL && strcmp(kPlayer.m_szHandicap, pkHandicap->GetType()) == 0)
				{
					eHandicap = (HandicapTypes)iHandicap;
					break;
				}
			}

			CvPreGame::setHandicap(ePlayer, eHandicap);
		}

		CvPreGame::setTeamType(ePlayer, (TeamTypes)kPlayer.m_byTeam);
		CvPreGame::setMinorCiv(ePlayer, false);

		if(strlen(kPlayer.m_szLeaderName) > 0)
			CvPreGame::setLeaderName(ePlayer, kPlayer.m_szLeaderName);

		if(strlen(kPlayer.m_szCivName) > 0)
		{
			CvPreGame::setCivilizationShortDescription(ePlayer, kPlayer.m_szCivName);
			CvPreGame::setCivilizationDescription(ePlayer, kPlayer.m_szCivName);
		}
	}

	// If this is a map with defined player slots then close any slots out of this defined range
	// that are being filled by computer players.  Hopefully there aren't any human players there.
	const uint uiPlayerSlotCount = sg_kMapInfo.uiMajorCivStartingPositions + sg_kMapInfo.uiPlayers;
	if(uiPlayerSlotCount > 0)
	{
		for(uint i = uiPlayerSlotCount; i < MAX_MAJOR_CIVS; ++i)
		{
			const PlayerTypes ePlayer = (PlayerTypes)i;
			const SlotStatus eStatus = CvPreGame::slotStatus(ePlayer);
			if(eStatus == SS_COMPUTER)
				CvPreGame::resetPlayer(ePlayer);	// Do a full reset, which will also close the slot.  A reset is better so that the player data does not get initialized later.
		}
	}

	const uint uiCityStateCount = std::min(sg_kSave.GetCityStateCount(), (byte)MAX_MINOR_CIVS);

	if(uiCityStateCount > 0)
		CvPreGame::setNumMinorCivs(uiCityStateCount);
	else if(sg_kMapInfo.uiMinorCivStartingPositions > 0)
		CvPreGame::setNumMinorCivs(sg_kMapInfo.uiMinorCivStartingPositions);
	else if(!CvPreGame::mapNoPlayers())
		CvPreGame::setNumMinorCivs(0);

	for(uint i = 0; i < uiCityStateCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)(i + MAX_MAJOR_CIVS);
		const CvWorldBuilderMap::Player& kPlayer = sg_kSave.GetCityState(i);

		MinorCivTypes eCivType = NO_MINORCIV;
		for(int iCiv = 0; iCiv < GC.getNumMinorCivInfos(); ++iCiv)
		{
			CvMinorCivInfo* pkCivilization = GC.getMinorCivInfo((MinorCivTypes)iCiv);
			if(pkCivilization != NULL && strcmp(kPlayer.m_szCivType, pkCivilization->GetType()) == 0)
			{
				eCivType = (MinorCivTypes)iCiv;
				break;
			}
		}

		CvPreGame::setMinorCivType(ePlayer, eCivType);

		PlayerColorTypes eColorType = NO_PLAYERCOLOR;
		for(int iColor = 0; iColor < GC.GetNumPlayerColorInfos(); ++iColor)
		{
			CvPlayerColorInfo* pkColor = GC.GetPlayerColorInfo((PlayerColorTypes)iColor);
			if(pkColor != NULL && strcmp(kPlayer.m_szTeamColor, pkColor->GetType()) == 0)
			{
				eColorType = (PlayerColorTypes)iColor;
				break;
			}
		}

		CvPreGame::setPlayerColor(ePlayer, eColorType);

		HandicapTypes eHandicap = NO_HANDICAP;
		for(int iHandicap = 0; iHandicap < GC.getNumHandicapInfos(); ++iHandicap)
		{
			CvHandicapInfo* pkHandicap = GC.getHandicapInfo((HandicapTypes)iHandicap);
			if(pkHandicap != NULL && strcmp(kPlayer.m_szHandicap, pkHandicap->GetType()) == 0)
			{
				eHandicap = (HandicapTypes)iHandicap;
				break;
			}
		}

		CvPreGame::setHandicap(ePlayer, eHandicap);

		CvPreGame::setSlotStatus(ePlayer, SS_COMPUTER);
		CvPreGame::setTeamType(ePlayer, (TeamTypes)kPlayer.m_byTeam);
		CvPreGame::setMinorCiv(ePlayer, true);
	}
}

void SetPlayerInitialItems(CvPlayer& kGameplayPlayer, const CvWorldBuilderMap::Player& kSavedPlayer)
{
	CvTreasury* pkTreasury = kGameplayPlayer.GetTreasury();
	FAssertMsg(pkTreasury, "Unable to set gold amount.  Treasury Missing!");
	if(pkTreasury != NULL)
		pkTreasury->SetGold(kSavedPlayer.m_uiGold);

	kGameplayPlayer.setJONSCulture(kSavedPlayer.m_uiCulture);

	const int iPolicyCount = GC.getNumPolicyInfos();
	for(int iPolicy = 0; iPolicy < iPolicyCount; ++iPolicy)
	{
		const PolicyTypes ePolicy = (PolicyTypes)iPolicy;
		if(kSavedPlayer.m_kPolicies[iPolicy])
		{
			// Check the policy branch
			CvPlayerPolicies* pkPlayerPolicies = kGameplayPlayer.GetPlayerPolicies();
			const CvPolicyEntry* pkPolicy = GC.getPolicyInfo(ePolicy);
			if(pkPolicy != NULL && pkPlayerPolicies != NULL)
			{
				PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes)pkPolicy->GetPolicyBranchType();

				// NO_POLICY_BRANCH_TYPE means that this is probably a free policy.
				// We'll have to look through the policy branches to find a matching one.
				if(ePolicyBranch == NO_POLICY_BRANCH_TYPE)
				{
					const int iPolicyBranchCount = GC.getNumPolicyBranchInfos();
					for(int iPolicyBranch = 0; iPolicyBranch < iPolicyBranchCount; ++iPolicyBranch)
					{
						const PolicyBranchTypes eCurrentBranch = (PolicyBranchTypes)iPolicyBranch;
						const CvPolicyBranchEntry* pkBranch = GC.getPolicyBranchInfo(eCurrentBranch);
						if(pkBranch != NULL && pkBranch->GetFreePolicy() == ePolicy)
						{
							ePolicyBranch = eCurrentBranch;
							break;
						}
					}
				}

				// Unlock the policy branch if it hasn't been unlocked yet
				if(ePolicyBranch != NO_POLICY_BRANCH_TYPE &&
				        !pkPlayerPolicies->IsPolicyBranchUnlocked(ePolicyBranch))
				{
					pkPlayerPolicies->SetPolicyBranchUnlocked(ePolicyBranch, true, false);

					// Also, unlock the free policy for the branch
					const CvPolicyBranchEntry* pkBranch = GC.getPolicyBranchInfo(ePolicyBranch);
					if(pkBranch != NULL)
					{
						PolicyTypes eFreePolicy = (PolicyTypes)pkBranch->GetFreePolicy();
						if(eFreePolicy != NO_POLICY)
							kGameplayPlayer.setHasPolicy(eFreePolicy, true);
					}
				}
			}

			// Finally, unlock the policy
			kGameplayPlayer.setHasPolicy(ePolicy, true);
		}
	}

	const TeamTypes eTeam = kGameplayPlayer.getTeam();
	if(eTeam != NO_TEAM)
	{
		CvTeam& kTeam = GET_TEAM(eTeam);

		int iStartingEra = 0;
		const int iEraCount = GC.getNumEraInfos();
		for(int iEra = 0; iEra < iEraCount; ++iEra)
		{
			const EraTypes eEra = (EraTypes)iEra;
			const CvEraInfo* pkEra = GC.getEraInfo(eEra);
			if(pkEra != NULL && strcmp(pkEra->GetType(), kSavedPlayer.m_szEra) == 0)
			{
				iStartingEra = iEra;
				break;
			}
		}

		if(iStartingEra > 0)
		{
			const int iTechCount = GC.getNumTechInfos();
			for(int iTech = 0; iTech < iTechCount; ++iTech)
			{
				const TechTypes eTech = (TechTypes)iTech;
				const CvTechEntry* pkTech = GC.getTechInfo(eTech);
				if(pkTech != NULL && pkTech->GetEra() < iStartingEra)
					kTeam.setHasTech(eTech, true, kGameplayPlayer.GetID(), false, false);
			}
		}
	}
}

void SetTeamInitialItems(CvTeam& kGameplayTeam, const CvWorldBuilderMap::Team& kSavedTeam)
{
	const int iTechCount = GC.getNumTechInfos();
	for(int iTech = 0; iTech < iTechCount; ++iTech)
	{
		const TechTypes eTech = (TechTypes)iTech;
		if(kSavedTeam.m_kTechs[iTech])
			kGameplayTeam.setHasTech(eTech, true, NO_PLAYER, false, false);
	}
}

void CvWorldBuilderMapLoader::SetInitialItems(bool bFirstCall)
{
	const uint uiTeamCount = std::min(sg_kSave.GetTeamCount(), (byte)MAX_TEAMS);
	for(uint i = 0; i < uiTeamCount; ++i)
	{
		const TeamTypes eTeam = (TeamTypes)i;
		SetTeamInitialItems(GET_TEAM(eTeam), sg_kSave.GetTeam(i));
	}

	const uint uiPlayerCount = std::min(sg_kSave.GetPlayerCount(), (byte)MAX_CIV_PLAYERS);
	for(uint i = 0; i < uiPlayerCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)i;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		SetPlayerInitialItems(kPlayer, sg_kSave.GetPlayer(i));

		if(!bFirstCall)
		{
			// Assign all citizens
			CvCity* pLoopCity;
			int iLoop;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				pLoopCity->GetCityCitizens()->DoTurn();
			}

			// Update player data
			kPlayer.GetCityConnections()->Update();
			kPlayer.GetTreasury()->DoUpdateCityConnectionGold();
			kPlayer.GetTreasury()->DoGold();
			kPlayer.DoUpdateHappiness();
		}
	}

	const uint uiCityStateCount = std::min(sg_kSave.GetCityStateCount(), (byte)MAX_MINOR_CIVS);
	for(uint i = 0; i < uiCityStateCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)(i + MAX_MAJOR_CIVS);
		SetPlayerInitialItems(GET_PLAYER(ePlayer), sg_kSave.GetCityState(i));
	}
}

PlayerTypes GetPlayerType(byte byCulture)
{
	if(byCulture < CvWorldBuilderMap::MaxPlayers)
	{
		if(byCulture < MAX_MAJOR_CIVS)
			return (PlayerTypes)byCulture;
		else
			return NO_PLAYER; // Player out of range
	}
	else if(byCulture < CvWorldBuilderMap::MaxPlayers + CvWorldBuilderMap::MaxCityStates)
	{
		const uint uiCityState = byCulture - CvWorldBuilderMap::MaxPlayers;
		if(uiCityState < MAX_MINOR_CIVS)
			return (PlayerTypes)(uiCityState + MAX_MAJOR_CIVS);
		else
			return NO_PLAYER; // City State out of range
	}
	else if(byCulture == CvWorldBuilderMap::MaxPlayers + CvWorldBuilderMap::MaxCityStates)
	{
		return BARBARIAN_PLAYER;
	}
	else
	{
		return NO_PLAYER; // Completely invalid culture type
	}
}

void SetupCity(const CvWorldBuilderMap::City& kSavedCity, int iPlotX, int iPlotY)
{
	const PlayerTypes ePlayer = GetPlayerType(kSavedCity.m_byOwner);
	if(ePlayer != NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		CvCity* pkGameplayCity = kPlayer.initCity(iPlotX, iPlotY);
		if(pkGameplayCity != NULL)
		{
			pkGameplayCity->setName(kSavedCity.m_szName);
			pkGameplayCity->setPopulation(kSavedCity.m_wPopulation);

			if(kSavedCity.GetFlag(CvWorldBuilderMap::City::CITY_PUPPET_STATE))
			{
				pkGameplayCity->SetOccupied(true);
				pkGameplayCity->SetPuppet(true);
			}
			else if(kSavedCity.GetFlag(CvWorldBuilderMap::City::CITY_OCCUPIED))
			{
				pkGameplayCity->SetOccupied(true);
			}

			if(kSavedCity.m_uiHealth != kSavedCity.MaxHealth)
			{
				const int iMaxHitPoints = pkGameplayCity->GetMaxHitPoints();

				// TODO: Oh no!  It's floating point math!  This may be an issue for multi-player.
				const float fHitPoints = kSavedCity.GetHealthAsFloat() * (float)iMaxHitPoints;

				// Don't allow the city to be killed by a precision error
				int iHitPoints = (int)fHitPoints;
				if(iHitPoints == 0 && kSavedCity.m_uiHealth != 0)
					iHitPoints = 1;

				pkGameplayCity->setDamage(iMaxHitPoints - iHitPoints);
			}

			CvCityBuildings* pkCityBuildings = pkGameplayCity->GetCityBuildings();
			if(pkCityBuildings != NULL)
			{
				const int iBuildingTypeCount = GC.getNumBuildingInfos();
				for(int iBuilding = 0; iBuilding < iBuildingTypeCount; ++iBuilding)
				{
					const BuildingTypes eBuildingType = (BuildingTypes)iBuilding;
					if(kSavedCity.m_kBuildings[iBuilding])
						pkCityBuildings->SetNumRealBuilding(eBuildingType, 1);
				}
			}
			kPlayer.DoUpdateNextPolicyCost();
		}
	}
}

void SetupUnit(const CvWorldBuilderMap::Unit& kSavedUnit, int iPlotX, int iPlotY)
{
	const PlayerTypes ePlayer = GetPlayerType(kSavedUnit.m_byOwner);
	UnitTypes eUnitType = (UnitTypes)kSavedUnit.m_byUnitType;
	// Test to see if m_byUnitType is still a byte, if so, if it is 255, then it is really -1 and we must reflect this in the eUnitType
	// The World Builder map format should be changed so the serialization of the unit type is an int.  We are already close to
	// maxing out the byte range as it stands now.
	if (sizeof(kSavedUnit.m_byUnitType) == 1 && kSavedUnit.m_byUnitType == 255)
	{
		eUnitType = NO_UNIT;
	}

	UnitAITypes eAIType = NO_UNITAI;
	const CvUnitEntry* pkUnitType = GC.getUnitInfo(eUnitType);
	FAssert(pkUnitType); // We should probably be concerned if this unit type isn't valid
	if(pkUnitType != NULL)
		eAIType = (UnitAITypes)pkUnitType->GetDefaultUnitAIType();
	else
		return;	// The unit type is invalid, we really can't go any further.

	DirectionTypes eFacing = NO_DIRECTION;
	switch(kSavedUnit.m_byDirection)
	{
	case CvWorldBuilderMap::Unit::DIRECTION_EAST:
		eFacing = DIRECTION_EAST;
		break;

	case CvWorldBuilderMap::Unit::DIRECTION_NORTHEAST:
		eFacing = DIRECTION_NORTHEAST;
		break;

	case CvWorldBuilderMap::Unit::DIRECTION_NORTHWEST:
		eFacing = DIRECTION_NORTHWEST;
		break;

	case CvWorldBuilderMap::Unit::DIRECTION_SOUTHEAST:
		eFacing = DIRECTION_SOUTHEAST;
		break;

	case CvWorldBuilderMap::Unit::DIRECTION_SOUTHWEST:
		eFacing = DIRECTION_SOUTHWEST;
		break;

	case CvWorldBuilderMap::Unit::DIRECTION_WEST:
		eFacing = DIRECTION_WEST;
		break;

	default:
		eFacing = (DirectionTypes)GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES, "Picking Random Unit Direction");
	}

	if(ePlayer != NO_PLAYER)
	{
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		CvUnit* pkGameplayUnit = kPlayer.initUnit(eUnitType, iPlotX, iPlotY, eAIType, eFacing, false);
		if(pkGameplayUnit != NULL)
		{
			if(kSavedUnit.GetFlag(CvWorldBuilderMap::Unit::UNIT_FORTIFIED))
			{
				if(pkGameplayUnit->isFortifyable(true))
					gDLL->sendPushMission(pkGameplayUnit->GetID(), CvTypes::getMISSION_FORTIFY(), 0, 0, 0, false);
				else
					gDLL->sendPushMission(pkGameplayUnit->GetID(), CvTypes::getMISSION_SLEEP(), 0, 0, 0, false);
			}

			if(kSavedUnit.GetFlag(CvWorldBuilderMap::Unit::UNIT_EMBARKED))
			{
				const PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();
				pkGameplayUnit->setHasPromotion(ePromotionEmbarkation, true);
				pkGameplayUnit->setEmbarked(true);
				pkGameplayUnit->setMoves(pkGameplayUnit->maxMoves());
			}

			if(kSavedUnit.m_uiHealth != kSavedUnit.MaxHealth)
			{
				const int iMaxHitPoints = pkGameplayUnit->GetMaxHitPoints();

				// TODO: Oh no!  It's floating point math!  This may be an issue for multi-player.
				const float fHitPoints = kSavedUnit.GetHealthAsFloat() * (float)iMaxHitPoints;

				// Don't allow the unit to be killed by a precision error
				int iHitPoints = (int)fHitPoints;
				if(iHitPoints == 0 && kSavedUnit.m_uiHealth != 0)
					iHitPoints = 1;

				pkGameplayUnit->setDamage(iMaxHitPoints - iHitPoints);
			}

			pkGameplayUnit->setExperience((int)kSavedUnit.m_uiExperience);

			const int iPromotionTypeCount = GC.getNumPromotionInfos();
			for(int iPromotion = 0; iPromotion < iPromotionTypeCount; ++iPromotion)
			{
				const PromotionTypes ePromotionType = (PromotionTypes)iPromotion;
				if(kSavedUnit.m_kPromotions[iPromotion])
					pkGameplayUnit->setHasPromotion(ePromotionType, true);
			}

			if(kSavedUnit.m_hCustomName.Valid())
			{
				CvWorldBuilderMap::UnitName* pkUnitName = sg_kSave.m_kUnitNames[kSavedUnit.m_hCustomName];
				if(pkUnitName != NULL)
				{
					pkGameplayUnit->setName(pkUnitName->m_szText);
				}
			}
		}
	}
}

const CvCity* GetCity(const PlayerTypes ePlayer, const CvPlot* pkPlot)
{
	if(pkPlot != NULL)
	{
		const CvCity* pkCity = pkPlot->getPlotCity();
		if(pkCity != NULL && pkCity->getOwner() == ePlayer)
			return pkCity;
	}

	return NULL;
}

const CvCity* FindClosestCity(const PlayerTypes ePlayer, const CvPlot& kPlot)
{
	{
		const CvCity* pkPlotCity = GetCity(ePlayer, &kPlot);
		if(pkPlotCity != NULL)
			return pkPlotCity;
	}

	const CvMap& kMap = GC.getMap();

	const int iPlotX = kPlot.getX();
	const int iPlotY = kPlot.getY();

	const int iMapHeight = kMap.getGridHeight();
	const int iMapWidth  = kMap.getGridWidth();

	const bool bWorldWrap = kMap.isWrapX();

	// Do a search radiating from the search plot
	for(int iDist = 1; iDist < iMapHeight || iDist < iMapWidth; ++iDist)
	{
		// Center Row
		if(iPlotX - iDist >= 0)
		{
			const CvPlot* pkTestPlot = kMap.plot(iPlotX - iDist, iPlotY);
			const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
			if(pkCity) return pkCity;
		}
		else if(bWorldWrap)
		{
			int iX = iPlotX - iDist;
			while(iX < 0) iX += iMapWidth;

			const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY);
			const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
			if(pkCity) return pkCity;
		}

		if(iPlotX + iDist < iMapWidth)
		{
			const CvPlot* pkTestPlot = kMap.plot(iPlotX + iDist, iPlotY);
			const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
			if(pkCity) return pkCity;
		}
		else if(bWorldWrap)
		{
			int iX = iPlotX + iDist;
			while(iX >= iMapWidth) iX -= iMapWidth;

			const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY);
			const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
			if(pkCity) return pkCity;
		}

		// Top and Bottom Rows
		const int iRowOffset = (iPlotY % 2 == 0)? 0 : 1;
		for(int iRow = 1; iRow < iDist; ++iRow)
		{
			const int iRowWidth = (iDist * 2) - (iRow - 1);

			{
				// Left-most plot in the current row
				int iX = iPlotX - (iRowWidth / 2);
				if(iRow % 2 == 1) iX += iRowOffset;

				if(iX < 0)
				{
					if(bWorldWrap)
						do
						{
							iX += iMapWidth;
						}
						while(iX < 0);
				}

				if(iX >= 0)
				{
					if(iPlotY - iRow >= 0)
					{
						const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY - iRow);
						const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
						if(pkCity) return pkCity;
					}

					if(iPlotY + iRow < iMapHeight)
					{
						const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY + iRow);
						const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
						if(pkCity) return pkCity;
					}
				}
			}

			{
				// Right-most plot in the current row
				int iX = iPlotX + (iRowWidth / 2) - 1;
				if(iRow % 2 == 1) iX += iRowOffset;

				if(iX >= iMapWidth)
				{
					if(bWorldWrap)
						do
						{
							iX -= iMapWidth;
						}
						while(iX >= iMapWidth);
				}

				if(iX < iMapWidth)
				{
					if(iPlotY - iRow >= 0)
					{
						const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY - iRow);
						const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
						if(pkCity) return pkCity;
					}

					if(iPlotY + iRow < iMapHeight)
					{
						const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY + iRow);
						const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
						if(pkCity) return pkCity;
					}
				}
			}
		}

		// Top-most and Bottom-most rows
		{
			const int iRowWidth = iDist + 1;
			for(int iPlot = 0; iPlot < iRowWidth; ++iPlot)
			{
				int iX = iPlotX + iPlot - (iRowWidth / 2);
				if(iDist % 2 == 1) iX += iRowOffset;

				if(iX < 0)
				{
					if(!bWorldWrap)
						continue;

					do
					{
						iX += iMapWidth;
					}
					while(iX < 0);
				}
				else if(iX >= iMapWidth)
				{
					if(!bWorldWrap)
						continue;

					do
					{
						iX -= iMapWidth;
					}
					while(iX >= iMapWidth);
				}

				if(iPlotY - iDist >= 0)
				{
					const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY - iDist);
					const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
					if(pkCity) return pkCity;
				}

				if(iPlotY + iDist < iMapHeight)
				{
					const CvPlot* pkTestPlot = kMap.plot(iX, iPlotY + iDist);
					const CvCity* pkCity = GetCity(ePlayer, pkTestPlot);
					if(pkCity) return pkCity;
				}
			}
		}
	}

	return NULL;
}

bool CvWorldBuilderMapLoader::InitMap()
{
	CvMap& kMap = GC.getMap();

	FAssertMsg(
	    sg_kSave.GetWidth() == kMap.getGridWidth() &&
	    sg_kSave.GetHeight() == kMap.getGridHeight(),
	    "Save wrong size");

	OutputDebugStringA("Setting up players...\n");

	const uint uiPlayerCount = std::min(sg_kSave.GetPlayerCount(), (byte)MAX_CIV_PLAYERS);
	for(uint i = 0; i < uiPlayerCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)i;
		CvPlayer& kGameplayPlayer = GET_PLAYER(ePlayer);
		const CvWorldBuilderMap::Player& kSavedPlayer = sg_kSave.GetPlayer(i);

		if(kSavedPlayer.m_uiStartX < sg_kSave.GetWidth() &&
		        kSavedPlayer.m_uiStartY < sg_kSave.GetHeight())
		{
			CvPlot* pkStartingPlot = kMap.plot(kSavedPlayer.m_uiStartX, kSavedPlayer.m_uiStartY);
			kGameplayPlayer.setStartingPlot(pkStartingPlot);
		}
	}

	const uint uiCityStateCount = std::min(sg_kSave.GetCityStateCount(), (byte)MAX_MINOR_CIVS);
	for(uint i = 0; i < uiCityStateCount; ++i)
	{
		const PlayerTypes ePlayer = (PlayerTypes)(i + MAX_MAJOR_CIVS);
		CvPlayer& kGameplayPlayer = GET_PLAYER(ePlayer);
		const CvWorldBuilderMap::Player& kSavedPlayer = sg_kSave.GetCityState(i);

		if(kSavedPlayer.m_uiStartX < sg_kSave.GetWidth() &&
		        kSavedPlayer.m_uiStartY < sg_kSave.GetHeight())
		{
			CvPlot* pkStartingPlot = kMap.plot(kSavedPlayer.m_uiStartX, kSavedPlayer.m_uiStartY);
			kGameplayPlayer.setStartingPlot(pkStartingPlot);
		}
	}

	const uint uiTeamCount = std::min(sg_kSave.GetTeamCount(), (byte)MAX_TEAMS);

	// HACK: Call SetInitialItems early.  It has to be called later too :_(
	// This is because of what is essentially a circular dependency.  Techs
	// need to be initialized before units are created (for the embarkation
	// promotion among other things).  However, units must be created before
	// CvGame sets up the initial player state otherwise a scenario player may
	// be given starting units they don't need and shouldn't have.  Unfortunately
	// this also does stuff like reset starting techs.  Therefore SetInitialItems
	// must be called AGAIN to put the player state back to where it should be.
	// This second call is done from CvInitMgr::GameCoreNew.  You may be tempted to
	// remove this hack but be very careful if you do and test thoroughly with scenario saves.
	SetInitialItems(true);

	FFastVector<CvPlayer*> kMajorCivs;
	FFastVector<CvPlayer*> kMinorCivs;
	for(uint i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		PlayerTypes ePlayer = PlayerTypes(i);
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isAlive() && kPlayer.getStartingPlot() == NULL)
		{
			if(kPlayer.isMinorCiv())
				kMinorCivs.push_back(&kPlayer);
			else
				kMajorCivs.push_back(&kPlayer);
		}
	}

	uint uiMajorCivsPlaced = 0;
	uint uiMinorCivsPlaced = 0;

	OutputDebugStringA("Setting up map...\n");
	uint uiPlotCount = sg_kSave.GetWidth() * sg_kSave.GetHeight();
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		const CvWorldBuilderMap::PlotMapData& kPlotData = sg_kSave.GetPlotData(i);
		CvPlot* pkPlot = kMap.plotByIndex(i);
		FAssertMsg(pkPlot, "Missing CvPlot for this location");
		if(pkPlot == NULL) continue;

		pkPlot->setTerrainType((TerrainTypes)kPlotData.GetTerrainType(), false, false);

		CvWorldBuilderMap::PlotMapData::PlotHeight ePlotHeight = kPlotData.GetPlotHeight();
		if(ePlotHeight != CvWorldBuilderMap::PlotMapData::FLAT_TERRAIN)
		{
			if(ePlotHeight == CvWorldBuilderMap::PlotMapData::HILLS)
				pkPlot->setPlotType(PLOT_HILLS);
			else
				pkPlot->setPlotType(PLOT_MOUNTAIN);
		}

		if(kPlotData.GetResourceType() != CvWorldBuilderMap::PlotMapData::InvalidResource)
		{
			int iResourceAmount = kPlotData.GetResourceAmount();
			if(iResourceAmount <= 0)
				iResourceAmount = 1;

			pkPlot->setResourceType((ResourceTypes)kPlotData.GetResourceType(), iResourceAmount);
		}

		if(kPlotData.GetFeatureType() != CvWorldBuilderMap::PlotMapData::InvalidFeature)
			pkPlot->setFeatureType((FeatureTypes)kPlotData.GetFeatureType());

		if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::W_OF_RIVER))
		{
			if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_S))
				pkPlot->setWOfRiver(true, FLOWDIRECTION_SOUTH);
			else
				pkPlot->setWOfRiver(true, FLOWDIRECTION_NORTH);
		}

		if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::NW_OF_RIVER))
		{
			if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NE))
				pkPlot->setNWOfRiver(true, FLOWDIRECTION_NORTHEAST);
			else
				pkPlot->setNWOfRiver(true, FLOWDIRECTION_SOUTHWEST);
		}

		if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::NE_OF_RIVER))
		{
			if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NW))
				pkPlot->setNEOfRiver(true, FLOWDIRECTION_NORTHWEST);
			else
				pkPlot->setNEOfRiver(true, FLOWDIRECTION_SOUTHEAST);
		}

		if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::START_POS_MAJOR))
		{
			if(uiMajorCivsPlaced < kMajorCivs.size())
			{
				kMajorCivs[uiMajorCivsPlaced]->setStartingPlot(pkPlot);
				uiMajorCivsPlaced++;
			}
		}
		else if(kPlotData.GetFlag(CvWorldBuilderMap::PlotMapData::START_POS_MINOR))
		{
			if(uiMinorCivsPlaced < kMinorCivs.size())
			{
				kMinorCivs[uiMinorCivsPlaced]->setStartingPlot(pkPlot);
				uiMinorCivsPlaced++;
			}
		}

		pkPlot->SetContinentType(kPlotData.GetContinentType());
	}

	OutputDebugStringA("Calculating Areas...\n");

	kMap.recalculateAreas();

	OutputDebugStringA("Adding Scenario Elements...\n");

	GC.getGame().GetTacticalAnalysisMap()->Init(GC.getMap().numPlots());

	// Init Diplomacy
	for(uint uiTeam1 = 0; uiTeam1 < uiTeamCount; ++uiTeam1)
	{
		for(uint uiTeam2 = uiTeam1 + 1; uiTeam2 < uiTeamCount; ++uiTeam2)
		{
			const TeamTypes eTeam1 = (TeamTypes)uiTeam1;
			const TeamTypes eTeam2 = (TeamTypes)uiTeam2;
			CvTeam& kTeam1 = GET_TEAM(eTeam1);
			CvTeam& kTeam2 = GET_TEAM(eTeam2);

			if(sg_kSave.m_kTeamsInContact.Get(uiTeam1, uiTeam2))
				kTeam1.meet(eTeam2, true);

			if(sg_kSave.m_kTeamsAtWar.Get(uiTeam1, uiTeam2))
			{
				kTeam1.setAtWar(eTeam2, true);
				kTeam2.setAtWar(eTeam1, true);
			}

			if(sg_kSave.m_kTeamsAtPermanentWarOrPeace.Get(uiTeam1, uiTeam2))
			{
				kTeam1.setPermanentWarPeace(eTeam2, true);
				kTeam2.setPermanentWarPeace(eTeam1, true);
			}

			if(sg_kSave.m_kTeamsSharingOpenBorders.Get(uiTeam1, uiTeam2))
			{
				kTeam1.SetAllowsOpenBordersToTeam(eTeam2, true);
				kTeam2.SetAllowsOpenBordersToTeam(eTeam1, true);
			}

			if(sg_kSave.m_kTeamsSharingDefensivePacts.Get(uiTeam1, uiTeam2))
			{
				kTeam1.SetHasDefensivePact(eTeam2, true);
				kTeam2.SetHasDefensivePact(eTeam1, true);
			}
		}
	}

	// Init City State Relationships
	for(uint uiCityState = 0; uiCityState < uiCityStateCount; ++uiCityState)
	{
		const PlayerTypes eCityStatePlayer = GetPlayerType((byte)(uiCityState + CvWorldBuilderMap::MaxPlayers));
		if(eCityStatePlayer != NO_PLAYER)
		{
			CvPlayer& kCityState = GET_PLAYER(eCityStatePlayer);
			CvMinorCivAI* pkAI = kCityState.GetMinorCivAI();
			FAssertMsg(pkAI, "Cannot set minor civ influence.  Missing CvMinorCivAI.");
			if(pkAI != NULL)
			{
				for(uint uiMajorCiv = 0; uiMajorCiv < uiPlayerCount; ++uiMajorCiv)
				{
					const PlayerTypes eMajorCivPlayer = GetPlayerType((byte)uiMajorCiv);
					if(eMajorCivPlayer != NO_PLAYER)
					{
						pkAI->SetFriendshipWithMajor(eMajorCivPlayer, sg_kSave.m_aiCityStateInfluence[uiMajorCiv][uiCityState]);
					}
				}
			}
		}
	}

	// Scenario elements must come after areas have been calculated
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		const CvWorldBuilderMap::PlotScenarioData& kPlotData = sg_kSave.GetPlotScenarioData(i);
		CvPlot* pkPlot = kMap.plotByIndex(i);
		FAssertMsg(pkPlot, "Missing CvPlot for this location");
		if(pkPlot == NULL) continue;

		const int iPlotX = pkPlot->getX();
		const int iPlotY = pkPlot->getY();

		const byte byNaturalWonder = sg_kSave.GetPlotData(i).GetNaturalWonderType();
		if(byNaturalWonder != CvWorldBuilderMap::PlotMapData::InvalidNaturalWonder)
			pkPlot->setFeatureType((FeatureTypes)byNaturalWonder);

		if(kPlotData.m_byImprovement != CvWorldBuilderMap::PlotScenarioData::InvalidImprovement)
			pkPlot->setImprovementType((ImprovementTypes)kPlotData.m_byImprovement);

		if(kPlotData.m_byRoute != CvWorldBuilderMap::PlotScenarioData::InvalidImprovement)
		{
			RouteTypes eRoute;
			if(kPlotData.m_byRoute == 0)
				eRoute = ROUTE_ROAD;
			else
				eRoute = ROUTE_RAILROAD;

			pkPlot->setRouteType(eRoute);

			const PlayerTypes eOwner = GetPlayerType(kPlotData.m_byRouteOwner);
			if(eOwner != NO_PLAYER && !pkPlot->isOwned())
			{
				// Mark the player as responsible for this route and update the treasury
				CvTreasury* pkTreasury = GET_PLAYER(eOwner).GetTreasury();
				const CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkTreasury != NULL && pkRouteInfo != NULL)
				{
					if(pkPlot->MustPayMaintenanceHere(eOwner))
					{
						pkTreasury->ChangeBaseImprovementGoldMaintenance(pkRouteInfo->GetGoldMaintenance());
					}
					pkPlot->SetPlayerResponsibleForRoute(eOwner);
				}
			}
		}

		const CvWorldBuilderMap::City* pkCity = sg_kSave.m_kCities[kPlotData.m_hCity];
		if(pkCity != NULL)
		{
			SetupCity(*pkCity, iPlotX, iPlotY);
		}

		// Note - All calls to SetupUnit for this plot must come after SetupCity for this plot.
		// This is because of garrisoned units that need the city to be there first.
		for(const CvWorldBuilderMap::Unit* pkUnit = sg_kSave.m_kUnits[kPlotData.m_hUnitStack];
		        pkUnit != NULL;
		        pkUnit = sg_kSave.m_kUnits[pkUnit->m_hStackedUnit])
		{
			SetupUnit(*pkUnit, iPlotX, iPlotY);
		}

		for(uint uiTeam = 0; uiTeam < uiTeamCount; ++uiTeam)
		{
			if(sg_kSave.GetVisibility(iPlotX, iPlotY, uiTeam))
				pkPlot->setRevealed((TeamTypes)uiTeam, true);
		}
	}

	// Set culture.  This must come after all cities have been added.
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		const CvWorldBuilderMap::PlotScenarioData& kPlotData = sg_kSave.GetPlotScenarioData(i);
		CvPlot* pkPlot = kMap.plotByIndex(i);
		FAssertMsg(pkPlot, "Missing CvPlot for this location");
		if(pkPlot == NULL) continue;

		const PlayerTypes eOwner = GetPlayerType(kPlotData.m_byCulture);
		if(eOwner != NO_PLAYER)
		{
			int iCityID = 0;
			const CvCity* pkCity = FindClosestCity(eOwner, *pkPlot);
			if(pkCity) iCityID = pkCity->GetID();
			pkPlot->setOwner(eOwner, iCityID);
		}
	}

	OutputDebugStringA("Map Load Success!\n");

	return true;
}

bool CvWorldBuilderMapLoader::Load(const wchar_t* wszFilename)
{
	InitTypeDesc();

	return sg_kSave.Load(wszFilename, sg_kMapTypeDesc) && InitMap();
}

bool CvWorldBuilderMapLoader::Save(const wchar_t* wszFilename, const char* szMapName)
{
	InitTypeDesc();

	CvMap& kMap = GC.getMap();
	sg_kSave.Resize(kMap.getGridWidth(), kMap.getGridHeight());
	sg_kSave.ClearScenarioData();

	// Set map name
	if(szMapName == NULL)
	{
		FStringFixedBufferW(sFilename, MAX_PATH);
		FStringFixedBufferW(sFileTitle, MAX_PATH);

		sFilename = wszFilename;
		sFilename.ExtractFileTitle(sFileTitle);
		sFileTitle.CopyToUTF8(&sg_kSave.m_sMapName);
	}
	else
	{
		sg_kSave.m_sMapName = szMapName;
	}

	sg_kSave.m_sMapDescription = "";

	sg_kSave.m_uiFlags = 0;
	sg_kSave.SetFlag(CvWorldBuilderMap::FLAG_WORLD_WRAP, kMap.isWrapX());

	sg_kSave.SetWorldType(kMap.getWorldInfo().GetType());

	uint uiPlotCount = sg_kSave.GetWidth() * sg_kSave.GetHeight();
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		CvWorldBuilderMap::PlotMapData& kPlotData = sg_kSave.GetPlotData(i);
		CvPlot* pkPlot = kMap.plotByIndex(i);
		FAssertMsg(pkPlot, "Missing CvPlot for this location");
		if(pkPlot == NULL) continue;

		kPlotData.SetTerrainType((byte)pkPlot->getTerrainType());

		PlotTypes ePlotType = pkPlot->getPlotType();
		if(ePlotType == PLOT_HILLS)
			kPlotData.SetPlotHeight(CvWorldBuilderMap::PlotMapData::HILLS);
		else if(ePlotType == PLOT_MOUNTAIN)
			kPlotData.SetPlotHeight(CvWorldBuilderMap::PlotMapData::MOUNTAINS);
		else
			kPlotData.SetPlotHeight(CvWorldBuilderMap::
			                        PlotMapData::FLAT_TERRAIN);

		ResourceTypes eResourceType = pkPlot->getResourceType();
		if(eResourceType == NO_RESOURCE)
		{
			kPlotData.SetResourceType(CvWorldBuilderMap::PlotMapData::InvalidResource);
		}
		else
		{
			kPlotData.SetResourceType((byte)eResourceType);
			kPlotData.SetResourceAmount((byte)pkPlot->getNumResource());
		}

		FeatureTypes eFeatureType = pkPlot->getFeatureType();
		if(eFeatureType == NO_FEATURE)
		{
			kPlotData.SetFeatureType(CvWorldBuilderMap::PlotMapData::InvalidFeature);
			kPlotData.SetNaturalWonderType(CvWorldBuilderMap::PlotMapData::InvalidNaturalWonder);
		}
		else
		{
			CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeatureType);
			if(pkFeatureInfo != NULL && pkFeatureInfo->IsNaturalWonder())
			{
				kPlotData.SetFeatureType(CvWorldBuilderMap::PlotMapData::InvalidFeature);
				kPlotData.SetNaturalWonderType((byte)eFeatureType);
			}
			else
			{
				kPlotData.SetFeatureType((byte)eFeatureType);
				kPlotData.SetNaturalWonderType(CvWorldBuilderMap::PlotMapData::InvalidNaturalWonder);
			}
		}

		if(pkPlot->isWOfRiver())
		{
			kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::W_OF_RIVER);
			if(pkPlot->getRiverEFlowDirection() == FLOWDIRECTION_SOUTH)
				kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_S);
			else
				kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_S);
		}
		else
		{
			kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::W_OF_RIVER);
		}

		if(pkPlot->isNWOfRiver())
		{
			kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::NW_OF_RIVER);
			if(pkPlot->getRiverSEFlowDirection() == FLOWDIRECTION_NORTHEAST)
				kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NE);
			else
				kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NE);
		}
		else
		{
			kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::NW_OF_RIVER);
		}

		if(pkPlot->isNEOfRiver())
		{
			kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::NE_OF_RIVER);
			if(pkPlot->getRiverSWFlowDirection() == FLOWDIRECTION_NORTHWEST)
				kPlotData.SetFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NW);
			else
				kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::RIVER_FLOW_NW);
		}
		else
		{
			kPlotData.ClearFlag(CvWorldBuilderMap::PlotMapData::NE_OF_RIVER);
		}

		kPlotData.SetContinentType(pkPlot->GetContinentType());
	}

	return sg_kSave.Save(wszFilename, sg_kMapTypeDesc, false);
}

int CvWorldBuilderMapLoader::LoadModData(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const int iTop = lua_gettop(L);

	lua_getglobal(L, "MapModData");
	bool bFoundTable = lua_istable(L, -1);

	FAssertMsg(bFoundTable, "Unable to find mod data table");
	if(!bFoundTable)
	{
		lua_settop(L, iTop);
		return 0;
	}

	const int iModDataTable = iTop + 1;

	ModType& kGlobalsDataType = sg_kSave.m_kModData.m_kMapModData;
	const uint uiGlobalsCount = kGlobalsDataType.GetFieldCount();
	if(uiGlobalsCount > 0)
	{
		ModDataEntry kEntry = sg_kSave.m_kModData.GetMapDataEntry();

		lua_pushstring(L, "Globals");
		lua_createtable(L, 0, uiGlobalsCount);
		for(uint uiField = 0; uiField < uiGlobalsCount; ++uiField)
		{
			const char* szFieldName = kGlobalsDataType.GetFieldName(uiField);
			lua_pushstring(L, szFieldName);

			switch(kGlobalsDataType.GetFieldType(uiField))
			{
			case ModType::TYPE_BOOL:
			{
				const bool bValue = kEntry.GetFieldAsBool(uiField);
				lua_pushboolean(L, bValue);
			}
			break;

			case ModType::TYPE_FLOAT:
			{
				const float fValue = kEntry.GetFieldAsFloat(uiField);
				lua_pushnumber(L, fValue);
			}
			break;

			case ModType::TYPE_STRING:
			{
				const char* szField = kEntry.GetFieldAsString(uiField);
				lua_pushstring(L, szField);
			}
			break;

			case ModType::TYPE_ENUM:
			{
				const ModEnum* pkEnum = sg_kSave.m_kModData.m_kPlotModData.GetFieldEnumType(uiField);
				if(pkEnum != NULL)
				{
					int iValue = kEntry.GetFieldAsInt(uiField);
					const char* szMember = pkEnum->GetMember((uint)iValue);
					if(szMember == NULL) szMember = "";
					lua_pushstring(L, szMember);
				}
				else
				{
					lua_pushstring(L, "");
				}
			}
			}

			lua_rawset(L, -3);
		}

		lua_rawset(L, iModDataTable);
	}
	else
	{
		lua_pushstring(L, "Globals");
		lua_pushnil(L);
		lua_rawset(L, iModDataTable);
	}

	ModType& kPlotDataType = sg_kSave.m_kModData.m_kPlotModData;
	const uint uiFieldCount = kPlotDataType.GetFieldCount();
	if(uiFieldCount > 0)
	{
		const int iFieldStart = lua_gettop(L) + 1;
		for(uint uiField = 0; uiField < uiFieldCount; ++uiField)
		{
			const char* szFieldName = kPlotDataType.GetFieldName(uiField);
			lua_pushstring(L, szFieldName);

			switch(kPlotDataType.GetFieldType(uiField))
			{
			case ModType::TYPE_BOOL:
			{
				const bool bValue = kPlotDataType.GetFieldDefaultAsBool(uiField);
				lua_pushboolean(L, bValue);
			}
			break;

			case ModType::TYPE_FLOAT:
			{
				const float fValue = kPlotDataType.GetFieldDefaultAsFloat(uiField);
				lua_pushnumber(L, fValue);
			}
			break;

			case ModType::TYPE_STRING:
			{
				const char* szField = kPlotDataType.GetFieldDefaultAsString(uiField);
				lua_pushstring(L, szField);
			}
			break;

			case ModType::TYPE_ENUM:
			{
				int iValue = kPlotDataType.GetFieldDefaultAsInt(uiField);
				const ModEnum* pkEnum = sg_kSave.m_kModData.m_kPlotModData.GetFieldEnumType(uiField);
				if(pkEnum != NULL)
				{
					const char* szMember = pkEnum->GetMember((uint)iValue);
					if(szMember == NULL) szMember = "";
					lua_pushstring(L, szMember);
				}
				else
				{
					lua_pushstring(L, "");
				}
			}
			}
		}

		const uint uiMapWidth = sg_kSave.GetWidth();
		const uint uiMapHeight = sg_kSave.GetHeight();

		lua_pushstring(L, "Plots");
		lua_createtable(L, uiMapWidth, 1);
		for(uint x = 0; x < uiMapWidth; ++x)
		{
			lua_createtable(L, uiMapHeight, 1);
			for(uint y = 0; y < uiMapHeight; ++y)
			{
				lua_createtable(L, 0, uiFieldCount);
				ModDataEntry kPlot = sg_kSave.m_kModData.GetPlotDataEntry(x + y * uiMapWidth);
				for(uint uiField = 0; uiField < uiFieldCount; ++uiField)
				{
					if(kPlot.IsFieldDefaultValue(uiField))
					{
						lua_pushvalue(L, iFieldStart + 2 * uiField);
						lua_pushvalue(L, iFieldStart + 2 * uiField + 1);
						lua_rawset(L, -3);
					}
					else
					{
						switch(sg_kSave.m_kModData.m_kPlotModData.GetFieldType(uiField))
						{
						case ModType::TYPE_BOOL:
						{
							const bool bValue = kPlot.GetFieldAsBool(uiField);
							lua_pushvalue(L, iFieldStart + 2 * uiField);
							lua_pushboolean(L, bValue);
							lua_rawset(L, -3);
						}
						break;

						case ModType::TYPE_FLOAT:
						{
							const float fValue = kPlot.GetFieldAsFloat(uiField);
							lua_pushvalue(L, iFieldStart + 2 * uiField);
							lua_pushnumber(L, fValue);
							lua_rawset(L, -3);
						}
						break;

						case ModType::TYPE_STRING:
						{
							const char* szField = kPlot.GetFieldAsString(uiField);
							lua_pushvalue(L, iFieldStart + 2 * uiField);
							lua_pushstring(L, szField);
							lua_rawset(L, -3);
						}
						break;

						case ModType::TYPE_ENUM:
						{
							int iValue = kPlot.GetFieldAsInt(uiField);
							const ModEnum* pkEnum = kPlotDataType.GetFieldEnumType(uiField);
							if(pkEnum != NULL)
							{
								const char* szMember = pkEnum->GetMember((uint)iValue);
								if(szMember != NULL)
								{
									lua_pushvalue(L, iFieldStart + 2 * uiField);
									lua_pushstring(L, szMember);
									lua_rawset(L, -3);
								}
							}
						}
						}
					}
				}

				lua_rawseti(L, -2, y);
			}

			lua_rawseti(L, -2, x);
		}

		lua_rawset(L, iModDataTable);
	}
	else
	{
		lua_pushstring(L, "Plots");
		lua_pushnil(L);
		lua_rawset(L, iModDataTable);
	}

	lua_settop(L, iTop);

	return 0;
}



int CvWorldBuilderMapLoader::RunPostProcessScript(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const int iTop = lua_gettop(L);

	ModType& kGlobalsDataType = sg_kSave.m_kModData.m_kMapModData;
	const uint uiGlobalsCount = kGlobalsDataType.GetFieldCount();
	if(uiGlobalsCount > 0)
	{
		ModDataEntry kEntry = sg_kSave.m_kModData.GetMapDataEntry();
		for(uint uiField = 0; uiField < uiGlobalsCount; ++uiField)
		{
			const char* szFieldName = kGlobalsDataType.GetFieldName(uiField);
			if(szFieldName != NULL && strcmp(szFieldName, "PostProcessMapScript") == 0)
			{
				if(kGlobalsDataType.GetFieldType(uiField) == ModType::TYPE_STRING)
				{
					const char* szLua = kEntry.GetFieldAsString(uiField);
					if(szLua == NULL || strlen(szLua) == 0)
					{
						szLua = kGlobalsDataType.GetFieldDefaultAsString(uiField);
					}
					
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if(pkScriptSystem != NULL)
					{
						//1040 == _MAX_PATH * 4
						char szMapScriptPath[1040] = {0};
						const bool bResult = gDLL->GetEvaluatedMapScriptPath(szLua, szMapScriptPath, 1040);
						if(!bResult)
						{
							FAssertMsg(0, "Failed to find \"PostProcessMap\" in Post Process Map Script.");
						}

						const bool bLoadedMapGenerator = pkScriptSystem->LoadFile(L, szMapScriptPath);
						FAssertMsg(bLoadedMapGenerator, "Failed to load Post Process Map Script.");
						if(bLoadedMapGenerator)
						{
							lua_getglobal(L, "PostProcessMap");
							if(lua_isfunction(L, -1))
								pkScriptSystem->CallFunction(L, 0, 0);
							else
								FAssertMsg(0, "Failed to find \"PostProcessMap\" in Post Process Map Script.");
						}
					}
				}
			}
		}
	}

	lua_settop(L, iTop);

	return 0;
}


void CvWorldBuilderMapLoader::ValidateTerrain()
{
	CvMap& kMap = GC.getMap();

	TerrainTypes eValidTerrain = NO_TERRAIN;
	const int iNumTerrainTypes = GC.getNumTerrainInfos();
	for(int i = 0; i < iNumTerrainTypes; ++i)
	{
		const TerrainTypes eTerrain = (TerrainTypes)i;
		const CvTerrainInfo* pkTerrain = GC.getTerrainInfo(eTerrain);
		if(pkTerrain != NULL)
		{
			eValidTerrain = eTerrain;
			break;
		}
	}

	FAssertMsg(eValidTerrain != NO_TERRAIN, "Could not find ANY valid terrain types!");

	const int iPlotCount = kMap.getGridWidth() * kMap.getGridHeight();
	for(int i = 0; i < iPlotCount; ++i)
	{
		CvPlot* pkPlot = kMap.plotByIndex(i);
		if(pkPlot != NULL)
		{
			const TerrainTypes eTerrainType = pkPlot->getTerrainType();
			if(eTerrainType < 0 || eTerrainType >= iNumTerrainTypes)
			{
				pkPlot->setTerrainType(eValidTerrain);
			}
			else
			{
				const CvTerrainInfo* pkTerrain = GC.getTerrainInfo(eTerrainType);
				if(pkTerrain == NULL)
					pkPlot->setTerrainType(eValidTerrain);
			}
		}
	}
}

void CvWorldBuilderMapLoader::ValidateCoast()
{
	//FTimer kTimer;
	//kTimer.Start();

	CvMap& kMap = GC.getMap();
	const int iMapWidth = kMap.getGridWidth();
	const int iMapHeight = kMap.getGridHeight();
	const int iNumTerrainTypes = GC.getNumTerrainInfos();
	const TerrainTypes eCoast = (TerrainTypes)GC.getSHALLOW_WATER_TERRAIN();

	for(int y = 0; y < iMapHeight; ++y)
		for(int x = 0; x < iMapWidth; ++x)
		{
			CvPlot* pkPlot = kMap.plot(x, y);
			if(pkPlot != NULL)
			{
				const TerrainTypes eTerrainType = pkPlot->getTerrainType();
				if(eTerrainType >= 0 && eTerrainType < iNumTerrainTypes && eTerrainType != eCoast)
				{
					const CvTerrainInfo* pkTerrain = GC.getTerrainInfo(eTerrainType);
					if(pkTerrain != NULL && pkTerrain->isWater() && pkPlot->isAdjacentToLand())
					{
						pkPlot->setTerrainType(eCoast);
					}
				}
			}
		}

	//kTimer.Stop();
	//FStringFixedBuffer(sMsg, 64);
	//sMsg.Format("CvWorldBuilderMapLoader::ValidateCoast() took %fs\n", kTimer.m_fTimer);
	//OutputDebugStr(sMsg);
}

void CvWorldBuilderMapLoader::ClearResources()
{
	const uint uiPlotCount = sg_kSave.GetPlotCount();
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		CvWorldBuilderMap::PlotMapData& kPlot = sg_kSave.GetPlotData(i);
		kPlot.SetResourceType(CvWorldBuilderMap::PlotMapData::InvalidResource);
	}
}

void CvWorldBuilderMapLoader::ClearGoodies()
{
	const uint uiPlotCount = sg_kSave.GetPlotCount();
	for(uint i = 0; i < uiPlotCount; ++i)
	{
		CvWorldBuilderMap::PlotScenarioData& kPlot = sg_kSave.GetPlotScenarioData(i);
		if(kPlot.m_byImprovement != CvWorldBuilderMap::PlotScenarioData::InvalidImprovement)
		{
			ImprovementTypes eType = (ImprovementTypes)kPlot.m_byImprovement;
			const CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eType);
			if(pkImprovement == NULL || pkImprovement->IsGoody())
				kPlot.m_byImprovement = CvWorldBuilderMap::PlotScenarioData::InvalidImprovement;
		}
	}
}

WorldSizeTypes CvWorldBuilderMapLoader::GetCurrentWorldSizeType()
{
	return GetWorldSizeType(sg_kSave);
}

WorldSizeTypes CvWorldBuilderMapLoader::GetWorldSizeType(const CvWorldBuilderMap& kMap)
{
	WorldSizeTypes eWorldSize = NO_WORLDSIZE;
	const char* szWorldType = kMap.GetWorldType();
	if(szWorldType != NULL)
	{
		Database::Results kWorldSize;
		DB.Execute(kWorldSize, "Select ID from Worlds where Type = ? LIMIT 1");

		kWorldSize.Bind(1, szWorldType);
		if(kWorldSize.Step())
		{
			eWorldSize = (WorldSizeTypes)kWorldSize.GetInt(0);
		}
	}

	if(eWorldSize == NO_WORLDSIZE)
	{
		const int iArea = (int)(kMap.GetWidth() * kMap.GetHeight());
		int iSmallestAreaDifference = 64000; // Arbitrarily large at start

		Database::Results kWorldSizes;
		DB.SelectAll(kWorldSizes, "Worlds");
		while(kWorldSizes.Step())
		{
			CvWorldInfo kInfo;
			kInfo.CacheResult(kWorldSizes);

			int iSizeTypeArea = kInfo.getGridWidth() * kInfo.getGridHeight();
			int iAreaDifference = abs(iArea - iSizeTypeArea);
			if(iAreaDifference < iSmallestAreaDifference)
			{
				iSmallestAreaDifference = iAreaDifference;
				eWorldSize = (WorldSizeTypes)kInfo.GetID();
			}
		}
	}

	return eWorldSize;
}

void CvWorldBuilderMapLoader::ResetPlayerSlots()
{
	for(uint i = 0; i < MAX_CIV_PLAYERS; ++i)
		sg_auiPlayerSlots[i] = i;
}

void CvWorldBuilderMapLoader::MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot)
{
	if(uiPlayer < MAX_CIV_PLAYERS && ePlayerSlot >= 0 && ePlayerSlot < MAX_CIV_PLAYERS)
	{
		const uint uiCurrent = sg_auiPlayerSlots[ePlayerSlot];
		if(uiCurrent != uiPlayer)
		{
			PlayerTypes eOldSlot = NO_PLAYER;
			for(uint i = 0; i < MAX_CIV_PLAYERS; ++i)
			{
				if(sg_auiPlayerSlots[i] == uiPlayer)
				{
					eOldSlot = (PlayerTypes)i;
					break;
				}
			}

			sg_auiPlayerSlots[ePlayerSlot] = uiPlayer;

			FAssertMsg(eOldSlot != NO_PLAYER, "Player list has holes in it!")
			if(eOldSlot != NO_PLAYER)
				sg_auiPlayerSlots[eOldSlot] = uiCurrent;
		}
	}
}

PlayerTypes CvWorldBuilderMapLoader::GetMapPlayerSlot(uint uiPlayer)
{
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		if(sg_auiPlayerSlots[i] == uiPlayer)
			return static_cast<PlayerTypes>(i);
	}

	return NO_PLAYER;
}

int CvWorldBuilderMapLoader::GetMapPlayerCount()
{
	return MAX_CIV_PLAYERS;
}

uint CvWorldBuilderMapLoader::PreviewPlayableCivCount(const wchar_t* wszFilename)
{
	return CvWorldBuilderMap::PreviewPlayableCivCount(wszFilename);
}

static CvWorldBuilderMap sg_kTempMap;
static wchar_t sg_wszTempMapName[MAX_PATH] = {0};

bool CheckTempMap(const wchar_t* wszFilename)
{
	FAssert(wszFilename);

	if(wszFilename && *wszFilename)
	{
		return wcscmp(wszFilename, sg_wszTempMapName) == 0;
	}

	return false;
}

void TempMapLoaded(const wchar_t* wszFilename)
{
	wcscpy_s(sg_wszTempMapName, MAX_PATH, wszFilename);
}

int CvWorldBuilderMapLoader::GetMapPreview(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const char* szUTF8MapFile = luaL_checkstring(L, 1);

	wchar_t wszMapFile[MAX_PATH] = {0};
	MultiByteToWideChar(CP_UTF8, 0, szUTF8MapFile, -1, wszMapFile, MAX_PATH);

	if(CheckTempMap(wszMapFile) || sg_kTempMap.Load(wszMapFile, sg_kMapTypeDesc))
	{
		TempMapLoaded(wszMapFile);

		WorldSizeTypes eWorldSize = GetWorldSizeType(sg_kTempMap);

		const char* szDefaultSpeed = sg_kTempMap.GetDefaultGameSpeed();
		GameSpeedTypes eDefaultSpeed = (GameSpeedTypes)GC.getSTANDARD_GAMESPEED();
		const int iSpeedCount = GC.getNumGameSpeedInfos();
		for(int i = 0; i < iSpeedCount; ++i)
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes)i;
			const CvGameSpeedInfo* pkSpeed = GC.getGameSpeedInfo(eSpeed);
			if(pkSpeed != NULL && strcmp(pkSpeed->GetType(), szDefaultSpeed) == 0)
			{
				eDefaultSpeed = eSpeed;
				break;
			}
		}

		const char* szStartEra = "";
		for(int i = 0; i < sg_kTempMap.GetPlayerCount(); ++i)
		{
			const CvWorldBuilderMap::Player& kPlayer = sg_kTempMap.GetPlayer(i);
			if(kPlayer.m_bPlayable)
			{
				bool bFound = false;
				for(int iEra = 0; iEra < GC.getNumEraInfos(); ++iEra)
				{
					CvEraInfo* pkEraInfo = GC.getEraInfo((EraTypes)iEra);
					if(pkEraInfo != NULL && strcmp(pkEraInfo->GetType(), kPlayer.m_szEra) == 0)
					{
						szStartEra = pkEraInfo->GetText();
						bFound = true;
						break;
					}
				}

				if(bFound)
					break;
			}
		}

		lua_createtable(L, 0, 8);

		lua_pushstring(L, "MapSize");
		lua_pushinteger(L, eWorldSize);
		lua_rawset(L, -3);

		lua_pushstring(L, "Name");
		lua_pushstring(L, sg_kTempMap.m_sMapName);
		lua_rawset(L, -3);

		lua_pushstring(L, "Description");
		lua_pushstring(L, sg_kTempMap.m_sMapDescription);
		lua_rawset(L, -3);

		lua_pushstring(L, "PlayerCount");
		lua_pushnumber(L, sg_kTempMap.GetPlayerCount());
		lua_rawset(L, -3);

		lua_pushstring(L, "CityStateCount");
		lua_pushnumber(L, sg_kTempMap.GetCityStateCount());
		lua_rawset(L, -3);

		lua_pushstring(L, "MaxTurns");
		lua_pushnumber(L, sg_kTempMap.GetMaxTurns());
		lua_rawset(L, -3);

		lua_pushstring(L, "StartEra");
		lua_pushstring(L, szStartEra);
		lua_rawset(L, -3);

		lua_pushstring(L, "DefaultSpeed");
		lua_pushinteger(L, eDefaultSpeed);
		lua_rawset(L, -3);

		const int iVictoryTypes = GC.getNumVictoryInfos();
		lua_pushstring(L, "VictoryTypes");
		lua_createtable(L, iVictoryTypes, 0);
		for(int i = 0; i < iVictoryTypes; ++i)
		{
			const char* szVictoryType = "";
			const CvVictoryInfo* pkInfo = GC.getVictoryInfo((VictoryTypes)i);
			if(pkInfo != NULL)
				szVictoryType = pkInfo->GetType();

			lua_pushstring(L, szVictoryType);
			lua_rawseti(L, -2, i + 1);
		}
		lua_rawset(L, -3);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

int CvWorldBuilderMapLoader::GetMapPlayers(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const char* szUTF8MapFile = luaL_checkstring(L, 1);

	wchar_t wszMapFile[MAX_PATH] = {0};
	MultiByteToWideChar(CP_UTF8, 0, szUTF8MapFile, -1, wszMapFile, MAX_PATH);

	if(CheckTempMap(wszMapFile) || sg_kTempMap.Load(wszMapFile, sg_kMapTypeDesc))
	{
		TempMapLoaded(wszMapFile);

		const uint uiPlayerCount = sg_kTempMap.GetPlayerCount();
		const int iCivCount = GC.getNumCivilizationInfos();
		const int iHandicapCount = GC.getNumHandicapInfos();

		lua_createtable(L, uiPlayerCount, 0);
		for(uint uiPlayer = 0; uiPlayer < uiPlayerCount; ++uiPlayer)
		{
			const CvWorldBuilderMap::Player& kPlayer = sg_kTempMap.GetPlayer(uiPlayer);

			CivilizationTypes eCivType = NO_CIVILIZATION;
			for(int iCiv = 0; iCiv < iCivCount; ++iCiv)
			{
				const CivilizationTypes eCurrentCiv = (CivilizationTypes)iCiv;
				const CvCivilizationInfo* pkCiv = GC.getCivilizationInfo(eCurrentCiv);
				if(pkCiv != NULL && strcmp(pkCiv->GetType(), kPlayer.m_szCivType) == 0)
				{
					eCivType = eCurrentCiv;
					break;
				}
			}

			HandicapTypes eHandicapType = (HandicapTypes)GC.getSTANDARD_HANDICAP();
			for(int iHandicap = 0; iHandicap < iHandicapCount; ++iHandicap)
			{
				const HandicapTypes eCurrentHandicap = (HandicapTypes)iHandicap;
				const CvHandicapInfo* pkHandicap = GC.getHandicapInfo(eCurrentHandicap);
				if(pkHandicap != NULL && strcmp(pkHandicap->GetType(), kPlayer.m_szHandicap) == 0)
				{
					eHandicapType = eCurrentHandicap;
					break;
				}
			}

			lua_createtable(L, 0, 2);

			lua_pushstring(L, "CivType");
			lua_pushinteger(L, eCivType);
			lua_rawset(L, -3);

			lua_pushstring(L, "Playable");
			lua_pushboolean(L, kPlayer.m_bPlayable);
			lua_rawset(L, -3);

			lua_pushstring(L, "DefaultHandicap");
			lua_pushinteger(L, eHandicapType);
			lua_rawset(L, -3);

			lua_rawseti(L, -2, uiPlayer + 1);
		}
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

int CvWorldBuilderMapLoader::AddRandomItems(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const int iTop = lua_gettop(L);

	const bool bRandomGoodies =
	    sg_kSave.GetFlag(CvWorldBuilderMap::FLAG_RANDOM_GOODIES) ||
	    (sg_kSave.GetFlag(CvWorldBuilderMap::FLAG_OLD_GOODIE_RULES) && !CvPreGame::loadWBScenario());

	const bool bRandomResources = sg_kSave.GetFlag(CvWorldBuilderMap::FLAG_RANDOM_RECOURCES);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem != NULL && (bRandomResources || bRandomGoodies))
	{
		const char* szLuaFile = "WorldBuilderRandomItems.lua";
		const bool bLoadedMapGenerator = pkScriptSystem->LoadFile(L, szLuaFile);
		FAssertMsg1(bLoadedMapGenerator, "Failed to load %s", szLuaFile);
		if(bLoadedMapGenerator)
		{
			if(bRandomGoodies)
			{
				const char* szGoodiesFunction = "AddGoodies";
				lua_getglobal(L, szGoodiesFunction);
				if(lua_isfunction(L, -1))
					pkScriptSystem->CallFunction(L, 0, 0);
				else
					FAssertMsg2(0, "Failed to find \"%s\" in %s", szGoodiesFunction, szLuaFile);
			}

			if(bRandomResources)
			{
				const char* szResourcesFunction = "AddResourcesForWorldBuilderMap";
				lua_getglobal(L, szResourcesFunction);
				if(lua_isfunction(L, -1))
					pkScriptSystem->CallFunction(L, 0, 0);
				else
					FAssertMsg2(0, "Failed to find \"%s\" in %s", szResourcesFunction, szLuaFile);
			}
		}
	}

	lua_settop(L, iTop);

	return 0;
}

int CvWorldBuilderMapLoader::ScatterResources(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const int iTop = lua_gettop(L);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem != NULL)
	{
		const bool bLoadedMapGenerator = pkScriptSystem->LoadFile(L, "WorldBuilderRandomItems.lua");
		FAssertMsg(bLoadedMapGenerator, "Failed to load WorldBuilderRandomItems.lua");
		if(bLoadedMapGenerator)
		{
			lua_getglobal(L, "AddResourcesForWorldBuilderMap");
			if(lua_isfunction(L, -1))
				pkScriptSystem->CallFunction(L, 0, 0);
			else
				FAssertMsg(0, "Failed to find \"AddResourcesForWorldBuilderMap\" in WorldBuilderRandomItems.lua");
		}
	}

	lua_settop(L, iTop);

	return 0;
}

int CvWorldBuilderMapLoader::ScatterGoodies(lua_State* L)
{
	if(L == NULL)
	{
		FAssertMsg(L, "Seriously, you really need a lua state for this.");
		return 0;
	}

	const int iTop = lua_gettop(L);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem != NULL)
	{
		const bool bLoadedMapGenerator = pkScriptSystem->LoadFile(L, "WorldBuilderRandomItems.lua");
		FAssertMsg(bLoadedMapGenerator, "Failed to load WorldBuilderRandomItems.lua");
		if(bLoadedMapGenerator)
		{
			lua_getglobal(L, "AddGoodies");
			if(lua_isfunction(L, -1))
				pkScriptSystem->CallFunction(L, 0, 0);
			else
				FAssertMsg(0, "Failed to find \"AddGoodies\" in WorldBuilderRandomItems.lua");
		}
	}

	lua_settop(L, iTop);

	return 0;
}
