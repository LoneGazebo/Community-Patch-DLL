/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS) || defined (MOD_BALANCE_CORE_UNIT_INVESTMENTS)
#include "../CvInternalGameCoreUtils.h"
#endif

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
#if defined(MOD_BALANCE_CORE)
	Method(IsBlockadedTest);
#endif
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
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	Method(GetBuildingInvestment);
	Method(IsWorldWonder);
	Method(GetWorldWonderCost);
	Method(GetNumPoliciesNeeded);
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	Method(GetUnitInvestment);
#endif
	Method(GetProjectProductionNeeded);
	Method(GetProductionTurnsLeft);
	Method(GetUnitProductionTurnsLeft);
	Method(GetBuildingProductionTurnsLeft);
	Method(GetProjectProductionTurnsLeft);
	Method(GetSpecialistProductionTurnsLeft);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_PROCESS_STOCKPILE)
	Method(GetProcessProductionTurnsLeft);
#endif

	Method(CreateApolloProgram);

	Method(IsCanPurchase);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(Purchase);
#endif
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
	Method(IsOriginalMajorCapital);
	Method(IsCoastal);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(IsAddsFreshWater);
	Method(FoodConsumptionSpecialistTimes100);
#endif

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
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetNumBuildingClass);
	Method(IsHasBuildingClass);
	Method(GetLocalBuildingClassYield);
	Method(GetEventBuildingClassYield);
	Method(GetEventBuildingClassModifier);
	Method(GetEventCityYield);
#endif
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
	Method(GetSpecialistRate);
	Method(GetTotalGreatPeopleRateModifier);
	Method(ChangeBaseGreatPeopleRate);
	Method(GetGreatPeopleRateModifier);

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
	Method(GetJONSCulturePerTurnFromGreatWorks);
	Method(GetJONSCulturePerTurnFromTraits);
#if defined(MOD_BALANCE_CORE)
	Method(GetYieldPerTurnFromTraits);
	Method(GetYieldFromUnitsInCity);
#endif
#if defined(MOD_BUGFIX_LUA_API)
	Method(ChangeJONSCulturePerTurnFromReligion);
#endif
	Method(GetJONSCulturePerTurnFromReligion);
	Method(GetJONSCulturePerTurnFromLeagues);

	Method(GetCultureRateModifier);
	Method(ChangeCultureRateModifier);
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetCityYieldModFromMonopoly);
#endif
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetTourismRateModifier);
	Method(ChangeTourismRateModifier);
#endif

	Method(GetNumGreatWorks);
	Method(GetNumGreatWorkSlots);
	Method(GetBaseTourism);
#if defined(MOD_BALANCE_CORE)
	Method(RefreshTourism);
	Method(GetNumGreatWorksFilled);
	Method(GetNumAvailableGreatWorkSlots);
#endif
	Method(GetTourismMultiplier);
	Method(GetTourismTooltip);
	Method(GetFilledSlotsTooltip);
	Method(GetTotalSlotsTooltip);
	Method(ClearGreatWorks);
	Method(GetFaithBuildingTourism);

	Method(IsThemingBonusPossible);
	Method(GetThemingBonus);
	Method(GetThemingTooltip);

	Method(GetFaithPerTurn);
	Method(GetFaithPerTurnFromBuildings);
	Method(GetFaithPerTurnFromPolicies);
	Method(GetFaithPerTurnFromTraits);
	Method(GetFaithPerTurnFromReligion);
#if defined(MOD_BUGFIX_LUA_API)
	Method(ChangeFaithPerTurnFromReligion);
#endif

	Method(IsReligionInCity);
	Method(IsHolyCityForReligion);
	Method(IsHolyCityAnyReligion);

	Method(GetNumFollowers);
	Method(GetReligiousMajority);
	Method(GetSecondaryReligion);
	Method(GetSecondaryReligionPantheonBelief);
	Method(GetPressurePerTurn);
	Method(ConvertPercentFollowers);
	Method(AdoptReligionFully);
	Method(GetReligionBuildingClassHappiness);
	Method(GetReligionBuildingClassYieldChange);
	Method(GetLeagueBuildingClassYieldChange);
	Method(GetNumTradeRoutesAddingPressure);

	Method(GetNumWorldWonders);
	Method(GetNumTeamWonders);
	Method(GetNumNationalWonders);
	Method(GetNumBuildings);
#if defined(MOD_BALANCE_CORE)
	Method(GetNumTotalBuildings);
#endif

	Method(GetWonderProductionModifier);
	Method(ChangeWonderProductionModifier);

	Method(GetLocalResourceWonderProductionMod);

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetBuyPlotDistance);
	Method(GetWorkPlotDistance);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BUILDINGS_CITY_WORKING)
	Method(GetCityWorkingChange);
	Method(ChangeCityWorkingChange);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_HAPPINESS)
	Method(GetTheoreticalUnhappinessDecrease);
	Method(getHappinessDelta);
	Method(getThresholdSubtractions);
	Method(getThresholdAdditions);
	Method(GetUnhappinessFromCultureYield);
	Method(GetUnhappinessFromCultureNeeded);
	Method(GetUnhappinessFromCultureDeficit);
	Method(GetUnhappinessFromCulture);
	Method(GetUnhappinessFromScienceYield);
	Method(GetUnhappinessFromScienceNeeded);
	Method(GetUnhappinessFromScienceDeficit);
	Method(GetUnhappinessFromScience);
	Method(GetUnhappinessFromDefenseYield);
	Method(GetUnhappinessFromDefenseNeeded);
	Method(GetUnhappinessFromDefenseDeficit);
	Method(GetUnhappinessFromDefense);
	Method(GetUnhappinessFromGoldYield);
	Method(GetUnhappinessFromGoldNeeded);
	Method(GetUnhappinessFromGoldDeficit);
	Method(GetUnhappinessFromGold);
	Method(GetUnhappinessFromConnection);
	Method(GetUnhappinessFromPillaged);
	Method(GetUnhappinessFromStarving);
	Method(GetUnhappinessFromMinority);
#endif

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
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(ChangeOverflowProduction);
#endif
	Method(GetFeatureProduction);
	Method(SetFeatureProduction);
	Method(GetMilitaryProductionModifier);
	Method(GetSpaceProductionModifier);
	Method(GetBuildingDefense);
	Method(GetFreeExperience);
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

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	Method(GetBaseYieldRateFromGreatWorks);
#endif

	Method(GetBaseYieldRateFromTerrain);
	Method(ChangeBaseYieldRateFromTerrain);

	Method(GetBaseYieldRateFromBuildings);
	Method(ChangeBaseYieldRateFromBuildings);

	Method(GetBaseYieldRateFromSpecialists);
	Method(ChangeBaseYieldRateFromSpecialists);

	Method(GetBaseYieldRateFromMisc);
	Method(ChangeBaseYieldRateFromMisc);

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(GetBaseYieldRateFromProcess);
	Method(GetBaseYieldRateFromTradeRoutes);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CITYSTATES)
	// Base yield rate from League
	Method(GetBaseYieldRateFromLeague);
#endif
#if defined(MOD_BALANCE_CORE)
	Method(GetScienceFromCityYield);
#endif

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

	Method(IsCanAddSpecialistToBuilding);
	Method(GetSpecialistUpgradeThreshold);
	Method(GetNumSpecialistsAllowedByBuilding);
	Method(GetSpecialistCount);
#if defined(MOD_BALANCE_CORE)
	Method(GetTotalSpecialistCount);
	Method(GetSpecialistCityModifier);
#endif
	Method(GetSpecialistGreatPersonProgress);
	Method(GetSpecialistGreatPersonProgressTimes100);
	Method(ChangeSpecialistGreatPersonProgressTimes100);
	Method(GetExtraSpecialistPoints);
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
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_EVENTS_CITY_BOMBARD)
	Method(GetBombardRange);
#endif
	Method(CanRangeStrike);
	Method(CanRangeStrikeNow);
	Method(CanRangeStrikeAt);
	Method(HasPerformedRangedStrikeThisTurn);
	Method(RangeCombatUnitDefense);
	Method(RangeCombatDamage);
	Method(GetAirStrikeDefenseDamage);
	Method(GetMultiAttackBonusCity);

	Method(IsWorkingPlot);
	Method(AlterWorkingPlot);
	Method(IsForcedWorkingPlot);
	Method(GetNumRealBuilding);
	Method(SetNumRealBuilding);
	Method(GetNumFreeBuilding);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(SetNumFreeBuilding);
#endif
	Method(IsBuildingSellable);
	Method(GetSellBuildingRefund);
	Method(GetTotalBaseBuildingMaintenance);
	Method(GetBuildingGreatWork);
	Method(SetBuildingGreatWork);
	Method(IsHoldingGreatWork);
	Method(GetNumGreatWorksInBuilding);

	Method(ClearOrderQueue);
	Method(PushOrder);
	Method(PopOrder);
	Method(GetOrderQueueLength);

	Method(GetBuildingYieldChange);
	Method(SetBuildingYieldChange);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_POLICIES)
	Method(GetBuildingClassCultureChange);
	Method(GetReligionYieldRateModifier);
	Method(GetReligionBuildingYieldRateModifier);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE)
	Method(GetBaseYieldRateFromCSAlliance);
	Method(GetBuildingYieldChangeFromCorporationFranchises);
	Method(GetYieldChangeFromCorporationFranchises);
	Method(GetTradeRouteCityMod);
	Method(GetGreatWorkYieldMod);
	Method(GetActiveSpyYieldMod);
	Method(GetResourceQuantityPerXFranchises);
	Method(GetGPRateModifierPerXFranchises);
	Method(IsFranchised);
	Method(DoFranchiseAtCity);
	Method(HasOffice);
	Method(GetYieldChangeTradeRoute);
	Method(GetSpecialistYieldChange);
	Method(GetModFromWLTKD);
	Method(GetCultureModFromCarnaval);
	Method(GetModFromGoldenAge);
#endif
	Method(GetBuildingEspionageModifier);
	Method(GetBuildingGlobalEspionageModifier);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_ESPIONAGE)
	Method(HasDiplomat);
	Method(HasSpy);
	Method(HasCounterSpy);
	Method(GetCounterSpy);
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_RELIGION_CONVERSION_MODIFIERS)
	Method(GetBuildingConversionModifier);
	Method(GetBuildingGlobalConversionModifier);
#endif

	Method(GetNumCityPlots);
	Method(CanPlaceUnitHere);

	Method(GetSpecialistYield);
	Method(GetCultureFromSpecialist);

	Method(GetNumForcedWorkingPlots);

	Method(GetReligionCityRangeStrikeModifier);

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(AddMessage);
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(HasBelief);
	Method(HasBuilding);
	Method(HasBuildingClass);
	Method(HasAnyWonder);
	Method(HasWonder);
	Method(IsCivilization);
	Method(HasFeature);
	Method(HasWorkedFeature);
	Method(HasAnyNaturalWonder);
	Method(HasNaturalWonder);
	Method(HasImprovement);
	Method(HasWorkedImprovement);
	Method(HasPlotType);
	Method(HasWorkedPlotType);
	Method(HasAnyReligion);
	Method(HasReligion);
	Method(HasResource);
	Method(HasWorkedResource);
	Method(IsConnectedToCapital);
	Method(IsConnectedTo);
	Method(HasSpecialistSlot);
	Method(HasSpecialist);
	Method(HasTerrain);
	Method(HasWorkedTerrain);
	Method(HasAnyDomesticTradeRoute);
	Method(HasAnyInternationalTradeRoute);
	Method(HasTradeRouteToAnyCity);
	Method(HasTradeRouteTo);
	Method(HasTradeRouteFromAnyCity);
	Method(HasTradeRouteFrom);
	Method(IsOnFeature);
	Method(IsAdjacentToFeature);
	Method(IsWithinDistanceOfFeature);
#if defined(MOD_BALANCE_CORE)
	Method(IsWithinDistanceOfUnit);
	Method(IsWithinDistanceOfUnitClass);
	Method(IsWithinDistanceOfUnitCombatType);
	Method(IsWithinDistanceOfUnitPromotion);
#endif
	Method(IsOnImprovement);
	Method(IsAdjacentToImprovement);
	Method(IsWithinDistanceOfImprovement);
	Method(IsOnPlotType);
	Method(IsAdjacentToPlotType);
	Method(IsWithinDistanceOfPlotType);
	Method(IsOnResource);
	Method(IsAdjacentToResource);
	Method(IsWithinDistanceOfResource);
	Method(IsOnTerrain);
	Method(IsAdjacentToTerrain);
	Method(IsWithinDistanceOfTerrain);
	Method(CountNumWorkedFeature);
	Method(CountNumWorkedImprovement);
	Method(CountNumWorkedResource);
	Method(CountNumImprovement);
	Method(CountNumWorkedRiverTiles);
	Method(CountFeature);
	Method(CountWorkedFeature);
	Method(CountImprovement);
	Method(CountWorkedImprovement);
	Method(CountPlotType);
	Method(CountWorkedPlotType);
	Method(CountResource);
	Method(CountWorkedResource);
	Method(CountTerrain);
	Method(CountWorkedTerrain);
	Method(GetAdditionalFood);
	Method(SetAdditionalFood);
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
	Method(GetDisabledTooltip);
	Method(GetScaledEventChoiceValue);
	Method(IsCityEventChoiceActive);
	Method(DoCityEventChoice);
	Method(DoCityStartEvent);
	Method(DoCancelCityEventChoice);
	Method(GetCityEventCooldown);
	Method(SetCityEventCooldown);
	Method(GetCityEventChoiceCooldown);
	Method(SetCityEventChoiceCooldown);
	Method(IsCityEventChoiceValid);
#endif


#if defined(MOD_BALANCE_CORE_JFD)
	Method(IsColony);
	Method(SetColony);

	Method(GetProvinceLevel);
	Method(SetProvinceLevel);
	Method(HasProvinceLevel);

	Method(GetOrganizedCrime);
	Method(SetOrganizedCrime);
	Method(HasOrganizedCrime);

	Method(ChangeResistanceCounter);
	Method(SetResistanceCounter);
	Method(GetResistanceCounter);

	Method(ChangePlagueCounter);
	Method(SetPlagueCounter);
	Method(GetPlagueCounter);

	Method(GetPlagueTurns);
	Method(ChangePlagueTurns);
	Method(SetPlagueTurns);

	Method(GetPlagueType);
	Method(SetPlagueType);
	Method(HasPlague);

	Method(ChangeLoyaltyCounter);
	Method(SetLoyaltyCounter);
	Method(GetLoyaltyCounter);

	Method(ChangeDisloyaltyCounter);
	Method(SetDisloyaltyCounter);
	Method(GetDisloyaltyCounter);

	Method(GetLoyaltyState);
	Method(SetLoyaltyState);
	Method(HasLoyaltyState);

	Method(GetYieldModifierFromHappiness);
	Method(SetYieldModifierFromHappiness);

	Method(GetYieldModifierFromHealth);
	Method(SetYieldModifierFromHealth);

	Method(GetYieldModifierFromCrime);
	Method(SetYieldModifierFromCrime);

	Method(GetYieldModifierFromDevelopment);
	Method(SetYieldModifierFromDevelopment);

	Method(GetYieldFromHappiness);
	Method(SetYieldFromHappiness);

	Method(GetYieldFromHealth);
	Method(SetYieldFromHealth);

	Method(GetYieldFromCrime);
	Method(SetYieldFromCrime);

	Method(GetYieldFromDevelopment);
	Method(SetYieldFromDevelopment);
#endif
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
//int getCityPlotIndex(CvPlot* pPlot);
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
//bool canWork(CvPlot* pPlot);
int CvLuaCity::lCanWork(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkCity->GetCityCitizens()->IsCanWork(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsBlockaded(CvPlot* pPlot);
int CvLuaCity::lIsPlotBlockaded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkPlot->isBlockaded(pkCity->getOwner());

	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//bool IsBlockadedTest(CvPlot* pPlot);
int CvLuaCity::lIsBlockadedTest(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bResult = pkCity->IsBlockadedWaterAndLand();

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
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
//bool canTrain( int iUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bWillPurchase);
int CvLuaCity::lCanTrain(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iUnit = lua_tointeger(L, 2);
	const bool bContinue = luaL_optint(L, 3, 0);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bIgnoreCost = luaL_optint(L, 5, 0);
	const bool bWillPurchase = luaL_optint(L, 6, 0);
	const bool bResult = pkCity->canTrain((UnitTypes)iUnit, bContinue, bTestVisible, bIgnoreCost, bWillPurchase);

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
#if defined(MOD_API_EXTENSIONS)
	pkCity->canConstruct(eBuilding, false, false, false, false, &toolTip);
#else
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);
#endif

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
#if defined(MOD_API_EXTENSIONS)
	const bool bWillPurchase = luaL_optint(L, 6, 0);
	const bool bResult = pkCity->canConstruct((BuildingTypes)iBuilding, bContinue, bTestVisible, bIgnoreCost, bWillPurchase);
#else
	const bool bResult = pkCity->canConstruct((BuildingTypes)iBuilding, bContinue, bTestVisible, bIgnoreCost);
#endif

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
#if defined(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED)
	if(MOD_BALANCE_CORE_DIPLOMACY_ADVANCED && GET_PLAYER(pkCity->getOwner()).GetNumUnitsOutOfSupply() > 0)
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_NO_ACTION_NO_SUPPLY_PURCHASE");

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE)
	{
		if (GC.getUnitInfo(eUnit)->GetCombat() > 0 || GC.getUnitInfo(eUnit)->GetRangedCombat() > 0)
		{
			if (pkCity->GetUnitPurchaseCooldown() > 0)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_COOLDOWN_X_TURNS_REMAINING");
				localizedText << pkCity->GetUnitPurchaseCooldown();

				const char* const localized = localizedText.toUTF8();
				if (localized)
				{
					if (!toolTip.IsEmpty())
						toolTip += "[NEWLINE]";

					toolTip += localized;
				}
			}
		}
		else if(pkCity->GetUnitPurchaseCooldown(true) > 0)
		{
			Localization::String localizedText = Localization::Lookup("TXT_KEY_COOLDOWN_X_TURNS_REMAINING");
			localizedText << pkCity->GetUnitPurchaseCooldown(true);

			const char* const localized = localizedText.toUTF8();
			if (localized)
			{
				if (!toolTip.IsEmpty())
					toolTip += "[NEWLINE]";

				toolTip += localized;
			}
		}
	}
	if(eUnit != NO_UNIT)
	{
		CvUnitEntry* thisUnitInfo = GC.getUnitInfo(eUnit);
		// See if there are any BuildingClass requirements
		const int iNumBuildingClassInfos = GC.getNumBuildingClassInfos();
		const CvCivilizationInfo& thisCivilization = pkCity->getCivilizationInfo();
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < iNumBuildingClassInfos; iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			// Requires Building
			if(thisUnitInfo->GetBuildingClassPurchaseRequireds(eBuildingClass))
			{
				const BuildingTypes ePrereqBuilding = (BuildingTypes)(thisCivilization.getCivilizationBuildings(eBuildingClass));

				if(pkCity->GetCityBuildings()->GetNumBuilding(ePrereqBuilding) == 0)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(ePrereqBuilding);
					if(pkBuildingInfo)
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_NO_ACTION_UNIT_REQUIRES_BUILDING_PURCHASE");
						localizedText << pkBuildingInfo->GetDescriptionKey();

						const char* const localized = localizedText.toUTF8();
						if(localized)
						{
							if(!toolTip.IsEmpty())
								toolTip += "[NEWLINE]";

							toolTip += localized;
						}
					}
				}
			}
		}
	}
#endif

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
	CvUnitEntry* pGameUnit = GC.getUnitInfo(eUnit);
	if(MOD_BALANCE_CORE && pGameUnit && GET_PLAYER(pkCity->getOwner()).GetFaithPurchaseCooldown() > 0 && pGameUnit->GetGlobalFaithCooldown() > 0)
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_COOLDOWN_X_TURNS_REMAINING_FAITH");
		localizedText << GET_PLAYER(pkCity->getOwner()).GetFaithPurchaseCooldown();

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}
#if defined(MOD_BALANCE_CORE_UNIT_INVESTMENTS)
	if(MOD_BALANCE_CORE_UNIT_INVESTMENTS && eUnit != NO_UNIT)
	{
		//Have we already invested here?
		
		const UnitClassTypes eUnitClass = (UnitClassTypes)(pGameUnit->GetUnitClassType());
		if(pkCity->IsUnitInvestment(eUnitClass))
		{
			int iValue = (/*-50*/ GC.getBALANCE_UNIT_INVESTMENT_BASELINE() + GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetInvestmentModifier() + GET_PLAYER(pkCity->getOwner()).GetInvestmentModifier());
			iValue *= -1;

			Localization::String localizedText = Localization::Lookup("TXT_KEY_ALREADY_INVESTED_UNIT");
			localizedText << iValue;

			const char* const localized = localizedText.toUTF8();
			if(localized)
			{
				if(!toolTip.IsEmpty())
					toolTip += "[NEWLINE]";

				toolTip += localized;
			}
		}
	}
#endif

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
#if defined(MOD_API_EXTENSIONS)
	pkCity->canConstruct(eBuilding, false, false, false, false, &toolTip);
#else
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);
#endif
#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE && pkCity->GetBuildingPurchaseCooldown() > 0)
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_COOLDOWN_X_TURNS_REMAINING");
		localizedText << pkCity->GetBuildingPurchaseCooldown();

		const char* const localized = localizedText.toUTF8();
		if(localized)
		{
			if(!toolTip.IsEmpty())
				toolTip += "[NEWLINE]";

			toolTip += localized;
		}
	}
#endif
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
	if(MOD_BALANCE_CORE_BUILDING_INVESTMENTS && eBuilding != NO_BUILDING)
	{
		//Have we already invested here?
		CvBuildingEntry* pGameBuilding = GC.getBuildingInfo(eBuilding);
		const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pGameBuilding->GetBuildingClassType());
		if(pkCity->IsBuildingInvestment(eBuildingClass))
		{
			int iValue = (/*-50*/ GC.getBALANCE_BUILDING_INVESTMENT_BASELINE() + GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetInvestmentModifier() + GET_PLAYER(pkCity->getOwner()).GetInvestmentModifier());
			iValue *= -1;
			const CvBuildingClassInfo& kBuildingClassInfo = pGameBuilding->GetBuildingClassInfo();
			if(::isWorldWonderClass(kBuildingClassInfo))
			{
				iValue /= 2;
			}
			Localization::String localizedText = Localization::Lookup("TXT_KEY_ALREADY_INVESTED");
			localizedText << iValue;

			const char* const localized = localizedText.toUTF8();
			if(localized)
			{
				if(!toolTip.IsEmpty())
					toolTip += "[NEWLINE]";

				toolTip += localized;
			}
		}
	}
#endif
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
#if defined(MOD_API_EXTENSIONS)
	pkCity->canConstruct(eBuilding, false, false, false, false, &toolTip);
#else
	pkCity->canConstruct(eBuilding, false, false, false, &toolTip);
#endif

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
#if defined(MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
//------------------------------------------------------------------------------
//bool IsBuildingInvestment();
int CvLuaCity::lGetBuildingInvestment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iResult = 0;
	int iTotalDiscount = 0;
	const BuildingTypes eBuildingType = (BuildingTypes) lua_tointeger(L, 2);
	CvBuildingEntry* pGameBuilding = GC.getBuildingInfo(eBuildingType);
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)(pGameBuilding->GetBuildingClassType());
	if(pkCity->IsBuildingInvestment(eBuildingClass))
	{
		iResult = GET_PLAYER(pkCity->getOwner()).getProductionNeeded(eBuildingType);
		iTotalDiscount = (/*-50*/ GC.getBALANCE_BUILDING_INVESTMENT_BASELINE() + GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetInvestmentModifier() + GET_PLAYER(pkCity->getOwner()).GetInvestmentModifier());
		const CvBuildingClassInfo& kBuildingClassInfo = pGameBuilding->GetBuildingClassInfo();
		if(::isWorldWonderClass(kBuildingClassInfo))
		{
			iTotalDiscount /= 2;
		}
		iResult *= (iTotalDiscount + 100);
		iResult /= 100;
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsWorldWonder();
int CvLuaCity::lIsWorldWonder(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	bool bResult = false;
	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	CvBuildingEntry* pGameBuilding = GC.getBuildingInfo(eBuildingType);
	const CvBuildingClassInfo& kBuildingClassInfo = pGameBuilding->GetBuildingClassInfo();
	if (::isWorldWonderClass(kBuildingClassInfo))
	{
		bResult = true;
	}

	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaCity::lGetWorldWonderCost(lua_State* L)
{
	int iNumWorldWonderPercent = 0;
	CvCity* pkCity = GetInstance(L);

	const BuildingTypes eBuildingType = (BuildingTypes)lua_tointeger(L, 2);
	CvBuildingEntry* pGameBuilding = GC.getBuildingInfo(eBuildingType);
	const CvBuildingClassInfo& kBuildingClassInfo = pGameBuilding->GetBuildingClassInfo();
	if (MOD_BALANCE_CORE_WONDER_COST_INCREASE && ::isWorldWonderClass(kBuildingClassInfo))
	{
		const CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = GET_PLAYER(pkCity->getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(pkCity->getOwner()).nextCity(&iLoop))
		{
			if (pLoopCity->getNumWorldWonders() > 0)
			{
				for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
				{
					const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
					CvBuildingEntry* pkeBuildingInfo = GC.getBuildingInfo(eBuilding);

					// Has this Building
					if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						if (isWorldWonderClass(pkeBuildingInfo->GetBuildingClassInfo()))
						{
							if (pkeBuildingInfo->GetPrereqAndTech() != NO_TECH)
							{
								CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes)pkeBuildingInfo->GetPrereqAndTech());
								if (pkTechInfo)
								{
									// Loop through all eras and apply Building production mod based on how much time has passed
									EraTypes eBuildingUnlockedEra = (EraTypes)pkTechInfo->GetEra();

									if (eBuildingUnlockedEra == GET_PLAYER(pkCity->getOwner()).GetCurrentEra())
									{
										iNumWorldWonderPercent += GC.getBALANCE_CORE_WORLD_WONDER_SAME_ERA_COST_MODIFIER();
									}
									else if ((GET_PLAYER(pkCity->getOwner()).GetCurrentEra() - eBuildingUnlockedEra) == 1)
									{
										iNumWorldWonderPercent += GC.getBALANCE_CORE_WORLD_WONDER_PREVIOUS_ERA_COST_MODIFIER();
									}
									else if ((GET_PLAYER(pkCity->getOwner()).GetCurrentEra() - eBuildingUnlockedEra) > 1)
									{
										iNumWorldWonderPercent += GC.getBALANCE_CORE_WORLD_WONDER_EARLIER_ERA_COST_MODIFIER();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	lua_pushinteger(L, iNumWorldWonderPercent);
	return 1;
}
int CvLuaCity::lGetNumPoliciesNeeded(lua_State* L)
{
	int iNumPoliciesReduced = 0;
	int iTotalPoliciesNeeded = 0;
	CvCity* pkCity = GetInstance(L);

	bool bIgnoreRequirements = false;
	const BuildingTypes eBuilding = (BuildingTypes)lua_tointeger(L, 2);
	if (eBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pBuildingInfo)
		{
			//If # of policies will do it, then we need to see the either/or here.
			iTotalPoliciesNeeded = pBuildingInfo->GetNumPoliciesNeeded();
			if (iTotalPoliciesNeeded > 0)
			{
				CvGameReligions* pReligions = GC.getGame().GetGameReligions();
				ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(pkCity->getOwner());
				if (eFoundedReligion == NO_RELIGION)
				{
					eFoundedReligion = GET_PLAYER(pkCity->getOwner()).GetReligions()->GetReligionInMostCities();
				}
				if (eFoundedReligion != NO_RELIGION)
				{
					const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, pkCity->getOwner());
					if (pReligion)
					{
						CvCity* pHolyCity = NULL;
						CvPlot* pHolyCityPlot = GC.getMap().plot(pReligion->m_iHolyCityX, pReligion->m_iHolyCityY);
						if (pHolyCityPlot)
						{
							pHolyCity = pHolyCityPlot->getPlotCity();
						}
						if (pHolyCity == NULL)
						{
							pHolyCity = GET_PLAYER(pkCity->getOwner()).getCapitalCity();
						}
						// Depends on era of wonder
						EraTypes eEra;
						TechTypes eTech = (TechTypes)pBuildingInfo->GetPrereqAndTech();
						if (eTech != NO_TECH)
						{
							CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eTech);
							if (pEntry)
							{
								eEra = (EraTypes)pEntry->GetEra();
								if (eEra != NO_ERA)
								{
									bIgnoreRequirements = pReligion->m_Beliefs.IsIgnorePolicyRequirements(eEra, pkCity->getOwner(), pHolyCity);
								}
							}
						}
						if (!bIgnoreRequirements)
						{
							int iReligionPolicyReduction = pReligion->m_Beliefs.GetPolicyReductionWonderXFollowerCities(pkCity->getOwner(), pHolyCity);
							if (iReligionPolicyReduction > 0)
							{
								int iNumFollowerCities = pReligions->GetNumCitiesFollowing(eFoundedReligion);
								if (iNumFollowerCities > 0)
								{
									iNumPoliciesReduced += (iNumFollowerCities / iReligionPolicyReduction);
								}
							}
						}
					}
				}
				if (!bIgnoreRequirements)
				{
					int iCSPolicyReduction = GET_PLAYER(pkCity->getOwner()).GetCSAlliesLowersPolicyNeedWonders();
					if (iCSPolicyReduction > 0)
					{
						int iNumAllies = GET_PLAYER(pkCity->getOwner()).GetNumCSAllies();
						iNumPoliciesReduced += (iNumAllies / iCSPolicyReduction);
					}
				}
			}
		}
	}
	if (bIgnoreRequirements)
	{
		iTotalPoliciesNeeded = 0;
	}
	else
	{
		iTotalPoliciesNeeded -= iNumPoliciesReduced;
	}

	lua_pushinteger(L, iTotalPoliciesNeeded);
	return 1;
}

//------------------------------------------------------------------------------
//bool IsUnitInvestment();
int CvLuaCity::lGetUnitInvestment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iResult = 0;
	int iTotalDiscount = 0;
	const UnitTypes eUnitType = (UnitTypes) lua_tointeger(L, 2);
	CvUnitEntry* pGameUnit = GC.getUnitInfo(eUnitType);
	const UnitClassTypes eUnitClass = (UnitClassTypes)(pGameUnit->GetUnitClassType());
	if(pkCity->IsUnitInvestment(eUnitClass))
	{
		iResult = GET_PLAYER(pkCity->getOwner()).getProductionNeeded(eUnitType);
		iTotalDiscount = (/*-50*/ GC.getBALANCE_BUILDING_INVESTMENT_BASELINE() + GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetInvestmentModifier() + GET_PLAYER(pkCity->getOwner()).GetInvestmentModifier());
		iResult *= (iTotalDiscount + 100);
		iResult /= 100;
	}

	lua_pushinteger(L, iResult);
	return 1;
}
#endif
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
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_PROCESS_STOCKPILE)
//------------------------------------------------------------------------------
//int getProcessProductionTurnsLeft(ProcessTypes eProcess, int iNum);
int CvLuaCity::lGetProcessProductionTurnsLeft(lua_State* L)
{
	return BasicLuaMethod<int, ProcessTypes>(L, &CvCity::getProductionTurnsLeft);
}
#endif
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
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
// void Purchase(UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield);
int CvLuaCity::lPurchase(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const UnitTypes eUnitType = (UnitTypes) lua_tointeger(L, 2);
	const BuildingTypes eBuildingType = (BuildingTypes) lua_tointeger(L, 3);
	const ProjectTypes eProjectType = (ProjectTypes) lua_tointeger(L, 4);
	const YieldTypes ePurchaseYield = (YieldTypes) lua_tointeger(L, 5);

	pkCity->Purchase(eUnitType, eBuildingType, eProjectType, ePurchaseYield);

	return 0;
}
#endif
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
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 2);

	// City Yield Rate Modifier
	pkCity->getBaseYieldRateModifier(eYield, 0, &toolTip);

	// City Production Modifier
	if(eYield == YIELD_PRODUCTION)
	{
		pkCity->getProductionModifier(&toolTip);
	}

	// Trade Yield Modifier
	// This is actually added after all modifiers, except for Food (added after Consumption) and Culture (added to Base)
	//pkCity->GetTradeYieldModifier(eYield, &toolTip);

	// City Food Modifier
	if(eYield == YIELD_FOOD)
	{	
		GC.getGame().BuildProdModHelpText(&toolTip, "TXT_KEY_FOODMOD_EATEN_FOOD", pkCity->foodConsumption());
		pkCity->GetTradeYieldModifier(YIELD_FOOD, &toolTip);
		pkCity->foodDifferenceTimes100(true, &toolTip);
	}

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
//bool isOriginalMajorCapital();
int CvLuaCity::lIsOriginalMajorCapital(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsOriginalMajorCapital);
}
//------------------------------------------------------------------------------
//bool isCoastal(int iMinWaterSize);
int CvLuaCity::lIsCoastal(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isCoastal);
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//bool isAddsFreshWater();
int CvLuaCity::lIsAddsFreshWater(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::isAddsFreshWater);
}
//int foodConsumptionSpecialistTimes100();
int CvLuaCity::lFoodConsumptionSpecialistTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::foodConsumptionSpecialistTimes100);
}
#endif
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
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//int getNumBuildingClass(BuildingClassTypes eBuildingClassType);
int CvLuaCity::lGetNumBuildingClass(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	if(eBuildingClassType != NO_BUILDINGCLASS)
	{
		const CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(pkCity->getOwner()).getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClassType);
		const int iResult = pkCity->GetCityBuildings()->GetNumBuilding(eBuilding);
		lua_pushinteger(L, iResult);
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
//------------------------------------------------------------------------------
//bool isHasBuildingClass(BuildingClassTypes eBuildingClassType);
int CvLuaCity::lIsHasBuildingClass(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	if(eBuildingClassType != NO_BUILDINGCLASS)
	{
		const CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(pkCity->getOwner()).getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClassType);
		const bool bResult = pkCity->GetCityBuildings()->GetNumBuilding(eBuilding);
		lua_pushboolean(L, bResult);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetLocalBuildingClassYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 3);
	int iResult = 0;
	if(eBuildingClassType != NO_BUILDINGCLASS && eIndex != NO_YIELD)
	{	
		iResult = pkCity->getLocalBuildingClassYield(eBuildingClassType, eIndex);
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetEventBuildingClassYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 3);
	int iResult = 0;
	if(eBuildingClassType != NO_BUILDINGCLASS && eIndex != NO_YIELD)
	{	
		iResult = pkCity->GetEventBuildingClassCityYield(eBuildingClassType, eIndex);
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetEventBuildingClassModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 3);
	int iResult = 0;
	if(eBuildingClassType != NO_BUILDINGCLASS && eIndex != NO_YIELD)
	{	
		iResult = pkCity->GetEventBuildingClassCityYieldModifier(eBuildingClassType, eIndex);
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetEventCityYield(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetEventCityYield);
}
#endif
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
	CvArea* pkArea = GC.getMap().getArea(pkCity->getArea());
	CvLuaArea::Push(L, pkArea);
	return 1;
}
//------------------------------------------------------------------------------
//CyArea* waterArea();
int CvLuaCity::lWaterArea(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	std::vector<int> areas = pkCity->plot()->getAllAdjacentAreas();
	for (std::vector<int>::iterator it=areas.begin(); it!=areas.end(); ++it)
	{
		CvArea* pkArea = GC.getMap().getArea(*it);
		if (pkArea->isWater())
		{
			CvLuaArea::Push(L, pkArea);
			return 1;
		}
	}

	CvLuaArea::Push(L, NULL);
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
#if defined(MOD_BALANCE_CORE)
	const bool bPurchase = lua_toboolean(L, 2);
	CvPlot* pPlot = pkCity->GetNextBuyablePlot(bPurchase);
#else
	CvPlot* pPlot = pkCity->GetNextBuyablePlot();
#endif
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
#if defined(MOD_BALANCE_CORE)
	const bool bPurchase = lua_toboolean(L, 2);
	CvPlot* pkPlot = pkCity->GetNextBuyablePlot(bPurchase);
#else
	CvPlot* pkPlot = pkCity->GetNextBuyablePlot();
#endif
	CvLuaPlot::Push(L, pkPlot);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaCity::lGetBuyablePlotList(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	std::vector<int> aiPlotList;
	aiPlotList.resize(20, -1);
#if defined(MOD_BALANCE_CORE)
	const bool bPurchase = lua_toboolean(L, 2);
	pkCity->GetBuyablePlotList(aiPlotList, bPurchase);
#else
	pkCity->GetBuyablePlotList(aiPlotList);
#endif

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

int CvLuaCity::lGetSpecialistRate(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 2);
	int Rate = 0;
	if (eSpecialist != NO_SPECIALIST)
	{
		Rate = pkCity->GetCityCitizens()->GetSpecialistRate(eSpecialist);
	}
	lua_pushinteger(L, Rate);
	return 1;
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
#if defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	CvCity* pkCity = GetInstance(L);
	int iChange = lua_tointeger(L, 2);
	pkCity->ChangeBaseYieldRateFromBuildings(YIELD_CULTURE, iChange);

	return 0;
#else
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromBuildings);
#endif
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
//int GetJONSCulturePerTurnFromGreatWorks() const;
int CvLuaCity::lGetJONSCulturePerTurnFromGreatWorks(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromGreatWorks);
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromTraits() const;
int CvLuaCity::lGetJONSCulturePerTurnFromTraits(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromTraits);
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int GetYieldPerTurnFromTraits() const;
int CvLuaCity::lGetYieldPerTurnFromTraits(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetYieldPerTurnFromTraits);
}

//int GetYieldPerTurnFromTraits() const;
int CvLuaCity::lGetYieldFromUnitsInCity(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	YieldTypes eYieldType = (YieldTypes)lua_tointeger(L, 2);

	int Total = 0;
	CvPlot* pCityPlot = pkCity->plot();
	for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
	{
		int iTempVal = pCityPlot->getUnitByIndex(iUnitLoop)->GetYieldChange(eYieldType);
		if (iTempVal != 0)
		{
			Total += iTempVal;
		}
	}

	lua_pushinteger(L, Total);
	return 1;
}
#endif
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
#if defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	CvCity* pkCity = GetInstance(L);
	int iChange = lua_tointeger(L, 2);
	pkCity->ChangeBaseYieldRateFromReligion(YIELD_CULTURE, iChange);

	return 0;
#else
	return BasicLuaMethod(L, &CvCity::ChangeJONSCulturePerTurnFromReligion);
#endif
}
//------------------------------------------------------------------------------
//int GetJONSCulturePerTurnFromLeagues() const;
int CvLuaCity::lGetJONSCulturePerTurnFromLeagues(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetJONSCulturePerTurnFromLeagues);
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
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
//int GetCityYieldModFromMonopoly() const;
int CvLuaCity::lGetCityYieldModFromMonopoly(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	YieldTypes eYieldType = (YieldTypes)lua_tointeger(L, 2);
	int iModifier = 0;
	// Do we get increased yields from a resource monopoly?
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
		CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
		if (pInfo && pInfo->isMonopoly())
		{
			if(GET_PLAYER(pkCity->getOwner()).HasGlobalMonopoly(eResourceLoop) && pInfo->getCityYieldModFromMonopoly(eYieldType) > 0)
			{
				int iTemp = pInfo->getCityYieldModFromMonopoly(eYieldType);
				iTemp *= max(1, GET_PLAYER(pkCity->getOwner()).GetMonopolyModPercent());
				iModifier += iTemp;
			}
		}
	}
	lua_pushinteger(L, iModifier);
	return 1;
}
#endif
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//int getTourismRateModifier() const;
int CvLuaCity::lGetTourismRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getTourismRateModifier);
}
//------------------------------------------------------------------------------
//void changeTourismRateModifier(int iChange);
int CvLuaCity::lChangeTourismRateModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeTourismRateModifier);
}
#endif
//------------------------------------------------------------------------------
//int GetNumGreatWorks();
int CvLuaCity::lGetNumGreatWorks(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	const bool bIgnoreYield = luaL_optbool(L, 2, true);
	lua_pushinteger(L, pkCity->GetCityCulture()->GetNumGreatWorks(bIgnoreYield));
#else
	lua_pushinteger(L, pkCity->GetCityCulture()->GetNumGreatWorks());
#endif
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumGreatWorkSlots();
int CvLuaCity::lGetNumGreatWorkSlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetCityCulture()->GetNumGreatWorkSlots());
	return 1;
}
//------------------------------------------------------------------------------
//int GetBaseTourism();
int CvLuaCity::lGetBaseTourism(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
#if defined(MOD_BALANCE_CORE)
	lua_pushinteger(L, pkCity->GetBaseTourism());
#else
	lua_pushinteger(L, pkCity->GetCityCulture()->GetBaseTourism());
#endif
	return 1;
}
#if defined(MOD_BALANCE_CORE)
int CvLuaCity::lRefreshTourism(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
	pkCity->GetCityCulture()->CalculateBaseTourism();
	return 0;
}
//------------------------------------------------------------------------------
//int GetNumGreatWorksFilled();
int CvLuaCity::lGetNumGreatWorksFilled(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	GreatWorkSlotType eGreatWorkSlot = static_cast<GreatWorkSlotType>(lua_tointeger(L, 2));

	lua_pushinteger(L, pkCity->GetCityCulture()->GetNumFilledGreatWorkSlots(eGreatWorkSlot));
	return 1;
}

int CvLuaCity::lGetNumAvailableGreatWorkSlots(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	GreatWorkSlotType eGreatWorkSlot = static_cast<GreatWorkSlotType>(lua_tointeger(L, 2));

	lua_pushinteger(L, pkCity->GetCityCulture()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot));
	return 1;
}


#endif
//------------------------------------------------------------------------------
//int GetTourismMultiplier(PlayerTypes ePlayer);
int CvLuaCity::lGetTourismMultiplier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkCity->GetCityCulture()->GetTourismMultiplier(ePlayer, false, false, false, false, false));
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetTourismTooltip();
int CvLuaCity::lGetTourismTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	toolTip = pkCity->GetCityCulture()->GetTourismTooltip();
	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetFilledSlotsTooltip();
int CvLuaCity::lGetFilledSlotsTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	toolTip = pkCity->GetCityCulture()->GetFilledSlotsTooltip();
	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetTotalSlotsTooltip();
int CvLuaCity::lGetTotalSlotsTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	toolTip = pkCity->GetCityCulture()->GetTotalSlotsTooltip();
	lua_pushstring(L, toolTip.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//void ClearGreatWorks();
int CvLuaCity::lClearGreatWorks(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	pkCity->GetCityCulture()->ClearGreatWorks();
	return 1;
}
//------------------------------------------------------------------------------
// int GetFaithBuildingTourism()
int CvLuaCity::lGetFaithBuildingTourism(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pkCity->getOwner());
	if(pReligion)
	{
		iRtnValue = pReligion->m_Beliefs.GetFaithBuildingTourism(pkCity->getOwner(), pkCity);
	}
	lua_pushinteger(L, iRtnValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsThemingBonusPossible(BuildingClassTypes eBuildingClass);
int CvLuaCity::lIsThemingBonusPossible(lua_State* L)
{
	bool bPossible;
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	bPossible = pkCity->GetCityCulture()->IsThemingBonusPossible(iIndex);
	lua_pushboolean(L, bPossible);
	return 1;
}
//------------------------------------------------------------------------------
//int GetThemingBonus(BuildingClassTypes eBuildingClass);
int CvLuaCity::lGetThemingBonus(lua_State* L)
{
	int iBonus;
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	iBonus = pkCity->GetCityCulture()->GetThemingBonus(iIndex);
	lua_pushinteger(L, iBonus);
	return 1;
}
//------------------------------------------------------------------------------
//CvString GetThemingTooltip(BuildingClassTypes eBuildingClass);
int CvLuaCity::lGetThemingTooltip(lua_State* L)
{
	CvString toolTip;
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	toolTip = pkCity->GetCityCulture()->GetThemingTooltip(iIndex);
	lua_pushstring(L, toolTip.c_str());
	return 1;
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
#if defined(MOD_API_UNIFIED_YIELDS)
	int iBonus = 0;
	CvCity* pkCity = GetInstance(L);
	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		FeatureTypes eFeature = (FeatureTypes) iI;
		if(eFeature != NO_FEATURE)
		{
			iBonus += pkCity->GetYieldPerTurnFromUnimprovedFeatures(eFeature, YIELD_FAITH);
		}
	}
	lua_pushinteger(L, iBonus);
	return 1;
#else
	return BasicLuaMethod(L, &CvCity::GetFaithPerTurnFromTraits);
#endif
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
#if defined(MOD_API_UNIFIED_YIELDS_CONSOLIDATION)
	CvCity* pkCity = GetInstance(L);
	int iChange = lua_tointeger(L, 2);
	pkCity->ChangeBaseYieldRateFromReligion(YIELD_FAITH, iChange);

	return 0;
#else
	return BasicLuaMethod(L, &CvCity::ChangeFaithPerTurnFromReligion);
#endif
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
//int GetSecondaryReligion() const;
int CvLuaCity::lGetSecondaryReligion(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = (int)pkCity->GetCityReligions()->GetSecondaryReligion();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetSecondaryReligionPantheonBelief() const;
int CvLuaCity::lGetSecondaryReligionPantheonBelief(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = (int)pkCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetPressurePerTurn() const;
int CvLuaCity::lGetPressurePerTurn(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	int iNumTradeRoutes;
	const int iResult = (int)pkCity->GetCityReligions()->GetPressurePerTurn(eReligion, iNumTradeRoutes);
	lua_pushinteger(L, iResult);
	lua_pushinteger(L, iNumTradeRoutes);
	return 2;
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
			iHappinessFromBuilding += pReligion->m_Beliefs.GetBuildingClassHappiness(eBuildingClass, iFollowers, pkCity->getOwner(), pkCity);
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
			iYieldFromBuilding += pReligion->m_Beliefs.GetBuildingClassYieldChange(eBuildingClass, eYieldType, iFollowers, pkCity->getOwner(), pkCity);
			if (::isWorldWonderClass(*GC.getBuildingClassInfo(eBuildingClass)))
			{
				iYieldFromBuilding += pReligion->m_Beliefs.GetYieldChangeWorldWonder(eYieldType, pkCity->getOwner(), pkCity);
			}
			BeliefTypes eSecondaryPantheon = pkCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
			if (eSecondaryPantheon != NO_BELIEF)
			{
				iFollowers =  pkCity->GetCityReligions()->GetNumFollowers(pkCity->GetCityReligions()->GetSecondaryReligion());
				if (iFollowers >= GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetMinFollowers())
				{
					iYieldFromBuilding += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetBuildingClassYieldChange(eBuildingClass, eYieldType);
				}
			}
		}
	}
	lua_pushinteger(L, iYieldFromBuilding);
	return 1;
}
//------------------------------------------------------------------------------
//int GetLeagueBuildingClassYieldChange(eBuildingClass, eYieldType) const;
int CvLuaCity::lGetLeagueBuildingClassYieldChange(lua_State* L)
{
	int iYieldFromBuilding = 0;

	CvCity* pkCity = GetInstance(L);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)lua_tointeger(L, 2);
	YieldTypes eYieldType = (YieldTypes)lua_tointeger(L, 3);

	CvBuildingClassInfo* pInfo = GC.getBuildingClassInfo(eBuildingClass);
	if (pInfo && pInfo->getMaxGlobalInstances() != -1)
	{
		int iYieldChange = GC.getGame().GetGameLeagues()->GetWorldWonderYieldChange(pkCity->getOwner(), eYieldType);
		if (iYieldChange != 0)
		{
			iYieldFromBuilding += iYieldChange;
		}
	}

	lua_pushinteger(L, iYieldFromBuilding);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetNumTradeRoutesAddingPressure(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	ReligionTypes eReligion = (ReligionTypes)lua_tointeger(L, 2);
	
	int iNumTradeRoutes = pkCity->GetCityReligions()->GetNumTradeRouteConnections(eReligion);
	lua_pushinteger(L, iNumTradeRoutes);
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
#if defined(MOD_BALANCE_CORE)
int CvLuaCity::lGetNumTotalBuildings(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iResult = 0;
	const bool bSkipDummy = luaL_optbool(L, 2, true);
	const bool bSkipWW = luaL_optbool(L, 3, true);
	const bool bSkipNW = luaL_optbool(L, 4, true);
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			if(bSkipDummy && pkBuildingInfo->IsDummy())
				continue;

			if(bSkipWW && ::isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
				continue;

			if(bSkipNW && ::isNationalWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
				continue;

			iResult += pkCity->GetCityBuildings()->GetNumBuilding(eBuilding);
		}
	}

	lua_pushinteger(L, iResult);
	return 1;
}
#endif

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

#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//int GetBuyPlotDistance();
int CvLuaCity::lGetBuyPlotDistance(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getBuyPlotDistance);
}

//------------------------------------------------------------------------------
//void GetWorkPlotDistance();
int CvLuaCity::lGetWorkPlotDistance(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::getWorkPlotDistance);
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BUILDINGS_CITY_WORKING)
//------------------------------------------------------------------------------
//int getCityWorkingChange();
int CvLuaCity::lGetCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetCityWorkingChange);
}

//------------------------------------------------------------------------------
//void changeCityWorkingChange(int iChange);
int CvLuaCity::lChangeCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeCityWorkingChange);
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_HAPPINESS)
int CvLuaCity::lGetTheoreticalUnhappinessDecrease(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);
	int iThreshold = 0;
	if(eBuilding != NO_BUILDING)
	{
		if(pkCity->canConstruct(eBuilding, false, false, false, false))
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				int iResult = (pkBuildingInfo->GetPovertyHappinessChangeBuilding() + pkBuildingInfo->GetPovertyHappinessChangeBuildingGlobal());
				if(iResult != 0)
				{				
					iThreshold = pkCity->getUnhappinessFromGoldNeeded(iResult);
					lua_pushinteger(L, iThreshold);
					return 1;
				}
				iResult = (pkBuildingInfo->GetIlliteracyHappinessChangeBuilding() + pkBuildingInfo->GetIlliteracyHappinessChangeBuildingGlobal());
				if(iResult != 0)
				{
					iThreshold = pkCity->getUnhappinessFromScienceNeeded(iResult);
					lua_pushinteger(L, iThreshold);
					return 1;
				}
				iResult = (pkBuildingInfo->GetDefenseHappinessChangeBuilding() + pkBuildingInfo->GetDefenseHappinessChangeBuildingGlobal());
				if(iResult != 0)
				{
					iThreshold = pkCity->getUnhappinessFromDefenseNeeded(iResult);
					lua_pushinteger(L, iThreshold);
					return 1;
				}
				iResult = (pkBuildingInfo->GetUnculturedHappinessChangeBuilding() + pkBuildingInfo->GetUnculturedHappinessChangeBuildingGlobal());
				if(iResult != 0)
				{
					iThreshold = pkCity->getUnhappinessFromCultureNeeded(iResult);
					lua_pushinteger(L, iThreshold);
					return 1;
				}
			}
		}
	}
	lua_pushinteger(L, iThreshold);
	return 1;
}
//int getHappinessDelta();
int CvLuaCity::lgetHappinessDelta(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getHappinessDelta());
	return 1;
}
//int getThresholdSubtractions();
int CvLuaCity::lgetThresholdSubtractions(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 2);
	const int iResult = pkCity->getThresholdSubtractions(eYield);

	lua_pushinteger(L, iResult);
	return 1;
}
//int getThresholdAdditions();
int CvLuaCity::lgetThresholdAdditions(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	lua_pushinteger(L, pkCity->getThresholdAdditions(eYield));
	return 1;
}
//int getUnhappinessFromCultureYield();
int CvLuaCity::lGetUnhappinessFromCultureYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromCultureYield());
	return 1;
}
//int getUnhappinessFromCultureNeeded();
int CvLuaCity::lGetUnhappinessFromCultureNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromCultureNeeded());
	return 1;
}
//int GetUnhappinessFromCultureDeficit();
int CvLuaCity::lGetUnhappinessFromCultureDeficit(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iPop = pkCity->getPopulation();
	int iResult = 0;
	int iYield = pkCity->getUnhappinessFromCultureYield() * iPop;
	int iNeed = pkCity->getUnhappinessFromCultureNeeded() * iPop;
	if(iNeed > iYield)
	{
		iResult = iNeed - iYield;
	}
	else
	{
		iResult = iYield - iNeed;
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//int getUnhappinessFromCulture();
int CvLuaCity::lGetUnhappinessFromCulture(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromCulture());
	return 1;
}
//int getUnhappinessFromScienceYield();
int CvLuaCity::lGetUnhappinessFromScienceYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromScienceYield());
	return 1;
}
//int getUnhappinessFromScienceNeeded();
int CvLuaCity::lGetUnhappinessFromScienceNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromScienceNeeded());
	return 1;
}
//int GetUnhappinessFromScienceDeficit();
int CvLuaCity::lGetUnhappinessFromScienceDeficit(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iPop = pkCity->getPopulation();
	int iResult = 0;
	int iYield = pkCity->getUnhappinessFromScienceYield() * iPop;
	int iNeed = pkCity->getUnhappinessFromScienceNeeded() * iPop;
	if(iNeed > iYield)
	{
		iResult = iNeed - iYield;
	}
	else
	{
		iResult = iYield - iNeed;
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//int getUnhappinessFromScience();
int CvLuaCity::lGetUnhappinessFromScience(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromScience());
	return 1;
}
//int getUnhappinessFromCultureYield();
int CvLuaCity::lGetUnhappinessFromDefenseYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromDefenseYield());
	return 1;
}
//int getUnhappinessFromDefenseNeeded();
int CvLuaCity::lGetUnhappinessFromDefenseNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromDefenseNeeded());
	return 1;
}
//int GetUnhappinessFromDefenseDeficit();
int CvLuaCity::lGetUnhappinessFromDefenseDeficit(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iPop = pkCity->getPopulation();
	int iResult = 0;
	int iYield = pkCity->getUnhappinessFromDefenseYield() * iPop;
	int iNeed = pkCity->getUnhappinessFromDefenseNeeded() * iPop;
	if(iNeed > iYield)
	{
		iResult = iNeed - iYield;
	}
	else
	{
		iResult = iYield - iNeed;
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//int getUnhappinessFromDefense();
int CvLuaCity::lGetUnhappinessFromDefense(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromDefense());
	return 1;
}
//int getUnhappinessFromGoldYield();
int CvLuaCity::lGetUnhappinessFromGoldYield(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromGoldYield());
	return 1;
}
//int getUnhappinessFromGoldNeeded();
int CvLuaCity::lGetUnhappinessFromGoldNeeded(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromGoldNeeded());
	return 1;
}
//int GetUnhappinessFromGoldDeficit();
int CvLuaCity::lGetUnhappinessFromGoldDeficit(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iPop = pkCity->getPopulation();
	int iResult = 0;
	int iYield = pkCity->getUnhappinessFromGoldYield() * iPop;
	int iNeed = pkCity->getUnhappinessFromGoldNeeded() * iPop;
	if(iNeed > iYield)
	{
		iResult = iNeed - iYield;
	}
	else
	{
		iResult = iYield - iNeed;
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//int getUnhappinessFromGold();
int CvLuaCity::lGetUnhappinessFromGold(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromGold());
	return 1;
}
//int getUnhappinessFromConnection();
int CvLuaCity::lGetUnhappinessFromConnection(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromConnection());
	return 1;
}
//int getUnhappinessFromPillaged();
int CvLuaCity::lGetUnhappinessFromPillaged(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromPillaged());
	return 1;
}
//int getUnhappinessFromStarving();
int CvLuaCity::lGetUnhappinessFromStarving(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromStarving());
	return 1;
}
//int getUnhappinessFromMinority();
int CvLuaCity::lGetUnhappinessFromMinority(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->getUnhappinessFromReligion());
	return 1;
}
#endif

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
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//void changeOverflowProduction(int iChange);
int CvLuaCity::lChangeOverflowProduction(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::changeOverflowProduction);
}
#endif
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
int CvLuaCity::lIsBlockaded(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::IsBlockadedWaterAndLand);
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
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
//------------------------------------------------------------------------------
int CvLuaCity::lGetBaseYieldRateFromGreatWorks(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromGreatWorks);
}
#endif
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
#if defined(MOD_API_LUA_EXTENSIONS)
// Base yield rate from active conversion process
int CvLuaCity::lGetBaseYieldRateFromProcess(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromProcess);
}
//	Base yield rate from trade routes established with this city
int CvLuaCity::lGetBaseYieldRateFromTradeRoutes(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	int iReturnValue = GET_PLAYER(pkCity->getOwner()).GetTrade()->GetTradeValuesAtCityTimes100(pkCity, eIndex);
	lua_pushinteger(L, iReturnValue);
	return 1;
}
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CITYSTATES)
// Base yield rate from League
int CvLuaCity::lGetBaseYieldRateFromLeague(lua_State* L)
{
	return BasicLuaMethod(L, &CvCity::GetBaseYieldRateFromLeague);
}
#endif
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
int CvLuaCity::lGetScienceFromCityYield(lua_State* L)
{
	int iResult = 0;
	CvCity* pkCity = GetInstance(L);
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eIndex = (YieldTypes)iI;
		if(eIndex == NO_YIELD)
		{
			continue;
		}
		iResult += pkCity->GetScienceFromYield(eIndex);
	}
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
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
	const bool bIgnoreTrade = luaL_optbool(L, 3, false);
	const int iResult = pkCity->getYieldRate(eIndex, bIgnoreTrade);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getYieldRateTimes100(YieldTypes eIndex);
int CvLuaCity::lGetYieldRateTimes100(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const bool bIgnoreTrade = luaL_optbool(L, 3, false);
	const int iResult = pkCity->getYieldRateTimes100(eIndex, bIgnoreTrade);

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
	const UnitClassTypes eUnitClassType = toValue<UnitClassTypes>(L, 2);
	const int iResult = pkCity->GetCityCitizens()->GetSpecialistUpgradeThreshold(eUnitClassType);

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
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int GetTotalSpecialistCount();
int CvLuaCity::lGetTotalSpecialistCount(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = pkCity->GetCityCitizens()->GetTotalSpecialistCount();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int lGetSpecialistCityModifier(SpecialistTypes eIndex);
int CvLuaCity::lGetSpecialistCityModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iIndex = lua_tointeger(L, 2);
	int iResult = pkCity->GetSpecialistRateModifier(toValue<SpecialistTypes>(L, 2));

	int iNumPuppets = GET_PLAYER(pkCity->getOwner()).GetNumPuppetCities();
	if(iNumPuppets > 0)
	{
		GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist((SpecialistTypes)toValue<SpecialistTypes>(L, 2));
		if(eGreatPerson != NO_GREATPERSON)
		{
			iResult += (iNumPuppets * GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));			
		}
	}

	lua_pushinteger(L, iResult);
	return 1;
}
#endif
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
int CvLuaCity::lGetExtraSpecialistPoints(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 2);

	if (eSpecialist != NO_SPECIALIST)
	{
		ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
		if (eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pkCity->getOwner());
			if (pReligion)
			{
				lua_pushinteger(L, pReligion->m_Beliefs.GetGreatPersonPoints(GetGreatPersonFromSpecialist(eSpecialist), pkCity->getOwner(), pkCity, true));
				return 1;
			}
		}
	}
	lua_pushinteger(L, 0);
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
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_EVENTS_CITY_BOMBARD)
//int, bool GetBombardRange();
int CvLuaCity::lGetBombardRange(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	bool bIndirectFireAllowed;
	const int iResult = pkCity->getBombardRange(bIndirectFireAllowed);

	lua_pushinteger(L, iResult);
	lua_pushinteger(L, bIndirectFireAllowed);
	return 2;
}
#endif
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
int CvLuaCity::lGetMultiAttackBonusCity(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, 2);

	if (pkUnit == NULL || pkCity == NULL)
		return 0;

	int iModifier = 0;
	//bonus for attacking same unit over and over in a turn?
	if (GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus() != 0)
	{
		int iTempModifier = GET_PLAYER(pkCity->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus() * pkUnit->GetNumTimesAttackedThisTurn(pkCity->getOwner());
		iModifier += iTempModifier;
	}

	lua_pushinteger(L, iModifier);
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
#if defined(MOD_BALANCE_CORE)
		const bool bNoBonus = luaL_optbool(L, 4, true);
		pkCity->GetCityBuildings()->SetNumRealBuilding(iIndex, iNewValue, bNoBonus);
#else
		pkCity->GetCityBuildings()->SetNumRealBuilding(iIndex, iNewValue);
#endif
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
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//void SetNumFreeBuilding(BuildingTypes iIndex, int iNewValue);
int CvLuaCity::lSetNumFreeBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingTypes iIndex = toValue<BuildingTypes>(L, 2);
	if(iIndex != NO_BUILDING)
	{
		const int iNewValue = lua_tointeger(L, 3);
		pkCity->GetCityBuildings()->SetNumFreeBuilding(iIndex, iNewValue);
	}

	return 0;
}
#endif
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
//int GetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot) const;
int CvLuaCity::lGetBuildingGreatWork(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	const int iSlot = lua_tointeger(L, 3);
	const int iResult = pkCity->GetCityBuildings()->GetBuildingGreatWork(iIndex, iSlot);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int SetBuildingGreatWork(BuildingClassTypes eBuildingClass, int iSlot, int iGreatWorkIndex);
int CvLuaCity::lSetBuildingGreatWork(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	const int iSlot = lua_tointeger(L, 3);
	const int iGreatWorkIndex = lua_tointeger(L, 4);
	if(iIndex != NO_BUILDING)
	{
		pkCity->GetCityBuildings()->SetBuildingGreatWork(iIndex, iSlot, iGreatWorkIndex);
	}
	return 1;
}
//------------------------------------------------------------------------------
//int IsHoldingGreatWork(BuildingClassTypes eBuildingClass) const;
int CvLuaCity::lIsHoldingGreatWork(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	if(iIndex != NO_BUILDINGCLASS)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(iIndex);
		if(pkBuildingClassInfo)
		{
			bResult = pkCity->GetCityBuildings()->IsHoldingGreatWork(iIndex);
		}
	}
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumGreatWorksInBuilding(BuildingClassTypes eBuildingClass) const;
int CvLuaCity::lGetNumGreatWorksInBuilding(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes iIndex = toValue<BuildingClassTypes>(L, 2);
	const int iResult = pkCity->GetCityBuildings()->GetNumGreatWorksInBuilding(iIndex);
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
#if defined(MOD_BUGFIX_LUA_API)
			lua_pushinteger(L, pkOrder->iData2);
#else
			lua_pushinteger(L, pkOrder->iData1);
#endif
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

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE_POLICIES)
int CvLuaCity::lGetBuildingClassCultureChange(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClassType = (BuildingClassTypes)lua_tointeger(L, 2);
	const int iResult = pkCity->getBuildingClassCultureChange((BuildingClassTypes)eBuildingClassType);

	lua_pushinteger(L, iResult);
	return 1;
}
//int getReligionYieldRateModifier(YieldTypes eYield);
int CvLuaCity::lGetReligionYieldRateModifier(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 2);
	const PlayerTypes ePlayer = pkCity->getOwner();
	ReligionTypes eMajority = pkCity->GetCityReligions()->GetReligiousMajority();
	ReligionTypes ePlayerReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
	if(ePlayerReligion != NO_RELIGION && eMajority == ePlayerReligion)
	{
		const int iRtnValue = GET_PLAYER(ePlayer).getReligionYieldRateModifier(eYield);
		lua_pushinteger(L, iRtnValue);
	}
	else
	{
		lua_pushinteger(L, 0);
	}

	return 1;
}
//int getReligionBuildingYieldRateModifier(BuildingClassTypes eBuildingClass, YieldTypes eYield);
int CvLuaCity::lGetReligionBuildingYieldRateModifier(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	const int eBuildingClass = lua_tointeger(L, 2);
	const int eYield = lua_tointeger(L, 3);
	const PlayerTypes ePlayer = pkCity->getOwner();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = pkCity->GetCityReligions()->GetReligiousMajority();
	ReligionTypes ePlayerReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
	if(ePlayerReligion != NO_RELIGION && eReligion == ePlayerReligion)
	{
		 iRtnValue = pkCity->getReligionBuildingYieldRateModifier((BuildingClassTypes)eBuildingClass, (YieldTypes)eYield);
	}

	lua_pushinteger(L, iRtnValue);

	return 1;
}
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_BALANCE_CORE)
int CvLuaCity::lGetBaseYieldRateFromCSAlliance(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	int iResult = pkCity->GetBaseYieldRateFromCSAlliance(eIndex);
	iResult += pkCity->GetBaseYieldRateFromCSFriendship(eIndex);

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetBuildingYieldChangeFromCorporationFranchises(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)lua_tointeger(L, 2);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 3);
	int iResult = pkCity->GetBuildingYieldChangeFromCorporationFranchises(eBuildingClass, eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetYieldChangeFromCorporationFranchises(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	int iResult = pkCity->GetYieldChangeFromCorporationFranchises(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetTradeRouteCityMod(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = (pkCity->GetTradeRouteCityMod(eIndex));

	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaCity::lGetGreatWorkYieldMod(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = min(20, (GET_PLAYER(pkCity->getOwner()).getYieldModifierFromGreatWorks(eIndex) * pkCity->GetCityBuildings()->GetNumGreatWorks()));

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetActiveSpyYieldMod(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	const int iResult = min(30, (GET_PLAYER(pkCity->getOwner()).getYieldModifierFromActiveSpies(eIndex) * GET_PLAYER(pkCity->getOwner()).GetEspionage()->GetNumAssignedSpies()));

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetResourceQuantityPerXFranchises(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResource = lua_tointeger(L, 2);
	int iFranchises = GET_PLAYER(pkCity->getOwner()).GetCorporations()->GetNumFranchises();
	int iCorpResource = pkCity->GetResourceQuantityPerXFranchises((ResourceTypes)iResource);
	int iResult = 0;
	if(iCorpResource > 0)
	{
		iResult = (iFranchises / iCorpResource);
	}

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lGetGPRateModifierPerXFranchises(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iResult = (pkCity->GetGPRateModifierPerXFranchises());

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaCity::lIsFranchised(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 2);
	bResult = pkCity->IsHasFranchise(GET_PLAYER(ePlayer).GetCorporations()->GetFoundedCorporation());
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaCity::lDoFranchiseAtCity(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	CvCity* pkDestCity = GetInstance(L, 2);
	GET_PLAYER(pkCity->getOwner()).GetCorporations()->BuildFranchiseInCity(pkCity, pkDestCity);
	return 1;
}
int CvLuaCity::lHasOffice(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	bResult = pkCity->IsHasOffice();
	lua_pushboolean(L, bResult);
	return 1;
}
int CvLuaCity::lGetYieldChangeTradeRoute(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	int iResult = 0;
	const YieldTypes eIndex = (YieldTypes)lua_tointeger(L, 2);
	if(pkCity->IsRouteToCapitalConnected())
	{
		iResult = GET_PLAYER(pkCity->getOwner()).GetYieldChangeTradeRoute(eIndex);
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//int BeliefClasses::GetSpecialistYieldChange(SpecialistTypes eSpecialist, YieldTypes eYield);
int CvLuaCity::lGetSpecialistYieldChange(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	const SpecialistTypes eSpecialist = (SpecialistTypes) lua_tointeger(L, 2);
	const YieldTypes eYield = (YieldTypes) lua_tointeger(L, 3);
	const PlayerTypes ePlayer = pkCity->getOwner();
	ReligionTypes eReligion = pkCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pkCity->getOwner());
	if(eReligion != NO_RELIGION)
	{
		iRtnValue = pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield, pkCity->getOwner(), pkCity);
	}
	BeliefTypes eSecondaryPantheon = pkCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
	if (eSecondaryPantheon != NO_BELIEF)
	{
		iRtnValue += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetSpecialistYieldChange(eSpecialist, eYield);
	}
#if defined(MOD_BALANCE_CORE_EVENTS)
	iRtnValue += pkCity->GetEventSpecialistYield(eSpecialist, eYield);
#endif

	lua_pushinteger(L, iRtnValue);

	return 1;
}
//int GetModFromWLTKD(YieldTypes eYield);
int CvLuaCity::lGetModFromWLTKD(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	const int eYield = lua_tointeger(L, 2);
	if(pkCity->GetWeLoveTheKingDayCounter() > 0)
	{
		const PlayerTypes ePlayer = pkCity->getOwner();
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eReligion = pkCity->GetCityReligions()->GetReligiousMajority();
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = pReligions->GetReligion(eReligion, ePlayer);
			iRtnValue = pReligion->m_Beliefs.GetYieldFromWLTKD((YieldTypes)eYield, ePlayer, pkCity);
		}
		iRtnValue += GET_PLAYER(ePlayer).GetYieldFromWLTKD((YieldTypes)eYield);
		iRtnValue += pkCity->GetYieldFromWLTKD((YieldTypes)eYield);
	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
int CvLuaCity::lGetCultureModFromCarnaval(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	if(pkCity->GetWeLoveTheKingDayCounter() > 0)
	{
		const PlayerTypes ePlayer = pkCity->getOwner();
		iRtnValue += GET_PLAYER(ePlayer).GetPlayerTraits()->GetWLTKDCulture();
	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
//int GetModFromGoldenAge(YieldTypes eYield);
int CvLuaCity::lGetModFromGoldenAge(lua_State* L)
{
	int iRtnValue = 0;
	CvCity* pkCity = GetInstance(L);
	const int eYield = lua_tointeger(L, 2);
	const PlayerTypes ePlayer = pkCity->getOwner();
	if(GET_PLAYER(ePlayer).getGoldenAgeTurns() > 0)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eFoundedReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionCreatedByPlayer();
		if(eFoundedReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, ePlayer);
			if(pkCity->GetCityReligions()->IsHolyCityForReligion(pReligion->m_eReligion))
			{
				iRtnValue = pReligion->m_Beliefs.GetYieldBonusGoldenAge((YieldTypes)eYield, ePlayer, pkCity);
			}
		}
		iRtnValue += pkCity->GetGoldenAgeYieldMod((YieldTypes)eYield);
	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
#endif
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

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_ESPIONAGE)
//------------------------------------------------------------------------------
int CvLuaCity::lHasDiplomat(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	const PlayerTypes iPlayer = toValue<PlayerTypes>(L, 2);
	if(iPlayer != NO_PLAYER && pkCity->isCapital())
	{
		int iSpyIndex = pkCity->GetCityEspionage()->m_aiSpyAssignment[iPlayer];
		bResult = (iSpyIndex != -1 && GET_PLAYER(iPlayer).GetEspionage()->IsDiplomat(iSpyIndex));
	}
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lHasSpy(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	const PlayerTypes iPlayer = toValue<PlayerTypes>(L, 2);
	if(iPlayer != NO_PLAYER)
	{
		int iSpyIndex = pkCity->GetCityEspionage()->m_aiSpyAssignment[iPlayer];
		bResult = (iSpyIndex != -1 && !GET_PLAYER(iPlayer).GetEspionage()->IsDiplomat(iSpyIndex));
	}
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lHasCounterSpy(lua_State* L)
{
	bool bResult = false;
	CvCity* pkCity = GetInstance(L);
	bResult = (pkCity->GetCityEspionage()->m_aiSpyAssignment[pkCity->getOwner()] != -1);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lGetCounterSpy(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetCityEspionage()->m_aiSpyAssignment[pkCity->getOwner()]);
	return 1;
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_RELIGION_CONVERSION_MODIFIERS)
//int GetBuildingConversionModifier(BuildingTypes eBuilding)
int CvLuaCity::lGetBuildingConversionModifier(lua_State* L)
{
	//CvCity* pkCity = GetInstance(L);
	const BuildingTypes eBuilding = (BuildingTypes) lua_tointeger(L, 2);
	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvAssertMsg(pBuildingInfo, "pBuildingInfo is null!");
	if (pBuildingInfo)
	{
		lua_pushinteger(L, pBuildingInfo->GetConversionModifier());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}

// int GetBuildingGlobalConversionModifier(BuildingTypes eBuilding)
int CvLuaCity::lGetBuildingGlobalConversionModifier(lua_State* L)
{
	const BuildingTypes eBuilding = (BuildingTypes)lua_tointeger(L, 2);
	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvAssertMsg(pBuildingInfo, "pBuildingInfo is null!");
	if (pBuildingInfo)
	{
		lua_pushinteger(L, pBuildingInfo->GetGlobalConversionModifier());
	}
	else
	{
		lua_pushinteger(L, 0);
	}
	return 1;
}
#endif

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
#if defined(MOD_API_LUA_EXTENSIONS)
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetNumWorkablePlots());
#else
	lua_pushinteger(L, NUM_CITY_PLOTS);
#endif
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

	const int iValue = (GET_PLAYER(ePlayer).specialistYield(eSpecialist, eYield) + pkCity->getSpecialistExtraYield(eSpecialist, eYield));

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
			iReligionRangeStrikeMod = pReligion->m_Beliefs.GetCityRangeStrikeModifier(pkCity->getOwner(), pkCity);
			BeliefTypes eSecondaryPantheon = pkCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
			if (eSecondaryPantheon != NO_BELIEF)
			{
				iReligionRangeStrikeMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetCityRangeStrikeModifier();
			}
		}
	}

	lua_pushinteger(L, iReligionRangeStrikeMod);

	return 1;
}

#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaCity::lAddMessage(lua_State* L)
{
	CvCity* pCity = GetInstance(L);
	const char* szMessage = lua_tostring(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes) luaL_optinteger(L, 3, pCity->getOwner());

	SHOW_CITY_MESSAGE(pCity, ePlayer, szMessage);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCountNumWorkedFeature(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const FeatureTypes eFeature = (FeatureTypes) lua_tointeger(L, 2);
	const int iValue = pkCity->CountNumWorkedFeature(eFeature);

	lua_pushinteger(L, iValue);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCountNumWorkedImprovement(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes) lua_tointeger(L, 2);
	const bool bIgnorePillaged = luaL_optbool(L, 3, true);
	const int iValue = pkCity->CountNumWorkedImprovement(eImprovement, bIgnorePillaged);

	lua_pushinteger(L, iValue);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCountNumWorkedResource(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 2);
	const int iValue = pkCity->CountNumWorkedResource(eResource);

	lua_pushinteger(L, iValue);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCountNumImprovement(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const ImprovementTypes eImprovement = (ImprovementTypes) lua_tointeger(L, 2);
	const int iValue = pkCity->CountNumImprovement(eImprovement);

	lua_pushinteger(L, iValue);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaCity::lCountNumWorkedRiverTiles(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const TerrainTypes eTerrain = (TerrainTypes)luaL_optint(L, 2, NO_TERRAIN);
	const int iValue = pkCity->CountNumWorkedRiverTiles(eTerrain);

	lua_pushinteger(L, iValue);

	return 1;
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
LUAAPIIMPL(City, HasBelief)
LUAAPIIMPL(City, HasBuilding)
LUAAPIIMPL(City, HasBuildingClass)
LUAAPIIMPL(City, HasAnyWonder)
LUAAPIIMPL(City, HasWonder)
LUAAPIIMPL(City, IsCivilization)
LUAAPIIMPL(City, HasFeature)
LUAAPIIMPL(City, HasWorkedFeature)
LUAAPIIMPL(City, HasAnyNaturalWonder)
LUAAPIIMPL(City, HasNaturalWonder)
LUAAPIIMPL(City, HasImprovement)
LUAAPIIMPL(City, HasWorkedImprovement)
LUAAPIIMPL(City, HasPlotType)
LUAAPIIMPL(City, HasWorkedPlotType)
LUAAPIIMPL(City, HasAnyReligion)
LUAAPIIMPL(City, HasReligion)
LUAAPIIMPL(City, HasResource)
LUAAPIIMPL(City, HasWorkedResource)
LUAAPIIMPL(City, IsConnectedToCapital)
LUAAPIIMPL(City, IsConnectedTo)
LUAAPIIMPL(City, HasSpecialistSlot)
LUAAPIIMPL(City, HasSpecialist)
LUAAPIIMPL(City, HasTerrain)
LUAAPIIMPL(City, HasWorkedTerrain)
LUAAPIIMPL(City, HasAnyDomesticTradeRoute)
LUAAPIIMPL(City, HasAnyInternationalTradeRoute)
LUAAPIIMPL(City, HasTradeRouteToAnyCity)
LUAAPIIMPL(City, HasTradeRouteTo)
LUAAPIIMPL(City, HasTradeRouteFromAnyCity)
LUAAPIIMPL(City, HasTradeRouteFrom)
LUAAPIIMPL(City, IsOnFeature)
LUAAPIIMPL(City, IsAdjacentToFeature)
LUAAPIIMPL(City, IsWithinDistanceOfFeature)
#if defined(MOD_BALANCE_CORE)
LUAAPIIMPL(City, IsWithinDistanceOfUnit)
LUAAPIIMPL(City, IsWithinDistanceOfUnitClass)
LUAAPIIMPL(City, IsWithinDistanceOfUnitCombatType)
LUAAPIIMPL(City, IsWithinDistanceOfUnitPromotion)
#endif
LUAAPIIMPL(City, IsOnImprovement)
LUAAPIIMPL(City, IsAdjacentToImprovement)
LUAAPIIMPL(City, IsWithinDistanceOfImprovement)
LUAAPIIMPL(City, IsOnPlotType)
LUAAPIIMPL(City, IsAdjacentToPlotType)
LUAAPIIMPL(City, IsWithinDistanceOfPlotType)
LUAAPIIMPL(City, IsOnResource)
LUAAPIIMPL(City, IsAdjacentToResource)
LUAAPIIMPL(City, IsWithinDistanceOfResource)
LUAAPIIMPL(City, IsOnTerrain)
LUAAPIIMPL(City, IsAdjacentToTerrain)
LUAAPIIMPL(City, IsWithinDistanceOfTerrain)
LUAAPIIMPL(City, CountFeature)
LUAAPIIMPL(City, CountWorkedFeature)
LUAAPIIMPL(City, CountImprovement)
LUAAPIIMPL(City, CountWorkedImprovement)
LUAAPIIMPL(City, CountPlotType)
LUAAPIIMPL(City, CountWorkedPlotType)
LUAAPIIMPL(City, CountResource)
LUAAPIIMPL(City, CountWorkedResource)
LUAAPIIMPL(City, CountTerrain)
LUAAPIIMPL(City, CountWorkedTerrain)

int CvLuaCity::lGetAdditionalFood(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iAdditionalFood = pkCity->GetAdditionalFood();
	lua_pushinteger(L, iAdditionalFood);

	return 1;
}
int CvLuaCity::lSetAdditionalFood(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iFood = lua_tointeger(L, 2);
	pkCity->SetAdditionalFood(iFood);
	return 1;
}
#endif
#if defined(MOD_BALANCE_CORE_EVENTS)
int CvLuaCity::lGetDisabledTooltip(lua_State* L)
{
	CvString DisabledTT = "";
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)lua_tointeger(L, 2);
	if(eEventChoice != NO_EVENT_CHOICE_CITY)
	{
		DisabledTT = pkCity->GetDisabledTooltip(eEventChoice);
	}

	lua_pushstring(L, DisabledTT.c_str());
	return 1;
}
int CvLuaCity::lGetScaledEventChoiceValue(lua_State* L)
{
	CvString CoreYieldTip = "";
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)lua_tointeger(L, 2);
	const bool bYieldsOnly = lua_toboolean(L, 3);
	if(eEventChoice != NO_EVENT_CHOICE_CITY)
	{
		CoreYieldTip = pkCity->GetScaledHelpText(eEventChoice, bYieldsOnly);
	}

	lua_pushstring(L, CoreYieldTip.c_str());
	return 1;
}
int CvLuaCity::lIsCityEventChoiceActive(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)lua_tointeger(L, 2);
	const bool bInstantEvents = luaL_optbool(L, 3, false);
	bool bResult = false;
	if(eEventChoice != NO_EVENT_CHOICE_CITY)
	{
		CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
		if(pkEventChoiceInfo != NULL)
		{
			if(pkCity->IsEventChoiceActive(eEventChoice))
			{
				if(bInstantEvents)
				{
					if(!pkEventChoiceInfo->isOneShot() && !pkEventChoiceInfo->Expires())
					{
						for(int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
						{
							CityEventTypes eEvent = (CityEventTypes)iLoop;
							if(eEvent != NO_EVENT)
							{
								CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
								if(pkEventInfo != NULL)
								{
									if(pkEventChoiceInfo->isParentEvent(eEvent))
									{
										CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
										if(pkEventInfo != NULL)
										{
											if(pkEventInfo->getNumChoices() == 1)
											{
												bResult = true;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{		
					if(pkEventChoiceInfo->isOneShot() || pkEventChoiceInfo->Expires())
					{
						bResult = true;
					}
				}
			}
		}
	}
	lua_pushboolean(L, bResult);
	return 1;
}

int CvLuaCity::lDoCityEventChoice(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)lua_tointeger(L, 2);
	pkCity->DoEventChoice(eEventChoice);
	return 1;
}
int CvLuaCity::lDoCityStartEvent(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventTypes eEvent = (CityEventTypes)lua_tointeger(L, 2);
	pkCity->DoStartEvent(eEvent);
	return 1;
}
int CvLuaCity::lDoCancelCityEventChoice(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEvent = (CityEventChoiceTypes)lua_tointeger(L, 2);
	pkCity->DoCancelEventChoice(eEvent);
	return 1;
}
int CvLuaCity::lGetCityEventCooldown(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventTypes eEvent = (CityEventTypes)lua_tointeger(L, 2);
	const int iCooldown = pkCity->GetEventCooldown(eEvent);
	CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
	if(pkEventInfo != NULL && pkEventInfo->isOneShot())
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, iCooldown);
	return 1;
}
int CvLuaCity::lSetCityEventCooldown(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventTypes eEvent = (CityEventTypes)lua_tointeger(L, 2);
	const int iCooldown = lua_tointeger(L, 3);
	pkCity->SetEventCooldown(eEvent, iCooldown);
	return 1;
}
int CvLuaCity::lGetCityEventChoiceCooldown(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEvent = (CityEventChoiceTypes)lua_tointeger(L, 2);
	CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEvent);
	if(pkEventChoiceInfo != NULL && pkEventChoiceInfo->isOneShot())
	{
		lua_pushinteger(L, -1);
		return 1;
	}
	const int iCooldown = pkCity->GetEventChoiceDuration(eEvent);
	lua_pushinteger(L, iCooldown);
	return 1;
}
int CvLuaCity::lSetCityEventChoiceCooldown(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEvent = (CityEventChoiceTypes)lua_tointeger(L, 2);
	const int iCooldown = lua_tointeger(L, 3);
	pkCity->SetEventChoiceDuration(eEvent, iCooldown);
	return 1;
}
int CvLuaCity::lIsCityEventChoiceValid(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)lua_tointeger(L, 2);
	const CityEventTypes eEvent = (CityEventTypes)lua_tointeger(L, 3);
	const bool bValue = pkCity->IsCityEventChoiceValid(eEventChoice, eEvent);

	lua_pushboolean(L, bValue);

	return 1;
}
#endif

#if defined(MOD_BALANCE_CORE_JFD)
int CvLuaCity::lIsColony(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	lua_pushboolean(L, pkCity->IsColony());

	return 1;
}
int CvLuaCity::lSetColony(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const bool bValue = lua_toboolean(L, 2);
	pkCity->SetColony(bValue);
	return 1;
}
int CvLuaCity::lGetProvinceLevel(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetProvinceLevel());
	return 1;
}
int CvLuaCity::lSetProvinceLevel(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetProvinceLevel(iValue);
	return 1;
}

int CvLuaCity::lHasProvinceLevel(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	const bool bValue = (pkCity->GetProvinceLevel() == iValue);
	lua_pushboolean(L, bValue);

	return 1;
}

int CvLuaCity::lGetOrganizedCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetOrganizedCrime());
	return 1;
}
int CvLuaCity::lSetOrganizedCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetOrganizedCrime(iValue);
	return 1;
}
int CvLuaCity::lHasOrganizedCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	lua_pushboolean(L, pkCity->HasOrganizedCrime());

	return 1;
}

int CvLuaCity::lChangeResistanceCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->ChangeResistanceCounter(iValue);
	return 1;
}
int CvLuaCity::lSetResistanceCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetResistanceCounter(iValue);
	return 1;
}
int CvLuaCity::lGetResistanceCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetResistanceCounter());
	return 1;
}
int CvLuaCity::lChangePlagueCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->ChangePlagueCounter(iValue);
	return 1;
}
int CvLuaCity::lSetPlagueCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetPlagueCounter(iValue);
	return 1;
}
int CvLuaCity::lGetPlagueCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetPlagueCounter());
	return 1;
}
int CvLuaCity::lGetPlagueTurns(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetPlagueTurns());
	return 1;
}
int CvLuaCity::lChangePlagueTurns(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->ChangePlagueTurns(iValue);
	return 1;
}
int CvLuaCity::lSetPlagueTurns(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetPlagueTurns(iValue);
	return 1;
}

int CvLuaCity::lGetPlagueType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetPlagueType());
	return 1;
}
int CvLuaCity::lSetPlagueType(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetPlagueType(iValue);
	return 1;
}
int CvLuaCity::lHasPlague(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);

	lua_pushboolean(L, pkCity->HasPlague());

	return 1;
}

int CvLuaCity::lChangeLoyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->ChangeLoyaltyCounter(iValue);
	return 1;
}
int CvLuaCity::lSetLoyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetLoyaltyCounter(iValue);
	return 1;
}
int CvLuaCity::lGetLoyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetLoyaltyCounter());
	return 1;
}
int CvLuaCity::lChangeDisloyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->ChangeDisloyaltyCounter(iValue);
	return 1;
}
int CvLuaCity::lSetDisloyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	pkCity->SetDisloyaltyCounter(iValue);
	return 1;
}
int CvLuaCity::lGetDisloyaltyCounter(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	lua_pushinteger(L, pkCity->GetDisloyaltyCounter());
	return 1;
}
int CvLuaCity::lGetLoyaltyState(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int eLoyalty = pkCity->GetLoyaltyState();
	lua_pushinteger(L, (LoyaltyStateTypes)eLoyalty);
	return 1;
}
int CvLuaCity::lSetLoyaltyState(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const LoyaltyStateTypes eLoyalty = (LoyaltyStateTypes)lua_tointeger(L, 2);
	pkCity->SetLoyaltyState((int)eLoyalty);
	return 1;
}

int CvLuaCity::lHasLoyaltyState(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);
	const bool bValue = (pkCity->GetLoyaltyState() == iValue);
	lua_pushboolean(L, bValue);

	return 1;
}

int CvLuaCity::lGetYieldModifierFromHappiness(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldModifierFromHappiness(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldModifierFromHappiness(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldModifierFromHappiness(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldModifierFromHealth(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldModifierFromHealth(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldModifierFromHealth(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldModifierFromHealth(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldModifierFromCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldModifierFromCrime(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldModifierFromCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldModifierFromCrime(eYield, iValue);
	return 1;
}


int CvLuaCity::lGetYieldModifierFromDevelopment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldModifierFromDevelopment(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldModifierFromDevelopment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldModifierFromDevelopment(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldFromHappiness(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldFromHappiness(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldFromHappiness(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldFromHappiness(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldFromHealth(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldFromHealth(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldFromHealth(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldFromHealth(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldFromCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldFromCrime(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldFromCrime(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldFromCrime(eYield, iValue);
	return 1;
}

int CvLuaCity::lGetYieldFromDevelopment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	const int iValue = pkCity->GetYieldFromDevelopment(eYield);
	lua_pushinteger(L, iValue);
	return 1;
}
int CvLuaCity::lSetYieldFromDevelopment(lua_State* L)
{
	CvCity* pkCity = GetInstance(L);
	const int iValue = lua_tointeger(L, 3);
	const YieldTypes eYield = (YieldTypes)lua_tointeger(L, 2);
	pkCity->SetYieldFromDevelopment(eYield, iValue);
	return 1;
}

#endif