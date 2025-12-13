-------------------------------------------------
-- Notification Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions" );

local g_NotificationInstanceManager = InstanceManager:new( "NotificationButton", "Button", Controls.NotificationButtonStack );
local m_PopupInfo = nil;

-- Notification Settings Variables
local g_InstantYieldsManager = InstanceManager:new("InstantYieldsInstance", "Base", Controls.AllInstantYieldsStack);

local g_InstantYieldClassification = {}
for i = 0, InstantYieldType.NUM_INSTANT_YIELD_TYPES - 1 do
	if  i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH or
		i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH_RETROACTIVE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CONSTRUCTION or
		i == InstantYieldType.INSTANT_YIELD_TYPE_BORDERS or
		i == InstantYieldType.INSTANT_YIELD_TYPE_U_PROD or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PURCHASE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_TILE_PURCHASE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_FOUND or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CONSTRUCTION_WONDER or
		i == InstantYieldType.INSTANT_YIELD_TYPE_FAITH_PURCHASE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_REFUND or
		i == InstantYieldType.INSTANT_YIELD_TYPE_FAITH_REFUND or
		i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH_HOLY_CITY or
		i == InstantYieldType.INSTANT_YIELD_TYPE_WLTKD_START
	then
		g_InstantYieldClassification[i] = "City"
	elseif
		i == InstantYieldType.INSTANT_YIELD_TYPE_DEATH or
		i == InstantYieldType.INSTANT_YIELD_TYPE_F_CONQUEST or
		i == InstantYieldType.INSTANT_YIELD_TYPE_VICTORY or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SCOUTING or
		i == InstantYieldType.INSTANT_YIELD_TYPE_ANCIENT_RUIN or
		i == InstantYieldType.INSTANT_YIELD_TYPE_UNIT_GIFT or
		i == InstantYieldType.INSTANT_YIELD_TYPE_LEVEL_UP or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_VICTORY_GLOBAL or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE_GLOBAL or
		i == InstantYieldType.INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE_UNIT or
		i == InstantYieldType.INSTANT_YIELD_TYPE_BULLY or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CITY_DAMAGE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PLUNDER_TRADE_ROUTE
	then
		g_InstantYieldClassification[i] = "Military"
	elseif
		i == InstantYieldType.INSTANT_YIELD_TYPE_GP_USE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_GP_BORN or
		i == InstantYieldType.INSTANT_YIELD_TYPE_F_SPREAD or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CONVERSION or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPREAD or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CULTURE_BOMB or
		i == InstantYieldType.INSTANT_YIELD_TYPE_REMOVE_HERESY or
		i == InstantYieldType.INSTANT_YIELD_TYPE_TR_END or
		i == InstantYieldType.INSTANT_YIELD_TYPE_TR_MOVEMENT or
		i == InstantYieldType.INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN or
		i == InstantYieldType.INSTANT_YIELD_TYPE_IMPROVEMENT_BUILD
	then
		g_InstantYieldClassification[i] = "Civilian"
	elseif
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_ATTACK or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_DEFENSE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_IDENTIFY or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID or
		i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_RIG_ELECTION
	then
		g_InstantYieldClassification[i] = "Spies"
	elseif
		i == InstantYieldType.INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE or
		i == InstantYieldType.INSTANT_YIELD_TYPE_TR_PRODUCTION_SIPHON or
		i == InstantYieldType.INSTANT_YIELD_TYPE_CONVERSION_EXPO or
		i == InstantYieldType.INSTANT_YIELD_TYPE_PROMOTION_OBTAINED
	then
		g_InstantYieldClassification[i] = "Unused"
	else
		g_InstantYieldClassification[i] = "Misc"
	end
end

function GetInstantYieldName(i)
	local str;
	if i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BIRTH_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_DEATH then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_DEATH_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PROPOSAL then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PROPOSAL_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_ERA_UNLOCK then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_ERA_UNLOCK_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_POLICY_UNLOCK then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_POLICY_UNLOCK_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_INSTANT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_INSTANT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TECH then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TECH_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CONSTRUCTION then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CONSTRUCTION_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BORDERS then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BORDERS_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_GP_USE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_GP_USE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_GP_BORN then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_GP_BORN_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_F_SPREAD then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_F_SPREAD_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_F_CONQUEST then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_F_CONQUEST_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_VICTORY then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_VICTORY_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_U_PROD then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_U_PROD_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PURCHASE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PURCHASE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TILE_PURCHASE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TILE_PURCHASE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_FOUND then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_FOUND_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TR_END then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TR_END_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CONVERSION then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CONVERSION_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPREAD then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPREAD_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BULLY then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BULLY_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TR_MOVEMENT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TR_MOVEMENT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SCOUTING then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SCOUTING_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_LEVEL_UP then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_LEVEL_UP_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PILLAGE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH_RETROACTIVE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BIRTH_RETROACTIVE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_ATTACK then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPY_ATTACK_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_DEFENSE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPY_DEFENSE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_DELEGATES then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_DELEGATES_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CONSTRUCTION_WONDER then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CONSTRUCTION_WONDER_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CULTURE_BOMB then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CULTURE_BOMB_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_REMOVE_HERESY then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_REMOVE_HERESY_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_FAITH_PURCHASE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_FAITH_PURCHASE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_VICTORY_GLOBAL then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_VICTORY_GLOBAL_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE_GLOBAL then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PILLAGE_GLOBAL_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CONVERSION_EXPO then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CONVERSION_EXPO_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PROMOTION_OBTAINED then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PROMOTION_OBTAINED_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TR_PRODUCTION_SIPHON then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TR_PRODUCTION_SIPHON_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_IMPROVEMENT_BUILD then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_IMPROVEMENT_BUILD_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_LUA then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_LUA_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_RESEARCH_AGREMEENT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_RESEARCH_AGREMEENT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_REFUND then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_REFUND_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_FAITH_REFUND then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_FAITH_REFUND_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BIRTH_HOLY_CITY then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BIRTH_HOLY_CITY_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_TECH_RETROACTIVE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_TECH_RETROACTIVE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PILLAGE_UNIT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PILLAGE_UNIT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_IDENTIFY then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPY_IDENTIFY_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_SPY_RIG_ELECTION then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_SPY_RIG_ELECTION_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_INSTANT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_INSTANT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_CITY_DAMAGE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_CITY_DAMAGE_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_LUXURY_RESOURCE_GAIN then
		str = Locale.Lookup("TXT_KEY_IINSTANT_YIELD_TYPE_LUXURY_RESOURCE_GAIN_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_GOLDEN_AGE_START then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_GOLDEN_AGE_START_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_UNIT_GIFT then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_UNIT_GIFT_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_BAKTUN_END then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_BAKTUN_END_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_WLTKD_START then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_WLTKD_START_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_ANCIENT_RUIN then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_ANCIENT_RUIN_SHORT");
	elseif i == InstantYieldType.INSTANT_YIELD_TYPE_PLUNDER_TRADE_ROUTE then
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_TYPE_PLUNDER_TRADE_ROUTE_SHORT");
	else
		-- failsafe
		str = Locale.Lookup("TXT_KEY_INSTANT_YIELD_UNKNOWN", i)
	end
	return str;
end

-- sort instant yields by localized name
local g_InstantYieldCitySorted = {}
local g_InstantYieldMilitarySorted = {}
local g_InstantYieldCivilianSorted = {}
local g_InstantYieldSpiesSorted = {}
local g_InstantYieldMiscSorted = {}

for i = 0, InstantYieldType.NUM_INSTANT_YIELD_TYPES - 1 do
	if g_InstantYieldClassification[i] ~= "Unused" then
		local entry = {}
		entry["Index"] = i
		entry["Name"] = GetInstantYieldName(i)
		if g_InstantYieldClassification[i] == "City" then
			table.insert(g_InstantYieldCitySorted, entry);
		elseif g_InstantYieldClassification[i] == "Military" then
			table.insert(g_InstantYieldMilitarySorted, entry);
		elseif g_InstantYieldClassification[i] == "Civilian" then
			table.insert(g_InstantYieldCivilianSorted, entry);
		elseif g_InstantYieldClassification[i] == "Spies" then
			table.insert(g_InstantYieldSpiesSorted, entry);
		else
			table.insert(g_InstantYieldMiscSorted, entry);
		end
	end
end
table.sort(g_InstantYieldCitySorted, function(a, b) return a.Name < b.Name end)
table.sort(g_InstantYieldMilitarySorted, function(a, b) return a.Name < b.Name end)
table.sort(g_InstantYieldCivilianSorted, function(a, b) return a.Name < b.Name end)
table.sort(g_InstantYieldSpiesSorted, function(a, b) return a.Name < b.Name end)
table.sort(g_InstantYieldMiscSorted, function(a, b) return a.Name < b.Name end)

-- init button text
Controls.InstantYieldCityButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CITY_BUTTON" ) );
Controls.InstantYieldMilitaryButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MILITARY_BUTTON" ) );
Controls.InstantYieldCivilianButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CIVILIAN_BUTTON" ) );
Controls.InstantYieldSpiesButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_SPIES_BUTTON" ) );
Controls.InstantYieldMiscButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MISC_BUTTON" ) );

-------------------------------------------------
-- Tab Switching Functions
-------------------------------------------------
function ShowLogTab()
	Controls.NotificationLogPanel:SetHide(false);
	Controls.NotificationSettingsPanel:SetHide(true);
	Controls.LogSelectHighlight:SetHide(false);
	Controls.SettingsSelectHighlight:SetHide(true);
	Controls.TurnOffAllButton:SetHide(true);
	Controls.TurnOnAllButton:SetHide(true);
end

function ShowSettingsTab()
	Controls.NotificationLogPanel:SetHide(true);
	Controls.NotificationSettingsPanel:SetHide(false);
	Controls.LogSelectHighlight:SetHide(true);
	Controls.SettingsSelectHighlight:SetHide(false);
	Controls.TurnOffAllButton:SetHide(false);
	Controls.TurnOnAllButton:SetHide(false);
	RefreshSettings();
end

Controls.TabButtonLog:RegisterCallback(Mouse.eLClick, ShowLogTab);
Controls.TabButtonSettings:RegisterCallback(Mouse.eLClick, ShowSettingsTab);

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_NOTIFICATION_LOG ) then
		return;
	end

	-- Set Civ Icon
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );

	m_PopupInfo = popupInfo;

	-- Show Log tab by default
	ShowLogTab();

	g_NotificationInstanceManager:ResetInstances();

	local player = Players[Game.GetActivePlayer()];
	local numNotifications = player:GetNumNotifications();
	for i = 0, numNotifications - 1
	do
		local str = player:GetNotificationStr((numNotifications - 1) - i);
		local index = player:GetNotificationIndex((numNotifications - 1) - i);
		local turn = player:GetNotificationTurn((numNotifications - 1) - i);
		local dismissed = player:GetNotificationDismissed((numNotifications - 1) - i);
		AddNotificationButton(index, str, turn, dismissed);
	end

	Controls.NotificationButtonStack:CalculateSize();
	Controls.NotificationButtonStack:ReprocessAnchoring();
	Controls.NotificationScrollPanel:CalculateInternalSize();

	if( m_PopupInfo.Data1 == 1 ) then
    	if( ContextPtr:IsHidden() == false ) then
    	    OnClose();
        else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
    	end
	else
    	UIManager:QueuePopup( ContextPtr, PopupPriority.NotificationLog );
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function AddNotificationButton( id, description, turn, dismissed )

	local controlTable = g_NotificationInstanceManager:GetInstance();
	controlTable.NotificationText:SetText(description);
	controlTable.NotificationTurnText:SetText(Locale.ConvertTextKey("TXT_KEY_TP_TURN_COUNTER", turn));
    controlTable.Button:SetVoid1( id ); -- indicates type
    controlTable.Button:SetVoid2( void );
    controlTable.Button:SetDisabled( dismissed );

    controlTable.TextStack:CalculateSize();
    controlTable.TextStack:ReprocessAnchoring();

    local sizeY = controlTable.TextStack:GetSizeY()
    controlTable.Button:SetSizeY(sizeY);
    controlTable.TextAnim:SetSizeY(sizeY);
    controlTable.TextHL:SetSizeY(sizeY);

    controlTable.Button:RegisterCallback( Mouse.eLClick, NotificationSelected );
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function NotificationSelected (id)
    OnClose();
	UI.ActivateNotification(id);
end

-------------------------------------------------------------------------------
-- Notification Settings Functions
-------------------------------------------------------------------------------
function OnTurnOffAll()
	local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
	for i = 0, InstantYieldType.NUM_INSTANT_YIELD_TYPES - 1 do
		if g_InstantYieldClassification[i] ~= "Unused" then
			pPlayer:SetInstantYieldNotificationDisabled(i, true);
		end
	end
	RefreshSettings()
end

function OnTurnOnAll()
	local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];
	for i = 0, InstantYieldType.NUM_INSTANT_YIELD_TYPES - 1 do
		if g_InstantYieldClassification[i] ~= "Unused" then
			pPlayer:SetInstantYieldNotificationDisabled(i, false);
		end
	end
	RefreshSettings()
end

Controls.TurnOffAllButton:RegisterCallback(Mouse.eLClick, OnTurnOffAll);
Controls.TurnOnAllButton:RegisterCallback(Mouse.eLClick, OnTurnOnAll);

function ToggleInstantYieldSetting(checkbox, pPlayer, Type)
	local NotificationDisabled;
    if checkbox:IsChecked() then
        NotificationDisabled = false;
	else
		NotificationDisabled = true;
	end
	pPlayer:SetInstantYieldNotificationDisabled(Type, NotificationDisabled);
end

function ToggleStack( stacknumber )
    if( stacknumber == 0 ) then
        if( Controls.InstantYieldCityStack:IsHidden() ) then
            Controls.InstantYieldCityStack:SetHide( false );
            Controls.InstantYieldCityButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CITY_BUTTON" ) );
        else
            Controls.InstantYieldCityStack:SetHide( true );
            Controls.InstantYieldCityButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CITY_BUTTON" ) );
        end
    elseif( stacknumber == 1 ) then
        if( Controls.InstantYieldMilitaryStack:IsHidden() ) then
            Controls.InstantYieldMilitaryStack:SetHide( false );
            Controls.InstantYieldMilitaryButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MILITARY_BUTTON" ) );
        else
            Controls.InstantYieldMilitaryStack:SetHide( true );
            Controls.InstantYieldMilitaryButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MILITARY_BUTTON" ) );
        end
    elseif( stacknumber == 2 ) then
        if( Controls.InstantYieldCivilianStack:IsHidden() ) then
            Controls.InstantYieldCivilianStack:SetHide( false );
            Controls.InstantYieldCivilianButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CIVILIAN_BUTTON" ) );
        else
            Controls.InstantYieldCivilianStack:SetHide( true );
            Controls.InstantYieldCivilianButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_CIVILIAN_BUTTON" ) );
        end
    elseif( stacknumber == 3 ) then
        if( Controls.InstantYieldSpiesStack:IsHidden() ) then
            Controls.InstantYieldSpiesStack:SetHide( false );
            Controls.InstantYieldSpiesButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_SPIES_BUTTON" ) );
        else
            Controls.InstantYieldSpiesStack:SetHide( true );
            Controls.InstantYieldSpiesButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_SPIES_BUTTON" ) );
        end
    elseif( stacknumber == 4 ) then
        if( Controls.InstantYieldMiscStack:IsHidden() ) then
            Controls.InstantYieldMiscStack:SetHide( false );
            Controls.InstantYieldMiscButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MISC_BUTTON" ) );
        else
            Controls.InstantYieldMiscStack:SetHide( true );
            Controls.InstantYieldMiscButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_NOTIFICATION_SETTINGS_MISC_BUTTON" ) );
        end
    end
end

Controls.InstantYieldCityButton:SetVoid1( 0 );
Controls.InstantYieldCityButton:RegisterCallback( Mouse.eLClick, ToggleStack );
Controls.InstantYieldMilitaryButton:SetVoid1( 1 );
Controls.InstantYieldMilitaryButton:RegisterCallback( Mouse.eLClick, ToggleStack );
Controls.InstantYieldCivilianButton:SetVoid1( 2 );
Controls.InstantYieldCivilianButton:RegisterCallback( Mouse.eLClick, ToggleStack );
Controls.InstantYieldSpiesButton:SetVoid1( 3 );
Controls.InstantYieldSpiesButton:RegisterCallback( Mouse.eLClick, ToggleStack );
Controls.InstantYieldMiscButton:SetVoid1( 4 );
Controls.InstantYieldMiscButton:RegisterCallback( Mouse.eLClick, ToggleStack );

function RefreshSettings()
	local iPlayer = Game.GetActivePlayer();
    local pPlayer = Players[ iPlayer ];

	Controls.InstantYieldCityStack:DestroyAllChildren();
    Controls.InstantYieldMilitaryStack:DestroyAllChildren();
    Controls.InstantYieldCivilianStack:DestroyAllChildren();
    Controls.InstantYieldSpiesStack:DestroyAllChildren();
    Controls.InstantYieldMiscStack:DestroyAllChildren();

	for i,v in ipairs(g_InstantYieldCitySorted) do
		local entry = {};
		ContextPtr:BuildInstanceForControl( "InstantYieldsInstance", entry, Controls.InstantYieldCityStack );
		entry.InstantYieldName:SetText(v.Name);
		entry.InstantYieldCheckbox:SetCheck(not pPlayer:IsInstantYieldNotificationDisabled(v.Index));
		entry.InstantYieldCheckbox:RegisterCheckHandler( function() ToggleInstantYieldSetting(entry.InstantYieldCheckbox, pPlayer, v.Index) end );
	end
	for i,v in ipairs(g_InstantYieldMilitarySorted) do
		local entry = {};
		ContextPtr:BuildInstanceForControl( "InstantYieldsInstance", entry, Controls.InstantYieldMilitaryStack );
		entry.InstantYieldName:SetText(v.Name);
		entry.InstantYieldCheckbox:SetCheck(not pPlayer:IsInstantYieldNotificationDisabled(v.Index));
		entry.InstantYieldCheckbox:RegisterCheckHandler( function() ToggleInstantYieldSetting(entry.InstantYieldCheckbox, pPlayer, v.Index) end );
	end
	for i,v in ipairs(g_InstantYieldCivilianSorted) do
		local entry = {};
		ContextPtr:BuildInstanceForControl( "InstantYieldsInstance", entry, Controls.InstantYieldCivilianStack);
		entry.InstantYieldName:SetText(v.Name);
		entry.InstantYieldCheckbox:SetCheck(not pPlayer:IsInstantYieldNotificationDisabled(v.Index));
		entry.InstantYieldCheckbox:RegisterCheckHandler( function() ToggleInstantYieldSetting(entry.InstantYieldCheckbox, pPlayer, v.Index) end );
	end
	for i,v in ipairs(g_InstantYieldSpiesSorted) do
		local entry = {};
		ContextPtr:BuildInstanceForControl( "InstantYieldsInstance", entry, Controls.InstantYieldSpiesStack );
		entry.InstantYieldName:SetText(v.Name);
		entry.InstantYieldCheckbox:SetCheck(not pPlayer:IsInstantYieldNotificationDisabled(v.Index));
		entry.InstantYieldCheckbox:RegisterCheckHandler( function() ToggleInstantYieldSetting(entry.InstantYieldCheckbox, pPlayer, v.Index) end );
	end
	for i,v in ipairs(g_InstantYieldMiscSorted) do
		entry = {};
		ContextPtr:BuildInstanceForControl( "InstantYieldsInstance", entry, Controls.InstantYieldMiscStack );
		entry.InstantYieldName:SetText(v.Name);
		entry.InstantYieldCheckbox:SetCheck(not pPlayer:IsInstantYieldNotificationDisabled(v.Index));
		entry.InstantYieldCheckbox:RegisterCheckHandler( function() ToggleInstantYieldSetting(entry.InstantYieldCheckbox, pPlayer, v.Index) end );
	end

	Controls.InstantYieldCityStack:CalculateSize();
    Controls.InstantYieldMilitaryStack:CalculateSize();
    Controls.InstantYieldCivilianStack:CalculateSize();
    Controls.InstantYieldSpiesStack:CalculateSize();
    Controls.InstantYieldMiscStack:CalculateSize();

	Controls.AllInstantYieldsStack:CalculateSize();
	Controls.AllInstantYieldsStack:ReprocessAnchoring();
	Controls.AllInstantYieldsScrollPanel:CalculateInternalSize();
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_NOTIFICATION_LOG, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);
