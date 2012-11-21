/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvTypes.h"

//Static variables for types
//MissionTypes
MissionTypes s_eMISSION_MOVE_TO = NO_MISSION;
MissionTypes s_eMISSION_ROUTE_TO = NO_MISSION;
MissionTypes s_eMISSION_MOVE_TO_UNIT = NO_MISSION;
MissionTypes s_eMISSION_SWAP_UNITS = NO_MISSION;
MissionTypes s_eMISSION_SKIP = NO_MISSION;
MissionTypes s_eMISSION_SLEEP = NO_MISSION;
MissionTypes s_eMISSION_ALERT = NO_MISSION;
MissionTypes s_eMISSION_FORTIFY = NO_MISSION;
MissionTypes s_eMISSION_GARRISON = NO_MISSION;
MissionTypes s_eMISSION_SET_UP_FOR_RANGED_ATTACK = NO_MISSION;
MissionTypes s_eMISSION_EMBARK = NO_MISSION;
MissionTypes s_eMISSION_DISEMBARK = NO_MISSION;
MissionTypes s_eMISSION_AIRPATROL = NO_MISSION;
MissionTypes s_eMISSION_HEAL = NO_MISSION;
MissionTypes s_eMISSION_AIRLIFT = NO_MISSION;
MissionTypes s_eMISSION_NUKE = NO_MISSION;
MissionTypes s_eMISSION_PARADROP = NO_MISSION;
MissionTypes s_eMISSION_AIR_SWEEP = NO_MISSION;
MissionTypes s_eMISSION_REBASE = NO_MISSION;
MissionTypes s_eMISSION_RANGE_ATTACK = NO_MISSION;
MissionTypes s_eMISSION_PILLAGE = NO_MISSION;
MissionTypes s_eMISSION_FOUND = NO_MISSION;
MissionTypes s_eMISSION_JOIN = NO_MISSION;
MissionTypes s_eMISSION_CONSTRUCT = NO_MISSION;
MissionTypes s_eMISSION_DISCOVER = NO_MISSION;
MissionTypes s_eMISSION_HURRY = NO_MISSION;
MissionTypes s_eMISSION_TRADE = NO_MISSION;
MissionTypes s_eMISSION_SPACESHIP = NO_MISSION;
MissionTypes s_eMISSION_CULTURE_BOMB = NO_MISSION;
MissionTypes s_eMISSION_GOLDEN_AGE = NO_MISSION;
MissionTypes s_eMISSION_BUILD = NO_MISSION;
MissionTypes s_eMISSION_LEAD = NO_MISSION;
MissionTypes s_eMISSION_DIE_ANIMATION = NO_MISSION;
MissionTypes s_eMISSION_BEGIN_COMBAT = NO_MISSION;
MissionTypes s_eMISSION_END_COMBAT = NO_MISSION;
MissionTypes s_eMISSION_AIRSTRIKE = NO_MISSION;
MissionTypes s_eMISSION_SURRENDER = NO_MISSION;
MissionTypes s_eMISSION_CAPTURED = NO_MISSION;
MissionTypes s_eMISSION_IDLE = NO_MISSION;
MissionTypes s_eMISSION_DIE = NO_MISSION;
MissionTypes s_eMISSION_DAMAGE = NO_MISSION;
MissionTypes s_eMISSION_MULTI_SELECT = NO_MISSION;
MissionTypes s_eMISSION_MULTI_DESELECT = NO_MISSION;
MissionTypes s_eMISSION_WAIT_FOR = NO_MISSION;
unsigned int s_uiNUM_MISSION_TYPES = 0;

void CvTypes::AcquireTypes(Database::Connection& db)
{
	//MissionTypes
	{
		typedef std::tr1::unordered_map<std::string, MissionTypes*> LookupTable;
		LookupTable kMissionTypesLookupTable;
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_MOVE_TO"), &s_eMISSION_MOVE_TO));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_ROUTE_TO"), &s_eMISSION_ROUTE_TO));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_MOVE_TO_UNIT"), &s_eMISSION_MOVE_TO_UNIT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SWAP_UNITS"), &s_eMISSION_SWAP_UNITS));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SKIP"), &s_eMISSION_SKIP));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SLEEP"), &s_eMISSION_SLEEP));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_ALERT"), &s_eMISSION_ALERT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_FORTIFY"), &s_eMISSION_FORTIFY));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_GARRISON"), &s_eMISSION_GARRISON));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SET_UP_FOR_RANGED_ATTACK"), &s_eMISSION_SET_UP_FOR_RANGED_ATTACK));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_EMBARK"), &s_eMISSION_EMBARK));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_DISEMBARK"), &s_eMISSION_DISEMBARK));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_AIRPATROL"), &s_eMISSION_AIRPATROL));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_HEAL"), &s_eMISSION_HEAL));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_AIRLIFT"), &s_eMISSION_AIRLIFT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_NUKE"), &s_eMISSION_NUKE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_PARADROP"), &s_eMISSION_PARADROP));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_AIR_SWEEP"), &s_eMISSION_AIR_SWEEP));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_REBASE"), &s_eMISSION_REBASE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_RANGE_ATTACK"), &s_eMISSION_RANGE_ATTACK));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_PILLAGE"), &s_eMISSION_PILLAGE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_FOUND"), &s_eMISSION_FOUND));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_JOIN"), &s_eMISSION_JOIN));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_CONSTRUCT"), &s_eMISSION_CONSTRUCT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_DISCOVER"), &s_eMISSION_DISCOVER));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_HURRY"), &s_eMISSION_HURRY));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_TRADE"), &s_eMISSION_TRADE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SPACESHIP"), &s_eMISSION_SPACESHIP));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_CULTURE_BOMB"), &s_eMISSION_CULTURE_BOMB));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_GOLDEN_AGE"), &s_eMISSION_GOLDEN_AGE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_BUILD"), &s_eMISSION_BUILD));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_LEAD"), &s_eMISSION_LEAD));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_DIE_ANIMATION"), &s_eMISSION_DIE_ANIMATION));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_BEGIN_COMBAT"), &s_eMISSION_BEGIN_COMBAT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_END_COMBAT"), &s_eMISSION_END_COMBAT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_AIRSTRIKE"), &s_eMISSION_AIRSTRIKE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_SURRENDER"), &s_eMISSION_SURRENDER));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_CAPTURED"), &s_eMISSION_CAPTURED));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_IDLE"), &s_eMISSION_IDLE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_DIE"), &s_eMISSION_DIE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_DAMAGE"), &s_eMISSION_DAMAGE));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_MULTI_SELECT"), &s_eMISSION_MULTI_SELECT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_MULTI_DESELECT"), &s_eMISSION_MULTI_DESELECT));
		kMissionTypesLookupTable.insert(make_pair(std::string("MISSION_WAIT_FOR"), &s_eMISSION_WAIT_FOR));

		Database::Results kResults;
		if(db.Execute(kResults, "SELECT Type, ID from Missions"))
		{
			while(kResults.Step())
			{
				std::string strMissionType = kResults.GetText(0);
				LookupTable::iterator it = kMissionTypesLookupTable.find(strMissionType);
				if(it != kMissionTypesLookupTable.end())
				{
					(*it->second) = static_cast<MissionTypes>(kResults.GetInt(1));
				}
			}
		}

		int iNumTypes = db.Count("Missions", false);
		if(iNumTypes >= 0)
		{
			s_uiNUM_MISSION_TYPES = static_cast<unsigned int>(iNumTypes);
		}

		for(LookupTable::iterator it = kMissionTypesLookupTable.begin(); it != kMissionTypesLookupTable.end(); ++it)
		{
			if( (*it->second) == NO_MISSION)
			{
				char msg[256] = {0};
				sprintf_s(msg, "MissionType - %s is used in the DLL but does not exist in the database.", it->first.c_str());
				FILogFile* pLog = LOGFILEMGR.GetLog("Gamecore.log", FILogFile::kDontTimeStamp);
				pLog->WarningMsg(msg);
				CvAssertMsg(false, msg);
			}
		}
	}
}

//MissionTypes
const MissionTypes CvTypes::getMISSION_MOVE_TO(){ return s_eMISSION_MOVE_TO; }
const MissionTypes CvTypes::getMISSION_ROUTE_TO(){ return s_eMISSION_ROUTE_TO; }
const MissionTypes CvTypes::getMISSION_MOVE_TO_UNIT(){ return s_eMISSION_MOVE_TO_UNIT; }
const MissionTypes CvTypes::getMISSION_SWAP_UNITS(){ return s_eMISSION_SWAP_UNITS; }
const MissionTypes CvTypes::getMISSION_SKIP(){ return s_eMISSION_SKIP; }
const MissionTypes CvTypes::getMISSION_SLEEP(){ return s_eMISSION_SLEEP; }
const MissionTypes CvTypes::getMISSION_ALERT(){ return s_eMISSION_ALERT; }
const MissionTypes CvTypes::getMISSION_FORTIFY(){ return s_eMISSION_FORTIFY; }
const MissionTypes CvTypes::getMISSION_GARRISON(){ return s_eMISSION_GARRISON; }
const MissionTypes CvTypes::getMISSION_SET_UP_FOR_RANGED_ATTACK(){ return s_eMISSION_SET_UP_FOR_RANGED_ATTACK; }
const MissionTypes CvTypes::getMISSION_EMBARK(){ return s_eMISSION_EMBARK; }
const MissionTypes CvTypes::getMISSION_DISEMBARK(){ return s_eMISSION_DISEMBARK; }
const MissionTypes CvTypes::getMISSION_AIRPATROL(){ return s_eMISSION_AIRPATROL; }
const MissionTypes CvTypes::getMISSION_HEAL(){ return s_eMISSION_HEAL; }
const MissionTypes CvTypes::getMISSION_AIRLIFT(){ return s_eMISSION_AIRLIFT; }
const MissionTypes CvTypes::getMISSION_NUKE(){ return s_eMISSION_NUKE; }
const MissionTypes CvTypes::getMISSION_PARADROP(){ return s_eMISSION_PARADROP; }
const MissionTypes CvTypes::getMISSION_AIR_SWEEP(){ return s_eMISSION_AIR_SWEEP; }
const MissionTypes CvTypes::getMISSION_REBASE(){ return s_eMISSION_REBASE; }
const MissionTypes CvTypes::getMISSION_RANGE_ATTACK(){ return s_eMISSION_RANGE_ATTACK; }
const MissionTypes CvTypes::getMISSION_PILLAGE(){ return s_eMISSION_PILLAGE; }
const MissionTypes CvTypes::getMISSION_FOUND(){ return s_eMISSION_FOUND; }
const MissionTypes CvTypes::getMISSION_JOIN(){ return s_eMISSION_JOIN; }
const MissionTypes CvTypes::getMISSION_CONSTRUCT(){ return s_eMISSION_CONSTRUCT; }
const MissionTypes CvTypes::getMISSION_DISCOVER(){ return s_eMISSION_DISCOVER; }
const MissionTypes CvTypes::getMISSION_HURRY(){ return s_eMISSION_HURRY; }
const MissionTypes CvTypes::getMISSION_TRADE(){ return s_eMISSION_TRADE; }
const MissionTypes CvTypes::getMISSION_SPACESHIP(){ return s_eMISSION_SPACESHIP; }
const MissionTypes CvTypes::getMISSION_CULTURE_BOMB(){ return s_eMISSION_CULTURE_BOMB; }
const MissionTypes CvTypes::getMISSION_GOLDEN_AGE(){ return s_eMISSION_GOLDEN_AGE; }
const MissionTypes CvTypes::getMISSION_BUILD(){ return s_eMISSION_BUILD; }
const MissionTypes CvTypes::getMISSION_LEAD(){ return s_eMISSION_LEAD; }
const MissionTypes CvTypes::getMISSION_DIE_ANIMATION(){ return s_eMISSION_DIE_ANIMATION; }
const MissionTypes CvTypes::getMISSION_BEGIN_COMBAT(){ return s_eMISSION_BEGIN_COMBAT; }
const MissionTypes CvTypes::getMISSION_END_COMBAT(){ return s_eMISSION_END_COMBAT; }
const MissionTypes CvTypes::getMISSION_AIRSTRIKE(){ return s_eMISSION_AIRSTRIKE; }
const MissionTypes CvTypes::getMISSION_SURRENDER(){ return s_eMISSION_SURRENDER; }
const MissionTypes CvTypes::getMISSION_CAPTURED(){ return s_eMISSION_CAPTURED; }
const MissionTypes CvTypes::getMISSION_IDLE(){ return s_eMISSION_IDLE; }
const MissionTypes CvTypes::getMISSION_DIE(){ return s_eMISSION_DIE; }
const MissionTypes CvTypes::getMISSION_DAMAGE(){ return s_eMISSION_DAMAGE; }
const MissionTypes CvTypes::getMISSION_MULTI_SELECT(){ return s_eMISSION_MULTI_SELECT; }
const MissionTypes CvTypes::getMISSION_MULTI_DESELECT(){ return s_eMISSION_MULTI_DESELECT; }
const MissionTypes CvTypes::getMISSION_WAIT_FOR(){ return s_eMISSION_WAIT_FOR; }
const unsigned int CvTypes::getNUM_MISSION_TYPES(){return s_uiNUM_MISSION_TYPES;}