﻿/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvGlobals.h

#ifndef CIV5_GLOBALS_H
#define CIV5_GLOBALS_H

#include <vector>

//
// 'global' vars for Civ V.  singleton class.
// All globals and global types should be contained in this class
// Author -	Mustafa Thamer
//			Jon Shafer - 03/2005
//
class CvRandom;
class CvGame;
class CvStepFinder;
class CvTwoLayerPathFinder;
class CvInterface;
class CvEngine;
class CvMap;
class CvPlayerAI;
class CvTeam;
class CvInterfaceModeInfo;
class CvColorInfo;
class CvPlayerColorInfo;
class CvEntityEventInfo;
class CvLandscapeInfo;
#if defined(MOD_API_PLOT_YIELDS)
class CvPlotInfo;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
class CvGreatPersonInfo;
#endif
class CvTerrainInfo;
class CvResourceClassInfo;
class CvResourceInfo;
class CvFeatureInfo;
class CvCivilizationInfo;
class CvMinorCivInfo;
class CvLeaderHeadInfo;
class CvSpecialUnitInfo;
class CvYieldInfo;
class CvRouteInfo;
class CvImprovementInfo;
class CvBuildInfo;
class CvHandicapInfo;
class CvGameSpeedInfo;
class CvTurnTimerInfo;
class CvProcessInfo;
class CvVoteInfo;
class CvBuildingClassInfo;
class CvUnitClassInfo;
class CvActionInfo;
class CvMissionInfo;
class CvControlInfo;
class CvCommandInfo;
class CvAutomateInfo;
class CvSpecialistInfo;
class CvEraInfo;
class CvHurryInfo;
class CvVictoryInfo;
class CvSmallAwardInfo;
class CvGameOptionInfo;
class CvMPOptionInfo;
class CvPlayerOptionInfo;
class CvEventTriggerInfo;
class CvEventInfo;
class CvVoteSourceInfo;
#if defined(MOD_BALANCE_CORE_EVENTS)
class CvModEventInfo;
class CvModEventChoiceInfo;
class CvModCityEventInfo;
class CvModEventCityChoiceInfo;

class CvEventLinkingInfo;
class CvEventChoiceLinkingInfo;
class CvCityEventLinkingInfo;
class CvCityEventChoiceLinkingInfo;
#endif
class CvMultiUnitFormationInfo;
class CvEconomicAIStrategyXMLEntries;
class CvEconomicAIStrategyXMLEntry;
class CvCitySpecializationXMLEntries;
class CvCitySpecializationXMLEntry;
class CvTacticalMoveXMLEntries;
class CvTacticalMoveXMLEntry;
class CvMilitaryAIStrategyXMLEntries;
class CvMilitaryAIStrategyXMLEntry;
class CvAIGrandStrategyXMLEntries;
class CvAIGrandStrategyXMLEntry;
class CvAICityStrategies;
class CvAICityStrategyEntry;
class CvPolicyXMLEntries;
class CvPolicyEntry;
class CvPolicyBranchEntry;
class CvTechXMLEntries;
class CvTechEntry;
class CvBuildingEntry;
class CvBuildingXMLEntries;
class CvUnitEntry;
class CvUnitXMLEntries;
class CvProjectEntry;
class CvProjectXMLEntries;
class CvPromotionEntry;
class CvPromotionXMLEntries;
class CvImprovementEntry;
class CvImprovementXMLEntries;
class CvEmphasisEntry;
class CvEmphasisXMLEntries;
class CvTraitEntry;
class CvTraitXMLEntries;
class CvNotificationEntry;
class CvNotificationXMLEntries;
#if defined(MOD_API_ACHIEVEMENTS) || defined(ACHIEVEMENT_HACKS)
class CvAchievementInfo;
class CvAchievementXMLEntries;
#endif
class CvBaseInfo;
class CvReplayInfo;
class CvReligion;
class CvReligionEntry;
class CvReligionXMLEntries;
class CvBeliefEntry;
class CvBeliefXMLEntries;
class CvLeagueSpecialSessionEntry;
class CvLeagueSpecialSessionXMLEntries;
class CvLeagueNameEntry;
class CvLeagueNameXMLEntries;
class CvLeagueProjectEntry;
class CvLeagueProjectXMLEntries;
class CvLeagueProjectRewardEntry;
class CvLeagueProjectRewardXMLEntries;
class CvResolutionEntry;
class CvResolutionXMLEntries;
class CvDeal;
class CvNetMessageHandler;
#if defined(MOD_BALANCE_CORE)
class CvCorporationEntry;
class CvCorporationXMLEntries;
class CvContractEntry;
class CvContractXMLEntries;
#endif

class CvDLLInterfaceIFaceBase;
class ICvDLLDatabaseUtility1;
class ICvEngineUtility1;
class ICvEngineUtility2;
class ICvScriptSystemUtility1;
class ICvCity1;
class ICvDeal1;
class ICvGameContext1;
class ICvPlayer1;
class ICvPlot1;
class ICvRandom1;
class ICvUnit1;


class CvGlobals
{
public:

	typedef stdext::hash_map<std::string /* type string */, int /* info index */> InfosMap;
	typedef std::map<uint /* FString::HashType */, int /* info index */> InfosHashMap;

	// singleton accessor
	static CvGlobals& getInstance();

	CvGlobals();
	virtual ~CvGlobals();

	//DLL Utility Methods
	CvCity* UnwrapCityPointer(ICvCity1* pCity);
	auto_ptr<ICvCity1> WrapCityPointer(CvCity* pCity);

	CvDeal* UnwrapDealPointer(ICvDeal1* pDeal);
	auto_ptr<ICvDeal1> WrapDealPointer(CvDeal* pDeal);

	CvPlot* UnwrapPlotPointer(ICvPlot1* pPlot);
	auto_ptr<ICvPlot1> WrapPlotPointer(CvPlot* pPlot);

	CvRandom* UnwrapRandomPointer(ICvRandom1* pRandom);
	auto_ptr<ICvRandom1> WrapRandomPointer(CvRandom* pRandom);

	auto_ptr<ICvUnit1> WrapUnitPointer(CvUnit* pUnit);
	CvUnit*   UnwrapUnitPointer(ICvUnit1* pUnit);

	void init();
	void uninit();

	CvMap& getMap()
	{
		return *m_map;    // inlined for perf reasons, do not use outside of dll
	}
	CvMap* getMapPointer()
	{
		return m_map;
	}
	CvGame& getGame()
	{
		return *m_game;    // inlined for perf reasons, do not use outside of dll
	}
	CvGame* getGamePointer()
	{
		return m_game;
	}
	int* getPlotDirectionX()
	{
		return m_aiPlotDirectionX;
	}
	int* getPlotDirectionY()
	{
		return m_aiPlotDirectionY;
	}

	CvRandom& getASyncRand();

	void InitializePathfinders(int iX, int iY, bool bWx, bool bWy);
	CvTwoLayerPathFinder& GetPathFinder();
	CvStepFinder& GetStepFinder();

	ICvDLLDatabaseUtility1* getDatabaseLoadUtility();

	std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfo();
	CvInterfaceModeInfo* getInterfaceModeInfo(InterfaceModeTypes e);

	bool getLogging();
	void setLogging(bool bEnable);

	int getRandLogging();
	void setRandLogging(int iRandLoggingFlags);

	bool getAILogging();
	void setAILogging(bool bEnable);

	bool getAIPerfLogging();
	void setAIPerfLogging(bool bEnable);

	bool GetBuilderAILogging();
	void SetBuilderAILogging(bool bEnable);

	bool getPlayerAndCityAILogSplit();
	void setPlayerAndCityAILogSplit(bool bEnable);

	bool GetTutorialLogging();
	void SetTutorialLogging(bool bEnable);

	bool GetTutorialDebugging();
	void SetTutorialDebugging(bool bEnable);

	bool GetAllowRClickMovementWhileScrolling();
	void SetAllowRClickMovementWhileScrolling(bool bAllow);

	bool GetPostTurnAutosaves();
	void SetPostTurnAutosaves(bool bEnable);

	void LogMessage(const char* szMessage);

	inline int* getCityPlotX()
	{
		return m_aiRingPlotXOffset;
	}
	inline int* getCityPlotY()
	{
		return m_aiRingPlotYOffset;
	}

	int* getCityPlotPriority();
	int getRingIterationIndexHex(int i, int j);

	DirectionTypes* getTurnLeftDirection();
	DirectionTypes getTurnLeftDirection(int i);
	DirectionTypes* getTurnRightDirection();
	DirectionTypes getTurnRightDirection(int i);

	DirectionTypes getXYDirection(int i, int j, int stagger);

	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//
	void SetGameDatabase(Database::Connection* pGameDatabase);
	Database::Connection* GetGameDatabase();
	const Database::Connection* GetGameDatabase() const;

	const InfosMap& GetInfoTypes() const;
	int getInfoTypeForString(const char* szType, bool hideAssert = false) const;			// returns the infos index, use this when searching for an info type string
	int getInfoTypeForHash(uint uiHash, bool hideAssert = false) const;			// returns the infos index, use this when searching for an info type string
	void setInfoTypeFromString(const char* szType, int idx);
	void infoTypeFromStringReset();
	void infosReset();

	int getNumWorldInfos();
	int getNumClimateInfos();
	int getNumSeaLevelInfos();

	int GetNumColorInfos();
	std::vector<CvColorInfo*>& GetColorInfo();
	_Ret_maybenull_ CvColorInfo* GetColorInfo(ColorTypes e);

	int GetNumPlayerColorInfos();
	std::vector<CvPlayerColorInfo*>& GetPlayerColorInfo();
	_Ret_maybenull_ CvPlayerColorInfo* GetPlayerColorInfo(PlayerColorTypes e);

	int getNumEntityEventInfos();
	std::vector<CvEntityEventInfo*>& getEntityEventInfo();
	_Ret_maybenull_ CvEntityEventInfo* getEntityEventInfo(EntityEventTypes e);

	int getNumMultiUnitFormationInfos();
	std::vector<CvMultiUnitFormationInfo*>& getMultiUnitFormationInfo();
	_Ret_maybenull_ CvMultiUnitFormationInfo* getMultiUnitFormationInfo(int i);

#if defined(MOD_API_PLOT_YIELDS)
	int getNumPlotInfos();
	std::vector<CvPlotInfo*>& getPlotInfo();
	CvPlotInfo* getPlotInfo(PlotTypes ePlotNum);
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	int getNumGreatPersonInfos();
	std::vector<CvGreatPersonInfo*>& getGreatPersonInfo();
	CvGreatPersonInfo* getGreatPersonInfo(GreatPersonTypes eGreatPersonNum);
#endif

	int getNumTerrainInfos();
	std::vector<CvTerrainInfo*>& getTerrainInfo();
	CvTerrainInfo* getTerrainInfo(TerrainTypes eTerrainNum);

	int getNumResourceClassInfos();
	std::vector<CvResourceClassInfo*>& getResourceClassInfo();
	_Ret_maybenull_ CvResourceClassInfo* getResourceClassInfo(ResourceClassTypes eResourceNum);

	int getNumResourceInfos();
	std::vector<CvResourceInfo*>& getResourceInfo();
	CvResourceInfo* getResourceInfo(ResourceTypes eResourceNum);

	int getNumFeatureInfos();
	std::vector<CvFeatureInfo*>& getFeatureInfo();
	CvFeatureInfo* getFeatureInfo(FeatureTypes eFeatureNum);

	int& getNumPlayableCivilizationInfos();
	int& getNumAIPlayableCivilizationInfos();
	int getNumCivilizationInfos();
	std::vector<CvCivilizationInfo*>& getCivilizationInfo();
	_Ret_maybenull_ CvCivilizationInfo* getCivilizationInfo(CivilizationTypes eCivilizationNum);
	CivilizationTypes getCivilizationInfoIndex(const char* pszType);

	int getNumMinorCivInfos();
	int GetNumPlayableMinorCivs() const;
	std::vector<CvMinorCivInfo*>& getMinorCivInfo();
	CvMinorCivInfo* getMinorCivInfo(MinorCivTypes eMinorCivNum);

	int getNumLeaderHeadInfos();
	std::vector<CvLeaderHeadInfo*>& getLeaderHeadInfo();
	_Ret_maybenull_ CvLeaderHeadInfo* getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum);

	int getNumUnitInfos();
	std::vector<CvUnitEntry*>& getUnitInfo();
	_Ret_maybenull_ CvUnitEntry* getUnitInfo(UnitTypes eUnitNum);
	CvUnitXMLEntries* GetGameUnits() const;

	int getNumSpecialUnitInfos();
	std::vector<CvSpecialUnitInfo*>& getSpecialUnitInfo();
	_Ret_maybenull_ CvSpecialUnitInfo* getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum);

	int getNumVoteSourceInfos();
	std::vector<CvVoteSourceInfo*>& getVoteSourceInfo();
	_Ret_maybenull_ CvVoteSourceInfo* getVoteSourceInfo(VoteSourceTypes e);

#if defined(MOD_BALANCE_CORE_EVENTS)
	int getNumEventInfos();
	std::vector<CvModEventInfo*>& getEventInfo();
	_Ret_maybenull_ CvModEventInfo* getEventInfo(EventTypes e);

	int getNumEventChoiceInfos();
	std::vector<CvModEventChoiceInfo*>& getEventChoiceInfo();
	_Ret_maybenull_ CvModEventChoiceInfo* getEventChoiceInfo(EventChoiceTypes e);

	int getNumCityEventInfos();
	std::vector<CvModCityEventInfo*>& getCityEventInfo();
	_Ret_maybenull_ CvModCityEventInfo* getCityEventInfo(CityEventTypes e);

	int getNumCityEventChoiceInfos();
	std::vector<CvModEventCityChoiceInfo*>& getCityEventChoiceInfo();
	_Ret_maybenull_ CvModEventCityChoiceInfo* getCityEventChoiceInfo(CityEventChoiceTypes e);

	int getNumEventLinkingInfos();
	std::vector<CvEventLinkingInfo*>& getEventLinkingInfo();
	_Ret_maybenull_ CvEventLinkingInfo* getEventLinkingInfo(EventTypes e);

	int getNumEventChoiceLinkingInfos();
	std::vector<CvEventChoiceLinkingInfo*>& getEventChoiceLinkingInfo();
	_Ret_maybenull_ CvEventChoiceLinkingInfo* getEventChoiceLinkingInfo(EventChoiceTypes e);

	int getNumCityEventLinkingInfos();
	std::vector<CvCityEventLinkingInfo*>& getCityEventLinkingInfo();
	_Ret_maybenull_ CvCityEventLinkingInfo* getCityEventLinkingInfo(CityEventTypes e);

	int getNumCityEventChoiceLinkingInfos();
	std::vector<CvCityEventChoiceLinkingInfo*>& getCityEventChoiceLinkingInfo();
	_Ret_maybenull_ CvCityEventChoiceLinkingInfo* getCityEventChoiceLinkingInfo(CityEventChoiceTypes e);
	
#endif

	int getNumUnitCombatClassInfos();
	std::vector<CvBaseInfo*>& getUnitCombatClassInfo();
	_Ret_maybenull_ CvBaseInfo* getUnitCombatClassInfo(UnitCombatTypes e);

	std::vector<CvBaseInfo*>& getUnitAIInfo();
	_Ret_maybenull_ CvBaseInfo* getUnitAIInfo(UnitAITypes eUnitAINum);

	int getNumGameOptionInfos();
	std::vector<CvGameOptionInfo*>& getGameOptionInfo();
	_Ret_maybenull_ CvGameOptionInfo* getGameOptionInfo(GameOptionTypes eGameOptionNum);

	int getNumMPOptionInfos();
	std::vector<CvMPOptionInfo*>& getMPOptionInfo();
	_Ret_maybenull_ CvMPOptionInfo* getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum);

	std::vector<CvPlayerOptionInfo*>& getPlayerOptionInfo();
	_Ret_maybenull_ CvPlayerOptionInfo* getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum);

	std::vector<CvYieldInfo*>& getYieldInfo();
	_Ret_maybenull_ CvYieldInfo* getYieldInfo(YieldTypes eYieldNum);

	int getNumRouteInfos();
	std::vector<CvRouteInfo*>& getRouteInfo();
	_Ret_maybenull_ CvRouteInfo* getRouteInfo(RouteTypes eRouteNum);

	int getNumImprovementInfos();
	std::vector<CvImprovementEntry*>& getImprovementInfo();
	CvImprovementEntry* getImprovementInfo(ImprovementTypes eImprovementNum);
	CvImprovementXMLEntries* GetGameImprovements() const;

	int getNumBuildInfos();
	std::vector<CvBuildInfo*>& getBuildInfo();
	CvBuildInfo* getBuildInfo(BuildTypes eBuildNum);

	int getNumHandicapInfos();
	std::vector<CvHandicapInfo*>& getHandicapInfo();
	_Ret_maybenull_ CvHandicapInfo* getHandicapInfo(HandicapTypes eHandicapNum);

	int getNumGameSpeedInfos();
	std::vector<CvGameSpeedInfo*>& getGameSpeedInfo();
	_Ret_maybenull_ CvGameSpeedInfo* getGameSpeedInfo(GameSpeedTypes eGameSpeedNum);

#if defined(MOD_EVENTS_DIPLO_MODIFIERS)
	int getNumDiploModifierInfos();
	std::vector<CvDiploModifierInfo*>& getDiploModifierInfo();
	_Ret_maybenull_ CvDiploModifierInfo* getDiploModifierInfo(DiploModifierTypes eDiploModifierNum);
#endif

	int getNumProcessInfos();
	std::vector<CvProcessInfo*>& getProcessInfo();
	CvProcessInfo* getProcessInfo(ProcessTypes e);

	int getNumVoteInfos();
	std::vector<CvVoteInfo*>& getVoteInfo();
	CvVoteInfo* getVoteInfo(VoteTypes e);

	int getNumProjectInfos();
	std::vector<CvProjectEntry*>& getProjectInfo();
	CvProjectEntry* getProjectInfo(ProjectTypes e);
	CvProjectXMLEntries* GetGameProjects() const;

	int getNumBuildingClassInfos();
	std::vector<CvBuildingClassInfo*>& getBuildingClassInfo();
	_Ret_maybenull_ CvBuildingClassInfo* getBuildingClassInfo(BuildingClassTypes eBuildingClassNum);

	int getNumBuildingInfos();
	std::vector<CvBuildingEntry*>& getBuildingInfo();
	_Ret_maybenull_ CvBuildingEntry* getBuildingInfo(BuildingTypes eBuildingNum);
	CvBuildingXMLEntries* GetGameBuildings() const;

	int getNumUnitClassInfos();
	std::vector<CvUnitClassInfo*>& getUnitClassInfo();
	_Ret_maybenull_ CvUnitClassInfo* getUnitClassInfo(UnitClassTypes eUnitClassNum);

	int getNumActionInfos();
	std::vector<CvActionInfo*>& getActionInfo();
	CvActionInfo* getActionInfo(int i);

	std::vector<CvMissionInfo*>& getMissionInfo();
	_Ret_maybenull_ CvMissionInfo* getMissionInfo(MissionTypes eMissionNum);

	std::vector<CvControlInfo*>& getControlInfo();
	_Ret_maybenull_ CvControlInfo* getControlInfo(ControlTypes eControlNum);

	std::vector<CvCommandInfo*>& getCommandInfo();
	_Ret_maybenull_ CvCommandInfo* getCommandInfo(CommandTypes eCommandNum);

	int getNumAutomateInfos();
	std::vector<CvAutomateInfo*>& getAutomateInfo();
	_Ret_maybenull_ CvAutomateInfo* getAutomateInfo(int iAutomateNum);

	int getNumPromotionInfos();
	std::vector<CvPromotionEntry*>& getPromotionInfo();
	_Ret_maybenull_ CvPromotionEntry* getPromotionInfo(PromotionTypes ePromotionNum);
	CvPromotionXMLEntries* GetGamePromotions() const;

	int getNumSpecialistInfos();
	std::vector<CvSpecialistInfo*>& getSpecialistInfo();
	_Ret_maybenull_ CvSpecialistInfo* getSpecialistInfo(SpecialistTypes eSpecialistNum);

	int getNumEconomicAIStrategyInfos();
	std::vector<CvEconomicAIStrategyXMLEntry*>& getEconomicAIStrategyInfo();
	_Ret_maybenull_ CvEconomicAIStrategyXMLEntry* getEconomicAIStrategyInfo(EconomicAIStrategyTypes eAIStrategyNum);
	CvEconomicAIStrategyXMLEntries* GetGameEconomicAIStrategies() const;

	int getNumCitySpecializationInfos();
	std::vector<CvCitySpecializationXMLEntry*>& getCitySpecializationInfo();
	_Ret_maybenull_ CvCitySpecializationXMLEntry* getCitySpecializationInfo(CitySpecializationTypes eCitySpecialization);
	CvCitySpecializationXMLEntries* GetGameCitySpecializations() const;

	int getNumTacticalMoveInfos();
	std::vector<CvTacticalMoveXMLEntry*>& getTacticalMoveInfo();
	_Ret_maybenull_ CvTacticalMoveXMLEntry* getTacticalMoveInfo(TacticalAIMoveTypes eTacticalMove);
	CvTacticalMoveXMLEntries* GetGameTacticalMoves() const;

	int getNumMilitaryAIStrategyInfos();
	std::vector<CvMilitaryAIStrategyXMLEntry*>& getMilitaryAIStrategyInfo();
	_Ret_maybenull_ CvMilitaryAIStrategyXMLEntry* getMilitaryAIStrategyInfo(MilitaryAIStrategyTypes eAIStrategyNum);
	CvMilitaryAIStrategyXMLEntries* GetGameMilitaryAIStrategies() const;

	int getNumAIGrandStrategyInfos();
	std::vector<CvAIGrandStrategyXMLEntry*>& getAIGrandStrategyInfo();
	_Ret_maybenull_ CvAIGrandStrategyXMLEntry* getAIGrandStrategyInfo(AIGrandStrategyTypes eAIGrandStrategyNum);
	CvAIGrandStrategyXMLEntries* GetGameAIGrandStrategies() const;

	int getNumAICityStrategyInfos();
	std::vector<CvAICityStrategyEntry*>& getAICityStrategyInfo();
	_Ret_maybenull_ CvAICityStrategyEntry* getAICityStrategyInfo(AICityStrategyTypes eAICityStrategyNum);
	CvAICityStrategies* GetGameAICityStrategies() const;

	int getNumPolicyInfos();
	std::vector<CvPolicyEntry*>& getPolicyInfo();
	_Ret_maybenull_ CvPolicyEntry* getPolicyInfo(PolicyTypes ePolicyNum);
	CvPolicyXMLEntries* GetGamePolicies() const;

	int getNumPolicyBranchInfos();
	std::vector<CvPolicyBranchEntry*>& getPolicyBranchInfo();
	_Ret_maybenull_ CvPolicyBranchEntry* getPolicyBranchInfo(PolicyBranchTypes ePolicyBranchNum);

	int getNumEmphasisInfos();
	std::vector<CvEmphasisEntry*>& getEmphasisInfo();
	_Ret_maybenull_ CvEmphasisEntry* getEmphasisInfo(EmphasizeTypes eEmphasisNum);
	CvEmphasisXMLEntries* GetGameEmphases() const;

	int getNumTraitInfos();
	std::vector<CvTraitEntry*>& getTraitInfo();
	_Ret_maybenull_ CvTraitEntry* getTraitInfo(TraitTypes eTraitNum);
	CvTraitXMLEntries* GetGameTraits() const;

	int getNumReligionInfos();
	std::vector<CvReligionEntry*>& getReligionInfo();
	_Ret_maybenull_ CvReligionEntry* getReligionInfo(ReligionTypes eReligionNum);
	CvReligionXMLEntries* GetGameReligions() const;

#if defined(MOD_BALANCE_CORE)
	int getNumCorporationInfos();
	std::vector<CvCorporationEntry*>& getCorporationInfo();
	_Ret_maybenull_ CvCorporationEntry* getCorporationInfo(CorporationTypes eCorporationNum);
	CvCorporationXMLEntries* GetGameCorporations() const;

	int getNumContractInfos();
	std::vector<CvContractEntry*>& getContractInfo();
	_Ret_maybenull_ CvContractEntry* getContractInfo(ContractTypes eContract);
	CvContractXMLEntries* GetGameContracts() const;
#endif

	int getNumBeliefInfos();
	std::vector<CvBeliefEntry*>& getBeliefInfo();
	_Ret_maybenull_ CvBeliefEntry* getBeliefInfo(BeliefTypes eBeliefNum);
	CvBeliefXMLEntries* GetGameBeliefs() const;

	int getNumLeagueSpecialSessionInfos();
	std::vector<CvLeagueSpecialSessionEntry*>& getLeagueSpecialSessionInfo();
	_Ret_maybenull_ CvLeagueSpecialSessionEntry* getLeagueSpecialSessionInfo(LeagueSpecialSessionTypes eLeagueSpecialSessionNum);
	CvLeagueSpecialSessionXMLEntries* GetGameLeagueSpecialSessions() const;
	
	int getNumLeagueNameInfos();
	std::vector<CvLeagueNameEntry*>& getLeagueNameInfo();
	_Ret_maybenull_ CvLeagueNameEntry* getLeagueNameInfo(LeagueNameTypes eLeagueNameNum);
	CvLeagueNameXMLEntries* GetGameLeagueNames() const;
	
	int getNumLeagueProjectInfos();
	std::vector<CvLeagueProjectEntry*>& getLeagueProjectInfo();
	_Ret_maybenull_ CvLeagueProjectEntry* getLeagueProjectInfo(LeagueProjectTypes eLeagueProjectNum);
	CvLeagueProjectXMLEntries* GetGameLeagueProjects() const;

	int getNumLeagueProjectRewardInfos();
	std::vector<CvLeagueProjectRewardEntry*>& getLeagueProjectRewardInfo();
	_Ret_maybenull_ CvLeagueProjectRewardEntry* getLeagueProjectRewardInfo(LeagueProjectRewardTypes eLeagueProjectRewardNum);
	CvLeagueProjectRewardXMLEntries* GetGameLeagueProjectRewards() const;

	int getNumResolutionInfos();
	std::vector<CvResolutionEntry*>& getResolutionInfo();
	_Ret_maybenull_ CvResolutionEntry* getResolutionInfo(ResolutionTypes eResolutionNum);
	CvResolutionXMLEntries* GetGameResolutions() const;

	int getNumTechInfos();
	std::vector<CvTechEntry*>& getTechInfo();
	_Ret_maybenull_ CvTechEntry* getTechInfo(TechTypes eTechNum);
	CvTechXMLEntries* GetGameTechs() const;

	int getNumEraInfos();
	std::vector<CvEraInfo*>& getEraInfo();
	CvEraInfo* getEraInfo(EraTypes eEraNum);

	int getNumHurryInfos();
	std::vector<CvHurryInfo*>& getHurryInfo();
	_Ret_maybenull_ CvHurryInfo* getHurryInfo(HurryTypes eHurryNum);

	int getNumVictoryInfos();
	std::vector<CvVictoryInfo*>& getVictoryInfo();
	_Ret_maybenull_ CvVictoryInfo* getVictoryInfo(VictoryTypes eVictoryNum);

	int getNumSmallAwardInfos();
	std::vector<CvSmallAwardInfo*>& getSmallAwardInfo();
	_Ret_maybenull_ CvSmallAwardInfo* getSmallAwardInfo(SmallAwardTypes eSmallAwardNum);

	CvNotificationXMLEntries* GetNotificationEntries();
#if defined(MOD_API_ACHIEVEMENTS) || defined(ACHIEVEMENT_HACKS)
	int getNumAchievementInfos();
	std::vector<CvAchievementInfo*>& getAchievementInfo();
	_Ret_maybenull_ CvAchievementInfo* getAchievementInfo(EAchievement eAchievementNum);
	CvAchievementXMLEntries* GetGameAchievements() const;
#endif

	//
	// Global Types
	// All type strings are upper case and are kept in this hash map for fast lookup
	// The other functions are kept for convenience when enumerating, but most are not used
	//
	int getNUM_YIELD_TYPES() const;
	int getNUM_CONTROL_TYPES() const;

	// int& getNumFlavorTypes();
	// CvString*& getFlavorTypes();
	// CvString& getFlavorTypes(FlavorTypes e);
	void       setNumFlavorTypes(const int iTypes)
	{
		m_iNumFlavorTypes = iTypes;
	}
	int        getNumFlavorTypes()
	{
		return m_iNumFlavorTypes;
	}
	CvString*& getFlavorTypes()
	{
		return m_paszFlavorTypes;
	}
	CvString&  getFlavorTypes(FlavorTypes e)
	{
		CvAssert(e > -1); /*CvAssert(e < GC.getNumFlavorTypes())*/;
		return m_paszFlavorTypes[e];
	}

	int getNumUnitDomainInfos();
	std::vector<CvDomainInfo*>& getUnitDomainInfo();
	_Ret_maybenull_ CvDomainInfo* getUnitDomainInfo(DomainTypes eDomainNum);

	CvString*& getFootstepAudioTags();
	CvString& getFootstepAudioTags(int i);

	const char** GetHexDebugLayerNames();
	float GetHexDebugLayerScale(const char* szLayerName);
	bool GetHexDebugLayerString(CvPlot* pkPlot, const char* szLayerName, PlayerTypes ePlayer, char* szBuffer, unsigned int uiBufferLength);

	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//
	void cacheGlobals();

	// use very sparingly - this is costly
	CvString getDefineSTRING(const char* szName, bool bReportErrors = true);

	// -- ints --

	inline int getAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD()
	{
		return m_iAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD;
	}
	inline int getBARBARIAN_UNIT_GOLD_RANSOM_exp()
	{
		return m_iBARBARIAN_UNIT_GOLD_RANSOM;
	}
	inline int getINITIAL_AI_CITY_PRODUCTION()
	{
		return m_iINITIAL_AI_CITY_PRODUCTION;
	}
	inline int getAI_CAN_DISBAND_UNITS()
	{
		return m_iAI_CAN_DISBAND_UNITS;
	}
	inline int getAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT()
	{
		return m_iAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT;
	}
	inline int getPOLICY_WEIGHT_PROPAGATION_PERCENT()
	{
		return m_iPOLICY_WEIGHT_PROPAGATION_PERCENT;
	}
	inline int getPOLICY_WEIGHT_PROPAGATION_LEVELS()
	{
		return m_iPOLICY_WEIGHT_PROPAGATION_LEVELS;
	}
	inline int getPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH()
	{
		return m_iPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH;
	}
	inline int getTECH_WEIGHT_PROPAGATION_PERCENT()
	{
		return m_iTECH_WEIGHT_PROPAGATION_PERCENT;
	}
	inline int getTECH_WEIGHT_PROPAGATION_LEVELS()
	{
		return m_iTECH_WEIGHT_PROPAGATION_LEVELS;
	}
	inline int getTECH_PRIORITY_UNIQUE_ITEM()
	{
		return m_iTECH_PRIORITY_UNIQUE_ITEM;
	}
	inline int getTECH_PRIORITY_MAYA_CALENDAR_BONUS()
	{
		return m_iTECH_PRIORITY_MAYA_CALENDAR_BONUS;
	}
	inline int getAI_VALUE_OF_YIELD_GOLD()
	{
		return m_iAI_VALUE_OF_YIELD_GOLD;
	}
	inline int getAI_VALUE_OF_YIELD_PRODUCTION()
	{
		return m_iAI_VALUE_OF_YIELD_PRODUCTION;
	}
	inline int getAI_VALUE_OF_YIELD_SCIENCE()
	{
		return m_iAI_VALUE_OF_YIELD_SCIENCE;
	}
	inline int getDEFAULT_FLAVOR_VALUE()
	{
		return m_iDEFAULT_FLAVOR_VALUE;
	}
	inline int getPERSONALITY_FLAVOR_MAX_VALUE()
	{
		return m_iPERSONALITY_FLAVOR_MAX_VALUE;
	}
	inline int getPERSONALITY_FLAVOR_MIN_VALUE()
	{
		return m_iPERSONALITY_FLAVOR_MIN_VALUE;
	}
	inline int getFLAVOR_MIN_VALUE()
	{
		return m_iFLAVOR_MIN_VALUE;
	}
	inline int getFLAVOR_MAX_VALUE()
	{
		return m_iFLAVOR_MAX_VALUE;
	}
	inline int getFLAVOR_RANDOMIZATION_RANGE()
	{
		return m_iFLAVOR_RANDOMIZATION_RANGE;
	}
	inline int getFLAVOR_EXPANDGROW_COEFFICIENT()
	{
		return m_iFLAVOR_EXPANDGROW_COEFFICIENT;
	}
	inline int getAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE()
	{
		return m_iAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE;
	}
	inline int getAI_GS_RAND_ROLL()
	{
		return m_iAI_GS_RAND_ROLL;
	}
	inline int getAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD;
	}
	inline int getAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER()
	{
		return m_iAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER;
	}
	inline int getAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN()
	{
		return m_iAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT;
	}
	inline int getAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN()
	{
		return m_iAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED;
	}
	inline int getAI_GS_CULTURE_RATIO_MULTIPLIER()
	{
		return m_iAI_GS_CULTURE_RATIO_MULTIPLIER;
	}
	inline int getAI_GS_TOURISM_RATIO_MULTIPLIER()
	{
		return m_iAI_GS_TOURISM_RATIO_MULTIPLIER;
	}
	inline int getAI_GS_CULTURE_AHEAD_WEIGHT()
	{
		return m_iAI_GS_CULTURE_AHEAD_WEIGHT;
	}
	inline int getAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT()
	{
		return m_iAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT;
	}
	inline int getAI_GS_CULTURE_INFLUENTIAL_CIV_MOD()
	{
		return m_iAI_GS_CULTURE_INFLUENTIAL_CIV_MOD;
	}
	inline int getAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT;
	}
	inline int getAI_GS_UN_SECURED_VOTE_MOD()
	{
		return m_iAI_GS_UN_SECURED_VOTE_MOD;
	}
	inline int getAI_GS_SS_HAS_APOLLO_PROGRAM()
	{
		return m_iAI_GS_SS_HAS_APOLLO_PROGRAM;
	}
	inline int getAI_GS_SS_TECH_PROGRESS_MOD()
	{
		return m_iAI_GS_SS_TECH_PROGRESS_MOD;
	}
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	inline int getEVENT_MIN_DURATION_BETWEEN()
	{
		return m_iEVENT_MIN_DURATION_BETWEEN;
	}
	inline int getBALANCE_HAPPINESS_THRESHOLD_PERCENTILE()
	{
		return m_iBALANCE_HAPPINESS_THRESHOLD_PERCENTILE;
	}
	inline int getGLOBAL_RESOURCE_MONOPOLY_THRESHOLD()
	{
		return m_iGLOBAL_RESOURCE_MONOPOLY_THRESHOLD;
	}
	inline int getSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD()
	{
		return m_iSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD;
	}
	inline int getRELIGION_MIN_FAITH_SECOND_PROPHET()
	{
		return m_iRELIGION_MIN_FAITH_SECOND_PROPHET;
	}
#endif
	inline int getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT;
	}
	inline int getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR;
	}
	inline int getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR;
	}
#if defined(MOD_BALANCE_CORE_SETTLER)
	inline int getAI_STRATEGY_EARLY_EXPANSION_RELATIVE_TILE_QUALITY()
	{
		return m_iAI_STRATEGY_EARLY_EXPANSION_RELATIVE_TILE_QUALITY;
	}
#else
	inline int getAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED()
	{
		return m_iAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED;
	}
#endif
	inline int getAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT()
	{
		return m_iAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT;
	}
	inline float getAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO()
	{
		return m_fAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO;
	}
	inline int getAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT()
	{
		return m_iAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT;
	}
	inline int getAI_STRATEGY_AREA_IS_FULL_PERCENT()
	{
		return m_iAI_STRATEGY_AREA_IS_FULL_PERCENT;
	}
	inline int getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY()
	{
		return m_iAI_STRATEGY_MINIMUM_SETTLE_FERTILITY;
	}
	inline int getAI_BUY_PLOT_TEST_PROBES()
	{
		return m_iAI_BUY_PLOT_TEST_PROBES;
	}
	inline int getAI_PLOT_VALUE_STRATEGIC_RESOURCE()
	{
		return m_iAI_PLOT_VALUE_STRATEGIC_RESOURCE;
	}
	inline int getAI_PLOT_VALUE_LUXURY_RESOURCE()
	{
		return m_iAI_PLOT_VALUE_LUXURY_RESOURCE;
	}
	inline int getAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_YIELD_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_YIELD_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_FIERCE_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_FIERCE_DISPUTE;
	}
	inline int getAI_PLOT_VALUE_STRONG_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_STRONG_DISPUTE;
	}
	inline int getAI_PLOT_VALUE_WEAK_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_WEAK_DISPUTE;
	}
	inline int getAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE()
	{
		return m_iAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE;
	}
	inline int getAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM()
	{
		return m_iAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM;
	}
	inline int getAI_GOLD_PRIORITY_UPGRADE_BASE()
	{
		return m_iAI_GOLD_PRIORITY_UPGRADE_BASE;
	}
	inline int getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT()
	{
		return m_iAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT;
	}
	inline int getAI_GOLD_PRIORITY_DIPLOMACY_BASE()
	{
		return m_iAI_GOLD_PRIORITY_DIPLOMACY_BASE;
	}
	inline int getAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT()
	{
		return m_iAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT;
	}
	inline int getAI_GOLD_PRIORITY_UNIT()
	{
		return m_iAI_GOLD_PRIORITY_UNIT;
	}
	inline int getAI_GOLD_PRIORITY_DEFENSIVE_BUILDING()
	{
		return m_iAI_GOLD_PRIORITY_DEFENSIVE_BUILDING;
	}
	inline int getAI_GOLD_PRIORITY_BUYOUT_CITY_STATE()
	{
		return m_iAI_GOLD_PRIORITY_BUYOUT_CITY_STATE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE;
	}
	inline int getBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY()
	{
		return m_iBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY;
	}
	inline int getBUILDER_TASKING_BUILD_ROUTE_TENDENCY()
	{
		return m_iBUILDER_TASKING_BUILD_ROUTE_TENDENCY;
	}
	inline int getBUILDER_TASKING_DIRECTIVES_TO_EVALUATE()
	{
		return m_iBUILDER_TASKING_DIRECTIVES_TO_EVALUATE;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_ROUTES()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_ROUTES;
	}
	inline int getBUILDER_TASKING_BASELINE_REPAIR()
	{
		return m_iBUILDER_TASKING_BASELINE_REPAIR;
	}
	inline int getBUILDER_TASKING_BASELINE_SCRUB_FALLOUT()
	{
		return m_iBUILDER_TASKING_BASELINE_SCRUB_FALLOUT;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_CULTURE()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_CULTURE;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_MINOR()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_MINOR;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_MAJOR()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_MAJOR;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_SEVERE()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_SEVERE;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_CRITICAL()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_CRITICAL;
	}
	inline int getAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS()
	{
		return m_iAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS;
	}
	inline int getAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE()
	{
		return m_iAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE;
	}
	inline int getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()
	{
		return m_iAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL;
	}
	inline int getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT()
	{
		return m_iAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT;
	}
	inline int getAI_MILITARY_IGNORE_BAD_ODDS()
	{
		return m_iAI_MILITARY_IGNORE_BAD_ODDS;
	}
	inline int getAI_MILITARY_RECAPTURING_OWN_CITY()
	{
		return m_iAI_MILITARY_RECAPTURING_OWN_CITY;
	}
	inline int getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL()
	{
		return m_iAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL;
	}
	inline int getAI_CITY_SPECIALIZATION_EARLIEST_TURN()
	{
		return m_iAI_CITY_SPECIALIZATION_EARLIEST_TURN;
	}
	inline int getAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL()
	{
		return m_iAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL;
	}
	inline int getAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT()
	{
		return m_iAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY;
	}
	inline int getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD()
	{
		return m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD;
	}
	inline int getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE()
	{
		return m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE;
	}
	inline int getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE()
	{
		return m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE;
	}
	inline int getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP()
	{
		return m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED;
	}
	inline int getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN;
	}
	inline int getAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE()
	{
		return m_iAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE;
	}
	inline int getAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT()
	{
		return m_iAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT;
	}
	inline int getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER()
	{
		return m_iAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER;
	}
	inline int getAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT()
	{
		return m_iAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT;
	}
	inline int getAI_CITIZEN_VALUE_FOOD()
	{
		return m_iAI_CITIZEN_VALUE_FOOD;
	}
	inline int getAI_CITIZEN_VALUE_PRODUCTION()
	{
		return m_iAI_CITIZEN_VALUE_PRODUCTION;
	}
	inline int getAI_CITIZEN_VALUE_GOLD()
	{
		return m_iAI_CITIZEN_VALUE_GOLD;
	}
	inline int getAI_CITIZEN_VALUE_SCIENCE()
	{
		return m_iAI_CITIZEN_VALUE_SCIENCE;
	}
	inline int getAI_CITIZEN_VALUE_CULTURE()
	{
		return m_iAI_CITIZEN_VALUE_CULTURE;
	}
	inline int getAI_CITIZEN_VALUE_FAITH()
	{
		return m_iAI_CITIZEN_VALUE_FAITH;
	}
	inline int getAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF()
	{
		return m_iAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF;
	}
	inline int getAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT()
	{
		return m_iAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT;
	}
	inline int getAI_CITIZEN_MOD_FOOD_DEFICIT()
	{
		return m_iAI_CITIZEN_MOD_FOOD_DEFICIT;
	}
	inline int getAI_CITIZEN_MOD_PRODUCTION_DEFICIT()
	{
		return m_iAI_CITIZEN_MOD_PRODUCTION_DEFICIT;
	}
	inline int getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER()
	{
		return m_iAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER;
	}
	inline int getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()
	{
		return m_iAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION;
	}
	inline int getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER()
	{
		return m_iAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER;
	}
	inline int getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE()
	{
		return m_iAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE;
	}
	inline int getAI_TACTICAL_MAP_TEMP_ZONE_RADIUS()
	{
		return m_iAI_TACTICAL_MAP_TEMP_ZONE_RADIUS;
	}
	inline int getAI_TACTICAL_MAP_TEMP_ZONE_TURNS()
	{
		return m_iAI_TACTICAL_MAP_TEMP_ZONE_TURNS;
	}
	inline int getAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS()
	{
		return m_iAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS;
	}
	inline int getAI_TACTICAL_RECRUIT_RANGE()
	{
		return m_iAI_TACTICAL_RECRUIT_RANGE;
	}
	inline int getAI_TACTICAL_REPOSITION_RANGE()
	{
		return m_iAI_TACTICAL_REPOSITION_RANGE;
	}
	inline int getAI_TACTICAL_OVERKILL_PERCENT()
	{
		return m_iAI_TACTICAL_OVERKILL_PERCENT;
	}
	inline int getAI_TACTICAL_BARBARIAN_RELEASE_VARIATION()
	{
		return m_iAI_TACTICAL_BARBARIAN_RELEASE_VARIATION;
	}
	inline int getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS()
	{
		return m_iAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS;
	}
	inline int getAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS()
	{
		return m_iAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS;
	}
	inline int getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS()
	{
		return m_iAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS;
	}
	inline int getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE()
	{
		return m_iAI_HOMELAND_ESTIMATE_TURNS_DISTANCE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SETTLE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SETTLE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_HEAL()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_HEAL;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WORKER()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WORKER;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_EXPLORE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SENTRY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SENTRY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_PATROL()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_PATROL;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_UPGRADE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_UPGRADE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WRITER()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WRITER;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ARTIST()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ARTIST;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MUSICIAN()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MUSICIAN;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	inline int getAI_HOMELAND_MOVE_PRIORITY_DIPLOMAT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_DIPLOMAT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MESSENGER()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MESSENGER;
	}
#endif
	inline int getAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MISSIONARY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MISSIONARY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_INQUISITOR()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_INQUISITOR;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TREASURE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TREASURE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_AIRLIFT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_AIRLIFT;
	}
	inline int getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT()
	{
		return m_iAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT;
	}
	inline int getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT()
	{
		return m_iAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE;
	}
#if defined(MOD_CONFIG_AI_IN_XML)
	GD_INT_DEF(AI_CONFIG_MILITARY_MELEE_PER_AA)
	GD_INT_DEF(AI_CONFIG_MILITARY_AIRCRAFT_PER_CARRIER_SPACE)
	GD_INT_DEF(AI_CONFIG_MILITARY_TILES_PER_SHIP)
	GD_INT_DEF(AI_HOMELAND_MOVE_PRIORITY_SECONDARY_SETTLER)
	GD_INT_DEF(AI_HOMELAND_MOVE_PRIORITY_SECONDARY_WORKER)
#endif
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DEF(RELIGION_LAST_FOUND_ERA)
	GD_INT_DEF(RELIGION_GP_FAITH_PURCHASE_ERA)
	GD_INT_DEF(IDEOLOGY_START_ERA)
	GD_INT_DEF(IDEOLOGY_PREREQ_ERA)
#endif
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	GD_INT_DEF(TOURISM_START_TECH)
	GD_INT_DEF(TOURISM_START_ERA)
#endif
	inline int getMINOR_BULLY_GOLD()
	{
		return m_iMINOR_BULLY_GOLD;
	}
	inline int getMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM()
	{
		return m_iMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM;
	}
	inline int getMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION()
	{
		return m_iMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR;
	}
	inline int getMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE;
	}
	inline int getMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN()
	{
		return m_iMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN;
	}
	inline int getMINOR_FRIENDSHIP_AT_WAR()
	{
		return m_iMINOR_FRIENDSHIP_AT_WAR;
	}
	inline int getMINOR_CIV_AGGRESSOR_THRESHOLD()
	{
		return m_iMINOR_CIV_AGGRESSOR_THRESHOLD;
	}
	inline int getMINOR_CIV_WARMONGER_THRESHOLD()
	{
		return m_iMINOR_CIV_WARMONGER_THRESHOLD;
	}
	inline int getPERMANENT_WAR_AGGRESSOR_CHANCE()
	{
		return m_iPERMANENT_WAR_AGGRESSOR_CHANCE;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_DISTANT()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_DISTANT;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_FAR()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_FAR;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_CLOSE()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_CLOSE;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS;
	}
	inline int getPERMANENT_WAR_OTHER_AT_WAR()
	{
		return m_iPERMANENT_WAR_OTHER_AT_WAR;
	}
	inline int getFRIENDSHIP_NEUTRAL_ON_DEATH()
	{
		return m_iFRIENDSHIP_NEUTRAL_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_NEUTRAL()
	{
		return m_iFRIENDSHIP_THRESHOLD_NEUTRAL;
	}
	inline int getFRIENDSHIP_FRIENDS_ON_DEATH()
	{
		return m_iFRIENDSHIP_FRIENDS_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_FRIENDS()
	{
		return m_iFRIENDSHIP_THRESHOLD_FRIENDS;
	}
	inline int getFRIENDSHIP_ALLIES_ON_DEATH()
	{
		return m_iFRIENDSHIP_ALLIES_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_ALLIES()
	{
		return m_iFRIENDSHIP_THRESHOLD_ALLIES;
	}
	inline int getFRIENDSHIP_THRESHOLD_MAX()
	{
		return m_iFRIENDSHIP_THRESHOLD_MAX;
	}
	inline int getFRIENDSHIP_THRESHOLD_CAN_BULLY()
	{
		return m_iFRIENDSHIP_THRESHOLD_CAN_BULLY;
	}
	inline int getFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT()
	{
		return m_iFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT;
	}
#if defined(MOD_CITY_STATE_SCALE)
	GD_INT_DEF(FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL)
	GD_INT_DEF(FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL)
#endif
	inline int getMINOR_FRIENDSHIP_CLOSE_AMOUNT()
	{
		return m_iMINOR_FRIENDSHIP_CLOSE_AMOUNT;
	}
	inline int getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER()
	{
		return m_iMINOR_CIV_SCIENCE_BONUS_MULTIPLIER;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE()
	{
		return m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	}
	inline int getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE()
	{
		return m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	}
	inline int getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE()
	{
		return m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	}
	inline int getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE()
	{
		return m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	}
	inline int getALLIES_CAPITAL_FOOD_BONUS_AMOUNT()
	{
		return m_iALLIES_CAPITAL_FOOD_BONUS_AMOUNT;
	}
	inline int getALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT()
	{
		return m_iALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT;
	}
	inline int getFRIENDS_BASE_TURNS_UNIT_SPAWN()
	{
		return m_iFRIENDS_BASE_TURNS_UNIT_SPAWN;
	}
	inline int getFRIENDS_RAND_TURNS_UNIT_SPAWN()
	{
		return m_iFRIENDS_RAND_TURNS_UNIT_SPAWN;
	}
	inline int getALLIES_EXTRA_TURNS_UNIT_SPAWN()
	{
		return m_iALLIES_EXTRA_TURNS_UNIT_SPAWN;
	}
	inline int getUNIT_SPAWN_BIAS_MULTIPLIER()
	{
		return m_iUNIT_SPAWN_BIAS_MULTIPLIER;
	}
	inline int getUNIT_SPAWN_NUM_CHOICES()
	{
		return m_iUNIT_SPAWN_NUM_CHOICES;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY;
	}
	inline int getMINOR_ADDITIONAL_ALLIES_GP_CHANGE()
	{
		return m_iMINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	}
	inline int getMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE()
	{
		return m_iMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	}
	inline int getMAX_DISTANCE_MINORS_BARB_QUEST()
	{
		return m_iMAX_DISTANCE_MINORS_BARB_QUEST;
	}
	inline int getTXT_KEY_MINOR_GIFT_UNITS_REMINDER()
	{
		return m_iTXT_KEY_MINOR_GIFT_UNITS_REMINDER;
	}
	inline int getWAR_QUEST_COMPLETE_FRIENDSHIP()
	{
		return m_iWAR_QUEST_COMPLETE_FRIENDSHIP;
	}
	inline int getWAR_QUEST_UNITS_TO_KILL_DIVISOR()
	{
		return m_iWAR_QUEST_UNITS_TO_KILL_DIVISOR;
	}
	inline int getWAR_QUEST_MIN_UNITS_TO_KILL()
	{
		return m_iWAR_QUEST_MIN_UNITS_TO_KILL;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_ROUTE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_ROUTE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_KILL_CAMP()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_KILL_CAMP;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_GREAT_PERSON()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_GREAT_PERSON;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_FIND_PLAYER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_FIND_PLAYER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_GIVE_GOLD()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_GIVE_GOLD;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_INVEST()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_INVEST;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	inline int getMINOR_QUEST_FRIENDSHIP_WAR()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_WAR;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_FIND_CITY_STATE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_FIND_CITY_STATE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_INFLUENCE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_INFLUENCE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_TOURISM()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_TOURISM;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_ARCHAEOLOGY()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_ARCHAEOLOGY;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CIRCUMNAVIGATION()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CIRCUMNAVIGATION;
	}
//Ideology Resistance Stuff
	inline int getIDEOLOGY_PER_CITY_UNHAPPY()
	{
	return m_iIDEOLOGY_PER_CITY_UNHAPPY;
	}
	inline int getIDEOLOGY_POP_PER_UNHAPPY()
	{
	return m_iIDEOLOGY_POP_PER_UNHAPPY;
	}
//World War
	inline int getWARMONGER_THREAT_PER_TURN_DECAY_INCREASED()
	{
	return m_iWARMONGER_THREAT_PER_TURN_DECAY_INCREASED;
	}
	inline int getWARMONGER_THREAT_PER_TURN_DECAY_DECREASED()
	{
	return m_iWARMONGER_THREAT_PER_TURN_DECAY_DECREASED;
	}
	inline int getWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_WAR()
	{
	return m_iWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_WAR;
	}
	inline int getWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE()
	{
	return m_iWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE;
	}
	inline int getWARMONGER_THREAT_DECREASED_WORLD_WAR()
	{
	return m_iWARMONGER_THREAT_DECREASED_WORLD_WAR;
	}
	inline int getSCHOLAR_MINOR_ALLY_MULTIPLIER()
	{
	return m_iSCHOLAR_MINOR_ALLY_MULTIPLIER;
	}
	inline int getSCIENCE_LEAGUE_GREAT_WORK_MODIFIER()
	{
	return m_iSCIENCE_LEAGUE_GREAT_WORK_MODIFIER;
	}
	inline int getPIONEER_POPULATION_CHANGE()
	{
	return m_iPIONEER_POPULATION_CHANGE;
	}
	inline int getPIONEER_EXTRA_PLOTS()
	{
	return m_iPIONEER_EXTRA_PLOTS;
	}
	inline int getPIONEER_FOOD_PERCENT()
	{
	return m_iPIONEER_FOOD_PERCENT;
	}
	inline int getCOLONIST_POPULATION_CHANGE()
	{
	return m_iCOLONIST_POPULATION_CHANGE;
	}
	inline int getCOLONIST_EXTRA_PLOTS()
	{
	return m_iCOLONIST_EXTRA_PLOTS;
	}
	inline int getCOLONIST_FOOD_PERCENT()
	{
	return m_iCOLONIST_FOOD_PERCENT;
	}
	inline int getMINOR_QUEST_REBELLION_TIMER()
	{
	return m_iMINOR_QUEST_REBELLION_TIMER;
	}
#endif
	inline int getMINOR_QUEST_STANDARD_CONTEST_LENGTH()
	{
		return m_iMINOR_QUEST_STANDARD_CONTEST_LENGTH;
	}
	inline int getMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER()
	{
		return m_iMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER;
	}
	inline int getMINOR_CIV_GOLD_GIFT_GAME_DIVISOR()
	{
		return m_iMINOR_CIV_GOLD_GIFT_GAME_DIVISOR;
	}
	inline int getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR()
	{
		return m_iMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR;
	}
	inline int getMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD()
	{
		return m_iMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD;
	}
	inline int getMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS()
	{
		return m_iMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS;
	}
	inline int getMINOR_CIV_RESOURCE_SEARCH_RADIUS()
	{
		return m_iMINOR_CIV_RESOURCE_SEARCH_RADIUS;
	}
	inline int getFRIENDSHIP_PER_UNIT_INTRUDING()
	{
		return m_iFRIENDSHIP_PER_UNIT_INTRUDING;
	}
	inline int getFRIENDSHIP_PER_BARB_KILLED()
	{
		return m_iFRIENDSHIP_PER_BARB_KILLED;
	}
	inline int getFRIENDSHIP_PER_UNIT_GIFTED()
	{
		return m_iFRIENDSHIP_PER_UNIT_GIFTED;
	}
	inline int getMAX_INFLUENCE_FROM_MINOR_GIFTS()
	{
		return m_iMAX_INFLUENCE_FROM_MINOR_GIFTS;
	}
	inline int getMINOR_LIBERATION_FRIENDSHIP()
	{
		return m_iMINOR_LIBERATION_FRIENDSHIP;
	}
	inline int getRETURN_CIVILIAN_FRIENDSHIP()
	{
		return m_iRETURN_CIVILIAN_FRIENDSHIP;
	}
	inline int getMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER()
	{
		return m_iMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER;
	}
	inline int getMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER()
	{
		return m_iMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	}
	inline int getMINOR_CIV_QUEST_KILL_CAMP_RANGE()
	{
		return m_iMINOR_CIV_QUEST_KILL_CAMP_RANGE;
	}
	inline int getMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD()
	{
		return m_iMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD;
	}
	inline int getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD()
	{
		return m_iMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD;
	}
	inline int getMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY()
	{
		return m_iMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY;
	}
	inline int getMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED()
	{
		return m_iMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED;
	}
	inline int getQUEST_DISABLED_ROUTE()
	{
		return m_iQUEST_DISABLED_ROUTE;
	}
	inline int getQUEST_DISABLED_KILL_CAMP()
	{
		return m_iQUEST_DISABLED_KILL_CAMP;
	}
	inline int getQUEST_DISABLED_CONNECT_RESOURCE()
	{
		return m_iQUEST_DISABLED_CONNECT_RESOURCE;
	}
	inline int getQUEST_DISABLED_CONSTRUCT_WONDER()
	{
		return m_iQUEST_DISABLED_CONSTRUCT_WONDER;
	}
	inline int getQUEST_DISABLED_GREAT_PERSON()
	{
		return m_iQUEST_DISABLED_GREAT_PERSON;
	}
	inline int getQUEST_DISABLED_KILL_CITY_STATE()
	{
		return m_iQUEST_DISABLED_KILL_CITY_STATE;
	}
	inline int getQUEST_DISABLED_FIND_PLAYER()
	{
		return m_iQUEST_DISABLED_FIND_PLAYER;
	}
	inline int getQUEST_DISABLED_NATURAL_WONDER()
	{
		return m_iQUEST_DISABLED_NATURAL_WONDER;
	}
	inline int getQUEST_DISABLED_GIVE_GOLD()
	{
		return m_iQUEST_DISABLED_GIVE_GOLD;
	}
	inline int getQUEST_DISABLED_PLEDGE_TO_PROTECT()
	{
		return m_iQUEST_DISABLED_PLEDGE_TO_PROTECT;
	}
	inline int getQUEST_DISABLED_CONTEST_CULTURE()
	{
		return m_iQUEST_DISABLED_CONTEST_CULTURE;
	}
	inline int getQUEST_DISABLED_CONTEST_FAITH()
	{
		return m_iQUEST_DISABLED_CONTEST_FAITH;
	}
	inline int getQUEST_DISABLED_CONTEST_TECHS()
	{
		return m_iQUEST_DISABLED_CONTEST_TECHS;
	}
	inline int getQUEST_DISABLED_INVEST()
	{
		return m_iQUEST_DISABLED_INVEST;
	}
	inline int getQUEST_DISABLED_BULLY_CITY_STATE()
	{
		return m_iQUEST_DISABLED_BULLY_CITY_STATE;
	}
	inline int getQUEST_DISABLED_DENOUNCE_MAJOR()
	{
		return m_iQUEST_DISABLED_DENOUNCE_MAJOR;
	}
	inline int getQUEST_DISABLED_SPREAD_RELIGION()
	{
		return m_iQUEST_DISABLED_SPREAD_RELIGION;
	}
	inline int getQUEST_DISABLED_TRADE_ROUTE()
	{
		return m_iQUEST_DISABLED_TRADE_ROUTE;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	inline int getQUEST_DISABLED_WAR()
	{
		return m_iQUEST_DISABLED_WAR;
	}
	inline int getQUEST_DISABLED_FIND_CITY_STATE()
	{
		return m_iQUEST_DISABLED_FIND_CITY_STATE;
	}
	inline int getQUEST_DISABLED_INFLUENCE()
	{
		return m_iQUEST_DISABLED_INFLUENCE;
	}
	inline int getQUEST_DISABLED_TOURISM()
	{
		return m_iQUEST_DISABLED_TOURISM;
	}
	inline int getQUEST_DISABLED_CIRCUMNAVIGATION()
	{
		return m_iQUEST_DISABLED_CIRCUMNAVIGATION;
	}
	inline int getQUEST_DISABLED_ARCHAEOLOGY()
	{
		return m_iQUEST_DISABLED_ARCHAEOLOGY;
	}
	inline int getQUEST_DISABLED_LIBERATION()
	{
		return m_iQUEST_DISABLED_LIBERATION;
	}
		inline int getQUEST_DISABLED_HORDE()
	{
		return m_iQUEST_DISABLED_HORDE;
	}
	inline int getQUEST_DISABLED_REBELLION()
	{
		return m_iQUEST_DISABLED_REBELLION;
	}
#endif
#if defined(MOD_BALANCE_CORE)
	inline int getUNITCLASS_FOR_CS_BULLY()
	{
		return m_iUNITCLASS_FOR_CS_BULLY;
	}
	inline int getQUEST_DISABLED_CP_QUESTS()
	{
		return m_iQUEST_DISABLED_CP_QUESTS;
	}
#endif
	inline int getMINOR_CIV_QUEST_WEIGHT_DEFAULT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_DEFAULT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//War on Major
	inline int getMINOR_CIV_QUEST_WAR_MILITARISTIC_VALUE()
	{
		return m_iMINOR_CIV_QUEST_WAR_MILITARISTIC_VALUE;
	}
	inline int getMINOR_CIV_QUEST_WAR_HOSTILE_VALUE()
	{
		return m_iMINOR_CIV_QUEST_WAR_HOSTILE_VALUE;
	}
	inline int getMINOR_CIV_QUEST_WAR_CULTURED_VALUE()
	{
		return m_iMINOR_CIV_QUEST_WAR_CULTURED_VALUE;
	}
//National Wonder
	inline int getMINOR_CIV_QUEST_NATIONAL_WONDER_CULTURED_VALUE()
	{
		return m_iMINOR_CIV_QUEST_NATIONAL_WONDER_CULTURED_VALUE;
	}
//Find Minor
	inline int getMINOR_CIV_QUEST_FIND_CITYSTATE_MARITIME_VALUE()
	{
		return m_iMINOR_CIV_QUEST_FIND_CITYSTATE_MARITIME_VALUE;
	}
	inline int getMINOR_CIV_QUEST_FIND_CITYSTATE_MERCANTILE_VALUE()
	{
		return m_iMINOR_CIV_QUEST_FIND_CITYSTATE_MERCANTILE_VALUE;
	}
//Archaeology
		inline int getMINOR_CIV_QUEST_ARCHAEOLOGY_MILITARISTIC_VALUE()
	{
		return m_iMINOR_CIV_QUEST_ARCHAEOLOGY_MILITARISTIC_VALUE;
	}
	inline int getMINOR_CIV_QUEST_ARCHAEOLOGY_RELIGIOUS_VALUE()
	{
		return m_iMINOR_CIV_QUEST_ARCHAEOLOGY_RELIGIOUS_VALUE;
	}
//Circumnavigation
	inline int getMINOR_CIV_QUEST_CIRCUMNAVIGATION_MARITIME_VALUE()
	{
		return m_iMINOR_CIV_QUEST_CIRCUMNAVIGATION_MARITIME_VALUE;
	}
//Influence
	inline int getMINOR_CIV_QUEST_INFLUENCE_HOSTILE_VALUE()
	{
		return m_iMINOR_CIV_QUEST_INFLUENCE_HOSTILE_VALUE;
	}
//Tourism
	inline int getMINOR_CIV_QUEST_TOURISM_FRIENDLY_VALUE()
	{
		return m_iMINOR_CIV_QUEST_TOURISM_FRIENDLY_VALUE;
	}
//Liberation
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION;
	}
//Distance Weight for Diplomatic Missions
	inline int getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE()
	{
		return m_iINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE;
	}
//Weight for War if Given War Quest
	inline int getAPPROACH_WAR_MINOR_QUEST_WAR()
	{
		return m_iAPPROACH_WAR_MINOR_QUEST_WAR;
	}

//Weight for City Target if Liberation Quest Active
	inline int getAI_MILITARY_RECAPTURING_CITY_STATE()
	{
		return m_iAI_MILITARY_RECAPTURING_CITY_STATE;
	}
	inline int getNEED_DIPLOMAT_THRESHOLD_MODIFIER()
	{
		return m_iNEED_DIPLOMAT_THRESHOLD_MODIFIER;
	}
	inline int getNEED_DIPLOMAT_DESIRE_MODIFIER()
	{
		return m_iNEED_DIPLOMAT_DESIRE_MODIFIER;
	}
	inline int getNEED_DIPLOMAT_DISTASTE_MODIFIER()
	{
		return m_iNEED_DIPLOMAT_DISTASTE_MODIFIER;
	}
		inline int getINFLUENCE_MINOR_QUEST_BOOST()
	{
		return m_iINFLUENCE_MINOR_QUEST_BOOST;
	}
	inline int getQUEST_REBELLION_FREQUENCY()
	{
		return m_iQUEST_REBELLION_FREQUENCY;
	}
	inline int getBARBARIAN_HORDE_FREQUENCY()
	{
		return m_iBARBARIAN_HORDE_FREQUENCY;
	}
	inline int getLEAGUE_AID_MAX()
	{
		return m_iLEAGUE_AID_MAX;
	}
	inline int getCSD_GOLD_GIFT_DISABLED()
	{
		return m_iCSD_GOLD_GIFT_DISABLED;
	}
	inline int getCSD_DIPLO_BUILDING_YIELD()
	{
		return m_iCSD_DIPLO_BUILDING_YIELD;
	}
	inline int getBALANCE_GAME_DIFFICULTY_MULTIPLIER()
	{
		return m_iBALANCE_GAME_DIFFICULTY_MULTIPLIER;
	}
#endif
	inline int getRELIGION_BELIEF_SCORE_CITY_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_CITY_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_MISSIONARY_RANGE_IN_TURNS()
	{
		return m_iRELIGION_MISSIONARY_RANGE_IN_TURNS;
	}
	inline int getRELIGION_MAX_MISSIONARIES()
	{
		return m_iRELIGION_MAX_MISSIONARIES;
	}
	inline float getNORMAL_ANNEX()
	{
		return m_fNORMAL_ANNEX;
	}
	inline float getAGGRESIVE_ANNEX()
	{
		return m_fAGGRESSIVE_ANNEX;
	}
	inline int getMC_GIFT_WEIGHT_THRESHOLD()
	{
		return m_iMC_GIFT_WEIGHT_THRESHOLD;
	}
	inline int getMC_ALWAYS_GIFT_DIPLO_THRESHOLD()
	{
		return m_iMC_ALWAYS_GIFT_DIPLO_THRESHOLD;
	}
	inline int getMC_SOMETIMES_GIFT_RAND_MULTIPLIER()
	{
		return m_iMC_SOMETIMES_GIFT_RAND_MULTIPLIER;
	}
	inline int getMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER()
	{
		return m_iMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	}
	inline int getMC_GIFT_WEIGHT_PASS_OTHER_PLAYER()
	{
		return m_iMC_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	}
	inline int getMC_GIFT_WEIGHT_MARITIME_GROWTH()
	{
		return m_iMC_GIFT_WEIGHT_MARITIME_GROWTH;
	}
	inline int getMC_GIFT_WEIGHT_DIPLO_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_DIPLO_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_CULTURE_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_CULTURE_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_CONQUEST_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_CONQUEST_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_MILITARISTIC()
	{
		return m_iMC_GIFT_WEIGHT_MILITARISTIC;
	}
	inline int getMC_GIFT_WEIGHT_RESOURCE_WE_NEED()
	{
		return m_iMC_GIFT_WEIGHT_RESOURCE_WE_NEED;
	}
	inline int getMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL()
	{
		return m_iMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL;
	}
	inline int getMC_GIFT_WEIGHT_PROTECTIVE()
	{
		return m_iMC_GIFT_WEIGHT_PROTECTIVE;
	}
	inline int getMC_GIFT_WEIGHT_HOSTILE()
	{
		return m_iMC_GIFT_WEIGHT_HOSTILE;
	}
	inline int getMC_GIFT_WEIGHT_NEIGHBORS()
	{
		return m_iMC_GIFT_WEIGHT_NEIGHBORS;
	}
	inline int getMC_GIFT_WEIGHT_CLOSE()
	{
		return m_iMC_GIFT_WEIGHT_CLOSE;
	}
	inline int getMC_GIFT_WEIGHT_FAR()
	{
		return m_iMC_GIFT_WEIGHT_FAR;
	}
	inline int getMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES()
	{
		return m_iMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES;
	}
	inline int getMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS()
	{
		return m_iMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS;
	}
	inline int getOPINION_WEIGHT_LAND_FIERCE()
	{
		return m_iOPINION_WEIGHT_LAND_FIERCE;
	}
	inline int getOPINION_WEIGHT_LAND_STRONG()
	{
		return m_iOPINION_WEIGHT_LAND_STRONG;
	}
	inline int getOPINION_WEIGHT_LAND_WEAK()
	{
		return m_iOPINION_WEIGHT_LAND_WEAK;
	}
	inline int getOPINION_WEIGHT_LAND_NONE()
	{
		return m_iOPINION_WEIGHT_LAND_NONE;
	}
	inline int getOPINION_WEIGHT_VICTORY_FIERCE()
	{
		return m_iOPINION_WEIGHT_VICTORY_FIERCE;
	}
	inline int getOPINION_WEIGHT_VICTORY_STRONG()
	{
		return m_iOPINION_WEIGHT_VICTORY_STRONG;
	}
	inline int getOPINION_WEIGHT_VICTORY_WEAK()
	{
		return m_iOPINION_WEIGHT_VICTORY_WEAK;
	}
	inline int getOPINION_WEIGHT_VICTORY_NONE()
	{
		return m_iOPINION_WEIGHT_VICTORY_NONE;
	}
	inline int getOPINION_WEIGHT_WONDER_FIERCE()
	{
		return m_iOPINION_WEIGHT_WONDER_FIERCE;
	}
	inline int getOPINION_WEIGHT_WONDER_STRONG()
	{
		return m_iOPINION_WEIGHT_WONDER_STRONG;
	}
	inline int getOPINION_WEIGHT_WONDER_WEAK()
	{
		return m_iOPINION_WEIGHT_WONDER_WEAK;
	}
	inline int getOPINION_WEIGHT_WONDER_NONE()
	{
		return m_iOPINION_WEIGHT_WONDER_NONE;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_FIERCE()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_FIERCE;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_STRONG()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_STRONG;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_WEAK()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_WEAK;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_NONE()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_NONE;
	}
	inline int getOPINION_WEIGHT_WARMONGER_CRITICAL()
	{
		return m_iOPINION_WEIGHT_WARMONGER_CRITICAL;
	}
	inline int getOPINION_WEIGHT_WARMONGER_SEVERE()
	{
		return m_iOPINION_WEIGHT_WARMONGER_SEVERE;
	}
	inline int getOPINION_WEIGHT_WARMONGER_MAJOR()
	{
		return m_iOPINION_WEIGHT_WARMONGER_MAJOR;
	}
	inline int getOPINION_WEIGHT_WARMONGER_MINOR()
	{
		return m_iOPINION_WEIGHT_WARMONGER_MINOR;
	}
	inline int getOPINION_WEIGHT_WARMONGER_NONE()
	{
		return m_iOPINION_WEIGHT_WARMONGER_NONE;
	}
	inline int getOPINION_WEIGHT_ASKED_NO_SETTLE()
	{
		return m_iOPINION_WEIGHT_ASKED_NO_SETTLE;
	}
	inline int getOPINION_WEIGHT_ASKED_STOP_SPYING()
	{
		return m_iOPINION_WEIGHT_ASKED_STOP_SPYING;
	}
	inline int getOPINION_WEIGHT_MADE_DEMAND_OF_US()
	{
		return m_iOPINION_WEIGHT_MADE_DEMAND_OF_US;
	}
	inline int getOPINION_WEIGHT_RETURNED_CIVILIAN()
	{
		return m_iOPINION_WEIGHT_RETURNED_CIVILIAN;
	}
	inline int getOPINION_WEIGHT_BUILT_LANDMARK()
	{
		return m_iOPINION_WEIGHT_BUILT_LANDMARK;
	}
	inline int getOPINION_WEIGHT_RESURRECTED()
	{
		return m_iOPINION_WEIGHT_RESURRECTED;
	}
	inline int getOPINION_WEIGHT_LIBERATED_ONE_CITY()
	{
		return m_iOPINION_WEIGHT_LIBERATED_ONE_CITY;
	}
	inline int getOPINION_WEIGHT_LIBERATED_TWO_CITIES()
	{
		return m_iOPINION_WEIGHT_LIBERATED_TWO_CITIES;
	}
	inline int getOPINION_WEIGHT_LIBERATED_THREE_CITIES()
	{
		return m_iOPINION_WEIGHT_LIBERATED_THREE_CITIES;
	}
	inline int getOPINION_WEIGHT_EMBASSY()
	{
		return m_iOPINION_WEIGHT_EMBASSY;
	}
	inline int getOPINION_WEIGHT_CULTURE_BOMBED()
	{
		return m_iOPINION_WEIGHT_CULTURE_BOMBED;
	}
	inline int getOPINION_WEIGHT_PER_NEGATIVE_CONVERSION()
	{
		return m_iOPINION_WEIGHT_PER_NEGATIVE_CONVERSION;
	}
	inline int getOPINION_WEIGHT_ADOPTING_HIS_RELIGION()
	{
		return m_iOPINION_WEIGHT_ADOPTING_HIS_RELIGION;
	}
	inline int getOPINION_WEIGHT_ADOPTING_MY_RELIGION()
	{
		return m_iOPINION_WEIGHT_ADOPTING_MY_RELIGION;
	}
	inline int getOPINION_WEIGHT_SAME_LATE_POLICIES()
	{
		return m_iOPINION_WEIGHT_SAME_LATE_POLICIES;
	}
	inline int getOPINION_WEIGHT_DIFFERENT_LATE_POLICIES()
	{
		return m_iOPINION_WEIGHT_DIFFERENT_LATE_POLICIES;
	}
	inline int getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD()
	{
		return m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD;
	}
	inline int getOPINION_WEIGHT_IGNORED_MILITARY_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_MILITARY_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_BORDER_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_BORDER_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_BORDER_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_BORDER_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD()
	{
		return m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD;
	}
	inline int getOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_SPY_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_SPY_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_SPY_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_SPY_PROMISE;
	}
	inline int getOPINION_WEIGHT_FORGAVE_FOR_SPYING()
	{
		return m_iOPINION_WEIGHT_FORGAVE_FOR_SPYING;
	}
	inline int getOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE;
	}
	inline int getOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_KILLED_PROTECTED_MINOR()
	{
		return m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR;
	}
	inline int getOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO;
	}
	inline int getOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO;
	}
	inline int getOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR()
	{
		return m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR;
	}
	inline int getOPINION_WEIGHT_DOF()
	{
		return m_iOPINION_WEIGHT_DOF;
	}
	inline int getOPINION_WEIGHT_DOF_WITH_FRIEND()
	{
		return m_iOPINION_WEIGHT_DOF_WITH_FRIEND;
	}
	inline int getOPINION_WEIGHT_DOF_WITH_ENEMY()
	{
		return m_iOPINION_WEIGHT_DOF_WITH_ENEMY;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ME_FRIENDS()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ME_FRIENDS;
	}
	inline int getOPINION_WEIGHT_WAR_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_WAR_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_WAR_ME_FRIENDS()
	{
		return m_iOPINION_WEIGHT_WAR_ME_FRIENDS;
	}
	inline int getOPINION_WEIGHT_REFUSED_REQUEST_EACH()
	{
		return m_iOPINION_WEIGHT_REFUSED_REQUEST_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ME()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ME;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_THEM()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_THEM;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_FRIEND()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_FRIEND;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ENEMY()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ENEMY;
	}
	inline int getOPINION_WEIGHT_RECKLESS_EXPANDER()
	{
		return m_iOPINION_WEIGHT_RECKLESS_EXPANDER;
	}
	inline int getOPINION_WEIGHT_TRADE_MAX()
	{
		return m_iOPINION_WEIGHT_TRADE_MAX;
	}
	inline int getOPINION_WEIGHT_COMMON_FOE_MAX()
	{
		return m_iOPINION_WEIGHT_COMMON_FOE_MAX;
	}
	inline int getOPINION_WEIGHT_ASSIST_MAX()
	{
		return m_iOPINION_WEIGHT_ASSIST_MAX;
	}
	inline int getOPINION_WEIGHT_LIBERATED_CAPITAL()
	{
		return m_iOPINION_WEIGHT_LIBERATED_CAPITAL;
	}
	inline int getOPINION_WEIGHT_LIBERATED_CITY()
	{
		return m_iOPINION_WEIGHT_LIBERATED_CITY;
	}
	inline int getOPINION_WEIGHT_GAVE_ASSISTANCE()
	{
		return m_iOPINION_WEIGHT_GAVE_ASSISTANCE;
	}
	inline int getOPINION_WEIGHT_PAID_TRIBUTE()
	{
		return m_iOPINION_WEIGHT_PAID_TRIBUTE;
	}
	inline int getOPINION_WEIGHT_NUKED_MAX()
	{
		return m_iOPINION_WEIGHT_NUKED_MAX;
	}
	inline int getOPINION_WEIGHT_ROBBED_BY()
	{
		return m_iOPINION_WEIGHT_ROBBED_BY;
	}
	inline int getOPINION_WEIGHT_INTRIGUE_SHARED_BY()
	{
		return m_iOPINION_WEIGHT_INTRIGUE_SHARED_BY;
	}
	inline int getOPINION_WEIGHT_CAPTURED_CAPITAL()
	{
		return m_iOPINION_WEIGHT_CAPTURED_CAPITAL;
	}
	inline int getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL()
	{
		return m_iOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL;
	}
	inline int getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL()
	{
		return m_iOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL;
	}
	inline int getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL()
	{
		return m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL;
	}
	inline int getOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL()
	{
		return m_iOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL;
	}
	inline int getOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING()
	{
		return m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING;
	}
	inline int getOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS;
	}
	inline int getOPINION_THRESHOLD_UNFORGIVABLE()
	{
		return m_iOPINION_THRESHOLD_UNFORGIVABLE;
	}
	inline int getOPINION_THRESHOLD_ENEMY()
	{
		return m_iOPINION_THRESHOLD_ENEMY;
	}
	inline int getOPINION_THRESHOLD_COMPETITOR()
	{
		return m_iOPINION_THRESHOLD_COMPETITOR;
	}
	inline int getOPINION_THRESHOLD_FAVORABLE()
	{
		return m_iOPINION_THRESHOLD_FAVORABLE;
	}
	inline int getOPINION_THRESHOLD_FRIEND()
	{
		return m_iOPINION_THRESHOLD_FRIEND;
	}
	inline int getOPINION_THRESHOLD_ALLY()
	{
		return m_iOPINION_THRESHOLD_ALLY;
	}
	inline int getAPPROACH_NEUTRAL_DEFAULT()
	{
		return m_iAPPROACH_NEUTRAL_DEFAULT;
	}
	inline int getAPPROACH_BIAS_FOR_CURRENT()
	{
		return m_iAPPROACH_BIAS_FOR_CURRENT;
	}
	inline int getAPPROACH_WAR_CURRENTLY_DECEPTIVE()
	{
		return m_iAPPROACH_WAR_CURRENTLY_DECEPTIVE;
	}
	inline int getAPPROACH_HOSTILE_CURRENTLY_HOSTILE()
	{
		return m_iAPPROACH_HOSTILE_CURRENTLY_HOSTILE;
	}
	inline int getAPPROACH_WAR_CURRENTLY_WAR()
	{
		return m_iAPPROACH_WAR_CURRENTLY_WAR;
	}
	inline int getAPPROACH_RANDOM_PERCENT()
	{
		return m_iAPPROACH_RANDOM_PERCENT;
	}
	inline int getAPPROACH_WAR_CONQUEST_GRAND_STRATEGY()
	{
		return m_iAPPROACH_WAR_CONQUEST_GRAND_STRATEGY;
	}
	inline int getAPPROACH_OPINION_UNFORGIVABLE_WAR()
	{
		return m_iAPPROACH_OPINION_UNFORGIVABLE_WAR;
	}
	inline int getAPPROACH_OPINION_UNFORGIVABLE_HOSTILE()
	{
		return m_iAPPROACH_OPINION_UNFORGIVABLE_HOSTILE;
	}
	inline int getAPPROACH_OPINION_UNFORGIVABLE_DECEPTIVE()
	{
		return m_iAPPROACH_OPINION_UNFORGIVABLE_DECEPTIVE;
	}
	inline int getAPPROACH_OPINION_UNFORGIVABLE_GUARDED()
	{
		return m_iAPPROACH_OPINION_UNFORGIVABLE_GUARDED;
	}
	inline int getAPPROACH_OPINION_ENEMY_WAR()
	{
		return m_iAPPROACH_OPINION_ENEMY_WAR;
	}
	inline int getAPPROACH_OPINION_ENEMY_HOSTILE()
	{
		return m_iAPPROACH_OPINION_ENEMY_HOSTILE;
	}
	inline int getAPPROACH_OPINION_ENEMY_DECEPTIVE()
	{
		return m_iAPPROACH_OPINION_ENEMY_DECEPTIVE;
	}
	inline int getAPPROACH_OPINION_ENEMY_GUARDED()
	{
		return m_iAPPROACH_OPINION_ENEMY_GUARDED;
	}
	inline int getAPPROACH_OPINION_COMPETITOR_WAR()
	{
		return m_iAPPROACH_OPINION_COMPETITOR_WAR;
	}
	inline int getAPPROACH_OPINION_COMPETITOR_HOSTILE()
	{
		return m_iAPPROACH_OPINION_COMPETITOR_HOSTILE;
	}
	inline int getAPPROACH_OPINION_COMPETITOR_DECEPTIVE()
	{
		return m_iAPPROACH_OPINION_COMPETITOR_DECEPTIVE;
	}
	inline int getAPPROACH_OPINION_COMPETITOR_GUARDED()
	{
		return m_iAPPROACH_OPINION_COMPETITOR_GUARDED;
	}
	inline int getAPPROACH_OPINION_NEUTRAL_DECEPTIVE()
	{
		return m_iAPPROACH_OPINION_NEUTRAL_DECEPTIVE;
	}
	inline int getAPPROACH_OPINION_NEUTRAL_FRIENDLY()
	{
		return m_iAPPROACH_OPINION_NEUTRAL_FRIENDLY;
	}
	inline int getAPPROACH_OPINION_FAVORABLE_HOSTILE()
	{
		return m_iAPPROACH_OPINION_FAVORABLE_HOSTILE;
	}
	inline int getAPPROACH_OPINION_FAVORABLE_DECEPTIVE()
	{
		return m_iAPPROACH_OPINION_FAVORABLE_DECEPTIVE;
	}
	inline int getAPPROACH_OPINION_FAVORABLE_FRIENDLY()
	{
		return m_iAPPROACH_OPINION_FAVORABLE_FRIENDLY;
	}
	inline int getAPPROACH_OPINION_FRIEND_HOSTILE()
	{
		return m_iAPPROACH_OPINION_FRIEND_HOSTILE;
	}
	inline int getAPPROACH_OPINION_FRIEND_FRIENDLY()
	{
		return m_iAPPROACH_OPINION_FRIEND_FRIENDLY;
	}
	inline int getAPPROACH_OPINION_ALLY_FRIENDLY()
	{
		return m_iAPPROACH_OPINION_ALLY_FRIENDLY;
	}
	inline int getAPPROACH_DECEPTIVE_WORKING_WITH_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_WORKING_WITH_PLAYER;
	}
	inline int getAPPROACH_FRIENDLY_WORKING_WITH_PLAYER()
	{
		return m_iAPPROACH_FRIENDLY_WORKING_WITH_PLAYER;
	}
	inline int getAPPROACH_HOSTILE_WORKING_WITH_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_WORKING_WITH_PLAYER;
	}
	inline int getAPPROACH_GUARDED_WORKING_WITH_PLAYER()
	{
		return m_iAPPROACH_GUARDED_WORKING_WITH_PLAYER;
	}
	inline int getAPPROACH_DECEPTIVE_WORKING_AGAINST_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_WORKING_AGAINST_PLAYER;
	}
	inline int getAPPROACH_HOSTILE_WORKING_AGAINST_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_WORKING_AGAINST_PLAYER;
	}
	inline int getAPPROACH_WAR_WORKING_AGAINST_PLAYER()
	{
		return m_iAPPROACH_WAR_WORKING_AGAINST_PLAYER;
	}
	inline int getAPPROACH_WAR_DENOUNCED()
	{
		return m_iAPPROACH_WAR_DENOUNCED;
	}
	inline int getAPPROACH_HOSTILE_DENOUNCED()
	{
		return m_iAPPROACH_HOSTILE_DENOUNCED;
	}
	inline int getAPPROACH_GUARDED_DENOUNCED()
	{
		return m_iAPPROACH_GUARDED_DENOUNCED;
	}
	inline int getAPPROACH_FRIENDLY_DENOUNCED()
	{
		return m_iAPPROACH_FRIENDLY_DENOUNCED;
	}
	inline int getAPPROACH_DECEPTIVE_DENOUNCED()
	{
		return m_iAPPROACH_DECEPTIVE_DENOUNCED;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_WAR()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_WAR;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_HOSTILE()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_HOSTILE;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_GUARDED()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_GUARDED;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_WAR()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_WAR;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_HOSTILE()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_HOSTILE;
	}
	inline int getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_GUARDED()
	{
		return m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_GUARDED;
	}
	inline int getAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL()
	{
		return m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL;
	}
	inline int getAPPROACH_GUARDED_MILITARY_THREAT_CRITICAL()
	{
		return m_iAPPROACH_GUARDED_MILITARY_THREAT_CRITICAL;
	}
	inline int getAPPROACH_AFRAID_MILITARY_THREAT_CRITICAL()
	{
		return m_iAPPROACH_AFRAID_MILITARY_THREAT_CRITICAL;
	}
	inline int getAPPROACH_DECEPTIVE_DEMAND()
	{
		return m_iAPPROACH_DECEPTIVE_DEMAND;
	}
	inline int getAPPROACH_FRIENDLY_DEMAND()
	{
		return m_iAPPROACH_FRIENDLY_DEMAND;
	}
	inline int getAPPROACH_WAR_BROKEN_MILITARY_PROMISE()
	{
		return m_iAPPROACH_WAR_BROKEN_MILITARY_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE;
	}
	inline int getAPPROACH_WAR_BROKEN_MILITARY_PROMISE_WORLD()
	{
		return m_iAPPROACH_WAR_BROKEN_MILITARY_PROMISE_WORLD;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE_WORLD()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE_WORLD;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE_WORLD()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE_WORLD;
	}
	inline int getAPPROACH_DECEPTIVE_IGNORED_MILITARY_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_IGNORED_MILITARY_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_IGNORED_MILITARY_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_IGNORED_MILITARY_PROMISE;
	}
	inline int getAPPROACH_WAR_BROKEN_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_WAR_BROKEN_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_WAR_IGNORED_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_WAR_IGNORED_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_IGNORED_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_IGNORED_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_IGNORED_EXPANSION_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_IGNORED_EXPANSION_PROMISE;
	}
	inline int getAPPROACH_WAR_BROKEN_BORDER_PROMISE()
	{
		return m_iAPPROACH_WAR_BROKEN_BORDER_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_BORDER_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_BORDER_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_BORDER_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_BORDER_PROMISE;
	}
	inline int getAPPROACH_WAR_IGNORED_BORDER_PROMISE()
	{
		return m_iAPPROACH_WAR_IGNORED_BORDER_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_IGNORED_BORDER_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_IGNORED_BORDER_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_IGNORED_BORDER_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_IGNORED_BORDER_PROMISE;
	}
	inline int getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE_WORLD()
	{
		return m_iAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE_WORLD;
	}
	inline int getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE_WORLD()
	{
		return m_iAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE_WORLD;
	}
	inline int getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE_WORLD()
	{
		return m_iAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE_WORLD;
	}
	inline int getAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE()
	{
		return m_iAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE;
	}
	inline int getAPPROACH_FRIENDLY_MILITARY_THREAT_CRITICAL()
	{
		return m_iAPPROACH_FRIENDLY_MILITARY_THREAT_CRITICAL;
	}
	inline int getAPPROACH_DECEPTIVE_MILITARY_THREAT_SEVERE()
	{
		return m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_SEVERE;
	}
	inline int getAPPROACH_GUARDED_MILITARY_THREAT_SEVERE()
	{
		return m_iAPPROACH_GUARDED_MILITARY_THREAT_SEVERE;
	}
	inline int getAPPROACH_AFRAID_MILITARY_THREAT_SEVERE()
	{
		return m_iAPPROACH_AFRAID_MILITARY_THREAT_SEVERE;
	}
	inline int getAPPROACH_FRIENDLY_MILITARY_THREAT_SEVERE()
	{
		return m_iAPPROACH_FRIENDLY_MILITARY_THREAT_SEVERE;
	}
	inline int getAPPROACH_DECEPTIVE_MILITARY_THREAT_MAJOR()
	{
		return m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_MAJOR;
	}
	inline int getAPPROACH_GUARDED_MILITARY_THREAT_MAJOR()
	{
		return m_iAPPROACH_GUARDED_MILITARY_THREAT_MAJOR;
	}
	inline int getAPPROACH_AFRAID_MILITARY_THREAT_MAJOR()
	{
		return m_iAPPROACH_AFRAID_MILITARY_THREAT_MAJOR;
	}
	inline int getAPPROACH_FRIENDLY_MILITARY_THREAT_MAJOR()
	{
		return m_iAPPROACH_FRIENDLY_MILITARY_THREAT_MAJOR;
	}
	inline int getAPPROACH_DECEPTIVE_MILITARY_THREAT_MINOR()
	{
		return m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_MINOR;
	}
	inline int getAPPROACH_GUARDED_MILITARY_THREAT_MINOR()
	{
		return m_iAPPROACH_GUARDED_MILITARY_THREAT_MINOR;
	}
	inline int getAPPROACH_AFRAID_MILITARY_THREAT_MINOR()
	{
		return m_iAPPROACH_AFRAID_MILITARY_THREAT_MINOR;
	}
	inline int getAPPROACH_FRIENDLY_MILITARY_THREAT_MINOR()
	{
		return m_iAPPROACH_FRIENDLY_MILITARY_THREAT_MINOR;
	}
	inline int getAPPROACH_HOSTILE_MILITARY_THREAT_NONE()
	{
		return m_iAPPROACH_HOSTILE_MILITARY_THREAT_NONE;
	}
	inline int getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL()
	{
		return m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL;
	}
	inline int getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING()
	{
		return m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING;
	}
	inline int getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING()
	{
		return m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING;
	}
	inline int getAPPROACH_HOSTILE_AT_WAR_WITH_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_AT_WAR_WITH_PLAYER;
	}
	inline int getAPPROACH_DECEPTIVE_AT_WAR_WITH_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_AT_WAR_WITH_PLAYER;
	}
	inline int getAPPROACH_GUARDED_AT_WAR_WITH_PLAYER()
	{
		return m_iAPPROACH_GUARDED_AT_WAR_WITH_PLAYER;
	}
	inline int getAPPROACH_FRIENDLY_AT_WAR_WITH_PLAYER()
	{
		return m_iAPPROACH_FRIENDLY_AT_WAR_WITH_PLAYER;
	}
	inline int getAPPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_HOSTILE_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_DECEPTIVE_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_GUARDED_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_GUARDED_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_FRIENDLY_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_FRIENDLY_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_WAR_HOSTILE_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_WAR_HOSTILE_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_HOSTILE_HOSTILE_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_HOSTILE_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_DECEPTIVE_HOSTILE_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_HOSTILE_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_FRIENDLY_HOSTILE_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_FRIENDLY_HOSTILE_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_WAR_AFRAID_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_WAR_AFRAID_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_HOSTILE_AFRAID_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_HOSTILE_AFRAID_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_DECEPTIVE_AFRAID_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_DECEPTIVE_AFRAID_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_FRIENDLY_AFRAID_WITH_ANOTHER_PLAYER()
	{
		return m_iAPPROACH_FRIENDLY_AFRAID_WITH_ANOTHER_PLAYER;
	}
	inline int getAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT;
	}
	inline int getAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT;
	}
	inline int getAPPROACH_WAR_PROJECTION_STALEMATE_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_STALEMATE_PERCENT;
	}
	inline int getAPPROACH_WAR_PROJECTION_UNKNOWN_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_UNKNOWN_PERCENT;
	}
	inline int getAPPROACH_WAR_PROJECTION_GOOD_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_GOOD_PERCENT;
	}
	inline int getAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT()
	{
		return m_iAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_DESTRUCTION_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_DESTRUCTION_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_DEFEAT_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_DEFEAT_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_STALEMATE_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_STALEMATE_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_UNKNOWN_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_UNKNOWN_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_GOOD_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_GOOD_PERCENT;
	}
	inline int getAPPROACH_GUARDED_PROJECTION_VERY_GOOD_PERCENT()
	{
		return m_iAPPROACH_GUARDED_PROJECTION_VERY_GOOD_PERCENT;
	}
	inline int getTURNS_SINCE_PEACE_WEIGHT_DAMPENER()
	{
		return m_iTURNS_SINCE_PEACE_WEIGHT_DAMPENER;
	}
	inline int getAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT()
	{
		return m_iAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT;
	}
	inline int getAPPROACH_WAR_RECKLESS_EXPANDER()
	{
		return m_iAPPROACH_WAR_RECKLESS_EXPANDER;
	}
	inline int getAPPROACH_WAR_PROXIMITY_NEIGHBORS()
	{
		return m_iAPPROACH_WAR_PROXIMITY_NEIGHBORS;
	}
	inline int getAPPROACH_WAR_PROXIMITY_CLOSE()
	{
		return m_iAPPROACH_WAR_PROXIMITY_CLOSE;
	}
	inline int getAPPROACH_WAR_PROXIMITY_FAR()
	{
		return m_iAPPROACH_WAR_PROXIMITY_FAR;
	}
	inline int getAPPROACH_WAR_PROXIMITY_DISTANT()
	{
		return m_iAPPROACH_WAR_PROXIMITY_DISTANT;
	}
	inline int getMINOR_APPROACH_IGNORE_DEFAULT()
	{
		return m_iMINOR_APPROACH_IGNORE_DEFAULT;
	}
	inline int getMINOR_APPROACH_BIAS_FOR_CURRENT()
	{
		return m_iMINOR_APPROACH_BIAS_FOR_CURRENT;
	}
	inline int getMINOR_APPROACH_IGNORE_CURRENTLY_WAR()
	{
		return m_iMINOR_APPROACH_IGNORE_CURRENTLY_WAR;
	}
	inline int getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_CONQUEST_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_CONQUEST_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_FRIENDLY_CONQUEST_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_FRIENDLY_CONQUEST_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_WAR_DIPLO_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_WAR_DIPLO_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_IGNORE_DIPLO_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_IGNORE_DIPLO_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_DIPLO_GRAND_STRATEGY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_DIPLO_GRAND_STRATEGY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_WAR_CULTURE_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_WAR_CULTURE_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_IGNORE_CULTURE_GRAND_STRATEGY()
	{
		return m_iMINOR_APPROACH_IGNORE_CULTURE_GRAND_STRATEGY;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_CULTURE_GRAND_STRATEGY_CST()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_CULTURE_GRAND_STRATEGY_CST;
	}
	inline int getMINOR_APPROACH_WAR_CURRENTLY_PROTECTIVE()
	{
		return m_iMINOR_APPROACH_WAR_CURRENTLY_PROTECTIVE;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_CURRENTLY_PROTECTIVE()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_CURRENTLY_PROTECTIVE;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_CURRENTLY_WAR()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_CURRENTLY_WAR;
	}
	inline int getMINOR_APPROACH_FRIENDLY_CURRENTLY_WAR()
	{
		return m_iMINOR_APPROACH_FRIENDLY_CURRENTLY_WAR;
	}
	inline int getMINOR_APPROACH_FRIENDLY_RESOURCES()
	{
		return m_iMINOR_APPROACH_FRIENDLY_RESOURCES;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_RESOURCES()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_RESOURCES;
	}
	inline int getMINOR_APPROACH_WAR_FRIENDS()
	{
		return m_iMINOR_APPROACH_WAR_FRIENDS;
	}
	inline int getMINOR_APPROACH_FRIENDLY_FRIENDS()
	{
		return m_iMINOR_APPROACH_FRIENDLY_FRIENDS;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_FRIENDS()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_FRIENDS;
	}
	inline int getMINOR_APPROACH_IGNORE_PROXIMITY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_IGNORE_PROXIMITY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PROXIMITY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PROXIMITY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PROXIMITY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_CONQUEST_PROXIMITY_NEIGHBORS()
	{
		return m_iMINOR_APPROACH_CONQUEST_PROXIMITY_NEIGHBORS;
	}
	inline int getMINOR_APPROACH_IGNORE_PROXIMITY_CLOSE()
	{
		return m_iMINOR_APPROACH_IGNORE_PROXIMITY_CLOSE;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PROXIMITY_CLOSE()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PROXIMITY_CLOSE;
	}
	inline int getMINOR_APPROACH_CONQUEST_PROXIMITY_CLOSE()
	{
		return m_iMINOR_APPROACH_CONQUEST_PROXIMITY_CLOSE;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PROXIMITY_FAR()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_FAR;
	}
	inline int getMINOR_APPROACH_CONQUEST_PROXIMITY_FAR()
	{
		return m_iMINOR_APPROACH_CONQUEST_PROXIMITY_FAR;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PROXIMITY_DISTANT()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_DISTANT;
	}
	inline int getMINOR_APPROACH_CONQUEST_PROXIMITY_DISTANT()
	{
		return m_iMINOR_APPROACH_CONQUEST_PROXIMITY_DISTANT;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PERSONALITY_FRIENDLY()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_FRIENDLY;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PERSONALITY_PROTECTIVE()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_PROTECTIVE;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PERSONALITY_NEUTRAL()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_NEUTRAL;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PERSONALITY_NEUTRAL()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_NEUTRAL;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PERSONALITY_HOSTILE()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_HOSTILE;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PERSONALITY_HOSTILE()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_HOSTILE;
	}
	inline int getMINOR_APPROACH_CONQUEST_PERSONALITY_HOSTILE()
	{
		return m_iMINOR_APPROACH_CONQUEST_PERSONALITY_HOSTILE;
	}
	inline int getMINOR_APPROACH_FRIENDLY_PERSONALITY_IRRATIONAL()
	{
		return m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_IRRATIONAL;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_PERSONALITY_IRRATIONAL()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_IRRATIONAL;
	}
	inline int getMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER()
	{
		return m_iMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	}
	inline int getMINOR_APPROACH_PROTECTIVE_WITH_ANOTHER_PLAYER()
	{
		return m_iMINOR_APPROACH_PROTECTIVE_WITH_ANOTHER_PLAYER;
	}
	inline int getMINOR_APPROACH_WAR_TARGET_IMPOSSIBLE()
	{
		return m_iMINOR_APPROACH_WAR_TARGET_IMPOSSIBLE;
	}
	inline int getMINOR_APPROACH_WAR_TARGET_BAD()
	{
		return m_iMINOR_APPROACH_WAR_TARGET_BAD;
	}
	inline int getMINOR_APPROACH_WAR_TARGET_AVERAGE()
	{
		return m_iMINOR_APPROACH_WAR_TARGET_AVERAGE;
	}
	inline int getMINOR_APPROACH_WAR_TARGET_FAVORABLE()
	{
		return m_iMINOR_APPROACH_WAR_TARGET_FAVORABLE;
	}
	inline int getMINOR_APPROACH_WAR_TARGET_SOFT()
	{
		return m_iMINOR_APPROACH_WAR_TARGET_SOFT;
	}
	inline int getCOOPERATION_DESIRE_THRESHOLD_EAGER()
	{
		return m_iCOOPERATION_DESIRE_THRESHOLD_EAGER;
	}
	inline int getCOOPERATION_DESIRE_THRESHOLD_STRONG()
	{
		return m_iCOOPERATION_DESIRE_THRESHOLD_STRONG;
	}
	inline int getCOOPERATION_DESIRE_THRESHOLD_DECENT()
	{
		return m_iCOOPERATION_DESIRE_THRESHOLD_DECENT;
	}
	inline int getCOOPERATION_DESIRE_THRESHOLD_WEAK()
	{
		return m_iCOOPERATION_DESIRE_THRESHOLD_WEAK;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_CALM()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_CALM;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_NEARLY_WON()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_NEARLY_WON;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_OFFENSIVE()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_OFFENSIVE;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_STALEMATE()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_STALEMATE;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_DEFENSIVE()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_DEFENSIVE;
	}
	inline int getCOOPERATION_DESIRE_WAR_STATE_NEARLY_DEFEATED()
	{
		return m_iCOOPERATION_DESIRE_WAR_STATE_NEARLY_DEFEATED;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_IMMENSE()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_IMMENSE;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_POWERFUL()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_POWERFUL;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_STRONG()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_STRONG;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_AVERAGE()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_AVERAGE;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_POOR()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_POOR;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_WEAK()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_WEAK;
	}
	inline int getCOOPERATION_DESIRE_MILITARY_STRENGTH_PATHETIC()
	{
		return m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_PATHETIC;
	}
	inline int getCOOPERATION_DESIRE_OPINION_ALLY()
	{
		return m_iCOOPERATION_DESIRE_OPINION_ALLY;
	}
	inline int getCOOPERATION_DESIRE_OPINION_FRIEND()
	{
		return m_iCOOPERATION_DESIRE_OPINION_FRIEND;
	}
	inline int getCOOPERATION_DESIRE_OPINION_NEUTRAL()
	{
		return m_iCOOPERATION_DESIRE_OPINION_NEUTRAL;
	}
	inline int getCOOPERATION_DESIRE_OPINION_COMPETITOR()
	{
		return m_iCOOPERATION_DESIRE_OPINION_COMPETITOR;
	}
	inline int getCOOPERATION_DESIRE_OPINION_ENEMY()
	{
		return m_iCOOPERATION_DESIRE_OPINION_ENEMY;
	}
	inline int getCOOPERATION_DESIRE_GAME_THREAT_CRITICAL()
	{
		return m_iCOOPERATION_DESIRE_GAME_THREAT_CRITICAL;
	}
	inline int getCOOPERATION_DESIRE_GAME_THREAT_SEVERE()
	{
		return m_iCOOPERATION_DESIRE_GAME_THREAT_SEVERE;
	}
	inline int getCOOPERATION_DESIRE_GAME_THREAT_MAJOR()
	{
		return m_iCOOPERATION_DESIRE_GAME_THREAT_MAJOR;
	}
	inline int getCOOPERATION_DESIRE_GAME_THREAT_MINOR()
	{
		return m_iCOOPERATION_DESIRE_GAME_THREAT_MINOR;
	}
	inline int getCOOPERATION_DESIRE_GAME_THREAT_NONE()
	{
		return m_iCOOPERATION_DESIRE_GAME_THREAT_NONE;
	}
	inline int getCOOPERATION_DESIRE_THEM_US_SAME_COMPETITOR()
	{
		return m_iCOOPERATION_DESIRE_THEM_US_SAME_COMPETITOR;
	}
	inline int getCOOPERATION_DESIRE_THEM_US_COMPETITOR_ENEMY()
	{
		return m_iCOOPERATION_DESIRE_THEM_US_COMPETITOR_ENEMY;
	}
	inline int getCOOPERATION_DESIRE_THEM_US_ENEMY_COMPETITOR()
	{
		return m_iCOOPERATION_DESIRE_THEM_US_ENEMY_COMPETITOR;
	}
	inline int getCOOPERATION_DESIRE_THEM_US_SAME_ENEMY()
	{
		return m_iCOOPERATION_DESIRE_THEM_US_SAME_ENEMY;
	}
	inline int getEVALUATE_WAR_PLAYER_THRESHOLD()
	{
		return m_iEVALUATE_WAR_PLAYER_THRESHOLD;
	}
	inline int getEVALUATE_WAR_WILLING_TO_DECLARE_THRESHOLD()
	{
		return m_iEVALUATE_WAR_WILLING_TO_DECLARE_THRESHOLD;
	}
	inline int getEVALUATE_WAR_CONQUEST_GRAND_STRATEGY()
	{
		return m_iEVALUATE_WAR_CONQUEST_GRAND_STRATEGY;
	}
	inline int getEVALUATE_WAR_NOT_PREPARED()
	{
		return m_iEVALUATE_WAR_NOT_PREPARED;
	}
	inline int getEVALUATE_WAR_ALREADY_FIGHTING()
	{
		return m_iEVALUATE_WAR_ALREADY_FIGHTING;
	}
	inline int getEVALUATE_WAR_GAME_THREAT_CRITICAL()
	{
		return m_iEVALUATE_WAR_GAME_THREAT_CRITICAL;
	}
	inline int getEVALUATE_WAR_GAME_THREAT_SEVERE()
	{
		return m_iEVALUATE_WAR_GAME_THREAT_SEVERE;
	}
	inline int getEVALUATE_WAR_GAME_THREAT_MAJOR()
	{
		return m_iEVALUATE_WAR_GAME_THREAT_MAJOR;
	}
	inline int getEVALUATE_WAR_GAME_THREAT_MINOR()
	{
		return m_iEVALUATE_WAR_GAME_THREAT_MINOR;
	}
	inline int getEVALUATE_WAR_GAME_THREAT_NONE()
	{
		return m_iEVALUATE_WAR_GAME_THREAT_NONE;
	}
	inline int getEVALUATE_WAR_SOFT_TARGET()
	{
		return m_iEVALUATE_WAR_SOFT_TARGET;
	}
	inline int getEVALUATE_WAR_FAVORABLE_TARGET()
	{
		return m_iEVALUATE_WAR_FAVORABLE_TARGET;
	}
	inline int getEVALUATE_WAR_AVERAGE_TARGET()
	{
		return m_iEVALUATE_WAR_AVERAGE_TARGET;
	}
	inline int getEVALUATE_WAR_BAD_TARGET()
	{
		return m_iEVALUATE_WAR_BAD_TARGET;
	}
	inline int getEVALUATE_WAR_IMPOSSIBLE_TARGET()
	{
		return m_iEVALUATE_WAR_IMPOSSIBLE_TARGET;
	}
	inline int getREQUEST_PEACE_TURN_THRESHOLD()
	{
		return m_iREQUEST_PEACE_TURN_THRESHOLD;
	}
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DEF(WAR_MAJOR_MINIMUM_TURNS)
	GD_INT_DEF(WAR_MINOR_MINIMUM_TURNS)
#endif
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_DESTRUCTION()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_DESTRUCTION;
	}
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_DEFEAT()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_DEFEAT;
	}
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_STALEMATE()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_STALEMATE;
	}
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_UNKNOWN()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_UNKNOWN;
	}
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_GOOD()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_GOOD;
	}
	inline int getPEACE_WILLINGNESS_OFFER_PROJECTION_VERY_GOOD()
	{
		return m_iPEACE_WILLINGNESS_OFFER_PROJECTION_VERY_GOOD;
	}
	inline int getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE()
	{
		return m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE;
	}
	inline int getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR()
	{
		return m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR;
	}
	inline int getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR()
	{
		return m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR;
	}
	inline int getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS()
	{
		return m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS;
	}
	inline int getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED()
	{
		return m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT;
	}
	inline int getPEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE()
	{
		return m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DESTRUCTION()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_DESTRUCTION;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DEFEAT()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_DEFEAT;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_STALEMATE()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_STALEMATE;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_UNKNOWN()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_UNKNOWN;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_GOOD()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_GOOD;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_PROJECTION_VERY_GOOD()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_VERY_GOOD;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_TARGET_IMPOSSIBLE()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_TARGET_IMPOSSIBLE;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_TARGET_BAD()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_TARGET_BAD;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_TARGET_AVERAGE()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_TARGET_AVERAGE;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_TARGET_FAVORABLE()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_TARGET_FAVORABLE;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_TARGET_SOFT()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_TARGET_SOFT;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT;
	}
	inline int getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE()
	{
		return m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE;
	}
	inline int getARMISTICE_GPT_DIVISOR()
	{
		return m_iARMISTICE_GPT_DIVISOR;
	}
	inline int getARMISTICE_MIN_GOLD()
	{
		return m_iARMISTICE_MIN_GOLD;
	}
	inline int getWAR_GOAL_THRESHOLD_CONQUEST()
	{
		return m_iWAR_GOAL_THRESHOLD_CONQUEST;
	}
	inline int getWAR_GOAL_THRESHOLD_DAMAGE()
	{
		return m_iWAR_GOAL_THRESHOLD_DAMAGE;
	}
	inline int getWAR_GOAL_THRESHOLD_PEACE()
	{
		return m_iWAR_GOAL_THRESHOLD_PEACE;
	}
	inline int getWAR_GOAL_STATE_CALM()
	{
		return m_iWAR_GOAL_STATE_CALM;
	}
	inline int getWAR_GOAL_STATE_NEARLY_WON()
	{
		return m_iWAR_GOAL_STATE_NEARLY_WON;
	}
	inline int getWAR_GOAL_STATE_OFFENSIVE()
	{
		return m_iWAR_GOAL_STATE_OFFENSIVE;
	}
	inline int getWAR_GOAL_STATE_STALEMATE()
	{
		return m_iWAR_GOAL_STATE_STALEMATE;
	}
	inline int getWAR_GOAL_STATE_DEFENSIVE()
	{
		return m_iWAR_GOAL_STATE_DEFENSIVE;
	}
	inline int getWAR_GOAL_STATE_NEARLY_DEFEATED()
	{
		return m_iWAR_GOAL_STATE_NEARLY_DEFEATED;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_CALM()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_CALM;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_WON()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_WON;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_OFFENSIVE()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_OFFENSIVE;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_STALEMATE()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_STALEMATE;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_DEFENSIVE()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_DEFENSIVE;
	}
	inline int getWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_DEFEATED()
	{
		return m_iWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_DEFEATED;
	}
	inline int getWAR_GOAL_DAMAGE_CRIPPLED()
	{
		return m_iWAR_GOAL_DAMAGE_CRIPPLED;
	}
	inline int getWAR_GOAL_DAMAGE_SERIOUS()
	{
		return m_iWAR_GOAL_DAMAGE_SERIOUS;
	}
	inline int getWAR_GOAL_DAMAGE_MAJOR()
	{
		return m_iWAR_GOAL_DAMAGE_MAJOR;
	}
	inline int getWAR_GOAL_DAMAGE_MINOR()
	{
		return m_iWAR_GOAL_DAMAGE_MINOR;
	}
	inline int getWAR_GOAL_DAMAGE_NONE()
	{
		return m_iWAR_GOAL_DAMAGE_NONE;
	}
	inline int getWAR_GOAL_GAME_THREAT_CRITICAL()
	{
		return m_iWAR_GOAL_GAME_THREAT_CRITICAL;
	}
	inline int getWAR_GOAL_GAME_THREAT_SEVERE()
	{
		return m_iWAR_GOAL_GAME_THREAT_SEVERE;
	}
	inline int getWAR_GOAL_GAME_THREAT_MAJOR()
	{
		return m_iWAR_GOAL_GAME_THREAT_MAJOR;
	}
	inline int getWAR_GOAL_GAME_THREAT_MINOR()
	{
		return m_iWAR_GOAL_GAME_THREAT_MINOR;
	}
	inline int getWAR_GOAL_GAME_THREAT_NONE()
	{
		return m_iWAR_GOAL_GAME_THREAT_NONE;
	}
	inline int getWAR_GOAL_TARGET_SOFT()
	{
		return m_iWAR_GOAL_TARGET_SOFT;
	}
	inline int getWAR_GOAL_TARGET_FAVORABLE()
	{
		return m_iWAR_GOAL_TARGET_FAVORABLE;
	}
	inline int getWAR_GOAL_TARGET_AVERAGE()
	{
		return m_iWAR_GOAL_TARGET_AVERAGE;
	}
	inline int getWAR_GOAL_TARGET_BAD()
	{
		return m_iWAR_GOAL_TARGET_BAD;
	}
	inline int getWAR_GOAL_TARGET_IMPOSSIBLE()
	{
		return m_iWAR_GOAL_TARGET_IMPOSSIBLE;
	}
	inline int getWAR_PROJECTION_THRESHOLD_VERY_GOOD()
	{
		return m_iWAR_PROJECTION_THRESHOLD_VERY_GOOD;
	}
	inline int getWAR_PROJECTION_THRESHOLD_GOOD()
	{
		return m_iWAR_PROJECTION_THRESHOLD_GOOD;
	}
	inline int getWAR_PROJECTION_THRESHOLD_DESTRUCTION()
	{
		return m_iWAR_PROJECTION_THRESHOLD_DESTRUCTION;
	}
	inline int getWAR_PROJECTION_THRESHOLD_DEFEAT()
	{
		return m_iWAR_PROJECTION_THRESHOLD_DEFEAT;
	}
	inline int getWAR_PROJECTION_THRESHOLD_STALEMATE()
	{
		return m_iWAR_PROJECTION_THRESHOLD_STALEMATE;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_PATHETIC()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_PATHETIC;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_WEAK()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_WEAK;
	}
	inline int getWAR_PROJECTION_RECKLESS_EXPANDER()
	{
		return m_iWAR_PROJECTION_RECKLESS_EXPANDER;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POOR()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POOR;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_AVERAGE()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_AVERAGE;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_STRONG()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_STRONG;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POWERFUL()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POWERFUL;
	}
	inline int getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_IMMENSE()
	{
		return m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_IMMENSE;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_PATHETIC()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_PATHETIC;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_WEAK()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_WEAK;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POOR()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POOR;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_AVERAGE()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_AVERAGE;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_STRONG()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_STRONG;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POWERFUL()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POWERFUL;
	}
	inline int getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_IMMENSE()
	{
		return m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_IMMENSE;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_US_NONE()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_US_NONE;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_US_MINOR()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_US_MINOR;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_US_MAJOR()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_US_MAJOR;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_US_SERIOUS()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_US_SERIOUS;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_US_CRIPPLED()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_US_CRIPPLED;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_THEM_NONE()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_THEM_NONE;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_THEM_MINOR()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_THEM_MINOR;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_THEM_MAJOR()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_THEM_MAJOR;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_THEM_SERIOUS()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_THEM_SERIOUS;
	}
	inline int getWAR_PROJECTION_WAR_DAMAGE_THEM_CRIPPLED()
	{
		return m_iWAR_PROJECTION_WAR_DAMAGE_THEM_CRIPPLED;
	}
	inline int getWAR_PROJECTION_WAR_DURATION_SCORE_CAP()
	{
		return m_iWAR_PROJECTION_WAR_DURATION_SCORE_CAP;
	}
	inline int getWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES()
	{
		return m_iWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES;
	}
	inline int getWAR_STATE_THRESHOLD_NEARLY_WON()
	{
		return m_iWAR_STATE_THRESHOLD_NEARLY_WON;
	}
	inline int getWAR_STATE_THRESHOLD_OFFENSIVE()
	{
		return m_iWAR_STATE_THRESHOLD_OFFENSIVE;
	}
	inline int getWAR_STATE_THRESHOLD_STALEMATE()
	{
		return m_iWAR_STATE_THRESHOLD_STALEMATE;
	}
	inline int getWAR_STATE_THRESHOLD_DEFENSIVE()
	{
		return m_iWAR_STATE_THRESHOLD_DEFENSIVE;
	}
	inline int getWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED()
	{
		return m_iWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED;
	}
	inline int getWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS()
	{
		return m_iWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS;
	}
	inline int getWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR()
	{
		return m_iWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR;
	}
	inline int getWAR_DAMAGE_LEVEL_THRESHOLD_MINOR()
	{
		return m_iWAR_DAMAGE_LEVEL_THRESHOLD_MINOR;
	}
	inline int getWAR_DAMAGE_LEVEL_CITY_WEIGHT()
	{
		return m_iWAR_DAMAGE_LEVEL_CITY_WEIGHT;
	}
	inline int getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER()
	{
		return m_iWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER;
	}
	inline int getWAR_DAMAGE_LEVEL_UNINVOLVED_CITY_POP_MULTIPLIER()
	{
		return m_iWAR_DAMAGE_LEVEL_UNINVOLVED_CITY_POP_MULTIPLIER;
	}
	inline int getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE()
	{
		return m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE;
	}
	inline int getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH()
	{
		return m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH;
	}
	inline int getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM()
	{
		return m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM;
	}
	inline int getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW()
	{
		return m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW;
	}
	inline int getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_HIGH()
	{
		return m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_HIGH;
	}
	inline int getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_MEDIUM()
	{
		return m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_MEDIUM;
	}
	inline int getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_LOW()
	{
		return m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_LOW;
	}
	inline int getPLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD()
	{
		return m_iPLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD;
	}
	inline int getPLOT_BUYING_POSTURE_HIGH_THRESHOLD()
	{
		return m_iPLOT_BUYING_POSTURE_HIGH_THRESHOLD;
	}
	inline int getPLOT_BUYING_POSTURE_MEDIUM_THRESHOLD()
	{
		return m_iPLOT_BUYING_POSTURE_MEDIUM_THRESHOLD;
	}
	inline int getPLOT_BUYING_POSTURE_LOW_THRESHOLD()
	{
		return m_iPLOT_BUYING_POSTURE_LOW_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_BASE()
	{
		return m_iMILITARY_STRENGTH_BASE;
	}
	inline int getMILITARY_STRENGTH_RATIO_MULTIPLIER()
	{
		return m_iMILITARY_STRENGTH_RATIO_MULTIPLIER;
	}
	inline int getMILITARY_STRENGTH_CITY_MOD()
	{
		return m_iMILITARY_STRENGTH_CITY_MOD;
	}
	inline int getMILITARY_STRENGTH_IMMENSE_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_IMMENSE_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_POWERFUL_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_POWERFUL_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_STRONG_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_STRONG_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_AVERAGE_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_AVERAGE_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_POOR_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_POOR_THRESHOLD;
	}
	inline int getMILITARY_STRENGTH_WEAK_THRESHOLD()
	{
		return m_iMILITARY_STRENGTH_WEAK_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_RATIO_MULTIPLIER()
	{
		return m_iECONOMIC_STRENGTH_RATIO_MULTIPLIER;
	}
	inline int getECONOMIC_STRENGTH_IMMENSE_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_IMMENSE_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_POWERFUL_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_POWERFUL_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_STRONG_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_STRONG_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_AVERAGE_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_AVERAGE_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_POOR_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_POOR_THRESHOLD;
	}
	inline int getECONOMIC_STRENGTH_WEAK_THRESHOLD()
	{
		return m_iECONOMIC_STRENGTH_WEAK_THRESHOLD;
	}
	inline int getTARGET_IMPOSSIBLE_THRESHOLD()
	{
		return m_iTARGET_IMPOSSIBLE_THRESHOLD;
	}
	inline int getTARGET_BAD_THRESHOLD()
	{
		return m_iTARGET_BAD_THRESHOLD;
	}
	inline int getTARGET_AVERAGE_THRESHOLD()
	{
		return m_iTARGET_AVERAGE_THRESHOLD;
	}
	inline int getTARGET_FAVORABLE_THRESHOLD()
	{
		return m_iTARGET_FAVORABLE_THRESHOLD;
	}
	inline int getTARGET_INCREASE_WAR_TURNS()
	{
		return m_iTARGET_INCREASE_WAR_TURNS;
	}
	inline int getTARGET_MILITARY_STRENGTH_IMMENSE()
	{
		return m_iTARGET_MILITARY_STRENGTH_IMMENSE;
	}
	inline int getTARGET_MILITARY_STRENGTH_POWERFUL()
	{
		return m_iTARGET_MILITARY_STRENGTH_POWERFUL;
	}
	inline int getTARGET_MILITARY_STRENGTH_STRONG()
	{
		return m_iTARGET_MILITARY_STRENGTH_STRONG;
	}
	inline int getTARGET_MILITARY_STRENGTH_AVERAGE()
	{
		return m_iTARGET_MILITARY_STRENGTH_AVERAGE;
	}
	inline int getTARGET_MILITARY_STRENGTH_POOR()
	{
		return m_iTARGET_MILITARY_STRENGTH_POOR;
	}
	inline int getTARGET_MILITARY_STRENGTH_WEAK()
	{
		return m_iTARGET_MILITARY_STRENGTH_WEAK;
	}
	inline int getTARGET_MILITARY_STRENGTH_PATHETIC()
	{
		return m_iTARGET_MILITARY_STRENGTH_PATHETIC;
	}
	inline int getTARGET_ALREADY_WAR_EACH_PLAYER()
	{
		return m_iTARGET_ALREADY_WAR_EACH_PLAYER;
	}
	inline int getTARGET_NEIGHBORS()
	{
		return m_iTARGET_NEIGHBORS;
	}
	inline int getTARGET_CLOSE()
	{
		return m_iTARGET_CLOSE;
	}
	inline int getTARGET_FAR()
	{
		return m_iTARGET_FAR;
	}
	inline int getTARGET_DISTANT()
	{
		return m_iTARGET_DISTANT;
	}
	inline int getTARGET_MINOR_BACKUP_PATHETIC()
	{
		return m_iTARGET_MINOR_BACKUP_PATHETIC;
	}
	inline int getTARGET_MINOR_BACKUP_WEAK()
	{
		return m_iTARGET_MINOR_BACKUP_WEAK;
	}
	inline int getTARGET_MINOR_BACKUP_POOR()
	{
		return m_iTARGET_MINOR_BACKUP_POOR;
	}
	inline int getTARGET_MINOR_BACKUP_AVERAGE()
	{
		return m_iTARGET_MINOR_BACKUP_AVERAGE;
	}
	inline int getTARGET_MINOR_BACKUP_STRONG()
	{
		return m_iTARGET_MINOR_BACKUP_STRONG;
	}
	inline int getTARGET_MINOR_BACKUP_POWERFUL()
	{
		return m_iTARGET_MINOR_BACKUP_POWERFUL;
	}
	inline int getTARGET_MINOR_BACKUP_IMMENSE()
	{
		return m_iTARGET_MINOR_BACKUP_IMMENSE;
	}
	inline int getTARGET_MINOR_BACKUP_DISTANT()
	{
		return m_iTARGET_MINOR_BACKUP_DISTANT;
	}
	inline int getTARGET_MINOR_BACKUP_FAR()
	{
		return m_iTARGET_MINOR_BACKUP_FAR;
	}
	inline int getTARGET_MINOR_BACKUP_CLOSE()
	{
		return m_iTARGET_MINOR_BACKUP_CLOSE;
	}
	inline int getTARGET_MINOR_BACKUP_NEIGHBORS()
	{
		return m_iTARGET_MINOR_BACKUP_NEIGHBORS;
	}
	inline int getGAME_THREAT_CRITICAL_THRESHOLD()
	{
		return m_iGAME_THREAT_CRITICAL_THRESHOLD;
	}
	inline int getGAME_THREAT_SEVERE_THRESHOLD()
	{
		return m_iGAME_THREAT_SEVERE_THRESHOLD;
	}
	inline int getGAME_THREAT_MAJOR_THRESHOLD()
	{
		return m_iGAME_THREAT_MAJOR_THRESHOLD;
	}
	inline int getGAME_THREAT_MINOR_THRESHOLD()
	{
		return m_iGAME_THREAT_MINOR_THRESHOLD;
	}
	inline int getGAME_THREAT_AT_WAR_STALEMATE()
	{
		return m_iGAME_THREAT_AT_WAR_STALEMATE;
	}
	inline int getGAME_THREAT_AT_WAR_DEFENSIVE()
	{
		return m_iGAME_THREAT_AT_WAR_DEFENSIVE;
	}
	inline int getGAME_THREAT_AT_WAR_NEARLY_DEFEATED()
	{
		return m_iGAME_THREAT_AT_WAR_NEARLY_DEFEATED;
	}
	inline int getGAME_THREAT_AT_WAR_MILITARY_STRENGTH_IMMENSE()
	{
		return m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_IMMENSE;
	}
	inline int getGAME_THREAT_AT_WAR_MILITARY_STRENGTH_POWERFUL()
	{
		return m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_POWERFUL;
	}
	inline int getGAME_THREAT_AT_WAR_MILITARY_STRENGTH_STRONG()
	{
		return m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_STRONG;
	}
	inline int getGAME_THREAT_AT_WAR_MILITARY_STRENGTH_AVERAGE()
	{
		return m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_AVERAGE;
	}
	inline int getGAME_THREAT_GUESS_AT_WAR()
	{
		return m_iGAME_THREAT_GUESS_AT_WAR;
	}
	inline int getGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_IMMENSE()
	{
		return m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_IMMENSE;
	}
	inline int getGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_POWERFUL()
	{
		return m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_POWERFUL;
	}
	inline int getGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_STRONG()
	{
		return m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_STRONG;
	}
	inline int getGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_AVERAGE()
	{
		return m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_AVERAGE;
	}
	inline int getMILITARY_THREAT_CRITICAL_THRESHOLD()
	{
		return m_iMILITARY_THREAT_CRITICAL_THRESHOLD;
	}
	inline int getMILITARY_THREAT_SEVERE_THRESHOLD()
	{
		return m_iMILITARY_THREAT_SEVERE_THRESHOLD;
	}
	inline int getMILITARY_THREAT_MAJOR_THRESHOLD()
	{
		return m_iMILITARY_THREAT_MAJOR_THRESHOLD;
	}
	inline int getMILITARY_THREAT_MINOR_THRESHOLD()
	{
		return m_iMILITARY_THREAT_MINOR_THRESHOLD;
	}
	inline int getMILITARY_THREAT_STRENGTH_IMMENSE()
	{
		return m_iMILITARY_THREAT_STRENGTH_IMMENSE;
	}
	inline int getMILITARY_THREAT_STRENGTH_POWERFUL()
	{
		return m_iMILITARY_THREAT_STRENGTH_POWERFUL;
	}
	inline int getMILITARY_THREAT_STRENGTH_STRONG()
	{
		return m_iMILITARY_THREAT_STRENGTH_STRONG;
	}
	inline int getMILITARY_THREAT_STRENGTH_AVERAGE()
	{
		return m_iMILITARY_THREAT_STRENGTH_AVERAGE;
	}
	inline int getMILITARY_THREAT_STRENGTH_POOR()
	{
		return m_iMILITARY_THREAT_STRENGTH_POOR;
	}
	inline int getMILITARY_THREAT_STRENGTH_WEAK()
	{
		return m_iMILITARY_THREAT_STRENGTH_WEAK;
	}
	inline int getMILITARY_THREAT_STRENGTH_PATHETIC()
	{
		return m_iMILITARY_THREAT_STRENGTH_PATHETIC;
	}
	inline int getMILITARY_THREAT_WAR_STATE_CALM()
	{
		return m_iMILITARY_THREAT_WAR_STATE_CALM;
	}
	inline int getMILITARY_THREAT_WAR_STATE_NEARLY_WON()
	{
		return m_iMILITARY_THREAT_WAR_STATE_NEARLY_WON;
	}
	inline int getMILITARY_THREAT_WAR_STATE_OFFENSIVE()
	{
		return m_iMILITARY_THREAT_WAR_STATE_OFFENSIVE;
	}
	inline int getMILITARY_THREAT_WAR_STATE_STALEMATE()
	{
		return m_iMILITARY_THREAT_WAR_STATE_STALEMATE;
	}
	inline int getMILITARY_THREAT_WAR_STATE_DEFENSIVE()
	{
		return m_iMILITARY_THREAT_WAR_STATE_DEFENSIVE;
	}
	inline int getMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED()
	{
		return m_iMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED;
	}
	inline int getMILITARY_THREAT_NEIGHBORS()
	{
		return m_iMILITARY_THREAT_NEIGHBORS;
	}
	inline int getMILITARY_THREAT_CLOSE()
	{
		return m_iMILITARY_THREAT_CLOSE;
	}
	inline int getMILITARY_THREAT_FAR()
	{
		return m_iMILITARY_THREAT_FAR;
	}
	inline int getMILITARY_THREAT_DISTANT()
	{
		return m_iMILITARY_THREAT_DISTANT;
	}
	inline int getMILITARY_THREAT_PER_MINOR_ATTACKED()
	{
		return m_iMILITARY_THREAT_PER_MINOR_ATTACKED;
	}
	inline int getMILITARY_THREAT_PER_MINOR_CONQUERED()
	{
		return m_iMILITARY_THREAT_PER_MINOR_CONQUERED;
	}
	inline int getMILITARY_THREAT_PER_MAJOR_ATTACKED()
	{
		return m_iMILITARY_THREAT_PER_MAJOR_ATTACKED;
	}
	inline int getMILITARY_THREAT_PER_MAJOR_CONQUERED()
	{
		return m_iMILITARY_THREAT_PER_MAJOR_CONQUERED;
	}
	inline int getMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER()
	{
		return m_iMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER;
	}
	inline int getWARMONGER_THREAT_CRITICAL_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_CRITICAL_THRESHOLD;
	}
	inline int getWARMONGER_THREAT_SEVERE_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_SEVERE_THRESHOLD;
	}
	inline int getWARMONGER_THREAT_MAJOR_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_MAJOR_THRESHOLD;
	}
	inline int getWARMONGER_THREAT_MINOR_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_MINOR_THRESHOLD;
	}
	inline int getWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT()
	{
		return m_iWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT;
	}
	inline int getWARMONGER_THREAT_MINOR_CONQUERED_WEIGHT()
	{
		return m_iWARMONGER_THREAT_MINOR_CONQUERED_WEIGHT;
	}
	inline int getWARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT()
	{
		return m_iWARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT;
	}
	inline int getWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT()
	{
		return m_iWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT;
	}
#if defined(MOD_CONFIG_AI_IN_XML)
	GD_INT_DEF(WARMONGER_THREAT_MAJOR_CITY_WEIGHT)
	GD_INT_DEF(WARMONGER_THREAT_MINOR_CITY_WEIGHT)
	GD_INT_DEF(WARMONGER_THREAT_CAPITAL_CITY_PERCENT)
	GD_INT_DEF(WARMONGER_THREAT_KNOWS_ATTACKER_PERCENT)
	GD_INT_DEF(WARMONGER_THREAT_KNOWS_DEFENDER_PERCENT)
	GD_INT_DEF(WARMONGER_THREAT_AGGRIEVED_PERCENT)
	GD_INT_DEF(WARMONGER_THREAT_COOP_WAR_PERCENT)
	GD_INT_DEF(WARMONGER_THREAT_DEF_PACT_ENABLED)
	GD_INT_DEF(WARMONGER_THREAT_CITY_SIZE_ENABLED)
	GD_INT_DEF(WARMONGER_THREAT_MODIFIER_NEGATIVE_MEDIUM)
	GD_INT_DEF(WARMONGER_THREAT_MODIFIER_MEDIUM)
	GD_INT_DEF(WARMONGER_THREAT_MODIFIER_LARGE)
	GD_INT_DEF(WARMONGER_THREAT_MODIFIER_SMALL)
	GD_INT_DEF(WARMONGER_THREAT_MODIFIER_NEGATIVE_SMALL)
	GD_INT_DEF(WARMONGER_THREAT_APPROACH_DECAY_LARGE)
	GD_INT_DEF(WARMONGER_THREAT_APPROACH_DECAY_SMALL)
	GD_INT_DEF(WARMONGER_THREAT_APPROACH_DECAY_MEDIUM)
#endif
	inline int getWARMONGER_THREAT_PER_TURN_DECAY()
	{
		return m_iWARMONGER_THREAT_PER_TURN_DECAY;
	}
	inline int getWARMONGER_THREAT_PERSONALITY_MOD()
	{
		return m_iWARMONGER_THREAT_PERSONALITY_MOD;
	}
	inline int getWARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD;
	}
	inline int getWARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD()
	{
		return m_iWARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD;
	}
	inline int getWARMONGER_ON_CITY_STATE_MULTIPLIER()
	{
		return m_iWARMONGER_ON_CITY_STATE_MULTIPLIER;
	}
	inline int getLAND_DISPUTE_FIERCE_THRESHOLD()
	{
		return m_iLAND_DISPUTE_FIERCE_THRESHOLD;
	}
	inline int getLAND_DISPUTE_STRONG_THRESHOLD()
	{
		return m_iLAND_DISPUTE_STRONG_THRESHOLD;
	}
	inline int getLAND_DISPUTE_WEAK_THRESHOLD()
	{
		return m_iLAND_DISPUTE_WEAK_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_FIERCE_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_FIERCE_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_STRONG_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_STRONG_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_WEAK_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_WEAK_THRESHOLD;
	}
	inline int getWONDER_DISPUTE_FIERCE_THRESHOLD()
	{
		return m_iWONDER_DISPUTE_FIERCE_THRESHOLD;
	}
	inline int getWONDER_DISPUTE_STRONG_THRESHOLD()
	{
		return m_iWONDER_DISPUTE_STRONG_THRESHOLD;
	}
	inline int getWONDER_DISPUTE_WEAK_THRESHOLD()
	{
		return m_iWONDER_DISPUTE_WEAK_THRESHOLD;
	}
	inline int getMINOR_CIV_DISPUTE_FIERCE_THRESHOLD()
	{
		return m_iMINOR_CIV_DISPUTE_FIERCE_THRESHOLD;
	}
	inline int getMINOR_CIV_DISPUTE_STRONG_THRESHOLD()
	{
		return m_iMINOR_CIV_DISPUTE_STRONG_THRESHOLD;
	}
	inline int getMINOR_CIV_DISPUTE_WEAK_THRESHOLD()
	{
		return m_iMINOR_CIV_DISPUTE_WEAK_THRESHOLD;
	}
	inline int getMINOR_CIV_DISPUTE_ALLIES_WEIGHT()
	{
		return m_iMINOR_CIV_DISPUTE_ALLIES_WEIGHT;
	}
	inline int getMINOR_CIV_DISPUTE_FRIENDS_WEIGHT()
	{
		return m_iMINOR_CIV_DISPUTE_FRIENDS_WEIGHT;
	}
	inline int getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_NONE()
	{
		return m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_NONE;
	}
	inline int getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_LOW()
	{
		return m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_LOW;
	}
	inline int getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_MEDIUM()
	{
		return m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_MEDIUM;
	}
	inline int getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_HIGH()
	{
		return m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_HIGH;
	}
	inline int getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_INCREDIBLE()
	{
		return m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_INCREDIBLE;
	}
	inline int getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_NONE()
	{
		return m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_NONE;
	}
	inline int getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_LOW()
	{
		return m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_LOW;
	}
	inline int getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_MEDIUM()
	{
		return m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_MEDIUM;
	}
	inline int getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_HIGH()
	{
		return m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_HIGH;
	}
	inline int getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_INCREDIBLE()
	{
		return m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_INCREDIBLE;
	}
	inline int getLAND_DISPUTE_DISTANT()
	{
		return m_iLAND_DISPUTE_DISTANT;
	}
	inline int getLAND_DISPUTE_FAR()
	{
		return m_iLAND_DISPUTE_FAR;
	}
	inline int getLAND_DISPUTE_CLOSE()
	{
		return m_iLAND_DISPUTE_CLOSE;
	}
	inline int getLAND_DISPUTE_NEIGHBORS()
	{
		return m_iLAND_DISPUTE_NEIGHBORS;
	}
	inline int getLAND_DISPUTE_CRAMPED_MULTIPLIER()
	{
		return m_iLAND_DISPUTE_CRAMPED_MULTIPLIER;
	}
	inline int getLAND_DISPUTE_NO_EXPANSION_STRATEGY()
	{
		return m_iLAND_DISPUTE_NO_EXPANSION_STRATEGY;
	}
	inline int getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE()
	{
		return m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE;
	}
	inline int getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY()
	{
		return m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY;
	}
	inline int getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE()
	{
		return m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_10()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_10;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_9()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_9;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_8()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_8;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_7()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_7;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_6()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_6;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_5()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_5;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_4()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_4;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_3()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_3;
	}
	inline int getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_2()
	{
		return m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_2;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_FIERCE_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_FIERCE_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_STRONG_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_STRONG_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_WEAK_THRESHOLD()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_WEAK_THRESHOLD;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY;
	}
	inline int getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE()
	{
		return m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE;
	}
	inline int getIC_MEMORY_TURN_EXPIRATION()
	{
		return m_iIC_MEMORY_TURN_EXPIRATION;
	}
	inline int getSTOP_SPYING_MEMORY_TURN_EXPIRATION()
	{
		return m_iSTOP_SPYING_MEMORY_TURN_EXPIRATION;
	}
	inline int getMOVE_TROOPS_MEMORY_TURN_EXPIRATION()
	{
		return m_iMOVE_TROOPS_MEMORY_TURN_EXPIRATION;
	}
	inline int getCOOP_WAR_SOON_COUNTER()
	{
		return m_iCOOP_WAR_SOON_COUNTER;
	}
	inline int getCOOP_WAR_LOCKED_TURNS()
	{
		return m_iCOOP_WAR_LOCKED_TURNS;
	}
	inline int getCOOP_WAR_LOCKED_TURNS_WAR_WEIGHT()
	{
		return m_iCOOP_WAR_LOCKED_TURNS_WAR_WEIGHT;
	}
	inline int getDEMAND_TURN_LIMIT_MIN()
	{
		return m_iDEMAND_TURN_LIMIT_MIN;
	}
	inline int getDEMAND_TURN_LIMIT_RAND()
	{
		return m_iDEMAND_TURN_LIMIT_RAND;
	}
	inline int getDONT_SETTLE_RAND()
	{
		return m_iDONT_SETTLE_RAND;
	}
	inline int getDONT_SETTLE_FLAVOR_BASE()
	{
		return m_iDONT_SETTLE_FLAVOR_BASE;
	}
	inline int getDONT_SETTLE_FLAVOR_MULTIPLIER()
	{
		return m_iDONT_SETTLE_FLAVOR_MULTIPLIER;
	}
	inline int getDONT_SETTLE_FRIENDLY()
	{
		return m_iDONT_SETTLE_FRIENDLY;
	}
	inline int getDONT_SETTLE_STRENGTH_PATHETIC()
	{
		return m_iDONT_SETTLE_STRENGTH_PATHETIC;
	}
	inline int getDONT_SETTLE_STRENGTH_WEAK()
	{
		return m_iDONT_SETTLE_STRENGTH_WEAK;
	}
	inline int getDONT_SETTLE_STRENGTH_POOR()
	{
		return m_iDONT_SETTLE_STRENGTH_POOR;
	}
	inline int getDONT_SETTLE_STRENGTH_AVERAGE()
	{
		return m_iDONT_SETTLE_STRENGTH_AVERAGE;
	}
	inline int getDONT_SETTLE_STRENGTH_STRONG()
	{
		return m_iDONT_SETTLE_STRENGTH_STRONG;
	}
	inline int getDONT_SETTLE_STRENGTH_POWERFUL()
	{
		return m_iDONT_SETTLE_STRENGTH_POWERFUL;
	}
	inline int getDONT_SETTLE_STRENGTH_IMMENSE()
	{
		return m_iDONT_SETTLE_STRENGTH_IMMENSE;
	}
	inline int getDONT_SETTLE_MOD_MILITARY_POSTURE_NONE()
	{
		return m_iDONT_SETTLE_MOD_MILITARY_POSTURE_NONE;
	}
	inline int getDONT_SETTLE_MOD_MILITARY_POSTURE_LOW()
	{
		return m_iDONT_SETTLE_MOD_MILITARY_POSTURE_LOW;
	}
	inline int getDONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM()
	{
		return m_iDONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM;
	}
	inline int getDONT_SETTLE_MOD_MILITARY_POSTURE_HIGH()
	{
		return m_iDONT_SETTLE_MOD_MILITARY_POSTURE_HIGH;
	}
	inline int getDONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE()
	{
		return m_iDONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE;
	}
	inline int getDOF_TURN_BUFFER()
	{
		return m_iDOF_TURN_BUFFER;
	}
	inline int getDOF_THRESHOLD()
	{
		return m_iDOF_THRESHOLD;
	}
	inline int getDENUNCIATION_EXPIRATION_TIME()
	{
		return m_iDENUNCIATION_EXPIRATION_TIME;
	}
	inline int getDOF_EXPIRATION_TIME()
	{
		return m_iDOF_EXPIRATION_TIME;
	}
	inline int getEACH_GOLD_VALUE_PERCENT()
	{
		return m_iEACH_GOLD_VALUE_PERCENT;
	}
	inline int getEACH_GOLD_PER_TURN_VALUE_PERCENT()
	{
		return m_iEACH_GOLD_PER_TURN_VALUE_PERCENT;
	}
	inline int getEXPANSION_BICKER_TIMEOUT()
	{
		return m_iEXPANSION_BICKER_TIMEOUT;
	}
	inline int getEXPANSION_PROMISE_TURNS_EFFECTIVE()
	{
		return m_iEXPANSION_PROMISE_TURNS_EFFECTIVE;
	}
	inline int getEXPANSION_PROMISE_BROKEN_PER_TURN_DECAY()
	{
		return m_iEXPANSION_PROMISE_BROKEN_PER_TURN_DECAY;
	}
	inline int getBROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT()
	{
		return m_iBROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT;
	}
	inline int getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX()
	{
		return m_iOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX;
	}
	inline int getEXPANSION_PROMISE_IGNORED_PER_TURN_DECAY()
	{
		return m_iEXPANSION_PROMISE_IGNORED_PER_TURN_DECAY;
	}
	inline int getIGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT()
	{
		return m_iIGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT;
	}
	inline int getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX()
	{
		return m_iOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX;
	}

	inline int getBORDER_PROMISE_TURNS_EFFECTIVE()
	{
		return m_iBORDER_PROMISE_TURNS_EFFECTIVE;
	}
	inline int getBORDER_PROMISE_BROKEN_PER_TURN_DECAY()
	{
		return m_iBORDER_PROMISE_BROKEN_PER_TURN_DECAY;
	}
	inline int getBROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT()
	{
		return m_iBROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT;
	}
	inline int getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX()
	{
		return m_iOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX;
	}
	inline int getBORDER_PROMISE_IGNORED_PER_TURN_DECAY()
	{
		return m_iBORDER_PROMISE_IGNORED_PER_TURN_DECAY;
	}
	inline int getIGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT()
	{
		return m_iIGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT;
	}
	inline int getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX()
	{
		return m_iOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX;
	}

	inline int getDECLARED_WAR_ON_FRIEND_PER_TURN_DECAY()
	{
		return m_iDECLARED_WAR_ON_FRIEND_PER_TURN_DECAY;
	}
	inline int getDECLARED_WAR_ON_FRIEND_PER_OPINION_WEIGHT()
	{
		return m_iDECLARED_WAR_ON_FRIEND_PER_OPINION_WEIGHT;
	}

	inline int getDEAL_VALUE_PER_TURN_DECAY()
	{
		return m_iDEAL_VALUE_PER_TURN_DECAY;
	}
	inline int getDEAL_VALUE_PER_OPINION_WEIGHT()
	{
		return m_iDEAL_VALUE_PER_OPINION_WEIGHT;
	}
	inline int getCOMMON_FOE_VALUE_PER_TURN_DECAY()
	{
		return m_iCOMMON_FOE_VALUE_PER_TURN_DECAY;
	}
	inline int getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT()
	{
		return m_iCOMMON_FOE_VALUE_PER_OPINION_WEIGHT;
	}
	inline int getWANT_RESEARCH_AGREEMENT_RAND()
	{
		return m_iWANT_RESEARCH_AGREEMENT_RAND;
	}
	inline int getDEMAND_RAND()
	{
		return m_iDEMAND_RAND;
	}
	inline int getCAN_WORK_WATER_FROM_GAME_START()
	{
		return m_iCAN_WORK_WATER_FROM_GAME_START;
	}
	inline int getNAVAL_PLOT_BLOCKADE_RANGE()
	{
		return m_iNAVAL_PLOT_BLOCKADE_RANGE;
	}
	inline int getEVENT_MESSAGE_TIME()
	{
		return m_iEVENT_MESSAGE_TIME;
	}
	inline int getSTART_YEAR()
	{
		return m_iSTART_YEAR;
	}
	inline int getWEEKS_PER_MONTHS()
	{
		return m_iWEEKS_PER_MONTHS;
	}
	inline int getHIDDEN_START_TURN_OFFSET()
	{
		return m_iHIDDEN_START_TURN_OFFSET;
	}
	inline int getRECON_VISIBILITY_RANGE()
	{
		return m_iRECON_VISIBILITY_RANGE;
	}
	inline int getPLOT_VISIBILITY_RANGE()
	{
		return m_iPLOT_VISIBILITY_RANGE;
	}
	inline int getUNIT_VISIBILITY_RANGE()
	{
		return m_iUNIT_VISIBILITY_RANGE;
	}
	inline int getAIR_UNIT_REBASE_RANGE_MULTIPLIER()
	{
		return m_iAIR_UNIT_REBASE_RANGE_MULTIPLIER;
	}
	inline int getMOUNTAIN_SEE_FROM_CHANGE()
	{
		return m_iMOUNTAIN_SEE_FROM_CHANGE;
	}
	inline int getMOUNTAIN_SEE_THROUGH_CHANGE()
	{
		return m_iMOUNTAIN_SEE_THROUGH_CHANGE;
	}
	inline int getHILLS_SEE_FROM_CHANGE()
	{
		return m_iHILLS_SEE_FROM_CHANGE;
	}
	inline int getHILLS_SEE_THROUGH_CHANGE()
	{
		return m_iHILLS_SEE_THROUGH_CHANGE;
	}
	inline int getSEAWATER_SEE_FROM_CHANGE()
	{
		return m_iSEAWATER_SEE_FROM_CHANGE;
	}
	inline int getSEAWATER_SEE_THROUGH_CHANGE()
	{
		return m_iSEAWATER_SEE_THROUGH_CHANGE;
	}
	inline int getMAX_YIELD_STACK()
	{
		return m_iMAX_YIELD_STACK;
	}
	inline int getMOVE_DENOMINATOR()
	{
		return m_iMOVE_DENOMINATOR;
	}
	inline int getSTARTING_DISTANCE_PERCENT()
	{
		return m_iSTARTING_DISTANCE_PERCENT;
	}
	inline int getMIN_CIV_STARTING_DISTANCE()
	{
		return m_iMIN_CIV_STARTING_DISTANCE;
	}
	inline int getMIN_CITY_RANGE()
	{
		return m_iMIN_CITY_RANGE;
	}
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DEF(CITY_STARTING_RINGS)
#endif
	inline int getOWNERSHIP_SCORE_DURATION_THRESHOLD()
	{
		return m_iOWNERSHIP_SCORE_DURATION_THRESHOLD;
	}
	inline int getNUM_POLICY_BRANCHES_ALLOWED()
	{
		return m_iNUM_POLICY_BRANCHES_ALLOWED;
	}
	inline int getVICTORY_POINTS_NEEDED_TO_WIN()
	{
		return m_iVICTORY_POINTS_NEEDED_TO_WIN;
	}
	inline int getNUM_VICTORY_POINT_AWARDS()
	{
		return m_iNUM_VICTORY_POINT_AWARDS;
	}
	inline int getNUM_OR_TECH_PREREQS()
	{
		return m_iNUM_OR_TECH_PREREQS;
	}
	inline int getNUM_AND_TECH_PREREQS()
	{
		return m_iNUM_AND_TECH_PREREQS;
	}
	inline int getNUM_UNIT_AND_TECH_PREREQS()
	{
		return m_iNUM_UNIT_AND_TECH_PREREQS;
	}
	inline int getNUM_BUILDING_AND_TECH_PREREQS()
	{
		return m_iNUM_BUILDING_AND_TECH_PREREQS;
	}
	inline int getNUM_BUILDING_RESOURCE_PREREQS()
	{
		return m_iNUM_BUILDING_RESOURCE_PREREQS;
	}
	inline int getBASE_RESEARCH_RATE()
	{
		return m_iBASE_RESEARCH_RATE;
	}
	inline int getMAX_WORLD_WONDERS_PER_CITY()
	{
		return m_iMAX_WORLD_WONDERS_PER_CITY;
	}
	inline int getMAX_TEAM_WONDERS_PER_CITY()
	{
		return m_iMAX_TEAM_WONDERS_PER_CITY;
	}
	inline int getMAX_NATIONAL_WONDERS_PER_CITY()
	{
		return m_iMAX_NATIONAL_WONDERS_PER_CITY;
	}
	inline int getMAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC()
	{
		return m_iMAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC;
	}
	inline int getMAX_BUILDINGS_PER_CITY()
	{
		return m_iMAX_BUILDINGS_PER_CITY;
	}
	inline int getINITIAL_CITY_POPULATION()
	{
		return m_iINITIAL_CITY_POPULATION;
	}
	inline int getBASE_CITY_AIR_STACKING()
	{
		return m_iBASE_CITY_AIR_STACKING;
	}
	inline int getCITY_CAPTURE_POPULATION_PERCENT()
	{
		return m_iCITY_CAPTURE_POPULATION_PERCENT;
	}
	inline int getBASE_CITY_GROWTH_THRESHOLD()
	{
		return m_iBASE_CITY_GROWTH_THRESHOLD;
	}
	inline int getFOOD_CONSUMPTION_PER_POPULATION()
	{
		return m_iFOOD_CONSUMPTION_PER_POPULATION;
	}
	inline int getHAPPINESS_PER_CITY_WITH_STATE_RELIGION()
	{
		return m_iHAPPINESS_PER_CITY_WITH_STATE_RELIGION;
	}
	inline int getHAPPINESS_PER_NATURAL_WONDER()
	{
		return m_iHAPPINESS_PER_NATURAL_WONDER;
	}
	inline int getHAPPINESS_PER_EXTRA_LUXURY()
	{
		return m_iHAPPINESS_PER_EXTRA_LUXURY;
	}
	inline int getUNHAPPINESS_PER_POPULATION()
	{
		return m_iUNHAPPINESS_PER_POPULATION;
	}
	inline float getUNHAPPINESS_PER_OCCUPIED_POPULATION()
	{
		return m_fUNHAPPINESS_PER_OCCUPIED_POPULATION;
	}
	inline int getUNHAPPINESS_PER_CITY()
	{
		return m_iUNHAPPINESS_PER_CITY;
	}
	inline int getUNHAPPINESS_PER_CAPTURED_CITY()
	{
		return m_iUNHAPPINESS_PER_CAPTURED_CITY;
	}
	inline int getUNHAPPY_GROWTH_PENALTY()
	{
		return m_iUNHAPPY_GROWTH_PENALTY;
	}
	inline int getVERY_UNHAPPY_GROWTH_PENALTY()
	{
		return m_iVERY_UNHAPPY_GROWTH_PENALTY;
	}
	inline int getVERY_UNHAPPY_CANT_TRAIN_SETTLERS()
	{
		return m_iVERY_UNHAPPY_CANT_TRAIN_SETTLERS;
	}
	inline int getVERY_UNHAPPY_THRESHOLD()
	{
		return m_iVERY_UNHAPPY_THRESHOLD;
	}
	inline int getSUPER_UNHAPPY_THRESHOLD()
	{
		return m_iSUPER_UNHAPPY_THRESHOLD;
	}
	inline int getUPRISING_COUNTER_MIN()
	{
		return m_iUPRISING_COUNTER_MIN;
	}
	inline int getUPRISING_COUNTER_POSSIBLE()
	{
		return m_iUPRISING_COUNTER_POSSIBLE;
	}
	inline int getUPRISING_NUM_BASE()
	{
		return m_iUPRISING_NUM_BASE;
	}
	inline int getUPRISING_NUM_CITY_COUNT()
	{
		return m_iUPRISING_NUM_CITY_COUNT;
	}
	inline int getREVOLT_COUNTER_MIN()
	{
		return m_iREVOLT_COUNTER_MIN;
	}
	inline int getVERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY()
	{
		return m_iVERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY;
	}
	inline int getVERY_UNHAPPY_MAX_COMBAT_PENALTY()
	{
		return m_iVERY_UNHAPPY_MAX_COMBAT_PENALTY;
	}
	inline int getSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY()
	{
		return m_iSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY;
	}
	inline int getVERY_UNHAPPY_PRODUCTION_PENALTY_PER_UNHAPPY()
	{
		return m_iVERY_UNHAPPY_PRODUCTION_PENALTY_PER_UNHAPPY;
	}
	inline int getVERY_UNHAPPY_MAX_PRODUCTION_PENALTY()
	{
		return m_iVERY_UNHAPPY_MAX_PRODUCTION_PENALTY;
	}
	inline int getVERY_UNHAPPY_GOLD_PENALTY_PER_UNHAPPY()
	{
		return m_iVERY_UNHAPPY_GOLD_PENALTY_PER_UNHAPPY;
	}
	inline int getVERY_UNHAPPY_MAX_GOLD_PENALTY()
	{
		return m_iVERY_UNHAPPY_MAX_GOLD_PENALTY;
	}
	inline int getWLTKD_GROWTH_MULTIPLIER()
	{
		return m_iWLTKD_GROWTH_MULTIPLIER;
	}
	inline int getINDUSTRIAL_ROUTE_PRODUCTION_MOD()
	{
		return m_iINDUSTRIAL_ROUTE_PRODUCTION_MOD;
	}
	inline int getRESOURCE_DEMAND_COUNTDOWN_BASE()
	{
		return m_iRESOURCE_DEMAND_COUNTDOWN_BASE;
	}
	inline int getRESOURCE_DEMAND_COUNTDOWN_CAPITAL_ADD()
	{
		return m_iRESOURCE_DEMAND_COUNTDOWN_CAPITAL_ADD;
	}
	inline int getRESOURCE_DEMAND_COUNTDOWN_RAND()
	{
		return m_iRESOURCE_DEMAND_COUNTDOWN_RAND;
	}
	inline int getNEW_HURRY_MODIFIER()
	{
		return m_iNEW_HURRY_MODIFIER;
	}
	inline int getGREAT_GENERAL_RANGE()
	{
		return m_iGREAT_GENERAL_RANGE;
	}
	inline int getGREAT_GENERAL_STRENGTH_MOD()
	{
		return m_iGREAT_GENERAL_STRENGTH_MOD;
	}
	inline int getBONUS_PER_ADJACENT_FRIEND()
	{
		return m_iBONUS_PER_ADJACENT_FRIEND;
	}
	inline int getPOLICY_ATTACK_BONUS_MOD()
	{
		return m_iPOLICY_ATTACK_BONUS_MOD;
	}
	inline int getCONSCRIPT_MIN_CITY_POPULATION()
	{
		return m_iCONSCRIPT_MIN_CITY_POPULATION;
	}
	inline int getCONSCRIPT_POPULATION_PER_COST()
	{
		return m_iCONSCRIPT_POPULATION_PER_COST;
	}
	inline int getMIN_TIMER_UNIT_DOUBLE_MOVES()
	{
		return m_iMIN_TIMER_UNIT_DOUBLE_MOVES;
	}
	inline int getCOMBAT_DAMAGE()
	{
		return m_iCOMBAT_DAMAGE;
	}
	inline int getNONCOMBAT_UNIT_RANGED_DAMAGE()
	{
		return m_iNONCOMBAT_UNIT_RANGED_DAMAGE;
	}
	inline int getNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER()
	{
		return m_iNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER;
	}
	inline int getLAKE_PLOT_RAND()
	{
		return m_iLAKE_PLOT_RAND;
	}
	inline int getPLOTS_PER_RIVER_EDGE()
	{
		return m_iPLOTS_PER_RIVER_EDGE;
	}
	inline int getRIVER_SOURCE_MIN_RIVER_RANGE()
	{
		return m_iRIVER_SOURCE_MIN_RIVER_RANGE;
	}
	inline int getRIVER_SOURCE_MIN_SEAWATER_RANGE()
	{
		return m_iRIVER_SOURCE_MIN_SEAWATER_RANGE;
	}
	inline int getLAKE_MAX_AREA_SIZE()
	{
		return m_iLAKE_MAX_AREA_SIZE;
	}
	inline int getINITIAL_GOLD_PER_UNIT_TIMES_100()
	{
		return m_iINITIAL_GOLD_PER_UNIT_TIMES_100;
	}
	inline int getINITIAL_FREE_OUTSIDE_UNITS()
	{
		return m_iINITIAL_FREE_OUTSIDE_UNITS;
	}
	inline int getINITIAL_OUTSIDE_UNIT_GOLD_PERCENT()
	{
		return m_iINITIAL_OUTSIDE_UNIT_GOLD_PERCENT;
	}
	inline int getUNIT_MAINTENANCE_GAME_MULTIPLIER()
	{
		return m_iUNIT_MAINTENANCE_GAME_MULTIPLIER;
	}
	inline int getUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR()
	{
		return m_iUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR;
	}
	inline int getFREE_UNIT_HAPPINESS()
	{
		return m_iFREE_UNIT_HAPPINESS;
	}
	inline int getTRADE_ROUTE_BASE_GOLD()
	{
		return m_iTRADE_ROUTE_BASE_GOLD;
	}
#if defined(MOD_TRADE_ROUTE_SCALING)
	GD_INT_DEF(TRADE_ROUTE_BASE_TARGET_TURNS)
	GD_INT_DEF(TRADE_ROUTE_BASE_LAND_DISTANCE)
	GD_INT_DEF(TRADE_ROUTE_BASE_LAND_MODIFIER)
	GD_INT_DEF(TRADE_ROUTE_BASE_SEA_DISTANCE)
	GD_INT_DEF(TRADE_ROUTE_BASE_SEA_MODIFIER)
	GD_INT_DEF(TRADE_ROUTE_BASE_FOOD_VALUE)
	GD_INT_DEF(TRADE_ROUTE_BASE_PRODUCTION_VALUE)
	GD_INT_DEF(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100)
	GD_INT_DEF(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100)
	GD_INT_DEF(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE)
	GD_INT_DEF(TRADE_ROUTE_RIVER_CITY_MODIFIER)
	GD_INT_DEF(TRADE_ROUTE_BASE_PLUNDER_GOLD)
	GD_INT_DEF(TRADE_ROUTE_PLUNDER_TURNS_COUNTER)
	GD_INT_DEF(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA)
	GD_INT_DEF(TRADE_ROUTE_CS_FRIEND_SCIENCE_DELTA)
	GD_INT_DEF(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA)
	GD_INT_DEF(TRADE_ROUTE_CS_FRIEND_CULTURE_DELTA)

	GD_INT_DEF(TRADE_ROUTE_CS_ALLY_GOLD)
	GD_INT_DEF(TRADE_ROUTE_CS_FRIEND_GOLD)
#endif
#if defined(MOD_CIV6_ROADS)
	GD_INT_DEF(TRADE_ROUTE_CREATE_RAILROADS_ERA)
	GD_INT_DEF(TRADE_ROUTE_CREATE_RAILROADS_TECH_ID)
#endif
#if defined(MOD_CITY_STATE_SCALE)
	GD_INT_DEF(CITY_STATE_SCALE_PER_CITY_MOD)
#endif
	inline int getTRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER()
	{
		return m_iTRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER;
	}
	inline int getTRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER()
	{
		return m_iTRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER;
	}
	inline int getDEFICIT_UNIT_DISBANDING_THRESHOLD()
	{
		return m_iDEFICIT_UNIT_DISBANDING_THRESHOLD;
	}
	inline int getGOLDEN_AGE_BASE_THRESHOLD_HAPPINESS()
	{
		return m_iGOLDEN_AGE_BASE_THRESHOLD_HAPPINESS;
	}
	inline float getGOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER()
	{
		return m_fGOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER;
	}
	inline int getGOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS()
	{
		return m_iGOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS;
	}
	inline int getGOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR()
	{
		return m_iGOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR;
	}
	inline int getBASE_GOLDEN_AGE_UNITS()
	{
		return m_iBASE_GOLDEN_AGE_UNITS;
	}
	inline int getGOLDEN_AGE_UNITS_MULTIPLIER()
	{
		return m_iGOLDEN_AGE_UNITS_MULTIPLIER;
	}
	inline int getGOLDEN_AGE_LENGTH()
	{
		return m_iGOLDEN_AGE_LENGTH;
	}
	inline int getGOLDEN_AGE_GREAT_PEOPLE_MODIFIER()
	{
		return m_iGOLDEN_AGE_GREAT_PEOPLE_MODIFIER;
	}
	inline int getMIN_UNIT_GOLDEN_AGE_TURNS()
	{
		return m_iMIN_UNIT_GOLDEN_AGE_TURNS;
	}
	inline int getGREAT_WORK_CULTURE_MULTIPLIER()
	{
		return m_iGREAT_WORK_CULTURE_MULTIPLIER;
	}
	inline int getTHEME_GREAT_WORK_GA_MULTIPLIER()
	{
		return m_iTHEME_GREAT_WORK_GA_MULTIPLIER;
	}	
	inline int getGOLDEN_AGE_CULTURE_MODIFIER()
	{
		return m_iGOLDEN_AGE_CULTURE_MODIFIER;
	}
	inline int getHILLS_EXTRA_MOVEMENT()
	{
		return m_iHILLS_EXTRA_MOVEMENT;
	}
	inline int getRIVER_EXTRA_MOVEMENT()
	{
		return m_iRIVER_EXTRA_MOVEMENT;
	}
	inline int getFEATURE_GROWTH_MODIFIER()
	{
		return m_iFEATURE_GROWTH_MODIFIER;
	}
	inline int getROUTE_FEATURE_GROWTH_MODIFIER()
	{
		return m_iROUTE_FEATURE_GROWTH_MODIFIER;
	}
	inline int getEXTRA_YIELD()
	{
		return m_iEXTRA_YIELD;
	}
	inline int getFORTIFY_MODIFIER_PER_TURN()
	{
		return m_iFORTIFY_MODIFIER_PER_TURN;
	}
	inline int getMAX_FORTIFY_TURNS()
	{
		return m_iMAX_FORTIFY_TURNS;
	}
	inline int getNUKE_FALLOUT_PROB()
	{
		return m_iNUKE_FALLOUT_PROB;
	}
	inline int getNUKE_UNIT_DAMAGE_BASE()
	{
		return m_iNUKE_UNIT_DAMAGE_BASE;
	}
	inline int getNUKE_UNIT_DAMAGE_RAND_1()
	{
		return m_iNUKE_UNIT_DAMAGE_RAND_1;
	}
	inline int getNUKE_UNIT_DAMAGE_RAND_2()
	{
		return m_iNUKE_UNIT_DAMAGE_RAND_2;
	}
	inline int getNUKE_NON_COMBAT_DEATH_THRESHOLD()
	{
		return m_iNUKE_NON_COMBAT_DEATH_THRESHOLD;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_BASE()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_BASE;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_RAND_1()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_1;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_RAND_2()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_2;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_BASE()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_BASE;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_RAND_1()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_1;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_RAND_2()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_2;
	}
	inline int getNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD()
	{
		return m_iNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD;
	}
	inline int getNUKE_CITY_HIT_POINT_DAMAGE()
	{
		return m_iNUKE_CITY_HIT_POINT_DAMAGE;
	}
	inline int getNUKE_BLAST_RADIUS()
	{
		return m_iNUKE_BLAST_RADIUS;
	}
	inline int getTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER()
	{
		return m_iTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER;
	}
	inline int getTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER()
	{
		return m_iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER;
	}
	inline int getTECH_COST_KNOWN_PREREQ_MODIFIER()
	{
		return m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
	}
	inline int getPEACE_TREATY_LENGTH()
	{
		return m_iPEACE_TREATY_LENGTH;
	}
	inline int getCOOP_WAR_LOCKED_LENGTH()
	{
		return m_iCOOP_WAR_LOCKED_LENGTH;
	}
	inline int getBASE_FEATURE_PRODUCTION_PERCENT()
	{
		return m_iBASE_FEATURE_PRODUCTION_PERCENT;
	}
	inline int getFEATURE_PRODUCTION_PERCENT_MULTIPLIER()
	{
		return m_iFEATURE_PRODUCTION_PERCENT_MULTIPLIER;
	}
	inline int getDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT()
	{
		return m_iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT;
	}
	inline int getDEFAULT_WAR_VALUE_FOR_UNIT()
	{
		return m_iDEFAULT_WAR_VALUE_FOR_UNIT;
	}
	inline int getUNIT_PRODUCTION_PERCENT()
	{
		return m_iUNIT_PRODUCTION_PERCENT;
	}
	inline int getMAX_UNIT_SUPPLY_PRODMOD()
	{
		return m_iMAX_UNIT_SUPPLY_PRODMOD;
	}
	inline int getBUILDING_PRODUCTION_PERCENT()
	{
		return m_iBUILDING_PRODUCTION_PERCENT;
	}
	inline int getPROJECT_PRODUCTION_PERCENT()
	{
		return m_iPROJECT_PRODUCTION_PERCENT;
	}
	inline int getMAXED_UNIT_GOLD_PERCENT()
	{
		return m_iMAXED_UNIT_GOLD_PERCENT;
	}
	inline int getMAXED_BUILDING_GOLD_PERCENT()
	{
		return m_iMAXED_BUILDING_GOLD_PERCENT;
	}
	inline int getMAXED_PROJECT_GOLD_PERCENT()
	{
		return m_iMAXED_PROJECT_GOLD_PERCENT;
	}
	inline int getMAX_CITY_DEFENSE_DAMAGE()
	{
		return m_iMAX_CITY_DEFENSE_DAMAGE;
	}
	inline int getCIRCUMNAVIGATE_FREE_MOVES()
	{
		return m_iCIRCUMNAVIGATE_FREE_MOVES;
	}
	inline int getBASE_CAPTURE_GOLD()
	{
		return m_iBASE_CAPTURE_GOLD;
	}
	inline int getCAPTURE_GOLD_PER_POPULATION()
	{
		return m_iCAPTURE_GOLD_PER_POPULATION;
	}
	inline int getCAPTURE_GOLD_RAND1()
	{
		return m_iCAPTURE_GOLD_RAND1;
	}
	inline int getCAPTURE_GOLD_RAND2()
	{
		return m_iCAPTURE_GOLD_RAND2;
	}
	inline int getCAPTURE_GOLD_MAX_TURNS()
	{
		return m_iCAPTURE_GOLD_MAX_TURNS;
	}
	inline int getBARBARIAN_CITY_ATTACK_MODIFIER()
	{
		return m_iBARBARIAN_CITY_ATTACK_MODIFIER;
	}
	inline int getBUILDING_PRODUCTION_DECAY_TIME()
	{
		return m_iBUILDING_PRODUCTION_DECAY_TIME;
	}
	inline int getBUILDING_PRODUCTION_DECAY_PERCENT()
	{
		return m_iBUILDING_PRODUCTION_DECAY_PERCENT;
	}
	inline int getUNIT_PRODUCTION_DECAY_TIME()
	{
		return m_iUNIT_PRODUCTION_DECAY_TIME;
	}
	inline int getUNIT_PRODUCTION_DECAY_PERCENT()
	{
		return m_iUNIT_PRODUCTION_DECAY_PERCENT;
	}
	inline int getBASE_UNIT_UPGRADE_COST()
	{
		return m_iBASE_UNIT_UPGRADE_COST;
	}
	inline int getUNIT_UPGRADE_COST_PER_PRODUCTION()
	{
		return m_iUNIT_UPGRADE_COST_PER_PRODUCTION;
	}
	inline int getUNIT_UPGRADE_COST_VISIBLE_DIVISOR()
	{
		return m_iUNIT_UPGRADE_COST_VISIBLE_DIVISOR;
	}
	inline int getUNIT_UPGRADE_COST_DISCOUNT_MAX()
	{
		return m_iUNIT_UPGRADE_COST_DISCOUNT_MAX;
	}
	inline int getWAR_SUCCESS_UNIT_CAPTURING()
	{
		return m_iWAR_SUCCESS_UNIT_CAPTURING;
	}
	inline int getWAR_SUCCESS_CITY_CAPTURING()
	{
		return m_iWAR_SUCCESS_CITY_CAPTURING;
	}
	inline int getDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL()
	{
		return m_iDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL;
	}
	inline int getTEAM_VOTE_MIN_CANDIDATES()
	{
		return m_iTEAM_VOTE_MIN_CANDIDATES;
	}
	inline int getRESEARCH_AGREEMENT_TIMER()
	{
		return m_iRESEARCH_AGREEMENT_TIMER;
	}
	inline int getRESEARCH_AGREEMENT_BOOST_DIVISOR()
	{
		return m_iRESEARCH_AGREEMENT_BOOST_DIVISOR;
	}
	inline int getSCORE_POPULATION_FACTOR()
	{
		return m_iSCORE_POPULATION_FACTOR;
	}
	inline int getSCORE_LAND_FACTOR()
	{
		return m_iSCORE_LAND_FACTOR;
	}
	inline int getSCORE_WONDER_FACTOR()
	{
		return m_iSCORE_WONDER_FACTOR;
	}
	inline int getSCORE_TECH_FACTOR()
	{
		return m_iSCORE_TECH_FACTOR;
	}
	inline int getSCORE_FREE_PERCENT()
	{
		return m_iSCORE_FREE_PERCENT;
	}
	inline int getSCORE_VICTORY_PERCENT()
	{
		return m_iSCORE_VICTORY_PERCENT;
	}
	inline int getSCORE_HANDICAP_PERCENT_OFFSET()
	{
		return m_iSCORE_HANDICAP_PERCENT_OFFSET;
	}
	inline int getSCORE_HANDICAP_PERCENT_PER()
	{
		return m_iSCORE_HANDICAP_PERCENT_PER;
	}
	inline int getMINIMAP_RENDER_SIZE()
	{
		return m_iMINIMAP_RENDER_SIZE;
	}
	inline int getMAX_INTERCEPTION_PROBABILITY()
	{
		return m_iMAX_INTERCEPTION_PROBABILITY;
	}
	inline int getMAX_EVASION_PROBABILITY()
	{
		return m_iMAX_EVASION_PROBABILITY;
	}
	inline int getPLAYER_ALWAYS_RAZES_CITIES()
	{
		return m_iPLAYER_ALWAYS_RAZES_CITIES;
	}
	inline int getMIN_WATER_SIZE_FOR_OCEAN()
	{
		return m_iMIN_WATER_SIZE_FOR_OCEAN;
	}
	inline int getCITY_SCREEN_CLICK_WILL_EXIT()
	{
		return m_iCITY_SCREEN_CLICK_WILL_EXIT;
	}
	inline int getWATER_POTENTIAL_CITY_WORK_FOR_AREA()
	{
		return m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
	}
	inline int getLAND_UNITS_CAN_ATTACK_WATER_CITIES()
	{
		return m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
	}
	inline int getCITY_MAX_NUM_BUILDINGS()
	{
		return m_iCITY_MAX_NUM_BUILDINGS;
	}
	inline int getCITY_MIN_SIZE_FOR_SETTLERS()
	{
		return m_iCITY_MIN_SIZE_FOR_SETTLERS;
	}
	inline int getADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES()
	{
		return m_iADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES;
	}
	inline int getADVANCED_START_MAX_UNITS_PER_CITY()
	{
		return m_iADVANCED_START_MAX_UNITS_PER_CITY;
	}
	inline int getADVANCED_START_CITY_COST()
	{
		return m_iADVANCED_START_CITY_COST;
	}
	inline int getADVANCED_START_CITY_COST_INCREASE()
	{
		return m_iADVANCED_START_CITY_COST_INCREASE;
	}
	inline int getADVANCED_START_POPULATION_COST()
	{
		return m_iADVANCED_START_POPULATION_COST;
	}
	inline int getADVANCED_START_POPULATION_COST_INCREASE()
	{
		return m_iADVANCED_START_POPULATION_COST_INCREASE;
	}
	inline int getADVANCED_START_VISIBILITY_COST()
	{
		return m_iADVANCED_START_VISIBILITY_COST;
	}
	inline int getADVANCED_START_VISIBILITY_COST_INCREASE()
	{
		return m_iADVANCED_START_VISIBILITY_COST_INCREASE;
	}
	inline int getADVANCED_START_CITY_PLACEMENT_MAX_RANGE()
	{
		return m_iADVANCED_START_CITY_PLACEMENT_MAX_RANGE;
	}
	inline int getNEW_CITY_BUILDING_VALUE_MODIFIER()
	{
		return m_iNEW_CITY_BUILDING_VALUE_MODIFIER;
	}
	inline int getPATH_DAMAGE_WEIGHT()
	{
		return m_iPATH_DAMAGE_WEIGHT;
	}
	inline int getPUPPET_SCIENCE_MODIFIER()
	{
		return m_iPUPPET_SCIENCE_MODIFIER;
	}
	inline int getPUPPET_CULTURE_MODIFIER()
	{
		return m_iPUPPET_CULTURE_MODIFIER;
	}
	inline int getPUPPET_GOLD_MODIFIER()
	{
		return m_iPUPPET_GOLD_MODIFIER;
	}
	inline int getPUPPET_FAITH_MODIFIER()
	{
		return m_iPUPPET_FAITH_MODIFIER;
	}
	inline int getPUPPET_GROWTH_MODIFIER()
	{
		return m_iPUPPET_GROWTH_MODIFIER;
	}
	inline int getPUPPET_PRODUCTION_MODIFIER()
	{
		return m_iPUPPET_PRODUCTION_MODIFIER;
	}
	inline int getBLOCKADE_GOLD_PENALTY()
	{
		return m_iBLOCKADE_GOLD_PENALTY;
	}
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	inline int getPUPPET_TOURISM_MODIFIER()
	{
		return m_iPUPPET_TOURISM_MODIFIER;
	}
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
	inline int getPUPPET_GOLDEN_AGE_MODIFIER()
	{
		return m_iPUPPET_GOLDEN_AGE_MODIFIER;
	}
#endif
	inline int getBASE_POLICY_COST()
	{
		return m_iBASE_POLICY_COST;
	}
	inline int getPOLICY_COST_INCREASE_TO_BE_EXPONENTED()
	{
		return m_iPOLICY_COST_INCREASE_TO_BE_EXPONENTED;
	}
	inline int getPOLICY_COST_VISIBLE_DIVISOR()
	{
		return m_iPOLICY_COST_VISIBLE_DIVISOR;
	}
	inline int getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS()
	{
		return m_iSWITCH_POLICY_BRANCHES_ANARCHY_TURNS;
	}
	inline int getSWITCH_POLICY_BRANCHES_TENETS_LOST()
	{
		return m_iSWITCH_POLICY_BRANCHES_TENETS_LOST;
	}
	inline int getPOLICY_COST_DISCOUNT_MAX()
	{
		return m_iPOLICY_COST_DISCOUNT_MAX;
	}
	inline int getGOLD_PURCHASE_GOLD_PER_PRODUCTION()
	{
		return m_iGOLD_PURCHASE_GOLD_PER_PRODUCTION;
	}
	inline int getGOLD_PURCHASE_VISIBLE_DIVISOR()
	{
		return m_iGOLD_PURCHASE_VISIBLE_DIVISOR;
	}
	inline int getPROJECT_PURCHASING_DISABLED()
	{
		return m_iPROJECT_PURCHASING_DISABLED;
	}
	inline int getINFLUENCE_MOUNTAIN_COST()
	{
		return m_iINFLUENCE_MOUNTAIN_COST;
	}
	inline int getINFLUENCE_HILL_COST()
	{
		return m_iINFLUENCE_HILL_COST;
	}
	inline int getINFLUENCE_RIVER_COST()
	{
		return m_iINFLUENCE_RIVER_COST;
	}
	inline int getUSE_FIRST_RING_INFLUENCE_TERRAIN_COST()
	{
		return m_iUSE_FIRST_RING_INFLUENCE_TERRAIN_COST;
	}
	inline int getNUM_RESOURCE_QUANTITY_TYPES()
	{
		return m_iNUM_RESOURCE_QUANTITY_TYPES;
	}
	inline int getSPECIALISTS_DIVERT_POPULATION_ENABLED()
	{
		return m_iSPECIALISTS_DIVERT_POPULATION_ENABLED;
	}
	inline int getSCIENCE_PER_POPULATION()
	{
		return m_iSCIENCE_PER_POPULATION;
	}
	inline int getRESEARCH_AGREEMENT_MOD()
	{
		return m_iRESEARCH_AGREEMENT_MOD;
	}
	inline int getBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD()
	{
		return m_iBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD;
	}
	inline int getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING()
	{
		return m_iBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING;
	}
	inline int getBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL()
	{
		return m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL;
	}
	inline int getBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP()
	{
		return m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP;
	}
	inline int getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL()
	{
		return m_iBARBARIAN_CAMP_COASTAL_SPAWN_ROLL;
	}
	inline int getBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE()
	{
		return m_iBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE;
	}
	inline int getBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN()
	{
		return m_iBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN;
	}
	inline int getMAX_BARBARIANS_FROM_CAMP_NEARBY()
	{
		return m_iMAX_BARBARIANS_FROM_CAMP_NEARBY;
	}
	inline int getMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE()
	{
		return m_iMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE;
	}
	inline int getGOLD_FROM_BARBARIAN_CONVERSION()
	{
		return m_iGOLD_FROM_BARBARIAN_CONVERSION;
	}
	inline int getBARBARIAN_CITY_GOLD_RANSOM()
	{
		return m_iBARBARIAN_CITY_GOLD_RANSOM;
	}
	inline int getBARBARIAN_UNIT_GOLD_RANSOM()
	{
		return m_iBARBARIAN_UNIT_GOLD_RANSOM;
	}
	inline int getEMBARKED_UNIT_MOVEMENT()
	{
		return m_iEMBARKED_UNIT_MOVEMENT;
	}
	inline int getEMBARKED_VISIBILITY_RANGE()
	{
		return m_iEMBARKED_VISIBILITY_RANGE;
	}
	inline int getDEFAULT_MAX_NUM_BUILDERS()
	{
		return m_iDEFAULT_MAX_NUM_BUILDERS;
	}
	inline int getBARBARIAN_TECH_PERCENT()
	{
		return m_iBARBARIAN_TECH_PERCENT;
	}
	inline int getCITY_RESOURCE_WLTKD_TURNS()
	{
		return m_iCITY_RESOURCE_WLTKD_TURNS;
	}
	inline int getMAX_SPECIALISTS_FROM_BUILDING()
	{
		return m_iMAX_SPECIALISTS_FROM_BUILDING;
	}
	inline int getGREAT_PERSON_THRESHOLD_BASE()
	{
		return m_iGREAT_PERSON_THRESHOLD_BASE;
	}
	inline int getGREAT_PERSON_THRESHOLD_INCREASE()
	{
		return m_iGREAT_PERSON_THRESHOLD_INCREASE;
	}
	inline int getCULTURE_BOMB_COOLDOWN()
	{
		return m_iCULTURE_BOMB_COOLDOWN;
	}
	inline int getCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE()
	{
		return m_iCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE;
	}
	inline int getLANDMARK_MINOR_FRIENDSHIP_CHANGE()
	{
		return m_iLANDMARK_MINOR_FRIENDSHIP_CHANGE;
	}
	inline int getUNIT_AUTO_EXPLORE_DISABLED()
	{
		return m_iUNIT_AUTO_EXPLORE_DISABLED;
	}
	inline int getUNIT_AUTO_EXPLORE_FULL_DISABLED()
	{
		return m_iUNIT_AUTO_EXPLORE_FULL_DISABLED;
	}
	inline int getUNIT_WORKER_AUTOMATION_DISABLED()
	{
		return m_iUNIT_WORKER_AUTOMATION_DISABLED;
	}
	inline int getUNIT_DELETE_DISABLED()
	{
		return m_iUNIT_DELETE_DISABLED;
	}
	inline int getMIN_START_AREA_TILES()
	{
		return m_iMIN_START_AREA_TILES;
	}
	inline int getMIN_DISTANCE_OTHER_AREA_PERCENT()
	{
		return m_iMIN_DISTANCE_OTHER_AREA_PERCENT;
	}
	inline int getMINOR_CIV_FOOD_REQUIREMENT()
	{
		return m_iMINOR_CIV_FOOD_REQUIREMENT;
	}
	inline int getMAJOR_CIV_FOOD_REQUIREMENT()
	{
		return m_iMAJOR_CIV_FOOD_REQUIREMENT;
	}
	inline int getMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST()
	{
		return m_iMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST;
	}
	inline int getSTART_AREA_FOOD_MULTIPLIER()
	{
		return m_iSTART_AREA_FOOD_MULTIPLIER;
	}
	inline int getSTART_AREA_HAPPINESS_MULTIPLIER()
	{
		return m_iSTART_AREA_HAPPINESS_MULTIPLIER;
	}
	inline int getSTART_AREA_PRODUCTION_MULTIPLIER()
	{
		return m_iSTART_AREA_PRODUCTION_MULTIPLIER;
	}
	inline int getSTART_AREA_GOLD_MULTIPLIER()
	{
		return m_iSTART_AREA_GOLD_MULTIPLIER;
	}
	inline int getSTART_AREA_SCIENCE_MULTIPLIER()
	{
		return m_iSTART_AREA_SCIENCE_MULTIPLIER;
	}
	inline int getSTART_AREA_FAITH_MULTIPLIER()
	{
		return m_iSTART_AREA_FAITH_MULTIPLIER;
	}
	inline int getSTART_AREA_RESOURCE_MULTIPLIER()
	{
		return m_iSTART_AREA_RESOURCE_MULTIPLIER;
	}
	inline int getSTART_AREA_STRATEGIC_MULTIPLIER()
	{
		return m_iSTART_AREA_STRATEGIC_MULTIPLIER;
	}
	inline int getSTART_AREA_BUILD_ON_COAST_PERCENT()
	{
		return m_iSTART_AREA_BUILD_ON_COAST_PERCENT;
	}
	inline int getSETTLER_FOOD_MULTIPLIER()
	{
		return m_iSETTLER_FOOD_MULTIPLIER;
	}
	inline int getSETTLER_HAPPINESS_MULTIPLIER()
	{
		return m_iSETTLER_HAPPINESS_MULTIPLIER;
	}
	inline int getSETTLER_PRODUCTION_MULTIPLIER()
	{
		return m_iSETTLER_PRODUCTION_MULTIPLIER;
	}
	inline int getSETTLER_GOLD_MULTIPLIER()
	{
		return m_iSETTLER_GOLD_MULTIPLIER;
	}
	inline int getSETTLER_SCIENCE_MULTIPLIER()
	{
		return m_iSETTLER_SCIENCE_MULTIPLIER;
	}
	inline int getSETTLER_FAITH_MULTIPLIER()
	{
		return m_iSETTLER_FAITH_MULTIPLIER;
	}
	inline int getSETTLER_RESOURCE_MULTIPLIER()
	{
		return m_iSETTLER_RESOURCE_MULTIPLIER;
	}
	inline int getSETTLER_STRATEGIC_MULTIPLIER()
	{
		return m_iSETTLER_STRATEGIC_MULTIPLIER;
	}
	inline int getSETTLER_BUILD_ON_COAST_PERCENT()
	{
		return m_iSETTLER_BUILD_ON_COAST_PERCENT;
	}
	inline int getCITY_RING_1_MULTIPLIER()
	{
		return m_iCITY_RING_1_MULTIPLIER;
	}
	inline int getCITY_RING_2_MULTIPLIER()
	{
		return m_iCITY_RING_2_MULTIPLIER;
	}
	inline int getCITY_RING_3_MULTIPLIER()
	{
		return m_iCITY_RING_3_MULTIPLIER;
	}
	inline int getCITY_RING_4_MULTIPLIER()
	{
		return m_iCITY_RING_4_MULTIPLIER;
	}
	inline int getCITY_RING_5_MULTIPLIER()
	{
		return m_iCITY_RING_5_MULTIPLIER;
	}
	inline int getSETTLER_EVALUATION_DISTANCE()
	{
		return m_iSETTLER_EVALUATION_DISTANCE;
	}
	inline int getSETTLER_DISTANCE_DROPOFF_MODIFIER()
	{
		return m_iSETTLER_DISTANCE_DROPOFF_MODIFIER;
	}
	inline int getBUILD_ON_RESOURCE_PERCENT()
	{
		return m_iBUILD_ON_RESOURCE_PERCENT;
	}
	inline int getBUILD_ON_RIVER_PERCENT()
	{
		return m_iBUILD_ON_RIVER_PERCENT;
	}
	inline int getCHOKEPOINT_STRATEGIC_VALUE()
	{
		return m_iCHOKEPOINT_STRATEGIC_VALUE;
	}
	inline int getHILL_STRATEGIC_VALUE()
	{
		return m_iHILL_STRATEGIC_VALUE;
	}
	inline int getALREADY_OWNED_STRATEGIC_VALUE()
	{
		return m_iALREADY_OWNED_STRATEGIC_VALUE;
	}
	inline int getMINOR_CIV_CONTACT_GOLD_FIRST()
	{
		return m_iMINOR_CIV_CONTACT_GOLD_FIRST;
	}
	inline int getMINOR_CIV_CONTACT_GOLD_OTHER()
	{
		return m_iMINOR_CIV_CONTACT_GOLD_OTHER;
	}
#if defined(MOD_GLOBAL_CS_GIFTS)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_FRIENDSHIP)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_CULTURE)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_FAITH)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_GOLD)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_FOOD)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_BONUS_UNIT)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_XP_PER_ERA)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_XP_RANDOM)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_PLAYER_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_PLAYER_DIVISOR)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_DIVISOR)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_DIVISOR)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_DIVISOR)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_DIVISOR)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_MULTIPLIER)
	GD_INT_DEF(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_DIVISOR)
#endif
	inline int getMINOR_CIV_GROWTH_PERCENT()
	{
		return m_iMINOR_CIV_GROWTH_PERCENT;
	}
	inline int getMINOR_CIV_PRODUCTION_PERCENT()
	{
		return m_iMINOR_CIV_PRODUCTION_PERCENT;
	}
	inline int getMINOR_CIV_GOLD_PERCENT()
	{
		return m_iMINOR_CIV_GOLD_PERCENT;
	}
	inline int getMINOR_CIV_TECH_PERCENT()
	{
		return m_iMINOR_CIV_TECH_PERCENT;
	}
	inline int getMINOR_POLICY_RESOURCE_MULTIPLIER()
	{
		return m_iMINOR_POLICY_RESOURCE_MULTIPLIER;
	}
	inline int getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER()
	{
		return m_iMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER;
	}
	inline int getMINOR_GOLD_GIFT_LARGE()
	{
		return m_iMINOR_GOLD_GIFT_LARGE;
	}
	inline int getMINOR_GOLD_GIFT_MEDIUM()
	{
		return m_iMINOR_GOLD_GIFT_MEDIUM;
	}
	inline int getMINOR_GOLD_GIFT_SMALL()
	{
		return m_iMINOR_GOLD_GIFT_SMALL;
	}
	inline int getMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST()
	{
		return m_iMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST;
	}
	inline int getMINOR_CIV_BUYOUT_COST()
	{
		return m_iMINOR_CIV_BUYOUT_COST;
	}
	inline int getMINOR_CIV_BUYOUT_TURNS()
	{
		return m_iMINOR_CIV_BUYOUT_TURNS;
	}
	inline int getMINOR_FRIENDSHIP_FROM_TRADE_MISSION()
	{
		return m_iMINOR_FRIENDSHIP_FROM_TRADE_MISSION;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_DEFAULT()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_DEFAULT;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF;
	}
	inline int getMINOR_UNIT_GIFT_TRAVEL_TURNS()
	{
		return m_iMINOR_UNIT_GIFT_TRAVEL_TURNS;
	}
	inline int getPLOT_UNIT_LIMIT()
	{
		return m_iPLOT_UNIT_LIMIT;
	}
#if defined(MOD_GLOBAL_STACKING_RULES)
	GD_INT_DEF(CITY_UNIT_LIMIT)
#endif
	inline int getZONE_OF_CONTROL_ENABLED()
	{
		return m_iZONE_OF_CONTROL_ENABLED;
	}
	inline int getFIRE_SUPPORT_DISABLED()
	{
		return m_iFIRE_SUPPORT_DISABLED;
	}
	inline int getMAX_HIT_POINTS()
	{
		return m_iMAX_HIT_POINTS;
	}
	inline int getMAX_CITY_HIT_POINTS()
	{
		return m_iMAX_CITY_HIT_POINTS;
	}
	inline int getCITY_HIT_POINTS_HEALED_PER_TURN()
	{
		return m_iCITY_HIT_POINTS_HEALED_PER_TURN;
	}
	inline int getFLAT_LAND_EXTRA_DEFENSE()
	{
		return m_iFLAT_LAND_EXTRA_DEFENSE;
	}
	inline int getHILLS_EXTRA_DEFENSE()
	{
		return m_iHILLS_EXTRA_DEFENSE;
	}
	inline int getRIVER_ATTACK_MODIFIER()
	{
		return m_iRIVER_ATTACK_MODIFIER;
	}
	inline int getAMPHIB_ATTACK_MODIFIER()
	{
		return m_iAMPHIB_ATTACK_MODIFIER;
	}
	inline int getENEMY_HEAL_RATE()
	{
		return m_iENEMY_HEAL_RATE;
	}
	inline int getNEUTRAL_HEAL_RATE()
	{
		return m_iNEUTRAL_HEAL_RATE;
	}
	inline int getFRIENDLY_HEAL_RATE()
	{
		return m_iFRIENDLY_HEAL_RATE;
	}
	inline int getINSTA_HEAL_RATE()
	{
		return m_iINSTA_HEAL_RATE;
	}
	inline int getCITY_HEAL_RATE()
	{
		return m_iCITY_HEAL_RATE;
	}
	inline int getATTACK_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iATTACK_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getRANGE_ATTACK_RANGED_DEFENDER_MOD()
	{
		return m_iRANGE_ATTACK_RANGED_DEFENDER_MOD;
	}
	inline int getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE()
	{
		return m_iAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE;
	}
	inline int getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE()
	{
		return m_iAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE;
	}
	inline int getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD()
	{
		return m_iAIR_SWEEP_INTERCEPTION_DAMAGE_MOD;
	}
	inline int getWOUNDED_DAMAGE_MULTIPLIER()
	{
		return m_iWOUNDED_DAMAGE_MULTIPLIER;
	}
	inline int getTRAIT_WOUNDED_DAMAGE_MOD()
	{
		return m_iTRAIT_WOUNDED_DAMAGE_MOD;
	}
	inline int getCITY_STRENGTH_DEFAULT()
	{
		return m_iCITY_STRENGTH_DEFAULT;
	}
	inline int getCITY_STRENGTH_POPULATION_CHANGE()
	{
		return m_iCITY_STRENGTH_POPULATION_CHANGE;
	}
	inline int getCITY_STRENGTH_UNIT_DIVISOR()
	{
		return m_iCITY_STRENGTH_UNIT_DIVISOR;
	}
	inline int getCITY_STRENGTH_HILL_CHANGE()
	{
		return m_iCITY_STRENGTH_HILL_CHANGE;
	}
	inline int getCITY_ATTACKING_DAMAGE_MOD()
	{
		return m_iCITY_ATTACKING_DAMAGE_MOD;
	}
	inline int getATTACKING_CITY_MELEE_DAMAGE_MOD()
	{
		return m_iATTACKING_CITY_MELEE_DAMAGE_MOD;
	}
#if defined(MOD_EVENTS_CITY_BOMBARD)
	GD_INT_DEF(MAX_CITY_ATTACK_RANGE)
#endif
	inline int getCITY_ATTACK_RANGE()
	{
		return m_iCITY_ATTACK_RANGE;
	}
	inline int getCAN_CITY_USE_INDIRECT_FIRE()
	{
		return m_iCAN_CITY_USE_INDIRECT_FIRE;
	}
	inline int getCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER()
	{
		return m_iCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER;
	}
	inline int getMIN_CITY_STRIKE_DAMAGE()
	{
		return m_iMIN_CITY_STRIKE_DAMAGE;
	}
	inline int getCITY_CAPTURE_DAMAGE_PERCENT()
	{
		return m_iCITY_CAPTURE_DAMAGE_PERCENT;
	}
	inline int getEXPERIENCE_PER_LEVEL()
	{
		return m_iEXPERIENCE_PER_LEVEL;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_MELEE()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_MELEE;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_MELEE()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_MELEE;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_AIR()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_AIR;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_AIR()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_AIR;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_RANGED()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_RANGED;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_RANGED()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_RANGED;
	}
	inline int getEXPERIENCE_ATTACKING_AIR_SWEEP()
	{
		return m_iEXPERIENCE_ATTACKING_AIR_SWEEP;
	}
	inline int getEXPERIENCE_DEFENDING_AIR_SWEEP_AIR()
	{
		return m_iEXPERIENCE_DEFENDING_AIR_SWEEP_AIR;
	}
	inline int getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND()
	{
		return m_iEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_MELEE()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_MELEE;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_RANGED()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_RANGED;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_AIR()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_AIR;
	}
	inline int getBARBARIAN_MAX_XP_VALUE()
	{
		return m_iBARBARIAN_MAX_XP_VALUE;
	}
	inline int getMINOR_MAX_XP_VALUE()
	{
		return m_iMINOR_MAX_XP_VALUE;
	}
	inline int getCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT()
	{
		return m_iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT;
	}
	inline int getGREAT_GENERALS_THRESHOLD_INCREASE()
	{
		return m_iGREAT_GENERALS_THRESHOLD_INCREASE;
	}
	inline int getGREAT_GENERALS_THRESHOLD_INCREASE_TEAM()
	{
		return m_iGREAT_GENERALS_THRESHOLD_INCREASE_TEAM;
	}
	inline int getGREAT_GENERALS_THRESHOLD()
	{
		return m_iGREAT_GENERALS_THRESHOLD;
	}
	inline int getUNIT_DEATH_XP_GREAT_GENERAL_LOSS()
	{
		return m_iUNIT_DEATH_XP_GREAT_GENERAL_LOSS;
	}
	inline int getMIN_EXPERIENCE_PER_COMBAT()
	{
		return m_iMIN_EXPERIENCE_PER_COMBAT;
	}
	inline int getMAX_EXPERIENCE_PER_COMBAT()
	{
		return m_iMAX_EXPERIENCE_PER_COMBAT;
	}
	inline int getCRAMPED_RANGE_FROM_CITY()
	{
		return m_iCRAMPED_RANGE_FROM_CITY;
	}
	inline int getCRAMPED_USABLE_PLOT_PERCENT()
	{
		return m_iCRAMPED_USABLE_PLOT_PERCENT;
	}
	inline int getPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT()
	{
		return m_iPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT;
	}
	inline int getPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY()
	{
		return m_iPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MAX()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MAX;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MIN()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MIN;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MAX()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MAX;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MIN()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MIN;
	}
	inline int getPLOT_BASE_COST()
	{
		return m_iPLOT_BASE_COST;
	}
	inline int getPLOT_ADDITIONAL_COST_PER_PLOT()
	{
		return m_iPLOT_ADDITIONAL_COST_PER_PLOT;
	}
	inline int getPLOT_COST_APPEARANCE_DIVISOR()
	{
		return m_iPLOT_COST_APPEARANCE_DIVISOR;
	}
	inline int getCULTURE_COST_FIRST_PLOT()
	{
		return m_iCULTURE_COST_FIRST_PLOT;
	}
	inline int getCULTURE_COST_LATER_PLOT_MULTIPLIER()
	{
		return m_iCULTURE_COST_LATER_PLOT_MULTIPLIER;
	}
	inline int getCULTURE_COST_VISIBLE_DIVISOR()
	{
		return m_iCULTURE_COST_VISIBLE_DIVISOR;
	}
	inline int getCULTURE_PLOT_COST_MOD_MINIMUM()
	{
		return m_iCULTURE_PLOT_COST_MOD_MINIMUM;
	}
	inline int getMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER()
	{
		return m_iMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER;
	}
	inline int getMAXIMUM_WORK_PLOT_DISTANCE()
	{
		return m_iMAXIMUM_WORK_PLOT_DISTANCE;
	}
	inline int getMAXIMUM_BUY_PLOT_DISTANCE()
	{
		return m_iMAXIMUM_BUY_PLOT_DISTANCE;
	}
	inline int getMAXIMUM_ACQUIRE_PLOT_DISTANCE()
	{

		return std::min(m_iMAXIMUM_ACQUIRE_PLOT_DISTANCE, MAX_CITY_RADIUS);
	}
	inline int getPLOT_INFLUENCE_BASE_MULTIPLIER()
	{
		return m_iPLOT_INFLUENCE_BASE_MULTIPLIER;
	}
	inline int getPLOT_INFLUENCE_DISTANCE_MULTIPLIER()
	{
		return m_iPLOT_INFLUENCE_DISTANCE_MULTIPLIER;
	}
	inline int getPLOT_INFLUENCE_DISTANCE_DIVISOR()
	{
		return m_iPLOT_INFLUENCE_DISTANCE_DIVISOR;
	}
	inline int getPLOT_INFLUENCE_RING_COST()
	{
		return m_iPLOT_INFLUENCE_RING_COST;
	}
	inline int getPLOT_INFLUENCE_WATER_COST()
	{
		return m_iPLOT_INFLUENCE_WATER_COST;
	}
	inline int getPLOT_INFLUENCE_IMPROVEMENT_COST()
	{
		return m_iPLOT_INFLUENCE_IMPROVEMENT_COST;
	}
	inline int getPLOT_INFLUENCE_ROUTE_COST()
	{
		return m_iPLOT_INFLUENCE_ROUTE_COST;
	}
	inline int getPLOT_INFLUENCE_RESOURCE_COST()
	{
		return m_iPLOT_INFLUENCE_RESOURCE_COST;
	}
	inline int getPLOT_INFLUENCE_NW_COST()
	{
		return m_iPLOT_INFLUENCE_NW_COST;
	}
#if defined(MOD_UI_CITY_EXPANSION)
	GD_INT_DEF(PLOT_INFLUENCE_COST_VISIBLE_DIVISOR)
#endif
	inline int getPLOT_BUY_RESOURCE_COST()
	{
		return m_iPLOT_BUY_RESOURCE_COST;
	}
	inline int getPLOT_BUY_YIELD_COST()
	{
		return m_iPLOT_BUY_YIELD_COST;
	}
	inline int getPLOT_INFLUENCE_YIELD_POINT_COST()
	{
		return m_iPLOT_INFLUENCE_YIELD_POINT_COST;
	}
	inline int getPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST()
	{
		return m_iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST;
	}
	inline int getUNITED_NATIONS_COUNTDOWN_TURNS()
	{
		return m_iUNITED_NATIONS_COUNTDOWN_TURNS;
	}
	inline int getOWN_UNITED_NATIONS_VOTE_BONUS()
	{
		return m_iOWN_UNITED_NATIONS_VOTE_BONUS;
	}
	inline int getDIPLO_VICTORY_ALGORITHM_THRESHOLD()
	{
		return m_iDIPLO_VICTORY_ALGORITHM_THRESHOLD;
	}
	inline int getDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER()
	{
		return m_iDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER;
	}
	inline int getDIPLO_VICTORY_DEFAULT_VOTE_PERCENT()
	{
		return m_iDIPLO_VICTORY_DEFAULT_VOTE_PERCENT;
	}
	inline int getSCORE_CITY_MULTIPLIER()
	{
		return m_iSCORE_CITY_MULTIPLIER;
	}
	inline int getSCORE_POPULATION_MULTIPLIER()
	{
		return m_iSCORE_POPULATION_MULTIPLIER;
	}
	inline int getSCORE_LAND_MULTIPLIER()
	{
		return m_iSCORE_LAND_MULTIPLIER;
	}
	inline int getSCORE_WONDER_MULTIPLIER()
	{
		return m_iSCORE_WONDER_MULTIPLIER;
	}
	inline int getSCORE_TECH_MULTIPLIER()
	{
		return m_iSCORE_TECH_MULTIPLIER;
	}
	inline int getSCORE_FUTURE_TECH_MULTIPLIER()
	{
		return m_iSCORE_FUTURE_TECH_MULTIPLIER;
	}
	inline int getSCORE_POLICY_MULTIPLIER()
	{
		return m_iSCORE_POLICY_MULTIPLIER;
	}
	inline int getSCORE_GREAT_WORK_MULTIPLIER()
	{
		return m_iSCORE_GREAT_WORK_MULTIPLIER;
	}
	inline int getSCORE_BELIEF_MULTIPLIER()
	{
		return m_iSCORE_BELIEF_MULTIPLIER;
	}
	inline int getSCORE_RELIGION_CITIES_MULTIPLIER()
	{
		return m_iSCORE_RELIGION_CITIES_MULTIPLIER;
	}
	inline int getVICTORY_POINTS_PER_ERA()
	{
		return m_iVICTORY_POINTS_PER_ERA;
	}
	inline int getMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY()
	{
		return m_iMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY;
	}
	inline int getZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS;
	}
	inline int getZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS;
	}
	inline int getZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS;
	}
	inline int getMAX_CITY_DIST_HIGHWATER_MARK()
	{
		return m_iMAX_CITY_DIST_HIGHWATER_MARK;
	}
	inline int getHEAVY_RESOURCE_THRESHOLD()
	{
		return m_iHEAVY_RESOURCE_THRESHOLD;
	}
	inline int getPROGRESS_POPUP_TURN_FREQUENCY()
	{
		return m_iPROGRESS_POPUP_TURN_FREQUENCY;
	}
	inline int getSETTLER_PRODUCTION_SPEED()
	{
		return m_iSETTLER_PRODUCTION_SPEED;
	}
	inline int getBUY_PLOTS_DISABLED()
	{
		return m_iBUY_PLOTS_DISABLED;
	}
	inline int getCITY_STRENGTH_TECH_MULTIPLIER()
	{
		return m_iCITY_STRENGTH_TECH_MULTIPLIER;
	}
	inline int getWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT()
	{
		return m_iWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT;
	}
	inline int getBUILDING_SALE_DIVISOR()
	{
		return m_iBUILDING_SALE_DIVISOR;
	}
	inline int getDISBAND_UNIT_REFUND_PERCENT()
	{
		return m_iDISBAND_UNIT_REFUND_PERCENT;
	}
	inline int getMINOR_CIV_ROUTE_QUEST_WEIGHT()
	{
		return m_iMINOR_CIV_ROUTE_QUEST_WEIGHT;
	}
	inline int getWITHDRAW_MOD_ENEMY_MOVES()
	{
		return m_iWITHDRAW_MOD_ENEMY_MOVES;
	}
	inline int getWITHDRAW_MOD_BLOCKED_TILE()
	{
		return m_iWITHDRAW_MOD_BLOCKED_TILE;
	}
	inline int getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT()
	{
		return m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT;
	}
	inline int getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY()
	{
		return m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY;
	}
	inline int getWALLS_BUILDINGCLASS() const
	{
		return m_iWALLS_BUILDINGCLASS;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_HURRY)
	inline int getAI_GOLD_TREASURY_BUFFER()
	{
		return m_iAI_GOLD_TREASURY_BUFFER;
	}
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS) || defined(MOD_BALANCE_CORE)
	inline int getBALANCE_MINOR_ANCHOR_ATTACK()
	{
		return m_iBALANCE_MINOR_ANCHOR_ATTACK;
	}
	inline int getBALANCE_CS_WAR_COOLDOWN_RATE()
	{
		return m_iBALANCE_CS_WAR_COOLDOWN_RATE;
	}
	inline int getBALANCE_CS_FORGIVENESS_CHANCE()
	{
		return m_iBALANCE_CS_FORGIVENESS_CHANCE;
	}
#endif
#if defined(MOD_BALANCE_CORE)
	inline int getMOD_BALANCE_FOLLOWER_GROWTH_BONUS()
	{
		return m_iMOD_BALANCE_FOLLOWER_GROWTH_BONUS;
	}
	inline int getFRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getFRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getFRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getBALANCE_MAX_CS_ALLY_STRENGTH()
	{
		return m_iBALANCE_MAX_CS_ALLY_STRENGTH;
	}
	inline int getBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS()
	{
		return m_iBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS;
	}
	inline int getBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX()
	{
		return m_iBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX;
	}
	inline int getBALANCE_CS_ALLIANCE_DEFENSE_BONUS()
	{
		return m_iBALANCE_CS_ALLIANCE_DEFENSE_BONUS;
	}
	inline int getUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED()
	{
		return m_iUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED;
	}
	inline int getBALANCE_CORE_ARABIA_TILE_BONUS()
	{
		return m_iBALANCE_CORE_ARABIA_TILE_BONUS;
	}
	inline int getBALANCE_MARRIAGE_GP_RATE()
	{
		return m_iBALANCE_MARRIAGE_GP_RATE;
	}
	inline int getBALANCE_SPY_RESPAWN_TIMER()
	{
		return m_iBALANCE_SPY_RESPAWN_TIMER;
	}
	inline int getBALANCE_SPY_SABOTAGE_RATE()
	{
		return m_iBALANCE_SPY_SABOTAGE_RATE;
	}
	inline int getBALANCE_SPY_TO_MINOR_RATIO()
	{
		return m_iBALANCE_SPY_TO_MINOR_RATIO;
	}
	inline int getBALANCE_HAPPINESS_EMPIRE_MOD()
	{
		return m_iBALANCE_HAPPINESS_EMPIRE_MOD;
	}
	inline int getBALANCE_HAPPINESS_THRESHOLD()
	{
		return m_iBALANCE_HAPPINESS_THRESHOLD;
	}
	inline int getBALANCE_HAPPINESS_BONUS_MAXIMUM()
	{
		return m_iBALANCE_HAPPINESS_BONUS_MAXIMUM;
	}
	inline int getBALANCE_HAPPINESS_BONUS_MINIMUM()
	{
		return m_iBALANCE_HAPPINESS_BONUS_MINIMUM;
	}
	inline int getBALANCE_HAPPINESS_PENALTY_MAXIMUM()
	{
		return m_iBALANCE_HAPPINESS_PENALTY_MAXIMUM;
	}
	inline int getBALANCE_HAPPINESS_PENALTY_MINIMUM()
	{
		return m_iBALANCE_HAPPINESS_PENALTY_MINIMUM;
	}
	inline int getBALANCE_HAPPINESS_SCIENCE_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_SCIENCE_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_GOLD_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_GOLD_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_PRODUCTION_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_PRODUCTION_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_FOOD_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_FOOD_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_FAITH_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_FAITH_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_CULTURE_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_CULTURE_MODIFIER;
	}
	inline int getBALANCE_HAPPINESS_THRESHOLD_MAIN()
	{
		return m_iBALANCE_HAPPINESS_THRESHOLD_MAIN;
	}
	inline int getBALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE()
	{
		return m_iBALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE;
	}
	inline int getBALANCE_COAST_STRATEGIC_VALUE()
	{
		return m_iBALANCE_COAST_STRATEGIC_VALUE;
	}
	inline int getBALANCE_FRESH_WATER_STRATEGIC_VALUE()
	{
		return m_iBALANCE_FRESH_WATER_STRATEGIC_VALUE;
	}
	inline int getBALANCE_CHOKEPOINT_STRATEGIC_VALUE()
	{
		return m_iBALANCE_CHOKEPOINT_STRATEGIC_VALUE;
	}
	inline int getBALANCE_BAD_TILES_STRATEGIC_VALUE()
	{
		return m_iBALANCE_BAD_TILES_STRATEGIC_VALUE;
	}
	inline int getBALANCE_BARBARIAN_HEAL_RATE()
	{
		return m_iBALANCE_BARBARIAN_HEAL_RATE;
	}
	inline int getBALANCE_BASIC_ATTACK_ARMY_SIZE()
	{
		return m_iBALANCE_BASIC_ATTACK_ARMY_SIZE;
	}
	inline int getBALANCE_NAVY_START_SIZE()
	{
		return m_iBALANCE_NAVY_START_SIZE;
	}
	// -- HAPPINESS DEFINES //
	inline int getBALANCE_UNHAPPINESS_PER_SPECIALIST()
	{
		return m_iBALANCE_UNHAPPINESS_PER_SPECIALIST;
	}
	inline int getBALANCE_HAPPINESS_LUXURY_BASE()
	{
		return m_iBALANCE_HAPPINESS_LUXURY_BASE;
	}
	inline int getBALANCE_HAPPINESS_LUXURY_SCALER()
	{
		return m_iBALANCE_HAPPINESS_LUXURY_SCALER;
	}
	inline int getBALANCE_HAPPINESS_POPULATION_DIVISOR()
	{
		return m_iBALANCE_HAPPINESS_POPULATION_DIVISOR;
	}
	inline float getBALANCE_UNHAPPINESS_PER_MINORITY_POP()
	{
		return m_fBALANCE_UNHAPPINESS_PER_MINORITY_POP;
	}
	inline float getBALANCE_UNHAPPINESS_FROM_STARVING_PER_POP()
	{
		return m_fBALANCE_UNHAPPINESS_FROM_STARVING_PER_POP;
	}
	inline float getBALANCE_UNHAPPINESS_PER_PILLAGED()
	{
		return m_fBALANCE_UNHAPPINESS_PER_PILLAGED;
	}
	inline float getBALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP()
	{
		return m_fBALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP;
	}
	inline int getBALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD()
	{
		return m_iBALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD;
	}
	inline float getBALANCE_UNHAPPINESS_PER_GOLD_RATE()
	{
		return m_fBALANCE_UNHAPPINESS_PER_GOLD_RATE;
	}
	inline int getBALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD()
	{
		return m_iBALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD;
	}
	inline float getBALANCE_UNHAPPINESS_PER_DEFENSE_RATE()
	{
		return m_fBALANCE_UNHAPPINESS_PER_DEFENSE_RATE;
	}
	inline int getBALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD()
	{
		return m_iBALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD;
	}
	inline float getBALANCE_UNHAPPINESS_PER_CULTURE_RATE()
	{
		return m_fBALANCE_UNHAPPINESS_PER_CULTURE_RATE;
	}
	inline int getBALANCE_HAPPINESS_ERA_BASE_INCREASE()
	{
		return m_iBALANCE_HAPPINESS_ERA_BASE_INCREASE;
	}
	inline int getBALANCE_HAPPINESS_POP_MULTIPLIER()
	{
		return m_iBALANCE_HAPPINESS_POP_MULTIPLIER;
	}
	inline int getBALANCE_UNHAPPY_CITY_BASE_VALUE()
	{
		return m_iBALANCE_UNHAPPY_CITY_BASE_VALUE;
	}
	inline int getBALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM()
	{
		return m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM;
	}
	inline int getBALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY()
	{
		return m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY;
	}
	inline int getBALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER()
	{
		return m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER;
	}
	inline int getBALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY()
	{
		return m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY;
	}
	inline int getBALANCE_HAPPINESS_ERA_MAX_INCREASE()
	{
		return m_iBALANCE_HAPPINESS_ERA_MAX_INCREASE;
	}
	inline int getBALANCE_TECH_RATE_CULTURE()
	{
		return m_iBALANCE_TECH_RATE_CULTURE;
	}
	inline int getBALANCE_TECH_RATE_SCIENCE()
	{
		return m_iBALANCE_TECH_RATE_SCIENCE;
	}
	inline int getBALANCE_TECH_RATE_DEFENSE()
	{
		return m_iBALANCE_TECH_RATE_DEFENSE;
	}
	inline int getBALANCE_TECH_RATE_GOLD()
	{
		return m_iBALANCE_TECH_RATE_GOLD;
	}
	inline int getBALANCE_FAITH_PERCENTAGE_VALUE()
	{
		return m_iBALANCE_FAITH_PERCENTAGE_VALUE;
	}
	inline int getBALANCE_SCIENCE_PERCENTAGE_VALUE()
	{
		return m_iBALANCE_SCIENCE_PERCENTAGE_VALUE;
	}
	inline int getBALANCE_GA_PERCENTAGE_VALUE()
	{
		return m_iBALANCE_GA_PERCENTAGE_VALUE;
	}
	inline int getBALANCE_CULTURE_PERCENTAGE_VALUE()
	{
		return m_iBALANCE_CULTURE_PERCENTAGE_VALUE;
	}
	inline int getBALANCE_WONDER_BEATEN_CONSOLATION_PRIZE()
	{
		return m_iBALANCE_WONDER_BEATEN_CONSOLATION_PRIZE;
	}
	inline int getBALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD()
	{
		return m_iBALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD;
	}
	inline int getBALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC()
	{
		return m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC;
	}
	inline int getBALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR()
	{
		return m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR;
	}
	inline int getBALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR()
	{
		return m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR;
	}
	inline int getBALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL()
	{
		return m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL;
	}
	inline int getBALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT()
	{
		return m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT;
	}
	inline int getBALANCE_CONQUEST_REDUCTION_BOOST()
	{
		return m_iBALANCE_CONQUEST_REDUCTION_BOOST;
	}
	inline int getBALANCE_SPY_BOOST_INFLUENCE_EXOTIC()
	{
		return m_iBALANCE_SPY_BOOST_INFLUENCE_EXOTIC;
	}
	inline int getBALANCE_SPY_BOOST_INFLUENCE_FAMILIAR()
	{
		return m_iBALANCE_SPY_BOOST_INFLUENCE_FAMILIAR;
	}
	inline int getBALANCE_SPY_BOOST_INFLUENCE_POPULAR()
	{
		return m_iBALANCE_SPY_BOOST_INFLUENCE_POPULAR;
	}
	inline int getBALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL()
	{
		return m_iBALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL;
	}
	inline int getBALANCE_SPY_BOOST_INFLUENCE_DOMINANT()
	{
		return m_iBALANCE_SPY_BOOST_INFLUENCE_DOMINANT;
	}
	inline int getBALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR()
	{
		return m_iBALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR;
	}
	inline int getBALANCE_GOLD_INFLUENCE_LEVEL_POPULAR()
	{
		return m_iBALANCE_GOLD_INFLUENCE_LEVEL_POPULAR;
	}
	inline int getBALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL()
	{
		return m_iBALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL;
	}
	inline int getBALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT()
	{
		return m_iBALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT;
	}
	inline int getBALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC()
	{
		return m_iBALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC;
	}
	inline int getBALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR()
	{
		return m_iBALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR;
	}
	inline int getBALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR()
	{
		return m_iBALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR;
	}
	inline int getBALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL()
	{
		return m_iBALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL;
	}
	inline int getBALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT()
	{
		return m_iBALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT;
	}
	inline int getBALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC()
	{
		return m_iBALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC;
	}
	inline int getBALANCE_EMBARK_DEFENSE_DIVISOR()
	{
		return m_iBALANCE_EMBARK_DEFENSE_DIVISOR;
	}
	inline int getBALANCE_NAVAL_DEFENSE_CITY_STRIKE_MODIFIER()
	{
		return m_iBALANCE_NAVAL_DEFENSE_CITY_STRIKE_MODIFIER ;
	}
	inline int getSETTLER_MOVE_ON_START()
	{
		return m_iSETTLER_MOVE_ON_START;
	}
	inline int getBALANCE_HAPPINESS_CAPITAL_MODIFIER()
	{
		return m_iBALANCE_HAPPINESS_CAPITAL_MODIFIER;
	}
	inline float getBALANCE_HAPPINESS_TECH_BASE_MODIFIER()
	{
		return m_fBALANCE_HAPPINESS_TECH_BASE_MODIFIER;
	}
	inline int getBALANCE_CITY_PURCHASE_MOD()
	{
		return m_iBALANCE_CITY_PURCHASE_MOD;
	}
	inline int getBALANCE_INFLUENCE_BOOST_PATRONAGE_POLICY()
	{
		return m_iBALANCE_INFLUENCE_BOOST_PATRONAGE_POLICY;
	}
	inline int getBALANCE_INFLUENCE_BOOST_PROTECTION_MINOR()
	{
		return m_iBALANCE_INFLUENCE_BOOST_PROTECTION_MINOR;
	}
	inline int getBALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY()
	{
		return m_iBALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY;
	}
	inline int getBALANCE_MOD_POLICIES_NEEDED_IDEOLOGY()
	{
		return m_iBALANCE_MOD_POLICIES_NEEDED_IDEOLOGY;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_FOOD()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_FOOD;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_GOLD()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_GOLD;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_FAITH()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_FAITH;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_PRODUCTION()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_PRODUCTION;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_SCIENCE()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_SCIENCE;
	}
	inline int getBALANCE_MINOR_PROTECTION_MINIMUM_DURATION()
	{
		return m_iBALANCE_MINOR_PROTECTION_MINIMUM_DURATION;
	}
	inline int getBALANCE_SCOUT_XP_RANDOM_VALUE()
	{
		return m_iBALANCE_SCOUT_XP_RANDOM_VALUE;
	}
	inline int getBALANCE_SCOUT_XP_MAXIMUM()
	{
		return m_iBALANCE_SCOUT_XP_MAXIMUM;
	}
	inline int getBALANCE_SCOUT_XP_NATURAL_WONDER()
	{
		return m_iBALANCE_SCOUT_XP_NATURAL_WONDER;
	}
	inline int getBALANCE_SCOUT_XP_BASE()
	{
		return m_iBALANCE_SCOUT_XP_BASE;
	}
	inline int getBALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER()
	{
		return m_iBALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER;
	}
	inline int getBALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER()
	{
		return m_iBALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER;
	}
	inline int getBALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER()
	{
		return m_iBALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER;
	}
	inline int getOPINION_WEIGHT_DP_WITH_ENEMY()
	{
		return m_iOPINION_WEIGHT_DP_WITH_ENEMY;
	}
	inline int getOPINION_WEIGHT_DP_WITH_FRIEND()
	{
		return m_iOPINION_WEIGHT_DP_WITH_FRIEND;
	}
	inline int getOPINION_WEIGHT_DP()
	{
		return m_iOPINION_WEIGHT_DP;
	}
	inline int getOPINION_WEIGHT_OPEN_BORDERS_MUTUAL()
	{
		return m_iOPINION_WEIGHT_OPEN_BORDERS_MUTUAL;
	}
	inline int getOPINION_WEIGHT_OPEN_BORDERS_US()
	{
		return m_iOPINION_WEIGHT_OPEN_BORDERS_US;
	}
	inline int getOPINION_WEIGHT_OPEN_BORDERS_THEM()
	{
		return m_iOPINION_WEIGHT_OPEN_BORDERS_THEM;
	}
	inline int getGWAM_THRESHOLD_DECREASE()
	{
		return m_iGWAM_THRESHOLD_DECREASE;
	}
	inline int getBALANCE_BUILDING_INVESTMENT_BASELINE()
	{
		return m_iBALANCE_BUILDING_INVESTMENT_BASELINE;
	}
	inline int getBALANCE_UNIT_INVESTMENT_BASELINE()
	{
		return m_iBALANCE_UNIT_INVESTMENT_BASELINE;
	}
	inline int getOPEN_BORDERS_MODIFIER_TRADE_GOLD()
	{
		return m_iOPEN_BORDERS_MODIFIER_TRADE_GOLD;
	}
	inline float  getMOD_BALANCE_CORE_MINIMUM_RANKING_PTP()
	{
		return m_fMOD_BALANCE_CORE_MINIMUM_RANKING_PTP;
	}
	inline int getMOD_BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT()
	{
		return m_iMOD_BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT;
	}
#endif

	// -- floats --

	inline float getDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT()
	{
		return m_fDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT;
	}
	inline float getDIPLO_VICTORY_CIV_DELEGATES_CONSTANT()
	{
		return m_fDIPLO_VICTORY_CIV_DELEGATES_CONSTANT;
	}
	inline float getDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT()
	{
		return m_fDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT;
	}
	inline float getDIPLO_VICTORY_CS_DELEGATES_CONSTANT()
	{
		return m_fDIPLO_VICTORY_CS_DELEGATES_CONSTANT;
	}
	inline float getAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT()
	{
		return m_fAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT;
	}
	inline float getAI_PRODUCTION_WEIGHT_BASE_MOD()
	{
		return m_fAI_PRODUCTION_WEIGHT_BASE_MOD;
	}
	inline float getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT()
	{
		return m_fAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT;
	}
	inline float getAI_RESEARCH_WEIGHT_BASE_MOD()
	{
		return m_fAI_RESEARCH_WEIGHT_BASE_MOD;
	}
	inline float getHURRY_GOLD_PRODUCTION_EXPONENT()
	{
		return m_fHURRY_GOLD_PRODUCTION_EXPONENT;
	}
	inline float getCITY_ZOOM_LEVEL_1()
	{
		return m_fCITY_ZOOM_LEVEL_1;
	}
	inline float getCITY_ZOOM_LEVEL_2()
	{
		return m_fCITY_ZOOM_LEVEL_2;
	}
	inline float getCITY_ZOOM_LEVEL_3()
	{
		return m_fCITY_ZOOM_LEVEL_3;
	}
	inline float getCITY_ZOOM_OFFSET()
	{
		return m_fCITY_ZOOM_OFFSET;
	}
	inline float getCULTURE_COST_LATER_PLOT_EXPONENT()
	{
		return m_fCULTURE_COST_LATER_PLOT_EXPONENT;
	}
	inline float getPOST_COMBAT_TEXT_DELAY()
	{
		return m_fPOST_COMBAT_TEXT_DELAY;
	}
	inline float getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER()
	{
		return m_fAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_WAR()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_WAR;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_HOSTILE()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_HOSTILE;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_DECEPTIVE()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_DECEPTIVE;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_GUARDED()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_GUARDED;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_AFRAID()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_AFRAID;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_FRIENDLY()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_FRIENDLY;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_NEUTRAL()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_NEUTRAL;
	}
	inline float getAI_DANGER_MINOR_APPROACH_NEUTRAL()
	{
		return m_fAI_DANGER_MINOR_APPROACH_NEUTRAL;
	}
	inline float getAI_DANGER_MINOR_APPROACH_FRIENDLY()
	{
		return m_fAI_DANGER_MINOR_APPROACH_FRIENDLY;
	}
	inline float getAI_DANGER_MINOR_APPROACH_BULLY()
	{
		return m_fAI_DANGER_MINOR_APPROACH_BULLY;
	}
	inline float getAI_DANGER_MINOR_APPROACH_CONQUEST()
	{
		return m_fAI_DANGER_MINOR_APPROACH_CONQUEST;
	}
	inline float getFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER()
	{
		return m_fFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER;
	}
	inline float getDIPLO_VICTORY_TEAM_MULTIPLIER()
	{
		return m_fDIPLO_VICTORY_TEAM_MULTIPLIER;
	}
	inline float getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION()
	{
		return m_fAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION;
	}
	inline float getAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY()
	{
		return m_fAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY;
	}
	inline float getAI_STRATEGY_NAVAL_UNITS_PER_CITY()
	{
		return m_fAI_STRATEGY_NAVAL_UNITS_PER_CITY;
	}
	inline float getGOLD_GIFT_FRIENDSHIP_EXPONENT()
	{
		return m_fGOLD_GIFT_FRIENDSHIP_EXPONENT;
	}
	inline float getGOLD_GIFT_FRIENDSHIP_DIVISOR()
	{
		return m_fGOLD_GIFT_FRIENDSHIP_DIVISOR;
	}
	inline float getHURRY_GOLD_TECH_EXPONENT()
	{
		return m_fHURRY_GOLD_TECH_EXPONENT;
	}
	inline float getHURRY_GOLD_CULTURE_EXPONENT()
	{
		return m_fHURRY_GOLD_CULTURE_EXPONENT;
	}
	inline float getCITY_GROWTH_MULTIPLIER()
	{
		return m_fCITY_GROWTH_MULTIPLIER;
	}
	inline float getCITY_GROWTH_EXPONENT()
	{
		return m_fCITY_GROWTH_EXPONENT;
	}
	inline float getPOLICY_COST_EXPONENT()
	{
		return m_fPOLICY_COST_EXPONENT;
	}
	inline float getPOLICY_COST_EXTRA_VALUE()
	{
		return m_fPOLICY_COST_EXTRA_VALUE;
	}
	inline float getUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA()
	{
		return m_fUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA;
	}
	inline float getUNIT_UPGRADE_COST_EXPONENT()
	{
		return m_fUNIT_UPGRADE_COST_EXPONENT;
	}
	inline float getCITY_STRENGTH_TECH_BASE()
	{
		return m_fCITY_STRENGTH_TECH_BASE;
	}
	inline float getCITY_STRENGTH_TECH_EXPONENT()
	{
		return m_fCITY_STRENGTH_TECH_EXPONENT;
	}

	// post defines

	inline int getLAND_TERRAIN()
	{
		return m_iLAND_TERRAIN;
	}
	inline int getDEEP_WATER_TERRAIN()
	{
		return m_iDEEP_WATER_TERRAIN;
	}
	inline int getSHALLOW_WATER_TERRAIN()
	{
		return m_iSHALLOW_WATER_TERRAIN;
	}
	inline int getRUINS_IMPROVEMENT()
	{
		return m_iRUINS_IMPROVEMENT;
	}
	inline int getNUKE_FEATURE()
	{
		return m_iNUKE_FEATURE;
	}
	inline int getARTIFACT_RESOURCE()
	{
		return m_iARTIFACT_RESOURCE;
	}
	inline int getHIDDEN_ARTIFACT_RESOURCE()
	{
		return m_iHIDDEN_ARTIFACT_RESOURCE;
	}
	inline int getCAPITAL_BUILDINGCLASS()
	{
		return m_iCAPITAL_BUILDINGCLASS;
	}
	inline int getDEFAULT_SPECIALIST()
	{
		return m_iDEFAULT_SPECIALIST;
	}
	inline int getSPACE_RACE_TRIGGER_PROJECT()
	{
		return m_iSPACE_RACE_TRIGGER_PROJECT;
	}
	inline int getSPACESHIP_CAPSULE()
	{
		return m_iSPACESHIP_CAPSULE;
	}
	inline int getSPACESHIP_BOOSTER()
	{
		return m_iSPACESHIP_BOOSTER;
	}
	inline int getSPACESHIP_STASIS()
	{
		return m_iSPACESHIP_STASIS;
	}
	inline int getSPACESHIP_ENGINE()
	{
		return m_iSPACESHIP_ENGINE;
	}
	inline int getMANHATTAN_PROJECT()
	{
		return m_iMANHATTAN_PROJECT;
	}
	inline int getINITIAL_CITY_ROUTE_TYPE()
	{
		return m_iINITIAL_CITY_ROUTE_TYPE;
	}
	inline int getSTANDARD_HANDICAP()
	{
		return m_iSTANDARD_HANDICAP;
	}
	inline int getMULTIPLAYER_HANDICAP()
	{
		return m_iMULTIPLAYER_HANDICAP;
	}
	inline int getSTANDARD_HANDICAP_QUICK()
	{
		return m_iSTANDARD_HANDICAP_QUICK;
	}
	inline int getSTANDARD_GAMESPEED()
	{
		return m_iSTANDARD_GAMESPEED;
	}
	inline int getSTANDARD_TURNTIMER()
	{
		return m_iSTANDARD_TURNTIMER;
	}
	inline int getSTANDARD_CLIMATE()
	{
		return m_iSTANDARD_CLIMATE;
	}
	inline int getSTANDARD_WORLD_SIZE()
	{
		return m_iSTANDARD_WORLD_SIZE;
	}
	inline int getSTANDARD_SEALEVEL()
	{
		return m_iSTANDARD_SEALEVEL;
	}
	inline int getSTANDARD_ERA()
	{
		return m_iSTANDARD_ERA;
	}
	inline int getLAST_EMBARK_ART_ERA()
	{
		return m_iLAST_EMBARK_ART_ERA;
	}
	inline int getLAST_UNIT_ART_ERA()
	{
		return m_iLAST_UNIT_ART_ERA;
	}
	inline int getLAST_BRIDGE_ART_ERA()
	{
		return m_iLAST_BRIDGE_ART_ERA;
	}
	inline int getSTANDARD_CALENDAR()
	{
		return m_iSTANDARD_CALENDAR;
	}
	inline int getBARBARIAN_HANDICAP()
	{
		return m_iBARBARIAN_HANDICAP;
	}
	inline int getBARBARIAN_CIVILIZATION()
	{
		return m_iBARBARIAN_CIVILIZATION;
	}
	inline int getBARBARIAN_LEADER()
	{
		return m_iBARBARIAN_LEADER;
	}
	inline int getMINOR_CIV_HANDICAP()
	{
		return m_iMINOR_CIV_HANDICAP;
	}
	inline int getMINOR_CIVILIZATION()
	{
		return m_iMINOR_CIVILIZATION;
	}
	inline int getPROMOTION_EMBARKATION()
	{
		return m_iPROMOTION_EMBARKATION;
	}
	inline int getPROMOTION_DEFENSIVE_EMBARKATION()
	{
		return m_iPROMOTION_DEFENSIVE_EMBARKATION;
	}
	inline int getPROMOTION_ALLWATER_EMBARKATION()
	{
		return m_iPROMOTION_ALLWATER_EMBARKATION;
	}
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	GD_INT_DEF(PROMOTION_DEEPWATER_EMBARKATION)
	GD_INT_DEF(PROMOTION_DEFENSIVE_DEEPWATER_EMBARKATION)
#endif
#if defined(MOD_PROMOTIONS_FLAGSHIP)
	GD_INT_DEF(PROMOTION_FLAGSHIP)
#endif
	inline int getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY()
	{
		return m_iPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY;
	}
	inline int getPROMOTION_OCEAN_IMPASSABLE()
	{
		return m_iPROMOTION_OCEAN_IMPASSABLE;
	}
	inline int getPROMOTION_ONLY_DEFENSIVE()
	{
		return m_iPROMOTION_ONLY_DEFENSIVE;
	}
	inline int getPROMOTION_UNWELCOME_EVANGELIST()
	{
		return m_iPROMOTION_UNWELCOME_EVANGELIST;
	}
	inline int getCOMBAT_CAPTURE_HEALTH()
	{
		return m_iCOMBAT_CAPTURE_HEALTH;
	}
	inline int getCOMBAT_CAPTURE_MIN_CHANCE()
	{
		return m_iCOMBAT_CAPTURE_MIN_CHANCE;
	}
	inline int getCOMBAT_CAPTURE_MAX_CHANCE()
	{
		return m_iCOMBAT_CAPTURE_MAX_CHANCE;
	}
	inline int getCOMBAT_CAPTURE_RATIO_MULTIPLIER()
	{
		return m_iCOMBAT_CAPTURE_RATIO_MULTIPLIER;
	}
	inline int getAI_HANDICAP()
	{
		return m_iAI_HANDICAP;
	}
	inline int getBARBARIAN_CAMP_IMPROVEMENT()
	{
		return m_iBARBARIAN_CAMP_IMPROVEMENT;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	inline int getEMBASSY_IMPROVEMENT()
	{
		return m_iEMBASSY_IMPROVEMENT;
	}
#endif
	inline int getRELIGION_MIN_FAITH_FIRST_PANTHEON()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_PANTHEON;
	}
	inline int getRELIGION_MIN_FAITH_FIRST_PROPHET()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_PROPHET;
	}
	inline int getRELIGION_MIN_FAITH_FIRST_GREAT_PERSON()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_GREAT_PERSON;
	}
	inline int getRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON()
	{
		return m_iRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON;
	}
	inline int getRELIGION_FAITH_DELTA_NEXT_PROPHET()
	{
		return m_iRELIGION_FAITH_DELTA_NEXT_PROPHET;
	}
	inline int getRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON()
	{
		return m_iRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON;
	}
	inline int getRELIGION_BASE_CHANCE_PROPHET_SPAWN()
	{
		return m_iRELIGION_BASE_CHANCE_PROPHET_SPAWN;
	}
	inline int getRELIGION_ATHEISM_PRESSURE_PER_POP()
	{
		return m_iRELIGION_ATHEISM_PRESSURE_PER_POP;
	}
	inline int getRELIGION_INITIAL_FOUNDING_CITY_PRESSURE()
	{
		return m_iRELIGION_INITIAL_FOUNDING_CITY_PRESSURE;
	}
	inline int getRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE()
	{
		return m_iRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE;
	}
	inline int getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER()
	{
		return m_iRELIGION_MISSIONARY_PRESSURE_MULTIPLIER;
	}
	inline int getRELIGION_ADJACENT_CITY_DISTANCE()
	{
		return m_iRELIGION_ADJACENT_CITY_DISTANCE;
	}
	inline int getRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_THRESHOLD()
	{
		return m_iRELIGION_DIPLO_HIT_THRESHOLD;
	}
	inline int getESPIONAGE_GATHERING_INTEL_COST_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_COST_PERCENT;
	}
	inline int getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT;
	}
	inline int getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT;
	}
	inline int getESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS()
	{
		return m_iESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS;
	}
	inline int getESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION()
	{
		return m_iESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION;
	}
	inline int getESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION()
	{
		return m_iESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION;
	}
	inline int getESPIONAGE_SURVEILLANCE_SIGHT_RANGE()
	{
		return m_iESPIONAGE_SURVEILLANCE_SIGHT_RANGE;
	}
	inline int getESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP()
	{
		return m_iESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP;
	}
	inline float getESPIONAGE_COUP_NOBODY_BONUS()
	{
		return m_fESPIONAGE_COUP_NOBODY_BONUS;
	}
	inline float getESPIONAGE_COUP_MULTIPLY_CONSTANT()
	{
		return m_fESPIONAGE_COUP_MULTIPLY_CONSTANT;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR;
	}
	inline int getINTERNATIONAL_TRADE_BASE()
	{
		return m_iINTERNATIONAL_TRADE_BASE;
	}
	inline int getINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION()
	{
		return m_iINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION;
	}
	inline int getINTERNATIONAL_TRADE_CITY_GPT_DIVISOR()
	{
		return m_iINTERNATIONAL_TRADE_CITY_GPT_DIVISOR;
	}
	inline int getPILLAGE_HEAL_AMOUNT()
	{
		return m_iPILLAGE_HEAL_AMOUNT;
	}
	inline int getCITY_CONNECTIONS_CONNECT_TO_CAPITAL()
	{
		return m_iCITY_CONNECTIONS_CONNECT_TO_CAPITAL;
	}
	inline float getUNRESEARCHED_TECH_BONUS_FROM_KILLS_SLOPE()
	{
		return m_fUNRESEARCHED_TECH_FROM_KILLS_SLOPE;
	}
	inline float getUNRESEARCHED_TECH_BONUS_FROM_KILLS_INTERCEPT()
	{
		return m_fUNRESEARCHED_TECH_FROM_KILLS_INTERCEPT;
	}
	inline int getBASE_CULTURE_PER_GREAT_WORK()
	{
		return m_iBASE_CULTURE_PER_GREAT_WORK;
	}
	inline int getBASE_TOURISM_PER_GREAT_WORK()
	{
		return m_iBASE_TOURISM_PER_GREAT_WORK;
	}
	inline int getTOURISM_MODIFIER_SHARED_RELIGION()
	{
		return m_iTOURISM_MODIFIER_SHARED_RELIGION;
	}
	inline int getTOURISM_MODIFIER_TRADE_ROUTE()
	{
		return m_iTOURISM_MODIFIER_TRADE_ROUTE;
	}
	inline int getTOURISM_MODIFIER_OPEN_BORDERS()
	{
		return m_iTOURISM_MODIFIER_OPEN_BORDERS;
	}
	inline int getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES()
	{
		return m_iTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES;
	}
	inline int getTOURISM_MODIFIER_DIPLOMAT()
	{
		return m_iTOURISM_MODIFIER_DIPLOMAT;
	}
	inline int getMINIUMUM_TOURISM_BLAST_STRENGTH()
	{
		return m_iMINIUMUM_TOURISM_BLAST_STRENGTH;
	}
	inline int getCULTURE_LEVEL_EXOTIC()
	{
		return m_iCULTURE_LEVEL_EXOTIC;
	}
	inline int getCULTURE_LEVEL_FAMILIAR()
	{
		return m_iCULTURE_LEVEL_FAMILIAR;
	}
	inline int getCULTURE_LEVEL_POPULAR()
	{
		return m_iCULTURE_LEVEL_POPULAR;
	}
	inline int getCULTURE_LEVEL_INFLUENTIAL()
	{
		return m_iCULTURE_LEVEL_INFLUENTIAL;
	}
	inline int getCULTURE_LEVEL_DOMINANT()
	{
		return m_iCULTURE_LEVEL_DOMINANT;
	}
	inline int getPROMOTION_GOODY_HUT_PICKER()
	{
		return m_iPROMOTION_GOODY_HUT_PICKER;
	}
	inline int getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF()
	{
		return m_iIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF;
	}
	inline int getIDEOLOGY_SCORE_GRAND_STRATS()
	{
		return m_iIDEOLOGY_SCORE_GRAND_STRATS;
	}
	inline int getIDEOLOGY_SCORE_HAPPINESS()
	{
		return m_iIDEOLOGY_SCORE_HAPPINESS;
	}
	inline int getIDEOLOGY_SCORE_PER_FREE_TENET()
	{
		return m_iIDEOLOGY_SCORE_PER_FREE_TENET;
	}
	inline int getIDEOLOGY_SCORE_HOSTILE()
	{
		return m_iIDEOLOGY_SCORE_HOSTILE;
	}
	inline int getIDEOLOGY_SCORE_GUARDED()
	{
		return m_iIDEOLOGY_SCORE_GUARDED;
	}
	inline int getIDEOLOGY_SCORE_AFRAID()
	{
		return m_iIDEOLOGY_SCORE_AFRAID;
	}
	inline int getIDEOLOGY_SCORE_FRIENDLY()
	{
		return m_iIDEOLOGY_SCORE_FRIENDLY;
	}
	inline int getPOLICY_BRANCH_FREEDOM()
	{
		return m_iPOLICY_BRANCH_FREEDOM;
	}
	inline int getPOLICY_BRANCH_AUTOCRACY()
	{
		return m_iPOLICY_BRANCH_AUTOCRACY;
	}
	inline int getPOLICY_BRANCH_ORDER()
	{
		return m_iPOLICY_BRANCH_ORDER;
	}
#if defined(MOD_ISKA_HERITAGE)
	inline int getPOLICY_BRANCH_HERITAGE()
	{
		return m_iPOLICY_BRANCH_HERITAGE;
	}
#endif

	inline int getMIN_DIG_SITES_PER_MAJOR_CIV()
	{
		return m_iMIN_DIG_SITES_PER_MAJOR_CIV;
	}
	inline int getMAX_DIG_SITES_PER_MAJOR_CIV()
	{
		return m_iMAX_DIG_SITES_PER_MAJOR_CIV;
	}
	inline int getPERCENT_SITES_HIDDEN()
	{
		return m_iPERCENT_SITES_HIDDEN;
	}
	inline int getPERCENT_HIDDEN_SITES_WRITING()
	{
		return m_iPERCENT_HIDDEN_SITES_WRITING;
	}
	inline int getSAPPED_CITY_ATTACK_MODIFIER()
	{
		return m_iSAPPED_CITY_ATTACK_MODIFIER;
	}
	inline int getSAPPER_BONUS_RANGE()
	{
		return m_iSAPPER_BONUS_RANGE;
	}

	inline int getEXOTIC_GOODS_GOLD_MIN()
	{
		return m_iEXOTIC_GOODS_GOLD_MIN;
	}
	inline int getEXOTIC_GOODS_GOLD_MAX()
	{
		return m_iEXOTIC_GOODS_GOLD_MAX;
	}
	inline int getEXOTIC_GOODS_XP_MIN()
	{
		return m_iEXOTIC_GOODS_XP_MIN;
	}
	inline int getEXOTIC_GOODS_XP_MAX()
	{
		return m_iEXOTIC_GOODS_XP_MAX;
	}

	inline int getTEMPORARY_CULTURE_BOOST_MOD()
	{
		return m_iTEMPORARY_CULTURE_BOOST_MOD;
	}
	inline int getTEMPORARY_TOURISM_BOOST_MOD()
	{
		return m_iTEMPORARY_TOURISM_BOOST_MOD;
	}

	inline int getLEAGUE_SESSION_INTERVAL_BASE_TURNS()
	{
		return m_iLEAGUE_SESSION_INTERVAL_BASE_TURNS;
	}
	inline int getLEAGUE_SESSION_SOON_WARNING_TURNS()
	{
		return m_iLEAGUE_SESSION_SOON_WARNING_TURNS;
	}
	inline int getLEAGUE_MEMBER_PROPOSALS_BASE()
	{
		return m_iLEAGUE_MEMBER_PROPOSALS_BASE;
	}
	inline int getLEAGUE_MEMBER_VOTES_BASE()
	{
		return m_iLEAGUE_MEMBER_VOTES_BASE;
	}
	inline int getLEAGUE_MEMBER_VOTES_FOR_HOST()
	{
		return m_iLEAGUE_MEMBER_VOTES_FOR_HOST;
	}
	inline int getLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY()
	{
		return m_iLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY;
	}
	inline float getLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD()
	{
		return m_fLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD;
	}
	inline float getLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD()
	{
		return m_fLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	inline int getLEAGUE_PROPOSERS_PER_SESSION()
	{
		return m_iLEAGUE_PROPOSERS_PER_SESSION;
	}
	inline int getLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES()
	{
		return m_iLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES;
	}
	inline int getLEAGUE_PROJECT_PROGRESS_PERCENT_WARNING()
	{
		return m_iLEAGUE_PROJECT_PROGRESS_PERCENT_WARNING;
	}
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	inline int getHELP_REQUEST_TURN_LIMIT_MIN()
	{
		return m_iHELP_REQUEST_TURN_LIMIT_MIN;
	}
	inline int getHELP_REQUEST_TURN_LIMIT_RAND()
	{
		return m_iHELP_REQUEST_TURN_LIMIT_RAND;
	}
	inline int getSHARE_OPINION_TURN_BUFFER()
	{
		return m_iSHARE_OPINION_TURN_BUFFER;
	}
	inline int getSHARE_OPINION_RAND()
	{
		return m_iSHARE_OPINION_RAND;
	}
	inline int getSHARE_OPINION_FLAVOR_BASE()
	{
		return m_iSHARE_OPINION_FLAVOR_BASE;
	}
	inline int getSHARE_OPINION_FLAVOR_MULTIPLIER()
	{
		return m_iSHARE_OPINION_FLAVOR_MULTIPLIER;
	}
	inline float getTECH_COST_ERA_EXPONENT()
	{
		return m_fTECH_COST_ERA_EXPONENT;
	}
	inline int getVASSAL_RELIGIOUS_PRESSURE_MODIFIER()
	{
		return m_iVASSAL_RELIGIOUS_PRESSURE_MODIFIER;
	}
	inline int getVASSAL_TOURISM_MODIFIER()
	{
		return m_iVASSAL_TOURISM_MODIFIER;
	}
	inline int getVASSALAGE_VASSAL_TAX_PERCENT_MINIMUM()
	{
		return m_iVASSALAGE_VASSAL_TAX_PERCENT_MINIMUM;
	}
	inline int getVASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM()
	{
		return m_iVASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM;
	}
	inline float getVASSALAGE_VASSAL_CITY_POP_EXPONENT()
	{
		return m_fVASSALAGE_VASSAL_CITY_POP_EXPONENT;
	}
	inline int getVASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT()
	{
		return m_iVASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT;
	}
	inline int getVASSAL_SCORE_PERCENT()
	{
		return m_iVASSAL_SCORE_PERCENT;
	}
	inline int getVASSAL_SCIENCE_PERCENT()
	{
		return m_iVASSAL_SCIENCE_PERCENT;
	}
	inline int getVASSAL_HAPPINESS_PERCENT()
	{
		return m_iVASSAL_HAPPINESS_PERCENT;
	}
	inline int getVASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT()
	{
		return m_iVASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT;
	}
	inline int getVASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT()
	{
		return m_iVASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT;
	}
	inline int getVASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT()
	{
		return m_iVASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT;
	}
	inline int getVASSALAGE_FAILED_PROTECT_CITY_DISTANCE()
	{
		return m_iVASSALAGE_FAILED_PROTECT_CITY_DISTANCE;
	}
	inline int getVASSALAGE_FAILED_PROTECT_PER_TURN_DECAY()
	{
		return m_iVASSALAGE_FAILED_PROTECT_PER_TURN_DECAY;
	}
	inline int getVASSALAGE_PROTECTED_PER_TURN_DECAY()
	{
		return m_iVASSALAGE_PROTECTED_PER_TURN_DECAY;
	}
	inline int getVASSALAGE_VASSAL_LOST_CITIES_THRESHOLD()
	{
		return m_iVASSALAGE_VASSAL_LOST_CITIES_THRESHOLD;
	}
	inline int getVASSALAGE_VASSAL_POPULATION_THRESHOLD()
	{
		return m_iVASSALAGE_VASSAL_POPULATION_THRESHOLD;
	}
	inline int getVASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD()
	{
		return m_iVASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD;
	}
	inline int getVASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD()
	{
		return m_iVASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD;
	}
	inline int getVASSALAGE_CAPITULATE_BASE_THRESHOLD()
	{
		return m_iVASSALAGE_CAPITULATE_BASE_THRESHOLD;
	}
	inline int getOPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE()
	{
		return m_iOPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD;
	}
	inline int getOPINION_WEIGHT_DEMANDED_WHILE_VASSAL()
	{
		return m_iOPINION_WEIGHT_DEMANDED_WHILE_VASSAL;
	}
	inline int getVASSALAGE_TREATMENT_THRESHOLD_DISAGREE()
	{
		return m_iVASSALAGE_TREATMENT_THRESHOLD_DISAGREE;
	}
	inline int getVASSALAGE_TREATMENT_THRESHOLD_MISTREATED()
	{
		return m_iVASSALAGE_TREATMENT_THRESHOLD_MISTREATED;
	}
	inline int getVASSALAGE_TREATMENT_THRESHOLD_UNHAPPY()
	{
		return m_iVASSALAGE_TREATMENT_THRESHOLD_UNHAPPY;
	}
	inline int getVASSALAGE_TREATMENT_THRESHOLD_ENSLAVED()
	{
		return m_iVASSALAGE_TREATMENT_THRESHOLD_ENSLAVED;
	}
	inline float getOPINION_WEIGHT_VASSAL_TAX_EXPONENT()
	{
		return m_fOPINION_WEIGHT_VASSAL_TAX_EXPONENT;
	}
	inline int getOPINION_WEIGHT_VASSAL_TAX_DIVISOR()
	{
		return m_iOPINION_WEIGHT_VASSAL_TAX_DIVISOR;
	}
	inline int getOPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER()
	{
		return m_iOPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_PROTECT_MAX()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_PROTECT_MAX;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT_OPINION_WEIGHT()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT_OPINION_WEIGHT;
	}
	inline int getOPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS()
	{
		return m_iOPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS;
	}

	inline int getAPPROACH_HOSTILE_WE_ARE_VASSAL()
	{
		return m_iAPPROACH_HOSTILE_WE_ARE_VASSAL;
	}
	inline int getAPPROACH_GUARDED_WE_ARE_VASSAL()
	{
		return m_iAPPROACH_GUARDED_WE_ARE_VASSAL;
	}
	inline int getAPPROACH_DECEPTIVE_WE_ARE_VASSAL()
	{
		return m_iAPPROACH_DECEPTIVE_WE_ARE_VASSAL;
	}
	inline int getAPPROACH_WAR_VASSAL_FORCEFULLY_REVOKED()
	{
		return m_iAPPROACH_WAR_VASSAL_FORCEFULLY_REVOKED;
	}
	inline int getAPPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED()
	{
		return m_iAPPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED;
	}
	inline int getAPPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED()
	{
		return m_iAPPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED;
	}
	inline int getAPPROACH_WAR_VASSAL_PEACEFULLY_REVOKED()
	{
		return m_iAPPROACH_WAR_VASSAL_PEACEFULLY_REVOKED;
	}
	inline int getAPPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED()
	{
		return m_iAPPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED;
	}
	inline int getAPPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED()
	{
		return m_iAPPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED;
	}
	inline int getAPPROACH_WAR_MY_VASSAL()
	{
		return m_iAPPROACH_WAR_MY_VASSAL;
	}	
	inline int getAPPROACH_GUARDED_TOO_MANY_VASSALS()
	{
		return m_iAPPROACH_GUARDED_TOO_MANY_VASSALS;
	}
	inline int getAPPROACH_WAR_TOO_MANY_VASSALS()
	{
		return m_iAPPROACH_WAR_TOO_MANY_VASSALS;
	}

	inline int getGLOBAL_STATE_NEARLY_DEFEATED_WEIGHT()
	{
		return m_iGLOBAL_STATE_NEARLY_DEFEATED_WEIGHT;
	}
	inline int getGLOBAL_STATE_DEFENSIVE_WEIGHT()
	{
		return m_iGLOBAL_STATE_DEFENSIVE_WEIGHT;
	}
	inline int getGLOBAL_STATE_STALEMATE_WEIGHT()
	{
		return m_iGLOBAL_STATE_STALEMATE_WEIGHT;
	}
	inline int getGLOBAL_STATE_CALM_WEIGHT()
	{
		return m_iGLOBAL_STATE_CALM_WEIGHT;
	}
	inline int getGLOBAL_STATE_OFFENSIVE_WEIGHT()
	{
		return m_iGLOBAL_STATE_OFFENSIVE_WEIGHT;
	}
	inline int getGLOBAL_STATE_NEARLY_WON_WEIGHT()
	{
		return m_iGLOBAL_STATE_NEARLY_WON_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_IMMENSE_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_IMMENSE_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_POWERFUL_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_POWERFUL_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_STRONG_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_STRONG_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_AVERAGE_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_AVERAGE_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_WEAK_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_WEAK_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_POOR_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_POOR_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIL_STRENGTH_PATHETIC_WEIGHT()
	{
		return m_iGLOBAL_STATE_MIL_STRENGTH_PATHETIC_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_IMMENSE_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_IMMENSE_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_POWERFUL_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_POWERFUL_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_STRONG_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_STRONG_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_AVERAGE_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_AVERAGE_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_WEAK_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_WEAK_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_POOR_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_POOR_WEIGHT;
	}
	inline int getGLOBAL_STATE_ECON_STRENGTH_PATHETIC_WEIGHT()
	{
		return m_iGLOBAL_STATE_ECON_STRENGTH_PATHETIC_WEIGHT;
	}
	inline int getGLOBAL_STATE_NUM_VASSALS_MULTIPLIER()
	{
		return m_iGLOBAL_STATE_NUM_VASSALS_MULTIPLIER;
	}
	inline int getGLOBAL_STATE_TECH_PERCENT_VERY_GOOD_WEIGHT()
	{
		return m_iGLOBAL_STATE_TECH_PERCENT_VERY_GOOD_WEIGHT;
	}
	inline int getGLOBAL_STATE_TECH_PERCENT_GOOD_WEIGHT()
	{
		return m_iGLOBAL_STATE_TECH_PERCENT_GOOD_WEIGHT;
	}
	inline int getGLOBAL_STATE_TECH_PERCENT_AVERAGE_WEIGHT()
	{
		return m_iGLOBAL_STATE_TECH_PERCENT_AVERAGE_WEIGHT;
	}
	inline int getGLOBAL_STATE_TECH_PERCENT_BAD_WEIGHT()
	{
		return m_iGLOBAL_STATE_TECH_PERCENT_BAD_WEIGHT;
	}
	inline int getGLOBAL_STATE_TECH_PERCENT_VERY_BAD_WEIGHT()
	{
		return m_iGLOBAL_STATE_TECH_PERCENT_VERY_BAD_WEIGHT;
	}
	inline int getGLOBAL_STATE_PER_POLICY_WEIGHT()
	{
		return m_iGLOBAL_STATE_PER_POLICY_WEIGHT;
	}
	inline int getGLOBAL_STATE_PER_ALLY_WEIGHT()
	{
		return m_iGLOBAL_STATE_PER_ALLY_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_EXTREMELY_GOOD_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_EXTREMELY_GOOD_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_VERY_GOOD_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_VERY_GOOD_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_GOOD_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_GOOD_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_AVERAGE_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_AVERAGE_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_BAD_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_BAD_WEIGHT;
	}
	inline int getGLOBAL_STATE_SCORE_PERCENT_VERY_BAD_WEIGHT()
	{
		return m_iGLOBAL_STATE_SCORE_PERCENT_VERY_BAD_WEIGHT;
	}
	inline int getGLOBAL_STATE_MIN_TURNS_BEFORE_PROCESS_STATE()
	{
		return m_iGLOBAL_STATE_MIN_TURNS_BEFORE_PROCESS_STATE;
	}
	inline int getTARGET_VASSAL_BACKUP_PATHETIC()
	{
		return m_iTARGET_VASSAL_BACKUP_PATHETIC;
	}
	inline int getTARGET_VASSAL_BACKUP_WEAK()
	{
		return m_iTARGET_VASSAL_BACKUP_WEAK;
	}
	inline int getTARGET_VASSAL_BACKUP_POOR()
	{
		return m_iTARGET_VASSAL_BACKUP_POOR;
	}
	inline int getTARGET_VASSAL_BACKUP_AVERAGE()
	{
		return m_iTARGET_VASSAL_BACKUP_AVERAGE;
	}
	inline int getTARGET_VASSAL_BACKUP_STRONG()
	{
		return m_iTARGET_VASSAL_BACKUP_STRONG;
	}
	inline int getTARGET_VASSAL_BACKUP_POWERFUL()
	{
		return m_iTARGET_VASSAL_BACKUP_POWERFUL;
	}
	inline int getTARGET_VASSAL_BACKUP_IMMENSE()
	{
		return m_iTARGET_VASSAL_BACKUP_IMMENSE;
	}
	inline int getTARGET_VASSAL_BACKUP_DISTANT()
	{
		return m_iTARGET_VASSAL_BACKUP_DISTANT;
	}
	inline int getTARGET_VASSAL_BACKUP_FAR()
	{
		return m_iTARGET_VASSAL_BACKUP_FAR;
	}
	inline int getTARGET_VASSAL_BACKUP_CLOSE()
	{
		return m_iTARGET_VASSAL_BACKUP_CLOSE;
	}
	inline int getTARGET_VASSAL_BACKUP_NEIGHBORS()
	{
		return m_iTARGET_VASSAL_BACKUP_NEIGHBORS;
	}
#if defined(MOD_PROMOTIONS_AURA_CHANGE)
	GD_INT_DEF(GREAT_GENERAL_MAX_RANGE)
#endif
#endif
	inline int getMAX_PLOTS_PER_EXPLORER()
	{
		return m_iMAX_PLOTS_PER_EXPLORER;
	}

	////////////// END DEFINES //////////////////

	void setDLLIFace(ICvEngineUtility4* pDll)
	{
		m_pDLL = pDll;
	}

	ICvEngineUtility4* getDLLIFace()
	{
		return m_pDLL;
	}

	void SetEngineUserInterface(ICvUserInterface2* pUI)
	{
		m_pEngineUI = pUI;
	}

	ICvUserInterface2* GetEngineUserInterface()
	{
		return m_pEngineUI;
	}

	bool IsGraphicsInitialized() const;
	void SetGraphicsInitialized(bool bVal);

	// for caching
	bool readEventInfoArray(FDataStream& kStream);
	void writeEventInfoArray(FDataStream& kStream);

	bool readEventTriggerInfoArray(FDataStream& kStream);
	void writeEventTriggerInfoArray(FDataStream& kStream);

	// So that CvEnums are moddable in the DLL
	int getNumDirections() const;
	int getNumGameOptions() const;
	int getNumMPOptions() const;
	int getNumSpecialOptions() const;
	int getNumGraphicOptions() const;
	int getNumCommandInfos() const;
	int getNumControlInfos() const;
	int getNumPlayerOptionInfos() const;
	int getNumGraphicLevels() const;

	bool getOutOfSyncDebuggingEnabled() const;
	void setOutOfSyncDebuggingEnabled(bool isEnabled);

	void deleteInfoArrays();

protected:

	bool m_bGraphicsInitialized;
	bool m_bDLLProfiler;
	bool m_bTutorialDisabled;
	bool m_bLogging;
	int  m_iRandLogging;
	bool m_bSynchLogging;
	bool m_bAILogging;
	bool m_bAIPerfLogging;
	bool m_bBuilderAILogging;
	bool m_bSerializationLogging;
	bool m_bPlayerAndCityAILogSplit;
	bool m_bTutorialLogging;
	bool m_bTutorialDebugging;
	bool m_bAllowRClickMovementWhileCameraScrolling;
	bool m_bPostTurnAutosaves;
	bool m_bOverwriteLogs;
	bool m_bOutOfSyncDebuggingEnabled;
	CvPoint3  m_pt3CameraDir;
	int m_iNewPlayers;

	bool m_bZoomOut;
	bool m_bZoomIn;
	bool m_bLoadGameFromFile;

	CvRandom* m_asyncRand;

	CvGame* m_game;

	CvMap* m_map;

	CvTwoLayerPathFinder* m_pathFinder;
	CvTwoLayerPathFinder* m_interfacePathFinder;
	CvStepFinder* m_stepFinder;
	
	ICvDLLDatabaseUtility1* m_pkDatabaseLoadUtility;
	int m_aiPlotDirectionX[NUM_DIRECTION_TYPES+2];
	int m_aiPlotDirectionY[NUM_DIRECTION_TYPES+2];

	int m_aiRingPlotXOffset[MAX_CITY_PLOTS];
	int m_aiRingPlotYOffset[MAX_CITY_PLOTS];
	int m_aiCityPlotPriority[MAX_CITY_PLOTS];
	int m_aaiRingPlotIndex[2*MAX_CITY_RADIUS+1][2*MAX_CITY_RADIUS+1];

	DirectionTypes m_aeTurnLeftDirection[NUM_DIRECTION_TYPES];
	DirectionTypes m_aeTurnRightDirection[NUM_DIRECTION_TYPES];

	FlowDirectionTypes* m_aeTurnLeftFlowDirection[NUM_FLOWDIRECTION_TYPES];
	FlowDirectionTypes* m_aeTurnRightFlowDirection[NUM_FLOWDIRECTION_TYPES];

	//InterfaceModeInfo m_aInterfaceModeInfo[NUM_INTERFACEMODE_TYPES] =
	std::vector<CvInterfaceModeInfo*> m_paInterfaceModeInfo;

	/***********************************************************************************************************************
	Globals loaded from Database
	************************************************************************************************************************/
	Database::Connection* m_pGameDatabase;
	Database::Results m_kGlobalDefinesLookup;

	// all type strings are upper case and are kept in this hash map for fast lookup, Moose
	InfosMap		m_infosMap;
	InfosHashMap	m_infosHashMap;		// Hash of the type string, mapped to the index of the info in its array.

	std::vector<CvColorInfo*> m_paColorInfo;
	std::vector<CvPlayerColorInfo*> m_paPlayerColorInfo;

#if defined(MOD_API_PLOT_YIELDS)
	std::vector<CvPlotInfo*> m_paPlotInfo;
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
	std::vector<CvGreatPersonInfo*> m_paGreatPersonInfo;
#endif
	std::vector<CvTerrainInfo*> m_paTerrainInfo;
	std::vector<CvYieldInfo*> m_paYieldInfo;
	std::vector<CvRouteInfo*> m_paRouteInfo;
	std::vector<CvFeatureInfo*> m_paFeatureInfo;
	std::vector<CvResourceClassInfo*> m_paResourceClassInfo;
	std::vector<CvResourceInfo*> m_paResourceInfo;
	std::vector<CvBuildInfo*> m_paBuildInfo;
	std::vector<CvHandicapInfo*> m_paHandicapInfo;
	std::vector<CvGameSpeedInfo*> m_paGameSpeedInfo;
#if defined(MOD_EVENTS_DIPLO_MODIFIERS)
	std::vector<CvDiploModifierInfo*> m_paDiploModifierInfo;
#endif
	std::vector<CvTurnTimerInfo*> m_paTurnTimerInfo;
	std::vector<CvCivilizationInfo*> m_paCivilizationInfo;
	int m_iNumPlayableCivilizationInfos;
	int m_iNumAIPlayableCivilizationInfos;
	std::vector<CvMinorCivInfo*> m_paMinorCivInfo;
	std::vector<CvLeaderHeadInfo*> m_paLeaderHeadInfo;
	std::vector<CvProcessInfo*> m_paProcessInfo;
	std::vector<CvVoteInfo*> m_paVoteInfo;
	std::vector<CvBuildingClassInfo*> m_paBuildingClassInfo;
	std::vector<CvUnitClassInfo*> m_paUnitClassInfo;
	std::vector<CvSpecialUnitInfo*> m_paSpecialUnitInfo;
	std::vector<CvVoteSourceInfo*> m_paVoteSourceInfo;
#if defined(MOD_BALANCE_CORE_EVENTS)
	std::vector<CvModEventInfo*> m_paEventInfo;
	std::vector<CvModEventChoiceInfo*> m_paEventChoiceInfo;
	std::vector<CvModCityEventInfo*> m_paCityEventInfo;
	std::vector<CvModEventCityChoiceInfo*> m_paCityEventChoiceInfo;
	std::vector<CvEventLinkingInfo*> m_paEventLinkingInfo;
	std::vector<CvEventChoiceLinkingInfo*> m_paEventChoiceLinkingInfo;
	std::vector<CvCityEventLinkingInfo*> m_paCityEventLinkingInfo;
	std::vector<CvCityEventChoiceLinkingInfo*> m_paCityEventChoiceLinkingInfo;
	std::vector<CvContractEntry*> m_paContractInfo;
#endif
	std::vector<CvBaseInfo*> m_paUnitCombatClassInfo;
	std::vector<CvBaseInfo*> m_paUnitAIInfos;
	std::vector<CvGameOptionInfo*> m_paGameOptionInfos;
	std::vector<CvMPOptionInfo*> m_paMPOptionInfos;
	std::vector<CvPlayerOptionInfo*> m_paPlayerOptionInfos;
	std::vector<CvSpecialistInfo*> m_paSpecialistInfo;
	std::vector<CvActionInfo*> m_paActionInfo;
	std::vector<CvMissionInfo*> m_paMissionInfo;
	std::vector<CvControlInfo*> m_paControlInfo;
	std::vector<CvCommandInfo*> m_paCommandInfo;
	std::vector<CvAutomateInfo*> m_paAutomateInfo;
	std::vector<CvEraInfo*> m_aEraInfo;	// [NUM_ERA_TYPES];
	std::vector<CvHurryInfo*> m_paHurryInfo;
	std::vector<CvVictoryInfo*> m_paVictoryInfo;
	std::vector<CvSmallAwardInfo*> m_paSmallAwardInfo;
	std::vector<CvEntityEventInfo*> m_paEntityEventInfo;
	std::vector<CvDomainInfo*> m_paUnitDomainInfo;

	std::vector<CvMultiUnitFormationInfo*> m_paMultiUnitFormationInfo;

	CvEconomicAIStrategyXMLEntries* m_pEconomicAIStrategies;
	CvCitySpecializationXMLEntries* m_pCitySpecializations;
	CvTacticalMoveXMLEntries* m_pTacticalMoves;
	CvMilitaryAIStrategyXMLEntries* m_pMilitaryAIStrategies;
	CvAIGrandStrategyXMLEntries* m_pAIGrandStrategies;
	CvAICityStrategies* m_pAICityStrategies;
	CvPolicyXMLEntries* m_pPolicies;
	CvTechXMLEntries* m_pTechs;
	CvBuildingXMLEntries* m_pBuildings;
	CvUnitXMLEntries* m_pUnits;
	CvProjectXMLEntries* m_pProjects;
	CvPromotionXMLEntries* m_pPromotions;
	CvImprovementXMLEntries* m_pImprovements;
	CvEmphasisXMLEntries* m_pEmphases;
	CvTraitXMLEntries* m_pTraits;
	CvReligionXMLEntries* m_pReligions;
	CvBeliefXMLEntries* m_pBeliefs;
	CvLeagueSpecialSessionXMLEntries* m_pLeagueSpecialSessions;
	CvLeagueNameXMLEntries* m_pLeagueNames;
	CvLeagueProjectXMLEntries* m_pLeagueProjects;
	CvLeagueProjectRewardXMLEntries* m_pLeagueProjectRewards;
	CvResolutionXMLEntries* m_pResolutions;
	CvNotificationXMLEntries* m_pNotifications;
#if defined(MOD_API_ACHIEVEMENTS) || defined(ACHIEVEMENT_HACKS)
	CvAchievementXMLEntries* m_pAchievements;
#endif
#if defined(MOD_BALANCE_CORE)
	CvCorporationXMLEntries* m_pCorporations;
	CvContractXMLEntries* m_pContracts;
#endif

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL TYPES
	//////////////////////////////////////////////////////////////////////////

	// XXX These are duplicates and are kept for enumeration convenience - most could be removed, Moose
	CvString* m_paszFlavorTypes;
	int m_iNumFlavorTypes;

	CvString* m_paszFootstepAudioTags;
	int m_iNumFootstepAudioTags;

	//////////////////////////////////////////////////////////////////////////
	// Formerly Global Defines
	//////////////////////////////////////////////////////////////////////////

	int getDefineINT(const char* szName, bool bReportErrors = true);
	float getDefineFLOAT(const char* szName, bool bReportErrors = true);

	bool getDefineValue(const char* szName, int& iValue, bool bReportErrors = true);
	bool getDefineValue(const char* szName, float& fValue, bool bReportErrors = true);
	bool getDefineValue(const char* szName, CvString& szValue, bool bReportErrors = true);

	// -- ints --

	int m_iAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD;
	int m_iINITIAL_AI_CITY_PRODUCTION;
	int m_iAI_CAN_DISBAND_UNITS;
	int m_iAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT;
	int m_iPOLICY_WEIGHT_PROPAGATION_PERCENT;
	int m_iPOLICY_WEIGHT_PROPAGATION_LEVELS;
	int m_iPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH;
	int m_iTECH_WEIGHT_PROPAGATION_PERCENT;
	int m_iTECH_WEIGHT_PROPAGATION_LEVELS;
	int m_iTECH_PRIORITY_UNIQUE_ITEM;
	int m_iTECH_PRIORITY_MAYA_CALENDAR_BONUS;
	int m_iAI_VALUE_OF_YIELD_GOLD;
	int m_iAI_VALUE_OF_YIELD_PRODUCTION;
	int m_iAI_VALUE_OF_YIELD_SCIENCE;
	int m_iDEFAULT_FLAVOR_VALUE;
	int m_iPERSONALITY_FLAVOR_MAX_VALUE;
	int m_iPERSONALITY_FLAVOR_MIN_VALUE;
	int m_iFLAVOR_MIN_VALUE;
	int m_iFLAVOR_MAX_VALUE;
	int m_iFLAVOR_RANDOMIZATION_RANGE;
	int m_iFLAVOR_EXPANDGROW_COEFFICIENT;
	int m_iAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE;
	int m_iAI_GS_RAND_ROLL;
	int m_iAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT;
	int m_iAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT;
	int m_iAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD;
	int m_iAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD;
	int m_iAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER;
	int m_iAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN;
	int m_iAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT;
	int m_iAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT;
	int m_iAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN;
	int m_iAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER;
	int m_iAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED;
	int m_iAI_GS_CULTURE_RATIO_MULTIPLIER;
	int m_iAI_GS_TOURISM_RATIO_MULTIPLIER;
	int m_iAI_GS_CULTURE_AHEAD_WEIGHT;
	int m_iAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT;
	int m_iAI_GS_CULTURE_INFLUENTIAL_CIV_MOD;
	int m_iAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT;
	int m_iAI_GS_UN_SECURED_VOTE_MOD;
	int m_iAI_GS_SS_HAS_APOLLO_PROGRAM;
	int m_iAI_GS_SS_TECH_PROGRESS_MOD;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int m_iEVENT_MIN_DURATION_BETWEEN;
	int m_iBALANCE_HAPPINESS_THRESHOLD_PERCENTILE;
	int m_iGLOBAL_RESOURCE_MONOPOLY_THRESHOLD;
	int m_iSTRATEGIC_RESOURCE_MONOPOLY_THRESHOLD;
	int m_iRELIGION_MIN_FAITH_SECOND_PROPHET;
#endif
	int m_iAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT;
	int m_iAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR;
	int m_iAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR;
#if defined(MOD_BALANCE_CORE_SETTLER)
	int m_iAI_STRATEGY_EARLY_EXPANSION_RELATIVE_TILE_QUALITY;
#else
	int m_iAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED;
#endif
	int m_iAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT;
	float m_fAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO;
	int m_iAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT;
	int m_iAI_STRATEGY_AREA_IS_FULL_PERCENT;
	int m_iAI_STRATEGY_MINIMUM_SETTLE_FERTILITY;
	int m_iAI_BUY_PLOT_TEST_PROBES;
	int m_iAI_PLOT_VALUE_STRATEGIC_RESOURCE;
	int m_iAI_PLOT_VALUE_LUXURY_RESOURCE;
	int m_iAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER;
	int m_iAI_PLOT_VALUE_YIELD_MULTIPLIER;
	int m_iAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER;
	int m_iAI_PLOT_VALUE_FIERCE_DISPUTE;
	int m_iAI_PLOT_VALUE_STRONG_DISPUTE;
	int m_iAI_PLOT_VALUE_WEAK_DISPUTE;
	int m_iAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE;
	int m_iAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM;
	int m_iAI_GOLD_PRIORITY_UPGRADE_BASE;
	int m_iAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT;
	int m_iAI_GOLD_PRIORITY_DIPLOMACY_BASE;
	int m_iAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT;
	int m_iAI_GOLD_PRIORITY_UNIT;
	int m_iAI_GOLD_PRIORITY_DEFENSIVE_BUILDING;
	int m_iAI_GOLD_PRIORITY_BUYOUT_CITY_STATE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE;
	int m_iBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY;
	int m_iBUILDER_TASKING_BUILD_ROUTE_TENDENCY;
	int m_iBUILDER_TASKING_DIRECTIVES_TO_EVALUATE;
	int m_iBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS;
	int m_iBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS;
	int m_iBUILDER_TASKING_BASELINE_BUILD_ROUTES;
	int m_iBUILDER_TASKING_BASELINE_REPAIR;
	int m_iBUILDER_TASKING_BASELINE_SCRUB_FALLOUT;
	int m_iBUILDER_TASKING_BASELINE_ADDS_CULTURE;
	int m_iAI_MILITARY_THREAT_WEIGHT_MINOR;
	int m_iAI_MILITARY_THREAT_WEIGHT_MAJOR;
	int m_iAI_MILITARY_THREAT_WEIGHT_SEVERE;
	int m_iAI_MILITARY_THREAT_WEIGHT_CRITICAL;
	int m_iAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS;
	int m_iAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE;
	int m_iAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL;
	int m_iAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT;
	int m_iAI_MILITARY_IGNORE_BAD_ODDS;
	int m_iAI_MILITARY_RECAPTURING_OWN_CITY;
	int m_iAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL;
	int m_iAI_CITY_SPECIALIZATION_EARLIEST_TURN;
	int m_iAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL;
	int m_iAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY;
	int m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD;
	int m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE;
	int m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE;
	int m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED;
	int m_iAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN;
	int m_iAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE;
	int m_iAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT;
	int m_iAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER;
	int m_iAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT;
	int m_iAI_CITIZEN_VALUE_FOOD;
	int m_iAI_CITIZEN_VALUE_PRODUCTION;
	int m_iAI_CITIZEN_VALUE_GOLD;
	int m_iAI_CITIZEN_VALUE_SCIENCE;
	int m_iAI_CITIZEN_VALUE_CULTURE;
	int m_iAI_CITIZEN_VALUE_FAITH;
	int m_iAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF;
	int m_iAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT;
	int m_iAI_CITIZEN_MOD_FOOD_DEFICIT;
	int m_iAI_CITIZEN_MOD_PRODUCTION_DEFICIT;
	int m_iAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER;
	int m_iAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION;
	int m_iAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER;
	int m_iAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE;
	int m_iAI_TACTICAL_MAP_TEMP_ZONE_RADIUS;
	int m_iAI_TACTICAL_MAP_TEMP_ZONE_TURNS;
	int m_iAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS;
	int m_iAI_TACTICAL_RECRUIT_RANGE;
	int m_iAI_TACTICAL_REPOSITION_RANGE;
	int m_iAI_TACTICAL_OVERKILL_PERCENT;
	int m_iAI_TACTICAL_BARBARIAN_RELEASE_VARIATION;
	int m_iAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS;
	int m_iAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS;
	int m_iAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS;
	int m_iAI_HOMELAND_ESTIMATE_TURNS_DISTANCE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SETTLE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_HEAL;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WORKER;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA;
	int m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SENTRY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_PATROL;
	int m_iAI_HOMELAND_MOVE_PRIORITY_UPGRADE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS;
	int m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WRITER;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ARTIST;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MUSICIAN;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iAI_HOMELAND_MOVE_PRIORITY_DIPLOMAT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MESSENGER;
#endif
	int m_iAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MISSIONARY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_INQUISITOR;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART;
	int m_iAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TREASURE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST;
	int m_iAI_HOMELAND_MOVE_PRIORITY_AIRLIFT;
	int m_iAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT;
	int m_iAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE;
#if defined(MOD_CONFIG_AI_IN_XML)
	GD_INT_DECL(AI_CONFIG_MILITARY_MELEE_PER_AA);
	GD_INT_DECL(AI_CONFIG_MILITARY_AIRCRAFT_PER_CARRIER_SPACE);
	GD_INT_DECL(AI_CONFIG_MILITARY_TILES_PER_SHIP);
	GD_INT_DECL(AI_HOMELAND_MOVE_PRIORITY_SECONDARY_SETTLER);
	GD_INT_DECL(AI_HOMELAND_MOVE_PRIORITY_SECONDARY_WORKER);
#endif
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DECL(RELIGION_LAST_FOUND_ERA);
	GD_INT_DECL(RELIGION_GP_FAITH_PURCHASE_ERA);
	GD_INT_DECL(IDEOLOGY_START_ERA);
	GD_INT_DECL(IDEOLOGY_PREREQ_ERA);
#endif
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	GD_INT_DECL(TOURISM_START_TECH);
	GD_INT_DECL(TOURISM_START_ERA);
#endif
	int m_iMINOR_BULLY_GOLD;
	int m_iMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM;
	int m_iMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR;
	int m_iMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE;
	int m_iMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN;
	int m_iMINOR_FRIENDSHIP_AT_WAR;
	int m_iMINOR_CIV_AGGRESSOR_THRESHOLD;
	int m_iMINOR_CIV_WARMONGER_THRESHOLD;
	int m_iPERMANENT_WAR_AGGRESSOR_CHANCE;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS;
	int m_iPERMANENT_WAR_OTHER_CHANCE_DISTANT;
	int m_iPERMANENT_WAR_OTHER_CHANCE_FAR;
	int m_iPERMANENT_WAR_OTHER_CHANCE_CLOSE;
	int m_iPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS;
	int m_iPERMANENT_WAR_OTHER_AT_WAR;
	int m_iFRIENDSHIP_NEUTRAL_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_NEUTRAL;
	int m_iFRIENDSHIP_FRIENDS_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_FRIENDS;
	int m_iFRIENDSHIP_ALLIES_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_ALLIES;
	int m_iFRIENDSHIP_THRESHOLD_MAX;
	int m_iFRIENDSHIP_THRESHOLD_CAN_BULLY;
	int m_iFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT;
#if defined(MOD_CITY_STATE_SCALE)
	GD_INT_DECL(FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL);
	GD_INT_DECL(FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL);
#endif
	int m_iMINOR_FRIENDSHIP_CLOSE_AMOUNT;
	int m_iMINOR_CIV_SCIENCE_BONUS_MULTIPLIER;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	int m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	int m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	int m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	int m_iALLIES_CAPITAL_FOOD_BONUS_AMOUNT;
	int m_iALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT;
	int m_iFRIENDS_BASE_TURNS_UNIT_SPAWN;
	int m_iFRIENDS_RAND_TURNS_UNIT_SPAWN;
	int m_iALLIES_EXTRA_TURNS_UNIT_SPAWN;
	int m_iUNIT_SPAWN_BIAS_MULTIPLIER;
	int m_iUNIT_SPAWN_NUM_CHOICES;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY;
	int m_iMINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	int m_iMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	int m_iMAX_DISTANCE_MINORS_BARB_QUEST;
	int m_iTXT_KEY_MINOR_GIFT_UNITS_REMINDER;
	int m_iWAR_QUEST_COMPLETE_FRIENDSHIP;
	int m_iWAR_QUEST_UNITS_TO_KILL_DIVISOR;
	int m_iWAR_QUEST_MIN_UNITS_TO_KILL;
	int m_iMINOR_QUEST_FRIENDSHIP_ROUTE;
	int m_iMINOR_QUEST_FRIENDSHIP_KILL_CAMP;
	int m_iMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE;
	int m_iMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER;
	int m_iMINOR_QUEST_FRIENDSHIP_GREAT_PERSON;
	int m_iMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE;
	int m_iMINOR_QUEST_FRIENDSHIP_FIND_PLAYER;
	int m_iMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER;
	int m_iMINOR_QUEST_FRIENDSHIP_GIVE_GOLD;
	int m_iMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS;
	int m_iMINOR_QUEST_FRIENDSHIP_INVEST;
	int m_iMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE;
	int m_iMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR;
	int m_iMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION;
	int m_iMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	int m_iMINOR_QUEST_FRIENDSHIP_WAR;
	int m_iMINOR_QUEST_FRIENDSHIP_FIND_CITY_STATE;
	int m_iMINOR_QUEST_FRIENDSHIP_INFLUENCE;
	int m_iMINOR_QUEST_FRIENDSHIP_TOURISM;
	int m_iMINOR_QUEST_FRIENDSHIP_ARCHAEOLOGY;
	int m_iMINOR_QUEST_FRIENDSHIP_CIRCUMNAVIGATION;
	int m_iIDEOLOGY_PER_CITY_UNHAPPY;
	int m_iIDEOLOGY_POP_PER_UNHAPPY;
	int m_iWARMONGER_THREAT_PER_TURN_DECAY_INCREASED;
	int m_iWARMONGER_THREAT_PER_TURN_DECAY_DECREASED;
	int m_iWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_WAR;
	int m_iWARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE;
	int m_iWARMONGER_THREAT_DECREASED_WORLD_WAR;
	int m_iSCHOLAR_MINOR_ALLY_MULTIPLIER;
	int m_iSCIENCE_LEAGUE_GREAT_WORK_MODIFIER;
	int m_iPIONEER_POPULATION_CHANGE;
	int m_iPIONEER_EXTRA_PLOTS;
	int m_iPIONEER_FOOD_PERCENT;
	int m_iCOLONIST_POPULATION_CHANGE;
	int m_iCOLONIST_EXTRA_PLOTS;
	int	m_iCOLONIST_FOOD_PERCENT;
	int m_iMINOR_QUEST_REBELLION_TIMER;
#endif
	int m_iMINOR_QUEST_STANDARD_CONTEST_LENGTH;
	int m_iMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER;
	int m_iMINOR_CIV_GOLD_GIFT_GAME_DIVISOR;
	int m_iMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR;
	int m_iMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD;
	int m_iMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS;
	int m_iMINOR_CIV_RESOURCE_SEARCH_RADIUS;
	int m_iFRIENDSHIP_PER_UNIT_INTRUDING;
	int m_iFRIENDSHIP_PER_BARB_KILLED;
	int m_iFRIENDSHIP_PER_UNIT_GIFTED;
	int m_iMAX_INFLUENCE_FROM_MINOR_GIFTS;
	int m_iMINOR_LIBERATION_FRIENDSHIP;
	int m_iRETURN_CIVILIAN_FRIENDSHIP;
	int m_iMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER;
	int m_iMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER;
	int m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN;
	int m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	int m_iMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN;
	int m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN;
	int m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	int m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN;
	int m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	int m_iMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN;
	int m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN;
	int m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	int m_iMINOR_CIV_QUEST_KILL_CAMP_RANGE;
	int m_iMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD;
	int m_iMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD;
	int m_iMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY;
	int m_iMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED;
	int m_iQUEST_DISABLED_ROUTE;
	int m_iQUEST_DISABLED_KILL_CAMP;
	int m_iQUEST_DISABLED_CONNECT_RESOURCE;
	int m_iQUEST_DISABLED_CONSTRUCT_WONDER;
	int m_iQUEST_DISABLED_GREAT_PERSON;
	int m_iQUEST_DISABLED_KILL_CITY_STATE;
	int m_iQUEST_DISABLED_FIND_PLAYER;
	int m_iQUEST_DISABLED_NATURAL_WONDER;
	int m_iQUEST_DISABLED_GIVE_GOLD;
	int m_iQUEST_DISABLED_PLEDGE_TO_PROTECT;
	int m_iQUEST_DISABLED_CONTEST_CULTURE;
	int m_iQUEST_DISABLED_CONTEST_FAITH;
	int m_iQUEST_DISABLED_CONTEST_TECHS;
	int m_iQUEST_DISABLED_INVEST;
	int m_iQUEST_DISABLED_BULLY_CITY_STATE;
	int m_iQUEST_DISABLED_DENOUNCE_MAJOR;
	int m_iQUEST_DISABLED_SPREAD_RELIGION;
	int m_iQUEST_DISABLED_TRADE_ROUTE;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	int m_iQUEST_DISABLED_WAR;
	int m_iQUEST_DISABLED_FIND_CITY_STATE;
	int m_iQUEST_DISABLED_INFLUENCE;
	int m_iQUEST_DISABLED_TOURISM;
	int m_iQUEST_DISABLED_ARCHAEOLOGY;
	int m_iQUEST_DISABLED_CIRCUMNAVIGATION;
	int m_iQUEST_DISABLED_LIBERATION;
	int m_iQUEST_DISABLED_HORDE;
	int m_iQUEST_DISABLED_REBELLION;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iUNITCLASS_FOR_CS_BULLY;
	int m_iQUEST_DISABLED_CP_QUESTS;
#endif
	int m_iMINOR_CIV_QUEST_WEIGHT_DEFAULT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	int m_iMINOR_CIV_QUEST_WAR_MILITARISTIC_VALUE;
	int m_iMINOR_CIV_QUEST_WAR_HOSTILE_VALUE;
	int m_iMINOR_CIV_QUEST_WAR_CULTURED_VALUE;
	int m_iMINOR_CIV_QUEST_NATIONAL_WONDER_CULTURED_VALUE;
	int m_iMINOR_CIV_QUEST_FIND_CITYSTATE_MARITIME_VALUE;
	int m_iMINOR_CIV_QUEST_FIND_CITYSTATE_MERCANTILE_VALUE;
	int m_iMINOR_CIV_QUEST_ARCHAEOLOGY_MILITARISTIC_VALUE;
	int m_iMINOR_CIV_QUEST_ARCHAEOLOGY_RELIGIOUS_VALUE;
	int m_iMINOR_CIV_QUEST_CIRCUMNAVIGATION_MARITIME_VALUE;
	int m_iMINOR_CIV_QUEST_INFLUENCE_HOSTILE_VALUE;
	int m_iMINOR_CIV_QUEST_TOURISM_FRIENDLY_VALUE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION;
	int m_iINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE;
	int m_iAPPROACH_WAR_MINOR_QUEST_WAR;
	int	m_iAI_MILITARY_RECAPTURING_CITY_STATE;
	int m_iNEED_DIPLOMAT_THRESHOLD_MODIFIER;
	int m_iNEED_DIPLOMAT_DESIRE_MODIFIER;
	int m_iNEED_DIPLOMAT_DISTASTE_MODIFIER;
	int m_iINFLUENCE_MINOR_QUEST_BOOST;
	int m_iQUEST_REBELLION_FREQUENCY;
	int m_iBARBARIAN_HORDE_FREQUENCY;
	int m_iLEAGUE_AID_MAX;
	int m_iCSD_GOLD_GIFT_DISABLED;
	int m_iCSD_DIPLO_BUILDING_YIELD;
	int m_iBALANCE_GAME_DIFFICULTY_MULTIPLIER;
#endif
	int m_iRELIGION_BELIEF_SCORE_CITY_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER;
	int m_iRELIGION_MISSIONARY_RANGE_IN_TURNS;
	int m_iRELIGION_MAX_MISSIONARIES;
	int m_iMC_GIFT_WEIGHT_THRESHOLD;
	int m_iMC_ALWAYS_GIFT_DIPLO_THRESHOLD;
	int m_iMC_SOMETIMES_GIFT_RAND_MULTIPLIER;
	int m_iMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	int m_iMC_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	int m_iMC_GIFT_WEIGHT_MARITIME_GROWTH;
	int m_iMC_GIFT_WEIGHT_DIPLO_VICTORY;
	int m_iMC_GIFT_WEIGHT_CULTURE_VICTORY;
	int m_iMC_GIFT_WEIGHT_CONQUEST_VICTORY;
	int m_iMC_GIFT_WEIGHT_MILITARISTIC;
	int m_iMC_GIFT_WEIGHT_RESOURCE_WE_NEED;
	int m_iMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL;
	int m_iMC_GIFT_WEIGHT_PROTECTIVE;
	int m_iMC_GIFT_WEIGHT_HOSTILE;
	int m_iMC_GIFT_WEIGHT_NEIGHBORS;
	int m_iMC_GIFT_WEIGHT_CLOSE;
	int m_iMC_GIFT_WEIGHT_FAR;
	int m_iMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES;
	int m_iMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS;
	int m_iOPINION_WEIGHT_LAND_FIERCE;
	int m_iOPINION_WEIGHT_LAND_STRONG;
	int m_iOPINION_WEIGHT_LAND_WEAK;
	int m_iOPINION_WEIGHT_LAND_NONE;
	int m_iOPINION_WEIGHT_VICTORY_FIERCE;
	int m_iOPINION_WEIGHT_VICTORY_STRONG;
	int m_iOPINION_WEIGHT_VICTORY_WEAK;
	int m_iOPINION_WEIGHT_VICTORY_NONE;
	int m_iOPINION_WEIGHT_WONDER_FIERCE;
	int m_iOPINION_WEIGHT_WONDER_STRONG;
	int m_iOPINION_WEIGHT_WONDER_WEAK;
	int m_iOPINION_WEIGHT_WONDER_NONE;
	int m_iOPINION_WEIGHT_MINOR_CIV_FIERCE;
	int m_iOPINION_WEIGHT_MINOR_CIV_STRONG;
	int m_iOPINION_WEIGHT_MINOR_CIV_WEAK;
	int m_iOPINION_WEIGHT_MINOR_CIV_NONE;
	int m_iOPINION_WEIGHT_WARMONGER_CRITICAL;
	int m_iOPINION_WEIGHT_WARMONGER_SEVERE;
	int m_iOPINION_WEIGHT_WARMONGER_MAJOR;
	int m_iOPINION_WEIGHT_WARMONGER_MINOR;
	int m_iOPINION_WEIGHT_WARMONGER_NONE;
	int m_iOPINION_WEIGHT_ASKED_NO_SETTLE;
	int m_iOPINION_WEIGHT_ASKED_STOP_SPYING;
	int m_iOPINION_WEIGHT_MADE_DEMAND_OF_US;
	int m_iOPINION_WEIGHT_RETURNED_CIVILIAN;
	int m_iOPINION_WEIGHT_BUILT_LANDMARK;
	int m_iOPINION_WEIGHT_RESURRECTED;
	int m_iOPINION_WEIGHT_LIBERATED_ONE_CITY;
	int m_iOPINION_WEIGHT_LIBERATED_TWO_CITIES;
	int m_iOPINION_WEIGHT_LIBERATED_THREE_CITIES;
	int m_iOPINION_WEIGHT_EMBASSY;
	int m_iOPINION_WEIGHT_CULTURE_BOMBED;
	int m_iOPINION_WEIGHT_PER_NEGATIVE_CONVERSION;
	int m_iOPINION_WEIGHT_ADOPTING_HIS_RELIGION;
	int m_iOPINION_WEIGHT_ADOPTING_MY_RELIGION;
	int m_iOPINION_WEIGHT_SAME_LATE_POLICIES;
	int m_iOPINION_WEIGHT_DIFFERENT_LATE_POLICIES;
	int m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD;
	int m_iOPINION_WEIGHT_IGNORED_MILITARY_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_BORDER_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_BORDER_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD;
	int m_iOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_SPY_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_SPY_PROMISE;
	int m_iOPINION_WEIGHT_FORGAVE_FOR_SPYING;
	int m_iOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE;
	int m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR;
	int m_iOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO;
	int m_iOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO;
	int m_iOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR;
	int m_iOPINION_WEIGHT_DOF;
	int m_iOPINION_WEIGHT_DOF_WITH_FRIEND;
	int m_iOPINION_WEIGHT_DOF_WITH_ENEMY;
	int m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE;
	int m_iOPINION_WEIGHT_DENOUNCED_FRIEND_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_ME_FRIENDS;
	int m_iOPINION_WEIGHT_WAR_FRIEND_EACH;
	int m_iOPINION_WEIGHT_WAR_ME_FRIENDS;
	int m_iOPINION_WEIGHT_REFUSED_REQUEST_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_ME;
	int m_iOPINION_WEIGHT_DENOUNCED_THEM;
	int m_iOPINION_WEIGHT_DENOUNCED_FRIEND;
	int m_iOPINION_WEIGHT_DENOUNCED_ENEMY;
	int m_iOPINION_WEIGHT_RECKLESS_EXPANDER;
	int m_iOPINION_WEIGHT_TRADE_MAX;
	int m_iOPINION_WEIGHT_COMMON_FOE_MAX;
	int m_iOPINION_WEIGHT_ASSIST_MAX;
	int m_iOPINION_WEIGHT_LIBERATED_CAPITAL;
	int m_iOPINION_WEIGHT_LIBERATED_CITY;
	int m_iOPINION_WEIGHT_GAVE_ASSISTANCE;
	int m_iOPINION_WEIGHT_PAID_TRIBUTE;
	int m_iOPINION_WEIGHT_NUKED_MAX;
	int m_iOPINION_WEIGHT_ROBBED_BY;
	int m_iOPINION_WEIGHT_INTRIGUE_SHARED_BY;
	int m_iOPINION_WEIGHT_CAPTURED_CAPITAL;
	int m_iOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL;
	int m_iOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS;
	int m_iOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL;
	int m_iOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS;
	int m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL;
	int m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS;
	int m_iOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL;
	int m_iOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS;
	int m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING;
	int m_iOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS;
	int m_iOPINION_THRESHOLD_UNFORGIVABLE;
	int m_iOPINION_THRESHOLD_ENEMY;
	int m_iOPINION_THRESHOLD_COMPETITOR;
	int m_iOPINION_THRESHOLD_FAVORABLE;
	int m_iOPINION_THRESHOLD_FRIEND;
	int m_iOPINION_THRESHOLD_ALLY;
	int m_iAPPROACH_NEUTRAL_DEFAULT;
	int m_iAPPROACH_BIAS_FOR_CURRENT;
	int m_iAPPROACH_WAR_CURRENTLY_DECEPTIVE;
	int m_iAPPROACH_HOSTILE_CURRENTLY_HOSTILE;
	int m_iAPPROACH_WAR_CURRENTLY_WAR;
	int m_iAPPROACH_RANDOM_PERCENT;
	int m_iAPPROACH_WAR_CONQUEST_GRAND_STRATEGY;
	int m_iAPPROACH_OPINION_UNFORGIVABLE_WAR;
	int m_iAPPROACH_OPINION_UNFORGIVABLE_HOSTILE;
	int m_iAPPROACH_OPINION_UNFORGIVABLE_DECEPTIVE;
	int m_iAPPROACH_OPINION_UNFORGIVABLE_GUARDED;
	int m_iAPPROACH_OPINION_ENEMY_WAR;
	int m_iAPPROACH_OPINION_ENEMY_HOSTILE;
	int m_iAPPROACH_OPINION_ENEMY_DECEPTIVE;
	int m_iAPPROACH_OPINION_ENEMY_GUARDED;
	int m_iAPPROACH_OPINION_COMPETITOR_WAR;
	int m_iAPPROACH_OPINION_COMPETITOR_HOSTILE;
	int m_iAPPROACH_OPINION_COMPETITOR_DECEPTIVE;
	int m_iAPPROACH_OPINION_COMPETITOR_GUARDED;
	int m_iAPPROACH_OPINION_NEUTRAL_DECEPTIVE;
	int m_iAPPROACH_OPINION_NEUTRAL_FRIENDLY;
	int m_iAPPROACH_OPINION_FAVORABLE_HOSTILE;
	int m_iAPPROACH_OPINION_FAVORABLE_DECEPTIVE;
	int m_iAPPROACH_OPINION_FAVORABLE_FRIENDLY;
	int m_iAPPROACH_OPINION_FRIEND_HOSTILE;
	int m_iAPPROACH_OPINION_FRIEND_FRIENDLY;
	int m_iAPPROACH_OPINION_ALLY_FRIENDLY;
	int m_iAPPROACH_DECEPTIVE_WORKING_WITH_PLAYER;
	int m_iAPPROACH_FRIENDLY_WORKING_WITH_PLAYER;
	int m_iAPPROACH_HOSTILE_WORKING_WITH_PLAYER;
	int m_iAPPROACH_GUARDED_WORKING_WITH_PLAYER;
	int m_iAPPROACH_DECEPTIVE_WORKING_AGAINST_PLAYER;
	int m_iAPPROACH_HOSTILE_WORKING_AGAINST_PLAYER;
	int m_iAPPROACH_WAR_WORKING_AGAINST_PLAYER;
	int m_iAPPROACH_WAR_DENOUNCED;
	int m_iAPPROACH_HOSTILE_DENOUNCED;
	int m_iAPPROACH_GUARDED_DENOUNCED;
	int m_iAPPROACH_FRIENDLY_DENOUNCED;
	int m_iAPPROACH_DECEPTIVE_DENOUNCED;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_WAR;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_HOSTILE;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_GUARDED;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_WAR;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_HOSTILE;
	int m_iAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_GUARDED;
	int m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL;
	int m_iAPPROACH_GUARDED_MILITARY_THREAT_CRITICAL;
	int m_iAPPROACH_AFRAID_MILITARY_THREAT_CRITICAL;
	int m_iAPPROACH_DECEPTIVE_DEMAND;
	int m_iAPPROACH_FRIENDLY_DEMAND;
	int m_iAPPROACH_WAR_BROKEN_MILITARY_PROMISE;
	int m_iAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE;
	int m_iAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE;
	int m_iAPPROACH_WAR_BROKEN_MILITARY_PROMISE_WORLD;
	int m_iAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE_WORLD;
	int m_iAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE_WORLD;
	int m_iAPPROACH_DECEPTIVE_IGNORED_MILITARY_PROMISE;
	int m_iAPPROACH_FRIENDLY_IGNORED_MILITARY_PROMISE;
	int m_iAPPROACH_WAR_BROKEN_EXPANSION_PROMISE;
	int m_iAPPROACH_DECEPTIVE_BROKEN_EXPANSION_PROMISE;
	int m_iAPPROACH_FRIENDLY_BROKEN_EXPANSION_PROMISE;
	int m_iAPPROACH_WAR_IGNORED_EXPANSION_PROMISE;
	int m_iAPPROACH_DECEPTIVE_IGNORED_EXPANSION_PROMISE;
	int m_iAPPROACH_FRIENDLY_IGNORED_EXPANSION_PROMISE;
	int m_iAPPROACH_WAR_BROKEN_BORDER_PROMISE;
	int m_iAPPROACH_DECEPTIVE_BROKEN_BORDER_PROMISE;
	int m_iAPPROACH_FRIENDLY_BROKEN_BORDER_PROMISE;
	int m_iAPPROACH_WAR_IGNORED_BORDER_PROMISE;
	int m_iAPPROACH_DECEPTIVE_IGNORED_BORDER_PROMISE;
	int m_iAPPROACH_FRIENDLY_IGNORED_BORDER_PROMISE;
	int m_iAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE;
	int m_iAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE;
	int m_iAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE;
	int m_iAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE_WORLD;
	int m_iAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE_WORLD;
	int m_iAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE_WORLD;
	int m_iAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE;
	int m_iAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE;
	int m_iAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE;
	int m_iAPPROACH_FRIENDLY_MILITARY_THREAT_CRITICAL;
	int m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_SEVERE;
	int m_iAPPROACH_GUARDED_MILITARY_THREAT_SEVERE;
	int m_iAPPROACH_AFRAID_MILITARY_THREAT_SEVERE;
	int m_iAPPROACH_FRIENDLY_MILITARY_THREAT_SEVERE;
	int m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_MAJOR;
	int m_iAPPROACH_GUARDED_MILITARY_THREAT_MAJOR;
	int m_iAPPROACH_AFRAID_MILITARY_THREAT_MAJOR;
	int m_iAPPROACH_FRIENDLY_MILITARY_THREAT_MAJOR;
	int m_iAPPROACH_DECEPTIVE_MILITARY_THREAT_MINOR;
	int m_iAPPROACH_GUARDED_MILITARY_THREAT_MINOR;
	int m_iAPPROACH_AFRAID_MILITARY_THREAT_MINOR;
	int m_iAPPROACH_FRIENDLY_MILITARY_THREAT_MINOR;
	int m_iAPPROACH_HOSTILE_MILITARY_THREAT_NONE;
	int m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL;
	int m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING;
	int m_iAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING;
	int m_iAPPROACH_HOSTILE_AT_WAR_WITH_PLAYER;
	int m_iAPPROACH_DECEPTIVE_AT_WAR_WITH_PLAYER;
	int m_iAPPROACH_GUARDED_AT_WAR_WITH_PLAYER;
	int m_iAPPROACH_FRIENDLY_AT_WAR_WITH_PLAYER;
	int m_iAPPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_HOSTILE_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_DECEPTIVE_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_GUARDED_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_FRIENDLY_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_WAR_HOSTILE_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_HOSTILE_HOSTILE_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_DECEPTIVE_HOSTILE_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_FRIENDLY_HOSTILE_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_WAR_AFRAID_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_HOSTILE_AFRAID_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_DECEPTIVE_AFRAID_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_FRIENDLY_AFRAID_WITH_ANOTHER_PLAYER;
	int m_iAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT;
	int m_iAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT;
	int m_iAPPROACH_WAR_PROJECTION_STALEMATE_PERCENT;
	int m_iAPPROACH_WAR_PROJECTION_UNKNOWN_PERCENT;
	int m_iAPPROACH_WAR_PROJECTION_GOOD_PERCENT;
	int m_iAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_DESTRUCTION_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_DEFEAT_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_STALEMATE_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_UNKNOWN_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_GOOD_PERCENT;
	int m_iAPPROACH_GUARDED_PROJECTION_VERY_GOOD_PERCENT;
	int m_iTURNS_SINCE_PEACE_WEIGHT_DAMPENER;
	int m_iAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT;
	int m_iAPPROACH_WAR_RECKLESS_EXPANDER;
	int m_iAPPROACH_WAR_PROXIMITY_NEIGHBORS;
	int m_iAPPROACH_WAR_PROXIMITY_CLOSE;
	int m_iAPPROACH_WAR_PROXIMITY_FAR;
	int m_iAPPROACH_WAR_PROXIMITY_DISTANT;
	int m_iMINOR_APPROACH_IGNORE_DEFAULT;
	int m_iMINOR_APPROACH_BIAS_FOR_CURRENT;
	int m_iMINOR_APPROACH_IGNORE_CURRENTLY_WAR;
	int m_iMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_PROTECTIVE_CONQUEST_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_FRIENDLY_CONQUEST_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY_NEIGHBORS;
	int m_iMINOR_APPROACH_WAR_DIPLO_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_IGNORE_DIPLO_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_PROTECTIVE_DIPLO_GRAND_STRATEGY_NEIGHBORS;
	int m_iMINOR_APPROACH_WAR_CULTURE_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_IGNORE_CULTURE_GRAND_STRATEGY;
	int m_iMINOR_APPROACH_PROTECTIVE_CULTURE_GRAND_STRATEGY_CST;
	int m_iMINOR_APPROACH_WAR_CURRENTLY_PROTECTIVE;
	int m_iMINOR_APPROACH_PROTECTIVE_CURRENTLY_PROTECTIVE;
	int m_iMINOR_APPROACH_PROTECTIVE_CURRENTLY_WAR;
	int m_iMINOR_APPROACH_FRIENDLY_CURRENTLY_WAR;
	int m_iMINOR_APPROACH_FRIENDLY_RESOURCES;
	int m_iMINOR_APPROACH_PROTECTIVE_RESOURCES;
	int m_iMINOR_APPROACH_WAR_FRIENDS;
	int m_iMINOR_APPROACH_FRIENDLY_FRIENDS;
	int m_iMINOR_APPROACH_PROTECTIVE_FRIENDS;
	int m_iMINOR_APPROACH_IGNORE_PROXIMITY_NEIGHBORS;
	int m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_NEIGHBORS;
	int m_iMINOR_APPROACH_PROTECTIVE_PROXIMITY_NEIGHBORS;
	int m_iMINOR_APPROACH_CONQUEST_PROXIMITY_NEIGHBORS;
	int m_iMINOR_APPROACH_IGNORE_PROXIMITY_CLOSE;
	int m_iMINOR_APPROACH_PROTECTIVE_PROXIMITY_CLOSE;
	int m_iMINOR_APPROACH_CONQUEST_PROXIMITY_CLOSE;
	int m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_FAR;
	int m_iMINOR_APPROACH_CONQUEST_PROXIMITY_FAR;
	int m_iMINOR_APPROACH_FRIENDLY_PROXIMITY_DISTANT;
	int m_iMINOR_APPROACH_CONQUEST_PROXIMITY_DISTANT;
	int m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_FRIENDLY;
	int m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_PROTECTIVE;
	int m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_NEUTRAL;
	int m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_NEUTRAL;
	int m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_HOSTILE;
	int m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_HOSTILE;
	int m_iMINOR_APPROACH_CONQUEST_PERSONALITY_HOSTILE;
	int m_iMINOR_APPROACH_FRIENDLY_PERSONALITY_IRRATIONAL;
	int m_iMINOR_APPROACH_PROTECTIVE_PERSONALITY_IRRATIONAL;
	int m_iMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER;
	int m_iMINOR_APPROACH_PROTECTIVE_WITH_ANOTHER_PLAYER;
	int m_iMINOR_APPROACH_WAR_TARGET_IMPOSSIBLE;
	int m_iMINOR_APPROACH_WAR_TARGET_BAD;
	int m_iMINOR_APPROACH_WAR_TARGET_AVERAGE;
	int m_iMINOR_APPROACH_WAR_TARGET_FAVORABLE;
	int m_iMINOR_APPROACH_WAR_TARGET_SOFT;
	int m_iCOOPERATION_DESIRE_THRESHOLD_EAGER;
	int m_iCOOPERATION_DESIRE_THRESHOLD_STRONG;
	int m_iCOOPERATION_DESIRE_THRESHOLD_DECENT;
	int m_iCOOPERATION_DESIRE_THRESHOLD_WEAK;
	int m_iCOOPERATION_DESIRE_WAR_STATE_CALM;
	int m_iCOOPERATION_DESIRE_WAR_STATE_NEARLY_WON;
	int m_iCOOPERATION_DESIRE_WAR_STATE_OFFENSIVE;
	int m_iCOOPERATION_DESIRE_WAR_STATE_STALEMATE;
	int m_iCOOPERATION_DESIRE_WAR_STATE_DEFENSIVE;
	int m_iCOOPERATION_DESIRE_WAR_STATE_NEARLY_DEFEATED;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_IMMENSE;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_POWERFUL;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_STRONG;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_AVERAGE;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_POOR;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_WEAK;
	int m_iCOOPERATION_DESIRE_MILITARY_STRENGTH_PATHETIC;
	int m_iCOOPERATION_DESIRE_OPINION_ALLY;
	int m_iCOOPERATION_DESIRE_OPINION_FRIEND;
	int m_iCOOPERATION_DESIRE_OPINION_NEUTRAL;
	int m_iCOOPERATION_DESIRE_OPINION_COMPETITOR;
	int m_iCOOPERATION_DESIRE_OPINION_ENEMY;
	int m_iCOOPERATION_DESIRE_GAME_THREAT_CRITICAL;
	int m_iCOOPERATION_DESIRE_GAME_THREAT_SEVERE;
	int m_iCOOPERATION_DESIRE_GAME_THREAT_MAJOR;
	int m_iCOOPERATION_DESIRE_GAME_THREAT_MINOR;
	int m_iCOOPERATION_DESIRE_GAME_THREAT_NONE;
	int m_iCOOPERATION_DESIRE_THEM_US_SAME_COMPETITOR;
	int m_iCOOPERATION_DESIRE_THEM_US_COMPETITOR_ENEMY;
	int m_iCOOPERATION_DESIRE_THEM_US_ENEMY_COMPETITOR;
	int m_iCOOPERATION_DESIRE_THEM_US_SAME_ENEMY;
	int m_iEVALUATE_WAR_PLAYER_THRESHOLD;
	int m_iEVALUATE_WAR_WILLING_TO_DECLARE_THRESHOLD;
	int m_iEVALUATE_WAR_CONQUEST_GRAND_STRATEGY;
	int m_iEVALUATE_WAR_NOT_PREPARED;
	int m_iEVALUATE_WAR_ALREADY_FIGHTING;
	int m_iEVALUATE_WAR_GAME_THREAT_CRITICAL;
	int m_iEVALUATE_WAR_GAME_THREAT_SEVERE;
	int m_iEVALUATE_WAR_GAME_THREAT_MAJOR;
	int m_iEVALUATE_WAR_GAME_THREAT_MINOR;
	int m_iEVALUATE_WAR_GAME_THREAT_NONE;
	int m_iEVALUATE_WAR_SOFT_TARGET;
	int m_iEVALUATE_WAR_FAVORABLE_TARGET;
	int m_iEVALUATE_WAR_AVERAGE_TARGET;
	int m_iEVALUATE_WAR_BAD_TARGET;
	int m_iEVALUATE_WAR_IMPOSSIBLE_TARGET;
	int m_iREQUEST_PEACE_TURN_THRESHOLD;
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DECL(WAR_MAJOR_MINIMUM_TURNS);
	GD_INT_DECL(WAR_MINOR_MINIMUM_TURNS);
#endif
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_DESTRUCTION;
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_DEFEAT;
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_STALEMATE;
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_UNKNOWN;
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_GOOD;
	int m_iPEACE_WILLINGNESS_OFFER_PROJECTION_VERY_GOOD;
	int m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE;
	int m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR;
	int m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR;
	int m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS;
	int m_iPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT;
	int m_iPEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_DESTRUCTION;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_DEFEAT;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_STALEMATE;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_UNKNOWN;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_GOOD;
	int m_iPEACE_WILLINGNESS_ACCEPT_PROJECTION_VERY_GOOD;
	int m_iPEACE_WILLINGNESS_ACCEPT_TARGET_IMPOSSIBLE;
	int m_iPEACE_WILLINGNESS_ACCEPT_TARGET_BAD;
	int m_iPEACE_WILLINGNESS_ACCEPT_TARGET_AVERAGE;
	int m_iPEACE_WILLINGNESS_ACCEPT_TARGET_FAVORABLE;
	int m_iPEACE_WILLINGNESS_ACCEPT_TARGET_SOFT;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT;
	int m_iPEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE;
	int m_iARMISTICE_GPT_DIVISOR;
	int m_iARMISTICE_MIN_GOLD;
	int m_iWAR_GOAL_THRESHOLD_CONQUEST;
	int m_iWAR_GOAL_THRESHOLD_DAMAGE;
	int m_iWAR_GOAL_THRESHOLD_PEACE;
	int m_iWAR_GOAL_STATE_CALM;
	int m_iWAR_GOAL_STATE_NEARLY_WON;
	int m_iWAR_GOAL_STATE_OFFENSIVE;
	int m_iWAR_GOAL_STATE_STALEMATE;
	int m_iWAR_GOAL_STATE_DEFENSIVE;
	int m_iWAR_GOAL_STATE_NEARLY_DEFEATED;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_CALM;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_WON;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_OFFENSIVE;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_STALEMATE;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_DEFENSIVE;
	int m_iWAR_GOAL_ANOTHER_PLAYER_STATE_NEARLY_DEFEATED;
	int m_iWAR_GOAL_DAMAGE_CRIPPLED;
	int m_iWAR_GOAL_DAMAGE_SERIOUS;
	int m_iWAR_GOAL_DAMAGE_MAJOR;
	int m_iWAR_GOAL_DAMAGE_MINOR;
	int m_iWAR_GOAL_DAMAGE_NONE;
	int m_iWAR_GOAL_GAME_THREAT_CRITICAL;
	int m_iWAR_GOAL_GAME_THREAT_SEVERE;
	int m_iWAR_GOAL_GAME_THREAT_MAJOR;
	int m_iWAR_GOAL_GAME_THREAT_MINOR;
	int m_iWAR_GOAL_GAME_THREAT_NONE;
	int m_iWAR_GOAL_TARGET_SOFT;
	int m_iWAR_GOAL_TARGET_FAVORABLE;
	int m_iWAR_GOAL_TARGET_AVERAGE;
	int m_iWAR_GOAL_TARGET_BAD;
	int m_iWAR_GOAL_TARGET_IMPOSSIBLE;
	int m_iWAR_PROJECTION_THRESHOLD_VERY_GOOD;
	int m_iWAR_PROJECTION_THRESHOLD_GOOD;
	int m_iWAR_PROJECTION_THRESHOLD_DESTRUCTION;
	int m_iWAR_PROJECTION_THRESHOLD_DEFEAT;
	int m_iWAR_PROJECTION_THRESHOLD_STALEMATE;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_PATHETIC;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_WEAK;
	int m_iWAR_PROJECTION_RECKLESS_EXPANDER;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POOR;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_AVERAGE;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_STRONG;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POWERFUL;
	int m_iWAR_PROJECTION_THEIR_MILITARY_STRENGTH_IMMENSE;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_PATHETIC;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_WEAK;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POOR;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_AVERAGE;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_STRONG;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POWERFUL;
	int m_iWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_IMMENSE;
	int m_iWAR_PROJECTION_WAR_DAMAGE_US_NONE;
	int m_iWAR_PROJECTION_WAR_DAMAGE_US_MINOR;
	int m_iWAR_PROJECTION_WAR_DAMAGE_US_MAJOR;
	int m_iWAR_PROJECTION_WAR_DAMAGE_US_SERIOUS;
	int m_iWAR_PROJECTION_WAR_DAMAGE_US_CRIPPLED;
	int m_iWAR_PROJECTION_WAR_DAMAGE_THEM_NONE;
	int m_iWAR_PROJECTION_WAR_DAMAGE_THEM_MINOR;
	int m_iWAR_PROJECTION_WAR_DAMAGE_THEM_MAJOR;
	int m_iWAR_PROJECTION_WAR_DAMAGE_THEM_SERIOUS;
	int m_iWAR_PROJECTION_WAR_DAMAGE_THEM_CRIPPLED;
	int m_iWAR_PROJECTION_WAR_DURATION_SCORE_CAP;
	int m_iWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES;
	int m_iWAR_STATE_THRESHOLD_NEARLY_WON;
	int m_iWAR_STATE_THRESHOLD_OFFENSIVE;
	int m_iWAR_STATE_THRESHOLD_STALEMATE;
	int m_iWAR_STATE_THRESHOLD_DEFENSIVE;
	int m_iWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED;
	int m_iWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS;
	int m_iWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR;
	int m_iWAR_DAMAGE_LEVEL_THRESHOLD_MINOR;
	int m_iWAR_DAMAGE_LEVEL_CITY_WEIGHT;
	int m_iWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER;
	int m_iWAR_DAMAGE_LEVEL_UNINVOLVED_CITY_POP_MULTIPLIER;
	int m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE;
	int m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH;
	int m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM;
	int m_iMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW;
	int m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_HIGH;
	int m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_MEDIUM;
	int m_iEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_LOW;
	int m_iPLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD;
	int m_iPLOT_BUYING_POSTURE_HIGH_THRESHOLD;
	int m_iPLOT_BUYING_POSTURE_MEDIUM_THRESHOLD;
	int m_iPLOT_BUYING_POSTURE_LOW_THRESHOLD;
	int m_iMILITARY_STRENGTH_BASE;
	int m_iMILITARY_STRENGTH_RATIO_MULTIPLIER;
	int m_iMILITARY_STRENGTH_CITY_MOD;
	int m_iMILITARY_STRENGTH_IMMENSE_THRESHOLD;
	int m_iMILITARY_STRENGTH_POWERFUL_THRESHOLD;
	int m_iMILITARY_STRENGTH_STRONG_THRESHOLD;
	int m_iMILITARY_STRENGTH_AVERAGE_THRESHOLD;
	int m_iMILITARY_STRENGTH_POOR_THRESHOLD;
	int m_iMILITARY_STRENGTH_WEAK_THRESHOLD;
	int m_iECONOMIC_STRENGTH_RATIO_MULTIPLIER;
	int m_iECONOMIC_STRENGTH_IMMENSE_THRESHOLD;
	int m_iECONOMIC_STRENGTH_POWERFUL_THRESHOLD;
	int m_iECONOMIC_STRENGTH_STRONG_THRESHOLD;
	int m_iECONOMIC_STRENGTH_AVERAGE_THRESHOLD;
	int m_iECONOMIC_STRENGTH_POOR_THRESHOLD;
	int m_iECONOMIC_STRENGTH_WEAK_THRESHOLD;
	int m_iTARGET_IMPOSSIBLE_THRESHOLD;
	int m_iTARGET_BAD_THRESHOLD;
	int m_iTARGET_AVERAGE_THRESHOLD;
	int m_iTARGET_FAVORABLE_THRESHOLD;
	int m_iTARGET_INCREASE_WAR_TURNS;
	int m_iTARGET_MILITARY_STRENGTH_IMMENSE;
	int m_iTARGET_MILITARY_STRENGTH_POWERFUL;
	int m_iTARGET_MILITARY_STRENGTH_STRONG;
	int m_iTARGET_MILITARY_STRENGTH_AVERAGE;
	int m_iTARGET_MILITARY_STRENGTH_POOR;
	int m_iTARGET_MILITARY_STRENGTH_WEAK;
	int m_iTARGET_MILITARY_STRENGTH_PATHETIC;
	int m_iTARGET_ALREADY_WAR_EACH_PLAYER;
	int m_iTARGET_NEIGHBORS;
	int m_iTARGET_CLOSE;
	int m_iTARGET_FAR;
	int m_iTARGET_DISTANT;
	int m_iTARGET_MINOR_BACKUP_PATHETIC;
	int m_iTARGET_MINOR_BACKUP_WEAK;
	int m_iTARGET_MINOR_BACKUP_POOR;
	int m_iTARGET_MINOR_BACKUP_AVERAGE;
	int m_iTARGET_MINOR_BACKUP_STRONG;
	int m_iTARGET_MINOR_BACKUP_POWERFUL;
	int m_iTARGET_MINOR_BACKUP_IMMENSE;
	int m_iTARGET_MINOR_BACKUP_DISTANT;
	int m_iTARGET_MINOR_BACKUP_FAR;
	int m_iTARGET_MINOR_BACKUP_CLOSE;
	int m_iTARGET_MINOR_BACKUP_NEIGHBORS;
	int m_iGAME_THREAT_CRITICAL_THRESHOLD;
	int m_iGAME_THREAT_SEVERE_THRESHOLD;
	int m_iGAME_THREAT_MAJOR_THRESHOLD;
	int m_iGAME_THREAT_MINOR_THRESHOLD;
	int m_iGAME_THREAT_AT_WAR_STALEMATE;
	int m_iGAME_THREAT_AT_WAR_DEFENSIVE;
	int m_iGAME_THREAT_AT_WAR_NEARLY_DEFEATED;
	int m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_IMMENSE;
	int m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_POWERFUL;
	int m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_STRONG;
	int m_iGAME_THREAT_AT_WAR_MILITARY_STRENGTH_AVERAGE;
	int m_iGAME_THREAT_GUESS_AT_WAR;
	int m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_IMMENSE;
	int m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_POWERFUL;
	int m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_STRONG;
	int m_iGAME_THREAT_GUESS_AT_WAR_MILITARY_STRENGTH_AVERAGE;
	int m_iMILITARY_THREAT_CRITICAL_THRESHOLD;
	int m_iMILITARY_THREAT_SEVERE_THRESHOLD;
	int m_iMILITARY_THREAT_MAJOR_THRESHOLD;
	int m_iMILITARY_THREAT_MINOR_THRESHOLD;
	int m_iMILITARY_THREAT_STRENGTH_IMMENSE;
	int m_iMILITARY_THREAT_STRENGTH_POWERFUL;
	int m_iMILITARY_THREAT_STRENGTH_STRONG;
	int m_iMILITARY_THREAT_STRENGTH_AVERAGE;
	int m_iMILITARY_THREAT_STRENGTH_POOR;
	int m_iMILITARY_THREAT_STRENGTH_WEAK;
	int m_iMILITARY_THREAT_STRENGTH_PATHETIC;
	int m_iMILITARY_THREAT_WAR_STATE_CALM;
	int m_iMILITARY_THREAT_WAR_STATE_NEARLY_WON;
	int m_iMILITARY_THREAT_WAR_STATE_OFFENSIVE;
	int m_iMILITARY_THREAT_WAR_STATE_STALEMATE;
	int m_iMILITARY_THREAT_WAR_STATE_DEFENSIVE;
	int m_iMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED;
	int m_iMILITARY_THREAT_NEIGHBORS;
	int m_iMILITARY_THREAT_CLOSE;
	int m_iMILITARY_THREAT_FAR;
	int m_iMILITARY_THREAT_DISTANT;
	int m_iMILITARY_THREAT_PER_MINOR_ATTACKED;
	int m_iMILITARY_THREAT_PER_MINOR_CONQUERED;
	int m_iMILITARY_THREAT_PER_MAJOR_ATTACKED;
	int m_iMILITARY_THREAT_PER_MAJOR_CONQUERED;
	int m_iMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER;
	int m_iWARMONGER_THREAT_CRITICAL_THRESHOLD;
	int m_iWARMONGER_THREAT_SEVERE_THRESHOLD;
	int m_iWARMONGER_THREAT_MAJOR_THRESHOLD;
	int m_iWARMONGER_THREAT_MINOR_THRESHOLD;
	int m_iWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT;
	int m_iWARMONGER_THREAT_MINOR_CONQUERED_WEIGHT;
	int m_iWARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT;
	int m_iWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT;
#if defined(MOD_CONFIG_AI_IN_XML)
	GD_INT_DECL(WARMONGER_THREAT_MAJOR_CITY_WEIGHT);
	GD_INT_DECL(WARMONGER_THREAT_MINOR_CITY_WEIGHT);
	GD_INT_DECL(WARMONGER_THREAT_CAPITAL_CITY_PERCENT);
	GD_INT_DECL(WARMONGER_THREAT_KNOWS_ATTACKER_PERCENT);
	GD_INT_DECL(WARMONGER_THREAT_KNOWS_DEFENDER_PERCENT);
	GD_INT_DECL(WARMONGER_THREAT_AGGRIEVED_PERCENT);
	GD_INT_DECL(WARMONGER_THREAT_COOP_WAR_PERCENT);
	GD_INT_DECL(WARMONGER_THREAT_DEF_PACT_ENABLED);
	GD_INT_DECL(WARMONGER_THREAT_CITY_SIZE_ENABLED);
	GD_INT_DECL(WARMONGER_THREAT_MODIFIER_NEGATIVE_MEDIUM);
	GD_INT_DECL(WARMONGER_THREAT_MODIFIER_MEDIUM);
	GD_INT_DECL(WARMONGER_THREAT_MODIFIER_LARGE);
	GD_INT_DECL(WARMONGER_THREAT_MODIFIER_SMALL);
	GD_INT_DECL(WARMONGER_THREAT_MODIFIER_NEGATIVE_SMALL);
	GD_INT_DECL(WARMONGER_THREAT_APPROACH_DECAY_LARGE);
	GD_INT_DECL(WARMONGER_THREAT_APPROACH_DECAY_SMALL);
	GD_INT_DECL(WARMONGER_THREAT_APPROACH_DECAY_MEDIUM);
#endif
	int m_iWARMONGER_THREAT_PER_TURN_DECAY;
	int m_iWARMONGER_THREAT_PERSONALITY_MOD;
	int m_iWARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD;
	int m_iWARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD;
	int m_iWARMONGER_ON_CITY_STATE_MULTIPLIER;
	int m_iLAND_DISPUTE_FIERCE_THRESHOLD;
	int m_iLAND_DISPUTE_STRONG_THRESHOLD;
	int m_iLAND_DISPUTE_WEAK_THRESHOLD;
	int m_iVICTORY_DISPUTE_FIERCE_THRESHOLD;
	int m_iVICTORY_DISPUTE_STRONG_THRESHOLD;
	int m_iVICTORY_DISPUTE_WEAK_THRESHOLD;
	int m_iWONDER_DISPUTE_FIERCE_THRESHOLD;
	int m_iWONDER_DISPUTE_STRONG_THRESHOLD;
	int m_iWONDER_DISPUTE_WEAK_THRESHOLD;
	int m_iMINOR_CIV_DISPUTE_FIERCE_THRESHOLD;
	int m_iMINOR_CIV_DISPUTE_STRONG_THRESHOLD;
	int m_iMINOR_CIV_DISPUTE_WEAK_THRESHOLD;
	int m_iMINOR_CIV_DISPUTE_ALLIES_WEIGHT;
	int m_iMINOR_CIV_DISPUTE_FRIENDS_WEIGHT;
	int m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_NONE;
	int m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_LOW;
	int m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_MEDIUM;
	int m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_HIGH;
	int m_iLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_INCREDIBLE;
	int m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_NONE;
	int m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_LOW;
	int m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_MEDIUM;
	int m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_HIGH;
	int m_iLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_INCREDIBLE;
	int m_iLAND_DISPUTE_DISTANT;
	int m_iLAND_DISPUTE_FAR;
	int m_iLAND_DISPUTE_CLOSE;
	int m_iLAND_DISPUTE_NEIGHBORS;
	int m_iLAND_DISPUTE_CRAMPED_MULTIPLIER;
	int m_iLAND_DISPUTE_NO_EXPANSION_STRATEGY;
	int m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE;
	int m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY;
	int m_iVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_10;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_9;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_8;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_7;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_6;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_5;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_4;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_3;
	int m_iLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_2;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_FIERCE_THRESHOLD;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_STRONG_THRESHOLD;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_WEAK_THRESHOLD;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY;
	int m_iVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE;
	int m_iIC_MEMORY_TURN_EXPIRATION;
	int m_iSTOP_SPYING_MEMORY_TURN_EXPIRATION;
	int m_iMOVE_TROOPS_MEMORY_TURN_EXPIRATION;
	int m_iCOOP_WAR_SOON_COUNTER;
	int m_iCOOP_WAR_LOCKED_TURNS;
	int m_iCOOP_WAR_LOCKED_TURNS_WAR_WEIGHT;
	int m_iDEMAND_TURN_LIMIT_MIN;
	int m_iDEMAND_TURN_LIMIT_RAND;
	int m_iDONT_SETTLE_RAND;
	int m_iDONT_SETTLE_FLAVOR_BASE;
	int m_iDONT_SETTLE_FLAVOR_MULTIPLIER;
	int m_iDONT_SETTLE_FRIENDLY;
	int m_iDONT_SETTLE_STRENGTH_PATHETIC;
	int m_iDONT_SETTLE_STRENGTH_WEAK;
	int m_iDONT_SETTLE_STRENGTH_POOR;
	int m_iDONT_SETTLE_STRENGTH_AVERAGE;
	int m_iDONT_SETTLE_STRENGTH_STRONG;
	int m_iDONT_SETTLE_STRENGTH_POWERFUL;
	int m_iDONT_SETTLE_STRENGTH_IMMENSE;
	int m_iDONT_SETTLE_MOD_MILITARY_POSTURE_NONE;
	int m_iDONT_SETTLE_MOD_MILITARY_POSTURE_LOW;
	int m_iDONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM;
	int m_iDONT_SETTLE_MOD_MILITARY_POSTURE_HIGH;
	int m_iDONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE;
	int m_iDOF_TURN_BUFFER;
	int m_iDOF_THRESHOLD;
	int m_iDENUNCIATION_EXPIRATION_TIME;
	int m_iDOF_EXPIRATION_TIME;
	int m_iEACH_GOLD_VALUE_PERCENT;
	int m_iEACH_GOLD_PER_TURN_VALUE_PERCENT;

	int m_iEXPANSION_BICKER_TIMEOUT;
	int m_iEXPANSION_PROMISE_TURNS_EFFECTIVE;
	int m_iEXPANSION_PROMISE_BROKEN_PER_TURN_DECAY;
	int m_iBROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT;
	int m_iOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX;
	int m_iEXPANSION_PROMISE_IGNORED_PER_TURN_DECAY;
	int m_iIGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT;
	int m_iOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX;

	int m_iBORDER_PROMISE_TURNS_EFFECTIVE;
	int m_iBORDER_PROMISE_BROKEN_PER_TURN_DECAY;
	int m_iBROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT;
	int m_iOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX;
	int m_iBORDER_PROMISE_IGNORED_PER_TURN_DECAY;
	int m_iIGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT;
	int m_iOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX;

	int m_iDECLARED_WAR_ON_FRIEND_PER_TURN_DECAY;
	int m_iDECLARED_WAR_ON_FRIEND_PER_OPINION_WEIGHT;

	int m_iDEAL_VALUE_PER_TURN_DECAY;
	int m_iDEAL_VALUE_PER_OPINION_WEIGHT;
	int m_iCOMMON_FOE_VALUE_PER_TURN_DECAY;
	int m_iCOMMON_FOE_VALUE_PER_OPINION_WEIGHT;
	int m_iWANT_RESEARCH_AGREEMENT_RAND;
	int m_iDEMAND_RAND;
	int m_iCAN_WORK_WATER_FROM_GAME_START;
	int m_iNAVAL_PLOT_BLOCKADE_RANGE;
	int m_iEVENT_MESSAGE_TIME;
	int m_iSTART_YEAR;
	int m_iWEEKS_PER_MONTHS;
	int m_iHIDDEN_START_TURN_OFFSET;
	int m_iRECON_VISIBILITY_RANGE;
	int m_iPLOT_VISIBILITY_RANGE;
	int m_iUNIT_VISIBILITY_RANGE;
	int m_iAIR_UNIT_REBASE_RANGE_MULTIPLIER;
	int m_iMOUNTAIN_SEE_FROM_CHANGE;
	int m_iMOUNTAIN_SEE_THROUGH_CHANGE;
	int m_iHILLS_SEE_FROM_CHANGE;
	int m_iHILLS_SEE_THROUGH_CHANGE;
	int m_iSEAWATER_SEE_FROM_CHANGE;
	int m_iSEAWATER_SEE_THROUGH_CHANGE;
	int m_iMAX_YIELD_STACK;
	int m_iMOVE_DENOMINATOR;
	int m_iSTARTING_DISTANCE_PERCENT;
	int m_iMIN_CIV_STARTING_DISTANCE;
	int m_iMIN_CITY_RANGE;
#if defined(MOD_CONFIG_GAME_IN_XML)
	GD_INT_DECL(CITY_STARTING_RINGS);
#endif
	int m_iOWNERSHIP_SCORE_DURATION_THRESHOLD;
	int m_iNUM_POLICY_BRANCHES_ALLOWED;
	int m_iVICTORY_POINTS_NEEDED_TO_WIN;
	int m_iNUM_VICTORY_POINT_AWARDS;
	int m_iNUM_OR_TECH_PREREQS;
	int m_iNUM_AND_TECH_PREREQS;
	int m_iNUM_UNIT_AND_TECH_PREREQS;
	int m_iNUM_BUILDING_AND_TECH_PREREQS;
	int m_iNUM_BUILDING_RESOURCE_PREREQS;
	int m_iBASE_RESEARCH_RATE;
	int m_iMAX_WORLD_WONDERS_PER_CITY;
	int m_iMAX_TEAM_WONDERS_PER_CITY;
	int m_iMAX_NATIONAL_WONDERS_PER_CITY;
	int m_iMAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC;
	int m_iMAX_BUILDINGS_PER_CITY;
	int m_iINITIAL_CITY_POPULATION;
	int m_iBASE_CITY_AIR_STACKING;
	int m_iCITY_CAPTURE_POPULATION_PERCENT;
	int m_iBASE_CITY_GROWTH_THRESHOLD;
	int m_iFOOD_CONSUMPTION_PER_POPULATION;
	int m_iHAPPINESS_PER_CITY_WITH_STATE_RELIGION;
	int m_iHAPPINESS_PER_NATURAL_WONDER;
	int m_iHAPPINESS_PER_EXTRA_LUXURY;
	int m_iUNHAPPINESS_PER_POPULATION;
	float m_fUNHAPPINESS_PER_OCCUPIED_POPULATION;
	int m_iUNHAPPINESS_PER_CITY;
	int m_iUNHAPPINESS_PER_CAPTURED_CITY;
	int m_iUNHAPPY_GROWTH_PENALTY;
	int m_iVERY_UNHAPPY_GROWTH_PENALTY;
	int m_iVERY_UNHAPPY_CANT_TRAIN_SETTLERS;
	int m_iVERY_UNHAPPY_THRESHOLD;
	int m_iSUPER_UNHAPPY_THRESHOLD;
	int m_iUPRISING_COUNTER_MIN;
	int m_iUPRISING_COUNTER_POSSIBLE;
	int m_iUPRISING_NUM_BASE;
	int m_iUPRISING_NUM_CITY_COUNT;
	int m_iREVOLT_COUNTER_MIN;
	int m_iVERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY;
	int m_iVERY_UNHAPPY_MAX_COMBAT_PENALTY;
	int m_iSTRATEGIC_RESOURCE_EXHAUSTED_PENALTY;
	int m_iVERY_UNHAPPY_PRODUCTION_PENALTY_PER_UNHAPPY;
	int m_iVERY_UNHAPPY_MAX_PRODUCTION_PENALTY;
	int m_iVERY_UNHAPPY_GOLD_PENALTY_PER_UNHAPPY;
	int m_iVERY_UNHAPPY_MAX_GOLD_PENALTY;
	int m_iWLTKD_GROWTH_MULTIPLIER;
	int m_iINDUSTRIAL_ROUTE_PRODUCTION_MOD;
	int m_iRESOURCE_DEMAND_COUNTDOWN_BASE;
	int m_iRESOURCE_DEMAND_COUNTDOWN_CAPITAL_ADD;
	int m_iRESOURCE_DEMAND_COUNTDOWN_RAND;
	int m_iNEW_HURRY_MODIFIER;
	int m_iGREAT_GENERAL_RANGE;
	int m_iGREAT_GENERAL_STRENGTH_MOD;
	int m_iBONUS_PER_ADJACENT_FRIEND;
	int m_iPOLICY_ATTACK_BONUS_MOD;
	int m_iCONSCRIPT_MIN_CITY_POPULATION;
	int m_iCONSCRIPT_POPULATION_PER_COST;
	int m_iMIN_TIMER_UNIT_DOUBLE_MOVES;
	int m_iCOMBAT_DAMAGE;
	int m_iNONCOMBAT_UNIT_RANGED_DAMAGE;
	int m_iNAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER;
	int m_iLAKE_PLOT_RAND;
	int m_iPLOTS_PER_RIVER_EDGE;
	int m_iRIVER_SOURCE_MIN_RIVER_RANGE;
	int m_iRIVER_SOURCE_MIN_SEAWATER_RANGE;
	int m_iLAKE_MAX_AREA_SIZE;
	int m_iINITIAL_GOLD_PER_UNIT_TIMES_100;
	int m_iINITIAL_FREE_OUTSIDE_UNITS;
	int m_iINITIAL_OUTSIDE_UNIT_GOLD_PERCENT;
	int m_iUNIT_MAINTENANCE_GAME_MULTIPLIER;
	int m_iUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR;
	int m_iFREE_UNIT_HAPPINESS;
	int m_iTRADE_ROUTE_BASE_GOLD;
#if defined(MOD_TRADE_ROUTE_SCALING)
	GD_INT_DECL(TRADE_ROUTE_BASE_TARGET_TURNS);
	GD_INT_DECL(TRADE_ROUTE_BASE_LAND_DISTANCE);
	GD_INT_DECL(TRADE_ROUTE_BASE_LAND_MODIFIER);
	GD_INT_DECL(TRADE_ROUTE_BASE_SEA_DISTANCE);
	GD_INT_DECL(TRADE_ROUTE_BASE_SEA_MODIFIER);
	GD_INT_DECL(TRADE_ROUTE_BASE_FOOD_VALUE);
	GD_INT_DECL(TRADE_ROUTE_BASE_PRODUCTION_VALUE);
	GD_INT_DECL(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100);
	GD_INT_DECL(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100);
	GD_INT_DECL(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE);
	GD_INT_DECL(TRADE_ROUTE_RIVER_CITY_MODIFIER);
	GD_INT_DECL(TRADE_ROUTE_BASE_PLUNDER_GOLD);
	GD_INT_DECL(TRADE_ROUTE_PLUNDER_TURNS_COUNTER);
	GD_INT_DECL(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA);
	GD_INT_DECL(TRADE_ROUTE_CS_FRIEND_SCIENCE_DELTA);
	GD_INT_DECL(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA);
	GD_INT_DECL(TRADE_ROUTE_CS_FRIEND_CULTURE_DELTA);
	GD_INT_DECL(TRADE_ROUTE_CS_ALLY_GOLD);
	GD_INT_DECL(TRADE_ROUTE_CS_FRIEND_GOLD);
#endif
#if defined(MOD_CIV6_ROADS)
	GD_INT_DECL(TRADE_ROUTE_CREATE_RAILROADS_ERA);
	GD_INT_DECL(TRADE_ROUTE_CREATE_RAILROADS_TECH_ID);
#endif
#if defined(MOD_CITY_STATE_SCALE)
	GD_INT_DECL(CITY_STATE_SCALE_PER_CITY_MOD);
#endif
	int m_iTRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER;
	int m_iTRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER;
	int m_iDEFICIT_UNIT_DISBANDING_THRESHOLD;
	int m_iGOLDEN_AGE_BASE_THRESHOLD_HAPPINESS;
	float m_fGOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER;
	int m_iGOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS;
	int m_iGOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR;
	int m_iBASE_GOLDEN_AGE_UNITS;
	int m_iGOLDEN_AGE_UNITS_MULTIPLIER;
	int m_iGOLDEN_AGE_LENGTH;
	int m_iGOLDEN_AGE_GREAT_PEOPLE_MODIFIER;
	int m_iMIN_UNIT_GOLDEN_AGE_TURNS;
	int m_iGREAT_WORK_CULTURE_MULTIPLIER;
	int m_iTHEME_GREAT_WORK_GA_MULTIPLIER;
	int m_iGOLDEN_AGE_CULTURE_MODIFIER;
	int m_iHILLS_EXTRA_MOVEMENT;
	int m_iRIVER_EXTRA_MOVEMENT;
	int m_iFEATURE_GROWTH_MODIFIER;
	int m_iROUTE_FEATURE_GROWTH_MODIFIER;
	int m_iEXTRA_YIELD;
	int m_iFORTIFY_MODIFIER_PER_TURN;
	int m_iMAX_FORTIFY_TURNS;
	int m_iNUKE_FALLOUT_PROB;
	int m_iNUKE_UNIT_DAMAGE_BASE;
	int m_iNUKE_UNIT_DAMAGE_RAND_1;
	int m_iNUKE_UNIT_DAMAGE_RAND_2;
	int m_iNUKE_NON_COMBAT_DEATH_THRESHOLD;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_BASE;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_1;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_2;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_BASE;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_1;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_2;
	int m_iNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD;
	int m_iNUKE_CITY_HIT_POINT_DAMAGE;
	int m_iNUKE_BLAST_RADIUS;
	int m_iTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER;
	int m_iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER;
	int m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
	int m_iPEACE_TREATY_LENGTH;
	int m_iCOOP_WAR_LOCKED_LENGTH;
	int m_iBASE_FEATURE_PRODUCTION_PERCENT;
	int m_iFEATURE_PRODUCTION_PERCENT_MULTIPLIER;
	int m_iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT;
	int m_iDEFAULT_WAR_VALUE_FOR_UNIT;
	int m_iUNIT_PRODUCTION_PERCENT;
	int m_iMAX_UNIT_SUPPLY_PRODMOD;
	int m_iBUILDING_PRODUCTION_PERCENT;
	int m_iPROJECT_PRODUCTION_PERCENT;
	int m_iMAXED_UNIT_GOLD_PERCENT;
	int m_iMAXED_BUILDING_GOLD_PERCENT;
	int m_iMAXED_PROJECT_GOLD_PERCENT;
	int m_iMAX_CITY_DEFENSE_DAMAGE;
	int m_iCIRCUMNAVIGATE_FREE_MOVES;
	int m_iBASE_CAPTURE_GOLD;
	int m_iCAPTURE_GOLD_PER_POPULATION;
	int m_iCAPTURE_GOLD_RAND1;
	int m_iCAPTURE_GOLD_RAND2;
	int m_iCAPTURE_GOLD_MAX_TURNS;
	int m_iBARBARIAN_CITY_ATTACK_MODIFIER;
	int m_iBUILDING_PRODUCTION_DECAY_TIME;
	int m_iBUILDING_PRODUCTION_DECAY_PERCENT;
	int m_iUNIT_PRODUCTION_DECAY_TIME;
	int m_iUNIT_PRODUCTION_DECAY_PERCENT;
	int m_iBASE_UNIT_UPGRADE_COST;
	int m_iUNIT_UPGRADE_COST_PER_PRODUCTION;
	int m_iUNIT_UPGRADE_COST_VISIBLE_DIVISOR;
	int m_iUNIT_UPGRADE_COST_DISCOUNT_MAX;
	int m_iWAR_SUCCESS_UNIT_CAPTURING;
	int m_iWAR_SUCCESS_CITY_CAPTURING;
	int m_iDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL;
	int m_iTEAM_VOTE_MIN_CANDIDATES;
	int m_iRESEARCH_AGREEMENT_TIMER;
	int m_iRESEARCH_AGREEMENT_BOOST_DIVISOR;
	int m_iSCORE_POPULATION_FACTOR;
	int m_iSCORE_LAND_FACTOR;
	int m_iSCORE_WONDER_FACTOR;
	int m_iSCORE_TECH_FACTOR;
	int m_iSCORE_FREE_PERCENT;
	int m_iSCORE_VICTORY_PERCENT;
	int m_iSCORE_HANDICAP_PERCENT_OFFSET;
	int m_iSCORE_HANDICAP_PERCENT_PER;
	int m_iMINIMAP_RENDER_SIZE;
	int m_iMAX_INTERCEPTION_PROBABILITY;
	int m_iMAX_EVASION_PROBABILITY;
	int m_iPLAYER_ALWAYS_RAZES_CITIES;
	int m_iMIN_WATER_SIZE_FOR_OCEAN;
	int m_iCITY_SCREEN_CLICK_WILL_EXIT;
	int m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
	int m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
	int m_iCITY_MAX_NUM_BUILDINGS;
	int m_iCITY_MIN_SIZE_FOR_SETTLERS;
	int m_iADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES;
	int m_iADVANCED_START_MAX_UNITS_PER_CITY;
	int m_iADVANCED_START_CITY_COST;
	int m_iADVANCED_START_CITY_COST_INCREASE;
	int m_iADVANCED_START_POPULATION_COST;
	int m_iADVANCED_START_POPULATION_COST_INCREASE;
	int m_iADVANCED_START_VISIBILITY_COST;
	int m_iADVANCED_START_VISIBILITY_COST_INCREASE;
	int m_iADVANCED_START_CITY_PLACEMENT_MAX_RANGE;
	int m_iNEW_CITY_BUILDING_VALUE_MODIFIER;
	int m_iPATH_DAMAGE_WEIGHT;
	int m_iPUPPET_SCIENCE_MODIFIER;
	int m_iPUPPET_CULTURE_MODIFIER;
	int m_iPUPPET_GOLD_MODIFIER;
	int m_iPUPPET_FAITH_MODIFIER;
	int m_iPUPPET_GROWTH_MODIFIER;
	int m_iPUPPET_PRODUCTION_MODIFIER;
	int m_iBLOCKADE_GOLD_PENALTY;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	int m_iPUPPET_TOURISM_MODIFIER;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
	int m_iPUPPET_GOLDEN_AGE_MODIFIER;
#endif
	int m_iBASE_POLICY_COST;
	int m_iPOLICY_COST_INCREASE_TO_BE_EXPONENTED;
	int m_iPOLICY_COST_VISIBLE_DIVISOR;
	int m_iSWITCH_POLICY_BRANCHES_ANARCHY_TURNS;
	int m_iSWITCH_POLICY_BRANCHES_TENETS_LOST;
	int m_iPOLICY_COST_DISCOUNT_MAX;
	int m_iGOLD_PURCHASE_GOLD_PER_PRODUCTION;
	int m_iGOLD_PURCHASE_VISIBLE_DIVISOR;
	int m_iPROJECT_PURCHASING_DISABLED;
	int m_iINFLUENCE_MOUNTAIN_COST;
	int m_iINFLUENCE_HILL_COST;
	int m_iINFLUENCE_RIVER_COST;
	int m_iUSE_FIRST_RING_INFLUENCE_TERRAIN_COST;
	int m_iNUM_RESOURCE_QUANTITY_TYPES;
	int m_iSPECIALISTS_DIVERT_POPULATION_ENABLED;
	int m_iSCIENCE_PER_POPULATION;
	int m_iRESEARCH_AGREEMENT_MOD;
	int m_iBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD;
	int m_iBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING;
	int m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL;
	int m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP;
	int m_iBARBARIAN_CAMP_COASTAL_SPAWN_ROLL;
	int m_iBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE;
	int m_iBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN;
	int m_iMAX_BARBARIANS_FROM_CAMP_NEARBY;
	int m_iMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE;
	int m_iGOLD_FROM_BARBARIAN_CONVERSION;
	int m_iBARBARIAN_CITY_GOLD_RANSOM;
	int m_iBARBARIAN_UNIT_GOLD_RANSOM;
	int m_iEMBARKED_UNIT_MOVEMENT;
	int m_iEMBARKED_VISIBILITY_RANGE;
	int m_iDEFAULT_MAX_NUM_BUILDERS;
	int m_iBARBARIAN_TECH_PERCENT;
	int m_iCITY_RESOURCE_WLTKD_TURNS;
	int m_iMAX_SPECIALISTS_FROM_BUILDING;
	int m_iGREAT_PERSON_THRESHOLD_BASE;
	int m_iGREAT_PERSON_THRESHOLD_INCREASE;
	int m_iCULTURE_BOMB_COOLDOWN;
	int m_iCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE;
	int m_iLANDMARK_MINOR_FRIENDSHIP_CHANGE;
	int m_iUNIT_AUTO_EXPLORE_DISABLED;
	int m_iUNIT_AUTO_EXPLORE_FULL_DISABLED;
	int m_iUNIT_WORKER_AUTOMATION_DISABLED;
	int m_iUNIT_DELETE_DISABLED;
	int m_iMIN_START_AREA_TILES;
	int m_iMIN_DISTANCE_OTHER_AREA_PERCENT;
	int m_iMINOR_CIV_FOOD_REQUIREMENT;
	int m_iMAJOR_CIV_FOOD_REQUIREMENT;
	int m_iMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST;
	int m_iSTART_AREA_FOOD_MULTIPLIER;
	int m_iSTART_AREA_HAPPINESS_MULTIPLIER;
	int m_iSTART_AREA_PRODUCTION_MULTIPLIER;
	int m_iSTART_AREA_GOLD_MULTIPLIER;
	int m_iSTART_AREA_SCIENCE_MULTIPLIER;
	int m_iSTART_AREA_FAITH_MULTIPLIER;
	int m_iSTART_AREA_RESOURCE_MULTIPLIER;
	int m_iSTART_AREA_STRATEGIC_MULTIPLIER;
	int m_iSTART_AREA_BUILD_ON_COAST_PERCENT;
	int m_iSETTLER_FOOD_MULTIPLIER;
	int m_iSETTLER_HAPPINESS_MULTIPLIER;
	int m_iSETTLER_PRODUCTION_MULTIPLIER;
	int m_iSETTLER_GOLD_MULTIPLIER;
	int m_iSETTLER_SCIENCE_MULTIPLIER;
	int m_iSETTLER_FAITH_MULTIPLIER;
	int m_iSETTLER_RESOURCE_MULTIPLIER;
	int m_iSETTLER_STRATEGIC_MULTIPLIER;
	int m_iSETTLER_BUILD_ON_COAST_PERCENT;
	int m_iCITY_RING_1_MULTIPLIER;
	int m_iCITY_RING_2_MULTIPLIER;
	int m_iCITY_RING_3_MULTIPLIER;
	int m_iCITY_RING_4_MULTIPLIER;
	int m_iCITY_RING_5_MULTIPLIER;
	int m_iSETTLER_EVALUATION_DISTANCE;
	int m_iSETTLER_DISTANCE_DROPOFF_MODIFIER;
	int m_iBUILD_ON_RESOURCE_PERCENT;
	int m_iBUILD_ON_RIVER_PERCENT;
	int m_iCHOKEPOINT_STRATEGIC_VALUE;
	int m_iHILL_STRATEGIC_VALUE;
	int m_iALREADY_OWNED_STRATEGIC_VALUE;
	int m_iMINOR_CIV_CONTACT_GOLD_FIRST;
	int m_iMINOR_CIV_CONTACT_GOLD_OTHER;
#if defined(MOD_GLOBAL_CS_GIFTS)
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_FRIENDSHIP);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_CULTURE);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_FAITH);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_GOLD);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_FOOD);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_BONUS_UNIT);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_XP_PER_ERA);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_XP_RANDOM);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_PLAYER_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_PLAYER_DIVISOR);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_DIVISOR);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_DIVISOR);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_DIVISOR);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_DIVISOR);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_MULTIPLIER);
	GD_INT_DECL(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_DIVISOR);
#endif
	int m_iMINOR_CIV_GROWTH_PERCENT;
	int m_iMINOR_CIV_PRODUCTION_PERCENT;
	int m_iMINOR_CIV_GOLD_PERCENT;
	int m_iMINOR_CIV_TECH_PERCENT;
	int m_iMINOR_POLICY_RESOURCE_MULTIPLIER;
	int m_iMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER;
	int m_iMINOR_GOLD_GIFT_LARGE;
	int m_iMINOR_GOLD_GIFT_MEDIUM;
	int m_iMINOR_GOLD_GIFT_SMALL;
	int m_iMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST;
	int m_iMINOR_CIV_BUYOUT_COST;
	int m_iMINOR_CIV_BUYOUT_TURNS;
	int m_iMINOR_FRIENDSHIP_FROM_TRADE_MISSION;
	int m_iMINOR_FRIENDSHIP_ANCHOR_DEFAULT;
	int m_iMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED;
	int m_iMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF;
	int m_iMINOR_UNIT_GIFT_TRAVEL_TURNS;
	int m_iPLOT_UNIT_LIMIT;
#if defined(MOD_GLOBAL_STACKING_RULES)
	GD_INT_DECL(CITY_UNIT_LIMIT);
#endif
	int m_iZONE_OF_CONTROL_ENABLED;
	int m_iFIRE_SUPPORT_DISABLED;
	int m_iMAX_HIT_POINTS;
	int m_iMAX_CITY_HIT_POINTS;
	int m_iCITY_HIT_POINTS_HEALED_PER_TURN;
	int m_iFLAT_LAND_EXTRA_DEFENSE;
	int m_iHILLS_EXTRA_DEFENSE;
	int m_iRIVER_ATTACK_MODIFIER;
	int m_iAMPHIB_ATTACK_MODIFIER;
	int m_iENEMY_HEAL_RATE;
	int m_iNEUTRAL_HEAL_RATE;
	int m_iFRIENDLY_HEAL_RATE;
	int m_iINSTA_HEAL_RATE;
	int m_iCITY_HEAL_RATE;
	int m_iATTACK_SAME_STRENGTH_MIN_DAMAGE;
	int m_iRANGE_ATTACK_RANGED_DEFENDER_MOD;
	int m_iATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE;
	int m_iRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE;
	int m_iAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE;
	int m_iINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE;
	int m_iINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iAIR_SWEEP_INTERCEPTION_DAMAGE_MOD;
	int m_iWOUNDED_DAMAGE_MULTIPLIER;
	int m_iTRAIT_WOUNDED_DAMAGE_MOD;
	int m_iCITY_STRENGTH_DEFAULT;
	int m_iCITY_STRENGTH_POPULATION_CHANGE;
	int m_iCITY_STRENGTH_UNIT_DIVISOR;
	int m_iCITY_STRENGTH_HILL_CHANGE;
	int m_iCITY_ATTACKING_DAMAGE_MOD;
	int m_iATTACKING_CITY_MELEE_DAMAGE_MOD;
#if defined(MOD_EVENTS_CITY_BOMBARD)
	GD_INT_DECL(MAX_CITY_ATTACK_RANGE);
#endif
	int m_iCITY_ATTACK_RANGE;
	int m_iCAN_CITY_USE_INDIRECT_FIRE;
	int m_iCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER;
	int m_iMIN_CITY_STRIKE_DAMAGE;
	int m_iCITY_CAPTURE_DAMAGE_PERCENT;
	int m_iEXPERIENCE_PER_LEVEL;
	int m_iEXPERIENCE_ATTACKING_UNIT_MELEE;
	int m_iEXPERIENCE_DEFENDING_UNIT_MELEE;
	int m_iEXPERIENCE_ATTACKING_UNIT_AIR;
	int m_iEXPERIENCE_DEFENDING_UNIT_AIR;
	int m_iEXPERIENCE_ATTACKING_UNIT_RANGED;
	int m_iEXPERIENCE_DEFENDING_UNIT_RANGED;
	int m_iEXPERIENCE_ATTACKING_AIR_SWEEP;
	int m_iEXPERIENCE_DEFENDING_AIR_SWEEP_AIR;
	int m_iEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND;
	int m_iEXPERIENCE_ATTACKING_CITY_MELEE;
	int m_iEXPERIENCE_ATTACKING_CITY_RANGED;
	int m_iEXPERIENCE_ATTACKING_CITY_AIR;
	int m_iBARBARIAN_MAX_XP_VALUE;
	int m_iMINOR_MAX_XP_VALUE;
	int m_iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT;
	int m_iGREAT_GENERALS_THRESHOLD_INCREASE;
	int m_iGREAT_GENERALS_THRESHOLD_INCREASE_TEAM;
	int m_iGREAT_GENERALS_THRESHOLD;
	int m_iUNIT_DEATH_XP_GREAT_GENERAL_LOSS;
	int m_iMIN_EXPERIENCE_PER_COMBAT;
	int m_iMAX_EXPERIENCE_PER_COMBAT;
	int m_iCRAMPED_RANGE_FROM_CITY;
	int m_iCRAMPED_USABLE_PLOT_PERCENT;
	int m_iPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT;
	int m_iPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY;
	int m_iPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER;
	int m_iPROXIMITY_CLOSE_DISTANCE_MAX;
	int m_iPROXIMITY_CLOSE_DISTANCE_MIN;
	int m_iPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER;
	int m_iPROXIMITY_FAR_DISTANCE_MAX;
	int m_iPROXIMITY_FAR_DISTANCE_MIN;
	int m_iPLOT_BASE_COST;
	int m_iPLOT_ADDITIONAL_COST_PER_PLOT;
	int m_iPLOT_COST_APPEARANCE_DIVISOR;
	int m_iCULTURE_COST_FIRST_PLOT;
	int m_iCULTURE_COST_LATER_PLOT_MULTIPLIER;
	int m_iCULTURE_COST_VISIBLE_DIVISOR;
	int m_iCULTURE_PLOT_COST_MOD_MINIMUM;
	int m_iMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER;
	int m_iMAXIMUM_WORK_PLOT_DISTANCE;
	int m_iMAXIMUM_BUY_PLOT_DISTANCE;
	int m_iMAXIMUM_ACQUIRE_PLOT_DISTANCE;
	int m_iPLOT_INFLUENCE_BASE_MULTIPLIER;
	int m_iPLOT_INFLUENCE_DISTANCE_MULTIPLIER;
	int m_iPLOT_INFLUENCE_DISTANCE_DIVISOR;
	int m_iPLOT_INFLUENCE_RING_COST;
	int m_iPLOT_INFLUENCE_WATER_COST;
	int m_iPLOT_INFLUENCE_IMPROVEMENT_COST;
	int m_iPLOT_INFLUENCE_ROUTE_COST;
	int m_iPLOT_INFLUENCE_RESOURCE_COST;
	int m_iPLOT_INFLUENCE_NW_COST;
#if defined(MOD_UI_CITY_EXPANSION)
	GD_INT_DECL(PLOT_INFLUENCE_COST_VISIBLE_DIVISOR);
#endif
	int m_iPLOT_BUY_RESOURCE_COST;
	int m_iPLOT_BUY_YIELD_COST;
	int m_iPLOT_INFLUENCE_YIELD_POINT_COST;
	int m_iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST;
	int m_iUNITED_NATIONS_COUNTDOWN_TURNS;
	int m_iOWN_UNITED_NATIONS_VOTE_BONUS;
	int m_iDIPLO_VICTORY_ALGORITHM_THRESHOLD;
	int m_iDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER;
	int m_iDIPLO_VICTORY_DEFAULT_VOTE_PERCENT;
	int m_iSCORE_CITY_MULTIPLIER;
	int m_iSCORE_POPULATION_MULTIPLIER;
	int m_iSCORE_LAND_MULTIPLIER;
	int m_iSCORE_WONDER_MULTIPLIER;
	int m_iSCORE_TECH_MULTIPLIER;
	int m_iSCORE_FUTURE_TECH_MULTIPLIER;
	int m_iSCORE_POLICY_MULTIPLIER;
	int m_iSCORE_GREAT_WORK_MULTIPLIER;
	int m_iSCORE_BELIEF_MULTIPLIER;
	int m_iSCORE_RELIGION_CITIES_MULTIPLIER;
	int m_iVICTORY_POINTS_PER_ERA;
	int m_iMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY;
	int m_iZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS;
	int m_iZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS;
	int m_iZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS;
	int m_iMAX_CITY_DIST_HIGHWATER_MARK;
	int m_iHEAVY_RESOURCE_THRESHOLD;
	int m_iPROGRESS_POPUP_TURN_FREQUENCY;
	int m_iSETTLER_PRODUCTION_SPEED;
	int m_iBUY_PLOTS_DISABLED;
	int m_iCITY_STRENGTH_TECH_MULTIPLIER;
	int m_iWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT;
	int m_iBUILDING_SALE_DIVISOR;
	int m_iDISBAND_UNIT_REFUND_PERCENT;
	int m_iMINOR_CIV_ROUTE_QUEST_WEIGHT;
	int m_iWITHDRAW_MOD_ENEMY_MOVES;
	int m_iWITHDRAW_MOD_BLOCKED_TILE;
	int m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT;
	int m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY;
	int m_iRELIGION_MIN_FAITH_FIRST_PANTHEON;
	int m_iRELIGION_MIN_FAITH_FIRST_PROPHET;
	int m_iRELIGION_MIN_FAITH_FIRST_GREAT_PERSON;
	int m_iRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON;
	int m_iRELIGION_FAITH_DELTA_NEXT_PROPHET;
	int m_iRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON;
	int m_iRELIGION_BASE_CHANCE_PROPHET_SPAWN;
	int m_iRELIGION_ATHEISM_PRESSURE_PER_POP;
	int m_iRELIGION_INITIAL_FOUNDING_CITY_PRESSURE;
	int m_iRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE;
	int m_iRELIGION_MISSIONARY_PRESSURE_MULTIPLIER;
	int m_iRELIGION_ADJACENT_CITY_DISTANCE;
	int m_iRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY;
	int m_iRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY;
	int m_iRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY;
	int m_iRELIGION_DIPLO_HIT_THRESHOLD;
	int m_iESPIONAGE_GATHERING_INTEL_COST_PERCENT;
	int m_iESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT;
	int m_iESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT;
	int m_iESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS;
	int m_iESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION;
	int m_iESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION;
	int m_iESPIONAGE_SURVEILLANCE_SIGHT_RANGE;
	int m_iESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP;
	int m_iINTERNATIONAL_TRADE_BASE;
	int m_iINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION;
	int m_iINTERNATIONAL_TRADE_CITY_GPT_DIVISOR;
	int m_iCOMBAT_CAPTURE_HEALTH;
	int m_iCOMBAT_CAPTURE_MIN_CHANCE;
	int m_iCOMBAT_CAPTURE_MAX_CHANCE;
	int m_iCOMBAT_CAPTURE_RATIO_MULTIPLIER;
#if defined(MOD_DIPLOMACY_CITYSTATES_HURRY) || defined(MOD_BALANCE_CORE)
	int m_iAI_GOLD_TREASURY_BUFFER;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS) || defined(MOD_BALANCE_CORE)
	int m_iBALANCE_MINOR_ANCHOR_ATTACK;
	int m_iBALANCE_CS_WAR_COOLDOWN_RATE;
	int m_iBALANCE_CS_FORGIVENESS_CHANCE;
#endif
#if defined(MOD_BALANCE_CORE)
	int m_iMOD_BALANCE_FOLLOWER_GROWTH_BONUS;
	int m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iFRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iFRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iBALANCE_MAX_CS_ALLY_STRENGTH;
	int m_iBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS;
	int m_iBALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX;
	int m_iBALANCE_CS_ALLIANCE_DEFENSE_BONUS;
	int m_iUNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED;
	int m_iBALANCE_CORE_ARABIA_TILE_BONUS;
	int m_iBALANCE_MARRIAGE_GP_RATE;
	int m_iBALANCE_SPY_RESPAWN_TIMER;
	int m_iBALANCE_SPY_SABOTAGE_RATE;
	int m_iBALANCE_SPY_TO_MINOR_RATIO;
	int m_iBALANCE_HAPPINESS_EMPIRE_MOD;
	int m_iBALANCE_HAPPINESS_THRESHOLD;
	int m_iBALANCE_HAPPINESS_BONUS_MAXIMUM;
	int m_iBALANCE_HAPPINESS_BONUS_MINIMUM;
	int m_iBALANCE_HAPPINESS_PENALTY_MAXIMUM;
	int m_iBALANCE_HAPPINESS_PENALTY_MINIMUM;
	int m_iBALANCE_HAPPINESS_SCIENCE_MODIFIER;
	int m_iBALANCE_HAPPINESS_GOLD_MODIFIER;
	int m_iBALANCE_HAPPINESS_PRODUCTION_MODIFIER;
	int m_iBALANCE_HAPPINESS_FOOD_MODIFIER;
	int m_iBALANCE_HAPPINESS_FAITH_MODIFIER;
	int m_iBALANCE_HAPPINESS_CULTURE_MODIFIER;
	int m_iBALANCE_HAPPINESS_THRESHOLD_MAIN;
	int m_iBALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE;
	int m_iBALANCE_COAST_STRATEGIC_VALUE;
	int m_iBALANCE_FRESH_WATER_STRATEGIC_VALUE;
	int m_iBALANCE_CHOKEPOINT_STRATEGIC_VALUE;
	int m_iBALANCE_BAD_TILES_STRATEGIC_VALUE;
	int m_iBALANCE_BARBARIAN_HEAL_RATE;
	int m_iBALANCE_BASIC_ATTACK_ARMY_SIZE;
	int m_iBALANCE_NAVY_START_SIZE;
	int m_iBALANCE_UNHAPPINESS_PER_SPECIALIST;
	int m_iBALANCE_HAPPINESS_LUXURY_BASE;
	int m_iBALANCE_HAPPINESS_LUXURY_SCALER;
	int m_iBALANCE_HAPPINESS_POPULATION_DIVISOR;
	float m_fBALANCE_UNHAPPINESS_PER_MINORITY_POP;
	float m_fBALANCE_UNHAPPINESS_FROM_STARVING_PER_POP;
	float m_fBALANCE_UNHAPPINESS_PER_PILLAGED;
	float m_fBALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP;
	int m_iBALANCE_UNHAPPINESS_PER_GOLD_THRESHOLD;
	float m_fBALANCE_UNHAPPINESS_PER_GOLD_RATE;
	int m_iBALANCE_UNHAPPINESS_PER_DEFENSE_THRESHOLD;
	float m_fBALANCE_UNHAPPINESS_PER_DEFENSE_RATE;
	int m_iBALANCE_UNHAPPINESS_PER_CULTURE_THRESHOLD;
	float m_fBALANCE_UNHAPPINESS_PER_CULTURE_RATE;
	int m_iBALANCE_HAPPINESS_ERA_BASE_INCREASE;
	int m_iBALANCE_HAPPINESS_POP_MULTIPLIER;
	int m_iBALANCE_UNHAPPY_CITY_BASE_VALUE;
	int m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM;
	int m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY;
	int m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER;
	int m_iBALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY;
	int m_iBALANCE_HAPPINESS_ERA_MAX_INCREASE;
	int m_iBALANCE_TECH_RATE_SCIENCE;
	int m_iBALANCE_TECH_RATE_CULTURE;
	int m_iBALANCE_TECH_RATE_DEFENSE;
	int m_iBALANCE_TECH_RATE_GOLD;
	int m_iBALANCE_WONDER_BEATEN_CONSOLATION_PRIZE;
	int m_iBALANCE_CULTURE_PERCENTAGE_VALUE;
	int m_iBALANCE_GA_PERCENTAGE_VALUE;
	int m_iBALANCE_SCIENCE_PERCENTAGE_VALUE;
	int m_iBALANCE_FAITH_PERCENTAGE_VALUE;
	int m_iBALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD;
	int m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC;
	int m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR;
	int m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR;
	int m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL;
	int m_iBALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT;
	int m_iBALANCE_CONQUEST_REDUCTION_BOOST;
	int m_iBALANCE_SPY_BOOST_INFLUENCE_EXOTIC;
	int m_iBALANCE_SPY_BOOST_INFLUENCE_FAMILIAR;
	int m_iBALANCE_SPY_BOOST_INFLUENCE_POPULAR;
	int m_iBALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL;
	int m_iBALANCE_SPY_BOOST_INFLUENCE_DOMINANT;
	int m_iBALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR;
	int m_iBALANCE_GOLD_INFLUENCE_LEVEL_POPULAR;
	int m_iBALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL;
	int m_iBALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT;
	int m_iBALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC;
	int m_iBALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR;
	int m_iBALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR;
	int m_iBALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL;
	int m_iBALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT;
	int m_iBALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC;
	int m_iBALANCE_EMBARK_DEFENSE_DIVISOR;
	int m_iBALANCE_NAVAL_DEFENSE_CITY_STRIKE_MODIFIER;
	int m_iSETTLER_MOVE_ON_START;
	int m_iBALANCE_HAPPINESS_CAPITAL_MODIFIER;
	float m_fBALANCE_HAPPINESS_TECH_BASE_MODIFIER;
	int m_iBALANCE_CITY_PURCHASE_MOD;
	int m_iBALANCE_INFLUENCE_BOOST_PATRONAGE_POLICY;
	int m_iBALANCE_INFLUENCE_BOOST_PROTECTION_MINOR;
	int m_iBALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY;
	int m_iBALANCE_MOD_POLICIES_NEEDED_IDEOLOGY;
	int m_iBUILDER_TASKING_BASELINE_ADDS_FOOD;
	int m_iBUILDER_TASKING_BASELINE_ADDS_GOLD;
	int m_iBUILDER_TASKING_BASELINE_ADDS_FAITH;
	int m_iBUILDER_TASKING_BASELINE_ADDS_PRODUCTION;
	int m_iBUILDER_TASKING_BASELINE_ADDS_SCIENCE;
	int m_iBALANCE_MINOR_PROTECTION_MINIMUM_DURATION;
	int m_iBALANCE_SCOUT_XP_RANDOM_VALUE;
	int m_iBALANCE_SCOUT_XP_MAXIMUM;
	int m_iBALANCE_SCOUT_XP_NATURAL_WONDER;
	int m_iBALANCE_SCOUT_XP_BASE;
	int m_iBALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER;
	int m_iBALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER;
	int m_iBALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER;
	int m_iOPINION_WEIGHT_DP_WITH_ENEMY;
	int m_iOPINION_WEIGHT_DP_WITH_FRIEND;
	int m_iOPINION_WEIGHT_DP;
	int m_iOPINION_WEIGHT_OPEN_BORDERS_MUTUAL;
	int m_iOPINION_WEIGHT_OPEN_BORDERS_US;
	int m_iOPINION_WEIGHT_OPEN_BORDERS_THEM;
	int m_iGWAM_THRESHOLD_DECREASE;
	int m_iBALANCE_BUILDING_INVESTMENT_BASELINE;
	int m_iBALANCE_UNIT_INVESTMENT_BASELINE;
	int m_iOPEN_BORDERS_MODIFIER_TRADE_GOLD;
	float m_fMOD_BALANCE_CORE_MINIMUM_RANKING_PTP;
	int m_iMOD_BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT;
#endif
	// -- floats --

	float m_fDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT;
	float m_fDIPLO_VICTORY_CIV_DELEGATES_CONSTANT;
	float m_fDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT;
	float m_fDIPLO_VICTORY_CS_DELEGATES_CONSTANT;
	float m_fAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT;
	float m_fAI_PRODUCTION_WEIGHT_BASE_MOD;
	float m_fAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT;
	float m_fAI_RESEARCH_WEIGHT_BASE_MOD;
	float m_fHURRY_GOLD_PRODUCTION_EXPONENT;
	float m_fCITY_ZOOM_LEVEL_1;
	float m_fCITY_ZOOM_LEVEL_2;
	float m_fCITY_ZOOM_LEVEL_3;
	float m_fCITY_ZOOM_OFFSET;
	float m_fCULTURE_COST_LATER_PLOT_EXPONENT;
	float m_fPOST_COMBAT_TEXT_DELAY;
	float m_fAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER;
	float m_fAI_DANGER_MAJOR_APPROACH_WAR;
	float m_fAI_DANGER_MAJOR_APPROACH_HOSTILE;
	float m_fAI_DANGER_MAJOR_APPROACH_DECEPTIVE;
	float m_fAI_DANGER_MAJOR_APPROACH_GUARDED;
	float m_fAI_DANGER_MAJOR_APPROACH_AFRAID;
	float m_fAI_DANGER_MAJOR_APPROACH_FRIENDLY;
	float m_fAI_DANGER_MAJOR_APPROACH_NEUTRAL;
	float m_fAI_DANGER_MINOR_APPROACH_NEUTRAL;
	float m_fAI_DANGER_MINOR_APPROACH_FRIENDLY;
	float m_fAI_DANGER_MINOR_APPROACH_BULLY;
	float m_fAI_DANGER_MINOR_APPROACH_CONQUEST;
	float m_fFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER;
	float m_fDIPLO_VICTORY_TEAM_MULTIPLIER;
	float m_fAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION;
	float m_fAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY;
	float m_fAI_STRATEGY_NAVAL_UNITS_PER_CITY;
	float m_fGOLD_GIFT_FRIENDSHIP_EXPONENT;
	float m_fGOLD_GIFT_FRIENDSHIP_DIVISOR;
	float m_fHURRY_GOLD_TECH_EXPONENT;
	float m_fHURRY_GOLD_CULTURE_EXPONENT;
	float m_fCITY_GROWTH_MULTIPLIER;
	float m_fCITY_GROWTH_EXPONENT;
	float m_fPOLICY_COST_EXPONENT;
	float m_fPOLICY_COST_EXTRA_VALUE;
	float m_fUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA;
	float m_fUNIT_UPGRADE_COST_EXPONENT;
	float m_fCITY_STRENGTH_TECH_BASE;
	float m_fCITY_STRENGTH_TECH_EXPONENT;
	float m_fESPIONAGE_COUP_NOBODY_BONUS;
	float m_fESPIONAGE_COUP_MULTIPLY_CONSTANT;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR;
	int   m_iPILLAGE_HEAL_AMOUNT;
	int   m_iCITY_CONNECTIONS_CONNECT_TO_CAPITAL;
	float m_fNORMAL_ANNEX;
	float m_fAGGRESSIVE_ANNEX;
	float m_fUNRESEARCHED_TECH_FROM_KILLS_SLOPE;
	float m_fUNRESEARCHED_TECH_FROM_KILLS_INTERCEPT;
	int   m_iBASE_CULTURE_PER_GREAT_WORK;
	int   m_iBASE_TOURISM_PER_GREAT_WORK;
	int   m_iTOURISM_MODIFIER_SHARED_RELIGION;
	int   m_iTOURISM_MODIFIER_TRADE_ROUTE;
	int   m_iTOURISM_MODIFIER_OPEN_BORDERS;
	int   m_iTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES;
	int   m_iTOURISM_MODIFIER_DIPLOMAT;
	int   m_iMINIUMUM_TOURISM_BLAST_STRENGTH;
	int   m_iCULTURE_LEVEL_EXOTIC;
	int   m_iCULTURE_LEVEL_FAMILIAR;
	int   m_iCULTURE_LEVEL_POPULAR;
	int   m_iCULTURE_LEVEL_INFLUENTIAL;
	int   m_iCULTURE_LEVEL_DOMINANT;
	int   m_iMIN_DIG_SITES_PER_MAJOR_CIV;
	int   m_iMAX_DIG_SITES_PER_MAJOR_CIV;
	int   m_iPERCENT_SITES_HIDDEN;
	int   m_iPERCENT_HIDDEN_SITES_WRITING;
	int   m_iSAPPED_CITY_ATTACK_MODIFIER;
	int   m_iSAPPER_BONUS_RANGE;
	int   m_iEXOTIC_GOODS_GOLD_MIN;
	int   m_iEXOTIC_GOODS_GOLD_MAX;
	int   m_iEXOTIC_GOODS_XP_MIN;
	int   m_iEXOTIC_GOODS_XP_MAX;
	int   m_iTEMPORARY_CULTURE_BOOST_MOD;
	int   m_iTEMPORARY_TOURISM_BOOST_MOD;
	int   m_iLEAGUE_SESSION_INTERVAL_BASE_TURNS;
	int   m_iLEAGUE_SESSION_SOON_WARNING_TURNS;
	int   m_iLEAGUE_MEMBER_PROPOSALS_BASE;
	int   m_iLEAGUE_MEMBER_VOTES_BASE;
	int   m_iLEAGUE_MEMBER_VOTES_FOR_HOST;
	int   m_iLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY;
	float m_fLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD;
	float m_fLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int   m_iLEAGUE_PROPOSERS_PER_SESSION;
	int   m_iLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES;
	int   m_iLEAGUE_PROJECT_PROGRESS_PERCENT_WARNING;
#endif
	int   m_iPROMOTION_GOODY_HUT_PICKER;
	int   m_iIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF;
	int   m_iIDEOLOGY_SCORE_GRAND_STRATS;
	int   m_iIDEOLOGY_SCORE_HAPPINESS;
	int   m_iIDEOLOGY_SCORE_PER_FREE_TENET;
	int   m_iIDEOLOGY_SCORE_HOSTILE;
	int   m_iIDEOLOGY_SCORE_GUARDED;
	int   m_iIDEOLOGY_SCORE_AFRAID;
	int   m_iIDEOLOGY_SCORE_FRIENDLY;
	int   m_iPOLICY_BRANCH_FREEDOM;
	int   m_iPOLICY_BRANCH_AUTOCRACY;
	int   m_iPOLICY_BRANCH_ORDER;
#if defined(MOD_ISKA_HERITAGE)
	int   m_iPOLICY_BRANCH_HERITAGE;
#endif

	// -- post defines --

	int m_iLAND_TERRAIN;
	int m_iDEEP_WATER_TERRAIN;
	int m_iSHALLOW_WATER_TERRAIN;
	int m_iRUINS_IMPROVEMENT;
	int m_iNUKE_FEATURE;
	int m_iARTIFACT_RESOURCE;
	int m_iHIDDEN_ARTIFACT_RESOURCE;
	int m_iCAPITAL_BUILDINGCLASS;
	int m_iDEFAULT_SPECIALIST;
	int m_iSPACE_RACE_TRIGGER_PROJECT;
	int m_iSPACESHIP_CAPSULE;
	int m_iSPACESHIP_BOOSTER;
	int m_iSPACESHIP_STASIS;
	int m_iSPACESHIP_ENGINE;
	int m_iMANHATTAN_PROJECT;
	int m_iINITIAL_CITY_ROUTE_TYPE;
	int m_iSTANDARD_HANDICAP;
	int m_iMULTIPLAYER_HANDICAP;
	int m_iSTANDARD_HANDICAP_QUICK;
	int m_iSTANDARD_GAMESPEED;
	int m_iSTANDARD_TURNTIMER;
	int m_iSTANDARD_CLIMATE;
	int m_iSTANDARD_WORLD_SIZE;
	int m_iSTANDARD_SEALEVEL;
	int m_iSTANDARD_ERA;
	int m_iLAST_EMBARK_ART_ERA;
	int m_iLAST_UNIT_ART_ERA;
	int m_iLAST_BRIDGE_ART_ERA;
	int m_iSTANDARD_CALENDAR;
	int m_iBARBARIAN_HANDICAP;
	int m_iBARBARIAN_CIVILIZATION;
	int m_iBARBARIAN_LEADER;
	int m_iMINOR_CIV_HANDICAP;
	int m_iMINOR_CIVILIZATION;
	int m_iPROMOTION_EMBARKATION;
	int m_iPROMOTION_DEFENSIVE_EMBARKATION;
	int m_iPROMOTION_ALLWATER_EMBARKATION;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	GD_INT_DECL(PROMOTION_DEEPWATER_EMBARKATION);
	GD_INT_DECL(PROMOTION_DEFENSIVE_DEEPWATER_EMBARKATION);
#endif
#if defined(MOD_PROMOTIONS_FLAGSHIP)
	GD_INT_DECL(PROMOTION_FLAGSHIP);
#endif
#if defined(MOD_PROMOTIONS_AURA_CHANGE)
	GD_INT_DECL(GREAT_GENERAL_MAX_RANGE);
#endif
	int m_iPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY;
	int m_iPROMOTION_OCEAN_IMPASSABLE;
	int m_iAI_HANDICAP;
	int m_iBARBARIAN_CAMP_IMPROVEMENT;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	int m_iEMBASSY_IMPROVEMENT;
#endif
	int m_iWALLS_BUILDINGCLASS;
	int m_iPROMOTION_ONLY_DEFENSIVE;
	int m_iPROMOTION_UNWELCOME_EVANGELIST;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	float m_fTECH_COST_ERA_EXPONENT;
	float m_fVASSALAGE_VASSAL_CITY_POP_EXPONENT;

	int m_iHELP_REQUEST_TURN_LIMIT_MIN;
	int m_iHELP_REQUEST_TURN_LIMIT_RAND;
	int m_iSHARE_OPINION_RAND;
	int m_iSHARE_OPINION_FLAVOR_BASE;
	int m_iSHARE_OPINION_FLAVOR_MULTIPLIER;
	int m_iSHARE_OPINION_TURN_BUFFER;
	int m_iVASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT;
	int m_iVASSAL_SCORE_PERCENT;
	int m_iVASSAL_SCIENCE_PERCENT;
	int m_iVASSAL_HAPPINESS_PERCENT;
	int m_iVASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT;
	int m_iVASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT;
	int m_iVASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT;
	int m_iVASSALAGE_FAILED_PROTECT_CITY_DISTANCE;
	int m_iVASSALAGE_FAILED_PROTECT_PER_TURN_DECAY;
	int m_iVASSALAGE_PROTECTED_PER_TURN_DECAY;
	int m_iVASSALAGE_VASSAL_LOST_CITIES_THRESHOLD;
	int m_iVASSALAGE_VASSAL_POPULATION_THRESHOLD;
	int m_iVASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD;
	int m_iVASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD;
	int m_iVASSALAGE_CAPITULATE_BASE_THRESHOLD;
	int m_iVASSALAGE_VASSAL_TAX_PERCENT_MINIMUM;
	int m_iVASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM;

	int m_iVASSALAGE_TREATMENT_THRESHOLD_DISAGREE;
	int m_iVASSALAGE_TREATMENT_THRESHOLD_MISTREATED;
	int m_iVASSALAGE_TREATMENT_THRESHOLD_UNHAPPY;
	int m_iVASSALAGE_TREATMENT_THRESHOLD_ENSLAVED;

	int m_iOPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD;
	int m_iOPINION_WEIGHT_DEMANDED_WHILE_VASSAL;
	float m_fOPINION_WEIGHT_VASSAL_TAX_EXPONENT;
	int m_iOPINION_WEIGHT_VASSAL_TAX_DIVISOR;
	int m_iOPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER;
	int m_iOPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE;
	int m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL;
	int m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL;
	int m_iOPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER;
	int m_iOPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED;
	int m_iOPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED;
	int m_iOPINION_WEIGHT_VASSALAGE_PROTECT_MAX;
	int m_iOPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX;
	int m_iOPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN;
	int m_iOPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT_OPINION_WEIGHT;
	int m_iOPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS;

	int m_iAPPROACH_HOSTILE_WE_ARE_VASSAL;
	int m_iAPPROACH_GUARDED_WE_ARE_VASSAL;
	int m_iAPPROACH_DECEPTIVE_WE_ARE_VASSAL;
	int m_iAPPROACH_WAR_VASSAL_FORCEFULLY_REVOKED;
	int m_iAPPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED;
	int m_iAPPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED;
	int m_iAPPROACH_WAR_VASSAL_PEACEFULLY_REVOKED;
	int m_iAPPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED;
	int m_iAPPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED;
	int m_iAPPROACH_WAR_MY_VASSAL;
	int m_iAPPROACH_GUARDED_TOO_MANY_VASSALS;
	int m_iAPPROACH_WAR_TOO_MANY_VASSALS;

	int m_iGLOBAL_STATE_NEARLY_DEFEATED_WEIGHT;
	int m_iGLOBAL_STATE_DEFENSIVE_WEIGHT;
	int m_iGLOBAL_STATE_STALEMATE_WEIGHT;
	int m_iGLOBAL_STATE_CALM_WEIGHT;
	int m_iGLOBAL_STATE_OFFENSIVE_WEIGHT;
	int m_iGLOBAL_STATE_NEARLY_WON_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_IMMENSE_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_POWERFUL_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_STRONG_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_AVERAGE_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_WEAK_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_POOR_WEIGHT;
	int m_iGLOBAL_STATE_MIL_STRENGTH_PATHETIC_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_IMMENSE_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_POWERFUL_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_STRONG_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_AVERAGE_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_WEAK_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_POOR_WEIGHT;
	int m_iGLOBAL_STATE_ECON_STRENGTH_PATHETIC_WEIGHT;
	int m_iGLOBAL_STATE_NUM_VASSALS_MULTIPLIER;
	int m_iGLOBAL_STATE_TECH_PERCENT_VERY_GOOD_WEIGHT;
	int m_iGLOBAL_STATE_TECH_PERCENT_GOOD_WEIGHT;
	int m_iGLOBAL_STATE_TECH_PERCENT_AVERAGE_WEIGHT;
	int m_iGLOBAL_STATE_TECH_PERCENT_BAD_WEIGHT;
	int m_iGLOBAL_STATE_TECH_PERCENT_VERY_BAD_WEIGHT;
	int m_iGLOBAL_STATE_PER_POLICY_WEIGHT;
	int m_iGLOBAL_STATE_PER_ALLY_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_EXTREMELY_GOOD_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_VERY_GOOD_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_GOOD_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_AVERAGE_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_BAD_WEIGHT;
	int m_iGLOBAL_STATE_SCORE_PERCENT_VERY_BAD_WEIGHT;
	int m_iGLOBAL_STATE_MIN_TURNS_BEFORE_PROCESS_STATE;

	int m_iTARGET_VASSAL_BACKUP_PATHETIC;
	int m_iTARGET_VASSAL_BACKUP_WEAK;
	int m_iTARGET_VASSAL_BACKUP_POOR;
	int m_iTARGET_VASSAL_BACKUP_AVERAGE;
	int m_iTARGET_VASSAL_BACKUP_STRONG;
	int m_iTARGET_VASSAL_BACKUP_POWERFUL;
	int m_iTARGET_VASSAL_BACKUP_IMMENSE;
	int m_iTARGET_VASSAL_BACKUP_DISTANT;
	int m_iTARGET_VASSAL_BACKUP_FAR;
	int m_iTARGET_VASSAL_BACKUP_CLOSE;
	int m_iTARGET_VASSAL_BACKUP_NEIGHBORS;
	int m_iVASSAL_RELIGIOUS_PRESSURE_MODIFIER;
	int m_iVASSAL_TOURISM_MODIFIER;
	int m_iMAX_PLOTS_PER_EXPLORER;
#endif

	// DLL interface
	ICvEngineUtility4* m_pDLL;

	// Engine UI interface
	ICvUserInterface2* m_pEngineUI;
};

//extern CvGlobals gGlobals;	// for debugging

//
// inlines
//
//
// helpers
//
extern CvGlobals gGlobals;
#define GC gGlobals
#define gDLL GC.getDLLIFace()
#define DB (*GC.GetGameDatabase())
#define DLLUI (GC.GetEngineUserInterface())
#define DLLUIClass CvDLLInterfaceIFaceBase

inline Database::Connection* CvGlobals::GetGameDatabase()
{
	return m_pGameDatabase;
}

inline const Database::Connection* CvGlobals::GetGameDatabase() const
{
	return m_pGameDatabase;
}

//cannot use GC.getGame().getActivePlayer() in observer mode
PlayerTypes GetCurrentPlayer();

#ifdef STACKWALKER
#include "Stackwalker/Stackwalker.h"

class MyStackWalker : public StackWalker
{
public:
	MyStackWalker() : m_pLog(NULL), StackWalker() {}
	void SetLog(FILogFile* pLog) { m_pLog=pLog; }
protected:
	virtual void OnSymInit(LPCSTR, DWORD, LPCSTR) { /*dummy*/ }
	virtual void OnLoadModule(LPCSTR, LPCSTR, DWORD64, DWORD, DWORD, LPCSTR, LPCSTR, ULONGLONG) { /*dummy*/ }
	virtual void OnOutput(LPCSTR szText) 
	{ 
		if (strstr(szText,"ERROR")==NULL && strstr(szText,"not available")==NULL) 
		{
			if (m_pLog)
				m_pLog->Msg(szText);
			else
				OutputDebugString(szText);
		}
	}
	FILogFile* m_pLog;
};

extern MyStackWalker gStackWalker;
#endif

#endif //CIV5_GLOBALS_H