-- modified by bc1 from 1.0.3.144 code
-- allows minimap resizing
----------------------------------------------------------------
----------------------------------------------------------------
include( "InstanceManager" ); local GenerationalInstanceManager = GenerationalInstanceManager
local GenerationalInstanceManager = GenerationalInstanceManager

local gk_mode = Game.GetReligionName ~= nil;
local bnw_mode = Game.GetActiveLeague ~= nil;
local pairs = pairs

local ContextPtr = ContextPtr
local Controls = Controls
local Events = Events
local Game = Game
local GameInfo = GameInfo
local GameViewTypes = GameViewTypes
local GetGameViewRenderType = GetGameViewRenderType
local GetOverlayLegend = GetOverlayLegend
local GetStrategicViewIconSettings = GetStrategicViewIconSettings
local GetStrategicViewOverlays = GetStrategicViewOverlays
local InStrategicView = InStrategicView
local Locale = Locale
local LuaEvents = LuaEvents
local Mouse = Mouse
local OptionsManager = OptionsManager
local PreGame = PreGame
local SetGameViewRenderType = SetGameViewRenderType
local SetStrategicViewIconSetting = SetStrategicViewIconSetting
local SetStrategicViewOverlay = SetStrategicViewOverlay
local SlotStatus = SlotStatus
local StrategicViewShowFeatures = StrategicViewShowFeatures
local StrategicViewShowFogOfWar = StrategicViewShowFogOfWar
local ToggleStrategicView = ToggleStrategicView
local UI = UI
local YieldDisplayTypes = YieldDisplayTypes

local g_width, g_height;
local g_LegendIM = GenerationalInstanceManager:new( "LegendKey", "Item", Controls.LegendStack );
local g_Overlays = GetStrategicViewOverlays();
local g_IconModes = GetStrategicViewIconSettings();

----------------------------------------------------------------
----------------------------------------------------------------
Events.MinimapTextureBroadcastEvent.Add(
function( uiHandle, width, height )--, paddingX )
	Controls.Minimap:SetTextureHandle( uiHandle );
	if width ~= g_width or height ~= g_height then
		g_width, g_height = width, height;
		Controls.Minimap:SetSizeVal( width, height );
		Controls.MinimapPanel:SetSizeVal( width+35, height+87 );
		local EndTurnButton = ContextPtr:LookUpControl( "../ActionInfoPanel/EndTurnButton" );
		if EndTurnButton then
			EndTurnButton:SetOffsetY( height + 10 );
		end
		local OuterStack = ContextPtr:LookUpControl( "../ActionInfoPanel/NotificationPanel/OuterStack" );
		if OuterStack then
			OuterStack:SetOffsetY( height + 50 );
		end
		Controls.OuterStack:CalculateSize();
		Controls.OuterStack:ReprocessAnchoring();
	end
end)
UI:RequestMinimapBroadcast();

----------------------------------------------------------------
----------------------------------------------------------------
Controls.Minimap:RegisterCallback( Mouse.eLClick,
function( _, _, _, x, y )
	Events.MinimapClickedEvent( x, y );
end)

----------------------------------------------------------------
----------------------------------------------------------------
Controls.StrategicViewButton:RegisterCallback( Mouse.eLClick,
function()
	if PreGame.GetSlotStatus( Game.GetActivePlayer() ) == SlotStatus.SS_OBSERVER then
		-- Observer gets to toggle the world view completely off.
		local eViewType = GetGameViewRenderType();
		if eViewType == GameViewTypes.GAMEVIEW_NONE then
			SetGameViewRenderType(GameViewTypes.GAMEVIEW_STANDARD);
		else
			if eViewType == GameViewTypes.GAMEVIEW_STANDARD then
				SetGameViewRenderType(GameViewTypes.GAMEVIEW_STRATEGIC);
			else
				SetGameViewRenderType(GameViewTypes.GAMEVIEW_NONE);
			end
		end
	else
		ToggleStrategicView();
	end
end)

----------------------------------------------------------------
----------------------------------------------------------------
Events.StrategicViewStateChanged.Add(
function(bStrategicView)
	if bStrategicView then
		Controls.ShowResources:SetDisabled( true );
		Controls.ShowResources:SetAlpha( 0.5 );
		Controls.StrategicViewButton:SetTexture( "assets/UI/Art/Icons/MainWorldButton.dds" );
		Controls.StrategicMO:SetTexture( "assets/UI/Art/Icons/MainWorldButton.dds" );
		Controls.StrategicHL:SetTexture( "assets/UI/Art/Icons/MainWorldButtonHL.dds" );
	else
		Controls.ShowGrid:SetCheck(OptionsManager.GetGridOn());
		Controls.ShowResources:SetDisabled( false );
		Controls.ShowResources:SetAlpha( 1.0 );
		Controls.StrategicViewButton:SetTexture( "assets/UI/Art/Icons/MainStrategicButton.dds" );
		Controls.StrategicMO:SetTexture( "assets/UI/Art/Icons/MainStrategicButton.dds" );
		Controls.StrategicHL:SetTexture( "assets/UI/Art/Icons/MainStrategicButtonHL.dds" );
	end

	SetLegend();
	UpdateOptionsPanel();
end)


----------------------------------------------------------------
----------------------------------------------------------------
function OpenMapOptions()
	SetLegend();
	UpdateOptionsPanel();
	Controls.OptionsPanel:SetHide( false );
	Controls.SideStack:CalculateSize();
	Controls.SideStack:ReprocessAnchoring();
end

function CloseMapOptions()
	Controls.OptionsPanel:SetHide( true );
	Controls.SideStack:CalculateSize();
	Controls.SideStack:ReprocessAnchoring();
end

function ToggleMapOptions()
	if Controls.OptionsPanel:IsHidden() then
		OpenMapOptions()
	else
		CloseMapOptions()
	end
end
--TODO open on mouseover / close on mouseexit
--Controls.MapOptionsButton:RegisterCallback( Mouse.eMouseEnter, OpenMapOptions );
--Controls.MapOptionsButton:RegisterCallback( Mouse.eMouseExit, CloseMapOptions );
Controls.MapOptionsButton:RegisterCallback( Mouse.eLClick, ToggleMapOptions );

----------------------------------------------------------------
----------------------------------------------------------------
local g_MapOptionDefaults;
local g_PerPlayerMapOptions = {};
----------------------------------------------------------------
-- Get the map option defaults from the options manager
----------------------------------------------------------------
function GetMapOptionDefaults()
	if g_MapOptionDefaults == nil then
		-- Pull in the current setting the first time through
		g_MapOptionDefaults = {};
		if bnw_mode then
			g_MapOptionDefaults.ShowTrade = OptionsManager.GetShowTradeOn();
		end
		g_MapOptionDefaults.ShowGrid = OptionsManager.GetGridOn();
		g_MapOptionDefaults.ShowYield = OptionsManager.GetYieldOn();
		g_MapOptionDefaults.ShowResources = OptionsManager.GetResourceOn();
		g_MapOptionDefaults.HideTileRecommendations = OptionsManager.IsNoTileRecommendations();

		g_MapOptionDefaults.SVIconMode = 1;
		g_MapOptionDefaults.SVOverlayMode = 1;
		g_MapOptionDefaults.SVShowFeatures = true;
		g_MapOptionDefaults.SVShowFOW = true;
	end

	local mapOptions = {};
	for k, v in pairs(g_MapOptionDefaults) do mapOptions[k] = v; end

	return mapOptions;
end

----------------------------------------------------------------
-- Take the current options and apply them to the game
----------------------------------------------------------------
function ApplyMapOptions(mapOptions)

	UI.SetGridVisibleMode( mapOptions.ShowGrid );
	UI.SetYieldVisibleMode( mapOptions.ShowYield );
	UI.SetResourceVisibleMode( mapOptions.ShowResources );
	if bnw_mode then
		Events.Event_ToggleTradeRouteDisplay(mapOptions.ShowTrade);
	end
	LuaEvents.OnRecommendationCheckChanged(mapOptions.HideTileRecommendations);

	-- Because outside contexted will also want to access the settings, we have to push them back to the OptionsManager
	if PreGame.IsHotSeatGame() then
		local bChanged = false;

		if OptionsManager.GetGridOn() ~= mapOptions.ShowGrid then
			OptionsManager.SetGridOn_Cached( mapOptions.ShowGrid );
			bChanged = true;
		end
		if bnw_mode and (OptionsManager.GetShowTradeOn() ~= mapOptions.ShowTrade) then
			OptionsManager.SetShowTradeOn_Cached( mapOptions.ShowTrade );
			bChanged = true;
		end
		if OptionsManager.GetYieldOn() ~= mapOptions.ShowYield then
			OptionsManager.SetYieldOn_Cached( mapOptions.ShowYield );
			bChanged = true;
		end
		if OptionsManager.GetResourceOn() ~= mapOptions.ShowResources then
			OptionsManager.SetResourceOn_Cached( mapOptions.ShowResources );
			bChanged = true;
		end
		if OptionsManager.IsNoTileRecommendations() ~= mapOptions.HideTileRecommendations then
			OptionsManager.SetNoTileRecommendations_Cached(mapOptions.HideTileRecommendations );
			bChanged = true;
		end
		-- We will tell the manager to not write them out
		if bChanged then
			OptionsManager.CommitGameOptions(true);
		end
	end

	StrategicViewShowFeatures( mapOptions.SVShowFeatures );
	StrategicViewShowFogOfWar( mapOptions.SVShowFOW );
	SetStrategicViewIconSetting( mapOptions.SVIconMode );
	SetStrategicViewOverlay( mapOptions.SVOverlayMode );
end

----------------------------------------------------------------
-- Store the current options to the specified player's slot
----------------------------------------------------------------
function StoreMapOptions(iPlayer, mapOptions)
	g_PerPlayerMapOptions[iPlayer] = mapOptions;
end

----------------------------------------------------------------
----------------------------------------------------------------
function GetMapOptions(iPlayer)

	local mapOptions;

	-- Get the options from the local player cache
	if g_PerPlayerMapOptions[iPlayer] == nil then
		-- Initialize with the defaults
		mapOptions = GetMapOptionDefaults();
		StoreMapOptions(iPlayer, mapOptions);
	else
		mapOptions = g_PerPlayerMapOptions[iPlayer];
	end

	return mapOptions;
end

----------------------------------------------------------------
----------------------------------------------------------------
function UpdateOptionsPanel()

	local mapOptions = GetMapOptions(Game.GetActivePlayer());

	if bnw_mode then
		Controls.ShowTrade:SetCheck( mapOptions.ShowTrade );
	end
	Controls.ShowGrid:SetCheck( mapOptions.ShowGrid );
	Controls.ShowYield:SetCheck( mapOptions.ShowYield );
	Controls.ShowResources:SetCheck( mapOptions.ShowResources );
	Controls.HideRecommendation:SetCheck( mapOptions.HideTileRecommendations );
	Controls.StrategicStack:SetHide( not InStrategicView() );

	Controls.ShowFeatures:SetCheck( mapOptions.SVShowFeatures );
	Controls.ShowFogOfWar:SetCheck( mapOptions.SVShowFOW );

	Controls.OverlayDropDown:GetButton():SetText(Locale.ConvertTextKey( g_Overlays[mapOptions.SVOverlayMode] ));
	SetLegend(mapOptions.SVOverlayMode);

	Controls.IconDropDown:GetButton():SetText( Locale.ConvertTextKey( g_IconModes[mapOptions.SVIconMode] ));

	Controls.StrategicStack:CalculateSize();
	Controls.MainStack:CalculateSize();
	Controls.OptionsPanel:DoAutoSize();

	Controls.SideStack:CalculateSize();
	Controls.SideStack:ReprocessAnchoring();
end
Events.GameOptionsChanged.Add(UpdateOptionsPanel);

----------------------------------------------------------------
----------------------------------------------------------------
function PopulateOverlayPulldown( pullDown )

	for i, text in pairs( g_Overlays ) do
		local controlTable = {};
		pullDown:BuildEntry( "InstanceOne", controlTable );

		controlTable.Button:SetVoid1( i );
		controlTable.Button:SetText( Locale.ConvertTextKey( text ) );
	end

	Controls.OverlayDropDown:GetButton():SetText(Locale.ConvertTextKey( g_Overlays[1] ));

	pullDown:CalculateInternals();
	pullDown:RegisterSelectionCallback( OnOverlaySelected );
end


----------------------------------------------------------------
----------------------------------------------------------------
function OnOverlaySelected( index )
	SetStrategicViewOverlay(index);

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.SVOverlayMode = index;

	Controls.OverlayDropDown:GetButton():SetText( Locale.ConvertTextKey( g_Overlays[index] ));
	SetLegend(index);
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetLegend(index)
	g_LegendIM:ResetInstances();

	local info = GetOverlayLegend();
	if index then
		Controls.OverlayTitle:SetText(Locale.ConvertTextKey( g_Overlays[index] ));
	end

	if info and InStrategicView() then
		for i, v in pairs(info) do
			local controlTable = g_LegendIM:GetInstance();

			local keyColor = { x = v.Color.R, y = v.Color.G, z = v.Color.B, w = 1 };
			controlTable.KeyColor:SetColor(keyColor);
			controlTable.KeyName:LocalizeAndSetText(v.Name);
		end

		Controls.LegendStack:CalculateSize();
		Controls.LegendStack:ReprocessAnchoring();

		Controls.LegendFrame:SetHide(false);
		Controls.LegendFrame:DoAutoSize();

		Controls.SideStack:CalculateSize();
		Controls.SideStack:ReprocessAnchoring();
	else
		Controls.LegendFrame:SetHide(true);
		Controls.SideStack:CalculateSize();
		Controls.SideStack:ReprocessAnchoring();
	end

end

----------------------------------------------------------------
----------------------------------------------------------------
function PopulateIconPulldown( pullDown )

	for i, text in pairs(g_IconModes) do
		local controlTable = {};
		pullDown:BuildEntry( "InstanceOne", controlTable );

		controlTable.Button:SetVoid1( i );
		controlTable.Button:SetText( Locale.ConvertTextKey( text ) );
	end

	Controls.IconDropDown:GetButton():SetText(Locale.ConvertTextKey( g_IconModes[1] ));

	pullDown:CalculateInternals();
	pullDown:RegisterSelectionCallback( OnIconModeSelected );
end


----------------------------------------------------------------
----------------------------------------------------------------
function OnIconModeSelected( index )
	SetStrategicViewIconSetting(index);

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.SVIconMode = index;

	Controls.IconDropDown:GetButton():SetText( Locale.ConvertTextKey( g_IconModes[index] ));
end


----------------------------------------------------------------
----------------------------------------------------------------
function OnShowFeaturesChecked( bIsChecked )
	StrategicViewShowFeatures( bIsChecked );
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.SVShowFeatures = bIsChecked;
end
Controls.ShowFeatures:RegisterCheckHandler( OnShowFeaturesChecked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnShowFOWChecked( bIsChecked )
	StrategicViewShowFogOfWar( bIsChecked );
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.SVShowFOW = bIsChecked;
end
Controls.ShowFogOfWar:RegisterCheckHandler( OnShowFOWChecked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnShowGridChecked( bIsChecked )

	UI.SetGridVisibleMode(bIsChecked);
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowGrid = bIsChecked;

	OptionsManager.SetGridOn_Cached( bIsChecked );
	OptionsManager.CommitGameOptions( PreGame.IsHotSeatGame() );

end
Controls.ShowGrid:RegisterCheckHandler( OnShowGridChecked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnGridOn()
	Controls.ShowGrid:SetCheck( true );

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowGrid = true;

end
Events.SerialEventHexGridOn.Add( OnGridOn )


----------------------------------------------------------------
----------------------------------------------------------------
function OnGridOff()
	Controls.ShowGrid:SetCheck( false );

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowGrid = false;

end
Events.SerialEventHexGridOff.Add( OnGridOff )

----------------------------------------------------------------
----------------------------------------------------------------

if bnw_mode then -----------------------------------------------

function OnShowTradeToggled( bIsChecked )
	Controls.ShowTrade:SetCheck(bIsChecked);

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowTrade = bIsChecked;
end
Events.Event_ToggleTradeRouteDisplay.Add( OnShowTradeToggled )

function OnShowTradeChecked( bIsChecked )
	Events.Event_ToggleTradeRouteDisplay(bIsChecked);

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowTrade = bIsChecked;

	OptionsManager.SetShowTradeOn_Cached( bIsChecked );
	OptionsManager.CommitGameOptions( PreGame.IsHotSeatGame() );
end
Controls.ShowTrade:RegisterCheckHandler( OnShowTradeChecked );

end -- bnw_mode ------------------------------------------------

----------------------------------------------------------------
----------------------------------------------------------------
function OnYieldChecked( bIsChecked )
	UI.SetYieldVisibleMode(bIsChecked);
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowYield = bIsChecked;

	OptionsManager.SetYieldOn_Cached( bIsChecked );
	OptionsManager.CommitGameOptions( PreGame.IsHotSeatGame() );
end
Controls.ShowYield:RegisterCheckHandler( OnYieldChecked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnResourcesChecked( bIsChecked )
	UI.SetResourceVisibleMode(bIsChecked);
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowResources = bIsChecked;

	OptionsManager.SetResourceOn_Cached( bIsChecked );
	OptionsManager.CommitGameOptions(PreGame.IsHotSeatGame());

end
Controls.ShowResources:RegisterCheckHandler( OnResourcesChecked );


----------------------------------------------------------------
----------------------------------------------------------------
function OnYieldDisplay( type )

	local mapOptions = GetMapOptions(Game.GetActivePlayer());

	if type == YieldDisplayTypes.USER_ALL_ON then
		Controls.ShowYield:SetCheck( true );
		mapOptions.ShowYield = true;

	elseif type == YieldDisplayTypes.USER_ALL_OFF then
		Controls.ShowYield:SetCheck( false );
		mapOptions.ShowYield = false;

	elseif type == YieldDisplayTypes.USER_ALL_RESOURCE_ON then
		Controls.ShowResources:SetCheck( true );
		mapOptions.ShowResources = true;

	elseif type == YieldDisplayTypes.USER_ALL_RESOURCE_OFF then
		Controls.ShowResources:SetCheck( false );
		mapOptions.ShowResources = false;
	end

end
Events.RequestYieldDisplay.Add( OnYieldDisplay );


----------------------------------------------------------------
----------------------------------------------------------------
function OnRecommendationChecked( bIsChecked )

	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.HideTileRecommendations = bIsChecked;

	OptionsManager.SetNoTileRecommendations_Cached( bIsChecked );
	OptionsManager.CommitGameOptions(PreGame.IsHotSeatGame());
	LuaEvents.OnRecommendationCheckChanged( bIsChecked );
end
Controls.HideRecommendation:RegisterCheckHandler( OnRecommendationChecked );

----------------------------------------------------------------
----------------------------------------------------------------
function UpdateStrategicViewToggleTT()
	local controlInfo = GameInfo.Controls.CONTROL_TOGGLE_STRATEGIC_VIEW;
	if type(controlInfo) == "table" then
		local hotKey = controlInfo.HotKey;
		if type(hotKey) == "string" then
			local keyDesc = Locale.GetHotKeyDescription(hotKey, controlInfo.CtrlDown, controlInfo.AltDown, controlInfo.ShiftDown);
			if type(keyDesc) == "string" then
				local sToolTip = Locale.ConvertTextKey("TXT_KEY_POP_STRATEGIC_VIEW_TT");
				Controls.StrategicViewButton:SetToolTipString(sToolTip .. " (" .. keyDesc .. ")");
			end
		end
	end
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged( iActivePlayer )--, iPrevActivePlayer )

	local mapOptions = GetMapOptions(iActivePlayer);
	ApplyMapOptions(mapOptions);
	UpdateOptionsPanel();
	CloseMapOptions();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

PopulateOverlayPulldown( Controls.OverlayDropDown );
PopulateIconPulldown( Controls.IconDropDown );
UpdateOptionsPanel();
UpdateStrategicViewToggleTT();
