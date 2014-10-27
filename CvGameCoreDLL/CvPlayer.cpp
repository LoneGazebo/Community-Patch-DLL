/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"
#include "CvAStar.h"
#include "CvGameTextMgr.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvCitySpecializationAI.h"
#include "CvWonderProductionAI.h"
#include "CvGrandStrategyAI.h"
#include "CvDiplomacyAI.h"
#include "CvTechAI.h"
#include "CvFlavorManager.h"
#include "CvHomelandAI.h"
#include "CvMinorCivAI.h"
#include "CvDealAI.h"
#include "CvImprovementClasses.h"
#include "CvBuilderTaskingAI.h"
#include "CvDangerPlots.h"
#include "CvCityConnections.h"
#include "CvNotifications.h"
#include "CvDiplomacyRequests.h"
#include "cvStopWatch.h"
#include "CvTypes.h"

#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include <sstream>

#include "CvInternalGameCoreUtils.h"
#include "CvAchievementUnlocker.h"
#include "CvInfosSerializationHelper.h"

#include "CvDllCity.h"
#include "CvPopupInfoSerialization.h"
#include "CvGoodyHuts.h"

// Include this after all other headers.
#define LINT_WARNINGS_ONLY
#include "LintFree.h"

//------------------------------------------------------------------------------
// CvPlayer Version History
// Version 25 
//	 * Added m_bDynamicTurnsSimultMode for simultaneous/sequential turn toggling in dynamic turn mode.
// Version 24 
//	 * Gods & Kings released version (as of expansion 2 development start).
//------------------------------------------------------------------------------
const int g_CurrentCvPlayerVersion = 25;

//Simply empty check utility.
bool isEmpty(const char* szString)
{
	return szString == NULL || szString[0] == '\0';
}

//	--------------------------------------------------------------------------------
// Public Functions...
namespace FSerialization
{
	void SyncPlayer()
	{
		if(GC.getGame().isNetworkMultiPlayer())
		{
			PlayerTypes eAuthoritativePlayerID = GC.getGame().getActivePlayer();
			CvPlayer & authoritativePlayer = GET_PLAYER(eAuthoritativePlayerID);
			const FAutoArchive & archive = authoritativePlayer.getSyncArchive();
			if(archive.hasDeltas())
			{
				FMemoryStream ms;
				std::vector<std::pair<std::string, std::string> > callStacks;
				archive.saveDelta(ms, callStacks);
				gDLL->sendPlayerSyncCheck(eAuthoritativePlayerID, ms, callStacks);
			}

			// host is authoritative for AI players

			if(gDLL->IsHost())
			{
				for(int i = 0; i < MAX_PLAYERS; ++i)
				{
					CvPlayer & player = GET_PLAYER(static_cast<PlayerTypes>(i));
					if(!player.isHuman() && player.isAlive())
					{
						const FAutoArchive & aiArchive = player.getSyncArchive();
						FMemoryStream ms;
						std::vector<std::pair<std::string, std::string> > callStacks;
						aiArchive.saveDelta(ms, callStacks);
						gDLL->sendPlayerSyncCheck(static_cast<PlayerTypes>(i), ms, callStacks);
					}
				}
			}
		}
	}

	// clears ALL deltas for ALL players
	void ClearPlayerDeltas()
	{
		int i = 0;
		for(i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer & player = GET_PLAYER(static_cast<PlayerTypes>(i));
			FAutoArchive & archive = player.getSyncArchive();
			archive.clearDelta();
		}
	}
}

//	--------------------------------------------------------------------------------
CvPlayer::CvPlayer() :
m_syncArchive(*this)
, m_iStartingX("CvPlayer::m_iStartingX", m_syncArchive)
, m_iStartingY("CvPlayer::m_iStartingY", m_syncArchive)
, m_iTotalPopulation("CvPlayer::m_iTotalPopulation", m_syncArchive, true)
, m_iTotalLand("CvPlayer::m_iTotalLand", m_syncArchive)
, m_iTotalLandScored("CvPlayer::m_iTotalLandScored", m_syncArchive)
, m_iJONSCulturePerTurnForFree("CvPlayer::m_iJONSCulturePerTurnForFree", m_syncArchive)
, m_iJONSCulturePerTurnFromMinorCivs("CvPlayer::m_iJONSCulturePerTurnFromMinorCivs", m_syncArchive)
, m_iJONSCultureCityModifier("CvPlayer::m_iJONSCultureCityModifier", m_syncArchive)
, m_iJONSCulture("CvPlayer::m_iJONSCulture", m_syncArchive, true)
, m_iJONSCultureEverGenerated("CvPlayer::m_iJONSCulture", m_syncArchive)
, m_iCulturePerWonder("CvPlayer::m_iCulturePerWonder", m_syncArchive)
, m_iCultureWonderMultiplier("CvPlayer::m_iCultureWonderMultiplier", m_syncArchive)
, m_iCulturePerTechResearched("CvPlayer::m_iCulturePerTechResearched", m_syncArchive)
, m_iHappiness("CvPlayer::m_iHappiness", m_syncArchive)
, m_iRevoltCounter("CvPlayer::m_iRevoltCounter", m_syncArchive)
, m_iExtraHappinessPerLuxury("CvPlayer::m_iExtraHappinessPerLuxury", m_syncArchive)
, m_iUnhappinessFromUnits("CvPlayer::m_iUnhappinessFromUnits", m_syncArchive)
, m_iUnhappinessFromUnitsMod("CvPlayer::m_iUnhappinessFromUnitsMod", m_syncArchive)
, m_iUnhappinessMod("CvPlayer::m_iUnhappinessMod", m_syncArchive)
, m_iCityCountUnhappinessMod("CvPlayer::m_iCityCountUnhappinessMod", m_syncArchive)
, m_iOccupiedPopulationUnhappinessMod("CvPlayer::m_iOccupiedPopulationUnhappinessMod", m_syncArchive)
, m_iCapitalUnhappinessMod("CvPlayer::m_iCapitalUnhappinessMod", m_syncArchive)
, m_iHappinessFromGarrisonedUnits("CvPlayer::m_iHappinessFromGarrisonedUnits", m_syncArchive)
, m_iHappinessPerGarrisonedUnitCount("CvPlayer::m_iHappinessPerGarrisonedUnitCount", m_syncArchive)
, m_iHappinessPerTradeRouteCount("CvPlayer::m_iHappinessPerTradeRouteCount", m_syncArchive)
, m_iHappinessPerXPopulation(0)
, m_iSpecialPolicyBuildingHappiness("CvPlayer::m_iSpecialPolicyBuildingHappiness", m_syncArchive)
, m_iWoundedUnitDamageMod("CvPlayer::m_iWoundedUnitDamageMod", m_syncArchive)
, m_iUnitUpgradeCostMod("CvPlayer::m_iUnitUpgradeCostMod", m_syncArchive)
, m_iBarbarianCombatBonus("CvPlayer::m_iBarbarianCombatBonus", m_syncArchive)
, m_iAlwaysSeeBarbCampsCount("CvPlayer::m_iAlwaysSeeBarbCampsCount", m_syncArchive)
, m_iHappinessFromBuildings("CvPlayer::m_iHappinessFromBuildings", m_syncArchive)
, m_iHappinessPerCity("CvPlayer::m_iHappinessPerCity", m_syncArchive)
, m_iHappinessPerXPolicies(0)
, m_iAdvancedStartPoints("CvPlayer::m_iAdvancedStartPoints", m_syncArchive)
, m_iAttackBonusTurns("CvPlayer::m_iAttackBonusTurns", m_syncArchive)
, m_iGoldenAgeProgressMeter("CvPlayer::m_iGoldenAgeProgressMeter", m_syncArchive, true)
, m_iGoldenAgeMeterMod("CvPlayer::m_iGoldenAgeMeterMod", m_syncArchive)
, m_iNumGoldenAges("CvPlayer::m_iNumGoldenAges", m_syncArchive)
, m_iGoldenAgeTurns("CvPlayer::m_iGoldenAgeTurns", m_syncArchive)
, m_iNumUnitGoldenAges("CvPlayer::m_iNumUnitGoldenAges", m_syncArchive)
, m_iStrikeTurns("CvPlayer::m_iStrikeTurns", m_syncArchive)
, m_iGoldenAgeModifier("CvPlayer::m_iGoldenAgeModifier", m_syncArchive)
, m_iGreatPeopleCreated("CvPlayer::m_iGreatPeopleCreated", m_syncArchive)
, m_iGreatGeneralsCreated("CvPlayer::m_iGreatGeneralsCreated", m_syncArchive)
, m_iGreatPeopleThresholdModifier("CvPlayer::m_iGreatPeopleThresholdModifier", m_syncArchive)
, m_iGreatGeneralsThresholdModifier("CvPlayer::m_iGreatGeneralsThresholdModifier", m_syncArchive)
, m_iAnarchyNumTurns("CvPlayer::m_iAnarchyNumTurns", m_syncArchive)
, m_iPolicyCostModifier("CvPlayer::m_iPolicyCostModifier", m_syncArchive)
, m_iGreatPeopleRateModifier("CvPlayer::m_iGreatPeopleRateModifier", m_syncArchive)
, m_iGreatPeopleRateModFromBldgs("CvPlayer::m_iGreatPeopleRateModFromBldgs", m_syncArchive)
, m_iGreatGeneralRateModifier("CvPlayer::m_iGreatGeneralRateModifier", m_syncArchive)
, m_iGreatGeneralRateModFromBldgs("CvPlayer::m_iGreatGeneralRateModFromBldgs", m_syncArchive)
, m_iDomesticGreatGeneralRateModifier("CvPlayer::m_iDomesticGreatGeneralRateModifier", m_syncArchive)
, m_iDomesticGreatGeneralRateModFromBldgs("CvPlayer::m_iDomesticGreatGeneralRateModFromBldgs", m_syncArchive)
, m_iGreatPersonExpendGold(0)
, m_iMaxGlobalBuildingProductionModifier("CvPlayer::m_iMaxGlobalBuildingProductionModifier", m_syncArchive)
, m_iMaxTeamBuildingProductionModifier("CvPlayer::m_iMaxTeamBuildingProductionModifier", m_syncArchive)
, m_iMaxPlayerBuildingProductionModifier("CvPlayer::m_iMaxPlayerBuildingProductionModifier", m_syncArchive)
, m_iFreeExperience("CvPlayer::m_iFreeExperience", m_syncArchive)
, m_iFreeExperienceFromBldgs("CvPlayer::m_iFreeExperienceFromBldgs", m_syncArchive)
, m_iFreeExperienceFromMinors("CvPlayer::m_iFreeExperienceFromMinors", m_syncArchive)
, m_iFeatureProductionModifier("CvPlayer::m_iFeatureProductionModifier", m_syncArchive)
, m_iWorkerSpeedModifier("CvPlayer::m_iWorkerSpeedModifier", m_syncArchive)
, m_iImprovementCostModifier("CvPlayer::m_iImprovementCostModifier", m_syncArchive)
, m_iImprovementUpgradeRateModifier("CvPlayer::m_iImprovementUpgradeRateModifier", m_syncArchive)
, m_iSpecialistProductionModifier("CvPlayer::m_iSpecialistProductionModifier", m_syncArchive)
, m_iMilitaryProductionModifier("CvPlayer::m_iMilitaryProductionModifier", m_syncArchive)
, m_iSpaceProductionModifier("CvPlayer::m_iSpaceProductionModifier", m_syncArchive)
, m_iCityDefenseModifier("CvPlayer::m_iCityDefenseModifier", m_syncArchive)
, m_iWonderProductionModifier("CvPlayer::m_iWonderProductionModifier", m_syncArchive)
, m_iSettlerProductionModifier("CvPlayer::m_iSettlerProductionModifier", m_syncArchive)
, m_iCapitalSettlerProductionModifier("CvPlayer::m_iCapitalSettlerProductionModifier", m_syncArchive)
, m_iUnitProductionMaintenanceMod("CvPlayer::m_iUnitProductionMaintenanceMod", m_syncArchive)
, m_iPolicyCostBuildingModifier("CvPlayer::m_iPolicyCostBuildingModifier", m_syncArchive)
, m_iPolicyCostMinorCivModifier("CvPlayer::m_iPolicyCostMinorCivModifier", m_syncArchive)
, m_iNumNukeUnits("CvPlayer::m_iNumNukeUnits", m_syncArchive)
, m_iNumOutsideUnits("CvPlayer::m_iNumOutsideUnits", m_syncArchive, true)
, m_iBaseFreeUnits("CvPlayer::m_iBaseFreeUnits", m_syncArchive)
, m_iBaseFreeMilitaryUnits("CvPlayer::m_iBaseFreeMilitaryUnits", m_syncArchive)
, m_iFreeUnitsPopulationPercent("CvPlayer::m_iFreeUnitsPopulationPercent", m_syncArchive)
, m_iFreeMilitaryUnitsPopulationPercent("CvPlayer::m_iFreeMilitaryUnitsPopulationPercent", m_syncArchive)
, m_iGoldPerUnit("CvPlayer::m_iGoldPerUnit", m_syncArchive)
, m_iGoldPerMilitaryUnit("CvPlayer::m_iGoldPerMilitaryUnit", m_syncArchive)
, m_iRouteGoldMaintenanceMod("CvPlayer::m_iRouteGoldMaintenanceMod", m_syncArchive)
, m_iBuildingGoldMaintenanceMod("CvPlayer::m_iBuildingGoldMaintenanceMod", m_syncArchive)
, m_iUnitGoldMaintenanceMod("CvPlayer::m_iUnitGoldMaintenanceMod", m_syncArchive)
, m_iUnitSupplyMod("CvPlayer::m_iUnitSupplyMod", m_syncArchive)
, m_iExtraUnitCost("CvPlayer::m_iExtraUnitCost", m_syncArchive)
, m_iNumMilitaryUnits("CvPlayer::m_iNumMilitaryUnits", m_syncArchive)
, m_iHappyPerMilitaryUnit("CvPlayer::m_iHappyPerMilitaryUnit", m_syncArchive)
, m_iHappinessToCulture("CvPlayer::m_iHappinessToCulture", m_syncArchive)
, m_iHappinessToScience("CvPlayer::m_iHappinessToScience", m_syncArchive)
, m_iHalfSpecialistUnhappinessCount("CvPlayer::m_iHalfSpecialistUnhappinessCount", m_syncArchive)
, m_iHalfSpecialistFoodCount("CvPlayer::m_iHalfSpecialistFoodCount", m_syncArchive)
, m_iMilitaryFoodProductionCount("CvPlayer::m_iMilitaryFoodProductionCount", m_syncArchive)
, m_iConscriptCount("CvPlayer::m_iConscriptCount", m_syncArchive)
, m_iMaxConscript("CvPlayer::m_iMaxConscript", m_syncArchive)
, m_iHighestUnitLevel("CvPlayer::m_iHighestUnitLevel", m_syncArchive)
, m_iOverflowResearch("CvPlayer::m_iOverflowResearch", m_syncArchive, true)
, m_iExpModifier("CvPlayer::m_iExpModifier", m_syncArchive)
, m_iExpInBorderModifier("CvPlayer::m_iExpInBorderModifier", m_syncArchive)
, m_iLevelExperienceModifier("CvPlayer::m_iLevelExperienceModifier", m_syncArchive)
, m_iMinorQuestFriendshipMod("CvPlayer::m_iMinorQuestFriendshipMod", m_syncArchive)
, m_iMinorGoldFriendshipMod("CvPlayer::m_iMinorGoldFriendshipMod", m_syncArchive)
, m_iMinorFriendshipMinimum("CvPlayer::m_iMinorFriendshipMinimum", m_syncArchive)
, m_iMinorFriendshipDecayMod("CvPlayer::m_iMinorFriendshipDecayMod", m_syncArchive)
, m_iMinorScienceAlliesCount("CvPlayer::m_iMinorScienceAlliesCount", m_syncArchive)
, m_iMinorResourceBonusCount("CvPlayer::m_iMinorResourceBonusCount", m_syncArchive)
, m_iFreeSpecialist("CvPlayer::m_iFreeSpecialist", m_syncArchive)
, m_iCultureBombTimer("CvPlayer::m_iCultureBombTimer", m_syncArchive)
, m_iConversionTimer("CvPlayer::m_iConversionTimer", m_syncArchive)
, m_iCapitalCityID("CvPlayer::m_iCapitalCityID", m_syncArchive)
, m_iCitiesLost("CvPlayer::m_iCitiesLost", m_syncArchive)
, m_iMilitaryMight("CvPlayer::m_iMilitaryMight", m_syncArchive)
, m_iEconomicMight("CvPlayer::m_iEconomicMight", m_syncArchive)
, m_iTurnMightRecomputed("CvPlayer::m_iTurnMightRecomputed", m_syncArchive)
, m_iNewCityExtraPopulation("CvPlayer::m_iNewCityExtraPopulation", m_syncArchive)
, m_iFreeFoodBox("CvPlayer::m_iFreeFoodBox", m_syncArchive)
, m_iPopulationScore("CvPlayer::m_iPopulationScore", m_syncArchive)
, m_iLandScore("CvPlayer::m_iLandScore", m_syncArchive)
, m_iTechScore("CvPlayer::m_iTechScore", m_syncArchive)
, m_iWondersScore("CvPlayer::m_iWondersScore", m_syncArchive)
, m_iScoreFromFutureTech("CvPlayer::m_iScoreFromFutureTech", m_syncArchive)
, m_iCombatExperience("CvPlayer::m_iCombatExperience", m_syncArchive)
, m_iLifetimeCombatExperience(0)
, m_iPopRushHurryCount("CvPlayer::m_iPopRushHurryCount", m_syncArchive)
, m_iTotalImprovementsBuilt("CvPlayer::m_iTotalImprovementsBuilt", m_syncArchive)
, m_iNextOperationID("CvPlayer::m_iNextOperationID", m_syncArchive)
, m_iCostNextPolicy("CvPlayer::m_iCostNextPolicy", m_syncArchive)
, m_iNumBuilders("CvPlayer::m_iNumBuilders", m_syncArchive, true)
, m_iMaxNumBuilders("CvPlayer::m_iMaxNumBuilders", m_syncArchive)
, m_iCityStrengthMod("CvPlayer::m_iCityStrengthMod", m_syncArchive)
, m_iCityGrowthMod("CvPlayer::m_iCityGrowthMod", m_syncArchive)
, m_iCapitalGrowthMod("CvPlayer::m_iCapitalGrowthMod", m_syncArchive)
, m_iNumPlotsBought("CvPlayer::m_iNumPlotsBought", m_syncArchive)
, m_iPlotGoldCostMod("CvPlayer::m_iPlotGoldCostMod", m_syncArchive)
, m_iPlotCultureCostModifier("CvPlayer::m_iPlotCultureCostModifier", m_syncArchive)
, m_iPlotCultureExponentModifier(0)
, m_iNumCitiesPolicyCostDiscount(0)
, m_iGarrisonedCityRangeStrikeModifier(0)
, m_iGarrisonFreeMaintenanceCount(0)
, m_iNumCitiesFreeCultureBuilding(0)
, m_iUnitPurchaseCostModifier("CvPlayer::m_iUnitPurchaseCostModifier", m_syncArchive)
, m_iAllFeatureProduction("CvPlayer::m_iAllFeatureProduction", m_syncArchive)
, m_iCityDistanceHighwaterMark("CvPlayer::m_iCityDistanceHighwaterMark", m_syncArchive)
, m_iOriginalCapitalX("CvPlayer::m_iOriginalCapitalX", m_syncArchive)
, m_iOriginalCapitalY("CvPlayer::m_iOriginalCapitalY", m_syncArchive)
, m_iNumWonders("CvPlayer::m_iNumWonders", m_syncArchive)
, m_iNumPolicies("CvPlayer::m_iNumPolicies", m_syncArchive)
, m_iNumGreatPeople("CvPlayer::m_iNumGreatPeople", m_syncArchive)
, m_uiStartTime("CvPlayer::m_uiStartTime", m_syncArchive)  // XXX save these?
, m_bHasBetrayedMinorCiv("CvPlayer::m_bHasBetrayedMinorCiv", m_syncArchive)
, m_bAlive("CvPlayer::m_bAlive", m_syncArchive)
, m_bEverAlive("CvPlayer::m_bEverAlive", m_syncArchive)
, m_bTurnActive("CvPlayer::m_bTurnActive", m_syncArchive, false, true)
, m_bAutoMoves("CvPlayer::m_bAutoMoves", m_syncArchive, false, true)
, m_bEndTurn("CvPlayer::m_bEndTurn", m_syncArchive, false, true)
, m_bDynamicTurnsSimultMode(true)
, m_bPbemNewTurn("CvPlayer::m_bPbemNewTurn", m_syncArchive)
, m_bExtendedGame("CvPlayer::m_bExtendedGame", m_syncArchive)
, m_bFoundedFirstCity("CvPlayer::m_bFoundedFirstCity", m_syncArchive)
, m_bStrike("CvPlayer::m_bStrike", m_syncArchive)
, m_bCramped("CvPlayer::m_bCramped", m_syncArchive)
, m_bLostCapital("CvPlayer::m_bLostCapital", m_syncArchive)
, m_eConqueror(NO_PLAYER)
, m_bHasAdoptedStateReligion("CvPlayer::m_bHasAdoptedStateReligion", m_syncArchive)
, m_bAlliesGreatPersonBiasApplied("CvPlayer::m_bAlliesGreatPersonBiasApplied", m_syncArchive)
, m_eID("CvPlayer::m_eID", m_syncArchive)
, m_ePersonalityType("CvPlayer::m_ePersonalityType", m_syncArchive)
, m_aiCityYieldChange("CvPlayer::m_aiCityYieldChange", m_syncArchive)
, m_aiCoastalCityYieldChange("CvPlayer::m_aiCoastalCityYieldChange", m_syncArchive)
, m_aiCapitalYieldChange("CvPlayer::m_aiCapitalYieldChange", m_syncArchive)
, m_aiCapitalYieldPerPopChange("CvPlayer::m_aiCapitalYieldPerPopChange", m_syncArchive)
, m_aiSeaPlotYield("CvPlayer::m_aiSeaPlotYield", m_syncArchive)
, m_aiYieldRateModifier("CvPlayer::m_aiYieldRateModifier", m_syncArchive)
, m_aiCapitalYieldRateModifier("CvPlayer::m_aiCapitalYieldRateModifier", m_syncArchive)
, m_aiExtraYieldThreshold("CvPlayer::m_aiExtraYieldThreshold", m_syncArchive)
, m_aiSpecialistExtraYield("CvPlayer::m_aiSpecialistExtraYield", m_syncArchive)
, m_aiProximityToPlayer("CvPlayer::m_aiProximityToPlayer", m_syncArchive, true)
, m_aiResearchAgreementCounter("CvPlayer::m_aiResearchAgreementCounter", m_syncArchive)
, m_aiIncomingUnitTypes("CvPlayer::m_aiIncomingUnitTypes", m_syncArchive, true)
, m_aiIncomingUnitCountdowns("CvPlayer::m_aiIncomingUnitCountdowns", m_syncArchive, true)
, m_aOptions("CvPlayer::m_aOptions", m_syncArchive)
, m_strReligionKey("CvPlayer::m_strReligionKey", m_syncArchive)
, m_strScriptData("CvPlayer::m_strScriptData", m_syncArchive)
, m_paiNumResourceUsed("CvPlayer::m_paiNumResourceUsed", m_syncArchive)
, m_paiNumResourceTotal("CvPlayer::m_paiNumResourceTotal", m_syncArchive)
, m_paiResourceGiftedToMinors("CvPlayer::m_paiResourceGiftedToMinors", m_syncArchive)
, m_paiResourceExport("CvPlayer::m_paiResourceExport", m_syncArchive)
, m_paiResourceImport("CvPlayer::m_paiResourceImport", m_syncArchive)
, m_paiResourceFromMinors("CvPlayer::m_paiResourceFromMinors", m_syncArchive)
, m_paiImprovementCount("CvPlayer::m_paiImprovementCount", m_syncArchive)
, m_paiFreeBuildingCount("CvPlayer::m_paiFreeBuildingCount", m_syncArchive)
, m_paiFreePromotionCount("CvPlayer::m_paiFreePromotionCount", m_syncArchive)
, m_paiUnitCombatProductionModifiers("CvPlayer::m_paiUnitCombatProductionModifiers", m_syncArchive)
, m_paiUnitCombatFreeExperiences("CvPlayer::m_paiUnitCombatFreeExperiences", m_syncArchive)
, m_paiUnitClassCount("CvPlayer::m_paiUnitClassCount", m_syncArchive, true)
, m_paiUnitClassMaking("CvPlayer::m_paiUnitClassMaking", m_syncArchive, true)
, m_paiBuildingClassCount("CvPlayer::m_paiBuildingClassCount", m_syncArchive)
, m_paiBuildingClassMaking("CvPlayer::m_paiBuildingClassMaking", m_syncArchive, true)
, m_paiProjectMaking("CvPlayer::m_paiProjectMaking", m_syncArchive)
, m_paiHurryCount("CvPlayer::m_paiHurryCount", m_syncArchive)
, m_paiHurryModifier("CvPlayer::m_paiHurryModifier", m_syncArchive)
, m_pabLoyalMember("CvPlayer::m_pabLoyalMember", m_syncArchive)
, m_pabGetsScienceFromPlayer("CvPlayer::m_pabGetsScienceFromPlayer", m_syncArchive)
, m_ppaaiSpecialistExtraYield("CvPlayer::m_ppaaiSpecialistExtraYield", m_syncArchive)
, m_ppaaiImprovementYieldChange("CvPlayer::m_ppaaiImprovementYieldChange", m_syncArchive)
, m_ppaaiBuildingClassYieldMod("CvPlayer::m_ppaaiBuildingClassYieldMod", m_syncArchive)
, m_UnitCycle(this)
, m_bEverPoppedGoody("CvPlayer::m_bEverPoppedGoody", m_syncArchive)
, m_bEverTrainedBuilder("CvPlayer::m_bEverTrainedBuilder", m_syncArchive)
, m_iCityConnectionHappiness("CvPlayer::m_iCityConnectionHappiness", m_syncArchive)
, m_iHolyCityID("CvPlayer::m_iHolyCityID", m_syncArchive)
, m_iTurnsSinceSettledLastCity("CvPlayer::m_iTurnsSinceSettledLastCity", m_syncArchive)
, m_iNumNaturalWondersDiscoveredInArea("CvPlayer::m_iNumNaturalWondersDiscoveredInArea", m_syncArchive)
, m_iStrategicResourceMod("CvPlayer::m_iStrategicResourceMod", m_syncArchive)
, m_iSpecialistCultureChange("CvPlayer::m_iSpecialistCultureChange", m_syncArchive)
, m_iGreatPeopleSpawnCounter("CvPlayer::m_iGreatPeopleSpawnCounter", m_syncArchive)
, m_iFreeTechCount("CvPlayer::m_iFreeTechCount", m_syncArchive, true)
, m_iMedianTechPercentage(50)
, m_iNumFreePolicies("CvPlayer::m_iNumFreePolicies", m_syncArchive)
, m_iNumFreePoliciesEver("CvPlayer::m_iNumFreePoliciesEver", m_syncArchive)
, m_iLastSliceMoved(0)
, m_eEndTurnBlockingType(NO_ENDTURN_BLOCKING_TYPE)
, m_iEndTurnBlockingNotificationIndex(0)
, m_activeWaitingForEndTurnMessage(false)
, m_endTurnBusyUnitUpdatesLeft(0)
, m_lastGameTurnInitialAIProcessed(-1)
, m_iNumFreeGreatPeople(0)
, m_bProcessedAutoMoves(false)
{
	m_pPlayerPolicies = FNEW(CvPlayerPolicies, c_eCiv5GameplayDLL, 0);
	m_pEconomicAI = FNEW(CvEconomicAI, c_eCiv5GameplayDLL, 0);
	m_pMilitaryAI = FNEW(CvMilitaryAI, c_eCiv5GameplayDLL, 0);
	m_pCitySpecializationAI = FNEW(CvCitySpecializationAI, c_eCiv5GameplayDLL, 0);
	m_pWonderProductionAI = FNEW(CvWonderProductionAI(this, GC.GetGameBuildings()), c_eCiv5GameplayDLL, 0);
	m_pGrandStrategyAI = FNEW(CvGrandStrategyAI, c_eCiv5GameplayDLL, 0);
	m_pDiplomacyAI = FNEW(CvDiplomacyAI, c_eCiv5GameplayDLL, 0);
	m_pPlayerTechs = FNEW(CvPlayerTechs, c_eCiv5GameplayDLL, 0);
	m_pFlavorManager = FNEW(CvFlavorManager, c_eCiv5GameplayDLL, 0);
	m_pTacticalAI = FNEW(CvTacticalAI, c_eCiv5GameplayDLL, 0);
	m_pHomelandAI = FNEW(CvHomelandAI, c_eCiv5GameplayDLL, 0);
	m_pMinorCivAI = FNEW(CvMinorCivAI, c_eCiv5GameplayDLL, 0);
	m_pDealAI = FNEW(CvDealAI, c_eCiv5GameplayDLL, 0);
	m_pBuilderTaskingAI = FNEW(CvBuilderTaskingAI, c_eCiv5GameplayDLL, 0);
	m_pDangerPlots = FNEW(CvDangerPlots, c_eCiv5GameplayDLL, 0);
	m_pCityConnections = FNEW(CvCityConnections, c_eCiv5GameplayDLL, 0);
	m_pTreasury = FNEW(CvTreasury, c_eCiv5GameplayDLL, 0);
	m_pTraits = FNEW(CvPlayerTraits, c_eCiv5GameplayDLL, 0);

	m_pNotifications = NULL;
	m_pDiplomacyRequests = NULL;

	m_iNextOperationID = 0;

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	reset(NO_PLAYER, true);
}


//	--------------------------------------------------------------------------------
CvPlayer::~CvPlayer()
{
	uninit();

	SAFE_DELETE(m_pDangerPlots);
	delete m_pPlayerPolicies;
	delete m_pEconomicAI;
	delete m_pMilitaryAI;
	delete m_pCitySpecializationAI;
	delete m_pWonderProductionAI;
	delete m_pGrandStrategyAI;
	delete m_pDiplomacyAI;
	delete m_pPlayerTechs;
	delete m_pFlavorManager;
	delete m_pTacticalAI;
	delete m_pHomelandAI;
	delete m_pMinorCivAI;
	delete m_pDealAI;
	delete m_pBuilderTaskingAI;
	SAFE_DELETE(m_pCityConnections);
	SAFE_DELETE(m_pNotifications);
	SAFE_DELETE(m_pDiplomacyRequests);
	SAFE_DELETE(m_pTreasury);
	SAFE_DELETE(m_pTraits);
}

//	--------------------------------------------------------------------------------
void CvPlayer::init(PlayerTypes eID)
{
	LeaderHeadTypes eBestPersonality;
	int iValue;
	int iBestValue;
	int iI, iJ;

	// only allocate notifications for civs that players can play as
	if (eID < MAX_MAJOR_CIVS)
	{
		m_pNotifications = FNEW(CvNotifications, c_eCiv5GameplayDLL, 0);
		m_pDiplomacyRequests = FNEW(CvDiplomacyRequests, c_eCiv5GameplayDLL, 0);
	}

	//--------------------------------
	// Init saved data
	reset(eID);

	//--------------------------------
	// Init containers
	m_cities.Init();

	m_units.Init();

	m_armyAIs.Init();

	m_AIOperations.clear();

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	CvAssert(getTeam() != NO_TEAM);
	GET_TEAM(getTeam()).changeNumMembers(1);
	PlayerTypes p = GetID();
	SlotStatus s = CvPreGame::slotStatus(p);
	if ((s == SS_TAKEN) || (s == SS_COMPUTER))
	{
		setAlive(true);

		if (GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
		{
			if (!isBarbarian() && !isMinorCiv())
			{
				iBestValue = 0;
				eBestPersonality = NO_LEADER;

				for (iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
				{
					if (iI != GC.getBARBARIAN_LEADER() && iI != GC.getMINOR_CIVILIZATION())
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "Choosing Personality"));

						for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == ((LeaderHeadTypes)iI))
								{
									iValue /= 2;
								}
							}
						}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestPersonality = ((LeaderHeadTypes)iI);
						}
					}
				}

				if (eBestPersonality != NO_LEADER)
				{
					setPersonalityType(eBestPersonality);
				}
			}
		}

		CvAssert(m_pTraits);
		m_pTraits->InitPlayerTraits();

		// Special handling for the Polynesian trait's overriding of embarked unit graphics
		if (m_pTraits->IsEmbarkedAllWater())
		{
			SetEmbarkedGraphicOverride("ART_DEF_UNIT_U_POLYNESIAN_WAR_CANOE");
		}
		else if (m_pTraits->IsEmbarkedToLandFlatCost())
		{
			SetEmbarkedGraphicOverride("ART_DEF_UNIT_U_DANISH_LONGBOAT");
		}

		changeGoldPerUnitTimes100(GC.getINITIAL_GOLD_PER_UNIT_TIMES_100());

		ChangeMaxNumBuilders(GC.getDEFAULT_MAX_NUM_BUILDERS());

		changeLevelExperienceModifier(GetPlayerTraits()->GetLevelExperienceModifier());
		changeMaxGlobalBuildingProductionModifier(GetPlayerTraits()->GetMaxGlobalBuildingProductionModifier());
		changeMaxTeamBuildingProductionModifier(GetPlayerTraits()->GetMaxTeamBuildingProductionModifier());
		changeMaxPlayerBuildingProductionModifier(GetPlayerTraits()->GetMaxPlayerBuildingProductionModifier());
		ChangePlotGoldCostMod(GetPlayerTraits()->GetPlotBuyCostModifier());
		ChangePlotCultureCostModifier(GetPlayerTraits()->GetPlotCultureCostModifier());
		GetTreasury()->ChangeTradeRouteGoldChange(GetPlayerTraits()->GetTradeRouteChange());
		changeWonderProductionModifier(GetPlayerTraits()->GetWonderProductionModifier());
		ChangeRouteGoldMaintenanceMod(GetPlayerTraits()->GetImprovementMaintenanceModifier());

		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			ChangeCityYieldChange((YieldTypes)iJ, 100 * GetPlayerTraits()->GetFreeCityYield((YieldTypes)iJ));
			changeYieldRateModifier((YieldTypes)iJ, GetPlayerTraits()->GetYieldRateModifier((YieldTypes)iJ));
		}

		recomputeGreatPeopleModifiers();

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
		}

		CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
		for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
			if(pkUnitClassInfo != NULL)
			{
				const UnitTypes eUnit = ((UnitTypes)(playerCivilizationInfo.getCivilizationUnits(iI)));
				if (NO_UNIT != eUnit)
				{
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
					if(NULL != pkUnitInfo && pkUnitInfo->IsFound())
					{
						setUnitExtraCost(eUnitClass, getNewCityProductionValue());
					}
				}
			}
		}
	}

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	AI_init();
}


//	--------------------------------------------------------------------------------
void CvPlayer::uninit()
{
	m_paiNumResourceUsed.clear();
	m_paiNumResourceTotal.clear();
	m_paiResourceGiftedToMinors.clear();
	m_paiResourceExport.clear();
	m_paiResourceImport.clear();
	m_paiResourceFromMinors.clear();
	m_paiImprovementCount.clear();
	m_paiFreeBuildingCount.clear();
	m_paiFreePromotionCount.clear();
	m_paiUnitCombatProductionModifiers.clear();
	m_paiUnitCombatFreeExperiences.clear();
	m_paiUnitClassCount.clear();
	m_paiUnitClassMaking.clear();
	m_paiBuildingClassCount.clear();
	m_paiBuildingClassMaking.clear();
	m_paiProjectMaking.clear();
	m_paiHurryCount.clear();
	m_paiHurryModifier.clear();

	m_pabLoyalMember.clear();
	m_pabGetsScienceFromPlayer.clear();

	m_pPlayerPolicies->Uninit();
	m_pEconomicAI->Uninit();
	m_pMilitaryAI->Uninit();
	m_pCitySpecializationAI->Uninit();
	m_pWonderProductionAI->Uninit();
	m_pGrandStrategyAI->Uninit();
	m_pDiplomacyAI->Uninit();
	m_pPlayerTechs->Uninit();
	m_pFlavorManager->Uninit();
	m_pTacticalAI->Uninit();
	m_pHomelandAI->Uninit();
	m_pMinorCivAI->Uninit();
	m_pDealAI->Uninit();
	m_pBuilderTaskingAI->Uninit();
	m_pCityConnections->Uninit();
	if (m_pNotifications)
	{
		m_pNotifications->Uninit();
	}
	if (m_pDiplomacyRequests)
	{
		m_pDiplomacyRequests->Uninit();
	}
	m_pTreasury->Uninit();
	m_pTraits->Uninit();

	if (m_pDangerPlots)
	{
		m_pDangerPlots->Uninit();
	}

	m_ppaaiSpecialistExtraYield.clear();
	m_ppaaiImprovementYieldChange.clear();
	m_ppaaiBuildingClassYieldMod.clear();

	m_UnitCycle.Clear();

	m_researchQueue.clear();

	m_cityNames.clear();

	m_cities.Uninit();

	m_units.Uninit();

	// loop through all entries freeing them up
	std::map<int , CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		delete(iter->second);
	}
	m_AIOperations.clear();

	clearPopups();

	m_aiPlots.clear();
	m_bfEverConqueredBy.ClearAll();

	FAutoArchive & archive = getSyncArchive();
	archive.clearDelta();

	m_iStartingX = INVALID_PLOT_COORD;
	m_iStartingY = INVALID_PLOT_COORD;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iTotalLandScored = 0;
	m_iCityConnectionHappiness = 0;
	m_iJONSCulturePerTurnForFree = 0;
	m_iJONSCulturePerTurnFromMinorCivs = 0;
	m_iJONSCultureCityModifier = 0;
	m_iJONSCulture = 0;
	m_iJONSCultureEverGenerated = 0;
	m_iCulturePerWonder = 0;
	m_iCultureWonderMultiplier = 0;
	m_iCulturePerTechResearched = 0;
	m_iHappiness = 0;
	m_iRevoltCounter = 0;
	m_iExtraHappinessPerLuxury = 0;
	m_iUnhappinessFromUnits = 0;
	m_iUnhappinessFromUnitsMod = 0;
	m_iUnhappinessMod = 0;
	m_iCityCountUnhappinessMod = 0;
	m_iOccupiedPopulationUnhappinessMod = 0;
	m_iCapitalUnhappinessMod = 0;
	m_iHappinessFromGarrisonedUnits = 0;
	m_iHappinessPerGarrisonedUnitCount = 0;
	m_iHappinessPerTradeRouteCount = 0;
	m_iHappinessPerXPopulation = 0;
	m_iSpecialPolicyBuildingHappiness = 0;
	m_iWoundedUnitDamageMod = 0;
	m_iUnitUpgradeCostMod = 0;
	m_iBarbarianCombatBonus = 0;
	m_iAlwaysSeeBarbCampsCount = 0;
	m_iHappinessFromBuildings = 0;
	m_iHappinessPerCity = 0;
	m_iHappinessPerXPolicies = 0;
	m_iAdvancedStartPoints = -1;
	m_iAttackBonusTurns = 0;
	m_iGoldenAgeProgressMeter = 0;
	m_iGoldenAgeMeterMod = 0;
	m_iNumGoldenAges = 0;
	m_iGoldenAgeTurns = 0;
	m_iNumUnitGoldenAges = 0;
	m_iStrikeTurns = 0;
	m_iGoldenAgeModifier = 0;
	m_iGreatPeopleCreated = 0;
	m_iGreatGeneralsCreated = 0;
	m_iGreatPeopleThresholdModifier = 0;
	m_iGreatGeneralsThresholdModifier = 0;
	m_iAnarchyNumTurns = 0;
	m_iPolicyCostModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleRateModFromBldgs = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iGreatGeneralRateModFromBldgs = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModFromBldgs = 0;
	m_iGreatPersonExpendGold = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iFreeExperienceFromBldgs = 0;
	m_iFreeExperienceFromMinors = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementCostModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iSpecialistProductionModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;
	m_iWonderProductionModifier = 0;
	m_iSettlerProductionModifier = 0;
	m_iCapitalSettlerProductionModifier = 0;
	m_iUnitProductionMaintenanceMod = 0;
	m_iPolicyCostBuildingModifier = 0;
	m_iPolicyCostMinorCivModifier = 0;
	m_iNumNukeUnits = 0;
	m_iNumOutsideUnits = 0;
	m_iBaseFreeUnits = 0;
	m_iBaseFreeMilitaryUnits = 0;
	m_iFreeUnitsPopulationPercent = 0;
	m_iFreeMilitaryUnitsPopulationPercent = 0;
	m_iGoldPerUnit = 0;
	m_iGoldPerMilitaryUnit = 0;
	m_iRouteGoldMaintenanceMod = 0;
	m_iBuildingGoldMaintenanceMod = 0;
	m_iUnitGoldMaintenanceMod = 0;
	m_iUnitSupplyMod = 0;
	m_iExtraUnitCost = 0;
	m_iNumMilitaryUnits = 0;
	m_iHappyPerMilitaryUnit = 0;
	m_iHappinessToCulture = 0;
	m_iHappinessToScience = 0;
	m_iHalfSpecialistUnhappinessCount = 0;
	m_iHalfSpecialistFoodCount = 0;
	m_iMilitaryFoodProductionCount = 0;
	m_iConscriptCount = 0;
	m_iMaxConscript = 0;
	m_iHighestUnitLevel = 1;
	m_iOverflowResearch = 0;
	m_iExpModifier = 0;
	m_iExpInBorderModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iMinorQuestFriendshipMod = 0;
	m_iMinorGoldFriendshipMod = 0;
	m_iMinorFriendshipMinimum = 0;
	m_iMinorFriendshipDecayMod = 0;
	m_iMinorScienceAlliesCount = 0;
	m_iMinorResourceBonusCount = 0;
	//m_iFreeSpecialist = 0;
	m_iCultureBombTimer = 0;
	m_iConversionTimer = 0;
	m_iCapitalCityID = FFreeList::INVALID_INDEX;
	m_iCitiesLost = 0;
	m_iMilitaryMight = 0;
	m_iEconomicMight = 0;
	m_iTurnMightRecomputed = -1;
	m_iNewCityExtraPopulation = 0;
	m_iFreeFoodBox = 0;
	m_iPopulationScore = 0;
	m_iLandScore = 0;
	m_iTechScore = 0;
	m_iWondersScore = 0;
	m_iScoreFromFutureTech = 0;
	m_iCombatExperience = 0;
	m_iLifetimeCombatExperience = 0;
	m_iBorderObstacleCount = 0;
	m_iPopRushHurryCount = 0;
	m_uiStartTime = 0;
	m_iTotalImprovementsBuilt = 0;
	m_iNextOperationID = 0;
	m_iCostNextPolicy = 0;
	m_iNumBuilders = 0;
	m_iMaxNumBuilders = 0;
	m_iCityStrengthMod = 0;
	m_iCityGrowthMod = 0;
	m_iCapitalGrowthMod = 0;
	m_iNumPlotsBought = 0;
	m_iPlotGoldCostMod = 0;
	m_iPlotCultureCostModifier = 0;
	m_iPlotCultureExponentModifier = 0;
	m_iNumCitiesPolicyCostDiscount = 0;
	m_iGarrisonedCityRangeStrikeModifier = 0;
	m_iGarrisonFreeMaintenanceCount = 0;
	m_iNumCitiesFreeCultureBuilding = 0;
	m_iUnitPurchaseCostModifier = 0;
	m_iAllFeatureProduction = 0;
	m_iCityDistanceHighwaterMark = 1;
	m_iOriginalCapitalX = -1;
	m_iOriginalCapitalY = -1;
	m_iNumWonders = 0;
	m_iNumPolicies = 0;
	m_iNumGreatPeople = 0;
	m_iHolyCityID = -1;
	m_iTurnsSinceSettledLastCity = -1;
	m_iNumNaturalWondersDiscoveredInArea = 0;
	m_iStrategicResourceMod = 0;
	m_iSpecialistCultureChange = 0;
	m_iGreatPeopleSpawnCounter = 0;
	m_iFreeTechCount = 0;
	m_iMedianTechPercentage = 50;
	m_iNumFreePolicies = 0;
	m_iNumFreePoliciesEver = 0;
	m_iNumFreeGreatPeople = 0;
	m_iLastSliceMoved = 0;

	m_bHasBetrayedMinorCiv = false;
	m_bAlive = false;
	m_bEverAlive = false;
	m_bTurnActive = false;
	m_bAutoMoves = false;
	m_bProcessedAutoMoves = false;
	m_bEndTurn = false;
	m_bDynamicTurnsSimultMode = true;
	m_bPbemNewTurn = false;
	m_bExtendedGame = false;
	m_bFoundedFirstCity = false;
	m_bStrike = false;
	m_bCramped = false;
	m_bLostCapital = false;
	m_eConqueror = NO_PLAYER;
	m_bHasAdoptedStateReligion = false;
	m_bAlliesGreatPersonBiasApplied = false;
	m_lastGameTurnInitialAIProcessed = -1;

	m_eID = NO_PLAYER;
}


//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	m_syncArchive.reset();
	//--------------------------------
	// Uninit class
	uninit();

	m_eID = eID;
	if (m_eID != NO_PLAYER)
	{
		m_ePersonalityType = CvPreGame::leaderHead(m_eID); //??? Is this repeated data???
	}
	else
	{
		m_ePersonalityType = NO_LEADER;
	}

	// tutorial info
	m_bEverPoppedGoody = false;

	m_aiCityYieldChange.clear();
	m_aiCityYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCoastalCityYieldChange.clear();
	m_aiCoastalCityYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldChange.clear();
	m_aiCapitalYieldChange.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldPerPopChange.clear();
	m_aiCapitalYieldPerPopChange.resize(NUM_YIELD_TYPES, 0);

	m_aiSeaPlotYield.clear();
	m_aiSeaPlotYield.resize(NUM_YIELD_TYPES, 0);

	m_aiYieldRateModifier.clear();
	m_aiYieldRateModifier.resize(NUM_YIELD_TYPES, 0);

	m_aiCapitalYieldRateModifier.clear();
	m_aiCapitalYieldRateModifier.resize(NUM_YIELD_TYPES, 0);

	m_aiExtraYieldThreshold.clear();
	m_aiExtraYieldThreshold.resize(NUM_YIELD_TYPES, 0);

	m_aiSpecialistExtraYield.clear();
	m_aiSpecialistExtraYield.resize(NUM_YIELD_TYPES, 0);

	m_aiProximityToPlayer.clear();
	m_aiProximityToPlayer.resize(MAX_PLAYERS, 0);

	m_aiResearchAgreementCounter.clear();
	m_aiResearchAgreementCounter.resize(MAX_PLAYERS, 0);

	m_aiIncomingUnitTypes.clear();
	m_aiIncomingUnitTypes.resize(MAX_INCOMING_UNITS, NO_UNIT);

	m_aiIncomingUnitCountdowns.clear();
	m_aiIncomingUnitCountdowns.resize(MAX_INCOMING_UNITS, -1);

	m_aOptions.clear();

	m_strReligionKey = "";
	m_strScriptData = "";
	m_strEmbarkedGraphicOverride = "";

	if (!bConstructorCall)
	{
		CvAssertMsg(0 < GC.getNumResourceInfos(), "GC.getNumResourceInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		m_paiNumResourceUsed.clear();
		m_paiNumResourceUsed.resize(GC.getNumResourceInfos(), 0);

		m_paiNumResourceTotal.clear();
		m_paiNumResourceTotal.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceGiftedToMinors.clear();
		m_paiResourceGiftedToMinors.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceExport.clear();
		m_paiResourceExport.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceImport.clear();
		m_paiResourceImport.resize(GC.getNumResourceInfos(), 0);

		m_paiResourceFromMinors.clear();
		m_paiResourceFromMinors.resize(GC.getNumResourceInfos(), 0);

		CvAssertMsg(0 < GC.getNumImprovementInfos(), "GC.getNumImprovementInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		m_paiImprovementCount.clear();
		m_paiImprovementCount.resize(GC.getNumImprovementInfos(), 0);

		m_paiUnitCombatProductionModifiers.clear();
		m_paiUnitCombatProductionModifiers.resize(GC.getNumUnitCombatClassInfos(), 0);

		m_paiUnitCombatFreeExperiences.clear();
		m_paiUnitCombatFreeExperiences.resize(GC.getNumUnitCombatClassInfos(), 0);

		m_paiFreeBuildingCount.clear();
		m_paiFreeBuildingCount.resize(GC.getNumBuildingInfos(), 0);

		m_paiFreePromotionCount.clear();
		m_paiFreePromotionCount.resize(GC.getNumPromotionInfos(), 0);

		m_paiUnitClassCount.clear();
		m_paiUnitClassCount.resize(GC.getNumUnitClassInfos(), 0);

		m_paiUnitClassMaking.clear();
		m_paiUnitClassMaking.resize(GC.getNumUnitClassInfos(), 0);

		m_paiBuildingClassCount.clear();
		m_paiBuildingClassCount.resize(GC.getNumBuildingClassInfos(), 0);

		m_paiBuildingClassMaking.clear();
		m_paiBuildingClassMaking.resize(GC.getNumBuildingClassInfos(), 0);

		m_paiProjectMaking.clear();
		m_paiProjectMaking.resize(GC.getNumProjectInfos(), 0);

		m_paiHurryCount.clear();
		m_paiHurryCount.resize(GC.getNumHurryInfos(), 0);

		m_paiHurryModifier.clear();
		m_paiHurryModifier.resize(GC.getNumHurryInfos(), 0);

		m_pabLoyalMember.clear();
		m_pabLoyalMember.resize(GC.getNumVoteSourceInfos(), true);

		m_pabGetsScienceFromPlayer.clear();
		m_pabGetsScienceFromPlayer.resize(MAX_CIV_PLAYERS, false);

		m_pEconomicAI->Init(GC.GetGameEconomicAIStrategies(), this);
		m_pMilitaryAI->Init(GC.GetGameMilitaryAIStrategies(), this, GetDiplomacyAI());
		m_pCitySpecializationAI->Init(GC.GetGameCitySpecializations(), this);
		m_pWonderProductionAI->Init(GC.GetGameBuildings(), this, false);
		m_pGrandStrategyAI->Init(GC.GetGameAIGrandStrategies(), this);
		m_pDiplomacyAI->Init(this);
		m_pPlayerTechs->Init(GC.GetGameTechs(), this, false);
		m_pPlayerPolicies->Init(GC.GetGamePolicies(), this, false);
		m_pTacticalAI->Init(this);
		m_pHomelandAI->Init(this);
		m_pMinorCivAI->Init(this);
		m_pDealAI->Init(this);
		m_pBuilderTaskingAI->Init(this);
		m_pCityConnections->Init(this);
		if (m_pNotifications)
		{
			m_pNotifications->Init(eID);
		}
		if (m_pDiplomacyRequests)
		{
			m_pDiplomacyRequests->Init(eID);
		}
		m_pDangerPlots->Init(eID, false /*bAllocate*/);
		m_pTreasury->Init(this);
		m_pTraits->Init(GC.GetGameTraits(), this);

		// Set up flavor manager
		m_pFlavorManager->Init(this);

		// And if this is a real player, hook up the player-level flavor recipients
		PlayerTypes p = GetID();
		if (p != NO_PLAYER)
		{
			SlotStatus s = CvPreGame::slotStatus(p);
			if ((s == SS_TAKEN || s == SS_COMPUTER) && !isBarbarian())
			{
				m_pFlavorManager->AddFlavorRecipient(m_pPlayerTechs);
				m_pFlavorManager->AddFlavorRecipient(m_pPlayerPolicies);
				m_pFlavorManager->AddFlavorRecipient(m_pWonderProductionAI);
			}
		}

		Firaxis::Array< int, NUM_YIELD_TYPES > yield;
		for( unsigned int j = 0; j < NUM_YIELD_TYPES; ++j )
		{
			yield[j] = 0;
		}

		m_ppaaiSpecialistExtraYield.clear();
		m_ppaaiSpecialistExtraYield.resize(GC.getNumSpecialistInfos());
		for( unsigned int i = 0; i < m_ppaaiSpecialistExtraYield.size(); ++i )
		{
			m_ppaaiSpecialistExtraYield.setAt( i, yield );
		}

		m_ppaaiImprovementYieldChange.clear();
		m_ppaaiImprovementYieldChange.resize(GC.getNumImprovementInfos());
		for( unsigned int i = 0; i < m_ppaaiImprovementYieldChange.size(); ++i )
		{
			m_ppaaiImprovementYieldChange.setAt( i, yield );
		}

		m_ppaaiBuildingClassYieldMod.clear();
		m_ppaaiBuildingClassYieldMod.resize(GC.getNumBuildingClassInfos());
		for( unsigned int i = 0; i < m_ppaaiBuildingClassYieldMod.size(); ++i )
		{
			m_ppaaiBuildingClassYieldMod.setAt( i, yield );
		}

		m_aVote.clear();
		m_aUnitExtraCosts.clear();
	}

	m_cities.RemoveAll();

	m_units.RemoveAll();

	m_armyAIs.RemoveAll();

	// loop through all entries freeing them up
	std::map<int , CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		delete(iter->second);
	}
	m_AIOperations.clear();

	if (!bConstructorCall)
	{
		AI_reset();
	}
}

//	--------------------------------------------------------------------------------
/// This is called after the map and other game constructs have been setup before the game starts.
/// Called on a loaded or new game.
void CvPlayer::gameStartInit()
{
	// if the game is loaded, don't init the danger plots. This was already done in the serialization process.
	if (CvPreGame::gameStartType() != GAME_LOADED)
	{
		if (!GC.GetEngineUserInterface()->IsLoadedGame())
		{
			InitDangerPlots(); // moved this up because everyone should have danger plots inited. This is bad because saved games get much bigger for no reason.
		}
	}

	verifyAlive();
	if (!isAlive())
	{
		return;
	}

	if (!GC.GetEngineUserInterface()->IsLoadedGame())
	{
		InitPlots();
		UpdatePlots();
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlayer::setupGraphical()
{
	CvCity* pLoopCity;
	CvUnit* pLoopUnit;

	// Setup m_cities
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setupGraphical();
	}

	// Setup m_units
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->setupGraphical();
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::initFreeState(CvGameInitialItemsOverrides& kOverrides)
{
	CvHandicapInfo& kHandicapInfo = getHandicapInfo();

	// Starting Gold
	if(kOverrides.GrantInitialGoldPerPlayer[GetID()])
	{
		GetTreasury()->SetGold(0);
		GetTreasury()->ChangeGold(kHandicapInfo.getStartingGold());
		GetTreasury()->ChangeGold(GC.getGame().getStartEraInfo().getStartingGold());
	}

	// Free Culture
	if(kOverrides.GrantInitialCulturePerPlayer[GetID()])
	{
		changeJONSCulture(GC.getGame().getStartEraInfo().getStartingCulture());
		changeJONSCulture(kHandicapInfo.getStartingPolicyPoints()); // I think policy points is a better name than Jon's Culture, don't you?
		ChangeJONSCulturePerTurnForFree(kHandicapInfo.getFreeCulturePerTurn()); // No, IMNSHO ;P
	}
	// Extra Happiness from Luxuries
	ChangeExtraHappinessPerLuxury(kHandicapInfo.getExtraHappinessPerLuxury());

	// Free starting Resources
	for (int iLoop = 0; iLoop < GC.getNumResourceInfos(); iLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource)
		{
			if (pkResource->getStartingResourceQuantity() != 0)
			{
				changeNumResourceTotal(eResource, pkResource->getStartingResourceQuantity());
			}
		}

	}

	DoUpdateHappiness();

	clearResearchQueue();
}


//	--------------------------------------------------------------------------------
void CvPlayer::initFreeUnits(CvGameInitialItemsOverrides& /*kOverrides*/)
{
	UnitTypes eLoopUnit;
	int iFreeCount;
	int iDefaultAI;
	int iI, iJ;

	CvEraInfo& gameStartEra = GC.getGame().getStartEraInfo();
	CvHandicapInfo& gameHandicap = GC.getGame().getHandicapInfo();
	CvHandicapInfo& playerHandicap = getHandicapInfo();
	CvCivilizationInfo& playerCivilization = getCivilizationInfo();

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			eLoopUnit = (UnitTypes)playerCivilization.getCivilizationUnits(iI);

			if (eLoopUnit != NO_UNIT)
			{
				iFreeCount = playerCivilization.getCivilizationFreeUnitsClass(iI);
				iDefaultAI = playerCivilization.getCivilizationFreeUnitsDefaultUnitAI(iI);

				iFreeCount *= (gameStartEra.getStartingUnitMultiplier() + ((!isHuman()) ? gameHandicap.getAIStartingUnitMultiplier() : 0));

				// City states only get 1 of something
				if (isMinorCiv() && iFreeCount > 1)
					iFreeCount = 1;

				for (iJ = 0; iJ < iFreeCount; iJ++)
				{
					addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);
				}
			}
		}
	}

	// Trait units
	int iUnitClass = GetPlayerTraits()->GetFirstFreeUnit(NO_TECH);
	while (iUnitClass != NO_UNITCLASS)
	{
		eLoopUnit = (UnitTypes)playerCivilization.getCivilizationUnits(iUnitClass);
		iDefaultAI = GC.GetGameUnits()->GetEntry(eLoopUnit)->GetDefaultUnitAIType();
		addFreeUnit(eLoopUnit,(UnitAITypes)iDefaultAI);

		// Another?
		iUnitClass = GetPlayerTraits()->GetNextFreeUnit();
	}

	// Defensive units
	iFreeCount = gameStartEra.getStartingDefenseUnits();
	iFreeCount += playerHandicap.getStartingDefenseUnits();

	if (!isHuman())
		iFreeCount += gameHandicap.getAIStartingDefenseUnits();

	if (iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_DEFENSE, iFreeCount);

	// Worker units
	iFreeCount = gameStartEra.getStartingWorkerUnits();
	iFreeCount += playerHandicap.getStartingWorkerUnits();

	if (!isHuman())
		iFreeCount += gameHandicap.getAIStartingWorkerUnits();

	if (iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_WORKER, iFreeCount);

	// Explore units
	iFreeCount = gameStartEra.getStartingExploreUnits();
	iFreeCount += playerHandicap.getStartingExploreUnits();

	if (!isHuman())
		iFreeCount += gameHandicap.getAIStartingExploreUnits();

	if (iFreeCount > 0 && !isMinorCiv())
		addFreeUnitAI(UNITAI_EXPLORE, iFreeCount);

	// If we only have one military unit and it's on defense then change its AI to explore
	if (GetNumUnitsWithUnitAI(UNITAI_EXPLORE) == 0)
	{
		int iLoop;
		CvUnit* pLoopUnit;
		for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSE)
			{
				pLoopUnit->AI_setUnitAIType(UNITAI_EXPLORE);
				break;
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::addFreeUnitAI(UnitAITypes eUnitAI, int iCount)
{
	int iI;

	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;

	CvCivilizationInfo& playerCivilzationInfo = getCivilizationInfo();
	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			UnitTypes eLoopUnit = (UnitTypes)playerCivilzationInfo.getCivilizationUnits(iI);
			if (eLoopUnit != NO_UNIT)
			{
				CvUnitEntry* pUnitInfo = GC.getUnitInfo(eLoopUnit);
				if(pUnitInfo != NULL)
				{
					if (canTrain(eLoopUnit))
					{
						bool bValid = true;
						for (int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
						{
							const ResourceTypes eResource = static_cast<ResourceTypes>(iJ);
							CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
							if(pkResource)
							{
								if (pUnitInfo->GetResourceQuantityRequirement(iJ) > 0)
								{
									bValid = false;
								}
							}
						}

						if (bValid)
						{
							int iValue = 0;

							// Default unit AI matches
							if (pUnitInfo->GetDefaultUnitAIType() == eUnitAI)
								iValue += (pUnitInfo->GetProductionCost() * 2);
							// Not default, but still possible
							else if (pUnitInfo->GetUnitAIType(eUnitAI))
								iValue += (pUnitInfo->GetProductionCost());

							if (iValue > iBestValue)
							{
								eBestUnit = eLoopUnit;
								iBestValue = iValue;
							}
						}
					}
				}
			}
		}

	}

	if (eBestUnit != NO_UNIT)
	{
		for (iI = 0; iI < iCount; iI++)
		{
			addFreeUnit(eBestUnit, eUnitAI);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Returns plot where new unit was created
CvPlot *CvPlayer::addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	CvPlot* pStartingPlot;
	CvPlot* pBestPlot;
	CvPlot* pLoopPlot;
	CvPlot* pReturnValuePlot = NULL;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
		return pReturnValuePlot;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if ((eUnitAI == UNITAI_SETTLE) || (pkUnitInfo->GetDefaultUnitAIType() == UNITAI_SETTLE))
		{
			if (GetNumUnitsWithUnitAI(UNITAI_SETTLE) >= 1)
			{
				return pReturnValuePlot;
			}
		}
	}

	CvCity* pCapital = getCapitalCity();

	if (pCapital)
	{
		pStartingPlot = pCapital->plot();
	}
	else
	{
		pStartingPlot = getStartingPlot();
	}

	if (pStartingPlot != NULL)
	{
		pBestPlot = NULL;

		if (isHuman())
		{
			if (!(pkUnitInfo->IsFound()))
			{
				DirectionTypes eDirection;

				bool bDirectionValid;

				int iCount = 0;

				// Find a random direction
				do
				{
					bDirectionValid = true;

					eDirection = (DirectionTypes) GC.getGame().getJonRandNum(NUM_DIRECTION_TYPES, "Placing Starting Units (Human)");

					if (bDirectionValid)
					{
						pLoopPlot = plotDirection(pStartingPlot->getX(), pStartingPlot->getY(), eDirection);

						if (pLoopPlot != NULL && pLoopPlot->getArea() == pStartingPlot->getArea())
						{
							if (!(pLoopPlot->isImpassable()))
							{
								if (!(pLoopPlot->isUnit()))
								{
									if (!(pLoopPlot->isGoody()))
									{
										pBestPlot = pLoopPlot;
										break;
									}
								}
							}
						}
					}

					// Emergency escape.  Should only really break on Debug Micro map or something really funky
					iCount++;
				}
				while (iCount < 1000);
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pStartingPlot;
		}

		CvUnit* pNewUnit = initUnit(eUnit, pBestPlot->getX(), pBestPlot->getY(), eUnitAI);
		CvAssert(pNewUnit);
		if (pNewUnit == NULL)
			return NULL;

		// Don't stack any units
		if (pBestPlot->getNumUnits() > 1)
		{
			if (!pNewUnit->jumpToNearestValidPlot())
			{
				pNewUnit->kill(false);		// Failed, kill it and return NULL.
				return NULL;
			}
		}
		pReturnValuePlot = pNewUnit->plot();
	}

	return pReturnValuePlot;
}


//	--------------------------------------------------------------------------------
CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits)
{
	CvCity* pCity = addCity();

	CvAssertMsg(pCity != NULL, "City is not assigned a valid value");
	if (pCity != NULL)
	{
		CvAssertMsg(!(GC.getMap().plot(iX, iY)->isCity()), "No city is expected at this plot when initializing new city");
		pCity->init(pCity->GetID(), GetID(), iX, iY, bBumpUnits);
		pCity->GetCityStrategyAI()->UpdateFlavorsForNewCity();
	}

	return pCity;
}


//	--------------------------------------------------------------------------------
void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bGift)
{
    if( pOldCity == NULL )
        return;

	IDInfo* pUnitNode;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvPlot* pCityPlot;

	CvString strBuffer;
	CvString strName;
	bool abEverOwned[MAX_PLAYERS];
	PlayerTypes eOldOwner;
	PlayerTypes eOriginalOwner;
	BuildingTypes eBuilding;
	bool bRecapture;
	int iCaptureGold;
	int iGameTurnFounded;
	int iPopulation;
	int iHighestPopulation;
	int iBattleDamage;
	int iI;
	FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL > oldUnits;

	pCityPlot = pOldCity->plot();

	pUnitNode = pCityPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode);
		pUnitNode = pCityPlot->nextUnitNode((IDInfo*)pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit && pLoopUnit->getTeam() != getTeam())
		{
			if (pLoopUnit->IsImmobile())
			{
				pLoopUnit->kill(false, GetID());
				DoUnitKilledCombat(pLoopUnit->getOwner(), pLoopUnit->getUnitType());
			}
		}
	}

	if (bConquest)
	{
		CvNotifications* pNotifications = GET_PLAYER(pOldCity->getOwner()).GetNotifications();
		if (pNotifications)
		{
			Localization::String locString = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_LOST");
			locString << pOldCity->getNameKey() << getNameKey();
			Localization::String locSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_LOST");
			locSummary << pOldCity->getNameKey();
			pNotifications->Add(NOTIFICATION_CITY_LOST, locString.toUTF8(), locSummary.toUTF8(), pOldCity->getX(), pOldCity->getY(), -1);
		}

		if (!isBarbarian() && !pOldCity->isBarbarian())
		{
			int iDefaultCityValue = /*150*/ GC.getWAR_DAMAGE_LEVEL_CITY_WEIGHT();

			// Notify Diplo AI that damage has been done
			int iValue = iDefaultCityValue;
			iValue += pOldCity->getPopulation() * /*100*/ GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER();
			// My viewpoint
			GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(pOldCity->getOwner(), GetID(), iValue);
			// Bad guy's viewpoint
			GET_PLAYER(pOldCity->getOwner()).GetDiplomacyAI()->ChangeWarValueLost(GetID(), iValue);

			PlayerTypes ePlayer;

			iValue = iDefaultCityValue;
			iValue += pOldCity->getPopulation() * /*120*/ GC.getWAR_DAMAGE_LEVEL_UNINVOLVED_CITY_POP_MULTIPLIER();

			// Now update everyone else in the world, but use a different multiplier (since they don't have complete info on the situation - they don't know when Units are killed)
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes) iPlayerLoop;

				// Not us and not the player we acquired City from
				if (ePlayer != GetID() && ePlayer != pOldCity->getOwner())
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeOtherPlayerWarValueLost(pOldCity->getOwner(), GetID(), iValue);
				}
			}
		}
	}

	if (pOldCity->getOriginalOwner() == pOldCity->getOwner())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(1);
	}
	else if (pOldCity->getOriginalOwner() == GetID())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(-1);
	}

	if (bConquest)
	{
		if (GetID() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_CAPTURED_CITY", pOldCity->getNameKey()).GetCString();
			GC.GetEngineUserInterface()->AddCityMessage(0, pOldCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_CITYCAPTURE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pOldCity->getX(), pOldCity->getY(), true, true*/);
		}

		strName.Format("%s (%s)", pOldCity->getName().GetCString(), GET_PLAYER(pOldCity->getOwner()).getName());

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if ((PlayerTypes)iI == GC.getGame().getActivePlayer())
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (iI != GetID())
					{
						if (pOldCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_CITY_CAPTURED_BY", strName.GetCString(), getCivilizationShortDescriptionKey());
							GC.GetEngineUserInterface()->AddCityMessage(0, pOldCity->GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pOldCity->getX(), pOldCity->getY(), true, true*/);
						}
					}
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_CITY_WAS_CAPTURED_BY", strName.GetCString(), getCivilizationShortDescriptionKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), strBuffer, pOldCity->getX(), pOldCity->getY());

#ifndef FINAL_RELEASE
		OutputDebugString("\n"); OutputDebugString(strBuffer); OutputDebugString("\n\n");
#endif
	}

	iCaptureGold = 0;

	if (bConquest)
	{
		// TODO: add scripting support for "doCityCaptureGold"
		iCaptureGold = 0;

		iCaptureGold += GC.getBASE_CAPTURE_GOLD();
		iCaptureGold += (pOldCity->getPopulation() * GC.getCAPTURE_GOLD_PER_POPULATION());
		iCaptureGold += GC.getGame().getJonRandNum(GC.getCAPTURE_GOLD_RAND1(), "Capture Gold 1");
		iCaptureGold += GC.getGame().getJonRandNum(GC.getCAPTURE_GOLD_RAND2(), "Capture Gold 2");

		if (GC.getCAPTURE_GOLD_MAX_TURNS() > 0)
		{
			iCaptureGold *= range((GC.getGame().getGameTurn() - pOldCity->getGameTurnAcquired()), 0, GC.getCAPTURE_GOLD_MAX_TURNS());
			iCaptureGold /= GC.getCAPTURE_GOLD_MAX_TURNS();
		}

		iCaptureGold *= (100 + pOldCity->getCapturePlunderModifier()) / 100;
		iCaptureGold *= (100 + GetPlayerTraits()->GetPlunderModifier()) / 100;
	}

	GetTreasury()->ChangeGold(iCaptureGold);

	int iNumBuildingInfos = GC.getNumBuildingInfos();
	std::vector<int> paiNumRealBuilding(iNumBuildingInfos, 0);
	std::vector<int> paiBuildingOriginalOwner(iNumBuildingInfos, 0);
	std::vector<int> paiBuildingOriginalTime(iNumBuildingInfos, 0);

	int iOldCityX = pOldCity->getX();
	int iOldCityY = pOldCity->getY();
	eOldOwner = pOldCity->getOwner();
	eOriginalOwner = pOldCity->getOriginalOwner();
	iGameTurnFounded = pOldCity->getGameTurnFounded();
	iPopulation = pOldCity->getPopulation();
	iHighestPopulation = pOldCity->getHighestPopulation();
	bool bEverCapital = pOldCity->IsEverCapital();
	strName = pOldCity->getNameKey();
	int iOldCultureLevel = pOldCity->GetJONSCultureLevel();
	bool bHasMadeAttack = pOldCity->isMadeAttack();

	iBattleDamage = pOldCity->getDamage();
	// Traded cities between humans don't heal (an exploit would be to trade a city back and forth between teammates to get an instant heal.)
	if (!bGift || !isHuman() || !GET_PLAYER(pOldCity->getOwner()).isHuman())
	{
		int iBattleDamgeThreshold = GC.getMAX_CITY_HIT_POINTS() * /*50*/ GC.getCITY_CAPTURE_DAMAGE_PERCENT();
		iBattleDamgeThreshold /= 100;

		if (iBattleDamage > iBattleDamgeThreshold)
		{
			iBattleDamage = iBattleDamgeThreshold;
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		abEverOwned[iI] = pOldCity->isEverOwned((PlayerTypes)iI);
	}

	abEverOwned[GetID()] = true;

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		paiNumRealBuilding[iI] = pOldCity->GetCityBuildings()->GetNumRealBuilding((BuildingTypes)iI);
		paiBuildingOriginalOwner[iI] = pOldCity->GetCityBuildings()->GetBuildingOriginalOwner((BuildingTypes)iI);
		paiBuildingOriginalTime[iI] = pOldCity->GetCityBuildings()->GetBuildingOriginalTime((BuildingTypes)iI);
	}

	std::vector<BuildingYieldChange> aBuildingYieldChange;
	for (iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if (!pkBuildingClassInfo)
		{
			continue;
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			BuildingYieldChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)iI;
			kChange.eYield = (YieldTypes)iYield;
			kChange.iChange = pOldCity->GetCityBuildings()->GetBuildingYieldChange((BuildingClassTypes)iI, (YieldTypes)iYield);
			if (0 != kChange.iChange)
			{
				aBuildingYieldChange.push_back(kChange);
			}
		}
	}

	bRecapture = false;

	bool bCapital = pOldCity->isCapital();

	// find the plot
	FStaticVector<int, 121, true, c_eCiv5GameplayDLL, 0> aiPurchasedPlotX;
	FStaticVector<int, 121, true, c_eCiv5GameplayDLL, 0> aiPurchasedPlotY;
	const int iMaxRange = /*5*/ GC.getMAXIMUM_ACQUIRE_PLOT_DISTANCE();

	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pLoopPlot && pLoopPlot->GetCityPurchaseOwner() == eOldOwner && pLoopPlot->GetCityPurchaseID() == pOldCity->GetID())
		{
			aiPurchasedPlotX.push_back(pLoopPlot->getX());
			aiPurchasedPlotY.push_back(pLoopPlot->getY());
			pLoopPlot->ClearCityPurchaseInfo();
		}
	}

	pOldCity->PreKill();

	{
		auto_ptr<ICvCity1> pkDllOldCity(new CvDllCity(pOldCity));
		gDLL->GameplayCityCaptured(pkDllOldCity.get(), GetID());
	}

	GET_PLAYER(eOldOwner).deleteCity(pOldCity->GetID());
	// adapted from PostKill()

	GC.getGame().addReplayMessage( REPLAY_MESSAGE_CITY_CAPTURED, m_eID, "", pCityPlot->getX(), pCityPlot->getY());

	PlayerTypes ePlayer;
	// Update Proximity between this Player and all others
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if (ePlayer != m_eID)
		{
			if (GET_PLAYER(ePlayer).isAlive())
			{
				GET_PLAYER(m_eID).DoUpdateProximityToPlayer(ePlayer);
				GET_PLAYER(ePlayer).DoUpdateProximityToPlayer(m_eID);
			}
		}
	}

	GC.getMap().updateWorkingCity(pCityPlot,NUM_CITY_RINGS*2);

	// Lost the capital!
	if (bCapital)
	{
		GET_PLAYER(eOldOwner).SetHasLostCapital(true, GetID());

		GET_PLAYER(eOldOwner).findNewCapital();

		GET_TEAM(getTeam()).resetVictoryProgress();
	}

	GC.GetEngineUserInterface()->setDirty(NationalBorders_DIRTY_BIT, true);
	// end adapted from PostKill()

	pNewCity = initCity(pCityPlot->getX(), pCityPlot->getY(), !bConquest);

	CvAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif
	pNewCity->setPreviousOwner(eOldOwner);
	pNewCity->setOriginalOwner(eOriginalOwner);
	pNewCity->setGameTurnFounded(iGameTurnFounded);
	pNewCity->SetEverCapital(bEverCapital);

	// Population change for capturing a city
	if (!bRecapture && bConquest)	// Don't drop it if we're recapturing our own City
	{
		iPopulation = max(1, iPopulation * /*50*/ GC.getCITY_CAPTURE_POPULATION_PERCENT() / 100);
	}

	pNewCity->setPopulation(iPopulation);
	pNewCity->setHighestPopulation(iHighestPopulation);
	pNewCity->setName(strName);
	pNewCity->setNeverLost(false);
	pNewCity->setDamage(iBattleDamage,true);
	pNewCity->setMadeAttack(bHasMadeAttack);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pNewCity->setEverOwned(((PlayerTypes)iI), abEverOwned[iI]);
	}

	pNewCity->SetJONSCultureLevel(iOldCultureLevel);

	CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkLoopBuildingInfo = GC.getBuildingInfo(eLoopBuilding);
		if(pkLoopBuildingInfo)
		{
			const CvBuildingClassInfo& kLoopBuildingClassInfo = pkLoopBuildingInfo->GetBuildingClassInfo();

			int iNum = 0;
			if (paiNumRealBuilding[iI] > 0)
			{
				const BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkLoopBuildingInfo->GetBuildingClassType();
				if (::isWorldWonderClass(kLoopBuildingClassInfo))
				{
					eBuilding = eLoopBuilding;
				}
				else
				{
					eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);
				}

				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if(pkBuildingInfo)
					{
						if (!pkLoopBuildingInfo->IsNeverCapture())
						{
							if (!isProductionMaxedBuildingClass(((BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType())), true))
							{
								// here would be a good place to put additional checks (for example, influence)
								if (!bConquest || bRecapture || GC.getGame().getJonRandNum(100, "Capture Probability") < pkLoopBuildingInfo->GetConquestProbability())
								{
									iNum += paiNumRealBuilding[iI];
								}
							}
						}

						// Check for Tomb Raider Achievement
						if (bConquest && !GC.getGame().isGameMultiPlayer() && pkLoopBuildingInfo->GetType() && _stricmp(pkLoopBuildingInfo->GetType(), "BUILDING_BURIAL_TOMB") == 0 && isHuman() )
						{
							if (iCaptureGold > 0) //Need to actually pillage something from the 'tomb'
							{
								gDLL->UnlockAchievement( ACHIEVEMENT_SPECIAL_TOMBRAIDER );
							}
						}

						// Check for Rome conquering Statue of Zeus Achievement
						if (bConquest && !GC.getGame().isGameMultiPlayer() && pkLoopBuildingInfo->GetType() && _stricmp(pkLoopBuildingInfo->GetType(), "BUILDING_STATUE_ZEUS") == 0 && isHuman() )
						{
							const char* pkCivKey = getCivilizationTypeKey();
							if (pkCivKey && strcmp(pkCivKey, "CIVILIZATION_ROME") == 0)
							{
								gDLL->UnlockAchievement( ACHIEVEMENT_SPECIAL_ROME_GETS_ZEUS );
							}
						}


						pNewCity->GetCityBuildings()->SetNumRealBuildingTimed(eBuilding, iNum, false, ((PlayerTypes)(paiBuildingOriginalOwner[iI])), paiBuildingOriginalTime[iI]);
					}
				}
			}
		}
	}

	for (std::vector<BuildingYieldChange>::iterator it = aBuildingYieldChange.begin(); it != aBuildingYieldChange.end(); ++it)
	{
		pNewCity->GetCityBuildings()->SetBuildingYieldChange((*it).eBuildingClass, (*it).eYield, (*it).iChange);
	}

	// Did we re-acquire our Capital?
	if (pCityPlot->getX() == GetOriginalCapitalX() && pCityPlot->getY() == GetOriginalCapitalY())
	{
		SetHasLostCapital(false, NO_PLAYER);

		const BuildingTypes eCapitalBuilding = (BuildingTypes) (getCivilizationInfo().getCivilizationBuildings(GC.getCAPITAL_BUILDINGCLASS()));
		if (eCapitalBuilding != NO_BUILDING)
		{
			if (getCapitalCity() != NULL)
			{
				getCapitalCity()->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 0);
			}
			CvAssertMsg(!(pNewCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding)), "(pBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
			pNewCity->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 1);
		}
	}

	GC.getMap().updateWorkingCity(pCityPlot,NUM_CITY_RINGS*2);

	if (bConquest)
	{
		for (int iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
		{
			for (int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXYWithRangeCheck(iOldCityX, iOldCityY, iDX, iDY, iMaxRange);
				if (pLoopPlot)
				{
					pLoopPlot->verifyUnitValidPlot();
				}
			}
		}

		// Check for Askia Achievement
		if( isHuman() && !CvPreGame::isNetworkMultiplayerGame() )
		{
			const char* pkLeaderKey = getLeaderTypeKey();
			if (pkLeaderKey && strcmp(pkLeaderKey, "LEADER_ASKIA") == 0)
			{
				CvCity *pkCaptialCity = getCapitalCity();
				if (pkCaptialCity != NULL)	// Shouldn't be NULL, but...
				{
					CvPlot *pkCapitalPlot = pkCaptialCity->plot();
					CvPlot *pkNewCityPlot = pNewCity->plot();
					if (pkCapitalPlot && pkNewCityPlot)
					{
						// Get the area each plot is located in.
						CvArea* pkCapitalArea = pkCapitalPlot->area();
						CvArea* pkNewCityArea = pkNewCityPlot->area();

						if (pkCapitalArea && pkNewCityArea)
						{
							// The area the new city is locate on has to be of a certain size to qualify so that tiny islands are not included
							#define ACHIEVEMENT_MIN_CONTINENT_SIZE	8
							if (pkNewCityArea->GetID() != pkCapitalArea->GetID() && pkNewCityArea->getNumTiles() >= ACHIEVEMENT_MIN_CONTINENT_SIZE)
							{
								gDLL->UnlockAchievement( ACHIEVEMENT_SPECIAL_WARCANOE );
							}
						}
					}
				}
			}
		}
	}

	pCityPlot->setRevealed(GET_PLAYER(eOldOwner).getTeam(), true);

	// If the old owner is "killed," then notify everyone's Grand Strategy AI
	if (GET_PLAYER(eOldOwner).getNumCities() == 0 && !GET_PLAYER(eOldOwner).GetPlayerTraits()->IsStaysAliveZeroCities())
	{
		if (!isMinorCiv() && !isBarbarian())
		{
			for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				if (GetID() != iMajorLoop && GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
				{
					// Have I met the player who killed the guy?
					if (GET_TEAM(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()).isHasMet(getTeam()))
					{
						GET_PLAYER((PlayerTypes) iMajorLoop).GetDiplomacyAI()->DoPlayerKilledSomeone(GetID(), eOldOwner);
					}
				}
			}
		}
	}
	// If not, old owner should look at city specializations
	else
	{
		GET_PLAYER(eOldOwner).GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_MY_CITY_CAPTURED);
	}

	// Do the same for the new owner
	GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_ENEMY_CITY_CAPTURED);

	bool bDisbanded = false;

	// In OCC games, all captured cities are toast
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		bDisbanded = true;
		disband(pNewCity);
	}
	else //if (bConquest)
	{
		// Is this City being Occupied?
		if (pNewCity->getOriginalOwner() != GetID())
		{
			pNewCity->SetOccupied(true);

			pNewCity->ChangeResistanceTurns(pNewCity->getPopulation());
		}

		long lResult = 0;

		if (lResult == 0)
		{
			PlayerTypes eLiberatedPlayer = NO_PLAYER;

			// Captured someone's city that didn't originally belong to us - Liberate a player?
			if (pNewCity->getOriginalOwner() != eOldOwner && pNewCity->getOriginalOwner() != GetID())
			{
				eLiberatedPlayer = pNewCity->getOriginalOwner();
				if (!CanLiberatePlayer(eLiberatedPlayer))
				{
					eLiberatedPlayer = NO_PLAYER;
				}
			}

			// AI decides what to do with a City
			if (!isHuman())
			{
				AI_conquerCity(pNewCity, eOldOwner); // could delete the pointer...
			}

			// Human decides what to do with a City
			else if (!GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
			{
				// Used to display info for annex/puppet/raze popup - turned off in DoPuppet and DoAnnex
				pNewCity->SetIgnoreCityForHappiness(true);

				// If this city wasn't originally ours, give the human the choice to annex or puppet it
				if (pNewCity->getOriginalOwner() != GetID())
				{
					if (GC.getGame().getActivePlayer() == GetID())
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_CAPTURED, pNewCity->GetID(), iCaptureGold, eLiberatedPlayer);
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
						// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
						CancelActivePlayerEndTurn();
					}
				}
			}

			// No choice but to capture it, tell about pillage gold (if any)
			else if (iCaptureGold > 0)
			{
				strBuffer = GetLocalizedText("TXT_KEY_POPUP_GOLD_CITY_CAPTURE", iCaptureGold, pNewCity->getNameKey());
				GC.GetEngineUserInterface()->AddCityMessage(0, pNewCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
			}
		}
	}

	// Cache whether the player is human or not.  If the player is killed, the CvPreGame::slotStatus is changed to SS_CLOSED
	// but the slot status is used to determine if the player is human or not, so it looks like it is an AI!
	// This should be fixed, but might have unforeseen ramifications so...
	CvPlayer& kOldOwner = GET_PLAYER(eOldOwner);
	bool bOldOwnerIsHuman = kOldOwner.isHuman();
	// This may 'kill' the player if it is deemed that he does not have the proper units to stay alive
	kOldOwner.verifyAlive();

	// You... you killed him!
	if (!kOldOwner.isAlive())
	{
		GET_TEAM(kOldOwner.getTeam()).SetKilledByTeam(getTeam());
		kOldOwner.SetEverConqueredBy(m_eID, true);

		// Leader pops up and whines
		if (!CvPreGame::isNetworkMultiplayerGame())		// Not in MP
		{
			if (!bOldOwnerIsHuman && !kOldOwner.isMinorCiv() && !kOldOwner.isBarbarian())
				kOldOwner.GetDiplomacyAI()->DoKilledByPlayer(GetID());
		}
	}

	// grant the new owner any of the plots that were purchased by the prior owner
	for (uint ui = 0; ui < aiPurchasedPlotX.size(); ui++)
	{
		CvPlot* pPlot = GC.getMap().plot(aiPurchasedPlotX[ui], aiPurchasedPlotY[ui]);
		if (!bDisbanded)
		{
			if (pPlot->getOwner() != pNewCity->getOwner())
				pPlot->setOwner(pNewCity->getOwner(), /*iAcquireCityID*/ pNewCity->GetID(), /*bCheckUnits*/ true, /*bUpdateResources*/ true);
		}
		else
		{
			pPlot->setOwner(NO_PLAYER, -1, /*bCheckUnits*/ true, /*bUpdateResources*/ true);
		}
	}

	if (GC.getGame().getActiveTeam() == GET_PLAYER(eOldOwner).getTeam())
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(eOldOwner);
		args->Push(bCapital);
		args->Push(pNewCity->getX());
		args->Push(pNewCity->getY());
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "CityCaptureComplete", args.get(), bResult);
	}
#ifdef _MSC_VER
#pragma warning ( pop ) // restore warning level suppressed for pNewCity null check
#endif// _MSC_VER
}


//	--------------------------------------------------------------------------------
void CvPlayer::killCities()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->kill();
	}
}

//	--------------------------------------------------------------------------------
CvString CvPlayer::getNewCityName() const
{
	const CLLNode<CvString>* pNode;
	CvString strName;

	for (pNode = headCityNameNode(); (pNode != NULL); pNode = nextCityNameNode(pNode))
	{
		strName = pNode->m_data;
		if (isCityNameValid(strName, true))
		{
			strName = pNode->m_data;
			break;
		}
	}

	if (strName.IsEmpty())
	{
		getCivilizationCityName(strName, getCivilizationType());
	}

	if (strName.IsEmpty())
	{
		// Pick a name from another random civ
		int iRandOffset = GC.getGame().getJonRandNum(GC.getNumCivilizationInfos(), "Place Units (Player)");
		for (int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
		{
			int iLoopName = ((iI + iRandOffset) % GC.getNumCivilizationInfos());

			getCivilizationCityName(strName, ((CivilizationTypes)iLoopName));

			if (!isEmpty(strName))
			{
				break;
			}
		}
	}

	if (strName.IsEmpty())
	{
		strName = "TXT_KEY_CITY";
	}

	return strName;
}


//	--------------------------------------------------------------------------------
void CvPlayer::getCivilizationCityName(CvString& szBuffer, CivilizationTypes eCivilization) const
{
	int iRandOffset;
	int iLoopName;

	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCivilization);
	if(pkCivilizationInfo == NULL)
	{
		//This should never happen.
		return;
	}

	if (isBarbarian())
	{
		iRandOffset = GC.getGame().getJonRandNum(pkCivilizationInfo->getNumCityNames(), "Random Barb Name");
	}
	else
	{
		iRandOffset = 0;
	}

	// Minor Civs use special lists
	if (isMinorCiv())
	{
		CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType());
		if(pkMinorCivInfo)
		{
			CvMinorCivInfo& kMinorCivInfo = *pkMinorCivInfo;
			for (int iI = 0; iI < kMinorCivInfo.getNumCityNames(); iI++)
			{
				iLoopName = ((iI + iRandOffset) % kMinorCivInfo.getNumCityNames());

				const CvString strCityName = kMinorCivInfo.getCityNames(iLoopName);
				CvString strName = GetLocalizedText(strCityName.c_str());

				if (isCityNameValid(strName, true))
				{
					szBuffer = strCityName;
					break;
				}
			}
		}
	}
	else
	{
		CvCivilizationInfo& kCivInfo = *pkCivilizationInfo;
		for (int iI = 0; iI < kCivInfo.getNumCityNames(); iI++)
		{
			iLoopName = ((iI + iRandOffset) % kCivInfo.getNumCityNames());

			const CvString strCityName = kCivInfo.getCityNames(iLoopName);
			CvString strName = GetLocalizedText(strCityName.c_str());

			if (isCityNameValid(strName, true))
			{
				szBuffer = strCityName;
				break;
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isCityNameValid(CvString& szName, bool bTestDestroyed) const
{
	const CvCity* pLoopCity;
	int iLoop;

	if (bTestDestroyed)
	{
		if (GC.getGame().isDestroyedCityName(szName))
		{
			return false;
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->getName() == szName)
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getName() == szName)
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// This player liberates iOldCityID and gives it back to eMinor
void CvPlayer::DoLiberatePlayer(PlayerTypes ePlayer, int iOldCityID)
{
	CvCity* pCity = getCity(iOldCityID);
	CvAssert(pCity);
	if (!pCity)
		return;

	PlayerTypes eOldOwner = pCity->getOwner();
	CvPlot* pPlot = pCity->plot();

	// Set that this team has been liberated
	TeamTypes eTeam = getTeam();
	TeamTypes eLiberatedTeam = GET_PLAYER(ePlayer).getTeam();

	GET_TEAM(eLiberatedTeam).SetLiberatedByTeam(eTeam);

	// Put everyone at peace with this guy
	for (int iOtherTeamLoop = 0; iOtherTeamLoop < MAX_CIV_TEAMS; iOtherTeamLoop++)
	{
		if (eLiberatedTeam != iOtherTeamLoop)
			GET_TEAM(eLiberatedTeam).makePeace((TeamTypes) iOtherTeamLoop);
	}

	// Give the city back to the liberated player
	GET_PLAYER(ePlayer).acquireCity(pCity, false, true);

	// Is this a Minor we're liberating?
	if (GET_PLAYER(ePlayer).isMinorCiv())
	{
		GET_PLAYER(ePlayer).GetMinorCivAI()->DoLiberationByMajor(eOldOwner);
	}

	// Move Units from player that don't belong here
	if (pPlot->getNumUnits() > 0)
	{
		// Get the current list of units because we will possibly be moving them out of the plot's list
		IDInfoVector currentUnits;
		if (pPlot->getUnits(&currentUnits) > 0)
		{
			for(IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
			{
				CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

				if(pLoopUnit && pLoopUnit->getOwner() == eOldOwner)
				{
					pLoopUnit->finishMoves();
					if (!pLoopUnit->jumpToNearestValidPlot())
						pLoopUnit->kill(false);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::CanLiberatePlayer (PlayerTypes ePlayer)
{
	// Other Player must be dead now
	CvPlayer& kOtherPlayer = GET_PLAYER(ePlayer);
	if (kOtherPlayer.isAlive())
	{
		return false;
	}

	if (kOtherPlayer.IsEverConqueredBy(m_eID))
	{
		return false;
	}

	if (GET_TEAM(kOtherPlayer.getTeam()).GetKilledByTeam() == getTeam())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical)
{
	CvAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	if (eUnit == NO_UNIT)
		return NULL;

	CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);
	CvAssertFmt(pkUnitDef != NULL, "Trying to create unit of type %d, which does not exist", eUnit);
	if (pkUnitDef == NULL)
		return NULL;

	CvUnit* pUnit = addUnit();
	CvAssertMsg(pUnit != NULL, "Unit is not assigned a valid value");
	if (NULL != pUnit)
	{
		pUnit->init(pUnit->GetID(), eUnit, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(pkUnitDef->GetDefaultUnitAIType())) : eUnitAI), GetID(), iX, iY, eFacingDirection, bNoMove, bSetupGraphical);

		// slewis - added for the tutorial
		if (pUnit->getUnitInfo().GetWorkRate() > 0 && pUnit->getUnitInfo().GetDomainType() == DOMAIN_LAND)
		{
			m_bEverTrainedBuilder = true;
		}
		// end added for the tutorial
	}

	return pUnit;
}


//	--------------------------------------------------------------------------------
void CvPlayer::disbandUnit(bool)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	char szBuffer[1024];
	const size_t lenBuffer = 1024;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = INT_MAX;
	pBestUnit = NULL;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (!(pLoopUnit->hasCargo()))
		{
			if (!(pLoopUnit->isGoldenAge()))
			{
				if (pLoopUnit->getUnitInfo().GetProductionCost() > 0)
				{
					{
						iValue = (10000 + GC.getGame().getJonRandNum(1000, "Disband Unit"));

						iValue += (pLoopUnit->getUnitInfo().GetProductionCost() * 5);

						iValue += (pLoopUnit->getExperience() * 20);
						iValue += (pLoopUnit->getLevel() * 100);

						if (pLoopUnit->IsCanDefend() && pLoopUnit->plot()->isCity())
						{
							iValue *= 2;
						}

						if (pLoopUnit->plot()->getTeam() == pLoopUnit->getTeam())
						{
							iValue *= 3;
						}

						switch (pLoopUnit->AI_getUnitAIType())
						{
						case UNITAI_UNKNOWN:
							break;

						case UNITAI_SETTLE:
							iValue *= 20;
							break;

						case UNITAI_WORKER:
							iValue *= 10;
							break;

						case UNITAI_ATTACK:
						case UNITAI_CITY_BOMBARD:
						case UNITAI_FAST_ATTACK:
						case UNITAI_DEFENSE:
						case UNITAI_COUNTER:
							iValue *= 2;
							break;

						case UNITAI_RANGED:
						case UNITAI_CITY_SPECIAL:
						case UNITAI_PARADROP:
							iValue *= 6;
							break;

						case UNITAI_EXPLORE:
							iValue *= 15;
							break;

						case UNITAI_ARTIST:
						case UNITAI_SCIENTIST:
						case UNITAI_GENERAL:
						case UNITAI_MERCHANT:
						case UNITAI_ENGINEER:
						case UNITAI_SPACESHIP_PART:
						case UNITAI_TREASURE:
							break;

						case UNITAI_ICBM:
							iValue *= 4;
							break;

						case UNITAI_WORKER_SEA:
							iValue *= 18;
							break;

						case UNITAI_ATTACK_SEA:
						case UNITAI_RESERVE_SEA:
						case UNITAI_ESCORT_SEA:
							break;

						case UNITAI_EXPLORE_SEA:
							iValue *= 25;
							break;

						case UNITAI_ASSAULT_SEA:
						case UNITAI_SETTLER_SEA:
						case UNITAI_CARRIER_SEA:
						case UNITAI_MISSILE_CARRIER_SEA:
							iValue *= 5;
							break;

						case UNITAI_PIRATE_SEA:
						case UNITAI_ATTACK_AIR:
							break;

						case UNITAI_DEFENSE_AIR:
						case UNITAI_CARRIER_AIR:
						case UNITAI_MISSILE_AIR:
							iValue *= 3;
							break;

						default:
							CvAssert(false);
							break;
						}

						if (pLoopUnit->getUnitInfo().GetExtraMaintenanceCost() > 0)
						{
							iValue /= (pLoopUnit->getUnitInfo().GetExtraMaintenanceCost() + 1);
						}

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (GetID() == GC.getGame().getActivePlayer())
		{
			sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_UNIT_DISBANDED_NO_MONEY", pBestUnit->getNameKey()).GetCString());
			GC.GetEngineUserInterface()->AddUnitMessage(0, pBestUnit->GetIDInfo(), GetID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer);//, "AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pBestUnit->getX(), pBestUnit->getY(), true, true);
		}

		CvAssert(!(pBestUnit->isGoldenAge()));

		pBestUnit->kill(false);
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::killUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->kill(false);
	}
}


//	--------------------------------------------------------------------------------
/// The number of Builders a player has
int CvPlayer::GetNumBuilders() const
{
	return m_iNumBuilders;
}

//	--------------------------------------------------------------------------------
/// Sets the number of Builders a player has
void CvPlayer::SetNumBuilders(int iNum)
{
	if (GetNumBuilders() != iNum)
	{
		m_iNumBuilders = iNum;
	}
}

//	--------------------------------------------------------------------------------
/// Changes the number of Builders a player has
void CvPlayer::ChangeNumBuilders(int iChange)
{
	if (iChange != 0)
	{
		SetNumBuilders(GetNumBuilders() + iChange);
	}
}


//	--------------------------------------------------------------------------------
/// The maximum number of Builders a player can Train
int CvPlayer::GetMaxNumBuilders() const
{
	return m_iMaxNumBuilders;
}

//	--------------------------------------------------------------------------------
/// Sets the maximum number of Builders a player can Train
void CvPlayer::SetMaxNumBuilders(int iNum)
{
	if (GetMaxNumBuilders() != iNum)
	{
		m_iMaxNumBuilders = iNum;
	}
}

//	--------------------------------------------------------------------------------
/// Changes the maximum number of Builders a player can Train
void CvPlayer::ChangeMaxNumBuilders(int iChange)
{
	if (iChange != 0)
	{
		SetMaxNumBuilders(GetMaxNumBuilders() + iChange);
	}
}


//	--------------------------------------------------------------------------------
/// Returns number of Units a player has with a particular UnitAI.  The second argument allows you to check whether or not to include Units currently being trained in Cities.
int CvPlayer::GetNumUnitsWithUnitAI(UnitAITypes eUnitAIType, bool bIncludeBeingTrained, bool bIncludeWater)
{
	int iNumUnits = 0;

	CvUnit* pLoopUnit;
	CvCity* pLoopCity;
	int iLoop;

	// Current Units
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// Don't include Water Units if we don't want them
		if (pLoopUnit->getDomainType() != DOMAIN_SEA || bIncludeWater)
		{
			if (pLoopUnit->AI_getUnitAIType() == eUnitAIType)
			{
				iNumUnits++;
			}
		}
	}

	// Units being trained now
	if (bIncludeBeingTrained)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->isProductionUnit())
			{
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
				if(pkUnitEntry)
				{
					// Don't include Water Units if we don't want them
					if (pkUnitEntry->GetDomainType() != DOMAIN_SEA || bIncludeWater)
					{
						if (pkUnitEntry->GetDefaultUnitAIType() == eUnitAIType)
						{
							iNumUnits++;
						}
					}
				}
			}
		}
	}

	return iNumUnits;
}

//	--------------------------------------------------------------------------------
/// Returns number of Units a player has of a particular domain.  The second argument allows you to check whether or not to include civilians.
int CvPlayer::GetNumUnitsWithDomain(DomainTypes eDomain, bool bMilitaryOnly)
{
	int iNumUnits = 0;

	CvUnit* pLoopUnit;
	int iLoop;

	// Current Units
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getDomainType() == eDomain)
		{
			if (!bMilitaryOnly || pLoopUnit->IsCombatUnit())
			{
				iNumUnits++;
			}
		}
	}

	return iNumUnits;
}

//	-----------------------------------------------------------------------------------------------
/// Setting up danger plots
void CvPlayer::InitDangerPlots()
{
	m_pDangerPlots->Init(GetID(), true /*bAllocate*/);
}

//	-----------------------------------------------------------------------------------------------
void CvPlayer::UpdateDangerPlots()
{
	m_pDangerPlots->UpdateDanger(false, false);
}

//	-----------------------------------------------------------------------------------------------
void CvPlayer::SetDangerPlotsDirty()
{
	m_pDangerPlots->SetDirty();
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isHuman() const
{
	if (GetID() == NO_PLAYER)
	{
		return false;
	}

	return CvPreGame::isHuman(GetID());
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isObserver() const
{
	if(GetID() == NO_PLAYER)
	{
		return false;
	}

	return CvPreGame::slotStatus(GetID()) == SS_OBSERVER;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isBarbarian() const
{
	return (GetID() == BARBARIAN_PLAYER);
}

//	--------------------------------------------------------------------------------
void CvPlayer::doBarbarianRansom(int iOption, int iUnitID)
{
	UnitHandle pUnit = getUnit(iUnitID);

	// Pay the Price
	if (iOption == 0)
	{
		int iNumGoldStolen = GC.getBARBARIAN_UNIT_GOLD_RANSOM();	// 100

		if (iNumGoldStolen > GetTreasury()->GetGold())
		{
			iNumGoldStolen = GetTreasury()->GetGold();
		}

		// Unit is ransomed for Gold
		GetTreasury()->ChangeGold(-iNumGoldStolen);
	}
	// Leave them to the Barbs
	else if (iOption == 1)
	{
		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->AddUnitMessage(0, pUnit->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_RANSOM_KILL_BY_BARBARIANS", pUnit->getNameKey()));//,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, pUnit->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pUnit->getX(), pUnit->getY(), true, true);
		}

		pUnit->kill(true, BARBARIAN_PLAYER);
	}
}

//	-----------------------------------------------------------------------------------------------
const char* CvPlayer::getName() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetDescription();
	}

	if (GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGame().getGameState() == GAMESTATE_ON)
	{
		return getLeaderInfo().GetDescription();
	}

	if (GC.getGame().isGameMultiPlayer() && isHuman())
	{
		const CvString& szDisplayName = CvPreGame::nicknameDisplayed(GetID());
		if(szDisplayName.GetLength())
			return szDisplayName.c_str();
	}

	const CvString& szPlayerName = CvPreGame::leaderName(GetID());
	if(szPlayerName.GetLength() == 0)
	{
		return getLeaderInfo().GetDescription();
	}

	return szPlayerName.c_str();
}

//	-----------------------------------------------------------------------------------------------
const char* CvPlayer::getNameKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetTextKey();
	}

	if (GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive())
	{
		return getLeaderInfo().GetTextKey();
	}

	if (GC.getGame().isGameMultiPlayer() && isHuman())
	{
		// No, this won't be a 'key', but it should just pass through the lookup code and display as is.
		const CvString& szDisplayName = CvPreGame::nicknameDisplayed(GetID());
		if(szDisplayName.GetLength())
			return szDisplayName.c_str();
	}

	const CvString& szPlayerName = CvPreGame::leaderNameKey(GetID());
	if(szPlayerName.GetLength() == 0)
	{
		return getLeaderInfo().GetTextKey();
	}

	return szPlayerName.c_str();
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationDescription() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetDescription();
	}
	else if (CvPreGame::civilizationDescription(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().GetDescription();
	}
	else
	{
		return CvPreGame::civilizationDescription(GetID()).c_str();
	}
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationDescriptionKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetTextKey();
	}
	else if (CvPreGame::civilizationDescriptionKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().GetTextKey();
	}
	else
	{
		return CvPreGame::civilizationDescriptionKey(GetID()).c_str();
	}
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationShortDescription() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getShortDescription();
	}
	else if (CvPreGame::civilizationShortDescription(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getShortDescription();
	}
	else
	{
		return CvPreGame::civilizationShortDescription(GetID()).c_str();
	}
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationShortDescriptionKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getShortDescriptionKey();
	}
	else if (CvPreGame::civilizationShortDescriptionKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getShortDescriptionKey();
	}
	else
	{
		return CvPreGame::civilizationShortDescriptionKey(GetID()).c_str();
	}
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationAdjective() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getAdjective();
	}
	else if (CvPreGame::civilizationAdjective(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getAdjective();
	}
	else
	{
		return CvPreGame::civilizationAdjective(GetID()).c_str();
	}
}

//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationAdjectiveKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->getAdjectiveKeyWide();
	}
	else if (CvPreGame::civilizationAdjectiveKey(GetID()).GetLength() == 0)
	{
		return getCivilizationInfo().getAdjectiveKey();
	}
	else
	{
		return CvPreGame::civilizationAdjectiveKey(GetID()).c_str();
	}
}

//	--------------------------------------------------------------------------------
const char* CvPlayer::getCivilizationTypeKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetType();
	}
	else
	{
		return getCivilizationInfo().GetType();
	}
}

//	--------------------------------------------------------------------------------
const char* CvPlayer::getLeaderTypeKey() const
{
	if (isMinorCiv())
	{
		return GC.getMinorCivInfo(GetMinorCivAI()->GetMinorCivType())->GetType();
	}
	else
	{
		return getLeaderInfo().GetType();
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isWhiteFlag() const
{
	return CvPreGame::isWhiteFlag(GetID());
}


//	--------------------------------------------------------------------------------
const char* CvPlayer::GetStateReligionName() const
{
	return GetLocalizedText(m_strReligionKey.get());
}

//	--------------------------------------------------------------------------------
CvString CvPlayer::GetStateReligionKey() const
{
	return m_strReligionKey.get();
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetStateReligionKey(const char* strKey)
{
	m_strReligionKey = strKey;
}


//	--------------------------------------------------------------------------------
const CvString CvPlayer::getWorstEnemyName() const
{
	TeamTypes eWorstEnemy;

	eWorstEnemy = NO_TEAM;

	if (eWorstEnemy != NO_TEAM)
	{
		return GET_TEAM(eWorstEnemy).getName();
	}

	return "";
}


//	--------------------------------------------------------------------------------
ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if (CvPreGame::artStyle(GetID()) == NO_ARTSTYLE)
	{
		return ((ArtStyleTypes)(getCivilizationInfo().getArtStyleType()));
	}
	else
	{
		return CvPreGame::artStyle(GetID());
	}
}

//	---------------------------------------------------------------------------
void CvPlayer::doTurn()
{
	// Time building of these maps
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayer::doTurn(), Turn %d, %s", GC.getGame().getGameTurn(), getCivilizationShortDescription()));

	CvAssertMsg(isAlive(), "isAlive is expected to be true");

	doUpdateCacheOnTurn();

	AI_doTurnPre();

	if (getCultureBombTimer() > 0)
		changeCultureBombTimer(-1);

	if (getConversionTimer() > 0)
		changeConversionTimer(-1);

	if (GetTurnsSinceSettledLastCity() >= 0)
		ChangeTurnsSinceSettledLastCity(1);

	DoResearchAgreements();

	setConscriptCount(0);

	DoUpdateCramped();

	DoUpdateRevolts();

	bool bHasActiveDiploRequest = false;
	if (isAlive())
	{
		if (!isBarbarian())
		{
			if (!isMinorCiv())
			{
				GetGrandStrategyAI()->DoTurn();
				if (GC.getGame().isHotSeat() && !isHuman())
				{
					// In Hotseat, AIs only do their diplomacy pass for other AIs on their turn
					// Diplomacy toward a human is done at the beginning of the humans turn.
					GetDiplomacyAI()->DoTurn((PlayerTypes)CvDiplomacyAI::DIPLO_AI_PLAYERS);		// Do diplomacy for toward everyone
				}
				else
					GetDiplomacyAI()->DoTurn((PlayerTypes)CvDiplomacyAI::DIPLO_ALL_PLAYERS);	// Do diplomacy for toward everyone

				if (!isHuman())
					bHasActiveDiploRequest = CvDiplomacyRequests::HasActiveDiploRequestWithHuman(GetID());
			}
		}
	}

	if(isHuman() && !GC.getGame().isGameMultiPlayer())
		doArmySize();

	if( (bHasActiveDiploRequest || GC.GetEngineUserInterface()->isDiploActive()) && !GC.getGame().isGameMultiPlayer() && !isHuman())
	{
		GC.getGame().SetWaitingForBlockingInput(m_eID);
	}
	else
	{
		doTurnPostDiplomacy();
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerDoTurn", args.get(), bResult);
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::doTurnPostDiplomacy()
{
	CvGame& kGame = GC.getGame();

	if (isAlive())
	{
		if (!isBarbarian())
		{
			GetEconomicAI()->DoTurn();
			GetMilitaryAI()->DoTurn();
			GetCitySpecializationAI()->DoTurn();
		}

		if (isMinorCiv())
		{
			GetMinorCivAI()->DoTurn();
		}
	}

	// Attack bonus turns
	if (GetAttackBonusTurns() > 0)
	{
		ChangeAttackBonusTurns(-1);
	}

	// Golden Age
	DoProcessGoldenAge();

	// Great People gifts from Allied City States (if we have that policy)
	DoGreatPeopleSpawnTurn();

	// Do turn for all Cities
	if (getNumCities() > 0)
	{
		int iLoop = 0;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->doTurn();
		}
	}

	// Gold
	GetTreasury()->DoGold();

	// Culture

	// Prevent exploits in turn timed MP games - no accumulation of culture if player hasn't picked yet
	if (kGame.isOption(GAMEOPTION_END_TURN_TIMER_ENABLED))
	{
		if (getJONSCulture() < getNextPolicyCost())
			changeJONSCulture(GetTotalJONSCulturePerTurn());
	}
	else
	{
		changeJONSCulture(GetTotalJONSCulturePerTurn());
	}

	// Compute the cost of policies for this turn
	DoUpdateNextPolicyCost();

	// if this is the human player, have the popup come up so that he can choose a new policy
	if(isAlive() && isHuman() && getNumCities() > 0)
	{
		if (!GC.GetEngineUserInterface()->IsPolicyNotificationSeen())
		{
			if (getNextPolicyCost() <= getJONSCulture() && !IsAnarchy() && GetPlayerPolicies()->GetNumPoliciesCanBeAdopted() > 0)
			{
				CvNotifications* pNotifications = GetNotifications();
				if (pNotifications)
				{
					CvString strBuffer;

					if (kGame.isOption(GAMEOPTION_POLICY_SAVING))
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY_DISMISS");
					else
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY");

					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_CULTURE_FOR_POLICY");
					pNotifications->Add(NOTIFICATION_POLICY, strBuffer, strSummary, -1, -1, -1);
				}
			}
		}
	}

	// Science
	doResearch();

	// Anarchy counter
	if (GetAnarchyNumTurns() > 0)
		ChangeAnarchyNumTurns(-1);

	DoIncomingUnits();

	const int iGameTurn = kGame.getGameTurn();

	GatherPerTurnReplayStats(iGameTurn);

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);

	AI_doTurnPost();
}

//	--------------------------------------------------------------------------------
void CvPlayer::doTurnUnits()
{
	CvArmyAI* pLoopArmyAI;
	CvUnit* pLoopUnit;
	int iLoop;

	AI_doTurnUnitsPre();

	// Start: TACTICAL AI UNIT PROCESSING
	m_pTacticalAI->DoTurn();

	// Start: OPERATIONAL AI UNIT PROCESSING
	std::map<int, CvAIOperation*>::iterator iter;
	bool bKilledSomething;
	do
	{
		bKilledSomething = false;
		for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
		{
			CvAIOperation* pThisOperation = iter->second;
			if (pThisOperation)
			{
				if (pThisOperation->DoDelayedDeath())
				{
					bKilledSomething = true;
					break;
				}
			}
		}
	} while (bKilledSomething);

	for (pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
	{
		pLoopArmyAI->DoDelayedDeath();
	}

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->doDelayedDeath();
	}

	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation)
		{
			pThisOperation->DoTurn();
		}
	}

	do
	{
		bKilledSomething = false;
		for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
		{
			CvAIOperation* pThisOperation = iter->second;
			if (pThisOperation)
			{
				if (pThisOperation->DoDelayedDeath())
				{
					bKilledSomething = true;
					break;
				}
			}
		}
	} while (bKilledSomething);

	for (pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
	{
		pLoopArmyAI->DoTurn();
	}

	// Homeland AI
	m_pHomelandAI->DoTurn();

	// Start: old unit AI processing
	for (int iPass = 0; iPass < 4; iPass++)
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			switch (pLoopUnit->getDomainType())
			{
			case DOMAIN_AIR:
			if (iPass == 1)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_SEA:
				if (iPass == 2)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_LAND:
				if (iPass == 3)
				{
					pLoopUnit->doTurn();
				}
				break;
			case DOMAIN_IMMOBILE:
				if (iPass == 0)
				{
					pLoopUnit->doTurn();
				}
				break;
			case NO_DOMAIN:
				CvAssertMsg(false, "Unit with no Domain");
				break;
			default:
				if (iPass == 3)
				{
					pLoopUnit->doTurn();
				}
				break;
			}
		}
	}

	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(Waypoints_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);

	AI_doTurnUnitsPost();
}

//	--------------------------------------------------------------------------------
/// Indicate that the AI has not processed any units yet
void CvPlayer::SetAllUnitsUnprocessed()
{
	CvUnit *pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->SetTurnProcessed(false);
	}
}

//	--------------------------------------------------------------------------------
/// Units heal and then get their movement back
void CvPlayer::DoUnitReset()
{
	CvUnit *pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// HEAL UNIT?
		if (!pLoopUnit->isEmbarked())
		{
			if (pLoopUnit->hasMoved())
			{
				if (pLoopUnit->isAlwaysHeal())
				{
					pLoopUnit->doHeal();
				}
			}
			else
			{
				if (pLoopUnit->IsHurt())
				{
					pLoopUnit->doHeal();
				}
			}
		}

		int iCitadelDamage;
		if (pLoopUnit->IsNearEnemyCitadel(iCitadelDamage))
		{
			pLoopUnit->changeDamage(iCitadelDamage, NO_PLAYER, /*fAdditionalTextDelay*/ 0.5f);
		}

		// Finally (now that healing is done), restore movement points
		pLoopUnit->setMoves(pLoopUnit->maxMoves());
		pLoopUnit->SetIgnoreDangerWakeup(false);
		pLoopUnit->setMadeAttack(false);
		pLoopUnit->setMadeInterception(false);

		if (!isHuman())
		{
			const MissionData* pkMissionData = pLoopUnit->GetHeadMissionData();
			if (pkMissionData)
			{
				if (pkMissionData->eMissionType == CvTypes::getMISSION_RANGE_ATTACK() ||
					pkMissionData->eMissionType == CvTypes::getMISSION_AIRSTRIKE() ||
					pkMissionData->eMissionType == CvTypes::getMISSION_AIR_SWEEP() ||
					pkMissionData->eMissionType == CvTypes::getMISSION_NUKE())
				{
					CvAssertMsg(0, "An AI unit has a combat mission queued at the end of its turn.");
					pLoopUnit->ClearMissionQueue();	// Clear the whole thing, the AI will re-evaluate next turn.
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Damage units from attrition (start of turn so we can get notifications)
void CvPlayer::DoUnitAttrition()
{
	CvUnit *pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->DoAttrition();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::RespositionInvalidUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (!pLoopUnit)
		{
			continue;
		}

		if (pLoopUnit->isDelayedDeath())
		{
			continue;
		}

		if (pLoopUnit->isCargo())
		{
			continue;
		}

		if (pLoopUnit->isInCombat())
		{
			continue;
		}

		CvPlot* pPlot = pLoopUnit->plot();
		if (!pPlot)
		{
			continue;
		}

		if (pPlot->getNumFriendlyUnitsOfType(pLoopUnit) > GC.getPLOT_UNIT_LIMIT())
		{
			if (!pLoopUnit->jumpToNearestValidPlot())
				pLoopUnit->kill(false);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::updateYield()
{
	// This will go through all of the plots and update the yield if the player owns it.
	// The plot will not contribute to the player's yield unless it is worked by a city.
	// Previously this would just go through all the plots the city can work (3 rings around it)
	// but all plots have their yields updated on load and not updating them here could lead to 
	// a visual discrepancy.
	CvMap& kMap = GC.getMap();
	int iNumPlots = kMap.numPlots();
	PlayerTypes ePlayer = GetID();
	for (int iI = 0; iI < iNumPlots; iI++)
	{
		CvPlot* pkPlot = kMap.plotByIndexUnchecked(iI);
		if (pkPlot->getOwner() == ePlayer)
			pkPlot->updateYield();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::updateExtraSpecialistYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateExtraSpecialistYield();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::updateCityPlotYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateYield();
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::updateCitySight(bool bIncrement)
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateSight(bIncrement);
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::UpdateNotifications ()
{
	if (GetNotifications())
	{
		GetNotifications()->Update();
	}

	if (GetDiplomacyRequests())
	{
		GetDiplomacyRequests()->Update();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::updateTimers()
{
	CvUnit *pLoopUnit;
	int iLoop;
	m_endTurnBusyUnitUpdatesLeft--;
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->UpdateMission();
		pLoopUnit->doDelayedDeath();
	}

	GetDiplomacyAI()->update();
}

//	--------------------------------------------------------------------------------
bool CvPlayer::hasPromotableUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->isPromotionReady() && !pLoopUnit->isDelayedDeath())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::hasReadyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetCountReadyUnits() const
{
	int iRtnValue = 0;
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
const CvUnit *CvPlayer::GetFirstReadyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			return pLoopUnit;
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
void CvPlayer::EndTurnsForReadyUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->ReadyToMove() && !pLoopUnit->isDelayedDeath())
		{
			pLoopUnit->finishMoves();
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::hasAutoUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->ReadyToAuto())
		{
			return true;
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
bool CvPlayer::hasBusyUnit() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsBusy())
		{
			return true;
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
bool CvPlayer::hasBusyCity() const
{
	const CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsBusy())
		{
			return true;
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
const CvCity* CvPlayer::getBusyCity() const
{
	const CvCity* pLoopCity;
	int iLoop;

	for(pLoopCity = firstCity(&iLoop); pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsBusy())
		{
			return pLoopCity;
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
bool CvPlayer::hasBusyUnitOrCity() const
{
	if (hasBusyUnit())
		return true;
	return hasBusyCity();
}

//	--------------------------------------------------------------------------------
const UnitHandle CvPlayer::getBusyUnit() const
{
	const UnitHandle result;
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsBusy())
		{
			result = pLoopUnit;
		}
	}
	return result;
}


//	--------------------------------------------------------------------------------
void CvPlayer::chooseTech(int iDiscover, const char* strText, TechTypes iTechJustDiscovered)
{
	if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return;
	}

	if (iDiscover > 0)
	{
		SetNumFreeTechs(GetNumFreeTechs()+iDiscover);
	}

	if(iDiscover > 0)
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_FREE_TECH, strText, strText, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
	else if(strText == 0 || strText[0] == 0)
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_NEW_RESEARCH");
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_NEW_RESEARCH");
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_TECH, strBuffer, strSummary, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
	else
	{
		CvNotifications* pNotifications = GetNotifications();
		if(pNotifications)
		{
			pNotifications->Add(NOTIFICATION_TECH, strText, strText, -1, -1, iDiscover, iTechJustDiscovered);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Civ 5 Score
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
/// What is this player's score?
int CvPlayer::GetScore(bool bFinal, bool bWinner) const
{
	if (!isAlive())
		return 0;

	if (GET_TEAM(getTeam()).getNumMembers() == 0)
		return 0;

	int iScore = 0;

	iScore += GetScoreFromCities();
	iScore += GetScoreFromPopulation();
	iScore += GetScoreFromLand();
	iScore += GetScoreFromWonders();
	iScore += GetScoreFromTechs();
	iScore += GetScoreFromFutureTech();

	// If the game is over, we apply a mod to the value, rewarding players who finish early
	if (bFinal && bWinner)
	{
		int iGameProgressPercent = 100 * GC.getGame().getGameTurn() / GC.getGame().getEstimateEndTurn();
		iGameProgressPercent = iGameProgressPercent < 1 ? 1 : iGameProgressPercent;
		iScore *= 100;
		iScore /= iGameProgressPercent;
	}

	return iScore;
}

//	--------------------------------------------------------------------------------
// Score from Cities: 10 per city (with mod for map size)
int CvPlayer::GetScoreFromCities() const
{
	int iScore = getNumCities() * /*10*/ GC.getSCORE_CITY_MULTIPLIER();

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}

//	--------------------------------------------------------------------------------
// Score from Population: 6 per pop (with mod for map size)
int CvPlayer::GetScoreFromPopulation() const
{
	int iScore = getTotalPopulation() * /*3*/ GC.getSCORE_POPULATION_MULTIPLIER();

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}

//	--------------------------------------------------------------------------------
// Score from Land: 6 per plot (with mod for map size)
int CvPlayer::GetScoreFromLand() const
{
	int iScore = getTotalLand() * /*1*/ GC.getSCORE_LAND_MULTIPLIER();

	iScore *= GC.getGame().GetMapScoreMod();
	iScore /= 100;

	return iScore;
}

//	--------------------------------------------------------------------------------
// Score from world wonders: 40 per
int CvPlayer::GetScoreFromWonders() const
{
	int iScore = GetNumWonders() * /*40*/ GC.getSCORE_WONDER_MULTIPLIER();
	return iScore;
}

//	--------------------------------------------------------------------------------
// Score from Tech: 4 per
int CvPlayer::GetScoreFromTechs() const
{
	// Kludge to allow another custom scoring field (besides future tech) in scenarios where science if off
	if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		// Use obsolete tech score field to hold our custom score value
		return getTechScore();
	}

	// Normally we recompute it each time
	int iScore = GET_TEAM(getTeam()).GetTeamTechs()->GetNumTechsKnown() * /*4*/ GC.getSCORE_TECH_MULTIPLIER();
	return iScore;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeScoreFromTechs(int iChange)
{
	if (iChange != 0)
	{
		// Use obsolete tech score field to hold our custom score value
		changeTechScore(iChange);
	}
}

//	--------------------------------------------------------------------------------
// Score from Future Tech: 10 per
int CvPlayer::GetScoreFromFutureTech() const
{
	return m_iScoreFromFutureTech;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeScoreFromFutureTech(int iChange)
{
	if (iChange != 0)
		m_iScoreFromFutureTech += iChange;
}

//////////////////////////////////////////////////////////////////////////
// End Civ 5 Score
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	const CvCity* pLoopCity;
	const CvPlot* pLoopPlot;
	int iCount;
	int iLoop;
	int iI;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = plotCity(pLoopCity->getX(), pLoopCity->getY(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getFeatureType() == eFeature)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	const CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			iCount += pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding);
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
/// How many cities in the empire surrounded by features?
int CvPlayer::countCitiesFeatureSurrounded() const
{
	const CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsFeatureSurrounded())
			iCount ++;
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsCapitalConnectedToPlayer(PlayerTypes ePlayer, RouteTypes eRestrictRoute)
{
	// everybody needs to be alive!
	if (!isAlive() || !(GET_PLAYER(ePlayer).isAlive()))
	{
		return false;
	}

	CvCity* pOtherPlayerCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if (pOtherPlayerCapital == NULL)
	{
		return false;
	}

	return IsCapitalConnectedToCity(pOtherPlayerCapital, eRestrictRoute);
}

//	---------------------------------------------------------------------------
bool CvPlayer::IsCapitalConnectedToCity (CvCity* pCity, RouteTypes eRestrictRoute)
{
	CvCity* pPlayerCapital = getCapitalCity();
	if (pPlayerCapital == NULL)
	{
		return false;
	}

	int iPathfinderFlags = GetID() | MOVE_ROUTE_ALLOW_UNEXPLORED;	// Since we just want to know if we are connected or not, allow the check to search unexplored terrain.
	if (eRestrictRoute == NO_ROUTE)
	{
		iPathfinderFlags |= MOVE_ANY_ROUTE;
	}
	else
	{
		// assuming that there are fewer than 256 players
		int iRouteValue = eRestrictRoute + 1;
		iPathfinderFlags |= (iRouteValue << 8);
	}

	GC.getRouteFinder().ForceReset();
	if (GC.getRouteFinder().GeneratePath(pPlayerCapital->getX(), pPlayerCapital->getY(), pCity->getX(), pCity->getY(), iPathfinderFlags, false))
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvPlayer::findNewCapital()
{
	CvCity* pOldCapital;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	BuildingTypes eCapitalBuilding;
	int iValue;
	int iBestValue;
	int iLoop;

	eCapitalBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(GC.getCAPITAL_BUILDINGCLASS())));

	if (eCapitalBuilding == NO_BUILDING)
	{
		return;
	}

	pOldCapital = getCapitalCity();

	iBestValue = 0;
	pBestCity = NULL;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pOldCapital)
		{
			if (0 == pLoopCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding))
			{
				iValue = (pLoopCity->getPopulation() * 4);

				int iYieldValueTimes100 = pLoopCity->getYieldRateTimes100(YIELD_FOOD);
				iYieldValueTimes100 += (pLoopCity->getYieldRateTimes100(YIELD_PRODUCTION) * 3);
				iYieldValueTimes100 += (pLoopCity->getYieldRateTimes100(YIELD_GOLD) * 2);
				iValue += (iYieldValueTimes100 / 100);

				iValue += (pLoopCity->getNumGreatPeople() * 2);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestCity = pLoopCity;
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		if (pOldCapital != NULL)
		{
			pOldCapital->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 0);
		}
		CvAssertMsg(!(pBestCity->GetCityBuildings()->GetNumRealBuilding(eCapitalBuilding)), "(pBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
		pBestCity->GetCityBuildings()->SetNumRealBuilding(eCapitalBuilding, 1);
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canRaze(CvCity* pCity, bool bIgnoreCapitals) const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_CITY_RAZING))
	{
		return false;
	}

	// If we don't own this city right now then we can't raze it!
	if (pCity->getOwner() != GetID())
	{
		return false;
	}

	// Can't raze a city that originally belonged to us
	if (pCity->getOriginalOwner() == GetID())
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(pCity->getOwner());
		args->Push(pCity->GetID());

		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CanRazeOverride", args.get(), bResult))
		{
			// Check the result.
			if(bResult == true)
			{
				return true;
			}
		}
	}

	// No razing of capitals
	CvPlayer* pOriginalOwner = &GET_PLAYER(pCity->getOriginalOwner());
	bool bOriginalCapital =	pCity->getX() == pOriginalOwner->GetOriginalCapitalX() &&
									pCity->getY() == pOriginalOwner->GetOriginalCapitalY();

	if (!bIgnoreCapitals && pCity->IsEverCapital() && bOriginalCapital)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
void CvPlayer::raze(CvCity* pCity)
{
	char szBuffer[1024];
	const size_t lenBuffer = 1024;
	int iI;

	if (!canRaze(pCity))
	{
		return;
	}

	CvAssert(pCity->getOwner() == GetID());

	if (GetID() == GC.getGame().getActivePlayer())
	{
		sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_DESTROYED_CITY", pCity->getNameKey()).GetCString());
		GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer/*, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX(), pCity->getY(), true, true*/);

	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (iI != GetID() && iI == GC.getGame().getActivePlayer())
			{
				if (pCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
					GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), ((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer/*, "AS2D_CITYRAZED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX(), pCity->getY(), true, true*/);
				}
			}
		}
	}

	sprintf_s(szBuffer, lenBuffer, GetLocalizedText("TXT_KEY_MISC_CITY_RAZED_BY", pCity->getNameKey(), getCivilizationShortDescriptionKey()).GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), szBuffer, pCity->getX(), pCity->getY());

	pCity->SetIgnoreCityForHappiness(false);
	DoUpdateHappiness();

	pCity->ChangeRazingTurns(pCity->getPopulation());

	DoUpdateNextPolicyCost();

	// Update City UI
	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::unraze(CvCity* pCity)
{
	pCity->DoAnnex();
	pCity->ChangeRazingTurns(-pCity->GetRazingTurns());

	DoUpdateNextPolicyCost();

	// Update City UI
	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}



//	--------------------------------------------------------------------------------
void CvPlayer::disband(CvCity* pCity)
{
	if (getNumCities() == 1)
	{
		setFoundedFirstCity(false);
	}

	GC.getGame().addDestroyedCityName(pCity->getNameKey());

	for (int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
	{
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo( (BuildingTypes) eBuildingType );
		if(buildingInfo)
		{
			// if this building exists
			int iExists = pCity->GetCityBuildings()->GetNumRealBuilding( (BuildingTypes) eBuildingType );
			int iPreferredPosition = buildingInfo->GetPreferredDisplayPosition();
			if (iPreferredPosition > 0)
			{
				auto_ptr<ICvCity1> pDllCity(new CvDllCity(pCity));

				if (iExists > 0)
				{
					// kill the wonder
					GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), (BuildingTypes) eBuildingType, 0);
				}
				else
				{
					// else if we are currently in the process of building this wonder
					if (pCity->getProductionBuilding() == eBuildingType)
					{
						// kill the half built wonder
						if (isWorldWonderClass(buildingInfo->GetBuildingClassInfo()))
						{
							GC.GetEngineUserInterface()->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), (BuildingTypes) eBuildingType, 0);
						}
					}
				}
			}
		}
	}

	{
		auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pCity));
		gDLL->GameplayCitySetDamage(pkDllCity.get(), 0, pCity->getDamage());
		gDLL->GameplayCityDestroyed(pkDllCity.get(), NO_PLAYER);
	}

	pCity->kill();

}


//	--------------------------------------------------------------------------------
/// Is a Particular Goody ID a valid Goody for a certain plot?
bool CvPlayer::canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const
{
	CvCity* pCity;
	UnitTypes eUnit;
	bool bTechFound;
	int iI;

	Database::SingleResult kResult;
	CvGoodyInfo kGoodyInfo;
	const bool bResult = DB.SelectAt(kResult, "GoodyHuts", eGoody);
	DEBUG_VARIABLE(bResult);
	CvAssertMsg(bResult, "Cannot find goody info.");
	kGoodyInfo.CacheResult(kResult);

	if (!CvGoodyHuts::IsCanPlayerReceiveGoody(GetID(), eGoody))
	{
		return false;
	}

	// No XP in first 10 turns
	if (kGoodyInfo.getExperience() > 0)
	{
		if ((pUnit == NULL) || !(pUnit->canAcquirePromotionAny()) || (GC.getGame().getElapsedGameTurns() < 10))
		{
			return false;
		}
	}

	// Unit Healing
	if (kGoodyInfo.getDamagePrereq() > 0)
	{
		if ((pUnit == NULL) || (pUnit->getDamage() < ((pUnit->GetMaxHitPoints() * kGoodyInfo.getDamagePrereq()) / 100)))
		{
			return false;
		}
	}

	// Population
	if (kGoodyInfo.getPopulation() > 0)
	{
		if (getNumCities() == 0)
		{
			return false;
		}

		// Don't give more Population if we're already over our Pop limit
		if (IsEmpireUnhappy())
		{
			return false;
		}
	}

	// Reveal Nearby Barbs
	if (kGoodyInfo.getRevealNearbyBarbariansRange() > 0)
	{
		int iDX, iDY;
		int iBarbCampDistance = kGoodyInfo.getRevealNearbyBarbariansRange();
		CvPlot* pNearbyPlot;

		int iNumCampsFound = 0;

		ImprovementTypes barbCampType = (ImprovementTypes) GC.getBARBARIAN_CAMP_IMPROVEMENT();

		// Look at nearby Plots to make sure another camp isn't too close
		for (iDX = -(iBarbCampDistance); iDX <= iBarbCampDistance; iDX++)
		{
			for (iDY = -(iBarbCampDistance); iDY <= iBarbCampDistance; iDY++)
			{
				pNearbyPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

				if (pNearbyPlot != NULL)
				{
					if (plotDistance(pNearbyPlot->getX(), pNearbyPlot->getY(), pPlot->getX(), pPlot->getY()) <= iBarbCampDistance)
					{
						if (pNearbyPlot->getImprovementType() == barbCampType)
						{
							iNumCampsFound++;
						}
					}
				}
			}
		}

		// Needs to be at least 2 nearby Camps
		if (iNumCampsFound < 2)
		{
			return false;
		}
	}

	// Reveal Unknown Resource
	if (kGoodyInfo.isRevealUnknownResource())
	{
		// Can't get this if you have no Capital City
		if (getCapitalCity() == NULL)
		{
			return false;
		}

		CvResourceInfo* pResource;
		ResourceClassTypes eResourceClassBonus = (ResourceClassTypes) GC.getInfoTypeForString("RESOURCECLASS_BONUS");

		bool bPlayerDoesntKnowOfResource = false;

		int iNumResourceInfos = GC.getNumResourceInfos();
		for (int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
		{
			pResource = GC.getResourceInfo((ResourceTypes) iResourceLoop);

			// No "Bonus" Resources (that only give Yield), because those are lame to get from a Hut
			if (pResource != NULL && pResource->getResourceClassType() != eResourceClassBonus)
			{
				if (!GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) pResource->getTechReveal()))
				{
					bPlayerDoesntKnowOfResource = true;
				}
			}
		}

		// If the player already knows where all the Resources are then there's no point in this Goody
		if (!bPlayerDoesntKnowOfResource)
		{
			return false;
		}
	}

	// Unit Upgrade
	if (kGoodyInfo.isUpgradeUnit())
	{
		if (pUnit == NULL)
		{
			return false;
		}

		if (pUnit->IsHasBeenPromotedFromGoody())
		{
			return false;
		}

		UnitClassTypes eUpgradeUnitClass = (UnitClassTypes) GC.getUnitInfo(pUnit->getUnitType())->GetGoodyHutUpgradeUnitClass();

		if (eUpgradeUnitClass == NO_UNITCLASS)
		{
			return false;
		}

		UnitTypes eUpgradeUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUpgradeUnitClass);

		if (eUpgradeUnit == NO_UNIT)
		{
			return false;
		}
	}

	// Tech
	if (kGoodyInfo.isTech())
	{
		bTechFound = false;

		int iNumTechInfos = GC.getNumTechInfos();
		for (iI = 0; iI < iNumTechInfos; iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTech = GC.getTechInfo(eTech);
			if (pkTech != NULL && pkTech->IsGoodyTech())
			{
				if (GetPlayerTechs()->CanResearch(eTech))
				{					
					bool bUseTech = true;
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem)
					{
						CvLuaArgsHandle args;
						args->Push(GetID());
						args->Push(eTech);

						// Attempt to execute the game events.
						// Will return false if there are no registered listeners.
						bool bScriptResult = false;
						if (LuaSupport::CallTestAll(pkScriptSystem, "GoodyHutCanResearch", args.get(), bScriptResult)) 
						{
							bUseTech = bScriptResult;
						}
					}

					if(bUseTech)
					{
						bTechFound = true;
					}
					break;
				}
			}
		}

		if (!bTechFound)
		{
			return false;
		}
	}

	///////////////////////////////////////
	///////////////////////////////////////
	// Bad Goodies follow beneath this line
	///////////////////////////////////////
	///////////////////////////////////////

	if (kGoodyInfo.isBad())
	{
		if ((pUnit == NULL) || pUnit->isNoBadGoodies())
		{
			return false;
		}
	}

	if (kGoodyInfo.getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = ((UnitTypes)(getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getUnitClassType())));

		if (eUnit == NO_UNIT)
		{
			return false;
		}

		CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnit);
		if(pUnitInfo == NULL)
		{
			return false;
		}

		// No combat units in MP in the first 20 turns
		if (pUnitInfo->GetCombat() > 0)
		{
			if (GC.getGame().isGameMultiPlayer() || (GC.getGame().getElapsedGameTurns() < 20))
			{
				return false;
			}
		}

		// Builders
		if (pUnitInfo->GetWorkRate() > 0)
		{
			// Max limit
			if (GetMaxNumBuilders() > -1 && GetNumBuilders() >= GetMaxNumBuilders())
			{
				return false;
			}

			bool bHasTechWhichUnlocksImprovement = false;

			// Need a tech which unlocks something to do
			int iNumTechInfos = GC.getNumTechInfos();
			int iNumBuildInfos = GC.getNumBuildInfos();
			for (int iTechLoop = 0; iTechLoop < iNumTechInfos; iTechLoop++)
			{
				if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iTechLoop))
				{
					// Look at Builds
					for (int iBuildLoop = 0; iBuildLoop < iNumBuildInfos; iBuildLoop++)
					{
						CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iBuildLoop);
						if (!pkBuildInfo)
						{
							continue;
						}
						if (pkBuildInfo->getTechPrereq() == (TechTypes) iTechLoop)
						{
							if (pkBuildInfo->getImprovement() != NO_IMPROVEMENT || pkBuildInfo->getRoute() != NO_ROUTE)
							{
								bHasTechWhichUnlocksImprovement = true;
								break;
							}
						}
					}
				}
				// Already found something
				if (bHasTechWhichUnlocksImprovement)
				{
					break;
				}
			}

			// Player doesn't have any Tech which allows Improvements
			if (!bHasTechWhichUnlocksImprovement)
			{
				return false;
			}
		}

		// OCC games - no Settlers
		if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (pUnitInfo->IsFound() || pUnitInfo->IsFoundAbroad())
			{
				return false;
			}
		}
	}

	// Barbarians
	if (kGoodyInfo.getBarbarianUnitClass() != NO_UNITCLASS)
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
		{
			return false;
		}

		if (getNumCities() == 0)
		{
			return false;
		}

		if (getNumCities() == 1)
		{
			pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());

			if (pCity != NULL)
			{
				if (plotDistance(pPlot->getX(), pPlot->getY(), pCity->getX(), pCity->getY()) <= (8 - getNumCities()))
				{
					return false;
				}
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot = NULL;
	CvString strBuffer;
	CvString strTempBuffer;
	TechTypes eBestTech;
	UnitTypes eUnit;
	int iGold;
	int iOffset;
	int iRange;
	int iBarbCount;
	int iValue;
	int iBestValue;
	int iPass;
	int iDX, iDY;
	int iI;

	CvAssertMsg(canReceiveGoody(pPlot, eGoody, pUnit), "Instance is expected to be able to recieve goody");

	Database::SingleResult kResult;
	CvGoodyInfo kGoodyInfo;
	const bool bResult = DB.SelectAt(kResult, "GoodyHuts", eGoody);
	DEBUG_VARIABLE(bResult);
	CvAssertMsg(bResult, "Cannot find goody info.");
	kGoodyInfo.CacheResult(kResult);

	CvGoodyHuts::DoPlayerReceivedGoody(GetID(), eGoody);

	strBuffer = kGoodyInfo.GetDescription();

	// Gold
	iGold = kGoodyInfo.getGold() + (kGoodyInfo.getNumGoldRandRolls() * GC.getGame().getJonRandNum(kGoodyInfo.getGoldRandAmount(), "Goody Gold Rand"));

	if (iGold != 0)
	{
		GetTreasury()->ChangeGold(iGold);

		strBuffer += GetLocalizedText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
	}

	// Population
	if (kGoodyInfo.getPopulation() > 0)
	{
		int iDistance;
		int iBestCityDistance = -1;
		CvCity* pBestCity = NULL;

		CvCity* pLoopCity;
		int iLoop;
		// Find the closest City to us to add a Pop point to
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

			if (iBestCityDistance == -1 || iDistance < iBestCityDistance)
			{
				iBestCityDistance = iDistance;
				pBestCity = pLoopCity;
			}
		}

		if (pBestCity != NULL)
		{
			pBestCity->changePopulation(kGoodyInfo.getPopulation());
		}
	}

	// Culture
	int iCulture = kGoodyInfo.getCulture();
	if (iCulture > 0)
	{
		// Game Speed Mod
		iCulture *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iCulture /= 100;

		changeJONSCulture(iCulture);
	}

	// Reveal Nearby Barbs
	if (kGoodyInfo.getRevealNearbyBarbariansRange() > 0)
	{
		// Look at nearby Plots to make sure another camp isn't too close
		const int iBarbCampDistance = kGoodyInfo.getRevealNearbyBarbariansRange();
		for (iDX = -(iBarbCampDistance); iDX <= iBarbCampDistance; iDX++)
		{
			for (iDY = -(iBarbCampDistance); iDY <= iBarbCampDistance; iDY++)
			{
				CvPlot* pNearbyBarbarianPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
				if (pNearbyBarbarianPlot != NULL)
				{
					if (plotDistance(pNearbyBarbarianPlot->getX(), pNearbyBarbarianPlot->getY(), pPlot->getX(), pPlot->getY()) <= iBarbCampDistance)
					{
						if (pNearbyBarbarianPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
						{
							// Reveal Plot
							pNearbyBarbarianPlot->setRevealed(getTeam(), true);
							// Reveal Barb Camp here
							pNearbyBarbarianPlot->setRevealedImprovementType(getTeam(), pNearbyBarbarianPlot->getImprovementType());
						}
					}
				}
			}
		}
	}

	// Map
	iRange = kGoodyInfo.getMapRange();

	if (iRange > 0)
	{
		iOffset = kGoodyInfo.getMapOffset();

		if (iOffset > 0)
		{
			iBestValue = 0;
			pBestPlot = NULL;

			int iRandLimit;

			for (iDX = -(iOffset); iDX <= iOffset; iDX++)
			{
				for (iDY = -(iOffset); iDY <= iOffset; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iOffset);

					if (pLoopPlot != NULL)
					{
						if (!(pLoopPlot->isRevealed(getTeam())))
						{
							// Avoid water plots!
							if (pPlot->isWater())
								iRandLimit = 10;
							else
								iRandLimit = 10000;

							iValue = (1 + GC.getGame().getJonRandNum(iRandLimit, "Goody Map"));

							iValue *= plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pPlot;
		}

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXY(pBestPlot->getX(), pBestPlot->getY(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (plotDistance(pBestPlot->getX(), pBestPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
					{
						if (GC.getGame().getJonRandNum(100, "Goody Map") < kGoodyInfo.getMapProb())
						{
							pLoopPlot->setRevealed(getTeam(), true);
						}
					}
				}
			}
		}
	}

	// Experience
	if (pUnit != NULL)
	{
		pUnit->changeExperience(kGoodyInfo.getExperience());
	}

	// Unit Heal
	if (pUnit != NULL)
	{
		pUnit->changeDamage(-(kGoodyInfo.getHealing()));
	}

	// Reveal Unknown Resource
	if (kGoodyInfo.isRevealUnknownResource())
	{
		if (getCapitalCity() != NULL)
		{
			CvCity* pCapital = getCapitalCity();

			CvPlot* pResourcePlot;
			int iResourceDistance;
			TechTypes eRevealTech;
			int iResourceCost;
			int iBestResourceCost = -1;
			ResourceTypes eResource;
			ResourceTypes eBestResource = NO_RESOURCE;
			CvPlot* pBestResourcePlot = NULL;

			ResourceClassTypes eResourceClassBonus;

			// Look at Resources on all Plots
			for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				pResourcePlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
				eResource = pResourcePlot->getResourceType();

				if (eResource != NO_RESOURCE)
				{
					CvResourceInfo& pResource = *GC.getResourceInfo(eResource);
					eResourceClassBonus = (ResourceClassTypes) GC.getInfoTypeForString("RESOURCECLASS_BONUS");

					// No "Bonus" Resources (that only give Yield), because those are lame to get from a Hut
					if (pResource.getResourceClassType() != eResourceClassBonus)
					{
						// Can't be on a Plot that we've already force-revealed!
						if (!pResourcePlot->IsResourceForceReveal(getTeam()))
						{
							// Must be a Resource we don't already see
							eRevealTech = (TechTypes) pResource.getTechReveal();
							if (!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eRevealTech))
							{
								iResourceDistance = plotDistance(pResourcePlot->getX(), pResourcePlot->getY(), pCapital->getX(), pCapital->getY());

								// Must be within 10 plots of our Capital
								if (iResourceDistance <= 10)
								{
									iResourceCost = GC.getTechInfo(eRevealTech)->GetResearchCost();

									// Find the one with the cheapest Tech (or pick one if we haven't identified one yet)
									if (iBestResourceCost == -1 || iResourceCost < iBestResourceCost)
									{
										iBestResourceCost = iResourceCost;
										eBestResource = eResource;
										pBestResourcePlot = pResourcePlot;
									}
								}
							}
						}
					}
				}
			}

			CvAssert(pBestResourcePlot);

			// Did we find something to show?
			if (pBestResourcePlot != NULL)
			{
				pBestResourcePlot->setRevealed(getTeam(), true);
				pBestResourcePlot->SetResourceForceReveal(getTeam(), true);
				//pBestPlot->updateFog();

				if (getTeam() == GC.getGame().getActiveTeam())
				{
					pBestResourcePlot->setLayoutDirty(true);
				}

				// Also reveal adjacent Plots
				CvPlot* pAdjacentPlot;
				for (int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
				{
					pAdjacentPlot = plotDirection(pBestResourcePlot->getX(), pBestResourcePlot->getY(), ((DirectionTypes) iDirectionLoop));

					if (pAdjacentPlot != NULL)
					{
						pAdjacentPlot->setRevealed(getTeam(), true);
					}
				}

				CvString strTempString;
				strTempString.Format(" (%s)", GC.getResourceInfo(eBestResource)->GetDescription());
				strBuffer += strTempString;
			}
		}
	}

	// Unit Upgrade
	if (kGoodyInfo.isUpgradeUnit())
	{
		UnitTypes eUpgradeUnit = NO_UNIT;

		if(pUnit != NULL)
		{
			UnitClassTypes eUpgradeUnitClass = (UnitClassTypes) pUnit->getUnitInfo().GetGoodyHutUpgradeUnitClass();
			eUpgradeUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUpgradeUnitClass);
		}

		if (eUpgradeUnit != NO_UNIT)
		{
			// Add new upgrade Unit
			CvUnit* pNewUnit = initUnit(eUpgradeUnit, pPlot->getX(), pPlot->getY(), pUnit->AI_getUnitAIType(), NO_DIRECTION, false, false);
			pUnit->finishMoves();
			pUnit->SetBeenPromotedFromGoody(true);
			if (pNewUnit != NULL)
			{
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem)
				{
					CvLuaArgsHandle args;
					args->Push(GetID());
					args->Push(pUnit->GetID());
					args->Push(pNewUnit->GetID());
					args->Push(true); // bGoodyHut

					bool bScriptResult;
					LuaSupport::CallHook(pkScriptSystem, "UnitUpgraded", args.get(), bScriptResult);
				}


				pNewUnit->convert(pUnit);
				pNewUnit->setupGraphical();
			}
			else
				pUnit->kill(false);

			// Since the old unit died, it will block the goody reward popup unless we call this
			GC.GetEngineUserInterface()->SetDontShowPopups(false);
		}
	}

	// Tech
	if (kGoodyInfo.isTech())
	{
		iBestValue = 0;
		eBestTech = NO_TECH;

		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTech = GC.getTechInfo(eTech);
			if (pkTech != NULL && pkTech->IsGoodyTech())
			{
				if (GetPlayerTechs()->CanResearch(eTech))
				{
					bool bUseTech = true;

					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem)
					{
						CvLuaArgsHandle args;
						args->Push(GetID());
						args->Push(eTech);

						// Attempt to execute the game events.
						// Will return false if there are no registered listeners.
						bool bScriptResult = false;
						if (LuaSupport::CallTestAll(pkScriptSystem, "GoodyHutCanResearch", args.get(), bScriptResult))
						{
							bUseTech = bScriptResult;
						}
					}

					if(bUseTech)
					{
						iValue = (1 + GC.getGame().getJonRandNum(10000, "Goody Tech"));

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestTech = eTech;
						}
					}
				}
			}
		}

		CvAssertMsg(eBestTech != NO_TECH, "BestTech is not assigned a valid value");

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(GetID());
			args->Push(eBestTech);

			bool bScriptResult;
			LuaSupport::CallHook(pkScriptSystem, "GoodyHutTechResearched", args.get(), bScriptResult);
		}

		GET_TEAM(getTeam()).setHasTech(eBestTech, true, GetID(), true, true);
		GET_TEAM(getTeam()).GetTeamTechs()->SetNoTradeTech(eBestTech, true);
	}

	// Units
	if (kGoodyInfo.getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getUnitClassType());

		if (eUnit != NO_UNIT)
		{
			CvUnit* pNewUnit = initUnit(eUnit, pPlot->getX(), pPlot->getY());
			// see if there is an open spot to put him - no over-stacking allowed!
			if (pNewUnit && pUnit && pUnit->AreUnitsOfSameType(*pNewUnit)) // pUnit isn't in this plot yet (if it even exists) so we can't check on if we are over-stacked directly
			{
				pBestPlot = NULL;
				iBestValue = INT_MAX;
				const int iPopRange = 2;
				for (iDX = -(iPopRange); iDX <= iPopRange; iDX++)
				{
					for (iDY = -(iPopRange); iDY <= iPopRange; iDY++)
					{
						pLoopPlot	= plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iPopRange);
						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->isValidDomainForLocation(*pNewUnit))
							{
								if (pNewUnit->canMoveInto(*pLoopPlot))
								{
									if (pLoopPlot->getNumFriendlyUnitsOfType(pUnit) < GC.getPLOT_UNIT_LIMIT())
									{
										if (pNewUnit->canEnterTerritory(pLoopPlot->getTeam()) && !pNewUnit->isEnemy(pLoopPlot->getTeam(), pLoopPlot))
										{
											if ((pNewUnit->getDomainType() != DOMAIN_AIR) || pLoopPlot->isFriendlyCity(*pNewUnit, true))
											{
												if (pLoopPlot->isRevealed(getTeam()))
												{
													iValue = 1 + GC.getGame().getJonRandNum(6, "spawn goody unit that would over-stack"); // okay, I'll admit it, not a great heuristic

													if (plotDistance(pPlot->getX(),pPlot->getY(),pLoopPlot->getX(),pLoopPlot->getY()) > 1)
													{
														iValue += 12;
													}

													if (pLoopPlot->area() != pPlot->area()) // jumped to a different land mass, cool
													{
														iValue *= 10;
													}

													if (iValue < iBestValue)
													{
														iBestValue = iValue;
														pBestPlot = pLoopPlot;
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
				if (pBestPlot != NULL)
				{
					bool bVis = pBestPlot->isVisibleToWatchingHuman();
					pNewUnit->setXY(pBestPlot->getX(), pBestPlot->getY(), false, true, true && bVis, true);
					pNewUnit->SetPosition(pBestPlot);	// Need this to put the unit in the right spot graphically
					pNewUnit->finishMoves();
				}
				else
				{
					pNewUnit->kill(false);
				}
			}
		}
	}

	// Barbarians
	if (kGoodyInfo.getBarbarianUnitClass() != NO_UNITCLASS)
	{
		iBarbCount = 0;

		eUnit = (UnitTypes)GET_PLAYER(BARBARIAN_PLAYER).getCivilizationInfo().getCivilizationUnits(kGoodyInfo.getBarbarianUnitClass());

		if (eUnit != NO_UNIT)
		{
			for (iPass = 0; iPass < 10; iPass++)
			{
				if (iBarbCount < kGoodyInfo.getMinBarbarians())
				{
					for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->getArea() == pPlot->getArea())
							{
								if (!(pLoopPlot->isImpassable()) && !(pLoopPlot->getPlotCity()))
								{
									if (pLoopPlot->getNumUnits() == 0)
									{
										if ((iPass > 0) || (GC.getGame().getJonRandNum(100, "Goody Barbs") < kGoodyInfo.getBarbarianUnitProb()))
										{
											GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pLoopPlot->getX(), pLoopPlot->getY(), ((pLoopPlot->isWater()) ? UNITAI_ATTACK_SEA : UNITAI_ATTACK));
											iBarbCount++;

											if ((iPass > 0) && (iBarbCount == kGoodyInfo.getMinBarbarians()))
											{
												break;
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

	if (!strBuffer.empty() && GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->AddPlotMessage(0, pPlot->GetPlotIndex(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
	}

	// If it's the active player then show the popup
	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.getMap().updateDeferredFog();

		bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();

		// Don't show in MP, or if the player has turned it off
		if (!GC.getGame().isNetworkMultiPlayer() && !bDontShowRewardPopup)	// KWG: Candidate for !GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)
		{
			int iSpecialValue = 0;

			if (iGold > 0)
				iSpecialValue = iGold;
			else if (iCulture > 0)
				iSpecialValue = iCulture;

			CvPopupInfo kPopupInfo(BUTTONPOPUP_GOODY_HUT_REWARD, eGoody, iSpecialValue);
			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
			// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
			CancelActivePlayerEndTurn();
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit)
{

	CvHandicapInfo& playerHandicapInfo = getHandicapInfo();

	GoodyTypes eGoody;

	CvAssertMsg(pPlot->isGoody(), "pPlot->isGoody is expected to be true");

	if (!isBarbarian())
	{
		m_bEverPoppedGoody = true;
		pPlot->removeGoody();

		// Minors don't get Goodies :(
		if (isMinorCiv())
		{
			return;
		}

		// Need to have Goodies in the Handicap file to pick from
		if (playerHandicapInfo.getNumGoodies() > 0)
		{
			// Make a list of valid Goodies to pick randomly from
			int iValidGoodiesLoop;
			bool bValid;

			std::vector<GoodyTypes> avValidGoodies;
			for (int iGoodyLoop = 0; iGoodyLoop < playerHandicapInfo.getNumGoodies(); iGoodyLoop++)
			{
				eGoody = (GoodyTypes) playerHandicapInfo.getGoodies(iGoodyLoop);
				bValid = false;

				// Check to see if we've already verified this Goody is valid (since there can be multiples in the vector)
				for (iValidGoodiesLoop = 0; iValidGoodiesLoop < (int) avValidGoodies.size(); iValidGoodiesLoop++)
				{
					if (avValidGoodies[iValidGoodiesLoop] == eGoody)
					{
						avValidGoodies.push_back(eGoody);
						bValid = true;
						break;
					}
				}

				if (bValid)
					continue;

				if (canReceiveGoody(pPlot, eGoody, pUnit))
				{
					avValidGoodies.push_back(eGoody);
				}
			}

			// Any valid Goodies?
			if (!avValidGoodies.empty())
			{
				int iRand = GC.getGame().getJonRandNum(avValidGoodies.size(), "Picking a Goody result");

				eGoody = (GoodyTypes) avValidGoodies[iRand];
				receiveGoody(pPlot, eGoody, pUnit);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const
{
	CvPlot *pPlot;

	pPlot = GC.getMap().plot(iX, iY);

	// Has the AI agreed to not settle here?
	if (!isMinorCiv() && !isBarbarian())
	{
		if (pPlot->IsNoSettling(GetID()))
			return false;
	}

	// Settlers cannot found cities while empire is very unhappy
	if (!bTestVisible)
	{
		if (IsEmpireVeryUnhappy())
			return false;
	}

	return GC.getGame().GetSettlerSiteEvaluator()->CanFound(pPlot, this, bTestVisible);
}


//	--------------------------------------------------------------------------------
void CvPlayer::found(int iX, int iY)
{
	if (!canFound(iX, iY))
	{
		return;
	}

	SetTurnsSinceSettledLastCity(0);

	CvCity* pCity = initCity(iX, iY);
	CvAssertMsg(pCity != NULL, "City is not assigned a valid value");
	if (pCity == NULL)
		return;

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iI);
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(pkBuildingClassInfo)
		{
			const BuildingTypes eLoopBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iI)));
			if (eLoopBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eLoopBuilding);
				if(pkBuildingInfo)
				{
					if (pkBuildingInfo->GetFreeStartEra() != NO_ERA)
					{
						if (GC.getGame().getStartEra() >= pkBuildingInfo->GetFreeStartEra())
						{
							if (pCity->canConstruct(eLoopBuilding))
							{
								pCity->GetCityBuildings()->SetNumRealBuilding(eLoopBuilding, 1);
							}
						}
					}
				}
			}
		}
	}

	int iNumFreeCultureBuildings = GetNumCitiesFreeCultureBuilding();
	if (iNumFreeCultureBuildings > 0)
	{
		BuildingTypes eBuilding = pCity->ChooseFreeCultureBuilding();
		if (eBuilding != NO_BUILDING)
		{
			pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 1);
		}

		ChangeNumCitiesFreeCultureBuilding(-1);
	}

	DoUpdateNextPolicyCost();

	if (isHuman() && getAdvancedStartPoints() < 0)
	{
		// Human player is prompted to choose production BEFORE the AI runs for the turn.
		// So we'll force the AI strategies on the city now, just after it is founded.
		// And if the very first turn, we haven't even run player strategies once yet, so do that too.
		if (GC.getGame().getGameTurn() == 0)
		{
			this->GetEconomicAI()->DoTurn();
			this->GetMilitaryAI()->DoTurn();
		}
		pCity->GetCityStrategyAI()->DoTurn();
		pCity->chooseProduction();
		pCity->doFoundMessage();

		// If this is the first city (or we still aren't getting tech for some other reason notify the player)
		if (GetPlayerTechs()->GetCurrentResearch() == NO_TECH && GetScienceTimes100() > 0)
		{
			if (GetID() == GC.getGame().getActivePlayer())
			{
				chooseTech();
			}
		}
	}
	else
	{
		pCity->doFoundMessage();

		// AI civ, may need to redo city specializations
		GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_CITY_FOUNDED);
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUniqueUnitStatus, CvString* toolTipSink) const
{
	CvUnitEntry* pUnitInfoPtr = GC.getUnitInfo(eUnit);
	if(pUnitInfoPtr == NULL)
		return false;

	CvUnitEntry& pUnitInfo = *pUnitInfoPtr;

	const UnitClassTypes eUnitClass = (UnitClassTypes) pUnitInfo.GetUnitClassType();
	if (eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	// Should we check whether this Unit has been blocked out by the civ XML?
	if (!bIgnoreUniqueUnitStatus)
	{
		UnitTypes eThisPlayersUnitType = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUnitClass);

		// If the player isn't allowed to train this Unit (via XML) then return false
		if (eThisPlayersUnitType != eUnit)
		{
			return false;
		}
	}

	if (!bIgnoreCost)
	{
		if (pUnitInfo.GetProductionCost() == -1)
		{
			return false;
		}
	}

	// One City Challenge
	if (pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
	{
		if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			return false;
		}
	}

	if (!bContinue)
	{
		if (!bTestVisible)
		{
			// Builder Limit
			if (pUnitInfo.GetWorkRate() > 0 && pUnitInfo.GetDomainType() == DOMAIN_LAND)
			{
				if (GetMaxNumBuilders() > -1 && GetNumBuilders() >= GetMaxNumBuilders())
				{
					return false;
				}
			}
		}
	}

	// Tech requirements
	if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetPrereqAndTech()))))
	{
		return false;
	}

	int iI;
	for (iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
	{
		if (pUnitInfo.GetPrereqAndTechs(iI) != NO_TECH)
		{
			if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetPrereqAndTechs(iI)))))
			{
				return false;
			}
		}
	}

	// Obsolete Tech
	if ((TechTypes)pUnitInfo.GetObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
		{
			return false;
		}
	}

	// Game Unit Class Max
	if (GC.getGame().isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}

	// Team Unit Class Max
	if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}

	// Player Unit Class Max
	if (isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}

	// Spaceship part we already have?
	ProjectTypes eProject = (ProjectTypes) pUnitInfo.GetSpaceshipProject();
	if (eProject != NO_PROJECT)
	{
		if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
			return false;

		int iUnitAndProjectCount = GET_TEAM(getTeam()).getProjectCount(eProject) + getUnitClassCount(eUnitClass) + GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0);
		if (iUnitAndProjectCount >= pkUnitClassInfo->getMaxPlayerInstances())
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		// Settlers
		if (pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
		{
			if (IsEmpireVeryUnhappy() && GC.getVERY_UNHAPPY_CANT_TRAIN_SETTLERS() == 1)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_VERY_UNHAPPY_SETTLERS");
				if (toolTipSink == NULL)
					return false;
			}
		}

		// Project required?
		ProjectTypes ePrereqProject = (ProjectTypes) pUnitInfo.GetProjectPrereq();
		if (ePrereqProject != NO_PROJECT)
		{
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo(ePrereqProject);
			if(pkProjectInfo)
			{
				if (GET_TEAM(getTeam()).getProjectCount(ePrereqProject) == 0)
				{
					GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_PROJECT_REQUIRED", pkProjectInfo->GetDescription());
					if (toolTipSink == NULL)
						return false;
				}
			}
		}

		// Resource Requirements
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo)
			{
				const int iNumResource = pUnitInfo.GetResourceQuantityRequirement(eResource);

				if (iNumResource > 0)
				{
					// Starting project, need enough Resources plus some to start
					if (!bContinue && getNumResourceAvailable(eResource) < iNumResource)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_LACKS_RESOURCES", pkResourceInfo->GetIconString(), pkResourceInfo->GetTextKey(), iNumResource);
						if (toolTipSink == NULL)
							return false;
					}
					// Continuing project, need enough Resources
					else if (bContinue && (getNumResourceAvailable(eResource) < 0))
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_LACKS_RESOURCES", pkResourceInfo->GetIconString(), pkResourceInfo->GetTextKey(), iNumResource);
						if (toolTipSink == NULL)
							return false;
					}
				}
			}

		}

		if (GC.getGame().isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_GAME_COUNT_MAX", "", "", pkUnitClassInfo->getMaxTeamInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_TEAM_COUNT_MAX", "", "", pkUnitClassInfo->getMaxTeamInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (isUnitClassMaxedOut(eUnitClass, (getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_PLAYER_COUNT_MAX", "", "", pkUnitClassInfo->getMaxPlayerInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (GC.getGame().isNoNukes() || !GC.getGame().isNukesValid())
		{
			if (pUnitInfo.GetNukeDamageLevel() != -1)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_NUKES");
				if (toolTipSink == NULL)
					return false;
			}
		}

		if (pUnitInfo.GetSpecialUnitType() != NO_SPECIALUNIT)
		{
			if (!(GC.getGame().isSpecialUnitValid((SpecialUnitTypes)(pUnitInfo.GetSpecialUnitType()))))
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_SPECIAL_UNIT");
				if (toolTipSink == NULL)
					return false;
			}
		}

	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eUnit);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanTrain", args.get(), bResult))
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


//	--------------------------------------------------------------------------------
bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, CvString* toolTipSink) const
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return false;

	CvBuildingEntry& pBuildingInfo = *pkBuildingInfo;

	int iI;
	CvTeam& currentTeam = GET_TEAM(getTeam());

	const BuildingClassTypes eBuildingClass = ((BuildingClassTypes)(pBuildingInfo.GetBuildingClassType()));
	const CvBuildingClassInfo& kBuildingClass = pkBuildingInfo->GetBuildingClassInfo();

	// Checks to make sure civilization doesn't have an override that prevents construction of this building
	if (getCivilizationInfo().getCivilizationBuildings(eBuildingClass) != eBuilding)
	{
		return false;
	}

	if (!bIgnoreCost)
	{
		if (pBuildingInfo.GetProductionCost() == -1)
		{
			return false;
		}
	}

	if (!(currentTeam.GetTeamTechs()->HasTech((TechTypes)(pBuildingInfo.GetPrereqAndTech()))))
	{
		return false;
	}

	for (iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
	{
		if (pBuildingInfo.GetPrereqAndTechs(iI) != NO_TECH)
		{
			if (!(currentTeam.GetTeamTechs()->HasTech((TechTypes)(pBuildingInfo.GetPrereqAndTechs(iI)))))
			{
				return false;
			}
		}
	}

	if (currentTeam.isObsoleteBuilding(eBuilding))
	{
		return false;
	}

	// Building upgrade to another type
	BuildingClassTypes eReplacementBuildingClass = (BuildingClassTypes) pBuildingInfo.GetReplacementBuildingClass();

	if (eReplacementBuildingClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eUpgradeBuilding = ((BuildingTypes) (getCivilizationInfo().getCivilizationBuildings(eReplacementBuildingClass)));

		if (canConstruct(eUpgradeBuilding))
		{
			return false;
		}
	}

	if (pBuildingInfo.GetVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGame().isVictoryValid((VictoryTypes)(pBuildingInfo.GetVictoryPrereq()))))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (currentTeam.getVictoryCountdown((VictoryTypes)pBuildingInfo.GetVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (pBuildingInfo.GetMaxStartEra() != NO_ERA)
	{
		if (GC.getGame().getStartEra() > pBuildingInfo.GetMaxStartEra())
		{
			return false;
		}
	}

	if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	if (currentTeam.isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	if (isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// Everything above this is what is checked to see if Building shows up in the list of construction items
	///////////////////////////////////////////////////////////////////////////////////

	if (!bTestVisible)
	{
		// Num buildings in the empire... uhhh, how is this different from the very last check in this function? (JON: It doesn't appear to be used, but I can't say for sure :)
		CvCivilizationInfo &civilizationInfo = getCivilizationInfo();
		int numBuildingClassInfos = GC.getNumBuildingClassInfos();

		for (iI = 0; iI < numBuildingClassInfos; iI++)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
			if (!pkBuildingClassInfo)
			{
				continue;
			}

			BuildingTypes ePrereqBuilding = (BuildingTypes)civilizationInfo.getCivilizationBuildings(iI);

			if (NO_BUILDING != ePrereqBuilding && currentTeam.isObsoleteBuilding(ePrereqBuilding))
			{
				CvBuildingEntry* pkPrereqBuilding = GC.getBuildingInfo(ePrereqBuilding);
				if(pkPrereqBuilding)
				{
					int iNumHave = getBuildingClassCount((BuildingClassTypes)iI);

					int iNumNeeded = getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes)iI, 0);

					if (iNumHave < iNumNeeded)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_COUNT_NEEDED", pkPrereqBuilding->GetTextKey(), "", iNumNeeded - iNumHave);

						if (toolTipSink == NULL)
							return false;
					}
				}
			}
		}

		// Resource Requirements
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource)
			{
				int iNumResource = pBuildingInfo.GetResourceQuantityRequirement(eResource);
				if (iNumResource > 0)
				{
					if (bContinue)
						iNumResource = 0;

					if (getNumResourceAvailable(eResource) < iNumResource)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_LACKS_RESOURCES", pkResource->GetIconString(), pkResource->GetTextKey(), iNumResource);
						if (toolTipSink == NULL)
							return false;
					}
				}
			}
		}

		if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_GAME_COUNT_MAX", "", "", kBuildingClass.getMaxGlobalInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (currentTeam.isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_TEAM_COUNT_MAX", "", "", kBuildingClass.getMaxTeamInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (isBuildingClassMaxedOut(eBuildingClass, (getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_PLAYER_COUNT_MAX", "", "", kBuildingClass.getMaxPlayerInstances());
			if (toolTipSink == NULL)
				return false;
		}

		if (getNumCities() < pBuildingInfo.GetNumCitiesPrereq())
		{
			return false;
		}

		if (getHighestUnitLevel() < pBuildingInfo.GetUnitLevelPrereq())
		{
			return false;
		}

		// How does this differ from the check above?
		BuildingTypes ePrereqBuilding;
		int iNumNeeded;
		for (iI = 0; iI < numBuildingClassInfos; iI++)
		{
			iNumNeeded = getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI), bContinue);
			//int iNumHave = getBuildingClassCount((BuildingClassTypes)iI);
			ePrereqBuilding = (BuildingTypes) civilizationInfo.getCivilizationBuildings(iI);
			if (NO_BUILDING != ePrereqBuilding)
			{
				CvBuildingEntry* pkPrereqBuilding = GC.getBuildingInfo(ePrereqBuilding);
				if(pkPrereqBuilding)
				{
					int iNumHave = 0;
					const CvCity* pLoopCity = NULL;
					int iLoop;
					for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if (pLoopCity && !pLoopCity->IsPuppet() && pLoopCity->GetCityBuildings()->GetNumBuilding(ePrereqBuilding) > 0)
						{
							iNumHave++;
						}
					}

					if (iNumHave < iNumNeeded)
					{
						ePrereqBuilding = (BuildingTypes) civilizationInfo.getCivilizationBuildings(iI);

						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_COUNT_NEEDED", pkPrereqBuilding->GetTextKey(), "", iNumNeeded - iNumHave);

						if (toolTipSink == NULL)
							return false;

						// If we have less than 5 to go, list what cities need them
						int iNonPuppetCities = getNumCities() - GetNumPuppetCities();
						if (iNumNeeded == iNonPuppetCities && iNumNeeded - iNumHave < 5)
						{
							(*toolTipSink) += "[NEWLINE]";

							for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
							{
								if (pLoopCity && !pLoopCity->IsPuppet() && pLoopCity->GetCityBuildings()->GetNumBuilding(ePrereqBuilding) == 0)
								{
									(*toolTipSink) += pLoopCity->getName();
									(*toolTipSink) += " ";
								}
							}
						}
					}
				}
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eBuilding);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanConstruct", args.get(), bResult))
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


//	--------------------------------------------------------------------------------
bool CvPlayer::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(!pkProjectInfo)
	{
		return false;
	}

	CvProjectEntry& pProjectInfo = *pkProjectInfo;

	int iI;

	// No projects for barbs
	if (isBarbarian())
	{
		return false;
	}

	// no minors either
	if (isMinorCiv())
	{
		return false;
	}

	// If cost is -1 then that means it can't be built
	if (pProjectInfo.GetProductionCost() == -1)
	{
		return false;
	}

	// Tech requirement
	if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pProjectInfo.GetTechPrereq()))))
	{
		return false;
	}

	// Policy branch requirement?
	if (pProjectInfo.GetCultureBranchesRequired() > 0)
	{
		if (GetPlayerPolicies()->GetNumPolicyBranchesFinished() < pProjectInfo.GetCultureBranchesRequired())
		{
			return false;
		}
	}

	// Requires a particular victory condition to be enabled?
	if (pProjectInfo.GetVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGame().isVictoryValid((VictoryTypes)(pProjectInfo.GetVictoryPrereq()))))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (GET_TEAM(getTeam()).getVictoryCountdown((VictoryTypes)pProjectInfo.GetVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (!bTestVisible)
	{
		// Resource Requirements
		ResourceTypes eResource;
		int iNumResource;
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;
			iNumResource = pProjectInfo.GetResourceQuantityRequirement(eResource);

			if (iNumResource > 0)
			{
				if (getNumResourceAvailable(eResource) < iNumResource)
				{
					return false;
				}
			}
		}

		if (GC.getGame().isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		// Nukes disabled? (by UN or something)
		if (GC.getGame().isNoNukes())
		{
			if (pProjectInfo.IsAllowsNukes())
			{
				for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo((UnitTypes)iI);
					if (pkUnitEntry && pkUnitEntry->GetNukeDamageLevel() != -1)
					{
						return false;
					}
				}
			}
		}

		if (pProjectInfo.GetAnyoneProjectPrereq() != NO_PROJECT)
		{
			if (GC.getGame().getProjectCreatedCount((ProjectTypes)(pProjectInfo.GetAnyoneProjectPrereq())) == 0)
			{
				return false;
			}
		}

		for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).getProjectCount((ProjectTypes)iI) < pProjectInfo.GetProjectsNeeded(iI))
			{
				return false;
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eProject);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanCreate", args.get(), bResult))
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


//	--------------------------------------------------------------------------------
bool CvPlayer::canPrepare(SpecialistTypes eSpecialist, bool) const
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eSpecialist);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanPrepare", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::canMaintain(ProcessTypes eProcess, bool) const
{
	if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(GC.getProcessInfo(eProcess)->getTechPrereq()))))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eProcess);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanMaintain", args.get(), bResult))
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

//	--------------------------------------------------------------------------------
bool CvPlayer::isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const
{
	if (eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	if (GC.getGame().isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity) const
{
	if (eBuildingClass == NO_BUILDINGCLASS)
	{
		return false;
	}

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
	if(pkBuildingClassInfo == NULL)
	{
		return false;
	}

	if (!bAcquireCity)
	{
		if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
		{
			return true;
		}
	}

	if (GET_TEAM(getTeam()).isBuildingClassMaxedOut(eBuildingClass))
	{
		return true;
	}

	if (isBuildingClassMaxedOut(eBuildingClass, ((bAcquireCity) ? pkBuildingClassInfo->getExtraPlayerInstances() : 0)))
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isProductionMaxedProject(ProjectTypes eProject) const
{
	if (eProject == NO_PROJECT)
	{
		return false;
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getProductionNeeded(UnitTypes eUnit) const
{
	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);

	CvAssertMsg(pkUnitEntry, "This should never be hit");
	if(pkUnitEntry == NULL)
		return 0;

	UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitEntry->GetUnitClassType();
	CvAssert(NO_UNITCLASS != eUnitClass);

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	CvAssert(pkUnitClassInfo);
	if(pkUnitClassInfo == NULL)
		return 0;

	int iProductionNeeded = pkUnitEntry->GetProductionCost();
	iProductionNeeded *= 100 + getUnitClassCount(eUnitClass) * pkUnitClassInfo->getInstanceCostModifier();
	iProductionNeeded /= 100;

	if (isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getUNIT_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getTrainPercent();
	iProductionNeeded /= 100;

	if (!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if (isWorldUnitClass(eUnitClass))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldTrainPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAITrainPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	iProductionNeeded += getUnitExtraCost(eUnitClass);

	return std::max(1, iProductionNeeded);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getProductionNeeded(BuildingTypes eBuilding) const
{
	int iProductionNeeded;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		//This should never happen.
		return 1;
	}

	iProductionNeeded = pkBuildingInfo->GetProductionCost();

	if (pkBuildingInfo->GetNumCityCostMod() > 0 && getNumCities() > 0)
	{
		iProductionNeeded += (pkBuildingInfo->GetNumCityCostMod() * getNumCities());
	}

	if (isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getBUILDING_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getConstructPercent();
	iProductionNeeded /= 100;

	if (pkBuildingInfo->GetPrereqAndTech() != NO_TECH)
	{
		CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes)pkBuildingInfo->GetPrereqAndTech());
		if(pkTechInfo)
		{
			// Loop through all eras and apply Building production mod based on how much time has passed
			int iTotalEraMod = 100;
			EraTypes eBuildingUnlockedEra = (EraTypes) pkTechInfo->GetEra();

			if (eBuildingUnlockedEra < GetCurrentEra())
			{
				for (int iLoop = eBuildingUnlockedEra; iLoop < GetCurrentEra(); iLoop++)
				{
					CvAssertMsg(iLoop >= 0, "Loop should be within era bounds");
					CvAssertMsg(iLoop <GC.getNumEraInfos(), "Loop should be within era bounds");

					if (iLoop >= 0 && iLoop < GC.getNumEraInfos())
					{
						CvEraInfo* pkEraInfo = GC.getEraInfo((EraTypes)iLoop);
						if(pkEraInfo)
						{
							iTotalEraMod += pkEraInfo->getLaterEraBuildingConstructMod();
						}
					}
				}

				// Don't make a change if there's no change to make
				if (iTotalEraMod != 100)
				{
					iProductionNeeded *= iTotalEraMod;
					iProductionNeeded /= 100;
				}
			}
		}
	}

	if (!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if (isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldConstructPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIConstructPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getProductionNeeded(ProjectTypes eProject) const
{
	int iProductionNeeded;

	iProductionNeeded = GC.getProjectInfo(eProject)->GetProductionCost();

	if (isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getPROJECT_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getCreatePercent();
	iProductionNeeded /= 100;

	if (!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		if (isWorldProject(eProject))
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAIWorldCreatePercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getGame().getHandicapInfo().getAICreatePercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionNeeded(SpecialistTypes eSpecialist) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialistInfo == NULL)
	{
		//This should never happen! If this does, fix the calling function!
		CvAssert(pkSpecialistInfo);
		return 0;
	}

	int iProductionNeeded;
	iProductionNeeded = pkSpecialistInfo->getCost();

	if (isMinorCiv())
	{
		iProductionNeeded *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
		iProductionNeeded /= 100;
	}

	iProductionNeeded *= GC.getGame().getGameSpeedInfo().getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGame().getStartEraInfo().getCreatePercent();
	iProductionNeeded /= 100;

	return std::max(1, iProductionNeeded);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionModifier(CvString* toolTipSink) const
{
	int iMultiplier = 0;

	int iTempMod;

	// Unit Supply
	iTempMod = GetUnitProductionMaintenanceMod();
	iMultiplier += iTempMod;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_SUPPLY", iTempMod);

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionModifier(UnitTypes eUnit, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);

	if (pUnitEntry)
	{
		// Military bonus
		if (pUnitEntry->IsMilitaryProduction())
		{
			iTempMod = getMilitaryProductionModifier();
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_MILITARY_PLAYER", iTempMod);
		}

		// Settler bonus
		if (pUnitEntry->IsFound())
		{
			iTempMod = getSettlerProductionModifier();
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SETTLER_PLAYER", iTempMod);
		}

		// Religion bonus
		if (pUnitEntry->IsReligious())
		{
			iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_RELIGION_PRODUCTION_MODIFIER);
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_RELIGION_PLAYER", iTempMod);
		}

		// Unit Combat class bonus
		if (pUnitEntry->GetUnitCombatType() != NO_UNITCOMBAT)
		{
			iTempMod = getUnitCombatProductionModifiers((UnitCombatTypes) pUnitEntry->GetUnitCombatType());
			iMultiplier += iTempMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_COBMAT_CLASS_PLAYER", iTempMod);
		}

		// Trait bonus
		int iNumTraits = GC.getNumTraitInfos();
		CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
		for (int iI = 0; iI < iNumTraits; iI++)
		{
			if (pPlayerTraits->HasTrait((TraitTypes)iI))
			{
				iMultiplier += pUnitEntry->GetProductionTraits(iI);

				if (pUnitEntry->GetSpecialUnitType() != NO_SPECIALUNIT)
				{
					CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo((SpecialUnitTypes) pUnitEntry->GetSpecialUnitType());
					if(pkSpecialUnitInfo)
					{
						iTempMod = pkSpecialUnitInfo->getProductionTraits(iI);
						iMultiplier += iTempMod;
						GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_TRAIT", iTempMod);
					}
				}
			}
		}
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionModifier(BuildingTypes eBuilding, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return iMultiplier;
	}

	CvGame& kGame = GC.getGame();
	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();

	int iTempMod;

	int iNumTraits = GC.getNumTraitInfos();
	CvPlayerTraits* pPlayerTraits = GetPlayerTraits();
	for (int iI = 0; iI < iNumTraits; iI++)
	{
		if (pPlayerTraits->HasTrait((TraitTypes)iI))
		{
			iTempMod = pkBuildingInfo->GetProductionTraits(iI);
			iMultiplier += iTempMod;
			kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_TRAIT", iTempMod);
		}
	}

	// World Wonder
	if (::isWorldWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxGlobalBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WORLD_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}

	// Team Wonder
	else if (::isTeamWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxTeamBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_TEAM_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}

	// National Wonder
	else if (::isNationalWonderClass(kBuildingClassInfo))
	{
		iTempMod = getMaxPlayerBuildingProductionModifier();
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_NATIONAL_WONDER_PLAYER", iTempMod);
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_WONDER_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_POLICY", iTempMod);
	}

	// Normal Building
	else
	{
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_BUILDING_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_POLICY_PLAYER", iTempMod);
	}

	// Religion
	if (pkBuildingInfo->IsReligious())
	{
		iTempMod = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_RELIGION_PRODUCTION_MODIFIER);
		iMultiplier += iTempMod;
		kGame.BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_RELIGION_PLAYER", iTempMod);
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionModifier(ProjectTypes eProject, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	if (GC.getProjectInfo(eProject)->IsSpaceship())
	{
		iTempMod = getSpaceProductionModifier();
		iMultiplier += iTempMod;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPACE_PLAYER", iTempMod);
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getProductionModifier(SpecialistTypes, CvString* toolTipSink) const
{
	int iMultiplier = getProductionModifier(toolTipSink);
	int iTempMod;

	iTempMod = getSpecialistProductionModifier();
	iMultiplier += iTempMod;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPECIALIST_PLAYER", iTempMod);

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra) const
{
	CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
	if(pkBuilding == NULL)
	{
		CvAssertMsg(pkBuilding, "Should never happen...");
		return -1;
	}

	int iPrereqs = pkBuilding->GetPrereqNumOfBuildingClass(ePrereqBuildingClass);

	// dont bother with the rest of the calcs if we have no prereqs
	if (iPrereqs == 0)
	{
		return 0;
	}
	// -1 means Building is needed in all Cities
	else if (iPrereqs == -1)
	{
		int iNonPuppetCities = 0;
		int iLoop = 0;
		const CvCity* pLoopCity = NULL;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity && !pLoopCity->IsPuppet())
			{
				iNonPuppetCities++;
			}
		}

		return iNonPuppetCities;
	}
	else
	{
		iPrereqs *= std::max(0, GC.getMap().getWorldInfo().getBuildingClassPrereqModifier() + 100);
		iPrereqs /= 100;
	}

	if (!isLimitedWonderClass(pkBuilding->GetBuildingClassInfo()))
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuilding->GetBuildingClassType();
		iPrereqs *= (getBuildingClassCount(eBuildingClass) + iExtra + 1);
	}

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		iPrereqs = std::min(1, iPrereqs);
	}

	return iPrereqs;
}


//	--------------------------------------------------------------------------------
void CvPlayer::removeBuildingClass(BuildingClassTypes eBuildingClass)
{
	CvCity* pLoopCity;
	BuildingTypes eBuilding;
	int iLoop;

	eBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));

	if (eBuilding != NO_BUILDING)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
			{
				pLoopCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, 0);
				break;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// What is the effect of a building on the player?
void CvPlayer::processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, CvArea* pArea)
{
	int iI, iJ;

	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pBuildingInfo == NULL)
		return;

	// One-shot items
	if (bFirst && iChange > 0)
	{
		// Free Policies
		int iFreePolicies = pBuildingInfo->GetFreePolicies();
		if (iFreePolicies > 0)
			ChangeNumFreePolicies(iFreePolicies);

		int iFreeGreatPeople = pBuildingInfo->GetFreeGreatPeople();
		if (iFreeGreatPeople > 0)
			ChangeNumFreeGreatPeople(iFreeGreatPeople);

		// Golden Age
		if (pBuildingInfo->IsGoldenAge())
		{
			int iGoldenAgeTurns = getGoldenAgeLength();
			changeGoldenAgeTurns(iGoldenAgeTurns);
		}

		// Global Pop change
		if (pBuildingInfo->GetGlobalPopulationChange() != 0)
		{
			CvCity* pLoopCity;
			int iLoop;

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
					{
						if (pBuildingInfo->IsTeamShare() || (iI == GetID()))
						{
							for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
							{
								pLoopCity->setPopulation(std::max(1, (pLoopCity->getPopulation() + iChange * GC.getBuildingInfo(eBuilding)->GetGlobalPopulationChange())));
							}
						}
					}
				}
			}
		}

		// Free techs
		if (pBuildingInfo->GetFreeTechs() > 0)
		{
			if (!isHuman())
			{
				for (iI = 0; iI < pBuildingInfo->GetFreeTechs(); iI++)
				{
					for (int iLoop = 0; iLoop < iChange; iLoop++)
						AI_chooseFreeTech();
				}
			}
			else
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH");
				localizedText << pBuildingInfo->GetTextKey();
				chooseTech(pBuildingInfo->GetFreeTechs() * iChange, localizedText.toUTF8());
			}
		}
		if (pBuildingInfo->GetMedianTechPercentChange() > 0)
		{
			ChangeMedianTechPercentage(pBuildingInfo->GetMedianTechPercentChange());
		}

		// Free Gold
		if (pBuildingInfo->GetGold() > 0)
			GetTreasury()->ChangeGold(pBuildingInfo->GetGold());

		// Instant Friendship change with all Minors
		int iMinorFriendshipChange = pBuildingInfo->GetMinorFriendshipChange();
		if (iMinorFriendshipChange != 0)
		{
			int iNewValue;
			iMinorFriendshipChange += 100;	// Make it a mod

			for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				iNewValue = GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->GetFriendshipWithMajorTimes100(GetID());
				iNewValue *= iMinorFriendshipChange;
				iNewValue /= 100;

				GET_PLAYER((PlayerTypes) iMinorLoop).GetMinorCivAI()->SetFriendshipWithMajorTimes100(GetID(), iNewValue);
			}
		}
	}

	if (pBuildingInfo->GetFreeBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eFreeBuilding = (BuildingTypes)getCivilizationInfo().getCivilizationBuildings(pBuildingInfo->GetFreeBuildingClass());
		changeFreeBuildingCount(eFreeBuilding, iChange);
	}

	// Unit upgrade cost mod
	ChangeUnitUpgradeCostMod(pBuildingInfo->GetUnitUpgradeCostMod() * iChange);

	// Policy cost mod
	ChangePolicyCostBuildingModifier(pBuildingInfo->GetPolicyCostModifier() * iChange);

	// Border growth mods
	ChangePlotCultureCostModifier(pBuildingInfo->GetGlobalPlotCultureCostModifier() * iChange);
	ChangePlotGoldCostMod(pBuildingInfo->GetGlobalPlotBuyCostModifier() * iChange);

	// City Culture Mod
	ChangeJONSCultureCityModifier(pBuildingInfo->GetGlobalCultureRateModifier() * iChange);

	// Trade route gold modifier
	GetTreasury()->ChangeTradeRouteGoldModifier(pBuildingInfo->GetTradeRouteModifier() * iChange);

	// Free Promotion
	PromotionTypes eFreePromotion = (PromotionTypes) pBuildingInfo->GetFreePromotion();
	if (eFreePromotion != NO_PROMOTION)
		ChangeFreePromotionCount(eFreePromotion, iChange);

	// Free Promotion Removed
	PromotionTypes eFreePromotionRemoved = (PromotionTypes) pBuildingInfo->GetFreePromotionRemoved();
	if (eFreePromotionRemoved != NO_PROMOTION)
		ChangeFreePromotionCount(eFreePromotionRemoved, -iChange);

	// Extra Happiness Per City
	ChangeExtraHappinessPerCity(pBuildingInfo->GetHappinessPerCity() * iChange);

	// Extra Happiness Per Policy
	ChangeExtraHappinessPerXPolicies(pBuildingInfo->GetHappinessPerXPolicies() * iChange);

	// City Count Unhappiness Mod
	ChangeCityCountUnhappinessMod(pBuildingInfo->GetCityCountUnhappinessMod() * iChange);

	// Hurries
	for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryModifier((HurryTypes) iI, (pBuildingInfo->GetHurryModifier(iI) * iChange));
	}

	changeGreatPeopleRateModFromBldgs(pBuildingInfo->GetGlobalGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModFromBldgs(pBuildingInfo->GetGreatGeneralRateModifier() * iChange);
	ChangeGreatPersonExpendGold(pBuildingInfo->GetGreatPersonExpendGold() * iChange);
	recomputeGreatPeopleModifiers();

	changeGoldenAgeModifier(pBuildingInfo->GetGoldenAgeModifier() * iChange);
	changeFreeExperienceFromBldgs(pBuildingInfo->GetGlobalFreeExperience() * iChange);
	changeWorkerSpeedModifier(pBuildingInfo->GetWorkerSpeedModifier() * iChange);
	ChangeSpecialistCultureChange(pBuildingInfo->GetSpecialistExtraCulture() * iChange);
    changeBorderObstacleCount(pBuildingInfo->IsPlayerBorderObstacle() * iChange);

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		pArea->changeYieldRateModifier(GetID(), ((YieldTypes)iI), (pBuildingInfo->GetAreaYieldModifier(iI) * iChange));
		changeYieldRateModifier(((YieldTypes)iI), (pBuildingInfo->GetGlobalYieldModifier(iI) * iChange));
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeSpecialistExtraYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (pBuildingInfo->GetSpecialistYieldChange(iI, iJ) * iChange));
		}
	}
}

//	--------------------------------------------------------------------------------
/// Can we eBuild on pPlot?
bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra, bool bTestVisible, bool bTestGold) const
{
	if (!(pPlot->canBuild(eBuild, GetID(), bTestVisible)))
	{
		return false;
	}

	if (GC.getBuildInfo(eBuild)->getTechPrereq() != NO_TECH)
	{
		if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq())))
		{
			if ((!bTestEra && !bTestVisible) || ((GetCurrentEra() + 1) < GC.getTechInfo((TechTypes) GC.getBuildInfo(eBuild)->getTechPrereq())->GetEra()))
			{
				return false;
			}
		}
	}

	// Is this an improvement that is only useable by a specific civ?
	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry *pkEntry = GC.getImprovementInfo(eImprovement);
		if (pkEntry->IsSpecificCivRequired())
		{
			CivilizationTypes eCiv = pkEntry->GetRequiredCivilization();
			if (eCiv != getCivilizationType())
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (IsBuildBlockedByFeature(eBuild, pPlot->getFeatureType()))
		{
			return false;
		}

		if (bTestGold)
		{
			if (std::max(0, GetTreasury()->GetGold()) < getBuildCost(pPlot, eBuild))
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Are we prevented from eBuild-ing because of a Feature on this plot?
bool CvPlayer::IsBuildBlockedByFeature(BuildTypes eBuild, FeatureTypes eFeature) const
{
	// No Feature here to block us
	if (eFeature == NO_FEATURE)
	{
		return false;
	}

	// Build does not remove the Feature on pPlot
	if (!GC.getBuildInfo(eBuild)->isFeatureRemove(eFeature))
	{
		return false;
	}

	TechTypes ePrereqTech = (TechTypes) GC.getBuildInfo(eBuild)->getFeatureTech(eFeature);

	// Clearing Feature doesn't require any Tech, so we can do it right now if we have to
	if (ePrereqTech == NO_TECH)
	{
		return false;
	}

	// Clearing eFeature requires a Tech, but we have it
	if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
	{
		return false;
	}

	// Feature is blocking us!
	return true;
}

//	--------------------------------------------------------------------------------
// Returns the cost
int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	CvAssert(eBuild >= 0 && eBuild < GC.getNumBuildInfos());


	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if(pkBuildInfo == NULL)
	{
		return 0;
	}

	if (pPlot->getBuildProgress(eBuild) > 0)
	{
		return 0;
	}

	if (pPlot->getRouteType() != NO_ROUTE && pPlot->getRouteType() == pkBuildInfo->getRoute() && pPlot->IsRoutePillaged())
	{
		return 0;
	}

	int iBuildCost = pkBuildInfo->getCost();

	// Cost increases as more Improvements are built
	iBuildCost += (getTotalImprovementsBuilt() * pkBuildInfo->getCostIncreasePerImprovement());

	iBuildCost *= (100 + getImprovementCostModifier());
	iBuildCost /= 100;

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		iBuildCost += pkBuildInfo->getFeatureCost(pPlot->getFeatureType());
	}

	iBuildCost *= getHandicapInfo().getImprovementCostPercent();
	iBuildCost /= 100;

	iBuildCost *= GC.getGame().getGameSpeedInfo().getImprovementPercent();
	iBuildCost /= 100;

	return std::max(0, iBuildCost);
}

//	--------------------------------------------------------------------------------
RouteTypes CvPlayer::getBestRoute(CvPlot* pPlot) const
{
	RouteTypes eRoute;
	RouteTypes eBestRoute;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	eBestRoute = NO_ROUTE;

	for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			eRoute = ((RouteTypes)(pkBuildInfo->getRoute()));
			if (eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					if ((pPlot != NULL) ? ((pPlot->getRouteType() == eRoute) || canBuild(pPlot, eBuild)) : GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkBuildInfo->getTechPrereq())))
					{
						iValue = pkRouteInfo->getValue();

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getImprovementUpgradeRate() const
{
	int iRate;

	iRate = 100; // XXX

	iRate *= std::max(0, (getImprovementUpgradeRateModifier() + 100));
	iRate /= 100;

	return iRate;
}

//	--------------------------------------------------------------------------------
/// How much Production do we get from removing ANY Feature in the game? (Policy Bonus)
int CvPlayer::GetAllFeatureProduction() const
{
	return m_iAllFeatureProduction;
}

//	--------------------------------------------------------------------------------
/// Changes how much Production we get from removing ANY Feature in the game (Policy Bonus)
void CvPlayer::ChangeAllFeatureProduction(int iChange)
{
	if (iChange != 0)
	{
		m_iAllFeatureProduction += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// How far a tile is from a city (1-3) This is used to determine camera zoom on the city view
int CvPlayer::GetCityDistanceHighwaterMark() const
{
	return m_iCityDistanceHighwaterMark;
}

//	--------------------------------------------------------------------------------
/// Set how far a tile is from a city (1-3) This is used to determine camera zoom on the city view
void CvPlayer::SetCityDistanceHighwaterMark(int iNewValue)
{
	m_iCityDistanceHighwaterMark = iNewValue;
}


//	--------------------------------------------------------------------------------
int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	const CvCity* pLoopCity;
	int iTotalYield = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalYield += pLoopCity->getYieldRateTimes100(eYield);
	}

	return iTotalYield / 100;
}

//	--------------------------------------------------------------------------------
/// How much does Production is being eaten up by Units? (cached)
int CvPlayer::GetUnitProductionMaintenanceMod() const
{
	// Kind of a cop-out, but it fixes some bugs for now
	return calculateUnitProductionMaintenanceMod();
}

//	--------------------------------------------------------------------------------
/// How much does Production is being eaten up by Units? (update cache)
void CvPlayer::UpdateUnitProductionMaintenanceMod()
{
	m_iUnitProductionMaintenanceMod = calculateUnitProductionMaintenanceMod();

	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
/// How much does Production is being eaten up by Units?
int CvPlayer::calculateUnitProductionMaintenanceMod() const
{
	int iPaidUnits = GetNumUnitsOutOfSupply();

	// Example: Player can support 8 Units, he has 12. 4 * 10 means he loses 40% of his Production
	int iMaintenanceMod = min(/*70*/ GC.getMAX_UNIT_SUPPLY_PRODMOD(), iPaidUnits * 10);
	iMaintenanceMod = -iMaintenanceMod;

	return iMaintenanceMod;
}

//	--------------------------------------------------------------------------------
/// How many Units can we support fof ree without paying Production?
int CvPlayer::GetNumUnitsSupplied() const
{
	int iFreeUnits = GetNumUnitsSuppliedByHandicap();
	iFreeUnits += GetNumUnitsSuppliedByCities();
	iFreeUnits += GetNumUnitsSuppliedByPopulation();

	if (!isMinorCiv() && !isHuman() && !IsAITeammateOfHuman())
	{
		int iMod = (100 + GC.getGame().getHandicapInfo().getAIUnitSupplyPercent());
		iFreeUnits *= iMod;
		iFreeUnits /= 100;
	}

	return iFreeUnits;
}

//	--------------------------------------------------------------------------------
/// Units supplied from Difficulty Level
int CvPlayer::GetNumUnitsSuppliedByHandicap() const
{
	return getHandicapInfo().getProductionFreeUnits();
}

//	--------------------------------------------------------------------------------
/// Units supplied from Difficulty Level
int CvPlayer::GetNumUnitsSuppliedByCities() const
{
	return getHandicapInfo().getProductionFreeUnitsPerCity() * getNumCities();
}

//	--------------------------------------------------------------------------------
/// Units supplied from Difficulty Level
int CvPlayer::GetNumUnitsSuppliedByPopulation() const
{
	return getTotalPopulation() * getHandicapInfo().getProductionFreeUnitsPopulationPercent() / 100;
}

//	--------------------------------------------------------------------------------
/// How much Units are eating Production?
int CvPlayer::GetNumUnitsOutOfSupply() const
{
	int iFreeUnits = GetNumUnitsSupplied();

	return std::max(0, getNumUnits() - iFreeUnits);
}

//	--------------------------------------------------------------------------------
int CvPlayer::calculateUnitCost() const
{
	int iFreeUnits;
	int iPaidUnits;
	int iBaseUnitCost;
	int iExtraCost;

	return GetTreasury()->CalculateUnitCost(iFreeUnits, iPaidUnits, iBaseUnitCost, iExtraCost);
}

//	--------------------------------------------------------------------------------
int CvPlayer::calculateUnitSupply() const
{
	int iPaidUnits;
	int iBaseSupplyCost;

	return GetTreasury()->CalculateUnitSupply(iPaidUnits, iBaseSupplyCost);
}

//	--------------------------------------------------------------------------------
int CvPlayer::calculateResearchModifier(TechTypes eTech) const
{
	int iModifier = 100;

	if (NO_TECH == eTech)
	{
		return iModifier;
	}

	int iKnownCount = 0;
	int iPossibleKnownCount = 0;

	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && !kLoopTeam.isMinorCiv())
		{
			if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
				if (kLoopTeam.GetTeamTechs()->HasTech(eTech))
				{
					iKnownCount++;
				}
			}
			iPossibleKnownCount++;
		}
	}

	if (iPossibleKnownCount > 0)
	{
		iModifier += (GC.getTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER() * iKnownCount) / iPossibleKnownCount;
	}

	int iPossiblePaths = 0;
	int iUnknownPaths = 0;

	for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		if (GC.getTechInfo(eTech)->GetPrereqOrTechs(iI) != NO_TECH)
		{
			if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(GC.getTechInfo(eTech)->GetPrereqOrTechs(iI)))))
			{
				iUnknownPaths++;
			}

			iPossiblePaths++;
		}
	}

	CvAssertMsg(iPossiblePaths >= iUnknownPaths, "The number of possible paths is expected to match or exceed the number of unknown ones");

	iModifier += (iPossiblePaths - iUnknownPaths) * GC.getTECH_COST_KNOWN_PREREQ_MODIFIER();

	return iModifier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::calculateGoldRate() const
{
	return calculateGoldRateTimes100() / 100;
}


//	--------------------------------------------------------------------------------
int CvPlayer::calculateGoldRateTimes100() const
{
	// If we're in anarchy, then no Gold is collected!
	if (IsAnarchy())
	{
		return 0;
	}

	int iRate = 0;

	iRate = GetTreasury()->CalculateBaseNetGoldTimes100();

	return iRate;
}

//	--------------------------------------------------------------------------------
int CvPlayer::unitsRequiredForGoldenAge() const
{
	return (GC.getBASE_GOLDEN_AGE_UNITS() + (getNumUnitGoldenAges() * GC.getGOLDEN_AGE_UNITS_MULTIPLIER()));
}


//	--------------------------------------------------------------------------------
int CvPlayer::unitsGoldenAgeCapable() const
{
	const CvUnit* pLoopUnit;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->isGoldenAge())
		{
			iCount++;
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvPlayer::unitsGoldenAgeReady() const
{
	const CvUnit* pLoopUnit;
	bool* pabUnitUsed;
	int iCount;
	int iLoop;
	int iI;

	pabUnitUsed = FNEW(bool[GC.getNumUnitInfos()], c_eCiv5GameplayDLL, 0);

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	iCount = 0;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (!(pabUnitUsed[pLoopUnit->getUnitType()]))
		{
		if (pLoopUnit->isGoldenAge())
		{
				pabUnitUsed[pLoopUnit->getUnitType()] = true;
				iCount++;
			}
		}
	}

	SAFE_DELETE_ARRAY(pabUnitUsed);

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvPlayer::greatGeneralThreshold() const
{
	int iThreshold;

	iThreshold = ((/*200*/ GC.getGREAT_GENERALS_THRESHOLD() * std::max(0, (getGreatGeneralsThresholdModifier() + 100))) / 100);

	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= std::max(1, GC.getGame().getGameSpeedInfo().getTrainPercent());

	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}


//	--------------------------------------------------------------------------------
int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialistInfo == NULL)
	{
		//This function REQUIRES a valid specialist info.
		CvAssert(pkSpecialistInfo);
		return 0;
	}

	return (pkSpecialistInfo->getYieldChange(eYield) + getSpecialistExtraYield(eSpecialist, eYield) + getSpecialistExtraYield(eYield));
}

//	--------------------------------------------------------------------------------
/// How much additional Yield does every City produce?
int CvPlayer::GetCityYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCityYieldChange[eYield];
}

//	--------------------------------------------------------------------------------
/// Changes how much additional Yield every City produces
void CvPlayer::ChangeCityYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCityYieldChange.setAt(eYield, m_aiCityYieldChange[eYield] + iChange);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// How much additional Yield do coastal Cities produce?
int CvPlayer::GetCoastalCityYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCoastalCityYieldChange[eYield];
}

//	--------------------------------------------------------------------------------
/// Changes how much additional Yield coastal Cities produce
void CvPlayer::ChangeCoastalCityYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCoastalCityYieldChange.setAt(eYield, m_aiCoastalCityYieldChange[eYield] + iChange);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// How much additional Yield does the Capital produce?
int CvPlayer::GetCapitalYieldChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldChange[eYield];
}

//	--------------------------------------------------------------------------------
/// Changes how much additional Yield the Capital produces
void CvPlayer::ChangeCapitalYieldChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalYieldChange.setAt(eYield, m_aiCapitalYieldChange[eYield] + iChange);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// How much additional Yield does the Capital produce per pop?
int CvPlayer::GetCapitalYieldPerPopChange(YieldTypes eYield) const
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldPerPopChange[eYield];
}

//	--------------------------------------------------------------------------------
/// Changes how much additional Yield the Capital produces per pop
void CvPlayer::ChangeCapitalYieldPerPopChange(YieldTypes eYield, int iChange)
{
	CvAssertMsg(eYield >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalYieldPerPopChange.setAt(eYield, m_aiCapitalYieldPerPopChange[eYield] + iChange);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
CvPlot* CvPlayer::getStartingPlot() const
{
	return GC.getMap().plotCheckInvalid(m_iStartingX, m_iStartingY);
}


//	--------------------------------------------------------------------------------
void CvPlayer::setStartingPlot(CvPlot* pNewValue)
{
	CvPlot* pOldStartingPlot;

	pOldStartingPlot = getStartingPlot();

	if (pOldStartingPlot != pNewValue)
	{
		if (pOldStartingPlot != NULL)
		{
			pOldStartingPlot->area()->changeNumStartingPlots(-1);
		}

		if (pNewValue == NULL)
		{
			m_iStartingX = INVALID_PLOT_COORD;
			m_iStartingY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iStartingX = pNewValue->getX();
			m_iStartingY = pNewValue->getY();

			getStartingPlot()->setStartingPlot(true);

			CvArea* pArea = getStartingPlot()->area();
			if( pArea != NULL)
				pArea->changeNumStartingPlots(1);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getAveragePopulation() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	return ((getTotalPopulation() / getNumCities()) + 1);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeTotalPopulation(int iChange)
{
	changePopScore(-(getPopulationScore(getTotalPopulation())));

	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	CvAssert(getTotalPopulation() >= 0);

	changePopScore(getPopulationScore(getTotalPopulation()));
}


//	--------------------------------------------------------------------------------
long CvPlayer::getRealPopulation() const
{
	const CvCity* pLoopCity;
	__int64 iTotalPopulation = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalPopulation += pLoopCity->getRealPopulation();
	}

	if (iTotalPopulation > INT_MAX)
	{
		iTotalPopulation = INT_MAX;
	}

	return ((long)(iTotalPopulation));
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNewCityExtraPopulation() const
{
	return m_iNewCityExtraPopulation;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeNewCityExtraPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iNewCityExtraPopulation += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetFreeFoodBox() const
{
	return m_iFreeFoodBox;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeFreeFoodBox(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeFoodBox += iChange;
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	CvAssert(getTotalLand() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeTotalLandScored(int iChange)
{
	if (iChange != 0)
	{
		changeLandScore(-(getLandPlotsScore(getTotalLandScored())));

		m_iTotalLandScored = (m_iTotalLandScored + iChange);
		CvAssert(getTotalLandScored() >= 0);

		changeLandScore(getLandPlotsScore(getTotalLandScored()));
	}
}

//	--------------------------------------------------------------------------------
/// Total culture per turn
int CvPlayer::GetTotalJONSCulturePerTurn() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}

	int iCulturePerTurn = 0;

	// Culture per turn from Cities
	iCulturePerTurn += GetJONSCulturePerTurnFromCities();

	// Special bonus which adds excess Happiness to Culture?
	iCulturePerTurn += GetJONSCulturePerTurnFromExcessHappiness();

	// Free culture that's part of the player
	iCulturePerTurn += GetJONSCulturePerTurnForFree();

	// Culture from Minor Civs
	iCulturePerTurn += GetJONSCulturePerTurnFromMinorCivs();

	return iCulturePerTurn;
}

//	--------------------------------------------------------------------------------
/// Culture per turn from Cities
int CvPlayer::GetJONSCulturePerTurnFromCities() const
{
	int iCulturePerTurn = 0;

	// Add in culture from Cities
	const CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCulturePerTurn += pLoopCity->getJONSCulturePerTurn();
	}

	return iCulturePerTurn;
}

//	--------------------------------------------------------------------------------
/// Special bonus which adds excess Happiness to Culture?
int CvPlayer::GetJONSCulturePerTurnFromExcessHappiness() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return 0;
	}

	if (getHappinessToCulture() != 0)
	{
		if (GetExcessHappiness() > 0)
		{
			int iFreeCulture = GetExcessHappiness() * getHappinessToCulture();
			iFreeCulture /= 100;

			return iFreeCulture;
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
/// Culture per turn player starts with for free
int CvPlayer::GetJONSCulturePerTurnForFree() const
{
	// No culture during Anarchy
	if (IsAnarchy())
	{
		return 0;
	}

	return m_iJONSCulturePerTurnForFree;
}

//	--------------------------------------------------------------------------------
/// Culture per turn player starts with for free
void CvPlayer::ChangeJONSCulturePerTurnForFree(int iChange)
{
	if (iChange != 0)
		m_iJONSCulturePerTurnForFree += iChange;

	if (GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
/// Culture per turn from minor civs
int CvPlayer::GetJONSCulturePerTurnFromMinorCivs() const
{
	// No culture during Anarchy
	if (IsAnarchy())
	{
		return 0;
	}

	return m_iJONSCulturePerTurnFromMinorCivs;
}

//	--------------------------------------------------------------------------------
/// Culture per turn from minor civs
void CvPlayer::ChangeJONSCulturePerTurnFromMinorCivs(int iChange)
{
	if (iChange != 0)
		m_iJONSCulturePerTurnFromMinorCivs += iChange;

	if (GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
/// Modifier for all Cities' culture
int CvPlayer::GetJONSCultureCityModifier() const
{
	return m_iJONSCultureCityModifier;
}

//	--------------------------------------------------------------------------------
/// Modifier for all Cities' culture
void CvPlayer::ChangeJONSCultureCityModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iJONSCultureCityModifier += iChange;

		if (GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getJONSCulture() const
{
	// City States can't pick Policies, sorry!
	if (isMinorCiv())
		return 0;

	if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
	{
		return 0;
	}

	return m_iJONSCulture;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setJONSCulture(int iNewValue)
{
	if (getJONSCulture() != iNewValue)
	{
		// Add to the total we've ever had
		if (iNewValue > m_iJONSCulture)
		{
			ChangeJONSCultureEverGenerated(iNewValue - m_iJONSCulture);
		}

		m_iJONSCulture = iNewValue;

		if (GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeJONSCulture(int iChange)
{
	setJONSCulture(getJONSCulture() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::GetJONSCultureEverGenerated() const
{
	return m_iJONSCultureEverGenerated;
}


//	--------------------------------------------------------------------------------
void CvPlayer::SetJONSCultureEverGenerated(int iNewValue)
{
	if (GetJONSCultureEverGenerated() != iNewValue)
	{
		m_iJONSCultureEverGenerated = iNewValue;
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeJONSCultureEverGenerated(int iChange)
{
	SetJONSCultureEverGenerated(GetJONSCultureEverGenerated() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::GetJONSCulturePerCityPerTurn() const
{
	int iCulture = GetJONSCultureEverGenerated();
	int iNumCities = getNumCities();

	// Puppet Cities don't count
	iNumCities -= GetNumPuppetCities();

	int iNumTurns = GC.getGame().getElapsedGameTurns();

	if (iNumTurns == 0)
	{
		iNumTurns = 1;
	}

	int iCulturePerCityPerTurn = 100 * iCulture / iNumCities / iNumTurns;
	return iCulturePerCityPerTurn;
}

//	--------------------------------------------------------------------------------
/// Amount of extra Culture per Wonder
int CvPlayer::GetCulturePerWonder() const
{
	return m_iCulturePerWonder;
}

//	--------------------------------------------------------------------------------
/// Changes amount of extra Culture per Wonder
void CvPlayer::ChangeCulturePerWonder(int iChange)
{
	if (iChange != 0)
	{
		m_iCulturePerWonder += iChange;

		int iTotalCultureChange;

		// Loop through all Cities and change how much Culture they produce based on how many Wonders they have
		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			iTotalCultureChange = pLoopCity->getNumWorldWonders() * iChange;
			pLoopCity->ChangeJONSCulturePerTurnFromPolicies(iTotalCultureChange);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Culture multiplier for having a world wonder
int CvPlayer::GetCultureWonderMultiplier() const
{
	return m_iCultureWonderMultiplier;
}

//	--------------------------------------------------------------------------------
/// Changes amount of extra Culture per Wonder
void CvPlayer::ChangeCultureWonderMultiplier(int iChange)
{
	if (iChange != 0)
		m_iCultureWonderMultiplier += iChange;
}

//	--------------------------------------------------------------------------------
/// Amount of Culture provided for each Tech Researched
int CvPlayer::GetCulturePerTechResearched() const
{
	return m_iCulturePerTechResearched;
}

//	--------------------------------------------------------------------------------
/// Changes amount of Culture provided for each Tech Researched
void CvPlayer::ChangeCulturePerTechResearched(int iChange)
{
	if (iChange != 0)
	{
		m_iCulturePerTechResearched += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Specialist Culture Modifier
int CvPlayer::GetSpecialistCultureChange() const
{
	return m_iSpecialistCultureChange;
}

//	--------------------------------------------------------------------------------
/// Specialist Culture Modifier
void CvPlayer::ChangeSpecialistCultureChange(int iChange)
{
	if (iChange != 0)
	{
		CvCity* pLoopCity;
		int iLoop;

		int iTotalCulture = 0;

		SpecialistTypes eSpecialist;
		int iSpecialistLoop;
		int iSpecialistCount;

		// Undo old culture
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for (iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				eSpecialist = (SpecialistTypes) iSpecialistLoop;
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
				iTotalCulture += (iSpecialistCount * pLoopCity->GetCultureFromSpecialist(eSpecialist));
			}

			pLoopCity->ChangeJONSCulturePerTurnFromSpecialists(-iTotalCulture);
		}

		// CHANGE VALUE
		m_iSpecialistCultureChange += iChange;

		iTotalCulture = 0;

		// Apply new culture
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for (iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				eSpecialist = (SpecialistTypes) iSpecialistLoop;
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
				iTotalCulture += (iSpecialistCount * pLoopCity->GetCultureFromSpecialist(eSpecialist));
			}

			pLoopCity->ChangeJONSCulturePerTurnFromSpecialists(iTotalCulture);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Cities remaining to get a free culture building
int CvPlayer::GetNumCitiesFreeCultureBuilding() const
{
	return m_iNumCitiesFreeCultureBuilding;
}

//	--------------------------------------------------------------------------------
/// Changes number of cities remaining to get a free culture building
void CvPlayer::ChangeNumCitiesFreeCultureBuilding(int iChange)
{
	if (iChange != 0)
		m_iNumCitiesFreeCultureBuilding += iChange;
}

//	--------------------------------------------------------------------------------
/// Handle earning culture from combat wins
void CvPlayer::ReportCultureFromKills(int iX, int iY, int iCulture, bool bWasBarbarian)
{
	if (iCulture > 0)
	{
		int iCultureValue = GetPlayerTraits()->GetCultureFromKills();
		iCultureValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_KILLS);

		// Do we get it for barbarians?
		if (bWasBarbarian)
		{
			iCultureValue += GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS);
		}

		iCulture = (iCultureValue * iCulture) / 100;
		if (iCulture > 0)
		{
			changeJONSCulture(iCulture);

			if (GetID() == GC.getGame().getActivePlayer())
			{
				char text[256];
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 3;
				text[0] = NULL;
				sprintf( text, "[COLOR_MAGENTA]+%d[ENDCOLOR]", iCulture );
				GC.GetEngineUserInterface()->AddPopupText( iX, iY, text, fDelay );
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Handle earning culture from combat wins
void CvPlayer::ReportGoldFromKills(int iX, int iY, int iGold)
{
	if (iGold > 0)
	{
		int iGoldValue = GetPlayerPolicies()->GetNumericModifier(POLICYMOD_GOLD_FROM_KILLS);

		iGold = (iGoldValue * iGold) / 100;
		if (iGold > 0)
		{
			GetTreasury()->ChangeGold(iGold);

			if (GetID() == GC.getGame().getActivePlayer())
			{
				char text[256];
				float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 3;
				text[0] = NULL;
				sprintf( text, "[COLOR_YELLOW]+%d[ENDCOLOR]", iGold );
				GC.GetEngineUserInterface()->AddPopupText( iX, iY, text, fDelay );
			}
		}
	}
}


//	--------------------------------------------------------------------------------
/// Updates how much Happiness we have
void CvPlayer::DoUpdateHappiness()
{
	// Start level
	m_iHappiness = getHandicapInfo().getHappinessDefault();

	// Increase from Luxury Resources
	int iNumHappinessFromResources = GetHappinessFromResources();
	m_iHappiness += iNumHappinessFromResources;

	// Increase from buildings
	m_iHappiness += GetHappinessFromBuildings();

	// Increase from policies
	m_iHappiness += GetHappinessFromPolicies();

	// Increase from num cities (player based, for buildings and such)
	m_iHappiness += getNumCities() * m_iHappinessPerCity;

	// Increase from Religion
	m_iHappiness += GetHappinessFromReligion();

	// Increase from Natural Wonders
	m_iHappiness += GetHappinessFromNaturalWonders();

	// Per-City adjustments
	m_iHappinessFromGarrisonedUnits = 0;
	CvCity* pLoopCity;
	int iLoop;
	int iHappinessPerGarrison = GetHappinessPerGarrisonedUnit();
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		// Increase from Cities with Garrisons
		if (pLoopCity->GetGarrisonedUnit() != NULL)
		{
			ChangeHappinessFromGarrisonedUnits(iHappinessPerGarrison);
		}
	}
	m_iHappiness += m_iHappinessFromGarrisonedUnits;

	// Friendship with Minors can provide Happiness
	PlayerTypes eMinor;
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;
		m_iHappiness += GetHappinessFromMinor(eMinor);
	}

	// Increase for each City connected to Capital with a Trade Route
	DoUpdateCityConnectionHappiness();
	m_iHappiness += GetHappinessFromTradeRoutes();

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
/// How much Happiness we have
int CvPlayer::GetHappiness() const
{
	return m_iHappiness;
}

//	--------------------------------------------------------------------------------
/// Sets how much Happiness we have
void CvPlayer::SetHappiness(int iNewValue)
{
	if (GetHappiness() != iNewValue)
	{
		m_iHappiness = iNewValue;
	}
}

//	--------------------------------------------------------------------------------
/// How much over our Happiness limit are we?
int CvPlayer::GetExcessHappiness() const
{
	return GetHappiness() - GetUnhappiness();
}

//	--------------------------------------------------------------------------------
/// Has the player passed the Happiness limit?
bool CvPlayer::IsEmpireUnhappy() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if (GetExcessHappiness() < 0)
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Is the empire REALLY unhappy? (other penalties)
bool CvPlayer::IsEmpireVeryUnhappy() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if (GetExcessHappiness() <= /*-10*/ GC.getVERY_UNHAPPY_THRESHOLD())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Is the empire SUPER unhappy? (leads to revolts)
bool CvPlayer::IsEmpireSuperUnhappy() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if (GetExcessHappiness() <= /*-20*/ GC.getSUPER_UNHAPPY_THRESHOLD())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Revolts pop up if the empire is Super Unhappy
void CvPlayer::DoUpdateRevolts()
{
	if (IsEmpireSuperUnhappy())
	{
		// If we're super unhappy, make the counter wind down
		if (GetRevoltCounter() > 0)
		{
			ChangeRevoltCounter(-1);

			// Time's up!
			if (GetRevoltCounter() == 0)
			{
				DoRevolt();
				DoResetRevoltCounter(/*bFirstTime*/ false);
			}
		}
		// Super Unhappy for the first time - seed the counter
		else
		{
			DoResetRevoltCounter(/*bFirstTime*/ true);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Revolt countdown - get
int CvPlayer::GetRevoltCounter() const
{
	return m_iRevoltCounter;
}

//	--------------------------------------------------------------------------------
/// Revolt countdown - set
void CvPlayer::SetRevoltCounter(int iValue)
{
	m_iRevoltCounter = iValue;
}

//	--------------------------------------------------------------------------------
/// Revolt countdown - change
void CvPlayer::ChangeRevoltCounter(int iChange)
{
	SetRevoltCounter(GetRevoltCounter() + iChange);
}

//	--------------------------------------------------------------------------------
/// Revolt countdown - seed
void CvPlayer::DoResetRevoltCounter(bool bFirstTime)
{
	int iTurns = /*4*/ GC.getREVOLT_COUNTER_MIN();
	CvGame& theGame = GC.getGame();
	int iExtra = theGame.getJonRandNum(/*3*/ GC.getREVOLT_COUNTER_POSSIBLE(), "Revolt counter rand");
	iTurns += iExtra;

	// Game speed mod
	int iMod = theGame.getGameSpeedInfo().getTrainPercent();
	// Only LENGTHEN time between rebels
	if (iMod > 100)
	{
		iTurns *= iMod;
		iTurns /= 100;
	}

	if (bFirstTime)
		iTurns /= 2;

	if (iTurns <= 0)
		iTurns = 1;

	SetRevoltCounter(iTurns);
}

//	--------------------------------------------------------------------------------
// Fire off a revolt somewhere
void CvPlayer::DoRevolt()
{
	// In hundreds
	int iNumRebels = /*100*/ GC.getREVOLT_NUM_BASE();
	int iExtraRoll = (getNumCities() - 1) * /*20*/ GC.getREVOLT_NUM_CITY_COUNT();
	iExtraRoll += 100;
	iNumRebels += GC.getGame().getJonRandNum(iExtraRoll, "Rebel count rand roll");
	iNumRebels /= 100;

	// Find a random city to pop up a bad man
	CvCity* pBestCity = NULL;
	int iBestWeight = 0;

	int iTempWeight;

	CvCity* pLoopCity;
	int iLoop;
	CvGame& theGame = GC.getGame();
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTempWeight = pLoopCity->getPopulation();
		iTempWeight += theGame.getJonRandNum(10, "Revolt rand weight.");

		if (iTempWeight > iBestWeight)
		{
			iBestWeight = iTempWeight;
			pBestCity = pLoopCity;
		}
	}

	// Found a place to set up a revolt?
	if (pBestCity != NULL)
	{
		int iBestPlot = -1;
		int iBestPlotWeight = -1;
		CvPlot* pPlot;

		CvCityCitizens* pCitizens = pBestCity->GetCityCitizens();

		// Start at 1, since ID 0 is the city plot itself
		for (int iPlotLoop = 1; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
		{
			pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

			if (!pPlot)		// Should be valid, but make sure
				continue;

			// Can't be impassable
			if (pPlot->isImpassable())
				continue;

			// Can't be water
			if (pPlot->isWater())
				continue;

			// Can't be ANOTHER city
			if (pPlot->isCity())
				continue;

			// Don't place on a plot where a unit is already standing
			if (pPlot->getNumUnits() > 0)
				continue;

			iTempWeight = theGame.getJonRandNum(10, "Revolt rand plot location.");

			// Add weight if there's an improvement here!
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iTempWeight += 4;

				// If also a a resource, even more weight!
				if (pPlot->getResourceType(getTeam()) != NO_RESOURCE)
					iTempWeight += 3;
			}

			// Add weight if there's a defensive bonus for this plot
			if (pPlot->defenseModifier(BARBARIAN_TEAM, false, false))
				iTempWeight += 4;

			// Don't pick plots that aren't ours
			if (pPlot->getOwner() != GetID())
				iTempWeight = -1;

			if (iTempWeight > iBestPlotWeight)
			{
				iBestPlotWeight = iTempWeight;
				iBestPlot = iPlotLoop;
			}
		}

		// Found valid plot
		if (iBestPlot != -1)
		{
			// Make barbs able to enter ANYONE'S territory
			theGame.SetBarbarianReleaseTurn(0);

			pPlot = pCitizens->GetCityPlotFromIndex(iBestPlot);

			// Pick a unit type
			UnitTypes eUnit = theGame.GetRandomSpawnUnitType(GetID(), /*bIncludeUUs*/ false, /*bIncludeRanged*/ false);

			CvNotifications* pNotifications = GetNotifications();
			if (pNotifications)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS");
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS_SUMMARY");
				pNotifications->Add(NOTIFICATION_REBELS, strMessage.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eUnit, BARBARIAN_PLAYER);
			}

			// Init unit
			GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
			iNumRebels--;	// Reduce the count since we just added the seed rebel

			// Loop until all rebels are placed
			do
			{
				iNumRebels--;

				// Init unit
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY());
				CvAssert(pUnit);
				if (pUnit)
				{
					if (!pUnit->jumpToNearestValidPlotWithinRange(5))
						pUnit->kill(false);
				}
			}
			while (iNumRebels > 0);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Returns the amount of Happiness being added by Policies
int CvPlayer::GetHappinessFromPolicies() const
{
	int iHappiness = m_pPlayerPolicies->GetNumericModifier (POLICYMOD_EXTRA_HAPPINESS);
	iHappiness += (getNumCities() * m_pPlayerPolicies->GetNumericModifier (POLICYMOD_EXTRA_HAPPINESS_PER_CITY));

	int iHappinessPerXPopulation;
	iHappinessPerXPopulation = GetHappinessPerXPopulation();

	if (iHappinessPerXPopulation > 0)
	{
		const CvCity *pLoopCity;
		int iLoop;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity && !pLoopCity->IsPuppet())
			{
				int iExtraHappiness = pLoopCity->getPopulation() / iHappinessPerXPopulation;

				iHappiness += iExtraHappiness;
			}
		}
	}

	return iHappiness;
}

//	--------------------------------------------------------------------------------
/// Returns the amount of Happiness being added by Buildings
int CvPlayer::GetHappinessFromBuildings() const
{
	int iHappiness;

	// Policy Building Mods
	int iSpecialPolicyBuildingHappiness = 0;
	int iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for (int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if (GetPlayerPolicies()->HasPolicy(ePolicy) && !GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
			{
				for (iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if (!pkBuildingClassInfo)
					{
						continue;
					}

					if (getBuildingClassCount(eBuildingClass) > 0)
					{
						if (pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
						{
							iSpecialPolicyBuildingHappiness += (getBuildingClassCount(eBuildingClass) * pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass));
						}
					}
				}
			}
		}
	}
	iHappiness = iSpecialPolicyBuildingHappiness;


	const CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iHappiness += pLoopCity->GetHappinessFromBuildings();
	}

	// Increase from num policies -- MOVE THIS CODE (and provide a new tool tip string) if we ever get happiness per X buildings to something beside a building
	if (m_iHappinessPerXPolicies > 0)
	{
		iHappiness += GetPlayerPolicies()->GetNumPoliciesOwned() / m_iHappinessPerXPolicies;
	}

	return iHappiness;
}

//	--------------------------------------------------------------------------------
/// Changes amount of Happiness being added by Buildings
void CvPlayer::changeHappinessFromBuildings(int change)
{
	// NOTE: THIS IS NOT USED ANY MORE!!!

	CvAssertMsg (m_iHappinessFromBuildings >= 0, "Count of buildings helping Happiness is corrupted");
	m_iHappinessFromBuildings += change;
}

//	--------------------------------------------------------------------------------
/// Returns the amount of extra Happiness per City
int CvPlayer::GetExtraHappinessPerCity() const
{
	return m_iHappinessPerCity;
}

//	--------------------------------------------------------------------------------
/// Changes amount of extra Happiness per City
void CvPlayer::ChangeExtraHappinessPerCity(int iChange)
{
	CvAssertMsg (m_iHappinessPerCity >= 0, "Count of buildings helping Happiness is corrupted");

	if (iChange != 0)
		m_iHappinessPerCity += iChange;
}

//	--------------------------------------------------------------------------------
/// Returns the amount of extra Happiness per City
int CvPlayer::GetExtraHappinessPerXPolicies() const
{
	return m_iHappinessPerXPolicies;
}

//	--------------------------------------------------------------------------------
/// Changes amount of extra Happiness per City
void CvPlayer::ChangeExtraHappinessPerXPolicies(int iChange)
{
	CvAssertMsg (m_iHappinessPerXPolicies >= 0, "Count of extra happiness per buildings is corrupted");

	if (iChange != 0)
		m_iHappinessPerXPolicies += iChange;
}

//	--------------------------------------------------------------------------------
/// Total amount of Happiness gained from Resources
int CvPlayer::GetHappinessFromResources() const
{
	int iTotalHappiness = 0;

	int iBaseHappiness;

	// Check all connected Resources
	ResourceTypes eResource;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if (getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				// Add base Happiness bonus for Resource
				iBaseHappiness = pkResourceInfo->getHappiness();

				// Resource bonus from Minors, and this is a Luxury we're getting from one (Policies, etc.)
				if (IsMinorResourceBonus() && getResourceFromMinors(eResource) > 0)
				{
					iBaseHappiness *= /*150*/ GC.getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER();
					iBaseHappiness /= 100;
				}

				iTotalHappiness += iBaseHappiness;
				iTotalHappiness += GetExtraHappinessPerLuxury();
			}
		}
	}

	// Happiness bonus for multiple Resource types
	iTotalHappiness += GetHappinessFromResourceVariety();

	return iTotalHappiness;
}

//	--------------------------------------------------------------------------------
/// Amount of Happiness from having a variety of Luxuries
int CvPlayer::GetHappinessFromResourceVariety() const
{
	int iHappiness = 0;

	int iMultipleLuxuriesBonus = /*1*/ GC.getHAPPINESS_PER_EXTRA_LUXURY();

	// Check all connected Resources
	int iNumHappinessResources = 0;

	ResourceTypes eResource;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if (getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumHappinessResources++;
			}
		}
	}

	if (iNumHappinessResources > 1)
	{
		iHappiness += (--iNumHappinessResources * iMultipleLuxuriesBonus);
	}

	return iHappiness;
}


//	--------------------------------------------------------------------------------
/// Total amount of Happiness gained from Religion
int CvPlayer::GetHappinessFromReligion()
{
	int iHappinessFromReligion = 0;
	int iHappinessPerCity = /*1*/ GC.getHAPPINESS_PER_CITY_WITH_STATE_RELIGION();

	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->GetPlayersReligion() == GetID())
		{
			iHappinessFromReligion += iHappinessPerCity;
		}
	}

	return iHappinessFromReligion;
}

//	--------------------------------------------------------------------------------
// Happiness from finding Natural Wonders
int CvPlayer::GetHappinessFromNaturalWonders() const
{
	int iNumWonders = GET_TEAM(getTeam()).GetNumNaturalWondersDiscovered();

	int iHappiness = iNumWonders * /*1*/ GC.getHAPPINESS_PER_NATURAL_WONDER();

	// Trait boosts this further?
	if (m_pTraits->GetNaturalWonderHappinessModifier() > 0)
	{
		iHappiness *= (100 + m_pTraits->GetNaturalWonderHappinessModifier());
		iHappiness /= 100;
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot *pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if (pPlot == NULL)
		{
			continue;
		}

		if (pPlot->getOwner() != m_eID)
		{
			continue;
		}

		FeatureTypes eFeature = pPlot->getFeatureType();
		if (eFeature == NO_FEATURE)
		{
			continue;
		}

		int iPlotHappiness = GC.getFeatureInfo(eFeature)->getInBorderHappiness();

		if (iPlotHappiness > 0)
		{
			// Trait boosts this further?
			if (m_pTraits->GetNaturalWonderYieldModifier() > 0)
			{
				iPlotHappiness *= (100 + m_pTraits->GetNaturalWonderYieldModifier());
				iPlotHappiness /= 100;
			}

			iHappiness += iPlotHappiness;
		}
	}

	return iHappiness;
}

//	--------------------------------------------------------------------------------
/// Extra Happiness from every connected Luxury
int CvPlayer::GetExtraHappinessPerLuxury() const
{
	return m_iExtraHappinessPerLuxury;
}

//	--------------------------------------------------------------------------------
/// Change Extra Happiness from every connected Luxury
void CvPlayer::ChangeExtraHappinessPerLuxury(int iChange)
{
	m_iExtraHappinessPerLuxury += iChange;
}

//	--------------------------------------------------------------------------------
/// How much Unhappiness are Units producing?
int CvPlayer::GetUnhappinessFromUnits() const
{
	int iUnhappinessFromUnits = m_iUnhappinessFromUnits;

	int iFreeUnitUnhappiness = /*0*/ GC.getFREE_UNIT_HAPPINESS();
	if (iFreeUnitUnhappiness != 0)
	{
		iUnhappinessFromUnits -= iFreeUnitUnhappiness;
	}

	// Can't be less than 0
	if (iUnhappinessFromUnits < 0)
	{
		iUnhappinessFromUnits = 0;
	}

	if (GetUnhappinessFromUnitsMod() != 0)
	{
		iUnhappinessFromUnits *= (100 + GetUnhappinessFromUnitsMod());
		iUnhappinessFromUnits /= 100;
	}

	return iUnhappinessFromUnits;
}

//	--------------------------------------------------------------------------------
/// Changes how much Happiness Units produce
void CvPlayer::ChangeUnhappinessFromUnits(int iChange)
{
	m_iUnhappinessFromUnits += iChange;
}

//	--------------------------------------------------------------------------------
/// How much of our Happiness is being used up? (Population + Units)
int CvPlayer::GetUnhappiness(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;

	// City Count Unhappiness
	iUnhappiness += GetUnhappinessFromCityCount(pAssumeCityAnnexed, pAssumeCityPuppeted);

	// Occupied City Count Unhappiness
	iUnhappiness += GetUnhappinessFromCapturedCityCount(pAssumeCityAnnexed, pAssumeCityPuppeted);

	// City Population Unhappiness
	iUnhappiness += GetUnhappinessFromCityPopulation(pAssumeCityAnnexed, pAssumeCityPuppeted);

	// Occupied City Population Unhappiness
	iUnhappiness += GetUnhappinessFromOccupiedCities(pAssumeCityAnnexed, pAssumeCityPuppeted);

	// Unit Unhappiness (Builders)
	iUnhappiness += GetUnhappinessFromUnits();

	iUnhappiness /= 100;

	// AI gets reduced Unhappiness on higher levels
	if (!isHuman() && !IsAITeammateOfHuman())
	{
		iUnhappiness *= GC.getGame().getHandicapInfo().getAIUnhappinessPercent();
		iUnhappiness /= 100;
	}

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Used for providing info to the player
int CvPlayer::GetUnhappinessFromCityForUI(CvCity* pCity) const
{
	int iNumCitiesUnhappinessTimes100 = 0;
	int iPopulationUnhappinessTimes100 = 0;

	int iPopulation = pCity->getPopulation() * 100;

	// No Unhappiness from Specialist Pop? (Policies, etc.)
	if (isHalfSpecialistUnhappiness())
	{
		int iSpecialistCount = pCity->GetCityCitizens()->GetTotalSpecialistCount() * 100;
		iPopulation -= (iSpecialistCount / 2);
	}

	// Occupied?
	if (pCity->IsOccupied() && !pCity->IsIgnoreCityForHappiness())
	{
		iNumCitiesUnhappinessTimes100 += (100 * /*5*/ GC.getUNHAPPINESS_PER_CAPTURED_CITY());
		iPopulationUnhappinessTimes100 += int(iPopulation * /*1.34f*/ GC.getUNHAPPINESS_PER_OCCUPIED_POPULATION());

		// Mod (Policies, etc.)
		if (GetOccupiedPopulationUnhappinessMod() != 0)
		{
			iPopulationUnhappinessTimes100 *= (100 + GetOccupiedPopulationUnhappinessMod());
			iPopulationUnhappinessTimes100 /= 100;
		}
	}
	// Normal City
	else
	{
		iNumCitiesUnhappinessTimes100 += (100 * /*2*/ GC.getUNHAPPINESS_PER_CITY());
		iPopulationUnhappinessTimes100 += (iPopulation * /*1*/ GC.getUNHAPPINESS_PER_POPULATION());

		if (pCity->isCapital() && GetCapitalUnhappinessMod() != 0)
		{
			iPopulationUnhappinessTimes100 *= (100 + GetCapitalUnhappinessMod());
			iPopulationUnhappinessTimes100 /= 100;
		}

		iPopulationUnhappinessTimes100 *= (100 + GetUnhappinessMod());
		iPopulationUnhappinessTimes100 /= 100;

		iPopulationUnhappinessTimes100 *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
		iPopulationUnhappinessTimes100 /= 100;
	}

	// Population Handicap mod
	iPopulationUnhappinessTimes100 *= getHandicapInfo().getPopulationUnhappinessMod();
	iPopulationUnhappinessTimes100 /= 100;

	// City Count Player mod
	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iNumCitiesUnhappinessTimes100 *= (100 + iMod);
	iNumCitiesUnhappinessTimes100 /= 100;

	// City Count Handicap mod
	iNumCitiesUnhappinessTimes100 *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iNumCitiesUnhappinessTimes100 /= 100;

	// City Count Map size mod
	iNumCitiesUnhappinessTimes100 *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iNumCitiesUnhappinessTimes100 /= 100;

	return iNumCitiesUnhappinessTimes100 + iPopulationUnhappinessTimes100;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from number of Cities
int CvPlayer::GetUnhappinessFromCityCount(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessPerCity = /*2*/ GC.getUNHAPPINESS_PER_CITY() * 100;

	bool bCityValid;

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;

		// Assume city is puppeted, and counts
		if (pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;
		// Assume city is annexed, and does NOT count
		else if (pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;
		// Assume city doesn't exist, and does NOT count
		else if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;
		// Normal city
		else if (!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if (bCityValid)
			iUnhappiness += iUnhappinessPerCity;
	}

	// Player count mod
	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iUnhappiness *= (100 + iMod);
	iUnhappiness /= 100;

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iUnhappiness /= 100;

	// Map size mod
	iUnhappiness *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from number of Captured Cities
int CvPlayer::GetUnhappinessFromCapturedCityCount(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessPerCapturedCity = /*5*/ GC.getUNHAPPINESS_PER_CAPTURED_CITY() * 100;

	bool bCityValid;

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;

		// Assume city is puppeted, and does NOT count
		if (pLoopCity == pAssumeCityPuppeted)
			bCityValid = false;
		// Assume city is annexed, and counts
		else if (pLoopCity == pAssumeCityAnnexed)
			bCityValid = true;
		// Assume city doesn't exist, and does NOT count
		else if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;
		// Occupied city
		else if (pLoopCity->IsOccupied() && !pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		// Extra Unhappiness from Occupied Cities
		if (bCityValid)
			iUnhappiness += iUnhappinessPerCapturedCity;
	}

	// Player count mod
	int iMod = 0;
	iMod += GetCityCountUnhappinessMod();
	iMod += GetPlayerTraits()->GetCityUnhappinessModifier();

	iUnhappiness *= (100 + iMod);
	iUnhappiness /= 100;

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getNumCitiesUnhappinessMod();
	iUnhappiness /= 100;

	// Map size mod
	iUnhappiness *= GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from City Population
int CvPlayer::GetUnhappinessFromCityPopulation(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	int iUnhappinessPerPop = /*1*/ GC.getUNHAPPINESS_PER_POPULATION() * 100;
	int iPopulation;
	int iSpecialistCount;

	bool bCityValid;

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;

		// Assume pLoopCity is Annexed, and does NOT count
		if (pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;
		// Assume that pLoopCity is a Puppet and IS counted here
		else if (pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;
		// Assume city doesn't exist, and does NOT count
		else if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;
		// Occupied Cities don't get counted here (see the next function)
		else if (!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if (bCityValid)
		{
			iPopulation = pLoopCity->getPopulation();

			// No Unhappiness from Specialist Pop? (Policies, etc.)
			if (isHalfSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++; // Round up
				iPopulation -= (iSpecialistCount / 2);
			}

			iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if (pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from Puppet City Population
int CvPlayer::GetUnhappinessFromPuppetCityPopulation() const
{
	int iUnhappiness = 0;
	int iUnhappinessPerPop = GC.getUNHAPPINESS_PER_POPULATION() * 100;

	int iLoop = 0;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bool bCityValid = false;

		if(pLoopCity->IsPuppet())
			bCityValid = true;

		// Assume city doesn't exist, and does NOT count
		if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;

		if (bCityValid)
		{
			int iPopulation = pLoopCity->getPopulation();

			// No Unhappiness from Specialist Pop? (Policies, etc.)
			if (isHalfSpecialistUnhappiness())
			{
				int iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++; // Round up
				iPopulation -= (iSpecialistCount / 2);
			}

			int iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if (pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetUnhappinessFromCitySpecialists(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	int iUnhappinessPerPop = /*1*/ GC.getUNHAPPINESS_PER_POPULATION() * 100;
	int iPopulation;

	bool bCityValid;

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;

		// Assume pLoopCity is Annexed, and does NOT count
		if (pLoopCity == pAssumeCityAnnexed)
			bCityValid = false;
		// Assume that pLoopCity is a Puppet and IS counted here
		else if (pLoopCity == pAssumeCityPuppeted)
			bCityValid = true;
		// Assume city doesn't exist, and does NOT count
		else if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;
		// Occupied Cities don't get counted here (see the next function)
		else if (!pLoopCity->IsOccupied() || pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if (bCityValid)
		{
			iPopulation = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();

			// No Unhappiness from Specialist Pop? (Policies, etc.)
			if (isHalfSpecialistUnhappiness())
			{
				iPopulation++; // Round up
				iPopulation /= 2;
			}

			iUnhappinessFromThisCity = iPopulation * iUnhappinessPerPop;

			if (pLoopCity->isCapital() && GetCapitalUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetCapitalUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	iUnhappiness *= (100 + GetUnhappinessMod());
	iUnhappiness /= 100;

	iUnhappiness *= 100 + GetPlayerTraits()->GetPopulationUnhappinessModifier();
	iUnhappiness /= 100;

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from City Population in Occupied Cities
int CvPlayer::GetUnhappinessFromOccupiedCities(CvCity* pAssumeCityAnnexed, CvCity* pAssumeCityPuppeted) const
{
	int iUnhappiness = 0;
	int iUnhappinessFromThisCity;

	double fUnhappinessPerPop = /*1.34f*/ GC.getUNHAPPINESS_PER_OCCUPIED_POPULATION() * 100;
	int iPopulation;
	int iSpecialistCount;

	bool bCityValid;

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		bCityValid = false;

		// Assume pLoopCity is Annexed, and counts
		if (pLoopCity == pAssumeCityAnnexed)
			bCityValid = true;
		// Assume that pLoopCity is a Puppet and does NOT count
		else if (pLoopCity == pAssumeCityPuppeted)
			bCityValid = false;
		// Assume city doesn't exist, and does NOT count
		else if (pLoopCity->IsIgnoreCityForHappiness())
			bCityValid = false;
		// Occupied Cities
		else if (pLoopCity->IsOccupied() && !pLoopCity->IsNoOccupiedUnhappiness())
			bCityValid = true;

		if (bCityValid)
		{
			iPopulation = pLoopCity->getPopulation();

			// No Unhappiness from Specialist Pop? (Policies, etc.)
			if (isHalfSpecialistUnhappiness())
			{
				iSpecialistCount = pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
				iSpecialistCount++; // Round up
				iPopulation -= (iSpecialistCount / 2);
			}

			iUnhappinessFromThisCity = int(double(iPopulation) * fUnhappinessPerPop);

			// Mod (Policies, etc.)
			if (GetOccupiedPopulationUnhappinessMod() != 0)
			{
				iUnhappinessFromThisCity *= (100 + GetOccupiedPopulationUnhappinessMod());
				iUnhappinessFromThisCity /= 100;
			}

			iUnhappiness += iUnhappinessFromThisCity;
		}
	}

	// Handicap mod
	iUnhappiness *= getHandicapInfo().getPopulationUnhappinessMod();
	iUnhappiness /= 100;

	return iUnhappiness;
}

//	--------------------------------------------------------------------------------
/// Unhappiness from Units Percent (50 = 50% of normal)
int CvPlayer::GetUnhappinessFromUnitsMod() const
{
	return m_iUnhappinessFromUnitsMod;
}

//	--------------------------------------------------------------------------------
/// Change Unhappiness from Units Percent (50 = 50% of normal)
void CvPlayer::ChangeUnhappinessFromUnitsMod(int iChange)
{
	if (iChange != 0)
	{
		m_iUnhappinessFromUnitsMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Unhappiness Mod (-50 = 50% of normal)
int CvPlayer::GetUnhappinessMod() const
{
	return m_iUnhappinessMod;
}

//	--------------------------------------------------------------------------------
/// Change Unhappiness Mod (-50 = 50% of normal)
void CvPlayer::ChangeUnhappinessMod(int iChange)
{
	if (iChange != 0)
	{
		m_iUnhappinessMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// City Count Unhappiness Mod (-50 = 50% of normal)
int CvPlayer::GetCityCountUnhappinessMod() const
{
	return m_iCityCountUnhappinessMod;
}

//	--------------------------------------------------------------------------------
/// Change City Count Unhappiness Mod (-50 = 50% of normal)
void CvPlayer::ChangeCityCountUnhappinessMod(int iChange)
{
	if (iChange != 0)
	{
		m_iCityCountUnhappinessMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Occupied Population Unhappiness Mod (-50 = 50% of normal)
int CvPlayer::GetOccupiedPopulationUnhappinessMod() const
{
	return m_iOccupiedPopulationUnhappinessMod;
}

//	--------------------------------------------------------------------------------
/// Occupied Population Count Unhappiness Mod (-50 = 50% of normal)
void CvPlayer::ChangeOccupiedPopulationUnhappinessMod(int iChange)
{
	if (iChange != 0)
	{
		m_iOccupiedPopulationUnhappinessMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Unhappiness from Capital Mod (-50 = 50% of normal)
int CvPlayer::GetCapitalUnhappinessMod() const
{
	return m_iCapitalUnhappinessMod;
}

//	--------------------------------------------------------------------------------
/// Change Unhappiness from Capital Mod (-50 = 50% of normal)
void CvPlayer::ChangeCapitalUnhappinessMod(int iChange)
{
	if (iChange != 0)
	{
		m_iCapitalUnhappinessMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// How much Happiness coming from Garrisoned Units?
int CvPlayer::GetHappinessFromGarrisonedUnits() const
{
	return m_iHappinessFromGarrisonedUnits;
}

//	--------------------------------------------------------------------------------
/// Change how much Happiness is coming from Garrisoned Units
void CvPlayer::ChangeHappinessFromGarrisonedUnits(int iChange)
{
	m_iHappinessFromGarrisonedUnits += iChange;
}

//	--------------------------------------------------------------------------------
/// How much Happiness are we getting from Units being Garrisoned in Cities?
int CvPlayer::GetHappinessPerGarrisonedUnit() const
{
	return m_iHappinessPerGarrisonedUnitCount;
}

//	--------------------------------------------------------------------------------
/// Set the amont of Happiness we're getting from Units being Garrisoned in Cities
void CvPlayer::SetHappinessPerGarrisonedUnit(int iValue)
{
	m_iHappinessPerGarrisonedUnitCount = iValue;
}

//	--------------------------------------------------------------------------------
/// Change the amont of Happiness we're getting from Units being Garrisoned in Cities
void CvPlayer::ChangeHappinessPerGarrisonedUnit(int iChange)
{
	SetHappinessPerGarrisonedUnit(m_iHappinessPerGarrisonedUnitCount + iChange);
}

/// Returns cached amount of Happiness being brought in for having Cities connected via a Route
int CvPlayer::GetHappinessFromTradeRoutes() const
{
	return m_iCityConnectionHappiness;
}

//	--------------------------------------------------------------------------------
/// How much Happiness coming from Trade Routes?
void CvPlayer::DoUpdateCityConnectionHappiness()
{
	int iHappinessPerTradeRoute = GetHappinessPerTradeRoute();

	int iNumCities = 0;
	if (iHappinessPerTradeRoute != 0)
	{
		CvCity* pCapitalCity = getCapitalCity();

		// Must have a capital before we can check if other Cities are connected to it!
		if (pCapitalCity != NULL && getNumCities() > 1)
		{
			CvCity* pLoopCity;

			int iLoop;
			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity != pCapitalCity && pLoopCity != NULL)
				{
					if (GetTreasury()->HasRouteBetweenCities(pCapitalCity, pLoopCity))
					{
						iNumCities++;
					}
				}
			}
		}
	}
	m_iCityConnectionHappiness = iHappinessPerTradeRoute * iNumCities / 100;	// Bring it out of hundreds
}

//	--------------------------------------------------------------------------------
/// How muchHappiness are we getting from Trade Routes?
int CvPlayer::GetHappinessPerTradeRoute() const
{
	return m_iHappinessPerTradeRouteCount;
}

//	--------------------------------------------------------------------------------
/// Set the amont of Happiness we're getting from Trade Routes
void CvPlayer::SetHappinessPerTradeRoute(int iValue)
{
	m_iHappinessPerTradeRouteCount = iValue;
}

//	--------------------------------------------------------------------------------
/// Change the amont of Happiness we're getting from Trade Routes
void CvPlayer::ChangeHappinessPerTradeRoute(int iChange)
{
	SetHappinessPerTradeRoute(m_iHappinessPerTradeRouteCount + iChange);
}

//	--------------------------------------------------------------------------------
/// How much Happiness are we getting from large cities?
int CvPlayer::GetHappinessPerXPopulation() const
{
	return m_iHappinessPerXPopulation;
}

//	--------------------------------------------------------------------------------
/// Set the amount of Happiness we're getting from large cities
void CvPlayer::SetHappinessPerXPopulation(int iValue)
{
	m_iHappinessPerXPopulation = iValue;
}

//	--------------------------------------------------------------------------------
/// Change the amount of Happiness we're getting from large cities
void CvPlayer::ChangeHappinessPerXPopulation(int iChange)
{
	SetHappinessPerXPopulation(m_iHappinessPerXPopulation + iChange);
}

//	--------------------------------------------------------------------------------
/// Happiness from Minors
int CvPlayer::GetHappinessFromMinor(PlayerTypes eMinor) const
{
	int iHappiness = 0;

	if (GET_PLAYER(eMinor).isAlive())
	{
		iHappiness += GET_PLAYER(eMinor).GetMinorCivAI()->GetHappinessFriendshipBonus(GetID());
	}

	return iHappiness;
}

//	--------------------------------------------------------------------------------
/// How much weaker do Units get when wounded?
int CvPlayer::GetWoundedUnitDamageMod() const
{
	return m_iWoundedUnitDamageMod;
}

//	--------------------------------------------------------------------------------
/// How much weaker do Units get when wounded?
void CvPlayer::SetWoundedUnitDamageMod(int iValue)
{
	m_iWoundedUnitDamageMod = iValue;

	if (m_iWoundedUnitDamageMod < /*50*/ -GC.getWOUNDED_DAMAGE_MULTIPLIER())
	{
		m_iWoundedUnitDamageMod = /*50*/ -GC.getWOUNDED_DAMAGE_MULTIPLIER();
	}
}

//	--------------------------------------------------------------------------------
/// How much weaker do Units get when wounded?
void CvPlayer::ChangeWoundedUnitDamageMod(int iChange)
{
	SetWoundedUnitDamageMod(m_iWoundedUnitDamageMod + iChange);
}

//	--------------------------------------------------------------------------------
/// Unit upgrade cost mod
int CvPlayer::GetUnitUpgradeCostMod() const
{
	return m_iUnitUpgradeCostMod;
}

//	--------------------------------------------------------------------------------
/// Unit upgrade cost mod
void CvPlayer::SetUnitUpgradeCostMod(int iValue)
{
	m_iUnitUpgradeCostMod = iValue;

	if (m_iUnitUpgradeCostMod < /*-75*/ GC.getUNIT_UPGRADE_COST_DISCOUNT_MAX())
		m_iUnitUpgradeCostMod = /*-75*/ GC.getUNIT_UPGRADE_COST_DISCOUNT_MAX();
}

//	--------------------------------------------------------------------------------
/// Unit upgrade cost mod
void CvPlayer::ChangeUnitUpgradeCostMod(int iChange)
{
	SetUnitUpgradeCostMod(m_iUnitUpgradeCostMod + iChange);
}

//	--------------------------------------------------------------------------------
/// How much of a combat bonus do we get VS Barbarian Units?
int CvPlayer::GetBarbarianCombatBonus() const
{
	return m_iBarbarianCombatBonus;
}

//	--------------------------------------------------------------------------------
/// Sets how much of a combat bonus we get VS Barbarian Units
void CvPlayer::SetBarbarianCombatBonus(int iValue)
{
	m_iBarbarianCombatBonus = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes how much of a combat bonus we get VS Barbarian Units
void CvPlayer::ChangeBarbarianCombatBonus(int iChange)
{
	SetBarbarianCombatBonus(m_iBarbarianCombatBonus + iChange);
}

//	--------------------------------------------------------------------------------
/// Do we always see where Barb Camps appear?
bool CvPlayer::IsAlwaysSeeBarbCamps() const
{
	return m_iAlwaysSeeBarbCampsCount > 0;
}

//	--------------------------------------------------------------------------------
/// Sets if we always see where Barb Camps appear
void CvPlayer::SetAlwaysSeeBarbCampsCount(int iValue)
{
	m_iAlwaysSeeBarbCampsCount = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes if we always see where Barb Camps appear
void CvPlayer::ChangeAlwaysSeeBarbCampsCount(int iChange)
{
	SetAlwaysSeeBarbCampsCount(m_iAlwaysSeeBarbCampsCount + iChange);
}

//	--------------------------------------------------------------------------------
CvPlayerTechs *CvPlayer::GetPlayerTechs() const
{
	return m_pPlayerTechs;
}

//	--------------------------------------------------------------------------------
CvPlayerPolicies *CvPlayer::GetPlayerPolicies() const
{
	return m_pPlayerPolicies;
}

//	--------------------------------------------------------------------------------
CvPlayerTraits *CvPlayer::GetPlayerTraits() const
{
	return m_pTraits;
}

//	--------------------------------------------------------------------------------
CvFlavorManager *CvPlayer::GetFlavorManager() const
{
	return m_pFlavorManager;
}

//	--------------------------------------------------------------------------------
CvTacticalAI *CvPlayer::GetTacticalAI() const
{
	return m_pTacticalAI;
}

//	--------------------------------------------------------------------------------
CvHomelandAI *CvPlayer::GetHomelandAI() const
{
	return m_pHomelandAI;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setHasPolicy(PolicyTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPolicyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (m_pPlayerPolicies->HasPolicy(eIndex) != bNewValue)
	{
		m_pPlayerPolicies->SetPolicy(eIndex, bNewValue);
		processPolicies(eIndex, bNewValue ? 1 : -1);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNextPolicyCost() const
{
	return m_iCostNextPolicy;
}

//	--------------------------------------------------------------------------------
void CvPlayer::DoUpdateNextPolicyCost()
{
	m_iCostNextPolicy = GetPlayerPolicies()->GetNextPolicyCost();
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canAdoptPolicy(PolicyTypes eIndex) const
{
	return GetPlayerPolicies()->CanAdoptPolicy (eIndex);
}

//	--------------------------------------------------------------------------------
void CvPlayer::doAdoptPolicy(PolicyTypes ePolicy)
{
	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
	CvAssert(pkPolicyInfo != NULL);
	if(pkPolicyInfo == NULL)
		return;

	// Can we actually adopt this?
	if (!canAdoptPolicy(ePolicy))
		return;

	// Pay Culture cost - if applicable
	if (GetNumFreePolicies() == 0)
	{
		changeJONSCulture(-getNextPolicyCost());
	}
	else
	{
		ChangeNumFreePolicies(-1);
	}

	setHasPolicy(ePolicy, true);

	// Update cost if trying to buy another policy this turn
	DoUpdateNextPolicyCost();

	// Branch unlocked
	PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes) pkPolicyInfo->GetPolicyBranchType();
	GetPlayerPolicies()->SetPolicyBranchUnlocked(ePolicyBranch, true);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	// This Dirty bit must only be set when changing something for the active player
	if (GC.getGame().getActivePlayer() == GetID())
	{
		GC.GetEngineUserInterface()->setDirty(Policies_DIRTY_BIT, true);
	}


	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(ePolicy);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicy", args.get(), bResult);
	}

	updateYield();		// Policies can change the yield
}

//	--------------------------------------------------------------------------------
/// Empire in Anarchy?
bool CvPlayer::IsAnarchy() const
{
	return GetAnarchyNumTurns() > 0;
}

//	--------------------------------------------------------------------------------
/// Empire in Anarchy?
int CvPlayer::GetAnarchyNumTurns() const
{
	return m_iAnarchyNumTurns;
}

//	--------------------------------------------------------------------------------
/// Empire in Anarchy?
void CvPlayer::SetAnarchyNumTurns(int iValue)
{
	if (iValue != GetAnarchyNumTurns())
	{
		m_iAnarchyNumTurns = iValue;

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Empire in Anarchy?
void CvPlayer::ChangeAnarchyNumTurns(int iChange)
{
	SetAnarchyNumTurns(GetAnarchyNumTurns() + iChange);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}


//	--------------------------------------------------------------------------------
// Get Attack Bonus for a certain period of time
int CvPlayer::GetAttackBonusTurns() const
{
	return m_iAttackBonusTurns;
}

//	--------------------------------------------------------------------------------
// Set Attack Bonus for a certain period of time
void CvPlayer::ChangeAttackBonusTurns(int iChange)
{
	if (iChange != 0)
	{
		m_iAttackBonusTurns += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Update all Golden-Age related stuff
void CvPlayer::DoProcessGoldenAge()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return;
	}

	// Minors and Barbs can't get GAs
	if (!isMinorCiv() && !isBarbarian())
	{
		// Already in a GA - don't decrement counter while in Anarchy
		if (getGoldenAgeTurns() > 0)
		{
			if (!IsAnarchy())
			{
				changeGoldenAgeTurns(-1);
			}
		}

		// Not in GA
		else
		{
			// Note: This will actually REDUCE the GA meter if the player is running in the red
			ChangeGoldenAgeProgressMeter(GetExcessHappiness());

			// Enough GA Progress to trigger new GA?
			if (GetGoldenAgeProgressMeter() >= GetGoldenAgeProgressThreshold())
			{
				int iOverflow = GetGoldenAgeProgressMeter() - GetGoldenAgeProgressThreshold();

				SetGoldenAgeProgressMeter(iOverflow);
				
				int iLength = getGoldenAgeLength();
				changeGoldenAgeTurns(iLength);

				// If it's the active player then show the popup
				if (GetID() == GC.getGame().getActivePlayer())
				{
					// Don't show in MP
					if (!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)
					{
						CvPopupInfo kPopupInfo(BUTTONPOPUP_GOLDEN_AGE_REWARD);
						GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// How much do we need in the GA meter to trigger the next one?
int CvPlayer::GetGoldenAgeProgressThreshold() const
{
	int iThreshold = /*500*/ GC.getGOLDEN_AGE_BASE_THRESHOLD_HAPPINESS();
	iThreshold += GetNumGoldenAges() * /*500*/ GC.getGOLDEN_AGE_EACH_GA_ADDITIONAL_HAPPINESS();

	// Increase cost based on the # of cities in the empire
	int iCostExtra = int(iThreshold * (getNumCities() - 1) * /*0.02*/ GC.getGOLDEN_AGE_THRESHOLD_CITY_MULTIPLIER());
	iThreshold += iCostExtra;

	if (GetGoldenAgeMeterMod() != 0)
	{
		iThreshold *= (100 + GetGoldenAgeMeterMod());
		iThreshold /= 100;
	}

	// Game Speed Mod
	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Make the number nice to look at
	int iVisibleDivisor = /*5*/ GC.getGOLDEN_AGE_VISIBLE_THRESHOLD_DIVISOR();
	iThreshold /= iVisibleDivisor;
	iThreshold *= iVisibleDivisor;

	return iThreshold;
}

//	--------------------------------------------------------------------------------
/// What is our progress towards the next GA?
int CvPlayer::GetGoldenAgeProgressMeter() const
{
	return m_iGoldenAgeProgressMeter;
}

//	--------------------------------------------------------------------------------
/// Sets what is our progress towards the next GA
void CvPlayer::SetGoldenAgeProgressMeter(int iValue)
{
	m_iGoldenAgeProgressMeter = iValue;

	if (m_iGoldenAgeProgressMeter < 0)
	{
		m_iGoldenAgeProgressMeter = 0;
	}
}

//	--------------------------------------------------------------------------------
/// Changes what is our progress towards the next GA
void CvPlayer::ChangeGoldenAgeProgressMeter(int iChange)
{
	SetGoldenAgeProgressMeter(GetGoldenAgeProgressMeter() + iChange);
}

//	--------------------------------------------------------------------------------
/// Modifier for how big the GA meter is (-50 = 50% of normal)
int CvPlayer::GetGoldenAgeMeterMod() const
{
	return m_iGoldenAgeMeterMod;
}

//	--------------------------------------------------------------------------------
/// Sets Modifier for how big the GA meter is (-50 = 50% of normal)
void CvPlayer::SetGoldenAgeMeterMod(int iValue)
{
	m_iGoldenAgeMeterMod = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes Modifier for how big the GA meter is (-50 = 50% of normal)
void CvPlayer::ChangeGoldenAgeMeterMod(int iChange)
{
	SetGoldenAgeMeterMod(GetGoldenAgeMeterMod() + iChange);
}

//	--------------------------------------------------------------------------------
/// How many GAs have we had in this game?
int CvPlayer::GetNumGoldenAges() const
{
	return m_iNumGoldenAges;
}

//	--------------------------------------------------------------------------------
/// Sets how many GAs have we had in this game
void CvPlayer::SetNumGoldenAges(int iValue)
{
	m_iNumGoldenAges = iValue;

	if(iValue > 0 && isHuman() && !GC.getGame().isGameMultiPlayer()&& GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
	{
		gDLL->UnlockAchievement( ACHIEVEMENT_GOLDEN_AGE );

		const char* strLeader = getLeaderTypeKey();
		if(m_iNumGoldenAges >=5 && NULL != strLeader && strcmp(strLeader, "LEADER_DARIUS") == 0)
		{
			gDLL->UnlockAchievement( ACHIEVEMENT_SPECIAL_ARCHAEMENNID );
		}
	}
}

//	--------------------------------------------------------------------------------
/// Changes how many GAs have we had in this game
void CvPlayer::ChangeNumGoldenAges(int iChange)
{
	SetNumGoldenAges(GetNumGoldenAges() + iChange);
}

//	--------------------------------------------------------------------------------
/// How many turns left in GA? (0 if not in GA)
int CvPlayer::getGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isGoldenAge() const
{
	return (getGoldenAgeTurns() > 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGoldenAgeTurns(int iChange)
{
	Localization::String locString;
	Localization::String locSummaryString;

	bool bOldGoldenAge;

	if (iChange != 0)
	{
		bOldGoldenAge = isGoldenAge();

		m_iGoldenAgeTurns = (m_iGoldenAgeTurns + iChange);
		CvAssert(getGoldenAgeTurns() >= 0);

		if (bOldGoldenAge != isGoldenAge())
		{
			GC.getMap().updateYield();	// Do the entire map, so that any potential golden age bonus is reflected in the yield icons.

			if (isGoldenAge())
			{
				ChangeNumGoldenAges(1);

				locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_BEGUN");
				locString << getCivilizationAdjectiveKey();
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), locString.toUTF8(), -1, -1);

				gDLL->GameplayGoldenAgeStarted();
			}
			else
			{
				gDLL->GameplayGoldenAgeEnded();
			}

			CvNotifications* pNotifications = GetNotifications();
			if (pNotifications)
			{
				NotificationTypes eNotification = NO_NOTIFICATION_TYPE;

				if (isGoldenAge())
				{
					eNotification = NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER;
					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER");
					locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER");
				}
				else
				{
					eNotification = NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER;
					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER");
					locSummaryString = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GOLDEN_AGE_ENDED_ACTIVE_PLAYER");
				}

				pNotifications->Add(eNotification, locString.toUTF8(), locSummaryString.toUTF8(), -1, -1, -1);
			}
		}

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGoldenAgeLength() const
{
	int iTurns = GC.getGame().goldenAgeLength();

	// Player modifier
	int iLengthModifier = getGoldenAgeModifier();

	// Trait modifier
	iLengthModifier += GetPlayerTraits()->GetGoldenAgeDurationModifier();

	if (iLengthModifier > 0)
	{
		iTurns = iTurns * (100 + iLengthModifier) / 100;
	}

	return iTurns;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumUnitGoldenAges() const
{
	return m_iNumUnitGoldenAges;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeNumUnitGoldenAges(int iChange)
{
	m_iNumUnitGoldenAges = (m_iNumUnitGoldenAges + iChange);
	CvAssert(getNumUnitGoldenAges() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getStrikeTurns() const
{
	return m_iStrikeTurns;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeStrikeTurns(int iChange)
{
	m_iStrikeTurns = (m_iStrikeTurns + iChange);
	CvAssert(getStrikeTurns() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeGoldenAgeModifier(int iChange)
{
	m_iGoldenAgeModifier += iChange;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatPeopleCreated() const
{
	return m_iGreatPeopleCreated;
}

//	--------------------------------------------------------------------------------
void CvPlayer::incrementGreatPeopleCreated()
{
	m_iGreatPeopleCreated++;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}

//	--------------------------------------------------------------------------------
void CvPlayer::incrementGreatGeneralsCreated()
{
	m_iGreatGeneralsCreated++;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatPeopleThresholdModifier() const
{
	return m_iGreatPeopleThresholdModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeGreatPeopleThresholdModifier(int iChange)
{
	m_iGreatPeopleThresholdModifier = (m_iGreatPeopleThresholdModifier + iChange);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getPolicyCostModifier() const
{
	return m_iPolicyCostModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::recomputePolicyCostModifier()
{
	int iCost = m_pPlayerPolicies->GetNumericModifier(POLICYMOD_POLICY_COST_MODIFIER);
	iCost += GetPolicyCostBuildingModifier();
	iCost += GetPolicyCostMinorCivModifier();
	iCost += GetPlayerTraits()->GetPolicyCostModifier();

	if (iCost < /*-75*/ GC.getPOLICY_COST_DISCOUNT_MAX())
		iCost = /*-75*/ GC.getPOLICY_COST_DISCOUNT_MAX();

	m_iPolicyCostModifier = iCost;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeGreatPeopleRateModFromBldgs(int ichange)
{
	m_iGreatPeopleRateModFromBldgs += ichange;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeGreatGeneralRateModFromBldgs(int ichange)
{
	m_iGreatGeneralRateModFromBldgs += ichange;
}

//	--------------------------------------------------------------------------------
/// Do effects when a unit is killed in combat
void CvPlayer::DoUnitKilledCombat(PlayerTypes eKilledPlayer, UnitTypes eUnitType)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eKilledPlayer);
		args->Push(eUnitType);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "UnitKilledInCombat", args.get(), bResult);
	}
}

//	--------------------------------------------------------------------------------
/// Do effects when a GP is consumed
void CvPlayer::DoGreatPersonExpended(UnitTypes eGreatPersonUnit)
{
	// Gold gained
	int iExpendGold = GetGreatPersonExpendGold();
	if (iExpendGold > 0)
	{
		GetTreasury()->ChangeGold(iExpendGold);

		if (isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
		{
			// Update Steam stat and check achievement
			const int HALICARNASSUS_ACHIEVEMENT_GOLD = 1000;
			int iHalicarnassus = GC.getInfoTypeForString("BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS");
			// Does player have DLC_06, and if so, do they have the Mausoleum of Halicarnassus?
			if (iHalicarnassus != -1 && getBuildingClassCount((BuildingClassTypes)iHalicarnassus) >= 1)
			{
				BuildingTypes eHalicarnassus = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MAUSOLEUM_HALICARNASSUS");
				CvBuildingEntry* pHalicarnassusInfo = GC.getBuildingInfo(eHalicarnassus);
				int iHalicarnassusGold = pHalicarnassusInfo->GetGreatPersonExpendGold();

				int32 iTotalHalicarnassusGold = 0;
				if (gDLL->GetSteamStat(ESTEAMSTAT_HALICARNASSUSGOLDEARNED, &iTotalHalicarnassusGold))
				{
					iTotalHalicarnassusGold += iHalicarnassusGold;
					gDLL->SetSteamStat(ESTEAMSTAT_HALICARNASSUSGOLDEARNED, iTotalHalicarnassusGold);
					if (iTotalHalicarnassusGold >= HALICARNASSUS_ACHIEVEMENT_GOLD)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_HALICARNASSUS_GOLD);
					}
				}
			}
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());
		args->Push(eGreatPersonUnit);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GreatPersonExpended", args.get(), bResult);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetGreatPersonExpendGold() const
{
	return m_iGreatPersonExpendGold;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeGreatPersonExpendGold(int ichange)
{
	m_iGreatPersonExpendGold += ichange;
}

//	--------------------------------------------------------------------------------
void CvPlayer::recomputeGreatPeopleModifiers()
{
	//=============
	// Initialize
	//=============
	m_iGreatPeopleRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;

	// Get from traits first
	m_iGreatPeopleRateModifier += m_pTraits->GetGreatPeopleRateModifier();
	m_iGreatGeneralRateModifier += m_pTraits->GetGreatGeneralRateModifier();

	// Then get from current policies
	m_iGreatPeopleRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_PERSON_RATE);
	m_iGreatGeneralRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_GREAT_GENERAL_RATE);
	m_iDomesticGreatGeneralRateModifier += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_DOMESTIC_GREAT_GENERAL_RATE);

	// Next add in buildings
	m_iGreatPeopleRateModifier += m_iGreatPeopleRateModFromBldgs;
	m_iGreatGeneralRateModifier += m_iGreatGeneralRateModFromBldgs;
	m_iDomesticGreatGeneralRateModifier += m_iDomesticGreatGeneralRateModFromBldgs;

	// Finally boost domestic general from combat experience
	m_iDomesticGreatGeneralRateModifier += GC.getCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT();
}



//////////////////////////////////////////////////////////////////////////
// ***** Great People Spawning *****
//////////////////////////////////////////////////////////////////////////


//	--------------------------------------------------------------------------------
// Figures out how long before we spawn a free Great Person for ePlayer
void CvPlayer::DoSeedGreatPeopleSpawnCounter()
{
	int iNumTurns = /*37*/ GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE();

	// Start at -1 since if we only have one ally we don't want to add any more
	int iExtraAllies = -1;

	PlayerTypes eMinor;
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;

		// Not alive
		if (!GET_PLAYER(eMinor).isAlive())
			continue;

		if (GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() == GetID())
			iExtraAllies++;
	}

	if (iExtraAllies > 0)
	{
		int iExtraAlliesChange = iExtraAllies * /*-1*/ GC.getMINOR_ADDITIONAL_ALLIES_GP_CHANGE();

		iExtraAlliesChange = max(/*-10*/ GC.getMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE(), iExtraAlliesChange);

		iNumTurns += iExtraAlliesChange;
	}

	int iRand = /*7*/ GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "Rand turns for Friendly Minor GreatPeople spawn");

	// If we're biasing the result then decrease the number of turns
	if (!IsAlliesGreatPersonBiasApplied())
	{
		iNumTurns *= /*50*/ GC.getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY();
		iNumTurns /= 100;

		SetAlliesGreatPersonBiasApplied(true);
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	if (iNumTurns < 1)
		iNumTurns = 1;

	SetGreatPeopleSpawnCounter(iNumTurns);
}

//	--------------------------------------------------------------------------------
/// We're now allies with someone, what happens with the GP bonus?
void CvPlayer::DoApplyNewAllyGPBonus()
{
	int iChange = /*-2*/ GC.getMINOR_ADDITIONAL_ALLIES_GP_CHANGE();
	ChangeGreatPeopleSpawnCounter(iChange);

	if (GetGreatPeopleSpawnCounter() < 1)
		SetGreatPeopleSpawnCounter(1);
}

//	--------------------------------------------------------------------------------
// How long before we spawn a free GreatPeople for ePlayer?
int CvPlayer::GetGreatPeopleSpawnCounter()
{
	return m_iGreatPeopleSpawnCounter;
}

//	--------------------------------------------------------------------------------
// Sets how long before we spawn a free GreatPeople for ePlayer
void CvPlayer::SetGreatPeopleSpawnCounter(int iValue)
{
	m_iGreatPeopleSpawnCounter = iValue;
}

//	--------------------------------------------------------------------------------
// Changes how long before we spawn a free GreatPeople for ePlayer
void CvPlayer::ChangeGreatPeopleSpawnCounter(int iChange)
{
	SetGreatPeopleSpawnCounter(GetGreatPeopleSpawnCounter() + iChange);
}

//	--------------------------------------------------------------------------------
/// Create a GreatPeople
void CvPlayer::DoSpawnGreatPerson(PlayerTypes eMinor)
{
	CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "eMinor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "eMinor is expected to be within maximum bounds (invalid Index)");

	CvCity* pCapital = GET_PLAYER(eMinor).getCapitalCity();

	// Minor must have Capital
	if (pCapital == NULL)
	{
		FAssertMsg(false, "MINOR CIV AI: Trying to spawn a GreatPeople for a major civ but the minor has no capital. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		return;
	}

	int iX = pCapital->getX();
	int iY = pCapital->getY();

	// Pick Great Person type
	UnitTypes eBestUnit = NO_UNIT;
	int iBestScore = -1;
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		UnitTypes eLoopUnit = (UnitTypes)iUnitLoop;
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eLoopUnit);


		if (pkUnitEntry && pkUnitEntry->GetSpecialUnitType() == eSpecialUnitGreatPerson)
		{
			int iScore = GC.getGame().getJonRandNum(100, "Rand");

			if (iScore > iBestScore)
			{
				iBestScore = iScore;
				eBestUnit = eLoopUnit;
			}
		}
	}

	// Spawn GreatPeople
	if (eBestUnit != NO_UNIT)
	{
		CvUnit* pNewGreatPeople = initUnit(eBestUnit, iX, iY);
		CvAssert(pNewGreatPeople != NULL);
		if (pNewGreatPeople)
		{
			if (!pNewGreatPeople->jumpToNearestValidPlot())
				pNewGreatPeople->kill(false);
			else
			{
				CvNotifications* pNotifications = GetNotifications();
				if (pNotifications)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
					strMessage << GET_PLAYER(eMinor).getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
					strSummary << GET_PLAYER(eMinor).getNameKey();
					pNotifications->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, eMinor);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Time to spawn a GreatPeople?
void CvPlayer::DoGreatPeopleSpawnTurn()
{
	// Tick down
	if (GetGreatPeopleSpawnCounter() > 0)
	{
		ChangeGreatPeopleSpawnCounter(-1);

		// Time to spawn! - Pick a random allied minor
		if (GetGreatPeopleSpawnCounter() == 0)
		{
			PlayerTypes eBestMinor = NO_PLAYER;
			int iBestScore = -1;
			int iScore;

			PlayerTypes eMinor;
			for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				eMinor = (PlayerTypes) iMinorLoop;

				// Not alive
				if (!GET_PLAYER(eMinor).isAlive())
					continue;

				// Not an ally
				if (GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() != GetID())
					continue;

				iScore = GC.getGame().getJonRandNum(100, "Random minor great person gift location.");

				// Best ally yet?
				if (eBestMinor == NO_PLAYER || iScore > iBestScore)
				{
					eBestMinor = eMinor;
					iBestScore = iScore;
				}
			}

			if (eBestMinor != NO_PLAYER)
				DoSpawnGreatPerson(eBestMinor);

			// Reseed counter
			DoSeedGreatPeopleSpawnCounter();
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMaxGlobalBuildingProductionModifier(int iChange)
{
	m_iMaxGlobalBuildingProductionModifier = (m_iMaxGlobalBuildingProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMaxTeamBuildingProductionModifier(int iChange)
{
	m_iMaxTeamBuildingProductionModifier = (m_iMaxTeamBuildingProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMaxPlayerBuildingProductionModifier(int iChange)
{
	m_iMaxPlayerBuildingProductionModifier = (m_iMaxPlayerBuildingProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getFreeExperience() const
{
	return m_iFreeExperience;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeFreeExperienceFromBldgs(int iChange)
{
	m_iFreeExperienceFromBldgs += iChange;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeFreeExperienceFromMinors(int iChange)
{
	m_iFreeExperienceFromMinors += iChange;
}

//	--------------------------------------------------------------------------------
void CvPlayer::recomputeFreeExperience()
{
	m_iFreeExperience = m_iFreeExperienceFromBldgs;
	m_iFreeExperience = m_iFreeExperienceFromMinors;
	m_iFreeExperience += m_pPlayerPolicies->GetNumericModifier(POLICYMOD_FREE_EXPERIENCE);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeFeatureProductionModifier(int iChange)
{
	m_iFeatureProductionModifier = (m_iFeatureProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier = (m_iWorkerSpeedModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getImprovementCostModifier() const
{
	return m_iImprovementCostModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeImprovementCostModifier(int iChange)
{
	m_iImprovementCostModifier = (m_iImprovementCostModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier = (m_iImprovementUpgradeRateModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getSpecialistProductionModifier() const
{
	return m_iSpecialistProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeSpecialistProductionModifier(int iChange)
{
	m_iSpecialistProductionModifier = (m_iSpecialistProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeCityDefenseModifier(int iChange)
{
	m_iCityDefenseModifier = (m_iCityDefenseModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getWonderProductionModifier() const
{
	return m_iWonderProductionModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeWonderProductionModifier(int iChange)
{
	m_iWonderProductionModifier = (m_iWonderProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getSettlerProductionModifier() const
{
	return m_iSettlerProductionModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeSettlerProductionModifier(int iChange)
{
	m_iSettlerProductionModifier = (m_iSettlerProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getCapitalSettlerProductionModifier() const
{
	return m_iCapitalSettlerProductionModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeCapitalSettlerProductionModifier(int iChange)
{
	m_iCapitalSettlerProductionModifier = (m_iCapitalSettlerProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::GetPolicyCostBuildingModifier() const
{
	return m_iPolicyCostBuildingModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangePolicyCostBuildingModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iPolicyCostBuildingModifier = (m_iPolicyCostBuildingModifier + iChange);

		recomputePolicyCostModifier();
		DoUpdateNextPolicyCost();
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetPolicyCostMinorCivModifier() const
{
	return m_iPolicyCostMinorCivModifier;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangePolicyCostMinorCivModifier(int iChange)
{
	m_iPolicyCostMinorCivModifier = (m_iPolicyCostMinorCivModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getNumNukeUnits() const
{
	return m_iNumNukeUnits;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeNumNukeUnits(int iChange)
{
	m_iNumNukeUnits = (m_iNumNukeUnits + iChange);
	CvAssert(getNumNukeUnits() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getNumOutsideUnits()
{
	int iOutsideUnitCount = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->plot() != NULL)
		{
			if (pLoopUnit->plot()->getOwner() != pLoopUnit->getOwner())
			{
				iOutsideUnitCount++;
			}
		}
	}

	return iOutsideUnitCount;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeNumOutsideUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumOutsideUnits += iChange;
		CvAssert(getNumOutsideUnits() >= 0);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeBaseFreeUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iBaseFreeUnits = (m_iBaseFreeUnits + iChange);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetVerifiedOutsideUnitCount()
{
	int iOutsideUnitCount = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->plot() != NULL)
		{
			if (pLoopUnit->plot()->getOwner() != pLoopUnit->getOwner())
			{
				iOutsideUnitCount++;
			}
		}
	}

	return iOutsideUnitCount;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getGoldPerUnit() const
{
	return getGoldPerUnitTimes100() / 100;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGoldPerUnit(int iChange)
{
	if (iChange != 0)
	{
		changeGoldPerUnitTimes100(iChange * 100);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGoldPerUnitTimes100() const
{
	return m_iGoldPerUnit;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGoldPerUnitTimes100(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldPerUnit = (m_iGoldPerUnit + iChange);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGoldPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldPerMilitaryUnit = (m_iGoldPerMilitaryUnit + iChange);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetRouteGoldMaintenanceMod() const
{
	return m_iRouteGoldMaintenanceMod;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeRouteGoldMaintenanceMod(int iChange)
{
	if (iChange != 0)
	{
		m_iRouteGoldMaintenanceMod = (m_iRouteGoldMaintenanceMod + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetBuildingGoldMaintenanceMod() const
{
	return m_iBuildingGoldMaintenanceMod;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeBuildingGoldMaintenanceMod(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoldMaintenanceMod = (m_iBuildingGoldMaintenanceMod + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetUnitGoldMaintenanceMod() const
{
	return m_iUnitGoldMaintenanceMod;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeUnitGoldMaintenanceMod(int iChange)
{
	if (iChange != 0)
	{
		m_iUnitGoldMaintenanceMod = (m_iUnitGoldMaintenanceMod + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetUnitSupplyMod() const
{
	return m_iUnitSupplyMod;
}


//	--------------------------------------------------------------------------------
void CvPlayer::ChangeUnitSupplyMod(int iChange)
{
	if (iChange != 0)
	{
		m_iUnitSupplyMod = (m_iUnitSupplyMod + iChange);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getExtraUnitCost() const
{
	return m_iExtraUnitCost;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeExtraUnitCost(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraUnitCost = (m_iExtraUnitCost + iChange);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNumMaintenanceFreeUnits() const
{
	int iNumFreeUnits = 0;

	// Loop through all units to see if any of them are free!
	const CvUnit* pLoopUnit;
	int iLoop;
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getUnitInfo().IsNoMaintenance())
		{
			iNumFreeUnits++;
		}
		else if (IsGarrisonFreeMaintenance() && pLoopUnit->IsGarrisoned())
		{
			iNumFreeUnits++;
		}
	}

	return iNumFreeUnits;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getNumMilitaryUnits() const
{
	return m_iNumMilitaryUnits;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeNumMilitaryUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumMilitaryUnits = (m_iNumMilitaryUnits + iChange);
		CvAssert(getNumMilitaryUnits() >= 0);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeHappyPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iHappyPerMilitaryUnit = (m_iHappyPerMilitaryUnit + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getHappinessToCulture() const
{
	return m_iHappinessToCulture;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeHappinessToCulture(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessToCulture = (m_iHappinessToCulture + iChange);
		CvAssert(getHappinessToCulture() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getHappinessToScience() const
{
	return m_iHappinessToScience;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeHappinessToScience(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessToScience = (m_iHappinessToScience + iChange);
		CvAssert(getHappinessToScience() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getHalfSpecialistUnhappinessCount() const
{
	return m_iHalfSpecialistUnhappinessCount;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isHalfSpecialistUnhappiness() const
{
	return (getHalfSpecialistUnhappinessCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeHalfSpecialistUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iHalfSpecialistUnhappinessCount = (m_iHalfSpecialistUnhappinessCount + iChange);
		CvAssert(getHalfSpecialistUnhappinessCount() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getHalfSpecialistFoodCount() const
{
	return m_iHalfSpecialistFoodCount;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isHalfSpecialistFood() const
{
	return (getHalfSpecialistFoodCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeHalfSpecialistFoodCount(int iChange)
{
	if (iChange != 0)
	{
		m_iHalfSpecialistFoodCount = (m_iHalfSpecialistFoodCount + iChange);
		CvAssert(getHalfSpecialistFoodCount() >= 0);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getMilitaryFoodProductionCount() const
{
	return m_iMilitaryFoodProductionCount;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isMilitaryFoodProduction() const
{
	return (getMilitaryFoodProductionCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMilitaryFoodProductionCount(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryFoodProductionCount = (m_iMilitaryFoodProductionCount + iChange);
		CvAssert(getMilitaryFoodProductionCount() >= 0);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getHighestUnitLevel()	const
{
	return m_iHighestUnitLevel;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	CvAssert(getHighestUnitLevel() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getMaxConscript() const
{
	return m_iMaxConscript;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMaxConscript(int iChange)
{
	m_iMaxConscript = (m_iMaxConscript + iChange);
	CvAssert(getMaxConscript() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getConscriptCount() const
{
	return m_iConscriptCount;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setConscriptCount(int iNewValue)
{
	m_iConscriptCount = iNewValue;
	CvAssert(getConscriptCount() >= 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeConscriptCount(int iChange)
{
	setConscriptCount(getConscriptCount() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getOverflowResearch() const
{
	return m_iOverflowResearch / 100;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setOverflowResearch(int iNewValue)
{
	setOverflowResearchTimes100(iNewValue*100);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeOverflowResearch(int iChange)
{
	changeOverflowResearchTimes100(iChange*100);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getOverflowResearchTimes100() const
{
	return m_iOverflowResearch;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setOverflowResearchTimes100(int iNewValue)
{
	m_iOverflowResearch = iNewValue;
	CvAssert(getOverflowResearchTimes100() >= 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeOverflowResearchTimes100(int iChange)
{
	setOverflowResearchTimes100(getOverflowResearchTimes100() + iChange);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getExpModifier() const
{
	return m_iExpModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeExpModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpModifier += iChange;
		CvAssert(getExpModifier() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
		CvAssert(getExpInBorderModifier() >= 0);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getMinorQuestFriendshipMod() const
{
	return m_iMinorQuestFriendshipMod;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMinorQuestFriendshipMod(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorQuestFriendshipMod += iChange;
		CvAssert(getMinorQuestFriendshipMod() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getMinorGoldFriendshipMod() const
{
	return m_iMinorGoldFriendshipMod;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeMinorGoldFriendshipMod(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorGoldFriendshipMod += iChange;
		CvAssert(getMinorGoldFriendshipMod() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMinorFriendshipMinimum() const
{
	return m_iMinorFriendshipMinimum;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGetMinorFriendshipMinimum(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorFriendshipMinimum += iChange;
		CvAssert(GetMinorFriendshipMinimum() >= -100);

		// Check all Minors that have EVER been alive to see if we need to change any values
		PlayerTypes eMinor;
		TeamTypes eMinorTeam;

		int iMinimumFriendship = GetMinorFriendshipMinimum();
		for (int iI = MAX_MAJOR_CIVS; iI < MAX_CIV_PLAYERS; iI++)
		{
			eMinor = (PlayerTypes) iI;

			if (GET_PLAYER(eMinor).isEverAlive())
			{
				eMinorTeam = GET_PLAYER(eMinor).getTeam();

				// Not at war with this Minor
				if (!GET_TEAM(getTeam()).isAtWar(eMinorTeam))
				{
					// Below minimum?
					if (GET_PLAYER(eMinor).GetMinorCivAI()->GetFriendshipWithMajor(GetID()) < iMinimumFriendship)
					{
						GET_PLAYER(eMinor).GetMinorCivAI()->SetFriendshipWithMajor(GetID(), iMinimumFriendship);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMinorFriendshipDecayMod() const
{
	return m_iMinorFriendshipDecayMod;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeGetMinorFriendshipDecayMod(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorFriendshipDecayMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsMinorScienceAllies() const
{
	return GetMinorScienceAlliesCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMinorScienceAlliesCount() const
{
	return m_iMinorScienceAlliesCount;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeMinorScienceAlliesCount(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorScienceAlliesCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsMinorResourceBonus() const
{
	return GetMinorResourceBonusCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMinorResourceBonusCount() const
{
	return m_iMinorResourceBonusCount;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeMinorResourceBonusCount(int iChange)
{
	if (iChange != 0)
	{
		m_iMinorResourceBonusCount += iChange;
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getCultureBombTimer() const
{
	return m_iCultureBombTimer;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setCultureBombTimer(int iNewValue)
{
	if (getCultureBombTimer() != iNewValue)
	{
		m_iCultureBombTimer = iNewValue;
		CvAssert(getCultureBombTimer() >= 0);
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeCultureBombTimer(int iChange)
{
	setCultureBombTimer(getCultureBombTimer() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getConversionTimer() const
{
	return m_iConversionTimer;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setConversionTimer(int iNewValue)
{
	if (getConversionTimer() != iNewValue)
	{
		m_iConversionTimer = iNewValue;
		CvAssert(getConversionTimer() >= 0);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeConversionTimer(int iChange)
{
	setConversionTimer(getConversionTimer() + iChange);
}

//	--------------------------------------------------------------------------------
CvCity* CvPlayer::getCapitalCity()
{
	return getCity(m_iCapitalCityID);
}

//	--------------------------------------------------------------------------------
const CvCity* CvPlayer::getCapitalCity() const
{
	return getCity(m_iCapitalCityID);
}

//	--------------------------------------------------------------------------------
void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	CvCity* pOldCapitalCity;

	pOldCapitalCity = getCapitalCity();

	if (pOldCapitalCity != pNewCapitalCity)
	{
		if (pNewCapitalCity != NULL)
		{
			// Need to set our original capital x,y?
			if (GetOriginalCapitalX() == -1 || GetOriginalCapitalY() == -1)
			{
				m_iOriginalCapitalX = pNewCapitalCity->getX();
				m_iOriginalCapitalY = pNewCapitalCity->getY();
			}

			m_iCapitalCityID = pNewCapitalCity->GetID();

			pNewCapitalCity->SetEverCapital(true);
		}
		else
		{
			m_iCapitalCityID = FFreeList::INVALID_INDEX;
		}
	}
}

//	--------------------------------------------------------------------------------
/// Where was our original capital located?
int CvPlayer::GetOriginalCapitalX() const
{
	return m_iOriginalCapitalX;
}

//	--------------------------------------------------------------------------------
/// Where was our original capital located?
int CvPlayer::GetOriginalCapitalY() const
{
	return m_iOriginalCapitalY;
}

//	--------------------------------------------------------------------------------
/// Have we lost our capital in war?
bool CvPlayer::IsHasLostCapital() const
{
	return m_bLostCapital;
}

//	--------------------------------------------------------------------------------
/// Sets us to having lost our capital in war
void CvPlayer::SetHasLostCapital(bool bValue, PlayerTypes eConqueror)
{
	if(bValue != m_bLostCapital)
	{
		m_bLostCapital = bValue;
		m_eConqueror = eConqueror;

		// Don't really care if a City State lost its capital
		if(!isMinorCiv())
		{
			// Someone just lost their capital, test to see if someone wins
			if(bValue)
			{
				GC.getGame().DoTestConquestVictory();

				int iNumPlayersWithCapitals = 0;

				PlayerTypes eLoopPlayer;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;
					if(GET_PLAYER(eLoopPlayer).isAlive())
					{
						if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
						{
							if(!GET_PLAYER(eLoopPlayer).IsHasLostCapital())
							{
								iNumPlayersWithCapitals++;
							}
						}
					}
				}

				if(iNumPlayersWithCapitals > 1)
				{
					Localization::String localizedBuffer;
					Localization::String localizedSummary;
					NotificationTypes eNotificationType = NOTIFICATION_CAPITAL_LOST;

					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						PlayerTypes ePlayer = (PlayerTypes)ui;
						CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
						if(!pNotifications)
						{
							continue;
						}

						// Active Player lost their capital
						if(ePlayer == GetID())
						{
							eNotificationType = NOTIFICATION_CAPITAL_LOST_ACTIVE_PLAYER;
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_LOST_CAPITAL");
							localizedBuffer << iNumPlayersWithCapitals;
							localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_YOU_LOST_CAPITAL");
						}
						// Unmet player
						else if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(getTeam()))
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_SOMEONE_LOST_CAPITAL");
							localizedBuffer << iNumPlayersWithCapitals;
							localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_SOMEONE_LOST_CAPITAL");
						}
						// Player we've met
						else
						{
							localizedBuffer = Localization::Lookup("TXT_KEY_NOTIFICATION_PLAYER_LOST_CAPITAL");
							localizedBuffer << getCivilizationShortDescriptionKey() << iNumPlayersWithCapitals;
							localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_LOST_CAPITAL");
							localizedSummary << getCivilizationShortDescriptionKey();
						}

						pNotifications->Add(NOTIFICATION_CAPITAL_LOST, localizedBuffer.toUTF8(), localizedSummary.toUTF8(), -1, -1, -1);
					}

					//replay message
					{
						Localization::String message = Localization::Lookup("TXT_KEY_NOTIFICATION_PLAYER_LOST_CAPITAL");
						message << getCivilizationShortDescriptionKey() << iNumPlayersWithCapitals;
						CvString translatedMessage = message.toUTF8();
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), translatedMessage, GetOriginalCapitalX(), GetOriginalCapitalY());
					}

				}
			}
			// Player recovered capital!
			else
			{
				for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes ePlayer = (PlayerTypes)ui;
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(!pNotifications)
					{
						continue;
					}

					CvString strBuffer;
					CvString strSummary;

					// Active Player lost their capital
					if(ePlayer == GetID())
					{
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_YOU_RECOVERED_CAPITAL");
						strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_YOU_RECOVERED_CAPITAL");
					}
					// Unmet player
					else if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(getTeam()))
					{
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SOMEONE_RECOVERED_CAPITAL");
						strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SOMEONE_RECOVERED_CAPITAL");
					}
					// Player we've met
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_PLAYER_RECOVERED_CAPITAL", getCivilizationShortDescriptionKey());
						strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_RECOVERED_CAPITAL", getCivilizationShortDescriptionKey());
					}

					pNotifications->Add(NOTIFICATION_CAPITAL_RECOVERED, strBuffer, strSummary, -1, -1, -1);
				}

				//replay message
				{
					Localization::String message = Localization::Lookup("TXT_KEY_NOTIFICATION_PLAYER_RECOVERED_CAPITAL");
					message << getCivilizationShortDescriptionKey();
					CvString translatedMessage = message.toUTF8();
					CvCity* pCity = getCapitalCity();
					GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), translatedMessage, pCity->getX(), pCity->getY());
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Player who first captured our capital
PlayerTypes CvPlayer::GetCapitalConqueror() const
{
	return m_eConqueror;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost = (m_iCitiesLost + iChange);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getPower() const
{
	if (m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{
		// more lazy evaluation
		const_cast<CvPlayer *>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer *>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer *>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iMilitaryMight + m_iEconomicMight;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMilitaryMight() const
{
	if (m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{
		// more lazy evaluation
		const_cast<CvPlayer *>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer *>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer *>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iMilitaryMight;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetEconomicMight() const
{
	if (m_iTurnMightRecomputed < GC.getGame().getElapsedGameTurns())
	{
		// more lazy evaluation
		const_cast<CvPlayer *>(this)->m_iTurnMightRecomputed = GC.getGame().getElapsedGameTurns();
		const_cast<CvPlayer *>(this)->m_iMilitaryMight = calculateMilitaryMight();
		const_cast<CvPlayer *>(this)->m_iEconomicMight = calculateEconomicMight();
	}
	return m_iEconomicMight;
}

//	--------------------------------------------------------------------------------
int CvPlayer::calculateMilitaryMight() const
{
	int rtnValue = 0;
	const CvUnit *pLoopUnit;
	int iLoop;

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// Current combat strength or bombard strength, whichever is higher
		rtnValue += pLoopUnit->GetPower();
	}

	//Simplistic increase based on player's gold
	//500 gold will increase might by 22%, 2000 by 45%, 8000 gold by 90%
	float fGoldMultiplier = 1.0f + (sqrt((float)GetTreasury()->GetGold()) / 100.0f);
	if (fGoldMultiplier > 2.0f) fGoldMultiplier = 2.0f;

	rtnValue = (int)(rtnValue * fGoldMultiplier);

	return rtnValue;
}


//	--------------------------------------------------------------------------------
int CvPlayer::calculateEconomicMight() const
{
	// Default to 5 so that a fluctuation in Population early doesn't swing things wildly
	int iEconomicMight = 5;

	iEconomicMight += getTotalPopulation();

	return iEconomicMight;
}


//	--------------------------------------------------------------------------------
int CvPlayer::getPopScore() const
{
	return (m_iPopulationScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}

//	--------------------------------------------------------------------------------
void CvPlayer::changePopScore(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationScore += iChange;
		CvAssert(getPopScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getLandScore() const
{
	return (m_iLandScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeLandScore(int iChange)
{
	if (iChange != 0)
	{
		m_iLandScore += iChange;
		CvAssert(getLandScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getWondersScore() const
{
	return m_iWondersScore;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeWondersScore(int iChange)
{
	if (iChange != 0)
	{
		m_iWondersScore += iChange;
		CvAssert(getWondersScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getTechScore() const
{
	return m_iTechScore;
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeTechScore(int iChange)
{
	if (iChange != 0)
	{
		m_iTechScore += iChange;
		CvAssert(getTechScore() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setCombatExperience(int iExperience)
{
	CvAssert(iExperience >= 0);

	if (iExperience != getCombatExperience())
	{
		m_iCombatExperience = iExperience;

		// Enough XP for a Great General to appear?
		if (!isBarbarian())
		{
			int iExperienceThreshold = greatGeneralThreshold();
			if (m_iCombatExperience >= iExperienceThreshold && iExperienceThreshold > 0)
			{
				// create great person
				CvCity* pBestCity = NULL;
				int iBestValue = INT_MAX;
				int iLoop;
				for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					int iValue = 4 * GC.getGame().getJonRandNum(getNumCities(), "Great General City Selection");

					for (int i = 0; i < NUM_YIELD_TYPES; i++)
					{
						iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
					}
					iValue += pLoopCity->findPopulationRank();

					if (iValue < iBestValue)
					{
						pBestCity = pLoopCity;
						iBestValue = iValue;
					}
				}

				if (pBestCity)
				{
					// Figure out which Promotion is the one which makes a unit a Great General
					PromotionTypes eGreatGeneralPromotion = NO_PROMOTION;
					for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
					{
						const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
						if(pkPromotionInfo)
						{
							if (pkPromotionInfo->IsGreatGeneral())
							{
								eGreatGeneralPromotion = eLoopPromotion;
							}
						}
					}

					// If GG promotion exists, find a unit which gets it for free (i.e. the Great General unit itself)
					if (eGreatGeneralPromotion != NO_PROMOTION)
					{
						for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
						{
							const UnitTypes eLoopUnit = static_cast<UnitTypes>(iI);
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
							if(pkUnitInfo)
							{
								if (pkUnitInfo->GetFreePromotions(eGreatGeneralPromotion))
								{
									// Is this the right unit of this class for this civ?
									const UnitTypes eUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits((UnitClassTypes)pkUnitInfo->GetUnitClassType());

									if (eUnit == eLoopUnit)
									{
										pBestCity->createGreatGeneral(eUnit);
										setCombatExperience(getCombatExperience() - iExperienceThreshold);
										break;
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
void CvPlayer::changeCombatExperience(int iChange)
{
	if (getCombatExperience() + iChange < 0)
	{
		setCombatExperience(0);
	}
	else
	{
		setCombatExperience(getCombatExperience() + iChange);
	}

	m_iLifetimeCombatExperience += iChange;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getLifetimeCombatExperience() const
{
	return m_iLifetimeCombatExperience;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getBorderObstacleCount() const
{
	return m_iBorderObstacleCount;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isBorderObstacle() const
{
	return (getBorderObstacleCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeBorderObstacleCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBorderObstacleCount = (m_iBorderObstacleCount + iChange);
		CvAssert(getBorderObstacleCount() >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNetID() const
{
	return CvPreGame::netID(GetID());
}

//	--------------------------------------------------------------------------------
void CvPlayer::setNetID(int iNetID)
{
	CvPreGame::setNetID(GetID(), iNetID);
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isConnected() const
{
	return gDLL->IsPlayerConnected(GetID());
}

//	--------------------------------------------------------------------------------
void CvPlayer::sendTurnReminder()
{
	//Send a game invite to the player if they aren't currently connected to the game.
	gDLL->sendTurnReminder(GetID());

	/* email notifications not implimented.
	if(!getPbemEmailAddress().empty() &&
	        !gDLL->GetPitbossSmtpHost().empty())
	{

	}
	*/
}

//	--------------------------------------------------------------------------------
uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}


//	--------------------------------------------------------------------------------
uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isMinorCiv() const
{
	return CvPreGame::isMinorCiv(m_eID);
}


//	--------------------------------------------------------------------------------
/// Has this player betrayed a Minor Civ he was bullying by declaring war on him?
bool CvPlayer::IsHasBetrayedMinorCiv() const
{
	return m_bHasBetrayedMinorCiv;
}

//	--------------------------------------------------------------------------------
/// Sets this player to have betrayed a Minor Civ he was bullying by declaring war on him
void CvPlayer::SetHasBetrayedMinorCiv(bool bValue)
{
	if (IsHasBetrayedMinorCiv() != bValue)
	{
		m_bHasBetrayedMinorCiv = bValue;
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::setAlive(bool bNewValue, bool bNotify)
{
	CvString strBuffer;
	int iI;

	if (isAlive() != bNewValue)
	{
		m_bAlive = bNewValue;

		GET_TEAM(getTeam()).changeAliveCount((isAlive()) ? 1 : -1);

		// Tell Minor Civ AI what's up so that it knows when to add/remove bonuses for players it's friends with
		if (isMinorCiv())
		{
			GetMinorCivAI()->DoChangeAliveStatus(bNewValue);
		}

		if (isAlive())
		{
			if (!isEverAlive())
			{
				m_bEverAlive = true;

				GET_TEAM(getTeam()).changeEverAliveCount(1);
			}

			if (getNumCities() == 0)
			{
				setFoundedFirstCity(false);
			}

			GET_TEAM(getTeam()).SetKilledByTeam(NO_TEAM);

			if(isSimultaneousTurns() || (GC.getGame().getNumGameTurnActive() == 0) || (GC.getGame().isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
			{
				setTurnActive(true);
			}

			gDLL->openSlot(GetID());
		}
		else
		{
			clearResearchQueue();
			killUnits();
			killCities();
			if(CvPreGame::isNetworkMultiplayerGame() && m_eID == GC.getGame().getActivePlayer())
				gDLL->netDisconnect();

			// Reset incoming units
			for (int iLoop = 0; iLoop < MAX_INCOMING_UNITS; iLoop++)
			{
				// Reset stuff
				SetIncomingUnitCountdown(iLoop, -1);
				SetIncomingUnitType(iLoop, NO_UNIT);
			}

			GC.getGame().GetGameDeals()->DoCancelAllDealsWithPlayer(GetID());

			setTurnActive(false);

			//if (!isHuman())
			{
				gDLL->closeSlot(GetID());
			}

			if (bNotify && !isBarbarian())
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_MISC_CIV_DESTROYED");
				strMessage << getCivilizationAdjectiveKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_CIV_DESTROYED_SHORT");
				strSummary << getCivilizationShortDescriptionKey();

				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const PlayerTypes eOtherPlayer = static_cast<PlayerTypes>(iI);
					CvPlayerAI& kOtherPlayer = GET_PLAYER(eOtherPlayer);

					if (kOtherPlayer.isAlive() && kOtherPlayer.GetNotifications())
					{
						kOtherPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_KILLED, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GetID(), strMessage.toUTF8(), -1, -1);
			}

			GC.getGame().testVictory();
		}

		GC.getGame().setScoreDirty(true);
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::verifyAlive()
{
	bool bKill;

	if (isAlive())
	{
		bKill = false;

		if (!bKill)
		{
			if (!isBarbarian())
			{
				if (getNumCities() == 0 && getAdvancedStartPoints() < 0)
				{
					if ((getNumUnits() == 0) || (!(GC.getGame().isOption(GAMEOPTION_COMPLETE_KILLS)) && isFoundedFirstCity()))
					{
						if (!GetPlayerTraits()->IsStaysAliveZeroCities())
						{
							bKill = true;
						}
					}
				}
			}
		}

		if (!bKill)
		{
			if (!isBarbarian())
			{
				if (GC.getGame().getMaxCityElimination() > 0)
				{
					if (getCitiesLost() >= GC.getGame().getMaxCityElimination())
					{
						bKill = true;
					}
				}
			}
		}

		if (bKill)
		{
			setAlive(false, false);
		}
	}
	else
	{
		if ((getNumCities() > 0) || (getNumUnits() > 0))
		{
			setAlive(true);
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isTurnActive() const
{
	return m_bTurnActive;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	CvAssertMsg(GC.getGame().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");

	// does nothing more than to set the member variable before saving the game
	// the rest of the turn will be performed upon loading the game
	// This allows the player to browse the game in paused mode after he has generated the save
	if (isTurnActive() != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGame().changeNumGameTurnActive(isTurnActive() ? 1 : -1, "setTurnActiveForPlayByEmail");
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
	if (isTurnActive() != bNewValue)
	{
		m_bTurnActive = bNewValue;
		DLLUI->PublishEndTurnDirty();

		CvGame& kGame = GC.getGame();

		/////////////////////////////////////////////
		// TURN IS BEGINNING
		/////////////////////////////////////////////

		if (isTurnActive())
		{
			CvAssertMsg(isAlive(), "isAlive is expected to be true");

			setEndTurn(false);

			DoUnitAttrition();

			if (kGame.getActivePlayer() == m_eID)
			{
				CvMap& theMap = GC.getMap();
				theMap.updateDeferredFog();
			}

			if ((kGame.isHotSeat() || kGame.isPbem()) && isHuman() && bDoTurn)
			{
				DLLUI->clearEventMessages();

				kGame.setActivePlayer(GetID());
			}

			if(CvPreGame::isPitBoss() && kGame.getActivePlayer() != m_eID && isHuman() && gDLL->IsHost() && !isConnected())
			{//send turn reminder if the player isn't actively connected to the game.
				sendTurnReminder();
			}

			std::ostringstream infoStream;
			infoStream << "setTurnActive() for player ";
			infoStream << (int)GetID();
			infoStream << " ";
			infoStream << getName();
			kGame.changeNumGameTurnActive(1, infoStream.str());

			DLLUI->PublishPlayerTurnStatus(DLLUIClass::TURN_START, GetID());

			if (bDoTurn)
			{
				SetAllUnitsUnprocessed();

				bool bCommonPathFinderMPCaching = GC.getPathFinder().SetMPCacheSafe(true);
				bool bIgnoreUnitsPathFinderMPCaching = GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(true);
				bool bTacticalPathFinderMPCaching = GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(true);
				bool bInfluencePathFinderMPCaching = GC.getInfluenceFinder().SetMPCacheSafe(true);
				bool bRoutePathFinderMPCaching = GC.getRouteFinder().SetMPCacheSafe(true);
				bool bWaterRoutePathFinderMPCaching = GC.GetWaterRouteFinder().SetMPCacheSafe(true);

				kGame.GetTacticalAnalysisMap()->RefreshDataForNextPlayer(this);

				{
					AI_PERF_FORMAT("AI-perf.csv", ("Connections/Gold, Turn %03d, %s", kGame.getElapsedGameTurns(), getCivilizationShortDescription()) );

					// This block all has things which might change based on city connections changing
					m_pCityConnections->Update();
					GetTreasury()->DoUpdateCityConnectionGold();
					DoUpdateHappiness();
				}

				{
					AI_PERF_FORMAT("AI-perf.csv", ("Builder Tasking, Turn %03d, %s", kGame.getElapsedGameTurns(), getCivilizationShortDescription()) );

					m_pBuilderTaskingAI->Update();
				}

				if (kGame.getElapsedGameTurns() > 0)
				{
					if (isAlive())
					{
						if (GetDiplomacyRequests())
						{
							GetDiplomacyRequests()->BeginTurn();
						}

						doTurn();

						doTurnUnits();
					}
				}

				GC.getPathFinder().SetMPCacheSafe(bCommonPathFinderMPCaching);
				GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(bIgnoreUnitsPathFinderMPCaching);
				GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(bTacticalPathFinderMPCaching);
				GC.getInfluenceFinder().SetMPCacheSafe(bInfluencePathFinderMPCaching);
				GC.getRouteFinder().SetMPCacheSafe(bRoutePathFinderMPCaching);
				GC.GetWaterRouteFinder().SetMPCacheSafe(bWaterRoutePathFinderMPCaching);

				if ((GetID() == kGame.getActivePlayer()) && (kGame.getElapsedGameTurns() > 0))
				{
					if (kGame.isNetworkMultiPlayer())
					{
						DLLUI->AddMessage(0, GetID(), true, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_TURN_BEGINS").GetCString(), "AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY);
					}
				}

				doWarnings();
			}

			if (GetID() == kGame.getActivePlayer())
			{
				GetUnitCycler().Rebuild();

				if (DLLUI->GetLengthSelectionList() == 0)
				{
					DLLUI->setCycleSelectionCounter(1);
				}

				DLLUI->setDirty(SelectionCamera_DIRTY_BIT, true);

				// slewis - added this so the tutorial knows when a turn begins
				DLLUI->PublishActivePlayerTurnStart();
			}
			else if(isHuman() && kGame.isGameMultiPlayer())
			{
				DLLUI->PublishRemotePlayerTurnStart();
			}
		}

		/////////////////////////////////////////////
		// TURN IS ENDING
		/////////////////////////////////////////////

		else
		{
			CvAssertFmt(GetEndTurnBlockingType() == NO_ENDTURN_BLOCKING_TYPE, "Expecting the end-turn blocking to be NO_ENDTURN_BLOCKING_TYPE, got %d", GetEndTurnBlockingType());
			SetEndTurnBlocking(NO_ENDTURN_BLOCKING_TYPE, -1);	// Make sure this is clear so the UI doesn't block when it is not our turn.

			DoUnitReset();

			if (!isHuman())
			{
				RespositionInvalidUnits();
			}

			if (GetNotifications())
			{
				GetNotifications()->EndOfTurnCleanup();
			}

			if (GetDiplomacyRequests())
			{
				GetDiplomacyRequests()->EndTurn();
			}

			if (GetID() == kGame.getActivePlayer())
			{
				DLLUI->PublishActivePlayerTurnEnd();
			}

			if (!isHuman() || (isHuman() && !isAlive()) || ( isHuman() && gDLL->HasReceivedTurnAllComplete(GetID()) ) || kGame.getAIAutoPlay())
				kGame.changeNumGameTurnActive(-1, std::string("setTurnActive() for player ") + getName());

			DLLUI->PublishPlayerTurnStatus(DLLUIClass::TURN_END, GetID());
		}
	}
	else
	{
		gDLL->netMessageDebugLog("SetTurnActive() called without changing the end turn status.");
	}
}

//	----------------------------------------------------------------------------
bool CvPlayer::isSimultaneousTurns() const
{
	if(GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS))
	{//in dynamic turns mode, our turn mode varies
		return m_bDynamicTurnsSimultMode;
	}
	else if(GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
	{
		return true;
	}

	return false;
}


//	----------------------------------------------------------------------------
void CvPlayer::setDynamicTurnsSimultMode(bool simultaneousTurns)
{
	if(simultaneousTurns != m_bDynamicTurnsSimultMode)
	{
		CvNotifications* pNotifications = GetNotifications();
		if (pNotifications)
		{
			NotificationTypes notifyType = NOTIFICATION_TURN_MODE_SEQUENTIAL;
			Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SEQUENTIAL");
			Localization::String localizedTextSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SEQUENTIAL_SUMMARY");
			if(simultaneousTurns)
			{
				notifyType = NOTIFICATION_TURN_MODE_SIMULTANEOUS;
				localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SIMULTANEOUS");
				localizedTextSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TURN_MODE_SIMULTANEOUS_SUMMARY");
			}

			pNotifications->Add(notifyType, localizedText.toUTF8(), localizedTextSummary.toUTF8(), -1, -1, -1);
		}

		m_bDynamicTurnsSimultMode = simultaneousTurns;
	}
	
}

//	----------------------------------------------------------------------------
bool CvPlayer::isAutoMoves() const
{
	return m_bAutoMoves;
}

//	----------------------------------------------------------------------------
void CvPlayer::setAutoMoves(bool bNewValue)
{
	if (isAutoMoves() != bNewValue)
	{
		m_bAutoMoves = bNewValue;
		m_bProcessedAutoMoves = false;
	}
}

//	----------------------------------------------------------------------------
bool CvPlayer::hasProcessedAutoMoves() const
{
	return m_bProcessedAutoMoves;
}

//	----------------------------------------------------------------------------
void CvPlayer::setProcessedAutoMoves(bool bNewValue)
{
	if (hasProcessedAutoMoves() != bNewValue)
	{
		m_bProcessedAutoMoves = bNewValue;
	}
}

//	----------------------------------------------------------------------------
bool CvPlayer::isEndTurn() const
{
	return m_bEndTurn;
}

//	------------------------------------------------------------------------------------------------
void CvPlayer::setEndTurn(bool bNewValue)
{
	CvGame & game = GC.getGame();

	if(isSimultaneousTurns()
		&& bNewValue 
		&& game.isNetworkMultiPlayer() 
		&& !gDLL->HasReceivedTurnAllCompleteFromAllPlayers())
	{//When doing simultaneous turns in multiplayer, we don't want anyone to end their turn until everyone has signalled TurnAllComplete.
		// No setting end turn to true until all the players have sent the TurnComplete network message
		return;
	}

	// If this is a remote player in an MP match, don't
	// honor the end of turn request if the player still
	// has units to run the simulation for the turn
	if(!isEndTurn() && isHuman() && GetID() != game.getActivePlayer())
	{
		if(hasBusyUnitOrCity() || (!gDLL->HasReceivedTurnComplete(GetID()) && hasReadyUnit()))
		{
			return;
		}
	}
	else if ( !isHuman() )
	{
		if (hasBusyUnitOrCity())
		{
			return;
		}
	}

	if (isEndTurn() != bNewValue)
	{
		CvAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		m_bEndTurn = bNewValue;

		if(isEndTurn())
		{
			if(!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
			{//fully simultaneous turns only run automoves after every human has moved.
				checkRunAutoMovesForEveryone();
			}
			else
			{
				setAutoMoves(true);
			}
		}
		else
			setAutoMoves(false);
	}
	else
	{
		// This check is here for the AI.  Currently, the setEndTurn(true) never seems to get called for AI players, the automoves are just set directly
		// Why is this?  It would be great if all players were processed the same.
		if (!bNewValue && isAutoMoves())
			setAutoMoves(false);
	}
}

//	---------------------------------------------------------------------------
void CvPlayer::checkRunAutoMovesForEveryone()
{
	bool runAutoMovesForEveryone = true;
	int i = 0;
	for(i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer & p = CvPlayerAI::getPlayer((PlayerTypes)i);
		if(p.isHuman() && !p.isObserver() 
			// Check to see if this human player hasn't gotten to the end turn phase of their turn.  
			// This gets tricky because hot joiners can hop into an ai civ that already finished their turn.
			// When this occurs, the hot joiner will not be turn active, will have already run their automoves,
			// and not have end turn set. (AIs do not set end turn) *sigh*
			// To handle that case, we assume that human players who are not endturn and turn inactive after TurnAllComplete
			// are ready for the human automoves phase.
			&& (!p.isEndTurn()
			&& (!gDLL->HasReceivedTurnAllCompleteFromAllPlayers() || p.isTurnActive()))) 
		{
			runAutoMovesForEveryone = false;
			break;
		}
	}

	if(runAutoMovesForEveryone)
	{
		for(i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer & p = CvPlayerAI::getPlayer((PlayerTypes)i);
			if(p.isHuman())
			{
				p.setAutoMoves(true);
			}
		}
	}
}

//	---------------------------------------------------------------------------
EndTurnBlockingTypes CvPlayer::GetEndTurnBlockingType (void) const
{
	return m_eEndTurnBlockingType;
}

//	---------------------------------------------------------------------------
int CvPlayer::GetEndTurnBlockingNotificationIndex (void) const
{
	return m_iEndTurnBlockingNotificationIndex;
}

//	---------------------------------------------------------------------------
void CvPlayer::SetEndTurnBlocking(EndTurnBlockingTypes eBlockingType, int iNotificationIndex)
{
	bool bFireEvent = false;
	if (m_eEndTurnBlockingType != eBlockingType || m_iEndTurnBlockingNotificationIndex != iNotificationIndex)
	{
		bFireEvent = true;
	}

	EndTurnBlockingTypes ePrevBlockingType = m_eEndTurnBlockingType;
	m_eEndTurnBlockingType = eBlockingType;
	m_iEndTurnBlockingNotificationIndex = iNotificationIndex;

	if (bFireEvent)
	{
		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->SetEndTurnBlockingChanged( ePrevBlockingType, m_eEndTurnBlockingType );
			GC.GetEngineUserInterface()->UpdateEndTurn();
		}
	}
}

//	---------------------------------------------------------------------------
bool CvPlayer::isTurnDone() const
{
	// if this returns true, popups and diplomacy will wait to appear until next turn
	if (!GC.getGame().isPbem() && !GC.getGame().isHotSeat())
	{
		return false;
	}
	if (!isHuman() )
	{
		return true;
	}
	if (!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}


//	--------------------------------------------------------------------------------
void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if (isFoundedFirstCity() != bNewValue)
	{
		m_bFoundedFirstCity = bNewValue;

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(PercentButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isStrike() const
{
	return m_bStrike;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setStrike(bool bNewValue)
{
	if (m_bStrike != bNewValue)
	{
		m_bStrike = bNewValue;

		if (m_bStrike)
		{
			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->AddMessage(0, GetID(), false, GC.getEVENT_MESSAGE_TIME(), GetLocalizedText("TXT_KEY_MISC_UNITS_ON_STRIKE").GetCString(), "AS2D_STRIKE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
/// Is the player is cramped in his current area?
bool CvPlayer::IsCramped() const
{
	return m_bCramped;
}

//	--------------------------------------------------------------------------------
/// Determines if the player is cramped in his current area.  Not a perfect algorithm, as it will double-count Plots shared by different Cities, but it should be good enough
void CvPlayer::DoUpdateCramped()
{
	CvCity* pLoopCity;
	CvPlot* pPlot;

	int iTotalPlotsNearby = 0;
	int iUsablePlotsNearby = 0;

	int iRange = GC.getCRAMPED_RANGE_FROM_CITY();

	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int iX = -iRange; iX <= iRange; iX++)
		{
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				pPlot = plotXYWithRangeCheck(pLoopCity->getX(), pLoopCity->getY(), iX, iY, iRange);

				if (pPlot != NULL)
				{
					// Plot not owned by me
					if (!pPlot->isOwned() || pPlot->getOwner() != GetID())
					{
						iTotalPlotsNearby++;

						// A "good" unowned Plot
						if (!pPlot->isOwned() && !pPlot->isImpassable() && !pPlot->isWater())
						{
							iUsablePlotsNearby++;
						}
					}
				}
			}
		}
	}

	if (iTotalPlotsNearby > 0)
	{
		if (100 * iUsablePlotsNearby / iTotalPlotsNearby <= GC.getCRAMPED_USABLE_PLOT_PERCENT())	// 20
		{
			m_bCramped = true;
		}
		else
		{
			m_bCramped = false;
		}
	}
}

//	--------------------------------------------------------------------------------
CvHandicapInfo& CvPlayer::getHandicapInfo() const
{
	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(getHandicapType());
	if(pkHandicapInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid handicap!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	return *pkHandicapInfo;
#pragma warning ( pop )
}

//	--------------------------------------------------------------------------------
HandicapTypes CvPlayer::getHandicapType() const
{
	return CvPreGame::handicap(GetID());
}

//	--------------------------------------------------------------------------------
CvCivilizationInfo& CvPlayer::getCivilizationInfo() const
{
	CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	if(pkCivilizationInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid civilization type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	return *pkCivilizationInfo;
#pragma warning ( pop )
}

//	--------------------------------------------------------------------------------
CivilizationTypes CvPlayer::getCivilizationType() const
{
	return CvPreGame::civilization(GetID());
}


//	--------------------------------------------------------------------------------
CvLeaderHeadInfo& CvPlayer::getLeaderInfo() const
{
	CvLeaderHeadInfo* pkLeaderInfo = GC.getLeaderHeadInfo(getLeaderType());
	if(pkLeaderInfo == NULL)
	{
		const char* szError = "ERROR: Player does not contain valid leader type!!";
		GC.LogMessage(szError);
		CvAssertMsg(false, szError);
	}

#pragma warning ( push )
#pragma warning ( disable : 6011 ) // Dereferencing NULL pointer
	return *pkLeaderInfo;
#pragma warning ( pop )
}

//	--------------------------------------------------------------------------------
LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return CvPreGame::leaderHead(GetID());
}


//	--------------------------------------------------------------------------------
LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
}


//	--------------------------------------------------------------------------------
EraTypes CvPlayer::GetCurrentEra() const
{
	return GET_TEAM(getTeam()).GetCurrentEra();
}

//	--------------------------------------------------------------------------------
void CvPlayer::setTeam(TeamTypes eTeam)
{
	CvAssert(eTeam != NO_TEAM);
	CvAssert(getTeam() != NO_TEAM);

	GET_TEAM(getTeam()).changeNumMembers(-1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(-1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(-1);
	}
	GET_TEAM(getTeam()).changeNumCities(-(getNumCities()));
	GET_TEAM(getTeam()).changeTotalPopulation(-(getTotalPopulation()));
	GET_TEAM(getTeam()).changeTotalLand(-(getTotalLand()));

	CvPreGame::setTeamType(GetID(), eTeam);

	GET_TEAM(getTeam()).changeNumMembers(1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(1);
	}
	GET_TEAM(getTeam()).changeNumCities(getNumCities());
	GET_TEAM(getTeam()).changeTotalPopulation(getTotalPopulation());
	GET_TEAM(getTeam()).changeTotalLand(getTotalLand());
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsAITeammateOfHuman() const
{
	bool bRtnValue = false;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer &player = GET_PLAYER(static_cast<PlayerTypes>(i));
		if (player.isHuman() && player.isAlive())
		{
			if (player.getTeam() == getTeam())
			{
				bRtnValue = true;
				break;
			}
		}
	}

	return bRtnValue;
}

//	--------------------------------------------------------------------------------
PlayerColorTypes CvPlayer::getPlayerColor() const
{
	return CvPreGame::playerColor(GetID());
}

//	--------------------------------------------------------------------------------
const CvColorA& CvPlayer::getPlayerTextColor() const
{
	CvAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	CvPlayerColorInfo* pkPlayerColorInfo = GC.GetPlayerColorInfo(getPlayerColor());
	CvColorInfo* pkColorInfo = NULL;
	if(pkPlayerColorInfo)
	{
		ColorTypes eTextColor = static_cast<ColorTypes>(pkPlayerColorInfo->GetColorTypeText());
		pkColorInfo = GC.GetColorInfo(eTextColor);
		if(pkColorInfo)
			return pkColorInfo->GetColor();
	}

	//Default to black text if no color exists.
	static CvColorA black(0,0,0,1.0f);
	return black;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSeaPlotYield[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSeaPlotYield.setAt(eIndex, m_aiSeaPlotYield[eIndex] + iChange);

		updateYield();
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYieldRateModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiYieldRateModifier.setAt(eIndex, m_aiYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldRateModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalYieldRateModifier.setAt(eIndex, m_aiCapitalYieldRateModifier[eIndex] + iChange);
		invalidateYieldRankCache(eIndex);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYieldThreshold[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	int iBestValue;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	iBestValue = GetPlayerTraits()->GetExtraYieldThreshold(eIndex);

	if (getExtraYieldThreshold(eIndex) != iBestValue)
	{
		m_aiExtraYieldThreshold.setAt(eIndex, iBestValue);
		CvAssert(getExtraYieldThreshold(eIndex) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetScience() const
{
	return GetScienceTimes100() / 100;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetScienceTimes100() const
{
	// If we're in anarchy, then no Research is done!
	if (IsAnarchy())
		return 0;

	int iValue = 0;

	// Science from our Cities
	iValue += GetScienceFromCitiesTimes100();

	// Science from other players!
	iValue += GetScienceFromOtherPlayersTimes100();

	// Happiness converted to Science? (Policies, etc.)
	iValue += GetScienceFromHappinessTimes100();

	// Research Agreement bonuses
	iValue += GetScienceFromResearchAgreementsTimes100();

	// If we have a negative Treasury + GPT then it gets removed from Science
	iValue += GetScienceFromBudgetDeficitTimes100();

	return max(iValue, 0);
}


//	--------------------------------------------------------------------------------
/// Where is our Science coming from?
int CvPlayer::GetScienceFromCitiesTimes100() const
{
	int iScience = 0;

	const CvCity* pLoopCity;

	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iScience += pLoopCity->getYieldRateTimes100(YIELD_SCIENCE);
	}

	return iScience;
}

//	--------------------------------------------------------------------------------
/// Where is our Science coming from?
int CvPlayer::GetScienceFromOtherPlayersTimes100() const
{
	int iScience = 0;

	PlayerTypes ePlayer;
	int iScienceFromPlayer;
	for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		iScienceFromPlayer = 0;

		if (IsGetsScienceFromPlayer(ePlayer))
		{
			iScienceFromPlayer = GET_PLAYER(ePlayer).GetMinorCivAI()->GetScienceFriendshipBonusTimes100();

			iScience += iScienceFromPlayer;
		}
	}

	return iScience;
}

//	--------------------------------------------------------------------------------
/// Where is our Science coming from?
int CvPlayer::GetScienceFromHappinessTimes100() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return 0;
	}

	int iScience = 0;

	if (getHappinessToScience() != 0)
	{
		if (GetExcessHappiness() >= 0)
		{
			int iFreeScience = GetScienceFromCitiesTimes100() * getHappinessToScience();
			iFreeScience /= 100;

			iScience += iFreeScience;
		}
	}

	return iScience;
}

//	--------------------------------------------------------------------------------
/// Where is our Science coming from?
int CvPlayer::GetScienceFromResearchAgreementsTimes100() const
{
	int iScience = GetScienceFromCitiesTimes100();

	int iResearchAgreementBonus = /*15*/ GC.getRESEARCH_AGREEMENT_MOD() * GET_TEAM(getTeam()).GetTotalNumResearchAgreements();	// 15% per agreement
	iScience *= iResearchAgreementBonus;	// Apply to the % to the current value
	iScience /= 100;

	return iScience;
}

//	--------------------------------------------------------------------------------
/// Where is our Science coming from?
int CvPlayer::GetScienceFromBudgetDeficitTimes100() const
{
	int iScience = 0;

	int iGoldPerTurn = calculateGoldRateTimes100();
	if (GetTreasury()->GetGoldTimes100() + iGoldPerTurn < 0)
	{
		iScience += (GetTreasury()->GetGoldTimes100() + iGoldPerTurn);
	}

	return iScience;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsGetsScienceFromPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	// Might have global modifier
	if (IsMinorScienceAllies() && GET_PLAYER(ePlayer).GetMinorCivAI()->IsAllies(GetID()))
	{
		return true;
	}

	return m_pabGetsScienceFromPlayer[ePlayer];
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetGetsScienceFromPlayer(PlayerTypes ePlayer, bool bNewValue)
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (bNewValue != m_pabGetsScienceFromPlayer[ePlayer])
	{
		m_pabGetsScienceFromPlayer.setAt(ePlayer, bNewValue);
	}
}

//	--------------------------------------------------------------------------------
/// Player spending too much cash?
void CvPlayer::DoDeficit()
{
	int iNumMilitaryUnits = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsCombatUnit())
			iNumMilitaryUnits++;
	}

	// If the player has more units than cities, start disbanding things
	if (iNumMilitaryUnits > getNumCities())
	{
		if (GC.getGame().getJonRandNum(100, "Disband rand") < 50)
		{
			UnitHandle pLandUnit;
			UnitHandle pNavalUnit;
			int iLandScore = MAX_INT;
			int iNavalScore = MAX_INT;

			// Look for obsolete land units if in deficit or have sufficient units
			if (GetMilitaryAI()->GetLandDefenseState() <= DEFENSE_STATE_NEUTRAL)
			{
				pLandUnit = GetMilitaryAI()->FindBestUnitToScrap(true /*bLand*/, true /*bDeficitForcedDisband*/, iLandScore);
			}

			// Look for obsolete naval units if in deficit or have sufficient units
			if (GetMilitaryAI()->GetNavalDefenseState() <= DEFENSE_STATE_NEUTRAL)
			{
				pNavalUnit = GetMilitaryAI()->FindBestUnitToScrap(false/*bNaval*/, true /*bDeficitForcedDisband*/, iNavalScore);
			}

			if (iLandScore < MAX_INT && (GetMilitaryAI()->GetLandDefenseState() <= GetMilitaryAI()->GetNavalDefenseState() || iLandScore <= iNavalScore))
			{
				if (pLandUnit)
				{
					CvNotifications* pNotifications = GetNotifications();
					if (pNotifications)
					{
						Localization::String locString = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED");
						Localization::String locSummary = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED_S");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, locString.toUTF8(), locSummary.toUTF8(), pLandUnit->getX(), pLandUnit->getY(), pLandUnit->getUnitType(), GetID());
					}

					pLandUnit->scrap();
					GetMilitaryAI()->LogDeficitScrapUnit(pLandUnit);
				}
			}
			else if (iNavalScore < MAX_INT)
			{
				if (pNavalUnit)
				{
					CvNotifications* pNotifications = GetNotifications();
					if (pNotifications)
					{
						Localization::String locString = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED");
						Localization::String locSummary = Localization::Lookup("TXT_KEY_NTFN_UNIT_DISBANDED_S");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, locString.toUTF8(), locSummary.toUTF8(), pNavalUnit->getX(), pNavalUnit->getY(), pNavalUnit->getUnitType(), GetID());
					}

					pNavalUnit->scrap();
					GetMilitaryAI()->LogDeficitScrapUnit(pNavalUnit);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getSpecialistExtraYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSpecialistExtraYield[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeSpecialistExtraYield(YieldTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		// Have to handle Specialists yield update manually here because the "updateYield()" below only accounts for land Yield!

		CvCity* pLoopCity;
		int iLoop;
		int iNumTotalSpecialists = 0;

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				iNumTotalSpecialists = pLoopCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes) iSpecialistLoop);

				for (int iTempLoop = 0; iTempLoop < iNumTotalSpecialists; iTempLoop++)
				{
					pLoopCity->processSpecialist((SpecialistTypes) iSpecialistLoop, -1);
				}
			}
		}

		m_aiSpecialistExtraYield.setAt(eIndex ,m_aiSpecialistExtraYield[eIndex] + iChange);
		CvAssert(getSpecialistExtraYield(eIndex) >= 0);

		updateYield();

		// Reprocess Specialist AFTER yield change
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				iNumTotalSpecialists = pLoopCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes) iSpecialistLoop);

				for (int iTempLoop = 0; iTempLoop < iNumTotalSpecialists; iTempLoop++)
				{
					pLoopCity->processSpecialist((SpecialistTypes) iSpecialistLoop, 1);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Returns how "close" we are to another player (useful for diplomacy, war planning, etc.)
PlayerProximityTypes CvPlayer::GetProximityToPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (PlayerProximityTypes) m_aiProximityToPlayer[ePlayer];
}

//	--------------------------------------------------------------------------------
/// Sets how "close" we are to another player (useful for diplomacy, war planning, etc.)
void CvPlayer::SetProximityToPlayer(PlayerTypes ePlayer, PlayerProximityTypes eProximity)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(GetID() != ePlayer, "Trying to calculate proximity to oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(eProximity >= NO_PLAYER_PROXIMITY, "eIndex is expected to be non-negative (invalid Index)");	// NO_PLAYER_PROXIMITY is valid because some players may have no Cities (e.g. on the first turn)
	CvAssertMsg(eProximity < NUM_PLAYER_PROXIMITIES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if ((GC.getLogging() && GC.getAILogging()))
	{
		if (eProximity != m_aiProximityToPlayer[ePlayer])
		{
			// Open the log file
			CvString strFileName = "PlayerProximityLog.csv";
			FILogFile *pLog;
			pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
			CvString strLog, strTemp;

			CvString strPlayerName;
			strPlayerName = getCivilizationShortDescription();
			strLog += strPlayerName;
			strLog += ",";

			strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
			strLog += strTemp;
			CvString strOtherPlayerName;
			strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
			strLog += strOtherPlayerName;
			strLog += ",";

			switch (m_aiProximityToPlayer[ePlayer])
			{
			case NO_PLAYER_PROXIMITY:
				strLog += "No player proximity,";
				break;
			case PLAYER_PROXIMITY_NEIGHBORS:
				strLog += "Neighbors,";
				break;
			case PLAYER_PROXIMITY_CLOSE:
				strLog += "Close,";
				break;
			case PLAYER_PROXIMITY_FAR:
				strLog += "Far,";
				break;
			case PLAYER_PROXIMITY_DISTANT:
				strLog += "Distant,";
				break;
			}

			strLog += "-->,";

			switch (eProximity)
			{
			case NO_PLAYER_PROXIMITY:
				strLog += "No player proximity,";
				break;
			case PLAYER_PROXIMITY_NEIGHBORS:
				strLog += "Neighbors,";
				break;
			case PLAYER_PROXIMITY_CLOSE:
				strLog += "Close,";
				break;
			case PLAYER_PROXIMITY_FAR:
				strLog += "Far,";
				break;
			case PLAYER_PROXIMITY_DISTANT:
				strLog += "Distant,";
				break;
			}

			pLog->Msg(strLog);
		}
	}

	m_aiProximityToPlayer.setAt(ePlayer, eProximity);
}

//	--------------------------------------------------------------------------------
/// Figure out how "close" we are to another player (useful for diplomacy, war planning, etc.)
void CvPlayer::DoUpdateProximityToPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iSmallestDistanceBetweenCities = GC.getMap().numPlots();
	int iAverageDistanceBetweenCities = 0;

	int iNumCityConnections = 0;

	CvCity* pLoopMyCity;
	CvCity* pLoopTheirCity;

	int iMyCityLoop;
	int iTheirCityLoop;

	int iTempDistance;

	// Loop through all of MY Cities
	for (pLoopMyCity = firstCity(&iMyCityLoop); pLoopMyCity != NULL; pLoopMyCity = nextCity(&iMyCityLoop))
	{
		// Loop through all of THEIR Cities
		for (pLoopTheirCity = GET_PLAYER(ePlayer).firstCity(&iTheirCityLoop); pLoopTheirCity != NULL; pLoopTheirCity = GET_PLAYER(ePlayer).nextCity(&iTheirCityLoop))
		{
			iNumCityConnections++;

			iTempDistance = plotDistance(pLoopMyCity->getX(), pLoopMyCity->getY(), pLoopTheirCity->getX(), pLoopTheirCity->getY());

			// Smallest distance between any two Cities
			if (iTempDistance < iSmallestDistanceBetweenCities)
			{
				iSmallestDistanceBetweenCities = iTempDistance;
			}

			iAverageDistanceBetweenCities += iTempDistance;
		}
	}

	// Seed this value with something reasonable to start.  This will be the value assigned if one player has 0 Cities.
	PlayerProximityTypes eProximity = NO_PLAYER_PROXIMITY;

	if (iNumCityConnections > 0)
	{
		iAverageDistanceBetweenCities /= iNumCityConnections;

		// Closest Cities must be within a certain range
		if (iSmallestDistanceBetweenCities <= /*7*/ GC.getPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT())
		{
			eProximity = PLAYER_PROXIMITY_NEIGHBORS;
		}
		// If our closest Cities are pretty near one another  and our average is less than the max then we can be considered CLOSE (will also look at City average below)
		else if (iSmallestDistanceBetweenCities <= /*11*/ GC.getPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY())
		{
			eProximity = PLAYER_PROXIMITY_CLOSE;
		}

		// If we've already set ourselves as Neighbors, no need to undo what we just did
		if (eProximity == NO_PLAYER_PROXIMITY)
		{
			int iMapFactor = (GC.getMap().getGridWidth() + GC.getMap().getGridHeight()) / 2;

			// Normally base distance on map size, but cap it at a certain point
			// Close can't be so big that it sits on Far's turf
			int iCloseDistance = iMapFactor * /*25*/ GC.getPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER() / 100;
			if (iCloseDistance > /*20*/ GC.getPROXIMITY_CLOSE_DISTANCE_MAX())
			{
				iCloseDistance = /*20*/ GC.getPROXIMITY_CLOSE_DISTANCE_MAX();
			}
			// Close also can't be so small that it sits on Neighbor's turf
			else if (iCloseDistance < /*10*/ GC.getPROXIMITY_CLOSE_DISTANCE_MIN())
			{
				iCloseDistance = /*10*/ GC.getPROXIMITY_CLOSE_DISTANCE_MIN();
			}

			// Far can't be so big that it sits on Distant's turf
			int iFarDistance = iMapFactor * /*45*/ GC.getPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER() / 100;
			if (iFarDistance > /*50*/ GC.getPROXIMITY_FAR_DISTANCE_MAX())
			{
				iFarDistance = /*50*/ GC.getPROXIMITY_FAR_DISTANCE_MAX();
			}
			// Far also can't be so small that it sits on Close's turf
			else if (iFarDistance < /*20*/ GC.getPROXIMITY_FAR_DISTANCE_MIN())
			{
				iFarDistance = /*20*/ GC.getPROXIMITY_FAR_DISTANCE_MIN();
			}

			// Close
			if (iAverageDistanceBetweenCities <= iCloseDistance)
			{
				eProximity = PLAYER_PROXIMITY_CLOSE;
			}
			// Far
			else if (iAverageDistanceBetweenCities <= iFarDistance)
			{
				eProximity = PLAYER_PROXIMITY_FAR;
			}
			// Distant
			else
			{
				eProximity = PLAYER_PROXIMITY_DISTANT;
			}
		}

		// Players NOT on the same landmass - bump up PROXIMITY by one level (unless we're already distant)
		if (eProximity != PLAYER_PROXIMITY_DISTANT)
		{
			// Both players have capitals, so we can check their areas to see if they're separated by water
			if (getCapitalCity() != NULL && GET_PLAYER(ePlayer).getCapitalCity() != NULL)
			{
				if (getCapitalCity()->getArea() != GET_PLAYER(ePlayer).getCapitalCity()->getArea())
				{
					eProximity = PlayerProximityTypes(eProximity - 1);
				}
			}
		}
	}

	int iNumMajorsLeft = GC.getGame().countMajorCivsAlive();

	// Only two players left, the farthest we can be considered is "Close"
	if (iNumMajorsLeft == 2)
		eProximity = max(eProximity, PLAYER_PROXIMITY_CLOSE);

	// Four or fewer players left, the farthest we can be considered is "Far"
	else if (iNumMajorsLeft <= 4)
		eProximity = max(eProximity, PLAYER_PROXIMITY_FAR);

	SetProximityToPlayer(ePlayer, eProximity);
}

//	--------------------------------------------------------------------------------
/// How long until a RA with a player takes effect
void CvPlayer::DoResearchAgreements()
{
	// Research Agreement counter
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (GetResearchAgreementCounter(eLoopPlayer) > 0)
		{
			ChangeResearchAgreementCounter(eLoopPlayer, -1);

			if (GetResearchAgreementCounter(eLoopPlayer) == 0)
			{
				if (!isHuman())
					AI_chooseFreeTech();
				else
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NTFN_RA_FREE_TECH");
					strMessage << GET_PLAYER(eLoopPlayer).getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NTFN_RA_FREE_TECH_S");

					CvNotifications* pNotifications = GetNotifications();
					if (pNotifications)
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);

					chooseTech(1, strMessage.toUTF8());
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// How long until a RA with a player takes effect
int CvPlayer::GetResearchAgreementCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiResearchAgreementCounter[ePlayer];
}

//	--------------------------------------------------------------------------------
/// How long until a RA with a player takes effect
void CvPlayer::SetResearchAgreementCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(GetID() != ePlayer, "Trying to make a RA Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_aiResearchAgreementCounter.setAt(ePlayer, iValue);
}

//	--------------------------------------------------------------------------------
/// How long until a RA with a player takes effect
void CvPlayer::ChangeResearchAgreementCounter(PlayerTypes ePlayer, int iChange)
{
	SetResearchAgreementCounter(ePlayer, GetResearchAgreementCounter(ePlayer) + iChange);
}

//	--------------------------------------------------------------------------------
/// Someone sent us a present!
void CvPlayer::DoCivilianReturnLogic(bool bReturn, PlayerTypes eToPlayer, int iUnitID)
{
	CvUnit* pUnit = getUnit(iUnitID);
	if(!pUnit)
	{
		return;
	}

	CvPlot* pPlot = pUnit->plot();
	if(!pPlot)
	{
		return;
	}

	// Kill any units this guy is transporting
	IDInfo* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (NULL != pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
			pLoopUnit->kill(true);
	}

	// What are the details for the new unit?
	UnitTypes eNewUnitType = pUnit->getUnitType();

	if (!bReturn)
		eNewUnitType = pUnit->getCaptureUnitType(getCivilizationType());

	int iX = pUnit->getX();
	int iY = pUnit->getY();

	// Returns to the previous owner
	if (bReturn)
	{
		pUnit->kill(true);
		CvUnit* pNewUnit = GET_PLAYER(eToPlayer).initUnit(eNewUnitType, iX, iY);
		CvAssert(pNewUnit != NULL);
		if (pNewUnit)
		{
			if (!pNewUnit->jumpToNearestValidPlot())
				pNewUnit->kill(false);
		}

		// Returned to a city-state
		if (GET_PLAYER(eToPlayer).isMinorCiv())
		{
			int iInfluence = /*45*/ GC.getRETURN_CIVILIAN_FRIENDSHIP();
			GET_PLAYER(eToPlayer).GetMinorCivAI()->ChangeFriendshipWithMajor(GetID(), iInfluence);
		}
		// Returned to major power
		else if (!GET_PLAYER(eToPlayer).isHuman())
		{
			GET_PLAYER(eToPlayer).GetDiplomacyAI()->ChangeNumCiviliansReturnedToMe(GetID(), 1);
		}
	}
	// Kept for oneself
	else
	{
		// Make a new unit because the kind we should capture doesn't match (e.g. Settler to Worker)
		if (eNewUnitType != pUnit->getUnitType())
		{
			pUnit->kill(true);
			CvUnit* pNewUnit = initUnit(eNewUnitType, iX, iY);
			CvAssert(pNewUnit != NULL);
			if (pNewUnit)
				pNewUnit->finishMoves();
		}
	}
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
void CvPlayer::DoIncomingUnits()
{
	for (int iLoop = 0; iLoop < MAX_INCOMING_UNITS; iLoop++)
	{
		CvAssertMsg(GetIncomingUnitCountdown(iLoop) >= -1, "Incoming Unit countdown is an invalid value. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		if (GetIncomingUnitCountdown(iLoop) > 0)
		{
			ChangeIncomingUnitCountdown(iLoop, -1);

			// Time to spawn a new unit
			if (GetIncomingUnitCountdown(iLoop) == 0)
			{
				// Must have capital to actually spawn unit
				CvCity* pCapital = getCapitalCity();
				if (pCapital)
				{
					CvUnit* pNewUnit = initUnit(GetIncomingUnitType(iLoop), pCapital->getX(), pCapital->getY());
					CvAssert(pNewUnit);
					if (pNewUnit)
					{
						if (pNewUnit->getDomainType() != DOMAIN_AIR)
						{
							if (!pNewUnit->jumpToNearestValidPlot())
								pNewUnit->kill(false);
						}
					}
				}

				// Reset stuff
				SetIncomingUnitCountdown(iLoop, -1);
				SetIncomingUnitType(iLoop, NO_UNIT);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Someone sent us a present!
void CvPlayer::DoDistanceGift(PlayerTypes eFromPlayer, CvUnit* pUnit)
{
	if(!pUnit)
	{
		return;
	}

	CvPlot* pPlot = pUnit->plot();
	if(! pPlot)
	{
		return;
	}

	// Also add any units this guy is transporting
	IDInfo* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (NULL != pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
			AddIncomingUnit(eFromPlayer, pLoopUnit);
	}

	AddIncomingUnit(eFromPlayer, pUnit);
}

//	--------------------------------------------------------------------------------
/// Someone sent us a present!
void CvPlayer::AddIncomingUnit(PlayerTypes eFromPlayer, CvUnit* pUnit)
{
	UnitTypes eUnitType = pUnit->getUnitType();

	int iNumTurns = 3;

	// Find a slot
	for (int iLoop = 0; iLoop < MAX_INCOMING_UNITS; iLoop++)
	{
		if (GetIncomingUnitCountdown(iLoop) == -1)
		{
			SetIncomingUnitCountdown(iLoop, iNumTurns);
			SetIncomingUnitType(iLoop, eUnitType);

			break;
		}
	}

	// Minor civ friendship
	if (isMinorCiv() && eFromPlayer < MAX_MAJOR_CIVS)
	{
		if (GetMinorCivAI()->GetNumUnitsGifted(eFromPlayer) < /*3*/ GC.getMAX_INFLUENCE_FROM_MINOR_GIFTS())
		{
			GetMinorCivAI()->ChangeNumUnitsGifted(eFromPlayer, 1);

			int iFriendship = /*2*/ GC.getFRIENDSHIP_PER_UNIT_GIFTED();
			GetMinorCivAI()->ChangeFriendshipWithMajor(eFromPlayer, iFriendship);
		}
	}

	// Get rid of the old unit
	pUnit->kill(true);
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
UnitTypes CvPlayer::GetIncomingUnitType(int iIndex) const
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_INCOMING_UNITS, "iIndex is expected to be within maximum bounds (invalid Index)");
	return (UnitTypes) m_aiIncomingUnitTypes[iIndex];
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
void CvPlayer::SetIncomingUnitType(int iIndex, UnitTypes eUnitType)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_INCOMING_UNITS, "iIndex is expected to be within maximum bounds (invalid Index)");

	CvAssertMsg(eUnitType >= NO_UNIT, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitType < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (eUnitType != m_aiIncomingUnitTypes[iIndex])
	{
		m_aiIncomingUnitTypes.setAt(iIndex, eUnitType);
	}
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
int CvPlayer::GetIncomingUnitCountdown(int iIndex) const
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_INCOMING_UNITS, "iIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiIncomingUnitCountdowns[iIndex];
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
void CvPlayer::SetIncomingUnitCountdown(int iIndex, int iNumTurns)
{
	CvAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(iIndex < MAX_INCOMING_UNITS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (iNumTurns != m_aiIncomingUnitCountdowns[iIndex])
		m_aiIncomingUnitCountdowns.setAt(iIndex, iNumTurns);
}

//	--------------------------------------------------------------------------------
/// Units in the ether coming towards us?
void CvPlayer::ChangeIncomingUnitCountdown(int iIndex, int iChange)
{
	if (iChange != 0)
		SetIncomingUnitCountdown(iIndex, GetIncomingUnitCountdown(iIndex) + iChange);
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isOption(PlayerOptionTypes eID) const
{
	for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
	{
		if ((*itr).first == eID)
			return (*itr).second != 0;
	}
	return false;
}


//	--------------------------------------------------------------------------------
void CvPlayer::setOption(PlayerOptionTypes eID, bool bNewValue)
{
	int iIndex = 0;
	for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
	{
		if ((*itr).first == eID)
		{
			m_aOptions.setAt(iIndex, PlayerOptionEntry((uint)eID, bNewValue?1:0));
			return;
		}
		++iIndex;
	}

	m_aOptions.push_back(PlayerOptionEntry((uint)eID, bNewValue?1:0));
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isPlayable() const
{
	return CvPreGame::isPlayable(GetID());
}

//	--------------------------------------------------------------------------------
void CvPlayer::setPlayable(bool bNewValue)
{
	CvPreGame::setPlayable(GetID(), bNewValue);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumResourceUsed(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumResourceUsed[eIndex];
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeNumResourceUsed(ResourceTypes eIndex, int iChange)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumResourceInfos());

	if (iChange != 0)
	{
		m_paiNumResourceUsed.setAt(eIndex, m_paiNumResourceUsed[eIndex] + iChange);
	}

	if (iChange > 0)
		DoTestOverResourceNotification(eIndex);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	CvAssert(m_paiNumResourceUsed[eIndex] >= 0);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumResourceTotal(ResourceTypes eIndex, bool bIncludeImport) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iTotalNumResource = m_paiNumResourceTotal[eIndex];

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eIndex);
	if (pkResourceInfo != NULL)
	{
		// Mod applied to how much we have?
		if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
		{
			if (GetStrategicResourceMod() != 0)
			{
				iTotalNumResource *= GetStrategicResourceMod();
				iTotalNumResource /= 100;
			}
		}

		if (bIncludeImport)
		{
			iTotalNumResource += getResourceImport(eIndex);
			iTotalNumResource += getResourceFromMinors(eIndex);
		}

		iTotalNumResource -= getResourceExport(eIndex);
	}

	return iTotalNumResource;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeNumResourceTotal(ResourceTypes eIndex, int iChange, bool bIgnoreResourceWarning)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumResourceInfos());

	if (iChange != 0)
	{
		m_paiNumResourceTotal.setAt(eIndex, m_paiNumResourceTotal[eIndex] + iChange);

		// Minors with an Ally give their Resources to their friend (awww)
		if (isMinorCiv())
		{
			PlayerTypes eBestRelationsPlayer = GetMinorCivAI()->GetAlly();

			if (eBestRelationsPlayer != NO_PLAYER)
			{
				ResourceUsageTypes eUsage = GC.getResourceInfo(eIndex)->getResourceUsage();

				if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
				{
					// Someone new is getting the bonus
					if (eBestRelationsPlayer != NO_PLAYER)
					{
						GET_PLAYER(eBestRelationsPlayer).changeResourceFromMinors(eIndex, iChange);
						changeResourceExport(eIndex, iChange);

						CvNotifications* pNotifications = GET_PLAYER(eBestRelationsPlayer).GetNotifications();
						if (pNotifications)
						{
							Localization::String strMessage;
							Localization::String strSummary;

							// Adding Resources
							if (iChange > 0)
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_NEW_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_NEW_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}
							// Lost Resources
							else
							{
								strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BFF_LOST_RESOURCE");
								strMessage << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BFF_LOST_RESOURCE");
								strSummary << getNameKey() << GC.getResourceInfo(eIndex)->GetDescriptionKey();
							}

							int iX = -1;
							int iY = -1;

							CvCity* capCity = getCapitalCity();

							if (capCity != NULL)
							{
								iX = capCity->getX();
								iY = capCity->getY();
							}

							pNotifications->Add(NOTIFICATION_MINOR, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, -1);
						}
					}
				}
			}
		}
	}

	if (iChange < 0 && !bIgnoreResourceWarning)
		DoTestOverResourceNotification(eIndex);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	CvAssert(m_paiNumResourceTotal[eIndex] >= 0);
}

//	--------------------------------------------------------------------------------
/// Are we over our resource limit? If so, give out a notification
void CvPlayer::DoTestOverResourceNotification(ResourceTypes eIndex)
{
	if (getNumResourceAvailable(eIndex, true) < 0)
	{
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eIndex);
		if (pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
		{
			CvNotifications* pNotifications = GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT");
				strText << pkResourceInfo->GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_OVER_RESOURCE_LIMIT");
				strSummary << pkResourceInfo->GetTextKey();
				pNotifications->Add(NOTIFICATION_DEMAND_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), -1, -1, eIndex);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Is our collection of Strategic Resources modified?
int CvPlayer::GetStrategicResourceMod() const
{
	return m_iStrategicResourceMod;
}

//	--------------------------------------------------------------------------------
/// Is our collection of Strategic Resources modified?
void CvPlayer::ChangeStrategicResourceMod(int iChange)
{
	m_iStrategicResourceMod += iChange;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumResourceAvailable(ResourceTypes eIndex, bool bIncludeImport) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return getNumResourceTotal(eIndex, bIncludeImport) - getNumResourceUsed(eIndex);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getResourceGiftedToMinors(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceGiftedToMinors[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeResourceGiftedToMinors(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiResourceGiftedToMinors.setAt(eIndex, m_paiResourceGiftedToMinors[eIndex] + iChange);
		CvAssert(getResourceGiftedToMinors(eIndex) >= 0);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getResourceExport(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceExport[eIndex];
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeResourceExport(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiResourceExport.setAt(eIndex, m_paiResourceExport[eIndex] + iChange);
		CvAssert(getResourceExport(eIndex) >= 0);

		DoUpdateHappiness();
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getResourceImport(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiResourceImport[eIndex];
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeResourceImport(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiResourceImport.setAt(eIndex, m_paiResourceImport[eIndex] + iChange);
		CvAssert(getResourceImport(eIndex) >= 0);

		DoUpdateHappiness();
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getResourceFromMinors(ResourceTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iNumResourceFromMinors = m_paiResourceFromMinors[eIndex];

	// Resource bonus doubles quantity of Resources from Minors (Policies, etc.)
	if (IsMinorResourceBonus())
	{
		iNumResourceFromMinors *= /*200*/ GC.getMINOR_POLICY_RESOURCE_MULTIPLIER();
		iNumResourceFromMinors /= 100;
	}

	return iNumResourceFromMinors;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeResourceFromMinors(ResourceTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumResourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiResourceFromMinors.setAt(eIndex, m_paiResourceFromMinors[eIndex] + iChange);
		CvAssert(getResourceFromMinors(eIndex) >= 0);

		DoUpdateHappiness();
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getTotalImprovementsBuilt() const
{
	return m_iTotalImprovementsBuilt;
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeTotalImprovementsBuilt(int iChange)
{
	m_iTotalImprovementsBuilt = (m_iTotalImprovementsBuilt + iChange);
	CvAssert(getTotalImprovementsBuilt() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiImprovementCount[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiImprovementCount.setAt(eIndex, m_paiImprovementCount[eIndex] + iChange);
	CvAssert(getImprovementCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getFreeBuildingCount(BuildingTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFreeBuildingCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isBuildingFree(BuildingTypes eIndex)	const
{
	return (getFreeBuildingCount(eIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeFreeBuildingCount(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iOldFreeBuildingCount;
	int iLoop;

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

		m_paiFreeBuildingCount.setAt(eIndex, m_paiFreeBuildingCount[eIndex] + iChange);
		CvAssert(getFreeBuildingCount(eIndex) >= 0);

		if (iOldFreeBuildingCount == 0)
		{
			CvAssertMsg(getFreeBuildingCount(eIndex) > 0, "getFreeBuildingCount(eIndex) is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eIndex, 1);
			}
		}
		else if (getFreeBuildingCount(eIndex) == 0)
		{
			CvAssertMsg(iOldFreeBuildingCount > 0, "iOldFreeBuildingCount is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->GetCityBuildings()->SetNumFreeBuilding(eIndex, 0);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Is ePromotion a free promotion?
int CvPlayer::GetFreePromotionCount(PromotionTypes ePromotion) const
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");
	return m_paiFreePromotionCount[ePromotion];
}

//	--------------------------------------------------------------------------------
/// Is ePromotion a free promotion?
bool CvPlayer::IsFreePromotion(PromotionTypes ePromotion)	const
{
	return (GetFreePromotionCount(ePromotion) > 0);
}

//	--------------------------------------------------------------------------------
/// Is ePromotion a free promotion?
void CvPlayer::ChangeFreePromotionCount(PromotionTypes ePromotion, int iChange)
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		bool bWasFree = IsFreePromotion(ePromotion);

		m_paiFreePromotionCount.setAt(ePromotion, m_paiFreePromotionCount[ePromotion] + iChange);

		CvAssert(GetFreePromotionCount(ePromotion) >= 0);

		// This promotion is now set to be free, but wasn't before we called this function
		if (IsFreePromotion(ePromotion) && !bWasFree)
		{
			// Loop through Units
			CvUnit* pLoopUnit;

			int iLoop;
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				// Valid Promotion for this Unit?
				if (::IsPromotionValidForUnitCombatType(ePromotion, pLoopUnit->getUnitType()))
				{
					pLoopUnit->setHasPromotion(ePromotion, true);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getUnitCombatProductionModifiers(UnitCombatTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCombatProductionModifiers[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeUnitCombatProductionModifiers(UnitCombatTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiUnitCombatProductionModifiers.setAt(eIndex, m_paiUnitCombatProductionModifiers[eIndex] + iChange);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getUnitCombatFreeExperiences(UnitCombatTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCombatFreeExperiences[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeUnitCombatFreeExperiences(UnitCombatTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiUnitCombatFreeExperiences.setAt(eIndex, m_paiUnitCombatFreeExperiences[eIndex] + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getUnitClassCount(UnitClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eIndex);
	if(pkUnitClassInfo == NULL)
	{
		return false;
	}

	if (!isNationalUnitClass(eIndex))
	{
		return false;
	}

	CvAssertMsg(getUnitClassCount(eIndex) <= pkUnitClassInfo->getMaxPlayerInstances(), "getUnitClassCount is expected to be less than maximum bound of MaxPlayerInstances (invalid index)");

	return ((getUnitClassCount(eIndex) + iExtra) >= pkUnitClassInfo->getMaxPlayerInstances());
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCount.setAt(eIndex, m_paiUnitClassCount[eIndex] + iChange);
	CvAssert(getUnitClassCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getUnitClassMaking(UnitClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassMaking[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeUnitClassMaking(UnitClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiUnitClassMaking.setAt(eIndex, m_paiUnitClassMaking[eIndex] + iChange);
		CvAssert(getUnitClassMaking(eIndex) >= 0);

		CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
		UnitTypes eUnit = static_cast<UnitTypes>(playerCivilizationInfo.getCivilizationUnits(eIndex));
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			// Builder Limit
			if (pkUnitInfo->GetWorkRate() > 0 && pkUnitInfo->GetDomainType() == DOMAIN_LAND)
			{
				ChangeNumBuilders(iChange);
			}

			// Update the amount of a Resource used up by Units in Production
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if(pkResource)
				{
					if (pkUnitInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						changeNumResourceUsed(eResource, iChange * pkUnitInfo->GetResourceQuantityRequirement(iResourceLoop));
					}
				}
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Help_DIRTY_BIT, true);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}


//	--------------------------------------------------------------------------------
int CvPlayer::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvPlayer::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eIndex);
	if(pkBuildingClassInfo == NULL)
	{
		CvAssertMsg(false, "This should never happen...");
		return false;
	}

	if (!isNationalWonderClass(*pkBuildingClassInfo))
	{
		return false;
	}

	CvAssertMsg(getBuildingClassCount(eIndex) <= (pkBuildingClassInfo->getMaxPlayerInstances() + pkBuildingClassInfo->getExtraPlayerInstances()), "BuildingClassCount is expected to be less than or match the number of max player instances plus extra player instances");

	return ((getBuildingClassCount(eIndex) + iExtra) >= (pkBuildingClassInfo->getMaxPlayerInstances() + pkBuildingClassInfo->getExtraPlayerInstances()));
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCount.setAt(eIndex, m_paiBuildingClassCount[eIndex] + iChange);
	CvAssert(getBuildingClassCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getBuildingClassMaking(BuildingClassTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassMaking[eIndex];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiBuildingClassMaking.setAt(eIndex, m_paiBuildingClassMaking[eIndex] + iChange);
		CvAssert(getBuildingClassMaking(eIndex) >= 0);

		const BuildingTypes eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eIndex);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			// Update the amount of a Resource used up by Buildings in Production
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo)
				{
					if (pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						changeNumResourceUsed(eResource, iChange * pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop));
					}
				}

			}
		}


		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}


//	--------------------------------------------------------------------------------
// The following two functions are only used to keep track of how many Projects are in progress so we know what each player's Resource situation is
// Check out CvTeam::getProjectMaking() for something used more
int CvPlayer::getProjectMaking(ProjectTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectMaking[eIndex];
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeProjectMaking(ProjectTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiProjectMaking.setAt(eIndex, m_paiProjectMaking[eIndex] + iChange);
		CvAssert(getProjectMaking(eIndex) >= 0);

		// Update the amount of a Resource used up by Projects in Production
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			if (GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop) > 0)
			{
				changeNumResourceUsed((ResourceTypes) iResourceLoop, iChange * GC.getProjectInfo(eIndex)->GetResourceQuantityRequirement(iResourceLoop));
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getHurryCount(HurryTypes eIndex) const
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumHurryInfos());
	return m_paiHurryCount[eIndex];
}


//	--------------------------------------------------------------------------------
// Do we have access to this Hurry type?
bool CvPlayer::IsHasAccessToHurry(HurryTypes eIndex) const
{
	return (getHurryCount(eIndex) > 0);
}

//	--------------------------------------------------------------------------------
/// Can we use this Hurry RIGHT NOW?
bool CvPlayer::IsCanHurry(HurryTypes eIndex) const
{
	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo == NULL)
		return false;

	int iCost = GetHurryGoldCost(eIndex);

	// Can we pay for this Hurry?
	if (iCost < 0 || GetTreasury()->GetGold() < iCost)
	{
		return false;
	}

	// Science Rushing
	if (pkHurryInfo->getGoldPerBeaker() > 0)
	{
		return true;
	}

	// Culture Rushing
	if (pkHurryInfo->getGoldPerCulture() > 0)
	{
		// If we already have enough Culture for the next Policy, there's nothing to rush!
		if (getNextPolicyCost() > getJONSCulture())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// How much Gold does it cost us to Hurry? (whatever the applicable item is)
int CvPlayer::GetHurryGoldCost(HurryTypes eHurry) const
{
	int iGold = -1;

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
	{
		//This should never happen.
		return -1;
	}

	// Science Rushing
	if (pkHurryInfo->getGoldPerBeaker() > 0)
	{
		TechTypes eTech = GetPlayerTechs()->GetCurrentResearch();

		if (eTech != NO_TECH)
		{
			int iTotalCost = GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech);
			int iResearchLeft = GET_TEAM(getTeam()).GetTeamTechs()->GetResearchLeft(eTech);

			// Cost of Gold rushing based on the ORIGINAL Research price
			int iGoldForFullPrice = iTotalCost * pkHurryInfo->getGoldPerBeaker();
			iGoldForFullPrice = (int) pow((double) iGoldForFullPrice, (double) /*1.10f*/ GC.getHURRY_GOLD_TECH_EXPONENT());

			// Figure out the actual cost by comparing what's left to the original Research cost, and multiplying that by the amount to Gold rush the original cost
			iGold = (iGoldForFullPrice * iResearchLeft / iTotalCost);
		}
	}

	// Culture Rushing
	if (pkHurryInfo->getGoldPerCulture() > 0)
	{
		int iCurrentPolicyCost = getNextPolicyCost();

		if (iCurrentPolicyCost > 0)
		{
			int iCultureLeft = iCurrentPolicyCost - getJONSCulture();

			// Cost of Gold rushing based on the ORIGINAL Culture price
			int iGoldForFullPrice = iCurrentPolicyCost * pkHurryInfo->getGoldPerCulture();
			iGoldForFullPrice = (int) pow((double) iGoldForFullPrice, (double) /*1.10f*/ GC.getHURRY_GOLD_CULTURE_EXPONENT());

			// Figure out the actual cost by comparing what's left to the original Culture cost, and multiplying that by the amount to Gold rush the original cost
			iGold = (iGoldForFullPrice * iCultureLeft / iCurrentPolicyCost);
		}
	}

	return iGold;
}

//	--------------------------------------------------------------------------------
/// Hurry something!
void CvPlayer::DoHurry(HurryTypes eIndex)
{
	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo)
	{
		if (IsCanHurry(eIndex))
		{
			int iGoldCost = GetHurryGoldCost(eIndex);
			GetTreasury()->ChangeGold(-iGoldCost);

			// Science Rushing
			if (pkHurryInfo->getGoldPerBeaker() > 0)
			{
				TechTypes eTech = GetPlayerTechs()->GetCurrentResearch();

				GET_TEAM(getTeam()).setHasTech(eTech, true, GetID(), false, false);
			}

			// Culture Rushing
			if (pkHurryInfo->getGoldPerCulture() > 0)
			{
				setJONSCulture(getNextPolicyCost());
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlayer::canPopRush()
{
	return (m_iPopRushHurryCount > 0);
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)
{
	CvAssert(eIndex >= 0);
	CvAssert(eIndex < GC.getNumHurryInfos());

	int oldHurryCount = m_paiHurryCount[eIndex];
	m_paiHurryCount.setAt(eIndex, m_paiHurryCount[eIndex] + iChange);
	CvAssert(getHurryCount(eIndex) >= 0);

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eIndex);
	if(pkHurryInfo == NULL)
		return;

	// if we just went from 0 to 1 (or the reverse)
	if ((oldHurryCount > 0) != (m_paiHurryCount[eIndex] > 0))
	{
		// does this hurry reduce population?
		if (pkHurryInfo->getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			CvAssert(m_iPopRushHurryCount >= 0);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getHurryModifier(HurryTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumHurryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHurryModifier[eIndex];
}

//	--------------------------------------------------------------------------------
void CvPlayer::changeHurryModifier(HurryTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
		CvAssertMsg(eIndex < GC.getNumHurryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
		m_paiHurryModifier.setAt(eIndex, m_paiHurryModifier[eIndex] + iChange);
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::setResearchingTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (m_pPlayerTechs->IsResearchingTech(eIndex) != bNewValue)
	{
		GetPlayerTechs()->SetResearchingTech(eIndex, bNewValue);

		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setDirty(Popup_DIRTY_BIT, true); // to check whether we still need the tech chooser popup
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");
	return m_ppaaiSpecialistExtraYield[eIndex1][eIndex2];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	CvAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiSpecialistExtraYield[eIndex1];
		yields[eIndex2] = (m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] + iChange);
		m_ppaaiSpecialistExtraYield.setAt(eIndex1, yields);
		CvAssert(getSpecialistExtraYield(eIndex1, eIndex2) >= 0);

		updateExtraSpecialistYield();
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


//	--------------------------------------------------------------------------------
void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	CvAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		CvAssertMsg(iChange > -50 && iChange < 50, "GAMEPLAY: Yield for a plot is either negative or a ridiculously large number. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		Firaxis::Array<int, NUM_YIELD_TYPES> yields = m_ppaaiImprovementYieldChange[eIndex1];
		yields[eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		m_ppaaiImprovementYieldChange.setAt(eIndex1, yields);
		CvAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

//	---------------------------------------------------------------------------
bool CvPlayer::removeFromArmy(int iArmyID, int iID)
{
	bool bRemoved = false;
	CvArmyAI* pThisArmyAI = getArmyAI(iArmyID);
	if (pThisArmyAI)
	{
		bRemoved = pThisArmyAI->RemoveUnit(iID);
	}

	return bRemoved;
}

//	---------------------------------------------------------------------------
bool CvPlayer::removeFromArmy(int iID)
{
	CvArmyAI* pLoopArmyAI;
	int iLoop;
	bool bRemoved = false;

	// for all the army AIs
	for (pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL && !bRemoved; pLoopArmyAI = nextArmyAI(&iLoop))
	{
		// attempt to remove from this army
		bRemoved = removeFromArmy (pLoopArmyAI->GetID(), iID);
	}
	return bRemoved;
}


//	---------------------------------------------------------------------------
//	Finds the path length from this tech type to one you already know
int CvPlayer::findPathLength(TechTypes eTech, bool bCost) const
{
	int i;
	int iNumSteps = 0;
	int iShortestPath = 0;
	int iPathLength = 0;
	TechTypes ePreReq;
	TechTypes eShortestOr;

	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return 0;

	if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech) || m_pPlayerTechs->IsResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		//	Base case return 0, we know it...
		return 0;
	}

	//	Cycle through the and paths and add up their tech lengths
	for (i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqAndTechs(i);

		if (ePreReq != NO_TECH)
		{
			iPathLength += findPathLength(ePreReq, bCost);
		}
	}

	eShortestOr = NO_TECH;
	iShortestPath = INT_MAX;
	//	Find the shortest OR tech
	for (i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		//	Grab the tech
		ePreReq = (TechTypes)pkTechInfo->GetPrereqOrTechs(i);

		//	If this is a valid tech
		if (ePreReq != NO_TECH)
		{
			//	Recursively find the path length (takes into account all ANDs)
			iNumSteps = findPathLength(ePreReq, bCost);

			//	If the prereq is a valid tech and its the current shortest, mark it as such
			if (iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}

	//	If the shortest OR is a valid tech, add the steps to it...
	if (eShortestOr != NO_TECH)
	{
		iPathLength += iShortestPath;
	}

	return (iPathLength + ((bCost) ? GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech) : 1));
}


//	--------------------------------------------------------------------------------
//	Function specifically for python/tech chooser screen
int CvPlayer::getQueuePosition(TechTypes eTech) const
{
	int i = 1;
	const CLLNode<TechTypes>* pResearchNode;

	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			return i;
		}
		i++;
	}

	return -1;
}


//	--------------------------------------------------------------------------------
void CvPlayer::clearResearchQueue()
{
	int iI;

	m_researchQueue.clear();

	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		setResearchingTech(((TechTypes)iI), false);
	}

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
//	Pushes research onto the queue.  If it is an append if will put it
//	and its pre-reqs into the queue.  If it is not an append it will change
//	research immediately and should be used with clear.  Clear will clear the entire queue.
bool CvPlayer::pushResearch(TechTypes eTech, bool bClear)
{
	int i;
	int iNumSteps;
	int iShortestPath;
	bool bOrPrereqFound;
	TechTypes ePreReq;
	TechTypes eShortestOr;

	CvAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return false;


	if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech) || m_pPlayerTechs->IsResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		return true;
	}

	if (!GetPlayerTechs()->CanEverResearch(eTech))
	{
		return false;
	}

	//	Pop the entire queue...
	if (bClear)
	{
		clearResearchQueue();
	}

	//	Add in all the pre-reqs for the and techs...
	for (i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqAndTechs(i);

		if (ePreReq != NO_TECH)
		{
			if (!pushResearch(ePreReq))
			{
				return false;
			}
		}
	}

	// Will return the shortest path of all the or techs.  Tie breaker goes to the first one...
	eShortestOr = NO_TECH;
	iShortestPath = INT_MAX;
	bOrPrereqFound = false;
	//	Cycle through all the OR techs
	for (i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)pkTechInfo->GetPrereqOrTechs(i);

		if (ePreReq != NO_TECH)
		{
			bOrPrereqFound = true;

			//	If the pre-req exists, and we have it, it is the shortest path, get out, we're done
			if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech(ePreReq))
			{
				eShortestOr = ePreReq;
				break;
			}

			if (GetPlayerTechs()->CanEverResearch(ePreReq))
			{
				//	Find the length of the path to this pre-req
				iNumSteps = findPathLength(ePreReq);

				//	If this pre-req is a valid tech, and its the shortest current path, set it as such
				if (iNumSteps < iShortestPath)
				{
					eShortestOr = ePreReq;
					iShortestPath = iNumSteps;
				}
			}
		}
	}

	//	If the shortest path tech is valid, push it (and its children) on to the research queue recursively
	if (eShortestOr != NO_TECH)
	{
		if (!pushResearch(eShortestOr))
		{
			return false;
		}
	}
	else if (bOrPrereqFound)
	{
		return false;
	}

	//	Insert this tech at the end of the queue
	m_researchQueue.insertAtEnd(eTech);

	setResearchingTech(eTech, true);

	//	Set the dirty bits
	if (getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvPlayer::popResearch(TechTypes eTech)
{
	CLLNode<TechTypes>* pResearchNode;

	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			m_researchQueue.deleteNode(pResearchNode);
			break;
		}
	}

	setResearchingTech(eTech, false);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
int CvPlayer::getLengthResearchQueue() const
{
	return m_researchQueue.getLength();
}


//	--------------------------------------------------------------------------------
CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(CLLNode<TechTypes>* pNode)
{
	return m_researchQueue.next(pNode);
}

//	--------------------------------------------------------------------------------
const CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(const CLLNode<TechTypes>* pNode) const
{
	return m_researchQueue.next(pNode);
}

//	--------------------------------------------------------------------------------
CLLNode<TechTypes>* CvPlayer::headResearchQueueNode()
{
	return m_researchQueue.head();
}

//	--------------------------------------------------------------------------------
const CLLNode<TechTypes>* CvPlayer::headResearchQueueNode() const
{
	return m_researchQueue.head();
}

//	--------------------------------------------------------------------------------
CLLNode<TechTypes>* CvPlayer::tailResearchQueueNode()
{
	return m_researchQueue.tail();
}


//	--------------------------------------------------------------------------------
void CvPlayer::addCityName(const CvString& szName)
{
	m_cityNames.insertAtEnd(szName);
}


//	--------------------------------------------------------------------------------
int CvPlayer::getNumCityNames() const
{
	return m_cityNames.getLength();
}


//	--------------------------------------------------------------------------------
CvString CvPlayer::getCityName(int iIndex) const
{
	CLLNode<CvString>* pCityNameNode;

	pCityNameNode = m_cityNames.nodeNum(iIndex);

	if (pCityNameNode != NULL)
	{
		return pCityNameNode->m_data;
	}
	else
	{
		return "";
	}
}

//	--------------------------------------------------------------------------------
CLLNode<CvString>* CvPlayer::nextCityNameNode(CLLNode<CvString>* pNode)
{
	return m_cityNames.next(pNode);
}

//	--------------------------------------------------------------------------------
const CLLNode<CvString>* CvPlayer::nextCityNameNode(const CLLNode<CvString>* pNode) const
{
	return m_cityNames.next(pNode);
}

//	--------------------------------------------------------------------------------
CLLNode<CvString>* CvPlayer::headCityNameNode()
{
	return m_cityNames.head();
}

//	--------------------------------------------------------------------------------
const CLLNode<CvString>* CvPlayer::headCityNameNode() const
{
	return m_cityNames.head();
}

//	--------------------------------------------------------------------------------
CvCity* CvPlayer::firstCity(int *pIterIdx, bool bRev)
{
	return !bRev ? m_cities.BeginIter(pIterIdx) : m_cities.EndIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
const CvCity* CvPlayer::firstCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.BeginIter(pIterIdx) : m_cities.EndIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
CvCity* CvPlayer::nextCity(int *pIterIdx, bool bRev)
{
	return !bRev ? m_cities.NextIter(pIterIdx) : m_cities.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
const CvCity* CvPlayer::nextCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.NextIter(pIterIdx) : m_cities.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumCities() const
{
	return m_cities.GetCount();
}


//	--------------------------------------------------------------------------------
CvCity* CvPlayer::getCity(int iID)
{
	return(m_cities.GetAt(iID));
}

//	--------------------------------------------------------------------------------
const CvCity* CvPlayer::getCity(int iID) const
{
	return(m_cities.GetAt(iID));
}


//	--------------------------------------------------------------------------------
CvCity* CvPlayer::addCity()
{
	return(m_cities.Add());
}

//	--------------------------------------------------------------------------------
void CvPlayer::deleteCity(int iID)
{
	m_cities.RemoveAt(iID);
}

//	--------------------------------------------------------------------------------
const CvUnit* CvPlayer::firstUnit(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_units.BeginIter(pIterIdx) : m_units.EndIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
const CvUnit* CvPlayer::nextUnit(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_units.NextIter(pIterIdx) : m_units.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlayer::firstUnit(int *pIterIdx, bool bRev)
{
	return !bRev ? m_units.BeginIter(pIterIdx) : m_units.EndIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlayer::nextUnit(int *pIterIdx, bool bRev)
{
	return !bRev ? m_units.NextIter(pIterIdx) : m_units.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumUnits() const
{
	return m_units.GetCount();
}


//	--------------------------------------------------------------------------------
const CvUnit * CvPlayer::getUnit(int iID) const
{
	return (m_units.GetAt(iID));
}

//	--------------------------------------------------------------------------------
CvUnit * CvPlayer::getUnit(int iID)
{
	return (m_units.GetAt(iID));
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlayer::addUnit()
{
	return (m_units.Add());
}


//	--------------------------------------------------------------------------------
void CvPlayer::deleteUnit(int iID)
{
	m_units.RemoveAt(iID);
}


//	--------------------------------------------------------------------------------
const CvArmyAI* CvPlayer::firstArmyAI(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_armyAIs.BeginIter(pIterIdx) : m_armyAIs.EndIter(pIterIdx);
}


//	--------------------------------------------------------------------------------
const CvArmyAI* CvPlayer::nextArmyAI(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_armyAIs.NextIter(pIterIdx) : m_armyAIs.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
CvArmyAI* CvPlayer::firstArmyAI(int *pIterIdx, bool bRev)
{
	return !bRev ? m_armyAIs.BeginIter(pIterIdx) : m_armyAIs.EndIter(pIterIdx);
}


//	--------------------------------------------------------------------------------
CvArmyAI* CvPlayer::nextArmyAI(int *pIterIdx, bool bRev)
{
	return !bRev ? m_armyAIs.NextIter(pIterIdx) : m_armyAIs.PrevIter(pIterIdx);
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNumArmyAIs() const
{
	return m_armyAIs.GetCount();
}


//	--------------------------------------------------------------------------------
const CvArmyAI* CvPlayer::getArmyAI(int iID) const
{
	return ((CvArmyAI *)(m_armyAIs.GetAt(iID)));
}

//	--------------------------------------------------------------------------------
CvArmyAI* CvPlayer::getArmyAI(int iID)
{
	return ((CvArmyAI *)(m_armyAIs.GetAt(iID)));
}


//	--------------------------------------------------------------------------------
CvArmyAI* CvPlayer::addArmyAI()
{
	return ((CvArmyAI *)(m_armyAIs.Add()));
}


//	--------------------------------------------------------------------------------
void CvPlayer::deleteArmyAI(int iID)
{
	bool bRemoved = m_armyAIs.RemoveAt(iID);

	DEBUG_VARIABLE(bRemoved);
	CvAssertMsg(bRemoved, "could not find army, delete failed");
}


//	--------------------------------------------------------------------------------
const CvAIOperation* CvPlayer::getAIOperation(int iID) const
{
	std::map<int, CvAIOperation*>::const_iterator it = m_AIOperations.find(iID);
	if (it != m_AIOperations.end())
	{
		return it->second;
	}
	return 0;
}

//	--------------------------------------------------------------------------------
CvAIOperation* CvPlayer::getFirstAIOperation()
{
	CvAIOperation *rtnValue = NULL;

	m_CurrentOperation = m_AIOperations.begin();
	if (m_CurrentOperation != m_AIOperations.end())
	{
		rtnValue = m_CurrentOperation->second;
	}
	return rtnValue;
}

//	--------------------------------------------------------------------------------
CvAIOperation* CvPlayer::getNextAIOperation()
{
	CvAIOperation *rtnValue = NULL;

	if (m_CurrentOperation != m_AIOperations.end())
	{
		++m_CurrentOperation;
		if (m_CurrentOperation != m_AIOperations.end())
		{
			rtnValue = m_CurrentOperation->second;
		}
	}
	return rtnValue;
}

//	--------------------------------------------------------------------------------
CvAIOperation* CvPlayer::getAIOperation(int iID)
{
	std::map<int, CvAIOperation*>::iterator it = m_AIOperations.find(iID);
	if (it != m_AIOperations.end())
	{
		return it->second;
	}
	return 0;
}


//	--------------------------------------------------------------------------------
CvAIOperation* CvPlayer::addAIOperation(int OperationType, PlayerTypes eEnemy, int iArea, CvCity *pTarget, CvCity *pMuster)
{
	CvAIOperation* pNewOperation = CvAIOperation::CreateOperation((AIOperationTypes) OperationType, m_eID);
	if (pNewOperation)
	{
		m_AIOperations.insert( std::make_pair(m_iNextOperationID.get(), pNewOperation) );
		pNewOperation->Init(m_iNextOperationID, m_eID, eEnemy, iArea, pTarget, pMuster);
		m_iNextOperationID++;
	}
	return pNewOperation;
}


//	--------------------------------------------------------------------------------
void CvPlayer::deleteAIOperation(int iID)
{
	std::map<int, CvAIOperation*>::iterator it = m_AIOperations.find(iID);
	if (it != m_AIOperations.end())
	{
		delete(it->second);
		m_AIOperations.erase(it);
	}
	else
	{
		CvAssertMsg(false, "could not find operation, delete failed");
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::haveAIOperationOfType(int iOperationType, int *piID /* optional return argument */, PlayerTypes eTargetPlayer /* optional additional match criteria */, CvPlot *pTarget /* optional additional match criteria */)
{
	// loop through all entries looking for match
	std::map<int , CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation->GetOperationType() == iOperationType)
		{
			if (eTargetPlayer == NO_PLAYER || eTargetPlayer == pThisOperation->GetEnemy())
			{
				if (pTarget == NULL || pTarget == pThisOperation->GetTargetPlot())
				{
					// Fill in optional parameter (ID) if passed in
					if (piID != NULL)
					{
						*piID = pThisOperation->GetID();
					}
					return true;
				}
			}
		}
	}
	// Fill in optional parameter (ID) if passed in
	if (piID != NULL)
	{
		*piID = -1;
	}
	return false;
}

//	--------------------------------------------------------------------------------
int CvPlayer::numOperationsOfType(int iOperationType)
{
	int iRtnValue = 0;

	std::map<int , CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation->GetOperationType() == iOperationType)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
/// Is an existing operation already going after this city?
bool CvPlayer::IsCityAlreadyTargeted(CvCity *pCity, DomainTypes eDomain, int iPercentToTarget, int iIgnoreOperationID) const
{
	CvAIOperation *pOperation;
	std::map<int , CvAIOperation*>::const_iterator iter;

	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		pOperation = iter->second;

		if (pOperation)
		{
			if (iIgnoreOperationID == -1 || iIgnoreOperationID != pOperation->GetID())
			{
				if (pOperation->GetTargetPlot() == pCity->plot() && pOperation->PercentFromMusterPointToTarget() < iPercentToTarget)
				{
					// Naval attacks are mixed land/naval operations
					if ((eDomain == NO_DOMAIN || eDomain == DOMAIN_SEA) && pOperation->IsMixedLandNavalOperation())
					{
						return true;
					}

					if ((eDomain == NO_DOMAIN || eDomain == DOMAIN_LAND) && !pOperation->IsMixedLandNavalOperation())
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
void CvPlayer::addPopup(CvPopupInfo* pInfo, bool bFront)
{
	if (isHuman())
	{
		if (bFront)
		{
			m_listPopups.push_front(pInfo);
		}
		else
		{
			m_listPopups.push_back(pInfo);
		}
	}
	else
	{
		SAFE_DELETE(pInfo);
	}
}


//	--------------------------------------------------------------------------------
void CvPlayer::clearPopups()
{
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
	{
		CvPopupInfo* pInfo = *it;
		if (NULL != pInfo)
		{
			delete pInfo;
		}
	}
	m_listPopups.clear();
}


//	--------------------------------------------------------------------------------
CvPopupInfo* CvPlayer::popFrontPopup()
{
	CvPopupInfo* pInfo = NULL;
	if (!m_listPopups.empty())
	{
		pInfo = m_listPopups.front();
		m_listPopups.pop_front();
	}
	return pInfo;
}


//	--------------------------------------------------------------------------------
const CvPopupQueue& CvPlayer::getPopups() const
{
	return (m_listPopups);
}

//	--------------------------------------------------------------------------------
unsigned int CvPlayer::getNumReplayDataSets() const
{
	return m_ReplayDataSets.size();
}

//	--------------------------------------------------------------------------------
const char* CvPlayer::getReplayDataSetName(unsigned int idx) const
{
	if(idx < m_ReplayDataSets.size())
		return m_ReplayDataSets[idx];

	return NULL;
}

//	--------------------------------------------------------------------------------
unsigned int CvPlayer::getReplayDataSetIndex(const char* szDataSetName)
{
	CvString dataSetName = szDataSetName;

	unsigned int idx = 0;
	for(std::vector<CvString>::iterator it = m_ReplayDataSets.begin(); it != m_ReplayDataSets.end(); ++it)
	{
		if( (*it) == dataSetName)
			return idx;

		idx++;
	}

	m_ReplayDataSets.push_back(dataSetName);
	m_ReplayDataSetValues.push_back(TurnData());
	return m_ReplayDataSets.size() - 1;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn) const
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		const TurnData& dataSet = m_ReplayDataSetValues[uiDataSet];
		TurnData::const_iterator it = dataSet.find(uiTurn);
		if(it != dataSet.end())
		{
			return (*it).second;
		}
	}

	return -1;
}
//	--------------------------------------------------------------------------------
void CvPlayer::setReplayDataValue(unsigned int uiDataSet, unsigned int uiTurn, int iValue)
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		TurnData& dataSet = m_ReplayDataSetValues[uiDataSet];
		dataSet[uiTurn] = iValue;
	}
}

//	--------------------------------------------------------------------------------
CvPlayer::TurnData CvPlayer::getReplayDataHistory(unsigned int uiDataSet) const
{
	if(uiDataSet < m_ReplayDataSetValues.size())
	{
		return m_ReplayDataSetValues[uiDataSet];
	}

	return CvPlayer::TurnData();
}

//	--------------------------------------------------------------------------------
std::string CvPlayer::getScriptData() const
{
	return m_strScriptData;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setScriptData(std::string strNewValue)
{
	m_strScriptData = strNewValue;
}

//	--------------------------------------------------------------------------------
const CvString& CvPlayer::getPbemEmailAddress() const
{
	return CvPreGame::emailAddress(GetID());
}

//	--------------------------------------------------------------------------------
void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	CvPreGame::setEmailAddress(GetID(), szAddress);
}

// Protected Functions...

//	--------------------------------------------------------------------------------
void CvPlayer::doResearch()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
	{
		return;
	}

	bool bForceResearchChoice;
	int iOverflowResearch;

	if (GetPlayerTechs()->IsResearch())
	{
		bForceResearchChoice = false;

		// Force player to pick Research if he doesn't have anything assigned
		if (GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
		{
			if (GetID() == GC.getGame().getActivePlayer() && GetScienceTimes100() > 0)
			{
				chooseTech();
			}

			if (GC.getGame().getElapsedGameTurns() > 4)
			{
				AI_chooseResearch();

				bForceResearchChoice = true;
			}
		}

		TechTypes eCurrentTech = GetPlayerTechs()->GetCurrentResearch();
		if (eCurrentTech == NO_TECH)
		{
			int iOverflow = (GetScienceTimes100()) / std::max(1, calculateResearchModifier(eCurrentTech));
			changeOverflowResearchTimes100(iOverflow);
		}
		else
		{
			iOverflowResearch = (getOverflowResearchTimes100() * calculateResearchModifier(eCurrentTech)) / 100;
			setOverflowResearch(0);
			if (GET_TEAM(getTeam()).GetTeamTechs())
				GET_TEAM(getTeam()).GetTeamTechs()->ChangeResearchProgressTimes100(eCurrentTech, (GetScienceTimes100() + iOverflowResearch), GetID());
		}

		if (bForceResearchChoice)
		{
			clearResearchQueue();
		}
	}
	GetPlayerTechs()->CheckForTechAchievement();

}

//	--------------------------------------------------------------------------------
void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (0 == getNumCities())
	{
		switch (eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:
			//Try to build this player's empire
			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setBusy(true);
			}
			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:
			// The first action must be to place a city
			// so players can lose by spending everything
			return;
		}
	}

	switch (eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		GetTreasury()->ChangeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if (GC.getGame().getActivePlayer() == GetID())
		{
			GC.GetEngineUserInterface()->setInAdvancedStart(false);
		}

		if (isHuman())
		{
			int iLoop;
			for (CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
			{
				pCity->chooseProduction();
			}

			chooseTech();
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setBusy(true);
		}
		if (GetID() == GC.getGame().getActivePlayer())
		{
			GC.GetEngineUserInterface()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
		{
			if(pPlot == NULL)
				return;

			UnitTypes eUnit = (UnitTypes) iData;
			int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add unit to the map
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					CvUnit* pUnit = initUnit(eUnit, iX, iY);
					if (NULL != pUnit)
					{
						pUnit->finishMoves();
						changeAdvancedStartPoints(-iCost);
					}
				}
			}

			// Remove unit from the map
			else
			{
				// If cost is -1 we already know this unit isn't present
				if (iCost != -1)
				{
					IDInfo* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (NULL != pLoopUnit && pLoopUnit->getUnitType() == eUnit)
						{
							pLoopUnit->kill(false);
							changeAdvancedStartPoints(iCost);
							return;
						}
					}
				}

				// Proper unit not found above, delete first found
				IDInfo* pUnitNode = pPlot->headUnitNode();
				if (pUnitNode != NULL)
				{
					CvUnit* pUnit = ::getUnit(*pUnitNode);

					iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false);
					CvAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
					pUnit->kill(false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_CITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartCityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add City to the map
			if (bAdd)
			{
				if (0 == getNumCities())
				{
					PlayerTypes eClosestPlayer = NO_PLAYER;
					int iMinDistance = INT_MAX;
					for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive())
						{
							if (kPlayer.getTeam() == getTeam())
							{
								if (0 == kPlayer.getNumCities())
								{
									CvAssert(kPlayer.getStartingPlot() != NULL);
									int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX(), kPlayer.getStartingPlot()->getY());
									if (iDistance < iMinDistance)
									{
										eClosestPlayer = kPlayer.GetID();
										iMinDistance = iDistance;
									}
								}
							}
						}
					}
					CvAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
					if (eClosestPlayer != GetID())
					{
						CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
						GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot());
						setStartingPlot(pTempPlot);
					}
				}
				if (getAdvancedStartPoints() >= iCost || 0 == getNumCities())
				{
					found(iX, iY);
					changeAdvancedStartPoints(-iCost);
					CvCity* pCity = pPlot->getPlotCity();
					if (pCity != NULL)
					{
						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
						}
					}
				}
			}

			// Remove City from the map
			else
			{
				pPlot->setRouteType(NO_ROUTE);
				pPlot->getPlotCity()->kill();
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_POP:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartPopCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Pop to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->changePopulation(1);
						changeAdvancedStartPoints(-iCost);
						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Pop from the city
				else
				{
					pCity->changePopulation(-1);
					changeAdvancedStartPoints(iCost);
					if (pCity->getPopulation() == 1)
					{
						pCity->setFood(0);
						pCity->setFoodKept(0);
					}
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_BUILDING:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				BuildingTypes eBuilding = (BuildingTypes) iData;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo == NULL)
				{
					return;
				}

				int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);
				if (iCost < 0)
				{
					return;
				}

				// Add Building to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding)+1);
						changeAdvancedStartPoints(-iCost);
						if (pkBuildingInfo->GetFoodKept() != 0)
						{
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Building from the map
				else
				{
					pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding)-1);
					changeAdvancedStartPoints(iCost);
					if (pkBuildingInfo->GetFoodKept() != 0)
					{
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_ROUTE:
		{
			if(pPlot == NULL)
				return;

			RouteTypes eRoute = (RouteTypes) iData;
			int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Route to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRouteType(eRoute);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Route from the Plot
			else
			{
				if (pPlot->getRouteType() != eRoute)
				{
					eRoute = pPlot->getRouteType();
					iCost = getAdvancedStartRouteCost(eRoute, bAdd);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setRouteType(NO_ROUTE);
				changeAdvancedStartPoints(iCost);
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
		{
			if(pPlot == NULL)
				return;

			ImprovementTypes eImprovement = (ImprovementTypes) iData;
			int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Improvement to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					if (pPlot->getFeatureType() != NO_FEATURE)
					{
						for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
						{
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
							if (!pkBuildInfo)
							{
								continue;
							}

							ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

							if (eImprovement == eLoopImprovement)
							{
								if (pkBuildInfo->isFeatureRemove(pPlot->getFeatureType()) && canBuild(pPlot, (BuildTypes)iI))
								{
									pPlot->setFeatureType(NO_FEATURE);
									break;
								}
							}
						}
					}

					pPlot->setImprovementType(eImprovement);

					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Improvement from the Plot
			else
			{
				if (pPlot->getImprovementType() != eImprovement)
				{
					eImprovement = pPlot->getImprovementType();
					iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_TECH:
		{
			TechTypes eTech = (TechTypes) iData;
			int iCost = getAdvancedStartTechCost(eTech, bAdd);

			if (iCost < 0)
			{
				return;
			}

			// Add Tech to team
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					GET_TEAM(getTeam()).setHasTech(eTech, true, GetID(), false, false);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Tech from the Team
			else
			{
				GET_TEAM(getTeam()).setHasTech(eTech, false, GetID(), false, false);
				changeAdvancedStartPoints(iCost);
			}

			if (GetID() == GC.getGame().getActivePlayer())
			{
				GC.GetEngineUserInterface()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_VISIBILITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartVisibilityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add Visibility to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRevealed(getTeam(), true, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Visibility from the Plot
			else
			{
				pPlot->setRevealed(getTeam(), false, true);
				changeAdvancedStartPoints(iCost);
			}
		}
		break;
	default:
		CvAssert(false);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a unit
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return -1;
	}

	int iCost = (getProductionNeeded(eUnit) * pkUnitInfo->GetAdvancedStartCost()) / 100;
	if (iCost < 0)
	{
		return -1;
	}

	if (NULL == pPlot)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canTrain(eUnit))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	else
	{
		CvCity* pCity = NULL;

		if (0 == GC.getADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES())
		{
			pCity = pPlot->getPlotCity();

			if (NULL == pCity || pCity->getOwner() != GetID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + pCity->getProductionModifier(eUnit));
		}
		else
		{
			if (pPlot->getOwner() != GetID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + getProductionModifier(eUnit));
		}


		if (bAdd)
		{
			int iMaxUnitsPerCity = GC.getADVANCED_START_MAX_UNITS_PER_CITY();
			if (iMaxUnitsPerCity >= 0)
			{
				if (pkUnitInfo->IsMilitarySupport() && getNumMilitaryUnits() >= iMaxUnitsPerCity * getNumCities())
				{
					return -1;
				}
			}

			if (NULL != pCity)
			{
				if (!pCity->canTrain(eUnit))
				{
					return -1;
				}
			}
			else
			{
				if (!pPlot->canTrain(eUnit, false, false))
				{
					return -1;
				}

				if (pPlot->isImpassable())
				{
					return -1;
				}
			}
		}
		// Must be this unit at plot in order to remove
		else
		{
			bool bUnitFound = false;

			IDInfo* pUnitNode = pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (NULL != pLoopUnit && pLoopUnit->getUnitType() == eUnit)
				{
					bUnitFound = true;
				}
			}

			if (!bUnitFound)
			{
				return -1;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a City
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot)
{
	int iNumCities = getNumCities();

	int iCost = getNewCityProductionValue();

	if (iCost < 0)
	{
		return -1;
	}

	// Valid plot?
	if (pPlot != NULL)
	{
		// Need valid plot to found on if adding
		if (bAdd)
		{
			if (!canFound(pPlot->getX(), pPlot->getY(), false))
			{
				return -1;
			}
		}
		// Need your own city present to remove
		else
		{
			if (pPlot->isCity())
			{
				if (pPlot->getPlotCity()->getOwner() != GetID())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}

		// Is there a distance limit on how far a city can be placed from a player's start/another city?
		if (GC.getADVANCED_START_CITY_PLACEMENT_MAX_RANGE() > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = INT_MAX;

			for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kPlayer.isAlive())
				{
					CvPlot* pStartingPlot = kPlayer.getStartingPlot();

					if (NULL != pStartingPlot)
					{
						int iDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY());
						if (iDistance <= GC.getADVANCED_START_CITY_PLACEMENT_MAX_RANGE())
						{
							if (iDistance < iClosestDistance || (iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
							{
								iClosestDistance = iDistance;
								eClosestPlayer = kPlayer.GetID();
							}
						}
					}
				}
			}

			if (NO_PLAYER == eClosestPlayer || GET_PLAYER(eClosestPlayer).getTeam() != getTeam())
			{
				return -1;
			}
			//Only allow founding a city at someone elses start point if
			//We have no cities and they have no cities.
			if ((GetID() != eClosestPlayer) && ((getNumCities() > 0) || (GET_PLAYER(eClosestPlayer).getNumCities() > 0)))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getADVANCED_START_CITY_COST_INCREASE())
	{
		if (!bAdd)
		{
			--iNumCities;
		}

		if (iNumCities > 0)
		{
			iCost *= 100 + GC.getADVANCED_START_CITY_COST_INCREASE() * iNumCities;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Population
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartPopCost(bool bAdd, CvCity* pCity)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getGrowthThreshold(1) * GC.getADVANCED_START_POPULATION_COST()) / 100;

	if (NULL != pCity)
	{
		if (pCity->getOwner() != GetID())
		{
			return -1;
		}

		int iPopulation = pCity->getPopulation();

		// Need to have Population to remove it
		if (!bAdd)
		{
			--iPopulation;

			if (iPopulation < GC.getINITIAL_CITY_POPULATION() + GC.getGame().getStartEraInfo().getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getADVANCED_START_POPULATION_COST()) / 100;

		// Increase cost if the XML defines that additional Pop will cost more
		if (0 != GC.getADVANCED_START_POPULATION_COST_INCREASE())
		{
			--iPopulation;

			if (iPopulation > 0)
			{
				iCost *= 100 + GC.getADVANCED_START_POPULATION_COST_INCREASE() * iPopulation;
				iCost /= 100;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a Building from a city
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return -1;
	}

	int iCost = getProductionNeeded(eBuilding);

	if (iCost < 0)
	{
		return -1;
	}

	if (pkBuildingInfo->GetFreeStartEra() != NO_ERA && GC.getGame().getStartEra() >=  pkBuildingInfo->GetFreeStartEra())
	{
		// you get this building for free
		return -1;
	}

	if (NULL == pCity)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canConstruct(eBuilding))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	if (NULL != pCity)
	{
		if (pCity->getOwner() != GetID())
		{
			return -1;
		}

		iCost *= 100;
		iCost /= std::max(1, 100 + pCity->getProductionModifier(eBuilding));

		if (bAdd)
		{
			if (!pCity->canConstruct(eBuilding, true, false, false))
			{
				return -1;
			}
		}
		else
		{
			if (pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) <= 0)
			{
				return -1;
			}

			// Check other buildings in this city and make sure none of them require this one

			// Loop through Buildings to see which are present
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				const BuildingTypes eBuildingLoop = static_cast<BuildingTypes>(iBuildingLoop);
				CvBuildingEntry* pkBuildingLoopInfo = GC.getBuildingInfo(eBuildingLoop);
				if(pkBuildingLoopInfo)
				{
					if (pCity->GetCityBuildings()->GetNumBuilding(eBuildingLoop) > 0)
					{
						// Loop through present Building's requirements
						for (int iBuildingClassPrereqLoop = 0; iBuildingClassPrereqLoop < GC.getNumBuildingClassInfos(); iBuildingClassPrereqLoop++)
						{
							const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassPrereqLoop);
							CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
							if(pkBuildingClassInfo)
							{
								if (pkBuildingLoopInfo->IsBuildingClassNeededInCity(iBuildingClassPrereqLoop))
								{
									if ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iBuildingClassPrereqLoop)) == eBuilding)
									{
										return -1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Route
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	if (eRoute == NO_ROUTE)
	{
		return -1;
	}

	CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
	if(pkRouteInfo == NULL)
	{
		return -1;
	}

	int iCost = pkRouteInfo->getAdvancedStartCost();

	// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGame().getGameSpeedInfo().getBuildPercent();
	iCost /= 100;

	// No invalid plots!
	if (pPlot != NULL)
	{
		if (pPlot->isCity())
		{
			return -1;
		}

		if (bAdd)
		{
			if (pPlot->isImpassable() || pPlot->isWater())
			{
				return -1;
			}
			// Can't place twice
			if (pPlot->getRouteType() == eRoute)
			{
				return -1;
			}
		}
		else
		{
			// Need Route to remove it
			if (pPlot->getRouteType() != eRoute)
			{
				return -1;
			}
		}

		// Must be owned by me
		if (pPlot->getOwner() != GetID())
		{
			return -1;
		}
	}

	// Tech requirement
	for (int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iBuildLoop);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			if (pkBuildInfo->getRoute() == eRoute)
			{
				if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)pkBuildInfo->getTechPrereq())))
				{
					return -1;
				}
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Improvement
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot)
{
	if (eImprovement == NO_IMPROVEMENT)
	{
		return -1;
	}

	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = 0;//GC.getImprovementInfo(eImprovement)->GetAdvancedStartCost();

		// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGame().getGameSpeedInfo().getBuildPercent();
	iCost /= 100;

	// Can this Improvement be on our plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			// Valid Plot
			if (!pPlot->canHaveImprovement(eImprovement, getTeam(), false))
			{
				return -1;
			}

			bool bValid = false;

			for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
				if (!pkBuildInfo)
				{
					continue;
				}
				ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

				if (eImprovement == eLoopImprovement && canBuild(pPlot, (BuildTypes)iI))
				{
					bValid = true;

					FeatureTypes eFeature = pPlot->getFeatureType();
					if (NO_FEATURE != eFeature && pkBuildInfo->isFeatureRemove(eFeature))
					{
						iCost += GC.getFeatureInfo(eFeature)->getAdvancedStartRemoveCost();
					}

					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}

			// Can't place twice
			if (pPlot->getImprovementType() == eImprovement)
			{
				return -1;
			}
		}
		else
		{
			// Need this improvement in order to remove it
			if (pPlot->getImprovementType() != eImprovement)
			{
				return -1;
			}
		}

		// Must be owned by me
		if (pPlot->getOwner() != GetID())
		{
			return -1;
		}
	}

	// Tech requirement
	for (int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
	{
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iBuildLoop);
		if (!pkBuildInfo)
		{
			continue;
		}

		if (pkBuildInfo->getImprovement() == eImprovement)
		{
			if (!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)pkBuildInfo->getTechPrereq())))
			{
				return -1;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Tech
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartTechCost(TechTypes eTech, bool bAdd)
{
	if (eTech == NO_TECH)
	{
		return -1;
	}

	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (GET_TEAM(getTeam()).GetTeamTechs()->GetResearchCost(eTech) * GC.getTechInfo(eTech)->GetAdvancedStartCost()) / 100;
	if (iCost < 0)
	{
		return -1;
	}

	if (bAdd)
	{
		if (!GetPlayerTechs()->CanResearch(eTech, false))
		{
			return -1;
		}
	}
	else if (!bAdd)
	{
		if (!GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTech))
		{
			return -1;
		}

		// Search through all techs to see if any of the currently owned ones requires this tech
		for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
		{
			TechTypes eTechLoop = (TechTypes) iTechLoop;

			if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eTechLoop))
			{
				int iPrereqLoop;

				// Or Prereqs
				for (iPrereqLoop = 0; iPrereqLoop < GC.getNUM_OR_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getTechInfo(eTechLoop)->GetPrereqOrTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}

				// And Prereqs
				for (iPrereqLoop = 0; iPrereqLoop < GC.getNUM_AND_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getTechInfo(eTechLoop)->GetPrereqAndTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}
			}
		}

		// If player has placed anything on the map which uses this tech then you cannot remove it
		int iLoop;

		// Units
		CvUnit* pLoopUnit;
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitInfo().GetPrereqAndTech() == eTech)
			{
				return -1;
			}

			for (int iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
			{
				if (pLoopUnit->getUnitInfo().GetPrereqAndTechs(iI) == eTech)
				{
					return -1;
				}
			}
		}

		// Cities
		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			// All Buildings
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo)
				{
					if (pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
					{
						if (pkBuildingInfo->GetPrereqAndTech() == eTech)
						{
							return -1;
						}

						for (int iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
						{
							if (pkBuildingInfo->GetPrereqAndTechs(iI) == eTech)
							{
								return -1;
							}
						}
					}
				}
			}
		}

	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Visibility
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumVisiblePlots = 0;
	int iCost = GC.getADVANCED_START_VISIBILITY_COST();

	// This denotes Visibility may not be purchased through Advanced Start
	if (iCost == -1)
	{
		return -1;
	}

	// Valid Plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			if (pPlot->isRevealed(getTeam()))
			{
				return -1;
			}
			if (!pPlot->isAdjacentRevealed(getTeam()))
			{
				return -1;
			}
		}
		else
		{
			if (!pPlot->isRevealed(getTeam()))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getADVANCED_START_VISIBILITY_COST_INCREASE())
	{
		const int nPlots = GC.getMap().numPlots();
		for (int iPlotLoop = 0; iPlotLoop < nPlots; iPlotLoop++)
		{
			CvPlot* pMapPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if (pMapPlot->isRevealed(getTeam()))
			{
				++iNumVisiblePlots;
			}
		}

		if (!bAdd)
		{
			--iNumVisiblePlots;
		}

		if (iNumVisiblePlots > 0)
		{
			iCost *= 100 + GC.getADVANCED_START_VISIBILITY_COST_INCREASE() * iNumVisiblePlots;
			iCost /= 100;
		}
	}

	return iCost;
}

//	--------------------------------------------------------------------------------
void CvPlayer::doWarnings()
{
	if (m_eID == GC.getGame().getActivePlayer())
	{
		//update enemy units close to your territory
		int iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			if (iMaxCount == 0)
			{
				break;
			}

			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

			if (pLoopPlot->isAdjacentPlayer(GetID()))
			{
				if (!(pLoopPlot->isCity()))
				{
					if (pLoopPlot->isVisible(getTeam()))
					{
						CvUnit *pUnit = pLoopPlot->getVisibleEnemyDefender(GetID());
						if (pUnit != NULL)
						{
							CvCity* pNearestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), GetID(), NO_TEAM, !(pLoopPlot->isWater()));

							if (pNearestCity != NULL)
							{
								CvString message = GetLocalizedText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED", pNearestCity->getNameKey());
								GC.GetEngineUserInterface()->AddPlotMessage(0, pLoopPlot->GetPlotIndex(), GetID(), true, GC.getEVENT_MESSAGE_TIME(), message);

								iMaxCount--;
							}
						}
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::processPolicies(PolicyTypes ePolicy, int iChange)
{
	int iI, iJ;

	CvPolicyEntry* pPolicy = GC.getPolicyInfo(ePolicy);
	if(pPolicy == NULL)
		return;

	const CvPolicyEntry& kPolicy = (*pPolicy);

	if (pPolicy->IsOneShot())
	{
		if (m_pPlayerPolicies->HasOneShotPolicyFired(ePolicy))
		{
			return;
		}
		else
		{
			m_pPlayerPolicies->SetOneShotPolicyFired(ePolicy,true);
		}
	}

	ChangeCulturePerWonder(pPolicy->GetCulturePerWonder() * iChange);
	ChangeCultureWonderMultiplier(pPolicy->GetCultureWonderMultiplier() * iChange);
	ChangeCulturePerTechResearched(pPolicy->GetCulturePerTechResearched() * iChange);
	ChangeGoldenAgeMeterMod(pPolicy->GetGoldenAgeMeterMod() * iChange);
	changeGoldenAgeModifier(pPolicy->GetGoldenAgeDurationMod() * iChange);
	changeWorkerSpeedModifier(pPolicy->GetWorkerSpeedModifier() * iChange);
	changeImprovementCostModifier(pPolicy->GetImprovementCostModifier() * iChange);
	changeImprovementUpgradeRateModifier(pPolicy->GetImprovementUpgradeRateModifier() * iChange);
	changeSpecialistProductionModifier(pPolicy->GetSpecialistProductionModifier() * iChange);
	changeMilitaryProductionModifier(pPolicy->GetMilitaryProductionModifier() * iChange);
	changeBaseFreeUnits(pPolicy->GetBaseFreeUnits() * iChange);
	ChangeHappinessPerGarrisonedUnit(pPolicy->GetHappinessPerGarrisonedUnit() * iChange);
	ChangeHappinessPerTradeRoute(pPolicy->GetHappinessPerTradeRoute() * iChange);
	ChangeHappinessPerXPopulation(pPolicy->GetHappinessPerXPopulation() * iChange);
	ChangeExtraHappinessPerLuxury(pPolicy->GetExtraHappinessPerLuxury() * iChange);
	ChangeUnhappinessFromUnitsMod(pPolicy->GetUnhappinessFromUnitsMod() * iChange);
	ChangeUnhappinessMod(pPolicy->GetUnhappinessMod() * iChange);
	ChangeCityCountUnhappinessMod(pPolicy->GetCityCountUnhappinessMod() * iChange);
	ChangeOccupiedPopulationUnhappinessMod(pPolicy->GetOccupiedPopulationUnhappinessMod() * iChange);
	ChangeCapitalUnhappinessMod(pPolicy->GetCapitalUnhappinessMod() * iChange);
	ChangeWoundedUnitDamageMod(pPolicy->GetWoundedUnitDamageMod() * iChange);
	ChangeUnitUpgradeCostMod(pPolicy->GetUnitUpgradeCostMod() * iChange);
	ChangeBarbarianCombatBonus(pPolicy->GetBarbarianCombatBonus() * iChange);
	ChangeAlwaysSeeBarbCampsCount(pPolicy->IsAlwaysSeeBarbCamps() * iChange);
	ChangeMaxNumBuilders(pPolicy->GetNumExtraBuilders() * iChange);
	ChangePlotGoldCostMod(pPolicy->GetPlotGoldCostMod() * iChange);
	ChangePlotCultureCostModifier(pPolicy->GetPlotCultureCostModifier() * iChange);
	ChangePlotCultureExponentModifier(pPolicy->GetPlotCultureExponentModifier() * iChange);
	ChangeNumCitiesPolicyCostDiscount(pPolicy->GetNumCitiesPolicyCostDiscount() * iChange);
	ChangeGarrisonFreeMaintenanceCount(pPolicy->IsGarrisonFreeMaintenance() * iChange);
	ChangeGarrisonedCityRangeStrikeModifier(pPolicy->GetGarrisonedCityRangeStrikeModifier() * iChange);
	ChangeUnitPurchaseCostModifier(pPolicy->GetUnitPurchaseCostModifier() * iChange);
	GetTreasury()->ChangeTradeRouteGoldModifier(pPolicy->GetTradeRouteGoldModifier() * iChange);
	changeGoldPerUnit(pPolicy->GetGoldPerUnit() * iChange);
	changeGoldPerMilitaryUnit(pPolicy->GetGoldPerMilitaryUnit() * iChange);
	ChangeCityStrengthMod(pPolicy->GetCityStrengthMod() * iChange);
	ChangeCityGrowthMod(pPolicy->GetCityGrowthMod() * iChange);
	ChangeCapitalGrowthMod(pPolicy->GetCapitalGrowthMod() * iChange);
	changeSettlerProductionModifier(pPolicy->GetSettlerProductionModifier() * iChange);
	changeCapitalSettlerProductionModifier(pPolicy->GetCapitalSettlerProductionModifier() * iChange);
	ChangeRouteGoldMaintenanceMod(pPolicy->GetRouteGoldMaintenanceMod() * iChange);
	ChangeBuildingGoldMaintenanceMod(pPolicy->GetBuildingGoldMaintenanceMod() * iChange);
	ChangeUnitGoldMaintenanceMod(pPolicy->GetUnitGoldMaintenanceMod() * iChange);
	ChangeUnitSupplyMod(pPolicy->GetUnitSupplyMod() * iChange);
	changeHappyPerMilitaryUnit(pPolicy->GetHappyPerMilitaryUnit() * iChange);
	changeHappinessToCulture(pPolicy->GetHappinessToCulture() * iChange);
	changeHappinessToScience(pPolicy->GetHappinessToScience() * iChange);
	changeHalfSpecialistUnhappinessCount((pPolicy->IsHalfSpecialistUnhappiness()) ? iChange : 0);
	changeHalfSpecialistFoodCount((pPolicy->IsHalfSpecialistFood()) ? iChange : 0);
	changeMilitaryFoodProductionCount((pPolicy->IsMilitaryFoodProduction()) ? iChange : 0);
	changeMaxConscript(getWorldSizeMaxConscript(kPolicy) * iChange);
	changeExpModifier(pPolicy->GetExpModifier() * iChange);
	changeExpInBorderModifier(pPolicy->GetExpInBorderModifier() * iChange);
	changeMinorQuestFriendshipMod(pPolicy->GetMinorQuestFriendshipMod() * iChange);
	changeMinorGoldFriendshipMod(pPolicy->GetMinorGoldFriendshipMod() * iChange);
	changeGetMinorFriendshipMinimum(pPolicy->GetMinorFriendshipMinimum() * iChange);
	changeGetMinorFriendshipDecayMod(pPolicy->GetMinorFriendshipDecayMod() * iChange);
	ChangeMinorScienceAlliesCount(pPolicy->IsMinorScienceAllies() * iChange);
	ChangeMinorResourceBonusCount(pPolicy->IsMinorResourceBonus() * iChange);
	ChangeNewCityExtraPopulation(pPolicy->GetNewCityExtraPopulation() * iChange);
	ChangeFreeFoodBox(pPolicy->GetFreeFoodBox() * iChange);
	ChangeStrategicResourceMod(pPolicy->GetStrategicResourceMod() * iChange);

	GetPlayerPolicies()->ChangeNumExtraBranches(pPolicy->GetNumExtraBranches() * iChange);

	ChangeAllFeatureProduction(pPolicy->GetAllFeatureProduction());

	int iMod;
	YieldTypes eYield;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		eYield = (YieldTypes) iI;

		iMod = pPolicy->GetYieldModifier(iI) * iChange;
		if (iMod != 0)
			changeYieldRateModifier(eYield, iMod);

		iMod = pPolicy->GetCityYieldChange(iI) * iChange;
		if (iMod != 0)
			ChangeCityYieldChange(eYield, iMod * 100);

		iMod = pPolicy->GetCoastalCityYieldChange(iI) * iChange;
		if (iMod != 0)
			ChangeCoastalCityYieldChange(eYield, iMod);

		iMod = pPolicy->GetCapitalYieldChange(iI) * iChange;
		if (iMod != 0)
			ChangeCapitalYieldChange(eYield, iMod * 100);

		iMod = pPolicy->GetCapitalYieldPerPopChange(iI) * iChange;
		if (iMod != 0)
			ChangeCapitalYieldPerPopChange(eYield, iMod);

		iMod = pPolicy->GetCapitalYieldModifier(iI) * iChange;
		if (iMod != 0)
			changeCapitalYieldRateModifier(eYield, iMod);

		iMod = pPolicy->GetSpecialistExtraYield(iI) * iChange;
		if (iMod != 0)
			changeSpecialistExtraYield(eYield, iMod);
	}

	for (iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		changeUnitCombatProductionModifiers((UnitCombatTypes)iI, (pPolicy->GetUnitCombatProductionModifiers(iI) * iChange));
		changeUnitCombatFreeExperiences((UnitCombatTypes)iI, (pPolicy->GetUnitCombatFreeExperiences(iI) * iChange));
	}

	for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		if (GC.getHurryInfo((HurryTypes) iI)->getPolicyPrereq() == ePolicy)
		{
			changeHurryCount(((HurryTypes)iI), iChange);
		}
		{
			changeHurryModifier((HurryTypes) iI, (pPolicy->GetHurryModifier(iI) * iChange));
		}
	}

	for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (pPolicy->GetImprovementYieldChanges(iI, iJ) * iChange));
		}
	}

	// Free Promotions
	PromotionTypes ePromotion;
	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if (pPolicy->IsFreePromotion(ePromotion))
			ChangeFreePromotionCount(ePromotion, iChange);
	}

	CvCity* pLoopCity;
	PlayerTypes ePlayer;

	// All player Capital Locations Revealed
	if (pPolicy->IsRevealAllCapitals())
	{
		for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			ePlayer = (PlayerTypes) iI;

			if (GET_PLAYER(ePlayer).isAlive())
			{
				pLoopCity = GET_PLAYER(ePlayer).getCapitalCity();

				if (pLoopCity != NULL)
				{
					pLoopCity->plot()->setRevealed(getTeam(), true);
				}
			}
		}
	}

	// Friendship Decay for OTHER PLAYERS
	CvNotifications* pNotifications;
	Localization::String locString;
	Localization::String locSummary;

	int iOtherPlayersDecay = pPolicy->GetOtherPlayersMinorFriendshipDecayMod();
	if (iOtherPlayersDecay != 0)
	{
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			ePlayer = (PlayerTypes) iI;

			if (GET_PLAYER(ePlayer).isEverAlive())
			{
				// Don't hurt us or teammates
				if (GET_PLAYER(ePlayer).getTeam() != getTeam())
				{
					GET_PLAYER(ePlayer).changeGetMinorFriendshipDecayMod(iOtherPlayersDecay * iChange);

					// Send notification to affected players
					locString = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_FRIENDSHIP_DECAY");
					locString << getNameKey();
					locSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDSHIP_DECAY");

					pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if (pNotifications)
					{
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, locString.toUTF8(), locSummary.toUTF8(), -1, -1, -1);
					}
				}
			}
		}
	}

	BuildingClassTypes eBuildingClass;
	BuildingTypes eBuilding;
	int iBuildingCount;
	int iYieldMod;
	int iYieldChange;

	// How many cities get free culture buildings?
	int iNumCitiesFreeCultureBuilding = pPolicy->GetNumCitiesFreeCultureBuilding();

	// Loop through Cities
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (iNumCitiesFreeCultureBuilding > 0)
		{
			BuildingTypes eCultureBuilding = pLoopCity->ChooseFreeCultureBuilding();
			if (eCultureBuilding != NO_BUILDING)
			{
				pLoopCity->GetCityBuildings()->SetNumRealBuilding(eCultureBuilding, 1);

				if (pLoopCity->getFirstBuildingOrder(eCultureBuilding) == 0)
				{
					pLoopCity->clearOrderQueue();
					pLoopCity->chooseProduction();		// Send a notification to the user that what they were building was given to them, and they need to produce something else.
				}
			}

			// Decrement cities left to get free culture building (at end of loop we'll set the remainder)
			iNumCitiesFreeCultureBuilding--;
		}

		bool bUpdateCulture = false;
		if (pPolicy->GetCultureImprovementChange() != 0)
		{
			bUpdateCulture = true;
		}
		else
		{
			for (int jJ = 0; jJ < GC.getNumImprovementInfos(); jJ++)
			{
				if (pPolicy->GetImprovementCultureChanges((ImprovementTypes)jJ))
				{
					bUpdateCulture = true;
					break;
				}
			}
		}
		if (bUpdateCulture)
		{
			pLoopCity->UpdateCulture();
		}

		// Free Culture-per-turn in every City
		int iCityCultureChange = pPolicy->GetCulturePerCity() * iChange;
		if (pLoopCity->GetGarrisonedUnit() != NULL)
		{
			iCityCultureChange += (pPolicy->GetCulturePerGarrisonedUnit() * iChange);
		}
		pLoopCity->ChangeJONSCulturePerTurnFromPolicies(iCityCultureChange);

		// Building modifiers
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eBuildingClass = (BuildingClassTypes) iI;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if (!pkBuildingClassInfo)
			{
				continue;
			}

			eBuilding = (BuildingTypes) getCivilizationInfo().getCivilizationBuildings(eBuildingClass);

			if(eBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
				if(pkBuilding)
				{
					iBuildingCount = pLoopCity->GetCityBuildings()->GetNumRealBuilding(eBuilding);

					if (iBuildingCount > 0)
					{
						pLoopCity->ChangeJONSCulturePerTurnFromPolicies(pPolicy->GetBuildingClassCultureChange(eBuildingClass) * iBuildingCount * iChange);

						// Building Class Yield Stuff
						for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							eYield = (YieldTypes) iJ;
							iYieldMod = pPolicy->GetBuildingClassYieldModifiers(eBuildingClass, eYield);
							if (iYieldMod > 0)
							{
								pLoopCity->changeYieldRateModifier(eYield, iYieldMod * iBuildingCount * iChange);
							}
							iYieldChange = pPolicy->GetBuildingClassYieldChanges(eBuildingClass, eYield);
							if (iYieldChange > 0)
							{
								pLoopCity->ChangeBaseYieldRateFromBuildings(eYield, iYieldChange * iBuildingCount * iChange);
							}
						}
					}
				}
			}
		}
	}

	// Store off number of newly built cities that will get a free culture building
	ChangeNumCitiesFreeCultureBuilding(iNumCitiesFreeCultureBuilding);

	// Not really techs but this is what we use (for now)
	for (iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		if (pPolicy->GetPolicyDisables(iI) != NO_POLICY)
		{
			if (m_pPlayerPolicies->HasPolicy((PolicyTypes) pPolicy->GetPolicyDisables(iI)))
			{
				setHasPolicy((PolicyTypes) pPolicy->GetPolicyDisables(iI), false);
			}
		}
	}

	// Attack bonus for a period of time
	int iTurns = pPolicy->GetAttackBonusTurns() * iChange;
	if (iTurns > 0)
	{
		ChangeAttackBonusTurns(iTurns);
	}

	// Golden Age!
	int iGoldenAgeTurns = pPolicy->GetGoldenAgeTurns() * iChange;
	if (iGoldenAgeTurns > 0)
	{
		// Player modifier
		int iLengthModifier = getGoldenAgeModifier();

		// Trait modifier
		iLengthModifier += GetPlayerTraits()->GetGoldenAgeDurationModifier();

		if (iLengthModifier > 0)
		{
			iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;
		}

		// Game Speed mod
		iGoldenAgeTurns *= GC.getGame().getGameSpeedInfo().getGoldenAgePercent();
		iGoldenAgeTurns /= 100;

		changeGoldenAgeTurns(iGoldenAgeTurns);
	}

	// Free Techs
	int iNumFreeTechs = pPolicy->GetNumFreeTechs() * iChange;
	if (iNumFreeTechs > 0)
	{
		if (!isHuman())
		{
			for (iI = 0; iI < iNumFreeTechs; iI++)
			{
				AI_chooseFreeTech();
			}
		}
		else
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH", pPolicy->GetTextKey());
			chooseTech(iNumFreeTechs, strBuffer.GetCString());
		}
	}

	ChangeMedianTechPercentage(pPolicy->GetMedianTechPercentChange());

	// Free Policies
	int iNumFreePolicies = pPolicy->GetNumFreePolicies() * iChange;
	if (iNumFreePolicies > 0)
	{
		ChangeNumFreePolicies(iNumFreePolicies);
	}

	if (pPolicy->IncludesOneShotFreeUnits())
	{
		if (!m_pPlayerPolicies->HaveOneShotFreeUnitsFired(ePolicy))
		{
			m_pPlayerPolicies->SetOneShotFreeUnitsFired(ePolicy,true);

			int iNumFreeGreatPeople = pPolicy->GetNumFreeGreatPeople() * iChange;
			if(iNumFreeGreatPeople > 0)
			{
				ChangeNumFreeGreatPeople(iNumFreeGreatPeople);
			}

			if (getCapitalCity() != NULL)
			{
				int iX = getCapitalCity()->getX();
				int iY = getCapitalCity()->getY();

				for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
				{
					const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
					CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
					if(pkUnitClassInfo)
					{
						int iNumFreeUnits = pPolicy->GetNumFreeUnitsByClass(eUnitClass);
						if (iNumFreeUnits > 0)
						{
							const UnitTypes eUnit = (UnitTypes) getCivilizationInfo().getCivilizationUnits(eUnitClass);

							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
							if(pkUnitInfo == NULL)
							{
								continue;
							}

							if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman() && pkUnitInfo->IsFound())
							{
								continue;
							}

							for (int iUnitLoop = 0; iUnitLoop < iNumFreeUnits; iUnitLoop++)
							{
								CvUnit* pNewUnit = initUnit(eUnit, iX, iY);
								CvAssert(pNewUnit);
								if (pNewUnit)
								{
									// Make sure the new Unit is in an appropriate place
									if (!pNewUnit->jumpToNearestValidPlot())
										pNewUnit->kill(false);
								}
							}
						}
					}
				}
			}
		}
	}

	// Great People bonus from Allied city-states
	if (pPolicy->IsMinorGreatPeopleAllies())
	{
		DoAdoptedGreatPersonCityStatePolicy();
	}

	DoUpdateHappiness();
	recomputeGreatPeopleModifiers();
	recomputePolicyCostModifier();
	recomputeFreeExperience();

	doUpdateBarbarianCampVisibility();

	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
/// If we should see where the locations of all current Barb Camps are, do it
void CvPlayer::doUpdateBarbarianCampVisibility()
{
	if (IsAlwaysSeeBarbCamps())
	{
		CvPlot* pPlot;

		ImprovementTypes eImprovement;

		for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); ++iPlotLoop)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

			if (pPlot->isRevealed(getTeam()))
			{
				eImprovement = pPlot->getImprovementType();

				// Camp here
				if (eImprovement == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					// We don't see Camp
					if (pPlot->getRevealedImprovementType(getTeam()) != eImprovement)
					{
						pPlot->setRevealedImprovementType(getTeam(), eImprovement);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}

//	--------------------------------------------------------------------------------
CvEconomicAI *CvPlayer::GetEconomicAI() const
{
	return m_pEconomicAI;
}

//	--------------------------------------------------------------------------------
CvMilitaryAI *CvPlayer::GetMilitaryAI() const
{
	return m_pMilitaryAI;
}

//	--------------------------------------------------------------------------------
CvCitySpecializationAI *CvPlayer::GetCitySpecializationAI() const
{
	return m_pCitySpecializationAI;
}

//	--------------------------------------------------------------------------------
CvWonderProductionAI *CvPlayer::GetWonderProductionAI() const
{
	return m_pWonderProductionAI;
}

//	--------------------------------------------------------------------------------
CvGrandStrategyAI *CvPlayer::GetGrandStrategyAI() const
{
	return m_pGrandStrategyAI;
}

//	--------------------------------------------------------------------------------
CvDiplomacyAI *CvPlayer::GetDiplomacyAI() const
{
	return m_pDiplomacyAI;
}

//	--------------------------------------------------------------------------------
CvMinorCivAI *CvPlayer::GetMinorCivAI() const
{
	return m_pMinorCivAI;
}

//	--------------------------------------------------------------------------------
CvDealAI *CvPlayer::GetDealAI() const
{
	return m_pDealAI;
}

//	--------------------------------------------------------------------------------
/// Get the object that decides what task the builders should perform
CvBuilderTaskingAI *CvPlayer::GetBuilderTaskingAI() const
{
	return m_pBuilderTaskingAI;
}

//	--------------------------------------------------------------------------------
/// Get the city connection that gives you information about the route connections between cities
CvCityConnections* CvPlayer::GetCityConnections() const
{
	return m_pCityConnections;
}

//	--------------------------------------------------------------------------------
CvNotifications* CvPlayer::GetNotifications () const
{
	return m_pNotifications;
}

//	--------------------------------------------------------------------------------
CvTreasury* CvPlayer::GetTreasury () const
{
	return m_pTreasury;
}

//	--------------------------------------------------------------------------------
CvDiplomacyRequests* CvPlayer::GetDiplomacyRequests () const
{
	return m_pDiplomacyRequests;
}

/////////////////////////////////////////////////////////////////////////
bool CvPlayer::HasActiveDiplomacyRequests () const
{
	PlayerTypes ePlayer = GetID();

	// Do I have any?
	CvDiplomacyRequests* pkDiploRequests = GetDiplomacyRequests();
	if (pkDiploRequests && pkDiploRequests->HasActiveRequest())
		return true;

	// Do I have any for others?
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		if (kPlayer.isAlive())
		{
			pkDiploRequests = kPlayer.GetDiplomacyRequests();
			if (pkDiploRequests)
			{
				if (pkDiploRequests->HasActiveRequestFrom(ePlayer))
					return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
//
// read object from a stream
// used during load
//
void CvPlayer::Read(FDataStream& kStream)
{
	// Init data before load
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iStartingX;
	kStream >> m_iStartingY;
	kStream >> m_iTotalPopulation;
	kStream >> m_iTotalLand;
	kStream >> m_iTotalLandScored;
	kStream >> m_iJONSCulturePerTurnForFree;
	kStream >> m_iJONSCulturePerTurnFromMinorCivs;
	kStream >> m_iJONSCultureCityModifier;
	kStream >> m_iJONSCulture;
	kStream >> m_iJONSCultureEverGenerated;
	kStream >> m_iCulturePerWonder;
	kStream >> m_iCultureWonderMultiplier;
	kStream >> m_iCulturePerTechResearched;
	kStream >> m_iHappiness;

	if (uiVersion >= 10)
		kStream >> m_iRevoltCounter;
	else
		m_iRevoltCounter = 0;

	kStream >> m_iExtraHappinessPerLuxury;
	kStream >> m_iUnhappinessFromUnits;
	kStream >> m_iUnhappinessFromUnitsMod;
	kStream >> m_iUnhappinessMod;
	kStream >> m_iCityCountUnhappinessMod;
	kStream >> m_iOccupiedPopulationUnhappinessMod;
	kStream >> m_iCapitalUnhappinessMod;
	kStream >> m_iHappinessFromGarrisonedUnits;
	kStream >> m_iHappinessPerGarrisonedUnitCount;
	kStream >> m_iHappinessPerTradeRouteCount;
	if (uiVersion >= 19)
	{
		kStream >> m_iHappinessPerXPopulation;
	}
	else
	{
		m_iHappinessPerXPopulation = 0;
	}
	if (uiVersion >= 23)
	{
		kStream >> m_iHappinessPerXPolicies;
	}
	else
	{
		m_iHappinessPerXPolicies = 0;
	}
	kStream >> m_iSpecialPolicyBuildingHappiness;
	kStream >> m_iWoundedUnitDamageMod;
	kStream >> m_iUnitUpgradeCostMod;
	kStream >> m_iBarbarianCombatBonus;
	kStream >> m_iAlwaysSeeBarbCampsCount;
	kStream >> m_iHappinessFromBuildings;
	kStream >> m_iHappinessPerCity;
	kStream >> m_iAdvancedStartPoints;
	kStream >> m_iAttackBonusTurns;
	kStream >> m_iGoldenAgeProgressMeter;
	kStream >> m_iGoldenAgeMeterMod;
	kStream >> m_iNumGoldenAges;
	kStream >> m_iGoldenAgeTurns;
	kStream >> m_iNumUnitGoldenAges;
	kStream >> m_iStrikeTurns;
	kStream >> m_iGoldenAgeModifier;
	kStream >> m_iGreatPeopleCreated;
	kStream >> m_iGreatGeneralsCreated;
	kStream >> m_iGreatPeopleThresholdModifier;
	kStream >> m_iGreatGeneralsThresholdModifier;
	kStream >> m_iAnarchyNumTurns;
	kStream >> m_iPolicyCostModifier;
	kStream >> m_iGreatPeopleRateModifier;
	kStream >> m_iGreatPeopleRateModFromBldgs;
	kStream >> m_iGreatGeneralRateModifier;
	kStream >> m_iGreatGeneralRateModFromBldgs;
	kStream >> m_iDomesticGreatGeneralRateModifier;
	kStream >> m_iDomesticGreatGeneralRateModFromBldgs;
	if (uiVersion >= 16)
	{
		kStream >> m_iGreatPersonExpendGold;
	}
	else
	{
		m_iGreatPersonExpendGold = 0;
	}
	kStream >> m_iMaxGlobalBuildingProductionModifier;
	kStream >> m_iMaxTeamBuildingProductionModifier;
	kStream >> m_iMaxPlayerBuildingProductionModifier;
	kStream >> m_iFreeExperience;
	kStream >> m_iFreeExperienceFromBldgs;
	kStream >> m_iFreeExperienceFromMinors;
	kStream >> m_iFeatureProductionModifier;
	kStream >> m_iWorkerSpeedModifier;
	kStream >> m_iImprovementCostModifier;
	kStream >> m_iImprovementUpgradeRateModifier;
	kStream >> m_iSpecialistProductionModifier;
	kStream >> m_iMilitaryProductionModifier;
	kStream >> m_iSpaceProductionModifier;
	kStream >> m_iCityDefenseModifier;
	kStream >> m_iWonderProductionModifier;
	kStream >> m_iSettlerProductionModifier;

	if (uiVersion >= 11)
		kStream >> m_iCapitalSettlerProductionModifier;
	else
		m_iCapitalSettlerProductionModifier = 0;

	kStream >> m_iUnitProductionMaintenanceMod;

	if (uiVersion >= 3)
		kStream >> m_iPolicyCostBuildingModifier;
	else
		m_iPolicyCostBuildingModifier = 0;

	kStream >> m_iPolicyCostMinorCivModifier;
	kStream >> m_iNumNukeUnits;
	kStream >> m_iNumOutsideUnits;
	kStream >> m_iBaseFreeUnits;
	kStream >> m_iBaseFreeMilitaryUnits;
	kStream >> m_iFreeUnitsPopulationPercent;
	kStream >> m_iFreeMilitaryUnitsPopulationPercent;
	kStream >> m_iGoldPerUnit;
	kStream >> m_iGoldPerMilitaryUnit;
	kStream >> m_iRouteGoldMaintenanceMod;
	kStream >> m_iBuildingGoldMaintenanceMod;
	kStream >> m_iUnitGoldMaintenanceMod;
	kStream >> m_iUnitSupplyMod;
	kStream >> m_iExtraUnitCost;
	kStream >> m_iNumMilitaryUnits;
	kStream >> m_iHappyPerMilitaryUnit;
	kStream >> m_iHappinessToCulture;
	kStream >> m_iHappinessToScience;
	kStream >> m_iHalfSpecialistUnhappinessCount;
	kStream >> m_iHalfSpecialistFoodCount;
	kStream >> m_iMilitaryFoodProductionCount;
	kStream >> m_iConscriptCount;
	kStream >> m_iMaxConscript;
	kStream >> m_iHighestUnitLevel;
	kStream >> m_iOverflowResearch;
	kStream >> m_iExpModifier;
	kStream >> m_iExpInBorderModifier;
	kStream >> m_iLevelExperienceModifier;
	kStream >> m_iMinorQuestFriendshipMod;
	kStream >> m_iMinorGoldFriendshipMod;
	kStream >> m_iMinorFriendshipMinimum;
	kStream >> m_iMinorFriendshipDecayMod;
	kStream >> m_iMinorScienceAlliesCount;
	kStream >> m_iMinorResourceBonusCount;
	kStream >> m_iConversionTimer;
	kStream >> m_iCapitalCityID;
	kStream >> m_iCitiesLost;
	kStream >> m_iMilitaryMight;
	kStream >> m_iEconomicMight;
	kStream >> m_iTurnMightRecomputed;
	kStream >> m_iNewCityExtraPopulation;
	kStream >> m_iFreeFoodBox;
	kStream >> m_iPopulationScore;
	kStream >> m_iLandScore;
	kStream >> m_iWondersScore;
	if (uiVersion >= 5)
		kStream >> m_iScoreFromFutureTech;
	else
		m_iScoreFromFutureTech = 0;
	kStream >> m_iTechScore;
	kStream >> m_iCombatExperience;
	if (uiVersion >= 17)
	{
		kStream >> m_iLifetimeCombatExperience;
	}
	else
	{
		m_iLifetimeCombatExperience = 0;
	}
	if (uiVersion >= 24)
	{
		kStream >> m_iBorderObstacleCount;
	}
	else
	{
		m_iBorderObstacleCount = 0;
	}
	kStream >> m_iNextOperationID;
	kStream >> m_iCostNextPolicy;
	kStream >> m_iNumBuilders;
	kStream >> m_iMaxNumBuilders;
	kStream >> m_iCityStrengthMod;
	kStream >> m_iCityGrowthMod;
	kStream >> m_iCapitalGrowthMod;
	kStream >> m_iNumPlotsBought;
	kStream >> m_iPlotGoldCostMod;
	kStream >> m_iPlotCultureCostModifier;
	if (uiVersion >= 14)
	{
		kStream >> m_iPlotCultureExponentModifier;
		kStream >> m_iNumCitiesPolicyCostDiscount;
		kStream >> m_iGarrisonFreeMaintenanceCount;
		kStream >> m_iGarrisonedCityRangeStrikeModifier;
	}
	else
	{
		m_iPlotCultureExponentModifier = 0;
		m_iNumCitiesPolicyCostDiscount = 0;
		m_iGarrisonFreeMaintenanceCount = 0;
		m_iGarrisonedCityRangeStrikeModifier = 0;
	}
	if (uiVersion >= 15)
	{
		kStream >> m_iNumCitiesFreeCultureBuilding;
	}
	else
	{
		m_iNumCitiesFreeCultureBuilding = 0;
	}
	kStream >> m_iUnitPurchaseCostModifier;
	kStream >> m_iAllFeatureProduction;
	kStream >> m_iCityDistanceHighwaterMark;
	kStream >> m_iOriginalCapitalX;
	kStream >> m_iOriginalCapitalY;
	kStream >> m_iNumWonders;
	kStream >> m_iNumPolicies;
	kStream >> m_iNumGreatPeople;
	kStream >> m_iCityConnectionHappiness;
	kStream >> m_iHolyCityID;

	if (uiVersion >= 6)
		kStream >> m_iTurnsSinceSettledLastCity;
	else
		m_iTurnsSinceSettledLastCity = -1;

	kStream >> m_iNumNaturalWondersDiscoveredInArea;
	kStream >> m_iStrategicResourceMod;
	kStream >> m_iSpecialistCultureChange;

	if (uiVersion >= 4)
		kStream >> m_iGreatPeopleSpawnCounter;
	else
		m_iGreatPeopleSpawnCounter = 0;

	kStream >> m_iFreeTechCount;

	if (uiVersion >= 20)
	{
		kStream >> m_iMedianTechPercentage;
	}
	else
	{
		m_iMedianTechPercentage = 50;
	}

	kStream >> m_iNumFreePolicies;
	kStream >> m_iNumFreePoliciesEver;

	if(uiVersion >= 15)
	{
		kStream >> m_iNumFreeGreatPeople;
	}
	else
	{
		m_iNumFreeGreatPeople = 0;
	}

	kStream >> m_iLastSliceMoved;

	kStream >> m_bHasBetrayedMinorCiv;
	kStream >> m_bAlive;
	kStream >> m_bEverAlive;
	kStream >> m_bTurnActive;
	kStream >> m_bAutoMoves;
	kStream >> m_bEndTurn;
	if(uiVersion >= 25)
	{
		kStream >> m_bDynamicTurnsSimultMode;
	}
	kStream >> m_bPbemNewTurn;
	kStream >> m_bExtendedGame;
	kStream >> m_bFoundedFirstCity;
	kStream >> m_bStrike;
	kStream >> m_bCramped;
	kStream >> m_bLostCapital;
	if (uiVersion >= 22)
	{
		kStream >> m_eConqueror;
	}
	else
	{
		m_eConqueror = NO_PLAYER;
	}
	kStream >> m_bHasAdoptedStateReligion;
	kStream >> m_bAlliesGreatPersonBiasApplied;

	kStream >> m_eID;
	kStream >> m_ePersonalityType;

	kStream >> m_aiCityYieldChange;
	kStream >> m_aiCoastalCityYieldChange;
	kStream >> m_aiCapitalYieldChange;

	// In v13, yield changes were changed to be in hundreds
	if (uiVersion < 13)
	{
		YieldTypes eYieldLoop;
		for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
		{
			eYieldLoop = (YieldTypes) iYieldLoop;

			m_aiCityYieldChange.setAt(eYieldLoop, m_aiCityYieldChange[eYieldLoop] * 100);
			m_aiCapitalYieldChange.setAt(eYieldLoop, m_aiCapitalYieldChange[eYieldLoop] * 100);
		}
	}

	if (uiVersion >= 11)
		kStream >> m_aiCapitalYieldPerPopChange;

	kStream >> m_aiSeaPlotYield;
	kStream >> m_aiYieldRateModifier;
	kStream >> m_aiCapitalYieldRateModifier;
	kStream >> m_aiExtraYieldThreshold;
	kStream >> m_aiSpecialistExtraYield;
	kStream >> m_aiProximityToPlayer;
	if (uiVersion >= 2)
	{
		kStream >> m_aiResearchAgreementCounter;
	}
	else
	{
		m_aiResearchAgreementCounter.clear();
		m_aiResearchAgreementCounter.resize(MAX_PLAYERS, 0);
	}
	kStream >> m_aiIncomingUnitTypes;

	if (uiVersion >= 8)
	{
		// no longer serializing m_abOptions
	}
	else
	{
		for(int i = 0; i < NUM_PLAYEROPTION_TYPES; i++)
		{
			bool bDummy;
			kStream >> bDummy;
		}
	}

	kStream >> m_strReligionKey;
	kStream >> m_strScriptData;

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but it is expected to be in CvPlayer::read");
	if (uiVersion <= 19)
	{
		std::vector<int> aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiNumResourceUsed = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiNumResourceTotal = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiResourceGiftedToMinors = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiResourceExport = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiResourceImport = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags)); m_paiResourceFromMinors = aTemp;
	}
	else
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceUsed.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourceTotal.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceGiftedToMinors.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceExport.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceImport.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiResourceFromMinors.dirtyGet());
	}

	kStream >> m_paiImprovementCount;

	if (uiVersion >= 9)
	{
		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for (int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> sTemp;
			if (sTemp != "NO_BUILDING")
			{
				iType = GC.getInfoTypeForString(sTemp);
				int iTempValue;
				kStream >> iTempValue;
				if (iType != -1)
				{
					m_paiFreeBuildingCount.setAt(iType, iTempValue);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Building Type not found: %s", sTemp);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
	else
	{
		int iTempValue;
		int iNumEntries;
		kStream >> iNumEntries;
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiFreeBuildingCount.setAt(iI, iTempValue);
		}
	}

	if (uiVersion >= 9)
	{
		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for (int iI = 0; iI < iNumEntries; iI++)
		{
			int iTempValue;
			kStream >> sTemp;
			kStream >> iTempValue;
			if (sTemp != "PROMOTION_OLIGARCHY")
			{
				iType = GC.getInfoTypeForString(sTemp);
				m_paiFreePromotionCount.setAt(iType, iTempValue);
			}
		}
	}
	else
	{
		int iNumEntries;
		int iTempValue;
		kStream >> iNumEntries;
		for (int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiFreePromotionCount.setAt(iI, iTempValue);
		}
	}

	kStream >> m_paiUnitCombatProductionModifiers;
	kStream >> m_paiUnitCombatFreeExperiences;
	kStream >> m_paiUnitClassCount;
	kStream >> m_paiUnitClassMaking;
	kStream >> m_paiBuildingClassCount;
	kStream >> m_paiBuildingClassMaking;
	kStream >> m_paiProjectMaking;
	kStream >> m_paiHurryCount;
	kStream >> m_paiHurryModifier;

	CvAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::read");

	kStream >> m_pabLoyalMember;

	kStream >> m_pabGetsScienceFromPlayer;

	m_pPlayerPolicies->Read(kStream);
	m_pEconomicAI->Read(kStream);
	m_pCitySpecializationAI->Read(kStream);
	m_pWonderProductionAI->Read(kStream);
	m_pMilitaryAI->Read(kStream);
	m_pGrandStrategyAI->Read(kStream);
	m_pDiplomacyAI->Read(kStream);
	m_pPlayerTechs->Read(kStream);
	m_pFlavorManager->Read(kStream);
	m_pTacticalAI->Read(kStream);
	m_pHomelandAI->Read(kStream);
	m_pMinorCivAI->Read(kStream);
	m_pDealAI->Read(kStream);
	m_pBuilderTaskingAI->Read(kStream);
	m_pCityConnections->Read(kStream);
	m_pDangerPlots->Read(kStream);
	m_pTraits->Read(kStream);
	bool bReadNotifications;
	kStream >> bReadNotifications;
	if (bReadNotifications)
	{
		m_pNotifications = FNEW(CvNotifications, c_eCiv5GameplayDLL, 0);
		m_pNotifications->Init(GetID());
		m_pNotifications->Read(kStream);
	}
	m_pTreasury->Read(kStream);

	// If this is a real player, hook up the player-level flavor recipients
	if (GetID() != NO_PLAYER)
	{
		SlotStatus s = CvPreGame::slotStatus(GetID());
		if ((s == SS_TAKEN || s == SS_COMPUTER) && !isBarbarian())
		{
			m_pFlavorManager->AddFlavorRecipient(m_pPlayerTechs,        false /*bPropogateFlavors*/);
			m_pFlavorManager->AddFlavorRecipient(m_pPlayerPolicies,     false /*bPropogateFlavors*/);
			m_pFlavorManager->AddFlavorRecipient(m_pWonderProductionAI, false /*bPropogateFlavors*/);
		}
	}

	kStream >> m_ppaaiSpecialistExtraYield;
	kStream >> m_ppaaiImprovementYieldChange;

	// Obsolete, just used to read old saves
	if (uiVersion < 21)
	{
		kStream >> m_ppaaiBuildingClassYieldMod;
	}

	kStream >> m_UnitCycle;
	kStream >> m_researchQueue;

	kStream >> m_bEverPoppedGoody;
	kStream >> m_bEverTrainedBuilder;
	kStream >> m_eEndTurnBlockingType;
	kStream >> m_iEndTurnBlockingNotificationIndex;

	kStream >> m_cityNames;

	kStream >> m_cities;
	kStream >> m_units;
	kStream >> m_armyAIs;

	{
		m_AIOperations.clear();
		uint iSize;
		int iID;
		int iOperationType;
		kStream >> iSize;
		for (uint i = 0; i < iSize; i++)
		{
			kStream >> iID;
			kStream >> iOperationType;
			CvAIOperation* pThisOperation = CvAIOperation::CreateOperation((AIOperationTypes)iOperationType, m_eID);
			pThisOperation->Read(kStream);
			m_AIOperations.insert( std::make_pair(pThisOperation->GetID(), pThisOperation) );
		}
	}

	// This used to be where the CvTalkingHeadMessages were read.  They were never used, but to keep compatibility, we have to read the length of the list, which should be 0.
	uint uiDummy;
	kStream >> uiDummy;
	CvAssert(uiDummy == 0);

	{
		clearPopups();
		CvPopupQueue::_Alloc::size_type iSize;
		kStream >> iSize;
		for (CvPopupQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvPopupInfo* pInfo = FNEW(CvPopupInfo(), c_eCiv5GameplayDLL, 0);
			if (NULL != pInfo)
			{
				kStream >> *pInfo;
				m_listPopups.push_back(pInfo);
			}
		}
	}

	if(uiVersion >= 19)
	{
		kStream >> m_ReplayDataSets;
		kStream >> m_ReplayDataSetValues;
	}
	else if(uiVersion >= 18)
	{
		kStream >> m_ReplayDataSets;

		typedef std::vector< std::vector<int> > oldReplaySetType;
		oldReplaySetType oldReplayDataSetValues;
		kStream >> oldReplayDataSetValues;

		for(oldReplaySetType::iterator it = oldReplayDataSetValues.begin(); it != oldReplayDataSetValues.end(); ++it)
		{
			CvPlayer::TurnData turnData;
			unsigned int uiTurn = 0;
			for(std::vector<int>::iterator it1 = (*it).begin(); it1 != (*it).end(); ++it1)
			{
				turnData[uiTurn++] = (*it1);
			}

			m_ReplayDataSetValues.push_back(turnData);
		}
	}
	else
	{
		typedef FFastVector<int, true, c_eCiv5GameplayDLL, 0> CvTurnScoreList;
		CvTurnScoreList scoreHistory, economyHistory, industryHistory, agricultureHistory, powerHistory;

		kStream >> scoreHistory;
		kStream >> economyHistory;
		kStream >> industryHistory;
		kStream >> agricultureHistory;
		kStream >> powerHistory;

		unsigned int uiScoreDataSet = getReplayDataSetIndex("REPLAYDATASET_SCORE");
		for(unsigned int uiTurn = 0; uiTurn < scoreHistory.size(); ++uiTurn)
		{
			setReplayDataValue(uiScoreDataSet, uiTurn, scoreHistory[uiTurn]);
		}

		unsigned int uiEconomyDataSet = getReplayDataSetIndex("REPLAYDATASET_GOLDPERTURN");
		for(unsigned int uiTurn = 0; uiTurn < economyHistory.size(); ++uiTurn)
		{
			setReplayDataValue(uiEconomyDataSet, uiTurn, economyHistory[uiTurn]);
		}

		unsigned int uiIndustryDataSet = getReplayDataSetIndex("REPLAYDATASET_PRODUCTIONPERTURN");
		for(unsigned int uiTurn = 0; uiTurn < industryHistory.size(); ++uiTurn)
		{
			setReplayDataValue(uiIndustryDataSet, uiTurn, industryHistory[uiTurn]);
		}

		unsigned int uiAgricultureDataSet = getReplayDataSetIndex("REPLAYDATASET_FOODPERTURN");
		for(unsigned int uiTurn = 0; uiTurn < agricultureHistory.size(); ++uiTurn)
		{
			setReplayDataValue(uiAgricultureDataSet, uiTurn, agricultureHistory[uiTurn]);
		}
	}

	kStream >> m_aVote;
	kStream >> m_aUnitExtraCosts;

	// reading plot values
	{
		m_aiPlots.clear();
		m_aiPlots.push_back_copy(-1, GC.getMap().numPlots());

		// trying to cut down how big saves are
		int iSize;
		kStream >> iSize;
		for (int i = 0; i < iSize; i++)
		{
			kStream >> m_aiPlots[i];
		}
	}

	if (!isBarbarian())
	{
		// Get the NetID from the initialization structure
		setNetID(gDLL->getAssignedNetworkID(GetID()));
	}

	kStream >> m_iPopRushHurryCount;
	kStream >> m_iTotalImprovementsBuilt;

	if (uiVersion >= 7)
	{
		m_bfEverConqueredBy.ClearAll();
		int iSize;
		kStream >> iSize;
		for (int i = 0; i < iSize; i++)
		{
			bool bValue;
			kStream >> bValue;
			if (bValue)
			{
				m_bfEverConqueredBy.SetBit(i);
			}
		}
	}
	else
	{
		m_bfEverConqueredBy.ClearAll();
	}

	if (uiVersion >= 12)
	{
		kStream >> m_strEmbarkedGraphicOverride;
	}
	else
	{
		m_strEmbarkedGraphicOverride = "";
	}

	if (GetID() < MAX_MAJOR_CIVS)
	{
		if (!m_pDiplomacyRequests)
			m_pDiplomacyRequests = FNEW(CvDiplomacyRequests, c_eCiv5GameplayDLL, 0);
		else
			m_pDiplomacyRequests->Uninit();

		m_pDiplomacyRequests->Init(GetID());
	}

	if(m_bTurnActive)
		GC.getGame().changeNumGameTurnActive(1, std::string("setTurnActive() [loading save game] for player ") + getName());

}

//	--------------------------------------------------------------------------------
//
// save object to a stream
// used during save
//
void CvPlayer::Write(FDataStream& kStream) const
{
	//Save version number.  THIS MUST BE FIRST!!
	kStream << g_CurrentCvPlayerVersion;

	kStream << m_iStartingX;
	kStream << m_iStartingY;
	kStream << m_iTotalPopulation;
	kStream << m_iTotalLand;
	kStream << m_iTotalLandScored;
	kStream << m_iJONSCulturePerTurnForFree;
	kStream << m_iJONSCulturePerTurnFromMinorCivs;
	kStream << m_iJONSCultureCityModifier;
	kStream << m_iJONSCulture;
	kStream << m_iJONSCultureEverGenerated;
	kStream << m_iCulturePerWonder;
	kStream << m_iCultureWonderMultiplier;
	kStream << m_iCulturePerTechResearched;
	kStream << m_iHappiness;
	kStream << m_iRevoltCounter;
	kStream << m_iExtraHappinessPerLuxury;
	kStream << m_iUnhappinessFromUnits;
	kStream << m_iUnhappinessFromUnitsMod;
	kStream << m_iUnhappinessMod;
	kStream << m_iCityCountUnhappinessMod;
	kStream << m_iOccupiedPopulationUnhappinessMod;
	kStream << m_iCapitalUnhappinessMod;
	kStream << m_iHappinessFromGarrisonedUnits;
	kStream << m_iHappinessPerGarrisonedUnitCount;
	kStream << m_iHappinessPerTradeRouteCount;
	kStream << m_iHappinessPerXPopulation;
	kStream << m_iHappinessPerXPolicies;
	kStream << m_iSpecialPolicyBuildingHappiness;
	kStream << m_iWoundedUnitDamageMod;
	kStream << m_iUnitUpgradeCostMod;
	kStream << m_iBarbarianCombatBonus;
	kStream << m_iAlwaysSeeBarbCampsCount;
	kStream << m_iHappinessFromBuildings;
	kStream << m_iHappinessPerCity;
	kStream << m_iAdvancedStartPoints;
	kStream << m_iAttackBonusTurns;
	kStream << m_iGoldenAgeProgressMeter;
	kStream << m_iGoldenAgeMeterMod;
	kStream << m_iNumGoldenAges;
	kStream << m_iGoldenAgeTurns;
	kStream << m_iNumUnitGoldenAges;
	kStream << m_iStrikeTurns;
	kStream << m_iGoldenAgeModifier;
	kStream << m_iGreatPeopleCreated;
	kStream << m_iGreatGeneralsCreated;
	kStream << m_iGreatPeopleThresholdModifier;
	kStream << m_iGreatGeneralsThresholdModifier;
	kStream << m_iAnarchyNumTurns;
	kStream << m_iPolicyCostModifier;
	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatPeopleRateModFromBldgs;
	kStream << m_iGreatGeneralRateModifier;
	kStream << m_iGreatGeneralRateModFromBldgs;
	kStream << m_iDomesticGreatGeneralRateModifier;
	kStream << m_iDomesticGreatGeneralRateModFromBldgs;
	kStream << m_iGreatPersonExpendGold;
	kStream << m_iMaxGlobalBuildingProductionModifier;
	kStream << m_iMaxTeamBuildingProductionModifier;
	kStream << m_iMaxPlayerBuildingProductionModifier;
	kStream << m_iFreeExperience;
	kStream << m_iFreeExperienceFromBldgs;
	kStream << m_iFreeExperienceFromMinors;
	kStream << m_iFeatureProductionModifier;
	kStream << m_iWorkerSpeedModifier;
	kStream << m_iImprovementCostModifier;
	kStream << m_iImprovementUpgradeRateModifier;
	kStream << m_iSpecialistProductionModifier;
	kStream << m_iMilitaryProductionModifier;
	kStream << m_iSpaceProductionModifier;
	kStream << m_iCityDefenseModifier;
	kStream << m_iWonderProductionModifier;
	kStream << m_iSettlerProductionModifier;
	kStream << m_iCapitalSettlerProductionModifier;	// Version 11
	kStream << m_iUnitProductionMaintenanceMod;
	kStream << m_iPolicyCostBuildingModifier;		// Added in version 3
	kStream << m_iPolicyCostMinorCivModifier;
	kStream << m_iNumNukeUnits;
	kStream << m_iNumOutsideUnits;
	kStream << m_iBaseFreeUnits;
	kStream << m_iBaseFreeMilitaryUnits;
	kStream << m_iFreeUnitsPopulationPercent;
	kStream << m_iFreeMilitaryUnitsPopulationPercent;
	kStream << m_iGoldPerUnit;
	kStream << m_iGoldPerMilitaryUnit;
	kStream << m_iRouteGoldMaintenanceMod;
	kStream << m_iBuildingGoldMaintenanceMod;
	kStream << m_iUnitGoldMaintenanceMod;
	kStream << m_iUnitSupplyMod;
	kStream << m_iExtraUnitCost;
	kStream << m_iNumMilitaryUnits;
	kStream << m_iHappyPerMilitaryUnit;
	kStream << m_iHappinessToCulture;
	kStream << m_iHappinessToScience;
	kStream << m_iHalfSpecialistUnhappinessCount;
	kStream << m_iHalfSpecialistFoodCount;
	kStream << m_iMilitaryFoodProductionCount;
	kStream << m_iConscriptCount;
	kStream << m_iMaxConscript;
	kStream << m_iHighestUnitLevel;
	kStream << m_iOverflowResearch;
	kStream << m_iExpModifier;
	kStream << m_iExpInBorderModifier;
	kStream << m_iLevelExperienceModifier;
	kStream << m_iMinorQuestFriendshipMod;
	kStream << m_iMinorGoldFriendshipMod;
	kStream << m_iMinorFriendshipMinimum;
	kStream << m_iMinorFriendshipDecayMod;
	kStream << m_iMinorScienceAlliesCount;
	kStream << m_iMinorResourceBonusCount;
	kStream << m_iConversionTimer;
	kStream << m_iCapitalCityID;
	kStream << m_iCitiesLost;
	kStream << m_iMilitaryMight;
	kStream << m_iEconomicMight;
	kStream << m_iTurnMightRecomputed;
	kStream << m_iNewCityExtraPopulation;
	kStream << m_iFreeFoodBox;
	kStream << m_iPopulationScore;
	kStream << m_iLandScore;
	kStream << m_iWondersScore;
	kStream << m_iScoreFromFutureTech;
	kStream << m_iTechScore;
	kStream << m_iCombatExperience;
	kStream << m_iLifetimeCombatExperience;
	kStream << m_iBorderObstacleCount;
	kStream << m_iNextOperationID;
	kStream << m_iCostNextPolicy;
	kStream << m_iNumBuilders;
	kStream << m_iMaxNumBuilders;
	kStream << m_iCityStrengthMod;
	kStream << m_iCityGrowthMod;
	kStream << m_iCapitalGrowthMod;
	kStream << m_iNumPlotsBought;
	kStream << m_iPlotGoldCostMod;
	kStream << m_iPlotCultureCostModifier;
	kStream << m_iPlotCultureExponentModifier;
	kStream << m_iNumCitiesPolicyCostDiscount;
	kStream << m_iGarrisonFreeMaintenanceCount;
	kStream << m_iGarrisonedCityRangeStrikeModifier;
	kStream << m_iNumCitiesFreeCultureBuilding;
	kStream << m_iUnitPurchaseCostModifier;
	kStream << m_iAllFeatureProduction;
	kStream << m_iCityDistanceHighwaterMark;
	kStream << m_iOriginalCapitalX;
	kStream << m_iOriginalCapitalY;
	kStream << m_iNumWonders;
	kStream << m_iNumPolicies;
	kStream << m_iNumGreatPeople;
	kStream << m_iCityConnectionHappiness;
	kStream << m_iHolyCityID;
	kStream << m_iTurnsSinceSettledLastCity;
	kStream << m_iNumNaturalWondersDiscoveredInArea;
	kStream << m_iStrategicResourceMod;
	kStream << m_iSpecialistCultureChange;
	kStream << m_iGreatPeopleSpawnCounter;
	kStream << m_iFreeTechCount;
	kStream << m_iMedianTechPercentage;
	kStream << m_iNumFreePolicies;
	kStream << m_iNumFreePoliciesEver;
	kStream << m_iNumFreeGreatPeople;
	kStream << m_iLastSliceMoved;

	kStream << m_bHasBetrayedMinorCiv;
	kStream << m_bAlive;
	kStream << m_bEverAlive;
	kStream << m_bTurnActive;
	kStream << m_bAutoMoves;
	kStream << m_bEndTurn;
	kStream << m_bDynamicTurnsSimultMode;
	kStream << static_cast<bool>(m_bPbemNewTurn && GC.getGame().isPbem());
	kStream << m_bExtendedGame;
	kStream << m_bFoundedFirstCity;
	kStream << m_bStrike;
	kStream << m_bCramped;
	kStream << m_bLostCapital;
	kStream << m_eConqueror;
	kStream << m_bHasAdoptedStateReligion;
	kStream << m_bAlliesGreatPersonBiasApplied;

	kStream << m_eID;
	kStream << m_ePersonalityType;

	kStream << m_aiCityYieldChange;
	kStream << m_aiCoastalCityYieldChange;
	kStream << m_aiCapitalYieldChange;
	kStream << m_aiCapitalYieldPerPopChange;
	kStream << m_aiSeaPlotYield;
	kStream << m_aiYieldRateModifier;
	kStream << m_aiCapitalYieldRateModifier;
	kStream << m_aiExtraYieldThreshold;
	kStream << m_aiSpecialistExtraYield;
	kStream << m_aiProximityToPlayer;
	kStream << m_aiResearchAgreementCounter;   // Added in Version 2
	kStream << m_aiIncomingUnitTypes;

	kStream << m_strReligionKey;
	kStream << m_strScriptData;

	CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvPlayer::write");
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNumResourceUsed);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNumResourceTotal);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceGiftedToMinors);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceExport);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceImport);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiResourceFromMinors);

	kStream << m_paiImprovementCount;

	// Added for version 9
	FStringFixedBuffer(sTemp, 64);
	int iArraySize = GC.getNumBuildingInfos();
	kStream << iArraySize;
	for (int iI = 0; iI < iArraySize; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			sTemp = pkBuildingInfo->GetType();
			kStream << sTemp;
			kStream << m_paiFreeBuildingCount[iI];
		}
		else
		{
			sTemp = "NO_BUILDING";
			kStream << sTemp;
		}
	}

	// Added for version 9
	iArraySize = GC.getNumPromotionInfos();
	kStream << iArraySize;
	for (int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			sTemp = pkPromotionInfo->GetType();
			kStream << sTemp;
			kStream << m_paiFreePromotionCount[iI];
		}
	}

	kStream << m_paiUnitCombatProductionModifiers;
	kStream << m_paiUnitCombatFreeExperiences;
	kStream << m_paiUnitClassCount;
	kStream << m_paiUnitClassMaking;
	kStream << m_paiBuildingClassCount;
	kStream << m_paiBuildingClassMaking;
	kStream << m_paiProjectMaking;
	kStream << m_paiHurryCount;
	kStream << m_paiHurryModifier;


	CvAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::write");

	kStream << m_pabLoyalMember;

	kStream << m_pabGetsScienceFromPlayer;

	m_pPlayerPolicies->Write(kStream);
	m_pEconomicAI->Write(kStream);
	m_pCitySpecializationAI->Write(kStream);
	m_pWonderProductionAI->Write(kStream);
	m_pMilitaryAI->Write(kStream);
	m_pGrandStrategyAI->Write(kStream);
	m_pDiplomacyAI->Write(kStream);
	m_pPlayerTechs->Write(kStream);
	m_pFlavorManager->Write(kStream);
	m_pTacticalAI->Write(kStream);
	m_pHomelandAI->Write(kStream);
	m_pMinorCivAI->Write(kStream);
	m_pDealAI->Write(kStream);
	m_pBuilderTaskingAI->Write(kStream);
	m_pCityConnections->Write(kStream);
	m_pDangerPlots->Write(kStream);
	m_pTraits->Write(kStream);

	if (m_pNotifications)
	{
		kStream << true;
		m_pNotifications->Write(kStream);
	}
	else
	{
		kStream << false;
	}
	m_pTreasury->Write(kStream);

	kStream << m_ppaaiSpecialistExtraYield;
	kStream << m_ppaaiImprovementYieldChange;

	kStream << m_UnitCycle;
	kStream << m_researchQueue;

	kStream << m_bEverPoppedGoody;
	kStream << m_bEverTrainedBuilder;
	kStream << m_eEndTurnBlockingType;
	kStream << m_iEndTurnBlockingNotificationIndex;

	kStream << m_cityNames;
	kStream << m_cities;
	kStream << m_units;
	kStream << m_armyAIs;

	{
		uint iSize = m_AIOperations.size();
		kStream << iSize;
		std::map<int, CvAIOperation*>::const_iterator it;
		for (it = m_AIOperations.begin(); it != m_AIOperations.end(); ++it)
		{
			kStream << it->first;
			CvAIOperation* pThisOperation = it->second;
			kStream << pThisOperation->GetOperationType();
			pThisOperation->Write(kStream);
		}
	}

	// This used to be where the CvTalkingHeadMessages were output.  They were never used, but to keep compatibility, we'll write out a 0
	uint uiDummy = 0;
	kStream << uiDummy;

	{
		CvPopupQueue currentPopups;
		if (GC.getGame().isNetworkMultiPlayer())
		{
			// don't save open popups in MP to avoid having different state on different machines
			currentPopups.clear();
		}

		CvPopupQueue::_Alloc::size_type iSize = m_listPopups.size() + currentPopups.size();
		kStream << iSize;
		CvPopupQueue::const_iterator it;
		for (it = currentPopups.begin(); it != currentPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (NULL != pInfo)
			{
				kStream << *pInfo;
			}
		}
		for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (NULL != pInfo)
			{
				kStream << *pInfo;
			}
		}
	}

	kStream << m_ReplayDataSets;
	kStream << m_ReplayDataSetValues;

	kStream << m_aVote;
	kStream << m_aUnitExtraCosts;

	// writing out plot values
	{
		// trying to cut down how big saves are
		int iSize = -1;
		for (int i = m_aiPlots.size() - 1; i >= 0; i--)
		{
			if (m_aiPlots[i] != -1)
			{
				iSize = i + 1;
				break;
			}
		}

		if (iSize < 0)
		{
			iSize = 0;
		}

		kStream << iSize;
		for (int i = 0; i < iSize; i++)
		{
			kStream << m_aiPlots[i];
		}
	}

	kStream << m_iPopRushHurryCount;
	kStream << m_iTotalImprovementsBuilt;

	// writing out
	{
		int iSize = MAX_PLAYERS;
		kStream << iSize;
		for (int i = 0; i < iSize; i++)
		{
			bool bValue = m_bfEverConqueredBy.GetBit(i);
			kStream << bValue;
		}
	}

	kStream << m_strEmbarkedGraphicOverride;
}

//	--------------------------------------------------------------------------------
void CvPlayer::createGreatGeneral(UnitTypes eGreatPersonUnit, int iX, int iY)
{
	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY);
	if (NULL == pGreatPeopleUnit)
	{
		CvAssert(false);
		return;
	}

	ChangeNumGreatPeople(1);

	incrementGreatGeneralsCreated();

	changeGreatGeneralsThresholdModifier(/*50*/ GC.getGREAT_GENERALS_THRESHOLD_INCREASE() * ((getGreatGeneralsCreated() / 10) + 1));

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(/*50*/ GC.getGREAT_GENERALS_THRESHOLD_INCREASE_TEAM() * ((getGreatPeopleCreated() / 10) + 1));
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	//Achievements and Stats
	if(pGreatPeopleUnit->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		gDLL->IncrementSteamStat( ESTEAMSTAT_GREATGENERALS );
		const char* strLeader = GET_PLAYER(pGreatPeopleUnit->getOwner()).getLeaderTypeKey();
		if( strLeader && strcmp(strLeader, "LEADER_WU_ZETIAN") == 0 )
		{
			gDLL->UnlockAchievement( ACHIEVEMENT_SPECIAL_SUNTZU );
		}

		CvAchievementUnlocker::Check_PSG();
	}

	// Notification
	if (GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eGreatPersonUnit);
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::launch(VictoryTypes eVictory)
{
	CvTeam& kTeam = GET_TEAM(getTeam());

	if (!kTeam.canLaunch(eVictory))
	{
		return;
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::getUnitExtraCost(UnitClassTypes eUnitClass) const
{
	for (std::vector< std::pair<UnitClassTypes, int> >::const_iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnitClass)
		{
			return ((*it).second);
		}
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvPlayer::setUnitExtraCost(UnitClassTypes eUnitClass, int iCost)
{
	for (std::vector< std::pair<UnitClassTypes, int> >::iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnitClass)
		{
			if (0 == iCost)
			{
				m_aUnitExtraCosts.erase(it);
			}
			else
			{
				(*it).second = iCost;
			}
			return;
		}
	}

	if (0 != iCost)
	{
		m_aUnitExtraCosts.push_back(std::make_pair(eUnitClass, iCost));
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////

//	--------------------------------------------------------------------------------
void CvPlayer::invalidatePopulationRankCache()
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidatePopulationRankCache();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::invalidateYieldRankCache(YieldTypes)
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidateYieldRankCache();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::doUpdateCacheOnTurn()
{

}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	PlayerTypes eBestPlayer = kCity.getOriginalOwner();//kCity.getLiberationPlayer();

	if (NO_PLAYER != eBestPlayer)
	{
		CvPlayer& kBestPlayer = GET_PLAYER(eBestPlayer);

		if (kBestPlayer.getTeam() == getTeam())
		{
			return eBestPlayer;
		}
	}

	return GetID();
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canStealTech(PlayerTypes eTarget, TechTypes eTech) const
{
	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).GetTeamTechs()->HasTech(eTech))
	{
		if (GetPlayerTechs()->CanResearch(eTech))
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canSpyDestroyUnit(PlayerTypes, CvUnit& kUnit) const
{
	if (kUnit.getTeam() == getTeam())
	{
		return false;
	}

	if (kUnit.getUnitInfo().GetProductionCost() <= 0)
	{
		return false;
	}

	if (!kUnit.plot()->isVisible(getTeam()))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canSpyBribeUnit(PlayerTypes eTarget, CvUnit& kUnit) const
{
	if (!canSpyDestroyUnit(eTarget, kUnit))
	{
		return false;
	}

	// Can't buy units when at war
	if (kUnit.isEnemy(getTeam()))
	{
		return false;
	}

	// Can't buy units if they are not in a legal plot
	//if (!GET_TEAM(getTeam()).isFriendlyTerritory(GET_PLAYER(eTarget).getTeam()) && !GET_TEAM(getTeam()).isAllowsOpenBordersToTeam(GET_PLAYER(eTarget).getTeam()))
	//{
	//	return false;
	//}

	IDInfo* pUnitNode = kUnit.plot()->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = kUnit.plot()->nextUnitNode(pUnitNode);

		if (NULL != pLoopUnit && pLoopUnit != &kUnit)
		{
			if (pLoopUnit->isEnemy(getTeam()))
			{
				// If we buy the unit, we will be on the same plot as an enemy unit! Not good.
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canSpyDestroyBuilding(PlayerTypes, BuildingTypes eBuilding) const
{
	CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eBuilding);
	if(pkBuilding)
	{
		if (pkBuilding->GetProductionCost() <= 0)
		{
			return false;
		}

		if (::isLimitedWonderClass(pkBuilding->GetBuildingClassInfo()))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const
{
	CvProjectEntry& kProject = *GC.getProjectInfo(eProject);
	if (kProject.GetProductionCost() <= 0)
	{
		return false;
	}

	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getProjectCount(eProject) <= 0)
	{
		return false;
	}

	if (::isWorldProject(eProject))
	{
		return false;
	}

	if (!kProject.IsSpaceship())
	{
		return false;
	}
	else
	{
		VictoryTypes eVictory = (VictoryTypes)kProject.GetVictoryPrereq();
		if (NO_VICTORY != eVictory)
		{
			// Can't destroy spaceship components if we have already launched
			if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getVictoryCountdown(eVictory) >= 0)
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getNewCityProductionValue() const
{
	if (GC.getSETTLER_PRODUCTION_SPEED() != 0)
	{
		return GC.getSETTLER_PRODUCTION_SPEED();
	}

	int iValue = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iJ);
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(pkBuildingClassInfo)
		{
			const BuildingTypes eBuilding = ((BuildingTypes)(getCivilizationInfo().getCivilizationBuildings(iJ)));
			if (NO_BUILDING != eBuilding)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo)
				{
					if (pkBuildingInfo->GetFreeStartEra() != NO_ERA)
					{
						if (GC.getGame().getStartEra() >= pkBuildingInfo->GetFreeStartEra())
						{
							iValue += (100 * getProductionNeeded(eBuilding)) / std::max(1, 100 + getProductionModifier(eBuilding));
						}
					}
				}
			}
		}
	}

	iValue *= 100 + GC.getNEW_CITY_BUILDING_VALUE_MODIFIER();
	iValue /= 100;

	CvGame& kGame = GC.getGame();

	iValue += (GC.getADVANCED_START_CITY_COST() * kGame.getGameSpeedInfo().getGrowthPercent()) / 100;

	int iPopulation = GC.getINITIAL_CITY_POPULATION() + kGame.getStartEraInfo().getFreePopulation();
	for (int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getADVANCED_START_POPULATION_COST()) / 100;
	}

	return iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayer::getGrowthThreshold(int iPopulation) const
{
	CvAssertMsg(iPopulation > 0, "Population of city should be at least 1. Please show Jon this and send your last 5 autosaves.");

	int iThreshold;

	int iBaseThreshold = /*15*/ GC.getBASE_CITY_GROWTH_THRESHOLD();

	int iExtraPopThreshold = int((iPopulation-1) * /*6*/ GC.getCITY_GROWTH_MULTIPLIER());

	iBaseThreshold += iExtraPopThreshold;
	iExtraPopThreshold = (int) pow(double(iPopulation-1), (double) /*1.8*/ GC.getCITY_GROWTH_EXPONENT());

	iThreshold = iBaseThreshold + iExtraPopThreshold;

	if (isMinorCiv())
	{
		iThreshold *= GC.getMINOR_CIV_GROWTH_PERCENT();
		iThreshold /= 100;
	}

	iThreshold *= GC.getGame().getGameSpeedInfo().getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getGame().getStartEraInfo().getGrowthPercent();
	iThreshold /= 100;

	if (!isHuman() && !IsAITeammateOfHuman() && !isBarbarian())
	{
		iThreshold *= GC.getGame().getHandicapInfo().getAIGrowthPercent();
		iThreshold /= 100;

		iThreshold *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * GetCurrentEra()) + 100));
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}

//	--------------------------------------------------------------------------------
/// This sets up the m_aiPlots array that is used to contain which plots the player contains
void CvPlayer::InitPlots (void)
{
	int iNumPlots = GC.getMap().getGridHeight() * GC.getMap().getGridHeight();
	// in case we're loading
	if (iNumPlots != m_aiPlots.size())
	{
		m_aiPlots.clear();
		m_aiPlots.push_back_copy(-1, iNumPlots);
	}
}

//	--------------------------------------------------------------------------------
/// This determines what plots the player has under control
void CvPlayer::UpdatePlots (void)
{
	if (m_aiPlots.size() == 0) // not been inited
	{
		return;
	}

	int iPlotIndex = 0;
	int iMaxNumPlots = (int) m_aiPlots.size();
	while (iPlotIndex < iMaxNumPlots && m_aiPlots[iPlotIndex] != -1)
	{
		m_aiPlots[iPlotIndex] = -1;
		iPlotIndex++;
	}

	int iI;
	CvPlot* pLoopPlot;
	iPlotIndex = 0;
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for (iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
		if (pLoopPlot->getOwner() != m_eID)
		{
			continue;
		}

		m_aiPlots[iPlotIndex] = iI;
		iPlotIndex++;
	}
}

//	--------------------------------------------------------------------------------
/// Adds a plot at the end of the list
void CvPlayer::AddAPlot(CvPlot* pPlot)
{
	if (!pPlot)
	{
		return;
	}

	if (m_aiPlots.size() == 0) // not been inited
	{
		return;
	}

	if (pPlot->getOwner() == m_eID)
	{
		return;
	}

	int iPlotIndex = 0;
	int iMaxNumPlots = (int)m_aiPlots.size();
	while (iPlotIndex < iMaxNumPlots && m_aiPlots[iPlotIndex] != -1)
	{
		iPlotIndex++;
	}

	m_aiPlots[iPlotIndex] = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());

}

//	--------------------------------------------------------------------------------
/// Returns the list of the plots the player owns
CvPlotsVector& CvPlayer::GetPlots (void)
{
	return m_aiPlots;
}

//	--------------------------------------------------------------------------------
/// How many plots does this player own?
int CvPlayer::GetNumPlots() const
{
	int iNumPlots = 0;

	CvPlot* pLoopPlot;
	int iNumPlotsInEntireWorld = GC.getMap().numPlots();
	for (int iI = 0; iI < iNumPlotsInEntireWorld; iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

		if (pLoopPlot->getOwner() != m_eID)
			continue;

		iNumPlots++;
	}

	return iNumPlots;
}


//	--------------------------------------------------------------------------------
/// City strength mod (i.e. 100 = strength doubled)
int CvPlayer::GetCityStrengthMod() const
{
	return m_iCityStrengthMod;
}

//	--------------------------------------------------------------------------------
/// Sets City strength mod (i.e. 100 = strength doubled)
void CvPlayer::SetCityStrengthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCityStrengthMod = iValue;

	// Loop through all Cities and update their strength
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateStrengthValue();
	}
}

//	--------------------------------------------------------------------------------
/// Changes City strength mod (i.e. 100 = strength doubled)
void CvPlayer::ChangeCityStrengthMod(int iChange)
{
	if (iChange != 0)
	{
		SetCityStrengthMod(GetCityStrengthMod() + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// City growth percent mod (i.e. 100 = foodDifference doubled)
int CvPlayer::GetCityGrowthMod() const
{
	return m_iCityGrowthMod;
}

//	--------------------------------------------------------------------------------
/// Sets City growth percent mod (i.e. 100 = foodDifference doubled)
void CvPlayer::SetCityGrowthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCityGrowthMod = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes City growth percent mod (i.e. 100 = foodDifference doubled)
void CvPlayer::ChangeCityGrowthMod(int iChange)
{
	if (iChange != 0)
	{
		SetCityGrowthMod(GetCityGrowthMod() + iChange);
	}
}


//	--------------------------------------------------------------------------------
/// Capital growth percent mod (i.e. 100 = foodDifference doubled)
int CvPlayer::GetCapitalGrowthMod() const
{
	return m_iCapitalGrowthMod;
}

//	--------------------------------------------------------------------------------
/// Sets Capital growth percent mod (i.e. 100 = foodDifference doubled)
void CvPlayer::SetCapitalGrowthMod(int iValue)
{
	CvAssert(iValue >= 0);
	m_iCapitalGrowthMod = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes Capital growth percent mod (i.e. 100 = foodDifference doubled)
void CvPlayer::ChangeCapitalGrowthMod(int iChange)
{
	if (iChange != 0)
	{
		SetCapitalGrowthMod(GetCapitalGrowthMod() + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// How many Plot has this player bought (costs should ramp up as more are purchased)
int CvPlayer::GetNumPlotsBought() const
{
	return m_iNumPlotsBought;
}

//	--------------------------------------------------------------------------------
/// Sets how many Plot has this player bought (costs should ramp up as more are purchased)
void CvPlayer::SetNumPlotsBought(int iValue)
{
	CvAssert(iValue >= 0);
	m_iNumPlotsBought = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes how many Plot has this player bought (costs should ramp up as more are purchased)
void CvPlayer::ChangeNumPlotsBought(int iChange)
{
	if (iChange != 0)
	{
		SetNumPlotsBought(GetNumPlotsBought() + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// Gold cost of buying a new Plot
int CvPlayer::GetBuyPlotCost() const
{
	int iCost = /*50*/ GC.getPLOT_BASE_COST();
	iCost += (/*5*/ GC.getPLOT_ADDITIONAL_COST_PER_PLOT() * GetNumPlotsBought());

	// Cost Mod (Policies, etc.)
	if (GetPlotGoldCostMod() != 0)
	{
		iCost *= (100 + GetPlotGoldCostMod());
		iCost /= 100;
	}

	if (isMinorCiv())
	{
		iCost *= /*200*/ GC.getMINOR_CIV_GOLD_PERCENT();
		iCost /= 100;
	}

	return iCost;
}

//	--------------------------------------------------------------------------------
/// How much of a discount do we have for Plot buying
int CvPlayer::GetPlotGoldCostMod() const
{
	return m_iPlotGoldCostMod;
}

//	--------------------------------------------------------------------------------
/// Changes how much of a discount we have for Plot buying
void CvPlayer::ChangePlotGoldCostMod(int iChange)
{
	if (iChange != 0)
	{
		m_iPlotGoldCostMod += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// How much Culture is required for this City to acquire a new Plot
int CvPlayer::GetPlotCultureCostModifier() const
{
	return m_iPlotCultureCostModifier;
}

//	--------------------------------------------------------------------------------
/// Changes how much Culture is required for this City to acquire a new Plot
void CvPlayer::ChangePlotCultureCostModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iPlotCultureCostModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// How much do we dampen the exponent used to determine Culture needed for a City to acquire a new Plot?
int CvPlayer::GetPlotCultureExponentModifier() const
{
	return m_iPlotCultureExponentModifier;
}

//	--------------------------------------------------------------------------------
/// Changes how much we dampen the exponent used to determine Culture needed for a City to acquire a new Plot?
void CvPlayer::ChangePlotCultureExponentModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iPlotCultureExponentModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// How much do we dampen the growth of policy costs based on number of cities?
int CvPlayer::GetNumCitiesPolicyCostDiscount() const
{
	return m_iNumCitiesPolicyCostDiscount;
}

//	--------------------------------------------------------------------------------
/// Changes how much we dampen the growth of policy costs based on number of cities
void CvPlayer::ChangeNumCitiesPolicyCostDiscount(int iChange)
{
	if (iChange != 0)
	{
		m_iNumCitiesPolicyCostDiscount += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Do we save on unit maintenance for garrisons?
bool CvPlayer::IsGarrisonFreeMaintenance() const
{
	return m_iGarrisonFreeMaintenanceCount > 0;
}

//	--------------------------------------------------------------------------------
/// Changes setting on unit maintenance for garrisons
void CvPlayer::ChangeGarrisonFreeMaintenanceCount(int iChange)
{
	if (iChange != 0)
	{
		m_iGarrisonFreeMaintenanceCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Increase in city range strike due to garrison
int CvPlayer::GetGarrisonedCityRangeStrikeModifier() const
{
	return m_iGarrisonedCityRangeStrikeModifier;
}

//	--------------------------------------------------------------------------------
/// Changes increase in city range strike due to garrison
void CvPlayer::ChangeGarrisonedCityRangeStrikeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iGarrisonedCityRangeStrikeModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Cost of purchasing units modified?
int CvPlayer::GetUnitPurchaseCostModifier() const
{
	return m_iUnitPurchaseCostModifier;
}

//	--------------------------------------------------------------------------------
/// Cost of purchasing units modified?
void CvPlayer::ChangeUnitPurchaseCostModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iUnitPurchaseCostModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetPlotDanger(CvPlot & pPlot) const
{
	return m_pDangerPlots->GetDanger(pPlot);
}

//	--------------------------------------------------------------------------------
/// Find closest city to a plot (within specified search radius)
CvCity *CvPlayer::GetClosestFriendlyCity(CvPlot& plot, int iSearchRadius)
{
	CvCity *pClosestCity = NULL;
	CvCity* pLoopCity;
	int iBestDistance = INT_MAX;

	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iDistance = plotDistance(plot.getX(), plot.getY(), pLoopCity->getX(), pLoopCity->getY());
		if (iDistance < iBestDistance && iDistance <= iSearchRadius)
		{
			pClosestCity = pLoopCity;
			iBestDistance = iDistance;
		}
	}

	return pClosestCity;
}

//	--------------------------------------------------------------------------------
// How many Puppet Cities does this player own
int CvPlayer::GetNumPuppetCities() const
{
	int iNum = 0;

	const CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsPuppet())
		{
			iNum++;
		}
	}

	return iNum;
}

//	--------------------------------------------------------------------------------
/// How many Natural Wonders has this player found in its area?
int CvPlayer::GetNumNaturalWondersDiscoveredInArea() const
{
	return m_iNumNaturalWondersDiscoveredInArea;
}

//	--------------------------------------------------------------------------------
/// Sets how many Natural Wonders has this player found in its area
void CvPlayer::SetNumNaturalWondersDiscoveredInArea(int iValue)
{
	m_iNumNaturalWondersDiscoveredInArea = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes many Natural Wonders has this player found in its area
void CvPlayer::ChangeNumNaturalWondersDiscoveredInArea(int iChange)
{
	SetNumNaturalWondersDiscoveredInArea(GetNumNaturalWondersDiscoveredInArea() + iChange);
}

//	--------------------------------------------------------------------------------
/// How long ago did this guy last settle a city?
int CvPlayer::GetTurnsSinceSettledLastCity() const
{
	return m_iTurnsSinceSettledLastCity;
}

//	--------------------------------------------------------------------------------
/// How long ago did this guy last settle a city?
void CvPlayer::SetTurnsSinceSettledLastCity(int iValue)
{
	if (m_iTurnsSinceSettledLastCity != iValue)
		m_iTurnsSinceSettledLastCity = iValue;
}

//	--------------------------------------------------------------------------------
/// How long ago did this guy last settle a city?
void CvPlayer::ChangeTurnsSinceSettledLastCity(int iChange)
{
	if (iChange != 0)
		SetTurnsSinceSettledLastCity(GetTurnsSinceSettledLastCity() + iChange);
}

//	--------------------------------------------------------------------------------
/// Find best continents to settle next two cities; returns number found over minimum
int CvPlayer::GetBestSettleAreas(int iMinScore, int &iFirstArea, int &iSecondArea) const
{
	CvArea *pLoopArea;
	int iLoop;
	int iBestScore = -1;
	int iSecondBestScore = -1;
	int iBestArea = -1;
	int iSecondBestArea = -1;
	int iNumFound = 0;
	int iScore;

	CvMap& theMap = GC.getMap();

	// Find best two scores above minimum
	for (pLoopArea = theMap.firstArea(&iLoop); pLoopArea != NULL; pLoopArea = theMap.nextArea(&iLoop))
	{
		if (!pLoopArea->isWater())
		{
			// Found value divided by number of cities already on this landmass
			iScore = pLoopArea->getTotalFoundValue();

			if (iScore >= iMinScore)
			{
				if (iScore > iBestScore)
				{
					// Already have a best area?  If so demote to 2nd
					if (iBestScore > iMinScore)
					{
						iSecondBestScore = iBestScore;
						iSecondBestArea = iBestArea;
					}
					iBestArea = pLoopArea->GetID();
					iBestScore = iScore;
				}

				else if (iScore > iSecondBestScore)
				{
					iSecondBestArea = pLoopArea->GetID();
					iSecondBestScore = iScore;
				}
			}
		}
	}

	// Return data
	iFirstArea = iBestArea;
	iSecondArea = iSecondBestArea;

	if (iSecondArea != -1)
	{
		iNumFound = 2;
	}
	else if (iFirstArea != -1)
	{
		iNumFound = 1;
	}
	return iNumFound;
}

//	--------------------------------------------------------------------------------
/// Find the best spot in the entire world for this unit to settle
CvPlot* CvPlayer::GetBestSettlePlot(CvUnit* pUnit, bool bEscorted, int iArea) const
{
	if (!pUnit)
		return NULL;

	int iSettlerX = pUnit->getX();
	int iSettlerY = pUnit->getY();
	int iUnitArea = pUnit->getArea();
	PlayerTypes eOwner = pUnit->getOwner();
	TeamTypes eTeam = pUnit->getTeam();

	int iBestFoundValue = 0;
	CvPlot* pBestFoundPlot = NULL;

	int iEvalDistance = /*12*/ GC.getSETTLER_EVALUATION_DISTANCE();
	int iDistanceDropoffMod = /*99*/ GC.getSETTLER_DISTANCE_DROPOFF_MODIFIER();

	iEvalDistance += (GC.getGame().getGameTurn() * 5) / 100;

	// scale this based on world size
	const int iDefaultNumTiles = 80*52;
	int iDefaultEvalDistance = iEvalDistance;
	iEvalDistance = (iEvalDistance * GC.getMap().numPlots()) / iDefaultNumTiles;
	iEvalDistance = max(iDefaultEvalDistance,iEvalDistance);

	if (bEscorted && GC.getMap().GetAIMapHint() & 1) // this is primarily a naval map
	{
		iEvalDistance *= 3;
		iEvalDistance /= 2;
	}

	// Stay close to home if don't have an escort
	if (!bEscorted)
	{
		iEvalDistance /= 2;
	}

	CvMap& kMap = GC.getMap();
	int iNumPlots = kMap.numPlots();
	for (int iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
	{
		CvPlot* pPlot = kMap.plotByIndexUnchecked(iPlotLoop);

		if (!pPlot)
		{
			continue;
		}

		if (pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != eOwner)
		{
			continue;
		}

		if (!pPlot->isRevealed(getTeam()))
		{
			continue;
		}

		if (!pUnit->canFound(pPlot))
		{
			continue;
		}

		if (iArea != -1 && pPlot->getArea() != iUnitArea)
		{
			continue;
		}

		if (pPlot->IsAdjacentOwnedByOtherTeam(eTeam))
		{
			continue;
		}

		// Do we have to check if this is a safe place to go?
		if (bEscorted || (!pPlot->isVisibleEnemyUnit(eOwner)))
		{
			int iValue = pPlot->getFoundValue(eOwner);
			if (iValue > 5000)
			{
				int iSettlerDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), iSettlerX, iSettlerY);
				int iDistanceDropoff = min(99,(iDistanceDropoffMod * iSettlerDistance) / iEvalDistance);
				iDistanceDropoff = max(0,iDistanceDropoff);
				iValue = iValue * (100 - iDistanceDropoff) / 100;
				if (pPlot->getArea() != iUnitArea)
				{
					if (GC.getMap().GetAIMapHint() & 1) // this is primarily a naval map
					{
						iValue *= 3;
						iValue /= 2;
					}
					else
					{
						iValue *= 2;
						iValue /= 3;
					}
				}
				if (iValue > iBestFoundValue)
				{
					iBestFoundValue = iValue;
					pBestFoundPlot = pPlot;
				}
			}
		}
	}
	return pBestFoundPlot;
}


//	--------------------------------------------------------------------------------
/// How many Wonders has this Player constructed?
int CvPlayer::GetNumWonders() const
{
	return m_iNumWonders;
}

//	--------------------------------------------------------------------------------
/// Changes how many Wonders this Player has constructed
void CvPlayer::ChangeNumWonders(int iValue)
{
	if (iValue != 0)
	{
		m_iNumWonders += iValue;
	}
}

//	--------------------------------------------------------------------------------
/// How many Policies has this Player constructed?
int CvPlayer::GetNumPolicies() const
{
	return m_iNumPolicies;
}

//	--------------------------------------------------------------------------------
/// Changes how many Policies this Player has constructed
void CvPlayer::ChangeNumPolicies(int iValue)
{
	if (iValue != 0)
	{
		m_iNumPolicies += iValue;
	}
}

//	--------------------------------------------------------------------------------
/// How many GreatPeople has this Player constructed?
int CvPlayer::GetNumGreatPeople() const
{
	return m_iNumGreatPeople;
}

//	--------------------------------------------------------------------------------
/// Changes how many GreatPeople this Player has constructed
void CvPlayer::ChangeNumGreatPeople(int iValue)
{
	if (iValue != 0)
	{
		m_iNumGreatPeople += iValue;
	}
}

//	--------------------------------------------------------------------------------
/// Special ability where city-states gift great people
void CvPlayer::DoAdoptedGreatPersonCityStatePolicy()
{
	// Loop through all minors and if they're allies, seed the GP counter
	PlayerTypes eMinor;
	for (int iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eMinor = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(eMinor).isEverAlive())
		{
			if (GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly() == GetID())
			{
				DoSeedGreatPeopleSpawnCounter();

				break;
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Special ability where city-states gift great people
bool CvPlayer::IsAlliesGreatPersonBiasApplied() const
{
	return m_bAlliesGreatPersonBiasApplied;
}

//	--------------------------------------------------------------------------------
/// Special ability where city-states gift great people
void CvPlayer::SetAlliesGreatPersonBiasApplied(bool bValue)
{
	if (m_bAlliesGreatPersonBiasApplied != bValue)
	{
		m_bAlliesGreatPersonBiasApplied = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Has this player picked up a Religion yet
bool CvPlayer::IsHasAdoptedStateReligion() const
{
	return m_bHasAdoptedStateReligion;
}

//	--------------------------------------------------------------------------------
/// Sets this player picked up a Religion yet
void CvPlayer::SetHasAdoptedStateReligion(bool bValue)
{
	if (m_bHasAdoptedStateReligion != bValue)
	{
		m_bHasAdoptedStateReligion = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Number of Cities in the empire with our State Religion
int CvPlayer::GetNumCitiesWithStateReligion()
{
	int iNumCitiesWithStateReligion = 0;

	int iLoopCity;
	CvCity* pLoopCity = NULL;
	// Look at all of our Cities to see if they have our Religion
	for (pLoopCity = firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = nextCity(&iLoopCity))
	{
		if (pLoopCity->GetPlayersReligion() == GetID())
		{
			iNumCitiesWithStateReligion++;
		}
	}

	return iNumCitiesWithStateReligion;
}

//	--------------------------------------------------------------------------------
/// Where was this player's Religion adopted
CvCity* CvPlayer::GetHolyCity()
{
	return getCity(m_iHolyCityID);
}

//	--------------------------------------------------------------------------------
/// Sets where this player's Religion adopted
void CvPlayer::SetHolyCity(int iCityID)
{
	// This should only be set once under normal circumstances
	CvAssert(m_iHolyCityID == -1);

	m_iHolyCityID = iCityID;
}

//	--------------------------------------------------------------------------------
PromotionTypes CvPlayer::GetEmbarkationPromotion() const
{
	if (GET_TEAM(getTeam()).canDefensiveEmbark())
	{
		return (PromotionTypes)GC.getPROMOTION_DEFENSIVE_EMBARKATION();
	}

	if (m_pTraits)
	{
		if (m_pTraits->IsEmbarkedNotCivilian())
		{
			return (PromotionTypes)GC.getPROMOTION_DEFENSIVE_EMBARKATION();
		}

		if (m_pTraits->IsEmbarkedAllWater())
		{
			return (PromotionTypes)GC.getPROMOTION_ALLWATER_EMBARKATION();
		}
	}

	return (PromotionTypes)GC.getPROMOTION_EMBARKATION();
}

//	--------------------------------------------------------------------------------
/// Provide Notification about someone adopting a new Religon
void CvPlayer::DoAnnounceReligionAdoption()
{
	CvCity* pHolyCity = GetHolyCity();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if (thisPlayer.isHuman() && thisPlayer.isAlive() && thisPlayer.GetNotifications())
		{
			int iX = -1;
			int iY = -1;

			Localization::String localizedText;

			// Active Player
			if (GC.getGame().getActivePlayer() == GetID())
			{
				iX = pHolyCity->getX();
				iY = pHolyCity->getY();
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_YOU");
				localizedText << pHolyCity->getNameKey() << GetStateReligionKey();
			}
			// Met Player
			else if (GET_TEAM(GC.getGame().getActiveTeam()).isHasMet(thisPlayer.getTeam()))
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_ANOTHER_PLAYER");
				localizedText << GET_PLAYER(pHolyCity->getOwner()).getNameKey() << GetStateReligionKey();

				// We've seen this player's City
				if (pHolyCity->isRevealed(thisPlayer.getTeam(), false))
				{
					iX = pHolyCity->getX();
					iY = pHolyCity->getY();
				}
			}
			// Unmet Player
			else
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_RELIGION_ADOPTED_UNKNOWN");
			}

			thisPlayer.GetNotifications()->Add(NOTIFICATION_RELIGION_RACE, localizedText.toUTF8(), localizedText.toUTF8(), iX, iY, -1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Tutorial Stuff...
//////////////////////////////////////////////////////////////////////////

//	--------------------------------------------------------------------------------
bool CvPlayer::GetEverPoppedGoody ()
{
	return m_bEverPoppedGoody;
}

//	--------------------------------------------------------------------------------
CvPlot* CvPlayer::GetClosestGoodyPlot (bool bStopAfterFindingFirst)
{
	FFastVector<int> aiGoodyPlots = GetEconomicAI()->GetGoodyHutPlots();

	CvPlot* pResultPlot = NULL;
	int iShortestPath = INT_MAX;

	// cycle through goodies
	for (uint uiGoodyIndex = 0; uiGoodyIndex < aiGoodyPlots.size(); uiGoodyIndex++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aiGoodyPlots[uiGoodyIndex]);
		if (!pPlot || !pPlot->isGoody(getTeam()))
		{
			continue;
		}

		// cycle through units
		int iUnitLoop;
		CvUnit* pLoopUnit = NULL;

		// Setup m_units
		for(pLoopUnit = firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iUnitLoop))
		{
			if (!pLoopUnit)
			{
				continue;
			}

			if (pPlot->getArea() != pLoopUnit->getArea() && !pLoopUnit->CanEverEmbark())
			{
				continue;
			}

			int iReturnValue = INT_MAX;
			bool bResult = pLoopUnit->GeneratePath(pPlot, MOVE_UNITS_IGNORE_DANGER, true, &iReturnValue);

			if (bResult)
			{
				if (iReturnValue < iShortestPath)
				{
					pResultPlot = pPlot;
				}

				if (bStopAfterFindingFirst)
				{
					return pPlot;
				}
			}
		}
	}

	return pResultPlot;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::GetPlotHasOrder (CvPlot* pPlot)
{
	if (!pPlot)
	{
		return false;
	}

	int iLoopUnit;
	for (CvUnit* pLoopUnit = firstUnit(&iLoopUnit); pLoopUnit; pLoopUnit = nextUnit(&iLoopUnit))
	{
		CvPlot* pMissionPlot = pLoopUnit->GetMissionAIPlot();
		if (NULL != pMissionPlot && pMissionPlot->getX() == pPlot->getX() && pMissionPlot->getY() == pPlot->getY())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::GetAnyUnitHasOrderToGoody ()
{
	FFastVector<int> aiGoodyPlots = GetEconomicAI()->GetGoodyHutPlots();

	// cycle through goodies
	for (uint uiGoodyIndex = 0; uiGoodyIndex < aiGoodyPlots.size(); uiGoodyIndex++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aiGoodyPlots[uiGoodyIndex]);
		if (!pPlot)
		{
			continue;
		}

		if (!pPlot->isGoody(getTeam()))
		{
			continue;
		}

		if (GetPlotHasOrder(pPlot))
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::GetEverTrainedBuilder ()
{
	return m_bEverTrainedBuilder;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNumFreeTechs() const
{
	return m_iFreeTechCount;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetNumFreeTechs(int iValue)
{
	m_iFreeTechCount = iValue;
	if (GetID() == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(ResearchButtons_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetMedianTechPercentage() const
{
	return m_iMedianTechPercentage;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeMedianTechPercentage(int iValue)
{
	m_iMedianTechPercentage += iValue;
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNumFreePolicies() const
{
	return m_iNumFreePolicies;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetNumFreePolicies(int iValue)
{
	// Increase count of free Policies we've ever had
	int iDifference = iValue - m_iNumFreePolicies;
	if (iDifference > 0)
	{
		ChangeNumFreePoliciesEver(iDifference);
	}

	m_iNumFreePolicies = iValue;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeNumFreePolicies(int iChange)
{
	SetNumFreePolicies(GetNumFreePolicies() + iChange);

	if (iChange > 0)
	{
		CvNotifications* pNotifications = GetNotifications();
		if (pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FREE_POLICY_SUMMARY");
			pNotifications->Add(NOTIFICATION_FREE_POLICY, strBuffer, strSummary, -1, -1, -1);
		}

	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNumFreeGreatPeople() const
{
	return m_iNumFreeGreatPeople;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetNumFreeGreatPeople(int iValue)
{
	m_iNumFreeGreatPeople = iValue;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeNumFreeGreatPeople(int iChange)
{
	m_iNumFreeGreatPeople = GetNumFreeGreatPeople() + iChange;
	if(iChange > 0)
	{
		if(isHuman())
		{
			CvNotifications* pNotifications = GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_CHOOSE_FREE_GREAT_PERSON");
				CvString strSummary = GetLocalizedText("TXT_KEY_CHOOSE_FREE_GREAT_PERSON_TT");
				pNotifications->Add(NOTIFICATION_FREE_GREAT_PERSON, strSummary.c_str(), strBuffer.c_str(), -1, -1, -1);
			}
		}
		else
		{
			for (int iI = 0; iI < iChange; iI++)
			{
				AI_chooseFreeGreatPerson();
			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetNumFreePoliciesEver() const
{
	return m_iNumFreePoliciesEver;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetNumFreePoliciesEver(int iValue)
{
	m_iNumFreePoliciesEver = iValue;
}

//	--------------------------------------------------------------------------------
void CvPlayer::ChangeNumFreePoliciesEver(int iChange)
{
	SetNumFreePoliciesEver(GetNumFreePoliciesEver() + iChange);
}

//	--------------------------------------------------------------------------------
int CvPlayer::GetLastSliceMoved() const
{
	return m_iLastSliceMoved;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetLastSliceMoved(int iValue)
{
	m_iLastSliceMoved = iValue;
}

//	--------------------------------------------------------------------------------
void CvPlayer::SetEverConqueredBy(PlayerTypes ePlayer, bool bValue)
{
	if (bValue)
	{
		m_bfEverConqueredBy.SetBit(ePlayer);
	}
	else
	{
		m_bfEverConqueredBy.ClearBit(ePlayer);
	}
}

//	--------------------------------------------------------------------------------
bool CvPlayer::IsEverConqueredBy(PlayerTypes ePlayer)
{
	return m_bfEverConqueredBy.GetBit(ePlayer);
}

//	------------------------------------------------------------------------------------------------
const FAutoArchive & CvPlayer::getSyncArchive() const
{
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
FAutoArchive & CvPlayer::getSyncArchive()
{
	return m_syncArchive;
}

//	-----------------------------------------------------------------------------------------------
bool CvPlayer::isLocalPlayer() const
{
	return (GC.getGame().getActivePlayer() == GetID());
}

//	-----------------------------------------------------------------------------------------------
void CvPlayer::disconnected()
{
	bool isMultiplayer = GC.getGame().isGameMultiPlayer();
	if (isMultiplayer && isHuman() && !isLocalPlayer())
	{
		//log message for debugging the occasional lack of disconnect notification when Steam p2p connections timeout. - bolson 1/10/13
		FILogFile* logFile = LOGFILEMGR.GetLog("net_message_debug.log", 0);
		if(logFile)
		{
			logFile->DebugMsg("Attempted to post notification for player disconnect event.  Player(%i)", GetID());
		}

			CvNotifications* pNotifications = GET_PLAYER(GC.getGame().getActivePlayer()).GetNotifications();
			if(pNotifications){
				if(gDLL->IsPlayerKicked(GetID())){
					Localization::String kickedMsg = Localization::Lookup("TXT_KEY_PLAYER_KICKED");
					kickedMsg << getNameKey();
					pNotifications->Add(NOTIFICATION_PLAYER_KICKED, kickedMsg.toUTF8(), kickedMsg.toUTF8(), -1, -1, GetID());
				}
				else{
					Localization::String disconnectString = Localization::Lookup("TXT_KEY_PLAYER_DISCONNECTED");
					disconnectString << getNameKey();

					if(CvPreGame::isPitBoss()){
						disconnectString = Localization::Lookup("TXT_KEY_PLAYER_DISCONNECTED_PITBOSS");
						disconnectString << getNameKey();	
					}

					pNotifications->Add(NOTIFICATION_PLAYER_DISCONNECTED, disconnectString.toUTF8(), disconnectString.toUTF8(), -1, -1, GetID());
				}
			}

		if(!isObserver() && (!CvPreGame::isPitBoss() || gDLL->IsPlayerKicked(GetID())))
		{
			// JAR : First pass, automatically fall back to CPU so the
			// game can continue. Todo : add popup on host asking whether
			// the AI should take over or everyone should wait for the
			// player to reconnect
			CvPreGame::setSlotStatus(GetID(), SS_COMPUTER);
			CvPreGame::VerifyHandicap(GetID());	//Changing the handicap because we're switching to AI

			// Load leaderhead for this new AI player
			gDLL->NotifySpecificAILeaderInGame(GetID());
			
			if(!GC.getGame().isOption(GAMEOPTION_DYNAMIC_TURNS) && GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS))
			{//When in fully simultaneous turn mode, having a player disconnect might trigger the automove phase for all human players.
				checkRunAutoMovesForEveryone();
			}
		}
	}
}
//	-----------------------------------------------------------------------------------------------
void CvPlayer::reconnected()
{
	//Preserve observer status for the connecting human player's slot.
	if(CvPreGame::slotStatus(GetID()) != SS_OBSERVER){
		CvPreGame::setSlotStatus(GetID(), SS_TAKEN);
	}

	CvPreGame::VerifyHandicap(GetID()); //verify the handicap because we might have replaced an ai.

	CvGame& kGame = GC.getGame();
	bool isMultiplayer = kGame.isGameMultiPlayer();
	if (isMultiplayer && !isLocalPlayer())
	{
		FAutoArchive & archive = getSyncArchive();
		archive.clearDelta();

		Localization::String connectString = Localization::Lookup("TXT_KEY_PLAYER_CONNECTING");
		connectString << getNameKey();

		CvNotifications* pNotifications = GET_PLAYER(kGame.getActivePlayer()).GetNotifications();
		if (pNotifications)
		{
			pNotifications->Add(NOTIFICATION_PLAYER_CONNECTING, connectString.toUTF8(), connectString.toUTF8(), -1, -1, GetID());
		}
	}
}
//	-----------------------------------------------------------------------------------------------
bool CvPlayer::hasBusyUnitUpdatesRemaining() const
{
	return m_endTurnBusyUnitUpdatesLeft > 0;
}

//	-----------------------------------------------------------------------------------------------
void CvPlayer::setBusyUnitUpdatesRemaining(int iUpdateCount)
{
	m_endTurnBusyUnitUpdatesLeft = iUpdateCount;
}

//	-----------------------------------------------------------------------------------------------
const char * const CvPlayer::getNickName() const
{
	return CvPreGame::nicknameDisplayed(GetID()).c_str();
}

//	-----------------------------------------------------------------------------------------------
bool CvPlayer::hasUnitsThatNeedAIUpdate() const
{
	const CvUnit* pLoopUnit;
	int iLoop;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (!pLoopUnit->TurnProcessed() &&
			(pLoopUnit->IsAutomated() &&
			pLoopUnit->AI_getUnitAIType() != UNITAI_UNKNOWN &&
			pLoopUnit->canMove()))
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
std::string CvPlayer::debugDump(const FAutoVariableBase&) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	return result;
}

//	--------------------------------------------------------------------------------
std::string CvPlayer::stackTraceRemark(const FAutoVariableBase & var) const
{
	std::string result = debugDump(var);
	if(&var == &m_aOptions)
	{//detail output for player options array.
		result += "\nPlayer Options:";
		for (PlayerOptionsVector::const_iterator itr = m_aOptions.begin(); itr != m_aOptions.end(); ++itr )
		{
			CvString curOptionsStr;
			curOptionsStr.Format("\n%u, %d", itr->first, itr->second);
			result += curOptionsStr;
		}
		result += "\n";
	}
	return result;
}

//	--------------------------------------------------------------------------------
bool CvPlayer::hasTurnTimerExpired()
{//Indicates if this player's turn time has elapsed.
	return GC.getGame().hasTurnTimerExpired(GetID());
}

//	--------------------------------------------------------------------------------
void CvPlayer::doArmySize()
{
	int numUnits = 0;
	int32 nLargestArmy = 0;
	int iI;

	for(iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if( (UnitAITypes)iI == UNITAI_ARTIST ||(UnitAITypes)iI == UNITAI_ENGINEER || (UnitAITypes)iI == UNITAI_UNKNOWN ||
			(UnitAITypes)iI == UNITAI_GENERAL ||  (UnitAITypes)iI == UNITAI_SETTLE || (UnitAITypes)iI == UNITAI_WORKER ||
			(UnitAITypes)iI == UNITAI_SCIENTIST || (UnitAITypes)iI == UNITAI_MERCHANT || (UnitAITypes)iI == UNITAI_WORKER_SEA ||
			(UnitAITypes)iI == UNITAI_SPACESHIP_PART || (UnitAITypes)iI == UNITAI_TREASURE)
		{
			continue;
		}
		else
		{
			numUnits += GetNumUnitsWithUnitAI( (UnitAITypes)iI, false, true);
		}
	}
	gDLL->GetSteamStat( ESTEAMSTAT_STANDINGARMY, &nLargestArmy );

	if (nLargestArmy < numUnits)
	{
		gDLL->SetSteamStat( ESTEAMSTAT_STANDINGARMY, numUnits );

		CvAchievementUnlocker::Check_PSG();
	}
}

//	--------------------------------------------------------------------------------
void CvPlayer::checkInitialTurnAIProcessed()
{
	int turn = GC.getGame().getGameTurn();
	if(m_lastGameTurnInitialAIProcessed != turn)
	{
		//Note: Players that are not turn active at the beginning of the game turn will 
		//process their AI when they are turn active.  However, they should still 
		//act like their initial AI has been processed.
		if(!isTurnActive() || !hasUnitsThatNeedAIUpdate())
		{
			m_lastGameTurnInitialAIProcessed = turn;
			if(GC.getGame().getActivePlayer() == GetID())
				gDLL->sendPlayerInitialAIProcessed();
		}
	}
}

//------------------------------------------------------------------------------
void CvPlayer::GatherPerTurnReplayStats(int iGameTurn)
{
	cvStopWatch watch("Replay Stat Recording");

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "GatherPerTurnReplayStats", args.get(), bResult);
	}

	//Only record the following statistics if the player is alive.
	if(isAlive())
	{
		//	Production Per Turn
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_PRODUCTIONPERTURN"), iGameTurn, calculateTotalYield(YIELD_PRODUCTION));
		// 	Gold
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALGOLD"), iGameTurn, GetTreasury()->GetGold());
		// 	Gold per Turn
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDPERTURN"), iGameTurn, calculateTotalYield(YIELD_GOLD));
		// 	Num Cities
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_CITYCOUNT"), iGameTurn, getNumCities());

		//	Number of Techs known
		CvTeam& team = GET_TEAM(getTeam());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TECHSKNOWN"), iGameTurn, team.GetTeamTechs()->GetNumTechsKnown());

		// 	Science per Turn
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_SCIENCEPERTURN"), iGameTurn, calculateTotalYield(YIELD_SCIENCE));

		// 	Total Culture
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALCULTURE"), iGameTurn, getJONSCulture());

		// 	Culture per turn
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_CULTUREPERTURN"), iGameTurn, GetTotalJONSCulturePerTurn());

		// 	Happiness
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_EXCESSHAPINESS"), iGameTurn, GetExcessHappiness());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_HAPPINESS"), iGameTurn, GetHappiness());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_UNHAPPINESS"), iGameTurn, GetUnhappiness());

		// 	Golden Age turns
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GOLDENAGETURNS"), iGameTurn, getGoldenAgeTurns());

		// 	Population
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_POPULATION"), iGameTurn, getTotalPopulation());
		// 	Food Per Turn
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_FOODPERTURN"), iGameTurn, calculateTotalYield(YIELD_FOOD));

		//	Total Land
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_TOTALLAND"), iGameTurn, getTotalLand());

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GPTCITYCONNECTIONS"), iGameTurn, GetTreasury()->GetCityConnectionGold());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_GPTDEALS"), iGameTurn, GetTreasury()->GetGoldPerTurnFromDiplomacy());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_UNITMAINTENANCE"), iGameTurn, GetTreasury()->GetExpensePerTurnUnitMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_BUILDINGMAINTENANCE"), iGameTurn, GetTreasury()->GetBuildingGoldMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_IMPROVEMENTMAINTENANCE"), iGameTurn, GetTreasury()->GetImprovementGoldMaintenance());
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFPOLICIES"), iGameTurn, GetPlayerPolicies()->GetNumPoliciesOwned());

		// workers
		int iWorkerCount = 0;
		CvUnit* pLoopUnit;
		int iLoopUnit;
		for (pLoopUnit = firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				iWorkerCount++;
			}
		}
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFWORKERS"), iGameTurn, iWorkerCount);


		// go through all the plots the player has under their control
		CvPlotsVector& aiPlots = GetPlots();

		// worked tiles
		int iWorkedTiles = 0;
		int iImprovedTiles = 0;
		for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
		{
			// when we encounter the first plot that is invalid, the rest of the list will be invalid
			if (aiPlots[uiPlotIndex] == -1)
			{
				break;
			}

			CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
			if (!pPlot)
			{
				continue;
			}

			// plot has city in it, don't count
			if (pPlot->getPlotCity())
			{
				continue;
			}

			if (pPlot->isBeingWorked())
			{
				iWorkedTiles++;
			}

			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iImprovedTiles++;
			}
		}

		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_IMPROVEDTILES"), iGameTurn, iImprovedTiles);
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_WORKEDTILES"), iGameTurn, iWorkedTiles);
		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_NUMBEROFWORKERS"), iGameTurn, iWorkerCount);


		setReplayDataValue(getReplayDataSetIndex("REPLAYDATASET_MILITARYMIGHT"), iGameTurn, GetMilitaryMight());
	}
}

//	---------------------------------------------------------------------------
//	If the active player is in the end-turn processing phase, attempt to cancel that.
//	This should be called when something occurs that could happen during the end-turn
//	that may need the players attention this turn.
//	Ex.  A player's unit auto-moves into a goody hut plot.
bool CancelActivePlayerEndTurn()
{
	CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());

	if(kActivePlayer.isLocalPlayer() && kActivePlayer.isEndTurn())
	{
		if (gDLL->sendTurnUnready())	// This will see if we can actually do the unready, sometimes you can't in MP games.
		{
			kActivePlayer.setEndTurn(false);
			return true;
		}
		return false;
	}
	return true;
}
