// Installed from simulator/capture/VoxRlCaptureBuilders.cpp. Edit that source, then reinstall.

// Vox Deorum: recording capture block builders. See VoxRlCaptureBuilders.h.
#include "CvGameCoreDLLPCH.h"
#include "VoxDeorumRL/VoxRlCaptureBuilders.h"
#include "schema/VoxRlCollectors.generated.h"
#include "schema/MilitaryFlavors.h"
#include "schema/MissionIdentities.h"

#include "VoxDeorumRL/VoxRlCapture.h"
#include "VoxDeorumRL/VoxRlCaptureMilitaryEvents.h"
#include "CvDangerPlots.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvFlavorManager.h"
#include "CvGrandStrategyAI.h"
#include "CvInternalGameCoreUtils.h"
#include "CvMilitaryAI.h"
#include "CvTacticalAnalysisMap.h"
#include "CvTechClasses.h"
#include "CvTradeClasses.h"
#include "CvCorporationClasses.h"
#include "CvReligionClasses.h"
#include "CvPromotionClasses.h"
#include "CvTypes.h"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

// Copies one native trade connection's scalar state into either WORLD or REQUEST row.
// Returns false when the path position does not fit its narrowed field.
template <typename Row>
bool VoxRlAssignTradeConnection(Row& row, const TradeConnection& native)
{
	row.id = native.m_iID;
	row.unitId = native.m_unitID;
	row.originOwner = static_cast<i8>(native.m_eOriginOwner);
	row.originCityId = native.m_iOriginID;
	row.destOwner = static_cast<i8>(native.m_eDestOwner);
	row.destCityId = native.m_iDestID;
	row.domain = static_cast<i8>(native.m_eDomain);
	if (!VoxRlAssignChecked(row.locationIndex, native.m_iTradeUnitLocationIndex,
		"TradeConnectionRecord", "locationIndex", -1)) return false;
	row.setMovingForward(native.m_bTradeUnitMovingForward);
	VoxRlAssignClamped(row.speedFactor, native.m_iSpeedFactor);
	VoxRlAssignClamped(row.ownerRouteSpeed, GET_PLAYER(native.m_eOriginOwner).GetTrade()->GetTradeRouteSpeed(native.m_eDomain));
	VoxRlAssignClamped(row.circuitsCompleted, native.m_iCircuitsCompleted);
	VoxRlAssignClamped(row.circuitsToComplete, native.m_iCircuitsToComplete);
	row.setRecalled(native.m_bTradeUnitRecalled);
	const CorporationTypes corporation = GET_PLAYER(native.m_eOriginOwner).GetCorporations()->GetFoundedCorporation();
	const CvCorporationEntry* info = corporation != NO_CORPORATION ? GC.getCorporationInfo(corporation) : NULL;
	row.setCorporationInvulnerable(info != NULL && info->IsTradeRoutesInvulnerable());
	return true;
}

// Returns the shared capture log, or null when logging is unavailable.
FILogFile* VoxRlCaptureLog()
{
	return LOGFILEMGR.GetLog("VoxRlCapture.log", FILogFile::kDontTimeStamp);
}

// Records a rejected route path without discarding other captured routes.
void VoxRlNoteTradePathFailure(const TradeConnection& native, const char* reason, size_t pathIndex)
{
	FILogFile* log = VoxRlCaptureLog();
	if (log != NULL)
		log->Msg("Skipping trade route %d: %s at path index %u.\n",
			native.m_iID, reason, static_cast<unsigned int>(pathIndex));
}

// Resolves one native trade path to portable plot indices in native order.
template <typename Row>
bool VoxRlCollectTradePath(const TradeConnection& native, std::vector<Row>& path)
{
	for (size_t index = 0; index < native.m_aPlotList.size(); ++index)
	{
		const TradeConnectionPlot& nativePlot = native.m_aPlotList[index];
		CvPlot* plot = GC.getMap().plot(nativePlot.m_iX, nativePlot.m_iY);
		if (plot == NULL)
		{
			VoxRlNoteTradePathFailure(native, "plot lookup failed", index);
			return false;
		}
		if (plot->GetPlotIndex() < 0 || plot->GetPlotIndex() > 32767)
		{
			VoxRlNoteTradePathFailure(native, "plot index exceeds the wire range", index);
			return false;
		}
		Row row;
		std::memset(&row, 0, sizeof(row));
		row.plotIndex = static_cast<i16>(plot->GetPlotIndex());
		path.push_back(row);
	}
	if (path.size() >= 2U) return true;
	VoxRlNoteTradePathFailure(native, "path has fewer than two plots", path.size());
	return false;
}

// Captures every active native route and its fixed path for a WORLD checkpoint.
bool VoxRlCollectWorldTrade(VoxRlWorldData& data)
{
	CvGameTrade* trade = GC.getGame().GetGameTrade();
	if (trade == NULL) return false;
	for (size_t index = 0; index < trade->GetNumTradeConnections(); ++index)
	{
		const TradeConnection& native = trade->GetTradeConnection(index);
		if (trade->IsTradeRouteIndexEmpty(static_cast<int>(index)) || !native.isValid()) continue;
		TradeConnectionRecord row;
		std::memset(&row, 0, sizeof(row));
		if (!VoxRlAssignTradeConnection(row, native)) continue;
		std::vector<TradePathPlotRecord> path;
		if (!VoxRlCollectTradePath(native, path)) continue;
		if (!AppendTradeConnectionRecordPathRange(&row, &data, path)) return false;
		data.worldTradeConnections.push_back(row);
	}
	return true;
}

// Captures a complete trade roster replacement for a REQUEST checkpoint.
bool VoxRlCollectRequestTrade(VoxRlRequestData& data)
{
	if (!data.requestTradeRoster.empty()) return true;
	CvGameTrade* trade = GC.getGame().GetGameTrade();
	if (trade == NULL) return false;
	std::vector<RequestTradeConnectionRecord> routes;
	for (size_t index = 0; index < trade->GetNumTradeConnections(); ++index)
	{
		const TradeConnection& native = trade->GetTradeConnection(index);
		if (trade->IsTradeRouteIndexEmpty(static_cast<int>(index)) || !native.isValid()) continue;
		RequestTradeConnectionRecord row;
		std::memset(&row, 0, sizeof(row));
		if (!VoxRlAssignTradeConnection(row, native)) continue;
		std::vector<RequestTradePathPlotRecord> path;
		if (!VoxRlCollectTradePath(native, path)) continue;
		if (!AppendRequestTradeConnectionRecordPathRange(&row, &data, path)) return false;
		routes.push_back(row);
	}
	RequestTradeRosterRecord roster;
	std::memset(&roster, 0, sizeof(roster));
	if (!AppendRequestTradeRosterRecordRouteRange(&roster, &data, routes)) return false;
	data.requestTradeRoster.push_back(roster);
	return true;
}

// Returns a high-resolution monotonic timestamp in nanoseconds for the
// opt-in capture timings.
unsigned __int64 VoxRlTimingNanoseconds()
{
	static LONGLONG frequency = 0;
	if (frequency == 0)
	{
		LARGE_INTEGER period;
		if (!QueryPerformanceFrequency(&period)) return 0;
		frequency = period.QuadPart;
	}
	LARGE_INTEGER now;
	if (!QueryPerformanceCounter(&now)) return 0;
	// Seconds and remainder are converted separately so the multiply cannot
	// overflow the 64-bit total at any uptime.
	return static_cast<unsigned __int64>(now.QuadPart / frequency) * 1000000000ULL +
		(static_cast<unsigned __int64>(now.QuadPart % frequency) * 1000000000ULL) /
		static_cast<unsigned __int64>(frequency);
}

namespace
{
	// Adds one WORLD builder phase only when the caller requested timings.
	class ScopedWorldTiming
	{
	public:
		// Starts a phase only when the caller supplied an accumulator.
		ScopedWorldTiming(unsigned __int64* total)
			: m_total(total), m_start(total != NULL ? VoxRlTimingNanoseconds() : 0)
		{
		}
		// Adds the completed interval to the selected phase.
		~ScopedWorldTiming()
		{
			Stop();
		}
		// Completes the phase once and releases its accumulator reference.
		void Stop()
		{
			if (m_total != NULL)
			{
				*m_total += VoxRlTimingNanoseconds() - m_start;
				m_total = NULL;
			}
		}
	private:
		unsigned __int64* m_total;
		unsigned __int64 m_start;
	};

	// Zero-initializes one packed record so padding bytes and unset fields
	// serialize deterministically.
	template <typename Record>
	void ZeroRecord(Record& record)
	{
		std::memset(&record, 0, sizeof(Record));
	}

	// Collects the alive team ids in ascending order.
	void CollectAliveTeams(std::vector<TeamTypes>& teams)
	{
		for (int team = 0; team < MAX_TEAMS; ++team)
		{
			if (GET_TEAM(static_cast<TeamTypes>(team)).isAlive())
			{
				teams.push_back(static_cast<TeamTypes>(team));
			}
		}
	}

	// Collects the alive player ids in ascending order.
	void CollectAlivePlayers(std::vector<PlayerTypes>& players)
	{
		for (int player = 0; player < MAX_PLAYERS; ++player)
		{
			if (GET_PLAYER(static_cast<PlayerTypes>(player)).isAlive())
			{
				players.push_back(static_cast<PlayerTypes>(player));
			}
		}
	}

	// Checks and assigns one signed sixteen-bit field without hiding an invalid
	// native value through saturation.
	bool AssignCheckedI16(i16& destination, int value, const char* record,
		const char* field, int minimum = -32768)
	{
		if (value < minimum || value > 32767)
		{
			VoxRlNoteCaptureRangeFailure("range", record, field, value, minimum, 32767);
			return false;
		}
		destination = static_cast<i16>(value);
		return true;
	}

	// Assigns one native turns-to-checkpoint estimate. Native records INT_MAX when the
	// checkpoint is unreachable (TurnsToReachTarget) and only compares the estimates with
	// each other, so any estimate above the field range is stored as 32767, which keeps
	// those comparisons the same. Values below -1 still fail the capture.
	bool AssignCheckpointTurns(i16& destination, int value, const char* record)
	{
		return AssignCheckedI16(destination, value > 32767 ? 32767 : value, record, "checkpointTurns", -1);
	}

	// Returns whether a building class belongs to any wonder family. Wonder
	// supplies and costs are part of the player's external resource total.
	bool IsWonderBuilding(const CvBuildingEntry& building)
	{
		CvBuildingClassInfo* buildingClass = GC.getBuildingClassInfo(
			static_cast<BuildingClassTypes>(building.GetBuildingClassType()));
		return buildingClass != NULL && (isWorldWonderClass(*buildingClass) ||
			isTeamWonderClass(*buildingClass) || isNationalWonderClass(*buildingClass));
	}

	// Collects one city's ordinary-building net rows and optionally accumulates
	// ordinary supply and wonder consumption for the player's external total.
	template <typename ResourceRow>
	bool CollectCityResources(CvCity& city, std::vector<ResourceRow>& rows,
		std::vector<int>* ordinarySupply, std::vector<int>* wonderConsumption)
	{
		bool valid = true;
		const int resourceCount = GC.getNumResourceInfos();
		std::vector<int> contribution(resourceCount, 0);
		if (ordinarySupply != NULL && ordinarySupply->size() != static_cast<size_t>(resourceCount)) return false;
		if (wonderConsumption != NULL && wonderConsumption->size() != static_cast<size_t>(resourceCount)) return false;
		for (int buildingType = 0; buildingType < GC.getNumBuildingInfos(); ++buildingType)
		{
			CvBuildingEntry* building = GC.getBuildingInfo(static_cast<BuildingTypes>(buildingType));
			if (building == NULL) continue;
			const int active = city.GetCityBuildings()->GetNumActiveBuilding(static_cast<BuildingTypes>(buildingType));
			if (active == 0) continue;
			const bool wonder = IsWonderBuilding(*building);
			for (int resourceType = 0; resourceType < resourceCount; ++resourceType)
			{
				const int supply = active * building->GetResourceQuantity(resourceType);
				const int consumption = active * building->GetResourceQuantityRequirement(resourceType);
				if (wonder)
				{
					if (wonderConsumption != NULL) (*wonderConsumption)[resourceType] += consumption;
				}
				else
				{
					contribution[resourceType] += supply - consumption;
					if (ordinarySupply != NULL) (*ordinarySupply)[resourceType] += supply;
				}
			}
		}
		for (int resourceType = 0; resourceType < resourceCount; ++resourceType)
		{
			if (contribution[resourceType] == 0) continue;
			ResourceRow row;
			ZeroRecord(row);
			row.cityOwner = static_cast<i8>(city.getOwner());
			row.cityId = city.GetID();
			if (!AssignCheckedI16(row.resourceType, resourceType, "CityResourceRecord", "resourceType", 0)) valid = false;
			if (!AssignCheckedI16(row.contribution, contribution[resourceType], "CityResourceRecord", "contribution")) valid = false;
			rows.push_back(row);
		}
		return valid;
	}

	// Returns whether the owning team can ordinarily use this plot's resource.
	// Force reveal and major-gift bypasses remain in the external residual.
	bool IsOrdinaryTileResourceConnected(CvPlot& plot, ResourceTypes resource)
	{
		if (!plot.isOwned() || resource == NO_RESOURCE) return false;
		const TeamTypes team = plot.getTeam();
		if (team == NO_TEAM) return false;
		CvTeam& ownerTeam = GET_TEAM(team);
		return ownerTeam.IsResourceRevealed(resource) && plot.IsResourceImprovedForOwner();
	}

	// Accumulates raw quantities from ordinarily connected owned tiles once. Quantity
	// bonuses stay in the external residual because the runtime stores raw plot counts.
	void CollectTileResourceTotals(std::vector<std::vector<int> >& totals)
	{
		CvMap& map = GC.getMap();
		for (int plotIndex = 0; plotIndex < map.numPlots(); ++plotIndex)
		{
			CvPlot* plot = map.plotByIndex(plotIndex);
			if (plot == NULL || plot->getOwner() < 0 || plot->getOwner() >= MAX_PLAYERS) continue;
			const ResourceTypes resource = plot->getResourceType();
			if (resource == NO_RESOURCE || plot->getNumResource() <= 0 ||
				!IsOrdinaryTileResourceConnected(*plot, resource)) continue;
			totals[plot->getOwner()][resource] += plot->getNumResource();
		}
	}

	// Collects one player's external resource rows from already separated tile,
	// ordinary-building, and wonder-consumption totals.
	template <typename ResourceRow>
	bool CollectPlayerResources(CvPlayer& player, const std::vector<int>& tileSupply,
		const std::vector<int>& ordinarySupply, const std::vector<int>& wonderConsumption,
		std::vector<ResourceRow>& rows)
	{
		bool valid = true;
		const int resourceCount = GC.getNumResourceInfos();
		if (tileSupply.size() != static_cast<size_t>(resourceCount) ||
			ordinarySupply.size() != static_cast<size_t>(resourceCount) ||
			wonderConsumption.size() != static_cast<size_t>(resourceCount)) return false;
		for (int resourceType = 0; resourceType < resourceCount; ++resourceType)
		{
			const int external = player.getNumResourceTotal(static_cast<ResourceTypes>(resourceType), true) -
				tileSupply[resourceType] - ordinarySupply[resourceType] - wonderConsumption[resourceType];
			ResourceRow row;
			ZeroRecord(row);
			row.player = static_cast<i8>(player.GetID());
			if (!AssignCheckedI16(row.resourceType, resourceType, "PlayerResourceRecord", "resourceType", 0)) valid = false;
			if (!AssignCheckedI16(row.externalContribution, external, "PlayerResourceRecord", "externalContribution")) valid = false;
			rows.push_back(row);
		}
		return valid;
	}

	// Collects the shared balance and maintenance fields into a WORLD or REQUEST row.
	template <typename EconomicsRow>
	bool CollectPlayerEconomics(CvPlayer& player, EconomicsRow& row)
	{
		ZeroRecord(row);
		row.player = static_cast<i8>(player.GetID());
		row.goldTimes100 = player.GetTreasury()->GetGoldTimes100();
		row.faithTimes100 = player.GetFaithTimes100();
		row.faithPerTurnTimes100 = player.GetTotalFaithPerTurnTimes100();
		int maintenance = 0;
		int rate = 0;
		int weight = 0;
		if (!VoxRlGetMilitaryMaintenanceBaseline(player.GetID(), maintenance, rate, weight))
		{
			maintenance = player.GetTreasury()->CalculateUnitCost() * 100;
			rate = player.getGoldPerUnitTimes100();
			int loop = 0;
			for (CvUnit* unit = player.firstUnit(&loop); unit != NULL; unit = player.nextUnit(&loop))
				weight += rate + unit->getUnitInfo().GetExtraMaintenanceCost() * 100;
		}
		row.unitMaintenanceTimes100 = maintenance;
		VoxRlAssignClamped(row.baseGoldPerUnitTimes100, rate);
		row.maintenanceWeight = weight;
		int intervalTurn = -1;
		int accruedGold = 0;
		VoxRlGetMilitaryEconomicInterval(player.GetID(), intervalTurn, accruedGold);
		if (!AssignCheckedI16(row.economicIntervalTurn, intervalTurn,
			"PlayerEconomicsRecord", "economicIntervalTurn", -1)) return false;
		row.externalGoldAccruedTimes100 = accruedGold;
		row.baseBuildingMaintenance = player.GetTreasury()->GetBaseBuildingGoldMaintenance();
		row.buildingMaintenance = player.GetTreasury()->GetBuildingGoldMaintenance();
		return true;
	}

}

// Reports one capture conversion or row the wire contract rejected. The generated
// collectors and split call sites report each rejected value while collecting,
// so the capture log names the reason, record, field, original value, and allowed
// interval.
void VoxRlNoteCaptureRangeFailure(const char* reason, const char* record, const char* field, i32 value, i32 minValue, i32 maxValue)
{
	FILogFile* log = VoxRlCaptureLog();
	if (log != NULL)
	{
		log->Msg("Capture rejected a value: reason=%s record=%s field=%s value=%d allowed=%d..%d.\n",
			reason != NULL ? reason : "?", record != NULL ? record : "?",
			field != NULL ? field : "?", value, minValue, maxValue);
	}
}

// Notes one split or expand failure through the shared capture log.
void VoxRlNoteSplitFailure(const VoxRlFieldRangeFailure& failure)
{
	VoxRlNoteCaptureRangeFailure(failure.reason, failure.record, failure.field,
		failure.value, failure.minValue, failure.maxValue);
}

// Collects one city's complete ordinary-building resource replacement rows.
bool VoxRlCollectCityResourceRows(CvCity& city,
	std::vector<RequestCityResourceRecord>& rows)
{
	return CollectCityResources(city, rows, NULL, NULL);
}

// Collects complete external resource replacement rows for selected players with
// one shared scan of the map's ordinary connected tile supplies.
bool VoxRlCollectPlayerResourceRows(const std::set<int>& players,
	std::vector<RequestPlayerResourceRecord>& rows)
{
	bool valid = true;
	const int resourceCount = GC.getNumResourceInfos();
	std::vector<std::vector<int> > allTileSupply(MAX_PLAYERS, std::vector<int>(resourceCount, 0));
	CollectTileResourceTotals(allTileSupply);
	for (std::set<int>::const_iterator playerId = players.begin(); playerId != players.end(); ++playerId)
	{
		if (*playerId < 0 || *playerId >= MAX_PLAYERS) return false;
		CvPlayer& player = GET_PLAYER(static_cast<PlayerTypes>(*playerId));
		std::vector<int> ordinarySupply(resourceCount, 0);
		std::vector<int> wonderConsumption(resourceCount, 0);
		std::vector<RequestCityResourceRecord> ignoredRows;
		int loop = 0;
		for (CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
		{
			if (!CollectCityResources(*city, ignoredRows, &ordinarySupply, &wonderConsumption)) valid = false;
		}
		if (!CollectPlayerResources(player, allTileSupply[*playerId], ordinarySupply,
			wonderConsumption, rows)) valid = false;
	}
	return valid;
}

// Collects one player's current balances and retained maintenance baseline.
bool VoxRlCollectPlayerEconomicsRecord(CvPlayer& player,
	RequestPlayerEconomicsRecord& row)
{
	return CollectPlayerEconomics(player, row);
}

// Resolves the owner of a bare damage-ledger unit id following the item 7
// replay disposition: the smallest non-acting owner that has the unit, and
// the acting player when nobody else does.
int VoxRlResolveDamagedUnitOwner(PlayerTypes actingPlayer, int unitId)
{
	if (unitId < 0)
	{
		return static_cast<int>(actingPlayer);
	}
	int owner = static_cast<int>(actingPlayer);
	for (int candidate = 0; candidate < MAX_PLAYERS; ++candidate)
	{
		if (candidate == static_cast<int>(actingPlayer)) continue;
		CvUnit* unit = GET_PLAYER(static_cast<PlayerTypes>(candidate)).getUnit(unitId);
		if (unit == NULL) continue;
		if (owner == static_cast<int>(actingPlayer) || candidate < owner)
		{
			owner = candidate;
		}
	}
	return owner;
}

// Resolves the owner of a damaged city through the assignment's target plot
// first, then by the smallest owner that has the city id.
int VoxRlResolveDamagedCityOwner(PlayerTypes actingPlayer, const STacticalAssignment& assignment)
{
	if (assignment.iDamagedCityId < 0)
	{
		return -1;
	}
	CvPlot* targetPlot = GC.getMap().plotByIndex(assignment.iToPlotIndex);
	if (targetPlot != NULL)
	{
		CvCity* plotCity = targetPlot->getPlotCity();
		if (plotCity != NULL && plotCity->GetID() == assignment.iDamagedCityId)
		{
			return static_cast<int>(plotCity->getOwner());
		}
	}
	int owner = -1;
	for (int candidate = 0; candidate < MAX_PLAYERS; ++candidate)
	{
		if (GET_PLAYER(static_cast<PlayerTypes>(candidate)).getCity(assignment.iDamagedCityId) != NULL)
		{
			owner = candidate;
			break;
		}
	}
	return owner;
}

// Collects result rows in native order and lets generated helpers own their
// owner-relative child ranges.
bool VoxRlCollectAssignmentRows(PlayerTypes actingPlayer,
	const std::vector<STacticalAssignment>& assignments, VoxRlResultData& data)
{
	bool valid = true;
	data.resultAssignments.clear();
	data.resultAssignmentDamage.clear();
	data.resultAssignmentHealing.clear();
	for (size_t index = 0; index < assignments.size(); ++index)
	{
		const STacticalAssignment& assignment = assignments[index];
		ResultAssignmentRecord row;
		ZeroRecord(row);
		row.unitId = static_cast<i32>(assignment.iUnitID);
		row.unitOwner = static_cast<i8>(actingPlayer);
		row.assignmentType = static_cast<u8>(assignment.eAssignmentType);
		row.moveType = static_cast<i8>(assignment.eMoveType);
		row.totalScore = static_cast<i16>(assignment.iTotalScore);
		row.fromPlotIndex = static_cast<i16>(assignment.iFromPlotIndex);
		row.toPlotIndex = static_cast<i16>(assignment.iToPlotIndex);
		row.remainingMoves = static_cast<i16>(assignment.iRemainingMoves);
		VoxRlAssignClamped(row.cityDamage, assignment.iCityDamage);
		VoxRlAssignClamped(row.selfDamage, assignment.iSelfDamage);
		row.damagedCityId = static_cast<i32>(assignment.iDamagedCityId);
		row.damagedCityOwner = static_cast<i8>(VoxRlResolveDamagedCityOwner(actingPlayer, assignment));
		std::vector<ResultAssignmentDamageRecord> damageRows;
		for (SUnitIDValueContainer::const_iterator entry = assignment.unitDamage.begin();
			entry != assignment.unitDamage.end(); ++entry)
		{
			ResultAssignmentDamageRecord damage;
			ZeroRecord(damage);
			damage.owner = static_cast<i8>(VoxRlResolveDamagedUnitOwner(actingPlayer, (*entry).first));
			damage.unitId = static_cast<i32>((*entry).first);
			VoxRlAssignClamped(damage.value, (*entry).second);
			damageRows.push_back(damage);
		}
		std::vector<ResultAssignmentHealingRecord> healingRows;
		for (SUnitIDValueContainer::const_iterator entry = assignment.unitHealing.begin();
			entry != assignment.unitHealing.end(); ++entry)
		{
			ResultAssignmentHealingRecord healing;
			ZeroRecord(healing);
			healing.owner = static_cast<i8>(actingPlayer);
			healing.unitId = static_cast<i32>((*entry).first);
			VoxRlAssignClamped(healing.value, (*entry).second);
			healingRows.push_back(healing);
		}
		if (!AppendResultAssignmentRecordUnitDamageRange(&row, &data, damageRows)) valid = false;
		if (!AppendResultAssignmentRecordUnitHealingRange(&row, &data, healingRows)) valid = false;
		data.resultAssignments.push_back(row);
	}
	return valid;
}

// Collects a unit's plague and blocked-promotion rows for sparse capture.
void VoxRlCollectUnitPlagueRows(PlayerTypes eOwner, int iUnitId, CvUnit* pUnit,
	std::vector<UnitPlagueRecord>& plagues, std::vector<UnitBlockedPromotionRecord>& blockedPromotions)
{
	if (pUnit == NULL) return;
	const std::vector<PlagueInfo>& toInflict = pUnit->GetPlaguesToInflict();
	for (size_t index = 0; index < toInflict.size(); ++index)
	{
		UnitPlagueRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(eOwner);
		row.unitId = static_cast<i32>(iUnitId);
		row.plague = static_cast<i16>(toInflict[index].ePlague);
		row.domain = static_cast<i8>(toInflict[index].eDomain);
		row.applyOnAttack = toInflict[index].bApplyOnAttack ? 1 : 0;
		row.applyOnDefense = toInflict[index].bApplyOnDefense ? 1 : 0;
		VoxRlAssignClamped(row.applyChance, toInflict[index].iApplyChance);
		plagues.push_back(row);
	}
	const int promotionCount = GC.getNumPromotionInfos();
	for (int promotion = 0; promotion < promotionCount; ++promotion)
	{
		if (!pUnit->IsPromotionBlocked(static_cast<PromotionTypes>(promotion))) continue;
		UnitBlockedPromotionRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(eOwner);
		row.unitId = static_cast<i32>(iUnitId);
		row.promotion = static_cast<i16>(promotion);
		blockedPromotions.push_back(row);
	}
}

// Collects one unit's complete mission queue and active timed promotion origins.
bool VoxRlCollectUnitTurnRows(CvUnit& unit, std::vector<UnitMissionRecord>& missions,
	std::vector<UnitPromotionTurnRecord>& promotionTurns)
{
	bool valid = true;
	const int missionCount = unit.GetLengthMissionQueue();
	if (missionCount > 255)
	{
		VoxRlNoteCaptureRangeFailure("range", "UnitMissionRecord", "queueIndex", missionCount - 1, 0, 255);
		valid = false;
	}
	for (int index = 0; index < missionCount && index <= 255; ++index)
	{
		const MissionData* mission = unit.GetMissionData(index);
		if (mission == NULL) { valid = false; continue; }
		UnitMissionRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(unit.getOwner());
		row.unitId = unit.GetID();
		row.queueIndex = static_cast<u8>(index);
		if (!AssignCheckedI16(row.missionType, mission->eMissionType,
			"UnitMissionRecord", "missionType")) valid = false;
		row.data1 = mission->iData1;
		row.data2 = mission->iData2;
		row.flags = mission->iFlags;
		if (!AssignCheckedI16(row.pushTurn, mission->iPushTurn,
			"UnitMissionRecord", "pushTurn")) valid = false;
		missions.push_back(row);
	}
	for (int promotion = 0; promotion < GC.getNumPromotionInfos(); ++promotion)
	{
		const PromotionTypes type = static_cast<PromotionTypes>(promotion);
		const CvPromotionEntry* info = GC.getPromotionInfo(type);
		if (info == NULL || info->PromotionDuration() <= 0 || !unit.isHasPromotion(type)) continue;
		UnitPromotionTurnRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(unit.getOwner());
		row.unitId = unit.GetID();
		row.promotion = static_cast<i16>(promotion);
		if (!AssignCheckedI16(row.turnGained, unit.getTurnPromotionGained(type),
			"UnitPromotionTurnRecord", "turnGained")) valid = false;
		promotionTurns.push_back(row);
	}
	return valid;
}

// Collects a unit's nonzero per-attacking-player counts for sparse capture rows.
void VoxRlCollectUnitAttackCountRows(PlayerTypes eOwner, int iUnitId, CvUnit* pUnit,
	std::vector<UnitAttackCountRecord>& rows)
{
	if (pUnit == NULL) return;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		// The ledger is read only through acting units of live owners, so counts for
		// eliminated attackers never reach a simulation and stay uncaptured.
		if (!GET_PLAYER(static_cast<PlayerTypes>(player)).isAlive()) continue;
		const int count = pUnit->GetNumTimesAttackedThisTurn(static_cast<PlayerTypes>(player));
		if (count == 0) continue;
		UnitAttackCountRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(eOwner);
		row.unitId = static_cast<i32>(iUnitId);
		row.attackingPlayer = static_cast<i8>(player);
		VoxRlAssignClamped(row.count, count);
		rows.push_back(row);
	}
}

void VoxRlCollectCityAttackCountRows(CvCity* pCity, std::vector<CityAttackCountRecord>& rows)
{
	if (pCity == NULL) return;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		// The ledger is read only through acting units of live owners, so counts for
		// eliminated attackers never reach a simulation and stay uncaptured.
		if (!GET_PLAYER(static_cast<PlayerTypes>(player)).isAlive()) continue;
		const int count = pCity->GetNumTimesAttackedThisTurn(static_cast<PlayerTypes>(player));
		if (count == 0) continue;
		CityAttackCountRecord row;
		ZeroRecord(row);
		row.cityOwner = static_cast<i8>(pCity->getOwner());
		row.cityId = static_cast<i32>(pCity->GetID());
		row.attackingPlayer = static_cast<i8>(player);
		VoxRlAssignClamped(row.count, count);
		rows.push_back(row);
	}
}

void VoxRlCollectPlayerResistanceRows(CvPlayer* pPlayer, std::vector<PlayerResistanceRecord>& rows)
{
	if (pPlayer == NULL) return;
	for (int opponent = 0; opponent < MAX_PLAYERS; ++opponent)
	{
		if (opponent == static_cast<int>(pPlayer->GetID())) continue;
		// A dead opponent can never attack and a dead player can never query, so pairs
		// involving an eliminated player never reach a simulation.
		if (!GET_PLAYER(static_cast<PlayerTypes>(opponent)).isAlive()) continue;
		const int resistance = pPlayer->GetDominationResistance(static_cast<PlayerTypes>(opponent));
		if (resistance == 0) continue;
		PlayerResistanceRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(pPlayer->GetID());
		row.opponent = static_cast<i8>(opponent);
		VoxRlAssignClamped(row.dominationResistance, resistance);
		rows.push_back(row);
	}
}

// Collects city combat state, basing capacity, and its operation production promise.
bool VoxRlCollectCityRecord(CvCity& city, PlayerTypes capturingPlayer, CityRecord& row)
{
	if (!CollectCityRecord(city, capturingPlayer, row)) return false;
	VoxRlAssignClamped(row.strengthValueRanged, city.getStrengthValueRanged());
	VoxRlAssignClamped(row.cityBeliefRangeStrikeModifier, city.GetCityBeliefRangeStrikeModifier());
	// CvCity reports its local capacity plus the current owner's global bonus.
	VoxRlAssignClamped(row.airCapacity, city.GetMaxAirUnits() - GET_PLAYER(city.getOwner()).getMaxAirUnits());
	const OperationSlot& promise = city.GetUnitBeingBuiltForOperation();
	row.promisedOperationId = promise.m_iOperationID;
	row.promisedArmyId = promise.m_iArmyID;
	VoxRlAssignClamped(row.promisedSlotIndex, promise.m_iSlotID);
	return VoxRlAssignChecked(row.promisedUnitType,
		city.IsBuildingUnitForOperation() ? city.GetUnitForOperation() : NO_UNIT,
		"CityRecord", "promisedUnitType", -1);
}

// Shares mutable physical plot collection between WORLD and dirty plot deltas.
bool VoxRlCollectPlotRecord(CvPlot& plot, PlotCaptureRecord& row)
{
	return CollectPlotCaptureRecord(plot, row);
}

// Collects one unit's mutable fields and movement-count capabilities.
bool VoxRlCollectUnitRecord(CvUnit& unit, TeamTypes capturingTeam, UnitRecord& row)
{
	bool valid = true;
	if (!CollectUnitRecord(unit, capturingTeam, row)) valid = false;
	// Preserve only XP percent history that the current active promotions cannot determine.
	row.experiencePercentOffset = unit.VoxRlGetExperiencePercentBase();
	for (int promotion = 0; promotion < GC.getNumPromotionInfos(); ++promotion)
	{
		const PromotionTypes type = static_cast<PromotionTypes>(promotion);
		if (unit.isHasPromotion(type) && unit.isPromotionActive(type))
			row.experiencePercentOffset -= GC.getPromotionInfo(type)->GetExperiencePercent();
	}
	int lineageOwner = -1;
	int lineageUnitId = -1;
	VoxRlGetUnitLineage(unit, lineageOwner, lineageUnitId);
	row.lineageOwner = static_cast<i8>(lineageOwner);
	row.lineageUnitId = lineageUnitId;
	// CvUnit::reset initializes the deployment turn to -100 before any operation.
	if (!AssignCheckedI16(row.deployFromOperationTurn, unit.GetDeployFromOperationTurn(),
		"UnitRecord", "deployFromOperationTurn", -100)) valid = false;
#if defined(MOD_BALANCE_CORE_JFD)
	row.setResourceConsumptionExempt(MOD_BALANCE_CORE_JFD && unit.isContractUnit());
#else
	row.setResourceConsumptionExempt(false);
#endif
	CvUnit* transport = unit.getTransportUnit();
	row.transportOwner = transport != NULL ? static_cast<i8>(transport->getOwner()) : static_cast<i8>(NO_PLAYER);
	row.transportUnitId = transport != NULL ? transport->GetID() : -1;
	// The passability arrays are builder-owned fixed-capacity fields, so the
	// builder guards the live table sizes itself instead of relying on the
	// generated collector's guards for other unit fields.
	if (GC.getNumTerrainInfos() > VoxRlTerrainCapacity ||
		GC.getNumFeatureInfos() > VoxRlFeatureCapacity) return false;
	for (int yield = 0; yield < NUM_YIELD_TYPES && yield < 32; ++yield)
	{
		VoxRlAssignClamped(row.yieldFromKills[yield], unit.getYieldFromKills(static_cast<YieldTypes>(yield)));
		VoxRlAssignClamped(row.yieldFromTRPlunder[yield], unit.getYieldFromTRPlunder(static_cast<YieldTypes>(yield)));
		VoxRlAssignClamped(row.yieldFromBarbarianKills[yield], unit.getYieldFromBarbarianKills(static_cast<YieldTypes>(yield)));
	}
	// The promotion passability tables pack one bit per terrain or feature index.
	row.setHasAllowTerrainPassable(unit.GetPromotions().HasAllowTerrainPassable());
	if (row.hasAllowTerrainPassable())
	{
		for (int terrain = 0; terrain < GC.getNumTerrainInfos(); ++terrain)
		{
			if (unit.GetPromotions().GetAllowTerrainPassable(
				static_cast<TerrainTypes>(terrain), unit.getTeam()))
				row.allowTerrainPassable[terrain / 8] = static_cast<u8>(row.allowTerrainPassable[terrain / 8] | (1 << (terrain % 8)));
		}
	}
	row.setHasAllowFeaturePassable(unit.GetPromotions().HasAllowFeaturePassable());
	if (row.hasAllowFeaturePassable())
	{
		for (int feature = 0; feature < GC.getNumFeatureInfos(); ++feature)
		{
			if (unit.GetPromotions().GetAllowFeaturePassable(
				static_cast<FeatureTypes>(feature), unit.getTeam()))
				row.allowFeaturePassable[feature / 8] = static_cast<u8>(row.allowFeaturePassable[feature / 8] | (1 << (feature % 8)));
		}
	}
	return valid;
}

// Captures one immutable event unit before later native changes can overwrite its
// initialized capabilities. Child rows omit the owner and unit keys because their
// parent event supplies the identity.
bool VoxRlAppendEventUnitSnapshot(CvUnit& unit, TeamTypes observingTeam,
	VoxRlRequestData& data, int* eventUnitIndex)
{
	if (eventUnitIndex == NULL) return false;
	UnitRecord full;
	ZeroRecord(full);
	if (!VoxRlCollectUnitRecord(unit, observingTeam, full)) return false;
	UnitWireRecord wire;
	std::vector<UnitSparseFieldRecord> sparse;
	VoxRlFieldRangeFailure failure;
	if (!VoxRlSplitUnitRecord(full, wire, sparse, &failure))
	{
		VoxRlNoteSplitFailure(failure);
		return false;
	}
	RequestEventUnitRecord row;
	std::memcpy(&row, &wire, sizeof(row));
	std::vector<RequestEventUnitSparseFieldRecord> eventSparse;
	for (size_t index = 0; index < sparse.size(); ++index)
	{
		RequestEventUnitSparseFieldRecord child;
		ZeroRecord(child);
		child.fieldId = sparse[index].fieldId;
		child.wordIndex = sparse[index].wordIndex;
		child.value = sparse[index].value;
		eventSparse.push_back(child);
	}
	if (!AppendRequestEventUnitRecordSparseFieldRange(&row, &data, eventSparse)) return false;
	std::vector<UnitPlagueRecord> plagues;
	std::vector<UnitBlockedPromotionRecord> blocked;
	VoxRlCollectUnitPlagueRows(unit.getOwner(), unit.GetID(), &unit, plagues, blocked);
	for (size_t index = 0; index < plagues.size(); ++index)
	{
		RequestEventUnitPlagueRecord child;
		ZeroRecord(child);
		child.plague = plagues[index].plague;
		child.domain = plagues[index].domain;
		child.applyOnAttack = plagues[index].applyOnAttack;
		child.applyOnDefense = plagues[index].applyOnDefense;
		child.applyChance = plagues[index].applyChance;
		data.requestEventUnitPlagues.push_back(child);
	}
	for (size_t index = 0; index < blocked.size(); ++index)
	{
		RequestEventUnitBlockedPromotionRecord child;
		ZeroRecord(child);
		child.promotion = blocked[index].promotion;
		data.requestEventUnitBlockedPromotions.push_back(child);
	}
	std::vector<UnitMissionRecord> missions;
	std::vector<UnitPromotionTurnRecord> promotionTurns;
	if (!VoxRlCollectUnitTurnRows(unit, missions, promotionTurns)) return false;
	for (size_t index = 0; index < missions.size(); ++index)
	{
		RequestEventUnitMissionRecord child;
		ZeroRecord(child);
		child.queueIndex = missions[index].queueIndex;
		child.missionType = missions[index].missionType;
		child.data1 = missions[index].data1;
		child.data2 = missions[index].data2;
		child.flags = missions[index].flags;
		child.pushTurn = missions[index].pushTurn;
		data.requestEventUnitMissions.push_back(child);
	}
	for (size_t index = 0; index < promotionTurns.size(); ++index)
	{
		RequestEventUnitPromotionTurnRecord child;
		ZeroRecord(child);
		child.promotion = promotionTurns[index].promotion;
		child.turnGained = promotionTurns[index].turnGained;
		data.requestEventUnitPromotionTurns.push_back(child);
	}
	std::vector<UnitAttackCountRecord> attacks;
	VoxRlCollectUnitAttackCountRows(unit.getOwner(), unit.GetID(), &unit, attacks);
	for (size_t index = 0; index < attacks.size(); ++index)
	{
		RequestEventUnitAttackCountRecord child;
		ZeroRecord(child);
		child.attackingPlayer = attacks[index].attackingPlayer;
		child.count = attacks[index].count;
		data.requestEventUnitAttackCounts.push_back(child);
	}
	*eventUnitIndex = static_cast<int>(data.requestEventUnits.size());
	data.requestEventUnits.push_back(row);
	return true;
}

// Collects the compact team-specific type tables that CvPlot::updateImpassable
// applies when a terrain or feature defines a prerequisite-passable technology.
bool VoxRlCollectTeamPassabilityRows(std::vector<TeamPassabilityRecord>& rows)
{
	if (GC.getNumTerrainInfos() > VoxRlTerrainCapacity ||
		GC.getNumFeatureInfos() > VoxRlFeatureCapacity) return false;
	rows.clear();
	rows.reserve(MAX_TEAMS);
	for (int teamIndex = 0; teamIndex < MAX_TEAMS; ++teamIndex)
	{
		const TeamTypes teamType = static_cast<TeamTypes>(teamIndex);
		CvTeam& team = GET_TEAM(teamType);
		// Passability rows follow the team rows: alive member teams plus the barbarian
		// team. The loader marks every other team absent and uses its terrain fallback.
		if (!team.isAlive() && teamIndex != BARBARIAN_TEAM) continue;
		TeamPassabilityRecord row;
		ZeroRecord(row);
		row.team = static_cast<i8>(teamType);
		for (int terrainIndex = 0; terrainIndex < GC.getNumTerrainInfos(); ++terrainIndex)
		{
			CvTerrainInfo* info = GC.getTerrainInfo(static_cast<TerrainTypes>(terrainIndex));
			if (info == NULL) return false;
			if (!info->isImpassable()) continue;
			const TechTypes prerequisite = static_cast<TechTypes>(info->GetPrereqPassable());
			row.terrainImpassable[terrainIndex] = prerequisite == NO_TECH ||
				!team.GetTeamTechs()->HasTech(prerequisite) ? 1 : 0;
		}
		for (int featureIndex = 0; featureIndex < GC.getNumFeatureInfos(); ++featureIndex)
		{
			CvFeatureInfo* info = GC.getFeatureInfo(static_cast<FeatureTypes>(featureIndex));
			if (info == NULL) return false;
			if (!info->isImpassable()) continue;
			const TechTypes prerequisite = static_cast<TechTypes>(info->GetPrereqPassable());
			row.featureImpassable[featureIndex] = prerequisite == NO_TECH ||
				!team.GetTeamTechs()->HasTech(prerequisite) ? 1 : 0;
		}
		rows.push_back(row);
	}
	return true;
}

// Collects complete resource capability bitsets for the checkpoint team roster.
bool VoxRlCollectTeamResourceRows(const std::set<int>& teams,
	std::vector<TeamResourceRecord>& rows)
{
	bool valid = true;
	if (GC.getNumResourceInfos() > VoxRlResourceCapacity) return false;
	rows.clear();
	rows.reserve(teams.size());
	for (std::set<int>::const_iterator team = teams.begin(); team != teams.end(); ++team)
	{
		if (*team < 0 || *team >= MAX_TEAMS) return false;
		TeamResourceRecord row;
		ZeroRecord(row);
		if (!CollectTeamResourceRecord(GET_TEAM(static_cast<TeamTypes>(*team)), row)) valid = false;
		rows.push_back(row);
	}
	return valid;
}

// Collects all live units' plague and blocked-promotion rows for sparse capture.
void VoxRlCollectAllUnitPlagueRows(std::vector<UnitPlagueRecord>& plagues,
	std::vector<UnitBlockedPromotionRecord>& blockedPromotions)
{
	int loop = 0;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		loop = 0;
		for (CvUnit* pUnit = owner.firstUnit(&loop); pUnit != NULL; pUnit = owner.nextUnit(&loop))
		{
			if (pUnit->isDelayedDeath()) continue;
			VoxRlCollectUnitPlagueRows(owner.GetID(), pUnit->GetID(), pUnit, plagues, blockedPromotions);
		}
	}
}

// Collects complete turn-state children for each live unit at a WORLD checkpoint.
bool VoxRlCollectAllUnitTurnRows(std::vector<UnitMissionRecord>& missions,
	std::vector<UnitPromotionTurnRecord>& promotionTurns)
{
	bool valid = true;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		int loop = 0;
		for (CvUnit* unit = owner.firstUnit(&loop); unit != NULL; unit = owner.nextUnit(&loop))
		{
			if (unit->isDelayedDeath() || unit->plot() == NULL) continue;
			if (!VoxRlCollectUnitTurnRows(*unit, missions, promotionTurns)) valid = false;
		}
	}
	return valid;
}

// Collects all live units' per-attacking-player counts for sparse capture rows.
void VoxRlCollectAllUnitAttackCountRows(std::vector<UnitAttackCountRecord>& rows)
{
	int loop = 0;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		loop = 0;
		for (CvUnit* pUnit = owner.firstUnit(&loop); pUnit != NULL; pUnit = owner.nextUnit(&loop))
		{
			if (pUnit->isDelayedDeath()) continue;
			VoxRlCollectUnitAttackCountRows(owner.GetID(), pUnit->GetID(), pUnit, rows);
		}
	}
}

void VoxRlCollectAllCityAttackCountRows(std::vector<CityAttackCountRecord>& rows)
{
	int loop = 0;
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		loop = 0;
		for (CvCity* pCity = owner.firstCity(&loop); pCity != NULL; pCity = owner.nextCity(&loop))
		{
			VoxRlCollectCityAttackCountRows(pCity, rows);
		}
	}
}

void VoxRlCollectAllPlayerResistanceRows(std::vector<PlayerResistanceRecord>& rows)
{
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		if (!owner.isAlive()) continue;
		VoxRlCollectPlayerResistanceRows(&owner, rows);
	}
}

// Collects the active trait union without probing every promotion and unit category.
static bool VoxRlCollectPlayerTraitPromotionRows(CvPlayer& player, VoxRlPlayerCitySnapshot& snapshot)
{
	CvPlayerTraits* traits = player.GetPlayerTraits();
	const std::vector<TraitTypes> potentialTraits = traits->GetPotentiallyActiveTraits();
	std::set<std::pair<int, int> > grantsCombat;
	std::set<std::pair<int, int> > grantsClass;
	for (size_t index = 0; index < potentialTraits.size(); ++index)
	{
		CvTraitEntry* trait = GC.getTraitInfo(potentialTraits[index]);
		if (trait == NULL || !traits->HasTrait(potentialTraits[index])) continue;
		const std::multimap<int, int>& entriesCombat = trait->VoxRlGetFreePromotionUnitCombats();
		for (std::multimap<int, int>::const_iterator entry = entriesCombat.begin(); entry != entriesCombat.end(); ++entry)
			if (entry->first >= 0 && entry->first < GC.getNumPromotionInfos() &&
				entry->second >= 0 && entry->second < GC.getNumUnitCombatClassInfos()) grantsCombat.insert(*entry);
		const std::multimap<int, int>& entriesClass = trait->VoxRlGetFreePromotionUnitClasses();
		for (std::multimap<int, int>::const_iterator entry = entriesClass.begin(); entry != entriesClass.end(); ++entry)
			if (entry->first >= 0 && entry->first < GC.getNumPromotionInfos() &&
				entry->second >= 0 && entry->second < GC.getNumUnitClassInfos()) grantsClass.insert(*entry);
	}
	for (std::set<std::pair<int, int> >::const_iterator grant = grantsCombat.begin(); grant != grantsCombat.end(); ++grant)
	{
		PlayerTraitPromotionCombatRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(player.GetID());
		if (!AssignCheckedI16(row.promotion, grant->first, "PlayerTraitPromotionCombatRecord", "promotion", 0) ||
			!AssignCheckedI16(row.unitCombat, grant->second, "PlayerTraitPromotionCombatRecord", "unitCombat", 0)) return false;
		snapshot.playerTraitPromotionCombats.push_back(row);
	}
	for (std::set<std::pair<int, int> >::const_iterator grant = grantsClass.begin(); grant != grantsClass.end(); ++grant)
	{
		PlayerTraitPromotionClassRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(player.GetID());
		if (!AssignCheckedI16(row.promotion, grant->first, "PlayerTraitPromotionClassRecord", "promotion", 0) ||
			!AssignCheckedI16(row.unitClass, grant->second, "PlayerTraitPromotionClassRecord", "unitClass", 0)) return false;
		snapshot.playerTraitPromotionClasses.push_back(row);
	}
	return true;
}

// Appends one player's military unit purchase prices in city then unit order.
// Native's convenience IsCanPurchase rebuilds the owner's building counts on every
// call, so the counts are built once here and passed to the overload it delegates to.
static bool VoxRlCollectCityPurchaseCostRows(CvPlayer& player, std::vector<CityPurchaseCostRecord>& rows)
{
	std::vector<UnitTypes> militaryUnits;
	for (int unitIndex = 0; unitIndex < GC.getNumUnitInfos(); ++unitIndex)
	{
		CvUnitEntry* unit = GC.getUnitInfo(static_cast<UnitTypes>(unitIndex));
		if (unit == NULL || (unit->GetCombat() <= 0 && unit->GetRangedCombat() <= 0 &&
			!unit->IsMilitarySupport() && !unit->IsMilitaryProduction())) continue;
		militaryUnits.push_back(static_cast<UnitTypes>(unitIndex));
	}
	if (militaryUnits.empty()) return true;
	std::vector<int> buildingCounts(GC.getNumBuildingInfos(), 0);
	int loop = 0;
	for (const CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
	{
		const std::vector<BuildingTypes>& buildings = city->GetCityBuildings()->GetAllBuildingsHere();
		for (size_t index = 0; index < buildings.size(); ++index)
			buildingCounts[buildings[index]]++;
	}
	for (CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
	{
		for (size_t index = 0; index < militaryUnits.size(); ++index)
		{
			const UnitTypes unitType = militaryUnits[index];
			const bool gold = city->IsCanPurchase(buildingCounts, false, true, unitType,
				NO_BUILDING, NO_PROJECT, YIELD_GOLD);
			const bool faith = city->IsCanPurchase(buildingCounts, false, true, unitType,
				NO_BUILDING, NO_PROJECT, YIELD_FAITH);
			if (!gold && !faith) continue;
			CityPurchaseCostRecord row;
			ZeroRecord(row);
			row.cityOwner = static_cast<i8>(player.GetID());
			row.cityId = city->GetID();
			if (!AssignCheckedI16(row.unitType, unitType,
				"CityPurchaseCostRecord", "unitType", 0)) return false;
			row.goldCost = gold ? city->GetPurchaseCost(unitType) : -1;
			row.faithCost = faith ? city->GetFaithPurchaseCost(unitType, true) : -1;
			rows.push_back(row);
		}
	}
	return true;
}

// Collects the player and city tables from native state in stable owner and type order.
bool VoxRlCollectPlayerCitySnapshot(VoxRlPlayerCitySnapshot& snapshot, PlayerTypes perspectivePlayer)
{
	snapshot = VoxRlPlayerCitySnapshot();
	for (int teamIndex = 0; teamIndex < MAX_TEAMS; ++teamIndex)
	{
		CvTeam& team = GET_TEAM(static_cast<TeamTypes>(teamIndex));
		if (!team.isAlive() && teamIndex != BARBARIAN_TEAM) continue;
		TeamRecord teamRow;
		ZeroRecord(teamRow);
		if (!CollectTeamRecord(team, teamRow)) return false;
		snapshot.teams.push_back(teamRow);
		for (int techIndex = 0; techIndex < GC.getNumTechInfos(); ++techIndex)
		{
			if (!team.GetTeamTechs()->HasTech(static_cast<TechTypes>(techIndex))) continue;
			TeamTechnologyRecord row;
			ZeroRecord(row);
			row.team = static_cast<i8>(teamIndex);
			if (!AssignCheckedI16(row.technology, techIndex,
				"TeamTechnologyRecord", "technology", 0)) return false;
			snapshot.teamTechnologies.push_back(row);
		}
	}
	for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
	{
		CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(playerIndex));
		if (!player.isAlive() && playerIndex != BARBARIAN_PLAYER) continue;
		if (!VoxRlCollectPlayerTraitPromotionRows(player, snapshot)) return false;
		const std::vector<int>& connectionPlots = player.VoxRlGetCityConnectionPlots();
		for (size_t index = 0; index < connectionPlots.size(); ++index)
		{
			CityConnectionPlotRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.plotIndex, connectionPlots[index],
				"CityConnectionPlotRecord", "plotIndex", 0)) return false;
			snapshot.cityConnectionPlots.push_back(row);
		}
		// Match HasSpecialUnitUpgrade's active-trait union without probing every class/type pair.
		// Activation remains live because beliefs, policies, and technologies can change it.
		CvPlayerTraits* traits = player.GetPlayerTraits();
		const std::vector<TraitTypes> potentialTraits = traits->GetPotentiallyActiveTraits();
		std::set<std::pair<int, int> > upgrades;
		for (size_t index = 0; index < potentialTraits.size(); ++index)
		{
			CvTraitEntry* trait = GC.getTraitInfo(potentialTraits[index]);
			if (trait == NULL || !traits->HasTrait(potentialTraits[index])) continue;
			const std::multimap<int, int>& entries = trait->VoxRlGetSpecialUnitUpgrades();
			for (std::multimap<int, int>::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
				if (entry->first >= 0 && entry->first < GC.getNumUnitClassInfos() &&
					entry->second >= 0 && entry->second < GC.getNumUnitInfos()) upgrades.insert(*entry);
		}
		for (std::set<std::pair<int, int> >::const_iterator upgrade = upgrades.begin(); upgrade != upgrades.end(); ++upgrade)
		{
			PlayerSpecialUpgradeRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.unitClass, upgrade->first,
				"PlayerSpecialUpgradeRecord", "unitClass", 0)) return false;
			if (!AssignCheckedI16(row.unitType, upgrade->second,
				"PlayerSpecialUpgradeRecord", "unitType", 0)) return false;
			snapshot.playerSpecialUpgrades.push_back(row);
		}
		for (int greatPerson = 0; greatPerson < GC.getNumGreatPersonInfos(); ++greatPerson)
		{
			const GreatPersonTypes type = static_cast<GreatPersonTypes>(greatPerson);
			const int rateModifier = player.GetGreatPersonRateModifier(type);
			const int costReduction = traits->GetGreatPersonCostReduction(type);
			if (rateModifier == 0 && costReduction == 0) continue;
			PlayerGreatPersonRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.greatPerson, greatPerson,
				"PlayerGreatPersonRecord", "greatPerson", 0)) return false;
			VoxRlAssignClamped(row.rateModifier, rateModifier);
			VoxRlAssignClamped(row.costReduction, costReduction);
			snapshot.playerGreatPersons.push_back(row);
		}
		const std::vector<ResourceTypes>& monopolies = player.GetStrategicMonopolies();
		const std::set<ResourceTypes> orderedMonopolies(monopolies.begin(), monopolies.end());
		for (std::set<ResourceTypes>::const_iterator resource = orderedMonopolies.begin();
			resource != orderedMonopolies.end(); ++resource)
		{
			PlayerStrategicMonopolyRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.resource, *resource,
				"PlayerStrategicMonopolyRecord", "resource", 0)) return false;
			snapshot.playerStrategicMonopolies.push_back(row);
		}
		const std::vector<CvPurchaseRequest>& savings = player.GetEconomicAI()->VoxRlGetRequestedSavings();
		for (size_t index = 0; index < savings.size(); ++index)
		{
			if (savings[index].m_iAmount == 0 && savings[index].m_iPriority == 0) continue;
			PlayerSavingsRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			row.purchaseType = static_cast<i8>(savings[index].m_eType);
			row.amount = savings[index].m_iAmount;
			VoxRlAssignClamped(row.priority, savings[index].m_iPriority);
			snapshot.playerSavings.push_back(row);
		}
		// Relations and personality flavors are the capturing player's own view. A later actor
		// binds them from its own WORLD, so other players' rows are not captured.
		const std::vector<PlayerTypes> noSettlePlayers = playerIndex == perspectivePlayer ?
			player.GetDiplomacyAI()->GetPlayersWithNoSettlePolicy() : std::vector<PlayerTypes>();
		for (int other = 0; other < MAX_CIV_PLAYERS && playerIndex == perspectivePlayer; ++other)
		{
			if (other == playerIndex ||
				!GET_PLAYER(static_cast<PlayerTypes>(other)).isAlive()) continue;
			PlayerRelationRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			row.otherPlayer = static_cast<i8>(other);
			row.rawMilitaryStrengthComparedToUs = static_cast<i8>(player.GetDiplomacyAI()->GetRawMilitaryStrengthComparedToUs(static_cast<PlayerTypes>(other)));
			row.rawTargetValue = static_cast<i8>(player.GetDiplomacyAI()->GetRawTargetValue(static_cast<PlayerTypes>(other)));
			for (size_t index = 0; index < noSettlePlayers.size(); ++index)
				if (noSettlePlayers[index] == static_cast<PlayerTypes>(other)) row.noSettlePolicy = 1;
			if (playerIndex < MAX_MAJOR_CIVS && other < MAX_MAJOR_CIVS)
			{
				row.approach = static_cast<i8>(player.GetDiplomacyAI()->GetCivApproach(static_cast<PlayerTypes>(other)));
				row.visibleApproachTowardsUs = static_cast<i8>(player.GetDiplomacyAI()->GetVisibleApproachTowardsUs(static_cast<PlayerTypes>(other)));
				row.opinion = static_cast<i8>(player.GetDiplomacyAI()->GetCivOpinion(static_cast<PlayerTypes>(other)));
				row.potentialMilitaryTargetOrThreat =
					player.GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(static_cast<PlayerTypes>(other), false) ? 1 : 0;
				row.armyInPlaceForAttack = player.GetDiplomacyAI()->IsArmyInPlaceForAttack(static_cast<PlayerTypes>(other)) ? 1 : 0;
				row.militaryAggressivePosture = static_cast<i8>(player.GetDiplomacyAI()->GetMilitaryAggressivePosture(static_cast<PlayerTypes>(other)));
				row.warmongerThreat = static_cast<i8>(player.GetDiplomacyAI()->GetWarmongerThreat(static_cast<PlayerTypes>(other)));
				// Relation rows exist only for the capturing player, so this row carries the nuclear
				// grudge counter in both directions; native reads the other player's own counter too.
				row.numTimesNuked = static_cast<i16>(player.GetDiplomacyAI()->GetNumTimesNuked(static_cast<PlayerTypes>(other)));
				row.numTimesNukedThem = static_cast<i16>(GET_PLAYER(static_cast<PlayerTypes>(other)).GetDiplomacyAI()->GetNumTimesNuked(static_cast<PlayerTypes>(playerIndex)));
			}
			snapshot.playerRelations.push_back(row);
		}
		for (int flavor = 0; flavor < GC.getNumFlavorTypes() && playerIndex == perspectivePlayer; ++flavor)
		{
			PlayerFlavorRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.flavorId, flavor,
				"PlayerFlavorRecord", "flavorId", 0)) return false;
			VoxRlAssignClamped(row.value, player.GetFlavorManager()->GetPersonalityIndividualFlavor(static_cast<FlavorTypes>(flavor)));
			VoxRlAssignClamped(row.personalityAndGrandStrategyValue,
				player.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy(static_cast<FlavorTypes>(flavor)));
			VoxRlAssignClamped(row.activeValue, player.GetFlavorManager()->GetActiveFlavor(static_cast<FlavorTypes>(flavor)));
			VoxRlAssignClamped(row.basePersonalityValue, player.GetFlavorManager()->GetBasePersonalityFlavor(static_cast<FlavorTypes>(flavor)));
			VoxRlAssignClamped(row.customValue, player.GetFlavorManager()->HasCustomFlavors()
				? player.GetFlavorManager()->GetCustomFlavor(static_cast<FlavorTypes>(flavor)) : -1);
			snapshot.playerFlavors.push_back(row);
		}
		for (int promotion = 0; promotion < GC.getNumPromotionInfos(); ++promotion)
		{
			const int count = player.GetFreePromotionCount(static_cast<PromotionTypes>(promotion));
			if (count == 0) continue;
			PlayerFreePromotionRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.promotion, promotion,
				"PlayerFreePromotionRecord", "promotion", 0)) return false;
			VoxRlAssignClamped(row.count, count);
			snapshot.playerFreePromotions.push_back(row);
		}
		// Healing uses the actor's state religion and qualifies each owned origin city.
		const CvReligion* religion = GC.getGame().GetGameReligions()->GetReligion(
			player.GetReligions()->GetStateReligion(), player.GetID());
		if (!VoxRlCollectCityPurchaseCostRows(player, snapshot.cityPurchaseCosts)) return false;
		int loop = 0;
		for (CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
		{
			if (religion != NULL && !player.isMinorCiv() && !player.isBarbarian())
				for (int yield = 0; yield < NUM_YIELD_TYPES; ++yield)
					for (int ownedTerritory = 0; ownedTerritory < 2; ++ownedTerritory)
					{
						const int coefficient = religion->m_Beliefs.GetYieldPerHeal(
							static_cast<YieldTypes>(yield), player.GetID(), city, true, ownedTerritory != 0);
						if (coefficient == 0) continue;
						CityHealingYieldRecord row;
						ZeroRecord(row);
						row.cityOwner = static_cast<i8>(playerIndex);
						row.cityId = city->GetID();
						if (!AssignCheckedI16(row.yieldType, yield,
							"CityHealingYieldRecord", "yieldType", 0)) return false;
						row.ownedTerritory = ownedTerritory != 0 ? 1 : 0;
						VoxRlAssignClamped(row.yieldPer100Hp, coefficient);
						snapshot.cityHealingYields.push_back(row);
					}
			const std::vector<PromotionTypes> promotions = city->getFreePromotions();
			for (size_t index = 0; index < promotions.size(); ++index)
			{
				CityFreePromotionRecord row;
				ZeroRecord(row);
				row.cityOwner = static_cast<i8>(playerIndex);
				row.cityId = city->GetID();
				if (!AssignCheckedI16(row.promotion, promotions[index],
					"CityFreePromotionRecord", "promotion", 0)) return false;
				snapshot.cityFreePromotions.push_back(row);
			}
		}
	}
	return true;
}

namespace
{
	// Reads the team owning a technology row.
	int ChildRowOwner(const TeamTechnologyRecord& row) { return row.team; }
	// Reads the player owning a special-upgrade row.
	int ChildRowOwner(const PlayerSpecialUpgradeRecord& row) { return row.player; }
	// Reads the player owning a savings row.
	int ChildRowOwner(const PlayerSavingsRecord& row) { return row.player; }
	// Reads the player owning a diplomacy row.
	int ChildRowOwner(const PlayerRelationRecord& row) { return row.player; }
	// Reads the player owning a personality flavor row.
	int ChildRowOwner(const PlayerFlavorRecord& row) { return row.player; }
	// Reads the player owning a free promotion count.
	int ChildRowOwner(const PlayerFreePromotionRecord& row) { return row.player; }
	// Reads the city owning a purchase cost row.
	VoxRlCityKey ChildRowOwner(const CityPurchaseCostRecord& row) { return VoxRlCityKey(row.cityOwner, row.cityId); }
	// Reads the city owning a promotion row.
	VoxRlCityKey ChildRowOwner(const CityFreePromotionRecord& row) { return VoxRlCityKey(row.cityOwner, row.cityId); }
	// Reads the spying player owning an espionage sight row.
	int ChildRowOwner(const CityEspionageSightRecord& row) { return row.spyOwner; }
	// Reads the player owning a found-value row.
	int ChildRowOwner(const PlayerFoundValueRecord& row) { return row.player; }

	// Sets the team for a technology replacement.
	void SetChildReplacementOwner(RequestTeamTechnologyReplacementRecord& row, int owner) { row.team = static_cast<i8>(owner); }
	// Sets the player for a special-upgrade replacement.
	void SetChildReplacementOwner(RequestPlayerSpecialUpgradeReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Sets the player for a savings replacement.
	void SetChildReplacementOwner(RequestPlayerSavingsReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Sets the player for a diplomacy replacement.
	void SetChildReplacementOwner(RequestPlayerRelationReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Sets the player for a flavor replacement.
	void SetChildReplacementOwner(RequestPlayerFlavorReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Sets the player for a free promotion replacement.
	void SetChildReplacementOwner(RequestPlayerFreePromotionReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Sets the city for a purchase-cost replacement.
	void SetChildReplacementOwner(RequestCityPurchaseCostReplacementRecord& row, VoxRlCityKey owner)
	{ row.cityOwner = static_cast<i8>(owner.first); row.cityId = owner.second; }
	// Sets the city for a free-promotion replacement.
	void SetChildReplacementOwner(RequestCityFreePromotionReplacementRecord& row, VoxRlCityKey owner)
	{ row.cityOwner = static_cast<i8>(owner.first); row.cityId = owner.second; }
	// Sets the spying player for an espionage sight replacement.
	void SetChildReplacementOwner(RequestCityEspionageSightReplacementRecord& row, int owner) { row.spyOwner = static_cast<i8>(owner); }
	// Sets the player for a found-value replacement.
	void SetChildReplacementOwner(RequestPlayerFoundValueReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }

	// Copies a researched technology into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const TeamTechnologyRecord& source, RequestTeamTechnologyRowRecord& row)
	{ row.technology = source.technology; }
	// Copies a special unit upgrade into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerSpecialUpgradeRecord& source, RequestPlayerSpecialUpgradeRowRecord& row)
	{ row.unitClass = source.unitClass; row.unitType = source.unitType; }
	// Copies a savings commitment into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerSavingsRecord& source, RequestPlayerSavingsRowRecord& row)
	{ row.purchaseType = source.purchaseType; row.amount = source.amount; row.priority = source.priority; }
	// Copies a diplomatic assessment into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerRelationRecord& source, RequestPlayerRelationRowRecord& row)
	{ row.otherPlayer = source.otherPlayer; row.approach = source.approach;
		row.visibleApproachTowardsUs = source.visibleApproachTowardsUs; row.opinion = source.opinion;
		row.potentialMilitaryTargetOrThreat = source.potentialMilitaryTargetOrThreat;
		row.rawMilitaryStrengthComparedToUs = source.rawMilitaryStrengthComparedToUs;
		row.rawTargetValue = source.rawTargetValue; row.noSettlePolicy = source.noSettlePolicy;
		row.armyInPlaceForAttack = source.armyInPlaceForAttack;
		row.militaryAggressivePosture = source.militaryAggressivePosture;
		row.numTimesNuked = source.numTimesNuked; row.numTimesNukedThem = source.numTimesNukedThem;
		row.warmongerThreat = source.warmongerThreat; }
	// Copies a personality flavor into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerFlavorRecord& source, RequestPlayerFlavorRowRecord& row)
	{ row.flavorId = source.flavorId; row.value = source.value;
		row.personalityAndGrandStrategyValue = source.personalityAndGrandStrategyValue; row.activeValue = source.activeValue;
		row.basePersonalityValue = source.basePersonalityValue; row.customValue = source.customValue; }
	// Copies an espionage sight source into its spying-player REQUEST row.
	void CopyChildReplacementRow(const CityEspionageSightRecord& source, RequestCityEspionageSightRowRecord& row)
	{ row.cityOwner = source.cityOwner; row.cityId = source.cityId; row.surveillanceCount = source.surveillanceCount;
		row.revealCityScreen = source.revealCityScreen; row.visionBonus = source.visionBonus; }
	// Copies a cached found value into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerFoundValueRecord& source, RequestPlayerFoundValueRowRecord& row)
	{ row.plotIndex = source.plotIndex; row.value = source.value; }
	// Copies a free promotion count into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerFreePromotionRecord& source, RequestPlayerFreePromotionRowRecord& row)
	{ row.promotion = source.promotion; row.count = source.count; }
	// Copies a purchase cost into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const CityPurchaseCostRecord& source, RequestCityPurchaseCostRowRecord& row)
	{ row.unitType = source.unitType; row.goldCost = source.goldCost; row.faithCost = source.faithCost; }
	// Copies a city promotion into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const CityFreePromotionRecord& source, RequestCityFreePromotionRowRecord& row)
	{ row.promotion = source.promotion; }

	// Reads the player owning a PlayerGreatPerson row.
	int ChildRowOwner(const PlayerGreatPersonRecord& row) { return row.player; }
	// Sets the player for a complete PlayerGreatPerson replacement.
	void SetChildReplacementOwner(RequestPlayerGreatPersonReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Copies a PlayerGreatPerson input into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerGreatPersonRecord& source, RequestPlayerGreatPersonRowRecord& row)
	{ row.greatPerson = source.greatPerson; row.rateModifier = source.rateModifier; row.costReduction = source.costReduction; }

	// Reads the player owning a PlayerStrategicMonopoly row.
	int ChildRowOwner(const PlayerStrategicMonopolyRecord& row) { return row.player; }
	// Reads the player owning a city connection plot.
	int ChildRowOwner(const CityConnectionPlotRecord& row) { return row.player; }
	// Sets the player for a complete city connection replacement.
	void SetChildReplacementOwner(RequestCityConnectionReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Copies one captured connection member into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const CityConnectionPlotRecord& source, RequestCityConnectionRowRecord& row)
	{ row.plotIndex = source.plotIndex; }
	// Sets the player for a complete PlayerStrategicMonopoly replacement.
	void SetChildReplacementOwner(RequestPlayerStrategicMonopolyReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Copies a PlayerStrategicMonopoly input into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerStrategicMonopolyRecord& source, RequestPlayerStrategicMonopolyRowRecord& row)
	{ row.resource = source.resource; }

	// Reads the city owning a healing coefficient row.
	VoxRlCityKey ChildRowOwner(const CityHealingYieldRecord& row) { return VoxRlCityKey(row.cityOwner, row.cityId); }
	// Sets the city for a complete healing coefficient replacement.
	void SetChildReplacementOwner(RequestCityHealingYieldReplacementRecord& row, VoxRlCityKey owner)
	{ row.cityOwner = static_cast<i8>(owner.first); row.cityId = owner.second; }
	// Copies a CityHealingYield input into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const CityHealingYieldRecord& source, RequestCityHealingYieldRowRecord& row)
	{ row.yieldType = source.yieldType; row.ownedTerritory = source.ownedTerritory; row.yieldPer100Hp = source.yieldPer100Hp; }

	// Reads the player owning a trait promotion grant.
	int ChildRowOwner(const PlayerTraitPromotionCombatRecord& row) { return row.player; }
	// Sets the player for a complete trait promotion replacement.
	void SetChildReplacementOwner(RequestPlayerTraitPromotionCombatReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Copies a trait promotion grant into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerTraitPromotionCombatRecord& source, RequestPlayerTraitPromotionCombatRowRecord& row)
	{ row.promotion = source.promotion; row.unitCombat = source.unitCombat; }

	// Reads the player owning a trait promotion grant.
	int ChildRowOwner(const PlayerTraitPromotionClassRecord& row) { return row.player; }
	// Sets the player for a complete trait promotion replacement.
	void SetChildReplacementOwner(RequestPlayerTraitPromotionClassReplacementRecord& row, int owner) { row.player = static_cast<i8>(owner); }
	// Copies a trait promotion grant into its owner-qualified REQUEST row.
	void CopyChildReplacementRow(const PlayerTraitPromotionClassRecord& source, RequestPlayerTraitPromotionClassRowRecord& row)
	{ row.promotion = source.promotion; row.unitClass = source.unitClass; }

	// Emits each owner's complete table when any row in a family differs from the baseline.
	template <typename World, typename RequestRow, typename Replacement, typename Key>
	bool AppendChangedChildFamily(std::vector<World>& baseline, const std::vector<World>& current,
		const std::vector<Key>& owners, std::vector<Replacement>& replacements, VoxRlRequestData& data,
		bool (*append)(Replacement*, VoxRlRequestData*, const std::vector<RequestRow>&))
	{
		if (baseline.size() == current.size() &&
			(baseline.empty() || std::memcmp(&baseline[0], &current[0], baseline.size() * sizeof(World)) == 0))
			return true;
		// Group each side by owner once, keeping row order, so the owner walk stays linear.
		typedef std::map<Key, std::vector<World> > RowsByOwner;
		RowsByOwner oldByOwner;
		for (size_t index = 0; index < baseline.size(); ++index)
			oldByOwner[ChildRowOwner(baseline[index])].push_back(baseline[index]);
		RowsByOwner newByOwner;
		for (size_t index = 0; index < current.size(); ++index)
			newByOwner[ChildRowOwner(current[index])].push_back(current[index]);
		const std::vector<World> noRows;
		for (size_t ownerIndex = 0; ownerIndex < owners.size(); ++ownerIndex)
		{
			const typename RowsByOwner::const_iterator oldEntry = oldByOwner.find(owners[ownerIndex]);
			const std::vector<World>& oldRows = oldEntry == oldByOwner.end() ? noRows : oldEntry->second;
			const typename RowsByOwner::const_iterator newEntry = newByOwner.find(owners[ownerIndex]);
			const std::vector<World>& newRows = newEntry == newByOwner.end() ? noRows : newEntry->second;
			if (oldRows.size() == newRows.size() &&
				(oldRows.empty() || std::memcmp(&oldRows[0], &newRows[0], oldRows.size() * sizeof(World)) == 0))
				continue;
			Replacement replacement;
			ZeroRecord(replacement);
			SetChildReplacementOwner(replacement, owners[ownerIndex]);
			std::vector<RequestRow> rows;
			for (size_t index = 0; index < newRows.size(); ++index)
			{
				RequestRow row;
				ZeroRecord(row);
				CopyChildReplacementRow(newRows[index], row);
				rows.push_back(row);
			}
			if (!append(&replacement, &data, rows)) return false;
			replacements.push_back(replacement);
		}
		baseline = current;
		return true;
	}
}

// Emits child replacements and exact team scalars at a REQUEST boundary.
bool VoxRlAppendPlayerCityReplacements(VoxRlPlayerCitySnapshot& baseline,
	const VoxRlPlayerCitySnapshot& current, const std::vector<int>& playerOwners,
	const std::vector<int>& teamOwners, const std::vector<VoxRlCityKey>& cityOwners,
	VoxRlRequestData& data)
{
	bool valid = true;
	for (size_t index = 0; index < current.teams.size(); ++index)
	{
		const TeamRecord& row = current.teams[index];
		bool changed = true;
		for (size_t old = 0; old < baseline.teams.size(); ++old)
			if (baseline.teams[old].id == row.id)
			{ changed = std::memcmp(&baseline.teams[old], &row, sizeof(row)) != 0; break; }
		if (changed)
		{
			RequestDeltaTeamRecord delta;
			std::memcpy(&delta, &row, sizeof(delta));
			data.requestDeltaTeams.push_back(delta);
		}
	}
	baseline.teams = current.teams;
	if (!AppendChangedChildFamily(baseline.teamTechnologies, current.teamTechnologies,
		teamOwners, data.requestTeamTechnologyReplacements, data,
		&AppendRequestTeamTechnologyReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerSpecialUpgrades, current.playerSpecialUpgrades,
		playerOwners, data.requestPlayerSpecialUpgradeReplacements, data,
		&AppendRequestPlayerSpecialUpgradeReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerSavings, current.playerSavings,
		playerOwners, data.requestPlayerSavingsReplacements, data,
		&AppendRequestPlayerSavingsReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerRelations, current.playerRelations,
		playerOwners, data.requestPlayerRelationReplacements, data,
		&AppendRequestPlayerRelationReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerTraitPromotionCombats, current.playerTraitPromotionCombats,
		playerOwners, data.requestPlayerTraitPromotionCombatReplacements, data,
		&AppendRequestPlayerTraitPromotionCombatReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerTraitPromotionClasses, current.playerTraitPromotionClasses,
		playerOwners, data.requestPlayerTraitPromotionClassReplacements, data,
		&AppendRequestPlayerTraitPromotionClassReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerFlavors, current.playerFlavors,
		playerOwners, data.requestPlayerFlavorReplacements, data,
		&AppendRequestPlayerFlavorReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerFreePromotions, current.playerFreePromotions,
		playerOwners, data.requestPlayerFreePromotionReplacements, data,
		&AppendRequestPlayerFreePromotionReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.cityPurchaseCosts, current.cityPurchaseCosts,
		cityOwners, data.requestCityPurchaseCostReplacements, data,
		&AppendRequestCityPurchaseCostReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerGreatPersons, current.playerGreatPersons,
		playerOwners, data.requestPlayerGreatPersonReplacements, data,
		&AppendRequestPlayerGreatPersonReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.playerStrategicMonopolies, current.playerStrategicMonopolies,
		playerOwners, data.requestPlayerStrategicMonopolyReplacements, data,
		&AppendRequestPlayerStrategicMonopolyReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.cityConnectionPlots, current.cityConnectionPlots,
		playerOwners, data.requestCityConnectionReplacements, data,
		&AppendRequestCityConnectionReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.cityHealingYields, current.cityHealingYields,
		cityOwners, data.requestCityHealingYieldReplacements, data,
		&AppendRequestCityHealingYieldReplacementRecordRowRange)) valid = false;
	if (!AppendChangedChildFamily(baseline.cityFreePromotions, current.cityFreePromotions,
		cityOwners, data.requestCityFreePromotionReplacements, data,
		&AppendRequestCityFreePromotionReplacementRecordRowRange)) valid = false;
	return valid;
}

// Counts native military-support units that the live unit-row filter omits.
int VoxRlMilitaryUnitCountCorrection(CvPlayer& player)
{
	int capturedCount = 0;
	int loop = 0;
	for (CvUnit* unit = player.firstUnit(&loop); unit != NULL; unit = player.nextUnit(&loop))
		if (!unit->isDelayedDeath() && unit->plot() != NULL && unit->getUnitInfo().IsMilitarySupport())
			++capturedCount;
	return player.getNumMilitaryUnits() - capturedCount;
}

// Captures the state-religion and secondary-pantheon heal contribution. The independent
// permanent pantheon stays separate so native healing can apply it without a state religion.
// The CPO branch resolves religion from a nearby city, so its per-player contribution stays zero.
int VoxRlReligionFriendlyHealChange(CvPlayer& player)
{
	if (!MOD_BALANCE_VP) return 0;
	CvCity* pCapital = player.getCapitalCity();
	if (pCapital == NULL) return 0;

	const PlayerTypes ePlayer = player.GetID();
	const ReligionTypes eApplicableReligion = player.GetReligions()->GetStateReligion();
	const BeliefTypes eSecondaryPantheon = eApplicableReligion != NO_RELIGION
		? pCapital->GetCityReligions()->GetSecondaryReligionPantheonBelief()
		: NO_BELIEF;

	int iExtraFriendlyHeal = 0;
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eApplicableReligion, ePlayer);
	if (pReligion)
	{
		iExtraFriendlyHeal += pReligion->m_Beliefs.GetFriendlyHealChange(ePlayer, pReligion->GetHolyCity());
		iExtraFriendlyHeal += eSecondaryPantheon != NO_BELIEF ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange() : 0;
	}

	return iExtraFriendlyHeal;
}

// Captures the independent permanent pantheon belief after native double-counting checks.
int VoxRlPermanentPantheonHealBelief(CvPlayer& player)
{
	if (!MOD_BALANCE_VP || !MOD_BALANCE_PERMANENT_PANTHEONS) return NO_BELIEF;
	CvCity* pCapital = player.getCapitalCity();
	if (pCapital == NULL) return NO_BELIEF;
	const PlayerTypes ePlayer = player.GetID();
	if (!GC.getGame().GetGameReligions()->HasCreatedPantheon(ePlayer)) return NO_BELIEF;
	const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, ePlayer);
	const BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(ePlayer);
	if (pPantheon == NULL || ePantheonBelief == NO_BELIEF) return NO_BELIEF;
	const ReligionTypes eReligion = player.GetReligions()->GetStateReligion();
	const BeliefTypes eSecondaryPantheon = eReligion != NO_RELIGION
		? pCapital->GetCityReligions()->GetSecondaryReligionPantheonBelief() : NO_BELIEF;
	if (ePantheonBelief == eSecondaryPantheon) return NO_BELIEF;
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, ePlayer);
	if (pReligion != NULL && pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eReligion, ePlayer))
		return NO_BELIEF;
	return ePantheonBelief;
}

// Collects one player row, including the military strategy used by purchases.
bool VoxRlCollectPlayerRecord(CvPlayer& player, PlayerTypes capturingPlayer, PlayerRecord& row)
{
	if (!CollectPlayerRecord(player, capturingPlayer, row)) return false;
	const int needAirStrategy = GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR", true);
	row.militaryNeedAirStrategy = needAirStrategy >= 0 && player.GetMilitaryAI()->IsUsingStrategy(static_cast<MilitaryAIStrategyTypes>(needAirStrategy));
	const int atWarStrategy = GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR", true);
	row.militaryAtWarStrategy = atWarStrategy >= 0 &&
		player.GetMilitaryAI()->IsUsingStrategy(static_cast<MilitaryAIStrategyTypes>(atWarStrategy)) ? 1 : 0;
	// Native doTurn gives every Found unit class the same getNewCityProductionValue amount, so
	// the first nonzero class cost is the player's current amount.
	for (int unitClass = 0; unitClass < GC.getNumUnitClassInfos(); ++unitClass)
	{
		const int extraCost = player.getUnitExtraCost(static_cast<UnitClassTypes>(unitClass));
		if (extraCost == 0) continue;
		row.unitExtraCost = extraCost;
		break;
	}
	return true;
}

// Reads the GOODY_GOLD row that the simulator pays for every goody hut. Native loads goody
// rows from the GoodyHuts table on demand, so no CvGlobals accessor exists. A missing row
// leaves the fields at zero.
static void CollectGoodyGoldRules(StaticRulesRecord& rules)
{
	Database::Results kGoody;
	if (!DB.SelectAt(kGoody, "GoodyHuts", "Type", "GOODY_GOLD") || !kGoody.Step()) return;
	rules.goodyGoldAmount = kGoody.GetInt("Gold");
	rules.goodyGoldNumRandRolls = kGoody.GetInt("NumGoldRandRolls");
	rules.goodyGoldRandAmount = kGoody.GetInt("GoldRandAmount");
}

// Records nonzero era operands without resolving a creation-time promotion or combat class.
static bool VoxRlCollectUnitEraRows(const CvUnitEntry& unit, int unitType, VoxRlStaticData& data)
{
	bool valid = true;
	for (int category = 0; category < GC.getNumUnitCombatClassInfos(); ++category)
		for (int era = 0; era < GC.getNumEraInfos(); ++era)
		{
			const int value = unit.GetUnitNewEraCombatType(category, era);
			if (value == 0) continue;
			UnitEraCombatRecord row;
			ZeroRecord(row);
			row.unitType = unitType;
			row.value = value;
			if (!AssignCheckedI16(row.unitCombat, category, "UnitEraCombatRecord", "unitCombat", 0) ||
				!AssignCheckedI16(row.era, era, "UnitEraCombatRecord", "era", 0)) valid = false;
			data.staticUnitEraCombats.push_back(row);
		}
	for (int category = 0; category < GC.getNumPromotionInfos(); ++category)
		for (int era = 0; era < GC.getNumEraInfos(); ++era)
		{
			const int value = unit.GetUnitNewEraPromotions(category, era);
			if (value == 0) continue;
			UnitEraPromotionRecord row;
			ZeroRecord(row);
			row.unitType = unitType;
			row.value = value;
			if (!AssignCheckedI16(row.promotion, category, "UnitEraPromotionRecord", "promotion", 0) ||
				!AssignCheckedI16(row.era, era, "UnitEraPromotionRecord", "era", 0)) valid = false;
			data.staticUnitEraPromotions.push_back(row);
		}
	return valid;
}

// Captures every raw promotion plague and incompatibility row in native container order.
static bool VoxRlCollectPromotionChildRows(VoxRlStaticData& data)
{
	bool valid = true;
	for (int promotion = 0; promotion < GC.getNumPromotionInfos(); ++promotion)
	{
		const CvPromotionEntry* info = GC.getPromotionInfo(static_cast<PromotionTypes>(promotion));
		if (info == NULL) continue;
		const std::vector<PlagueInfo> plagues = info->GetPlagues();
		for (size_t index = 0; index < plagues.size(); ++index)
		{
			const PlagueInfo& native = plagues[index];
			PromotionPlagueRecord row;
			ZeroRecord(row);
			if (!AssignCheckedI16(row.promotion, promotion,
				"PromotionPlagueRecord", "promotion", 0)) valid = false;
			if (!AssignCheckedI16(row.plague, static_cast<int>(native.ePlague),
				"PromotionPlagueRecord", "plague", 0)) valid = false;
			if (!AssignCheckedI16(row.domain, static_cast<int>(native.eDomain),
				"PromotionPlagueRecord", "domain", 0)) valid = false;
			row.setOnAttack(native.bApplyOnAttack);
			row.setOnDefense(native.bApplyOnDefense);
			VoxRlAssignClamped(row.chance, native.iApplyChance);
			data.staticPromotionPlagues.push_back(row);
		}
		const std::set<int> blockedPromotions = info->GetBlockedPromotions();
		for (std::set<int>::const_iterator blocked = blockedPromotions.begin();
			blocked != blockedPromotions.end(); ++blocked)
		{
			PromotionBlockedPromotionRecord row;
			ZeroRecord(row);
			if (!AssignCheckedI16(row.promotion, promotion,
				"PromotionBlockedPromotionRecord", "promotion", 0)) valid = false;
			if (!AssignCheckedI16(row.blockedPromotion, *blocked,
				"PromotionBlockedPromotionRecord", "blockedPromotion", 0)) valid = false;
			data.staticPromotionBlockedPromotions.push_back(row);
		}
	}
	return valid;
}

// Captures type names once alongside the existing indexed promotion info table.
static bool VoxRlCollectPromotionTypeRows(VoxRlStaticData& data)
{
	bool valid = true;
	// Preserve promotion identities once; consumers resolve names to the existing info rows.
	for (int promotion = 0; promotion < GC.getNumPromotionInfos(); ++promotion)
	{
		const CvPromotionEntry* info = GC.getPromotionInfo(static_cast<PromotionTypes>(promotion));
		if (info == NULL) continue;
		const char* name = info->GetType();
		if (name == NULL || name[0] == '\0') return false;
		const size_t nameLength = std::strlen(name);
		if (nameLength > 65535U) return false;
		PromotionTypeRecord row;
		ZeroRecord(row);
		if (!AssignCheckedI16(row.promotion, promotion, "PromotionTypeRecord", "promotion", 0)) valid = false;
		row.nameOffset = static_cast<u32>(data.staticPromotionTypeNames.size());
		row.nameLength = static_cast<u16>(nameLength);
		data.staticPromotionTypeNames.insert(data.staticPromotionTypeNames.end(), name, name + nameLength);
		data.staticPromotionTypes.push_back(row);
	}
	return valid;
}

// Captures unit combat type names so simulator name lookups resolve to native indices.
static bool VoxRlCollectUnitCombatTypeRows(VoxRlStaticData& data)
{
	bool valid = true;
	for (int unitCombat = 0; unitCombat < GC.getNumUnitCombatClassInfos(); ++unitCombat)
	{
		const CvBaseInfo* info = GC.getUnitCombatClassInfo(static_cast<UnitCombatTypes>(unitCombat));
		const char* name = info != NULL ? info->GetType() : NULL;
		if (name == NULL || name[0] == '\0') return false;
		const size_t nameLength = std::strlen(name);
		if (nameLength > 65535U) return false;
		UnitCombatTypeRecord row;
		ZeroRecord(row);
		if (!AssignCheckedI16(row.unitCombat, unitCombat, "UnitCombatTypeRecord", "unitCombat", 0)) valid = false;
		row.nameOffset = static_cast<u32>(data.staticUnitCombatTypeNames.size());
		row.nameLength = static_cast<u16>(nameLength);
		data.staticUnitCombatTypeNames.insert(data.staticUnitCombatTypeNames.end(), name, name + nameLength);
		data.staticUnitCombatTypes.push_back(row);
	}
	return valid;
}

// Collects one player's values that change only at rare events (trait, policy, and leader
// changes): its STATIC player row plus sparse unit class and unit combat rows. The extra cost
// amount changes every turn, so a class row only flags it and PlayerRecord carries the amount.
bool VoxRlCollectPlayerMapRows(PlayerTypes eIndex, VoxRlPlayerMapRows& rows)
{
	bool valid = true;
	const int playerIndex = static_cast<int>(eIndex);
	CvPlayerAI& player = GET_PLAYER(eIndex);
	ZeroRecord(rows.player);
	rows.player.player = static_cast<i8>(playerIndex);
	if (!CollectStaticPlayerRecord(player, rows.player)) valid = false;
	rows.unitClasses.clear();
	rows.unitCombatCosts.clear();
	for (int unitClass = 0; unitClass < GC.getNumUnitClassInfos(); ++unitClass)
	{
		const UnitClassTypes type = static_cast<UnitClassTypes>(unitClass);
		const int replacement = player.GetUnitClassReplacement(type);
		const int extraCost = player.getUnitExtraCost(type);
		if (replacement == NO_UNITCLASS && extraCost == 0) continue;
		StaticPlayerUnitClassRecord classRow;
		ZeroRecord(classRow);
		classRow.player = static_cast<i8>(playerIndex);
		if (!AssignCheckedI16(classRow.unitClass, unitClass,
			"StaticPlayerUnitClassRecord", "unitClass", 0) ||
			!AssignCheckedI16(classRow.replacementClass, replacement,
			"StaticPlayerUnitClassRecord", "replacementClass")) valid = false;
		classRow.hasExtraCost = extraCost != 0 ? 1 : 0;
		rows.unitClasses.push_back(classRow);
	}
	CvPlayerTraits* traits = player.GetPlayerTraits();
	for (int unitCombat = 0; unitCombat < GC.getNumUnitCombatClassInfos(); ++unitCombat)
	{
		const std::pair<int, bool> modifier = traits->GetUnitCombatProductionCostModifier(
			static_cast<UnitCombatTypes>(unitCombat));
		if (modifier.first == 0) continue;
		StaticPlayerUnitCombatCostRecord combatRow;
		ZeroRecord(combatRow);
		combatRow.player = static_cast<i8>(playerIndex);
		if (!AssignCheckedI16(combatRow.unitCombat, unitCombat,
			"StaticPlayerUnitCombatCostRecord", "unitCombat", 0)) valid = false;
		VoxRlAssignClamped(combatRow.modifier, modifier.first);
		combatRow.goldenAgeOnly = modifier.second ? 1 : 0;
		rows.unitCombatCosts.push_back(combatRow);
	}
	return valid;
}

// Reports whether two player maps hold the same rows.
bool VoxRlPlayerMapRowsEqual(const VoxRlPlayerMapRows& left, const VoxRlPlayerMapRows& right)
{
	return std::memcmp(&left.player, &right.player, sizeof(left.player)) == 0 &&
		VoxRlSameRows(left.unitClasses, right.unitClasses) && VoxRlSameRows(left.unitCombatCosts, right.unitCombatCosts);
}

// Appends one player's complete map to a REQUEST as its replacement. The REQUEST mirrors
// share the STATIC row layouts.
void VoxRlAppendPlayerMapReplacement(const VoxRlPlayerMapRows& rows, VoxRlRequestData& data)
{
	RequestPlayerMapPlayerRecord player;
	std::memcpy(&player, &rows.player, sizeof(player));
	data.requestPlayerMapPlayers.push_back(player);
	for (size_t index = 0; index < rows.unitClasses.size(); ++index)
	{
		RequestPlayerMapUnitClassRecord row;
		std::memcpy(&row, &rows.unitClasses[index], sizeof(row));
		data.requestPlayerMapUnitClasses.push_back(row);
	}
	for (size_t index = 0; index < rows.unitCombatCosts.size(); ++index)
	{
		RequestPlayerMapUnitCombatCostRecord row;
		std::memcpy(&row, &rows.unitCombatCosts[index], sizeof(row));
		data.requestPlayerMapUnitCombatCosts.push_back(row);
	}
}

// Appends one player's map replacement when its current map differs from the cached one, and
// advances the cache to the current map.
bool VoxRlAppendChangedPlayerMap(PlayerTypes player, std::map<int, VoxRlPlayerMapRows>& cache,
	VoxRlRequestData& data)
{
	VoxRlPlayerMapRows current;
	if (!VoxRlCollectPlayerMapRows(player, current)) return false;
	const int key = static_cast<int>(player);
	const std::map<int, VoxRlPlayerMapRows>::const_iterator cached = cache.find(key);
	if (cached != cache.end() && VoxRlPlayerMapRowsEqual(cached->second, current)) return true;
	VoxRlAppendPlayerMapReplacement(current, data);
	cache[key] = current;
	return true;
}

// Selects the cached player maps that differ from the maps STATIC holds, for WORLD overrides.
void VoxRlCollectPlayerMapOverrides(const std::map<int, VoxRlPlayerMapRows>& staticMaps,
	const std::map<int, VoxRlPlayerMapRows>& currentMaps, std::vector<VoxRlPlayerMapRows>& overrides)
{
	overrides.clear();
	for (std::map<int, VoxRlPlayerMapRows>::const_iterator current = currentMaps.begin(); current != currentMaps.end(); ++current)
	{
		const std::map<int, VoxRlPlayerMapRows>::const_iterator written = staticMaps.find(current->first);
		if (written == staticMaps.end() || !VoxRlPlayerMapRowsEqual(written->second, current->second))
			overrides.push_back(current->second);
	}
}

// Collects the player map of every player slot for STATIC: one row per slot plus sparse
// unit class and unit combat rows.
static bool VoxRlCollectStaticPlayerMap(VoxRlStaticData& data, std::vector<VoxRlPlayerMapRows>& playerMaps)
{
	bool valid = true;
	playerMaps.clear();
	for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
	{
		VoxRlPlayerMapRows rows;
		if (!VoxRlCollectPlayerMapRows(static_cast<PlayerTypes>(playerIndex), rows)) valid = false;
		data.staticPlayers.push_back(rows.player);
		data.staticPlayerUnitClasses.insert(data.staticPlayerUnitClasses.end(), rows.unitClasses.begin(), rows.unitClasses.end());
		data.staticPlayerUnitCombatCosts.insert(data.staticPlayerUnitCombatCosts.end(), rows.unitCombatCosts.begin(), rows.unitCombatCosts.end());
		playerMaps.push_back(rows);
	}
	return valid;
}

// Collects native tables, topology, and the player map for the generated STATIC builder.
bool VoxRlBuildStaticBlock(const VoxRlBlockIdentity& identity,
	VoxRlOwnedBlockStorage& storage, unsigned int& length, std::vector<VoxRlPlayerMapRows>& playerMaps)
{
	bool valid = true;
	VoxRlStaticData data;
	if (!VoxRlCollectStaticPlayerMap(data, playerMaps)) valid = false;
	ZeroRecord(data.staticRules);
	if (!CollectStaticRulesRecord(data.staticRules)) valid = false;
	data.staticRules.modAiUnitProduction = MOD_AI_UNIT_PRODUCTION ? 1 : 0;
	CollectGoodyGoldRules(data.staticRules);
	// Stores each native mission identity at its shared STATIC slot.
#define VOX_RL_CAPTURE_MISSION_IDENTITY(name) \
	if (!AssignCheckedI16(data.staticRules.missionTypes[kMissionIdentity_##name], \
		static_cast<int>(CvTypes::getMISSION_##name()), "StaticRulesRecord", "missionTypes")) valid = false;
	VOX_RL_MISSION_IDENTITIES(VOX_RL_CAPTURE_MISSION_IDENTITY)
#undef VOX_RL_CAPTURE_MISSION_IDENTITY
	for (int flavor = 0; flavor < GC.getNumFlavorTypes(); ++flavor)
	{
		const CvString& name = GC.getFlavorTypes(static_cast<FlavorTypes>(flavor));
		StaticFlavorInfoRecord row;
		ZeroRecord(row);
		if (!AssignCheckedI16(row.flavorId, flavor, "StaticFlavorInfoRecord", "flavorId", 0)) valid = false;
		row.nameOffset = static_cast<u32>(data.staticFlavorNames.size());
		if (name.length() > 65535U) return false;
		row.nameLength = static_cast<u16>(name.length());
		data.staticFlavorNames.insert(data.staticFlavorNames.end(), name.begin(), name.end());
		data.staticFlavorInfos.push_back(row);
	}
	// City-state personalities are randomized at game start and never change, and native
	// grand strategy runs only for majors, so STATIC carries every city-state's flavors once.
	for (int playerIndex = MAX_MAJOR_CIVS; playerIndex < MAX_CIV_PLAYERS; ++playerIndex)
	{
		CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(playerIndex));
		if (!player.isAlive() || !player.isMinorCiv()) continue;
		for (int flavor = 0; flavor < GC.getNumFlavorTypes(); ++flavor)
		{
			StaticMinorFlavorRecord row;
			ZeroRecord(row);
			row.player = static_cast<i8>(playerIndex);
			if (!AssignCheckedI16(row.flavorId, flavor, "StaticMinorFlavorRecord", "flavorId", 0)) valid = false;
			VoxRlAssignClamped(row.value, player.GetFlavorManager()->GetPersonalityIndividualFlavor(static_cast<FlavorTypes>(flavor)));
			VoxRlAssignClamped(row.personalityAndGrandStrategyValue,
				player.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy(static_cast<FlavorTypes>(flavor)));
			data.staticMinorFlavors.push_back(row);
		}
	}
	if (!VoxRlCollectNativeInfoTables(data)) valid = false;
	if (!VoxRlCollectPromotionTypeRows(data)) valid = false;
	if (!VoxRlCollectPromotionChildRows(data)) valid = false;
	if (!VoxRlCollectUnitCombatTypeRows(data)) valid = false;
	ZeroRecord(data.staticBuildIds);
	if (!CollectStaticBuildIdsRecord(data.staticBuildIds)) valid = false;
	// Plot indices travel as signed sixteen-bit wire values, so only maps of one through
	// 32768 plots can record. The wide product rejects bad dimensions before any
	// plot-indexed data is collected, and the same rule is rechecked on every load.
	{
		const int gridWidth = GC.getMap().getGridWidth();
		const int gridHeight = GC.getMap().getGridHeight();
		if (gridWidth <= 0 || gridHeight <= 0 ||
			static_cast<unsigned __int64>(gridWidth) * static_cast<unsigned __int64>(gridHeight) > 32768ULL) return false;
	}
	ZeroRecord(data.staticMapTopology);
	if (!CollectMapTopologyRecord(GC.getMap(), data.staticMapTopology)) valid = false;
	for (int index = 0; index < GC.getMap().numPlots(); ++index)
	{
		CvPlot* plot = GC.getMap().plotByIndex(index);
		if (plot == NULL) return false;
		PlotTopologyRecord row;
		ZeroRecord(row);
		if (!CollectPlotTopologyRecord(*plot, row)) valid = false;
		data.staticPlotTopology.push_back(row);
	}
	for (int formationIndex = 0; formationIndex < GC.getNumMultiUnitFormationInfos(); ++formationIndex)
	{
		CvMultiUnitFormationInfo* formation = GC.getMultiUnitFormationInfo(formationIndex);
		if (formation == NULL) continue;
		FormationInfoRecord row;
		ZeroRecord(row);
		row.formationType = formationIndex;
		std::vector<FormationSlotInfoRecord> slots;
		for (size_t slotIndex = 0; slotIndex < formation->getNumFormationSlotEntries(); ++slotIndex)
		{
			if (slotIndex > 32767U) return false;
			const CvFormationSlotEntry& native = formation->getFormationSlotEntry(slotIndex);
			FormationSlotInfoRecord slot;
			ZeroRecord(slot);
			slot.formationType = formationIndex;
			slot.slotIndex = static_cast<i16>(slotIndex);
			slot.primaryUnitAi = static_cast<i8>(native.m_primaryUnitType);
			slot.secondaryUnitAi = static_cast<i8>(native.m_secondaryUnitType);
			slot.required = native.m_requiredSlot ? 1 : 0;
			slots.push_back(slot);
		}
		if (!AppendFormationInfoRecordSlotRange(&row, &data, slots)) valid = false;
		data.staticFormationInfos.push_back(row);
	}
	for (int unitType = 0; unitType < GC.getNumUnitInfos(); ++unitType)
	{
		CvUnitEntry* unit = GC.getUnitInfo(static_cast<UnitTypes>(unitType));
		if (unit == NULL) continue;
		if (!VoxRlCollectUnitEraRows(*unit, unitType, data)) valid = false;
		for (int resourceType = 0; resourceType < GC.getNumResourceInfos(); ++resourceType)
		{
			const int quantity = unit->GetResourceQuantityRequirement(resourceType);
			if (quantity == 0) continue;
			UnitResourceRequirementRecord row;
			ZeroRecord(row);
			row.unitType = unitType;
			if (!AssignCheckedI16(row.resourceType, resourceType,
				"UnitResourceRequirementRecord", "resourceType", 0)) valid = false;
			if (!AssignCheckedI16(row.quantity, quantity,
				"UnitResourceRequirementRecord", "quantity", 0)) valid = false;
			data.staticUnitResourceRequirements.push_back(row);
		}
	}
	for (int improvementType = 0; improvementType < GC.getNumImprovementInfos(); ++improvementType)
	{
		CvImprovementEntry* improvement = GC.getImprovementInfo(static_cast<ImprovementTypes>(improvementType));
		if (improvement == NULL) continue;
		for (int resourceType = 0; resourceType < GC.getNumResourceInfos(); ++resourceType)
		{
			if (!improvement->IsConnectsResource(resourceType)) continue;
			ImprovementResourceCompatibilityRecord row;
			ZeroRecord(row);
			if (!AssignCheckedI16(row.improvementType, improvementType,
				"ImprovementResourceCompatibilityRecord", "improvementType", 0)) valid = false;
			if (!AssignCheckedI16(row.resourceType, resourceType,
				"ImprovementResourceCompatibilityRecord", "resourceType", 0)) valid = false;
			data.staticImprovementResourceCompatibilities.push_back(row);
		}
	}
	if (!valid) return false;
	return data.Write(identity, storage, length);
}

// Collects the native zone table and assignments without advancing its lifecycle.
bool VoxRlCollectZones(CvTacticalAnalysisMap* zoneMap, VoxRlZoneSnapshot& snapshot)
{
	bool valid = true;
	snapshot.zones.clear();
	snapshot.neighbors.clear();
	const int zoneCount = zoneMap != NULL ? zoneMap->GetNumZonesWithoutRefresh() : 0;
	for (int zoneIndex = 0; zoneIndex < zoneCount; ++zoneIndex)
	{
		const CvTacticalDominanceZone* zone = zoneMap->GetZoneByIndexWithoutRefresh(zoneIndex);
		if (zone == NULL)
		{
			valid = false;
			continue;
		}
		ZoneRecord row;
		ZeroRecord(row);
		if (!CollectZoneRecord(*zone, row)) valid = false;
		row.avgX = static_cast<i32>(zone->GetAverageX());
		row.avgY = static_cast<i32>(zone->GetAverageY());
		CvCity* city = zone->GetZoneCity();
		row.cityOwner = city != NULL ? static_cast<i8>(city->getOwner()) : static_cast<i8>(-1);
		row.cityId = city != NULL ? static_cast<i32>(city->GetID()) : -1;
		const std::vector<int>& nativeNeighbors = zone->GetNeighboringZones();
		std::vector<ZoneNeighborRecord> neighbors;
		for (size_t index = 0; index < nativeNeighbors.size(); ++index)
		{
			ZoneNeighborRecord entry;
			ZeroRecord(entry);
			entry.zoneId = static_cast<i32>(nativeNeighbors[index]);
			neighbors.push_back(entry);
		}
		row.neighborRangeFirst = static_cast<u32>(snapshot.neighbors.size());
		row.neighborRangeCount = static_cast<u32>(neighbors.size());
		snapshot.neighbors.insert(snapshot.neighbors.end(), neighbors.begin(), neighbors.end());
		snapshot.zones.push_back(row);
	}
	const int plotCount = GC.getMap().numPlots();
	snapshot.plotZones.resize(plotCount);
	for (int plot = 0; plot < plotCount; ++plot)
		snapshot.plotZones[plot] = zoneMap != NULL ? zoneMap->GetDominanceZoneIDWithoutRefresh(plot) : -1;
	return valid;
}

// Collects the three native espionage sight inputs for every watched city and each spying
// player in [firstSpyOwner, endSpyOwner): established surveillance per spy, the revealed city
// screen, and the passive vision bonus. The simulator rebuilds the lit plots from these rows
// with extracted changeEspionageSight.
static bool VoxRlCollectCityEspionageSightRows(std::vector<CityEspionageSightRecord>& rows,
	int firstSpyOwner, int endSpyOwner)
{
	bool valid = true;
	// Counts established spies per watched city and spying player.
	std::map<std::pair<VoxRlCityKey, int>, int> surveillance;
	for (int spyOwner = firstSpyOwner; spyOwner < endSpyOwner; ++spyOwner)
	{
		CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(spyOwner));
		CvPlayerEspionage* espionage = player.isAlive() ? player.GetEspionage() : NULL;
		if (espionage == NULL) continue;
		for (int spy = 0; spy < espionage->GetNumSpies(); ++spy)
		{
			if (!espionage->HasEstablishedSurveillance(static_cast<uint>(spy))) continue;
			CvCity* city = espionage->GetCityWithSpy(static_cast<uint>(spy));
			if (city == NULL) continue;
			++surveillance[std::make_pair(VoxRlCityKey(city->getOwner(), city->GetID()), spyOwner)];
		}
	}
	for (int cityOwner = 0; cityOwner < MAX_PLAYERS; ++cityOwner)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(cityOwner));
		if (!owner.isAlive()) continue;
		int loop = 0;
		for (CvCity* city = owner.firstCity(&loop); city != NULL; city = owner.nextCity(&loop))
		{
			CvCityEspionage* cityEspionage = city->GetCityEspionage();
			if (cityEspionage == NULL) continue;
			for (int spyOwner = firstSpyOwner; spyOwner < endSpyOwner; ++spyOwner)
			{
				const std::map<std::pair<VoxRlCityKey, int>, int>::const_iterator count = surveillance.find(
					std::make_pair(VoxRlCityKey(cityOwner, city->GetID()), spyOwner));
				const int surveillanceCount = count != surveillance.end() ? count->second : 0;
				const PlayerTypes spyPlayer = static_cast<PlayerTypes>(spyOwner);
				const bool revealCityScreen = cityEspionage->GetRevealCityScreen(spyPlayer);
				const int visionBonus = cityEspionage->GetVisionBonus(spyPlayer);
				if (surveillanceCount == 0 && !revealCityScreen && visionBonus <= 0) continue;
				CityEspionageSightRecord row;
				ZeroRecord(row);
				row.cityOwner = static_cast<i8>(cityOwner);
				row.cityId = city->GetID();
				row.spyOwner = static_cast<i8>(spyOwner);
				row.surveillanceCount = static_cast<i8>((std::min)(surveillanceCount, 127));
				row.revealCityScreen = revealCityScreen ? 1 : 0;
				// Native ring tables stop far below the eight-bit limit, so a larger bonus is a capture fault.
				if (visionBonus > 127)
				{
					VoxRlNoteCaptureRangeFailure("range", "CityEspionageSightRecord", "visionBonus", visionBonus, 0, 127);
					valid = false;
				}
				row.visionBonus = static_cast<i8>((std::max)((std::min)(visionBonus, 127), 0));
				rows.push_back(row);
			}
		}
	}
	return valid;
}

// Collects the espionage sight rows of one spying player. Only major civilizations run spies.
bool VoxRlCollectSpyOwnerEspionageRows(PlayerTypes spyOwner, std::vector<CityEspionageSightRecord>& rows)
{
	rows.clear();
	if (spyOwner < 0 || spyOwner >= MAX_MAJOR_CIVS) return true;
	return VoxRlCollectCityEspionageSightRows(rows, static_cast<int>(spyOwner), static_cast<int>(spyOwner) + 1);
}

// Appends one spying player's espionage sight replacement when its current rows differ
// from the baseline, and advances the baseline to the current rows.
bool VoxRlAppendEspionageSightReplacement(PlayerTypes spyOwner,
	std::vector<CityEspionageSightRecord>& baseline, VoxRlRequestData& data)
{
	std::vector<CityEspionageSightRecord> current;
	if (!VoxRlCollectSpyOwnerEspionageRows(spyOwner, current)) return false;
	return AppendChangedChildFamily(baseline, current, std::vector<int>(1, static_cast<int>(spyOwner)),
		data.requestCityEspionageSightReplacements, data, &AppendRequestCityEspionageSightReplacementRecordRowRange);
}

// Collects one player's positive cached found values in plot order. The read never refreshes the
// native lazy cache, because an early refresh would change which values native later reads.
bool VoxRlCollectPlayerFoundValueRows(PlayerTypes player, std::vector<PlayerFoundValueRecord>& rows)
{
	rows.clear();
	if (player < 0 || player >= MAX_PLAYERS) return true;
	const std::vector<int>& values = GET_PLAYER(player).VoxRlGetPlotFoundValues();
	for (size_t index = 0; index < values.size(); ++index)
	{
		if (values[index] <= 0) continue;
		PlayerFoundValueRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(player);
		if (!AssignCheckedI16(row.plotIndex, static_cast<int>(index),
			"PlayerFoundValueRecord", "plotIndex", 0)) return false;
		row.value = values[index];
		rows.push_back(row);
	}
	return true;
}

// Appends one player's found-value replacement when its current rows differ from the baseline,
// and advances the baseline to the current rows.
bool VoxRlAppendFoundValueReplacement(PlayerTypes player,
	std::vector<PlayerFoundValueRecord>& baseline, VoxRlRequestData& data)
{
	std::vector<PlayerFoundValueRecord> current;
	if (!VoxRlCollectPlayerFoundValueRows(player, current)) return false;
	return AppendChangedChildFamily(baseline, current, std::vector<int>(1, static_cast<int>(player)),
		data.requestPlayerFoundValueReplacements, data, &AppendRequestPlayerFoundValueReplacementRecordRowRange);
}

// Collects native WORLD state in stable owner and plot order for the generated writer.
bool VoxRlBuildWorldBlock(const VoxRlBlockIdentity& identity, PlayerTypes capturingPlayer,
	VoxRlOwnedBlockStorage& storage, unsigned int& length, VoxRlZoneSnapshot& zones,
	std::vector<TeamPassabilityRecord>& teamPassabilitySnapshot,
	std::vector<TeamResourceRecord>& teamResourceSnapshot,
	const std::vector<VoxRlPlayerMapRows>& playerMapOverrides, VoxRlWorldBuildTimings* timings,
	VoxRlPlayerCitySnapshot* playerCitySnapshot)
{
	bool valid = true;
	CvMap& map = GC.getMap();
	const int plotCount = map.numPlots();
	// The world build rechecks the map bound from the static generation, because capture
	// fails before any plot-indexed row is collected when the map exceeds the wire width.
	if (plotCount <= 0 || plotCount > 32768) return false;
	CvPlayerAI& capturing = GET_PLAYER(capturingPlayer);
	const TeamTypes capturingTeam = capturing.getTeam();
	VoxRlWorldData data;
	data.worldGameState.gameState = static_cast<i32>(GC.getGame().getGameState());
	VoxRlAssignClamped(data.worldGameState.elapsedGameTurns, GC.getGame().getGameTurn());
	VoxRlAssignClamped(data.worldGameState.maxTurns, GC.getGame().getMaxTurns());
	VoxRlAssignClamped(data.worldGameState.currentEra, GC.getGame().getCurrentEra());
	data.worldGameState.activePlayer = static_cast<i8>(GC.getGame().getActivePlayer());
	std::vector<TeamTypes> aliveTeams;
	CollectAliveTeams(aliveTeams);
	std::set<int> recordedTeams;
	for (int team = 0; team < MAX_TEAMS; ++team)
	{
		if (GET_TEAM(static_cast<TeamTypes>(team)).isAlive() || team == BARBARIAN_TEAM)
			recordedTeams.insert(team);
	}
	std::vector<PlayerTypes> alivePlayers;
	CollectAlivePlayers(alivePlayers);
	if (timings != NULL)
	{
		timings->plotCount = static_cast<unsigned int>(plotCount);
		timings->alivePlayerCount = static_cast<unsigned int>(alivePlayers.size());
		timings->aliveTeamCount = static_cast<unsigned int>(aliveTeams.size());
	}

	// Unit rows follow plot stacks, while these indices preserve owner iteration order.
	std::map<VoxRlEntityKey, unsigned int> iterationByUnit;
	ScopedWorldTiming ownerIterationTiming(timings != NULL ? &timings->ownerIterationNs : NULL);
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		unsigned int iteration = 0;
		int loop = 0;
		for (CvUnit* unit = owner.firstUnit(&loop); unit != NULL; unit = owner.nextUnit(&loop))
		{
			if (unit->isDelayedDeath() || unit->plot() == NULL) continue;
			iterationByUnit[VoxRlEntityKey(player, unit->GetID())] = iteration++;
		}
	}
	ownerIterationTiming.Stop();
	ScopedWorldTiming dangerSparseTiming(timings != NULL ? &timings->dangerSparseRelationsNs : NULL);
	const CvDangerPlots* danger = capturing.GetDangerPlots();
	if (danger == NULL) return false;
	DangerPlayerRecord dangerRow;
	ZeroRecord(dangerRow);
	if (!CollectDangerPlayerRecord(capturingPlayer, dangerRow)) valid = false;
	if (!AssignCheckedI16(dangerRow.turnBuilt, danger->GetTurnBuilt(),
		"DangerPlayerRecord", "turnBuilt", -1)) valid = false;
	dangerRow.dirty = danger->IsDirty() ? 1 : 0;
	std::vector<KnownAttackerRecord> known;
	for (UnitSet::const_iterator entry = danger->GetKnownUnits().begin(); entry != danger->GetKnownUnits().end(); ++entry)
	{
		KnownAttackerRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(entry->first);
		row.unitId = static_cast<i32>(entry->second);
		known.push_back(row);
	}
	std::vector<VanishedAttackerRecord> vanished;
	for (UnitSet::const_iterator entry = danger->GetVanishedUnits().begin(); entry != danger->GetVanishedUnits().end(); ++entry)
	{
		VanishedAttackerRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(entry->first);
		row.unitId = static_cast<i32>(entry->second);
		vanished.push_back(row);
	}
	if (!AppendDangerPlayerRecordKnownUnitRange(&dangerRow, &data, known)) valid = false;
	if (!AppendDangerPlayerRecordVanishedUnitRange(&dangerRow, &data, vanished)) valid = false;
	data.worldDangerPlayers.push_back(dangerRow);

	VoxRlCollectAllUnitPlagueRows(data.worldUnitPlagues, data.worldUnitBlockedPromotions);
	if (!VoxRlCollectAllUnitTurnRows(data.worldUnitMissions, data.worldUnitPromotionTurns)) valid = false;
	VoxRlCollectAllUnitAttackCountRows(data.worldUnitAttackCounts);
	VoxRlCollectAllCityAttackCountRows(data.worldCityAttackCounts);
	VoxRlCollectAllPlayerResistanceRows(data.worldPlayerResistances);
	for (size_t minorIndex = 0; minorIndex < alivePlayers.size(); ++minorIndex)
	{
		CvPlayerAI& minor = GET_PLAYER(alivePlayers[minorIndex]);
		if (!minor.isMinorCiv() || minor.isBarbarian()) continue;
		for (size_t majorIndex = 0; majorIndex < alivePlayers.size(); ++majorIndex)
		{
			CvPlayerAI& major = GET_PLAYER(alivePlayers[majorIndex]);
			if (major.isMinorCiv() || major.isBarbarian()) continue;
			MinorRelationRecord row;
			ZeroRecord(row);
			if (!CollectMinorRelationRecord(minor, *minor.GetMinorCivAI(), minor.GetID(), major.GetID(), row)) valid = false;
			data.worldMinorRelations.push_back(row);
		}
	}
	dangerSparseTiming.Stop();

	std::map<int, unsigned int> zoneRowByZoneId;
	ScopedWorldTiming zoneTiming(timings != NULL ? &timings->zoneNs : NULL);
	CvTacticalAnalysisMap* zoneMap = capturing.GetTacticalAI()->GetTacticalAnalysisMap();
	// Zones are the checkpoint table. The no-refresh accessors never trigger
	// the native rebuild, so capture cannot fire the tactical-time zone
	// recompute ahead of its normal schedule.
	if (!VoxRlCollectZones(zoneMap, zones)) valid = false;
	data.worldZones = zones.zones;
	data.worldZoneNeighbors = zones.neighbors;
	// Each plot's zone membership is encoded as a one-based index into the ordered table.
	// The 16-bit form covers tables up to 65,535 rows; larger tables take the wide form.
	for (size_t zoneIndex = 0; zoneIndex < zones.zones.size(); ++zoneIndex)
		zoneRowByZoneId[zones.zones[zoneIndex].zoneId] = static_cast<unsigned int>(zoneIndex + 1);
	zoneTiming.Stop();
	const bool wideZoneIndices = zones.zones.size() > 65535U;
	ScopedWorldTiming plotUnitTiming(timings != NULL ? &timings->plotUnitNs : NULL);
	std::vector<PlotCaptureRecord> plotCaptures;
	plotCaptures.reserve(static_cast<size_t>(plotCount));
	std::vector<bool> validPlots;
	validPlots.reserve(static_cast<size_t>(plotCount));
	bool allPlotsValid = true;
	for (int plotIndex = 0; plotIndex < plotCount; ++plotIndex)
	{
		CvPlot* plot = map.plotByIndex(plotIndex);
		if (plot == NULL) return false;
		// The full-width record is collected first; the compact core row, the counter
		// rows, and the city tokens are encoded from it after every plot is collected.
		PlotCaptureRecord row;
		ZeroRecord(row);
		const bool plotValid = VoxRlCollectPlotRecord(*plot, row);
		if (!plotValid)
		{
			valid = false;
			allPlotsValid = false;
		}
		plotCaptures.push_back(row);
		validPlots.push_back(plotValid);
		const i32 plotZone = zones.plotZones[plotIndex];
		unsigned int zoneTableIndex = 0;
		if (plotZone != -1)
		{
			std::map<int, unsigned int>::const_iterator zoneRow = zoneRowByZoneId.find(plotZone);
			if (zoneRow == zoneRowByZoneId.end()) valid = false;
			else zoneTableIndex = zoneRow->second;
		}
		if (wideZoneIndices)
		{
			PlotZoneIndexWideRecord indexRow;
			ZeroRecord(indexRow);
			indexRow.zoneTableIndex = zoneTableIndex;
			data.worldPlotZonesWide.push_back(indexRow);
		}
		else
		{
			PlotZoneIndexRecord indexRow;
			ZeroRecord(indexRow);
			indexRow.zoneTableIndex = static_cast<u16>(zoneTableIndex);
			data.worldPlotZones.push_back(indexRow);
		}
		std::vector<UnitWireRecord> units;
		// The default plot node chain omits trade and other managed layers.
		// WORLD ranges retain every physical unit so owner iteration and later
		// deltas remain complete while the simulator filters tactical occupancy.
		const int unitCount = plot->getNumLayerUnits();
		for (int unitIndex = 0; unitIndex < unitCount; ++unitIndex)
		{
			CvUnit* unit = plot->getLayerUnit(unitIndex);
			if (unit == NULL || unit->isDelayedDeath()) continue;
			UnitRecord unitRow;
			ZeroRecord(unitRow);
			if (!VoxRlCollectUnitRecord(*unit, capturingTeam, unitRow))
			{
				valid = false;
				continue;
			}
			std::map<VoxRlEntityKey, unsigned int>::const_iterator iteration =
				iterationByUnit.find(VoxRlEntityKey(static_cast<int>(unit->getOwner()), unit->GetID()));
			if (iteration == iterationByUnit.end()) return false;
			unitRow.iterationIndex = iteration->second;
			// The full record splits into the wire row and its sparse rows; both child
			// ranges append through their generated helpers in captured unit order.
			UnitWireRecord wireRow;
			std::vector<UnitSparseFieldRecord> sparseRows;
			VoxRlFieldRangeFailure splitFailure;
			if (!VoxRlSplitUnitRecord(unitRow, wireRow, sparseRows, &splitFailure))
			{
				VoxRlNoteSplitFailure(splitFailure);
				valid = false;
				continue;
			}
			if (!AppendUnitWireRecordSparseFieldRange(&wireRow, &data, sparseRows)) valid = false;
			units.push_back(wireRow);
		}
		// Unit rows append in captured plot order; the loader reconstructs each
		// plot's packed range from this sequence.
		data.worldUnits.insert(data.worldUnits.end(), units.begin(), units.end());
	}
	// The city reference table covers every non-null owning and effective owning city in
	// the block, then each plot's core row and optional counter row encode against it.
	std::vector<CityReferenceRecord> cityReferences;
	std::map<VoxRlCityKey, unsigned int> tokenByCity;
	// Invalid plot records keep their indices but never enter the plot codecs.
	std::vector<PlotCaptureRecord> validPlotCaptures;
	if (!allPlotsValid)
	{
		for (int plotIndex = 0; plotIndex < plotCount; ++plotIndex)
			if (validPlots[plotIndex]) validPlotCaptures.push_back(plotCaptures[plotIndex]);
	}
	const bool cityReferencesValid = VoxRlBuildCityReferenceTable(
		allPlotsValid ? plotCaptures : validPlotCaptures, &cityReferences, &tokenByCity);
	if (!cityReferencesValid) valid = false;
	data.worldCityReferences = cityReferences;
	for (int plotIndex = 0; plotIndex < plotCount; ++plotIndex)
	{
		if (!validPlots[plotIndex] || !cityReferencesValid) continue;
		PlotCoreRecord core;
		ZeroRecord(core);
		PlotSparseCountersRecord counters;
		ZeroRecord(counters);
		VoxRlFieldRangeFailure encodeFailure;
		if (!VoxRlEncodePlotCore(plotCaptures[plotIndex], tokenByCity, core, &encodeFailure))
		{
			VoxRlNoteSplitFailure(encodeFailure);
			valid = false;
		}
		VoxRlFieldRangeFailure counterFailure;
		if (VoxRlEncodePlotCounters(plotCaptures[plotIndex], plotIndex, counters, &counterFailure))
		{
			data.worldPlotSparseCounters.push_back(counters);
		}
		else if (counterFailure.record != 0)
		{
			VoxRlNoteSplitFailure(counterFailure);
			valid = false;
		}
		data.worldPlotCore.push_back(core);
	}
	if (!VoxRlCollectTeamPassabilityRows(data.worldTeamPassability)) valid = false;
	teamPassabilitySnapshot = data.worldTeamPassability;
	if (data.worldUnits.size() != iterationByUnit.size()) valid = false;
	if (timings != NULL) timings->unitCount = static_cast<unsigned int>(data.worldUnits.size());
	plotUnitTiming.Stop();

	// Each alive team contributes one plot bitset; all-zero optional detection is omitted.
	// Known visibility is mostly empty, so only teams with a set bit carry a slice.
	ScopedWorldTiming visibilityTiming(timings != NULL ? &timings->visibilityNs : NULL);
	const size_t bitBytes = (static_cast<size_t>(plotCount) + 7U) / 8U;
	std::vector<u8> knownVisibleSlice(bitBytes, 0);
	std::vector<u8>* bitsets[] = { &data.worldRevealedBits, &data.worldVisibleBits,
		&knownVisibleSlice, &data.worldInvisibleVisibleBits };
	for (int kind = 0; kind < 4; ++kind)
		if (kind != 2) bitsets[kind]->resize(bitBytes * aliveTeams.size(), 0);
	data.worldRevealedNoneOverrideBits.assign(bitBytes * aliveTeams.size(), 0);
	bool hasInvisibleVisibility = false;
	bool hasRevealedNoneOverrides = false;
	for (size_t teamIndex = 0; teamIndex < aliveTeams.size(); ++teamIndex)
	{
		const TeamTypes team = aliveTeams[teamIndex];
		PlotTeamRecord row;
		ZeroRecord(row);
		row.team = static_cast<i8>(team);
		std::vector<RevealedOverrideRecord> overrides;
		std::fill(knownVisibleSlice.begin(), knownVisibleSlice.end(), static_cast<u8>(0));
		bool hasKnownVisibility = false;
		for (int plotIndex = 0; plotIndex < plotCount; ++plotIndex)
		{
			CvPlot* plot = map.plotByIndex(plotIndex);
			const ImprovementTypes revealedImprovement = plot->getRevealedImprovementType(team);
			const RouteTypes revealedRoute = plot->getRevealedRouteType(team);
			const PlayerTypes revealedOwner = plot->getRevealedOwner(team);
			if (revealedImprovement != plot->getImprovementType() || revealedOwner != plot->getOwner() || revealedRoute != plot->getRouteType())
			{
				if (revealedImprovement == NO_IMPROVEMENT && revealedRoute == NO_ROUTE && revealedOwner == NO_PLAYER)
				{
					data.worldRevealedNoneOverrideBits[teamIndex * bitBytes + (plotIndex >> 3)] |= static_cast<u8>(1U << (plotIndex & 7));
					hasRevealedNoneOverrides = true;
				}
				else
				{
					RevealedOverrideRecord entry;
					ZeroRecord(entry);
					entry.team = static_cast<i8>(team);
					entry.plotIndex = plotIndex;
					entry.revealedImprovementType = revealedImprovement;
					entry.revealedRouteType = static_cast<i8>(revealedRoute);
					entry.revealedOwner = revealedOwner;
					overrides.push_back(entry);
				}
			}
			// The stored estimate, not GetKnownVisibilityCount: at this checkpoint the game's
			// current visibility team is still the previously processed player, and the accessor
			// would substitute that team's actual sight for the actor's knowledge of it.
			const bool bits[] = { plot->isRevealed(team), plot->isVisible(team),
				plot->GetKnownVisibilityEstimate(team) > 0, plot->isInvisibleVisibleUnit(team) };
			hasInvisibleVisibility = hasInvisibleVisibility || bits[3];
			hasKnownVisibility = hasKnownVisibility || bits[2];
			for (int kind = 0; kind < 4; ++kind)
				if (bits[kind]) (*bitsets[kind])[(kind == 2 ? 0U : teamIndex * bitBytes) + (plotIndex >> 3)] |= static_cast<u8>(1U << (plotIndex & 7));
		}
		row.knownVisibleOmitted = hasKnownVisibility ? 0 : 1;
		if (hasKnownVisibility)
			data.worldKnownVisibleBits.insert(data.worldKnownVisibleBits.end(), knownVisibleSlice.begin(), knownVisibleSlice.end());
		if (!AppendPlotTeamRecordRevealedOverrideRange(&row, &data, overrides)) valid = false;
		data.worldPlotTeams.push_back(row);
	}
	if (!hasInvisibleVisibility) data.worldInvisibleVisibleBits.clear();
	if (!hasRevealedNoneOverrides) data.worldRevealedNoneOverrideBits.clear();
	visibilityTiming.Stop();

	ScopedWorldTiming entityRelationTiming(timings != NULL ? &timings->entityRelationNs : NULL);
	const int resourceCount = GC.getNumResourceInfos();
	std::vector<std::vector<int> > tileResourceTotals(MAX_PLAYERS, std::vector<int>(resourceCount, 0));
	std::vector<std::vector<int> > ordinaryBuildingSupply(MAX_PLAYERS, std::vector<int>(resourceCount, 0));
	std::vector<std::vector<int> > wonderResourceConsumption(MAX_PLAYERS, std::vector<int>(resourceCount, 0));
	CollectTileResourceTotals(tileResourceTotals);
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		// The WORLD carries player rows only for alive players plus the barbarian slot.
		// A dead player's cities and units are gone or dying, and historical references
		// keep their IDs without needing captured player state.
		if (!owner.isAlive() && player != BARBARIAN_PLAYER) continue;
		unsigned int iteration = 0;
		int loop = 0;
		for (CvCity* city = owner.firstCity(&loop); city != NULL; city = owner.nextCity(&loop))
		{
			CityRecord row;
			ZeroRecord(row);
			if (!VoxRlCollectCityRecord(*city, capturingPlayer, row)) valid = false;
			row.iterationIndex = iteration++;
			data.worldCities.push_back(row);
			if (!CollectCityResources(*city, data.worldCityResources,
				&ordinaryBuildingSupply[player], &wonderResourceConsumption[player])) valid = false;
		}
		PlayerRecord row;
		ZeroRecord(row);
		if (!VoxRlCollectPlayerRecord(owner, capturingPlayer, row)) valid = false;
		row.id = static_cast<i8>(player);
		data.worldPlayers.push_back(row);
		// WORLD keeps only nonzero external contributions; an omitted row means zero.
		// REQUEST replacements stay complete so a replaced player's map is always whole.
		std::vector<PlayerResourceRecord> resourceRows;
		if (!CollectPlayerResources(owner, tileResourceTotals[player], ordinaryBuildingSupply[player],
			wonderResourceConsumption[player], resourceRows)) valid = false;
		for (size_t index = 0; index < resourceRows.size(); ++index)
			if (resourceRows[index].externalContribution != 0) data.worldPlayerResources.push_back(resourceRows[index]);
		PlayerEconomicsRecord economics;
		if (!CollectPlayerEconomics(owner, economics)) valid = false;
		data.worldPlayerEconomics.push_back(economics);
	}
	for (std::set<int>::const_iterator team = recordedTeams.begin(); team != recordedTeams.end(); ++team)
	{
		// Team rows follow the same rule as players: teams with an alive member plus the
		// barbarian team. The WORLD visibility team domain is the live-team list above.
		CvTeam& teamRecord = GET_TEAM(static_cast<TeamTypes>(*team));
		TeamRecord row;
		ZeroRecord(row);
		if (!CollectTeamRecord(teamRecord, row)) valid = false;
		data.worldTeams.push_back(row);
	}
	if (!VoxRlCollectTeamResourceRows(recordedTeams, data.worldTeamResources)) valid = false;
	teamResourceSnapshot = data.worldTeamResources;
	for (size_t team = 0; team < aliveTeams.size(); ++team)
		for (size_t other = 0; other < aliveTeams.size(); ++other)
		{
			if (team == other) continue;
			TeamRelationRecord row;
			ZeroRecord(row);
			if (!CollectTeamRelationRecord(GET_TEAM(aliveTeams[team]), aliveTeams[other], capturingPlayer, row)) valid = false;
			data.worldTeamRelations.push_back(row);
		}
	for (size_t player = 0; player < alivePlayers.size(); ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(alivePlayers[player]);
		InterceptorCacheRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(owner.GetID());
		const std::vector<std::pair<int, int> >& nativeEntries = owner.GetPossibleInterceptors();
		std::vector<InterceptorCacheEntryRecord> entries;
		for (size_t index = 0; index < nativeEntries.size(); ++index)
		{
			InterceptorCacheEntryRecord entry;
			ZeroRecord(entry);
			entry.owner = static_cast<i8>(owner.GetID());
			entry.unitId = static_cast<i32>(nativeEntries[index].first);
			entry.plotIndex = static_cast<i16>(nativeEntries[index].second);
			entries.push_back(entry);
		}
		if (!AppendInterceptorCacheRecordEntryRange(&row, &data, entries)) valid = false;
		data.worldInterceptorCaches.push_back(row);
	}
	VoxRlPlayerCitySnapshot playerCityState;
	if (!VoxRlCollectPlayerCitySnapshot(playerCityState, capturingPlayer)) valid = false;
	data.worldPlayerGreatPersons = playerCityState.playerGreatPersons;
	data.worldPlayerStrategicMonopolies = playerCityState.playerStrategicMonopolies;
	data.worldCityConnectionPlots = playerCityState.cityConnectionPlots;
	data.worldCityHealingYields = playerCityState.cityHealingYields;
	data.worldCityPurchaseCosts = playerCityState.cityPurchaseCosts;
	data.worldCityFreePromotions = playerCityState.cityFreePromotions;
	data.worldTeamTechnologies = playerCityState.teamTechnologies;
	data.worldPlayerSpecialUpgrades = playerCityState.playerSpecialUpgrades;
	data.worldPlayerSavings = playerCityState.playerSavings;
	data.worldPlayerRelations = playerCityState.playerRelations;
	data.worldPlayerTraitPromotionCombats = playerCityState.playerTraitPromotionCombats;
	data.worldPlayerTraitPromotionClasses = playerCityState.playerTraitPromotionClasses;
	data.worldPlayerFlavors = playerCityState.playerFlavors;
	data.worldPlayerFreePromotions = playerCityState.playerFreePromotions;
	if (playerCitySnapshot != NULL) *playerCitySnapshot = playerCityState;
	if (!VoxRlCollectCityEspionageSightRows(data.worldCityEspionageSights, 0, MAX_MAJOR_CIVS)) valid = false;
	// Found values are the capturing player's own settle cache, so other players' rows are not captured.
	if (!VoxRlCollectPlayerFoundValueRows(capturingPlayer, data.worldPlayerFoundValues)) valid = false;
	// The WORLD player map mirrors share the STATIC row layouts.
	for (size_t mapIndex = 0; mapIndex < playerMapOverrides.size(); ++mapIndex)
	{
		const VoxRlPlayerMapRows& rows = playerMapOverrides[mapIndex];
		WorldPlayerMapPlayerRecord player;
		std::memcpy(&player, &rows.player, sizeof(player));
		data.worldPlayerMapPlayers.push_back(player);
		for (size_t index = 0; index < rows.unitClasses.size(); ++index)
		{
			WorldPlayerMapUnitClassRecord row;
			std::memcpy(&row, &rows.unitClasses[index], sizeof(row));
			data.worldPlayerMapUnitClasses.push_back(row);
		}
		for (size_t index = 0; index < rows.unitCombatCosts.size(); ++index)
		{
			WorldPlayerMapUnitCombatCostRecord row;
			std::memcpy(&row, &rows.unitCombatCosts[index], sizeof(row));
			data.worldPlayerMapUnitCombatCosts.push_back(row);
		}
	}
	if (!VoxRlCollectWorldTrade(data)) valid = false;
	if (timings != NULL) timings->cityCount = static_cast<unsigned int>(data.worldCities.size());
	entityRelationTiming.Stop();
	if (!valid) return false;
	ScopedWorldTiming serializeTiming(timings != NULL ? &timings->serializeNs : NULL);
	const bool written = data.Write(identity, storage, length);
	serializeTiming.Stop();
	return written;
}

// Appends one native attack-target list, keeping its checkpoint order, scores, and preference.
// Attack targets and exposed cities share the CvAttackTarget field layout.
template <typename Row>
static bool AppendCampaignAttackTargets(const std::vector<CvAttackTarget>& targets, std::vector<Row>& rows,
	const char* recordName)
{
	bool valid = true;
	for (size_t index = 0; index < targets.size(); ++index)
	{
		const CvAttackTarget& target = targets[index];
		Row row;
		ZeroRecord(row);
		row.armyType = static_cast<i8>(target.m_armyType);
		// Native lists have valid muster and target plots; the shared encoder also admits
		// the optional staging plot's -1 sentinel, which the loader rejects for the others.
		const int plots[3] = { target.m_iMusterPlotIndex, target.m_iStagingPlotIndex, target.m_iTargetPlotIndex };
		i16* fields[3] = { &row.musterPlotIndex, &row.stagingPlotIndex, &row.targetPlotIndex };
		for (int plot = 0; plot < 3; ++plot)
		{
			VoxRlFieldRangeFailure plotFailure;
			if (!VoxRlEncodeOptionalPlotIndex(plots[plot], fields[plot], recordName, &plotFailure))
			{
				VoxRlNoteSplitFailure(plotFailure);
				valid = false;
			}
		}
		VoxRlAssignClamped(row.pathLength, target.m_iPathLength);
		VoxRlAssignClamped(row.approachScore, target.m_iApproachScore);
		row.preferred = target.m_bPreferred ? 1 : 0;
		rows.push_back(row);
	}
	return valid;
}

// Collects the operational entry snapshot without refreshing native zones.
bool VoxRlBuildCampaignBlock(const VoxRlBlockIdentity& identity, PlayerTypes capturingPlayer,
	unsigned int alignedWorldGeneration, unsigned int alignedNextDeltaSequence,
	const std::map<int, PlayerRecord>& retainedPlayers,
	VoxRlOwnedBlockStorage& storage, unsigned int& length)
{
	bool valid = true;
	CvPlayerAI& capturing = GET_PLAYER(capturingPlayer);
	CvMilitaryAI* military = capturing.GetMilitaryAI();
	if (military == NULL) return false;
	VoxRlCampaignData data;
	CampaignHeaderRecord& header = data.campaignHeader;
	ZeroRecord(header);
	if (!CollectCampaignHeaderRecord(*military, capturing, header)) valid = false;
	header.nextGlobalId = GC.getGame().VoxRlPeekNextGlobalID();
	header.alignedWorldGeneration = alignedWorldGeneration;
	header.alignedNextDeltaSequence = alignedNextDeltaSequence;
	// The general flavor vector every search starts from, and whether its RISK is set.
	STacticalFlavors generalFlavors;
	TacticalAIHelpers::GetGeneralTacticalFlavors(capturingPlayer, generalFlavors);
	for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
		header.militaryFlavors[slot] = static_cast<u8>(generalFlavors.iFlavor[slot]);
	header.militaryRiskSet = generalFlavors.bRiskSet ? 1 : 0;
	VoxRlAssignClamped(header.recommendedLandUnits, military->GetRecommendedLandUnits());
	VoxRlAssignClamped(header.recommendedNavalUnits, military->GetRecommendedNavalUnits());
	VoxRlAssignClamped(header.recommendedExplorerUnits, military->GetRecommendedExplorerUnits());
	// One row per other player with a nonzero tactical flavor modifier.
	data.campaignMilitaryFlavorModifiers.clear();
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		const short* modifiers = military->GetTacticalFlavorModifiers(static_cast<PlayerTypes>(player));
		if (modifiers == NULL) continue;
		CampaignMilitaryFlavorModifierRecord row;
		ZeroRecord(row);
		row.counterpartPlayer = static_cast<i8>(player);
		for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
			row.militaryFlavorModifiers[slot] = static_cast<i16>(modifiers[slot]);
		data.campaignMilitaryFlavorModifiers.push_back(row);
	}

	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		if (player == static_cast<int>(capturingPlayer)) continue;
		CvPlayerAI& other = GET_PLAYER(static_cast<PlayerTypes>(player));
		if (!other.isAlive()) continue;
		CampaignEnemyRecord row;
		ZeroRecord(row);
		row.player = static_cast<i8>(player);
		// Barbarians have no entry in the diplomacy AI's civilization war-state array.
		row.warState = static_cast<i8>(player < MAX_CIV_PLAYERS
			? capturing.GetDiplomacyAI()->GetWarState(static_cast<PlayerTypes>(player)) : NO_WAR_STATE_TYPE);
		row.warScore = static_cast<i8>(capturing.GetDiplomacyAI()->GetWarScore(static_cast<PlayerTypes>(player)));
		data.campaignEnemies.push_back(row);
	}
	// Capture the aligned WORLD roster, including players eliminated since that baseline.
	for (std::map<int, PlayerRecord>::const_iterator player = retainedPlayers.begin();
		player != retainedPlayers.end(); ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player->first));
		CvMilitaryAI* preferencesMilitary = owner.GetMilitaryAI();
		if (preferencesMilitary == NULL) return false;
		CampaignPlayerPreferencesRecord preferences;
		ZeroRecord(preferences);
		preferences.owner = static_cast<i8>(player->first);
		preferences.attackTargetRangeFirst = static_cast<u32>(data.campaignAttackTargets.size());
		preferences.exposedCityRangeFirst = static_cast<u32>(data.campaignExposedCities.size());
		if (!AppendCampaignAttackTargets(preferencesMilitary->GetPotentialAttackTargets(), data.campaignAttackTargets, "CampaignAttackTargetRecord")) valid = false;
		if (!AppendCampaignAttackTargets(preferencesMilitary->GetExposedCities(), data.campaignExposedCities, "CampaignExposedCityRecord")) valid = false;
		preferences.attackTargetRangeCount = static_cast<u32>(data.campaignAttackTargets.size()) - preferences.attackTargetRangeFirst;
		preferences.exposedCityRangeCount = static_cast<u32>(data.campaignExposedCities.size()) - preferences.exposedCityRangeFirst;
		data.campaignPlayerPreferences.push_back(preferences);
	}
	std::vector<CvCity*> threatened = capturing.GetThreatenedCities(false);
	std::vector<CampaignThreatenedCityRecord> threatenedRows;
	for (size_t index = 0; index < threatened.size(); ++index)
	{
		CampaignThreatenedCityRecord row;
		ZeroRecord(row);
		row.cityOwner = threatened[index] != NULL ? static_cast<i8>(threatened[index]->getOwner()) : static_cast<i8>(-1);
		row.cityId = threatened[index] != NULL ? static_cast<i32>(threatened[index]->GetID()) : -1;
		threatenedRows.push_back(row);
	}
	if (!AppendCampaignHeaderRecordThreatenedCityRange(&data, threatenedRows)) valid = false;
	std::vector<CvCity*> coastal = capturing.GetThreatenedCities(true);
	std::vector<CampaignCoastalThreatenedCityRecord> coastalRows;
	for (size_t index = 0; index < coastal.size(); ++index)
	{
		CampaignCoastalThreatenedCityRecord row;
		ZeroRecord(row);
		row.cityOwner = coastal[index] != NULL ? static_cast<i8>(coastal[index]->getOwner()) : static_cast<i8>(-1);
		row.cityId = coastal[index] != NULL ? static_cast<i32>(coastal[index]->GetID()) : -1;
		coastalRows.push_back(row);
	}
	if (!AppendCampaignHeaderRecordCoastalThreatenedCityRange(&data, coastalRows)) valid = false;
	for (size_t index = 0; index < capturing.getNumAIOperations(); ++index)
	{
		CvAIOperation* operation = capturing.getAIOperationByIndex(index);
		if (operation == NULL) continue;
		CampaignOperationRecord row;
		ZeroRecord(row);
		row.owner = static_cast<i8>(operation->GetOwner());
		row.id = static_cast<i32>(operation->GetID());
		row.type = static_cast<i8>(operation->GetOperationType());
		row.armyType = static_cast<i8>(operation->GetArmyType());
		row.currentState = static_cast<i8>(operation->GetOperationState());
		row.enemy = static_cast<i8>(operation->GetEnemy());
		row.musterX = static_cast<i32>(operation->GetMusterX());
		row.musterY = static_cast<i32>(operation->GetMusterY());
		row.targetX = static_cast<i32>(operation->GetTargetX());
		row.targetY = static_cast<i32>(operation->GetTargetY());
		if (!AssignCheckedI16(row.turnStarted, operation->GetTurnStarted(),
			"CampaignOperationRecord", "turnStarted", -1)) valid = false;
		VoxRlAssignClamped(row.distanceMusterToTarget, operation->GetDistanceMusterToTarget());
		row.abortReason = static_cast<i8>(operation->GetAbortReason());
		if (!AssignCheckedI16(row.lastTurnMoved, operation->GetLastTurnMoved(),
			"CampaignOperationRecord", "lastTurnMoved", -1)) valid = false;
		for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
			row.militaryFlavorModifiers[slot] = static_cast<i16>(operation->GetTacticalFlavorModifier(slot));
		std::vector<CampaignOperationArmyIdRecord> armyIds;
		const std::vector<int>& ids = operation->GetArmyIDs();
		for (size_t army = 0; army < ids.size(); ++army)
		{
			CampaignOperationArmyIdRecord id;
			ZeroRecord(id);
			id.armyId = static_cast<i32>(ids[army]);
			armyIds.push_back(id);
		}
		if (!AppendCampaignOperationRecordArmyIdRange(&row, &data, armyIds)) valid = false;
		data.campaignOperations.push_back(row);
		for (size_t army = 0; army < ids.size(); ++army)
		{
			CvArmyAI* nativeArmy = capturing.getArmyAI(ids[army]);
			if (nativeArmy == NULL) continue;
			CampaignArmyRecord armyRow;
			ZeroRecord(armyRow);
			armyRow.id = static_cast<i32>(nativeArmy->GetID());
			if (!AssignCheckedI16(armyRow.formation, static_cast<int>(nativeArmy->GetFormationType()),
				"CampaignArmyRecord", "formation", -1)) valid = false;
			armyRow.aiState = static_cast<i8>(nativeArmy->GetArmyAIState());
			armyRow.goalX = static_cast<i32>(nativeArmy->GetGoalX());
			armyRow.goalY = static_cast<i32>(nativeArmy->GetGoalY());
			VoxRlAssignClamped(armyRow.slotsFilled, nativeArmy->GetNumSlotsFilled());
			std::vector<CampaignFormationEntryRecord> slots;
			const std::vector<CvArmyFormationSlot>& nativeSlots = nativeArmy->GetSlotStatus();
			for (size_t slot = 0; slot < nativeSlots.size(); ++slot)
			{
				CampaignFormationEntryRecord entry;
				ZeroRecord(entry);
				VoxRlAssignClamped(entry.slotIndex, static_cast<i32>(slot));
				entry.unitId = static_cast<i32>(nativeSlots[slot].GetUnitID());
				entry.unitOwner = entry.unitId >= 0 ? static_cast<i8>(capturingPlayer) : static_cast<i8>(-1);
				entry.required = nativeSlots[slot].IsRequired() ? 1 : 0;
				for (size_t history = 0; history < VoxRlCheckpointhistoryCapacity; ++history)
					if (!AssignCheckpointTurns(entry.checkpointTurns[history], nativeSlots[slot].GetTurnsToCheckpoint(history),
						"CampaignFormationEntryRecord")) valid = false;
				slots.push_back(entry);
			}
			if (!AppendCampaignArmyRecordFormationRange(&armyRow, &data, slots)) valid = false;
			data.campaignArmies.push_back(armyRow);
		}
	}
	CvTacticalAnalysisMap* zoneMap = capturing.GetTacticalAI()->GetTacticalAnalysisMap();
	const int zoneCount = zoneMap != NULL ? zoneMap->GetNumZonesWithoutRefresh() : 0;
	for (int index = 0; index < zoneCount; ++index)
	{
		const CvTacticalDominanceZone* zone = zoneMap->GetZoneByIndexWithoutRefresh(index);
		if (zone == NULL) return false;
		CampaignZoneRecord row;
		ZeroRecord(row);
		if (!CollectCampaignZoneRecord(*zone, row)) valid = false;
		row.avgX = static_cast<i32>(zone->GetAverageX());
		row.avgY = static_cast<i32>(zone->GetAverageY());
		CvCity* city = zone->GetZoneCity();
		row.cityOwner = city != NULL ? static_cast<i8>(city->getOwner()) : static_cast<i8>(-1);
		row.cityId = city != NULL ? static_cast<i32>(city->GetID()) : static_cast<i32>(-1);
		std::vector<CampaignZoneNeighborRecord> neighbors;
		const std::vector<int>& nativeNeighbors = zone->GetNeighboringZones();
		for (size_t neighbor = 0; neighbor < nativeNeighbors.size(); ++neighbor)
		{
			CampaignZoneNeighborRecord entry;
			ZeroRecord(entry);
			entry.zoneId = static_cast<i32>(nativeNeighbors[neighbor]);
			neighbors.push_back(entry);
		}
		if (!AppendCampaignZoneRecordNeighborRange(&row, &data, neighbors)) valid = false;
		data.campaignZones.push_back(row);
	}
	const std::vector<CvFocusArea>& focusAreas = capturing.GetTacticalAI()->GetFocusAreas();
	for (size_t index = 0; index < focusAreas.size(); ++index)
	{
		CvPlot* center = GC.getMap().plot(focusAreas[index].m_iX, focusAreas[index].m_iY);
		if (center == NULL || focusAreas[index].m_iRadius < 0 || focusAreas[index].m_iRadius > 255 ||
			focusAreas[index].m_iLastTurn < -1 || focusAreas[index].m_iLastTurn > 32767)
		{
			valid = false;
			continue;
		}
		CampaignFocusAreaRecord row;
		ZeroRecord(row);
		row.centerPlotIndex = static_cast<i16>(center->GetPlotIndex());
		row.radius = static_cast<u8>(focusAreas[index].m_iRadius);
		row.expiryTurn = static_cast<i16>(focusAreas[index].m_iLastTurn);
		data.campaignFocusAreas.push_back(row);
	}
	VoxRlCollectPendingMilitaryTransfers(capturingPlayer, data);
	if (!valid) return false;
	return data.Write(identity, storage, length);
}

// Appends one operation record and the complete request-local state versions that
// explain its effective change. An already-moved invocation intentionally carries
// empty ranges because no state changed.
bool VoxRlAppendOperationRecord(CvAIOperation& operation, PlayerTypes initiatingPlayer,
	unsigned char kind, unsigned char changeCause, unsigned char invocationResult,
	signed char abortReason, VoxRlRequestData& data)
{
	bool valid = true;
	RequestOperationRecord record;
	ZeroRecord(record);
	record.kind = kind;
	record.operationOwner = static_cast<i8>(operation.GetOwner());
	record.initiatingPlayer = static_cast<i8>(initiatingPlayer);
	record.operationId = operation.GetID();
	record.changeCause = changeCause;
	record.invocationResult = invocationResult;
	record.abortReason = abortReason;
	if (kind == VOX_RL_OPERATION_RECORD_INVOCATION_RESULT &&
		invocationResult == VOX_RL_OPERATION_RESULT_ALREADY_MOVED)
	{
		data.requestOperations.push_back(record);
		return true;
	}
	const int turnStarted = operation.GetTurnStarted();
	const int lastTurnMoved = operation.GetLastTurnMoved();
	RequestOperationVersionRecord version;
	ZeroRecord(version);
	version.owner = static_cast<i8>(operation.GetOwner());
	version.id = operation.GetID();
	version.type = static_cast<i8>(operation.GetOperationType());
	version.armyType = static_cast<i8>(operation.GetArmyType());
	version.currentState = static_cast<i8>(operation.GetOperationState());
	version.enemy = static_cast<i8>(operation.GetEnemy());
	version.musterX = operation.GetMusterX();
	version.musterY = operation.GetMusterY();
	version.targetX = operation.GetTargetX();
	version.targetY = operation.GetTargetY();
	if (!AssignCheckedI16(version.turnStarted, turnStarted,
		"RequestOperationVersionRecord", "turnStarted", -1)) valid = false;
	VoxRlAssignClamped(version.distanceMusterToTarget, operation.GetDistanceMusterToTarget());
	version.abortReason = static_cast<i8>(operation.GetAbortReason());
	if (!AssignCheckedI16(version.lastTurnMoved, lastTurnMoved,
		"RequestOperationVersionRecord", "lastTurnMoved", -1)) valid = false;
	for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
		version.militaryFlavorModifiers[slot] = static_cast<i16>(operation.GetTacticalFlavorModifier(slot));
	std::vector<RequestOperationVersionRecord> versions(1, version);
	if (!AppendRequestOperationRecordOperationVersionRange(&record, &data, versions)) valid = false;

	std::vector<RequestArmyVersionRecord> armies;
	std::vector<RequestFormationEntryVersionRecord> slots;
	CvPlayerAI& owner = GET_PLAYER(operation.GetOwner());
	const std::vector<int>& armyIds = operation.GetArmyIDs();
	for (size_t armyIndex = 0; armyIndex < armyIds.size(); ++armyIndex)
	{
		CvArmyAI* army = owner.getArmyAI(armyIds[armyIndex]);
		if (army == NULL) continue;
		RequestArmyVersionRecord armyRow;
		ZeroRecord(armyRow);
		armyRow.operationOwner = static_cast<i8>(operation.GetOwner());
		armyRow.operationId = operation.GetID();
		armyRow.id = army->GetID();
		if (!AssignCheckedI16(armyRow.formation, static_cast<int>(army->GetFormationType()),
			"RequestArmyVersionRecord", "formation", -1)) valid = false;
		armyRow.aiState = static_cast<i8>(army->GetArmyAIState());
		armyRow.goalX = army->GetGoalX();
		armyRow.goalY = army->GetGoalY();
		VoxRlAssignClamped(armyRow.slotsFilled, army->GetNumSlotsFilled());
		armies.push_back(armyRow);
		const std::vector<CvArmyFormationSlot>& nativeSlots = army->GetSlotStatus();
		if (nativeSlots.size() > 32768U) return false;
		for (size_t slotIndex = 0; slotIndex < nativeSlots.size(); ++slotIndex)
		{
			RequestFormationEntryVersionRecord slot;
			ZeroRecord(slot);
			slot.operationOwner = static_cast<i8>(operation.GetOwner());
			slot.operationId = operation.GetID();
			slot.armyId = army->GetID();
			slot.slotIndex = static_cast<i16>(slotIndex);
			slot.unitId = nativeSlots[slotIndex].GetUnitID();
			slot.unitOwner = slot.unitId >= 0 ? static_cast<i8>(operation.GetOwner()) : static_cast<i8>(NO_PLAYER);
			slot.required = nativeSlots[slotIndex].IsRequired() ? 1 : 0;
			for (size_t history = 0; history < VoxRlCheckpointhistoryCapacity; ++history)
			{
				const int estimate = nativeSlots[slotIndex].GetTurnsToCheckpoint(history);
				if (!AssignCheckpointTurns(slot.checkpointTurns[history], estimate,
					"RequestFormationEntryVersionRecord")) valid = false;
			}
			slots.push_back(slot);
		}
	}
	if (!AppendRequestOperationRecordArmyVersionRange(&record, &data, armies)) valid = false;
	if (!AppendRequestOperationRecordFormationEntryVersionRange(&record, &data, slots)) valid = false;
	data.requestOperations.push_back(record);
	return valid;
}

// Reports the failed REQUEST build step with enough context to locate its input.
static bool NoteRequestBuildFailure(const VoxRlBlockIdentity& identity,
	const VoxRlRequestData& data, const char* stage, const char* section,
	const char* check, size_t rowIndex, u32 requiredBytes = 0)
{
	FILogFile* log = VoxRlCaptureLog();
	if (log != NULL)
	{
		log->Msg("Capture REQUEST build failed: stage=%s player=%d turn=%d delta=%u decision=%u phase=%u section=%s check=%s row=%d requiredBytes=%u operations=%u operationVersions=%u armyVersions=%u formationVersions=%u interceptorReplacements=%u interceptorEntries=%u.\n",
			stage, identity.player, identity.turn, identity.deltaSequence, identity.decisionId,
			static_cast<unsigned int>(data.requestHeader.phase), section, check,
			rowIndex == static_cast<size_t>(-1) ? -1 : static_cast<int>(rowIndex), requiredBytes,
			static_cast<unsigned int>(data.requestOperations.size()),
			static_cast<unsigned int>(data.requestOperationVersions.size()),
			static_cast<unsigned int>(data.requestArmyVersions.size()),
			static_cast<unsigned int>(data.requestFormationEntryVersions.size()),
			static_cast<unsigned int>(data.requestInterceptorReplacements.size()),
			static_cast<unsigned int>(data.requestInterceptorEntries.size()));
	}
	return false;
}

// Binds staged request children through their declared generated range helpers.
bool VoxRlBuildRequestBlock(const VoxRlBlockIdentity& identity, VoxRlRequestData& data,
	VoxRlOwnedBlockStorage& storage, unsigned int& length)
{
	length = 0;
	// Preserve every section, including parent rows and their request-local children.
	// Section row counts now replace the header helpers that used to copy these rows.
	VoxRlRequestData output = data;
	output.requestHeader.decisionId = static_cast<u32>(identity.decisionId);
	// Removal and unchanged-invocation rows have no children and arrive with
	// zeroed ranges. Bind their empty ranges at the preceding snapshot's end
	// so they tile the child sections alongside rows that do carry versions.
	u32 operationCursor = 0;
	u32 armyCursor = 0;
	u32 formationCursor = 0;
	for (size_t index = 0; index < output.requestOperations.size(); ++index)
	{
		RequestOperationRecord& operation = output.requestOperations[index];
		if (operation.operationVersionRangeCount == 0)
			operation.operationVersionRangeFirst = operationCursor;
		if (operation.armyVersionRangeCount == 0)
			operation.armyVersionRangeFirst = armyCursor;
		if (operation.formationEntryVersionRangeCount == 0)
			operation.formationEntryVersionRangeFirst = formationCursor;
		operationCursor = operation.operationVersionRangeFirst + operation.operationVersionRangeCount;
		armyCursor = operation.armyVersionRangeFirst + operation.armyVersionRangeCount;
		formationCursor = operation.formationEntryVersionRangeFirst + operation.formationEntryVersionRangeCount;
	}
	// Interceptor ranges are bound below from the collected entries in player order.
	output.requestInterceptorEntries.clear();
	size_t entryCursor = 0;
	for (size_t index = 0; index < data.requestInterceptorReplacements.size(); ++index)
	{
		RequestInterceptorReplacementRecord& replacement = output.requestInterceptorReplacements[index];
		std::vector<RequestInterceptorEntryRecord> entries;
		while (entryCursor < data.requestInterceptorEntries.size() &&
			static_cast<int>(data.requestInterceptorEntries[entryCursor].owner) == static_cast<int>(replacement.player))
		{
			entries.push_back(data.requestInterceptorEntries[entryCursor]);
			++entryCursor;
		}
		if (!AppendRequestInterceptorReplacementRecordEntryRange(&replacement, &output, entries))
			return NoteRequestBuildFailure(identity, data, "interceptorBinding",
				"requestInterceptorReplacements", "entryRange append", index);
	}
	if (entryCursor != data.requestInterceptorEntries.size())
		return NoteRequestBuildFailure(identity, data, "interceptorBinding",
			"requestInterceptorEntries", "unmatched owner", entryCursor);
	// The sparse replacement tiling and identity consistency are enforced by the
	// generated request validator. Diagnose only failures to keep successful builds cheap.
	u32 requiredBytes = 0;
	if (!output.ByteLength(&requiredBytes))
	{
		VoxRlBuilderValidationFailure failure;
		if (!VoxRlValidateRequestData(output, &failure))
			return NoteRequestBuildFailure(identity, output, "dataValidation",
				failure.section, failure.check, failure.rowIndex);
		return NoteRequestBuildFailure(identity, output, "byteLength",
			"request", "payload size limit or overflow", static_cast<size_t>(-1));
	}
	if (!storage.Allocate(requiredBytes))
		return NoteRequestBuildFailure(identity, output, "allocation",
			"request", "storage allocation", static_cast<size_t>(-1), requiredBytes);
	if (!output.Write(identity, storage.MutableBytes(), storage.ByteLength(), &length))
		return NoteRequestBuildFailure(identity, output, "serialization",
			"request", "writer rejected block", static_cast<size_t>(-1), requiredBytes);
	return true;
}
