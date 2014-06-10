/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"
#include "CvDllVersion.h"

class CvDllNetworkSyncronization;
class CvDllNetInitInfo;
class CvDllNetLoadGameInfo;
class CvDllNetMessageHandler;
class CvDllScriptSystemUtility;
class CvDllWorldBuilderMapLoader;

class CvDllGameContext : public ICvGameContext3
{
public:
	~CvDllGameContext();

	void* DLLCALL QueryInterface(GUID guidInterface);


	// This is a singleton class.
	static void InitializeSingleton();
	static void DestroySingleton();
	static CvDllGameContext* GetSingleton();

	//Memory management functions used by adapters.
	_Check_return_ static void* Allocate(size_t bytes);
	static void Free(_In_ void* p);

	HANDLE GetHeap();
	HANDLE DLLCALL Debug_GetHeap() const;

	GUID DLLCALL GetDLLGUID();
	const char* DLLCALL GetDLLVersion();

	ICvNetworkSyncronization1* DLLCALL GetNetworkSyncronizer();
	ICvNetMessageHandler1* DLLCALL GetNetMessageHandler();
	ICvWorldBuilderMapLoader1* DLLCALL GetWorldBuilderMapLoader();
	ICvPreGame1* DLLCALL GetPreGame();
	ICvGame1* DLLCALL GetGame();
	ICvGameAsynch1* DLLCALL GetGameAsynch();
	ICvMap1* DLLCALL GetMap();
	ICvTeam1* DLLCALL GetTeam(TeamTypes eTeam);

	//Infos Accessors
	int DLLCALL GetInfoTypeForString(const char* szType, bool hideAssert = false) const;
	int DLLCALL GetInfoTypeForHash(uint uiHash, bool hideAssert = false) const;

	CivilizationTypes DLLCALL GetCivilizationInfoIndex(const char* pszType);

	int DLLCALL GetNumPlayableCivilizationInfos();
	int DLLCALL GetNumAIPlayableCivilizationInfos();
	int DLLCALL GetNumPlayableMinorCivs();

	int DLLCALL GetNumBuildInfos();
	int DLLCALL GetNumBuildingInfos();
	int DLLCALL GetNumCivilizationInfos();
	int DLLCALL GetNumClimateInfos();
	int DLLCALL GetNumColorInfos();
	int DLLCALL GetNumEraInfos();
	int DLLCALL GetNumFeatureInfos();
	int DLLCALL GetNumGameOptionInfos();
	int DLLCALL GetNumGameSpeedInfos();
	int DLLCALL GetNumHandicapInfos();
	int DLLCALL GetNumImprovementInfos();
	int DLLCALL GetNumLeaderHeadInfos();
	int DLLCALL GetNumMinorCivInfos();
	int DLLCALL GetNumPlayerColorInfos();
	int DLLCALL GetNumPolicyInfos();
	int DLLCALL GetNumPromotionInfos();
	int DLLCALL GetNumResourceInfos();
	int DLLCALL GetNumSeaLevelInfos();
	int DLLCALL GetNumTechInfos();
	int DLLCALL GetNumTerrainInfos();
	int DLLCALL GetNumUnitCombatClassInfos();
	int DLLCALL GetNumUnitInfos();
	int DLLCALL GetNumVictoryInfos();
	int DLLCALL GetNumWorldInfos();

	ICvBuildInfo1* DLLCALL GetBuildInfo(BuildTypes eBuildNum);
	ICvBuildingInfo1* DLLCALL GetBuildingInfo(BuildingTypes eBuilding);
	ICvCivilizationInfo1* DLLCALL GetCivilizationInfo(CivilizationTypes eCivilizationNum);
	ICvColorInfo1* DLLCALL GetColorInfo(ColorTypes e);
	ICvEraInfo1* DLLCALL GetEraInfo(EraTypes eEraNum);
	ICvFeatureInfo1* DLLCALL GetFeatureInfo(FeatureTypes eFeatureNum);
	ICvGameOptionInfo1* DLLCALL GetGameOptionInfo(GameOptionTypes eGameOptionNum);
	ICvGameSpeedInfo1* DLLCALL GetGameSpeedInfo(GameSpeedTypes eGameSpeedNum);
	ICvHandicapInfo1* DLLCALL GetHandicapInfo(HandicapTypes eHandicapNum);
	ICvInterfaceModeInfo1* DLLCALL GetInterfaceModeInfo(InterfaceModeTypes e);
	ICvImprovementInfo1* DLLCALL GetImprovementInfo(ImprovementTypes eImprovement);
	ICvLeaderHeadInfo1* DLLCALL GetLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum);
	ICvMinorCivInfo1* DLLCALL GetMinorCivInfo(MinorCivTypes eMinorCiv);
	ICvMissionInfo1* DLLCALL GetMissionInfo(MissionTypes eMission);
	ICvPlayerColorInfo1* DLLCALL GetPlayerColorInfo(PlayerColorTypes e);
	ICvPlayerOptionInfo1* DLLCALL GetPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum);
	ICvPolicyInfo1* DLLCALL GetPolicyInfo(PolicyTypes ePolicy);
	ICvPromotionInfo1* DLLCALL GetPromotionInfo(PromotionTypes ePromotion);
	ICvResourceInfo1* DLLCALL GetResourceInfo(ResourceTypes eResourceNum);
	ICvTechInfo1* DLLCALL GetTechInfo(TechTypes eTech);
	ICvTerrainInfo1* DLLCALL GetTerrainInfo(TerrainTypes eTerrainNum);
	ICvUnitCombatClassInfo1* DLLCALL GetUnitCombatClassInfo(UnitCombatTypes eUnitCombat);
	ICvUnitInfo1* DLLCALL GetUnitInfo(UnitTypes eUnit);
	ICvVictoryInfo1* DLLCALL GetVictoryInfo(VictoryTypes eVictoryType);

	// ***** EXPOSED *****
	// use very sparingly - this is costly
	bool DLLCALL GetDefineSTRING(char* szBuffer, size_t lenBuffer, const char* szName, bool bReportErrors = true);

	int DLLCALL GetMOVE_DENOMINATOR() const;
	int DLLCALL GetMAX_CITY_HIT_POINTS() const;
	float DLLCALL GetCITY_ZOOM_OFFSET() const;
	float DLLCALL GetCITY_ZOOM_LEVEL_1() const;
	float DLLCALL GetCITY_ZOOM_LEVEL_2() const;
	float DLLCALL GetCITY_ZOOM_LEVEL_3() const;
	int DLLCALL GetRUINS_IMPROVEMENT() const;
	int DLLCALL GetSHALLOW_WATER_TERRAIN() const;
	int DLLCALL GetDEFICIT_UNIT_DISBANDING_THRESHOLD() const;
	int DLLCALL GetLAST_UNIT_ART_ERA() const;
	int DLLCALL GetLAST_EMBARK_ART_ERA() const;
	int DLLCALL GetHEAVY_RESOURCE_THRESHOLD() const;
	int DLLCALL GetSTANDARD_HANDICAP() const;
	int DLLCALL GetSTANDARD_GAMESPEED() const;
	int DLLCALL GetLAST_BRIDGE_ART_ERA() const;
	int DLLCALL GetBARBARIAN_CIVILIZATION() const;
	int DLLCALL GetMINOR_CIVILIZATION() const;
	int DLLCALL GetBARBARIAN_HANDICAP() const;
	int DLLCALL GetBARBARIAN_LEADER() const;
	int DLLCALL GetMINOR_CIV_HANDICAP() const;
	int DLLCALL GetWALLS_BUILDINGCLASS() const;
	int DLLCALL GetAI_HANDICAP() const;
	int DLLCALL GetNUM_CITY_PLOTS() const;

	const char** DLLCALL GetHexDebugLayerNames();
	float DLLCALL GetHexDebugLayerScale(const char* szLayerName);
	bool DLLCALL GetHexDebugLayerString(ICvPlot1* pPlot, const char* szLayerName, PlayerTypes ePlayer, char* szBuffer, unsigned int uiBufferLength);

	void DLLCALL Init();
	void DLLCALL Uninit();

	ICvScriptSystemUtility1* DLLCALL GetScriptSystemUtility();

	const char* DLLCALL GetNotificationType(int NotificationID) const;

	bool DLLCALL GetLogging();
	void DLLCALL SetLogging(bool bEnable);
	int DLLCALL GetRandLogging();
	void DLLCALL SetRandLogging(int iRandLoggingFlags);
	bool DLLCALL GetAILogging();
	void DLLCALL SetAILogging(bool bEnable);
	bool DLLCALL GetAIPerfLogging();
	void DLLCALL SetAIPerfLogging(bool bEnable);
	bool DLLCALL GetBuilderAILogging();
	void DLLCALL SetBuilderAILogging(bool bEnable);
	bool DLLCALL GetPlayerAndCityAILogSplit();
	void DLLCALL SetPlayerAndCityAILogSplit(bool bEnable);
	bool DLLCALL GetTutorialLogging();
	void DLLCALL SetTutorialLogging(bool bEnable);
	bool DLLCALL GetTutorialDebugging();
	void DLLCALL SetTutorialDebugging(bool bEnable);
	bool DLLCALL GetAllowRClickMovementWhileScrolling();
	void DLLCALL SetAllowRClickMovementWhileScrolling(bool bAllow);
	bool DLLCALL GetPostTurnAutosaves();
	void DLLCALL SetPostTurnAutosaves(bool bEnable);

	ICvDLLDatabaseUtility1* DLLCALL GetDatabaseLoadUtility();
	int* DLLCALL GetPlotDirectionX();
	int* DLLCALL GetPlotDirectionY();
	int* DLLCALL GetCityPlotX();
	int* DLLCALL GetCityPlotY();
	void DLLCALL SetGameDatabase(Database::Connection* pGameDatabase);
	bool DLLCALL SetDLLIFace(ICvEngineUtility1* pDll);

	bool DLLCALL IsGraphicsInitialized() const;
	void DLLCALL SetGraphicsInitialized(bool bVal);
	void DLLCALL SetOutOfSyncDebuggingEnabled(bool isEnabled);
	bool DLLCALL RandomNumberGeneratorSyncCheck(PlayerTypes ePlayer, ICvRandom1* pRandom, bool bIsHost);

	unsigned int DLLCALL CreateRandomNumberGenerator();
	ICvRandom1* DLLCALL GetRandomNumberGenerator(unsigned int index);
	void DLLCALL DestroyRandomNumberGenerator(unsigned int index);

	unsigned int DLLCALL CreateNetInitInfo();
	ICvNetInitInfo1* DLLCALL GetNetInitInfo(unsigned int index);
	void DLLCALL DestroyNetInitInfo(unsigned int index);

	unsigned int DLLCALL CreateNetLoadGameInfo();
	ICvNetLoadGameInfo1* DLLCALL GetNetLoadGameInfo(unsigned int index);
	void DLLCALL DestroyNetLoadGameInfo(unsigned int index);

	void DLLCALL TEMPOnHexUnitChanged(ICvUnit1* pUnit);
	void DLLCALL TEMPOnHexUnitChangedAttack(ICvUnit1* pUnit);
	ICvEnumerator* DLLCALL TEMPCalculatePathFinderUpdates(ICvUnit1* pHeadSelectedUnit, int iMouseMapX, int iMouseMapY);
	void DLLCALL ResetPathFinder();

	void DLLCALL SetEngineUserInterface(ICvUserInterface2* pUI);

protected:
	void DLLCALL Destroy();

	CvDllGameContext();


private:
	static CvDllGameContext* s_pSingleton;
	static HANDLE s_hHeap;

	CvDllNetworkSyncronization* m_pNetworkSyncronizer;
	CvDllNetMessageHandler* m_pNetMessageHandler;
	CvDllScriptSystemUtility* m_pScriptSystemUtility;
	CvDllWorldBuilderMapLoader* m_pWorldBuilderMapLoader;

	//Programmer Note:
	//Rather than use the common idiom of using the index of the object in an array as the public
	//identifier to the object, I chose to use a system where the index will always be unique even
	//after objects are destroyed.  This technique is prone to fewer bugs w/ heavily threaded apps.
	std::vector<std::pair<uint, CvRandom*> > m_RandomNumberGenerators;
	unsigned int m_uiRngCounter;

	std::vector<std::pair<uint, CvDllNetInitInfo*> > m_NetInitInfos;
	unsigned int m_uiNetInitInfoCounter;

	std::vector<std::pair<uint, CvDllNetLoadGameInfo*> > m_NetLoadGameInfos;
	unsigned int m_uiNetLoadGameInfoCounter;
};