-------------------------------------------------
-- Action Info Panel
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local g_ActiveNotifications = {};
local g_Instances = {};


-------------------------------------------------------------------------------
-- details for dynamically sizing the small notification stack
-------------------------------------------------------------------------------
local DIPLO_SIZE_GUESS = 120;
local _, screenY = UIManager:GetScreenSizeVal();
local _, offsetY = Controls.OuterStack:GetOffsetVal();
local g_SmallScrollMax = screenY - offsetY - DIPLO_SIZE_GUESS;



-------------------------------------------------
-------------------------------------------------
function GenericLeftClick( Id )
	UI.ActivateNotification( Id )
end


-------------------------------------------------
-------------------------------------------------
function GenericRightClick ( Id )
	UI.RemoveNotification( Id )
end


------------------------------------------------------------------------------------
-- set up the exceptions
------------------------------------------------------------------------------------
Controls["TechButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["TechButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["ProductionButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["ProductionButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["FreeTechButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["FreeTechButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["StealTechButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["StealTechButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["FreePolicyButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["FreePolicyButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["FreeGreatPersonButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["FreeGreatPersonButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["FoundPantheonButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["FoundReligionButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["EnhanceReligionButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["AddReformationBeliefButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["LeagueCallForProposalsButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["ChooseArchaeologyButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["LeagueCallForVotesButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["ChooseIdeologyButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
-- CP
Controls["EventChoiceButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["EventChoiceButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);
Controls["CityEventChoiceButton"]:RegisterCallback(Mouse.eLClick, GenericLeftClick);
Controls["CityEventChoiceButton"]:RegisterCallback(Mouse.eRClick, GenericRightClick);

------------------------------------------------------------------------------------
-- build the list of types we can handle
------------------------------------------------------------------------------------
local g_NameTable = {};
g_NameTable[ -1 ]																= "Generic";
g_NameTable[ NotificationTypes.NOTIFICATION_POLICY ]							= "SocialPolicy";
g_NameTable[ NotificationTypes.NOTIFICATION_MET_MINOR ]							= "MetCityState";
g_NameTable[ NotificationTypes.NOTIFICATION_MINOR ]								= "CityState";
g_NameTable[ NotificationTypes.NOTIFICATION_MINOR_QUEST ]						= "CityState";
g_NameTable[ NotificationTypes.NOTIFICATION_ENEMY_IN_TERRITORY ]				= "EnemyInTerritory";
g_NameTable[ NotificationTypes.NOTIFICATION_REBELS ]							= "EnemyInTerritory";
g_NameTable[ NotificationTypes.NOTIFICATION_CITY_RANGE_ATTACK ]					= "CityCanBombard";
g_NameTable[ NotificationTypes.NOTIFICATION_BARBARIAN ]							= "Barbarian";
g_NameTable[ NotificationTypes.NOTIFICATION_GOODY ]								= "AncientRuins";
g_NameTable[ NotificationTypes.NOTIFICATION_BUY_TILE ]							= "BuyTile";
g_NameTable[ NotificationTypes.NOTIFICATION_CITY_GROWTH ]						= "CityGrowth";
g_NameTable[ NotificationTypes.NOTIFICATION_CITY_TILE ]							= "CityTile";
g_NameTable[ NotificationTypes.NOTIFICATION_DEMAND_RESOURCE ]					= "BonusResource";
g_NameTable[ NotificationTypes.NOTIFICATION_UNIT_PROMOTION ]					= "UnitPromoted";
--g_NameTable[ NotificationTypes.NOTIFICATION_WONDER_STARTED ]					= "WonderConstructed";
g_NameTable[ NotificationTypes.NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER]     = "WonderConstructed";
g_NameTable[ NotificationTypes.NOTIFICATION_WONDER_COMPLETED ]					= "WonderConstructed";
g_NameTable[ NotificationTypes.NOTIFICATION_WONDER_BEATEN ]						= "WonderConstructed";
g_NameTable[ NotificationTypes.NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER ]	= "GoldenAge";
--g_NameTable[ NotificationTypes.NOTIFICATION_GOLDEN_AGE_BEGUN ]				= "GoldenAge";
g_NameTable[ NotificationTypes.NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER ]	= "GoldenAge";
--g_NameTable[ NotificationTypes.NOTIFICATION_GOLDEN_AGE_ENDED ]				= "GoldenAge";
g_NameTable[ NotificationTypes.NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER ]		= "GreatPerson";
--g_NameTable[ NotificationTypes.NOTIFICATION_GREAT_PERSON ]					= "GreatPerson";
g_NameTable[ NotificationTypes.NOTIFICATION_STARVING ]							= "Starving";
g_NameTable[ NotificationTypes.NOTIFICATION_WAR_ACTIVE_PLAYER ]                 = "War";
g_NameTable[ NotificationTypes.NOTIFICATION_WAR ]								= "WarOther";
g_NameTable[ NotificationTypes.NOTIFICATION_PEACE_ACTIVE_PLAYER ]				= "Peace";
g_NameTable[ NotificationTypes.NOTIFICATION_PEACE ]								= "PeaceOther";
g_NameTable[ NotificationTypes.NOTIFICATION_VICTORY ]							= "Victory";
g_NameTable[ NotificationTypes.NOTIFICATION_UNIT_DIED ]							= "UnitDied";
g_NameTable[ NotificationTypes.NOTIFICATION_CITY_LOST ]							= "CapitalLost";
g_NameTable[ NotificationTypes.NOTIFICATION_CAPITAL_LOST_ACTIVE_PLAYER ]        = "CapitalLost";
g_NameTable[ NotificationTypes.NOTIFICATION_CAPITAL_LOST ]						= "CapitalLost";
g_NameTable[ NotificationTypes.NOTIFICATION_CAPITAL_RECOVERED ]					= "CapitalRecovered";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_KILLED ]						= "CapitalLost";
g_NameTable[ NotificationTypes.NOTIFICATION_DISCOVERED_LUXURY_RESOURCE ]		= "LuxuryResource";
g_NameTable[ NotificationTypes.NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE ]		= "StrategicResource";
g_NameTable[ NotificationTypes.NOTIFICATION_DISCOVERED_BONUS_RESOURCE ]			= "BonusResource";
--g_NameTable[ NotificationTypes.NOTIFICATION_POLICY_ADOPTION ]					= "Generic";
g_NameTable[ NotificationTypes.NOTIFICATION_DIPLO_VOTE ]						= "Generic";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_RACE ]						= "Generic";
g_NameTable[ NotificationTypes.NOTIFICATION_EXPLORATION_RACE ]					= "NaturalWonder";
g_NameTable[ NotificationTypes.NOTIFICATION_DIPLOMACY_DECLARATION ]				= "Diplomacy";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_GPT ]					= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_RESOURCE ]				= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS ]			= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT ]		= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_RESEARCH_AGREEMENT ]	= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT ]		= "DiplomacyX";
g_NameTable[ NotificationTypes.NOTIFICATION_TECH_AWARD ]						= "TechAward";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_DEAL ]						= "Diplomacy";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_DEAL_RECEIVED ]				= "DiplomacyPlayer";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_DEAL_RESOLVED ]				= "Diplomacy";
g_NameTable[ NotificationTypes.NOTIFICATION_PROJECT_COMPLETED ]					= "ProjectConstructed";


g_NameTable[ NotificationTypes.NOTIFICATION_TECH ]				= "Tech";
g_NameTable[ NotificationTypes.NOTIFICATION_PRODUCTION ]		= "Production";
g_NameTable[ NotificationTypes.NOTIFICATION_FREE_TECH ]			= "FreeTech";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_STOLE_TECH ]    = "StealTech";
g_NameTable[ NotificationTypes.NOTIFICATION_FREE_POLICY ]		= "FreePolicy";
g_NameTable[ NotificationTypes.NOTIFICATION_FREE_GREAT_PERSON ]	= "FreeGreatPerson";

g_NameTable[ NotificationTypes.NOTIFICATION_DENUNCIATION_EXPIRED ]	= "Diplomacy";
g_NameTable[ NotificationTypes.NOTIFICATION_FRIENDSHIP_EXPIRED ] = "Diplomacy";

g_NameTable[ NotificationTypes.NOTIFICATION_FOUND_PANTHEON] = "FoundPantheon";
g_NameTable[ NotificationTypes.NOTIFICATION_FOUND_RELIGION] = "FoundReligion";
g_NameTable[ NotificationTypes.NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER ] = "PantheonFounded";
g_NameTable[ NotificationTypes.NOTIFICATION_PANTHEON_FOUNDED ] = "PantheonFounded";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER ] = "ReligionFounded";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_FOUNDED ] = "ReligionFounded";
g_NameTable[ NotificationTypes.NOTIFICATION_ENHANCE_RELIGION] = "EnhanceReligion";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER ] = "ReligionEnhanced";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_ENHANCED ] = "ReligionEnhanced";
g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_SPREAD ] = "ReligionSpread";

g_NameTable[ NotificationTypes.NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER ]					= "NewSpy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_CANT_STEAL_TECH ]						= "SpyCannotSteal";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_EVICTED ]								= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_TECH_STOLEN_SPY_DETECTED ]					= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED ]				= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_KILLED_A_SPY ]							= "SpyKilledASpy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_WAS_KILLED ]							= "SpyWasKilled";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_REPLACEMENT ]							= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_PROMOTION ]								= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_DECEPTION ]						= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_ARMY ]       = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_AMPHIBIOUS ] = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN ]   = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN ]     = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN ]	  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN ]		  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN ]			  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN ] = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN ]	  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN ]	  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN ]	  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN ]			  = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_INTRIGUE_CONSTRUCTING_WONDER ]							  = "Spy";

g_NameTable[ NotificationTypes.NOTIFICATION_SPY_RIG_ELECTION_SUCCESS ]				    = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_RIG_ELECTION_FAILURE ]				    = "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_RIG_ELECTION_ALERT ]					= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS ]				= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE ]				= "SpyWasKilled";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_STAGE_COUP_SUCCESS ]					= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_SPY_STAGE_COUP_FAILURE ]					= "Spy";
g_NameTable[ NotificationTypes.NOTIFICATION_DIPLOMAT_EJECTED ]							= "Diplomat";

g_NameTable[ NotificationTypes.NOTIFICATION_CAN_BUILD_MISSIONARY ] = "EnoughFaith";
g_NameTable[ NotificationTypes.NOTIFICATION_AUTOMATIC_FAITH_PURCHASE_STOPPED] = "AutomaticFaithStop";
g_NameTable[ NotificationTypes.NOTIFICATION_OTHER_PLAYER_NEW_ERA ] = "OtherPlayerNewEra";

g_NameTable[ NotificationTypes.NOTIFICATION_MAYA_LONG_COUNT ]	= "FreeGreatPerson";
g_NameTable[ NotificationTypes.NOTIFICATION_FAITH_GREAT_PERSON ]	= "FreeGreatPerson";

g_NameTable[ NotificationTypes.NOTIFICATION_EXPANSION_PROMISE_EXPIRED ] = "Diplomacy";
g_NameTable[ NotificationTypes.NOTIFICATION_BORDER_PROMISE_EXPIRED ] = "Diplomacy";

g_NameTable[ NotificationTypes.NOTIFICATION_TRADE_ROUTE ] = "TradeRoute";
g_NameTable[ NotificationTypes.NOTIFICATION_TRADE_ROUTE_BROKEN ] = "TradeRouteBroken";

g_NameTable[ NotificationTypes.NOTIFICATION_RELIGION_SPREAD_NATURAL ] = "ReligionNaturalSpread";

g_NameTable[ NotificationTypes.NOTIFICATION_MINOR_BUYOUT ] = "CityStateBuyout";
g_NameTable[ NotificationTypes.NOTIFICATION_REQUEST_RESOURCE ] = "BonusResource";

g_NameTable[ NotificationTypes.NOTIFICATION_ADD_REFORMATION_BELIEF] = "AddReformationBelief";
g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_CALL_FOR_PROPOSALS ] = "LeagueCallForProposals";

g_NameTable[ NotificationTypes.NOTIFICATION_CHOOSE_ARCHAEOLOGY ] = "ChooseArchaeology";
g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_CALL_FOR_VOTES ] = "LeagueCallForVotes";

g_NameTable[ NotificationTypes.NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER ] = "ReformationBeliefAdded";
g_NameTable[ NotificationTypes.NOTIFICATION_REFORMATION_BELIEF_ADDED ] = "ReformationBeliefAdded";

g_NameTable[ NotificationTypes.NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER ] = "GreatWork";

g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_VOTING_DONE ] = "LeagueVotingDone";
g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_VOTING_SOON ] = "LeagueVotingSoon";

g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL ] = "CultureVictoryPositive";
g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO ] = "CultureVictoryNegative";
g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER ] = "CultureVictoryPositive";
g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE ] = "CultureVictoryNegative";
g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER ] = "CultureVictoryPositive";
g_NameTable[ NotificationTypes.NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL ] = "CultureVictoryNegative";

g_NameTable[ NotificationTypes.NOTIFICATION_CHOOSE_IDEOLOGY ] = "ChooseIdeology";
g_NameTable[ NotificationTypes.NOTIFICATION_IDEOLOGY_CHOSEN ] = "IdeologyChosen";

g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_RECONNECTED ] = "PlayerReconnected";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_DISCONNECTED ] = "PlayerDisconnected";
g_NameTable[ NotificationTypes.NOTIFICATION_TURN_MODE_SEQUENTIAL ] = "SequentialTurns";
g_NameTable[ NotificationTypes.NOTIFICATION_TURN_MODE_SIMULTANEOUS ] = "SimultaneousTurns";
g_NameTable[ NotificationTypes.NOTIFICATION_HOST_MIGRATION ] = "HostMigration";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_CONNECTING ] = "PlayerConnecting";
g_NameTable[ NotificationTypes.NOTIFICATION_PLAYER_KICKED ] = "PlayerKicked";

g_NameTable[ NotificationTypes.NOTIFICATION_CITY_REVOLT_POSSIBLE ]= "Generic";
g_NameTable[ NotificationTypes.NOTIFICATION_CITY_REVOLT ] = "Generic";

g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_PROJECT_COMPLETE ] = "LeagueProjectComplete";
g_NameTable[ NotificationTypes.NOTIFICATION_LEAGUE_PROJECT_PROGRESS ] = "LeagueProjectProgress";

-- CP
g_NameTable[ NotificationTypes.NOTIFICATION_INSTANT_YIELD ] = "InstantYield";
g_NameTable[ NotificationTypes.NOTIFICATION_EVENT_CHOICE_CITY ] = "CityEventChoice";
g_NameTable[ NotificationTypes.NOTIFICATION_EVENT_CHOICE ] = "EventChoice";

g_NameTable[ NotificationTypes.NOTIFICATION_PRODUCTION_COST_MODIFIERS_FROM_RESOURCES ] = "BonusResource";
--END
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
--- Actual new notification entry point
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------

-------------------------------------------------
-------------------------------------------------
function OnNotificationAdded( Id, type, toolTip, strSummary, iGameValue, iExtraGameData, ePlayer )
	if(g_ActiveNotifications[ Id ] ~= nil) then
        return;
    end
       
    local name = g_NameTable[ type ];
    
    local button;
    local text;
    local bg;
    local fingerTitle;
    local root;
        
    if( name == "Production" or
        name == "Tech" or
        name == "FreeTech" or
        name == "StealTech" or
        name == "FreePolicy" or
		name == "FreeGreatPerson" or
		name == "FoundPantheon" or
		name == "FoundReligion" or
		name == "EnhanceReligion" or
		name == "AddReformationBelief" or
		name == "LeagueCallForProposals" or
		name == "ChooseArchaeology" or
		name == "LeagueCallForVotes" or
		--CP
		name == "EventChoice" or
		name == "CityEventChoice" or
		-- END
		name == "ChooseIdeology") then
        
        button = Controls[name .. "Button"];
        text = Controls[name .. "Text"];
        bg = Controls[name .. "BG"];
        
    else
        if(name == nil) then
            print( "Unknown Notification, Adding generic " .. Id );
            name = "Generic";
        end

        local instance = {};
        ContextPtr:BuildInstanceForControl( name .. "Item", instance, Controls.SmallStack );
        g_Instances[ Id ] = instance;
        
        button = instance[ name .. "Button" ];
        text   = instance[ name .. "Text" ];
        root   = instance[ name .. "Container" ];
        
        
        instance.FingerTitle:SetText( strSummary );
        root:BranchResetAnimation();
        
		if type == NotificationTypes.NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER
		or type == NotificationTypes.NOTIFICATION_WONDER_COMPLETED
		or type == NotificationTypes.NOTIFICATION_WONDER_BEATEN then
			if iGameValue ~= -1 then
				local portraitIndex = GameInfo.Buildings[iGameValue].PortraitIndex;
				if portraitIndex ~= -1 then
					IconHookup( portraitIndex, 80, GameInfo.Buildings[iGameValue].IconAtlas, instance.WonderConstructedAlphaAnim );				
				end
			end
			if iExtraGameData ~= -1 then
				CivIconHookup( iExtraGameData, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.WonderSmallCivFrame:SetHide(false);				
			else
				CivIconHookup( 22, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.WonderSmallCivFrame:SetHide(true);				
			end
		elseif type == NotificationTypes.NOTIFICATION_PROJECT_COMPLETED then
			if iGameValue ~= -1 then
				local portraitIndex = GameInfo.Projects[iGameValue].PortraitIndex;
				if portraitIndex ~= -1 then
					IconHookup( portraitIndex, 80, GameInfo.Projects[iGameValue].IconAtlas, instance.ProjectConstructedAlphaAnim );				
				end
			end
			if iExtraGameData ~= -1 then
				CivIconHookup( iExtraGameData, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.ProjectSmallCivFrame:SetHide(false);				
			else
				CivIconHookup( 22, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.ProjectSmallCivFrame:SetHide(true);				
			end
		elseif type == NotificationTypes.NOTIFICATION_DISCOVERED_LUXURY_RESOURCE 
		or type == NotificationTypes.NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE 
		or type == NotificationTypes.NOTIFICATION_DISCOVERED_BONUS_RESOURCE 
		or type == NotificationTypes.NOTIFICATION_DEMAND_RESOURCE
		or type == NotificationTypes.NOTIFICATION_REQUEST_RESOURCE
		or type == NotificationTypes.NOTIFICATION_PRODUCTION_COST_MODIFIERS_FROM_RESOURCES then
			local thisResourceInfo = GameInfo.Resources[iGameValue];
			local portraitIndex = thisResourceInfo.PortraitIndex;
			if portraitIndex ~= -1 then
				IconHookup( portraitIndex, 80, thisResourceInfo.IconAtlas, instance.ResourceImage );				
			end
		elseif type == NotificationTypes.NOTIFICATION_EXPLORATION_RACE then
			local thisFeatureInfo = GameInfo.Features[iGameValue];
			local portraitIndex = thisFeatureInfo.PortraitIndex;
			if portraitIndex ~= -1 then
				IconHookup( portraitIndex, 80, thisFeatureInfo.IconAtlas, instance.NaturalWonderImage );				
			end
		elseif type == NotificationTypes.NOTIFICATION_PLAYER_DEAL_RECEIVED then
			local index = iGameValue;
			CivIconHookup( index, 80, instance.DiplomacyPlayerImage, instance.CivIconBG, instance.CivIconShadow, false, true );
		elseif type == NotificationTypes.NOTIFICATION_TECH_AWARD then
			local thisTechInfo = GameInfo.Technologies[iExtraGameData];
			local portraitIndex = thisTechInfo.PortraitIndex;
			if portraitIndex ~= -1 then
				IconHookup( portraitIndex, 80, thisTechInfo.IconAtlas, instance.TechAwardImage );				
			else
				instance.TechAwardImage:SetHide( true );
			end
		elseif type == NotificationTypes.NOTIFICATION_UNIT_PROMOTION
		or type == NotificationTypes.NOTIFICATION_UNIT_DIED 
		or type == NotificationTypes.NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER 			
		or type == NotificationTypes.NOTIFICATION_ENEMY_IN_TERRITORY
		or type == NotificationTypes.NOTIFICATION_REBELS then
			local thisUnitType = iGameValue;
			local thisUnitInfo = GameInfo.Units[thisUnitType];
			local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(thisUnitType, ePlayer);
			if portraitOffset ~= -1 then
				IconHookup( portraitOffset, 80, portraitAtlas, instance.UnitImage );				
			end
		elseif type == NotificationTypes.NOTIFICATION_WAR_ACTIVE_PLAYER then
			local index = iGameValue;
			CivIconHookup( index, 80, instance.WarImage, instance.CivIconBG, instance.CivIconShadow, false, true ); 
		elseif type == NotificationTypes.NOTIFICATION_WAR then
			local index = iGameValue;
			CivIconHookup( index, 45, instance.War1Image, instance.Civ1IconBG, instance.Civ1IconShadow, false, true );
			local teamID = iExtraGameData;
			local team = Teams[teamID];
			index = team:GetLeaderID();
			CivIconHookup( index, 45, instance.War2Image, instance.Civ2IconBG, instance.Civ2IconShadow, false, true );
		elseif type == NotificationTypes.NOTIFICATION_PEACE_ACTIVE_PLAYER then
			local index = iGameValue;
			CivIconHookup( index, 80, instance.PeaceImage, instance.CivIconBG, instance.CivIconShadow, false, true );
		elseif type == NotificationTypes.NOTIFICATION_PEACE then
			local index = iGameValue;
			CivIconHookup( index, 45, instance.Peace1Image, instance.Civ1IconBG, instance.Civ1IconShadow, false, true );
			
			local teamID = iExtraGameData;
			local team = Teams[teamID];
			index = team:GetLeaderID();
			CivIconHookup( index, 45, instance.Peace2Image, instance.Civ2IconBG, instance.Civ2IconShadow, false, true );
		elseif type == NotificationTypes.NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER then
			--if iGameValue ~= -1 then
				--local portraitIndex = GameInfo.Buildings[iGameValue].PortraitIndex;
				--if portraitIndex ~= -1 then
					--IconHookup( portraitIndex, 80, GameInfo.Buildings[iGameValue].IconAtlas, instance.WonderConstructedAlphaAnim );				
				--end
			--end
			if iExtraGameData ~= -1 then
				CivIconHookup( iExtraGameData, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.WonderSmallCivFrame:SetHide(false);				
			else
				CivIconHookup( 22, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true );
				instance.WonderSmallCivFrame:SetHide(true);				
			end
		end
       
    end
    
    button:SetHide( false );
    button:SetVoid1( Id );
    button:RegisterCallback( Mouse.eLClick, GenericLeftClick );
    button:RegisterCallback( Mouse.eRClick, GenericRightClick );
    if (UI.IsTouchScreenEnabled()) then
        button:RegisterCallback( Mouse.eLDblClick, GenericRightClick );
	end
    
    local strToolTip = toolTip;
    button:SetToolTipString(strToolTip);
    
       
    g_ActiveNotifications[ Id ] = type;
    
    ProcessStackSizes();

end
Events.NotificationAdded.Add( OnNotificationAdded );


-------------------------------------------------
-------------------------------------------------
function RemoveNotificationID( Id )

    if( g_ActiveNotifications[ Id ] == nil )
    then
        print( "Attempt to remove unknown Notification " .. tostring( Id ) );
        return;
    end

    local name = g_NameTable[ g_ActiveNotifications[ Id ] ];
    
    if( name == "Production" or
        name == "Tech" or
        name == "FreeTech" or
        name == "StealTech" or
        name == "FreePolicy" or
		name == "FreeGreatPerson" or
		name == "FoundPantheon" or
		name == "FoundReligion" or
		name == "EnhanceReligion" or
		name == "AddReformationBelief" or
		name == "LeagueCallForProposals" or
		name == "ChooseArchaeology" or
		name == "LeagueCallForVotes" or
		--CP
		name == "EventChoice" or
		name == "CityEventChoice" or
		--END
		name == "ChooseIdeology")
    then
        Controls[ name .. "Button" ]:SetHide( true );
    else
     
        if( name == nil ) then
            name = "Generic";
        end
        
        local instance = g_Instances[ Id ];
        if( instance ~= nil ) then
			Controls.SmallStack:ReleaseChild( instance[ name .. "Container" ] );
		    g_Instances[ Id ] = nil;
		end
        
    end
    
	g_ActiveNotifications[ Id ] = nil;    
end

-------------------------------------------------
-------------------------------------------------
function NotificationRemoved( Id )

    --print( "removing Notification " .. Id .. " " .. tostring( g_ActiveNotifications[ Id ] ) .. " " .. tostring( g_NameTable[ g_ActiveNotifications[ Id ] ] ) );
        
	RemoveNotificationID( Id );	
    ProcessStackSizes();

end
Events.NotificationRemoved.Add( NotificationRemoved );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnNotificationPanelActivePlayerChanged( iActivePlayer, iPrevActivePlayer )

	-- Remove all the UI notifications.  The new player will rebroadcast any persistent ones from their last turn	
	for thisID, thisType in pairs(g_ActiveNotifications) do
		RemoveNotificationID(thisID);
	end

end
Events.GameplaySetActivePlayer.Add(OnNotificationPanelActivePlayerChanged);


-------------------------------------------------
-------------------------------------------------
function ProcessStackSizes()

    Controls.BigStack:CalculateSize();
    local bigY = Controls.BigStack:GetSizeY();
    Controls.SmallScrollPanel:SetSizeY( g_SmallScrollMax - bigY );

    Controls.SmallStack:CalculateSize();
    Controls.SmallStack:ReprocessAnchoring();

	Controls.SmallScrollPanel:CalculateInternalSize();
    if( Controls.SmallScrollPanel:GetRatio() ~= 1 ) then
        Controls.SmallScrollPanel:SetOffsetVal( 20, 0 );
    else
        Controls.SmallScrollPanel:SetOffsetVal( 0, 0 );
    end
    
    Controls.OuterStack:CalculateSize();
    Controls.OuterStack:ReprocessAnchoring();	  
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnNotificationsActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	UI.RebroadcastNotifications();
	ProcessStackSizes();
end
Events.GameplaySetActivePlayer.Add(OnNotificationsActivePlayerChanged);


UI.RebroadcastNotifications();
ProcessStackSizes();