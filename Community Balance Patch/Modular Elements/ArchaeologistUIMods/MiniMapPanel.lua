print("This is the modded MiniMapPanel from 'UI - Overlay Antiquities'")

----------------------------------------------------------------        
----------------------------------------------------------------        
include( "InstanceManager" );

local g_LegendIM = InstanceManager:new( "LegendKey", "Item", Controls.LegendStack );
local g_Overlays = GetStrategicViewOverlays();
local g_IconModes = GetStrategicViewIconSettings();


----------------------------------------------------------------        
----- MODS START
----------------------------------------------------------------        

--
-- The following LuaEvents are supported
--
--   LuaEvents.MiniMapOverlayAddin.Add(function OnMiniMapOverlayAddin(tab) ... end)
--     adds a new tab (or replaces a standard one) in the Overlay drop-down
--     tab is an associative array with the following entries
--       text - the string (or TXT_KEY_) to display in the overlay drop-down
--       group - the name of the group to place the entry in (see g_OverlayIndexes below for valid group names)
--       call - the method to call when the entry is selected/deselected (receives a single bShow parameter)
--       nav - the method to call when the next/previous legend buttons are pressed (omit or set to nil to disable the buttons)
--       next_tt - the tooltip string (or TXT_KEY_) for the next button
--       prev_tt - the tooltip string (or TXT_KEY_) for the previous button
--
--   LuaEvents.MiniMapOverlayLegend.Add(function PopulateLegend(title, info) ... end)
--     sets the legend panel title and defines the boxes to draw in the legend key
--     title - the string (or TXT_KEY_) to display at the top of the legend panel
--     info - a list of Name and Color entries to populate the legend key with
--
--   LuaEvents.MiniMapOverlayHideLegendNav.Add(function OnHideLegendNav(bHide) ... end)
--     hides/shows the legend next/previous buttons
--
--   LuaEvents.MiniMapOverlayResizeLegendOptions.Add(function OnResizeLegendOptions(sizeX, sizeY, offsetCallback) ... end)
--     resizes the options "space" in the Legend side panel
--     sizeX/sizeY - the dimensions of the space to allocate
--     offsetCallback - the method to call to receive the offsetX/offsetY location of the "hole"
--

local g_OverlayActive = -1; 
local g_OverlayIndexes = {none=1, culture=2, terrain=3, units=4, routes=5, resources=6, other=7};
-- To get the standard entries for the following list, in FireTuner(MiniMapPanel) execute
--   for i,v in ipairs(GetStrategicViewOverlays()) do print(i, v) end
local g_OverlayInfo = {
  {{text="TXT_KEY_SV_OVERLAY_NONE",             call=function(bShow) if bShow then OnOverlaySelected(1); end end}},
  {{text="TXT_KEY_SV_OVERLAY_CULTURE",          call=function(bShow) if bShow then OnOverlaySelected(2); end end}},
  {{text="TXT_KEY_SV_OVERLAY_PASSABLE_TERRAIN", call=function(bShow) if bShow then OnOverlaySelected(3); end end}},
  {{text="TXT_KEY_SV_OVERLAY_UNIT_CULTURE",     call=function(bShow) if bShow then OnOverlaySelected(4); end end},
   {text="TXT_KEY_SV_OVERLAY_UNIT_TYPE",        call=function(bShow) if bShow then OnOverlaySelected(5); end end},
   {text="TXT_KEY_SV_OVERLAY_UNIT_DOMAIN",      call=function(bShow) if bShow then OnOverlaySelected(6); end end},
   {text="TXT_KEY_SV_OVERLAY_UNIT_COMBAT_TYPE", call=function(bShow) if bShow then OnOverlaySelected(7); end end}},
  {{text="TXT_KEY_SV_OVERLAY_TRADE_ROUTES",     call=function(bShow) if bShow then OnOverlaySelected(8); end end}},
  {{text="TXT_KEY_SV_OVERLAY_RESOURCES",        call=function(bShow) if bShow then OnOverlaySelected(9); end end}},
  {} -- The (initially empty) catch-all "other" section
};
local g_OverlayCallbacks = {};


function OnMiniMapOverlayAddin(tab)
  local sGroup = tab.group or "other"
  local iIndex = g_OverlayIndexes[sGroup] or g_OverlayIndexes.other

  -- Allow for replacing a standard overlay
  local overlayGroup = g_OverlayInfo[iIndex]
  for i,_ in ipairs(overlayGroup) do
    if (overlayGroup[i].text == tab.text) then
      print(string.format("Replacing %s in group %s", Locale.ConvertTextKey(tab.text), sGroup))
      overlayGroup[i] = tab
      return
    end
  end

  print(string.format("Adding %s to group %s", Locale.ConvertTextKey(tab.text), sGroup))
  table.insert(overlayGroup, tab)
end
LuaEvents.MiniMapOverlayAddin.Add(OnMiniMapOverlayAddin)

minmapAddins = {}
-- handle MiniMapOverlayAddins
for addin in Modding.GetActivatedModEntryPoints("MiniMapOverlayAddin") do
	local addinFile = addin.File;
	local extension = Path.GetExtension(addinFile);
	local path = string.sub(addinFile, 1, #addinFile - #extension);
	ptr = ContextPtr:LoadNewContext(path)
	table.insert(minmapAddins, ptr)
end

----------------------------------------------------------------        
----- MODS END (more below)
----------------------------------------------------------------        

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnMinimapInfo( uiHandle, width, height, paddingX )
    Controls.Minimap:SetTextureHandle( uiHandle );
    Controls.Minimap:SetSizeVal( width, height );
end
Events.MinimapTextureBroadcastEvent.Add( OnMinimapInfo );
UI:RequestMinimapBroadcast();

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnMinimapClick( _, _, _, x, y )
    Events.MinimapClickedEvent( x, y );
end
Controls.Minimap:RegisterCallback( Mouse.eLClick, OnMinimapClick );

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnStrategicView()
	local bIsObserver = PreGame.GetSlotStatus( Game.GetActivePlayer() ) == SlotStatus.SS_OBSERVER;
	if (bIsObserver) then
		-- Observer gets to toggle the world view completely off.
		local eViewType = GetGameViewRenderType();
		if (eViewType == GameViewTypes.GAMEVIEW_NONE) then
			SetGameViewRenderType(GameViewTypes.GAMEVIEW_STANDARD);			
		else
			if (eViewType == GameViewTypes.GAMEVIEW_STANDARD) then
				SetGameViewRenderType(GameViewTypes.GAMEVIEW_STRATEGIC);
			else
				SetGameViewRenderType(GameViewTypes.GAMEVIEW_NONE);
			end
		end
	else
		ToggleStrategicView();
	end		
end
Controls.StrategicViewButton:RegisterCallback( Mouse.eLClick, OnStrategicView );

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnStrategicViewStateChanged(bStrategicView)
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
end
Events.StrategicViewStateChanged.Add( OnStrategicViewStateChanged );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnMapOptions()	
	Controls.OptionsPanel:SetHide( not Controls.OptionsPanel:IsHidden() );
	Controls.SideStack:CalculateSize();
	Controls.SideStack:ReprocessAnchoring();
	
	SetLegend();
    UpdateOptionsPanel();
end
Controls.MapOptionsButton:RegisterCallback( Mouse.eLClick, OnMapOptions );


----------------------------------------------------------------        
----------------------------------------------------------------        
g_MapOptionDefaults = nil;
local g_PerPlayerMapOptions = {};
----------------------------------------------------------------    
-- Get the map option defaults from the options manager    
----------------------------------------------------------------        
function GetMapOptionDefaults()
	if (g_MapOptionDefaults == nil) then
		-- Pull in the current setting the first time through
		g_MapOptionDefaults = {};
		g_MapOptionDefaults.ShowTrade = OptionsManager.GetShowTradeOn();
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
	Events.Event_ToggleTradeRouteDisplay(mapOptions.ShowTrade);
		
	LuaEvents.OnRecommendationCheckChanged(mapOptions.HideTileRecommendations);
	
	-- Because outside contexted will also want to access the settings, we have to push them back to the OptionsManager	
	if (PreGame.IsHotSeatGame()) then
		local bChanged = false;
		if (OptionsManager.GetGridOn() ~= mapOptions.ShowGrid) then
			OptionsManager.SetGridOn_Cached( mapOptions.ShowGrid );
			bChanged = true;
		end
		if (OptionsManager.GetShowTradeOn() ~= mapOptions.ShowTrade) then 
			OptionsManager.SetShowTradeOn_Cached( mapOptions.ShowTrade );
			bChanged = true;
		end
		if (OptionsManager.GetYieldOn() ~= mapOptions.ShowYield) then
			OptionsManager.SetYieldOn_Cached( mapOptions.ShowYield );
			bChanged = true;
		end
		if (OptionsManager.GetResourceOn() ~= mapOptions.ShowResources) then
			OptionsManager.SetResourceOn_Cached( mapOptions.ShowResources );
			bChanged = true;
		end
		if (OptionsManager.IsNoTileRecommendations() ~= mapOptions.HideTileRecommendations) then
			OptionsManager.SetNoTileRecommendations_Cached(mapOptions.HideTileRecommendations );
			bChanged = true;
		end
		-- We will tell the manager to not write them out
		if (bChanged) then 
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
	if (g_PerPlayerMapOptions[iPlayer] == nil) then
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

	Controls.ShowTrade:SetCheck( mapOptions.ShowTrade );
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

-------------------------------------------------
----- MODS START
-------------------------------------------------
--function PopulateOverlayPulldown( pullDown )
--
--	for i, text in pairs( g_Overlays ) do
--	    controlTable = {};
--        pullDown:BuildEntry( "InstanceOne", controlTable );
--
--        controlTable.Button:SetVoid1( i );
--        controlTable.Button:SetText( Locale.ConvertTextKey( text ) );
--   	end
--   	
--   	Controls.OverlayDropDown:GetButton():SetText(Locale.ConvertTextKey( g_Overlays[1] ));
--	
--	pullDown:CalculateInternals();
--    pullDown:RegisterSelectionCallback( OnOverlaySelected );
--end

function OnModularOverlay(id)
  -- Clear any existing overlay
  SetStrategicViewOverlay(1)

  -- Clear any existing highlights
  Events.ClearHexHighlights()

  -- Hide the legend options space
  Controls.LegendOptions:SetHide(true)

  -- Hide any previous legend options
  if (g_OverlayActive ~= -1) then
    g_OverlayCallbacks[g_OverlayActive].call(false)
  end

  -- Select this item
  Controls.OverlayDropDown:GetButton():SetText(Locale.ConvertTextKey(g_OverlayCallbacks[id].text));

  -- Show the nav buttons if required
  if (g_OverlayCallbacks[id].nav ~= nil) then
    Controls.NextItem:SetHide(false);
    Controls.PreviousItem:SetHide(false);

    Controls.NextItem:SetToolTipString(Locale.ConvertTextKey(g_OverlayCallbacks[id].next_tt));
    Controls.PreviousItem:SetToolTipString(Locale.ConvertTextKey(g_OverlayCallbacks[id].prev_tt));
  else
    Controls.NextItem:SetHide(true);
    Controls.PreviousItem:SetHide(true);
  end

  -- Display the required overlay
  g_OverlayCallbacks[id].call(true)

  g_OverlayActive = id;
end

function PopulateOverlayPulldown( pullDown )
  local i = 1
  g_OverlayCallbacks = {}

  -- process the "list of lists" into a single pull-down
  for _, group in ipairs(g_OverlayInfo) do
    for _, tab in ipairs(group) do
      local controlTable = {};
      pullDown:BuildEntry("InstanceOne", controlTable)

      controlTable.Button:LocalizeAndSetText(tab.text)
      controlTable.Button:LocalizeAndSetToolTip(tab.tip)
      controlTable.Button:SetVoid1(i)

      -- we only get an integer key for the selected item, so store a flattened list of callbacks
      g_OverlayCallbacks[i] = tab
      i = i + 1
    end
  end

  pullDown:GetButton():SetText(Locale.ConvertTextKey(g_OverlayInfo[1][1].text));

  pullDown:CalculateInternals()
  pullDown:RegisterSelectionCallback(OnModularOverlay)
end

function OnResizeLegendOptions(sizeX, sizeY, offsetCallback)
  Controls.LegendOptions:SetSizeVal(0, 0) -- controls don't redraw unless they actually change size!

  Controls.LegendOptions:SetHide(false)
  Controls.LegendOptions:SetSizeVal(sizeX, sizeY)
  Controls.LegendFrame:DoAutoSize()
  Controls.SideStack:CalculateSize()
  Controls.SideStack:ReprocessAnchoring()

  if (offsetCallback ~= nil) then
    local offsetX = Controls.SideStack:GetOffsetX() + Controls.OptionsPanel:GetSizeX() + 10
    local offsetY = Controls.SideStack:GetOffsetY() + 53
    offsetCallback(offsetX, offsetY)
  end
end
LuaEvents.MiniMapOverlayResizeLegendOptions.Add(OnResizeLegendOptions)

function OnHideLegendNav(bHide)
  Controls.NextItem:SetHide(bHide)
  Controls.PreviousItem:SetHide(bHide)
end
LuaEvents.MiniMapOverlayHideLegendNav.Add(OnHideLegendNav)

function OnNextItem()
  g_OverlayCallbacks[g_OverlayActive].nav(true)
end
Controls.NextItem:RegisterCallback(Mouse.eLClick, OnNextItem)

function OnPreviousItem()
  g_OverlayCallbacks[g_OverlayActive].nav(false)
end
Controls.PreviousItem:RegisterCallback(Mouse.eLClick, OnPreviousItem)

-------------------------------------------------
----- MODS END (more below)
-------------------------------------------------

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

----------------------------------------------------------------        
--- MODS START
----------------------------------------------------------------        
function SetLegend(index)
	local sTitle = nil;

	if (index ~= nil) then
		sTitle = g_Overlays[index];
	end

	-- Hide any previous legend options
	if (g_OverlayActive ~= -1 and g_OverlayActive ~= index) then
		g_OverlayCallbacks[g_OverlayActive].call(false)
	end

	PopulateLegend(sTitle, GetOverlayLegend());
end

function PopulateLegend(sTitle, info)
	if (sTitle ~= nil) then
		Controls.OverlayTitle:SetText(Locale.ConvertTextKey(sTitle));
	end

	g_LegendIM:ResetInstances();

	if (info ~= nil and InStrategicView()) then
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
LuaEvents.MiniMapOverlayLegend.Add(PopulateLegend)

----------------------------------------------------------------        
----- MODS END
----------------------------------------------------------------        

----------------------------------------------------------------        
----------------------------------------------------------------        
function PopulateIconPulldown( pullDown )
	
	for i, text in pairs(g_IconModes) do
	    controlTable = {};
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
function OnShowTradeToggled( bIsChecked )
	Controls.ShowTrade:SetCheck(bIsChecked);
	
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowTrade = bIsChecked;
end
Events.Event_ToggleTradeRouteDisplay.Add( OnShowTradeToggled )

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnShowTradeChecked( bIsChecked )
	Events.Event_ToggleTradeRouteDisplay(bIsChecked);
	
	local mapOptions = GetMapOptions(Game.GetActivePlayer());
	mapOptions.ShowTrade = bIsChecked;
    
	OptionsManager.SetShowTradeOn_Cached( bIsChecked );
	OptionsManager.CommitGameOptions( PreGame.IsHotSeatGame() );
end
Controls.ShowTrade:RegisterCheckHandler( OnShowTradeChecked );

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
	
    if( type == YieldDisplayTypes.USER_ALL_ON ) then
        Controls.ShowYield:SetCheck( true );
        mapOptions.ShowYield = true;
        
    elseif( type == YieldDisplayTypes.USER_ALL_OFF ) then
        Controls.ShowYield:SetCheck( false );
         mapOptions.ShowYield = false;
        
    elseif( type == YieldDisplayTypes.USER_ALL_RESOURCE_ON ) then
        Controls.ShowResources:SetCheck( true );
        mapOptions.ShowResources = true;
        
    elseif( type == YieldDisplayTypes.USER_ALL_RESOURCE_OFF ) then
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
	if( controlInfo ~= nil and type(controlInfo) == "table" ) then
		local hotKey = controlInfo.HotKey;
		if( hotKey ~= nil and type(hotKey) == "string" ) then
			local keyDesc = Locale.GetHotKeyDescription(hotKey, controlInfo.CtrlDown, controlInfo.AltDown, controlInfo.ShiftDown);
			if( keyDesc ~= nil and type(keyDesc) == "string" ) then
				local sToolTip = Locale.ConvertTextKey("TXT_KEY_POP_STRATEGIC_VIEW_TT");
				Controls.StrategicViewButton:SetToolTipString(sToolTip .. " (" .. keyDesc .. ")");
			end
		end
	end
end

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)

	local mapOptions = GetMapOptions(iActivePlayer);
	ApplyMapOptions(mapOptions);
	UpdateOptionsPanel();
	
	if (not Controls.OptionsPanel:IsHidden()) then
		OnMapOptions();
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);


PopulateOverlayPulldown( Controls.OverlayDropDown );
PopulateIconPulldown( Controls.IconDropDown );
UpdateOptionsPanel();
UpdateStrategicViewToggleTT();

