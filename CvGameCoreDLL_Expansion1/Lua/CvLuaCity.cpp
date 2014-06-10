/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


using namespace CvLuaArgs;

//------------------------------------------------------------------------------
void CvLuaCity::PushMethods(lua_State* L, int t)
{
	Method(IsNone);
	Method(Kill);

	Method(CreateGreatGeneral);
	Method(CreateGreatAdmiral);

	Method(DoTask);
	Method(ChooseProduction);
	Method(GetCityPlotIndex);
	Method(GetCityIndexPlot);
	Method(CanWork);
	Method(IsPlotBlockaded);
	Method(ClearWorkingOverride);
	Method(CountNumImprovedPlots);
	Method(CountNumWaterPlots);
	Method(CountNumRiverPlots);

	Method(FindPopulationRank);
	Method(FindBaseYieldRateRank);
	Method(FindYieldRateRank);

	Method(AllUpgradesAvailable);
	Method(IsWorldWondersMaxed);
	Method(IsTeamWondersMaxed);
	Method(IsNationalWondersMaxed);
	Method(IsBuildingsMaxed);

	Method(CanTrainTooltip);
	Method(CanTrain);
	Method(CanConstructTooltip);
	Method(CanConstruct);
	Method(CanCreate);
	Method(CanPrepare);
	Method(CanMaintain);

	Method(GetPurchaseUnitTooltip);
	Method(GetFaithPurchaseUnitTooltip);
	Method(GetPurchaseBuildingTooltip);
	Method(GetFaithPurchaseBuildingTooltip);

	Method(CanJoin);
	Method(IsBuildingLocalResourceValid);

	Method(GetResourceDemanded);
	Method(SetResourceDemanded);
	Method(DoPickResourceDemanded);

	Method(GetFoodTurnsLeft);
	Method(IsProduction);
	Method(IsProductionLimited);
	Method(IsProductionUnit);
	Method(IsProductionBuilding);
	Method(IsProductionProject);
	Method(IsProductionSpecialist);
	Method(IsProductionProcess);

	Method(CanContinueProduction);
	Method(GetProductionExperience);
	Method(AddProductionExperience);

	Method(GetProductionUnit);
	Method(GetProductionUnitAI);
	Method(GetProductionBuilding);
	Method(GetProductionProject);
	Method(GetProductionSpecialist);
	Method(GetProductionProcess);
	//Method(GetProductionName);
	Method(GetProductionNameKey);
	Method(GetGeneralProductionTurnsLeft);
	Method(IsFoodProduction);
	Method(GetFirstUnitOrder);
	Method(GetFirstProjectOrder);
	Method(GetFirstSpecialistOrder);

	Method(GetOrderFromQueue);

	Method(GetNumTrainUnitAI);
	Method(GetFirstBuildingOrder);
	Method(IsUnitFoodProduction);
	Method(GetProduction);
	Method(GetProductionTimes100);
	Method(GetProductionNeeded);
	Method(GetUnitProductionNeeded);
	Method(GetBuildingProductionNeeded);
	Method(GetProjectProductionNeeded);
	Method(GetProductionTurnsLeft);
	Method(GetUnitProductionTurnsLeft);
	Method(GetBuildingProductionTurnsLeft);
	Method(GetProjectProductionTurnsLeft);
	Method(GetSpecialistProductionTurnsLeft);

	Method(CreateApolloProgram);

	Method(IsCanPurchase);
	Method(GetUnitPurchaseCost);
	Method(GetUnitFaithPurchaseCost);
	Method(GetBuildingPurchaseCost);
	Method(GetBuildingFaithPurchaseCost);
	Method(GetProjectPurchaseCost);

	Method(SetProduction);
	Method(ChangeProduction);

	Method(GetYieldModifierTooltip);
	Method(GetProductionModifier);

	Method(GetCurrentProductionDifference);
	Method(GetRawProductionDifference);
	Method(GetCurrentProductionDifferenceTimes100);
	Method(GetRawProductionDifferenceTimes100);
	Method(GetUnitProductionModifier);
	Method(GetBuildingProductionModifier);
	Method(GetProjectProductionModifier);
	Method(GetSpecialistProductionModifier);

	Method(GetExtraProductionDifference);

	Method(CanHurry);
	Method(Hurry);
	Method(GetConscriptUnit);
	Method(GetConscriptPopulation);
	Method(ConscriptMinCityPopulation);
	Method(CanConscript);
	Method(Conscript);
	Method(GetResourceYieldRateModifier);
	Method(GetHandicapType);
	Method(GetCivilizationType);
	Method(GetPersonalityType);
	Method(GetArtStyleType);
	Method(GetCitySizeType);

	Method(IsBarbarian);
	Method(IsHuman);
	Method(IsVisible);

	Method(IsCapital);
	Method(IsOriginalCapital);
	Method(IsCoastal);

	Method(FoodConsumption);
	Method(FoodDifference);
	Method(FoodDifferenceTimes100);
	Method(GrowthThreshold);
	Method(ProductionLeft);
	Method(HurryCost);
	Method(HurryGold);
	Method(HurryPopulation);
	Method(HurryProduction);
	Method(MaxHurryPopulation);

	Method(GetNumBuilding);
	Method(IsHasBuilding);
	Method(GetNumActiveBuilding);
	Method(GetID);
	Method(GetX);
	Method(GetY);
	Method(At);
	Method(AtPlot);
	Method(Plot);
	Method(Area);
	Method(WaterArea);
	Method(GetRallyPlot);

	Method(CanBuyPlot);
	Method(CanBuyPlotAt);
	Method(GetNextBuyablePlot);
	Method(GetBuyablePlotList);
	Method(GetBuyPlotCost);
	Method(CanBuyAnyPlot);

	Method(GetGarrisonedUnit);

	Method(GetGameTurnFounded);
	Method(GetGameTurnAcquired);
	Method(GetGameTurnLastExpanded);
	Method(GetPopulation);
	Method(SetPopulation);
	Method(ChangePopulation);
	Method(GetRealPopulation);

	Method(GetHighestPopulation);
	Method(SetHighestPopulation);
	//Method(GetWorkingPopulation);
	//Method(GetSpecialistPopulation);
	Method(GetNumGreatPeople);
	Method(GetBaseGreatPeopleRate);
	Method(GetGreatPeopleRate);
	Method(GetTotalGreatPeopleRateModifier);
	Method(ChangeBaseGreatPeopleRate);
	Method(GetGreatPeopleRateModifier);
	Method(GetGreatPeopleProgress);
	Method(ChangeGreatPeopleProgress);

	Method(GetJONSCultureStored);
	Method(SetJONSCultureStored);
	Method(ChangeJONSCultureStored);
	Method(GetJONSCultureLevel);
	Method(SetJONSCultureLevel);
	Method(ChangeJONSCultureLevel);
	Method(DoJONSCultureLevelIncrease);
	Method(GetJONSCultureThreshold);

	Method(GetJONSCulturePerTurn);

	Method(GetBaseJONSCulturePerTurn);
	Method(GetJONSCulturePerTurnFromBuildings);
	Method(ChangeJONSCulturePerTurnFromBuildings);
	Method(GetJONSCulturePerTurnFromPolicies);
	Method(ChangeJONSCulturePerTurnFromPolicies);
	Method(GetJONSCulturePerTurnFromSpecialists);
	Method(ChangeJONSCulturePerTurnFromSpecialists);
	Method(GetJONSCulturePerTurnFromTraits);
	Method(GetJONSCulturePerTurnFromReligion);

	Method(GetCultureRateModifier);
	Method(ChangeCultureRateModifier);

	Method(GetFaithPerTurn);
	Method(GetFaithPerTurnFromBuildings);
	Method(GetFaithPerTurnFromPolicies);
	Method(GetFaithPerTurnFromTraits);
	Method(GetFaithPerTurnFromReligion);

	Method(IsReligionInCity);
	Method(IsHolyCityForReligion);
	Method(IsHolyCityAnyReligion);

	Method(GetNumFollowers);
	Method(GetReligiousMajority);
	Method(GetFavoredReligion);
	Method(GetPressurePerTurn);
	Method(ConvertPercentFollowers);
	Method(AdoptReligionFully);
	Method(GetReligionBuildingClassHappiness);
	Method(GetReligionBuildingClassYieldChange);

	Method(GetNumWorldWonders);
	Method(GetNumTeamWonders);
	Method(GetNumNationalWonders);
	Method(GetNumBuildings);

	Method(GetWonderProductionModifier);
	Method(ChangeWonderProductionModifier);

	Method(GetLocalResourceWonderProductionMod);

	Method(ChangeHealRate);

	Method(IsNoOccupiedUnhappiness);

	Method(GetFood);
	Method(GetFoodTimes100);
	Method(SetFood);
	Method(ChangeFood);
	Method(GetFoodKept);
	Method(GetMaxFoodKeptPercent);
	Method(GetOverflowProduction);
	Method(SetOverflowProduction);
	Method(GetFeatureProduction);
	Method(SetFeatureProduction);
	Method(GetMilitaryProductionModifier);
	Method(GetSpaceProductionModifier);
	Method(GetBuildingDefense);
	Method(GetFreeExperience);
	Method(GetCurrAirlift);
	Method(GetMaxAirlift);
	Method(GetAirModifier);
	Method(GetNukeModifier);
	//Method(GetFreeSpecialist);

	Method(IsResistance);
	Method(GetResistanceTurns);
	Method(ChangeResistanceTurns);

	Method(IsRazing);
	Method(GetRazingTurns);
	Method(ChangeRazingTurns);

	Method(IsOccupied);
	Method(SetOccupied);

	Method(IsPuppet);
	Method(SetPuppet);

	Method(GetHappinessFromBuildings);
	Method(GetHappiness);
	Method(GetLocalHappiness);

	Method(IsNeverLost);
	Method(SetNeverLost);
	Method(IsDrafted);
	Method(SetDrafted);
	Method(IsAirliftTargeted);
	Method(SetAirliftTargeted);

	Method(IsBlockaded);

	Method(GetWeLoveTheKingDayCounter);
	Method(SetWeLoveTheKingDayCounter);
	Method(ChangeWeLoveTheKingDayCounter);

	Method(GetNumThingsProduced);

	Method(IsProductionAutomated);
	Method(SetProductionAutomated);
	Method(SetCitySizeBoost);
	Method(GetOwner);
	Method(GetTeam);
	Method(GetPreviousOwner);
	Method(GetOriginalOwner);
	Method(GetSeaPlotYield);
	Method(GetRiverPlotYield);
	Method(GetLakePlotYield);

	Method(GetBaseYieldRate);

	Method(GetBaseYieldRateFromTerrain);
	Method(ChangeBaseYieldRateFromTerrain);

	Method(GetBaseYieldRateFromBuildings);
	Method(ChangeBaseYieldRateFromBuildings);

	Method(GetBaseYieldRateFromSpecialists);
	Method(ChangeBaseYieldRateFromSpecialists);

	Method(GetBaseYieldRateFromMisc);
	Method(ChangeBaseYieldRateFromMisc);

	Method(GetBaseYieldRateFromReligion);
	Method(ChangeBaseYieldRateFromReligion);

	Method(GetYieldPerPopTimes100);

	Method(GetBaseYieldRateModifier);
	Method(GetYieldRate);
	Method(GetYieldRateTimes100);
	Method(GetYieldRateModifier);

	Method(GetExtraSpecialistYield);
	Method(GetExtraSpecialistYieldOfType);

	Method(GetDomainFreeExperience);
	Method(GetDomainProductionModifier);

	Method(IsEverOwned);

	Method(IsRevealed);
	Method(SetRevealed);
	Method(GetNameKey);
	Method(GetName);
	Method(SetName);
	Method(IsHasResourceLocal);
	Method(GetBuildingProduction);
	Method(SetBuildingProduction);
	Method(ChangeBuildingProduction);
	Method(GetBuildingProductionTime);
	Method(SetBuildingProductionTime);
	Method(ChangeBuildingProductionTime);
	Method(GetBuildingOriginalOwner);
	Method(GetBuildingOriginalTime);
	Method(GetUnitProduction);
	Method(SetUnitProduction);
	Method(ChangeUnitProduction);
	Method(GetGreatPeopleUnitRate);
	Method(GetGreatPeopleUnitProgress);
	Method(SetGreatPeopleUnitProgress);
	Method(ChangeGreatPeopleUnitProgress);

	Method(IsCanAddSpecialistToBuilding);
	Method(GetSpecialistUpgradeThreshold);
	Method(GetNumSpecialistsAllowedByBuilding);
	Method(GetSpecialistCount);
	Method(GetSpecialistGreatPersonProgress);
	Method(GetSpecialistGreatPersonProgressTimes100);
	Method(ChangeSpecialistGreatPersonProgressTimes100);
	Method(GetNumSpecialistsInBuilding);
	Method(DoReallocateCitizens);
	Method(DoVerifyWorkingPlots);
	Method(IsNoAutoAssignSpecialists);

	Method(GetFocusType);
	Method(SetFocusType);

	Method(IsForcedAvoidGrowth);

	Method(GetUnitCombatFreeExperience);
	Method(GetFreePromotionCount);
	Method(IsFreePromotion);
	Method(GetSpecialistFreeExperience);

	Method(UpdateStrengthValue);
	Method(GetStrengthValue);

	Method(GetDamage);
	Method(SetDamage);
	Method(ChangeDamage);
	Method(GetMaxHitPoints);
	Method(CanRangeStrike);
	Method(CanRangeStrikeNow);
	Method(CanRangeStrikeAt);
	Method(HasPerformedRangedStrikeThisTurn);
	Method(RangeCombatUnitDefense);
	Method(RangeCombatDamage);
	Method(GetAirStrikeDefenseDamage);

	Method(IsWorkingPlot);
	Method(AlterWorkingPlot);
	Method(IsForcedWorkingPlot);
	Method(GetNumRealBuilding);
	Method(SetNumRealBuilding);
	Method(GetNumFreeBuilding);
	Method(IsBuildingSellable);
	Method(GetSellBuildingRefund);
	Method(GetTotalBaseBuildingMaintenance);

	Method(ClearOrderQueue);
	Method(PushOrder);
	Method(PopOrder);
	Method(GetOrderQueueLength);

	Method(GetBuildingYieldChange);
	Method(SetBuildingYieldChange);

	Method(GetBuildingEspionageModifier);
	Method(GetBuildingGlobalEspionageModifier);

	Method(GetNumCityPlots);
	Method(CanPlaceUnitHere);

	Method(GetSpecialistYield);
	Method(GetCultureFromSpecialist);

	Method(GetNumForcedWorkingPlots);

	Method(GetReligionCityRangeStrikeModifier);
}
//------------------------------------------------------------------------------
void CvLuaCity::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
const char* CvLuaCity::GetTypeName()
{
	return "City";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
//bool isNone();
int CvLuaCity::lIsNone(lua_State* L)
{
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);

	return 1;
}
//------------------------------------------------------------------------------
//void kill();
int CvLuaCity::lKill(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->kill();

	return 1;
}
//------------------------------------------------------------------------------
//void CreateGreatGeneral(UnitTypes eGreatPersonUnit);
int CvLuaCity::lCreateGreatGeneral(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::createGreatGeneral);
}
//------------------------------------------------------------------------------
//void CreateGreatAdmiral(UnitTypes eGreatPersonUnit);
int CvLuaCity::lCreateGreatAdmiral(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::createGreatAdmiral);
}
//------------------------------------------------------------------------------
//void doTask(TaskTypes eTask, int iData1, int iData2, bool bOption);
int CvLuaCity::lDoTask(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const TaskTypes eTask = (TaskTypes)lua_tointeger(L, 2);
	const int iData1 = luaL_optint(L, 3, -1);
	const int iData2 = luaL_optint(L, 4, -1);
	const bool bOption = luaL_optint(L, 5, 0);
	pkCity->doTask(eTask, iData1, iData2, bOption);

	return 1;
}
//------------------------------------------------------------------------------
//void chooseProduction(UnitTypes eTrainUnit, BuildingTypes eConstructBuilding, ProjectTypes eCreateProject, bool bFinish, bool bFront);
int CvLuaCity::lChooseProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->chooseProduction();

	return 1;
}
//------------------------------------------------------------------------------
//int getCityPlotIndex(CyPlot* pPlot);
int CvLuaCity::lGetCityPlotIndex(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetCityIndexFromPlot(pkPlot);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CyPlot* getCityIndexPlot(int iIndex);
int CvLuaCity::lGetCityIndexPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);

	CvPlot* pkPlot = pkCity->GetCityCitizens()->GetCityPlotFromIndex(iIndex);
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
//bool canWork(CyPlot* pPlot);
int CvLuaCity::lCanWork(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->GetCityCitizens()->IsCanWork(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsBlockaded(CyPlot* pPlot);
int CvLuaCity::lIsPlotBlockaded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->GetCityCitizens()->IsPlotBlockaded(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void clearWorkingOverride(int iIndex);
int CvLuaCity::lClearWorkingOverride(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::clearWorkingOverride);
}
//------------------------------------------------------------------------------
//int countNumImprovedPlots();
int CvLuaCity::lCountNumImprovedPlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->countNumImprovedPlots();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int countNumWaterPlots();
int CvLuaCity::lCountNumWaterPlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->countNumWaterPlots();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int countNumRiverPlots();
int CvLuaCity::lCountNumRiverPlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->countNumRiverPlots();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int findPopulationRank();
int CvLuaCity::lFindPopulationRank(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->findPopulationRank();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int findBaseYieldRateRank(YieldTypes eYield);
int CvLuaCity::lFindBaseYieldRateRank(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->findBaseYieldRateRank(eYield);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int findYieldRateRank(YieldTypes eYield);
int CvLuaCity::lFindYieldRateRank(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->findYieldRateRank(eYield);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0);
int CvLuaCity::lAllUpgradesAvailable(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);
	const int iUpgradeCount = luaL_optint(L, 3, 0);

	const UnitTypes eResult = pkCity->allUpgradesAvailable(eUnit, iUpgradeCount);
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isWorldWondersMaxed();
int CvLuaCity::lIsWorldWondersMaxed(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->isWorldWondersMaxed();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isTeamWondersMaxed();
int CvLuaCity::lIsTeamWondersMaxed(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->isTeamWondersMaxed();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNationalWondersMaxed();
int CvLuaCity::lIsNationalWondersMaxed(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->isNationalWondersMaxed();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isBuildingsMaxed();
int CvLuaCity::lIsBuildingsMaxed(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->isBuildingsMaxed();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCanTrainTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 2);

	// City Production Modifier
	pkCity->canTrain(eUnit, false, false, false, false, &toolTip);

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//bool canTrain( int iUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades);
int CvLuaCity::lCanTrain(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iUnit = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bIgnoreCost = luaL_optint(L, 5, 0);
	const bool bIgnoreUpgrades = luaL_optint(L, 6, 0);
	const bool bResult = pkCity->canTrain((UnitTypes)iUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCanConstructTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);

	// City Production Modifier
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//bool canConstruct( int iBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost);
int CvLuaCity::lCanConstruct(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iBuilding = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bIgnoreCost = luaL_optint(L, 5, 0);
	const bool bResult = pkCity->canConstruct((BuildingTypes)iBuilding, bContinue, bTestVisible, bIgnoreCost);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canCreate( int iProject, bool bContinue, bool bTestVisible );
int CvLuaCity::lCanCreate(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iProject = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bResult = pkCity->canCreate((ProjectTypes)iProject, bContinue, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canPrepare( int iSpecialist, bool bContinue );
int CvLuaCity::lCanPrepare(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iSpecialist = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bResult = pkCity->canPrepare((SpecialistTypes)iSpecialist, bContinue);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMaintain( int iProcess, bool bContinue );
int CvLuaCity::lCanMaintain(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iProcess = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bResult = pkCity->canMaintain((ProcessTypes)iProcess, bContinue);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetPurchaseUnitTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 2);

	// City Production Modifier
	pkCity->canTrain(eUnit, false, false, false, false, &toolTip);

	// Already a unit here
	if(!pkCity->CanPlaceUnitHere(eUnit))
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_UNIT_HERE");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	// Not enough cash money
	if(pkCity->GetPurchaseCost(eUnit) > GET_PLAYER(pkCity->getOwner()).GetTreasury()->GetGold())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_NO_GOLD");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetFaithPurchaseUnitTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes) lua_tointeger(L, 2);

	// Already a unit here
	if(!pkCity->CanPlaceUnitHere(eUnit))
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_UNIT_HERE");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	// Not enough faith
	if(pkCity->GetFaithPurchaseCost(eUnit, true) > GET_PLAYER(pkCity->getOwner()).GetFaith())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_NO_FAITH");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetPurchaseBuildingTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);

	// City Production Modifier
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);

	// Not enough cash money
	if(pkCity->GetPurchaseCost(eBuilding) > GET_PLAYER(pkCity->getOwner()).GetTreasury()->GetGold())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_NO_GOLD");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetFaithPurchaseBuildingTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);

	// City Production Modifier
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);

	// Not enough faith
	if(pkCity->GetFaithPurchaseCost(eBuilding) > GET_PLAYER(pkCity->getOwner()).GetFaith())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_CANNOT_PURCHASE_NO_FAITH");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//bool canJoin();
int CvLuaCity::lCanJoin(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->canJoin();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsBuildingLocalResourceValid(BuildingTypes eBuilding, bool bCheckForImprovement);
int CvLuaCity::lIsBuildingLocalResourceValid(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsBuildingLocalResourceValid);
}
//------------------------------------------------------------------------------
//bool GetResourceDemanded();
int CvLuaCity::lGetResourceDemanded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetResourceDemanded);
}
//------------------------------------------------------------------------------
//bool SetResourceDemanded(ResourceTypes eResource);
int CvLuaCity::lSetResourceDemanded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetResourceDemanded);
}
//------------------------------------------------------------------------------
//bool DoPickResourceDemanded();
int CvLuaCity::lDoPickResourceDemanded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::DoPickResourceDemanded);
}

//------------------------------------------------------------------------------
//int getFoodTurnsLeft();
int CvLuaCity::lGetFoodTurnsLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFoodTurnsLeft);

}
//------------------------------------------------------------------------------
//bool isProduction();
int CvLuaCity::lIsProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProduction);
}
//------------------------------------------------------------------------------
//bool isProductionLimited();
int CvLuaCity::lIsProductionLimited(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionLimited);
}
//------------------------------------------------------------------------------
//bool isProductionUnit();
int CvLuaCity::lIsProductionUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionUnit);
}
//------------------------------------------------------------------------------
//bool isProductionBuilding();
int CvLuaCity::lIsProductionBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionBuilding);
}
//------------------------------------------------------------------------------
//bool isProductionProject();
int CvLuaCity::lIsProductionProject(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionProject);
}
//------------------------------------------------------------------------------
//bool isProductionSpecialist();
int CvLuaCity::lIsProductionSpecialist(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionSpecialist);
}
//------------------------------------------------------------------------------
//bool isProductionProcess();
int CvLuaCity::lIsProductionProcess(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionProcess);
}
//------------------------------------------------------------------------------
//bool canContinueProduction(OrderTypes eOrderType, int iData1, int iData2, bool bSave)
int CvLuaCity::lCanContinueProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	OrderData order;
	order.eOrderType = (OrderTypes)lua_tointeger(L, 2);
	order.iData1 = lua_tointeger(L, 3);
	order.iData2 = lua_tointeger(L, 4);
	order.bSave = lua_toboolean(L, 5);

	const bool bResult = pkCity->canContinueProduction(order);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getProductionExperience(UnitTypes eUnit);
int CvLuaCity::lGetProductionExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProductionExperience);
}
//------------------------------------------------------------------------------
//void addProductionExperience(CyUnit* pUnit, bool bConscript = false);
int CvLuaCity::lAddProductionExperience(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bConscript = luaL_optint(L, 3, 0);
	pkCity->addProductionExperience(pkUnit, bConscript);

	return 1;
}
//------------------------------------------------------------------------------
//UnitTypes getProductionUnit()
int CvLuaCity::lGetProductionUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProductionUnit);
}
//------------------------------------------------------------------------------
//UnitAITypes getProductionUnitAI()
int CvLuaCity::lGetProductionUnitAI(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const UnitAITypes eValue = pkCity->getProductionUnitAI();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//BuildingTypes getProductionBuilding()
int CvLuaCity::lGetProductionBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eValue = pkCity->getProductionBuilding();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//ProjectTypes getProductionProject()
int CvLuaCity::lGetProductionProject(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ProjectTypes eValue = pkCity->getProductionProject();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//SpecialistTypes getProductionSpecialist()
int CvLuaCity::lGetProductionSpecialist(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eValue = pkCity->getProductionSpecialist();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//ProcessTypes getProductionProcess()
int CvLuaCity::lGetProductionProcess(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProductionProcess);
}
//------------------------------------------------------------------------------
//std::string getProductionName();
//------------------------------------------------------------------------------
//string getProductionNameKey();
int CvLuaCity::lGetProductionNameKey(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushstring(L, pkCity->getProductionNameKey());
	return 1;
}
//------------------------------------------------------------------------------
//int getGeneralProductionTurnsLeft();
int CvLuaCity::lGetGeneralProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGeneralProductionTurnsLeft);
}
//------------------------------------------------------------------------------
//bool isFoodProduction();
int CvLuaCity::lIsFoodProduction(lua_State* L)
{
	//return BasicLuaMethod<bool, UnitTypes>(L, &CvCity::isFoodProduction);
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->isFoodProduction();

	lua_pushboolean(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getFirstUnitOrder(UnitTypes eUnit);
int CvLuaCity::lGetFirstUnitOrder(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFirstUnitOrder);
}
//------------------------------------------------------------------------------
//int getFirstProjectOrder(ProjectTypes eProject);
int CvLuaCity::lGetFirstProjectOrder(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFirstProjectOrder);
}
//------------------------------------------------------------------------------
//int getFirstSpecialistOrder(SpecialistTypes eSpecialist);
int CvLuaCity::lGetFirstSpecialistOrder(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFirstSpecialistOrder);
}
//------------------------------------------------------------------------------
//int getNumTrainUnitAI(UnitAITypes eUnitAI);
int CvLuaCity::lGetNumTrainUnitAI(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getNumTrainUnitAI);
}
//------------------------------------------------------------------------------
//int getFirstBuildingOrder(BuildingTypes eBuilding);
int CvLuaCity::lGetFirstBuildingOrder(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFirstBuildingOrder);
}
//------------------------------------------------------------------------------
//bool isUnitFoodProduction(UnitTypes iUnit);
int CvLuaCity::lIsUnitFoodProduction(lua_State* L)
{
	return BasicLuaMethod<bool, UnitTypes>(L, &CvCity::isFoodProduction);
}
//------------------------------------------------------------------------------
//int getProduction();
int CvLuaCity::lGetProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProduction);
}
//------------------------------------------------------------------------------
//int getProductionTimes100();
int CvLuaCity::lGetProductionTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProductionTimes100);
}
//------------------------------------------------------------------------------
//int getProductionNeeded();
int CvLuaCity::lGetProductionNeeded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getProductionNeeded);
}
//------------------------------------------------------------------------------
//int GetUnitProductionNeeded();
int CvLuaCity::lGetUnitProductionNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnitType = (UnitTypes) lua_tointeger(L, 2);

	const int iResult = pkCity->getProductionNeeded(eUnitType);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetBuildingProductionNeeded();
int CvLuaCity::lGetBuildingProductionNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuildingType = (BuildingTypes) lua_tointeger(L, 2);

	const int iResult = pkCity->getProductionNeeded(eBuildingType);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetProjectProductionNeeded();
int CvLuaCity::lGetProjectProductionNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ProjectTypes eProjectType = (ProjectTypes) lua_tointeger(L, 2);

	const int iResult = pkCity->getProductionNeeded(eProjectType);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getProductionTurnsLeft();
int CvLuaCity::lGetProductionTurnsLeft(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getProductionTurnsLeft();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnitProductionTurnsLeft(UnitTypes iUnit, int iNum);
int CvLuaCity::lGetUnitProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod<int, UnitTypes>(L, &CvCity::getProductionTurnsLeft);
}
//------------------------------------------------------------------------------
//int getBuildingProductionTurnsLeft(BuildingTypes iBuilding, int iNum);
int CvLuaCity::lGetBuildingProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod<int, BuildingTypes>(L, &CvCity::getProductionTurnsLeft);
}
//------------------------------------------------------------------------------
//int getProjectProductionTurnsLeft(ProjectTypes eProject, int iNum);
int CvLuaCity::lGetProjectProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod<int, ProjectTypes>(L, &CvCity::getProductionTurnsLeft);
}
//------------------------------------------------------------------------------
//int getSpecialistProductionTurnsLeft(SpecialistTypes eSpecialist, int iNum);
int CvLuaCity::lGetSpecialistProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod<int, SpecialistTypes>(L, &CvCity::getProductionTurnsLeft);
}
//------------------------------------------------------------------------------
// int IsCanPurchase(UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);
int CvLuaCity::lIsCanPurchase(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bTestPurchaseCost = lua_toboolean(L, 2);
	const bool bTestTrainable = lua_toboolean(L, 3);
	const UnitTypes eUnitType = (UnitTypes) lua_tointeger(L, 4);
	const BuildingTypes eBuildingType = (BuildingTypes) lua_tointeger(L, 5);
	const ProjectTypes eProjectType = (ProjectTypes) lua_tointeger(L, 6);
	const YieldTypes ePurchaseYield = (YieldTypes) lua_tointeger(L, 7);

	const bool bResult = pkCity->IsCanPurchase(bTestPurchaseCost, bTestTrainable, eUnitType, eBuildingType, eProjectType, ePurchaseYield);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
// int GetPurchaseCost(UnitTypes eUnit);
int CvLuaCity::lGetUnitPurchaseCost(lua_State* L)
{
	return BasicLuaMethod<int, UnitTypes>(L, &CvCity::GetPurchaseCost);
}
//------------------------------------------------------------------------------
// int GetFaithPurchaseCost(UnitTypes eUnit, bool bIncludeBeliefDiscounts);
int CvLuaCity::lGetUnitFaithPurchaseCost(lua_State* L)
{
	return BasicLuaMethod<int, UnitTypes>(L, &CvCity::GetFaithPurchaseCost);
}
//------------------------------------------------------------------------------
//int GetPurchaseCost(BuildingTypes eBuilding);
int CvLuaCity::lGetBuildingPurchaseCost(lua_State* L)
{
	return BasicLuaMethod<int, BuildingTypes>(L, &CvCity::GetPurchaseCost);
}
//------------------------------------------------------------------------------
//int GetFaithPurchaseCost(BuildingTypes eBuilding);
int CvLuaCity::lGetBuildingFaithPurchaseCost(lua_State* L)
{
	return BasicLuaMethod<int, BuildingTypes>(L, &CvCity::GetFaithPurchaseCost);
}
//------------------------------------------------------------------------------
//int GetPurchaseCost(ProjectTypes eProject);
int CvLuaCity::lGetProjectPurchaseCost(lua_State* L)
{
	return BasicLuaMethod<int, ProjectTypes>(L, &CvCity::GetPurchaseCost);
}
//------------------------------------------------------------------------------
//void setProduction(int iNewValue);
int CvLuaCity::lSetProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setProduction);
}
//------------------------------------------------------------------------------
//void changeProduction(int iChange);
int CvLuaCity::lChangeProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeProduction);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetYieldModifierTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const YieldTypes iYield = (YieldTypes) lua_tointeger(L, 2);

	// City Food Modifier
	if(iYield == YIELD_FOOD)
		pkCity->foodDifferenceTimes100(true, &toolTip);

	// City Production Modifier
	if(iYield == YIELD_PRODUCTION)
		pkCity->getProductionModifier(&toolTip);

	// City Yield Rate Modifier
	pkCity->getBaseYieldRateModifier(iYield, 0, &toolTip);

	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//int getProductionModifier();
int CvLuaCity::lGetProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getProductionModifier();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetCurrentProductionDifference(bool bIgnoreFood, bool bOverflow);
int CvLuaCity::lGetCurrentProductionDifference(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getCurrentProductionDifference);
}
//------------------------------------------------------------------------------
//int GetRawProductionDifference(bool bIgnoreFood, bool bOverflow);
int CvLuaCity::lGetRawProductionDifference(lua_State *L)
{
	return BasicLuaMethod(L, &CvCity::getRawProductionDifference);
}
//------------------------------------------------------------------------------
//int GetCurrentProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow);
int CvLuaCity::lGetCurrentProductionDifferenceTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getCurrentProductionDifferenceTimes100);
}
//------------------------------------------------------------------------------
//int GetRawProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow);
int CvLuaCity::lGetRawProductionDifferenceTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getRawProductionDifferenceTimes100);
}
//------------------------------------------------------------------------------
//int getUnitProductionModifier(UnitTypes iUnit);
int CvLuaCity::lGetUnitProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const UnitTypes iUnit = (UnitTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getProductionModifier(iUnit);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingProductionModifier(BuildingTypes iBuilding);
int CvLuaCity::lGetBuildingProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iBuilding = (BuildingTypes)lua_tointeger(L, 2);
	if(iBuilding != NO_BUILDING)
	{
		const int iResult = pkCity->getProductionModifier(iBuilding);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int getProjectProductionModifier(ProjectTypes eProject);
int CvLuaCity::lGetProjectProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ProjectTypes eProject = (ProjectTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getProductionModifier(eProject);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getSpecialistProductionModifier(SpecialistTypes eSpecialist);
int CvLuaCity::lGetSpecialistProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getProductionModifier(eSpecialist);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraProductionDifference(int iExtra);
int CvLuaCity::lGetExtraProductionDifference(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getExtraProductionDifference);
}
//------------------------------------------------------------------------------
//bool canHurry(HurryTypes iHurry, bool bTestVisible);
int CvLuaCity::lCanHurry(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::canHurry);
}
//------------------------------------------------------------------------------
//void hurry(HurryTypes iHurry);
int CvLuaCity::lHurry(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::hurry);
}
//------------------------------------------------------------------------------
//UnitTypes getConscriptUnit()
int CvLuaCity::lGetConscriptUnit(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getConscriptUnit);
}
//------------------------------------------------------------------------------
//int getConscriptPopulation();
int CvLuaCity::lGetConscriptPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getConscriptPopulation);
}
//------------------------------------------------------------------------------
//int conscriptMinCityPopulation();
int CvLuaCity::lConscriptMinCityPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::conscriptMinCityPopulation);
}
//------------------------------------------------------------------------------
//bool canConscript();
int CvLuaCity::lCanConscript(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::canConscript);
}
//------------------------------------------------------------------------------
//void conscript();
int CvLuaCity::lConscript(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->conscript();

	return 1;
}
//------------------------------------------------------------------------------
//int getResourceYieldRateModifier(YieldTypes eIndex, ResourceTypes eResource);
int CvLuaCity::lGetResourceYieldRateModifier(lua_State* L)
{
	return BasicLuaMethod<int, YieldTypes, ResourceTypes>(L, &CvCity::getResourceYieldRateModifier);
}
//------------------------------------------------------------------------------
//HandicapTypes getHandicapType();
int CvLuaCity::lGetHandicapType(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getHandicapType);
}
//------------------------------------------------------------------------------
//CivilizationTypes getCivilizationType();
int CvLuaCity::lGetCivilizationType(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getCivilizationType);
}
//------------------------------------------------------------------------------
//LeaderHeadTypes getPersonalityType()
int CvLuaCity::lGetPersonalityType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const LeaderHeadTypes eValue = pkCity->getPersonalityType();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//ArtStyleTypes getArtStyleType()
int CvLuaCity::lGetArtStyleType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ArtStyleTypes eValue = pkCity->getArtStyleType();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//CitySizeTypes getCitySizeType()
int CvLuaCity::lGetCitySizeType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CitySizeTypes eValue = pkCity->getCitySizeType();

	lua_pushinteger(L, eValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool isBarbarian();
int CvLuaCity::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isBarbarian);
}
//------------------------------------------------------------------------------
//bool isHuman();
int CvLuaCity::lIsHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isHuman);
}
//------------------------------------------------------------------------------
//bool isVisible(TeamTypes eTeam, bool bDebug);
int CvLuaCity::lIsVisible(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isVisible);
}
//------------------------------------------------------------------------------
//bool isCapital();
int CvLuaCity::lIsCapital(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isCapital);
}
//------------------------------------------------------------------------------
//bool isOriginalCapital();
int CvLuaCity::lIsOriginalCapital(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsOriginalCapital);
}
//------------------------------------------------------------------------------
//bool isCoastal(int iMinWaterSize);
int CvLuaCity::lIsCoastal(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isCoastal);
}
//------------------------------------------------------------------------------
//int foodConsumption(bool bNoAngry, int iExtra);
int CvLuaCity::lFoodConsumption(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::foodConsumption);
}
//------------------------------------------------------------------------------
//int foodDifference(bool bBottom);
int CvLuaCity::lFoodDifference(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::foodDifference);
}
//------------------------------------------------------------------------------
//int foodDifferenceTimes100(bool bBottom);
int CvLuaCity::lFoodDifferenceTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::foodDifferenceTimes100);
}
//------------------------------------------------------------------------------
//int growthThreshold();
int CvLuaCity::lGrowthThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::growthThreshold);
}
//------------------------------------------------------------------------------
//int productionLeft();
int CvLuaCity::lProductionLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::productionLeft);
}
//------------------------------------------------------------------------------
//int hurryCost(HurryTypes iHurry, bool bExtra);
int CvLuaCity::lHurryCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::hurryCost);
}
//------------------------------------------------------------------------------
//int hurryGold(HurryTypes iHurry);
int CvLuaCity::lHurryGold(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::hurryGold);
}
//------------------------------------------------------------------------------
//int hurryPopulation(HurryTypes iHurry);
int CvLuaCity::lHurryPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::hurryPopulation);
}
//------------------------------------------------------------------------------
//int hurryProduction(HurryTypes iHurry);
int CvLuaCity::lHurryProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::hurryProduction);
}
//------------------------------------------------------------------------------
//int maxHurryPopulation();
int CvLuaCity::lMaxHurryPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::maxHurryPopulation);
}
//------------------------------------------------------------------------------
//int getNumBuilding(BuildingTypes eBuildingType);
int CvLuaCity::lGetNumBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	if(eBuildingType != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetNumBuilding(eBuildingType);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//bool isHasBuilding(BuildingTypes eBuildingType);
// This is a function to help modders out, since it was replaced with getNumBuildings() in the C++
int CvLuaCity::lIsHasBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	if(eBuildingType != NO_BUILDING)
	{
		const bool bResult = pkCity->GetCityBuildings()->GetNumBuilding(eBuildingType);
		lua_pushboolean(L, bResult);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int getNumActiveBuilding(BuildingTypes eBuildingType);
int CvLuaCity::lGetNumActiveBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	if(eBuildingType != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetNumActiveBuilding(eBuildingType);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int getID();
int CvLuaCity::lGetID(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetID();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getX();
int CvLuaCity::lGetX(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getX();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getY();
int CvLuaCity::lGetY(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getY();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool at(int iX, int iY);
int CvLuaCity::lAt(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);
	const bool bResult = pkCity->at(x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool atPlot(CyPlot* pPlot);
int CvLuaCity::lAtPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->at(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//CyPlot* plot();
int CvLuaCity::lPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = pkCity->plot();
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}
//------------------------------------------------------------------------------
//CyArea* area();
int CvLuaCity::lArea(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvArea* pkArea = pkCity->area();
	CvLuaArea::Push(L, pkArea);
	return 1;
}
//------------------------------------------------------------------------------
//CyArea* waterArea();
int CvLuaCity::lWaterArea(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvArea* pkArea = pkCity->waterArea();
	CvLuaArea::Push(L, pkArea);
	return 1;
}
//------------------------------------------------------------------------------
//CyPlot* getRallyPlot();
int CvLuaCity::lGetRallyPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = pkCity->getRallyPlot();
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}

//------------------------------------------------------------------------------
//bool getCanBuyPlot();
int CvLuaCity::lCanBuyPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pPlot = pkCity->GetNextBuyablePlot();
	lua_pushboolean(L, pkCity->CanBuyPlot(pPlot->getX(), pPlot->getY()));
	return 1;
}

//------------------------------------------------------------------------------
//bool getCanBuyPlotAt();
int CvLuaCity::lCanBuyPlotAt(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iX = lua_tointeger(L, 2);
	const int iY = lua_tointeger(L, 3);
	const bool bIgnoreCost = lua_toboolean(L, 4);
	lua_pushboolean(L, pkCity->CanBuyPlot(iX, iY, bIgnoreCost));
	return 1;
}
//------------------------------------------------------------------------------
//bool CanBuyAnyPlot(void)
int CvLuaCity::lCanBuyAnyPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushboolean(L, pkCity->CanBuyAnyPlot());
	return 1;
}
//------------------------------------------------------------------------------
//CvPlot* getNextBuyablePlot();
int CvLuaCity::lGetNextBuyablePlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = pkCity->GetNextBuyablePlot();
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaCity::lGetBuyablePlotList(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	std::vector<int> aiPlotList;
	aiPlotList.resize(20, -1);
	pkCity->GetBuyablePlotList(aiPlotList);

	int iReturnValues = 0;

	for(uint ui = 0; ui < aiPlotList.size(); ui++)
	{
		if(aiPlotList[ui] >= 0)
		{
			CvPlot* pkPlot = GC.getMap().plotByIndex(aiPlotList[ui]);
			CvLuaPlot::Push(L, pkPlot);
			iReturnValues++;
		}
		else
		{
			break;
		}
	}

	return iReturnValues;
}

//------------------------------------------------------------------------------
//int GetBuyPlotCost()
int CvLuaCity::lGetBuyPlotCost(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iX = lua_tointeger(L, 2);
	const int iY = lua_tointeger(L, 3);
	lua_pushinteger(L, pkCity->GetBuyPlotCost(iX, iY));
	return 1;
}

//------------------------------------------------------------------------------
// CvUnit* GetGarrisonedUnit()
int CvLuaCity::lGetGarrisonedUnit(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkUnit = pkCity->GetGarrisonedUnit();
	CvLuaUnit::Push(L, pkUnit);
	return 1;
}

//------------------------------------------------------------------------------
//int getGameTurnFounded();
int CvLuaCity::lGetGameTurnFounded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGameTurnFounded);
}
//------------------------------------------------------------------------------
//int getGameTurnAcquired();
int CvLuaCity::lGetGameTurnAcquired(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGameTurnAcquired);
}
//------------------------------------------------------------------------------
//int getGameTurnLastExpanded();
int CvLuaCity::lGetGameTurnLastExpanded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGameTurnLastExpanded);
}
//------------------------------------------------------------------------------
//int getPopulation();
int CvLuaCity::lGetPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getPopulation);
}
//------------------------------------------------------------------------------
//void setPopulation(int iNewValue);
int CvLuaCity::lSetPopulation(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iValue = lua_tointeger(L, 2);
	int bReassignPop = lua_toboolean(L, 3);
	CvAssertMsg(bReassignPop != 0, "It is super dangerous to set this to false.  Ken would love to see why you are doing this.");
	pkCity->setPopulation(iValue, bReassignPop);

	return 1;
//	return BasicLuaMethod(L, &CvCity::setPopulation);
}
//------------------------------------------------------------------------------
//void changePopulation(int iChange);
int CvLuaCity::lChangePopulation(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iChange = lua_tointeger(L, 2);
	int bReassignPop = lua_toboolean(L, 3);
	CvAssertMsg(bReassignPop != 0, "It is super dangerous to set this to false.  Ken would love to see why you are doing this.");
	pkCity->changePopulation(iChange, bReassignPop);

	return 1;
//	return BasicLuaMethod(L, &CvCity::changePopulation);
}
//------------------------------------------------------------------------------
//int getRealPopulation();
int CvLuaCity::lGetRealPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getRealPopulation);
}
//------------------------------------------------------------------------------
//int getHighestPopulation();
int CvLuaCity::lGetHighestPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getHighestPopulation);
}
//------------------------------------------------------------------------------
//void setHighestPopulation(int iNewValue);
int CvLuaCity::lSetHighestPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setHighestPopulation);
}
//------------------------------------------------------------------------------
//int getWorkingPopulation();
//int CvLuaCity::lGetWorkingPopulation(lua_State* L)
//{
//	return BasicLuaMethod(L, &CvCity::getWorkingPopulation);
//}
////------------------------------------------------------------------------------
////int getSpecialistPopulation();
//int CvLuaCity::lGetSpecialistPopulation(lua_State* L)
//{
//	return BasicLuaMethod(L, &CvCity::getSpecialistPopulation);
//}
//------------------------------------------------------------------------------
//int getNumGreatPeople();
int CvLuaCity::lGetNumGreatPeople(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getNumGreatPeople);
}
//------------------------------------------------------------------------------
//int getBaseGreatPeopleRate();
int CvLuaCity::lGetBaseGreatPeopleRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getBaseGreatPeopleRate);
}
//------------------------------------------------------------------------------
//int getGreatPeopleRate();
int CvLuaCity::lGetGreatPeopleRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGreatPeopleRate);
}
//------------------------------------------------------------------------------
//int getTotalGreatPeopleRateModifier();
int CvLuaCity::lGetTotalGreatPeopleRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getTotalGreatPeopleRateModifier);
}
//------------------------------------------------------------------------------
//void changeBaseGreatPeopleRate(int iChange);
int CvLuaCity::lChangeBaseGreatPeopleRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeBaseGreatPeopleRate);
}
//------------------------------------------------------------------------------
//int getGreatPeopleRateModifier();
int CvLuaCity::lGetGreatPeopleRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGreatPeopleRateModifier);
}
//------------------------------------------------------------------------------
//int getGreatPeopleProgress();
int CvLuaCity::lGetGreatPeopleProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getGreatPeopleProgress);
}
//------------------------------------------------------------------------------
//void changeGreatPeopleProgress(int iChange);
int CvLuaCity::lChangeGreatPeopleProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeGreatPeopleProgress);
}

//------------------------------------------------------------------------------
//int GetJONSCultureStored() const;
int CvLuaCity::lGetJONSCultureStored(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCultureStored);
}
//------------------------------------------------------------------------------
//void SetJONSCultureStored(int iValue);
int CvLuaCity::lSetJONSCultureStored(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetJONSCultureStored);
}
//------------------------------------------------------------------------------
//void ChangeJONSCultureStored(int iChange);
int CvLuaCity::lChangeJONSCultureStored(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCultureStored);
}
//------------------------------------------------------------------------------
//int GetJONSCultureLevel() const;
int CvLuaCity::lGetJONSCultureLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCultureLevel);
}
//------------------------------------------------------------------------------
//void SetJONSCultureLevel(int iValue);
int CvLuaCity::lSetJONSCultureLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetJONSCultureLevel);
}
//------------------------------------------------------------------------------
//void ChangeJONSCultureLevel(int iChange);
int CvLuaCity::lChangeJONSCultureLevel(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCultureLevel);
}
//------------------------------------------------------------------------------
//void DoJONSCultureLevelIncrease();
int CvLuaCity::lDoJONSCultureLevelIncrease(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::DoJONSCultureLevelIncrease);
}
//------------------------------------------------------------------------------
//int GetJONSCultureThreshold() const;
int CvLuaCity::lGetJONSCultureThreshold(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCultureThreshold);
}

//------------------------------------------------------------------------------
//int getJONSCulturePerTurn() const;
int CvLuaCity::lGetJONSCulturePerTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getJONSCulturePerTurn);
}
//------------------------------------------------------------------------------
//int GetBaseJONSCulturePerTurn() const;
int CvLuaCity::lGetBaseJONSCulturePerTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseJONSCulturePerTurn);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromBuildings() const;
int CvLuaCity::lGetJONSCulturePerTurnFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromBuildings);
}
//------------------------------------------------------------------------------
//void ChangeJONSCulturePerTurnFromBuildings(int iChange);
int CvLuaCity::lChangeJONSCulturePerTurnFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromBuildings);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromPolicies() const;
int CvLuaCity::lGetJONSCulturePerTurnFromPolicies(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromPolicies);
}
//------------------------------------------------------------------------------
//void ChangeJONSCulturePerTurnFromPolicies(int iChange);
int CvLuaCity::lChangeJONSCulturePerTurnFromPolicies(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromPolicies);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromSpecialists() const;
int CvLuaCity::lGetJONSCulturePerTurnFromSpecialists(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromSpecialists);
}
//------------------------------------------------------------------------------
//void ChangeJONSCulturePerTurnFromSpecialists(int iChange);
int CvLuaCity::lChangeJONSCulturePerTurnFromSpecialists(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromSpecialists);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromTraits() const;
int CvLuaCity::lGetJONSCulturePerTurnFromTraits(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromTraits);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromReligion() const;
int CvLuaCity::lGetJONSCulturePerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromReligion);
}
//------------------------------------------------------------------------------
//void ChangeJONSCulturePerTurnFromReligion(int iChange);
int CvLuaCity::lChangeJONSCulturePerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromReligion);
}
//------------------------------------------------------------------------------
//int getCultureRateModifier() const;
int CvLuaCity::lGetCultureRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getCultureRateModifier);
}
//------------------------------------------------------------------------------
//void changeCultureRateModifier(int iChange);
int CvLuaCity::lChangeCultureRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeCultureRateModifier);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurn() const;
int CvLuaCity::lGetFaithPerTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurn);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromBuildings() const;
int CvLuaCity::lGetFaithPerTurnFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurnFromBuildings);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromPolicies() const;
int CvLuaCity::lGetFaithPerTurnFromPolicies(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurnFromPolicies);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromTraits() const;
int CvLuaCity::lGetFaithPerTurnFromTraits(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurnFromTraits);
}
//------------------------------------------------------------------------------
//int GetFaithPerTurnFromReligion() const;
int CvLuaCity::lGetFaithPerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurnFromReligion);
}
//------------------------------------------------------------------------------
//void ChangeFaithPerTurnFromReligion(int iChange);
int CvLuaCity::lChangeFaithPerTurnFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeFaithPerTurnFromReligion);
}
//------------------------------------------------------------------------------
//int IsReligionInCity() const;
int CvLuaCity::lIsReligionInCity(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->GetCityReligions()->IsReligionInCity();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int IsHolyCityForReligion(ReligionTypes eReligion) const;
int CvLuaCity::lIsHolyCityForReligion(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	const bool bResult = pkCity->GetCityReligions()->IsHolyCityForReligion(eReligion);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int IsHolyCityAnyReligion() const;
int CvLuaCity::lIsHolyCityAnyReligion(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->GetCityReligions()->IsHolyCityAnyReligion();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumFollowers(ReligionTypes eReligion) const;
int CvLuaCity::lGetNumFollowers(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->GetCityReligions()->GetNumFollowers(eReligion);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetReligiousMajority() const;
int CvLuaCity::lGetReligiousMajority(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = (int)pkCity->GetCityReligions()->GetReligiousMajority();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetFavoredReligion() const;
int CvLuaCity::lGetFavoredReligion(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = (int)pkCity->GetCityReligions()->GetFavoredReligion();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetPressurePerTurn() const;
int CvLuaCity::lGetPressurePerTurn(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	const int iResult = (int)pkCity->GetCityReligions()->GetPressurePerTurn(eReligion);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int ConvertPercentFollowers(ReligionTypes eToReligion, ReligionTypes eFromReligion, int iPercent) const;
int CvLuaCity::lConvertPercentFollowers(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eToReligion = (ReligionTypes)lua_tointeger(L, 2);
	ReligionTypes eFromReligion = (ReligionTypes)lua_tointeger(L, 3);
	int iPercent = lua_tointeger(L, 4);
	pkCity->GetCityReligions()->ConvertPercentFollowers(eToReligion, eFromReligion, iPercent);
	return 1;
}
//------------------------------------------------------------------------------
//int AdoptReligionFully() const;
int CvLuaCity::lAdoptReligionFully(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	pkCity->GetCityReligions()->AdoptReligionFully(eReligion);
	return 1;
}
//------------------------------------------------------------------------------
//int lGetReligionBuildingClassHappiness(eBuildingClass) const;
int CvLuaCity::lGetReligionBuildingClassHappiness(lua_State* L)
{
	int iHappinessFromBuilding = 0;

	CvCity* pkCity = GetInstance(L);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)lua_tointeger(L, 2);

	ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pkCity->getOwner());
		if(pReligion)
		{	
			int iFollowers = pkCity->GetCityReligions()->GetNumFollowers(eMajority);
			iHappinessFromBuilding += pReligion->m_Beliefs.GetBuildingClassHappiness(eBuildingClass, iFollowers);
		}
	}
	lua_pushinteger(L, iHappinessFromBuilding);
	return 1;
}
//------------------------------------------------------------------------------
//int GetReligionBuildingClassYieldChange(eBuildingClass, eYieldType) const;
int CvLuaCity::lGetReligionBuildingClassYieldChange(lua_State* L)
{
	int iYieldFromBuilding = 0;

	CvCity* pkCity = GetInstance(L);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)lua_tointeger(L, 2);
	YieldTypes eYieldType = (YieldTypes)lua_tointeger(L, 3);

	ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pkCity->getOwner());
		if(pReligion)
		{	
			int iFollowers = pkCity->GetCityReligions()->GetNumFollowers(eMajority);
			iYieldFromBuilding += pReligion->m_Beliefs.GetBuildingClassYieldChange(eBuildingClass, eYieldType, iFollowers);
		}
	}
	lua_pushinteger(L, iYieldFromBuilding);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumWorldWonders();
int CvLuaCity::lGetNumWorldWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getNumWorldWonders);
}
//------------------------------------------------------------------------------
//int getNumTeamWonders();
int CvLuaCity::lGetNumTeamWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getNumTeamWonders);
}
//------------------------------------------------------------------------------
//int getNumNationalWonders();
int CvLuaCity::lGetNumNationalWonders(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getNumNationalWonders);
}
//------------------------------------------------------------------------------
//int getNumBuildings();
int CvLuaCity::lGetNumBuildings(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityBuildings()->GetNumBuildings();

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetWonderProductionModifier();
int CvLuaCity::lGetWonderProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetWonderProductionModifier);
}
//------------------------------------------------------------------------------
//void ChangeWonderProductionModifier(int iChange);
int CvLuaCity::lChangeWonderProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeWonderProductionModifier);
}
//------------------------------------------------------------------------------
//int GetLocalResourceWonderProductionMod(BuildingTypes eBuilding);
int CvLuaCity::lGetLocalResourceWonderProductionMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetLocalResourceWonderProductionMod);
}

//------------------------------------------------------------------------------
//void changeHealRate(int iChange);
int CvLuaCity::lChangeHealRate(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeHealRate);
}

//------------------------------------------------------------------------------
//bool IsNoOccupiedUnhappiness();
int CvLuaCity::lIsNoOccupiedUnhappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsNoOccupiedUnhappiness);
}
//------------------------------------------------------------------------------
//int getFood();
int CvLuaCity::lGetFood(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFood);
}
//------------------------------------------------------------------------------
//int getFoodTimes100();
int CvLuaCity::lGetFoodTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFoodTimes100);
}
//------------------------------------------------------------------------------
//void setFood(int iNewValue);
int CvLuaCity::lSetFood(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setFood);
}
//------------------------------------------------------------------------------
//void changeFood(int iChange);
int CvLuaCity::lChangeFood(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeFood);
}
//------------------------------------------------------------------------------
//int getFoodKept();
int CvLuaCity::lGetFoodKept(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFoodKept);
}
//------------------------------------------------------------------------------
//int getMaxFoodKeptPercent();
int CvLuaCity::lGetMaxFoodKeptPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getMaxFoodKeptPercent);
}
//------------------------------------------------------------------------------
//int getOverflowProduction();
int CvLuaCity::lGetOverflowProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getOverflowProduction);
}
//------------------------------------------------------------------------------
//void setOverflowProduction(int iNewValue);
int CvLuaCity::lSetOverflowProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setOverflowProduction);
}
//------------------------------------------------------------------------------
//int getFeatureProduction();
int CvLuaCity::lGetFeatureProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFeatureProduction);
}
//------------------------------------------------------------------------------
//void setFeatureProduction(int iNewValue);
int CvLuaCity::lSetFeatureProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setFeatureProduction);
}
//------------------------------------------------------------------------------
//int getMilitaryProductionModifier();
int CvLuaCity::lGetMilitaryProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getMilitaryProductionModifier();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getSpaceProductionModifier();
int CvLuaCity::lGetSpaceProductionModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getSpaceProductionModifier();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void CreateApolloProgram();
int CvLuaCity::lCreateApolloProgram(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	if(pkCity != NULL)
	{
		ProjectTypes eApolloProgram = (ProjectTypes)GC.getSPACE_RACE_TRIGGER_PROJECT();
		pkCity->CreateProject(eApolloProgram);
	}

	return 0;
}
//------------------------------------------------------------------------------
//int getBuildingDefense();
int CvLuaCity::lGetBuildingDefense(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityBuildings()->GetBuildingDefense();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getFreeExperience();
int CvLuaCity::lGetFreeExperience(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getFreeExperience();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getCurrAirlift();
int CvLuaCity::lGetCurrAirlift(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getCurrAirlift();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getMaxAirlift();
int CvLuaCity::lGetMaxAirlift(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getMaxAirlift();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getAirModifier();
int CvLuaCity::lGetAirModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getAirModifier();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNukeModifier();
int CvLuaCity::lGetNukeModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getNukeModifier();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getFreeSpecialist();
//int CvLuaCity::lGetFreeSpecialist(lua_State* L)
//{
//	CvCity* pkCity = GetInstance(L);
//	const int iResult = pkCity->getFreeSpecialist();
//
//	lua_pushinteger(L, iResult);
//	return 1;
//}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//int IsResistance();
int CvLuaCity::lIsResistance(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsResistance);
}
//int GetResistanceTurns();
int CvLuaCity::lGetResistanceTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetResistanceTurns);
}
//int ChangeResistanceTurns();
int CvLuaCity::lChangeResistanceTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeResistanceTurns);
}

//int IsRazing();
int CvLuaCity::lIsRazing(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsRazing);
}
//int GetRazingTurns();
int CvLuaCity::lGetRazingTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetRazingTurns);
}
//int ChangeRazingTurns();
int CvLuaCity::lChangeRazingTurns(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeRazingTurns);
}

//int IsOccupied();
int CvLuaCity::lIsOccupied(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsOccupied);
}
//------------------------------------------------------------------------------
//void SetOccupied(bool bValue);
int CvLuaCity::lSetOccupied(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetOccupied);
}
//------------------------------------------------------------------------------
//int IsPuppet();
int CvLuaCity::lIsPuppet(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsPuppet);
}
//------------------------------------------------------------------------------
//void SetPuppet(bool bValue);
int CvLuaCity::lSetPuppet(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetPuppet);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetHappinessFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetHappinessFromBuildings);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetLocalHappiness(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetLocalHappiness);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetHappiness(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	CvPlayerAI& kPlayer = GET_PLAYER(pkCity->getOwner());

	const int numPolicyInfos = GC.getNumPolicyInfos();
	const int numBuildingInfos = GC.getNumBuildingInfos();

	CvPlayerPolicies* pkPlayerPolicies = kPlayer.GetPlayerPolicies();
	CvCityBuildings* pkCityBuildings = pkCity->GetCityBuildings();

	int iHappiness = pkCity->GetHappinessFromBuildings();

	lua_pushinteger(L, iHappiness);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNeverLost();
int CvLuaCity::lIsNeverLost(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isNeverLost);
}
//------------------------------------------------------------------------------
//void setNeverLost(int iNewValue);
int CvLuaCity::lSetNeverLost(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setNeverLost);
}
//------------------------------------------------------------------------------
//bool isDrafted();
int CvLuaCity::lIsDrafted(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isDrafted);
}
//------------------------------------------------------------------------------
//void setDrafted(int iNewValue);
int CvLuaCity::lSetDrafted(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setDrafted);
}
//------------------------------------------------------------------------------
//bool isAirliftTargeted();
int CvLuaCity::lIsAirliftTargeted(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isAirliftTargeted);
}
//------------------------------------------------------------------------------
//void setAirliftTargeted(int iNewValue);
int CvLuaCity::lSetAirliftTargeted(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setAirliftTargeted);
}
//------------------------------------------------------------------------------
int CvLuaCity::lIsBlockaded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsBlockaded);
}

//------------------------------------------------------------------------------
//int GetWeLoveTheKingDayCounter();
int CvLuaCity::lGetWeLoveTheKingDayCounter(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetWeLoveTheKingDayCounter);
}
//------------------------------------------------------------------------------
//void SetWeLoveTheKingDayCounter(int iValue);
int CvLuaCity::lSetWeLoveTheKingDayCounter(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::SetWeLoveTheKingDayCounter);
}
//------------------------------------------------------------------------------
//void ChangeWeLoveTheKingDayCounter(int iChange);
int CvLuaCity::lChangeWeLoveTheKingDayCounter(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeWeLoveTheKingDayCounter);
}
//------------------------------------------------------------------------------
//void GetNumThingsProduced() const;
int CvLuaCity::lGetNumThingsProduced(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetNumThingsProduced);
}

//------------------------------------------------------------------------------
//bool isCitizensAutomated();
//int CvLuaCity::lIsCitizensAutomated(lua_State* L)
//{
//	return BasicLuaMethod(L, &CvCity::isCitizensAutomated);
//}
////------------------------------------------------------------------------------
////void setCitizensAutomated(bool bNewValue);
//int CvLuaCity::lSetCitizensAutomated(lua_State* L)
//{
//	return BasicLuaMethod(L, &CvCity::setCitizensAutomated);
//}
//------------------------------------------------------------------------------
//bool isProductionAutomated();
int CvLuaCity::lIsProductionAutomated(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isProductionAutomated);
}
//------------------------------------------------------------------------------
//void setProductionAutomated(bool bNewValue);
int CvLuaCity::lSetProductionAutomated(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setProductionAutomated);
}
//------------------------------------------------------------------------------
//void setCitySizeBoost(int iBoost);
int CvLuaCity::lSetCitySizeBoost(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setCitySizeBoost);
}
//------------------------------------------------------------------------------
//PlayerTypes getOwner();
int CvLuaCity::lGetOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getOwner);
}
//------------------------------------------------------------------------------
//TeamTypes getTeam();
int CvLuaCity::lGetTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getTeam);
}
//------------------------------------------------------------------------------
//PlayerTypes getPreviousOwner();
int CvLuaCity::lGetPreviousOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getPreviousOwner);
}
//------------------------------------------------------------------------------
//PlayerTypes getOriginalOwner();
int CvLuaCity::lGetOriginalOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getOriginalOwner);
}
//------------------------------------------------------------------------------
//int getSeaPlotYield(YieldTypes eIndex);
int CvLuaCity::lGetSeaPlotYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getSeaPlotYield(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getRiverPlotYield(YieldTypes eIndex);
int CvLuaCity::lGetRiverPlotYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getRiverPlotYield(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getLakePlotYield(YieldTypes eIndex);
int CvLuaCity::lGetLakePlotYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getLakePlotYield(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getBaseYieldRate(YieldTypes eIndex);
int CvLuaCity::lGetBaseYieldRate(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getBaseYieldRate(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromTerrain(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromTerrain);
}
//------------------------------------------------------------------------------
int CvLuaCity::lChangeBaseYieldRateFromTerrain(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeBaseYieldRateFromTerrain);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromBuildings);
}
//------------------------------------------------------------------------------
int CvLuaCity::lChangeBaseYieldRateFromBuildings(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeBaseYieldRateFromBuildings);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromSpecialists(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromSpecialists);
}
//------------------------------------------------------------------------------
int CvLuaCity::lChangeBaseYieldRateFromSpecialists(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeBaseYieldRateFromSpecialists);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromMisc(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromMisc);
}
//------------------------------------------------------------------------------
int CvLuaCity::lChangeBaseYieldRateFromMisc(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeBaseYieldRateFromMisc);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromReligion);
}
//------------------------------------------------------------------------------
int CvLuaCity::lChangeBaseYieldRateFromReligion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::ChangeBaseYieldRateFromReligion);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetYieldPerPopTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetYieldPerPopTimes100);
}

//------------------------------------------------------------------------------
//int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0);
int CvLuaCity::lGetBaseYieldRateModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iExtra = luaL_optint(L, 3, 0);
	const int iResult = pkCity->getBaseYieldRateModifier(eIndex, iExtra);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getYieldRate(YieldTypes eIndex);
int CvLuaCity::lGetYieldRate(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getYieldRate(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getYieldRateTimes100(YieldTypes eIndex);
int CvLuaCity::lGetYieldRateTimes100(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getYieldRateTimes100(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getYieldRateModifier(YieldTypes eIndex);
int CvLuaCity::lGetYieldRateModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getYieldRateModifier(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int getExtraSpecialistYield(YieldTypes eIndex);
int CvLuaCity::lGetExtraSpecialistYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getExtraSpecialistYield(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraSpecialistYieldOfType(YieldTypes eIndex, SpecialistTypes eSpecialist);
int CvLuaCity::lGetExtraSpecialistYieldOfType(lua_State* L)
{
	return BasicLuaMethod<int, YieldTypes, SpecialistTypes>(L, &CvCity::getExtraSpecialistYield);
}

//------------------------------------------------------------------------------
//int getDomainFreeExperience(DomainTypes eIndex);
int CvLuaCity::lGetDomainFreeExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getDomainFreeExperience);
}
//------------------------------------------------------------------------------
//int getDomainProductionModifier(DomainTypes eIndex);
int CvLuaCity::lGetDomainProductionModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getDomainProductionModifier);
}
//------------------------------------------------------------------------------
//bool isEverOwned(PlayerTypes eIndex);
int CvLuaCity::lIsEverOwned(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isEverOwned);
}
//------------------------------------------------------------------------------
//bool isRevealed(TeamTypes eIndex, bool bDebug);
int CvLuaCity::lIsRevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isRevealed);
}
//------------------------------------------------------------------------------
//void setRevealed(TeamTypes eIndex, bool bNewValue);
int CvLuaCity::lSetRevealed(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setRevealed);
}
//------------------------------------------------------------------------------
//std::string getName();
int CvLuaCity::lGetName(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushstring(L, pkCity->getName());
	return 1;
}

//------------------------------------------------------------------------------
//std::string getNameForm(int iForm);
//------------------------------------------------------------------------------
//string GetNameKey();
int CvLuaCity::lGetNameKey(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushstring(L, pkCity->getNameKey());
	return 1;
}
//------------------------------------------------------------------------------
//void SetName(string szNewValue, bool bFound);
int CvLuaCity::lSetName(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	const char* cityName = lua_tostring(L, 2);
	const bool bFound = luaL_optbool(L, 3, false);

	pkCity->setName(cityName, bFound);
	return 0;
}
//------------------------------------------------------------------------------
//bool IsHasResourceLocal(ResourceTypes iResource);
int CvLuaCity::lIsHasResourceLocal(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResource = lua_tointeger(L, 2);
	const bool bTestVisible = lua_toboolean(L, 3);
	const bool bResult = pkCity->IsHasResourceLocal((ResourceTypes)iResource, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingProduction(BuildingTypes iIndex);
int CvLuaCity::lGetBuildingProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetBuildingProduction((BuildingTypes)iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//void setBuildingProduction(BuildingTypes iIndex, int iNewValue);
int CvLuaCity::lSetBuildingProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iNewValue = lua_tointeger(L, 3);
	if(iIndex != NO_BUILDING)
	{
		pkCity->GetCityBuildings()->SetBuildingProduction((BuildingTypes)iIndex, iNewValue);
	}

	return 1;
}
//------------------------------------------------------------------------------
//void changeBuildingProduction(BuildingTypes iIndex, int iChange);
int CvLuaCity::lChangeBuildingProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iChange = lua_tointeger(L, 3);
	if(iIndex != NO_BUILDING)
	{
		pkCity->GetCityBuildings()->ChangeBuildingProduction((BuildingTypes)iIndex, iChange);
	}

	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingProductionTime(BuildingTypes eIndex);
int CvLuaCity::lGetBuildingProductionTime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetBuildingProductionTime((BuildingTypes)iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//void setBuildingProductionTime(BuildingTypes eIndex, int iNewValue);
int CvLuaCity::lSetBuildingProductionTime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iNewValue = lua_tointeger(L, 3);
	if(iIndex != NO_BUILDING)
	{
		pkCity->GetCityBuildings()->SetBuildingProductionTime((BuildingTypes)iIndex, iNewValue);
	}

	return 1;
}
//------------------------------------------------------------------------------
//void changeBuildingProductionTime(BuildingTypes eIndex, int iChange);
int CvLuaCity::lChangeBuildingProductionTime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		pkCity->GetCityBuildings()->ChangeBuildingProductionTime((BuildingTypes)iIndex, toValue<int>(L, 3));
	}

	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingOriginalOwner(BuildingTypes iIndex);
int CvLuaCity::lGetBuildingOriginalOwner(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetBuildingOriginalOwner((BuildingTypes)iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, -1);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int getBuildingOriginalTime(BuildingTypes iIndex);
int CvLuaCity::lGetBuildingOriginalTime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetBuildingOriginalTime((BuildingTypes)iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int getUnitProduction(int iIndex);
int CvLuaCity::lGetUnitProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getUnitProduction(toValue<UnitTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setUnitProduction(int iIndex, int iNewValue);
int CvLuaCity::lSetUnitProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->setUnitProduction(toValue<UnitTypes>(L, 2), toValue<int>(L, 3));

	return 1;
}
//------------------------------------------------------------------------------
//void changeUnitProduction(UnitTypes iIndex, int iChange);
int CvLuaCity::lChangeUnitProduction(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->changeUnitProduction(toValue<UnitTypes>(L, 2), toValue<int>(L, 3));

	return 1;
}
//------------------------------------------------------------------------------
//int getGreatPeopleUnitRate(UnitTypes iIndex);
int CvLuaCity::lGetGreatPeopleUnitRate(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getGreatPeopleUnitRate(toValue<UnitTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getGreatPeopleUnitProgress(UnitTypes iIndex);
int CvLuaCity::lGetGreatPeopleUnitProgress(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getGreatPeopleUnitProgress(toValue<UnitTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setGreatPeopleUnitProgress(UnitTypes iIndex, int iNewValue);
int CvLuaCity::lSetGreatPeopleUnitProgress(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->setGreatPeopleUnitProgress(toValue<UnitTypes>(L, 2), toValue<int>(L, 3));

	return 1;
}
//------------------------------------------------------------------------------
//void changeGreatPeopleUnitProgress(UnitTypes iIndex, int iChange);
int CvLuaCity::lChangeGreatPeopleUnitProgress(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->changeGreatPeopleUnitProgress(toValue<UnitTypes>(L, 2), toValue<int>(L, 3));

	return 1;
}
//------------------------------------------------------------------------------
//int IsCanAddSpecialistToBuilding(BuildingTypes eBuilding);
int CvLuaCity::lIsCanAddSpecialistToBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	BuildingTypes bt = toValue<BuildingTypes>(L, 2);
	if(bt != NO_BUILDING)
	{
		const bool bResult = pkCity->GetCityCitizens()->IsCanAddSpecialistToBuilding(bt);
		lua_pushboolean(L, bResult);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int GetSpecialistUpgradeThreshold();
int CvLuaCity::lGetSpecialistUpgradeThreshold(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityCitizens()->GetSpecialistUpgradeThreshold();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumSpecialistsAllowedByBuilding(BuildingTypes eBuilding);
int CvLuaCity::lGetNumSpecialistsAllowedByBuilding(lua_State* L)
{
	int iResult = 0;
	CvCity* pkCity = GetInstance(L);
	BuildingTypes bt = toValue<BuildingTypes>(L, 2);
	if(bt != NO_BUILDING)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(bt);
		if(pkBuildingInfo)
		{
			iResult = pkCity->GetCityCitizens()->GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo);
		}
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getSpecialistCount(SpecialistTypes eIndex);
int CvLuaCity::lGetSpecialistCount(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetSpecialistCount(toValue<SpecialistTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetSpecialistGreatPersonProgress(SpecialistTypes eIndex);
int CvLuaCity::lGetSpecialistGreatPersonProgress(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetSpecialistGreatPersonProgress(toValue<SpecialistTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex);
int CvLuaCity::lGetSpecialistGreatPersonProgressTimes100(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetSpecialistGreatPersonProgressTimes100(toValue<SpecialistTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange);
int CvLuaCity::lChangeSpecialistGreatPersonProgressTimes100(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iChange = lua_tointeger(L, 3);
	pkCity->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(toValue<SpecialistTypes>(L, 2), iChange);

	return 1;
}

//------------------------------------------------------------------------------
//int GetNumSpecialistsInBuilding(BuildingTypes eIndex);
int CvLuaCity::lGetNumSpecialistsInBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetNumSpecialistsInBuilding(toValue<BuildingTypes>(L, 2));

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int DoReallocateCitizens();
int CvLuaCity::lDoReallocateCitizens(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->GetCityCitizens()->DoReallocateCitizens();

	return 1;
}

//------------------------------------------------------------------------------
//int DoVerifyWorkingPlots();
int CvLuaCity::lDoVerifyWorkingPlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->GetCityCitizens()->DoVerifyWorkingPlots();

	return 1;
}

//------------------------------------------------------------------------------
//int IsNoAutoAssignSpecialists();
int CvLuaCity::lIsNoAutoAssignSpecialists(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->GetCityCitizens()->IsNoAutoAssignSpecialists();

	lua_pushboolean(L, bResult);

	return 1;
}

//------------------------------------------------------------------------------
//int GetFocusType();
int CvLuaCity::lGetFocusType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityCitizens()->GetFocusType();

	lua_pushinteger(L, iResult);

	return 1;
}

//------------------------------------------------------------------------------
//int DoVerifyWorkingPlots();
int CvLuaCity::lSetFocusType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iFocus = lua_tointeger(L, 2);

	pkCity->GetCityCitizens()->SetFocusType((CityAIFocusTypes) iFocus);

	return 1;
}

//------------------------------------------------------------------------------
//int GetForcedAvoidGrowth();
int CvLuaCity::lIsForcedAvoidGrowth(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityCitizens()->IsForcedAvoidGrowth();

	lua_pushboolean(L, iResult);

	return 1;
}



//------------------------------------------------------------------------------
//int getUnitCombatFreeExperience(UnitCombatTypes eIndex);
int CvLuaCity::lGetUnitCombatFreeExperience(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getUnitCombatFreeExperience);
}
//------------------------------------------------------------------------------
//int getFreePromotionCount(PromotionTypes eIndex);
int CvLuaCity::lGetFreePromotionCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getFreePromotionCount);
}
//------------------------------------------------------------------------------
//bool isFreePromotion(PromotionTypes eIndex);
int CvLuaCity::lIsFreePromotion(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isFreePromotion);
}
//------------------------------------------------------------------------------
//int getSpecialistFreeExperience();
int CvLuaCity::lGetSpecialistFreeExperience(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getSpecialistFreeExperience();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void updateStrengthValue();
int CvLuaCity::lUpdateStrengthValue(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->updateStrengthValue();

	return 1;
}
//------------------------------------------------------------------------------
//int getStrengthValue();
int CvLuaCity::lGetStrengthValue(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getStrengthValue();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getDamage();
int CvLuaCity::lGetDamage(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->getDamage();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setDamage(int iValue);
int CvLuaCity::lSetDamage(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::setDamage);
}
//------------------------------------------------------------------------------
//void changeDamage(int iChange);
int CvLuaCity::lChangeDamage(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeDamage);
}
//------------------------------------------------------------------------------
//int GetMaxHitPoints();
int CvLuaCity::lGetMaxHitPoints(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetMaxHitPoints();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanRangeStrike()
int CvLuaCity::lCanRangeStrike(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::canRangeStrike);
}
//------------------------------------------------------------------------------
int CvLuaCity::lCanRangeStrikeNow(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::CanRangeStrikeNow);
}
//------------------------------------------------------------------------------
//bool CanRangeStrikeAt(int x, int y)
int CvLuaCity::lCanRangeStrikeAt(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::canRangeStrikeAt);
}
//------------------------------------------------------------------------------
int CvLuaCity::lHasPerformedRangedStrikeThisTurn(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isMadeAttack);
}
//------------------------------------------------------------------------------
int CvLuaCity::lRangeCombatUnitDefense(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkDefender = CvLuaUnit::GetInstance(L, 2);

	const int iDefenseStr = pkCity->rangeCombatUnitDefense(pkDefender);
	lua_pushinteger(L, iDefenseStr);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lRangeCombatDamage(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkDefendingUnit = CvLuaUnit::GetInstance(L, 2, false);
	CvCity* pkDefendingCity = GetInstance(L, 3, false);
	bool bIncludeRand = luaL_optbool(L, 4, false);

	const int iRangedDamage = pkCity->rangeCombatDamage(pkDefendingUnit, pkDefendingCity, bIncludeRand);
	lua_pushinteger(L, iRangedDamage);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetAirStrikeDefenseDamage(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkAttackingUnit = CvLuaUnit::GetInstance(L, 2, false);
	bool bIncludeRand = luaL_optbool(L, 3, false);

	const int iRangedDamage = pkCity->GetAirStrikeDefenseDamage(pkAttackingUnit, bIncludeRand);
	lua_pushinteger(L, iRangedDamage);
	return 1;
}
//------------------------------------------------------------------------------
//bool isWorkingPlot(CyPlot* pPlot);
int CvLuaCity::lIsWorkingPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->GetCityCitizens()->IsWorkingPlot(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void alterWorkingPlot(int iIndex);
int CvLuaCity::lAlterWorkingPlot(lua_State* L)
{
//	return BasicLuaMethod(L, &CvCity::GetCityCitizens()->DoAlterWorkingPlot);
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	pkCity->GetCityCitizens()->DoAlterWorkingPlot(iIndex);

	return 1;
}
//------------------------------------------------------------------------------
//bool IsForcedWorkingPlot(CyPlot* pPlot);
int CvLuaCity::lIsForcedWorkingPlot(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->GetCityCitizens()->IsForcedWorkingPlot(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetNumForcedWorkingPlots(CyPlot* pPlot);
int CvLuaCity::lGetNumForcedWorkingPlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityCitizens()->GetNumForcedWorkingPlots();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getNumRealBuilding(BuildingTypes iIndex);
int CvLuaCity::lGetNumRealBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetNumRealBuilding(iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//void setNumRealBuilding(BuildingTypes iIndex, int iNewValue);
int CvLuaCity::lSetNumRealBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iNewValue = lua_tointeger(L, 3);
		pkCity->GetCityBuildings()->SetNumRealBuilding(iIndex, iNewValue);
	}

	return 1;
}
//------------------------------------------------------------------------------
//int getNumFreeBuilding(BuildingTypes iIndex);
int CvLuaCity::lGetNumFreeBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetNumFreeBuilding(iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//bool IsBuildingSellable(BuildingTypes iIndex);
int CvLuaCity::lIsBuildingSellable(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(iIndex);
		if(pkBuildingInfo)
		{
			bResult = pkCity->GetCityBuildings()->IsBuildingSellable(*pkBuildingInfo);
		}
	}

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetSellBuildingRefund(BuildingTypes iIndex);
int CvLuaCity::lGetSellBuildingRefund(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iResult = pkCity->GetCityBuildings()->GetSellBuildingRefund(iIndex);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}

//------------------------------------------------------------------------------
//int GetTotalBaseBuildingMaintenance(BuildingTypes iIndex);
int CvLuaCity::lGetTotalBaseBuildingMaintenance(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityBuildings()->GetTotalBaseBuildingMaintenance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void clearOrderQueue();
int CvLuaCity::lClearOrderQueue(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::clearOrderQueue);
}
//------------------------------------------------------------------------------
//void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce);
int CvLuaCity::lPushOrder(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const OrderTypes eOrder	= (OrderTypes)lua_tointeger(L, 2);
	const int iData1		= lua_tointeger(L, 3);
	const int iData2		= lua_tointeger(L, 4);
	const bool bSave		= lua_tointeger(L, 5);
	const bool bPop			= lua_toboolean(L, 6);
	const bool bAppend		= lua_toboolean(L, 7);
	const bool bForce		= luaL_optint(L, 8, 0);
	pkCity->pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);

	return 1;
}
//------------------------------------------------------------------------------
//void popOrder(int iNum, bool bFinish, bool bChoose);
int CvLuaCity::lPopOrder(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iNum = lua_tointeger(L, 2);
	const bool bFinish = luaL_optint(L, 3, 0);
	const bool bChoose = luaL_optint(L, 4, 0);
	pkCity->popOrder(iNum, bFinish, bChoose);

	return 1;
}
//------------------------------------------------------------------------------
//int getOrderQueueLength();
int CvLuaCity::lGetOrderQueueLength(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getOrderQueueLength);
}
//------------------------------------------------------------------------------
//OrderData* getOrderFromQueue(int iIndex);
//------------------------------------------------------------------------------
int CvLuaCity::lGetOrderFromQueue(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	if(pkCity)
	{
		const int iNum = lua_tointeger(L, 2);
		OrderData* pkOrder = pkCity->getOrderFromQueue(iNum);
		if(pkOrder)
		{
			lua_pushinteger(L, pkOrder->eOrderType);
			lua_pushinteger(L, pkOrder->iData1);
			lua_pushinteger(L, pkOrder->iData1);
			lua_pushboolean(L, pkOrder->bSave);
			lua_pushboolean(L, pkOrder->bRush);
			return 5;
		}
	}
	lua_pushinteger(L, -1);
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_pushboolean(L, false);
	lua_pushboolean(L, false);
	return 5;
}

//int getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield);
int CvLuaCity::lGetBuildingYieldChange(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int eBuildingClass = lua_tointeger(L, 2);
	const int eYield = lua_tointeger(L, 3);
	const int iResult = pkCity->GetCityBuildings()->GetBuildingYieldChange((BuildingClassTypes)eBuildingClass, (YieldTypes)eYield);

	lua_pushinteger(L, iResult);
	return 1;
}

//int GetBuildingEspionageModifier(BuildingClassTypes eBuildingClass)
int CvLuaCity::lGetBuildingEspionageModifier(lua_State* L)
{
	//CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);
	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvAssertMsg(pBuildingInfo, "pBuildingInfo is null!");
	if (pBuildingInfo)
	{
		lua_pushinteger(L, pBuildingInfo->GetEspionageModifier());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}

// int GetBuildingGlobalEspionageModifier(BuildingClassTypes eBuildingClass)
int CvLuaCity::lGetBuildingGlobalEspionageModifier(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes)lua_tointeger(L, 2);
	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvAssertMsg(pBuildingInfo, "pBuildingInfo is null!");
	if (pBuildingInfo)
	{
		lua_pushinteger(L, pBuildingInfo->GetGlobalEspionageModifier());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}

//------------------------------------------------------------------------------
//void setBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
int CvLuaCity::lSetBuildingYieldChange(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int eBuildingClass = lua_tointeger(L, 2);
	const int eYield = lua_tointeger(L, 3);
	const int iChange = lua_tointeger(L, 4);

	pkCity->GetCityBuildings()->SetBuildingYieldChange((BuildingClassTypes)eBuildingClass, (YieldTypes)eYield, iChange);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaCity::lGetNumCityPlots(lua_State* L)
{
	lua_pushinteger(L, NUM_CITY_PLOTS);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaCity::lCanPlaceUnitHere(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iUnitType = lua_tointeger(L, 2);
	lua_pushboolean(L, pkCity->CanPlaceUnitHere((UnitTypes)iUnitType));

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetSpecialistYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes) lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 3);

	const PlayerTypes ePlayer = pkCity->getOwner();

	const int iValue = GET_PLAYER(ePlayer).specialistYield(eSpecialist, eYield);

	lua_pushinteger(L, iValue);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetCultureFromSpecialist(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetCultureFromSpecialist);
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetReligionCityRangeStrikeModifier(lua_State* L)
{
	int iReligionRangeStrikeMod = 0;

	CvCity* pkCity = GetInstance(L);
	ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pkCity->getOwner());
		if(pReligion)
		{
			iReligionRangeStrikeMod = pReligion->m_Beliefs.GetCityRangeStrikeModifier();
		}
	}

	lua_pushinteger(L, iReligionRangeStrikeMod);

	return 1;
}

