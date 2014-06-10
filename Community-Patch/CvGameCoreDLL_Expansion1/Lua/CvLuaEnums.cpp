/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaEnums.cpp
//!  \brief     Private implementation to CvLuaEnums.
//!
//!		This file includes the implementation for exposing enums to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaEnums.h"
#include <CvEnums.h>
#include "../CvDiplomacyAI.h"
#include "../CvMinorCivAI.h"

#define RegisterEnum(e) pRegisterEnum(L, #e, e)
#define RegisterEnumUInt(e) pRegisterEnumUInt(L, #e, e)

void CvLuaEnums::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}
//------------------------------------------------------------------------------
int CvLuaEnums::pRegister(lua_State* L)
{
	// Things from CvDefines.h
	// Interface Dirty Bits
	EnumStart(L, "InterfaceDirtyBits");
	RegisterEnum(SelectionCamera_DIRTY_BIT);
	RegisterEnum(Fog_DIRTY_BIT);
	RegisterEnum(Waypoints_DIRTY_BIT);
	RegisterEnum(PercentButtons_DIRTY_BIT);
	RegisterEnum(MiscButtons_DIRTY_BIT);
	RegisterEnum(PlotListButtons_DIRTY_BIT);
	RegisterEnum(SelectionButtons_DIRTY_BIT);
	RegisterEnum(CitizenButtons_DIRTY_BIT);
	RegisterEnum(ResearchButtons_DIRTY_BIT);
	RegisterEnum(Event_DIRTY_BIT);
	RegisterEnum(Center_DIRTY_BIT);
	RegisterEnum(GameData_DIRTY_BIT);
	RegisterEnum(Score_DIRTY_BIT);
	RegisterEnum(TurnTimer_DIRTY_BIT);
	RegisterEnum(Help_DIRTY_BIT);
	RegisterEnum(MinimapSection_DIRTY_BIT);
	RegisterEnum(SelectionSound_DIRTY_BIT);
	RegisterEnum(Cursor_DIRTY_BIT);
	RegisterEnum(CityInfo_DIRTY_BIT);
	RegisterEnum(UnitInfo_DIRTY_BIT);
	RegisterEnum(Popup_DIRTY_BIT);
	RegisterEnum(CityScreen_DIRTY_BIT);
	RegisterEnum(InfoPane_DIRTY_BIT);
	RegisterEnum(HighlightPlot_DIRTY_BIT);
	RegisterEnum(ColoredPlots_DIRTY_BIT);
	RegisterEnum(BlockadedPlots_DIRTY_BIT);
	RegisterEnum(Financial_Screen_DIRTY_BIT);
	RegisterEnum(Foreign_Screen_DIRTY_BIT);
	RegisterEnum(Soundtrack_DIRTY_BIT);
	RegisterEnum(Domestic_Advisor_DIRTY_BIT);
	RegisterEnum(Advanced_Start_DIRTY_BIT);
	RegisterEnum(NationalBorders_DIRTY_BIT);
	RegisterEnum(Policies_DIRTY_BIT);
	RegisterEnum(PlotData_DIRTY_BIT);
	RegisterEnum(NUM_INTERFACE_DIRTY_BITS);
	EnumEnd(L);

	// City Update Types
	EnumStart(L, "CityUpdateTypes");
	RegisterEnum(NO_CITY_UPDATE_TYPE);
	RegisterEnum(CITY_UPDATE_TYPE_BANNER);
	RegisterEnum(CITY_UPDATE_TYPE_SPECIALISTS);
	RegisterEnum(CITY_UPDATE_TYPE_PRODUCTION);
	RegisterEnum(CITY_UPDATE_TYPE_ENEMY_IN_RANGE);
	RegisterEnum(CITY_UPDATE_TYPE_GARRISON);
	RegisterEnum(NUM_CITY_UPDATE_TYPES);
	EnumEnd(L);

	//Button Popup Types
	EnumStart(L, "ButtonPopupTypes");
	RegisterEnum(BUTTONPOPUP_TEXT);
	RegisterEnum(BUTTONPOPUP_MAIN_MENU);
	RegisterEnum(BUTTONPOPUP_CONFIRM_MENU);
	RegisterEnum(BUTTONPOPUP_DECLAREWARMOVE);
	RegisterEnum(BUTTONPOPUP_DECLAREWARRANGESTRIKE);
	RegisterEnum(BUTTONPOPUP_CONFIRMCOMMAND);
	RegisterEnum(BUTTONPOPUP_CONFIRM_CITY_TASK);
	RegisterEnum(BUTTONPOPUP_CONFIRM_IMPROVEMENT_REBUILD);
	RegisterEnum(BUTTONPOPUP_GREAT_PERSON);
	RegisterEnum(BUTTONPOPUP_LOADUNIT);
	RegisterEnum(BUTTONPOPUP_LEADUNIT);
	RegisterEnum(BUTTONPOPUP_CHOOSETECH);
	RegisterEnum(BUTTONPOPUP_CITY_CAPTURED);
	RegisterEnum(BUTTONPOPUP_ANNEX_CITY);
	RegisterEnum(BUTTONPOPUP_LIBERATE_MINOR);
	RegisterEnum(BUTTONPOPUP_DISBANDCITY);
	RegisterEnum(BUTTONPOPUP_CHOOSEPRODUCTION);
	RegisterEnum(BUTTONPOPUP_CHOOSEPOLICY);
	RegisterEnum(BUTTONPOPUP_CHOOSECITYPURCHASE);
	RegisterEnum(BUTTONPOPUP_CHOOSEELECTION);
	RegisterEnum(BUTTONPOPUP_DIPLOVOTE);
	RegisterEnum(BUTTONPOPUP_ALARM);
	RegisterEnum(BUTTONPOPUP_DEAL_CANCELED);
	RegisterEnum(BUTTONPOPUP_PYTHON);
	RegisterEnum(BUTTONPOPUP_PYTHON_SCREEN);
	RegisterEnum(BUTTONPOPUP_DETAILS);
	RegisterEnum(BUTTONPOPUP_ADMIN);
	RegisterEnum(BUTTONPOPUP_ADMIN_PASSWORD);
	RegisterEnum(BUTTONPOPUP_EXTENDED_GAME);
	RegisterEnum(BUTTONPOPUP_DIPLOMACY);
	RegisterEnum(BUTTONPOPUP_ADDBUDDY);
	RegisterEnum(BUTTONPOPUP_FORCED_DISCONNECT);
	RegisterEnum(BUTTONPOPUP_PITBOSS_DISCONNECT);
	RegisterEnum(BUTTONPOPUP_KICKED);
	RegisterEnum(BUTTONPOPUP_FREE_COLONY);
	RegisterEnum(BUTTONPOPUP_LAUNCH);
	RegisterEnum(BUTTONPOPUP_MINOR_GOLD);
	RegisterEnum(BUTTONPOPUP_MINOR_CIV_QUEST);
	RegisterEnum(BUTTONPOPUP_MINOR_CIV_QUEST_COMPLETED);
	RegisterEnum(BUTTONPOPUP_MINOR_CIV_INTRUSION);
	RegisterEnum(BUTTONPOPUP_MINOR_CIV_GREETING_AND_WAR);
	RegisterEnum(BUTTONPOPUP_BARBARIAN_RANSOM);
	RegisterEnum(BUTTONPOPUP_MINOR_CIV_ENTER_TERRITORY);
	RegisterEnum(BUTTONPOPUP_TEMP_SPECIALISTS);
	RegisterEnum(BUTTONPOPUP_NOTIFICATION_LOG);
	RegisterEnum(BUTTONPOPUP_ECONOMIC_OVERVIEW);
	RegisterEnum(BUTTONPOPUP_MILITARY_OVERVIEW);
	RegisterEnum(BUTTONPOPUP_DIPLOMATIC_OVERVIEW);
	RegisterEnum(BUTTONPOPUP_DEMOGRAPHICS);
	RegisterEnum(BUTTONPOPUP_VICTORY_INFO);
	RegisterEnum(BUTTONPOPUP_MINOR_CIVS_LIST);
	RegisterEnum(BUTTONPOPUP_GIFT_CONFIRM);
	RegisterEnum(BUTTONPOPUP_RETURN_CIVILIAN);
	RegisterEnum(BUTTONPOPUP_CONFIRM_POLICY_BRANCH_SWITCH);
	RegisterEnum(BUTTONPOPUP_WHOS_WINNING);
	RegisterEnum(BUTTONPOPUP_NEW_ERA);
	RegisterEnum(BUTTONPOPUP_NATURAL_WONDER_REWARD);
	RegisterEnum(BUTTONPOPUP_GOODY_HUT_REWARD);
	RegisterEnum(BUTTONPOPUP_BARBARIAN_CAMP_REWARD);
	RegisterEnum(BUTTONPOPUP_GOLDEN_AGE_REWARD);
	RegisterEnum(BUTTONPOPUP_GREAT_PERSON_REWARD);
	RegisterEnum(BUTTONPOPUP_CITY_STATE_GREETING);
	RegisterEnum(BUTTONPOPUP_CITY_STATE_MESSAGE);
	RegisterEnum(BUTTONPOPUP_CITY_STATE_DIPLO);
	RegisterEnum(BUTTONPOPUP_CITY_PLOT_MANAGEMENT);
	RegisterEnum(BUTTONPOPUP_DIPLO_VOTE);
	RegisterEnum(BUTTONPOPUP_VOTE_RESULTS);
	RegisterEnum(BUTTONPOPUP_TECH_TREE);
	RegisterEnum(BUTTONPOPUP_TECH_AWARD);
	RegisterEnum(BUTTONPOPUP_WONDER_COMPLETED_ACTIVE_PLAYER);
	RegisterEnum(BUTTONPOPUP_ADVISOR_INFO);
	RegisterEnum(BUTTONPOPUP_ADVISOR_COUNSEL);
	RegisterEnum(BUTTONPOPUP_ADVISOR_MODAL);
	RegisterEnum(BUTTONPOPUP_RENAME_CITY);
	RegisterEnum(BUTTONPOPUP_RENAME_UNIT);
	RegisterEnum(BUTTONPOPUP_CHOOSE_FREE_GREAT_PERSON);
	RegisterEnum(BUTTONPOPUP_FOUND_PANTHEON);
	RegisterEnum(BUTTONPOPUP_FOUND_RELIGION);
	RegisterEnum(BUTTONPOPUP_ESPIONAGE_OVERVIEW);
	RegisterEnum(BUTTONPOPUP_RELIGION_OVERVIEW);
	RegisterEnum(BUTTONPOPUP_CHOOSE_TECH_TO_STEAL);
	RegisterEnum(BUTTONPOPUP_CHOOSE_MAYA_BONUS);
	RegisterEnum(BUTTONPOPUP_CHOOSE_FAITH_GREAT_PERSON);

	RegisterEnum(BUTTONPOPUP_MODDER_0);
	RegisterEnum(BUTTONPOPUP_MODDER_1);
	RegisterEnum(BUTTONPOPUP_MODDER_2);
	RegisterEnum(BUTTONPOPUP_MODDER_3);
	RegisterEnum(BUTTONPOPUP_MODDER_4);
	RegisterEnum(BUTTONPOPUP_MODDER_5);
	RegisterEnum(BUTTONPOPUP_MODDER_6);
	RegisterEnum(BUTTONPOPUP_MODDER_7);
	RegisterEnum(BUTTONPOPUP_MODDER_8);
	RegisterEnum(BUTTONPOPUP_MODDER_9);
	RegisterEnum(BUTTONPOPUP_MODDER_10);
	RegisterEnum(BUTTONPOPUP_MODDER_11);

	EnumEnd(L);

	//YieldTypes
	EnumStart(L, "YieldTypes");
	RegisterEnum(NO_YIELD);
	RegisterEnum(YIELD_FOOD);
	RegisterEnum(YIELD_PRODUCTION);
	RegisterEnum(YIELD_GOLD);
	RegisterEnum(YIELD_SCIENCE);
	RegisterEnum(YIELD_CULTURE);
	RegisterEnum(YIELD_FAITH);
	RegisterEnum(NUM_YIELD_TYPES);
	EnumEnd(L);

	//GameOptionTypes
	EnumStart(L, "GameOptionTypes");
	RegisterEnum(NO_GAMEOPTION);
	RegisterEnum(GAMEOPTION_NO_CITY_RAZING);
	RegisterEnum(GAMEOPTION_NO_BARBARIANS);
	RegisterEnum(GAMEOPTION_RAGING_BARBARIANS);
	RegisterEnum(GAMEOPTION_ALWAYS_WAR);
	RegisterEnum(GAMEOPTION_ALWAYS_PEACE);
	RegisterEnum(GAMEOPTION_ONE_CITY_CHALLENGE);
	RegisterEnum(GAMEOPTION_NO_CHANGING_WAR_PEACE);
	RegisterEnum(GAMEOPTION_NEW_RANDOM_SEED);
	RegisterEnum(GAMEOPTION_LOCK_MODS);
	RegisterEnum(GAMEOPTION_COMPLETE_KILLS);
	RegisterEnum(GAMEOPTION_NO_GOODY_HUTS);
	RegisterEnum(GAMEOPTION_RANDOM_PERSONALITIES);
	RegisterEnum(GAMEOPTION_POLICY_SAVING);
	RegisterEnum(GAMEOPTION_PROMOTION_SAVING);
	RegisterEnum(GAMEOPTION_END_TURN_TIMER_ENABLED);
	RegisterEnum(GAMEOPTION_QUICK_COMBAT);
	RegisterEnum(GAMEOPTION_DISABLE_START_BIAS);
	RegisterEnum(GAMEOPTION_NO_SCIENCE);
	RegisterEnum(GAMEOPTION_NO_POLICIES);
	RegisterEnum(GAMEOPTION_NO_HAPPINESS);
	RegisterEnum(GAMEOPTION_NO_TUTORIAL);
	RegisterEnum(GAMEOPTION_NO_RELIGION);
	RegisterEnum(NUM_GAMEOPTION_TYPES);


	//DomainTypes
	EnumStart(L, "DomainTypes");
	RegisterEnum(NO_DOMAIN);
	RegisterEnum(DOMAIN_SEA);
	RegisterEnum(DOMAIN_AIR);
	RegisterEnum(DOMAIN_LAND);
	RegisterEnum(DOMAIN_IMMOBILE);
	RegisterEnum(DOMAIN_HOVER);
	RegisterEnum(NUM_DOMAIN_TYPES);
	EnumEnd(L);

	//FeatureTypes
	EnumStart(L, "FeatureTypes");
	RegisterEnum(NO_FEATURE);
	RegisterEnum(FEATURE_ICE);
	RegisterEnum(FEATURE_JUNGLE);
	RegisterEnum(FEATURE_MARSH);
	RegisterEnum(FEATURE_OASIS);
	RegisterEnum(FEATURE_FLOOD_PLAINS);
	RegisterEnum(FEATURE_FOREST);
	RegisterEnum(FEATURE_FALLOUT);
	RegisterEnum(NUM_FEATURE_TYPES);
	EnumEnd(L);

	//FogOfWarModeTypes
	EnumStart(L, "FogOfWarModeTypes");
	RegisterEnum(FOGOFWARMODE_OFF);
	RegisterEnum(FOGOFWARMODE_NOVIS);
	RegisterEnum(FOGOFWARMODE_UNEXPLORED);
	EnumEnd(L);

	EnumStart(L, "OrderTypes");
	RegisterEnum(NO_ORDER);
	RegisterEnum(ORDER_TRAIN);
	RegisterEnum(ORDER_CONSTRUCT);
	RegisterEnum(ORDER_CREATE);
	RegisterEnum(ORDER_PREPARE);
	RegisterEnum(ORDER_MAINTAIN);
	RegisterEnum(NUM_ORDER_TYPES);
	EnumEnd(L);

	//PlotTypes
	EnumStart(L, "PlotTypes");
	RegisterEnum(NO_PLOT);
	RegisterEnum(PLOT_MOUNTAIN);
	RegisterEnum(PLOT_HILLS);
	RegisterEnum(PLOT_LAND);
	RegisterEnum(PLOT_OCEAN);
	RegisterEnum(NUM_PLOT_TYPES);
	EnumEnd(L);

	//TerrainTypes
	EnumStart(L, "TerrainTypes");
	RegisterEnum(NO_TERRAIN);
	RegisterEnum(TERRAIN_GRASS);
	RegisterEnum(TERRAIN_PLAINS);
	RegisterEnum(TERRAIN_DESERT);
	RegisterEnum(TERRAIN_TUNDRA);
	RegisterEnum(TERRAIN_SNOW);
	RegisterEnum(TERRAIN_COAST);
	RegisterEnum(TERRAIN_OCEAN);
	RegisterEnum(TERRAIN_MOUNTAIN);
	RegisterEnum(TERRAIN_HILL);
	RegisterEnum(NUM_TERRAIN_TYPES);
	EnumEnd(L);

	//InterfaceModeTypes
	EnumStart(L, "InterfaceModeTypes");
	RegisterEnum(NO_INTERFACEMODE);
	RegisterEnum(INTERFACEMODE_DEBUG);
	RegisterEnum(INTERFACEMODE_SELECTION);
	RegisterEnum(INTERFACEMODE_PING);
	RegisterEnum(INTERFACEMODE_MOVE_TO);
	RegisterEnum(INTERFACEMODE_MOVE_TO_TYPE);
	RegisterEnum(INTERFACEMODE_MOVE_TO_ALL);
	RegisterEnum(INTERFACEMODE_ROUTE_TO);
	RegisterEnum(INTERFACEMODE_AIRLIFT);
	RegisterEnum(INTERFACEMODE_NUKE);
	RegisterEnum(INTERFACEMODE_PARADROP);
	RegisterEnum(INTERFACEMODE_ATTACK);
	RegisterEnum(INTERFACEMODE_RANGE_ATTACK);
	RegisterEnum(INTERFACEMODE_AIRSTRIKE);
	RegisterEnum(INTERFACEMODE_AIR_SWEEP);
	RegisterEnum(INTERFACEMODE_REBASE);
	RegisterEnum(INTERFACEMODE_PLACE_UNIT);
	RegisterEnum(INTERFACEMODE_EMBARK);
	RegisterEnum(INTERFACEMODE_DISEMBARK);
	RegisterEnum(INTERFACEMODE_GIFT_UNIT);
	RegisterEnum(INTERFACEMODE_CITY_PLOT_SELECTION);
	RegisterEnum(INTERFACEMODE_PURCHASE_PLOT);
	RegisterEnum(INTERFACEMODE_CITY_RANGE_ATTACK);
	RegisterEnum(INTERFACEMODE_GIFT_TILE_IMPROVEMENT);
	RegisterEnum(NUM_INTERFACEMODE_TYPES);
	EnumEnd(L);

	EnumStart(L, "NotificationTypes");
	CvNotificationXMLEntries* pkNotifications = GC.GetNotificationEntries();
	if (pkNotifications)
	{
		for (int i = 0; i < pkNotifications->GetNumNotifications(); ++i)
		{
			CvNotificationEntry* pkEntry = pkNotifications->GetEntry(i);		
			if (pkEntry)
				pRegisterEnum(L, pkEntry->GetTypeName(), FString::Hash(pkEntry->GetTypeName()));
		}
	}
	EnumEnd(L);

	// ActivityTypes
	EnumStart(L, "ActivityTypes");
	RegisterEnum(NO_ACTIVITY);
	RegisterEnum(ACTIVITY_AWAKE);
	RegisterEnum(ACTIVITY_HOLD);
	RegisterEnum(ACTIVITY_SLEEP);
	RegisterEnum(ACTIVITY_HEAL);
	RegisterEnum(ACTIVITY_SENTRY);
	RegisterEnum(ACTIVITY_INTERCEPT);
	RegisterEnum(ACTIVITY_MISSION);
	EnumEnd(L);

	// MissionTypes
	EnumStart(L, "MissionTypes");
	RegisterEnum(NO_MISSION);
	RegisterDynamicEnums(L, "Missions", "ID", "Type", "NUM_MISSION_TYPES");
	EnumEnd(L);


	// ActionSubTypes
	EnumStart(L, "ActionSubTypes");
	RegisterEnum(ACTIONSUBTYPE_INTERFACEMODE);
	RegisterEnum(ACTIONSUBTYPE_COMMAND);
	RegisterEnum(ACTIONSUBTYPE_BUILD);
	RegisterEnum(ACTIONSUBTYPE_PROMOTION);
	RegisterEnum(ACTIONSUBTYPE_SPECIALIST);
	RegisterEnum(ACTIONSUBTYPE_CONTROL);
	RegisterEnum(ACTIONSUBTYPE_AUTOMATE);
	RegisterEnum(ACTIONSUBTYPE_MISSION);
	RegisterEnum(NUM_ACTIONSUBTYPES);
	EnumEnd(L);


	// GameMessageTypes
	EnumStart(L, "GameMessageTypes");
	RegisterEnum(GAMEMESSAGE_NETWORK_READY);
	RegisterEnum(GAMEMESSAGE_VERIFY_VERSION);
	RegisterEnum(GAMEMESSAGE_VERSION_NACK);
	RegisterEnum(GAMEMESSAGE_VERSION_WARNING);
	RegisterEnum(GAMEMESSAGE_GAME_TYPE);
	RegisterEnum(GAMEMESSAGE_ID_ASSIGNMENT);
	RegisterEnum(GAMEMESSAGE_FILE_INFO);
	RegisterEnum(GAMEMESSAGE_PICK_YOUR_CIV);
	RegisterEnum(GAMEMESSAGE_CIV_CHOICE);
	RegisterEnum(GAMEMESSAGE_CONFIRM_CIV_CLAIM);
	RegisterEnum(GAMEMESSAGE_CLAIM_INFO);
	RegisterEnum(GAMEMESSAGE_CIV_CHOICE_ACK);
	RegisterEnum(GAMEMESSAGE_CIV_CHOICE_NACK);
	RegisterEnum(GAMEMESSAGE_CIV_CHOSEN);
	RegisterEnum(GAMEMESSAGE_INTERIM_NOTICE);
	RegisterEnum(GAMEMESSAGE_INIT_INFO);
	RegisterEnum(GAMEMESSAGE_MAPSCRIPT_CHECK);
	RegisterEnum(GAMEMESSAGE_MAPSCRIPT_ACK);
	RegisterEnum(GAMEMESSAGE_LOAD_GAME);
	RegisterEnum(GAMEMESSAGE_PLAYER_ID);
	RegisterEnum(GAMEMESSAGE_SLOT_REASSIGNMENT);
	RegisterEnum(GAMEMESSAGE_PLAYER_INFO);
	RegisterEnum(GAMEMESSAGE_GAME_INFO);
	RegisterEnum(GAMEMESSAGE_REASSIGN_PLAYER);
	RegisterEnum(GAMEMESSAGE_PITBOSS_INFO);
	RegisterEnum(GAMEMESSAGE_LAUNCHING_INFO);
	RegisterEnum(GAMEMESSAGE_INIT_GAME);
	RegisterEnum(GAMEMESSAGE_INIT_PLAYERS);
	RegisterEnum(GAMEMESSAGE_AUTH_REQUEST);
	RegisterEnum(GAMEMESSAGE_AUTH_RESPONSE);
	RegisterEnum(GAMEMESSAGE_SYNCH_START);
	RegisterEnum(GAMEMESSAGE_PLAYER_OPTION);
	RegisterEnum(GAMEMESSAGE_EXTENDED_GAME);
	RegisterEnum(GAMEMESSAGE_AUTO_MOVES);
	RegisterEnum(GAMEMESSAGE_TURN_COMPLETE);
	RegisterEnum(GAMEMESSAGE_PUSH_MISSION);
	RegisterEnum(GAMEMESSAGE_AUTO_MISSION);
	RegisterEnum(GAMEMESSAGE_DO_COMMAND);
	RegisterEnum(GAMEMESSAGE_PUSH_ORDER);
	RegisterEnum(GAMEMESSAGE_POP_ORDER);
	RegisterEnum(GAMEMESSAGE_DO_TASK);
	RegisterEnum(GAMEMESSAGE_RESEARCH);
	RegisterEnum(GAMEMESSAGE_CONVERT);
	RegisterEnum(GAMEMESSAGE_CHAT);
	RegisterEnum(GAMEMESSAGE_PING);
	RegisterEnum(GAMEMESSAGE_SIGN);
	RegisterEnum(GAMEMESSAGE_LINE_ENTITY);
	RegisterEnum(GAMEMESSAGE_SIGN_DELETE);
	RegisterEnum(GAMEMESSAGE_LINE_ENTITY_DELETE);
	RegisterEnum(GAMEMESSAGE_LINE_GROUP_DELETE);
	RegisterEnum(GAMEMESSAGE_PAUSE);
	RegisterEnum(GAMEMESSAGE_MP_KICK);
	RegisterEnum(GAMEMESSAGE_MP_RETIRE);
	RegisterEnum(GAMEMESSAGE_CLOSE_CONNECTION);
	RegisterEnum(GAMEMESSAGE_NEVER_JOINED);
	RegisterEnum(GAMEMESSAGE_MP_DROP_INIT);
	RegisterEnum(GAMEMESSAGE_MP_DROP_VOTE);
	RegisterEnum(GAMEMESSAGE_MP_DROP_UPDATE);
	RegisterEnum(GAMEMESSAGE_MP_DROP_RESULT);
	RegisterEnum(GAMEMESSAGE_MP_DROP_SAVE);
	RegisterEnum(GAMEMESSAGE_TOGGLE_TRADE);
	RegisterEnum(GAMEMESSAGE_IMPLEMENT_OFFER);
	RegisterEnum(GAMEMESSAGE_CHANGE_WAR);
	RegisterEnum(GAMEMESSAGE_LIBERATE_PLAYER);
	RegisterEnum(GAMEMESSAGE_CHOOSE_ELECTION);
	RegisterEnum(GAMEMESSAGE_DIPLO_VOTE);
	RegisterEnum(GAMEMESSAGE_APPLY_EVENT);
	RegisterEnum(GAMEMESSAGE_CONTACT_CIV);
	RegisterEnum(GAMEMESSAGE_DIPLO_CHAT);
	RegisterEnum(GAMEMESSAGE_SEND_OFFER);
	RegisterEnum(GAMEMESSAGE_RENEGOTIATE);
	RegisterEnum(GAMEMESSAGE_RENEGOTIATE_ITEM);
	RegisterEnum(GAMEMESSAGE_EXIT_TRADE);
	RegisterEnum(GAMEMESSAGE_KILL_DEAL);
	RegisterEnum(GAMEMESSAGE_UPDATE_POLICIES);
	RegisterEnum(GAMEMESSAGE_CLEAR_TABLE);
	RegisterEnum(GAMEMESSAGE_POPUP_PROCESSED);
	RegisterEnum(GAMEMESSAGE_DIPLOMACY_FROM_UI);
	RegisterEnum(GAMEMESSAGE_HOT_DROP_NOTICE);
	RegisterEnum(GAMEMESSAGE_POPUP);
	RegisterEnum(GAMEMESSAGE_EVENT_TRIGGERED);
	RegisterEnum(GAMEMESSAGE_LAUNCH_SPACESHIP);
	RegisterEnum(GAMEMESSAGE_ADVANCED_START_ACTION);
	RegisterEnum(GAMEMESSAGE_MINOR_CIV_QUEST_NO_INTEREST);
	RegisterEnum(GAMEMESSAGE_MINOR_CIV_QUEST_COMPLETED);
	RegisterEnum(GAMEMESSAGE_MINOR_CIV_INTRUSION);
	RegisterEnum(GAMEMESSAGE_MINOR_CIV_ENTER_TERRITORY);
	RegisterEnum(GAMEMESSAGE_BARBARIAN_RANSOM);
	RegisterEnum(GAMEMESSAGE_PLAYER_HURRY);
	RegisterEnum(GAMEMESSAGE_MOD_NET_MESSAGE);
	RegisterEnum(GAMEMESSAGE_SWAP_UNITS);
	RegisterEnum(GAMEMESSAGE_SWAP_ORDER);
	RegisterEnum(GAMEMESSAGE_AVOID_GROWTH);
	EnumEnd(L);

	// TaskTypes
	EnumStart(L, "TaskTypes");
	RegisterEnum(TASK_RAZE);
	RegisterEnum(TASK_UNRAZE);
	RegisterEnum(TASK_DISBAND);
	RegisterEnum(TASK_GIFT);
	RegisterEnum(TASK_SET_AUTOMATED_CITIZENS);
	RegisterEnum(TASK_SET_AUTOMATED_PRODUCTION);
	RegisterEnum(TASK_SET_EMPHASIZE);
	RegisterEnum(TASK_NO_AUTO_ASSIGN_SPECIALISTS);
	RegisterEnum(TASK_ADD_SPECIALIST);
	RegisterEnum(TASK_REMOVE_SPECIALIST);
	RegisterEnum(TASK_CHANGE_WORKING_PLOT);
	RegisterEnum(TASK_REMOVE_SLACKER);
	RegisterEnum(TASK_CLEAR_WORKING_OVERRIDE);
	RegisterEnum(TASK_HURRY);
	RegisterEnum(TASK_CONSCRIPT);
	RegisterEnum(TASK_CLEAR_ORDERS);
	RegisterEnum(TASK_RALLY_PLOT);
	RegisterEnum(TASK_CLEAR_RALLY_PLOT);
	RegisterEnum(TASK_RANGED_ATTACK);
	RegisterEnum(TASK_CREATE_PUPPET);
	RegisterEnum(TASK_ANNEX_PUPPET);
	RegisterEnum(NUM_TASK_TYPES);
	EnumEnd(L);

	// CommandTypes
	EnumStart(L, "CommandTypes");
	RegisterEnum(COMMAND_PROMOTION);
	RegisterEnum(COMMAND_UPGRADE);
	RegisterEnum(COMMAND_AUTOMATE);
	RegisterEnum(COMMAND_WAKE);
	RegisterEnum(COMMAND_CANCEL);
	RegisterEnum(COMMAND_CANCEL_ALL);
	RegisterEnum(COMMAND_STOP_AUTOMATION);
	RegisterEnum(COMMAND_DELETE);
	RegisterEnum(COMMAND_GIFT);
	RegisterEnum(COMMAND_HOTKEY);
	RegisterEnum(NUM_COMMAND_TYPES);
	EnumEnd(L);

	// DirectionTypes
	EnumStart(L, "DirectionTypes");
	RegisterEnum(NO_DIRECTION);
	RegisterEnum(DIRECTION_NORTHEAST);
	RegisterEnum(DIRECTION_EAST);
	RegisterEnum(DIRECTION_SOUTHEAST);
	RegisterEnum(DIRECTION_SOUTHWEST);
	RegisterEnum(DIRECTION_WEST);
	RegisterEnum(DIRECTION_NORTHWEST);
	RegisterEnum(NUM_DIRECTION_TYPES);
	EnumEnd(L);

	// DiploUIStateTypes
	EnumStart(L, "DiploUIStateTypes");
	RegisterEnum(NO_DIPLO_UI_STATE);

	RegisterEnum(DIPLO_UI_STATE_DEFAULT_ROOT);

	RegisterEnum(DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN);
	RegisterEnum(DIPLO_UI_STATE_PEACE_MADE_BY_HUMAN);

	RegisterEnum(DIPLO_UI_STATE_TRADE);
	RegisterEnum(DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER);
	RegisterEnum(DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER);
	RegisterEnum(DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER);
	RegisterEnum(DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND);
	RegisterEnum(DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST);
	RegisterEnum(DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS);

	RegisterEnum(DIPLO_UI_STATE_HUMAN_DEMAND);

	RegisterEnum(DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT);
	RegisterEnum(DIPLO_UI_STATE_BLANK_DISCUSSION);
	RegisterEnum(DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN);
	RegisterEnum(DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI);
	RegisterEnum(DIPLO_UI_STATE_AI_DECLARED_WAR);

	RegisterEnum(DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED);

	RegisterEnum(DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_PROTECT_MINOR_CIV);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING);

	RegisterEnum(DIPLO_UI_STATE_DISCUSS_WORK_WITH_US);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_COOP_WAR);
	RegisterEnum(DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME);

	RegisterEnum(DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT);

	RegisterEnum(DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE);

	RegisterEnum(DIPLO_UI_STATE_CAUGHT_YOUR_SPY);
	RegisterEnum(DIPLO_UI_STATE_KILLED_YOUR_SPY);
	RegisterEnum(DIPLO_UI_STATE_KILLED_MY_SPY);
	RegisterEnum(DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY);

	RegisterEnum(DIPLO_UI_STATE_STOP_CONVERSIONS);

	RegisterEnum(NUM_DIPLO_UI_STATES);
	EnumEnd(L);

	EnumStart(L, "FlowDirectionTypes");
	RegisterEnum(NO_FLOWDIRECTION);

	RegisterEnum(FLOWDIRECTION_NORTH);
	RegisterEnum(FLOWDIRECTION_NORTHEAST);
	RegisterEnum(FLOWDIRECTION_SOUTHEAST);
	RegisterEnum(FLOWDIRECTION_SOUTH);
	RegisterEnum(FLOWDIRECTION_SOUTHWEST);
	RegisterEnum(FLOWDIRECTION_NORTHWEST);

	RegisterEnum(NUM_FLOWDIRECTION_TYPES);

	RegisterEnum(FLOWDIRECTION_NORTH_MASK);
	RegisterEnum(FLOWDIRECTION_SOUTH_MASK);
	RegisterEnum(FLOWDIRECTION_SOUTHEAST_MASK);
	RegisterEnum(FLOWDIRECTION_NORTHWEST_MASK);
	RegisterEnum(FLOWDIRECTION_SOUTHWEST_MASK);
	RegisterEnum(FLOWDIRECTION_NORTHEAST_MASK);
	EnumEnd(L);

	// PolicyBranchTypes
	EnumStart(L, "PolicyBranchTypes");
	RegisterEnum(NO_POLICY_BRANCH_TYPE);
	EnumEnd(L);

	// FromUIDiploEventTypes
	EnumStart(L, "FromUIDiploEventTypes");
	RegisterEnum(NO_FROM_UI_DIPLO_EVENT);

	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_WANTS_DISCUSSION);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_DONT_SETTLE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_END_WORK_WITH_US);

	RegisterEnum(FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL);
	RegisterEnum(FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL);

	RegisterEnum(FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_DENOUNCE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_COOP_WAR_OFFER);
	RegisterEnum(FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE);

	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DEMAND);

	RegisterEnum(FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE);

	// Post Civ 5 release
	RegisterEnum(FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_INTRIGUE);
	RegisterEnum(FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPREADING_RELIGION);

	RegisterEnum(FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS);

	RegisterEnum(NUM_FROM_UI_DIPLO_EVENTS);
	EnumEnd(L);

	// CoopWarStates
	EnumStart(L, "CoopWarStates");
	RegisterEnum(NO_COOP_WAR_STATE);
	RegisterEnum(COOP_WAR_STATE_REJECTED);
	RegisterEnum(COOP_WAR_STATE_SOON);
	RegisterEnum(COOP_WAR_STATE_ACCEPTED);
	RegisterEnum(NUM_COOP_WAR_STATES);
	EnumEnd(L);

	// ThreatTypes
	EnumStart(L, "ThreatTypes");
	RegisterEnum(NO_THREAT_VALUE);
	RegisterEnum(THREAT_NONE);
	RegisterEnum(THREAT_MINOR);
	RegisterEnum(THREAT_MAJOR);
	RegisterEnum(THREAT_SEVERE);
	RegisterEnum(THREAT_CRITICAL);
	RegisterEnum(NUM_THREAT_VALUES);
	EnumEnd(L);

	// DisputeLevelTypes
	EnumStart(L, "DisputeLevelTypes");
	RegisterEnum(NO_DISPUTE_LEVEL);
	RegisterEnum(DISPUTE_LEVEL_NONE);
	RegisterEnum(DISPUTE_LEVEL_WEAK);
	RegisterEnum(DISPUTE_LEVEL_STRONG);
	RegisterEnum(DISPUTE_LEVEL_FIERCE);
	RegisterEnum(NUM_DISPUTE_LEVELS);
	EnumEnd(L);

	// LeaderheadAnimationTypes
	EnumStart(L, "LeaderheadAnimationTypes");
	RegisterEnum(NO_LEADERHEAD_ANIM);
	RegisterEnum(LEADERHEAD_ANIM_INTRO);
	RegisterEnum(LEADERHEAD_ANIM_NEUTRAL_HELLO);
	RegisterEnum(LEADERHEAD_ANIM_PEACEFUL);
	RegisterEnum(LEADERHEAD_ANIM_OPENING_GLOAT);
	RegisterEnum(LEADERHEAD_ANIM_DECLARE_WAR);
	RegisterEnum(LEADERHEAD_ANIM_ATTACKED);
	RegisterEnum(LEADERHEAD_ANIM_HATE_HELLO);
	RegisterEnum(LEADERHEAD_ANIM_DEFEATED);
	RegisterEnum(LEADERHEAD_ANIM_REQUEST);
	RegisterEnum(LEADERHEAD_ANIM_DEMAND);
	RegisterEnum(LEADERHEAD_ANIM_NEUTRAL_IDLE);
	RegisterEnum(LEADERHEAD_ANIM_HATE_IDLE);
	RegisterEnum(LEADERHEAD_ANIM_LETS_HEAR_IT);
	RegisterEnum(LEADERHEAD_ANIM_YES);
	RegisterEnum(LEADERHEAD_ANIM_NO);
	EnumEnd(L);

	// TradeableItems
	EnumStart(L, "TradeableItems");
	RegisterEnum(TRADE_ITEM_NONE);
	RegisterEnum(TRADE_ITEM_GOLD);
	RegisterEnum(TRADE_ITEM_GOLD_PER_TURN);
	RegisterEnum(TRADE_ITEM_MAPS);
	RegisterEnum(TRADE_ITEM_RESOURCES);
	RegisterEnum(TRADE_ITEM_CITIES);
	RegisterEnum(TRADE_ITEM_UNITS);
	RegisterEnum(TRADE_ITEM_OPEN_BORDERS);
	RegisterEnum(TRADE_ITEM_DEFENSIVE_PACT);
	RegisterEnum(TRADE_ITEM_RESEARCH_AGREEMENT);
	RegisterEnum(TRADE_ITEM_TRADE_AGREEMENT);
	RegisterEnum(TRADE_ITEM_PERMANENT_ALLIANCE);
	RegisterEnum(TRADE_ITEM_SURRENDER);
	RegisterEnum(TRADE_ITEM_TRUCE);
	RegisterEnum(TRADE_ITEM_PEACE_TREATY);
	RegisterEnum(TRADE_ITEM_THIRD_PARTY_PEACE);
	RegisterEnum(TRADE_ITEM_THIRD_PARTY_WAR);
	RegisterEnum(TRADE_ITEM_THIRD_PARTY_EMBARGO);
	RegisterEnum(TRADE_ITEM_ALLOW_EMBASSY);
	RegisterEnum(TRADE_ITEM_DECLARATION_OF_FRIENDSHIP);
	RegisterEnum(NUM_TRADEABLE_ITEMS);
	EnumEnd(L);

	// EndTurnBlockingTypes
	EnumStart(L, "EndTurnBlockingTypes");
	RegisterEnum(NO_ENDTURN_BLOCKING_TYPE);
	RegisterEnum(ENDTURN_BLOCKING_POLICY);
	RegisterEnum(ENDTURN_BLOCKING_RESEARCH);
	RegisterEnum(ENDTURN_BLOCKING_PRODUCTION);
	RegisterEnum(ENDTURN_BLOCKING_UNITS);
	RegisterEnum(ENDTURN_BLOCKING_DIPLO_VOTE);
	RegisterEnum(ENDTURN_BLOCKING_MINOR_QUEST);
	RegisterEnum(ENDTURN_BLOCKING_FREE_TECH);
	RegisterEnum(ENDTURN_BLOCKING_STACKED_UNITS);
	RegisterEnum(ENDTURN_BLOCKING_UNIT_NEEDS_ORDERS);
	RegisterEnum(ENDTURN_BLOCKING_UNIT_PROMOTION);
	RegisterEnum(ENDTURN_BLOCKING_CITY_RANGE_ATTACK);
	RegisterEnum(ENDTURN_BLOCKING_FREE_POLICY);
	RegisterEnum(ENDTURN_BLOCKING_FREE_ITEMS);
	RegisterEnum(ENDTURN_BLOCKING_FOUND_PANTHEON);
	RegisterEnum(ENDTURN_BLOCKING_FOUND_RELIGION);
	RegisterEnum(ENDTURN_BLOCKING_ENHANCE_RELIGION);
	RegisterEnum(ENDTURN_BLOCKING_STEAL_TECH);
	RegisterEnum(ENDTURN_BLOCKING_MAYA_LONG_COUNT);
	RegisterEnum(ENDTURN_BLOCKING_FAITH_GREAT_PERSON);
	RegisterEnum(NUM_ENDTURN_BLOCKING_TYPES);
	EnumEnd(L);

	// ResourceUsageTypes
	EnumStart(L, "ResourceUsageTypes");
	RegisterEnum(RESOURCEUSAGE_BONUS);
	RegisterEnum(RESOURCEUSAGE_STRATEGIC);
	RegisterEnum(RESOURCEUSAGE_LUXURY);
	RegisterEnum(NUM_RESOURCEUSAGE_TYPES);
	EnumEnd(L);

	// MajorCivApproachTypes
	EnumStart(L, "MajorCivApproachTypes");
	RegisterEnum(NO_MAJOR_CIV_APPROACH);
	RegisterEnum(MAJOR_CIV_APPROACH_WAR);
	RegisterEnum(MAJOR_CIV_APPROACH_HOSTILE);
	RegisterEnum(MAJOR_CIV_APPROACH_DECEPTIVE);
	RegisterEnum(MAJOR_CIV_APPROACH_GUARDED);
	RegisterEnum(MAJOR_CIV_APPROACH_AFRAID);
	RegisterEnum(MAJOR_CIV_APPROACH_FRIENDLY);
	RegisterEnum(MAJOR_CIV_APPROACH_NEUTRAL);
	RegisterEnum(NUM_MAJOR_CIV_APPROACHES);
	EnumEnd(L);

	// MinorCivTraitTypes
	EnumStart(L, "MinorCivTraitTypes");
	RegisterEnum(NO_MINOR_CIV_TRAIT_TYPE);
	RegisterEnum(MINOR_CIV_TRAIT_CULTURED);
	RegisterEnum(MINOR_CIV_TRAIT_MILITARISTIC);
	RegisterEnum(MINOR_CIV_TRAIT_MARITIME);
	RegisterEnum(MINOR_CIV_TRAIT_MERCANTILE);
	RegisterEnum(MINOR_CIV_TRAIT_RELIGIOUS);
	RegisterEnum(NUM_MINOR_CIV_TRAIT_TYPES);
	EnumEnd(L);

	// MinorCivPersonalityTypes
	EnumStart(L, "MinorCivPersonalityTypes");
	RegisterEnum(NO_MINOR_CIV_PERSONALITY_TYPE);
	RegisterEnum(MINOR_CIV_PERSONALITY_FRIENDLY);
	RegisterEnum(MINOR_CIV_PERSONALITY_NEUTRAL);
	RegisterEnum(MINOR_CIV_PERSONALITY_HOSTILE);
	RegisterEnum(MINOR_CIV_PERSONALITY_IRRATIONAL);
	RegisterEnum(NUM_MINOR_CIV_PERSONALITY_TYPES);
	EnumEnd(L);

	// MinorCivQuestTypes
	EnumStart(L, "MinorCivQuestTypes");
	RegisterEnum(NO_MINOR_CIV_QUEST_TYPE);
	RegisterEnum(MINOR_CIV_QUEST_ROUTE);
	RegisterEnum(MINOR_CIV_QUEST_KILL_CAMP);
	RegisterEnum(MINOR_CIV_QUEST_CONNECT_RESOURCE);
	RegisterEnum(MINOR_CIV_QUEST_CONSTRUCT_WONDER);
	RegisterEnum(MINOR_CIV_QUEST_GREAT_PERSON);
	RegisterEnum(MINOR_CIV_QUEST_KILL_CITY_STATE);
	RegisterEnum(MINOR_CIV_QUEST_FIND_PLAYER);
	RegisterEnum(MINOR_CIV_QUEST_FIND_NATURAL_WONDER);
	RegisterEnum(MINOR_CIV_QUEST_GIVE_GOLD);
	RegisterEnum(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT);
	RegisterEnum(MINOR_CIV_QUEST_CONTEST_CULTURE);
	RegisterEnum(MINOR_CIV_QUEST_CONTEST_FAITH);
	RegisterEnum(MINOR_CIV_QUEST_CONTEST_TECHS);
	RegisterEnum(MINOR_CIV_QUEST_INVEST);
	RegisterEnum(MINOR_CIV_QUEST_BULLY_CITY_STATE);
	RegisterEnum(MINOR_CIV_QUEST_DENOUNCE_MAJOR);
	RegisterEnum(MINOR_CIV_QUEST_SPREAD_RELIGION);
	RegisterEnum(NUM_MINOR_CIV_QUEST_TYPES);
	EnumEnd(L);


	// Resource Usage
	EnumStart(L, "ResourceUsageTypes");
	RegisterEnum(RESOURCEUSAGE_BONUS);
	RegisterEnum(RESOURCEUSAGE_STRATEGIC);
	RegisterEnum(RESOURCEUSAGE_LUXURY);
	RegisterEnum(NUM_RESOURCEUSAGE_TYPES);
	EnumEnd(L);

	// CityAIFocusTypes
	EnumStart(L, "CityAIFocusTypes");
	RegisterEnum(NO_CITY_AI_FOCUS_TYPE);
	RegisterEnum(CITY_AI_FOCUS_TYPE_FOOD);
	RegisterEnum(CITY_AI_FOCUS_TYPE_PRODUCTION);
	RegisterEnum(CITY_AI_FOCUS_TYPE_GOLD);
	RegisterEnum(CITY_AI_FOCUS_TYPE_GREAT_PEOPLE);
	RegisterEnum(CITY_AI_FOCUS_TYPE_SCIENCE);
	RegisterEnum(CITY_AI_FOCUS_TYPE_CULTURE);
	RegisterEnum(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
	RegisterEnum(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
	RegisterEnum(CITY_AI_FOCUS_TYPE_FAITH);
	RegisterEnum(NUM_CITY_AI_FOCUS_TYPES);
	EnumEnd(L);

	// Resource Usage
	EnumStart(L, "AdvisorTypes");
	RegisterEnum(NO_ADVISOR_TYPE);
	RegisterEnum(ADVISOR_ECONOMIC);
	RegisterEnum(ADVISOR_MILITARY);
	RegisterEnum(ADVISOR_FOREIGN);
	RegisterEnum(ADVISOR_SCIENCE);
	RegisterEnum(NUM_ADVISOR_TYPES);
	EnumEnd(L);

	// GenericWorldAnchorTypes
	EnumStart(L, "GenericWorldAnchorTypes");
	RegisterEnum(NO_WORLD_ANCHOR);
	RegisterEnum(WORLD_ANCHOR_NATURAL_WONDER);
	RegisterEnum(WORLD_ANCHOR_SETTLER);
	RegisterEnum(WORLD_ANCHOR_WORKER);
	RegisterEnum(NUM_WORLD_ANCHORS);
	EnumEnd(L);

	// Game States
	EnumStart(L, "GameStates");
	pRegisterEnum(L, "MainMenu", 0);
	pRegisterEnum(L, "MainGameView", 1);
	pRegisterEnum(L, "GameLoad", 2);
	pRegisterEnum(L, "LeaderHead", 3);
	pRegisterEnum(L, "NUM_GAME_STATES", 4);
	EnumEnd(L);

	// Gameplay Game States (not to be confused with the engine game states above)
	EnumStart(L, "GameplayGameStateTypes");
	RegisterEnum(GAMESTATE_ON);
	RegisterEnum(GAMESTATE_OVER);
	RegisterEnum(GAMESTATE_EXTENDED);
	EnumEnd(L);

	//
	EnumStart(L, "CombatPredictionTypes");
	RegisterEnum(NO_COMBAT_PREDICTION);
	RegisterEnum(COMBAT_PREDICTION_RANGED);
	RegisterEnum(COMBAT_PREDICTION_STALEMATE);
	RegisterEnum(COMBAT_PREDICTION_TOTAL_DEFEAT);
	RegisterEnum(COMBAT_PREDICTION_TOTAL_VICTORY);
	RegisterEnum(COMBAT_PREDICTION_MAJOR_VICTORY);
	RegisterEnum(COMBAT_PREDICTION_SMALL_VICTORY);
	RegisterEnum(COMBAT_PREDICTION_MAJOR_DEFEAT);
	RegisterEnum(COMBAT_PREDICTION_SMALL_DEFEAT);
	RegisterEnum(NUM_COMBAT_PREDICTIONS);
	EnumEnd(L);

	EnumStart(L, "ChatTargetTypes");
	RegisterEnum(NO_CHATTARGET);
	RegisterEnum(CHATTARGET_ALL);
	RegisterEnum(CHATTARGET_TEAM);
	RegisterEnum(CHATTARGET_PLAYER);
	EnumEnd(L);

	EnumStart(L, "ReligionTypes");
	RegisterEnum(NO_RELIGION);
	RegisterEnum(RELIGION_PANTHEON);
	EnumEnd(L);

	EnumStart(L, "BeliefTypes");
	RegisterEnum(NO_BELIEF);
	EnumEnd(L);

	EnumStart(L, "FaithPurchaseTypes");
	RegisterEnum(NO_AUTOMATIC_FAITH_PURCHASE);
	RegisterEnum(FAITH_PURCHASE_SAVE_PROPHET);
	RegisterEnum(FAITH_PURCHASE_UNIT);
	RegisterEnum(FAITH_PURCHASE_BUILDING);
	EnumEnd(L);

	EnumStart(L, "ControlTypes");
	RegisterEnum(NO_CONTROL);
	RegisterEnum(CONTROL_CENTERONSELECTION);
	RegisterEnum(CONTROL_SELECTYUNITTYPE);
	RegisterEnum(CONTROL_SELECTYUNITALL);
	RegisterEnum(CONTROL_SELECTCITY);
	RegisterEnum(CONTROL_SELECTCAPITAL);
	RegisterEnum(CONTROL_NEXTCITY);
	RegisterEnum(CONTROL_PREVCITY);
	RegisterEnum(CONTROL_NEXTUNIT);
	RegisterEnum(CONTROL_PREVUNIT);
	RegisterEnum(CONTROL_CYCLEUNIT);
	RegisterEnum(CONTROL_CYCLEUNIT_ALT);
	RegisterEnum(CONTROL_CYCLEWORKER);
	RegisterEnum(CONTROL_LASTUNIT);
	RegisterEnum(CONTROL_ENDTURN);
	RegisterEnum(CONTROL_ENDTURN_ALT);
	RegisterEnum(CONTROL_FORCEENDTURN);
	RegisterEnum(CONTROL_AUTOMOVES);
	RegisterEnum(CONTROL_PING);
	RegisterEnum(CONTROL_YIELDS);
	RegisterEnum(CONTROL_RESOURCE_ALL);
	RegisterEnum(CONTROL_UNIT_ICONS);
	RegisterEnum(CONTROL_SCORES);
	RegisterEnum(CONTROL_LOAD_GAME);
	RegisterEnum(CONTROL_OPTIONS_SCREEN);
	RegisterEnum(CONTROL_RETIRE);
	RegisterEnum(CONTROL_SAVE_GROUP);
	RegisterEnum(CONTROL_SAVE_NORMAL);
	RegisterEnum(CONTROL_QUICK_SAVE);
	RegisterEnum(CONTROL_QUICK_LOAD);
	RegisterEnum(CONTROL_CIVILOPEDIA);
	RegisterEnum(CONTROL_POLICIES_SCREEN);
	RegisterEnum(CONTROL_FOREIGN_SCREEN);
	RegisterEnum(CONTROL_MILITARY_SCREEN);
	RegisterEnum(CONTROL_TECH_CHOOSER);
	RegisterEnum(CONTROL_TURN_LOG);
	RegisterEnum(CONTROL_DOMESTIC_SCREEN);
	RegisterEnum(CONTROL_VICTORY_SCREEN);
	RegisterEnum(CONTROL_INFO);
	RegisterEnum(CONTROL_SELECT_HEALTHY);
	RegisterEnum(CONTROL_TOGGLE_STRATEGIC_VIEW);
	RegisterEnum(CONTROL_ADVISOR_COUNSEL);
	RegisterEnum(CONTROL_ESPIONAGE_OVERVIEW);
	RegisterEnum(CONTROL_RELIGION_OVERVIEW);
	RegisterEnum(CONTROL_RESTART_GAME);
	EnumEnd(L);

	return 0;
}
//------------------------------------------------------------------------------
void CvLuaEnums::pRegisterEnum(lua_State* L, const char* enumName, const int enumVal)
{
	lua_pushinteger(L, enumVal);
	lua_setfield(L, -2, enumName);
}
//------------------------------------------------------------------------------
void CvLuaEnums::pRegisterEnumUInt(lua_State* L, const char* enumName, const uint enumVal)
{
	lua_pushnumber(L, enumVal);
	lua_setfield(L, -2, enumName);
}
//------------------------------------------------------------------------------
void CvLuaEnums::EnumStart(lua_State* L, const char* enumType)
{
	lua_getglobal(L, enumType);
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, enumType);
	}
}
//------------------------------------------------------------------------------
void CvLuaEnums::EnumEnd(lua_State* L)
{
	lua_checkstack(L, 1);
	lua_pop(L, 1);
}
//------------------------------------------------------------------------------
void CvLuaEnums::RegisterDynamicEnums(lua_State* L, const char* szTableName, const char* szIdField, const char* szNameField, const char* szCountName)
{
	Database::Connection& db = *GC.GetGameDatabase();
	Database::Results kResults;

	char zSQL[256];
	sprintf_s(zSQL, 255, "select %s, %s from %s", szIdField, szNameField, szTableName);

	if(db.Execute(kResults, zSQL))
	{
		while(kResults.Step())
		{
			const int value = kResults.GetInt(0);
			const char* name = kResults.GetText(1);

			lua_pushstring(L, name);
			lua_pushvalue(L, -1);

			lua_pushinteger(L, value);
			lua_rawset(L, -4);

			lua_rawseti(L, -2, value);
		}
	}

	if(szCountName != NULL)
	{
		lua_pushstring(L, szCountName);
		lua_pushinteger(L, db.Count(szTableName, false));
		lua_rawset(L, -3);
	}
}