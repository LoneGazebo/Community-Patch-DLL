/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllContext.h"

#include "CvDllBuildingInfo.h"
#include "CvDllGame.h"
#include "CvDllGameAsynch.h"
#include "CvDllImprovementInfo.h"
#include "CvDllMap.h"
#include "CvDllTeam.h"
#include "CvDllBuildInfo.h"
#include "CvDllCivilizationInfo.h"
#include "CvDllColorInfo.h"
#include "CvDllEraInfo.h"
#include "CvDllFeatureInfo.h"
#include "CvDllGameOptionInfo.h"
#include "CvDllGameSpeedInfo.h"
#include "CvDllHandicapInfo.h"
#include "CvDllInterfaceModeInfo.h"
#include "CvDllLeaderheadInfo.h"
#include "CvDllMinorCivInfo.h"
#include "CvDllMissionInfo.h"
#include "CvDllNetInitInfo.h"
#include "CvDllNetLoadGameInfo.h"
#include "CvDllNetMessageHandler.h"
#include "CvDllNetworkSyncronization.h"
#include "CvDllPathFinderUpdate.h"
#include "CvDllPlayerColorInfo.h"
#include "CvDllPlayerOptionInfo.h"
#include "CvDllPlot.h"
#include "CvDllPolicyInfo.h"
#include "CvDllPreGame.h"
#include "CvDllPromotionInfo.h"
#include "CvDllRandom.h"
#include "CvDllResourceInfo.h"
#include "CvDllScriptSystemUtility.h"
#include "CvDllTechInfo.h"
#include "CvDllTerrainInfo.h"
#include "CvDllUnitInfo.h"
#include "CvDllUnitCombatClassInfo.h"
#include "CvDllVictoryInfo.h"
#include "CvDllWorldBuilderMapLoader.h"


CvDllGameContext* CvDllGameContext::s_pSingleton = NULL;
HANDLE CvDllGameContext::s_hHeap = INVALID_HANDLE_VALUE;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CvDllGameContext::CvDllGameContext()
: m_uiRngCounter(0)
, m_uiNetInitInfoCounter(0)
, m_uiNetLoadGameInfoCounter(0)
{
	m_pNetworkSyncronizer = new CvDllNetworkSyncronization();
	m_pNetMessageHandler = new CvDllNetMessageHandler();
	m_pScriptSystemUtility = new CvDllScriptSystemUtility();
	m_pWorldBuilderMapLoader = new CvDllWorldBuilderMapLoader();
}
//------------------------------------------------------------------------------
CvDllGameContext::~CvDllGameContext()
{
	delete m_pNetworkSyncronizer;
	delete m_pNetMessageHandler;
	delete m_pScriptSystemUtility;
	delete m_pWorldBuilderMapLoader;

	for(std::vector<std::pair<uint, CvRandom*> >::iterator it = m_RandomNumberGenerators.begin();
		it != m_RandomNumberGenerators.end(); ++it)
	{
		delete (*it).second;
		it->second = NULL;
	}
	m_RandomNumberGenerators.clear();

	for(std::vector<std::pair<uint, CvDllNetInitInfo*> >::iterator it = m_NetInitInfos.begin();
		it != m_NetInitInfos.end(); ++it)
	{
		delete (*it).second;
		it->second = NULL;
	}
	m_NetInitInfos.clear();


	for(std::vector<std::pair<uint, CvDllNetLoadGameInfo*> >::iterator it = m_NetLoadGameInfos.begin();
		it != m_NetLoadGameInfos.end(); ++it)
	{
		delete (*it).second;
		it->second = NULL;
	}
	m_NetLoadGameInfos.clear();
}
//------------------------------------------------------------------------------
void* CvDllGameContext::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvGameContext1::GetInterfaceId() ||
		guidInterface == ICvGameContext2::GetInterfaceId() ||
		guidInterface == ICvGameContext3::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}

//	---------------------------------------------------------------------------
GUID CvDllGameContext::GetDLLGUID()
{
	return CIV5_CORE_RELEASE_DLL_GUID;
}

//	---------------------------------------------------------------------------
const char * CvDllGameContext::GetDLLVersion()
{
	return CIV5_CORE_RELEASE_DLL_VERSION;
}

//------------------------------------------------------------------------------
void CvDllGameContext::Destroy()
{
	//Do nothing, it's a singleton controlled by the DLL.
}
//------------------------------------------------------------------------------
void CvDllGameContext::InitializeSingleton()
{
	if(s_pSingleton == NULL)
	{
		FAssert(s_hHeap == INVALID_HANDLE_VALUE);
		s_hHeap = HeapCreate(0, 0, 0);

		//
		// Enable the low-fragmentation heap (LFH). Starting with Windows Vista,
		// the LFH is enabled by default but this call does not cause an error.
		//
		ULONG HeapInformation = 2;	//Low Fragmentation Heap
		HeapSetInformation(s_hHeap,
			HeapCompatibilityInformation,
			&HeapInformation,
			sizeof(HeapInformation));


		s_pSingleton = FNEW(CvDllGameContext(), c_eCiv5GameplayDLL, 0);
	}
}
//------------------------------------------------------------------------------
void CvDllGameContext::DestroySingleton()
{
	SAFE_DELETE(s_pSingleton);
	HeapDestroy(s_hHeap);
	s_hHeap = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------
CvDllGameContext* CvDllGameContext::GetSingleton()
{
	return s_pSingleton;
}
//------------------------------------------------------------------------------
HANDLE CvDllGameContext::GetHeap()
{
	return s_hHeap;
}
//------------------------------------------------------------------------------
HANDLE CvDllGameContext::Debug_GetHeap() const
{
	return s_hHeap;
}
//------------------------------------------------------------------------------
void* CvDllGameContext::Allocate(size_t bytes)
{
	return HeapAlloc(s_hHeap, 0, bytes);
}
//------------------------------------------------------------------------------
void CvDllGameContext::Free(void* p)
{
	HeapFree(s_hHeap, 0, p);
}
//------------------------------------------------------------------------------
ICvNetworkSyncronization1* CvDllGameContext::GetNetworkSyncronizer()
{
	return m_pNetworkSyncronizer;
}
//------------------------------------------------------------------------------
ICvNetMessageHandler1* CvDllGameContext::GetNetMessageHandler()
{
	return m_pNetMessageHandler;
}
//------------------------------------------------------------------------------
ICvWorldBuilderMapLoader1* CvDllGameContext::GetWorldBuilderMapLoader()
{
	return m_pWorldBuilderMapLoader;
}
//------------------------------------------------------------------------------
ICvPreGame1* CvDllGameContext::GetPreGame()
{
	return new CvDllPreGame();
}
//------------------------------------------------------------------------------
ICvGame1* CvDllGameContext::GetGame()
{
	CvGame* pkGame = GC.getGamePointer();
	if (pkGame)
		return new CvDllGame(pkGame);
	return NULL;
}
//------------------------------------------------------------------------------
ICvGameAsynch1* CvDllGameContext::GetGameAsynch()
{
	CvGame* pkGame = GC.getGamePointer();
	if (pkGame)
		return new CvDllGameAsynch(pkGame);
	return NULL;
}
//------------------------------------------------------------------------------
ICvMap1* CvDllGameContext::GetMap()
{
	CvMap* pkMap = GC.getMapPointer();
	if (pkMap)
		return new CvDllMap(pkMap);
	return NULL;
}
//------------------------------------------------------------------------------
ICvTeam1* CvDllGameContext::GetTeam(TeamTypes eTeam)
{
	if(eTeam < MAX_TEAMS)
	{
		CvTeam& kTeam = CvTeam::getTeam(eTeam);
		return new CvDllTeam(&kTeam);
	}

	return NULL;
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetInfoTypeForString(const char* szType, bool hideAssert) const
{
	return GC.getInfoTypeForString(szType, hideAssert);
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetInfoTypeForHash(uint uiHash, bool hideAssert) const
{
	return GC.getInfoTypeForHash(uiHash, hideAssert);
}
//------------------------------------------------------------------------------
CivilizationTypes CvDllGameContext::GetCivilizationInfoIndex(const char* pszType)
{
	return GC.getCivilizationInfoIndex(pszType);
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumPlayableCivilizationInfos()
{
	return GC.getNumPlayableCivilizationInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumAIPlayableCivilizationInfos()
{
	return GC.getNumAIPlayableCivilizationInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumPlayableMinorCivs()
{
	return GC.GetNumPlayableMinorCivs();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumBuildInfos()
{
	return GC.getNumBuildInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumBuildingInfos()
{
	return GC.getNumBuildingInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumCivilizationInfos()
{
	return GC.getNumCivilizationInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumClimateInfos()
{
	return GC.getNumClimateInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumColorInfos()
{
	return GC.GetNumColorInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumEraInfos()
{
	return GC.getNumEraInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumFeatureInfos()
{
	return GC.getNumFeatureInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumGameOptionInfos()
{
	return GC.getNumGameOptionInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumGameSpeedInfos()
{
	return GC.getNumGameSpeedInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumHandicapInfos()
{
	return GC.getNumHandicapInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumImprovementInfos()
{
	return GC.getNumImprovementInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumLeaderHeadInfos()
{
	return GC.getNumLeaderHeadInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumMinorCivInfos()
{
	return GC.getNumMinorCivInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumPlayerColorInfos()
{
	return GC.GetNumPlayerColorInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumPolicyInfos()
{
	return GC.getNumPolicyInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumPromotionInfos()
{
	return GC.getNumPromotionInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumResourceInfos()
{
	return GC.getNumResourceInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumSeaLevelInfos()
{
	return GC.getNumSeaLevelInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumTechInfos()
{
	return GC.getNumTechInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumTerrainInfos()
{
	return GC.getNumTerrainInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumUnitCombatClassInfos()
{
	return GC.getNumUnitCombatClassInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumUnitInfos()
{
	return GC.getNumUnitInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumVictoryInfos()
{
	return GC.getNumVictoryInfos();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNumWorldInfos()
{
	return GC.getNumWorldInfos();
}
//------------------------------------------------------------------------------
ICvBuildInfo1* CvDllGameContext::GetBuildInfo(BuildTypes eBuildNum)
{
	CvBuildInfo* pkInfo = GC.getBuildInfo(eBuildNum);
	return (NULL != pkInfo)? new CvDllBuildInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvBuildingInfo1* CvDllGameContext::GetBuildingInfo(BuildingTypes eBuilding)
{
	CvBuildingEntry* pkEntry = GC.getBuildingInfo(eBuilding);
	return (NULL != pkEntry)? new CvDllBuildingInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
ICvCivilizationInfo1* CvDllGameContext::GetCivilizationInfo(CivilizationTypes eCivilizationNum)
{
	CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(eCivilizationNum);
	return (NULL != pkInfo)? new CvDllCivilizationInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvColorInfo1* CvDllGameContext::GetColorInfo(ColorTypes e)
{
	CvColorInfo* pkInfo = GC.GetColorInfo(e);
	return (NULL != pkInfo)? new CvDllColorInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvEraInfo1* CvDllGameContext::GetEraInfo(EraTypes eEraNum)
{
	CvEraInfo* pkInfo = GC.getEraInfo(eEraNum);
	return (NULL != pkInfo)? new CvDllEraInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvFeatureInfo1* CvDllGameContext::GetFeatureInfo(FeatureTypes eFeatureNum)
{
	CvFeatureInfo* pkInfo = GC.getFeatureInfo(eFeatureNum);
	return (NULL != pkInfo)? new CvDllFeatureInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvGameOptionInfo1* CvDllGameContext::GetGameOptionInfo(GameOptionTypes eGameOptionNum)
{
	CvGameOptionInfo* pkInfo = GC.getGameOptionInfo(eGameOptionNum);
	return (NULL != pkInfo)? new CvDllGameOptionInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvGameSpeedInfo1* CvDllGameContext::GetGameSpeedInfo(GameSpeedTypes eGameSpeedNum)
{
	CvGameSpeedInfo* pkInfo = GC.getGameSpeedInfo(eGameSpeedNum);
	return (NULL != pkInfo)? new CvDllGameSpeedInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvHandicapInfo1* CvDllGameContext::GetHandicapInfo(HandicapTypes eHandicapNum)
{
	CvHandicapInfo* pkInfo = GC.getHandicapInfo(eHandicapNum);
	return (NULL != pkInfo)? new CvDllHandicapInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvInterfaceModeInfo1* CvDllGameContext::GetInterfaceModeInfo(InterfaceModeTypes e)
{
	CvInterfaceModeInfo* pkInfo = GC.getInterfaceModeInfo(e);
	return (NULL != pkInfo)? new CvDllInterfaceModeInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvImprovementInfo1* CvDllGameContext::GetImprovementInfo(ImprovementTypes eImprovement)
{
	CvImprovementEntry* pkEntry = GC.getImprovementInfo(eImprovement);
	return (NULL != pkEntry)? new CvDllImprovementInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
ICvLeaderHeadInfo1* CvDllGameContext::GetLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum)
{
	CvLeaderHeadInfo* pkInfo = GC.getLeaderHeadInfo(eLeaderHeadNum);
	return (NULL != pkInfo)? new CvDllLeaderHeadInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvMinorCivInfo1* CvDllGameContext::GetMinorCivInfo(MinorCivTypes eMinorCiv)
{
	CvMinorCivInfo* pkInfo = GC.getMinorCivInfo(eMinorCiv);
	return (NULL != pkInfo)? new CvDllMinorCivInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvMissionInfo1* CvDllGameContext::GetMissionInfo(MissionTypes eMission)
{
	CvMissionInfo* pkInfo = GC.getMissionInfo(eMission);
	return (NULL != pkInfo)? new CvDllMissionInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvPlayerColorInfo1* CvDllGameContext::GetPlayerColorInfo(PlayerColorTypes e)
{
	CvPlayerColorInfo* pkInfo = GC.GetPlayerColorInfo(e);
	return (NULL != pkInfo)? new CvDllPlayerColorInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvPlayerOptionInfo1* CvDllGameContext::GetPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum)
{
	CvPlayerOptionInfo* pkInfo = GC.getPlayerOptionInfo(ePlayerOptionNum);
	return (NULL != pkInfo)? new CvDllPlayerOptionInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvPolicyInfo1* CvDllGameContext::GetPolicyInfo(PolicyTypes ePolicy)
{
	CvPolicyEntry* pkInfo = GC.getPolicyInfo(ePolicy);
	return (NULL != pkInfo)? new CvDllPolicyInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvPromotionInfo1* CvDllGameContext::GetPromotionInfo(PromotionTypes ePromotion)
{
	CvPromotionEntry* pkInfo = GC.getPromotionInfo(ePromotion);
	return (NULL != pkInfo)? new CvDllPromotionInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvResourceInfo1* CvDllGameContext::GetResourceInfo(ResourceTypes eResourceNum)
{
	CvResourceInfo* pkInfo = GC.getResourceInfo(eResourceNum);
	return (NULL != pkInfo)? new CvDllResourceInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvTechInfo1* CvDllGameContext::GetTechInfo(TechTypes eTech)
{
	CvTechEntry* pkEntry = GC.getTechInfo(eTech);
	return (NULL != pkEntry)? new CvDllTechInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
ICvTerrainInfo1* CvDllGameContext::GetTerrainInfo(TerrainTypes eTerrainNum)
{
	CvTerrainInfo* pkInfo = GC.getTerrainInfo(eTerrainNum);
	return (NULL != pkInfo)? new CvDllTerrainInfo(pkInfo) : NULL;
}
//------------------------------------------------------------------------------
ICvUnitInfo1* CvDllGameContext::GetUnitInfo(UnitTypes eUnit)
{
	CvUnitEntry* pkEntry = GC.getUnitInfo(eUnit);
	return (NULL != pkEntry)? new CvDllUnitInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
ICvUnitCombatClassInfo1* CvDllGameContext::GetUnitCombatClassInfo(UnitCombatTypes eUnitCombat)
{
	CvBaseInfo* pkEntry = GC.getUnitCombatClassInfo(eUnitCombat);
	return (NULL != pkEntry)? new CvDllUnitCombatClassInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
ICvVictoryInfo1* CvDllGameContext::GetVictoryInfo(VictoryTypes eVictoryType)
{
	CvVictoryInfo* pkEntry = GC.getVictoryInfo(eVictoryType);
	return (NULL != pkEntry)? new CvDllVictoryInfo(pkEntry) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetDefineSTRING(char* szBuffer, size_t lenBuffer, const char* szName, bool bReportErrors)
{
	if(szBuffer != NULL && lenBuffer > 0)
	{
		CvString strDefine = GC.getDefineSTRING(szName, bReportErrors);
		if(strDefine.size() < lenBuffer)
		{
			strncpy_s(szBuffer, lenBuffer, strDefine.c_str(), strDefine.size());
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetMOVE_DENOMINATOR() const
{
	return GC.getMOVE_DENOMINATOR();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetMAX_CITY_HIT_POINTS() const
{
	return GC.getMAX_CITY_HIT_POINTS();
}
//------------------------------------------------------------------------------
float CvDllGameContext::GetCITY_ZOOM_OFFSET() const
{
	return GC.getCITY_ZOOM_OFFSET();
}
//------------------------------------------------------------------------------
float CvDllGameContext::GetCITY_ZOOM_LEVEL_1() const
{
	return GC.getCITY_ZOOM_LEVEL_1();
}
//------------------------------------------------------------------------------
float CvDllGameContext::GetCITY_ZOOM_LEVEL_2() const
{
	return GC.getCITY_ZOOM_LEVEL_2();
}
//------------------------------------------------------------------------------
float CvDllGameContext::GetCITY_ZOOM_LEVEL_3() const
{
	return GC.getCITY_ZOOM_LEVEL_3();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetRUINS_IMPROVEMENT() const
{
	return GC.getRUINS_IMPROVEMENT();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetSHALLOW_WATER_TERRAIN() const
{
	return GC.getSHALLOW_WATER_TERRAIN();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetDEFICIT_UNIT_DISBANDING_THRESHOLD() const
{
	return GC.getDEFICIT_UNIT_DISBANDING_THRESHOLD();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetLAST_UNIT_ART_ERA() const
{
	return GC.getLAST_UNIT_ART_ERA();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetLAST_EMBARK_ART_ERA() const
{
	return GC.getLAST_EMBARK_ART_ERA();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetHEAVY_RESOURCE_THRESHOLD() const
{
	return GC.getHEAVY_RESOURCE_THRESHOLD();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetSTANDARD_HANDICAP() const
{
	return GC.getSTANDARD_HANDICAP();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetSTANDARD_GAMESPEED() const
{
	return GC.getSTANDARD_GAMESPEED();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetLAST_BRIDGE_ART_ERA() const
{
	return GC.getLAST_BRIDGE_ART_ERA();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetBARBARIAN_CIVILIZATION() const
{
	return GC.getBARBARIAN_CIVILIZATION();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetMINOR_CIVILIZATION() const
{
	return GC.getMINOR_CIVILIZATION();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetBARBARIAN_HANDICAP() const
{
	return GC.getBARBARIAN_HANDICAP();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetBARBARIAN_LEADER() const
{
	return GC.getBARBARIAN_LEADER();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetMINOR_CIV_HANDICAP() const
{
	return GC.getMINOR_CIV_HANDICAP();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetWALLS_BUILDINGCLASS() const
{
	return GC.getWALLS_BUILDINGCLASS();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetAI_HANDICAP() const
{
	return GC.getAI_HANDICAP();
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetNUM_CITY_PLOTS() const
{
	return NUM_CITY_PLOTS;
}
//------------------------------------------------------------------------------
const char** CvDllGameContext::GetHexDebugLayerNames()
{
	return GC.GetHexDebugLayerNames();
}
//------------------------------------------------------------------------------
float CvDllGameContext::GetHexDebugLayerScale(const char* szLayerName)
{
	return GC.GetHexDebugLayerScale(szLayerName);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetHexDebugLayerString(ICvPlot1* pPlot, const char* szLayerName, PlayerTypes ePlayer, char* szBuffer, unsigned int uiBufferLength)
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	if(pkPlot != NULL)
	{
		return GC.GetHexDebugLayerString(pkPlot, szLayerName, ePlayer, szBuffer, uiBufferLength);
	}

	return false;
}
//------------------------------------------------------------------------------
void CvDllGameContext::Init()
{
	GC.init();
}
//------------------------------------------------------------------------------
void CvDllGameContext::Uninit()
{
	GC.uninit();
}
//------------------------------------------------------------------------------
ICvScriptSystemUtility1* CvDllGameContext::GetScriptSystemUtility()
{
	return m_pScriptSystemUtility;
}
//------------------------------------------------------------------------------
const char* CvDllGameContext::GetNotificationType(int NotificationID) const
{
	CvNotificationXMLEntries* pkEntries = GC.GetNotificationEntries();
	if(pkEntries != NULL)
	{
		CvNotificationEntry* pkEntry = pkEntries->GetEntry(NotificationID);
		if(pkEntry != NULL)
			return pkEntry->GetTypeName();
	}

	return NULL;
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetLogging()
{
	return GC.getLogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetLogging(bool bEnable)
{
	GC.setLogging(bEnable);
}
//------------------------------------------------------------------------------
int CvDllGameContext::GetRandLogging()
{
	return GC.getRandLogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetRandLogging(int iRandLoggingFlags)
{
	GC.setRandLogging(iRandLoggingFlags);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetAILogging()
{
	return GC.getAILogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetAILogging(bool bEnable)
{
	GC.setAILogging(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetAIPerfLogging()
{
	return GC.getAIPerfLogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetAIPerfLogging(bool bEnable)
{
	GC.setAIPerfLogging(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetBuilderAILogging()
{
	return GC.GetBuilderAILogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetBuilderAILogging(bool bEnable)
{
	GC.SetBuilderAILogging(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetPlayerAndCityAILogSplit()
{
	return GC.getPlayerAndCityAILogSplit();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetPlayerAndCityAILogSplit(bool bEnable)
{
	GC.setPlayerAndCityAILogSplit(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetTutorialLogging()
{
	return GC.GetTutorialLogging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetTutorialLogging(bool bEnable)
{
	GC.SetTutorialLogging(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetTutorialDebugging()
{
	return GC.GetTutorialDebugging();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetTutorialDebugging(bool bEnable)
{
	GC.SetTutorialDebugging(bEnable);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetAllowRClickMovementWhileScrolling()
{
	return GC.GetAllowRClickMovementWhileScrolling();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetAllowRClickMovementWhileScrolling(bool bAllow)
{
	GC.SetAllowRClickMovementWhileScrolling(bAllow);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::GetPostTurnAutosaves()
{
	return GC.GetPostTurnAutosaves();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetPostTurnAutosaves(bool bEnable)
{
	GC.SetPostTurnAutosaves(bEnable);
}
//------------------------------------------------------------------------------
ICvDLLDatabaseUtility1* CvDllGameContext::GetDatabaseLoadUtility()
{
	return GC.getDatabaseLoadUtility();
}
//------------------------------------------------------------------------------
int* CvDllGameContext::GetPlotDirectionX()
{
	return GC.getPlotDirectionX();
}
//------------------------------------------------------------------------------
int* CvDllGameContext::GetPlotDirectionY()
{
	return GC.getPlotDirectionY();
}
//------------------------------------------------------------------------------
int* CvDllGameContext::GetCityPlotX()
{
	return GC.getCityPlotX();
}
//------------------------------------------------------------------------------
int* CvDllGameContext::GetCityPlotY()
{
	return GC.getCityPlotY();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetGameDatabase(Database::Connection* pGameDatabase)
{
	GC.SetGameDatabase(pGameDatabase);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::SetDLLIFace(ICvEngineUtility1* pDll)
{
	//Since we're using QueryInterface to allocate a new instance, we need to explicitly clean up the old reference.
	ICvEngineUtility4* pOldDll = GC.getDLLIFace();
	if(pOldDll != NULL)
	{
		delete pOldDll;
	}

	ICvEngineUtility4* pDllInterface = (pDll != NULL)? pDll->QueryInterface<ICvEngineUtility4>() : NULL;
	GC.setDLLIFace(pDllInterface);
	return pDllInterface != NULL;
}
//------------------------------------------------------------------------------
bool CvDllGameContext::IsGraphicsInitialized() const
{
	return GC.IsGraphicsInitialized();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetGraphicsInitialized(bool bVal)
{
	GC.SetGraphicsInitialized(bVal);
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetOutOfSyncDebuggingEnabled(bool isEnabled)
{
	GC.setOutOfSyncDebuggingEnabled(isEnabled);
}
//------------------------------------------------------------------------------
bool CvDllGameContext::RandomNumberGeneratorSyncCheck(PlayerTypes ePlayer, ICvRandom1* pRandom, bool bIsHost)
{
	// uh oh! Check the Random number generator!
	const CvRandom & localSimRandomNumberGenerator = GC.getGame().getJonRand();
	CvRandom* pkRandom = GC.UnwrapRandomPointer(pRandom);

	if(localSimRandomNumberGenerator != *pkRandom)
	{
		FILogFile* logFile = LOGFILEMGR.GetLog("net_message_debug.log", 0);

		char formatBuf[128] = {"\0"};
		std::string rngLogMessage = "Game Random Number Generators are out of sync : local.seed=";
		rngLogMessage += _itoa_s(localSimRandomNumberGenerator.getSeed(), formatBuf, 10);
		rngLogMessage += ", remote.seed=";
		rngLogMessage += _itoa_s(pkRandom->getSeed(), formatBuf, 10);
		rngLogMessage += "\n\tlocal.callCount=";
		rngLogMessage += _itoa_s(localSimRandomNumberGenerator.getCallCount(), formatBuf, 10);
		rngLogMessage += ", remote.callCount=";
		rngLogMessage += _itoa_s(pkRandom->getCallCount(), formatBuf, 10);
		rngLogMessage += "\n\tlocal.resetCount=";
		rngLogMessage += _itoa_s(localSimRandomNumberGenerator.getResetCount(), formatBuf, 10);
		rngLogMessage += ", remote.resetCount=";
		rngLogMessage += _itoa_s(pkRandom->getResetCount(), formatBuf, 10);
		rngLogMessage += "\n";

		if(localSimRandomNumberGenerator.callStackDebuggingEnabled() && pkRandom->callStackDebuggingEnabled())
		{
			std::string localCallStack("");
			std::string remoteCallStack("");
			localSimRandomNumberGenerator.resolveCallStacks();
			const std::vector<std::string> & localCallStacks = localSimRandomNumberGenerator.getResolvedCallStacks();
			const std::vector<std::string> & remoteCallStacks =  pkRandom->getResolvedCallStacks();
			const std::vector<unsigned long> & localSeedHistory = localSimRandomNumberGenerator.getSeedHistory();
			const std::vector<unsigned long> & remoteSeedHistory = pkRandom->getSeedHistory();
			std::vector<unsigned long>::const_iterator localSeedIterator = localSeedHistory.begin();
			std::vector<unsigned long>::const_iterator remoteSeedIterator = remoteSeedHistory.begin();
			unsigned int callNumber = 0;
			for(localSeedIterator = localSeedHistory.begin(), remoteSeedIterator = remoteSeedHistory.begin(); localSeedIterator != localSeedHistory.end() && remoteSeedIterator != remoteSeedHistory.end(); ++localSeedIterator, ++remoteSeedIterator, ++callNumber)
			{
				if(*localSeedIterator != *remoteSeedIterator)
				{
					if(callNumber < localCallStacks.size() && callNumber < remoteCallStacks.size())
					{
						rngLogMessage += "At Call #";
						rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
						rngLogMessage += " random number seeds are different.\n";
						if(callNumber > 0)
						{
							rngLogMessage += "Call #";
							rngLogMessage += _itoa_s(callNumber - 1, formatBuf, 10);
							rngLogMessage += "\nLocal:\n";
							rngLogMessage += localCallStacks[callNumber - 1];
							rngLogMessage += "\nRemote:\n";
							rngLogMessage += remoteCallStacks[callNumber - 1];
						}
						rngLogMessage += "Call #";
						rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
						rngLogMessage += "\nLocal:\n";
						rngLogMessage += localCallStacks[callNumber];
						rngLogMessage += "\nRemote:\n";
						rngLogMessage += remoteCallStacks[callNumber];
						break;
					}
				}
			}
			if(localSeedIterator != localSeedHistory.end() && remoteSeedIterator == remoteSeedHistory.end())
			{
				rngLogMessage += "\nLocal random number generator called more than remote at call #";
				rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
				rngLogMessage += "\n";
				if(callNumber < localSeedHistory.size())
				{
					rngLogMessage += "Local CallStack:\n";
					rngLogMessage += localCallStacks[callNumber];
				}
			}
			else if(remoteSeedIterator != remoteSeedHistory.end() && localSeedIterator == localSeedHistory.end())
			{
				rngLogMessage += "\nremote random number generator called more than local at call #";
				rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
				rngLogMessage += "\n";
				if(callNumber < remoteSeedHistory.size())
				{
					rngLogMessage += "remote CallStack:\n";
					rngLogMessage += remoteCallStacks[callNumber];
				}
			}

			// find first different call
			std::vector<std::string>::const_iterator localCallStackIterator;
			std::vector<std::string>::const_iterator remoteCallStackIterator;
			for(localCallStackIterator = localCallStacks.begin(), remoteCallStackIterator = remoteCallStacks.begin(), callNumber = 0; localCallStackIterator != localCallStacks.end() && remoteCallStackIterator != remoteCallStacks.end(); ++localCallStackIterator, ++remoteCallStackIterator, ++callNumber)
			{
				if(*localCallStackIterator != *remoteCallStackIterator)
				{
					rngLogMessage += "\nFirst different callstack at call #";
					rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
					rngLogMessage += "\n";
					rngLogMessage += "Local :\n";
					rngLogMessage += *localCallStackIterator;
					rngLogMessage += "Remote :\n";
					rngLogMessage += *remoteCallStackIterator;
					break;
				}
			}

			if(logFile)
			{
				logFile->DebugMsg(rngLogMessage.c_str());
			}

			CvAssertMsg(false, rngLogMessage.c_str());

			if(logFile)
			{
				rngLogMessage = "\nDebug dump:\n";


				// add full history
				rngLogMessage += "Seed History -\n";
				localSeedIterator = localSeedHistory.begin();
				remoteSeedIterator = remoteSeedHistory.begin();
				callNumber = 0;
				while(localSeedIterator != localSeedHistory.end() || remoteSeedIterator != remoteSeedHistory.end())
				{
					++callNumber;
					rngLogMessage += "\t Call #";
					rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
					rngLogMessage += " LOCAL=";
					if(localSeedIterator != localSeedHistory.end())
					{
						rngLogMessage += _itoa_s(*localSeedIterator, formatBuf, 10);
						++localSeedIterator;
					}
					rngLogMessage += "\tREMOTE=";
					if(remoteSeedIterator != remoteSeedHistory.end())
					{
						rngLogMessage += _itoa_s(*remoteSeedIterator, formatBuf, 10);
						++remoteSeedIterator;
					}
					rngLogMessage += "\n";
				}

				rngLogMessage += "\nLocal callstack history\n";
				callNumber = 0;
				std::string lastCallStackLogged;
				unsigned int repeatCount = 0;
				std::vector<std::string>::const_iterator i;
				for(i = localCallStacks.begin(); i != localCallStacks.end(); ++i)
				{
					if(lastCallStackLogged != *i)
					{
						if(repeatCount > 0)
						{
							rngLogMessage += "Last message repeated ";
							rngLogMessage += _itoa_s(repeatCount, formatBuf, 10);
							rngLogMessage += " times\n";
						}
						rngLogMessage += "Call #";
						rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
						rngLogMessage += "\n";
						rngLogMessage += *i;
						lastCallStackLogged = *i;
						repeatCount = 0;
					}
					else
					{
						repeatCount++;
					}
					++callNumber;
				}

				rngLogMessage += "\nRemote callstack history\n";
				callNumber = 0;
				repeatCount = 0;
				for(i = remoteCallStacks.begin(); i != remoteCallStacks.end(); ++i)
				{
					if(lastCallStackLogged != *i)
					{
						if(repeatCount > 0)
						{
							rngLogMessage += "Last message repeated ";
							rngLogMessage += _itoa_s(repeatCount, formatBuf, 10);
							rngLogMessage += " times\n";
						}
						rngLogMessage += "Call #";
						rngLogMessage += _itoa_s(callNumber, formatBuf, 10);
						rngLogMessage += "\n";
						rngLogMessage += *i;
						lastCallStackLogged = *i;
						repeatCount = 0;
					}
					else
					{
						repeatCount++;
					}
					++callNumber;
				}
				logFile->DebugMsg(rngLogMessage.c_str());
			}

		}
		else
		{
			if(logFile)
			{
				logFile->DebugMsg(rngLogMessage.c_str());
			}
			CvAssertMsg(false, rngLogMessage.c_str());
		}


		/* If this point has been reached, the OOS may be nearly unrecoverable
		// attempt to recover
		if(m_isHost)
		{
			CvRandom & rng = auto_ptr<ICvGame1> pGame = GameCore::GetGame();\n.getJonRand();
			rng.reset(m_simRandomNumberGenerator.getSeed());
		}
		// brute force approach
		// JAR : todo - this is the solution of last resort if
		// loadDelta() doesn't work out
		*/
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameContext::CreateRandomNumberGenerator()
{
	uint index = m_uiRngCounter++;
	std::pair<uint, CvRandom*> entry(index, FNEW(CvRandom, c_eCiv5GameplayDLL, 0));

	m_RandomNumberGenerators.push_back(entry);
	return index;
}
//------------------------------------------------------------------------------
ICvRandom1* CvDllGameContext::GetRandomNumberGenerator(unsigned int index)
{
	for(std::vector<std::pair<uint, CvRandom*> >::iterator it = m_RandomNumberGenerators.begin();
		it != m_RandomNumberGenerators.end(); ++it)
	{
		if((*it).first == index)
			return new CvDllRandom((*it).second);
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllGameContext::DestroyRandomNumberGenerator(unsigned int index)
{
	std::vector<std::pair<uint, CvRandom*> >::iterator it = m_RandomNumberGenerators.end();
	for(it = m_RandomNumberGenerators.begin();
		it != m_RandomNumberGenerators.end(); ++it)
	{
		if((*it).first == index)
			break;
	}

	if(it != m_RandomNumberGenerators.end())
	{
		delete (*it).second;
		m_RandomNumberGenerators.erase(it);
	}
}
//------------------------------------------------------------------------------
unsigned int CvDllGameContext::CreateNetInitInfo()
{
	uint index = m_uiNetInitInfoCounter++;
	std::pair<uint, CvDllNetInitInfo*> entry(index, new CvDllNetInitInfo());

	m_NetInitInfos.push_back(entry);
	return index;
}
//------------------------------------------------------------------------------
ICvNetInitInfo1* CvDllGameContext::GetNetInitInfo(unsigned int index)
{
	for(std::vector<std::pair<uint, CvDllNetInitInfo*> >::iterator it = m_NetInitInfos.begin();
		it != m_NetInitInfos.end(); ++it)
	{
		if((*it).first == index)
		{
			(*it).second->IncrementReference();
			return (*it).second;
		}
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllGameContext::DestroyNetInitInfo(unsigned int index)
{
	std::vector<std::pair<uint, CvDllNetInitInfo*> >::iterator it = m_NetInitInfos.end();
	for(it = m_NetInitInfos.begin();
		it != m_NetInitInfos.end(); ++it)
	{
		if((*it).first == index)
			break;
	}

	if(it != m_NetInitInfos.end())
	{
		delete (*it).second;
		m_NetInitInfos.erase(it);
	}
}
//------------------------------------------------------------------------------
unsigned int CvDllGameContext::CreateNetLoadGameInfo()
{
	uint index = m_uiNetLoadGameInfoCounter++;
	std::pair<uint, CvDllNetLoadGameInfo*> entry(index, new CvDllNetLoadGameInfo());

	m_NetLoadGameInfos.push_back(entry);
	return index;
}
//------------------------------------------------------------------------------
ICvNetLoadGameInfo1* CvDllGameContext::GetNetLoadGameInfo(unsigned int index)
{
	for(std::vector<std::pair<uint, CvDllNetLoadGameInfo*> >::iterator it = m_NetLoadGameInfos.begin();
		it != m_NetLoadGameInfos.end(); ++it)
	{
		if((*it).first == index)
		{
			(*it).second->IncrementReference();
			return (*it).second;
		}
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllGameContext::DestroyNetLoadGameInfo(unsigned int index)
{
	std::vector<std::pair<uint, CvDllNetLoadGameInfo*> >::iterator it = m_NetLoadGameInfos.end();
	for(it = m_NetLoadGameInfos.begin();
		it != m_NetLoadGameInfos.end(); ++it)
	{
		if((*it).first == index)
			break;
	}

	if(it != m_NetLoadGameInfos.end())
	{
		delete (*it).second;
		m_NetLoadGameInfos.erase(it);
	}
}
//------------------------------------------------------------------------------
void CvDllGameContext::TEMPOnHexUnitChanged(ICvUnit1* pUnit)
{
	CvUnit* pkUnit = GC.UnwrapUnitPointer(pUnit);
	CvTwoLayerPathFinder& thePathfinder = GC.getInterfacePathFinder();

	// change the unit pathfinder to use these funcs instead
	thePathfinder.SetDestValidFunc(NULL);
	thePathfinder.SetValidFunc(UIPathValid);
	thePathfinder.SetNotifyChildFunc(UIPathAdd);

	// call the pathfinder
	thePathfinder.SetData(pkUnit);
	bool bCanFindPath = thePathfinder.GeneratePath(pkUnit->getX(), pkUnit->getY(), -1, -1, MOVE_DECLARE_WAR, false);

	// change the unit pathfinder back
	thePathfinder.SetDestValidFunc(PathDestValid);
	thePathfinder.SetValidFunc(PathValid);
	thePathfinder.SetNotifyChildFunc(PathAdd);
	thePathfinder.ForceReset();

}
//------------------------------------------------------------------------------
void CvDllGameContext::TEMPOnHexUnitChangedAttack(ICvUnit1* pUnit)
{
	CvUnit* pkUnit = GC.UnwrapUnitPointer(pUnit);
	CvTwoLayerPathFinder& thePathfinder = GC.getInterfacePathFinder();

	// change the unit pathfinder to use these funcs instead
	thePathfinder.SetDestValidFunc(NULL);
	thePathfinder.SetValidFunc(UIPathValid);
	thePathfinder.SetNotifyChildFunc(AttackPathAdd);

	// call the pathfinder
	thePathfinder.SetData(pkUnit);
	bool bCanFindPath = thePathfinder.GeneratePath(pkUnit->getX(), pkUnit->getY(), -1, -1, MOVE_DECLARE_WAR, false);

	// change the unit pathfinder back
	thePathfinder.SetDestValidFunc(PathDestValid);
	thePathfinder.SetValidFunc(PathValid);
	thePathfinder.SetNotifyChildFunc(PathAdd);
	thePathfinder.ForceReset();
}
//------------------------------------------------------------------------------
ICvEnumerator* CvDllGameContext::TEMPCalculatePathFinderUpdates(ICvUnit1* pHeadSelectedUnit, int iMouseMapX, int iMouseMapY)
{
	CvUnit* pkUnit = GC.UnwrapUnitPointer(pHeadSelectedUnit);
	CvTwoLayerPathFinder& thePathfinder = GC.getInterfacePathFinder();

	thePathfinder.SetData(pkUnit);
	if(thePathfinder.GeneratePath(pkUnit->getX(), pkUnit->getY(), iMouseMapX, iMouseMapY, MOVE_DECLARE_WAR, false))
	{
		// seed the pathfinder with a unit
		thePathfinder.SetData(pkUnit);

		//get the number of waypoints on the path
		CvAStarNode *lastNode = thePathfinder.GetLastNode();
		CvAStarNode *pathNode = lastNode;

		pathNode = lastNode;
		int size = 0;
		while (pathNode != NULL)
		{
			size++;
			pathNode = pathNode->m_pParent;
		}


		std::vector<CvDllPathFinderUpdateListData> pUpdateData;

		pUpdateData.reserve(size);

		// now fill out the event array in reverse order
		pathNode = lastNode;
		int index = 0;
		while (pathNode != NULL)
		{
			CvDllPathFinderUpdateListData update;
			update.iX = pathNode->m_iX;
			update.iY = pathNode->m_iY;
			update.iTurnNumber = pathNode->m_iData2;

			pUpdateData.push_back(update);

			index++;
			pathNode = pathNode->m_pParent;
		}

		std::reverse(pUpdateData.begin(), pUpdateData.end());
		return new CvDllPathFinderUpdateList(pUpdateData);
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllGameContext::ResetPathFinder()
{
	GC.getInterfacePathFinder().ForceReset();
}
//------------------------------------------------------------------------------
void CvDllGameContext::SetEngineUserInterface(ICvUserInterface2* pUI)
{
	GC.SetEngineUserInterface(pUI);
}
//------------------------------------------------------------------------------