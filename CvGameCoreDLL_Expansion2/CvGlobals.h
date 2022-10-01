/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#include <array>

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
class CvPlotInfo;
class CvGreatPersonInfo;
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
#if defined(MOD_API_ACHIEVEMENTS)
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
	enum SaveVersionTags
	{
		SAVE_VERSION_LATEST = 0,
	};

	typedef stdext::hash_map<std::string /* type string */, int /* info index */> InfosMap;
	typedef std::map<uint /* FString::HashType */, int /* info index */> InfosHashMap;
	typedef tr1::array<uint32, 4> GameDataHash;

	// singleton accessor
	static CvGlobals& getInstance();

	CvGlobals();
	virtual ~CvGlobals();

	//DLL Utility Methods
	CvCity* UnwrapCityPointer(ICvCity1* pCity);
	CvInterfacePtr<ICvCity1> WrapCityPointer(CvCity* pCity);

	CvDeal* UnwrapDealPointer(ICvDeal1* pDeal);
	CvInterfacePtr<ICvDeal1> WrapDealPointer(CvDeal* pDeal);

	CvPlot* UnwrapPlotPointer(ICvPlot1* pPlot);
	CvInterfacePtr<ICvPlot1> WrapPlotPointer(CvPlot* pPlot);

	CvRandom* UnwrapRandomPointer(ICvRandom1* pRandom);
	CvInterfacePtr<ICvRandom1> WrapRandomPointer(CvRandom* pRandom);

	CvInterfacePtr<ICvUnit1> WrapUnitPointer(CvUnit* pUnit);
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

	int* getRingFromLinearOffset();
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

	void GameDataPostCache(); ///prepare some more caching
	void calcGameDataHash();

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

	int getNumPlotInfos();
	std::vector<CvPlotInfo*>& getPlotInfo();
	CvPlotInfo* getPlotInfo(PlotTypes ePlotNum);

	int getNumGreatPersonInfos();
	std::vector<CvGreatPersonInfo*>& getGreatPersonInfo();
	CvGreatPersonInfo* getGreatPersonInfo(GreatPersonTypes eGreatPersonNum);

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

	//some caching to avoid iterating all building types
	const std::vector <BuildingTypes>& getBuildingInteractions(BuildingTypes eRefBuilding) const;

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
#if defined(MOD_API_ACHIEVEMENTS)
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

	////////////// BEGIN DEFINES //////////////////
	/////////// THESE ARE READ-ONLY ///////////////

	void cacheGlobals();

	// -- ints -- //
	GD_INT_MEMBER(AI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD);
	GD_INT_MEMBER(INITIAL_AI_CITY_PRODUCTION);
	GD_INT_MEMBER(POLICY_WEIGHT_PROPAGATION_PERCENT);
	GD_INT_MEMBER(POLICY_WEIGHT_PROPAGATION_LEVELS);
	GD_INT_MEMBER(POLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH);
	GD_INT_MEMBER(TECH_WEIGHT_PROPAGATION_PERCENT);
	GD_INT_MEMBER(TECH_WEIGHT_PROPAGATION_LEVELS);
	GD_INT_MEMBER(TECH_PRIORITY_UNIQUE_ITEM);
	GD_INT_MEMBER(TECH_PRIORITY_MAYA_CALENDAR_BONUS);
	GD_INT_MEMBER(DEFAULT_FLAVOR_VALUE);
	GD_INT_MEMBER(PERSONALITY_FLAVOR_MAX_VALUE);
	GD_INT_MEMBER(PERSONALITY_FLAVOR_MIN_VALUE);
	GD_INT_MEMBER(DIPLO_PERSONALITY_FLAVOR_MAX_VALUE); // VP
	GD_INT_MEMBER(DIPLO_PERSONALITY_FLAVOR_MIN_VALUE); // VP
	GD_INT_MEMBER(FLAVOR_MIN_VALUE);
	GD_INT_MEMBER(FLAVOR_MAX_VALUE);
	GD_INT_MEMBER(FLAVOR_RANDOMIZATION_RANGE);
	GD_INT_MEMBER(FLAVOR_EXPANDGROW_COEFFICIENT);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER);
	GD_INT_MEMBER(AI_GS_CONQUEST_NOBODY_MET_FIRST_TURN);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT);
	GD_INT_MEMBER(AI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED);
	GD_INT_MEMBER(AI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED);
	GD_INT_MEMBER(AI_GS_CULTURE_RATIO_MULTIPLIER);
	GD_INT_MEMBER(AI_GS_TOURISM_RATIO_MULTIPLIER);
	GD_INT_MEMBER(AI_GS_CULTURE_AHEAD_WEIGHT);
	GD_INT_MEMBER(AI_GS_CULTURE_TOURISM_AHEAD_WEIGHT);
	GD_INT_MEMBER(AI_GS_CULTURE_INFLUENTIAL_CIV_MOD);
	GD_INT_MEMBER(AI_GS_UN_SECURED_VOTE_MOD);
	GD_INT_MEMBER(AI_GS_SS_HAS_APOLLO_PROGRAM);
	GD_INT_MEMBER(AI_GS_SS_TECH_PROGRESS_MOD);
	GD_INT_MEMBER(EVENT_MIN_DURATION_BETWEEN); // VP
	GD_INT_MEMBER(CITY_EVENT_MIN_DURATION_BETWEEN); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_THRESHOLD_PERCENTILE); // VP
	GD_INT_MEMBER(GLOBAL_RESOURCE_MONOPOLY_THRESHOLD); // VP
	GD_INT_MEMBER(STRATEGIC_RESOURCE_MONOPOLY_THRESHOLD); // VP
	GD_INT_MEMBER(MAX_PLOTS_PER_EXPLORER);
	GD_INT_MEMBER(AI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT);
	GD_INT_MEMBER(AI_PLOT_VALUE_STRATEGIC_RESOURCE);
	GD_INT_MEMBER(AI_PLOT_VALUE_LUXURY_RESOURCE);
	GD_INT_MEMBER(AI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER);
	GD_INT_MEMBER(AI_PLOT_VALUE_YIELD_MULTIPLIER);
	GD_INT_MEMBER(AI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER);
	GD_INT_MEMBER(AI_PLOT_VALUE_FIERCE_DISPUTE);
	GD_INT_MEMBER(AI_PLOT_VALUE_STRONG_DISPUTE);
	GD_INT_MEMBER(AI_PLOT_VALUE_WEAK_DISPUTE);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE);
	GD_INT_MEMBER(AI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_UPGRADE_BASE);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_DIPLOMACY_BASE);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_UNIT);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_DEFENSIVE_BUILDING);
	GD_INT_MEMBER(AI_GOLD_PRIORITY_BUYOUT_CITY_STATE);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE);
	GD_INT_MEMBER(BUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_BUILD_ROUTES);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_REPAIR);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_SCRUB_FALLOUT);
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_FOOD); // VP
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_GOLD); // VP
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_FAITH); // VP
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_PRODUCTION); // VP
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_SCIENCE); // VP
	GD_INT_MEMBER(BUILDER_TASKING_BASELINE_ADDS_CULTURE);
	GD_INT_MEMBER(AI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS);
	GD_INT_MEMBER(AI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL);
	GD_INT_MEMBER(AI_MILITARY_RECAPTURING_OWN_CITY);
	GD_INT_MEMBER(AI_MILITARY_CAPTURING_ORIGINAL_CAPITAL);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_EARLIEST_TURN);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE);
	GD_INT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY);
	GD_INT_MEMBER(AI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD);
	GD_INT_MEMBER(AI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD);
	GD_INT_MEMBER(AI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD);
	GD_INT_MEMBER(AI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN);
	GD_INT_MEMBER(AI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE);
	GD_INT_MEMBER(AI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT);
	GD_INT_MEMBER(AI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER);
	GD_INT_MEMBER(AI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT);
	GD_INT_MEMBER(AI_MOSTLY_HAPPY_THRESHOLD); // VP
	GD_INT_MEMBER(AI_CITY_VALUE_MULTIPLIER); // VP
	GD_INT_MEMBER(AI_CAPITAL_VALUE_MULTIPLIER); // VP
	GD_INT_MEMBER(AI_CITY_HIGH_VALUE_THRESHOLD); // VP
	GD_INT_MEMBER(AI_CITY_SOME_VALUE_THRESHOLD); // VP
	GD_INT_MEMBER(AI_CITY_PUPPET_BONUS_THRESHOLD); // VP
	GD_INT_MEMBER(AI_CITIZEN_VALUE_FOOD);
	GD_INT_MEMBER(AI_CITIZEN_VALUE_PRODUCTION);
	GD_INT_MEMBER(AI_CITIZEN_VALUE_GOLD);
	GD_INT_MEMBER(AI_CITIZEN_VALUE_SCIENCE);
	GD_INT_MEMBER(AI_CITIZEN_VALUE_CULTURE);
	GD_INT_MEMBER(AI_CITIZEN_VALUE_FAITH);
	GD_INT_MEMBER(AI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION);
	GD_INT_MEMBER(AI_TACTICAL_MAP_DOMINANCE_PERCENTAGE);
	GD_INT_MEMBER(AI_TACTICAL_MAP_TEMP_ZONE_TURNS);
	GD_INT_MEMBER(AI_TACTICAL_RECRUIT_RANGE);
	GD_INT_MEMBER(AI_TACTICAL_BARBARIAN_RELEASE_VARIATION);
	GD_INT_MEMBER(AI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS);
	GD_INT_MEMBER(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT);
	GD_INT_MEMBER(AI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT);
	GD_INT_MEMBER(AI_CONFIG_MILITARY_MELEE_PER_AA); // VP
	GD_INT_MEMBER(AI_CONFIG_MILITARY_TILES_PER_SHIP); // VP
	GD_INT_MEMBER(MINOR_BULLY_GOLD);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_PER_TURN);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_AT_WAR);
	GD_INT_MEMBER(MINOR_CIV_AGGRESSOR_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_WARMONGER_THRESHOLD);
	GD_INT_MEMBER(PERMANENT_WAR_AGGRESSOR_CHANCE);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_CHANCE_DISTANT);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_CHANCE_FAR);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_CHANCE_CLOSE);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_CHANCE_NEIGHBORS);
	GD_INT_MEMBER(PERMANENT_WAR_OTHER_AT_WAR);
	GD_INT_MEMBER(FRIENDSHIP_NEUTRAL_ON_DEATH);
	GD_INT_MEMBER(FRIENDSHIP_FRIENDS_ON_DEATH);
	GD_INT_MEMBER(FRIENDSHIP_ALLIES_ON_DEATH);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_NEUTRAL);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_FRIENDS);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_ALLIES);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_CAN_BULLY);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL); // VP
	GD_INT_MEMBER(FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL); // VP
	GD_INT_MEMBER(MINOR_FRIENDSHIP_CLOSE_AMOUNT);
	GD_INT_MEMBER(MINOR_CIV_SCIENCE_BONUS_MULTIPLIER);
	GD_INT_MEMBER(FRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(FRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(FRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(ALLIES_CULTURE_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(ALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(ALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE);
	GD_INT_MEMBER(FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE);
	GD_INT_MEMBER(FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE);
	GD_INT_MEMBER(FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE);
	GD_INT_MEMBER(ALLIES_CAPITAL_FOOD_BONUS_AMOUNT);
	GD_INT_MEMBER(ALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT);
	GD_INT_MEMBER(FRIENDS_BASE_TURNS_UNIT_SPAWN);
	GD_INT_MEMBER(FRIENDS_RAND_TURNS_UNIT_SPAWN);
	GD_INT_MEMBER(ALLIES_EXTRA_TURNS_UNIT_SPAWN);
	GD_INT_MEMBER(UNIT_SPAWN_BIAS_MULTIPLIER);
	GD_INT_MEMBER(UNIT_SPAWN_NUM_CHOICES);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL);
	GD_INT_MEMBER(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE);
	GD_INT_MEMBER(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(ALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT);
	GD_INT_MEMBER(ALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL);
	GD_INT_MEMBER(ALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL);
	GD_INT_MEMBER(ALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE);
	GD_INT_MEMBER(ALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	GD_INT_MEMBER(MINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE);
	GD_INT_MEMBER(MINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND);
	GD_INT_MEMBER(MINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY);
	GD_INT_MEMBER(MINOR_ADDITIONAL_ALLIES_GP_CHANGE);
	GD_INT_MEMBER(MAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE);
	GD_INT_MEMBER(MAX_DISTANCE_MINORS_BARB_QUEST);
	GD_INT_MEMBER(MINOR_QUEST_REBELLION_TIMER); // VP
	GD_INT_MEMBER(SCHOLAR_MINOR_ALLY_MULTIPLIER); // VP
	GD_INT_MEMBER(SCIENCE_LEAGUE_GREAT_WORK_MODIFIER); // VP
	GD_INT_MEMBER(PIONEER_POPULATION_CHANGE); // VP
	GD_INT_MEMBER(PIONEER_EXTRA_PLOTS); // VP
	GD_INT_MEMBER(PIONEER_FOOD_PERCENT); // VP
	GD_INT_MEMBER(COLONIST_POPULATION_CHANGE); // VP
	GD_INT_MEMBER(COLONIST_EXTRA_PLOTS); // VP
	GD_INT_MEMBER(COLONIST_FOOD_PERCENT); // VP
	GD_INT_MEMBER(MINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER);
	GD_INT_MEMBER(MINOR_CIV_GOLD_GIFT_GAME_DIVISOR);
	GD_INT_MEMBER(MINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR);
	GD_INT_MEMBER(MINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD);
	GD_INT_MEMBER(FRIENDSHIP_PER_UNIT_INTRUDING);
	GD_INT_MEMBER(FRIENDSHIP_PER_BARB_KILLED);
	GD_INT_MEMBER(FRIENDSHIP_PER_UNIT_GIFTED);
	GD_INT_MEMBER(MINOR_LIBERATION_FRIENDSHIP);
	GD_INT_MEMBER(RETURN_CIVILIAN_FRIENDSHIP);
	GD_INT_MEMBER(MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN);
	GD_INT_MEMBER(MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND);
	GD_INT_MEMBER(MINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN);
	GD_INT_MEMBER(MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN);
	GD_INT_MEMBER(MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER);
	GD_INT_MEMBER(MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN);
	GD_INT_MEMBER(MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND);
	GD_INT_MEMBER(MINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN);
	GD_INT_MEMBER(MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN);
	GD_INT_MEMBER(MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_KILL_CAMP_RANGE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_MERCANTILE_RESOURCES_QUANTITY);
	GD_INT_MEMBER(MINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED);
	GD_INT_MEMBER(QUEST_DISABLED_ROUTE);
	GD_INT_MEMBER(QUEST_DISABLED_KILL_CAMP);
	GD_INT_MEMBER(QUEST_DISABLED_CONNECT_RESOURCE);
	GD_INT_MEMBER(QUEST_DISABLED_CONSTRUCT_WONDER);
	GD_INT_MEMBER(QUEST_DISABLED_GIFT_SPECIFIC_UNIT);
	GD_INT_MEMBER(QUEST_DISABLED_GREAT_PERSON);
	GD_INT_MEMBER(QUEST_DISABLED_KILL_CITY_STATE);
	GD_INT_MEMBER(QUEST_DISABLED_FIND_PLAYER);
	GD_INT_MEMBER(QUEST_DISABLED_NATURAL_WONDER);
	GD_INT_MEMBER(QUEST_DISABLED_GIVE_GOLD);
	GD_INT_MEMBER(QUEST_DISABLED_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(QUEST_DISABLED_CONTEST_CULTURE);
	GD_INT_MEMBER(QUEST_DISABLED_CONTEST_FAITH);
	GD_INT_MEMBER(QUEST_DISABLED_CONTEST_TECHS);
	GD_INT_MEMBER(QUEST_DISABLED_INVEST);
	GD_INT_MEMBER(QUEST_DISABLED_BULLY_CITY_STATE);
	GD_INT_MEMBER(QUEST_DISABLED_DENOUNCE_MAJOR);
	GD_INT_MEMBER(QUEST_DISABLED_SPREAD_RELIGION);
	GD_INT_MEMBER(QUEST_DISABLED_TRADE_ROUTE);
	GD_INT_MEMBER(QUEST_DISABLED_WAR); // VP
	GD_INT_MEMBER(QUEST_DISABLED_FIND_CITY_STATE); // VP
	GD_INT_MEMBER(QUEST_DISABLED_INFLUENCE); // VP
	GD_INT_MEMBER(QUEST_DISABLED_TOURISM); // VP
	GD_INT_MEMBER(QUEST_DISABLED_CIRCUMNAVIGATION); // VP
	GD_INT_MEMBER(QUEST_DISABLED_ARCHAEOLOGY); // VP
	GD_INT_MEMBER(QUEST_DISABLED_LIBERATION); // VP
	GD_INT_MEMBER(QUEST_DISABLED_HORDE); // VP
	GD_INT_MEMBER(QUEST_DISABLED_REBELLION); // VP
	GD_INT_MEMBER(QUEST_DISABLED_CP_QUESTS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_KILL_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_IRRATIONAL_KILL_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_BULLY_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_IRRATIONAL_BULLY_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_BULLY_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_DENOUNCE_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_DENOUNCE_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_DENOUNCE_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_SPREAD_RELIGION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_TRADE_ROUTE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_TRADE_ROUTE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_DISCOVER_PLOT); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_DISCOVER_PLOT); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_DISCOVER_PLOT); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_BUILD_BUILDINGS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_BUILD_BUILDINGS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_BUILD_BUILDINGS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_SPY_ON_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_SPY_ON_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_SPY_ON_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_SPY_ON_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_SPY_ON_MAJOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_COUP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_COUP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_COUP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_COUP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_COUP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_CAPTURE_CITY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_CAPTURE_CITY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CAPTURE_CITY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_CAPTURE_CITY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_CAPTURE_CITY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_WAR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_WAR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_WAR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_WAR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_NATIONAL_WONDER); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_NATIONAL_WONDER); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIFT_SPECIFIC_UNIT); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIFT_SPECIFIC_UNIT); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_CITY_STATE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_ARCHAEOLOGY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_ARCHAEOLOGY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_ARCHAEOLOGY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CIRCUMNAVIGATION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_LIBERATION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_LIBERATION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_LIBERATION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_LIBERATION); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_KILL_CAMP); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP);
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONTEST_CULTURE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_CONTEST_FAITH); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_CONTEST_FAITH); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_RELIGIOUS_CONTEST_TECHS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_INVEST); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_INFLUENCE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_INFLUENCE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_TOURISM); // VP
	GD_INT_MEMBER(BARBARIAN_HORDE_FREQUENCY); // VP
	GD_INT_MEMBER(QUEST_REBELLION_FREQUENCY); // VP
	GD_INT_MEMBER(INFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE); // VP
	GD_INT_MEMBER(AI_MILITARY_RECAPTURING_CITY_STATE); // VP
	GD_INT_MEMBER(NEED_DIPLOMAT_THRESHOLD_MODIFIER); // VP
	GD_INT_MEMBER(NEED_DIPLOMAT_DESIRE_MODIFIER); // VP
	GD_INT_MEMBER(NEED_DIPLOMAT_DISTASTE_MODIFIER); // VP
	GD_INT_MEMBER(INFLUENCE_MINOR_QUEST_BOOST); // VP
	GD_INT_MEMBER(MINOR_FRIENDSHIP_DROP_PER_TURN_DAMAGED_CAPITAL_MULTIPLIER); // VP
	GD_INT_MEMBER(LEAGUE_AID_MAX); // VP
	GD_INT_MEMBER(CSD_GOLD_GIFT_DISABLED); // VP
	GD_INT_MEMBER(RELIGION_BELIEF_SCORE_CITY_MULTIPLIER);
	GD_INT_MEMBER(RELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER);
	GD_INT_MEMBER(RELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER);
	GD_INT_MEMBER(RELIGION_MISSIONARY_RANGE_IN_TURNS);
	GD_INT_MEMBER(RELIGION_MAX_MISSIONARIES);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_THRESHOLD);
	GD_INT_MEMBER(MC_ALWAYS_GIFT_DIPLO_THRESHOLD);
	GD_INT_MEMBER(MC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_PASS_OTHER_PLAYER);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_MARITIME_GROWTH);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_DIPLO_VICTORY);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_CULTURE_VICTORY);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_CONQUEST_VICTORY);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_MILITARISTIC);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_RESOURCE_WE_NEED);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_PROTECTIVE);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_HOSTILE);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_NEIGHBORS);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_CLOSE);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_FAR);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_ALMOST_NOT_ALLIES);
	GD_INT_MEMBER(MC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS);
	GD_INT_MEMBER(OPINION_THRESHOLD_UNFORGIVABLE);
	GD_INT_MEMBER(OPINION_THRESHOLD_ENEMY);
	GD_INT_MEMBER(OPINION_THRESHOLD_COMPETITOR);
	GD_INT_MEMBER(OPINION_THRESHOLD_FAVORABLE);
	GD_INT_MEMBER(OPINION_THRESHOLD_FRIEND);
	GD_INT_MEMBER(OPINION_THRESHOLD_ALLY);
	GD_INT_MEMBER(OPINION_THRESHOLD_MAJOR_POSITIVE); // VP
	GD_INT_MEMBER(OPINION_THRESHOLD_MAJOR_NEGATIVE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_BASE_HUMAN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_BASE_AI); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_FIERCE);
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_STRONG);
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_WEAK);
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_NONE);
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_WARMONGER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LAND_NONE_WARMONGER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_FIERCE);
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_STRONG);
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_WEAK);
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_NONE);
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_PER_ERA); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_BLOCK_FIERCE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_BLOCK_STRONG); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_BLOCK_WEAK); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_BLOCK_NONE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VICTORY_BLOCK_PER_ERA); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_FIERCE);
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_STRONG);
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_WEAK);
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_NONE);
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_CULTURAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_NONE_CULTURAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_FIERCE);
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_STRONG);
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_WEAK);
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_NONE);
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_DIPLOMAT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MINOR_CIV_NONE_DIPLOMAT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_TECH_FIERCE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_TECH_STRONG); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_TECH_WEAK); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_TECH_NONE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_POLICY_FIERCE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_POLICY_STRONG); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_POLICY_WEAK); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_POLICY_NONE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_LOW); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_MEDIUM); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_HIGH); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_INCREDIBLE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_ASKED_NO_SETTLE);
	GD_INT_MEMBER(OPINION_WEIGHT_ASKED_STOP_SPYING);
	GD_INT_MEMBER(OPINION_WEIGHT_MADE_DEMAND_OF_US);
	GD_INT_MEMBER(OPINION_WEIGHT_MADE_DEMAND_OF_US_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_CIVILIAN);
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_CIVILIAN_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_BUILT_LANDMARK);
	GD_INT_MEMBER(OPINION_WEIGHT_BUILT_LANDMARK_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RESURRECTED);
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATOR_CAPTURED_CAPITAL_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATOR_CAPTURED_HOLY_CITY_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_EMBASSY);
	GD_INT_MEMBER(OPINION_WEIGHT_EMBASSY_THEM); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_EMBASSY_MUTUAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DIPLOMAT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DIPLOMAT_MOD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CULTURE_BOMBED);
	GD_INT_MEMBER(RELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY);
	GD_INT_MEMBER(RELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY);
	GD_INT_MEMBER(RELIGION_DIPLO_HIT_CONVERT_HOLY_CITY);
	GD_INT_MEMBER(RELIGION_DIPLO_HIT_THRESHOLD);
	GD_INT_MEMBER(OPINION_WEIGHT_PER_NEGATIVE_CONVERSION);
	GD_INT_MEMBER(OPINION_WEIGHT_ADOPTING_HIS_RELIGION);
	GD_INT_MEMBER(OPINION_WEIGHT_ADOPTING_MY_RELIGION);
	GD_INT_MEMBER(OPINION_WEIGHT_SAME_STATE_RELIGIONS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DIFFERENT_STATE_RELIGIONS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DIFFERENT_OWNED_RELIGIONS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WORLD_RELIGION_MODIFIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_SAME_LATE_POLICIES);
	GD_INT_MEMBER(OPINION_WEIGHT_DIFFERENT_LATE_POLICIES);
	GD_INT_MEMBER(OPINION_WEIGHT_WORLD_IDEOLOGY_MODIFIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_IDEOLOGY_VASSAL_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_MILITARY_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_MILITARY_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_EXPANSION_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_EXPANSION_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_BORDER_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_BORDER_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_BULLY_CITY_STATE_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_BULLY_CITY_STATE_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_SPY_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_SPY_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_FORGAVE_FOR_SPYING);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_NO_DIG_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_IGNORED_NO_DIG_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE);
	GD_INT_MEMBER(OPINION_WEIGHT_KILLED_PROTECTED_MINOR);
	GD_INT_MEMBER(OPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS);
	GD_INT_MEMBER(OPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN);
	GD_INT_MEMBER(OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY);
	GD_INT_MEMBER(OPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS);
	GD_INT_MEMBER(OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN);
	GD_INT_MEMBER(OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY);
	GD_INT_MEMBER(OPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS);
	GD_INT_MEMBER(OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN);
	GD_INT_MEMBER(OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR);
	GD_INT_MEMBER(OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_AGGRESSIVE_MOD);
	GD_INT_MEMBER(OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN);
	GD_INT_MEMBER(OPINION_WEIGHT_DOF);
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_MOST_VALUED_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_TYPE_FRIENDS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_TYPE_ALLIES); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_TYPE_BATTLE_BROTHERS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_FRIEND);
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_FRIEND_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_MOST_VALUED_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_MOST_VALUED_ALLY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_ENEMY);
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_ENEMY_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_BIGGEST_COMPETITOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DOF_WITH_BIGGEST_LEAGUE_RIVAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_BETRAYED_OUR_FRIEND_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_FRIEND_EACH);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_ME_FRIENDS);
	GD_INT_MEMBER(OPINION_WEIGHT_ATTACKED_OWN_VASSAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WAR_FRIEND_EACH);
	GD_INT_MEMBER(OPINION_WEIGHT_WAR_ME_FRIENDS);
	GD_INT_MEMBER(OPINION_WEIGHT_ATTACKED_RESURRECTED_PLAYER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RESURRECTOR_ATTACKED_US); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_ME);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_THEM);
	GD_INT_MEMBER(OPINION_WEIGHT_MUTUAL_DENOUNCEMENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_FRIEND);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_FRIEND_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_MOST_VALUED_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_MOST_VALUED_ALLY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_ENEMY);
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_ENEMY_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BIGGEST_COMPETITOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BIGGEST_LEAGUE_RIVAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DENOUNCED_BY_THEIR_KEY_FRIEND); // VP
	GD_INT_MEMBER(RECKLESS_EXPANDER_CITIES_THRESHOLD); // VP
	GD_INT_MEMBER(RECKLESS_EXPANDER_LAND_THRESHOLD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RECKLESS_EXPANDER);
	GD_INT_MEMBER(OPINION_WEIGHT_RECKLESS_EXPANDER_PER_CITY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RECKLESS_EXPANDER_PER_TILE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RECKLESS_EXPANDER_STRATEGIC_MOD); // VP
	GD_INT_MEMBER(WONDER_SPAMMER_THRESHOLD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_SPAMMER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_SPAMMER_PER_WONDER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_SPAMMER_CAP); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WONDER_SPAMMER_STRATEGIC_MOD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_TRADE_MAX);
	GD_INT_MEMBER(DEAL_VALUE_PER_TURN_DECAY);
	GD_INT_MEMBER(DEAL_VALUE_PER_OPINION_WEIGHT);
	GD_INT_MEMBER(OPINION_WEIGHT_STRATEGIC_TRADE_PARTNER_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_COMMON_FOE_MAX);
	GD_INT_MEMBER(COMMON_FOE_VALUE_PER_TURN_DECAY);
	GD_INT_MEMBER(COMMON_FOE_VALUE_PER_OPINION_WEIGHT);
	GD_INT_MEMBER(OPINION_WEIGHT_ASSIST_MAX);
	GD_INT_MEMBER(ASSIST_VALUE_PER_TURN_DECAY);
	GD_INT_MEMBER(ASSIST_VALUE_PER_OPINION_WEIGHT);
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_CAPITAL);
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_CAPITAL_VASSAL_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_HOLY_CITY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_HOLY_CITY_VASSAL_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_CITY);
	GD_INT_MEMBER(OPINION_WEIGHT_LIBERATED_CITY_VASSAL_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_CAPITAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_CAPITAL_VASSAL_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_HOLY_CITY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RETURNED_HOLY_CITY_VASSAL_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_NUKED_MAX);
	GD_INT_MEMBER(OPINION_WEIGHT_CIVILIAN_KILLER_MAX); // VP
	GD_INT_MEMBER(CIVILIAN_KILLER_VALUE_PER_OPINION_WEIGHT); // VP
	GD_INT_MEMBER(CIVILIAN_KILLER_VALUE_PER_TURN_DECAY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CIVILIAN_KILLER_WORLD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CIVILIAN_KILLER_WORLD_THRESHOLD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PER_SIMILAR_POLICY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PER_DIVERGENT_POLICY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_SIMILAR_POLICIES); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DIVERGENT_POLICIES); // VP
	GD_INT_MEMBER(POLICY_SCORE_NEEDY_THRESHOLD); // VP
	GD_INT_MEMBER(POLICY_SCORE_NEEDY_BONUS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PTP_SAME_MINOR_EACH); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PTP_SAME_MINOR_MIN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PTP_SAME_MINOR_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPTURED_CAPITAL);
	GD_INT_MEMBER(OPINION_WEIGHT_CAPTURED_HOLY_CITY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPTURED_KEY_CITY_RETURNED_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPTURED_KEY_CITY_CAPITULATION_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_ROBBED_BY);
	GD_INT_MEMBER(OPINION_WEIGHT_INTRIGUE_SHARED_BY);
	GD_INT_MEMBER(OPINION_WEIGHT_INTRIGUE_SHARED_BY_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_MOST_VALUED_ALLY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_FRIEND_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_MOST_VALUED_ALLY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_MOST_VALUED_FRIEND); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_ENEMY); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_ENEMY_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_BIGGEST_COMPETITOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DP_WITH_BIGGEST_LEAGUE_RIVAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RA); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_RA_SCIENTIFIC_MOD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_OPEN_BORDERS_MUTUAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_OPEN_BORDERS_US); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_OPEN_BORDERS_THEM); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PLUNDERED_TRADE_ROUTE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PLOTTED_AGAINST_US); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PLOTTED_AGAINST_US_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_PERFORMED_COUP); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_EXCAVATED_ARTIFACT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VOTING_HISTORY_MAX); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VOTING_HISTORY_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_MAX); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_PLAYER_VOTE_PERCENT_VALUE); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_OUTCOME_VOTE_PERCENT_VALUE); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_WEAK); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STANDARD); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STRONG); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_OVERWHELMING); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_LEAGUE_ALIGNMENT_SCALER); // VP
	GD_INT_MEMBER(VOTING_HISTORY_SCORE_PRIME_COMPETITOR_THRESHOLD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_WEAK); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL);
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS);
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_WEAK); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL);
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_MAX); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_MAX); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING);
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_MAX); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS);
	GD_INT_MEMBER(OPINION_WEIGHT_PER_VOTE_PERCENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SANCTIONED_US); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SANCTIONED_US_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_SANCTIONED_US_FAILURE_DIVISOR); // VP
	GD_INT_MEMBER(SANCTIONED_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_UNSANCTIONED_US); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_UNSANCTIONED_US_DIPLOMAT_MULTIPLIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_THEY_UNSANCTIONED_US_FAILURE_DIVISOR); // VP
	GD_INT_MEMBER(UNSANCTIONED_US_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER); // VP - Start C4DF Opinion Values
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_SAME_MASTER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_DEMANDED_WHILE_VASSAL); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPITULATED_VASSAL_PLUNDERED_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPITULATED_VASSAL_SPYING_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_CAPITULATED_VASSAL_CULTURE_BOMB_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_DENOUNCED_BY_MASTER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_TAX_DIVISOR); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_PROTECT_MAX); // VP
	GD_INT_MEMBER(VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT); // VP
	GD_INT_MEMBER(VASSALAGE_PROTECTED_PER_TURN_DECAY); // VP
	GD_INT_MEMBER(VASSALAGE_PROTECTED_CITY_DISTANCE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX); // VP
	GD_INT_MEMBER(VASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT); // VP
	GD_INT_MEMBER(VASSALAGE_FAILED_PROTECT_PER_TURN_DECAY); // VP
	GD_INT_MEMBER(VASSALAGE_FAILED_PROTECT_CITY_DISTANCE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_TRADE_ROUTE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_TRADE_ROUTE_SUBSEQUENT); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_OPEN_BORDERS); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_FOUNDER_MASTER_ADOPTED_RELIGION); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_HAPPILY_ADOPTED_RELIGION); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSAL_SAME_STATE_RELIGION); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(MASTER_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(OPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS); // VP - End C4DF Opinion Values
	GD_INT_MEMBER(APPROACH_NEUTRAL_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_NEUTRAL_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_AFRAID_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_AFRAID_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_GUARDED_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_GUARDED_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_DECEPTIVE_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_DECEPTIVE_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_HOSTILE_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_HOSTILE_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_WAR_HUMAN); // VP
	GD_INT_MEMBER(APPROACH_WAR_HUMAN_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_NEUTRAL_AI); // VP
	GD_INT_MEMBER(APPROACH_NEUTRAL_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_AI); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_AFRAID_AI); // VP
	GD_INT_MEMBER(APPROACH_AFRAID_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_GUARDED_AI); // VP
	GD_INT_MEMBER(APPROACH_GUARDED_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_DECEPTIVE_AI); // VP
	GD_INT_MEMBER(APPROACH_DECEPTIVE_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_HOSTILE_AI); // VP
	GD_INT_MEMBER(APPROACH_HOSTILE_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_WAR_AI); // VP
	GD_INT_MEMBER(APPROACH_WAR_AI_PERCENT); // VP
	GD_INT_MEMBER(APPROACH_WAR_PROXIMITY_NEIGHBORS);
	GD_INT_MEMBER(APPROACH_WAR_PROXIMITY_CLOSE);
	GD_INT_MEMBER(APPROACH_WAR_PROXIMITY_FAR);
	GD_INT_MEMBER(APPROACH_WAR_PROXIMITY_DISTANT);
	GD_INT_MEMBER(APPROACH_MULTIPLIER_PROXIMITY_NEIGHBORS); // VP
	GD_INT_MEMBER(APPROACH_MULTIPLIER_PROXIMITY_CLOSE); // VP
	GD_INT_MEMBER(APPROACH_MULTIPLIER_PROXIMITY_FAR); // VP
	GD_INT_MEMBER(APPROACH_MULTIPLIER_PROXIMITY_DISTANT); // VP
	GD_INT_MEMBER(MAJOR_WAR_MULTIPLIER_TARGET_SOFT); // VP
	GD_INT_MEMBER(MAJOR_WAR_MULTIPLIER_TARGET_FAVORABLE); // VP
	GD_INT_MEMBER(MAJOR_WAR_MULTIPLIER_TARGET_AVERAGE); // VP
	GD_INT_MEMBER(MAJOR_WAR_MULTIPLIER_TARGET_BAD); // VP
	GD_INT_MEMBER(MAJOR_WAR_MULTIPLIER_TARGET_IMPOSSIBLE); // VP
	GD_INT_MEMBER(MINOR_APPROACH_WAR_TARGET_SOFT);
	GD_INT_MEMBER(MINOR_APPROACH_WAR_TARGET_FAVORABLE);
	GD_INT_MEMBER(MINOR_APPROACH_WAR_TARGET_AVERAGE);
	GD_INT_MEMBER(MINOR_APPROACH_WAR_TARGET_BAD);
	GD_INT_MEMBER(MINOR_APPROACH_WAR_TARGET_IMPOSSIBLE);
	GD_INT_MEMBER(CONQUEST_WAR_MULTIPLIER_TARGET_SOFT); // VP
	GD_INT_MEMBER(CONQUEST_WAR_MULTIPLIER_TARGET_FAVORABLE); // VP
	GD_INT_MEMBER(CONQUEST_WAR_MULTIPLIER_TARGET_AVERAGE); // VP
	GD_INT_MEMBER(CONQUEST_WAR_MULTIPLIER_TARGET_BAD); // VP
	GD_INT_MEMBER(CONQUEST_WAR_MULTIPLIER_TARGET_IMPOSSIBLE); // VP
	GD_INT_MEMBER(APPROACH_NEUTRAL_DEFAULT);
	GD_INT_MEMBER(APPROACH_BIAS_FOR_CURRENT);
	GD_INT_MEMBER(APPROACH_WAR_CURRENTLY_WAR);
	GD_INT_MEMBER(APPROACH_RANDOM_PERCENT);
	GD_INT_MEMBER(TURNS_SINCE_PEACE_WEIGHT_DAMPENER);
	GD_INT_MEMBER(MINOR_APPROACH_IGNORE_DEFAULT);
	GD_INT_MEMBER(APPROACH_WAR_VASSAL_PEACEFULLY_REVOKED); // VP - Start C4DF Approach Values
	GD_INT_MEMBER(APPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED); // VP
	GD_INT_MEMBER(APPROACH_WAR_VASSAL_FORCEFULLY_REVOKED); // VP
	GD_INT_MEMBER(APPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED); // VP
	GD_INT_MEMBER(APPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED); // VP
	GD_INT_MEMBER(APPROACH_WAR_TOO_MANY_VASSALS); // VP
	GD_INT_MEMBER(APPROACH_GUARDED_TOO_MANY_VASSALS); // VP - End C4DF Approach Values
	GD_INT_MEMBER(CLOSE_TO_DOMINATION_VICTORY_THRESHOLD); // VP
	GD_INT_MEMBER(CLOSE_TO_DIPLOMATIC_VICTORY_THRESHOLD); // VP
	GD_INT_MEMBER(CLOSE_TO_SCIENCE_VICTORY_THRESHOLD); // VP
	GD_INT_MEMBER(CLOSE_TO_CULTURE_VICTORY_THRESHOLD); // VP
	GD_INT_MEMBER(COOP_WAR_DESIRE_THRESHOLD); // VP
	GD_INT_MEMBER(AI_WORLD_LEADER_BASE_WEIGHT_SELF); // VP
	GD_INT_MEMBER(REQUEST_PEACE_TURN_THRESHOLD);
	GD_INT_MEMBER(REQUEST_PEACE_THRESHOLD_REDUCTION_PER_WAR); // VP
	GD_INT_MEMBER(REQUEST_PEACE_LEEWAY_THRESHOLD); // VP
	GD_INT_MEMBER(REQUEST_PEACE_LEEWAY_THRESHOLD_REDUCTION_PER_WAR); // VP
	GD_INT_MEMBER(WAR_MAJOR_MINIMUM_TURNS); // VP
	GD_INT_MEMBER(WAR_MINOR_MINIMUM_TURNS); // VP
	GD_INT_MEMBER(DEFENSIVE_PACT_LIMIT_BASE); // VP
	GD_INT_MEMBER(DEFENSIVE_PACT_LIMIT_SCALER); // VP
	GD_INT_MEMBER(AI_DEFENSIVE_PACT_LIMIT_BASE); // VP
	GD_INT_MEMBER(AI_DEFENSIVE_PACT_LIMIT_SCALER); // VP
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT);
	GD_INT_MEMBER(PEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT);
	GD_INT_MEMBER(PEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE);
	GD_INT_MEMBER(WAR_DAMAGE_LEVEL_CITY_WEIGHT);
	GD_INT_MEMBER(WAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER);
	GD_INT_MEMBER(WAR_DAMAGE_LEVEL_WORLD_WONDER_MULTIPLIER); // VP
	GD_INT_MEMBER(DEFAULT_WAR_VALUE_FOR_UNIT);
	GD_INT_MEMBER(PILLAGED_TILE_BASE_WAR_VALUE); // VP
	GD_INT_MEMBER(STOLEN_TILE_BASE_WAR_VALUE); // VP
	GD_INT_MEMBER(MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE);
	GD_INT_MEMBER(MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH);
	GD_INT_MEMBER(MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM);
	GD_INT_MEMBER(MILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW);
	GD_INT_MEMBER(PLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD);
	GD_INT_MEMBER(PLOT_BUYING_POSTURE_HIGH_THRESHOLD);
	GD_INT_MEMBER(PLOT_BUYING_POSTURE_MEDIUM_THRESHOLD);
	GD_INT_MEMBER(PLOT_BUYING_POSTURE_LOW_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_BASE);
	GD_INT_MEMBER(MILITARY_STRENGTH_RATIO_MULTIPLIER);
	GD_INT_MEMBER(MILITARY_STRENGTH_CITY_MOD);
	GD_INT_MEMBER(MILITARY_STRENGTH_IMMENSE_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_POWERFUL_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_STRONG_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_AVERAGE_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_POOR_THRESHOLD);
	GD_INT_MEMBER(MILITARY_STRENGTH_WEAK_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_RATIO_MULTIPLIER);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_IMMENSE_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_POWERFUL_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_STRONG_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_AVERAGE_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_POOR_THRESHOLD);
	GD_INT_MEMBER(ECONOMIC_STRENGTH_WEAK_THRESHOLD);
	GD_INT_MEMBER(MILITARY_RATING_STARTING_VALUE); // VP
	GD_INT_MEMBER(MILITARY_RATING_HUMAN_BUFFER_VALUE); // VP
	GD_INT_MEMBER(TARGET_IMPOSSIBLE_THRESHOLD);
	GD_INT_MEMBER(TARGET_BAD_THRESHOLD);
	GD_INT_MEMBER(TARGET_AVERAGE_THRESHOLD);
	GD_INT_MEMBER(TARGET_FAVORABLE_THRESHOLD);
	GD_INT_MEMBER(TARGET_ALREADY_WAR_EACH_PLAYER);
	GD_INT_MEMBER(TARGET_NEIGHBORS);
	GD_INT_MEMBER(TARGET_CLOSE);
	GD_INT_MEMBER(TARGET_FAR);
	GD_INT_MEMBER(TARGET_DISTANT);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_PATHETIC);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_WEAK);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_POOR);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_AVERAGE);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_STRONG);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_POWERFUL);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_IMMENSE);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_DISTANT);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_FAR);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_CLOSE);
	GD_INT_MEMBER(TARGET_MINOR_BACKUP_NEIGHBORS);
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_PATHETIC); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_WEAK); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_POOR); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_AVERAGE); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_STRONG); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_POWERFUL); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_IMMENSE); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_DISTANT); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_FAR); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_CLOSE); // VP
	GD_INT_MEMBER(TARGET_MAJOR_BACKUP_NEIGHBORS); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_CRITICAL_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_SEVERE_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_MAJOR_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_MINOR_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_MINOR_ATTACKED_WEIGHT);
	GD_INT_MEMBER(WARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT);
	GD_INT_MEMBER(WARMONGER_THREAT_USED_NUKE_WEIGHT); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_CITY_VALUE_MULTIPLIER); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_CAPITAL_CITY_PERCENT); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_SHARED_FATE_PERCENT); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_LIBERATED_TEAM_BONUS_PERCENT); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_COOP_WAR_PERCENT); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_MODIFIER_LARGE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_MODIFIER_MEDIUM); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_MODIFIER_SMALL); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_MODIFIER_NEGATIVE_SMALL); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_MODIFIER_NEGATIVE_MEDIUM); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_IMMENSE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_POWERFUL); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_STRONG); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_AVERAGE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_POOR); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_WEAK); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKER_STRENGTH_PATHETIC); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_DEFENDER_STRENGTH_IMMENSE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_DEFENDER_STRENGTH_POWERFUL); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_DEFENDER_STRENGTH_STRONG); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_DEFENDER_STRENGTH_AVERAGE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_IMMENSE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_POWERFUL); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_STRONG); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_AVERAGE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_POOR); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_WEAK); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_STRENGTH_DECAY_PATHETIC); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_PER_TURN_DECAY);
	GD_INT_MEMBER(WARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD);
	GD_INT_MEMBER(WARMONGER_THREAT_PER_TURN_DECAY_INCREASED); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_PER_TURN_DECAY_DECREASED); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_WAR); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE); // VP
	GD_INT_MEMBER(WARMONGER_THREAT_ATTACKED_SANCTIONED_PLAYER); // VP
	GD_INT_MEMBER(LAND_DISPUTE_FIERCE_THRESHOLD);
	GD_INT_MEMBER(LAND_DISPUTE_STRONG_THRESHOLD);
	GD_INT_MEMBER(LAND_DISPUTE_WEAK_THRESHOLD);
	GD_INT_MEMBER(VICTORY_DISPUTE_FIERCE_THRESHOLD);
	GD_INT_MEMBER(VICTORY_DISPUTE_STRONG_THRESHOLD);
	GD_INT_MEMBER(VICTORY_DISPUTE_WEAK_THRESHOLD);
	GD_INT_MEMBER(VICTORY_BLOCK_FIERCE_THRESHOLD); // VP
	GD_INT_MEMBER(VICTORY_BLOCK_STRONG_THRESHOLD); // VP
	GD_INT_MEMBER(VICTORY_BLOCK_WEAK_THRESHOLD); // VP
	GD_INT_MEMBER(TECH_BLOCK_FIERCE_THRESHOLD); // VP
	GD_INT_MEMBER(TECH_BLOCK_STRONG_THRESHOLD); // VP
	GD_INT_MEMBER(TECH_BLOCK_WEAK_THRESHOLD); // VP
	GD_INT_MEMBER(POLICY_BLOCK_FIERCE_THRESHOLD); // VP
	GD_INT_MEMBER(POLICY_BLOCK_STRONG_THRESHOLD); // VP
	GD_INT_MEMBER(POLICY_BLOCK_WEAK_THRESHOLD); // VP
	GD_INT_MEMBER(WONDER_DISPUTE_FIERCE_THRESHOLD);
	GD_INT_MEMBER(WONDER_DISPUTE_STRONG_THRESHOLD);
	GD_INT_MEMBER(WONDER_DISPUTE_WEAK_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_DISPUTE_FIERCE_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_DISPUTE_STRONG_THRESHOLD);
	GD_INT_MEMBER(MINOR_CIV_DISPUTE_WEAK_THRESHOLD);
	GD_INT_MEMBER(LAND_DISPUTE_CRAMPED_MULTIPLIER);
	GD_INT_MEMBER(VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE);
	GD_INT_MEMBER(VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY);
	GD_INT_MEMBER(VICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE);
	GD_INT_MEMBER(VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_POSITIVE);
	GD_INT_MEMBER(VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_LIKELY);
	GD_INT_MEMBER(VICTORY_BLOCK_GRAND_STRATEGY_DIFFERENCE_UNSURE);
	GD_INT_MEMBER(COOP_WAR_SOON_COUNTER);
	GD_INT_MEMBER(COOP_WAR_LOCKED_LENGTH);
	GD_INT_MEMBER(DEMAND_TURN_LIMIT_MIN);
	GD_INT_MEMBER(DEMAND_TURN_LIMIT_RAND);
	GD_INT_MEMBER(DONT_SETTLE_FLAVOR_BASE);
	GD_INT_MEMBER(DONT_SETTLE_FLAVOR_MULTIPLIER);
	GD_INT_MEMBER(DONT_SETTLE_FRIENDLY);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_PATHETIC);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_WEAK);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_POOR);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_AVERAGE);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_STRONG);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_POWERFUL);
	GD_INT_MEMBER(DONT_SETTLE_STRENGTH_IMMENSE);
	GD_INT_MEMBER(DONT_SETTLE_MOD_MILITARY_POSTURE_NONE);
	GD_INT_MEMBER(DONT_SETTLE_MOD_MILITARY_POSTURE_LOW);
	GD_INT_MEMBER(DONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM);
	GD_INT_MEMBER(DONT_SETTLE_MOD_MILITARY_POSTURE_HIGH);
	GD_INT_MEMBER(DONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE);
	GD_INT_MEMBER(JUST_MET_TURN_BUFFER); // VP
	GD_INT_MEMBER(DOF_TURN_BUFFER);
	GD_INT_MEMBER(DOF_TURN_BUFFER_REDUCTION_PER_ERA); // VP
	GD_INT_MEMBER(EACH_GOLD_VALUE_PERCENT);
	GD_INT_MEMBER(EACH_GOLD_PER_TURN_VALUE_PERCENT);
	GD_INT_MEMBER(MOVE_TROOPS_MEMORY_TURN_EXPIRATION); // VP
	GD_INT_MEMBER(MILITARY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(MILITARY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(EXPANSION_BICKER_TIMEOUT);
	GD_INT_MEMBER(EXPANSION_BICKER_RANGE_HIGH); // VP
	GD_INT_MEMBER(EXPANSION_BICKER_RANGE_MEDIUM); // VP
	GD_INT_MEMBER(EXPANSION_BICKER_RANGE_LOW); // VP
	GD_INT_MEMBER(EXPANSION_PROMISE_TURNS_EFFECTIVE);
	GD_INT_MEMBER(EXPANSION_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(EXPANSION_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(BORDER_PROMISE_TURNS_EFFECTIVE);
	GD_INT_MEMBER(BORDER_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(BORDER_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(BULLY_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(BULLY_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(ATTACK_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(ATTACK_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(SPY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(SPY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(CONVERT_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(CONVERT_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(DIGGING_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(DIGGING_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(COOP_WAR_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(DOF_BROKEN_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(DOF_BROKEN_TURNS_UNTIL_FORGIVEN_FRIENDS); // VP
	GD_INT_MEMBER(DOF_BROKEN_BACKSTAB_TIMER); // VP
	GD_INT_MEMBER(FRIEND_DENOUNCED_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(FRIEND_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(PLUNDERED_TRADE_ROUTE_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(RETURNED_CIVILIAN_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(BUILT_LANDMARK_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(LIBERATED_CITY_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(FORGAVE_FOR_SPYING_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(SHARED_INTRIGUE_TURNS_UNTIL_FORGOTTEN); // VP
	GD_INT_MEMBER(ROBBED_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(PLOTTED_AGAINST_US_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(BEATEN_TO_WONDER_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(LOWERED_OUR_INFLUENCE_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(PERFORMED_COUP_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(EXCAVATED_ARTIFACT_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(MADE_DEMAND_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(RELIGIOUS_CONVERSION_TURNS_UNTIL_FORGIVEN); // VP
	GD_INT_MEMBER(CAN_WORK_WATER_FROM_GAME_START);
	GD_INT_MEMBER(NAVAL_PLOT_BLOCKADE_RANGE);
	GD_INT_MEMBER(EVENT_MESSAGE_TIME);
	GD_INT_MEMBER(START_YEAR);
	GD_INT_MEMBER(WEEKS_PER_MONTHS);
	GD_INT_MEMBER(HIDDEN_START_TURN_OFFSET);
	GD_INT_MEMBER(RECON_VISIBILITY_RANGE);
	GD_INT_MEMBER(PLOT_VISIBILITY_RANGE);
	GD_INT_MEMBER(UNIT_VISIBILITY_RANGE);
	GD_INT_MEMBER(AIR_UNIT_REBASE_RANGE_MULTIPLIER);
	GD_INT_MEMBER(MOUNTAIN_SEE_FROM_CHANGE);
	GD_INT_MEMBER(MOUNTAIN_SEE_THROUGH_CHANGE);
	GD_INT_MEMBER(HILLS_SEE_FROM_CHANGE);
	GD_INT_MEMBER(HILLS_SEE_THROUGH_CHANGE);
	GD_INT_MEMBER(SEAWATER_SEE_FROM_CHANGE);
	GD_INT_MEMBER(SEAWATER_SEE_THROUGH_CHANGE);
	GD_INT_MEMBER(MOVE_DENOMINATOR);
	GD_INT_MEMBER(STARTING_DISTANCE_PERCENT);
	GD_INT_MEMBER(MIN_CIV_STARTING_DISTANCE);
	GD_INT_MEMBER(MIN_CITY_RANGE);
	GD_INT_MEMBER(CITY_STARTING_RINGS);
	GD_INT_MEMBER(OWNERSHIP_SCORE_DURATION_THRESHOLD);
	GD_INT_MEMBER(NUM_POLICY_BRANCHES_ALLOWED);
	GD_INT_MEMBER(NUM_VICTORY_POINT_AWARDS);
	GD_INT_MEMBER(NUM_OR_TECH_PREREQS);
	GD_INT_MEMBER(NUM_AND_TECH_PREREQS);
	GD_INT_MEMBER(NUM_UNIT_AND_TECH_PREREQS);
	GD_INT_MEMBER(NUM_BUILDING_AND_TECH_PREREQS);
	GD_INT_MEMBER(NUM_BUILDING_RESOURCE_PREREQS);
	GD_INT_MEMBER(MAX_WORLD_WONDERS_PER_CITY);
	GD_INT_MEMBER(MAX_TEAM_WONDERS_PER_CITY);
	GD_INT_MEMBER(MAX_NATIONAL_WONDERS_PER_CITY);
	GD_INT_MEMBER(MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC);
	GD_INT_MEMBER(MAX_BUILDINGS_PER_CITY);
	GD_INT_MEMBER(INITIAL_CITY_POPULATION);
	GD_INT_MEMBER(BASE_CITY_AIR_STACKING);
	GD_INT_MEMBER(CITY_CAPTURE_POPULATION_PERCENT);
	GD_INT_MEMBER(BASE_CITY_GROWTH_THRESHOLD);
	GD_INT_MEMBER(FOOD_CONSUMPTION_PER_POPULATION);
	GD_INT_MEMBER(HAPPINESS_PER_NATURAL_WONDER);
	GD_INT_MEMBER(HAPPINESS_PER_EXTRA_LUXURY);
	GD_INT_MEMBER(UNHAPPINESS_PER_POPULATION);
	GD_INT_MEMBER(UNHAPPINESS_PER_CITY);
	GD_INT_MEMBER(UNHAPPINESS_PER_CAPTURED_CITY);
	GD_INT_MEMBER(UNHAPPY_GROWTH_PENALTY);
	GD_INT_MEMBER(VERY_UNHAPPY_GROWTH_PENALTY);
	GD_INT_MEMBER(VERY_UNHAPPY_PRODUCTION_PENALTY_PER_UNHAPPY);
	GD_INT_MEMBER(VERY_UNHAPPY_MAX_PRODUCTION_PENALTY);
	GD_INT_MEMBER(UNHAPPY_PRODUCTION_PENALTY); // VP
	GD_INT_MEMBER(VERY_UNHAPPY_PRODUCTION_PENALTY); // VP
	GD_INT_MEMBER(VERY_UNHAPPY_GOLD_PENALTY_PER_UNHAPPY);
	GD_INT_MEMBER(VERY_UNHAPPY_MAX_GOLD_PENALTY);
	GD_INT_MEMBER(VERY_UNHAPPY_COMBAT_PENALTY_PER_UNHAPPY);
	GD_INT_MEMBER(VERY_UNHAPPY_MAX_COMBAT_PENALTY);
	GD_INT_MEMBER(VERY_UNHAPPY_CANT_TRAIN_SETTLERS);
	GD_INT_MEMBER(UNHAPPY_THRESHOLD); // VP
	GD_INT_MEMBER(VERY_UNHAPPY_THRESHOLD);
	GD_INT_MEMBER(SUPER_UNHAPPY_THRESHOLD);
	GD_INT_MEMBER(BALANCE_WAR_WEARINESS_POPULATION_CAP); // VP
	GD_INT_MEMBER(UPRISING_COUNTER_MIN);
	GD_INT_MEMBER(UPRISING_COUNTER_POSSIBLE);
	GD_INT_MEMBER(UPRISING_NUM_BASE);
	GD_INT_MEMBER(UPRISING_NUM_CITY_COUNT);
	GD_INT_MEMBER(REVOLT_COUNTER_MIN);
	GD_INT_MEMBER(STRATEGIC_RESOURCE_EXHAUSTED_PENALTY);
	GD_INT_MEMBER(WLTKD_GROWTH_MULTIPLIER);
	GD_INT_MEMBER(INDUSTRIAL_ROUTE_PRODUCTION_MOD);
	GD_INT_MEMBER(RESOURCE_DEMAND_COUNTDOWN_BASE);
	GD_INT_MEMBER(RESOURCE_DEMAND_COUNTDOWN_CAPITAL_ADD);
	GD_INT_MEMBER(RESOURCE_DEMAND_COUNTDOWN_RAND);
	GD_INT_MEMBER(NEW_HURRY_MODIFIER);
	GD_INT_MEMBER(GREAT_GENERAL_RANGE);
	GD_INT_MEMBER(GREAT_GENERAL_STRENGTH_MOD);
	GD_INT_MEMBER(BONUS_PER_ADJACENT_FRIEND);
	GD_INT_MEMBER(POLICY_ATTACK_BONUS_MOD);
	GD_INT_MEMBER(CONSCRIPT_MIN_CITY_POPULATION);
	GD_INT_MEMBER(CONSCRIPT_POPULATION_PER_COST);
	GD_INT_MEMBER(COMBAT_DAMAGE);
	GD_INT_MEMBER(NONCOMBAT_UNIT_RANGED_DAMAGE);
	GD_INT_MEMBER(NAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER);
	GD_INT_MEMBER(INITIAL_GOLD_PER_UNIT_TIMES_100);
	GD_INT_MEMBER(INITIAL_FREE_OUTSIDE_UNITS);
	GD_INT_MEMBER(INITIAL_OUTSIDE_UNIT_GOLD_PERCENT);
	GD_INT_MEMBER(UNIT_MAINTENANCE_GAME_MULTIPLIER);
	GD_INT_MEMBER(UNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR);
	GD_INT_MEMBER(FREE_UNIT_HAPPINESS);
	GD_INT_MEMBER(TRADE_ROUTE_BASE_GOLD);
	GD_INT_MEMBER(TRADE_ROUTE_BASE_TARGET_TURNS); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_LAND_DISTANCE); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_LAND_MODIFIER); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_SEA_DISTANCE); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_SEA_MODIFIER); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_FOOD_VALUE); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_PRODUCTION_VALUE); // VP
	GD_INT_MEMBER(TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CULTURE_DIVISOR_TIMES100); // VP
	GD_INT_MEMBER(TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE); // VP
	GD_INT_MEMBER(TRADE_ROUTE_RIVER_CITY_MODIFIER); // VP
	GD_INT_MEMBER(TRADE_ROUTE_BASE_PLUNDER_GOLD); // VP
	GD_INT_MEMBER(TRADE_ROUTE_PLUNDER_TURNS_COUNTER); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_ALLY_SCIENCE_DELTA); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_FRIEND_SCIENCE_DELTA); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_ALLY_CULTURE_DELTA); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_FRIEND_CULTURE_DELTA); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_ALLY_GOLD); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CS_FRIEND_GOLD); // VP
	GD_INT_MEMBER(CITY_STATE_SCALE_PER_CITY_MOD); // VP
	GD_INT_MEMBER(TRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER);
	GD_INT_MEMBER(TRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER);
	GD_INT_MEMBER(DEFICIT_UNIT_DISBANDING_THRESHOLD);
	GD_INT_MEMBER(GOLDEN_AGE_BASE_THRESHOLD_HAPPINESS);
	GD_INT_MEMBER(GOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS);
	GD_INT_MEMBER(GOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR);
	GD_INT_MEMBER(BASE_GOLDEN_AGE_UNITS);
	GD_INT_MEMBER(GOLDEN_AGE_UNITS_MULTIPLIER);
	GD_INT_MEMBER(GOLDEN_AGE_LENGTH);
	GD_INT_MEMBER(GOLDEN_AGE_GREAT_PEOPLE_MODIFIER);
	GD_INT_MEMBER(MIN_UNIT_GOLDEN_AGE_TURNS);
	GD_INT_MEMBER(GOLDEN_AGE_CULTURE_MODIFIER);
	GD_INT_MEMBER(HILLS_EXTRA_MOVEMENT);
	GD_INT_MEMBER(RIVER_EXTRA_MOVEMENT);
	GD_INT_MEMBER(EXTRA_YIELD);
	GD_INT_MEMBER(FORTIFY_MODIFIER_PER_TURN);
	GD_INT_MEMBER(NUKE_FALLOUT_PROB);
	GD_INT_MEMBER(NUKE_UNIT_DAMAGE_BASE);
	GD_INT_MEMBER(NUKE_UNIT_DAMAGE_RAND_1);
	GD_INT_MEMBER(NUKE_UNIT_DAMAGE_RAND_2);
	GD_INT_MEMBER(NUKE_NON_COMBAT_DEATH_THRESHOLD);
	GD_INT_MEMBER(NUKE_LEVEL1_POPULATION_DEATH_BASE);
	GD_INT_MEMBER(NUKE_LEVEL1_POPULATION_DEATH_RAND_1);
	GD_INT_MEMBER(NUKE_LEVEL1_POPULATION_DEATH_RAND_2);
	GD_INT_MEMBER(NUKE_LEVEL2_POPULATION_DEATH_BASE);
	GD_INT_MEMBER(NUKE_LEVEL2_POPULATION_DEATH_RAND_1);
	GD_INT_MEMBER(NUKE_LEVEL2_POPULATION_DEATH_RAND_2);
	GD_INT_MEMBER(NUKE_LEVEL2_ELIM_POPULATION_THRESHOLD);
	GD_INT_MEMBER(NUKE_CITY_HIT_POINT_DAMAGE);
	GD_INT_MEMBER(NUKE_BLAST_RADIUS);
	GD_INT_MEMBER(TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER);
	GD_INT_MEMBER(TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER);
	GD_INT_MEMBER(TECH_COST_KNOWN_PREREQ_MODIFIER);
	GD_INT_MEMBER(DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT);
	GD_INT_MEMBER(UNIT_PRODUCTION_PERCENT);
	GD_INT_MEMBER(BUILDING_PRODUCTION_PERCENT);
	GD_INT_MEMBER(PROJECT_PRODUCTION_PERCENT);
	GD_INT_MEMBER(PRODUCTION_PENALTY_PER_UNIT_OVER_SUPPLY); // VP
	GD_INT_MEMBER(GROWTH_PENALTY_PER_UNIT_OVER_SUPPLY); // VP
	GD_INT_MEMBER(MAX_UNIT_SUPPLY_PRODMOD);
	GD_INT_MEMBER(MAX_UNIT_SUPPLY_GROWTH_MOD); // VP
	GD_INT_MEMBER(MAXED_UNIT_GOLD_PERCENT);
	GD_INT_MEMBER(MAXED_BUILDING_GOLD_PERCENT);
	GD_INT_MEMBER(MAXED_PROJECT_GOLD_PERCENT);
	GD_INT_MEMBER(CIRCUMNAVIGATE_FREE_MOVES);
	GD_INT_MEMBER(BASE_CAPTURE_GOLD);
	GD_INT_MEMBER(CAPTURE_GOLD_PER_POPULATION);
	GD_INT_MEMBER(CAPTURE_GOLD_RAND1);
	GD_INT_MEMBER(CAPTURE_GOLD_RAND2);
	GD_INT_MEMBER(CAPTURE_GOLD_MAX_TURNS);
	GD_INT_MEMBER(BARBARIAN_CITY_ATTACK_MODIFIER);
	GD_INT_MEMBER(BUILDING_PRODUCTION_DECAY_TIME);
	GD_INT_MEMBER(BUILDING_PRODUCTION_DECAY_PERCENT);
	GD_INT_MEMBER(UNIT_PRODUCTION_DECAY_TIME);
	GD_INT_MEMBER(UNIT_PRODUCTION_DECAY_PERCENT);
	GD_INT_MEMBER(BASE_UNIT_UPGRADE_COST);
	GD_INT_MEMBER(UNIT_UPGRADE_COST_PER_PRODUCTION);
	GD_INT_MEMBER(UNIT_UPGRADE_COST_VISIBLE_DIVISOR);
	GD_INT_MEMBER(UNIT_UPGRADE_COST_DISCOUNT_MAX);
	GD_INT_MEMBER(RESEARCH_AGREEMENT_BOOST_DIVISOR);
	GD_INT_MEMBER(MAX_INTERCEPTION_PROBABILITY);
	GD_INT_MEMBER(MAX_EVASION_PROBABILITY);
	GD_INT_MEMBER(MIN_WATER_SIZE_FOR_OCEAN);
	GD_INT_MEMBER(WATER_POTENTIAL_CITY_WORK_FOR_AREA);
	GD_INT_MEMBER(CITY_MAX_NUM_BUILDINGS);
	GD_INT_MEMBER(CITY_MIN_SIZE_FOR_SETTLERS);
	GD_INT_MEMBER(ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES);
	GD_INT_MEMBER(ADVANCED_START_MAX_UNITS_PER_CITY);
	GD_INT_MEMBER(ADVANCED_START_CITY_COST);
	GD_INT_MEMBER(ADVANCED_START_CITY_COST_INCREASE);
	GD_INT_MEMBER(ADVANCED_START_POPULATION_COST);
	GD_INT_MEMBER(ADVANCED_START_POPULATION_COST_INCREASE);
	GD_INT_MEMBER(ADVANCED_START_VISIBILITY_COST);
	GD_INT_MEMBER(ADVANCED_START_VISIBILITY_COST_INCREASE);
	GD_INT_MEMBER(ADVANCED_START_CITY_PLACEMENT_MAX_RANGE);
	GD_INT_MEMBER(NEW_CITY_BUILDING_VALUE_MODIFIER);
	GD_INT_MEMBER(PATH_DAMAGE_WEIGHT);
	GD_INT_MEMBER(PUPPET_GROWTH_MODIFIER); // VP
	GD_INT_MEMBER(PUPPET_PRODUCTION_MODIFIER); // VP
	GD_INT_MEMBER(PUPPET_SCIENCE_MODIFIER);
	GD_INT_MEMBER(PUPPET_CULTURE_MODIFIER);
	GD_INT_MEMBER(PUPPET_GOLD_MODIFIER);
	GD_INT_MEMBER(PUPPET_FAITH_MODIFIER);
	GD_INT_MEMBER(PUPPET_TOURISM_MODIFIER); // VP
	GD_INT_MEMBER(PUPPET_GOLDEN_AGE_MODIFIER); // VP
	GD_INT_MEMBER(BLOCKADE_GOLD_PENALTY); // VP
	GD_INT_MEMBER(BASE_POLICY_COST);
	GD_INT_MEMBER(POLICY_COST_INCREASE_TO_BE_EXPONENTED);
	GD_INT_MEMBER(POLICY_COST_VISIBLE_DIVISOR);
	GD_INT_MEMBER(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS);
	GD_INT_MEMBER(SWITCH_POLICY_BRANCHES_TENETS_LOST);
	GD_INT_MEMBER(POLICY_COST_DISCOUNT_MAX);
	GD_INT_MEMBER(GOLD_PURCHASE_GOLD_PER_PRODUCTION);
	GD_INT_MEMBER(GOLD_PURCHASE_VISIBLE_DIVISOR);
	GD_INT_MEMBER(PROJECT_PURCHASING_DISABLED);
	GD_INT_MEMBER(INFLUENCE_MOUNTAIN_COST);
	GD_INT_MEMBER(INFLUENCE_HILL_COST);
	GD_INT_MEMBER(INFLUENCE_RIVER_COST);
	GD_INT_MEMBER(NUM_RESOURCE_QUANTITY_TYPES);
	GD_INT_MEMBER(SCIENCE_PER_POPULATION);
	GD_INT_MEMBER(RESEARCH_AGREEMENT_MOD);
	GD_INT_MEMBER(BARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD);
	GD_INT_MEMBER(BARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING);
	GD_INT_MEMBER(BARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL);
	GD_INT_MEMBER(BARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP);
	GD_INT_MEMBER(BARBARIAN_CAMP_COASTAL_SPAWN_ROLL);
	GD_INT_MEMBER(BARBARIAN_NAVAL_UNIT_START_TURN_SPAWN);
	GD_INT_MEMBER(MAX_BARBARIANS_FROM_CAMP_NEARBY);
	GD_INT_MEMBER(MAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE);
	GD_INT_MEMBER(GOLD_FROM_BARBARIAN_CONVERSION);
	GD_INT_MEMBER(BARBARIAN_CITY_GOLD_RANSOM);
	GD_INT_MEMBER(BARBARIAN_UNIT_GOLD_RANSOM);
	GD_INT_MEMBER(EMBARKED_UNIT_MOVEMENT);
	GD_INT_MEMBER(EMBARKED_VISIBILITY_RANGE);
	GD_INT_MEMBER(DEFAULT_MAX_NUM_BUILDERS);
	GD_INT_MEMBER(BARBARIAN_TECH_PERCENT);
	GD_INT_MEMBER(CITY_RESOURCE_WLTKD_TURNS);
	GD_INT_MEMBER(MAX_SPECIALISTS_FROM_BUILDING);
	GD_INT_MEMBER(GREAT_PERSON_THRESHOLD_BASE);
	GD_INT_MEMBER(GREAT_PERSON_THRESHOLD_INCREASE);
	GD_INT_MEMBER(CULTURE_BOMB_COOLDOWN);
	GD_INT_MEMBER(CULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE);
	GD_INT_MEMBER(LANDMARK_MINOR_FRIENDSHIP_CHANGE);
	GD_INT_MEMBER(UNIT_AUTO_EXPLORE_DISABLED);
	GD_INT_MEMBER(UNIT_AUTO_EXPLORE_FULL_DISABLED);
	GD_INT_MEMBER(UNIT_WORKER_AUTOMATION_DISABLED);
	GD_INT_MEMBER(UNIT_DELETE_DISABLED);
	GD_INT_MEMBER(MIN_START_AREA_TILES);
	GD_INT_MEMBER(MIN_DISTANCE_OTHER_AREA_PERCENT);
	GD_INT_MEMBER(MINOR_CIV_FOOD_REQUIREMENT);
	GD_INT_MEMBER(MAJOR_CIV_FOOD_REQUIREMENT);
	GD_INT_MEMBER(MIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST);
	GD_INT_MEMBER(START_AREA_FOOD_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_HAPPINESS_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_PRODUCTION_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_GOLD_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_SCIENCE_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_FAITH_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_RESOURCE_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_STRATEGIC_MULTIPLIER);
	GD_INT_MEMBER(START_AREA_BUILD_ON_COAST_PERCENT);
	GD_INT_MEMBER(SETTLER_FOOD_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_PRODUCTION_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_GOLD_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_SCIENCE_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_FAITH_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_STRATEGIC_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_HAPPINESS_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_RESOURCE_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_BUILD_ON_COAST_PERCENT);
	GD_INT_MEMBER(CITY_RING_1_MULTIPLIER);
	GD_INT_MEMBER(CITY_RING_2_MULTIPLIER);
	GD_INT_MEMBER(CITY_RING_3_MULTIPLIER);
	GD_INT_MEMBER(CITY_RING_4_MULTIPLIER);
	GD_INT_MEMBER(CITY_RING_5_MULTIPLIER);
	GD_INT_MEMBER(SETTLER_EVALUATION_DISTANCE);
	GD_INT_MEMBER(SETTLER_DISTANCE_DROPOFF_MODIFIER);
	GD_INT_MEMBER(BUILD_ON_RESOURCE_PERCENT);
	GD_INT_MEMBER(BUILD_ON_RIVER_PERCENT);
	GD_INT_MEMBER(CHOKEPOINT_STRATEGIC_VALUE);
	GD_INT_MEMBER(HILL_STRATEGIC_VALUE);
	GD_INT_MEMBER(BALANCE_BAD_TILES_STRATEGIC_VALUE); // VP
	GD_INT_MEMBER(BALANCE_FRESH_WATER_STRATEGIC_VALUE); // VP
	GD_INT_MEMBER(BALANCE_COAST_STRATEGIC_VALUE); // VP
	GD_INT_MEMBER(BALANCE_EMPIRE_BORDERLAND_STRATEGIC_VALUE); // VP
	GD_INT_MEMBER(MINOR_CIV_CONTACT_GOLD_FIRST);
	GD_INT_MEMBER(MINOR_CIV_CONTACT_GOLD_OTHER);
	GD_INT_MEMBER(COMBAT_AI_OFFENSE_DAMAGEWEIGHT); // VP; also for defense in fact
	GD_INT_MEMBER(COMBAT_AI_OFFENSE_DANGERWEIGHT); // VP
	GD_INT_MEMBER(COMBAT_AI_OFFENSE_SCORE_BIAS); // VP
	GD_INT_MEMBER(COMBAT_AI_DEFENSE_SCORE_BIAS); // VP
	GD_INT_MEMBER(MAJORS_CAN_MOVE_STARTING_SETTLER); // VP
	GD_INT_MEMBER(CS_CAN_MOVE_STARTING_SETTLER); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_VICTORY_COMPETITION); // VP - Start DiploAIOptions
	GD_INT_MEMBER(DIPLOAI_DISABLE_ENDGAME_AGGRESSION); // VP
	GD_INT_MEMBER(DIPLOAI_ENABLE_NUCLEAR_GANDHI); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_WAR_BRIBES); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_CITY_TRADING); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_VOLUNTARY_VASSALAGE); // VP
	GD_INT_MEMBER(DIPLOAI_NO_OTHER_HOST_VOTES); // VP
	GD_INT_MEMBER(DIPLOAI_NO_OTHER_WORLD_LEADER_VOTES); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_INSULT_MESSAGES); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_COMPLIMENT_MESSAGES); // VP
	GD_INT_MEMBER(DIPLOAI_NO_FAKE_OPINION_MODIFIERS); // VP
	GD_INT_MEMBER(DIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS); // VP
	GD_INT_MEMBER(DIPLOAI_SHOW_ALL_OPINION_VALUES); // VP
	GD_INT_MEMBER(DIPLOAI_SHOW_BASE_HUMAN_OPINION); // VP
	GD_INT_MEMBER(DIPLOAI_HIDE_OPINION_TABLE); // VP
	GD_INT_MEMBER(DIPLOAI_NO_FLAVOR_RANDOMIZATION); // VP
	GD_INT_MEMBER(DIPLOAI_ENABLE_LUMP_GOLD_TRADES); // VP
	GD_INT_MEMBER(DIPLOAI_THIRD_PARTY_PEACE_WARSCORE); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_FRIENDSHIP_REQUESTS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_GIFT_OFFERS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_COOP_WAR_REQUESTS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_HELP_REQUESTS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_TRADE_OFFERS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_PEACE_OFFERS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_DEMANDS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_INDEPENDENCE_REQUESTS); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_ALL_STATEMENTS); // VP
	GD_INT_MEMBER(DIPLOAI_PASSIVE_MODE); // VP
	GD_INT_MEMBER(DIPLOAI_AGGRESSIVE_MODE); // VP
	GD_INT_MEMBER(DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION); // VP
	GD_INT_MEMBER(DIPLOAI_ENABLE_DEBUG_MODE); // VP - End DiploAIOptions
	GD_INT_MEMBER(WAR_PROGRESS_INITIAL_VALUE); // VP
	GD_INT_MEMBER(WAR_PROGRESS_DECAY_VS_STRONGER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_DECAY_VS_EQUAL); // VP
	GD_INT_MEMBER(WAR_PROGRESS_DECAY_VS_WEAKER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_CAPTURED_WORKER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_CAPTURED_SETTLER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_PLUNDERED_TRADE_ROUTE); // VP
	GD_INT_MEMBER(WAR_PROGRESS_PILLAGED_IMPROVEMENT); // VP
	GD_INT_MEMBER(WAR_PROGRESS_STOLE_TILE); // VP
	GD_INT_MEMBER(WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_KILLED_UNIT); // VP
	GD_INT_MEMBER(WAR_PROGRESS_CAPTURED_CITY); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_WORKER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_SETTLER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_TRADE_ROUTE); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_IMPROVEMENT); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_TILE); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_UNIT); // VP
	GD_INT_MEMBER(WAR_PROGRESS_LOST_CITY); // VP
	GD_INT_MEMBER(WAR_PROGRESS_PER_STRATEGIC_DEFICIT); // VP
	GD_INT_MEMBER(WAR_PROGRESS_PER_UNHAPPY); // VP
	GD_INT_MEMBER(WAR_PROGRESS_CAPITAL_MULTIPLIER); // VP
	GD_INT_MEMBER(WAR_PROGRESS_HOLY_CITY_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_FRIENDSHIP); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_CULTURE); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_FAITH); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_GOLD); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_FOOD); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_BONUS_UNIT); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_XP_PER_ERA); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_XP_RANDOM); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_PLAYER_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_PLAYER_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_MULTIPLIER); // VP
	GD_INT_MEMBER(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_DIVISOR); // VP
	GD_INT_MEMBER(MINOR_CIV_GROWTH_PERCENT);
	GD_INT_MEMBER(MINOR_CIV_PRODUCTION_PERCENT);
	GD_INT_MEMBER(MINOR_CIV_GOLD_PERCENT);
	GD_INT_MEMBER(MINOR_CIV_TECH_PERCENT);
	GD_INT_MEMBER(MINOR_POLICY_RESOURCE_MULTIPLIER);
	GD_INT_MEMBER(MINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER);
	GD_INT_MEMBER(MINOR_GOLD_GIFT_LARGE);
	GD_INT_MEMBER(MINOR_GOLD_GIFT_MEDIUM);
	GD_INT_MEMBER(MINOR_GOLD_GIFT_SMALL);
	GD_INT_MEMBER(MINOR_CIV_TILE_IMPROVEMENT_GIFT_COST);
	GD_INT_MEMBER(MINOR_CIV_BUYOUT_COST);
	GD_INT_MEMBER(MINOR_CIV_BUYOUT_TURNS);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_FROM_TRADE_MISSION);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_ANCHOR_DEFAULT);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED);
	GD_INT_MEMBER(MINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF);
	GD_INT_MEMBER(MINOR_UNIT_GIFT_TRAVEL_TURNS);
	GD_INT_MEMBER(PLOT_UNIT_LIMIT);
	GD_INT_MEMBER(CITY_UNIT_LIMIT); // VP
	GD_INT_MEMBER(ZONE_OF_CONTROL_ENABLED);
	GD_INT_MEMBER(FIRE_SUPPORT_DISABLED);
	GD_INT_MEMBER(MAX_HIT_POINTS);
	GD_INT_MEMBER(MAX_CITY_HIT_POINTS);
	GD_INT_MEMBER(CITY_HIT_POINTS_HEALED_PER_TURN);
	GD_INT_MEMBER(HILLS_EXTRA_DEFENSE);
	GD_INT_MEMBER(RIVER_ATTACK_MODIFIER);
	GD_INT_MEMBER(AMPHIB_ATTACK_MODIFIER);
	GD_INT_MEMBER(ENEMY_HEAL_RATE);
	GD_INT_MEMBER(NEUTRAL_HEAL_RATE);
	GD_INT_MEMBER(FRIENDLY_HEAL_RATE);
	GD_INT_MEMBER(CITY_HEAL_RATE);
	GD_INT_MEMBER(INSTA_HEAL_RATE);
	GD_INT_MEMBER(BALANCE_BARBARIAN_HEAL_RATE);
	GD_INT_MEMBER(ATTACK_SAME_STRENGTH_MIN_DAMAGE);
	GD_INT_MEMBER(ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE);
	GD_INT_MEMBER(RANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE);
	GD_INT_MEMBER(RANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE);
	GD_INT_MEMBER(INTERCEPTION_SAME_STRENGTH_MIN_DAMAGE);
	GD_INT_MEMBER(INTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE);
	GD_INT_MEMBER(AIR_SWEEP_INTERCEPTION_DAMAGE_MOD);
	GD_INT_MEMBER(WOUNDED_DAMAGE_MULTIPLIER);
	GD_INT_MEMBER(TRAIT_WOUNDED_DAMAGE_MOD);
	GD_INT_MEMBER(CITY_STRENGTH_DEFAULT);
	GD_INT_MEMBER(CITY_STRENGTH_POPULATION_CHANGE);
	GD_INT_MEMBER(CITY_STRENGTH_TECH_MULTIPLIER);
	GD_INT_MEMBER(CITY_STRENGTH_UNIT_DIVISOR);
	GD_INT_MEMBER(CITY_STRENGTH_HILL_CHANGE);
	GD_INT_MEMBER(CITY_ATTACKING_DAMAGE_MOD);
	GD_INT_MEMBER(ATTACKING_CITY_MELEE_DAMAGE_MOD);
	GD_INT_MEMBER(MAX_CITY_ATTACK_RANGE); // VP
	GD_INT_MEMBER(CITY_ATTACK_RANGE);
	GD_INT_MEMBER(CAN_CITY_USE_INDIRECT_FIRE);
	GD_INT_MEMBER(CITY_RANGED_ATTACK_STRENGTH_MULTIPLIER);
	GD_INT_MEMBER(CITY_CAPTURE_DAMAGE_PERCENT);
	GD_INT_MEMBER(EXPERIENCE_PER_LEVEL);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_UNIT_MELEE);
	GD_INT_MEMBER(EXPERIENCE_DEFENDING_UNIT_MELEE);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_UNIT_AIR);
	GD_INT_MEMBER(EXPERIENCE_DEFENDING_UNIT_AIR);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_UNIT_RANGED);
	GD_INT_MEMBER(EXPERIENCE_DEFENDING_UNIT_RANGED);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_AIR_SWEEP);
	GD_INT_MEMBER(EXPERIENCE_DEFENDING_AIR_SWEEP_AIR);
	GD_INT_MEMBER(EXPERIENCE_DEFENDING_AIR_SWEEP_GROUND);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_CITY_MELEE);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_CITY_RANGED);
	GD_INT_MEMBER(EXPERIENCE_ATTACKING_CITY_AIR);
	GD_INT_MEMBER(BARBARIAN_MAX_XP_VALUE);
	GD_INT_MEMBER(MINOR_MAX_XP_VALUE); // VP
	GD_INT_MEMBER(COMBAT_EXPERIENCE_IN_BORDERS_PERCENT);
	GD_INT_MEMBER(GREAT_GENERALS_THRESHOLD);
	GD_INT_MEMBER(GREAT_GENERALS_THRESHOLD_INCREASE);
	GD_INT_MEMBER(GREAT_GENERALS_THRESHOLD_INCREASE_TEAM);
	GD_INT_MEMBER(UNIT_DEATH_XP_GREAT_GENERAL_LOSS);
	GD_INT_MEMBER(CRAMPED_RANGE_FROM_CITY);
	GD_INT_MEMBER(CRAMPED_USABLE_PLOT_PERCENT);
	GD_INT_MEMBER(PROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT);
	GD_INT_MEMBER(PROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY);
	GD_INT_MEMBER(PROXIMITY_FAR_DISTANCE_MAX);
	GD_INT_MEMBER(PLOT_BASE_COST);
	GD_INT_MEMBER(PLOT_ADDITIONAL_COST_PER_PLOT);
	GD_INT_MEMBER(PLOT_COST_APPEARANCE_DIVISOR);
	GD_INT_MEMBER(CULTURE_COST_FIRST_PLOT);
	GD_INT_MEMBER(CULTURE_COST_LATER_PLOT_MULTIPLIER);
	GD_INT_MEMBER(CULTURE_COST_VISIBLE_DIVISOR);
	GD_INT_MEMBER(CULTURE_PLOT_COST_MOD_MINIMUM);
	GD_INT_MEMBER(MINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER);
	GD_INT_MEMBER(MAXIMUM_WORK_PLOT_DISTANCE); // VP
	GD_INT_MEMBER(MAXIMUM_BUY_PLOT_DISTANCE);
	GD_INT_MEMBER(MAXIMUM_ACQUIRE_PLOT_DISTANCE);
	GD_INT_MEMBER(PLOT_INFLUENCE_BASE_MULTIPLIER);
	GD_INT_MEMBER(PLOT_INFLUENCE_DISTANCE_MULTIPLIER);
	GD_INT_MEMBER(PLOT_INFLUENCE_DISTANCE_DIVISOR);
	GD_INT_MEMBER(PLOT_INFLUENCE_RING_COST);
	GD_INT_MEMBER(PLOT_INFLUENCE_WATER_COST);
	GD_INT_MEMBER(PLOT_INFLUENCE_RESOURCE_COST);
	GD_INT_MEMBER(PLOT_INFLUENCE_NW_COST);
	GD_INT_MEMBER(PLOT_BUY_RESOURCE_COST);
	GD_INT_MEMBER(PLOT_INFLUENCE_YIELD_POINT_COST);
	GD_INT_MEMBER(PLOT_INFLUENCE_NO_ADJACENT_OWNED_COST);
	GD_INT_MEMBER(OWN_UNITED_NATIONS_VOTE_BONUS);
	GD_INT_MEMBER(SCORE_CITY_MULTIPLIER);
	GD_INT_MEMBER(SCORE_POPULATION_MULTIPLIER);
	GD_INT_MEMBER(SCORE_LAND_MULTIPLIER);
	GD_INT_MEMBER(SCORE_WONDER_MULTIPLIER);
	GD_INT_MEMBER(SCORE_TECH_MULTIPLIER);
	GD_INT_MEMBER(SCORE_FUTURE_TECH_MULTIPLIER);
	GD_INT_MEMBER(SCORE_POLICY_MULTIPLIER);
	GD_INT_MEMBER(SCORE_GREAT_WORK_MULTIPLIER);
	GD_INT_MEMBER(SCORE_BELIEF_MULTIPLIER);
	GD_INT_MEMBER(SCORE_RELIGION_CITIES_MULTIPLIER);
	GD_INT_MEMBER(MIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY);
	GD_INT_MEMBER(ZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS);
	GD_INT_MEMBER(ZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS);
	GD_INT_MEMBER(ZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS);
	GD_INT_MEMBER(MAX_CITY_DIST_HIGHWATER_MARK);
	GD_INT_MEMBER(HEAVY_RESOURCE_THRESHOLD);
	GD_INT_MEMBER(PROGRESS_POPUP_TURN_FREQUENCY);
	GD_INT_MEMBER(SETTLER_PRODUCTION_SPEED);
	GD_INT_MEMBER(BUY_PLOTS_DISABLED);
	GD_INT_MEMBER(WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT);
	GD_INT_MEMBER(BUILDING_SALE_DIVISOR);
	GD_INT_MEMBER(DISBAND_UNIT_REFUND_PERCENT);
	GD_INT_MEMBER(MINOR_CIV_ROUTE_QUEST_WEIGHT);
	GD_INT_MEMBER(WITHDRAW_MOD_ENEMY_MOVES);
	GD_INT_MEMBER(WITHDRAW_MOD_BLOCKED_TILE);
	GD_INT_MEMBER(AI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT);
	GD_INT_MEMBER(AI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY);
	GD_INT_MEMBER(AI_GOLD_TREASURY_BUFFER); // VP
	GD_INT_MEMBER(BALANCE_CS_WAR_COOLDOWN_RATE); // VP
	GD_INT_MEMBER(BALANCE_FOLLOWER_GROWTH_BONUS); // VP
	GD_INT_MEMBER(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL); // VP
	GD_INT_MEMBER(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE); // VP
	GD_INT_MEMBER(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL); // VP
	GD_INT_MEMBER(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL); // VP
	GD_INT_MEMBER(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT); // VP
	GD_INT_MEMBER(ALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL); // VP
	GD_INT_MEMBER(ALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE); // VP
	GD_INT_MEMBER(ALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL); // VP
	GD_INT_MEMBER(ALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL); // VP
	GD_INT_MEMBER(ALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT); // VP
	GD_INT_MEMBER(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL); // VP
	GD_INT_MEMBER(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE); // VP
	GD_INT_MEMBER(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL); // VP
	GD_INT_MEMBER(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL); // VP
	GD_INT_MEMBER(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT); // VP
	GD_INT_MEMBER(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL); // VP
	GD_INT_MEMBER(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE); // VP
	GD_INT_MEMBER(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL); // VP
	GD_INT_MEMBER(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL); // VP
	GD_INT_MEMBER(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT); // VP
	GD_INT_MEMBER(BALANCE_CORE_MUSICIAN_BLAST_HAPPINESS); // VP
	GD_INT_MEMBER(BALANCE_MAX_CS_ALLY_STRENGTH); // VP
	GD_INT_MEMBER(BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS); // VP
	GD_INT_MEMBER(BALANCE_CS_PLEDGE_TO_PROTECT_DEFENSE_BONUS_MAX); // VP
	GD_INT_MEMBER(BALANCE_CS_ALLIANCE_DEFENSE_BONUS); // VP
	GD_INT_MEMBER(UNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED); // VP
	GD_INT_MEMBER(BALANCE_MARRIAGE_GP_RATE); // VP
	GD_INT_MEMBER(BALANCE_SPY_RESPAWN_TIMER); // VP
	GD_INT_MEMBER(BALANCE_SPY_SABOTAGE_RATE); // VP
	GD_INT_MEMBER(BALANCE_SPY_TO_MINOR_RATIO); // VP
	GD_INT_MEMBER(BALANCE_SPY_BOOST_INFLUENCE_EXOTIC); // VP
	GD_INT_MEMBER(BALANCE_SPY_BOOST_INFLUENCE_FAMILIAR); // VP
	GD_INT_MEMBER(BALANCE_SPY_BOOST_INFLUENCE_POPULAR); // VP
	GD_INT_MEMBER(BALANCE_SPY_BOOST_INFLUENCE_INFLUENTIAL); // VP
	GD_INT_MEMBER(BALANCE_SPY_BOOST_INFLUENCE_DOMINANT); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_PENALTY_MAXIMUM); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_PRODUCTION_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_FOOD_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPINESS_FOOD_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_BASIC_ATTACK_ARMY_SIZE); // VP
	GD_INT_MEMBER(BALANCE_ARMY_NAVY_START_SIZE); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPINESS_PER_SPECIALIST); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_LUXURY_POP_SCALER); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_POP_MULTIPLIER); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_EMPIRE_MULTIPLIER); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPY_CITY_BASE_VALUE_DISTRESS); // VP
	GD_INT_MEMBER(BALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY); // VP
	GD_INT_MEMBER(BALANCE_FAITH_PERCENTAGE_VALUE); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_PERCENTAGE_VALUE); // VP
	GD_INT_MEMBER(BALANCE_GA_PERCENTAGE_VALUE); // VP
	GD_INT_MEMBER(BALANCE_CULTURE_PERCENTAGE_VALUE); // VP
	GD_INT_MEMBER(BALANCE_WONDER_BEATEN_CONSOLATION_PRIZE); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL); // VP
	GD_INT_MEMBER(BALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT); // VP
	GD_INT_MEMBER(BALANCE_CONQUEST_REDUCTION_BOOST); // VP
	GD_INT_MEMBER(BALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC); // VP
	GD_INT_MEMBER(BALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR); // VP
	GD_INT_MEMBER(BALANCE_GOLD_INFLUENCE_LEVEL_POPULAR); // VP
	GD_INT_MEMBER(BALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL); // VP
	GD_INT_MEMBER(BALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT); // VP
	GD_INT_MEMBER(BALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC); // VP
	GD_INT_MEMBER(BALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR); // VP
	GD_INT_MEMBER(BALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR); // VP
	GD_INT_MEMBER(BALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL); // VP
	GD_INT_MEMBER(BALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_CAPITAL_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_HAPPINESS_TECH_BASE_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_INFLUENCE_BOOST_PROTECTION_MINOR); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FRIENDLY); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_HOSTILE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GOLD_MERCANTILE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GOLD_MARITIME); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_SCIENCE_RELIGIOUS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_CULTURE_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_CULTURE_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_CULTURE_IRRATIONAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FAITH_RELIGIOUS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FAITH_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GAP_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GAP_RELIGIOUS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GAP_IRRATIONAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FOOD_MARITIME); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FOOD_MERCANTILE); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_FOOD_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_TOURISM_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_TOURISM_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_TOURISM_IRRATIONAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_IRRATIONAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_RELIGIOUS); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_NEUTRAL); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_MARITIME); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_CULTURED); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC); // VP
	GD_INT_MEMBER(MINOR_CIV_QUEST_REWARD_EXPERIENCE_RELIGIOUS); // VP
	GD_INT_MEMBER(BALANCE_MOD_POLICY_BRANCHES_NEEDED_IDEOLOGY); // VP
	GD_INT_MEMBER(BALANCE_MOD_POLICIES_NEEDED_IDEOLOGY); // VP
	GD_INT_MEMBER(BALANCE_MINOR_PROTECTION_MINIMUM_DURATION); // VP
	GD_INT_MEMBER(BALANCE_SCOUT_XP_BASE); // VP
	GD_INT_MEMBER(BALANCE_SCOUT_XP_RANDOM_VALUE); // VP
	GD_INT_MEMBER(BALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER); // VP
	GD_INT_MEMBER(BALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER); // VP
	GD_INT_MEMBER(GWAM_THRESHOLD_DECREASE); // VP
	GD_INT_MEMBER(BALANCE_BUILDING_INVESTMENT_BASELINE); // VP
	GD_INT_MEMBER(BALANCE_UNIT_INVESTMENT_BASELINE); // VP
	GD_INT_MEMBER(OPEN_BORDERS_MODIFIER_TRADE_GOLD); // VP
	GD_INT_MEMBER(MOD_BALANCE_CORE_MINIMUM_RANKING_PTP); // VP
	GD_INT_MEMBER(BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT); // VP
	GD_INT_MEMBER(COMBAT_CAPTURE_HEALTH);
	GD_INT_MEMBER(COMBAT_CAPTURE_MIN_CHANCE);
	GD_INT_MEMBER(COMBAT_CAPTURE_MAX_CHANCE);
	GD_INT_MEMBER(COMBAT_CAPTURE_RATIO_MULTIPLIER);
	GD_INT_MEMBER(RELIGION_MIN_FAITH_FIRST_PANTHEON);
	GD_INT_MEMBER(RELIGION_MIN_FAITH_FIRST_PROPHET);
	GD_INT_MEMBER(RELIGION_MIN_FAITH_FIRST_GREAT_PERSON);
	GD_INT_MEMBER(RELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON);
	GD_INT_MEMBER(RELIGION_MIN_FAITH_SECOND_PROPHET); // VP
	GD_INT_MEMBER(RELIGION_FAITH_DELTA_NEXT_PROPHET);
	GD_INT_MEMBER(RELIGION_FAITH_DELTA_NEXT_GREAT_PERSON);
	GD_INT_MEMBER(RELIGION_BASE_CHANCE_PROPHET_SPAWN);
	GD_INT_MEMBER(RELIGION_ATHEISM_PRESSURE_PER_POP);
	GD_INT_MEMBER(RELIGION_INITIAL_FOUNDING_CITY_PRESSURE);
	GD_INT_MEMBER(RELIGION_PER_TURN_FOUNDING_CITY_PRESSURE);
	GD_INT_MEMBER(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER);
	GD_INT_MEMBER(RELIGION_ADJACENT_CITY_DISTANCE);
	GD_INT_MEMBER(ESPIONAGE_GATHERING_INTEL_COST_DIVISOR); // VP
	GD_INT_MEMBER(ESPIONAGE_SPY_RESISTANCE_MAXIMUM);
	GD_INT_MEMBER(ESPIONAGE_GATHERING_INTEL_COST_PERCENT);
	GD_INT_MEMBER(ESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT);
	GD_INT_MEMBER(ESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT);
	GD_INT_MEMBER(ESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS);
	GD_INT_MEMBER(ESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION);
	GD_INT_MEMBER(ESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION);
	GD_INT_MEMBER(ESPIONAGE_SURVEILLANCE_SIGHT_RANGE);
	GD_INT_MEMBER(ESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP);
	GD_INT_MEMBER(ESPIONAGE_COUP_MULTIPLY_CONSTANT);
	GD_INT_MEMBER(ESPIONAGE_SPY_EXPERIENCE_DENOMINATOR); // VP
	GD_INT_MEMBER(ESPIONAGE_DEFENDING_SPY_EXPERIENCE); // VP
	GD_INT_MEMBER(ESPIONAGE_DIPLOMAT_SPY_EXPERIENCE); // VP
	GD_INT_MEMBER(ESPIONAGE_OFFENSIVE_SPY_EXPERIENCE); // VP
	GD_INT_MEMBER(INTERNATIONAL_TRADE_BASE);
	GD_INT_MEMBER(INTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION);
	GD_INT_MEMBER(INTERNATIONAL_TRADE_CITY_GPT_DIVISOR);
	GD_INT_MEMBER(PILLAGE_HEAL_AMOUNT);
	GD_INT_MEMBER(CITY_CONNECTIONS_CONNECT_TO_CAPITAL);
	GD_INT_MEMBER(BASE_CULTURE_PER_GREAT_WORK);
	GD_INT_MEMBER(BASE_TOURISM_PER_GREAT_WORK);
	GD_INT_MEMBER(TOURISM_MODIFIER_SHARED_RELIGION);
	GD_INT_MEMBER(TOURISM_MODIFIER_TRADE_ROUTE);
	GD_INT_MEMBER(TOURISM_MODIFIER_OPEN_BORDERS);
	GD_INT_MEMBER(TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES);
	GD_INT_MEMBER(TOURISM_MODIFIER_DIPLOMAT);
	GD_INT_MEMBER(MINIMUM_TOURISM_BLAST_STRENGTH);
	GD_INT_MEMBER(CULTURE_LEVEL_EXOTIC);
	GD_INT_MEMBER(CULTURE_LEVEL_FAMILIAR);
	GD_INT_MEMBER(CULTURE_LEVEL_POPULAR);
	GD_INT_MEMBER(CULTURE_LEVEL_INFLUENTIAL);
	GD_INT_MEMBER(CULTURE_LEVEL_DOMINANT);
	GD_INT_MEMBER(IDEOLOGY_PERCENT_CLEAR_VICTORY_PREF);
	GD_INT_MEMBER(IDEOLOGY_SCORE_GRAND_STRATS);
	GD_INT_MEMBER(IDEOLOGY_SCORE_HAPPINESS);
	GD_INT_MEMBER(IDEOLOGY_SCORE_PER_FREE_TENET);
	GD_INT_MEMBER(IDEOLOGY_SCORE_HOSTILE);
	GD_INT_MEMBER(IDEOLOGY_SCORE_GUARDED);
	GD_INT_MEMBER(IDEOLOGY_SCORE_AFRAID);
	GD_INT_MEMBER(IDEOLOGY_SCORE_FRIENDLY);
	GD_INT_MEMBER(MIN_DIG_SITES_PER_MAJOR_CIV);
	GD_INT_MEMBER(MAX_DIG_SITES_PER_MAJOR_CIV);
	GD_INT_MEMBER(PERCENT_SITES_HIDDEN);
	GD_INT_MEMBER(PERCENT_HIDDEN_SITES_WRITING);
	GD_INT_MEMBER(SAPPED_CITY_ATTACK_MODIFIER);
	GD_INT_MEMBER(BLOCKADED_CITY_ATTACK_MODIFIER); // VP
	GD_INT_MEMBER(EXOTIC_GOODS_GOLD_MIN);
	GD_INT_MEMBER(EXOTIC_GOODS_GOLD_MAX);
	GD_INT_MEMBER(EXOTIC_GOODS_XP_MIN);
	GD_INT_MEMBER(EXOTIC_GOODS_XP_MAX);
	GD_INT_MEMBER(TEMPORARY_CULTURE_BOOST_MOD);
	GD_INT_MEMBER(TEMPORARY_TOURISM_BOOST_MOD);
	GD_INT_MEMBER(LEAGUE_SESSION_INTERVAL_BASE_TURNS);
	GD_INT_MEMBER(LEAGUE_SESSION_SOON_WARNING_TURNS);
	GD_INT_MEMBER(LEAGUE_MEMBER_PROPOSALS_BASE);
	GD_INT_MEMBER(LEAGUE_MEMBER_VOTES_BASE);
	GD_INT_MEMBER(LEAGUE_PROPOSERS_PER_SESSION); // VP
	GD_INT_MEMBER(LEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES); // VP
	GD_INT_MEMBER(LEAGUE_PROJECT_PROGRESS_PERCENT_WARNING); // VP
	GD_INT_MEMBER(UNHAPPINESS_MEDIANS_MIN_POP_REQUIREMENT); // VP
	GD_INT_MEMBER(HELP_REQUEST_TURN_LIMIT_MIN); // VP - Start C4DF Other Values
	GD_INT_MEMBER(HELP_REQUEST_TURN_LIMIT_RAND); // VP
	GD_INT_MEMBER(SHARE_OPINION_TURN_BUFFER); // VP
	GD_INT_MEMBER(VASSAL_TOURISM_MODIFIER); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT); // VP
	GD_INT_MEMBER(VASSAL_SCORE_PERCENT); // VP
	GD_INT_MEMBER(VASSAL_HAPPINESS_PERCENT); // VP
	GD_INT_MEMBER(VASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_LOST_CITIES_THRESHOLD); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_POPULATION_THRESHOLD); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD); // VP
	GD_INT_MEMBER(VASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD); // VP
	GD_INT_MEMBER(VASSALAGE_CAPITULATE_BASE_THRESHOLD); // VP
	GD_INT_MEMBER(VASSALAGE_TREATMENT_THRESHOLD_DISAGREE); // VP
	GD_INT_MEMBER(VASSALAGE_TREATMENT_THRESHOLD_MISTREATED); // VP
	GD_INT_MEMBER(VASSALAGE_TREATMENT_THRESHOLD_UNHAPPY); // VP
	GD_INT_MEMBER(VASSALAGE_TREATMENT_THRESHOLD_ENSLAVED); // VP - End C4DF Other Values

	// -- floats -- //
	GD_FLOAT_MEMBER(AI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO);
	GD_FLOAT_MEMBER(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE);
	GD_FLOAT_MEMBER(AI_CITYSTRATEGY_YIELD_DEFICIENT_FAITH); // VP
	GD_FLOAT_MEMBER(UNHAPPINESS_PER_POPULATION_FLOAT); // VP
	GD_FLOAT_MEMBER(UNHAPPINESS_PER_OCCUPIED_POPULATION); // VP
	GD_FLOAT_MEMBER(GOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER);
	GD_FLOAT_MEMBER(UNIT_UPGRADE_COST_MULTIPLIER_PER_ERA);
	GD_FLOAT_MEMBER(UNIT_UPGRADE_COST_EXPONENT);
	GD_FLOAT_MEMBER(BALANCE_UNHAPPINESS_PER_MINORITY_POP); // VP
	GD_FLOAT_MEMBER(BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP); // VP
	GD_FLOAT_MEMBER(BALANCE_UNHAPPINESS_PER_PILLAGED); // VP
	GD_FLOAT_MEMBER(BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP); // VP
	GD_FLOAT_MEMBER(BALANCE_CORE_CORP_OFFICE_FRANCHISE_CONVERSION); // VP
	GD_FLOAT_MEMBER(BALANCE_CORE_CORP_OFFICE_TR_CONVERSION); // VP
	GD_FLOAT_MEMBER(DIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT);
	GD_FLOAT_MEMBER(DIPLO_VICTORY_CIV_DELEGATES_CONSTANT);
	GD_FLOAT_MEMBER(DIPLO_VICTORY_CS_DELEGATES_COEFFICIENT);
	GD_FLOAT_MEMBER(DIPLO_VICTORY_CS_DELEGATES_CONSTANT);
	GD_FLOAT_MEMBER(AI_PRODUCTION_WEIGHT_BASE_MOD);
	GD_FLOAT_MEMBER(AI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT);
	GD_FLOAT_MEMBER(AI_RESEARCH_WEIGHT_BASE_MOD);
	GD_FLOAT_MEMBER(AI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT);
	GD_FLOAT_MEMBER(HURRY_GOLD_PRODUCTION_EXPONENT);
	GD_FLOAT_MEMBER(CITY_ZOOM_LEVEL_1);
	GD_FLOAT_MEMBER(CITY_ZOOM_LEVEL_2);
	GD_FLOAT_MEMBER(CITY_ZOOM_LEVEL_3);
	GD_FLOAT_MEMBER(CITY_ZOOM_OFFSET);
	GD_FLOAT_MEMBER(CULTURE_COST_LATER_PLOT_EXPONENT);
	GD_FLOAT_MEMBER(POST_COMBAT_TEXT_DELAY);
	GD_FLOAT_MEMBER(FLAVOR_STANDARD_LOG10_TILES_PER_PLAYER);
	GD_FLOAT_MEMBER(AI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY);
	GD_FLOAT_MEMBER(GOLD_GIFT_FRIENDSHIP_EXPONENT);
	GD_FLOAT_MEMBER(GOLD_GIFT_FRIENDSHIP_DIVISOR);
	GD_FLOAT_MEMBER(HURRY_GOLD_TECH_EXPONENT);
	GD_FLOAT_MEMBER(HURRY_GOLD_CULTURE_EXPONENT);
	GD_FLOAT_MEMBER(CITY_GROWTH_MULTIPLIER);
	GD_FLOAT_MEMBER(CITY_GROWTH_EXPONENT);
	GD_FLOAT_MEMBER(POLICY_COST_EXPONENT);
	GD_FLOAT_MEMBER(POLICY_COST_EXTRA_VALUE);
	GD_FLOAT_MEMBER(CITY_STRENGTH_TECH_BASE);
	GD_FLOAT_MEMBER(CITY_STRENGTH_TECH_EXPONENT);
	GD_FLOAT_MEMBER(LEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD);
	GD_FLOAT_MEMBER(LEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD);
	GD_FLOAT_MEMBER(OPINION_WEIGHT_VASSAL_TAX_EXPONENT); // VP
	GD_FLOAT_MEMBER(TECH_COST_ERA_EXPONENT); // VP
	GD_FLOAT_MEMBER(VASSALAGE_VASSAL_CITY_POP_EXPONENT); // VP
	GD_FLOAT_MEMBER(MINOR_INFLUENCE_SCALING_DECAY_EXPONENT); // VP
	GD_FLOAT_MEMBER(APPROACH_SHIFT_PERCENT); // VP
	GD_FLOAT_MEMBER(DISTRESS_MEDIAN_RATE_CHANGE); // VP
	GD_FLOAT_MEMBER(POVERTY_MEDIAN_RATE_CHANGE); // VP
	GD_FLOAT_MEMBER(ILLITERACY_MEDIAN_RATE_CHANGE); // VP
	GD_FLOAT_MEMBER(BOREDOM_MEDIAN_RATE_CHANGE); // VP

	// -- post defines -- //
	GD_INT_MEMBER(LAND_TERRAIN);
	GD_INT_MEMBER(DEEP_WATER_TERRAIN);
	GD_INT_MEMBER(SHALLOW_WATER_TERRAIN);
	GD_INT_MEMBER(RUINS_IMPROVEMENT);
	GD_INT_MEMBER(NUKE_FEATURE);
	GD_INT_MEMBER(ARTIFACT_RESOURCE);
	GD_INT_MEMBER(HIDDEN_ARTIFACT_RESOURCE);
	GD_INT_MEMBER(CAPITAL_BUILDINGCLASS);
	GD_INT_MEMBER(WALLS_BUILDINGCLASS);
	GD_INT_MEMBER(DEFAULT_SPECIALIST);
	GD_INT_MEMBER(SPACE_RACE_TRIGGER_PROJECT);
	GD_INT_MEMBER(SPACESHIP_CAPSULE);
	GD_INT_MEMBER(SPACESHIP_BOOSTER);
	GD_INT_MEMBER(SPACESHIP_STASIS);
	GD_INT_MEMBER(SPACESHIP_ENGINE);
	GD_INT_MEMBER(INITIAL_CITY_ROUTE_TYPE);
	GD_INT_MEMBER(AI_HANDICAP);
	GD_INT_MEMBER(MULTIPLAYER_HANDICAP);
	GD_INT_MEMBER(STANDARD_HANDICAP);
	GD_INT_MEMBER(STANDARD_GAMESPEED);
	GD_INT_MEMBER(STANDARD_ERA);
	GD_INT_MEMBER(LAST_EMBARK_ART_ERA);
	GD_INT_MEMBER(LAST_UNIT_ART_ERA);
	GD_INT_MEMBER(LAST_BRIDGE_ART_ERA);
	GD_INT_MEMBER(BARBARIAN_HANDICAP);
	GD_INT_MEMBER(BARBARIAN_CIVILIZATION);
	GD_INT_MEMBER(BARBARIAN_LEADER);
	GD_INT_MEMBER(MINOR_CIV_HANDICAP);
	GD_INT_MEMBER(MINOR_CIVILIZATION);
	GD_INT_MEMBER(PROMOTION_EMBARKATION);
	GD_INT_MEMBER(PROMOTION_DEFENSIVE_EMBARKATION);
	GD_INT_MEMBER(PROMOTION_ALLWATER_EMBARKATION);
	GD_INT_MEMBER(PROMOTION_DEEPWATER_EMBARKATION); // VP
	GD_INT_MEMBER(PROMOTION_DEFENSIVE_DEEPWATER_EMBARKATION); // VP
	GD_INT_MEMBER(PROMOTION_FLAGSHIP); // VP
	GD_INT_MEMBER(PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY);
	GD_INT_MEMBER(PROMOTION_OCEAN_IMPASSABLE);
	GD_INT_MEMBER(PROMOTION_ONLY_DEFENSIVE);
	GD_INT_MEMBER(PROMOTION_UNWELCOME_EVANGELIST);
	GD_INT_MEMBER(BARBARIAN_CAMP_IMPROVEMENT);
	GD_INT_MEMBER(EMBASSY_IMPROVEMENT);
	GD_INT_MEMBER(PROMOTION_GOODY_HUT_PICKER);
	GD_INT_MEMBER(POLICY_BRANCH_FREEDOM);
	GD_INT_MEMBER(POLICY_BRANCH_AUTOCRACY);
	GD_INT_MEMBER(POLICY_BRANCH_ORDER);
	GD_INT_MEMBER(POLICY_BRANCH_HERITAGE);
	GD_INT_MEMBER(RELIGION_LAST_FOUND_ERA); // VP
	GD_INT_MEMBER(RELIGION_GP_FAITH_PURCHASE_ERA); // VP
	GD_INT_MEMBER(IDEOLOGY_START_ERA); // VP
	GD_INT_MEMBER(IDEOLOGY_PREREQ_ERA); // VP
	GD_INT_MEMBER(TOURISM_START_TECH); // VP
	GD_INT_MEMBER(TOURISM_START_ERA); // VP
	GD_INT_MEMBER(INQUISITION_EFFECTIVENESS); // VP
	GD_INT_MEMBER(INQUISITOR_CONVERSION_REDUCTION_FACTOR); // VP

	////////////// END DEFINES //////////////////

	public: // Note - Macros above clobber the visibility
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

	bool getDatabaseValue(const char* szName, int& iValue, bool bReportErrors = true);
	bool getDatabaseValue(const char* szName, float& fValue, bool bReportErrors = true);
	bool getDatabaseValue(const char* szName, CvString& szValue, bool bReportErrors = true);

	inline const GameDataHash& getGameDataHash() const { return m_gameDataHash; }
	inline uint32 getSaveVersion() const { return m_saveVersion; }
	inline void setSaveVersion(uint32 version) { m_saveVersion = version; }

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
	bool m_bPlayerAndCityAILogSplit;
	bool m_bTutorialLogging;
	bool m_bTutorialDebugging;
	bool m_bAllowRClickMovementWhileCameraScrolling;
	bool m_bPostTurnAutosaves;
	bool m_bOverwriteLogs;
	bool m_bOutOfSyncDebuggingEnabled;
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
	int m_aiRingFromLinearOffset[MAX_CITY_PLOTS];
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
	std::vector<CvPlotInfo*> m_paPlotInfo;
	std::vector<CvGreatPersonInfo*> m_paGreatPersonInfo;
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
	CvMilitaryAIStrategyXMLEntries* m_pMilitaryAIStrategies;
	CvAIGrandStrategyXMLEntries* m_pAIGrandStrategies;
	CvAICityStrategies* m_pAICityStrategies;
	CvPolicyXMLEntries* m_pPolicies;
	CvTechXMLEntries* m_pTechs;
	CvBuildingXMLEntries* m_pBuildings;
	std::map<BuildingTypes, std::vector<BuildingTypes>> m_buildingInteractionLookup;
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
#if defined(MOD_API_ACHIEVEMENTS)
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

	// The version number in save file being loaded.
	uint32 m_saveVersion;

	GameDataHash m_gameDataHash;

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
#include "stackwalker/StackWalker.h"

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
extern lua_State* gLuaState;
#endif


#endif //CIV5_GLOBALS_H