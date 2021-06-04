#pragma once

#ifndef CVENUMSUTIL
#define CVENUMSUTIL

#include "CvEnums.h"
#include "CvGameCoreEnums.h"
#include "CvGlobals.h"
#include "CvDLLUtilDefines.h"

#include <cstddef>

// Utilities for writing generic Infos code

namespace CvEnumsUtil
{
	template<typename EnumType>
	struct Traits;

	//
	// Dynamic count enums
	//
	template<> struct Traits<ColorTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.GetColorInfo().size(); }
	};
	template<> struct Traits<PlayerColorTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.GetPlayerColorInfo().size(); }
	};
	template<> struct Traits<EntityEventTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEntityEventInfo().size(); }
	};
	template<> struct Traits<MultiunitFormationTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getMultiUnitFormationInfo().size(); }
	};
	template<> struct Traits<PlotTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getPlotInfo().size(); }
	};
	template<> struct Traits<GreatPersonTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getGreatPersonInfo().size(); }
	};
	template<> struct Traits<TerrainTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getTerrainInfo().size(); }
	};
	template<> struct Traits<ResourceClassTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getResourceClassInfo().size(); }
	};
	template<> struct Traits<ResourceTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getResourceInfo().size(); }
	};
	template<> struct Traits<FeatureTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getFeatureInfo().size(); }
	};
	template<> struct Traits<CivilizationTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getFeatureInfo().size(); }
	};
	template<> struct Traits<MinorCivTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getMinorCivInfo().size(); }
	};
	template<> struct Traits<LeaderHeadTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getLeaderHeadInfo().size(); }
	};
	template<> struct Traits<UnitTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getUnitInfo().size(); }
	};
	template<> struct Traits<SpecialUnitTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getSpecialUnitInfo().size(); }
	};
	template<> struct Traits<VoteSourceTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getVoteSourceInfo().size(); }
	};
	template<> struct Traits<EventTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEventInfo().size(); }
	};
	template<> struct Traits<EventChoiceTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEventChoiceInfo().size(); }
	};
	template<> struct Traits<CityEventTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getCityEventInfo().size(); }
	};
	template<> struct Traits<CityEventChoiceTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getCityEventChoiceInfo().size(); }
	};
	template<> struct Traits<UnitCombatTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getUnitCombatClassInfo().size(); }
	};
	template<> struct Traits<GameOptionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getGameOptionInfo().size(); }
	};
	template<> struct Traits<MultiplayerOptionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getMPOptionInfo().size(); }
	};
	template<> struct Traits<YieldTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getYieldInfo().size(); }
	};
	template<> struct Traits<RouteTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getRouteInfo().size(); }
	};
	template<> struct Traits<ImprovementTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getImprovementInfo().size(); }
	};
	template<> struct Traits<BuildTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getBuildInfo().size(); }
	};
	template<> struct Traits<HandicapTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getHandicapInfo().size(); }
	};
	template<> struct Traits<GameSpeedTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getGameSpeedInfo().size(); }
	};
	template<> struct Traits<DiploModifierTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getDiploModifierInfo().size(); }
	};
	template<> struct Traits<ProcessTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getProcessInfo().size(); }
	};
	template<> struct Traits<VoteTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getVoteInfo().size(); }
	};
	template<> struct Traits<ProjectTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getProjectInfo().size(); }
	};
	template<> struct Traits<BuildingClassTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getBuildingClassInfo().size(); }
	};
	template<> struct Traits<BuildingTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getBuildingInfo().size(); }
	};
	template<> struct Traits<UnitClassTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getUnitClassInfo().size(); }
	};
	template<> struct Traits<MissionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getMissionInfo().size(); }
	};
	template<> struct Traits<ControlTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getControlInfo().size(); }
	};
	template<> struct Traits<CommandTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getCommandInfo().size(); }
	};
	template<> struct Traits<PromotionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getPromotionInfo().size(); }
	};
	template<> struct Traits<SpecialistTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getPromotionInfo().size(); }
	};
	template<> struct Traits<EconomicAIStrategyTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEconomicAIStrategyInfo().size(); }
	};
	template<> struct Traits<CitySpecializationTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getCitySpecializationInfo().size(); }
	};
	template<> struct Traits<MilitaryAIStrategyTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getMilitaryAIStrategyInfo().size(); }
	};
	template<> struct Traits<AIGrandStrategyTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getAIGrandStrategyInfo().size(); }
	};
	template<> struct Traits<AICityStrategyTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getAICityStrategyInfo().size(); }
	};
	template<> struct Traits<PolicyTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getPolicyInfo().size(); }
	};
	template<> struct Traits<PolicyBranchTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getPolicyBranchInfo().size(); }
	};
	template<> struct Traits<EmphasizeTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEmphasisInfo().size(); }
	};
	template<> struct Traits<TraitTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getTraitInfo().size(); }
	};
	template<> struct Traits<ReligionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getReligionInfo().size(); }
	};
	template<> struct Traits<CorporationTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getCorporationInfo().size(); }
	};
	template<> struct Traits<ContractTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getContractInfo().size(); }
	};
	template<> struct Traits<BeliefTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getBeliefInfo().size(); }
	};
	template<> struct Traits<LeagueSpecialSessionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getLeagueSpecialSessionInfo().size(); }
	};
	template<> struct Traits<LeagueNameTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getLeagueNameInfo().size(); }
	};
	template<> struct Traits<LeagueProjectTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getLeagueProjectInfo().size(); }
	};
	template<> struct Traits<LeagueProjectRewardTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getLeagueProjectRewardInfo().size(); }
	};
	template<> struct Traits<ResolutionTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getResolutionInfo().size(); }
	};
	template<> struct Traits<TechTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getTechInfo().size(); }
	};
	template<> struct Traits<EraTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getEraInfo().size(); }
	};
	template<> struct Traits<CvHurryInfo>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getHurryInfo().size(); }
	};
	template<> struct Traits<VictoryTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getVictoryInfo().size(); }
	};
	template<> struct Traits<SmallAwardTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getSmallAwardInfo().size(); }
	};
	template<> struct Traits<EAchievement>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getAchievementInfo().size(); }
	};
	template<> struct Traits<FlavorTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return static_cast<std::size_t>(GC.getNumFlavorTypes()); }
	};
	template<> struct Traits<DomainTypes>
	{
		enum { IsFixed = false };
		static inline std::size_t Count() { return GC.getUnitDomainInfo().size(); }
	};

	//
	// Fixed count enums
	//
	template<> struct Traits<PlayerTypes>
	{
		enum
		{
			IsFixed = true,
			CountConstant = MAX_PLAYERS,
		};
		static inline std::size_t Count() { return CountConstant; }
	};
	template<> struct Traits<TeamTypes>
	{
		enum
		{
			IsFixed = true,
			CountConstant = MAX_TEAMS,
		};
		static inline std::size_t Count() { return CountConstant; }
	};
	template<> struct Traits<DirectionTypes>
	{
		enum
		{
			IsFixed = true,
			CountConstant = NUM_DIRECTION_TYPES,
		};
		static inline std::size_t Count() { return CountConstant; }
	};
	
	template<typename EnumType>
	inline std::size_t Count()
	{
		return Traits<EnumType>::Count();
	}
}

#endif
