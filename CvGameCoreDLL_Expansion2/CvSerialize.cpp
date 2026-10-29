#include "CvGameCoreDLLPCH.h"
#include "CvSerialize.h"

const std::string EmptyString = "";

void CvSyncArchiveCollectDeltas(FAutoArchive& syncArchive, const std::vector<FAutoVariableBase*>& vars)
{
	for (std::vector<FAutoVariableBase*>::const_iterator it = vars.begin(); it != vars.end(); ++it)
	{
		ICvSyncVar& syncVar = static_cast<ICvSyncVar&>(*(*it));
		if (syncVar.hasDelta())
		{
			syncArchive.touch(syncVar);
			syncVar.clearDelta();
		}
	}
}

void CvSyncVarLogDesync(const ICvSyncVar& var, const std::string& remoteValue, const std::string& containerContext)
{
	// Deduplication and cascade detection
	// These are shared by all sync vars, so FIRST and CASCADE cover every variable in the turn.
	static std::map<std::string, int> s_desyncCountThisTurn;
	static std::map<int, std::pair<std::string, int> > s_firstDesyncPerTurn;
	static int s_lastLoggedTurn = -1;

	int currentTurn = GC.getGame().getGameTurn();

	// Clear tracking on new turn
	if (currentTurn != s_lastLoggedTurn)
	{
		s_desyncCountThisTurn.clear();
		s_lastLoggedTurn = currentTurn;
	}

	std::string logKey = var.name();
	int desyncCount = s_desyncCountThisTurn[logKey]++;

	// Only log first occurrence and a summary after that
	if (desyncCount == 0)
	{
		// Desync logging with available context
		std::ostringstream logMsg;
		logMsg << "[DESYNC_DETECTED";

		// Cascade detection - mark first desync
		if (s_firstDesyncPerTurn.find(currentTurn) == s_firstDesyncPerTurn.end())
		{
			logMsg << " - FIRST";
			s_firstDesyncPerTurn[currentTurn] = std::make_pair(var.name(), currentTurn);
		}
		else
		{
			logMsg << " - CASCADE";
		}

		logMsg << "] Variable=" << var.name()
		       << " | Turn=" << currentTurn
		       << " | Player=" << GC.getGame().getActivePlayer()
		       << " | Host=" << (gDLL->IsHost() ? "YES" : "NO")
		       << " | Local=" << var.toString()
		       << " | Remote=" << remoteValue;

		// Add container-specific context
		if (!containerContext.empty()) {
			logMsg << " | Context=" << containerContext;
		}

		// Show what first desync was if this is cascading
		if (s_firstDesyncPerTurn.find(currentTurn) != s_firstDesyncPerTurn.end() &&
		    s_firstDesyncPerTurn[currentTurn].first != var.name())
		{
			logMsg << " | First desync: " << s_firstDesyncPerTurn[currentTurn].first;
		}

		gGlobals.getDLLIFace()->netMessageDebugLog(logMsg.str());
		gGlobals.getGame().setDesynced(true);
	}
	else if (desyncCount == 1)
	{
		// Log suppression notice once
		std::ostringstream logMsg;
		logMsg << "[DESYNC_REPEATED] Variable=" << var.name()
		       << " | Turn=" << currentTurn
		       << " (suppressing further duplicates this turn)";
		gGlobals.getDLLIFace()->netMessageDebugLog(logMsg.str());
	}
	// After 2nd occurrence, suppress completely
}

std::string CvSyncVarDebugDump(const ICvSyncVar& var, const std::vector<std::pair<std::string, std::string> >& callStacks, const std::string& containerContext, const std::string* remoteValue)
{
	std::string result = var.FAutoVariableBase::debugDump(callStacks);
	result += std::string("\n") + containerContext + std::string("\n");
	if (remoteValue)
	{
		result += std::string("local value=") + var.toString() + "\n";
		result += std::string("remote value=") + *remoteValue + "\n";
	}
	return result;
}
