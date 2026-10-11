// Installed from simulator/capture/VoxRlCaptureBuilders.h. Edit that source, then reinstall.

// Vox Deorum: recording capture block builders. Builders stage rows from the
// generated collectors and hand-written loops, then write complete framed
// blocks through the shared writer in canonical section order. They perform
// no file I/O. Zone tables are always read without triggering a refresh.
#ifndef VOX_RL_CAPTURE_BUILDERS_H
#define VOX_RL_CAPTURE_BUILDERS_H

#include "VoxDeorumRL/schema/VoxRlBuilders.generated.h"
#include <cstring>
#include <limits>
#include <map>
#include <set>
#include <utility>
#include <vector>

class CvUnitEntry;
class FILogFile;

// One owner-qualified city identity with its stable (owner, id) ordering.
typedef std::pair<int, int> VoxRlCityKey;

// Holds the complete mutable child tables used to compare each REQUEST with WORLD.
struct VoxRlPlayerCitySnapshot
{
	std::vector<PlayerGreatPersonRecord> playerGreatPersons;
	std::vector<PlayerStrategicMonopolyRecord> playerStrategicMonopolies;
	std::vector<CityConnectionPlotRecord> cityConnectionPlots;
	std::vector<CityHealingYieldRecord> cityHealingYields;
	std::vector<CityPurchaseCostRecord> cityPurchaseCosts;
	std::vector<CityFreePromotionRecord> cityFreePromotions;
	std::vector<TeamTechnologyRecord> teamTechnologies;
	std::vector<PlayerSpecialUpgradeRecord> playerSpecialUpgrades;
	std::vector<PlayerSavingsRecord> playerSavings;
	std::vector<PlayerRelationRecord> playerRelations;
	std::vector<PlayerFlavorRecord> playerFlavors;
	std::vector<PlayerFreePromotionRecord> playerFreePromotions;
	std::vector<PlayerTraitPromotionCombatRecord> playerTraitPromotionCombats;
	std::vector<PlayerTraitPromotionClassRecord> playerTraitPromotionClasses;
	std::vector<TeamRecord> teams;
};

// Collects all player and city child tables and team scalars in stable owner order.
// Player relations and flavors cover only the perspective (capturing) player.
bool VoxRlCollectPlayerCitySnapshot(VoxRlPlayerCitySnapshot& snapshot, PlayerTypes perspectivePlayer);

// Collects the active native trade roster as one complete REQUEST replacement.
bool VoxRlCollectRequestTrade(VoxRlRequestData& data);

// Emits complete replacements for changed player and city child tables, including clears.
bool VoxRlAppendPlayerCityReplacements(VoxRlPlayerCitySnapshot& baseline,
	const VoxRlPlayerCitySnapshot& current, const std::vector<int>& playerOwners,
	const std::vector<int>& teamOwners, const std::vector<VoxRlCityKey>& cityOwners,
	VoxRlRequestData& data);

// Collects generated player fields and the at-war strategy bit.
bool VoxRlCollectPlayerRecord(class CvPlayer& player, PlayerTypes capturingPlayer, PlayerRecord& row);

// Returns native military-support units omitted from the live unit rows.
int VoxRlMilitaryUnitCountCorrection(class CvPlayer& player);

// Returns the collapsed friendly-territory religion heal native CvUnit::healRate adds.
int VoxRlReligionFriendlyHealChange(class CvPlayer& player);

// Returns the permanent pantheon belief eligible for an independent friendly heal contribution.
int VoxRlPermanentPantheonHealBelief(class CvPlayer& player);


// Saturates a native integer to the destination schema field's storage range before
// narrowing. Wide bounds keep signed inputs comparable with unsigned field limits.
template <typename Target>
void VoxRlAssignClamped(Target& destination, i32 value)
{
	const __int64 minimum = (std::numeric_limits<Target>::min)();
	const __int64 maximum = (std::numeric_limits<Target>::max)();
	destination = static_cast<Target>(value < minimum ? minimum : value > maximum ? maximum : value);
}

// Builds the sorted unique city reference table covering every non-null owning and
// effective owning city in the collected rows. Token zero stays reserved for the null
// pair, so row N's token is its one-based position. The template covers the WORLD rows
// and their request mirrors, which share the field layout. The table fits the
// sixteen-bit tokens.
template <typename ReferenceRow>
bool VoxRlBuildCityReferenceTable(const std::vector<PlotCaptureRecord>& rows,
	std::vector<ReferenceRow>* table, std::map<VoxRlCityKey, unsigned int>* tokenByCity)
{
	if (table == 0 || tokenByCity == 0) return false;
	table->clear();
	tokenByCity->clear();
	std::set<VoxRlCityKey> references;
	for (size_t index = 0; index < rows.size(); ++index)
	{
		const PlotCaptureRecord& row = rows[index];
		if (row.owningCityOwner >= 0 && row.owningCityId >= 0)
			references.insert(VoxRlCityKey(row.owningCityOwner, row.owningCityId));
		if (row.effectiveOwningCityOwner >= 0 && row.effectiveOwningCityId >= 0)
			references.insert(VoxRlCityKey(row.effectiveOwningCityOwner, row.effectiveOwningCityId));
	}
	// Tokens are sixteen bits with zero reserved, so the table itself stays below 65536.
	if (references.size() > 65535U) return false;
	for (std::set<VoxRlCityKey>::const_iterator reference = references.begin();
		reference != references.end(); ++reference)
	{
		ReferenceRow record = ReferenceRow();
		record.owner = static_cast<signed char>(reference->first);
		record.id = reference->second;
		tokenByCity->insert(std::make_pair(*reference, static_cast<unsigned int>(table->size() + 1U)));
		table->push_back(record);
	}
	return true;
}

// Encodes one collected plot's shared core fields into a WORLD or request delta row:
// city tokens from the block's table and bounded one-byte types, all checked against
// the wire contract before narrowing.
template <typename CoreRow>
bool VoxRlEncodePlotCore(const PlotCaptureRecord& source,
	const std::map<VoxRlCityKey, unsigned int>& tokenByCity, CoreRow& out,
	VoxRlFieldRangeFailure* failure);

// Encodes one collected plot's three counters into a sparse counter row. Movement
// cost saturates to its storage range; the other counters are checked against their
// wire domains. Returns false when every counter is zero, meaning the row is omitted.
template <typename CounterRow>
bool VoxRlEncodePlotCounters(const PlotCaptureRecord& source, int plotIndex,
	CounterRow& out, VoxRlFieldRangeFailure* failure);

// Resolves one city reference pair to its token: zero for the null pair, the table
// position otherwise. Mixed half-null pairs fail capture as invalid references.
inline bool VoxRlEncodeCityToken(int owner, int id, const std::map<VoxRlCityKey, unsigned int>& tokenByCity,
	unsigned short* out, VoxRlFieldRangeFailure* failure)
{
	const bool nullOwner = owner < 0;
	const bool nullId = id < 0;
	if (nullOwner != nullId)
		return VoxRlFailFieldRange(failure, "pair", "PlotCaptureRecord", "owningCity", owner < 0 ? id : owner, -1, -1);
	if (nullOwner && nullId)
	{
		*out = 0;
		return true;
	}
	std::map<VoxRlCityKey, unsigned int>::const_iterator token = tokenByCity.find(VoxRlCityKey(owner, id));
	if (token == tokenByCity.end())
		return VoxRlFailFieldRange(failure, "pair", "PlotCaptureRecord", "owningCity", id, -1, -1);
	*out = static_cast<unsigned short>(token->second);
	return true;
}

// Encodes one optional plot reference into its sixteen-bit wire field; -1 marks absence
// and any value outside the plot domain fails with a diagnostic naming the record instead of truncating.
inline bool VoxRlEncodeOptionalPlotIndex(int source, short* out, const char* record, VoxRlFieldRangeFailure* failure)
{
	if (source < -1 || source > 32767)
		return VoxRlFailFieldRange(failure, "range", record, "plotIndex", source, -1, 32767);
	*out = static_cast<short>(source);
	return true;
}

// Encodes one collected plot's shared core fields; see the declaration above.
template <typename CoreRow>
bool VoxRlEncodePlotCore(const PlotCaptureRecord& source,
	const std::map<VoxRlCityKey, unsigned int>& tokenByCity, CoreRow& out,
	VoxRlFieldRangeFailure* failure)
{
	if (!VoxRlEncodeCityToken(source.owningCityOwner, source.owningCityId, tokenByCity, &out.owningCityToken, failure)) return false;
	if (!VoxRlEncodeCityToken(source.effectiveOwningCityOwner, source.effectiveOwningCityId, tokenByCity, &out.effectiveOwningCityToken, failure)) return false;
	if (source.improvementType < -1 || source.improvementType > 127)
		return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "improvementType", source.improvementType, -1, 127);
	if (source.resourceType < -1 || source.resourceType > 127)
		return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "resourceType", source.resourceType, -1, 127);
	out.improvementType = static_cast<signed char>(source.improvementType);
	out.resourceType = static_cast<signed char>(source.resourceType);
	if (source.resourceCount < 0 || source.resourceCount > 127)
		return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "resourceCount", source.resourceCount, 0, 127);
	out.resourceCount = static_cast<signed char>(source.resourceCount);
	out.owner = source.owner;
	out.featureType = source.featureType;
	out.routeType = source.routeType;
	out.setBeingWorked(source.beingWorked != 0);
	out.setImprovementPillaged(source.improvementPillaged != 0);
	out.setImprovementPassable(source.improvementPassable != 0);
	out.setRoutePillaged(source.routePillaged != 0);
	out.setRestoreMoves(source.restoreMoves != 0);
	out.setFreeMoveAcross(source.freeMoveAcross != 0);
	return true;
}

// Encodes one collected plot's counters; see the declaration above.
template <typename CounterRow>
bool VoxRlEncodePlotCounters(const PlotCaptureRecord& source, int plotIndex,
	CounterRow& out, VoxRlFieldRangeFailure* failure)
{
	if (source.reconCount != 0 || source.extraMovePathCost != 0 || source.unitIncrement != 0)
	{
		if (plotIndex < 0 || plotIndex > 32767)
			return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "plotIndex", plotIndex, 0, 32767);
		if (source.reconCount < 0 || source.reconCount > 127)
			return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "reconCount", source.reconCount, 0, 127);
		if (source.unitIncrement < -32768 || source.unitIncrement > 32767)
			return VoxRlFailFieldRange(failure, "range", "PlotCaptureRecord", "unitIncrement", source.unitIncrement, -32768, 32767);
		out.plotIndex = static_cast<short>(plotIndex);
		out.reconCount = static_cast<signed char>(source.reconCount);
		VoxRlAssignClamped(out.extraMovePathCost, source.extraMovePathCost);
		out.unitIncrement = static_cast<short>(source.unitIncrement);
		return true;
	}
	return false;
}

// Retains a no-refresh tactical zone table and its per-plot assignments.
struct VoxRlZoneSnapshot
{
	std::vector<ZoneRecord> zones;
	std::vector<ZoneNeighborRecord> neighbors;
	std::vector<i32> plotZones;
};

// Returns a high-resolution monotonic timestamp in nanoseconds for the
// opt-in capture timings.
unsigned __int64 VoxRlTimingNanoseconds();

// Returns the shared capture log, or null when logging is unavailable.
FILogFile* VoxRlCaptureLog();

// Optional WORLD construction timings and row volumes. A null pointer at the
// builder boundary skips every clock read and counter update.
struct VoxRlWorldBuildTimings
{
	unsigned __int64 ownerIterationNs;
	unsigned __int64 dangerSparseRelationsNs;
	unsigned __int64 zoneNs;
	unsigned __int64 plotUnitNs;
	unsigned __int64 visibilityNs;
	unsigned __int64 entityRelationNs;
	unsigned __int64 serializeNs;
	unsigned int plotCount;
	unsigned int unitCount;
	unsigned int cityCount;
	unsigned int alivePlayerCount;
	unsigned int aliveTeamCount;
	// Initializes every optional phase and volume to zero.
	VoxRlWorldBuildTimings()
		: ownerIterationNs(0), dangerSparseRelationsNs(0), zoneNs(0),
		plotUnitNs(0), visibilityNs(0), entityRelationNs(0), serializeNs(0),
		plotCount(0), unitCount(0), cityCount(0), alivePlayerCount(0), aliveTeamCount(0)
	{
	}
};

// Collects current zone records without triggering native map preparation.
bool VoxRlCollectZones(class CvTacticalAnalysisMap* zoneMap, VoxRlZoneSnapshot& snapshot);

// Reports one rejected conversion or sparse row through the shared capture log. The
// generated collectors call the first form; split call sites wrap the failure struct.
void VoxRlNoteCaptureRangeFailure(const char* reason, const char* record, const char* field,
	int value, int minValue, int maxValue);
void VoxRlNoteSplitFailure(const VoxRlFieldRangeFailure& failure);

// Assigns one native identifier or index to a narrowed schema field when it fits the
// field's storage range and lower bound. Otherwise it notes a range failure and leaves
// the destination unchanged, so the caller can reject the row instead of saturating it.
template <typename Target>
bool VoxRlAssignChecked(Target& destination, i32 value, const char* record, const char* field,
	i32 minimum = static_cast<i32>((std::numeric_limits<Target>::min)()))
{
	const __int64 maximum = (std::numeric_limits<Target>::max)();
	if (value < minimum || value > maximum)
	{
		VoxRlNoteCaptureRangeFailure("range", record, field, value, minimum, static_cast<int>(maximum));
		return false;
	}
	destination = static_cast<Target>(value);
	return true;
}

// Holds one player's complete map: its STATIC player row and its sparse unit class and
// unit combat cost rows, in collection order. Rows are zeroed before collection, so two
// maps compare byte for byte.
struct VoxRlPlayerMapRows
{
	StaticPlayerRecord player;
	std::vector<StaticPlayerUnitClassRecord> unitClasses;
	std::vector<StaticPlayerUnitCombatCostRecord> unitCombatCosts;
};

// Collects one player's current map rows.
bool VoxRlCollectPlayerMapRows(PlayerTypes player, VoxRlPlayerMapRows& rows);

// Compares two portable row tables byte for byte.
template <typename Row>
inline bool VoxRlSameRows(const std::vector<Row>& left, const std::vector<Row>& right)
{
	return left.size() == right.size() &&
		(left.empty() || std::memcmp(&left[0], &right[0], left.size() * sizeof(Row)) == 0);
}

// Reports whether two player maps hold the same rows.
bool VoxRlPlayerMapRowsEqual(const VoxRlPlayerMapRows& left, const VoxRlPlayerMapRows& right);

// Appends one player's complete map to a REQUEST as its replacement.
void VoxRlAppendPlayerMapReplacement(const VoxRlPlayerMapRows& rows, VoxRlRequestData& data);

// Appends one player's map replacement when its current map differs from the cached one, and
// advances the cache to the current map.
bool VoxRlAppendChangedPlayerMap(PlayerTypes player, std::map<int, VoxRlPlayerMapRows>& cache,
	VoxRlRequestData& data);

// Selects the cached player maps that differ from the maps STATIC holds, for WORLD overrides.
void VoxRlCollectPlayerMapOverrides(const std::map<int, VoxRlPlayerMapRows>& staticMaps,
	const std::map<int, VoxRlPlayerMapRows>& currentMaps, std::vector<VoxRlPlayerMapRows>& overrides);

// Appends one spying player's espionage sight replacement when its current rows differ
// from the baseline, and advances the baseline to the current rows.
bool VoxRlAppendEspionageSightReplacement(PlayerTypes spyOwner,
	std::vector<CityEspionageSightRecord>& baseline, VoxRlRequestData& data);

// Collects the espionage sight rows of one spying player.
bool VoxRlCollectSpyOwnerEspionageRows(PlayerTypes spyOwner, std::vector<CityEspionageSightRecord>& rows);

// Collects one player's positive cached found values without refreshing the native cache.
bool VoxRlCollectPlayerFoundValueRows(PlayerTypes player, std::vector<PlayerFoundValueRecord>& rows);

// Appends one player's found-value replacement when its rows differ from the baseline.
bool VoxRlAppendFoundValueReplacement(PlayerTypes player,
	std::vector<PlayerFoundValueRecord>& baseline, VoxRlRequestData& data);

// Reserves storage and builds one complete STATIC block from the global
// defines, info tables, map topology, and player map. The player maps that
// went into the block are returned, one per player slot, so capture can tell
// which later maps differ from STATIC.
bool VoxRlBuildStaticBlock(const VoxRlBlockIdentity& identity,
	VoxRlOwnedBlockStorage& storage, unsigned int& length, std::vector<VoxRlPlayerMapRows>& playerMaps);

// Builds one complete WORLD block at the pre-refresh checkpoint for the
// capturing player. Each supplied player map differs from STATIC and
// replaces that player's STATIC map for this WORLD. When playerCitySnapshot
// is supplied, it receives the player and city tables that WORLD serialized.
bool VoxRlBuildWorldBlock(const VoxRlBlockIdentity& identity, PlayerTypes capturingPlayer,
	VoxRlOwnedBlockStorage& storage, unsigned int& length, VoxRlZoneSnapshot& zones,
	std::vector<TeamPassabilityRecord>& teamPassabilitySnapshot,
	std::vector<TeamResourceRecord>& teamResourceSnapshot,
	const std::vector<VoxRlPlayerMapRows>& playerMapOverrides,
	VoxRlWorldBuildTimings* timings = NULL,
	VoxRlPlayerCitySnapshot* playerCitySnapshot = NULL);

// Builds one complete CAMPAIGN block at the UpdateOperations entry for the
// capturing player. Zone data is read without triggering a refresh.
bool VoxRlBuildCampaignBlock(const VoxRlBlockIdentity& identity, PlayerTypes capturingPlayer,
	unsigned int alignedWorldGeneration, unsigned int alignedNextDeltaSequence,
	const std::map<int, PlayerRecord>& retainedPlayers,
	VoxRlOwnedBlockStorage& storage, unsigned int& length);

// Appends one complete operation snapshot and its army and slot versions to a
// request operation row. Empty version ranges remain available for no-op results.
bool VoxRlAppendOperationRecord(class CvAIOperation& operation, PlayerTypes initiatingPlayer,
	unsigned char kind, unsigned char changeCause, unsigned char invocationResult,
	signed char abortReason, VoxRlRequestData& data);

// Appends one immutable event-time unit snapshot and all of its request-local
// movement, sparse, modifier, plague, blocked-promotion, and attack-count children.
bool VoxRlAppendEventUnitSnapshot(class CvUnit& unit, TeamTypes observingTeam,
	VoxRlRequestData& data, int* eventUnitIndex);

// Collects one city's complete ordinary-building resource contribution rows for
// a replacement request. Zero contributions are omitted.
bool VoxRlCollectCityResourceRows(class CvCity& city,
	std::vector<RequestCityResourceRecord>& rows);

// Collects complete external resource replacement rows for selected players while
// sharing one map scan for their ordinary connected tile supplies.
bool VoxRlCollectPlayerResourceRows(const std::set<int>& players,
	std::vector<RequestPlayerResourceRecord>& rows);

// Collects one player's current balances and retained maintenance baseline for
// a turn-entry synchronization request.
bool VoxRlCollectPlayerEconomicsRecord(class CvPlayer& player,
	RequestPlayerEconomicsRecord& row);

// Builds one complete REQUEST block from staged rows.
bool VoxRlBuildRequestBlock(const VoxRlBlockIdentity& identity, VoxRlRequestData& data,
	VoxRlOwnedBlockStorage& storage, unsigned int& length);

// Collects one unit's plagues and blocked promotions into the sparse family
// records.
void VoxRlCollectUnitPlagueRows(PlayerTypes eOwner, int iUnitId, class CvUnit* pUnit,
	std::vector<UnitPlagueRecord>& plagues, std::vector<UnitBlockedPromotionRecord>& blockedPromotions);
// Collects the complete ordered queue and sparse promotion acquisition turns.
bool VoxRlCollectUnitTurnRows(class CvUnit& unit, std::vector<UnitMissionRecord>& missions,
	std::vector<UnitPromotionTurnRecord>& promotionTurns);
// Collects one unit's per-attacking-player counts into the sparse family
// records.
void VoxRlCollectUnitAttackCountRows(PlayerTypes eOwner, int iUnitId, class CvUnit* pUnit,
	std::vector<UnitAttackCountRecord>& rows);
void VoxRlCollectCityAttackCountRows(class CvCity* pCity, std::vector<CityAttackCountRecord>& rows);
void VoxRlCollectPlayerResistanceRows(class CvPlayer* pPlayer, std::vector<PlayerResistanceRecord>& rows);

// Collects the generated city fields and the builder-owned snapshots shared
// by WORLD builds and sparse city replacements.
bool VoxRlCollectCityRecord(class CvCity& city, PlayerTypes capturingPlayer, CityRecord& row);
// Collects a plot's mutable physical fields.
bool VoxRlCollectPlotRecord(class CvPlot& plot, PlotCaptureRecord& row);

// Collects the generated unit fields shared by WORLD builds and sparse unit replacements.
bool VoxRlCollectUnitRecord(class CvUnit& unit, TeamTypes capturingTeam, UnitRecord& row);

// Collects terrain and feature impassability for every team, including the
// barbarian team, from the info tables and each team's researched technology.
bool VoxRlCollectTeamPassabilityRows(std::vector<TeamPassabilityRecord>& rows);

// Collects ordinary reveal and improvement capabilities for every resource and
// each team identifier in the supplied checkpoint roster.
bool VoxRlCollectTeamResourceRows(const std::set<int>& teams,
	std::vector<TeamResourceRecord>& rows);

void VoxRlCollectAllUnitPlagueRows(std::vector<UnitPlagueRecord>& plagues,
	std::vector<UnitBlockedPromotionRecord>& blockedPromotions);
// Collects turn-state children for every live unit in WORLD.
bool VoxRlCollectAllUnitTurnRows(std::vector<UnitMissionRecord>& missions,
	std::vector<UnitPromotionTurnRecord>& promotionTurns);
void VoxRlCollectAllUnitAttackCountRows(std::vector<UnitAttackCountRecord>& rows);
void VoxRlCollectAllCityAttackCountRows(std::vector<CityAttackCountRecord>& rows);
void VoxRlCollectAllPlayerResistanceRows(std::vector<PlayerResistanceRecord>& rows);

struct STacticalAssignment;

// Copies collected WORLD-shaped rows into their mirrored request record
// shapes. The zone tables and six sparse replacement families (unit modifiers, plagues,
// blocked promotions, unit attack counts, player resistances, city attack
// counts) mirror their WORLD counterparts field for field; keyed delta
// mirrors such as the plot delta are not layout-identical and must not use
// this helper. The built-in size check rejects any other pairing at compile
// time.
template <typename To, typename From>
void VoxRlMirrorSparseRows(const std::vector<From>& from, std::vector<To>& to)
{
	typedef char VoxRlAssertMirrorSize[(sizeof(To) == sizeof(From)) ? 1 : -1];
	(void)sizeof(VoxRlAssertMirrorSize);
	to.resize(from.size());
	for (size_t index = 0; index < from.size(); ++index)
	{
		std::memcpy(&to[index], &from[index], sizeof(To));
	}
}

// Resolves the owner of a bare damage-ledger unit id following the item 7
// replay disposition: the smallest non-acting owner that has the unit, and
// the acting player when nobody else does.
int VoxRlResolveDamagedUnitOwner(PlayerTypes actingPlayer, int unitId);
// Resolves the owner of a damaged city through the assignment's target plot
// first, then by the smallest owner that has the city id.
int VoxRlResolveDamagedCityOwner(PlayerTypes actingPlayer, const STacticalAssignment& assignment);

// Collects native assignment results and appends their child damage and
// healing rows through the generated range helpers.
bool VoxRlCollectAssignmentRows(PlayerTypes actingPlayer,
	const std::vector<STacticalAssignment>& assignments, VoxRlResultData& data);

#endif
