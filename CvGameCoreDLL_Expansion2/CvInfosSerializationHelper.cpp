/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  AUTHOR:	Ken Grey
//
//  PURPOSE: Helper functions to serialize and unserialize CvInfo types and data associated with the types
//			 so that the type IDs are saved as unique values so the CvInfo*s tables can be changed by modding and DLC
//

#include "CvGameCoreDLLPCH.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays of variable length
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace CvInfosSerializationHelper
{
/// Read a fixed array of resource entry values and place them in the resource slots from the
/// initial release.
const char* ms_V0ResourceTags[27] =
{
	"RESOURCE_IRON",
	"RESOURCE_HORSE",
	"RESOURCE_COAL",
	"RESOURCE_OIL",
	"RESOURCE_ALUMINUM",
	"RESOURCE_URANIUM",
	"RESOURCE_WHEAT",
	"RESOURCE_COW",
	"RESOURCE_SHEEP",
	"RESOURCE_DEER",
	"RESOURCE_BANANA",
	"RESOURCE_FISH",
	"RESOURCE_WHALE",
	"RESOURCE_PEARLS",
	"RESOURCE_GOLD",
	"RESOURCE_SILVER",
	"RESOURCE_GEMS",
	"RESOURCE_MARBLE",
	"RESOURCE_IVORY",
	"RESOURCE_FUR",
	"RESOURCE_DYE",
	"RESOURCE_SPICES",
	"RESOURCE_SILK",
	"RESOURCE_SUGAR",
	"RESOURCE_COTTON",
	"RESOURCE_WINE",
	"RESOURCE_INCENSE"
};

const char* ms_V0PolicyTags[60] =
{
	"POLICY_LIBERTY",
	"POLICY_COLLECTIVE_RULE",
	"POLICY_CITIZENSHIP",
	"POLICY_REPUBLIC",
	"POLICY_REPRESENTATION",
	"POLICY_MERITOCRACY",
	"POLICY_TRADITION",
	"POLICY_ARISTOCRACY",
	"POLICY_OLIGARCHY",
	"POLICY_LEGALISM",
	"POLICY_LANDED_ELITE",
	"POLICY_MONARCHY",
	"POLICY_HONOR",
	"POLICY_WARRIOR_CODE",
	"POLICY_DISCIPLINE",
	"POLICY_MILITARY_TRADITION",
	"POLICY_MILITARY_CASTE",
	"POLICY_PROFESSIONAL_ARMY",
	"POLICY_PIETY",
	"POLICY_ORGANIZED_RELIGION",
	"POLICY_MANDATE_OF_HEAVEN",
	"POLICY_THEOCRACY",
	"POLICY_REFORMATION",
	"POLICY_FREE_RELIGION",
	"POLICY_PATRONAGE",
	"POLICY_PHILANTHROPY",
	"POLICY_AESTHETICS",
	"POLICY_SCHOLASTICISM",
	"POLICY_CULTURAL_DIPLOMACY",
	"POLICY_EDUCATED_ELITE",
	"POLICY_COMMERCE",
	"POLICY_TRADE_UNIONS",
	"POLICY_NAVAL_TRADITION",
	"POLICY_MERCANTILISM",
	"POLICY_MERCHANT_NAVY",
	"POLICY_PROTECTIONISM",
	"POLICY_RATIONALISM",
	"POLICY_SECULARISM",
	"POLICY_HUMANISM",
	"POLICY_FREE_THOUGHT",
	"POLICY_SOVEREIGNTY",
	"POLICY_SCIENTIFIC_REVOLUTION",
	"POLICY_FREEDOM",
	"POLICY_CONSTITUTION",
	"POLICY_UNIVERSAL_SUFFRAGE",
	"POLICY_CIVIL_SOCIETY",
	"POLICY_FREE_SPEECH",
	"POLICY_DEMOCRACY",
	"POLICY_ORDER",
	"POLICY_UNITED_FRONT",
	"POLICY_SOCIALISM",
	"POLICY_NATIONALISM",
	"POLICY_PLANNED_ECONOMY",
	"POLICY_COMMUNISM",
	"POLICY_AUTOCRACY",
	"POLICY_POPULISM",
	"POLICY_MILITARISM",
	"POLICY_FASCISM",
	"POLICY_POLICE_STATE",
	"POLICY_TOTAL_WAR"
};

const char* ms_V0PolicyBranchTags[10] =
{
	"POLICY_BRANCH_TRADITION",
	"POLICY_BRANCH_LIBERTY",
	"POLICY_BRANCH_HONOR",
	"POLICY_BRANCH_PIETY",
	"POLICY_BRANCH_PATRONAGE",
	"POLICY_BRANCH_COMMERCE",
	"POLICY_BRANCH_RATIONALISM",
	"POLICY_BRANCH_FREEDOM",
	"POLICY_BRANCH_ORDER",
	"POLICY_BRANCH_AUTOCRACY"
};

/// Helper function to read a single type ID as a string and convert it to an ID
int Read(FDataStream& kStream, bool* bValid /*= NULL*/)
{
	FStringFixedBuffer(sTemp, 256);
	kStream >> sTemp;
	if(bValid) *bValid = true;
	if(sTemp.GetLength() > 0 && sTemp != "NO_TYPE")
	{
		int iType = GC.getInfoTypeForString(sTemp);
		if(iType != -1)
		{
			return iType;
		}
		else
		{
			if(bValid) *bValid = false;
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", sTemp);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}

	return -1;
}

/// Helper function to read a single resource type ID as a hash and convert it to an ID
int ReadHashed(FDataStream& kStream, bool* bValid /*= NULL*/)
{
	uint uiHash;
	if(bValid) *bValid = true;
	kStream >> uiHash;
	if(uiHash != 0)
	{
		int iType = GC.getInfoTypeForHash(uiHash);
		if(iType != -1)
		{
			return iType;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found for hash: %u", uiHash);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
			if(bValid) *bValid = false;
		}
	}

	return -1;
}

/// Helper function to read a single type (as a string) and find its ID in a database table.
/// Assumes the type is in the table's field "Type"
int ReadDBLookup(FDataStream& kStream, const char* szTable, bool* bValid /*= NULL*/)
{
	FStringFixedBuffer(sTemp, 256);
	kStream >> sTemp;
	if (bValid) *bValid = true;
	if(sTemp.GetLength() > 0 && sTemp != "NO_TYPE")
	{
		Database::Connection* pDB = GC.GetGameDatabase();
		if(pDB)
		{
			Database::Results kResults;
			CvString szCommand;
			szCommand.Format("SELECT ID from %s where Type = ? LIMIT 1", szTable);
			if(pDB->Execute(kResults, szCommand.c_str()))
			{
				kResults.Bind(1, sTemp.c_str());
				if(kResults.Step())
				{
					return kResults.GetInt(0);
				}
				else
					if (bValid) *bValid = false;
			}
		}
		else
			if (bValid) *bValid = false;
	}
	
	return -1;
}


// Start Type specific functions.

// These functions convert a runtime type ID (index) to a unique type
// string or hash

IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(ResourceTypes, getResourceInfo, NO_RESOURCE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(PolicyTypes, getPolicyInfo, NO_POLICY);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(PolicyBranchTypes, getPolicyBranchInfo, NO_POLICY_BRANCH_TYPE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(TechTypes, getTechInfo, NO_TECH);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(BeliefTypes, getBeliefInfo, NO_BELIEF);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(ReligionTypes, getReligionInfo, NO_RELIGION);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(PlayerColorTypes, GetPlayerColorInfo, NO_PLAYERCOLOR);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(MinorCivTypes, getMinorCivInfo, NO_MINORCIV);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(SpecialistTypes, getSpecialistInfo, NO_SPECIALIST);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(EmphasizeTypes, getEmphasisInfo, NO_EMPHASIZE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(ProjectTypes, getProjectInfo, NO_PROJECT);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(VoteTypes, getVoteInfo, NO_VOTE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(VoteSourceTypes, getVoteSourceInfo, NO_VOTESOURCE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(SpecialUnitTypes, getSpecialUnitInfo, NO_SPECIALUNIT);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(VictoryTypes, getVictoryInfo, NO_VICTORY);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(BuildingTypes, getBuildingInfo, NO_BUILDING);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(PromotionTypes, getPromotionInfo, NO_PROMOTION);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(UnitTypes, getUnitInfo, NO_UNIT);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(FeatureTypes, getFeatureInfo, NO_FEATURE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(TerrainTypes, getTerrainInfo, NO_TERRAIN);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(AICityStrategyTypes, getAICityStrategyInfo, NO_AICITYSTRATEGY);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(SmallAwardTypes, getSmallAwardInfo, NO_SMALLAWARD);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(RouteTypes, getRouteInfo, NO_ROUTE);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(BuildTypes, getBuildInfo, NO_BUILD);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(ProcessTypes, getProcessInfo, NO_PROCESS);
IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(ImprovementTypes, getImprovementInfo, NO_IMPROVEMENT);
// Can't use this because nothing ever respects the values.  They are all hard-coded in the enum
//IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(UnitAITypes, getUnitAIInfo, NO_UNITAI);

/// Helper fun

/// End Type specific functions

/// Helper function to write out an info type ID as string
bool Write(FDataStream& kStream, const CvBaseInfo* pkInfo)
{
	if(pkInfo)
	{
		FStringFixedBuffer(sTemp, 256);
		sTemp = pkInfo->GetType();
		kStream << sTemp;
		return true;
	}
	else
	{
		// To save space, just write an empty string to denote a missing type
		int iLen = 0;
		kStream << iLen;
	}

	return false;
}

/// Helper function to write out an info type ID as a hash
bool WriteHashed(FDataStream& kStream, const CvBaseInfo* pkInfo)
{
	if(pkInfo && pkInfo->GetType() && pkInfo->GetType()[0] != 0)
	{
		uint uiHash = FString::Hash(pkInfo->GetType());
		kStream << uiHash;
		return true;
	}

	uint uiHash = 0;		// 0 hash == missing type
	kStream << uiHash;
	return false;
}

}
