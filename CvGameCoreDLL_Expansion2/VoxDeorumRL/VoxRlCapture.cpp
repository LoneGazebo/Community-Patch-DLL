// Installed from simulator/capture/VoxRlCapture.cpp. Edit that source, then reinstall.

// Vox Deorum: recording capture manager. See VoxRlCapture.h.
#include "CvGameCoreDLLPCH.h"
#include "VoxDeorumRL/VoxRlCapture.h"
#include "VoxDeorumRL/VoxRlCaptureBuilders.h"
#include "VoxDeorumRL/VoxRlCaptureMilitaryEvents.h"
#include "schema/VoxRlCollectors.generated.h"
#include "VoxDeorumRL/VoxRlCaptureFiles.h"
#include "VoxDeorumRL/schema/VoxRlBlockMetadata.h"
#include "VoxDeorumRL/schema/VoxRlBlockView.h"
#include "VoxDeorumRL/schema/VoxRlBlockStorage.h"
#include "VoxDeorumRL/schema/VoxRlBlockWriter.h"
#include "VoxDeorumRL/schema/MilitaryFlavors.h"

#include "../commit_id.inc"
#include "CvConnectionService.h"
#include "CvAIOperation.h"
#include "CvArmyAI.h"
#include "CvDangerPlots.h"
#include "CvGrandStrategyAI.h"
#include "CvTacticalAI.h"
#include "CvTacticalAnalysisMap.h"

#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <sstream>

namespace
{
	// REQUEST cannot add WORLD player identities or change their team membership.
	bool HasCheckpointPlayerRoster(const std::map<int, PlayerRecord>& players)
	{
		for (int index = 0; index < MAX_PLAYERS; ++index)
		{
			CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(index));
			const std::map<int, PlayerRecord>::const_iterator baseline = players.find(index);
			if (baseline == players.end())
			{
				if (player.isAlive() || index == BARBARIAN_PLAYER) return false;
			}
			else if (baseline->second.team != player.getTeam()) return false;
		}
		return true;
	}

	// Tracks the city scalar inputs that can change without a city setter notification,
	// including the military operands: heal rate, resistance, always-heal, building
	// defense percent, and remaining sapped (forced blockade) turns.
	struct CityCheckpointScalars
	{
		int productionAccumulated;
		int productionNeeded;
		int numWorkablePlots;
		int unitPurchaseCooldown;
		int civilianUnitPurchaseCooldown;
		int unitPurchaseCooldownMod;
		int civilianUnitPurchaseCooldownMod;
		int unitFaithPurchaseCooldown;
		int civilianUnitFaithPurchaseCooldown;
		int healRate;
		int resistance;
		int alwaysHeal;
		int buildingDefenseMod;
		int sappedTurns;
		// Copies the exact values carried by a WORLD or REQUEST city row.
		explicit CityCheckpointScalars(const CityRecord& row)
			: productionAccumulated(row.productionAccumulated),
			productionNeeded(row.productionNeeded), numWorkablePlots(row.numWorkablePlots),
			unitPurchaseCooldown(row.unitPurchaseCooldown),
			civilianUnitPurchaseCooldown(row.civilianUnitPurchaseCooldown),
			unitPurchaseCooldownMod(row.unitPurchaseCooldownMod),
			civilianUnitPurchaseCooldownMod(row.civilianUnitPurchaseCooldownMod),
			unitFaithPurchaseCooldown(row.unitFaithPurchaseCooldown),
			civilianUnitFaithPurchaseCooldown(row.civilianUnitFaithPurchaseCooldown),
			healRate(row.healRate), resistance(row.resistance), alwaysHeal(row.alwaysHeal),
			buildingDefenseMod(row.buildingDefenseMod), sappedTurns(row.sappedTurns) {}
		// Reads the current native values before deciding whether to mark a city dirty.
		explicit CityCheckpointScalars(CvCity& city)
			: productionAccumulated(city.getProduction()),
			productionNeeded(city.getProductionNeeded()), numWorkablePlots(city.GetNumWorkablePlots()),
			unitPurchaseCooldown(city.GetUnitPurchaseCooldown(false)),
			civilianUnitPurchaseCooldown(city.GetUnitPurchaseCooldown(true)),
			unitPurchaseCooldownMod(city.GetUnitPurchaseCooldownMod(false)),
			civilianUnitPurchaseCooldownMod(city.GetUnitPurchaseCooldownMod(true)),
			unitFaithPurchaseCooldown(city.GetUnitFaithPurchaseCooldown(false)),
			civilianUnitFaithPurchaseCooldown(city.GetUnitFaithPurchaseCooldown(true)),
			healRate(city.getHealRate()), resistance(city.IsResistance() ? 1 : 0),
			alwaysHeal(city.GetAlwaysHeal()),
			buildingDefenseMod(city.GetCityBuildings()->GetBuildingDefenseMod()),
			sappedTurns(city.GetSappedTurns()) {}
		// Compares the scalar inputs whose native setters do not notify capture.
		bool operator!=(const CityCheckpointScalars& other) const
		{ return productionAccumulated != other.productionAccumulated ||
			productionNeeded != other.productionNeeded || numWorkablePlots != other.numWorkablePlots ||
			unitPurchaseCooldown != other.unitPurchaseCooldown ||
			civilianUnitPurchaseCooldown != other.civilianUnitPurchaseCooldown ||
			unitPurchaseCooldownMod != other.unitPurchaseCooldownMod ||
			civilianUnitPurchaseCooldownMod != other.civilianUnitPurchaseCooldownMod ||
			unitFaithPurchaseCooldown != other.unitFaithPurchaseCooldown ||
			civilianUnitFaithPurchaseCooldown != other.civilianUnitFaithPurchaseCooldown ||
			healRate != other.healRate || resistance != other.resistance ||
			alwaysHeal != other.alwaysHeal || buildingDefenseMod != other.buildingDefenseMod ||
			sappedTurns != other.sappedTurns; }
	};

	// Keeps lazy native danger queries from reentering capture during collection.
	class ScopedCaptureCollection
	{
	public:
		// Preserve the enclosing collection state, including nested scopes.
		explicit ScopedCaptureCollection(bool& collecting)
			: m_collecting(collecting), m_previous(collecting)
		{
			m_collecting = true;
		}
		// Restore capture callbacks on every exit, including failed collection.
		~ScopedCaptureCollection()
		{
			m_collecting = m_previous;
		}
	private:
		bool& m_collecting;
		bool m_previous;
		ScopedCaptureCollection(const ScopedCaptureCollection&);
		ScopedCaptureCollection& operator=(const ScopedCaptureCollection&);
	};

	// Reads the opt-in once at DLL load, before any game lifecycle hooks run.
	bool ReadCaptureEnabled()
	{
		const char* enabled = getenv("VOX_RL_CAPTURE");
		return enabled != NULL && enabled[0] == '1';
	}

	// Checks paired trade row sizes in the DLL's C++03 build.
	typedef char VoxRlAssertTradeConnectionMirror[
		(sizeof(TradeConnectionRecord) == sizeof(RequestTradeConnectionRecord)) ? 1 : -1];
	typedef char VoxRlAssertTradePathMirror[
		(sizeof(TradePathPlotRecord) == sizeof(RequestTradePathPlotRecord)) ? 1 : -1];

	// Emits the complete trade roster when routes or their fixed paths changed.
	bool AppendChangedTradeRoster(VoxRlRequestData& current,
		std::vector<TradeConnectionRecord>& lastRoutes,
		std::vector<TradePathPlotRecord>& lastPaths, VoxRlRequestData& data)
	{
		std::vector<TradeConnectionRecord> routes(current.requestTradeConnections.size());
		std::vector<TradePathPlotRecord> paths(current.requestTradePathPlots.size());
		if (!routes.empty()) std::memcpy(&routes[0], &current.requestTradeConnections[0], routes.size() * sizeof(TradeConnectionRecord));
		if (!paths.empty()) std::memcpy(&paths[0], &current.requestTradePathPlots[0], paths.size() * sizeof(TradePathPlotRecord));
		if (VoxRlSameRows(routes, lastRoutes) && VoxRlSameRows(paths, lastPaths)) return false;
		data.requestTradeRoster.swap(current.requestTradeRoster);
		data.requestTradeConnections.swap(current.requestTradeConnections);
		data.requestTradePathPlots.swap(current.requestTradePathPlots);
		lastRoutes.swap(routes);
		lastPaths.swap(paths);
		return true;
	}

	// Appends a coupled city and resource replacement when either snapshot changed.
	bool AppendChangedCityReplacement(const VoxRlEntityKey& key,
		const RequestDeltaCityRecord& city, const std::vector<RequestCityResourceRecord>& resources,
		std::map<VoxRlEntityKey, RequestDeltaCityRecord>& lastCities,
		std::map<VoxRlEntityKey, std::vector<RequestCityResourceRecord> >& lastResources,
		VoxRlRequestData& data)
	{
		std::map<VoxRlEntityKey, RequestDeltaCityRecord>::const_iterator lastCity = lastCities.find(key);
		std::map<VoxRlEntityKey, std::vector<RequestCityResourceRecord> >::const_iterator lastResource =
			lastResources.find(key);
		if (lastCity != lastCities.end() &&
			std::memcmp(&lastCity->second, &city, sizeof(city)) == 0 &&
			lastResource != lastResources.end() && VoxRlSameRows(lastResource->second, resources)) return false;
		lastCities[key] = city;
		lastResources[key] = resources;
		data.requestDeltaCities.push_back(city);
		data.requestCityResources.insert(data.requestCityResources.end(), resources.begin(), resources.end());
		return true;
	}

	// Keeps relation deltas inside the directed team roster captured by WORLD.
	bool IsCapturedRelation(const TeamRelationRecord& relation, const std::set<int>& teams)
	{
		return relation.team != relation.otherTeam &&
			teams.count(static_cast<int>(relation.team)) != 0 &&
			teams.count(static_cast<int>(relation.otherTeam)) != 0;
	}

	// Marks retained cities whose derived military garrison need changed.
	void RefreshChangedNeedsGarrison(std::map<VoxRlEntityKey, unsigned char>& previous,
		std::set<VoxRlEntityKey>& dirtyCities)
	{
		for (std::map<VoxRlEntityKey, unsigned char>::iterator baseline = previous.begin();
			baseline != previous.end(); ++baseline)
		{
			const VoxRlEntityKey& key = baseline->first;
			CvCity* pCity = GET_PLAYER(static_cast<PlayerTypes>(key.owner)).getCity(key.id);
			if (pCity == NULL) continue;
			const unsigned char needsGarrison = pCity->NeedsGarrison() ? 1 : 0;
			if (baseline->second == needsGarrison) continue;
			dirtyCities.insert(key);
			baseline->second = needsGarrison;
		}
	}

	// Adds one scoped interval to a nanosecond total while timings are enabled.
	class ScopedTiming
	{
	public:
		ScopedTiming(bool enabled, unsigned __int64& total)
			: m_enabled(enabled), m_total(total), m_start(enabled ? VoxRlTimingNanoseconds() : 0)
		{
		}
		~ScopedTiming()
		{
			if (m_enabled) m_total += VoxRlTimingNanoseconds() - m_start;
		}
	private:
		bool m_enabled;
		unsigned __int64& m_total;
		unsigned __int64 m_start;
	};
}

const bool gVoxRlCaptureEnabled = ReadCaptureEnabled();

// Per-segment recording state. One segment is active at a time and spans
// one player turn between the pre-danger checkpoint and the next checkpoint.
struct VoxRlCapture::Segment
{
	PlayerTypes player;
	int turn;
	unsigned int staticGeneration;
	unsigned int worldGeneration;
	unsigned int campaignGeneration;

	std::string relativeSegmentDir;
	std::string streamPath;
	std::string indexPath;
	std::string staticRelPath;
	std::string campaignRelPath;
	std::string worldRelPath;
	unsigned int staticFramedLength;
	unsigned int campaignFramedLength;
	unsigned int worldFramedLength;

	unsigned int nextDeltaSequence;
	bool published;
	bool failed;
	bool closed;
	bool campaignSeamReached;
	bool worldReplacement;
	// True once a decision request frame has been enqueued. Publication
	// waits for the first decision so segments that never decide leave no
	// files behind.
	bool hasDecision;
	char closureReason[40];

	// Pending frames collected before a compatible CAMPAIGN permits
	// publication. Bounded by the pending byte budget.
	std::vector<VoxRlFrameEntry> frameTable;
	std::vector<unsigned char> pendingBytes;
	// The checkpoint WORLD stays in its completed owned storage until the
	// campaign seam permits publication, so unpublished observers leave no
	// WORLD file behind and no duplicate bytes are held. Directories follow
	// the same rule: they are created only when a file is about to be written.
	VoxRlOwnedBlockStorage worldStorage;
	VoxRlZoneSnapshot zoneSnapshot;
	unsigned int pendingCount;
	unsigned int committedFrameCount;
	unsigned int committedDecisionCount;
	unsigned int committedCoverageCount;
	unsigned int commitId;
	unsigned int firstRequestSequence;
	unsigned int lastRequestSequence;
	bool hasRequests;

	struct DecisionRow
	{
		unsigned int decisionId;
		int attemptIndex;
		int caller;
		int retryOutcome;
		unsigned int requestFrameIndex;
		unsigned int resultFrameIndex;
		bool hasPrevious;
		int previousAttemptIndex;
		unsigned int previousWorldGeneration;
	};
	std::vector<DecisionRow> decisions;
	// Coverage omission reasons accumulated since the previous commit. They
	// render as one coverage line per reason at commit time, and a segment
	// that never publishes carries them into its suppressed.jsonl line.
	std::vector<std::string> coverageReasons;

	// Dirty sets filled by setter notifications.
	std::set<VoxRlEntityKey> dirtyUnits;
	std::set<VoxRlEntityKey> removedUnits;
	std::set<VoxRlEntityKey> dirtyCities;
	std::set<VoxRlEntityKey> removedCities;
	std::set<int> dirtyPlots;
	std::map<VoxRlVisibilityKey, unsigned char> visibilityFlips;
	std::set<int> visibilityResets;                                    // known-visibility reset teams
	// Team ids present in the WORLD visibility sections. Delta visibility and
	// revealed-override rows must stay within this baseline-owned domain.
	std::set<int> capturedTeams;
	// Directed relation changes are valid only for the teams in the WORLD relation roster.
	std::set<int> capturedRelationTeams;
	std::map<VoxRlEntityKey, unsigned char> revealedOverrideUpserts;  // team-major key, 1 = present
	std::set<VoxRlEntityKey> removedRevealedOverrides;                // (team, plot)
	std::set<int> dirtyInterceptors;
	std::vector<RequestDangerEventRecord> pendingDangerEvents;
	// Sparse family dirty entity sets: creation, promotion, and counter hooks mark
	// only the affected entities, and removals drop their keys from collection work.
	std::set<VoxRlEntityKey> dirtyUnitPlagues;
	std::set<VoxRlEntityKey> dirtyUnitBlockedPromotions;
	std::set<VoxRlEntityKey> dirtyUnitAttackCounts;
	std::set<int> dirtyPlayerResistances;
	std::set<VoxRlEntityKey> dirtyCityAttackCounts;

	// Last emitted plot, city, and city-resource records for omit-unchanged comparison.
	// Only entities emitted as deltas in this segment are retained.
	std::map<VoxRlEntityKey, PlotCaptureRecord> lastPlotRows;
	std::map<VoxRlEntityKey, RequestDeltaCityRecord> lastCityRows;
	std::map<VoxRlEntityKey, std::vector<RequestCityResourceRecord> > lastCityResourceRows;
	std::map<VoxRlEntityKey, unsigned char> lastCityNeedsGarrison;
	std::map<VoxRlEntityKey, CityCheckpointScalars> lastCityCheckpointScalars;
	std::map<int, TeamPassabilityRecord> lastTeamPassabilityRows;
	std::map<int, TeamResourceRecord> lastTeamResourceRows;
	// Tracks the serialized WORLD value so REQUEST emits only native game-state changes.
	WorldGameStateRecord lastGameState;
	VoxRlPlayerCitySnapshot playerCitySnapshot;
	std::vector<TradeConnectionRecord> lastTradeConnections;
	std::vector<TradePathPlotRecord> lastTradePathPlots;
	// Latest player rows, seeded by WORLD and advanced with emitted replacements.
	std::map<int, PlayerRecord> lastPlayerRows;
	// The actor's espionage sight rows as the spying player, seeded by WORLD and advanced
	// with emitted replacements.
	std::vector<CityEspionageSightRecord> lastEspionageRows;
	// Holds the actor's found-value rows that WORLD and earlier REQUESTs gave the reader.
	std::vector<PlayerFoundValueRecord> lastFoundValueRows;
	// Set while the pre-tactical REQUEST collects the actor's outside-turn state: its
	// player map and its espionage sight rows.
	bool collectLateTurnState;
	// The zone-id-to-row decode table of the currently accepted zone snapshot;
	// rebuilt only when a replacement accepts a new table.
	std::map<int, unsigned int> zoneRowByZoneId;

	// Iteration index bookkeeping across deltas.
	std::map<VoxRlEntityKey, unsigned int> unitIteration;
	std::map<VoxRlEntityKey, unsigned int> cityIteration;
	std::map<int, unsigned int> nextUnitIteration;
	std::map<int, unsigned int> nextCityIteration;

	// Staged synchronization request during a refresh or discovery window.
	bool stagingRequest;
	VoxRlRequestData staged;

	// Opt-in timing accumulators for this segment, reported at closure. All
	// time members are nanoseconds from the high-resolution performance
	// counter; the byte and count members carry the measured volumes.
	struct Timings
	{
		unsigned __int64 worldBuildNs;
		VoxRlWorldBuildTimings worldPhases;
		unsigned __int64 worldWriteFlushNs;
		unsigned __int64 campaignConstructNs;
		unsigned __int64 campaignWriteFlushNs;
		unsigned __int64 deltaCollectNs;
		unsigned __int64 requestBuildNs;
		unsigned __int64 resultBuildNs;
		unsigned __int64 pendingBufferNs;
		unsigned __int64 pendingPublishWriteNs;
		unsigned __int64 streamWriteNs;
		unsigned __int64 commitNs;
		unsigned __int64 streamFlushNs;
		unsigned __int64 indexFlushNs;
		unsigned __int64 worldBuildBytes;
		unsigned __int64 campaignBuildBytes;
		unsigned __int64 requestBytes;
		unsigned __int64 resultBytes;
		unsigned __int64 pendingBufferBytes;
		unsigned __int64 pendingPublishWriteBytes;
		unsigned __int64 streamWriteBytes;
		unsigned __int64 indexBytes;
		unsigned __int64 deltaRows;
		unsigned int deltaCollectCount;
		unsigned int requestCount;
		unsigned int resultCount;
		unsigned int commitCount;
		Timings()
			: worldBuildNs(0), worldWriteFlushNs(0), campaignConstructNs(0), campaignWriteFlushNs(0),
			deltaCollectNs(0), requestBuildNs(0), resultBuildNs(0), pendingBufferNs(0),
			pendingPublishWriteNs(0), streamWriteNs(0), commitNs(0), streamFlushNs(0), indexFlushNs(0),
			worldBuildBytes(0), campaignBuildBytes(0), requestBytes(0), resultBytes(0),
			pendingBufferBytes(0), pendingPublishWriteBytes(0), streamWriteBytes(0), indexBytes(0), deltaRows(0),
			deltaCollectCount(0), requestCount(0), resultCount(0), commitCount(0)
		{
		}
	};
	Timings timings;

	Segment()
		: player(NO_PLAYER), turn(-1), staticGeneration(0), worldGeneration(0), campaignGeneration(0),
		staticFramedLength(0), campaignFramedLength(0), worldFramedLength(0),
		nextDeltaSequence(0), published(false), failed(false), closed(false), campaignSeamReached(false),
		worldReplacement(false), hasDecision(false), collectLateTurnState(false),
		pendingCount(0), committedFrameCount(0), committedDecisionCount(0), committedCoverageCount(0),
		commitId(0), firstRequestSequence(0), lastRequestSequence(0), hasRequests(false),
		stagingRequest(false)
	{
		std::memset(&lastGameState, 0, sizeof(lastGameState));
		closureReason[0] = '\0';
	}
};

// Player maps by player slot: the rows the current STATIC holds, and each player's latest
// rows. A player's map changes only in its own turn, so capture refreshes that player's
// entry at its turn completion and compares it at its pre-tactical REQUEST.
struct VoxRlCapture::PlayerMapCache
{
	std::map<int, VoxRlPlayerMapRows> staticMaps;
	std::map<int, VoxRlPlayerMapRows> currentMaps;
};

// One caller engagement: all attempts of one FindAndExecuteBestUnitAssignments
// or PerformRangedOpportunityAttack invocation share its decision id.
struct VoxRlCapture::Engagement
{
	bool active;
	int callerType;
	PlayerTypes player;
	unsigned int decisionId;
	int attemptIndex;
	int lastOutcome;
	unsigned int lastAttemptWorldGeneration;
	bool lastSearchEmpty;
	Engagement()
		: active(false), callerType(0), player(NO_PLAYER), decisionId(0), attemptIndex(0),
		lastOutcome(0), lastAttemptWorldGeneration(0), lastSearchEmpty(false)
	{
	}
};

// One nested operation label and optional request parent.
struct VoxRlCapture::OperationCauseContext
{
	PlayerTypes owner;
	PlayerTypes initiatingPlayer;
	int cause;
	int operationId;
	OperationCauseContext(PlayerTypes eOwner, PlayerTypes eInitiator, int changeCause, int id)
		: owner(eOwner), initiatingPlayer(eInitiator), cause(changeCause), operationId(id)
	{
	}
};

// Attachment-scoped operational capture state is filled by the military
// implementation below and survives segment replacement.
struct VoxRlCapture::OperationCaptureState
{
	std::map<VoxRlEntityKey, std::string> operationFingerprints;
	std::map<VoxRlEntityKey, int> operationAbortReasons;
	std::map<VoxRlEntityKey, int> operationCauses;
	std::map<VoxRlEntityKey, PlayerTypes> operationInitiators;
	// Retains the first stop context through later validation and deferred removal.
	void RememberOperationCause(const VoxRlEntityKey& key, int cause,
		PlayerTypes initiatingPlayer, int abortReason)
	{
		if (operationAbortReasons.count(key) != 0 && operationAbortReasons[key] != NO_ABORT_REASON) return;
		operationAbortReasons[key] = abortReason;
		operationCauses[key] = cause;
		operationInitiators[key] = initiatingPlayer;
	}
	std::map<int, VoxRlRequestData> pendingRowsByOwner;
	std::vector<RequestZoneChoiceRecord> pendingZoneChoices;
	bool stanceAssessmentValid;
	bool stanceChoicesReady;
	bool stanceBatchPublished;
	PlayerTypes assessmentPlayer;
	int assessmentTurn;
	unsigned int assessmentWorldGeneration;
	unsigned int assessmentDeltaSequence;
	bool collectTurnEntryEconomics;
	std::vector<TeamRelationRecord> bufferedRelations;
	size_t stagedRelationCount;
	bool relationsSeeded;
	OperationCaptureState()
		: stanceAssessmentValid(false), stanceChoicesReady(false), stanceBatchPublished(false),
		assessmentPlayer(NO_PLAYER), assessmentTurn(-1), assessmentWorldGeneration(0),
		assessmentDeltaSequence(0), collectTurnEntryEconomics(false),
		stagedRelationCount(0), relationsSeeded(false)
	{
	}
};

VoxRlCaptureConfig::VoxRlCaptureConfig()
	: enabled(false), filterPlayers(false), filterTurns(false), timings(false)
{
}

namespace
{
	// Memory ceiling for frames waiting for publication. Exceeding it
	// closes the affected segment at its last commit.
	const unsigned int kVoxRlPendingBudgetBytes = 8U * 1024U * 1024U;

	void AppendNumber(std::string& out, unsigned __int64 value)
	{
		char buffer[24];
		sprintf_s(buffer, 24, "%I64u", value);
		out += buffer;
	}

	void AppendInt(std::string& out, int value)
	{
		char buffer[16];
		sprintf_s(buffer, 16, "%d", value);
		out += buffer;
	}

	// Minimal JSON string escaping for controlled producer text.
	void AppendJsonString(std::string& out, const char* text)
	{
		out += '"';
		for (const char* cursor = text; *cursor != '\0'; ++cursor)
		{
			if (*cursor == '"' || *cursor == '\\')
			{
				out += '\\';
				out += *cursor;
			}
			else if (*cursor == '\n')
			{
				out += "\\n";
			}
			else
			{
				out += *cursor;
			}
		}
		out += '"';
	}

	// The manifest hash text: eight 32-bit words in order.
	std::string ManifestHashText()
	{
		std::string text;
		char buffer[16];
		for (int word = 0; word < 8; ++word)
		{
			sprintf_s(buffer, 16, "%08x", static_cast<unsigned int>(kVoxRlManifestHashWords[word]));
			text += buffer;
		}
		return text;
	}

	// Counts the rows one collected request carries across every section.
	unsigned __int64 CountRequestRows(const VoxRlRequestData& data)
	{
		return data.requestDangerEvents.size() + data.requestTeamRelations.size() +
			(data.hasRequestGameState ? 1U : 0U) +
			data.requestDeltaUnits.size() + data.requestDeltaPlots.size() +
			data.requestDeltaPlotZones.size() + data.requestDeltaPlotZonesWide.size() +
			data.requestZoneReplacements.size() + data.requestZoneNeighbors.size() +
			data.requestDeltaCities.size() + data.requestRemovedUnits.size() +
			data.requestRemovedCities.size() + data.requestDeltaTeams.size() +
			data.requestTeamPassability.size() + data.requestTeamResources.size() +
			data.requestTeamTechnologyReplacements.size() + data.requestTeamTechnologyRows.size() +
			data.requestPlayerSpecialUpgradeReplacements.size() + data.requestPlayerSpecialUpgradeRows.size() +
			data.requestPlayerSavingsReplacements.size() + data.requestPlayerSavingsRows.size() +
			data.requestPlayerRelationReplacements.size() + data.requestPlayerRelationRows.size() +
			data.requestPlayerFlavorReplacements.size() + data.requestPlayerFlavorRows.size() +
			data.requestPlayerFreePromotionReplacements.size() + data.requestPlayerFreePromotionRows.size() +
			data.requestPlayerTraitPromotionCombatReplacements.size() + data.requestPlayerTraitPromotionCombatRows.size() +
			data.requestPlayerTraitPromotionClassReplacements.size() + data.requestPlayerTraitPromotionClassRows.size() +
			data.requestCityConnectionReplacements.size() + data.requestCityConnectionRows.size() +
			data.requestTradeRoster.size() + data.requestTradeConnections.size() +
			data.requestTradePathPlots.size() +
			data.requestCityPurchaseCostReplacements.size() + data.requestCityPurchaseCostRows.size() +
			data.requestCityFreePromotionReplacements.size() + data.requestCityFreePromotionRows.size() +
			data.requestVisibilityWords.size() + data.requestVisibilityResets.size() + data.requestRevealedOverrideUpserts.size() +
			data.requestRemovedRevealedOverrides.size() + data.requestKnownAttackers.size() +
			data.requestInterceptorReplacements.size() + data.requestInterceptorEntries.size() +
			data.requestParticipants.size() + data.requestDroppedUnits.size() +
			data.requestUnitPlagueReplacements.size() + data.requestUnitPlagueRows.size() +
			data.requestUnitMissionReplacements.size() + data.requestUnitMissionRows.size() +
			data.requestUnitPromotionTurnReplacements.size() + data.requestUnitPromotionTurnRows.size() +
			data.requestUnitBlockedPromotionReplacements.size() + data.requestUnitBlockedPromotionRows.size() +
			data.requestUnitAttackCountReplacements.size() + data.requestUnitAttackCountRows.size() +
			data.requestPlayerResistanceReplacements.size() + data.requestPlayerResistanceRows.size() +
		data.requestCityAttackCountReplacements.size() + data.requestCityAttackCountRows.size() +
		data.requestDeltaUnitSparseFields.size() +
		data.requestDeltaPlotSparseCounters.size() + data.requestCityReferences.size() +
		data.requestDeltaPlayers.size() + data.requestOperations.size() +
		data.requestOperationVersions.size() + data.requestArmyVersions.size() +
		data.requestFormationEntryVersions.size() + data.requestCampaignBoundaries.size() +
		data.requestZoneChoices.size() + data.requestFocusAreas.size() +
		data.requestWorkerAssignments.size() +
		data.requestCityResources.size() + data.requestPlayerResources.size() +
		data.requestPlayerEconomics.size() + data.requestEconomicBatches.size() +
		data.requestMilitaryGoldTransactions.size() + data.requestEventUnits.size() +
		data.requestEventUnitSparseFields.size() + data.requestEventUnitPlagues.size() +
		data.requestEventUnitBlockedPromotions.size() + data.requestEventUnitAttackCounts.size() +
		data.requestEventUnitMissions.size() + data.requestEventUnitPromotionTurns.size() +
		data.requestMilitaryArrivals.size() + data.requestMilitaryDepartures.size() +
		data.requestBarbarianCampCreations.size() + data.requestDamage.size() +
		data.requestPlayerMapPlayers.size() + data.requestPlayerMapUnitClasses.size() +
		data.requestPlayerMapUnitCombatCosts.size() +
		data.requestCityEspionageSightReplacements.size() + data.requestCityEspionageSightRows.size() +
		data.requestPlayerFoundValueReplacements.size() + data.requestPlayerFoundValueRows.size();
}

	// Successful completion is terminal before Kill assigns its final native reason.
	int OperationRemovalReason(CvAIOperation& operation)
	{
		return operation.GetOperationState() == AI_OPERATION_STATE_SUCCESSFUL_FINISH
			? AI_ABORT_SUCCESS : operation.GetAbortReason();
	}

	// Produces a stable native fingerprint for one operation and all child armies.
	std::string OperationFingerprint(CvAIOperation& operation)
	{
		std::ostringstream out;
		out << static_cast<int>(operation.GetOwner()) << ',' << operation.GetID() << ','
			<< static_cast<int>(operation.GetOperationType()) << ','
			<< static_cast<int>(operation.GetArmyType()) << ','
			<< static_cast<int>(operation.GetOperationState()) << ','
			<< static_cast<int>(operation.GetEnemy()) << ','
			<< operation.GetMusterX() << ',' << operation.GetMusterY() << ','
			<< operation.GetTargetX() << ',' << operation.GetTargetY() << ','
			<< operation.GetTurnStarted() << ',' << operation.GetDistanceMusterToTarget() << ','
			<< static_cast<int>(operation.GetAbortReason()) << ',' << operation.GetLastTurnMoved();
		for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
			out << ",f:" << operation.GetTacticalFlavorModifier(slot);
		const std::vector<int>& armyIds = operation.GetArmyIDs();
		for (size_t armyIndex = 0; armyIndex < armyIds.size(); ++armyIndex)
		{
			CvArmyAI* army = GET_PLAYER(operation.GetOwner()).getArmyAI(armyIds[armyIndex]);
			out << ";a:" << armyIds[armyIndex];
			if (army == NULL) continue;
			out << ',' << static_cast<int>(army->GetFormationType()) << ','
				<< static_cast<int>(army->GetArmyAIState()) << ','
				<< army->GetGoalX() << ',' << army->GetGoalY() << ','
				<< army->GetNumSlotsFilled();
			for (size_t slotIndex = 0; slotIndex < army->GetNumFormationEntries(); ++slotIndex)
			{
				CvArmyFormationSlot* slot = army->GetSlotStatus(slotIndex);
				if (slot == NULL) continue;
				out << ";s:" << slotIndex << ',' << slot->GetUnitID() << ','
					<< (slot->IsRequired() ? 1 : 0);
				for (size_t history = 0; history < 3; ++history)
					out << ',' << slot->GetTurnsToCheckpoint(history);
			}
		}
		return out.str();
	}
}

VoxRlCapture& VoxRlCapture::GetInstance()
{
	static VoxRlCapture instance;
	return instance;
}

VoxRlCapture::VoxRlCapture()
	: m_configResolved(false),
	m_rootResolved(false),
	m_gameUuidKnown(false),
	m_staticGeneration(0),
	m_staticGenerationWritten(false),
	m_topologyInvalidated(false),
	m_worldReplacementPending(false),
	m_decisionIdCounter(0),
	m_identityPendingLogged(false),
	m_staticConstructNs(0),
	m_staticWriteFlushNs(0),
	m_staticBuildBytes(0),
	m_zoneSnapshotDirty(false),
	m_teamPassabilityDirty(false),
	m_campaignTurnPlayer(0xFFFFFFFF),
	m_campaignTurn(-1),
	m_campaignStaticGeneration(0),
	m_campaignGeneration(0),
	m_campaignAlignedWorldGeneration(0),
	m_campaignAlignedNextDeltaSequence(0),
	m_campaignFramedLength(0),
	m_segmentStreamBytes(0),
	m_segment(NULL),
	m_engagement(NULL),
	m_playerMaps(NULL),
	m_operationState(NULL),
	m_operationCauseStack(NULL),
	m_searchActive(false),
	m_collectingState(false),
	m_shuttingDown(false),
	m_concluded(false),
	m_worldReplacementLogged(false)
{
	std::memset(&m_gameUuid, 0, sizeof(m_gameUuid));
	std::memset(m_gameUuidText, 0, sizeof(m_gameUuidText));
	m_operationState = new OperationCaptureState();
	m_operationCauseStack = new std::vector<OperationCauseContext>();
	m_playerMaps = new PlayerMapCache();
}

VoxRlCapture::~VoxRlCapture()
{
	delete m_segment;
	m_segment = NULL;
	delete m_engagement;
	m_engagement = NULL;
	delete m_operationState;
	m_operationState = NULL;
	delete m_operationCauseStack;
	m_operationCauseStack = NULL;
	delete m_playerMaps;
	m_playerMaps = NULL;
}

bool VoxRlCapture::IsObserving(PlayerTypes ePlayer) const
{
	return m_segment != NULL && !m_segment->failed && !m_segment->closed &&
		m_segment->player == ePlayer && m_gameUuidKnown;
}

bool VoxRlCapture::DiscoverGameUuid()
{
	if (m_gameUuidKnown)
	{
		return true;
	}
	std::string uuidText;
	if (!CvConnectionService::GetInstance().TryReadGameUuid(uuidText))
	{
		return false;
	}
	// Canonical UUID text: 36 lowercase hex digits with hyphens at the
	// canonical positions. Reject anything else without arming capture.
	if (uuidText.size() != 36 || uuidText[8] != '-' || uuidText[13] != '-' ||
		uuidText[18] != '-' || uuidText[23] != '-')
	{
		return false;
	}
	for (size_t index = 0; index < uuidText.size(); ++index)
	{
		const char c = uuidText[index];
		const bool digit = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
		if (!digit && c != '-')
		{
			return false;
		}
	}
	unsigned int word = 0;
	int digitsInWord = 0;
	int wordIndex = 0;
	for (size_t index = 0; index < uuidText.size(); ++index)
	{
		if (uuidText[index] == '-') continue;
		const char c = uuidText[index];
		unsigned int digit = 0;
		if (c >= '0' && c <= '9') digit = static_cast<unsigned int>(c - '0');
		else digit = static_cast<unsigned int>(c - 'a' + 10);
		word = (word << 4) | digit;
		if (++digitsInWord == 8)
		{
			m_gameUuid.words[wordIndex] = word;
			++wordIndex;
			word = 0;
			digitsInWord = 0;
		}
	}
	if (wordIndex != 4)
	{
		return false;
	}
	m_gameUuidKnown = true;
	VoxRlFormatGameUuidText(m_gameUuid, m_gameUuidText);
	return true;
}

bool VoxRlCapture::ReadGameUuidThroughLua(std::string& uuidText)
{
	return CvConnectionService::GetInstance().TryReadGameUuid(uuidText);
}

bool VoxRlCapture::ResolveConfiguration()
{
	if (m_configResolved)
	{
		return m_config.enabled;
	}
	m_configResolved = true;
	m_config.enabled = gVoxRlCaptureEnabled;
	if (!m_config.enabled)
	{
		return false;
	}
	const char* root = getenv("VOX_RL_CAPTURE_ROOT");
	if (root != NULL && root[0] != '\0')
	{
		m_config.rootOverride = root;
	}
	const char* players = getenv("VOX_RL_CAPTURE_PLAYERS");
	if (players != NULL && players[0] != '\0')
	{
		m_config.filterPlayers = true;
		std::stringstream stream(players);
		std::string entry;
		while (std::getline(stream, entry, ','))
		{
			if (entry.empty()) continue;
			m_config.players.insert(atoi(entry.c_str()));
		}
	}
	// The opt-in producer timings accumulate in memory and report at segment
	// closure, so an armed game pays one branch per timing point.
	const char* timings = getenv("VOX_RL_CAPTURE_TIMINGS");
	m_config.timings = timings != NULL && timings[0] == '1';
	return true;
}

bool VoxRlCapture::ResolveCaptureRoot()
{
	if (m_rootResolved)
	{
		return !m_captureRoot.empty();
	}
	m_rootResolved = true;
	if (!m_config.rootOverride.empty())
	{
		m_captureRoot = m_config.rootOverride;
	}
	else
	{
		// The engine exposes the per-user cache folder, which always sits
		// inside Civ V's per-user game folder. Derive that folder rather
		// than resolving Documents independently in the DLL.
		const char* cachePath = gDLL->GetCacheFolderPath();
		if (cachePath == NULL || cachePath[0] == '\0')
		{
			return false;
		}
		std::string gameFolder;
		if (!VoxRlStripLastPathComponent(cachePath, gameFolder))
		{
			return false;
		}
		m_captureRoot = gameFolder;
		m_captureRoot += "/VoxDeorumRL";
	}
	// The root is only resolved here; the directory chain is created when
	// the first file is written, so an attachment that never publishes
	// leaves no folders behind.
	return true;
}

bool VoxRlCapture::AdmitsPlayer(PlayerTypes ePlayer) const
{
	// The barbarian slot can be included in the explicit environment filter,
	// but it never reaches the campaign seam and therefore cannot publish.
	if (ePlayer == BARBARIAN_PLAYER) return false;
	// Campaign capture is driven by CvMilitaryAI::UpdateOperations, which
	// only runs for major civilizations. Keep minor observers out of the
	// recording lifecycle while WORLD data still includes their entities.
	if (!GET_PLAYER(ePlayer).isMajorCiv()) return false;
	if (!m_config.filterPlayers) return true;
	return m_config.players.count(static_cast<int>(ePlayer)) != 0;
}

unsigned int VoxRlCapture::ReserveGeneration(const std::string& directory, const char* prefix)
{
	// Generation numbers count files within one target directory and
	// prefix: every kind starts at 1 and the search advances past
	// existing files, so prior or unfinished files are never
	// overwritten and process restarts are covered. A restarted
	// capture may select a free lower number in an existing directory,
	// so generation order is not chronological order. The numbers are
	// independent per kind; identity remains the existing game, turn,
	// player, kind, and generation combination.
	unsigned int candidate = 1;
	while (true)
	{
		char suffix[32];
		sprintf_s(suffix, 32, "/%s-%u.bin", prefix, candidate);
		const std::string path = directory + suffix;
		if (!VoxRlFileExists(path.c_str()))
		{
			return candidate;
		}
		++candidate;
		if (candidate == 0) return 0;
	}
}

void VoxRlCapture::OnGameStartOrLoad()
{
	// A load or new game ends any engagement and unfinished segment; the
	// identity is re-read from the loaded save at the next checkpoint.
	EndEngagement();
	CloseSegment("gameLoad");
	m_gameUuidKnown = false;
	std::memset(&m_gameUuid, 0, sizeof(m_gameUuid));
	std::memset(m_gameUuidText, 0, sizeof(m_gameUuidText));
	m_staticGeneration = 0;
	m_staticGenerationWritten = false;
	m_playerMaps->staticMaps.clear();
	m_playerMaps->currentMaps.clear();
	m_campaignTurnPlayer = 0xFFFFFFFF;
	m_campaignTurn = -1;
	m_campaignStaticGeneration = 0;
	m_campaignGeneration = 0;
	m_campaignAlignedWorldGeneration = 0;
	m_campaignAlignedNextDeltaSequence = 0;
	m_campaignRelPath.clear();
	m_campaignFramedLength = 0;
	m_campaignStorage.Clear();
	m_topologyInvalidated = false;
	m_worldReplacementPending = false;
	m_decisionIdCounter = 0;
	m_identityPendingLogged = false;
	m_staticConstructNs = 0;
	m_staticWriteFlushNs = 0;
	m_staticBuildBytes = 0;
	m_zoneSnapshotDirty = false;
	m_teamPassabilityDirty = false;
	m_shuttingDown = false;
	m_concluded = false;
	m_config = VoxRlCaptureConfig();
	m_configResolved = false;
	m_rootResolved = false;
	m_captureRoot.clear();
	m_gameDirectory.clear();
	delete m_operationState;
	m_operationState = new OperationCaptureState();
	m_operationCauseStack->clear();
	VoxRlResetMilitaryEvents();
}

bool VoxRlCapture::AdmitsMilitaryEvents(PlayerTypes ePlayer) const
{
	const int player = static_cast<int>(ePlayer);
	if (player < 0 || player >= MAX_PLAYERS) return false;
	return m_configResolved && m_config.enabled && !m_concluded &&
		AdmitsPlayer(ePlayer) && GET_PLAYER(ePlayer).isAlive();
}

void VoxRlCapture::SetMilitaryPhase(PlayerTypes ePlayer, int phase)
{
	// Event buffering is attachment-scoped and therefore receives context
	// for minors and other actors without an admitted capture segment.
	VoxRlSetMilitaryEventContext(ePlayer, phase, GC.getGame().getGameTurn());
}

void VoxRlCapture::PushOperationChangeCause(PlayerTypes eOwner,
	PlayerTypes eInitiatingPlayer, int cause, int operationId)
{
	if (m_operationCauseStack == NULL)
	{
		m_operationCauseStack = new std::vector<OperationCauseContext>();
	}
	// Owner-local helpers retain the actor who requested work on this owner's behalf.
	if (eInitiatingPlayer == NO_PLAYER || eInitiatingPlayer == eOwner)
	{
		const OperationCauseContext* context = FindOperationCause(eOwner, false);
		if (context != NULL) eInitiatingPlayer = context->initiatingPlayer;
	}
	m_operationCauseStack->push_back(OperationCauseContext(eOwner,
		eInitiatingPlayer == NO_PLAYER ? eOwner : eInitiatingPlayer, cause, operationId));
}

// Returns the nearest owner-local operation cause accepted by the caller.
const VoxRlCapture::OperationCauseContext* VoxRlCapture::FindOperationCause(
	PlayerTypes eOwner, bool requireOperationId) const
{
	if (m_operationCauseStack == NULL) return NULL;
	for (std::vector<OperationCauseContext>::const_reverse_iterator context =
		m_operationCauseStack->rbegin(); context != m_operationCauseStack->rend(); ++context)
	{
		if (context->owner != eOwner || (requireOperationId && context->operationId < 0)) continue;
		return &*context;
	}
	return NULL;
}

void VoxRlCapture::PopOperationChangeCause()
{
	if (m_operationCauseStack != NULL && !m_operationCauseStack->empty())
	{
		const OperationCauseContext context = m_operationCauseStack->back();
		if (context.operationId >= 0)
		{
			NoteOperationChanged(context.owner, context.operationId);
		}
		m_operationCauseStack->pop_back();
	}
}

void VoxRlCapture::NoteOperationChanged(PlayerTypes eOwner, int operationId)
{
	NoteOperationChangedInternal(eOwner, operationId, false);
}

void VoxRlCapture::NoteOperationCreated(PlayerTypes eOwner, int operationId)
{
	NoteOperationChangedInternal(eOwner, operationId, true);
}

void VoxRlCapture::NoteOperationChangedInternal(PlayerTypes eOwner, int operationId, bool allowNew)
{
	if (!AdmitsMilitaryEvents(eOwner) || m_operationState == NULL) return;
	CvAIOperation* operation = GET_PLAYER(eOwner).getAIOperation(operationId);
	if (operation == NULL) return;
	PlayerTypes initiatingPlayer = eOwner;
	int cause = VOX_RL_OPERATION_CHANGE_FORCED;
	const OperationCauseContext* context = FindOperationCause(eOwner, false);
	if (context != NULL)
	{
		initiatingPlayer = context->initiatingPlayer;
		cause = context->cause;
	}
	const VoxRlEntityKey key(static_cast<int>(eOwner), operationId);
	std::map<VoxRlEntityKey, std::string>::const_iterator previous =
		m_operationState->operationFingerprints.find(key);
	// Initialization mutates an operation several times before addAIOperation
	// decides whether to keep it. Only the explicit accepted-creation hook may
	// admit an identity that is not already in the captured inventory.
	if (previous == m_operationState->operationFingerprints.end() && !allowNew) return;
	const int removalReason = OperationRemovalReason(*operation);
	// Execution retains its stop context immediately; invocation reconciliation
	// owns version collection and fingerprinting for its other mutations.
	if (cause == VOX_RL_OPERATION_CHANGE_EXECUTION)
	{
		if (removalReason != NO_ABORT_REASON)
			m_operationState->RememberOperationCause(key, cause, initiatingPlayer, removalReason);
		return;
	}
	const std::string fingerprint = OperationFingerprint(*operation);
	if (previous != m_operationState->operationFingerprints.end() && previous->second == fingerprint)
		return;
	m_operationState->RememberOperationCause(key, cause, initiatingPlayer, removalReason);
	const int kind = cause == VOX_RL_OPERATION_CHANGE_CHOICE ||
		cause == VOX_RL_OPERATION_CHANGE_UNIT_ASSIGNMENT
		? VOX_RL_OPERATION_RECORD_CHOICE : VOX_RL_OPERATION_RECORD_MAINTENANCE;
	if (!VoxRlAppendOperationRecord(*operation, initiatingPlayer, static_cast<u8>(kind),
		static_cast<u8>(cause), static_cast<u8>(VOX_RL_OPERATION_RESULT_NONE),
		static_cast<i8>(operation->GetAbortReason()),
		m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)]))
	{
		if (IsObserving(eOwner)) FailSegment("operationVersionCollect");
		return;
	}
	m_operationState->operationFingerprints[key] = fingerprint;
}

void VoxRlCapture::NoteOperationRemoved(PlayerTypes eOwner, int operationId, int abortReason)
{
	if (!AdmitsMilitaryEvents(eOwner) || m_operationState == NULL) return;
	const VoxRlEntityKey key(static_cast<int>(eOwner), operationId);
	if (m_operationState->operationFingerprints.count(key) == 0) return;
	PlayerTypes initiatingPlayer = eOwner;
	int cause = VOX_RL_OPERATION_CHANGE_FORCED;
	const OperationCauseContext* context = FindOperationCause(eOwner, false);
	// Cleanup describes when removal happens, not why stopping began.
	if (m_operationState->operationAbortReasons.count(key) != 0 &&
		m_operationState->operationAbortReasons[key] != NO_ABORT_REASON)
	{
		initiatingPlayer = m_operationState->operationInitiators[key];
		cause = m_operationState->operationCauses[key];
	}
	else if (context != NULL)
	{
		initiatingPlayer = context->initiatingPlayer;
		cause = context->cause;
	}
	RequestOperationRecord row;
	std::memset(&row, 0, sizeof(row));
	row.kind = static_cast<u8>(VOX_RL_OPERATION_RECORD_REMOVAL);
	row.operationOwner = static_cast<i8>(eOwner);
	row.initiatingPlayer = static_cast<i8>(initiatingPlayer);
	row.operationId = operationId;
	row.changeCause = static_cast<u8>(cause);
	row.invocationResult = static_cast<u8>(VOX_RL_OPERATION_RESULT_NONE);
	row.abortReason = static_cast<i8>(abortReason);
	m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)].requestOperations.push_back(row);
	m_operationState->operationFingerprints.erase(key);
	m_operationState->operationAbortReasons.erase(key);
	m_operationState->operationCauses.erase(key);
	m_operationState->operationInitiators.erase(key);
}

bool VoxRlCapture::ReconcileOperations(PlayerTypes eOwner, int recordKind,
	int invocationResult, int operationId)
{
	if (!IsObserving(eOwner)) return false;
	CvPlayerAI& player = GET_PLAYER(eOwner);
	std::set<int> liveIds;
	for (size_t index = 0; index < player.getNumAIOperations(); ++index)
	{
		CvAIOperation* operation = player.getAIOperationByIndex(index);
		if (operation == NULL) continue;
		if (operationId >= 0 && operation->GetID() != operationId) continue;
		liveIds.insert(operation->GetID());
		const VoxRlEntityKey key(static_cast<int>(eOwner), operation->GetID());
		const std::string fingerprint = OperationFingerprint(*operation);
		const bool changed = m_operationState->operationFingerprints.count(key) == 0 ||
			m_operationState->operationFingerprints[key] != fingerprint;
		if (recordKind == VOX_RL_OPERATION_RECORD_INVOCATION_RESULT)
		{
			if (changed && invocationResult != VOX_RL_OPERATION_RESULT_ALREADY_MOVED)
			{
				if (!VoxRlAppendOperationRecord(*operation, eOwner, static_cast<u8>(recordKind),
					static_cast<u8>(VOX_RL_OPERATION_CHANGE_EXECUTION), static_cast<u8>(invocationResult),
					static_cast<i8>(operation->GetAbortReason()),
					m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)])) return false;
			}
			else
			{
				RequestOperationRecord row;
				std::memset(&row, 0, sizeof(row));
				row.kind = static_cast<u8>(recordKind);
				row.operationOwner = static_cast<i8>(eOwner);
				row.initiatingPlayer = static_cast<i8>(eOwner);
				row.operationId = operation->GetID();
				row.changeCause = static_cast<u8>(VOX_RL_OPERATION_CHANGE_EXECUTION);
				row.invocationResult = static_cast<u8>(invocationResult);
				row.abortReason = static_cast<i8>(operation->GetAbortReason());
				m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)].requestOperations.push_back(row);
			}
			m_operationState->RememberOperationCause(key, VOX_RL_OPERATION_CHANGE_EXECUTION,
				eOwner, OperationRemovalReason(*operation));
		}
		else if (changed)
		{
			const int cause = recordKind == VOX_RL_OPERATION_RECORD_MAINTENANCE
				? VOX_RL_OPERATION_CHANGE_MAINTENANCE : VOX_RL_OPERATION_CHANGE_CHOICE;
			if (!VoxRlAppendOperationRecord(*operation, eOwner, static_cast<u8>(recordKind),
				static_cast<u8>(cause), static_cast<u8>(VOX_RL_OPERATION_RESULT_NONE),
				static_cast<i8>(operation->GetAbortReason()),
				m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)])) return false;
			m_operationState->RememberOperationCause(key, cause, eOwner,
				OperationRemovalReason(*operation));
		}
		m_operationState->operationFingerprints[key] = fingerprint;
	}
	if (operationId < 0)
	{
		std::vector<VoxRlEntityKey> removed;
		for (std::map<VoxRlEntityKey, std::string>::const_iterator known =
			m_operationState->operationFingerprints.begin(); known != m_operationState->operationFingerprints.end(); ++known)
		{
			if (known->first.owner == static_cast<int>(eOwner) && liveIds.count(known->first.id) == 0)
				removed.push_back(known->first);
		}
		for (size_t index = 0; index < removed.size(); ++index)
		{
			const VoxRlEntityKey key = removed[index];
			const int reason = m_operationState->operationAbortReasons.count(key) != 0
				? m_operationState->operationAbortReasons[key] : NO_ABORT_REASON;
			NoteOperationRemoved(eOwner, key.id, reason);
		}
	}
	return true;
}

void VoxRlCapture::OnOperationSelectionComplete(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer)) return;
	if (!ReconcileOperations(ePlayer, VOX_RL_OPERATION_RECORD_CHOICE,
		VOX_RL_OPERATION_RESULT_NONE))
	{
		FailSegment("operationSelectionCollect");
		return;
	}
	RequestCampaignBoundaryRecord boundary;
	std::memset(&boundary, 0, sizeof(boundary));
	boundary.kind = static_cast<u8>(VOX_RL_CAMPAIGN_BOUNDARY_OPERATION_SELECTION);
	boundary.operationOwner = static_cast<i8>(NO_PLAYER);
	boundary.operationId = -1;
	boundary.zoneId = -1;
	boundary.observationWorldGeneration = m_segment->worldGeneration;
	boundary.observationDeltaSequence = m_segment->nextDeltaSequence;
	m_operationState->pendingRowsByOwner[static_cast<int>(ePlayer)].requestCampaignBoundaries.push_back(boundary);
	EmitSynchronizationRequest();
}

void VoxRlCapture::OnPreTacticalReconciliation(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer)) return;
	if (!ReconcileOperations(ePlayer, VOX_RL_OPERATION_RECORD_MAINTENANCE,
		VOX_RL_OPERATION_RESULT_NONE))
	{
		FailSegment("preTacticalOperationCollect");
		return;
	}
	// This REQUEST carries the actor's map and espionage changes from the city turns,
	// policies, research, and espionage that ran after the military checkpoint.
	m_segment->collectLateTurnState = true;
	EmitSynchronizationRequest();
	if (m_segment != NULL) m_segment->collectLateTurnState = false;
}

// Refreshes the cached maps of one player and its living teammates once STATIC exists.
// Native CvTeam::processTech reinitializes the traits of every team member, so a teammate's
// map can change during this player's turn.
void VoxRlCapture::RefreshTeamPlayerMaps(PlayerTypes ePlayer)
{
	if (!m_staticGenerationWritten || ePlayer < 0 || ePlayer >= MAX_PLAYERS) return;
	const TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
	{
		const PlayerTypes eMember = static_cast<PlayerTypes>(playerIndex);
		if (eMember != ePlayer && (!GET_PLAYER(eMember).isAlive() || GET_PLAYER(eMember).getTeam() != eTeam)) continue;
		VoxRlPlayerMapRows rows;
		if (VoxRlCollectPlayerMapRows(eMember, rows)) m_playerMaps->currentMaps[playerIndex] = rows;
	}
}

void VoxRlCapture::OnOperationInvocationComplete(PlayerTypes ePlayer, int operationId, int result)
{
	if (!IsObserving(ePlayer)) return;
	VoxRlOperationCaptureScope captureScope(true, ePlayer, ePlayer,
		VOX_RL_OPERATION_CHANGE_EXECUTION, operationId);
	if (!ReconcileOperations(ePlayer, VOX_RL_OPERATION_RECORD_INVOCATION_RESULT,
		result, operationId))
	{
		FailSegment("operationInvocationCollect");
		return;
	}
	EmitSynchronizationRequest();
}

void VoxRlCapture::OnOperationalMovesComplete(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer)) return;
	if (!ReconcileOperations(ePlayer, VOX_RL_OPERATION_RECORD_MAINTENANCE,
		VOX_RL_OPERATION_RESULT_NONE))
	{
		FailSegment("operationCleanupCollect");
		return;
	}
	RequestCampaignBoundaryRecord boundary;
	std::memset(&boundary, 0, sizeof(boundary));
	boundary.kind = static_cast<u8>(VOX_RL_CAMPAIGN_BOUNDARY_OPERATIONAL_ARMY_MOVES);
	boundary.operationOwner = static_cast<i8>(NO_PLAYER);
	boundary.operationId = -1;
	boundary.zoneId = -1;
	boundary.observationWorldGeneration = m_segment->worldGeneration;
	boundary.observationDeltaSequence = m_segment->nextDeltaSequence;
	m_operationState->pendingRowsByOwner[static_cast<int>(ePlayer)].requestCampaignBoundaries.push_back(boundary);
	EmitSynchronizationRequest();
}

// Binds the first pre-choice observation once for the actor-turn.
void VoxRlCapture::OnStanceAssessmentReady(PlayerTypes ePlayer)
{
	if (m_searchActive || !IsObserving(ePlayer)) return;
	// One assessment supplies this actor-turn's stance batch, including across WORLD replacement.
	if (m_operationState->stanceAssessmentValid &&
		m_operationState->assessmentPlayer == ePlayer &&
		m_operationState->assessmentTurn == m_segment->turn) return;
	m_operationState->pendingZoneChoices.clear();
	m_operationState->stanceAssessmentValid = false;
	m_operationState->stanceChoicesReady = false;
	m_operationState->stanceBatchPublished = false;
	if (!EmitSynchronizationRequest()) return;
	m_operationState->assessmentPlayer = ePlayer;
	m_operationState->assessmentTurn = m_segment->turn;
	m_operationState->assessmentWorldGeneration = m_segment->worldGeneration;
	m_operationState->assessmentDeltaSequence = m_segment->lastRequestSequence;
	m_operationState->stanceAssessmentValid = true;
}

// Retains the first assessment's labels after native posture selection and sorting.
void VoxRlCapture::OnStanceChoicesReady(PlayerTypes ePlayer)
{
	if (m_searchActive || !IsObserving(ePlayer) || m_operationState->stanceChoicesReady ||
		!m_operationState->stanceAssessmentValid ||
		m_operationState->assessmentPlayer != ePlayer ||
		m_operationState->assessmentTurn != m_segment->turn) return;
	VoxRlZoneSnapshot zones;
	if (!VoxRlCollectZones(GET_PLAYER(ePlayer).GetTacticalAI()->GetTacticalAnalysisMap(), zones))
	{
		FailSegment("stanceChoiceCollect");
		return;
	}
	m_operationState->pendingZoneChoices.clear();
	for (size_t index = 0; index < zones.zones.size(); ++index)
	{
		RequestZoneChoiceRecord row;
		std::memset(&row, 0, sizeof(row));
		row.zoneId = zones.zones[index].zoneId;
		row.posture = zones.zones[index].posture;
		row.priorityRank = static_cast<u16>((std::min)(index, static_cast<size_t>(65535)));
		row.assessmentWorldGeneration = m_operationState->assessmentWorldGeneration;
		row.assessmentDeltaSequence = m_operationState->assessmentDeltaSequence;
		m_operationState->pendingZoneChoices.push_back(row);
	}
	m_operationState->stanceChoicesReady = true;
}

// Publishes one stance batch, including a completed batch with no zone choices.
void VoxRlCapture::OnFirstZoneDispatch(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer) || m_operationState->stanceBatchPublished) return;
	// Flush work completed after assessment before publishing any label rows.
	if (!EmitSynchronizationRequest()) return;
	if (m_operationState->stanceAssessmentValid && m_operationState->stanceChoicesReady &&
		m_operationState->assessmentPlayer == ePlayer &&
		m_operationState->assessmentTurn == m_segment->turn)
	{
		VoxRlRequestData& pending = m_operationState->pendingRowsByOwner[static_cast<int>(ePlayer)];
		pending.requestZoneChoices = m_operationState->pendingZoneChoices;
		RequestCampaignBoundaryRecord boundary;
		std::memset(&boundary, 0, sizeof(boundary));
		boundary.kind = static_cast<u8>(VOX_RL_CAMPAIGN_BOUNDARY_ZONE_WORK);
		boundary.operationOwner = static_cast<i8>(NO_PLAYER);
		boundary.operationId = -1;
		boundary.zoneId = -1;
		boundary.observationWorldGeneration = m_operationState->assessmentWorldGeneration;
		boundary.observationDeltaSequence = m_operationState->assessmentDeltaSequence;
		pending.requestCampaignBoundaries.push_back(boundary);
		if (!EmitSynchronizationRequest()) return;
	}
	m_operationState->stanceBatchPublished = true;
}

void VoxRlCapture::OnZoneReinforcementComplete(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer)) return;
	RequestCampaignBoundaryRecord boundary;
	std::memset(&boundary, 0, sizeof(boundary));
	boundary.kind = static_cast<u8>(VOX_RL_CAMPAIGN_BOUNDARY_ZONE_REINFORCEMENT);
	boundary.operationOwner = static_cast<i8>(NO_PLAYER);
	boundary.operationId = -1;
	boundary.zoneId = -1;
	boundary.observationWorldGeneration = m_segment->worldGeneration;
	boundary.observationDeltaSequence = m_segment->nextDeltaSequence;
	m_operationState->pendingRowsByOwner[static_cast<int>(ePlayer)].requestCampaignBoundaries.push_back(boundary);
	EmitSynchronizationRequest();
}

void VoxRlCapture::NoteWorkerAssignment(PlayerTypes ePlayer, const CvUnit* pUnit, int plotIndex,
	int buildType, int directiveType, int source, int sentryKind, int sentryWeight)
{
	if (pUnit == NULL || !IsObserving(ePlayer)) return;
	// Only the narrowing casts are guarded here; the generated validator owns the manifest rules.
	const int unitType = pUnit->getUnitType();
	if (unitType < SHRT_MIN || unitType > SHRT_MAX || plotIndex < SHRT_MIN || plotIndex > SHRT_MAX ||
		buildType < SHRT_MIN || buildType > SHRT_MAX) return;
	if (directiveType < 0 || directiveType > UCHAR_MAX || source < 0 || source > UCHAR_MAX ||
		sentryKind < 0 || sentryKind > UCHAR_MAX) return;
	RequestWorkerAssignmentRecord row;
	std::memset(&row, 0, sizeof(row));
	row.owner = static_cast<i8>(ePlayer);
	row.unitId = pUnit->GetID();
	row.unitType = static_cast<i16>(unitType);
	row.plotIndex = static_cast<i16>(plotIndex);
	row.buildType = static_cast<i16>(buildType);
	row.directiveType = static_cast<u8>(directiveType);
	row.source = static_cast<u8>(source);
	row.sentryKind = static_cast<u8>(sentryKind);
	row.sentryWeight = sentryWeight;
	if (!VoxRlIsValidRequestWorkerAssignmentRecord(row)) return;
	m_operationState->pendingRowsByOwner[static_cast<int>(ePlayer)].requestWorkerAssignments.push_back(row);
}

void VoxRlCapture::OnWorkerMovesComplete(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer)) return;
	std::map<int, VoxRlRequestData>::const_iterator pending =
		m_operationState->pendingRowsByOwner.find(static_cast<int>(ePlayer));
	if (pending == m_operationState->pendingRowsByOwner.end() ||
		pending->second.requestWorkerAssignments.empty()) return;
	EmitSynchronizationRequest();
}

void VoxRlCapture::NoteFocusAreaChanged(PlayerTypes eOwner, int kind,
	int centerPlotIndex, int radius, int expiryTurn)
{
	if (!AdmitsMilitaryEvents(eOwner) || m_operationState == NULL) return;
	if (centerPlotIndex < 0 || centerPlotIndex > 32767) return;
	if (radius < 0 || radius > 255) return;
	if (kind != VOX_RL_FOCUS_AREA_REMOVE && (expiryTurn < -32768 || expiryTurn > 32767)) return;
	RequestFocusAreaRecord row;
	std::memset(&row, 0, sizeof(row));
	row.kind = static_cast<u8>(kind);
	row.centerPlotIndex = static_cast<i16>(centerPlotIndex);
	row.radius = static_cast<u8>(radius);
	row.expiryTurn = static_cast<i16>(kind == VOX_RL_FOCUS_AREA_REMOVE ? -1 : expiryTurn);
	m_operationState->pendingRowsByOwner[static_cast<int>(eOwner)].requestFocusAreas.push_back(row);
}

namespace
{
// One operation scope entry kept for tactical flavor lookups, with or without capture.
struct VoxRlTrackedOperation
{
	PlayerTypes owner;
	int operationId;
};

// Scopes nest only a few levels deep; deeper entries are not tracked.
const int kVoxRlTrackedOperationLimit = 16;
VoxRlTrackedOperation gVoxRlTrackedOperations[kVoxRlTrackedOperationLimit];
int gVoxRlTrackedOperationCount = 0;
}

VoxRlOperationCaptureScope::VoxRlOperationCaptureScope(bool enabled, PlayerTypes eOwner,
	PlayerTypes eInitiatingPlayer, int cause, int operationId)
	: m_enabled(enabled), m_tracked(false)
{
	if (gVoxRlTrackedOperationCount < kVoxRlTrackedOperationLimit)
	{
		gVoxRlTrackedOperations[gVoxRlTrackedOperationCount].owner = eOwner;
		gVoxRlTrackedOperations[gVoxRlTrackedOperationCount].operationId = operationId;
		gVoxRlTrackedOperationCount += 1;
		m_tracked = true;
	}
	if (m_enabled)
	{
		VoxRlCapture::GetInstance().PushOperationChangeCause(eOwner,
			eInitiatingPlayer, cause, operationId);
	}
}

VoxRlOperationCaptureScope::~VoxRlOperationCaptureScope()
{
	if (m_tracked)
	{
		gVoxRlTrackedOperationCount -= 1;
	}
	if (m_enabled)
	{
		VoxRlCapture::GetInstance().PopOperationChangeCause();
	}
}

int VoxRlCurrentOperationId(PlayerTypes eOwner)
{
	for (int index = gVoxRlTrackedOperationCount - 1; index >= 0; --index)
	{
		if (gVoxRlTrackedOperations[index].owner == eOwner && gVoxRlTrackedOperations[index].operationId >= 0)
		{
			return gVoxRlTrackedOperations[index].operationId;
		}
	}
	return -1;
}

void VoxRlCapture::Shutdown()
{
	m_shuttingDown = true;
	EndEngagement();
	CloseSegment("gameTeardown");
	// The retained CAMPAIGN bytes belong to the finished attachment.
	m_campaignStorage.Clear();
	m_operationCauseStack->clear();
	VoxRlResetMilitaryEvents();
}

void VoxRlCapture::OnGameConcluded()
{
	if (m_concluded)
	{
		return;
	}
	// The game ended: commit the open segment, release the output file
	// handles, and ignore capture hooks until a new game starts. The
	// victory archive can then move the recording directory.
	m_concluded = true;
	EndEngagement();
	CloseSegment("gameConcluded");
}

void VoxRlCapture::CloseSegment(const char* closureReason)
{
	if (m_segment == NULL)
	{
		return;
	}
	if (m_segment->stagingRequest)
	{
		// A refresh window straddled the boundary; discard the staged rows
		// and record the gap.
		m_segment->stagingRequest = false;
		AddCoverageOmission("interruptedRefresh");
	}
	// A segment that reached the campaign seam is a complete operational
	// example even when no tactical search ran. Publish its empty or
	// synchronization-only batches before closing it.
	if (!m_segment->failed && !m_segment->published && m_segment->campaignSeamReached)
	{
		PublishPendingFrames(true);
		if (m_segment == NULL || m_segment->failed)
		{
			return;
		}
	}
	if (!m_segment->failed && m_segment->published)
	{
		// Closure owns the durability sequence: the stream becomes durable
		// before the closing index commit is appended, and the index becomes
		// durable before its handle closes. Ordinary event commits perform no
		// synchronization, so this is the only point where the segment's
		// committed prefix is guaranteed to survive power loss.
		bool flushed = false;
		{
			ScopedTiming timing(m_config.timings, m_segment->timings.streamFlushNs);
			flushed = m_segmentStream.Flush();
		}
		if (!flushed)
		{
			FailSegment("streamFlush");
			return;
		}
		// A closure-only batch commits without adding binary frames.
		CommitBatch(true, closureReason);
		if (m_segment == NULL || m_segment->failed)
		{
			return;
		}
		bool indexFlushed = false;
		{
			ScopedTiming timing(m_config.timings, m_segment->timings.indexFlushNs);
			indexFlushed = m_segmentIndex.Flush();
		}
		if (!indexFlushed)
		{
			FailSegment("indexFlush");
			return;
		}
	}
	else if (!m_segment->failed && !m_segment->published && m_segment->hasDecision)
	{
		// A decision was made but the segment closed before it could publish
		// (its CAMPAIGN was never bound in time). Suppression is reserved for
		// segments that genuinely never decided, so this is a capture failure
		// rather than a silent drop of real decision data.
		FailSegment("unpublishedDecision");
		return;
	}
	else if (!m_segment->failed && !m_segment->published)
	{
		// A segment that never reached the operational entry point leaves no
		// files behind. Its true lifecycle flags and coverage omissions travel
		// to suppressed.jsonl so they still reach disk.
		AppendSuppressedLine(closureReason);
	}
	WriteTimingSummary(closureReason);
	delete m_segment;
	m_segment = NULL;
	m_segmentStream.Close();
	m_segmentIndex.Close();
}

void VoxRlCapture::FailSegment(const char* closureReason)
{
	if (m_segment == NULL || m_segment->failed)
	{
		return;
	}
	m_segment->failed = true;
	FILogFile* log = VoxRlCaptureLog();
	if (log != NULL)
	{
		log->Msg("Capture segment failed: stage=%s player=%d turn=%d.\n", closureReason,
			static_cast<int>(m_segment->player), m_segment->turn);
	}
	WriteTimingSummary(closureReason);
	// Retain the prior committed prefix exactly as it is on disk. No
	// further index lines are appended after a damaged tail; capture
	// restarts at a fresh supported checkpoint.
	delete m_segment;
	m_segment = NULL;
	m_segmentStream.Close();
	m_segmentIndex.Close();
}

bool VoxRlCapture::AppendIndexLine(const char* line)
{
	if (!m_segmentIndex.IsOpen())
	{
		return false;
	}
	return m_segmentIndex.Write(line, static_cast<unsigned int>(std::strlen(line)));
}

void VoxRlCapture::AddCoverageOmission(const char* reason)
{
	if (m_segment == NULL)
	{
		return;
	}
	m_segment->coverageReasons.push_back(reason);
}

// Writes one segment's accumulated timing summary to VoxRlCapture.csv. The
// timings are opt-in, accumulate in memory, and publish only at this segment
// boundary, so ordinary events never pay a log write.
void VoxRlCapture::WriteTimingSummary(const char* closureReason)
{
	if (m_segment == NULL || !m_config.timings)
	{
		return;
	}
	const Segment::Timings& t = m_segment->timings;
	FILogFile* log = LOGFILEMGR.GetLog("VoxRlCapture.csv", FILogFile::kDontTimeStamp);
	if (log == NULL)
	{
		m_staticConstructNs = 0;
		m_staticWriteFlushNs = 0;
		m_staticBuildBytes = 0;
		return;
	}
	static bool headerWritten = false;
	if (!headerWritten)
	{
		log->Msg("game_uuid,player,turn,world_generation,closure,failed,published,frames,commits,"
			"plots,units,cities,alive_players,alive_teams,static_construct_ns,static_write_flush_ns,static_bytes,"
			"world_construct_ns,world_owner_iteration_ns,world_danger_sparse_relations_ns,"
			"world_zone_ns,world_plot_unit_ns,world_visibility_ns,world_entity_relation_ns,world_serialize_ns,"
			"world_write_flush_ns,world_bytes,campaign_construct_ns,campaign_write_flush_ns,campaign_bytes,"
			"delta_collections,delta_collect_ns,delta_rows,requests,request_build_ns,request_bytes,results,"
			"result_build_ns,result_bytes,pending_buffer_ns,pending_buffer_bytes,pending_publish_write_ns,"
			"pending_publish_write_bytes,stream_write_ns,stream_write_bytes,commit_ns,index_bytes,"
			"stream_flush_ns,index_flush_ns\n");
		headerWritten = true;
	}
	log->Msg("%s,%d,%d,%u,%s,%d,%d,%u,%u,%u,%u,%u,%u,%u,"
		"%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,"
		"%I64u,%I64u,%I64u,%u,%I64u,%I64u,%u,%I64u,%I64u,%u,%I64u,%I64u,%I64u,%I64u,%I64u,"
		"%I64u,%I64u,%I64u,%I64u,%I64u,%I64u,%I64u\n",
		m_gameUuidText, static_cast<int>(m_segment->player), m_segment->turn, m_segment->worldGeneration,
		closureReason != NULL ? closureReason : "unknown", m_segment->failed ? 1 : 0,
		m_segment->published ? 1 : 0, static_cast<unsigned int>(m_segment->frameTable.size()), t.commitCount,
		t.worldPhases.plotCount, t.worldPhases.unitCount, t.worldPhases.cityCount,
		t.worldPhases.alivePlayerCount, t.worldPhases.aliveTeamCount,
		m_staticConstructNs, m_staticWriteFlushNs, m_staticBuildBytes,
		t.worldBuildNs, t.worldPhases.ownerIterationNs, t.worldPhases.dangerSparseRelationsNs,
		t.worldPhases.zoneNs, t.worldPhases.plotUnitNs,
		t.worldPhases.visibilityNs, t.worldPhases.entityRelationNs, t.worldPhases.serializeNs,
		t.worldWriteFlushNs, t.worldBuildBytes, t.campaignConstructNs, t.campaignWriteFlushNs,
		t.campaignBuildBytes, t.deltaCollectCount, t.deltaCollectNs, t.deltaRows,
		t.requestCount, t.requestBuildNs, t.requestBytes, t.resultCount, t.resultBuildNs, t.resultBytes,
		t.pendingBufferNs, t.pendingBufferBytes, t.pendingPublishWriteNs, t.pendingPublishWriteBytes,
		t.streamWriteNs, t.streamWriteBytes, t.commitNs, t.indexBytes, t.streamFlushNs, t.indexFlushNs);
	m_staticConstructNs = 0;
	m_staticWriteFlushNs = 0;
	m_staticBuildBytes = 0;
}

bool VoxRlCapture::BuildAndWriteStatic()
{
	VoxRlBlockIdentity identity;
	identity.session = m_gameUuid;
	identity.turn = kVoxRlAbsentTurn;
	identity.player = kVoxRlAbsentPlayer;
	identity.staticGeneration = m_staticGeneration;
	identity.generation = m_staticGeneration;

	VoxRlOwnedBlockStorage storage;
	unsigned int length = 0;
	char fileName[32];
	{
		ScopedTiming timing(m_config.timings, m_staticConstructNs);
		std::vector<VoxRlPlayerMapRows> playerMaps;
		if (!VoxRlBuildStaticBlock(identity, storage, length, playerMaps))
		{
			return false;
		}
		// A new STATIC absorbs every player's current map.
		m_playerMaps->staticMaps.clear();
		for (size_t index = 0; index < playerMaps.size(); ++index)
			m_playerMaps->staticMaps[static_cast<int>(playerMaps[index].player.player)] = playerMaps[index];
		m_playerMaps->currentMaps = m_playerMaps->staticMaps;
	}
	{
		ScopedTiming timing(m_config.timings, m_staticWriteFlushNs);
		const std::string directory = m_gameDirectory + "/baselines/static";
		sprintf_s(fileName, 32, "static-%u.bin", m_staticGeneration);
		const std::string path = directory + "/" + fileName;
		VoxRlOutputFile file;
		if (!file.OpenNew(path.c_str()) || !file.Write(storage.Bytes(), length) || !file.Flush())
		{
			return false;
		}
		m_staticBuildBytes += length;
	}
	m_staticRelPath = "baselines/static/" + std::string(fileName);
	m_staticFramedLength = length;
	m_staticGenerationWritten = true;
	m_topologyInvalidated = false;
	return true;
}

bool VoxRlCapture::BuildWorldBaseline(PlayerTypes ePlayer, int iTurn)
{
	ScopedCaptureCollection collecting(m_collectingState);
	Segment& segment = *m_segment;
	char folder[64];
	sprintf_s(folder, 64, "baselines/world/player-%d/turn-%d", static_cast<int>(ePlayer), iTurn);
	const std::string directory = m_gameDirectory + "/" + folder;
	segment.worldGeneration = ReserveGeneration(directory, "world");
	if (segment.worldGeneration == 0)
	{
		return false;
	}
	VoxRlBlockIdentity identity;
	identity.session = m_gameUuid;
	identity.turn = iTurn;
	identity.player = static_cast<int>(ePlayer);
	identity.generation = segment.worldGeneration;
	identity.staticGeneration = segment.staticGeneration;
	identity.worldGeneration = segment.worldGeneration;

	VoxRlOwnedBlockStorage storage;
	unsigned int length = 0;
	std::vector<TeamPassabilityRecord> teamPassability;
	std::vector<TeamResourceRecord> teamResources;
	{
		ScopedTiming timing(m_config.timings, m_segment->timings.worldBuildNs);
		VoxRlWorldBuildTimings* phases = m_config.timings ? &segment.timings.worldPhases : NULL;
		// WORLD carries every cached player map that differs from the one STATIC holds.
		std::vector<VoxRlPlayerMapRows> overrides;
		VoxRlCollectPlayerMapOverrides(m_playerMaps->staticMaps, m_playerMaps->currentMaps, overrides);
		if (!VoxRlBuildWorldBlock(identity, ePlayer, storage, length, segment.zoneSnapshot,
			teamPassability, teamResources, overrides, phases, &segment.playerCitySnapshot))
		{
			return false;
		}
	}
	segment.timings.worldBuildBytes += length;
	// Seed from the exact serialized WORLD rows so the first request compares against
	// the bytes that the reader will actually load.
	{
		VoxRlBlockView worldView;
		if (!worldView.Open(storage.Bytes(), length)) return false;
		const u8* gameBytes = worldView.SectionBytes(VOX_RL_SECTION_WORLD_GAME_STATE);
		if (gameBytes == NULL) return false;
		std::memcpy(&segment.lastGameState, gameBytes, sizeof(segment.lastGameState));
		const VoxRlSectionDirectoryEntry* tradeRoutes = worldView.FindSection(VOX_RL_SECTION_WORLD_TRADE_CONNECTIONS);
		const VoxRlSectionDirectoryEntry* tradePaths = worldView.FindSection(VOX_RL_SECTION_WORLD_TRADE_PATH_PLOTS);
		if (tradeRoutes == NULL || tradePaths == NULL) return false;
		segment.lastTradeConnections.resize(tradeRoutes->count);
		segment.lastTradePathPlots.resize(tradePaths->count);
		if (tradeRoutes->count != 0U)
			std::memcpy(&segment.lastTradeConnections[0], worldView.SectionBytes(VOX_RL_SECTION_WORLD_TRADE_CONNECTIONS), tradeRoutes->count * sizeof(TradeConnectionRecord));
		if (tradePaths->count != 0U)
			std::memcpy(&segment.lastTradePathPlots[0], worldView.SectionBytes(VOX_RL_SECTION_WORLD_TRADE_PATH_PLOTS), tradePaths->count * sizeof(TradePathPlotRecord));
		const VoxRlSectionDirectoryEntry* players = worldView.FindSection(VOX_RL_SECTION_WORLD_PLAYERS);
		const u32 playerCount = players == NULL ? 0U : players->count;
		const u8* playerBytes = playerCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_PLAYERS);
		if (playerCount != 0U && playerBytes == NULL) return false;
		segment.lastPlayerRows.clear();
		for (u32 index = 0; index < playerCount; ++index)
		{
			PlayerRecord row;
			std::memcpy(&row, playerBytes + index * sizeof(PlayerRecord), sizeof(row));
			if (!segment.lastPlayerRows.insert(std::make_pair(static_cast<int>(row.id), row)).second) return false;
		}
		const VoxRlSectionDirectoryEntry* espionage = worldView.FindSection(VOX_RL_SECTION_WORLD_CITY_ESPIONAGE_SIGHTS);
		const u32 espionageCount = espionage == NULL ? 0U : espionage->count;
		const u8* espionageBytes = espionageCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_CITY_ESPIONAGE_SIGHTS);
		if (espionageCount != 0U && espionageBytes == NULL) return false;
		segment.lastEspionageRows.clear();
		for (u32 index = 0; index < espionageCount; ++index)
		{
			CityEspionageSightRecord row;
			std::memcpy(&row, espionageBytes + index * sizeof(CityEspionageSightRecord), sizeof(row));
			if (row.spyOwner == static_cast<i8>(ePlayer)) segment.lastEspionageRows.push_back(row);
		}
		const VoxRlSectionDirectoryEntry* foundValues = worldView.FindSection(VOX_RL_SECTION_WORLD_PLAYER_FOUND_VALUES);
		const u32 foundValueCount = foundValues == NULL ? 0U : foundValues->count;
		const u8* foundValueBytes = foundValueCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_PLAYER_FOUND_VALUES);
		if (foundValueCount != 0U && foundValueBytes == NULL) return false;
		segment.lastFoundValueRows.clear();
		for (u32 index = 0; index < foundValueCount; ++index)
		{
			PlayerFoundValueRecord row;
			std::memcpy(&row, foundValueBytes + index * sizeof(PlayerFoundValueRecord), sizeof(row));
			if (row.player == static_cast<i8>(ePlayer)) segment.lastFoundValueRows.push_back(row);
		}
		const VoxRlSectionDirectoryEntry* cities = worldView.FindSection(VOX_RL_SECTION_WORLD_CITIES);
		const u32 cityCount = cities == NULL ? 0U : cities->count;
		const u8* cityBytes = cityCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_CITIES);
		if (cityCount != 0U && cityBytes == NULL) return false;
		segment.lastCityNeedsGarrison.clear();
		segment.lastCityCheckpointScalars.clear();
		for (u32 index = 0; index < cityCount; ++index)
		{
			CityRecord row;
			std::memcpy(&row, cityBytes + index * sizeof(CityRecord), sizeof(row));
			const VoxRlEntityKey key(static_cast<int>(row.owner), row.id);
			if (!segment.lastCityNeedsGarrison.insert(
				std::make_pair(key, static_cast<unsigned char>(row.needsGarrison))).second) return false;
		segment.lastCityCheckpointScalars.insert(std::make_pair(key, CityCheckpointScalars(row)));
		}
		// Keep the baseline's visibility domain even if a team dies later in the segment.
		segment.capturedTeams.clear();
		segment.capturedRelationTeams.clear();
		const VoxRlSectionDirectoryEntry* relations = worldView.FindSection(VOX_RL_SECTION_WORLD_TEAM_RELATIONS);
		const u32 relationCount = relations == NULL ? 0U : relations->count;
		const u8* relationBytes = relationCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_TEAM_RELATIONS);
		if (relationCount != 0U && relationBytes == NULL) return false;
		for (u32 index = 0; index < relationCount; ++index)
		{
			TeamRelationRecord row;
			std::memcpy(&row, relationBytes + index * sizeof(TeamRelationRecord), sizeof(row));
			segment.capturedRelationTeams.insert(static_cast<int>(row.team));
			segment.capturedRelationTeams.insert(static_cast<int>(row.otherTeam));
		}
		const VoxRlSectionDirectoryEntry* teams = worldView.FindSection(VOX_RL_SECTION_WORLD_PLOT_TEAMS);
		const u32 teamCount = teams == NULL ? 0U : teams->count;
		const u8* teamBytes = teamCount == 0U ? NULL : worldView.SectionBytes(VOX_RL_SECTION_WORLD_PLOT_TEAMS);
		if (teamCount != 0U && teamBytes == NULL) return false;
		for (u32 index = 0; index < teamCount; ++index)
		{
			PlotTeamRecord row;
			std::memcpy(&row, teamBytes + index * sizeof(PlotTeamRecord), sizeof(row));
			if (!segment.capturedTeams.insert(static_cast<int>(row.team)).second) return false;
		}
	}
	segment.lastTeamPassabilityRows.clear();
	for (size_t index = 0; index < teamPassability.size(); ++index)
		segment.lastTeamPassabilityRows[static_cast<int>(teamPassability[index].team)] = teamPassability[index];
	segment.lastTeamResourceRows.clear();
	for (size_t index = 0; index < teamResources.size(); ++index)
		segment.lastTeamResourceRows[static_cast<int>(teamResources[index].team)] = teamResources[index];
	// The WORLD build accepts a fresh zone snapshot and the collected team
	// passability table, so both dirty markers reset here.
	m_zoneSnapshotDirty = false;
	m_teamPassabilityDirty = false;
	segment.zoneRowByZoneId.clear();
	for (size_t zoneIndex = 0; zoneIndex < segment.zoneSnapshot.zones.size(); ++zoneIndex)
		segment.zoneRowByZoneId[segment.zoneSnapshot.zones[zoneIndex].zoneId] = static_cast<unsigned int>(zoneIndex + 1);
	char fileName[32];
	sprintf_s(fileName, 32, "world-%u.bin", segment.worldGeneration);
	segment.worldRelPath = folder + std::string("/") + fileName;
	segment.worldFramedLength = length;
	// The completed WORLD keeps its built storage; the swap transfers the
	// allocation without copying the bytes.
	segment.worldStorage.Swap(&storage);
	SeedWorldIterationIndices();
	// Later relation hooks may occur while no admitted player segment is open.
	m_operationState->relationsSeeded = true;
	return true;
}

bool VoxRlCapture::WriteWorldBaseline()
{
	Segment& segment = *m_segment;
	if (segment.worldStorage.ByteLength() == 0 || segment.worldRelPath.empty())
	{
		return false;
	}
	const std::string path = m_gameDirectory + "/" + segment.worldRelPath;
	VoxRlOutputFile file;
	bool written = false;
	{
		ScopedTiming timing(m_config.timings, segment.timings.worldWriteFlushNs);
		written = file.OpenNew(path.c_str()) &&
			file.Write(segment.worldStorage.Bytes(), segment.worldFramedLength) && file.Flush();
	}
	if (!written)
	{
		return false;
	}
	segment.worldStorage.Clear();
	return true;
}

// Builds the CAMPAIGN block into capture-level storage and binds this segment
// to it. The file write happens at publication, so a segment that closes
// without a decision leaves no CAMPAIGN file behind and the next same-turn
// segment can still publish the retained bytes.
bool VoxRlCapture::BuildAndBindCampaign(PlayerTypes ePlayer, int iTurn,
	unsigned int alignedWorldGeneration, unsigned int alignedNextDeltaSequence)
{
	Segment& segment = *m_segment;
	char folder[80];
	sprintf_s(folder, 80, "baselines/campaign/player-%d/turn-%d", static_cast<int>(ePlayer), iTurn);
	const std::string directory = m_gameDirectory + "/" + folder;
	const unsigned int campaignGeneration = ReserveGeneration(directory, "campaign");
	if (campaignGeneration == 0)
	{
		return false;
	}
	VoxRlBlockIdentity identity;
	identity.session = m_gameUuid;
	identity.turn = iTurn;
	identity.player = static_cast<int>(ePlayer);
	identity.generation = campaignGeneration;
	identity.staticGeneration = segment.staticGeneration;
	identity.campaignGeneration = campaignGeneration;

	VoxRlOwnedBlockStorage storage;
	unsigned int length = 0;
	char fileName[36];
	{
		ScopedTiming timing(m_config.timings, segment.timings.campaignConstructNs);
		if (!VoxRlBuildCampaignBlock(identity, ePlayer, alignedWorldGeneration,
			alignedNextDeltaSequence, segment.lastPlayerRows, storage, length))
		{
			return false;
		}
	}
	// The completed bytes swap into capture-level storage without copying.
	m_campaignStorage.Swap(&storage);
	sprintf_s(fileName, 36, "campaign-%u.bin", campaignGeneration);
	m_campaignTurnPlayer = static_cast<unsigned int>(ePlayer);
	m_campaignTurn = iTurn;
	m_campaignStaticGeneration = segment.staticGeneration;
	m_campaignGeneration = campaignGeneration;
	m_campaignAlignedWorldGeneration = alignedWorldGeneration;
	m_campaignAlignedNextDeltaSequence = alignedNextDeltaSequence;
	m_campaignRelPath = folder + std::string("/") + fileName;
	m_campaignFramedLength = length;
	segment.campaignGeneration = campaignGeneration;
	segment.campaignRelPath = m_campaignRelPath;
	segment.campaignFramedLength = length;
	// Seed change detection from the exact operation inventory represented by
	// CAMPAIGN while preserving any earlier mutation row already buffered.
	CvPlayerAI& player = GET_PLAYER(ePlayer);
	for (size_t index = 0; index < player.getNumAIOperations(); ++index)
	{
		CvAIOperation* operation = player.getAIOperationByIndex(index);
		if (operation == NULL) continue;
		const VoxRlEntityKey key(static_cast<int>(ePlayer), operation->GetID());
		if (m_operationState->operationFingerprints.count(key) == 0)
		{
			m_operationState->operationFingerprints[key] = OperationFingerprint(*operation);
			m_operationState->RememberOperationCause(key, VOX_RL_OPERATION_CHANGE_FORCED,
				ePlayer, OperationRemovalReason(*operation));
		}
	}
	return true;
}

// Writes the capture-level CAMPAIGN bytes to their reserved path. The first
// same-turn segment that publishes performs the write; later segments reuse
// the existing file.
bool VoxRlCapture::WriteCampaignBaseline()
{
	if (m_campaignStorage.ByteLength() == 0 || m_campaignRelPath.empty())
	{
		return false;
	}
	const std::string path = m_gameDirectory + "/" + m_campaignRelPath;
	VoxRlOutputFile file;
	bool written = false;
	{
		ScopedTiming timing(m_config.timings, m_segment->timings.campaignWriteFlushNs);
		written = file.OpenNew(path.c_str()) &&
			file.Write(m_campaignStorage.Bytes(), m_campaignFramedLength) && file.Flush();
	}
	if (!written)
	{
		return false;
	}
	if (m_config.timings) m_segment->timings.campaignBuildBytes += m_campaignFramedLength;
	// The bytes travel to disk once; the capture-level copy only served
	// deferred publication.
	m_campaignStorage.Clear();
	return true;
}

void VoxRlCapture::StartSegment(PlayerTypes ePlayer, int iTurn, bool worldReplacement)
{
	// The new WORLD already includes diplomacy changes before this checkpoint.
	m_worldReplacementPending = false;
	m_segment = new Segment();
	Segment& segment = *m_segment;
	segment.player = ePlayer;
	segment.turn = iTurn;
	segment.worldReplacement = worldReplacement;
	segment.staticGeneration = m_staticGeneration;
	if (!BuildWorldBaseline(ePlayer, iTurn))
	{
		FailSegment("worldBuild");
		return;
	}
	char folder[80];
	sprintf_s(folder, 80, "segments/player-%d/turn-%d/world-%u",
		static_cast<int>(ePlayer), iTurn, segment.worldGeneration);
	segment.relativeSegmentDir = folder;
	const std::string segmentDir = m_gameDirectory + "/" + folder;
	segment.streamPath = segmentDir + "/stream.bin";
	segment.indexPath = segmentDir + "/index.jsonl";
	segment.staticRelPath = m_staticRelPath;
	segment.staticFramedLength = m_staticFramedLength;
	// A same-turn WORLD replacement reuses the compatible CAMPAIGN.
	if (m_campaignTurnPlayer == static_cast<unsigned int>(ePlayer) && m_campaignTurn == iTurn &&
		m_campaignStaticGeneration == segment.staticGeneration && m_campaignGeneration != 0)
	{
		segment.campaignGeneration = m_campaignGeneration;
		segment.campaignRelPath = m_campaignRelPath;
		segment.campaignFramedLength = m_campaignFramedLength;
		segment.campaignSeamReached = true;
	}
	m_segmentStreamBytes = 0;
}

void VoxRlCapture::OnPreDangerCheckpoint(PlayerTypes ePlayer)
{
	if (m_concluded)
	{
		// The game concluded; post-victory play is not recorded.
		return;
	}
	if (!ResolveConfiguration())
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	const int iTurn = GC.getGame().getGameTurn();
	if (m_segment != NULL && !m_segment->closed &&
		(m_segment->player != ePlayer || m_segment->turn != iTurn))
	{
		// The previous segment's turn ended: another player's checkpoint or
		// a repeated checkpoint for the same turn starts a fresh segment.
		CloseSegment("turnComplete");
	}
	if (m_segment != NULL && !m_segment->closed)
	{
		// The checkpoint is idempotent within one player turn.
		return;
	}
	if (!AdmitsPlayer(ePlayer))
	{
		if (m_segment != NULL && !m_segment->closed)
		{
			CloseSegment("filterBoundary");
		}
		return;
	}
	if (!DiscoverGameUuid())
	{
		// The MCP synchronization has not established the game ID yet.
		// Capture stays unarmed and retries at the next checkpoint.
		if (!m_identityPendingLogged)
		{
			m_identityPendingLogged = true;
			if (GC.getLogging())
			{
				FILogFile* log = VoxRlCaptureLog();
				if (log != NULL)
				{
					log->Msg("Capture waits for the game identity; the next checkpoint retries.\n");
				}
			}
		}
		return;
	}
	if (!ResolveCaptureRoot())
	{
		return;
	}
	m_gameDirectory = m_captureRoot + "/" + m_gameUuidText;
	// STATIC is reused until a topology invalidation replaces it; WORLD and REQUEST carry
	// the player maps that changed since.
	if (!m_staticGenerationWritten || m_topologyInvalidated)
	{
		const std::string directory = m_gameDirectory + "/baselines/static";
		const unsigned int generation = ReserveGeneration(directory, "static");
		if (generation == 0)
		{
			return;
		}
		m_staticGeneration = generation;
		if (!BuildAndWriteStatic())
		{
			m_staticGenerationWritten = false;
			return;
		}
	}
	StartSegment(ePlayer, iTurn);
}

// Ends board and danger capture while retaining attachment-scoped military evidence.
void VoxRlCapture::OnTurnComplete(PlayerTypes ePlayer)
{
	// Every player's team maps are refreshed at its own turn end, observed or not, so a later
	// WORLD can carry the change.
	RefreshTeamPlayerMaps(ePlayer);
	if (m_segment == NULL || m_segment->player != ePlayer)
	{
		return;
	}
	CloseSegment("turnComplete");
	m_worldReplacementPending = false;
}

void VoxRlCapture::OnCampaignSeam(PlayerTypes ePlayer)
{
	if (!IsObserving(ePlayer))
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	Segment& segment = *m_segment;
	if (segment.campaignGeneration == 0)
	{
		// The entry synchronization request is always emitted, so the campaign
		// reference can bind its exclusive prefix before either block is built.
		if (!BuildAndBindCampaign(ePlayer, segment.turn, segment.worldGeneration,
			segment.nextDeltaSequence + 1))
		{
			FailSegment("campaignBuild");
			return;
		}
	}
	segment.campaignSeamReached = true;
	m_operationState->collectTurnEntryEconomics = true;
	if (!EmitSynchronizationRequest())
	{
		if (m_segment != NULL && !m_segment->failed)
		{
			FailSegment("campaignEntryRequest");
		}
		return;
	}
	if (!segment.published)
	{
		PublishPendingFrames(true);
	}
}

// Appends one suppressed-segment line to suppressed.jsonl at the recording
// root. The line carries the omissions the segment accumulated because no
// committed index will ever hold them. The generation number is omitted
// because ReserveGeneration may hand it to the next segment.
void VoxRlCapture::AppendSuppressedLine(const char* closureReason)
{
	const Segment& segment = *m_segment;
	std::string line = "{\"type\":\"suppressed\",\"player\":";
	AppendInt(line, static_cast<int>(segment.player));
	line += ",\"turn\":";
	AppendInt(line, segment.turn);
	line += ",\"requestCount\":";
	AppendNumber(line, segment.nextDeltaSequence);
	line += ",\"campaignBound\":";
	line += segment.campaignGeneration != 0 ? "true" : "false";
	line += ",\"operationalEntryReached\":";
	line += segment.campaignSeamReached ? "true" : "false";
	line += ",\"worldReplacement\":";
	line += segment.worldReplacement ? "true" : "false";
	line += ",\"suppressionReason\":\"noAdmittedBinding\"";
	line += ",\"closureReason\":";
	AppendJsonString(line, closureReason != NULL ? closureReason : "unknown");
	line += ",\"omissions\":[";
	for (size_t index = 0; index < segment.coverageReasons.size(); ++index)
	{
		if (index != 0) line += ",";
		line += "{\"reason\":";
		AppendJsonString(line, segment.coverageReasons[index].c_str());
		line += "}";
	}
	line += "]}\n";
	VoxRlOutputFile file;
	if (!file.OpenAppend((m_gameDirectory + "/suppressed.jsonl").c_str()) ||
		!file.Write(line.c_str(), static_cast<unsigned int>(line.size())))
	{
		if (GC.getLogging())
		{
			FILogFile* log = VoxRlCaptureLog();
			if (log != NULL)
			{
				log->Msg("Could not append the suppressed-segment line for player=%d turn=%d.\n",
					static_cast<int>(segment.player), segment.turn);
			}
		}
	}
}

void VoxRlCapture::PublishPendingFrames(bool allowWithoutDecision)
{
	Segment& segment = *m_segment;
	if (segment.published || segment.failed)
	{
		return;
	}
	// Publication requires a bound compatible CAMPAIGN and, outside the
	// pending-budget escape hatch, the segment's first decision request.
	// Without both the attempt is a no-op, not a failure.
	if (segment.campaignGeneration == 0)
	{
		return;
	}
	if (!segment.hasDecision && !allowWithoutDecision)
	{
		return;
	}
	// The retained CAMPAIGN bytes are written by whichever same-turn segment
	// publishes first; a later segment finds the file already present.
	if (m_campaignStorage.ByteLength() != 0)
	{
		if (!WriteCampaignBaseline())
		{
			FailSegment("campaignWrite");
			return;
		}
	}
	// The WORLD snapshot was captured at the pre-danger checkpoint. Write it
	// only now, when publication makes this segment's files reachable.
	if (!WriteWorldBaseline())
	{
		FailSegment("worldWrite");
		return;
	}
	// Dependencies must exist before the index publishes them.
	if (!VoxRlFileExists((m_gameDirectory + "/" + segment.staticRelPath).c_str()))
	{
		FailSegment("missingStaticDependency");
		return;
	}
	if (!VoxRlFileExists((m_gameDirectory + "/" + segment.campaignRelPath).c_str()))
	{
		FailSegment("missingCampaignDependency");
		return;
	}
	if (!VoxRlFileExists((m_gameDirectory + "/" + segment.worldRelPath).c_str()))
	{
		FailSegment("missingWorldDependency");
		return;
	}
	// Opening the stream and index creates the segment directory; until
	// this point the segment existed only in memory.
	if (!m_segmentStream.OpenNew(segment.streamPath.c_str()))
	{
		FailSegment("streamOpen");
		return;
	}
	if (!m_segmentIndex.OpenNew(segment.indexPath.c_str()))
	{
		FailSegment("indexOpen");
		return;
	}
	const std::string streamRelPath = segment.relativeSegmentDir + "/stream.bin";
	std::string line = "{\"type\":\"segment\",\"recordingVersion\":0,\"schemaVersion\":";
	{
		char number[32];
		sprintf_s(number, 32, "%u", static_cast<unsigned int>(VoxRlSchemaVersion));
		line += number;
	}
	line += ",\"manifestHash\":\"" + ManifestHashText() + "\"";
	line += ",\"producer\":{\"buildIdentifier\":";
	AppendJsonString(line, CURRENT_GAMECORE_VERSION);
	line += ",\"compiler\":\"msvc\",\"configuration\":\"dll\",\"localModifications\":\"untracked\"}";
	line += ",\"modConfig\":";
	{
		std::string modConfig = CURRENT_GAMECORE_VERSION;
		modConfig += ";IPC_CHANNEL=1";
		AppendJsonString(line, modConfig.c_str());
	}
	line += ",\"gameId\":\"" + std::string(m_gameUuidText) + "\"";
	{
		char number[32];
		sprintf_s(number, 32, "%d", static_cast<int>(segment.player));
		line += ",\"player\":" + std::string(number);
		sprintf_s(number, 32, "%d", segment.turn);
		line += ",\"turn\":" + std::string(number);
		sprintf_s(number, 32, "%u", segment.worldGeneration);
		line += ",\"worldGeneration\":" + std::string(number);
		sprintf_s(number, 32, "%u", segment.staticGeneration);
		line += ",\"staticGeneration\":" + std::string(number);
		sprintf_s(number, 32, "%u", segment.campaignGeneration);
		line += ",\"campaignGeneration\":" + std::string(number);
	}
	line += ",\"dependencies\":{";
	line += "\"static\":{\"path\":\"" + segment.staticRelPath + "\",\"blockKind\":1,\"generation\":" +
		NumberText(segment.staticGeneration) + ",\"framedLength\":\"" + NumberText(segment.staticFramedLength) + "\"}";
	line += ",\"campaign\":{\"path\":\"" + segment.campaignRelPath + "\",\"blockKind\":2,\"generation\":" +
		NumberText(segment.campaignGeneration) + ",\"framedLength\":\"" + NumberText(segment.campaignFramedLength) + "\"}";
	line += ",\"world\":{\"path\":\"" + segment.worldRelPath + "\",\"blockKind\":3,\"generation\":" +
		NumberText(segment.worldGeneration) + ",\"framedLength\":\"" + NumberText(segment.worldFramedLength) + "\"}";
	line += "},\"stream\":{\"path\":\"" + streamRelPath + "\"}";
	line += ",\"filter\":{\"enabled\":true,\"players\":";
	line += m_config.filterPlayers ? "\"filtered\"" : "\"all\"";
	line += ",\"turns\":";
	line += m_config.filterTurns ? "\"filtered\"" : "\"all\"";
	line += "}";
	line += ",\"checkpoint\":{\"kind\":\"postDiplomacy\",\"warReplacement\":";
	line += segment.worldReplacement ? "true" : "false";
	line += "}}\n";
	if (!AppendIndexLine(line.c_str()))
	{
		FailSegment("segmentIndexWrite");
		return;
	}
	segment.published = true;
	// Move the pending frames into the stream, then publish them as the
	// first batch together with the segment record's dependencies.
	const unsigned int pendingBytes = static_cast<unsigned int>(segment.pendingBytes.size());
	bool pendingWritten = false;
	{
		ScopedTiming timing(m_config.timings, segment.timings.pendingPublishWriteNs);
		pendingWritten = m_segmentStream.Write(
			segment.pendingBytes.empty() ? NULL : &segment.pendingBytes[0], pendingBytes);
	}
	if (!pendingWritten)
	{
		FailSegment("pendingPublishWrite");
		return;
	}
	if (m_config.timings) segment.timings.pendingPublishWriteBytes += pendingBytes;
	m_segmentStreamBytes = pendingBytes;
	segment.pendingBytes.clear();
	segment.pendingCount = 0;
	CommitBatch(false, NULL);
}

std::string VoxRlCapture::NumberText(unsigned __int64 value)
{
	std::string text;
	AppendNumber(text, value);
	return text;
}

void VoxRlCapture::CommitBatch(bool closureOnly, const char* closureReason)
{
	Segment& segment = *m_segment;
	if (!segment.published || segment.failed)
	{
		return;
	}
	// Frame lines for every frame appended since the previous commit.
	const unsigned int firstFrame = segment.committedFrameCount;
	std::string batch;
	for (unsigned int index = firstFrame; index < segment.frameTable.size(); ++index)
	{
		const VoxRlFrameEntry& frame = segment.frameTable[index];
		batch += "{\"type\":\"frame\",\"offset\":\"" + NumberText(frame.offset) +
			"\",\"framedLength\":\"" + NumberText(frame.length) +
			"\",\"blockKind\":" + NumberText(static_cast<unsigned int>(frame.blockKind)) +
			",\"requestSequence\":" + NumberText(frame.requestSequence) + ",\"decisionId\":";
		if (frame.decisionId == kVoxRlSyncOnlyDecisionId)
		{
			batch += "null";
		}
		else
		{
			batch += NumberText(frame.decisionId) + ",\"attemptIndex\":" + NumberText(static_cast<unsigned int>(frame.attemptIndex));
		}
		batch += "}\n";
	}
	// Decision lines reference request and result frames in this batch.
	const unsigned int firstDecision = segment.committedDecisionCount;
	for (unsigned int index = firstDecision; index < segment.decisions.size(); ++index)
	{
		const Segment::DecisionRow& row = segment.decisions[index];
		batch += "{\"type\":\"decision\",\"decisionId\":" + NumberText(row.decisionId) +
			",\"attemptIndex\":" + NumberText(static_cast<unsigned int>(row.attemptIndex)) +
			",\"caller\":" + NumberText(static_cast<unsigned int>(row.caller)) +
			",\"retryOutcome\":" + NumberText(static_cast<unsigned int>(row.retryOutcome)) +
			",\"worldGeneration\":" + NumberText(segment.worldGeneration) +
			",\"requestFrame\":" + NumberText(row.requestFrameIndex) +
			",\"resultFrame\":" + NumberText(row.resultFrameIndex) + ",\"previousAttempt\":";
		if (row.hasPrevious)
		{
			batch += "{\"attemptIndex\":" + NumberText(static_cast<unsigned int>(row.previousAttemptIndex)) +
				",\"worldGeneration\":" + NumberText(row.previousWorldGeneration) + "}";
		}
		else
		{
			batch += "null";
		}
		batch += "}\n";
	}
	// Coverage lines describe newly known omissions and are never rewritten.
	std::string coverageBatch;
	for (size_t index = 0; index < segment.coverageReasons.size(); ++index)
	{
		coverageBatch += "{\"type\":\"coverage\",\"omissions\":[{\"reason\":";
		AppendJsonString(coverageBatch, segment.coverageReasons[index].c_str());
		coverageBatch += "}]}\n";
	}
	const unsigned int coverageCount = segment.committedCoverageCount +
		static_cast<unsigned int>(segment.coverageReasons.size());
	segment.coverageReasons.clear();

	std::string commit = "{\"type\":\"commit\",\"commitId\":" + NumberText(segment.commitId + 1) +
		",\"frameCount\":" + NumberText(segment.frameTable.size()) +
		",\"decisionCount\":" + NumberText(segment.decisions.size()) +
		",\"coverageCount\":" + NumberText(coverageCount) +
		",\"streamCommittedLength\":\"" + NumberText(m_segmentStreamBytes) + "\"" +
		",\"firstRequestSequence\":";
	if (segment.hasRequests)
	{
		commit += NumberText(segment.firstRequestSequence);
	}
	else
	{
		commit += "null";
	}
	commit += ",\"lastRequestSequence\":";
	if (segment.hasRequests)
	{
		commit += NumberText(segment.lastRequestSequence);
	}
	else
	{
		commit += "null";
	}
	commit += ",\"state\":\"";
	commit += closureOnly ? "closed" : "open";
	commit += "\",\"closureReason\":";
	if (closureOnly)
	{
		AppendJsonString(commit, closureReason != NULL ? closureReason : "turnComplete");
	}
	else
	{
		commit += "null";
	}
	commit += "}\n";

	bool appended = false;
	{
		ScopedTiming timing(m_config.timings, segment.timings.commitNs);
		// Ordinary commits append the batch and its commit line without disk
		// synchronization: completed writes stay readable through the operating
		// system's file cache, and durability is established only by the closure
		// sequence in CloseSegment.
		appended = AppendIndexLine(batch.c_str()) && AppendIndexLine(coverageBatch.c_str()) &&
			AppendIndexLine(commit.c_str());
	}
	if (!appended)
	{
		FailSegment("commitIndexWrite");
		return;
	}
	if (m_config.timings)
	{
		segment.timings.commitCount += 1;
		segment.timings.indexBytes += batch.size() + coverageBatch.size() + commit.size();
	}
	segment.commitId += 1;
	segment.committedFrameCount = static_cast<unsigned int>(segment.frameTable.size());
	segment.committedDecisionCount = static_cast<unsigned int>(segment.decisions.size());
	segment.committedCoverageCount = coverageCount;
	if (closureOnly)
	{
		segment.closed = true;
	}
}

bool VoxRlCapture::EnqueueFrame(const void* bytes, unsigned int length, int blockKind,
	unsigned int requestSequence, unsigned int decisionId, int attemptIndex)
{
	Segment& segment = *m_segment;
	if (segment.failed)
	{
		return false;
	}
	if (segment.published)
	{
		VoxRlFrameEntry entry;
		entry.offset = m_segmentStreamBytes;
		entry.length = length;
		entry.blockKind = blockKind;
		entry.requestSequence = requestSequence;
		entry.decisionId = decisionId;
		entry.attemptIndex = attemptIndex;
		bool written = false;
		{
			ScopedTiming timing(m_config.timings, segment.timings.streamWriteNs);
			written = m_segmentStream.Write(bytes, length);
		}
		if (!written)
		{
			FailSegment("streamWrite");
			return false;
		}
		if (m_config.timings) segment.timings.streamWriteBytes += length;
		m_segmentStreamBytes += length;
		segment.frameTable.push_back(entry);
		return true;
	}
	// Pre-publication frames wait in bounded memory until the segment
	// publishes. When the budget fills before a decision arrives, a bound
	// compatible CAMPAIGN lets the synchronization backlog publish instead
	// of failing the segment.
	if (segment.pendingBytes.size() + length > kVoxRlPendingBudgetBytes)
	{
		PublishPendingFrames(true);
		if (m_segment == NULL || m_segment->failed || !m_segment->published)
		{
			FailSegment("budgetExceeded");
			return false;
		}
		return EnqueueFrame(bytes, length, blockKind, requestSequence, decisionId, attemptIndex);
	}
	VoxRlFrameEntry entry;
	entry.offset = static_cast<unsigned __int64>(segment.pendingBytes.size());
	entry.length = length;
	entry.blockKind = blockKind;
	entry.requestSequence = requestSequence;
	entry.decisionId = decisionId;
	entry.attemptIndex = attemptIndex;
	const unsigned char* cursor = static_cast<const unsigned char*>(bytes);
	{
		ScopedTiming timing(m_config.timings, segment.timings.pendingBufferNs);
		segment.pendingBytes.insert(segment.pendingBytes.end(), cursor, cursor + length);
	}
	if (m_config.timings) segment.timings.pendingBufferBytes += length;
	segment.pendingCount += 1;
	segment.frameTable.push_back(entry);
	return true;
}

void VoxRlCapture::ClearDirtyState()
{
	Segment& segment = *m_segment;
	segment.dirtyUnits.clear();
	segment.removedUnits.clear();
	segment.dirtyCities.clear();
	segment.removedCities.clear();
	segment.dirtyPlots.clear();
	segment.visibilityFlips.clear();
	segment.visibilityResets.clear();
	segment.revealedOverrideUpserts.clear();
	segment.removedRevealedOverrides.clear();
	segment.dirtyInterceptors.clear();
	segment.pendingDangerEvents.clear();
	segment.dirtyUnitPlagues.clear();
	segment.dirtyUnitBlockedPromotions.clear();
	segment.dirtyUnitAttackCounts.clear();
	segment.dirtyPlayerResistances.clear();
	segment.dirtyCityAttackCounts.clear();
}

void VoxRlCapture::CommitBufferedRelations()
{
	if (m_operationState == NULL || m_operationState->stagedRelationCount == 0)
	{
		return;
	}
	std::vector<TeamRelationRecord>& rows = m_operationState->bufferedRelations;
	const size_t count = (std::min)(m_operationState->stagedRelationCount, rows.size());
	rows.erase(rows.begin(), rows.begin() + count);
	m_operationState->stagedRelationCount = 0;
}

void VoxRlCapture::CommitOperationalRows()
{
	if (m_operationState == NULL || m_segment == NULL) return;
	m_operationState->pendingRowsByOwner.erase(static_cast<int>(m_segment->player));
	m_operationState->collectTurnEntryEconomics = false;
}

unsigned int VoxRlCapture::UnitIterationIndex(PlayerTypes eOwner, int iUnitId)
{
	Segment& segment = *m_segment;
	const VoxRlEntityKey key(static_cast<int>(eOwner), iUnitId);
	std::map<VoxRlEntityKey, unsigned int>::const_iterator found = segment.unitIteration.find(key);
	if (found != segment.unitIteration.end())
	{
		return (*found).second;
	}
	// Assign a fresh owner-relative index for units created after the WORLD
	// build; uniqueness within the owner is what the loader enforces.
	unsigned int& next = segment.nextUnitIteration[static_cast<int>(eOwner)];
	const unsigned int index = next;
	next += 1;
	segment.unitIteration[key] = index;
	return index;
}

unsigned int VoxRlCapture::CityIterationIndex(PlayerTypes eOwner, int iCityId)
{
	Segment& segment = *m_segment;
	const VoxRlEntityKey key(static_cast<int>(eOwner), iCityId);
	std::map<VoxRlEntityKey, unsigned int>::const_iterator found = segment.cityIteration.find(key);
	if (found != segment.cityIteration.end())
	{
		return (*found).second;
	}
	unsigned int& next = segment.nextCityIteration[static_cast<int>(eOwner)];
	const unsigned int index = next;
	next += 1;
	segment.cityIteration[key] = index;
	return index;
}

void VoxRlCapture::SeedWorldIterationIndices()
{
	Segment& segment = *m_segment;
	segment.unitIteration.clear();
	segment.cityIteration.clear();
	segment.nextUnitIteration.clear();
	segment.nextCityIteration.clear();
	// These loops match the WORLD builder's owner-relative first/next order.
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(player));
		unsigned int iteration = 0;
		int loop = 0;
		for (CvUnit* unit = owner.firstUnit(&loop); unit != NULL; unit = owner.nextUnit(&loop))
		{
			if (unit->isDelayedDeath() || unit->plot() == NULL) continue;
			segment.unitIteration[VoxRlEntityKey(player, unit->GetID())] = iteration;
			++iteration;
		}
		segment.nextUnitIteration[player] = iteration;
		iteration = 0;
		loop = 0;
		for (CvCity* city = owner.firstCity(&loop); city != NULL; city = owner.nextCity(&loop))
		{
			segment.cityIteration[VoxRlEntityKey(player, city->GetID())] = iteration;
			++iteration;
		}
		segment.nextCityIteration[player] = iteration;
	}
}

// Checks independent dirty records before rejecting an incomplete request delta.
bool VoxRlCapture::CollectDelta(VoxRlRequestData& data)
{
	ScopedCaptureCollection collecting(m_collectingState);
	bool valid = true;
	Segment& segment = *m_segment;
	// Reject unsupported roster changes before advancing any REQUEST baseline.
	if (!HasCheckpointPlayerRoster(segment.lastPlayerRows))
	{
		FILogFile* log = VoxRlCaptureLog();
		if (log != NULL) log->Msg("Capture REQUEST rejected: player roster changed; a fresh WORLD is required.\n");
		return false;
	}
	WorldGameStateRecord gameState;
	std::memset(&gameState, 0, sizeof(gameState));
	gameState.gameState = static_cast<i32>(GC.getGame().getGameState());
	VoxRlAssignClamped(gameState.elapsedGameTurns, GC.getGame().getGameTurn());
	VoxRlAssignClamped(gameState.maxTurns, GC.getGame().getMaxTurns());
	VoxRlAssignClamped(gameState.currentEra, GC.getGame().getCurrentEra());
	gameState.activePlayer = static_cast<i8>(GC.getGame().getActivePlayer());
	if (std::memcmp(&gameState, &segment.lastGameState, sizeof(gameState)) != 0)
	{
		std::memcpy(&data.requestGameState, &gameState, sizeof(gameState));
		data.hasRequestGameState = true;
		segment.lastGameState = gameState;
	}
	ScopedTiming timing(m_config.timings, segment.timings.deltaCollectNs);
	const PlayerTypes capturingPlayer = segment.player;
	CvPlayerAI& capturing = GET_PLAYER(capturingPlayer);
	const TeamTypes capturingTeam = capturing.getTeam();
	CvTacticalAnalysisMap* zoneMap = capturing.GetTacticalAI()->GetTacticalAnalysisMap();
	CvMap& map = GC.getMap();
	// Operational rows are built at their native mutation or batch boundary.
	// Their range offsets remain valid because these destination sections are empty.
	std::map<int, VoxRlRequestData>::const_iterator operationalRows =
		m_operationState->pendingRowsByOwner.find(static_cast<int>(segment.player));
	if (operationalRows != m_operationState->pendingRowsByOwner.end())
	{
		data.requestOperations = operationalRows->second.requestOperations;
		data.requestOperationVersions = operationalRows->second.requestOperationVersions;
		data.requestArmyVersions = operationalRows->second.requestArmyVersions;
		data.requestFormationEntryVersions = operationalRows->second.requestFormationEntryVersions;
		data.requestCampaignBoundaries = operationalRows->second.requestCampaignBoundaries;
		data.requestZoneChoices = operationalRows->second.requestZoneChoices;
		data.requestFocusAreas = operationalRows->second.requestFocusAreas;
		data.requestWorkerAssignments = operationalRows->second.requestWorkerAssignments;
	}
	// Team resource capabilities can change through technology, policy, or team state.
	// Compare the complete checkpoint roster on every request because those mutations do
	// not share one reliable dirty hook. Changed rows also refresh affected external
	// resource residuals against the same native state.
	std::set<int> resourceRefreshPlayers;
	std::set<int> resourceTeams;
	for (std::map<int, TeamResourceRecord>::const_iterator team = segment.lastTeamResourceRows.begin();
		team != segment.lastTeamResourceRows.end(); ++team)
		resourceTeams.insert(team->first);
	std::vector<TeamResourceRecord> teamResources;
	if (!VoxRlCollectTeamResourceRows(resourceTeams, teamResources))
	{
		valid = false;
		teamResources.clear();
	}
	for (size_t index = 0; index < teamResources.size(); ++index)
	{
		const TeamResourceRecord& record = teamResources[index];
		std::map<int, TeamResourceRecord>::iterator last =
			segment.lastTeamResourceRows.find(static_cast<int>(record.team));
		if (last == segment.lastTeamResourceRows.end()) return false;
		if (std::memcmp(&last->second, &record, sizeof(record)) == 0) continue;
		RequestTeamResourceRecord row;
		std::memcpy(&row, &record, sizeof(row));
		data.requestTeamResources.push_back(row);
		last->second = record;
		for (std::map<int, PlayerRecord>::const_iterator player = segment.lastPlayerRows.begin();
			player != segment.lastPlayerRows.end(); ++player)
		{
			if (GET_PLAYER(static_cast<PlayerTypes>(player->first)).getTeam() == static_cast<TeamTypes>(record.team))
				resourceRefreshPlayers.insert(player->first);
		}
	}
	if (m_operationState->collectTurnEntryEconomics)
	{
		CvPlayerAI& entryPlayer = GET_PLAYER(segment.player);
		resourceRefreshPlayers.insert(static_cast<int>(segment.player));
		RequestPlayerEconomicsRecord economics;
		if (!VoxRlCollectPlayerEconomicsRecord(entryPlayer, economics)) valid = false;
		else data.requestPlayerEconomics.push_back(economics);
	}
	if (!resourceRefreshPlayers.empty() &&
		!VoxRlCollectPlayerResourceRows(resourceRefreshPlayers, data.requestPlayerResources)) valid = false;
	const int plotCount = map.numPlots();
	// Compare the captured roster with its last emitted state, including policy
	// bonuses and empire totals, without requiring setter hooks.
	for (std::map<int, PlayerRecord>::iterator baseline = segment.lastPlayerRows.begin();
		baseline != segment.lastPlayerRows.end(); ++baseline)
	{
		PlayerRecord row;
		std::memset(&row, 0, sizeof(row));
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(baseline->first));
		if (!VoxRlCollectPlayerRecord(owner, capturingPlayer, row))
		{
			valid = false;
			continue;
		}
		row.id = static_cast<i8>(baseline->first);
		if (std::memcmp(&baseline->second, &row, sizeof(row)) == 0) continue;
		RequestDeltaPlayerRecord delta;
		std::memcpy(&delta, &row, sizeof(delta));
		data.requestDeltaPlayers.push_back(delta);
		baseline->second = row;
	}
	if (segment.collectLateTurnState)
	{
		// The actor's map and spy rows change only through its own city turns, policies,
		// research, and espionage, so the pre-tactical REQUEST compares them once. Research
		// also reinitializes every teammate's traits, so living teammates' maps are compared
		// too. The cached map is the one this segment's WORLD and earlier REQUESTs gave the reader.
		const TeamTypes captureTeam = GET_PLAYER(capturingPlayer).getTeam();
		for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
		{
			const PlayerTypes member = static_cast<PlayerTypes>(playerIndex);
			if (member != capturingPlayer && (!GET_PLAYER(member).isAlive() || GET_PLAYER(member).getTeam() != captureTeam)) continue;
			if (!VoxRlAppendChangedPlayerMap(member, m_playerMaps->currentMaps, data)) valid = false;
		}
		if (!VoxRlAppendEspionageSightReplacement(capturingPlayer, segment.lastEspionageRows, data)) valid = false;
		// The economic AI turn usually fills the actor's found-value cache after the checkpoint.
		if (!VoxRlAppendFoundValueReplacement(capturingPlayer, segment.lastFoundValueRows, data)) valid = false;
	}
	VoxRlPlayerCitySnapshot currentPlayerCityState;
	if (!VoxRlCollectPlayerCitySnapshot(currentPlayerCityState, capturingPlayer)) valid = false;
	else
	{
		std::vector<int> playerOwners;
		for (std::map<int, PlayerRecord>::const_iterator player = segment.lastPlayerRows.begin();
			player != segment.lastPlayerRows.end(); ++player) playerOwners.push_back(player->first);
		std::vector<int> teamOwners;
		for (size_t index = 0; index < segment.playerCitySnapshot.teams.size(); ++index)
			teamOwners.push_back(segment.playerCitySnapshot.teams[index].id);
		for (size_t index = 0; index < currentPlayerCityState.teams.size(); ++index)
			if (std::find(teamOwners.begin(), teamOwners.end(), currentPlayerCityState.teams[index].id) == teamOwners.end())
				teamOwners.push_back(currentPlayerCityState.teams[index].id);
		std::sort(teamOwners.begin(), teamOwners.end());
		std::vector<VoxRlCityKey> cityOwners;
		for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
		{
			CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(playerIndex));
			if (!player.isAlive() && playerIndex != BARBARIAN_PLAYER) continue;
			int loop = 0;
			for (CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
				cityOwners.push_back(VoxRlCityKey(playerIndex, city->GetID()));
		}
		if (!VoxRlAppendPlayerCityReplacements(segment.playerCitySnapshot, currentPlayerCityState,
			playerOwners, teamOwners, cityOwners, data)) valid = false;
	}
	RefreshChangedNeedsGarrison(segment.lastCityNeedsGarrison, segment.dirtyCities);
	for (int playerIndex = 0; playerIndex < MAX_PLAYERS; ++playerIndex)
	{
		CvPlayerAI& player = GET_PLAYER(static_cast<PlayerTypes>(playerIndex));
		if (!player.isAlive() && playerIndex != BARBARIAN_PLAYER) continue;
		int loop = 0;
		for (CvCity* city = player.firstCity(&loop); city != NULL; city = player.nextCity(&loop))
		{
			const VoxRlEntityKey key(playerIndex, city->GetID());
			const CityCheckpointScalars current(*city);
			std::map<VoxRlEntityKey, CityCheckpointScalars>::iterator last =
				segment.lastCityCheckpointScalars.find(key);
			if (last == segment.lastCityCheckpointScalars.end() || last->second != current)
			{
				segment.dirtyCities.insert(key);
				segment.lastCityCheckpointScalars.erase(key);
				segment.lastCityCheckpointScalars.insert(std::make_pair(key, current));
			}
		}
	}
	// The zone table is recollected only after a native rebuild marked it dirty.
	// The comparison stays because the native rebuild does not identify changed
	// plots: it accepts the rebuilt table, carries a replacement when the rows
	// differ, and records each plot whose assignment changed. While the marker
	// is clean, the table and every assignment are known unchanged, so the
	// whole pass is skipped.
	std::vector<int> zoneChangedPlots;
	if (m_zoneSnapshotDirty)
	{
		VoxRlZoneSnapshot zones;
		if (!VoxRlCollectZones(zoneMap, zones)) valid = false;
		else
		{
			if (zones.plotZones.size() != segment.zoneSnapshot.plotZones.size()) return false;
			if (!VoxRlSameRows(zones.zones, segment.zoneSnapshot.zones) ||
				!VoxRlSameRows(zones.neighbors, segment.zoneSnapshot.neighbors))
			{
				data.requestHeader.hasZoneReplacement = 1;
				VoxRlMirrorSparseRows(zones.zones, data.requestZoneReplacements);
				VoxRlMirrorSparseRows(zones.neighbors, data.requestZoneNeighbors);
				// The decode table is rebuilt only when a new table is accepted.
				segment.zoneRowByZoneId.clear();
				for (size_t zoneIndex = 0; zoneIndex < zones.zones.size(); ++zoneIndex)
					segment.zoneRowByZoneId[zones.zones[zoneIndex].zoneId] = static_cast<unsigned int>(zoneIndex + 1);
			}
			for (int plot = 0; plot < plotCount; ++plot)
			{
				if (zones.plotZones[plot] != segment.zoneSnapshot.plotZones[plot])
				{
					segment.dirtyPlots.insert(plot);
					zoneChangedPlots.push_back(plot);
				}
			}
			segment.zoneSnapshot.zones.swap(zones.zones);
			segment.zoneSnapshot.neighbors.swap(zones.neighbors);
			segment.zoneSnapshot.plotZones.swap(zones.plotZones);
			m_zoneSnapshotDirty = false;
		}
	}
	// The retained table is exactly the table this request's assignments decode
	// against: the replacement when one is carried, and otherwise the previously
	// accepted table. One-based indices address its ordered rows, and the 16-bit
	// form covers up to 65,535 rows before the wide form takes over.
	const bool wideZoneIndices = segment.zoneSnapshot.zones.size() > 65535U;

	// Removed units come first so an upsert of the same key is refused.
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.removedUnits.begin();
		key != segment.removedUnits.end(); ++key)
	{
		RequestRemovedUnitRecord row;
		std::memset(&row, 0, sizeof(row));
		row.owner = static_cast<i8>((*key).owner);
		row.unitId = static_cast<i32>((*key).id);
		data.requestRemovedUnits.push_back(row);
	}
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.removedCities.begin();
		key != segment.removedCities.end(); ++key)
	{
		RequestRemovedCityRecord row;
		std::memset(&row, 0, sizeof(row));
		row.owner = static_cast<i8>((*key).owner);
		row.cityId = static_cast<i32>((*key).id);
		data.requestRemovedCities.push_back(row);
	}

	// The dirty set deduplicates notifications, so each live dirty unit
	// emits one complete record per flush. The collected full record splits
	// into the wire row and its sparse rows, and the request mirror carries
	// request-local copies of both child row sections.
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyUnits.begin();
		key != segment.dirtyUnits.end(); ++key)
	{
		CvUnit* pUnit = GET_PLAYER(static_cast<PlayerTypes>((*key).owner)).getUnit((*key).id);
		if (pUnit == NULL)
		{
			// The unit died between the mark and the flush; its removal is
			// tracked separately, so nothing upserts here.
			continue;
		}
		if (pUnit->plot() == NULL || pUnit->isDelayedDeath())
		{
			continue;
		}
		UnitRecord record;
		std::memset(&record, 0, sizeof(record));
		if (!VoxRlCollectUnitRecord(*pUnit, capturingTeam, record))
		{
			valid = false;
			continue;
		}
		record.iterationIndex = UnitIterationIndex(static_cast<PlayerTypes>((*key).owner), (*key).id);
		UnitWireRecord wireRow;
		std::vector<UnitSparseFieldRecord> sparseRows;
		VoxRlFieldRangeFailure splitFailure;
		if (!VoxRlSplitUnitRecord(record, wireRow, sparseRows, &splitFailure))
		{
			VoxRlNoteSplitFailure(splitFailure);
			valid = false;
			continue;
		}
		RequestDeltaUnitRecord delta;
		std::memcpy(&delta, &wireRow, sizeof(wireRow));
		// Each delta carries an independent range into the request-local section; the
		// mirrored entries follow delta-row order and an empty range clears both tables.
		std::vector<RequestUnitSparseFieldRecord> requestSparseRows;
		VoxRlMirrorSparseRows(sparseRows, requestSparseRows);
		bool unitValid = true;
		if (!AppendRequestDeltaUnitRecordSparseFieldRange(&delta, &data, requestSparseRows)) unitValid = false;
		if (!unitValid)
		{
			valid = false;
			continue;
		}
		data.requestDeltaUnits.push_back(delta);
		// A changed unit replaces both child tables in full. Empty ranges clear a
		// queue or timed-promotion set left by an earlier checkpoint.
		std::vector<UnitMissionRecord> missions;
		std::vector<UnitPromotionTurnRecord> promotionTurns;
		if (!VoxRlCollectUnitTurnRows(*pUnit, missions, promotionTurns)) valid = false;
		RequestUnitMissionReplacementRecord missionReplacement;
		std::memset(&missionReplacement, 0, sizeof(missionReplacement));
		missionReplacement.owner = static_cast<i8>((*key).owner);
		missionReplacement.unitId = (*key).id;
		std::vector<RequestUnitMissionRowRecord> missionRows(missions.size());
		for (size_t index = 0; index < missions.size(); ++index)
		{
			missionRows[index].queueIndex = missions[index].queueIndex;
			missionRows[index].missionType = missions[index].missionType;
			missionRows[index].data1 = missions[index].data1;
			missionRows[index].data2 = missions[index].data2;
			missionRows[index].flags = missions[index].flags;
			missionRows[index].pushTurn = missions[index].pushTurn;
		}
		if (!AppendRequestUnitMissionReplacementRecordRowRange(&missionReplacement, &data, missionRows)) valid = false;
		data.requestUnitMissionReplacements.push_back(missionReplacement);
		RequestUnitPromotionTurnReplacementRecord promotionReplacement;
		std::memset(&promotionReplacement, 0, sizeof(promotionReplacement));
		promotionReplacement.owner = static_cast<i8>((*key).owner);
		promotionReplacement.unitId = (*key).id;
		std::vector<RequestUnitPromotionTurnRowRecord> promotionRows(promotionTurns.size());
		for (size_t index = 0; index < promotionTurns.size(); ++index)
		{
			promotionRows[index].promotion = promotionTurns[index].promotion;
			promotionRows[index].turnGained = promotionTurns[index].turnGained;
		}
		if (!AppendRequestUnitPromotionTurnReplacementRecordRowRange(&promotionReplacement, &data, promotionRows)) valid = false;
		data.requestUnitPromotionTurnReplacements.push_back(promotionReplacement);
	}

	// Physical plot deltas carry the compact core row plus an optional counter row for
	// each changed plot; zone membership travels in its own assignment rows driven by
	// the native zone comparison. The dedupe map retains the last collected full record,
	// so comparisons run on expanded plot values before the request's tokens exist.
	std::vector<PlotCaptureRecord> changedPlots;
	std::vector<int> changedPlotIndices;
	for (std::set<int>::const_iterator plotIndex = segment.dirtyPlots.begin();
		plotIndex != segment.dirtyPlots.end(); ++plotIndex)
	{
		if (*plotIndex < 0 || *plotIndex >= plotCount) continue;
		CvPlot* plot = map.plotByIndex(*plotIndex);
		if (plot == NULL) continue;
		PlotCaptureRecord record;
		std::memset(&record, 0, sizeof(record));
		if (!VoxRlCollectPlotRecord(*plot, record))
		{
			valid = false;
			continue;
		}
		const VoxRlEntityKey plotKey(-2, *plotIndex);
		std::map<VoxRlEntityKey, PlotCaptureRecord>::const_iterator last =
			segment.lastPlotRows.find(plotKey);
		if (last != segment.lastPlotRows.end() && std::memcmp(&last->second, &record, sizeof(record)) == 0)
		{
			continue;
		}
		changedPlots.push_back(record);
		changedPlotIndices.push_back(*plotIndex);
	}
	// The request carries its own city reference table, built only from the changed
	// plots it carries, so the tokens never reference another block's numbering.
	std::vector<RequestCityReferenceRecord> requestCityReferences;
	std::map<VoxRlCityKey, unsigned int> requestTokenByCity;
	const bool cityReferencesValid = VoxRlBuildCityReferenceTable(changedPlots, &requestCityReferences, &requestTokenByCity);
	if (!cityReferencesValid) valid = false;
	data.requestCityReferences = requestCityReferences;
	// A changed plot can name a city the replica has never received, because a city founded
	// while no segment was open is never marked changed while its plots are. Marking those
	// cities now emits them in this same request, below, so every plot token resolves.
	for (std::map<VoxRlCityKey, unsigned int>::const_iterator reference = requestTokenByCity.begin();
		cityReferencesValid && reference != requestTokenByCity.end(); ++reference)
	{
		const VoxRlEntityKey cityKey((*reference).first.first, (*reference).first.second);
		if (segment.cityIteration.count(cityKey) == 0) segment.dirtyCities.insert(cityKey);
	}
	for (size_t index = 0; cityReferencesValid && index < changedPlots.size(); ++index)
	{
		RequestDeltaPlotCoreRecord delta;
		std::memset(&delta, 0, sizeof(delta));
		VoxRlFieldRangeFailure encodeFailure;
		if (!VoxRlEncodePlotCore(changedPlots[index], requestTokenByCity, delta, &encodeFailure))
		{
			VoxRlNoteSplitFailure(encodeFailure);
			valid = false;
			continue;
		}
		delta.plotIndex = changedPlotIndices[index];
		data.requestDeltaPlots.push_back(delta);
		RequestDeltaPlotSparseCountersRecord counters;
		std::memset(&counters, 0, sizeof(counters));
		if (VoxRlEncodePlotCounters(changedPlots[index], changedPlotIndices[index], counters, &encodeFailure))
		{
			data.requestDeltaPlotSparseCounters.push_back(counters);
		}
		else if (encodeFailure.record != 0)
		{
			VoxRlNoteSplitFailure(encodeFailure);
			valid = false;
			continue;
		}
		segment.lastPlotRows[VoxRlEntityKey(-2, changedPlotIndices[index])] = changedPlots[index];
	}

	// Zone assignment deltas carry every plot whose native zone id changed, encoded
	// against the applicable table. Both widths stay present; exactly one carries rows.
	for (std::vector<int>::const_iterator plotIndex = zoneChangedPlots.begin();
		plotIndex != zoneChangedPlots.end(); ++plotIndex)
	{
		if (*plotIndex < 0 || *plotIndex >= plotCount) return false;
		const i32 plotZone = segment.zoneSnapshot.plotZones[*plotIndex];
		unsigned int zoneTableIndex = 0;
		if (plotZone != -1)
		{
			std::map<int, unsigned int>::const_iterator zoneRow = segment.zoneRowByZoneId.find(plotZone);
			if (zoneRow == segment.zoneRowByZoneId.end()) return false;
			zoneTableIndex = zoneRow->second;
		}
		if (wideZoneIndices)
		{
			RequestDeltaPlotZoneWideRecord row;
			std::memset(&row, 0, sizeof(row));
			row.plotIndex = static_cast<i32>(*plotIndex);
			row.zoneTableIndex = zoneTableIndex;
			data.requestDeltaPlotZonesWide.push_back(row);
		}
		else
		{
			RequestDeltaPlotZoneRecord row;
			std::memset(&row, 0, sizeof(row));
			row.plotIndex = static_cast<i32>(*plotIndex);
			row.zoneTableIndex = static_cast<u16>(zoneTableIndex);
			data.requestDeltaPlotZones.push_back(row);
		}
	}

	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyCities.begin();
		key != segment.dirtyCities.end(); ++key)
	{
		CvCity* pCity = GET_PLAYER(static_cast<PlayerTypes>((*key).owner)).getCity((*key).id);
		if (pCity == NULL)
		{
			continue;
		}
		CityRecord record;
		std::memset(&record, 0, sizeof(record));
		const bool cityValid = VoxRlCollectCityRecord(*pCity, capturingPlayer, record);
		record.iterationIndex = CityIterationIndex(static_cast<PlayerTypes>((*key).owner), (*key).id);
		RequestDeltaCityRecord delta;
		std::memcpy(&delta, &record, sizeof(record));
		// A dirty city is the replacement marker for its complete nonzero
		// building-resource contribution, including resource-only changes.
		std::vector<RequestCityResourceRecord> resourceRows;
		const bool resourcesValid = VoxRlCollectCityResourceRows(*pCity, resourceRows);
		if (!cityValid || !resourcesValid)
		{
			valid = false;
			continue;
		}
		if (!AppendChangedCityReplacement(*key, delta, resourceRows, segment.lastCityRows,
			segment.lastCityResourceRows, data)) continue;
		segment.lastCityNeedsGarrison[*key] = record.needsGarrison ? 1 : 0;
	}

	// The small team passability table is recollected only after a technology
	// ownership change marked it dirty; WORLD builds collect it in full.
	if (m_teamPassabilityDirty)
	{
		std::vector<TeamPassabilityRecord> teamPassability;
		const bool passabilityValid = VoxRlCollectTeamPassabilityRows(teamPassability);
		if (!passabilityValid)
		{
			valid = false;
			teamPassability.clear();
		}
		for (size_t index = 0; index < teamPassability.size(); ++index)
		{
			const TeamPassabilityRecord& record = teamPassability[index];
			std::map<int, TeamPassabilityRecord>::const_iterator last =
				segment.lastTeamPassabilityRows.find(static_cast<int>(record.team));
			if (last != segment.lastTeamPassabilityRows.end() &&
				std::memcmp(&last->second, &record, sizeof(record)) == 0) continue;
			RequestTeamPassabilityRecord row;
			std::memcpy(&row, &record, sizeof(row));
			segment.lastTeamPassabilityRows[static_cast<int>(record.team)] = record;
			data.requestTeamPassability.push_back(row);
		}
		if (passabilityValid) m_teamPassabilityDirty = false;
	}

	// Known-visibility resets travel before the ordinary flips of the same
	// request; the reader clears each named team's complete bitset first.
	for (std::set<int>::const_iterator team = segment.visibilityResets.begin();
		team != segment.visibilityResets.end(); ++team)
	{
		if (segment.capturedTeams.count(*team) == 0) continue;
		RequestVisibilityResetRecord row;
		std::memset(&row, 0, sizeof(row));
		row.team = static_cast<i8>(*team);
		data.requestVisibilityResets.push_back(row);
	}
	// Pending flips group into one masked word row per team, kind, and thirty-two-plot
	// word, built from the flips alone: the mask covers exactly the changed plots, so no
	// native read is needed. The map iterates team-major, so the grouping is deterministic.
	{
		std::map<VoxRlVisibilityWordKey, VoxRlVisibilityWordValue> words;
		for (std::map<VoxRlVisibilityKey, unsigned char>::const_iterator flip = segment.visibilityFlips.begin();
			flip != segment.visibilityFlips.end(); ++flip)
		{
			if (segment.capturedTeams.count((*flip).first.team) == 0) continue;
			const VoxRlVisibilityWordKey key((*flip).first.team, (*flip).first.kind, (*flip).first.plotIndex / 32);
			const unsigned int bit = static_cast<unsigned int>((*flip).first.plotIndex % 32);
			VoxRlVisibilityWordValue& word = words[key];
			word.mask |= 1U << bit;
			if ((*flip).second != 0) word.bits |= 1U << bit;
		}
		for (std::map<VoxRlVisibilityWordKey, VoxRlVisibilityWordValue>::const_iterator word = words.begin();
			word != words.end(); ++word)
		{
			RequestVisibilityWordRecord row;
			std::memset(&row, 0, sizeof(row));
			row.team = static_cast<i8>((*word).first.team);
			row.bitsetKind = static_cast<u8>((*word).first.kind);
			row.wordIndex = static_cast<u16>((*word).first.wordIndex);
			row.mask = (*word).second.mask;
			row.bits = (*word).second.bits;
			data.requestVisibilityWords.push_back(row);
		}
	}

	for (std::map<VoxRlEntityKey, unsigned char>::const_iterator upsert = segment.revealedOverrideUpserts.begin();
		upsert != segment.revealedOverrideUpserts.end(); ++upsert)
	{
		if ((*upsert).second == 0) continue;
		if (segment.capturedTeams.count((*upsert).first.owner) == 0) continue;
		if ((*upsert).first.id < 0 || (*upsert).first.id >= plotCount) continue;
		CvPlot* plot = map.plotByIndex((*upsert).first.id);
		if (plot == NULL) continue;
		RevealedOverrideRecord record;
		std::memset(&record, 0, sizeof(record));
		CollectRevealedOverrideRecord(*plot, static_cast<TeamTypes>((*upsert).first.owner), record);
		RequestRevealedOverrideUpsertRecord row;
		std::memcpy(&row, &record, sizeof(record));
		row.plotIndex = static_cast<i32>((*upsert).first.id);
		row.team = static_cast<i8>((*upsert).first.owner);
		data.requestRevealedOverrideUpserts.push_back(row);
	}
	for (std::set<VoxRlEntityKey>::const_iterator removal = segment.removedRevealedOverrides.begin();
		removal != segment.removedRevealedOverrides.end(); ++removal)
	{
		if (segment.capturedTeams.count((*removal).owner) == 0) continue;
		RequestRemovedRevealedOverrideRecord row;
		std::memset(&row, 0, sizeof(row));
		row.team = static_cast<i8>((*removal).owner);
		row.plotIndex = static_cast<i32>((*removal).id);
		data.requestRemovedRevealedOverrides.push_back(row);
	}

	// Interceptor replacements carry each named player's complete cache in
	// native vector order.
	unsigned int entryCursor = 0;
	for (std::set<int>::const_iterator player = segment.dirtyInterceptors.begin();
		player != segment.dirtyInterceptors.end(); ++player)
	{
		CvPlayerAI& owner = GET_PLAYER(static_cast<PlayerTypes>(*player));
		const std::vector<std::pair<int, int> >& interceptors = owner.GetPossibleInterceptors();
		RequestInterceptorReplacementRecord replacement;
		std::memset(&replacement, 0, sizeof(replacement));
		replacement.player = static_cast<i8>(*player);
		data.requestInterceptorReplacements.push_back(replacement);
		for (size_t index = 0; index < interceptors.size(); ++index)
		{
			RequestInterceptorEntryRecord row;
			std::memset(&row, 0, sizeof(row));
			row.owner = static_cast<i8>(*player);
			row.unitId = static_cast<i32>(interceptors[index].first);
			row.plotIndex = static_cast<i32>(interceptors[index].second);
			data.requestInterceptorEntries.push_back(row);
		}
		entryCursor += static_cast<unsigned int>(interceptors.size());
	}

	// Gap-buffered relation snapshots preserve occurrence order. The one
	// observer-dependent field is evaluated when the carrying segment flushes.
	for (size_t index = 0; index < m_operationState->bufferedRelations.size(); ++index)
	{
		TeamRelationRecord record = m_operationState->bufferedRelations[index];
		if (!IsCapturedRelation(record, segment.capturedRelationTeams)) continue;
		record.canDeclareWar = GET_TEAM(static_cast<TeamTypes>(record.team)).canDeclareWar(
			static_cast<TeamTypes>(record.otherTeam), capturingPlayer) ? 1 : 0;
		RequestTeamRelationRecord row;
		std::memcpy(&row, &record, sizeof(row));
		data.requestTeamRelations.push_back(row);
	}
	m_operationState->stagedRelationCount = m_operationState->bufferedRelations.size();

	// Plagues and blocked promotions come from one native pass per unit, so the two
	// families share a union walk and each emits only the replacements it marked.
	std::set<VoxRlEntityKey> plagueOrBlocked;
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyUnitPlagues.begin();
		key != segment.dirtyUnitPlagues.end(); ++key) plagueOrBlocked.insert(*key);
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyUnitBlockedPromotions.begin();
		key != segment.dirtyUnitBlockedPromotions.end(); ++key) plagueOrBlocked.insert(*key);
	for (std::set<VoxRlEntityKey>::const_iterator key = plagueOrBlocked.begin();
		key != plagueOrBlocked.end(); ++key)
	{
		CvUnit* pUnit = GET_PLAYER(static_cast<PlayerTypes>((*key).owner)).getUnit((*key).id);
		if (pUnit == NULL || pUnit->plot() == NULL || pUnit->isDelayedDeath()) continue;
		std::vector<UnitPlagueRecord> plagues;
		std::vector<UnitBlockedPromotionRecord> blocked;
		VoxRlCollectUnitPlagueRows(static_cast<PlayerTypes>((*key).owner), (*key).id, pUnit, plagues, blocked);
		if (segment.dirtyUnitPlagues.count(*key) != 0)
		{
			RequestUnitPlagueReplacementRecord replacement;
			std::memset(&replacement, 0, sizeof(replacement));
			replacement.owner = static_cast<i8>((*key).owner);
			replacement.unitId = static_cast<i32>((*key).id);
			std::vector<RequestUnitPlagueRowRecord> requestRows;
			requestRows.resize(plagues.size());
			for (size_t index = 0; index < plagues.size(); ++index)
			{
				requestRows[index].plague = plagues[index].plague;
				requestRows[index].domain = plagues[index].domain;
				requestRows[index].applyOnAttack = plagues[index].applyOnAttack;
				requestRows[index].applyOnDefense = plagues[index].applyOnDefense;
				requestRows[index].applyChance = plagues[index].applyChance;
			}
			if (!AppendRequestUnitPlagueReplacementRecordRowRange(&replacement, &data, requestRows)) valid = false;
			data.requestUnitPlagueReplacements.push_back(replacement);
		}
		if (segment.dirtyUnitBlockedPromotions.count(*key) != 0)
		{
			RequestUnitBlockedPromotionReplacementRecord replacement;
			std::memset(&replacement, 0, sizeof(replacement));
			replacement.owner = static_cast<i8>((*key).owner);
			replacement.unitId = static_cast<i32>((*key).id);
			std::vector<RequestUnitBlockedPromotionRowRecord> requestRows;
			requestRows.resize(blocked.size());
			for (size_t index = 0; index < blocked.size(); ++index)
			{
				requestRows[index].promotion = blocked[index].promotion;
			}
			if (!AppendRequestUnitBlockedPromotionReplacementRecordRowRange(&replacement, &data, requestRows)) valid = false;
			data.requestUnitBlockedPromotionReplacements.push_back(replacement);
		}
	}
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyUnitAttackCounts.begin();
		key != segment.dirtyUnitAttackCounts.end(); ++key)
	{
		CvUnit* pUnit = GET_PLAYER(static_cast<PlayerTypes>((*key).owner)).getUnit((*key).id);
		if (pUnit == NULL || pUnit->plot() == NULL || pUnit->isDelayedDeath()) continue;
		std::vector<UnitAttackCountRecord> rows;
		VoxRlCollectUnitAttackCountRows(static_cast<PlayerTypes>((*key).owner), (*key).id, pUnit, rows);
		RequestUnitAttackCountReplacementRecord replacement;
		std::memset(&replacement, 0, sizeof(replacement));
		replacement.owner = static_cast<i8>((*key).owner);
		replacement.unitId = static_cast<i32>((*key).id);
		std::vector<RequestUnitAttackCountRowRecord> requestRows;
		requestRows.resize(rows.size());
		for (size_t index = 0; index < rows.size(); ++index)
		{
			requestRows[index].attackingPlayer = rows[index].attackingPlayer;
			requestRows[index].count = rows[index].count;
		}
		if (!AppendRequestUnitAttackCountReplacementRecordRowRange(&replacement, &data, requestRows)) valid = false;
		data.requestUnitAttackCountReplacements.push_back(replacement);
	}
	for (std::set<int>::const_iterator player = segment.dirtyPlayerResistances.begin();
		player != segment.dirtyPlayerResistances.end(); ++player)
	{
		std::vector<PlayerResistanceRecord> rows;
		VoxRlCollectPlayerResistanceRows(&GET_PLAYER(static_cast<PlayerTypes>(*player)), rows);
		RequestPlayerResistanceReplacementRecord replacement;
		std::memset(&replacement, 0, sizeof(replacement));
		replacement.player = static_cast<i8>(*player);
		std::vector<RequestPlayerResistanceRowRecord> requestRows;
		requestRows.resize(rows.size());
		for (size_t index = 0; index < rows.size(); ++index)
		{
			requestRows[index].opponent = rows[index].opponent;
			requestRows[index].dominationResistance = rows[index].dominationResistance;
		}
		if (!AppendRequestPlayerResistanceReplacementRecordRowRange(&replacement, &data, requestRows)) valid = false;
		data.requestPlayerResistanceReplacements.push_back(replacement);
	}
	for (std::set<VoxRlEntityKey>::const_iterator key = segment.dirtyCityAttackCounts.begin();
		key != segment.dirtyCityAttackCounts.end(); ++key)
	{
		CvCity* pCity = GET_PLAYER(static_cast<PlayerTypes>((*key).owner)).getCity((*key).id);
		if (pCity == NULL) continue;
		std::vector<CityAttackCountRecord> rows;
		VoxRlCollectCityAttackCountRows(pCity, rows);
		RequestCityAttackCountReplacementRecord replacement;
		std::memset(&replacement, 0, sizeof(replacement));
		replacement.cityOwner = static_cast<i8>((*key).owner);
		replacement.cityId = static_cast<i32>((*key).id);
		std::vector<RequestCityAttackCountRowRecord> requestRows;
		requestRows.resize(rows.size());
		for (size_t index = 0; index < rows.size(); ++index)
		{
			requestRows[index].attackingPlayer = rows[index].attackingPlayer;
			requestRows[index].count = rows[index].count;
		}
		if (!AppendRequestCityAttackCountReplacementRecordRowRange(&replacement, &data, requestRows)) valid = false;
		data.requestCityAttackCountReplacements.push_back(replacement);
	}

	VoxRlRequestData currentTrade;
	if (!VoxRlCollectRequestTrade(currentTrade)) valid = false;
	else AppendChangedTradeRoster(currentTrade, segment.lastTradeConnections,
		segment.lastTradePathPlots, data);
	// Pending danger events carry their event-time board in this request.
	data.requestDangerEvents = segment.pendingDangerEvents;
	// Military events may have occurred outside the observer's segment.
	// Collection stages a prefix that is consumed only after enqueue succeeds.
	if (!VoxRlCollectMilitaryEvents(segment.player, data)) valid = false;
	if (m_config.timings)
	{
		segment.timings.deltaCollectCount += 1;
		segment.timings.deltaRows += CountRequestRows(data);
	}
	return valid;
}

// Builds one REQUEST block, enqueues it, and commits the producer state the
// staged and synchronization emitters share. recordTimings selects the staged
// path's build timing and request counters.
bool VoxRlCapture::EnqueueRequestBlock(VoxRlRequestData& data, const char* buildFailureReason, bool recordTimings)
{
	Segment& segment = *m_segment;
	InitializeRequestHeader(data);
	VoxRlBlockIdentity identity;
	identity.session = m_gameUuid;
	identity.turn = segment.turn;
	identity.player = static_cast<int>(segment.player);
	identity.staticGeneration = segment.staticGeneration;
	identity.worldGeneration = segment.worldGeneration;
	identity.campaignGeneration = segment.campaignGeneration;
	identity.deltaSequence = segment.nextDeltaSequence;
	identity.decisionId = kVoxRlSyncOnlyDecisionId;

	VoxRlOwnedBlockStorage storage;
	unsigned int length = 0;
	bool built = false;
	if (recordTimings)
	{
		ScopedTiming timing(m_config.timings, segment.timings.requestBuildNs);
		built = VoxRlBuildRequestBlock(identity, data, storage, length);
	}
	else
	{
		built = VoxRlBuildRequestBlock(identity, data, storage, length);
	}
	if (!built)
	{
		FailSegment(buildFailureReason);
		return false;
	}
	if (recordTimings && m_config.timings)
	{
		segment.timings.requestCount += 1;
		segment.timings.requestBytes += length;
	}
	if (!EnqueueFrame(storage.Bytes(), length, VOX_RL_BLOCK_REQUEST,
		segment.nextDeltaSequence, kVoxRlSyncOnlyDecisionId, -1))
	{
		return false;
	}
	if (!segment.hasRequests)
	{
		segment.hasRequests = true;
		segment.firstRequestSequence = segment.nextDeltaSequence;
	}
	segment.lastRequestSequence = segment.nextDeltaSequence;
	segment.nextDeltaSequence += 1;
	VoxRlCommitMilitaryEvents(segment.player);
	CommitBufferedRelations();
	CommitOperationalRows();
	ClearDirtyState();
	return true;
}

bool VoxRlCapture::EmitStagedRequest()
{
	Segment& segment = *m_segment;
	if (segment.failed)
	{
		return false;
	}
	if (!EnqueueRequestBlock(segment.staged, "stagedRequestBuild", true))
	{
		return false;
	}
	segment.staged = VoxRlRequestData();
	segment.stagingRequest = false;
	if (!segment.published && segment.campaignSeamReached)
	{
		PublishPendingFrames(true);
	}
	else if (segment.published)
	{
		CommitBatch(false, NULL);
	}
	return true;
}

void VoxRlCapture::InitializeRequestHeader(VoxRlRequestData& data)
{
	Segment& segment = *m_segment;
	data.requestHeader.phase = static_cast<u8>(VoxRlGetMilitaryEventPhase(segment.player, segment.turn));
	data.requestHeader.order = VoxRlTakeMilitaryEventOrderFor(segment.player, segment.turn);
	data.requestHeader.operationOwner = static_cast<i8>(NO_PLAYER);
	data.requestHeader.operationId = -1;
	data.requestHeader.zoneId = -1;
	data.requestHeader.assessmentWorldGeneration = kVoxRlAbsentGeneration;
	data.requestHeader.assessmentDeltaSequence = 0;
	if (m_operationState->stanceAssessmentValid &&
		m_operationState->assessmentPlayer == segment.player &&
		m_operationState->assessmentTurn == segment.turn)
	{
		data.requestHeader.assessmentWorldGeneration = m_operationState->assessmentWorldGeneration;
		data.requestHeader.assessmentDeltaSequence = m_operationState->assessmentDeltaSequence;
	}
	const OperationCauseContext* context = FindOperationCause(segment.player, true);
	if (context != NULL)
	{
		data.requestHeader.operationOwner = static_cast<i8>(context->owner);
		data.requestHeader.operationId = context->operationId;
	}
}

bool VoxRlCapture::EmitSynchronizationRequest()
{
	if (m_segment == NULL || m_segment->failed || m_segment->stagingRequest)
	{
		return false;
	}
	Segment& segment = *m_segment;
	VoxRlRequestData data;
	if (!CollectDelta(data))
	{
		FailSegment("synchronizationDeltaCollect");
		return false;
	}
	if (!EnqueueRequestBlock(data, "synchronizationRequestBuild", false))
	{
		return false;
	}
	if (!segment.published && segment.campaignSeamReached)
	{
		PublishPendingFrames(true);
	}
	else if (segment.published)
	{
		CommitBatch(false, NULL);
	}
	return m_segment != NULL && !m_segment->failed;
}

// Stages the board before the complete native danger refresh.
void VoxRlCapture::OnDangerRefreshBegin(const CvDangerPlots& danger)
{
	// City collectors can refresh danger before the outer request is staged.
	if (m_collectingState) return;
	if (m_searchActive)
	{
		// A decision request/result pair is in flight. Replay reexecutes its
		// native search from the recorded input, so do not publish a separate
		// synchronization request or replace the segment here.
		return;
	}
	if (m_segment == NULL || m_segment->failed)
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	if (danger.GetObserver() != m_segment->player)
	{
		return;
	}
	// The load path rebuilds from a save on the UI thread; ignore it.
	if (danger.GetTurnBuilt() == -1)
	{
		return;
	}
	if (m_segment->stagingRequest)
	{
		return;
	}
	// A pending war replacement builds the new WORLD at this actual
	// pre-refresh checkpoint, before the refresh consumes the board.
	if (m_worldReplacementPending)
	{
		m_worldReplacementPending = false;
		const PlayerTypes player = m_segment->player;
		const int turn = m_segment->turn;
		CloseSegment("worldReplacement");
		StartSegment(player, turn, true);
		if (m_segment == NULL || m_segment->failed)
		{
			return;
		}
		AddCoverageOmission("warReplacement");
	}
	// Stage the event-time board with the pending dirty event, then add
	// this refresh event. The delta collects directly into the segment's
	// staged request, so the staged vectors are transferred, not copied.
	if (!CollectDelta(m_segment->staged))
	{
		FailSegment("refreshDeltaCollect");
		return;
	}
	RequestDangerEventRecord event;
	std::memset(&event, 0, sizeof(event));
	event.kind = 2;
	event.observer = static_cast<i8>(m_segment->player);
	event.owner = static_cast<i8>(NO_PLAYER);
	event.unitId = -1;
	event.turn = 0;
	m_segment->staged.requestDangerEvents.push_back(event);
	m_segment->stagingRequest = true;
}

// Publishes the staged inputs after all native refresh passes finish.
void VoxRlCapture::OnDangerRefreshComplete(const CvDangerPlots& danger)
{
	// A refresh triggered by collection must not publish the enclosing request.
	if (m_collectingState) return;
	if (m_searchActive)
	{
		return;
	}
	if (m_segment == NULL || m_segment->failed || !m_segment->stagingRequest)
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	if (danger.GetObserver() != m_segment->player)
	{
		return;
	}
	// The refresh is complete; publish its synchronization request.
	EmitStagedRequest();
}

// Stages a discovery after native eligibility and duplicate checks pass.
void VoxRlCapture::OnDangerDiscoveryBegin(const CvDangerPlots& danger, const CvUnit* pUnit)
{
	if (m_collectingState) return;
	if (m_segment == NULL || m_segment->failed || m_segment->stagingRequest)
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	if (danger.GetObserver() != m_segment->player)
	{
		return;
	}
	// Stage the event-time board the discovery consumes. The delta collects
	// directly into the segment's staged request, so the staged vectors are
	// transferred, not copied.
	if (!CollectDelta(m_segment->staged))
	{
		FailSegment("knownAttackerDeltaCollect");
		return;
	}
	RequestKnownAttackerRecord row;
	std::memset(&row, 0, sizeof(row));
	row.observer = static_cast<i8>(m_segment->player);
	row.owner = static_cast<i8>(pUnit->getOwner());
	row.unitId = static_cast<i32>(pUnit->GetID());
	m_segment->staged.requestKnownAttackers.push_back(row);
	m_segment->stagingRequest = true;
}

// Publishes inputs for the successful native discovery.
void VoxRlCapture::OnDangerDiscoveryEnd(const CvDangerPlots& danger)
{
	if (m_collectingState) return;
	if (m_segment == NULL || m_segment->failed || !m_segment->stagingRequest)
	{
		return;
	}
	if (gDLL == NULL || !gDLL->IsGameCoreThread())
	{
		return;
	}
	if (danger.GetObserver() != m_segment->player)
	{
		return;
	}
	EmitStagedRequest();
}

void VoxRlCapture::NoteDangerDirty(PlayerTypes eObserver)
{
	if (!IsObserving(eObserver))
	{
		return;
	}
	Segment& segment = *m_segment;
	RequestDangerEventRecord event;
	std::memset(&event, 0, sizeof(event));
	event.kind = 1;
	event.observer = static_cast<i8>(eObserver);
	event.owner = static_cast<i8>(NO_PLAYER);
	event.unitId = -1;
	event.turn = 0;
	segment.pendingDangerEvents.push_back(event);
}

void VoxRlCapture::BeginEngagement(int callerType, PlayerTypes ePlayer)
{
	if (m_concluded)
	{
		return;
	}
	if (!ResolveConfiguration())
	{
		return;
	}
	if (m_engagement == NULL)
	{
		m_engagement = new Engagement();
	}
	if (m_engagement->active)
	{
		return;
	}
	m_engagement->active = true;
	m_engagement->callerType = callerType;
	m_engagement->player = ePlayer;
	// Decision identifiers are monotonic within a game attachment across
	// both caller paths, starting at one; zero stays synchronization-only.
	m_decisionIdCounter += 1;
	m_engagement->decisionId = m_decisionIdCounter;
	m_engagement->attemptIndex = 0;
	m_engagement->lastOutcome = VOX_RL_RETRY_FIRST;
	m_engagement->lastAttemptWorldGeneration = kVoxRlAbsentGeneration;
	m_engagement->lastSearchEmpty = false;
}

void VoxRlCapture::EndEngagement()
{
	if (m_engagement != NULL)
	{
		m_engagement->active = false;
	}
}

void VoxRlCapture::NoteExecutionResult(bool bSuccess)
{
	if (m_engagement == NULL || !m_engagement->active)
	{
		return;
	}
	if (!m_engagement->lastSearchEmpty && !bSuccess)
	{
		m_engagement->lastOutcome = VOX_RL_RETRY_PREVIOUS_EXECUTION_FAILED;
	}
}

std::vector<STacticalAssignment> VoxRlCapture::RunNativeSearch(const std::vector<CvUnit*>& vUnits,
	CvPlot* pTarget, int eAggression, std::set<int>& unuseableUnits,
	bool bTargetDistanceRelevant, bool bReturnToStartPositions, int iSaveMovement,
	const STacticalFlavors& flavors)
{
	return TacticalAIHelpers::FindBestUnitAssignments(vUnits, pTarget,
		static_cast<eAggressionLevel>(eAggression), unuseableUnits, bTargetDistanceRelevant,
		bReturnToStartPositions, iSaveMovement, flavors);
}

std::vector<STacticalAssignment> VoxRlCapture::SearchAssignments(int callerType, SearchIntent eSearchIntent, PlayerTypes ePlayer,
	const std::vector<CvUnit*>& vUnits, CvPlot* pTarget, int eAggression,
	std::set<int>& unuseableUnits, bool bTargetDistanceRelevant,
	bool bReturnToStartPositions, int iSaveMovement, const STacticalFlavors& flavors)
{
	std::vector<STacticalAssignment> results;
	const bool ephemeral = m_engagement == NULL || !m_engagement->active || m_engagement->player != ePlayer;
	if (ephemeral)
	{
		// The opportunity caller runs single-attempt engagements of its own.
		BeginEngagement(callerType, ePlayer);
	}
	const bool capture = !m_shuttingDown && !m_concluded && IsObserving(ePlayer) && pTarget != NULL &&
		m_engagement != NULL && m_engagement->active && m_engagement->player == ePlayer;
	if (capture)
	{
		RunCapturedSearch(callerType, eSearchIntent, ePlayer, vUnits, pTarget, eAggression, unuseableUnits,
			bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors, results);
	}
	else
	{
		results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
			bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
	}
	if (ephemeral)
	{
		EndEngagement();
	}
	return results;
}

void VoxRlCapture::RunCapturedSearch(int callerType, SearchIntent eSearchIntent, PlayerTypes ePlayer,
	const std::vector<CvUnit*>& vUnits, CvPlot* pTarget, int eAggression,
	std::set<int>& unuseableUnits, bool bTargetDistanceRelevant,
	bool bReturnToStartPositions, int iSaveMovement, const STacticalFlavors& flavors,
	std::vector<STacticalAssignment>& results)
{
	Segment& segment = *m_segment;
	Engagement& engagement = *m_engagement;
	// 1. Carry pending board changes and danger dirty events in the decision
	// request alongside its caller context. Refreshes and discoveries publish
	// their event-time inputs at their own native boundaries.
	VoxRlRequestData data;
	if (!CollectDelta(data))
	{
		FailSegment("decisionDeltaCollect");
		results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
			bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
		return;
	}
	InitializeRequestHeader(data);
	data.requestHeader.callerType = static_cast<u8>(callerType);
	data.requestHeader.searchIntent = static_cast<u8>(eSearchIntent);
	VoxRlAssignClamped(data.requestHeader.attemptIndex, static_cast<i32>(engagement.attemptIndex));
	data.requestHeader.retryOutcome = static_cast<u8>(engagement.lastOutcome);
	// The resolved flavors this search runs with, and the zone modifiers it could read.
	for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
		data.requestHeader.militaryFlavors[slot] = static_cast<u8>(flavors.iFlavor[slot]);
	data.requestHeader.militaryRiskSet = flavors.bRiskSet ? 1 : 0;
	data.requestHeader.flavorCounterpart = static_cast<i8>(flavors.iCounterpart);
	const std::map<int, std::vector<short> >& zoneModifiers =
		GET_PLAYER(ePlayer).GetTacticalAI()->GetTacticalAnalysisMap()->GetAllZoneTacticalFlavorModifiers();
	for (std::map<int, std::vector<short> >::const_iterator zone = zoneModifiers.begin(); zone != zoneModifiers.end(); ++zone)
	{
		RequestZoneFlavorModifierRecord row;
		std::memset(&row, 0, sizeof(row));
		row.zoneId = zone->first;
		for (int slot = 0; slot < kMilitaryFlavorCount; ++slot)
			row.militaryFlavorModifiers[slot] = static_cast<i16>(zone->second[slot]);
		data.requestZoneFlavorModifiers.push_back(row);
	}
	data.requestHeader.previousAttemptWorldGeneration = engagement.attemptIndex == 0
		? kVoxRlAbsentGeneration : engagement.lastAttemptWorldGeneration;
	data.requestHeader.targetPlotIndex = pTarget != NULL ? static_cast<i16>(pTarget->GetPlotIndex()) : static_cast<i16>(kVoxRlAbsentPlotIndex);
	VoxRlAssignClamped(data.requestHeader.saveMovement, iSaveMovement);
	data.requestHeader.aggressionLevel = static_cast<u8>(eAggression);
	data.requestHeader.targetDistanceRelevant = bTargetDistanceRelevant ? 1 : 0;
	data.requestHeader.returnToStartPositions = bReturnToStartPositions ? 1 : 0;
	if (pTarget != NULL && m_operationState->stanceAssessmentValid &&
		m_operationState->assessmentPlayer == ePlayer &&
		m_operationState->assessmentTurn == segment.turn)
	{
		data.requestHeader.zoneId = GET_PLAYER(ePlayer).GetTacticalAI()->
			GetTacticalAnalysisMap()->GetDominanceZoneIDWithoutRefresh(pTarget->GetPlotIndex());
	}
	for (size_t index = 0; index < vUnits.size(); ++index)
	{
		RequestParticipantRecord row;
		std::memset(&row, 0, sizeof(row));
		row.owner = static_cast<i8>(ePlayer);
		if (vUnits[index] != NULL)
		{
			row.unitId = static_cast<i32>(vUnits[index]->GetID());
		}
		else
		{
			row.unitId = -1;
		}
		data.requestParticipants.push_back(row);
	}
	for (std::set<int>::const_iterator unit = unuseableUnits.begin(); unit != unuseableUnits.end(); ++unit)
	{
		RequestDroppedUnitRecord row;
		std::memset(&row, 0, sizeof(row));
		row.owner = static_cast<i8>(ePlayer);
		row.unitId = static_cast<i32>(*unit);
		data.requestDroppedUnits.push_back(row);
	}

	VoxRlBlockIdentity identity;
	identity.session = m_gameUuid;
	identity.turn = segment.turn;
	identity.player = static_cast<int>(segment.player);
	identity.staticGeneration = segment.staticGeneration;
	identity.worldGeneration = segment.worldGeneration;
	identity.campaignGeneration = segment.campaignGeneration;
	identity.deltaSequence = segment.nextDeltaSequence;
	identity.decisionId = engagement.decisionId;

	VoxRlOwnedBlockStorage requestStorage;
	unsigned int requestLength = 0;
	bool requestBuilt = false;
	{
		ScopedTiming timing(m_config.timings, segment.timings.requestBuildNs);
		requestBuilt = VoxRlBuildRequestBlock(identity, data, requestStorage, requestLength);
	}
	if (!requestBuilt)
	{
		FailSegment("decisionRequestBuild");
		results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
			bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
		return;
	}
	if (m_config.timings)
	{
		segment.timings.requestCount += 1;
		segment.timings.requestBytes += requestLength;
	}
	// The decision request makes this segment publishable. Publication runs
	// before the request frame is enqueued so the request, its result, and
	// the decision row commit together in one batch; the reader rejects a
	// committed decision request without its row. A segment started by a
	// mid-turn war replacement after the seam publishes here at its first
	// decision instead of waiting for a second seam.
	segment.hasDecision = true;
	if (!segment.published)
	{
		PublishPendingFrames();
		if (m_segment == NULL || m_segment->failed)
		{
			results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
				bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
			return;
		}
	}
	if (!EnqueueFrame(requestStorage.Bytes(), requestLength, VOX_RL_BLOCK_REQUEST,
		segment.nextDeltaSequence, engagement.decisionId, engagement.attemptIndex))
	{
		results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
			bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
		return;
	}
	const unsigned int requestFrameIndex = static_cast<unsigned int>(segment.frameTable.size() - 1);
	if (!segment.hasRequests)
	{
		segment.hasRequests = true;
		segment.firstRequestSequence = segment.nextDeltaSequence;
	}
	segment.lastRequestSequence = segment.nextDeltaSequence;
	segment.nextDeltaSequence += 1;
	VoxRlCommitMilitaryEvents(segment.player);
	CommitBufferedRelations();
	CommitOperationalRows();
	ClearDirtyState();

	// 2. Invoke native search exactly once, keeping nested refreshes in this decision.
	m_searchActive = true;
	results = RunNativeSearch(vUnits, pTarget, eAggression, unuseableUnits,
		bTargetDistanceRelevant, bReturnToStartPositions, iSaveMovement, flavors);
	m_searchActive = false;

	// 3. Snapshot assignments into the result block.
	VoxRlResultData assignments;
	if (!VoxRlCollectAssignmentRows(ePlayer, results, assignments))
	{
		FailSegment("assignmentCollect");
		return;
	}

	VoxRlOwnedBlockStorage resultStorage;
	unsigned int resultLength = 0;
	bool resultBuilt = false;
	{
		ScopedTiming timing(m_config.timings, segment.timings.resultBuildNs);
		resultBuilt = assignments.Write(identity, resultStorage, resultLength);
	}
	if (!resultBuilt)
	{
		FailSegment("resultBuild");
		return;
	}
	if (m_config.timings)
	{
		segment.timings.resultCount += 1;
		segment.timings.resultBytes += resultLength;
	}
	if (!EnqueueFrame(resultStorage.Bytes(), resultLength, VOX_RL_BLOCK_RESULT,
		segment.lastRequestSequence, engagement.decisionId, engagement.attemptIndex))
	{
		return;
	}
	const unsigned int resultFrameIndex = static_cast<unsigned int>(segment.frameTable.size() - 1);

	// 4. Record the decision row and publish the pair as one batch.
	Segment::DecisionRow decision;
	decision.decisionId = engagement.decisionId;
	decision.attemptIndex = engagement.attemptIndex;
	decision.caller = callerType;
	decision.retryOutcome = engagement.lastOutcome;
	decision.requestFrameIndex = requestFrameIndex;
	decision.resultFrameIndex = resultFrameIndex;
	decision.hasPrevious = engagement.attemptIndex > 0;
	decision.previousAttemptIndex = engagement.attemptIndex - 1;
	decision.previousWorldGeneration = engagement.lastAttemptWorldGeneration;
	segment.decisions.push_back(decision);
	const unsigned int completedWorldGeneration = segment.worldGeneration;
	if (segment.published)
	{
		CommitBatch(false, NULL);
		if (m_segment == NULL || m_segment->failed)
		{
			return;
		}
	}

	// 5. Update the engagement for a possible retry. An empty result sets
	// the retry outcome directly; an execution failure is reported by the
	// caller loop before the next attempt runs.
	engagement.lastSearchEmpty = results.empty();
	if (results.empty())
	{
		engagement.lastOutcome = VOX_RL_RETRY_PREVIOUS_EMPTY;
	}
	engagement.lastAttemptWorldGeneration = completedWorldGeneration;
	engagement.attemptIndex += 1;
}

void VoxRlCapture::NoteUnitChanged(PlayerTypes eOwner, int iUnitId)
{
	if (m_segment == NULL || m_segment->failed) return;
	// A pending removal outlives ordinary change marks: native deletion still moves
	// the unit off its plot after the removal mark, and a delayed death may be touched
	// again before it is deleted. Only a creation mark revives the key.
	const VoxRlEntityKey key(static_cast<int>(eOwner), iUnitId);
	if (m_segment->removedUnits.count(key) != 0) return;
	m_segment->dirtyUnits.insert(key);
}

void VoxRlCapture::NoteUnitPromotionsChanged(PlayerTypes eOwner, int iUnitId)
{
	if (m_segment == NULL || m_segment->failed) return;
	NoteUnitChanged(eOwner, iUnitId);
	// Promotions change the sparse plague and blocked-promotion rows of this unit.
	const VoxRlEntityKey key(static_cast<int>(eOwner), iUnitId);
	m_segment->dirtyUnitPlagues.insert(key);
	m_segment->dirtyUnitBlockedPromotions.insert(key);
}

void VoxRlCapture::NoteUnitCreated(PlayerTypes eOwner, int iUnitId, int creationReason,
	const CvUnit* pSourceUnit)
{
	CvUnit* pUnit = GET_PLAYER(eOwner).getUnit(iUnitId);
	if (pUnit != NULL)
	{
		// Arrival evidence is retained before the segment gate because gifts
		// and other creations can occur during another actor's turn.
		VoxRlNoteMilitaryUnitCreated(*pUnit, creationReason, pSourceUnit);
	}
	if (m_segment == NULL || m_segment->failed) return;
	const VoxRlEntityKey key(static_cast<int>(eOwner), iUnitId);
	m_segment->removedUnits.erase(key);
	NoteUnitChanged(eOwner, iUnitId);
	m_segment->dirtyUnitPlagues.insert(key);
	m_segment->dirtyUnitBlockedPromotions.insert(key);
	m_segment->dirtyUnitAttackCounts.insert(key);
}

void VoxRlCapture::NoteUnitRemoved(PlayerTypes eOwner, int iUnitId)
{
	if (m_segment == NULL || m_segment->failed) return;
	const VoxRlEntityKey key(static_cast<int>(eOwner), iUnitId);
	// Refresh retained cities that last named this owner-qualified unit, even after native
	// state has already removed the unit or cleared the reverse garrison link.
	for (std::map<VoxRlEntityKey, RequestDeltaCityRecord>::const_iterator city = m_segment->lastCityRows.begin();
		city != m_segment->lastCityRows.end(); ++city)
	{
		if (city->second.garrisonOwner == eOwner && city->second.garrisonUnitId == iUnitId)
			m_segment->dirtyCities.insert(city->first);
	}
	m_segment->dirtyUnits.erase(key);
	// Only a unit the replica knows needs a removal row: one present since the WORLD
	// build or already flushed as a creation. Forgetting it here makes the mark
	// idempotent, so the final deletion of a delayed death emits nothing more, and a
	// unit created and removed before any flush leaves no trace.
	if (m_segment->unitIteration.erase(key) != 0)
	{
		m_segment->removedUnits.insert(key);
	}
	// Deletion removes the unit's associated sparse rows, so no family
	// tombstone or collection work remains for it.
	m_segment->dirtyUnitPlagues.erase(key);
	m_segment->dirtyUnitBlockedPromotions.erase(key);
	m_segment->dirtyUnitAttackCounts.erase(key);
}

void VoxRlCapture::NoteCityChanged(PlayerTypes eOwner, int iCityId)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyCities.insert(VoxRlEntityKey(static_cast<int>(eOwner), iCityId));
	m_segment->removedCities.erase(VoxRlEntityKey(static_cast<int>(eOwner), iCityId));
}

void VoxRlCapture::NoteCityCreated(PlayerTypes eOwner, int iCityId)
{
	if (m_segment == NULL || m_segment->failed) return;
	NoteCityChanged(eOwner, iCityId);
	m_segment->dirtyCityAttackCounts.insert(VoxRlEntityKey(static_cast<int>(eOwner), iCityId));
}

void VoxRlCapture::NoteCityRemoved(PlayerTypes eOwner, int iCityId)
{
	if (m_segment == NULL || m_segment->failed) return;
	const VoxRlEntityKey key(static_cast<int>(eOwner), iCityId);
	// Removing a city also invalidates plot references whose native getters now
	// resolve to another city or NULL, even without a plot ownership change.
	for (std::map<VoxRlEntityKey, PlotCaptureRecord>::const_iterator plot = m_segment->lastPlotRows.begin();
		plot != m_segment->lastPlotRows.end(); ++plot)
	{
		const PlotCaptureRecord& row = plot->second;
		if ((row.owningCityOwner == eOwner && row.owningCityId == iCityId) ||
			(row.effectiveOwningCityOwner == eOwner && row.effectiveOwningCityId == iCityId))
			m_segment->dirtyPlots.insert(plot->first.id);
	}
	m_segment->dirtyCities.erase(key);
	// Only a city the replica knows needs a removal row, matching NoteUnitRemoved. A city
	// created and removed before any flush, such as one liberated in the same window it
	// was captured in, leaves no trace.
	if (m_segment->cityIteration.erase(key) != 0)
	{
		m_segment->removedCities.insert(key);
	}
	m_segment->lastCityRows.erase(key);
	m_segment->lastCityResourceRows.erase(key);
	m_segment->lastCityNeedsGarrison.erase(key);
	m_segment->lastCityCheckpointScalars.erase(key);
	// Deletion removes the city's associated sparse rows, so no family
	// tombstone or collection work remains for it.
	m_segment->dirtyCityAttackCounts.erase(key);
}

void VoxRlCapture::NotePlotChanged(int iPlotIndex)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyPlots.insert(iPlotIndex);
}

void VoxRlCapture::NoteVisibilityChanged(TeamTypes eTeam, int iPlotIndex, int iBitsetKind, bool bValue)
{
	if (m_segment == NULL || m_segment->failed) return;
	if (eTeam == NO_TEAM) return;
	const VoxRlVisibilityKey key(static_cast<int>(eTeam), iPlotIndex, iBitsetKind);
	m_segment->visibilityFlips[key] = bValue ? 1 : 0;
}

void VoxRlCapture::NoteKnownVisibilityReset(TeamTypes eTeam)
{
	if (m_segment == NULL || m_segment->failed) return;
	if (eTeam == NO_TEAM) return;
	Segment& segment = *m_segment;
	segment.visibilityResets.insert(static_cast<int>(eTeam));
	// The reset supersedes this team's pending known-visible entries. The map
	// is keyed team-major, so the team's range is contiguous and the scan never
	// touches other teams' entries; other bitset kinds are preserved.
	std::map<VoxRlVisibilityKey, unsigned char>::iterator flip =
		segment.visibilityFlips.lower_bound(VoxRlVisibilityKey(static_cast<int>(eTeam), INT_MIN, 0));
	while (flip != segment.visibilityFlips.end() && (*flip).first.team == static_cast<int>(eTeam))
	{
		if ((*flip).first.kind == VOX_RL_BITSET_KNOWN_VISIBLE)
		{
			segment.visibilityFlips.erase(flip++);
		}
		else
		{
			++flip;
		}
	}
}

void VoxRlCapture::NoteTacticalZonesRebuilt()
{
	// The native dominance zone table was rebuilt; the next collection compares
	// it against the accepted snapshot and re-accepts what changed.
	m_zoneSnapshotDirty = true;
}

void VoxRlCapture::NoteTeamTechsChanged()
{
	// A technology ownership change can flip terrain and feature passability;
	// the next collection recollects the small team table.
	m_teamPassabilityDirty = true;
}

void VoxRlCapture::NoteRevealedOverrideChanged(TeamTypes eTeam, int iPlotIndex, bool bRemoved)
{
	if (m_segment == NULL || m_segment->failed) return;
	if (eTeam == NO_TEAM) return;
	const VoxRlEntityKey key(static_cast<int>(eTeam), iPlotIndex);
	if (bRemoved)
	{
		m_segment->revealedOverrideUpserts.erase(key);
		m_segment->removedRevealedOverrides.insert(key);
	}
	else
	{
		m_segment->removedRevealedOverrides.erase(key);
		m_segment->revealedOverrideUpserts[key] = 1;
	}
}

void VoxRlCapture::NoteInterceptorCacheChanged(PlayerTypes ePlayer)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyInterceptors.insert(static_cast<int>(ePlayer));
}

void VoxRlCapture::NoteTeamRelationChanged(TeamTypes eTeam, TeamTypes eOtherTeam)
{
	if (eTeam == NO_TEAM || eOtherTeam == NO_TEAM) return;
	// The first admitted WORLD already seeds relations. After recording starts,
	// keep skipped-player changes while any configured major can still receive them.
	if (m_operationState == NULL || !m_operationState->relationsSeeded || m_concluded) return;
	bool hasReceiver = false;
	for (int player = 0; player < MAX_MAJOR_CIVS; ++player)
	{
		if (!AdmitsMilitaryEvents(static_cast<PlayerTypes>(player))) continue;
		hasReceiver = true;
		break;
	}
	if (!hasReceiver) return;
	TeamRelationRecord row;
	std::memset(&row, 0, sizeof(row));
	row.team = static_cast<i8>(eTeam);
	row.otherTeam = static_cast<i8>(eOtherTeam);
	CvTeam& team = GET_TEAM(eTeam);
	row.atWar = team.isAtWar(eOtherTeam) ? 1 : 0;
	row.allowsOpenBorders = team.IsAllowsOpenBordersToTeam(eOtherTeam) ? 1 : 0;
	row.hasMet = team.isHasMet(eOtherTeam) ? 1 : 0;
	row.forcePeace = team.isForcePeace(eOtherTeam) ? 1 : 0;
	row.canDeclareWar = 0;
	m_operationState->bufferedRelations.push_back(row);
}

void VoxRlCapture::NoteWarStateChanged(TeamTypes eTeam, TeamTypes eOtherTeam)
{
	NoteTeamRelationChanged(eTeam, eOtherTeam);
	NoteTeamRelationChanged(eOtherTeam, eTeam);
	if (m_segment == NULL || m_segment->failed) return;
	// War state changes dirty every member's danger cache and mark the next
	// pre-refresh moment as a WORLD replacement checkpoint.
	const int teamA = static_cast<int>(eTeam);
	const int teamB = static_cast<int>(eOtherTeam);
	for (int player = 0; player < MAX_PLAYERS; ++player)
	{
		const PlayerTypes member = static_cast<PlayerTypes>(player);
		const int memberTeam = static_cast<int>(GET_PLAYER(member).getTeam());
		if (memberTeam == teamA || memberTeam == teamB)
		{
			if (IsObserving(member))
			{
				NoteDangerDirty(member);
			}
		}
	}
	m_worldReplacementPending = true;
}

void VoxRlCapture::NoteUnitAttackCountChanged(PlayerTypes eOwner, int iUnitId)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyUnitAttackCounts.insert(VoxRlEntityKey(static_cast<int>(eOwner), iUnitId));
}

void VoxRlCapture::NoteCityAttackCountChanged(PlayerTypes eOwner, int iCityId)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyCityAttackCounts.insert(VoxRlEntityKey(static_cast<int>(eOwner), iCityId));
}

void VoxRlCapture::NotePlayerResistanceChanged(PlayerTypes ePlayer, PlayerTypes eOpponent)
{
	if (m_segment == NULL || m_segment->failed) return;
	m_segment->dirtyPlayerResistances.insert(static_cast<int>(ePlayer));
}

void VoxRlCapture::NoteTopologyInvalidated()
{
	if (m_segment == NULL || m_segment->failed) return;
	m_topologyInvalidated = true;
}
