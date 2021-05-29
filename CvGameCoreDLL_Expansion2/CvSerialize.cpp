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