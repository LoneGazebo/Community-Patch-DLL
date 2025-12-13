if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "MPGameDefaults" );

local bIsModding = false;
local m_bIsInStagingRoom = false;

g_GameOptionsManager = InstanceManager:new("GameOptionInstance", "GameOptionRoot", Controls.GameOptionsStack);
g_DropDownOptionsManager = InstanceManager:new("DropDownOptionInstance", "DropDownOptionRoot", Controls.DropDownOptionsStack);
g_VictoryCondtionsManager = InstanceManager:new("GameOptionInstance", "GameOptionRoot", Controls.VictoryConditionsStack);
g_DLCAllowedManager = InstanceManager:new("GameOptionInstance", "GameOptionRoot", Controls.DLCAllowedStack);


-- Global Variables
g_MapEntries = {};

-- Turn Mode Pulldown
local g_turnModeOptions = { "TXT_KEY_GAME_OPTION_SIMULTANEOUS_TURNS",  
														"TXT_KEY_GAME_OPTION_SEQUENTIAL_TURNS",
														"TXT_KEY_GAME_OPTION_DYNAMIC_TURNS" }
																						
local g_turnModeData = {};
g_turnModeData["TXT_KEY_GAME_OPTION_SIMULTANEOUS_TURNS"]	= { tooltip = "TXT_KEY_GAME_OPTION_SIMULTANEOUS_TURNS_HELP",	index=0 };
g_turnModeData["TXT_KEY_GAME_OPTION_SEQUENTIAL_TURNS"]		= { tooltip = "TXT_KEY_GAME_OPTION_SEQUENTIAL_TURNS_HELP",		index=1 };
g_turnModeData["TXT_KEY_GAME_OPTION_DYNAMIC_TURNS"]				= { tooltip = "TXT_KEY_GAME_OPTION_DYNAMIC_TURNS_HELP",				index=2 };

local hoursStr = Locale.ConvertTextKey( "TXT_KEY_HOURS" );
local secondsStr = Locale.ConvertTextKey( "TXT_KEY_SECONDS" );

-------------------------------------------------
-- Return true if the play is in an active lobby
-------------------------------------------------
function IsInLobby()
	if ( PreGame.IsHotSeatGame() ) then		
		return m_bIsInStagingRoom;
	end
	
	return Matchmaking.GetLocalID() ~= -1;
end

-------------------------------------------------
-- Set if the user is in an active lobby
-------------------------------------------------
function SetInLobby(bState)
	if ( PreGame.IsHotSeatGame() ) then		
		m_bIsInStagingRoom = bState;
	end
	
	m_bIsInStagingRoom = (Matchmaking.GetLocalID() ~= -1);
end

-------------------------------------------------
-------------------------------------------------
function GetTurnModeStr()
	if(PreGame.GetGameOption("GAMEOPTION_DYNAMIC_TURNS") == 1) then
		return "TXT_KEY_GAME_OPTION_DYNAMIC_TURNS";
	elseif(PreGame.GetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS") == 1) then
		return "TXT_KEY_GAME_OPTION_SIMULTANEOUS_TURNS";
	else
		return "TXT_KEY_GAME_OPTION_SEQUENTIAL_TURNS";
	end
end

function GetTurnModeToolTipStr(turnModeStr)
	return g_turnModeData[turnModeStr].tooltip;
end

-------------------------------------------------
-------------------------------------------------
function CanEditGameOptions()
	return (Matchmaking.IsHost() and PreGame.GetLoadFileName() == "") or PreGame.IsHotSeatGame() or Matchmaking.GetLocalID() == -1;
end

-------------------------------------------------
-------------------------------------------------
function CanEditDLCGameOptions()
	-- Can only change DLC outside the staging room
	return (PreGame.GetLoadFileName() == "" and Matchmaking.GetLocalID() == -1) or PreGame.IsHotSeatGame();
end

-------------------------------------------------
-------------------------------------------------
function GetMaxPlayersForCurrentMap()

	if ( not PreGame.IsRandomMapScript() ) then 
		
		local mapScriptFileName = Locale.ToLower(PreGame.GetMapScript());
		for i, mapEntry in ipairs(g_MapEntries) do
			if(Locale.ToLower(mapEntry.FileName) == mapScriptFileName) then
				if(mapEntry.WBMapData ~= nil) then
					local wb = mapEntry.WBMapData;
										
					local numPlayers = wb.PlayerCount;
					if (numPlayers == 0) then
						numPlayers = GameInfo.Worlds[wb.MapSize].DefaultPlayers;
					end		    	
					
					return numPlayers;
				end
			end
		end					
	end
	
	if (PreGame.GetWorldSize() >= 0) then
		return GameInfo.Worlds[ PreGame.GetWorldSize() ].DefaultPlayers;
	end
	
	return 8;
end
-------------------------------------------------
-------------------------------------------------
function OnSenarioCheck()
	PreGame.SetLoadWBScenario(not PreGame.GetLoadWBScenario());
	if(PreGame.GetLoadWBScenario()) then
--		if(Controls.PrivateGameCheckbox:IsChecked()) then
			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_SCENARIO" );
--		else
--			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_FIND_SCENARIO" );
--		end
		
		local mapScriptFileName = PreGame.GetMapScript();
		if(IsWBMap(mapScriptFileName)) then 
			SetupForScenarioMap(mapScriptFileName);
		end
	else
--		if(Controls.PrivateGameCheckbox:IsChecked()) then
			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_GAME" );
--		else
--			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_FIND_GAME" );
--		end
	end
end
Controls.ScenarioCheck:RegisterCallback( Mouse.eLClick, OnSenarioCheck );

-------------------------------------------------
-- Update the May Type entry
-------------------------------------------------
function RefreshMapTypeDisplay()

	local pullDown = Controls.MapTypePullDown;
		
	if( not PreGame.IsRandomMapScript() ) then 
		local bFound = false;
		local mapScriptFileName = Locale.ToLower(Path.GetFileNameWithoutExtension(PreGame.GetMapScript()));
		
		for i, mapEntry in ipairs(g_MapEntries) do
			if(mapEntry.FileName ~= nil) then
				if(Locale.ToLower(Path.GetFileNameWithoutExtension(mapEntry.FileName)) == mapScriptFileName) then
					pullDown:GetButton():SetText( mapEntry.Name );
					pullDown:GetButton():SetToolTipString( mapEntry.Description or "" );
			    	bFound = true;
					break;
				end
			end
		end
		
		if(not bFound) then
		
			for _, map in ipairs(Modding.GetMapFiles()) do
		
				if(Locale.ToLower(Path.GetFileNameWithoutExtension(map.File)) == mapScriptFileName) then
					for row in GameInfo.Map_Sizes() do
						if(Locale.ToLower(Path.GetFileNameWithoutExtension(row.FileName)) == mapScriptFileName) then
							local mapEntry = GameInfo.Maps[row.MapType];
							pullDown:GetButton():LocalizeAndSetText(mapEntry.Name);
							pullDown:GetButton():LocalizeAndSetToolTip(mapEntry.Description);
							bFound = true;
							break;
						end
					end
					
					if(not bFound) then
						local mapData = UI.GetMapPreview(map.File);
						local name = "";
						local description = "";
					
						if(not Locale.IsNilOrWhitespace(map.Name)) then
							name = map.Name;
						elseif(not Locale.IsNilOrWhitespace(mapData.Name)) then
							name = mapData.Name;
						else
							name = Path.GetFileNameWithoutExtension(map.File);
						end
						
						if(not Locale.IsNilOrWhitespace(map.Description)) then
							description = map.Description;
						elseif(mapData.Description and #mapData.Description > 0) then
							description = mapData.Description;
						end
					
						pullDown:GetButton():LocalizeAndSetText(name);
						pullDown:GetButton():LocalizeAndSetToolTip(description);
						bFound = true;
					end
					
					break;
				end
			end
		end
        
        if (not bFound) then
			if(Matchmaking.GetLocalID() == -1 or Matchmaking.IsHost()) then
				-- Must pick a valid map script, if we are the host or we are not online yet
				PreGame.SetRandomMapScript(false);

				-- Default the the first in the list
				local foundMapEntry = nil;
				if (table.count(g_MapEntries) > 0) then
					foundMapEntry = g_MapEntries[1];
				end
				
				-- See if the continents is available though, that one is a better default
				for i, mapEntry in ipairs(g_MapEntries) do
					if(string.find(Locale.ToLower(mapEntry.FileName), "continents") ~= nil) then
						foundMapEntry = mapEntry;
						break;
					end
				end
								
				-- Find anything?
				if (foundMapEntry ~= nil) then
					PreGame.SetMapScript(foundMapEntry.FileName);
								
					pullDown:GetButton():SetText( foundMapEntry.Name );
					pullDown:GetButton():SetToolTipString( foundMapEntry.Description or "" );
				end
			else
				-- print("Cannot get info for map or map script - " .. tostring(mapScriptFileName));
				local fileTitle = Path.GetFileName(PreGame.GetMapScript());
				if (fileTitle ~= nil) then
					pullDown:GetButton():SetText("[COLOR_RED]" .. fileTitle .. "[ENDCOLOR]");
					pullDown:GetButton():LocalizeAndSetToolTip("TXT_KEY_FILE_INFO_NOT_FOUND");
				end
			end
		end
	end
	
	if(PreGame.IsRandomMapScript()) then
		pullDown:GetButton():LocalizeAndSetText("TXT_KEY_RANDOM_MAP_SCRIPT");
		pullDown:GetButton():LocalizeAndSetToolTip("TXT_KEY_RANDOM_MAP_SCRIPT_HELP");
	end	
end

-------------------------------------------------
-------------------------------------------------
function SendGameOptionChanged()
	if Matchmaking.IsHost() then
		Network.BroadcastGameSettings();
	end	
end

-------------------------------------------------
-- Make sure the civ selections have valid civs at their locations.
-------------------------------------------------
function ValidateCivSelections()
	for i = 0, GameDefines.MAX_MAJOR_CIVS-1 do
		local civIndex = PreGame.GetCivilization(i);
		if(civIndex ~= -1) then
			if(GameInfo.Civilizations[civIndex] == nil) then 
				PreGame.SetCivilization(i, -1);
			end
		end
	end
end
-------------------------------------------------
-- UpdateGameOptionsDisplay()
-- This method refreshes UI elements based on PreGame Data.
-- This function should only read from PreGame and not set items.
-- The one exception is the hack for ensuring all Civs are valid.
-------------------------------------------------
function UpdateGameOptionsDisplay(bUpdateOnly)

	-- In theory, PreGame should do this for me, but just to be sure.
	ValidateCivSelections();
	
	local bCanEdit = CanEditGameOptions();
	
	Controls.MapTypePullDown:SetHide(false);
	Controls.MapTypePullDown:SetDisabled( not bCanEdit );
	Controls.LoadScenarioBox:SetHide( true );
	Controls.MapSizePullDown:GetButton():SetDisabled( not bCanEdit );
	
	RefreshMapTypeDisplay();
					
	-- Update World Size
	if (not PreGame.IsRandomWorldSize()) then
		local info = GameInfo.Worlds[ PreGame.GetWorldSize() ];
		Controls.MapSizePullDown:GetButton():LocalizeAndSetText( info.Description );
		Controls.MapSizePullDown:GetButton():LocalizeAndSetToolTip( info.Help );
	else
		Controls.MapSizePullDown:GetButton():LocalizeAndSetText("TXT_KEY_RANDOM_MAP_SIZE");
		Controls.MapSizePullDown:GetButton():LocalizeAndSetToolTip("TXT_KEY_RANDOM_MAP_SIZE_HELP");
	end
		
	Controls.MinorCivsSlider:SetDisabled( not bCanEdit );
	if (PreGame.GetNumMinorCivs() == -1) then
		Controls.MinorCivsSlider:SetValue( 0 );
		Controls.MinorCivsLabel:LocalizeAndSetText("TXT_KEY_AD_SETUP_CITY_STATES", 0);
	else
		Controls.MinorCivsSlider:SetValue( PreGame.GetNumMinorCivs() / #GameInfo.MinorCivilizations );
		Controls.MinorCivsLabel:LocalizeAndSetText("TXT_KEY_AD_SETUP_CITY_STATES", PreGame.GetNumMinorCivs());
	end
		
	 -- Update Game Speed
    local info = GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ];
  	Controls.GameSpeedPullDown:GetButton():LocalizeAndSetText( info.Description );
	Controls.GameSpeedPullDown:GetButton():LocalizeAndSetToolTip( info.Help );
	Controls.GameSpeedPullDown:SetDisabled( not bCanEdit );
	
	-- Update Start Era
	local info = GameInfo.Eras[ PreGame.GetEra() ];
	if(info == nil) then
		Controls.EraPull:GetButton():LocalizeAndSetText( "TXT_KEY_MISC_UNKNOWN" );
		Controls.EraPull:GetButton():LocalizeAndSetToolTip( "TXT_KEY_MISC_UNKNOWN" );
	else
		Controls.EraPull:GetButton():LocalizeAndSetText( info.Description );
		Controls.EraPull:GetButton():LocalizeAndSetToolTip( info.Strategy );
	end
	Controls.EraPull:SetDisabled( not bCanEdit );
	
	-- Update Turn Mode
	local turnModeStr = GetTurnModeStr();
	Controls.TurnModePull:GetButton():LocalizeAndSetText( turnModeStr );
	Controls.TurnModePull:GetButton():LocalizeAndSetToolTip( g_turnModeData[turnModeStr].tooltip );
	Controls.TurnModePull:SetDisabled( not bCanEdit );
	Controls.TurnModeRoot:SetHide(PreGame.IsHotSeatGame());	

	-- Update the options panel
	RefreshGameOptions();
	RefreshDropDownOptions();		
	
	if (not bUpdateOnly) then
		SendGameOptionChanged();	
	end
end
----------------------------------------------------------------        
---------------------------------------------------------------- 
function SetupForScenarioMap(mapScriptFileName)
	local loadScenarioChecked = PreGame.GetLoadWBScenario();
	Controls.LoadScenarioBox:SetHide( false );
	Controls.ScenarioCheck:SetCheck( loadScenarioChecked );
	
	if (not IsInLobby()) then
		if (loadScenarioChecked)then
			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_SCENARIO" );
		else
			Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_GAME" );
		end
	end
		
	if(loadScenarioChecked and IsWBMap(mapScriptFileName)) then
		local civList = UI.GetMapPlayers(mapScriptFileName);
		local curCiv = PreGame.GetCivilization(0);
		local civValid = false;
		if(civList ~= nil) then
			for i, v in pairs(civList) do
				if(v.Playable) then
					if(v.CivType == curCiv) then
						civValid = true;
						break;
					end
				end
			end
			
			if(not civValid) then
				PreGame.SetCivilization(0, civList[1].CivType);
				PreGame.SetHandicap(0, civList[1].DefaultHandicap);
				SetSelectedCiv();
			end
		end
	end
end

----------------------------------------------------------------        
---------------------------------------------------------------- 
function SetupForNonScenarioMap()
	Controls.AdvancedButton:SetDisabled(true);
	Controls.LoadScenarioBox:SetHide( true );
	--Controls.ScenarioCheck:SetCheck( false );
	if (not IsInLobby()) then
		Controls.LaunchButton:LocalizeAndSetText( "TXT_KEY_HOST_GAME" );
	end
	PreGame.SetLoadWBScenario(false);
end

----------------------------------------------------------------        
---------------------------------------------------------------- 
function IsWBMap(file)
	return Path.UsesExtension(file,".Civ5Map"); 
end

----------------------------------------------------------------
-- GAME SPEED
local gameSpeeds = {};
for row in GameInfo.GameSpeeds() do
    table.insert(gameSpeeds, row);
end
table.sort(gameSpeeds, function(a, b) return b.ID < a.ID end);

for i, v in ipairs(gameSpeeds) do

    local controlTable = {};
    Controls.GameSpeedPullDown:BuildEntry( "InstanceOne", controlTable );
    
    controlTable.Button:SetText( Locale.ConvertTextKey( v.Description ) );
    controlTable.Button:SetToolTipString( Locale.ConvertTextKey( v.Help ) );
    controlTable.Button:SetVoid1( v.ID );
end
Controls.GameSpeedPullDown:CalculateInternals();

function SelectSpeed( id )
    PreGame.SetGameSpeed( id );
    Controls.GameSpeedPullDown:GetButton():LocalizeAndSetText(GameInfo.GameSpeeds[id].Description);
    Controls.GameSpeedPullDown:GetButton():LocalizeAndSetToolTip(GameInfo.GameSpeeds[id].Help);
	UpdateGameOptionsDisplay();
end
Controls.GameSpeedPullDown:RegisterSelectionCallback( SelectSpeed );


----------------------------------------------------------------
-- MAP SIZE
function PopulateMapSizePulldown()
	local pullDown = Controls.MapSizePullDown;
	pullDown:ClearEntries();
	
	
	local mapType;
	if(not PreGame.IsRandomMapScript()) then
		local filename = PreGame.GetMapScript();	
		for row in GameInfo.Map_Sizes() do
			if(Path.GetFileNameWithoutExtension(filename) == Path.GetFileNameWithoutExtension(row.FileName)) then
				mapType = row.MapType;
				break;
			end
		end
	end
	
	if(mapType ~= nil) then
		local numMapSizes = 0;
		local mapSizes = {};
		for row in GameInfo.Map_Sizes{MapType = mapType} do
			mapSizes[row.WorldSizeType] = row.FileName;
			numMapSizes = numMapSizes + 1;
		end
		
		if(numMapSizes > 1) then
			local instance = {};
			pullDown:BuildEntry( "InstanceOne", instance );
			instance.Button:LocalizeAndSetText("TXT_KEY_RANDOM_MAP_SIZE");
			instance.Button:LocalizeAndSetToolTip("TXT_KEY_RANDOM_MAP_SIZE_HELP");
			instance.Button:SetVoid1( -1 );
		end
		
		for info in GameInfo.Worlds("ID >= 0 ORDER BY ID") do
			local sizeEntry = mapSizes[info.Type];
			if(sizeEntry ~= nil) then
				local instance = {};
				pullDown:BuildEntry( "InstanceOne", instance );
				instance.Button:LocalizeAndSetText(info.Description);
				instance.Button:LocalizeAndSetToolTip(info.Help);
				instance.Button:SetVoid1( info.ID );
			end
		end			
	else
		local instance = {};
		pullDown:BuildEntry( "InstanceOne", instance );
		instance.Button:LocalizeAndSetText("TXT_KEY_RANDOM_MAP_SIZE");
		instance.Button:LocalizeAndSetToolTip("TXT_KEY_RANDOM_MAP_SIZE_HELP");
		instance.Button:SetVoid1( -1 );

		for info in GameInfo.Worlds("ID >= 0 ORDER BY ID") do
			local instance = {};
			pullDown:BuildEntry( "InstanceOne", instance );
			instance.Button:LocalizeAndSetText(info.Description);
			instance.Button:LocalizeAndSetToolTip(info.Help);
			instance.Button:SetVoid1( info.ID );
		end
	
	end	

	pullDown:CalculateInternals();
	
	pullDown:RegisterSelectionCallback( function(id)
	
		if( id == -1 ) then
			PreGame.SetRandomWorldSize( true );
		else
			
			local mapFilter;
			if(PreGame.IsRandomMapScript() == false) then
				 local mapScript = PreGame.GetMapScript();
				 for row in GameInfo.Map_Sizes() do 
					if(Path.GetFileName(mapScript) == Path.GetFileName(row.FileName)) then
						mapFilter = row.MapType;
						break;
					end
				end
			end
	        
			local world = GameInfo.Worlds[id];
			PreGame.SetRandomWorldSize( false );
			PreGame.SetWorldSize( id );
			PreGame.SetNumMinorCivs( world.DefaultMinorCivs );
	        
			if(mapFilter ~= nil) then		
				for row in GameInfo.Map_Sizes{MapType = mapFilter, WorldSizeType = world.Type} do
					PreGame.SetMapScript(row.FileName);
					
					local wb = UI.GetMapPreview(row.FileName);
					if(wb ~= nil) then
						
						PreGame.SetRandomWorldSize(false);
						PreGame.SetWorldSize(wb.MapSize);
						--PreGame.SetEra(wb.StartEra);
						--PreGame.SetGameSpeed(wb.DefaultSpeed);
						--PreGame.SetMaxTurns(wb.MaxTurns);
						--PreGame.SetNumMinorCivs(wb.CityStateCount);
						
						local world = GameInfo.Worlds[wb.MapSize];
						PreGame.SetNumMinorCivs(world.DefaultMinorCivs);
						
						local victories = {};
						for _, v in ipairs(wb.VictoryTypes) do
							victories[v] = true;
						end
						
						for row in GameInfo.Victories() do
							PreGame.SetVictory(row.ID, victories[row.Type]);
						end
						
						local numPlayers = wb.PlayerCount;
						if(numPlayers == 0) then
							numPlayers = GameInfo.Worlds[wb.MapSize].DefaultPlayers	
						end
						
						for i = numPlayers, GameDefines.MAX_MAJOR_CIVS - 1 do
							if( PreGame.GetSlotStatus(i) == SlotStatus.SS_COMPUTER) then
								PreGame.SetSlotStatus(i, SlotStatus.SS_OPEN);
							end
						end
					end		
				end
			end
			
			if(Matchmaking.IsHost()) then
				-- Send broadcast our player info (meaning all the player info as the host)
				-- so that everyone can see the changed number of player slots.
				Network.BroadcastPlayerInfo();
			end
		end
				
		UpdateGameOptionsDisplay();
	end);
end
----------------------------------------------------------------
-- Put all the world builder map scripts into the passed in table
function GetWorldBuilderMaps(mapScripts)

	for row in GameInfo.Maps() do	
		local script = {
			Name = Locale.Lookup(row.Name),
			Description = Locale.Lookup(row.Description),			
			MapType = row.Type,
		};		
		table.insert(mapScripts, script);	
	end
		
	-- Filter out map files that are part of size groups.
	local worldBuilderMapsToFilter = {};
	for row in GameInfo.Map_Sizes() do
		table.insert(worldBuilderMapsToFilter, row.FileName);
	end
	
	for _, map in ipairs(Modding.GetMapFiles()) do
		
		local bExclude = false;
		for i,v in ipairs(worldBuilderMapsToFilter) do
			if(v == map.File) then
				bExclude = true;
				break;
			end
		end 

		if(not bExclude) then
			local mapData = UI.GetMapPreview(map.File);
			
			local name, description;
			local isError = nil;
			
			if(mapData) then
				
				if(not Locale.IsNilOrWhitespace(map.Name)) then
					name = map.Name;
				
				elseif(not Locale.IsNilOrWhitespace(mapData.Name)) then
					name = Locale.Lookup(mapData.Name);
				
				else
					name = Path.GetFileNameWithoutExtension(map.File);
				end
				
				if(map.Description and #map.Description > 0) then
					description = map.Description;
				else
					description = Locale.ConvertTextKey(mapData.Description);
				end
			else
				local _;
				_, _, name = string.find(map.File, "\\.+\\(.+)%.");
				
				local nameTranslation = Locale.ConvertTextKey("TXT_KEY_INVALID_MAP_TITLE", name);
				if(nameTranslation and nameTranslation ~= "TXT_KEY_INVALID_MAP_TITLE") then
					name = nameTranslation;
				else
					name = "[COLOR_RED]" .. name .. "[ENDCOLOR]";
				end
				
				local descTranslation = Locale.ConvertTextKey("TXT_KEY_INVALID_MAP_DESC");
				if(descTranslation and descTranslation ~= "TXT_KEY_INVALID_MAP_DESC") then
					description = descTranslation;
				end
				
				isError = true;
			end
			
			local entry = {
				Name = name,
				Description = description,
				FileName = map.File,
				IconIndex = 0,
				WBMapData = mapData,
				Error = isError,
			};
			
			table.insert(mapScripts, entry);
		end

		
	end
end
----------------------------------------------------------------
-- Map Scripts
function RefreshMapScripts()

	local pullDown = Controls.MapTypePullDown;
	pullDown:ClearEntries();
	
	controlTable = {};
	
	g_MapEntries = {};
				
	if ( PreGame.IsHotSeatGame() ) then
		-- And all single and multiplayer maps.	
		for row in GameInfo.MapScripts{SupportsMultiplayer = 1} do
			local script = {};
			script.FileName = row.FileName;
			script.IconIndex = row.IconIndex;
			script.Name = Locale.ConvertTextKey(row.Name);
			script.Description = Locale.ConvertTextKey(row.Description or "");
			
			table.insert(g_MapEntries, script);	
		end		
	else
		-- Networked multiplayer
		for row in GameInfo.MapScripts{SupportsMultiplayer = 1} do
			local script = {};
			script.FileName = row.FileName;
			script.IconIndex = row.IconIndex;
			script.Name = Locale.ConvertTextKey(row.Name);
			script.Description = Locale.ConvertTextKey(row.Description or "");
			
			table.insert(g_MapEntries, script);	
		end	
	end
	
	-- All the world builder scripts
	GetWorldBuilderMaps(g_MapEntries);
						
	table.sort(g_MapEntries, function(a, b) return a.Name < b.Name end);
	
	for i, script in ipairs(g_MapEntries) do
	
		controlTable = {};
	    pullDown:BuildEntry( "InstanceOne", controlTable );
	    
		controlTable.Button:SetText(script.Name);
		controlTable.Button:SetToolTipString(script.Description);
		controlTable.Button:SetVoid1(i);
	end
	pullDown:CalculateInternals();

	function TypeSelected( id )
		local mapScript = g_MapEntries[id];
	
		-- If this is an "error" entry (invalid WB file for example), do nothing.
		if(mapScript.Error) then
			return;
		end
		
		if (CanEditGameOptions()) then
			PreGame.SetLoadWBScenario(false);
			
			local mapScriptFileName = mapScript.FileName;
			local mapScriptWBMapData = mapScript.WBMapData;
			
			-- If just the mapType was specified, grab the correct map file based on 
			-- type and current map size settings.
			if(mapScript.MapType ~= nil) then
				local mapType = mapScript.MapType;
				local mapSizes = {};
				
				for row in GameInfo.Map_Sizes{MapType = mapType} do
					local world = GameInfo.Worlds[row.WorldSizeType];
					
					mapSizes[world.ID] = row.FileName; 
				end
				
				local worldSize = PreGame.GetWorldSize();
				
				if(mapSizes[worldSize] ~= nil) then
					--Adjust Map Size
					mapScriptFileName = mapSizes[worldSize];
				else
					-- Not Found, pick random size and set filename to smallest world size.
					for row in GameInfo.Worlds("ID >= 0 ORDER BY ID") do
						local file = mapSizes[row.ID];
						if(file ~= nil) then
							mapScriptFileName = file;
							break;
						end
					end
				end
				mapScriptWBMapData = UI.GetMapPreview(mapScriptFileName);
			end
			
			PreGame.SetRandomMapScript(false);
			PreGame.SetMapScript(mapScriptFileName);
						
			-- If it's a WB Map, we have more to do.
			if(mapScriptWBMapData ~= nil) then
				local wb = mapScriptWBMapData;
				
				PreGame.SetRandomWorldSize(false);
				PreGame.SetWorldSize(wb.MapSize);
				
				local world = GameInfo.Worlds[wb.MapSize];
				PreGame.SetNumMinorCivs(world.DefaultMinorCivs);
											
				local victories = {};
				for _, v in ipairs(wb.VictoryTypes) do
					victories[v] = true;
				end
				
				for row in GameInfo.Victories() do
					PreGame.SetVictory(row.ID, victories[row.Type]);
				end
				
				local numPlayers = wb.PlayerCount;
				if(numPlayers == 0) then
					numPlayers = GameInfo.Worlds[wb.MapSize].DefaultPlayers	
				end
				
				for i = numPlayers, GameDefines.MAX_MAJOR_CIVS - 1 do
					if( PreGame.GetSlotStatus(i) == SlotStatus.SS_COMPUTER) then
						PreGame.SetSlotStatus(i, SlotStatus.SS_OPEN);
					end
				end
			end				
		end
	
		-- Refresh Map Sizes.
		PopulateMapSizePulldown();
						
		RefreshDropDownOptions();
		RefreshGameOptions();
		UpdateGameOptionsDisplay();
	end
	pullDown:RegisterSelectionCallback( TypeSelected );
end


----------------------------------------------------------------
-- STARTING ERA
for info in GameInfo.Eras() do

    local controlTable = {};
    Controls.EraPull:BuildEntry( "InstanceOne", controlTable );
    controlTable.Button:LocalizeAndSetText(info.Description);
    controlTable.Button:SetVoid1( info.ID );
end
Controls.EraPull:CalculateInternals();

function OnEraPull( id )
    PreGame.SetEra( id );
  	UpdateGameOptionsDisplay();
end
Controls.EraPull:RegisterSelectionCallback( OnEraPull );

----------------------------------------------------------------
-- TURN MODE
for i, turnMode in ipairs(g_turnModeOptions) do
	local controlTable = {};
	Controls.TurnModePull:BuildEntry( "InstanceOne", controlTable );
	controlTable.Button:LocalizeAndSetText(turnMode);
	controlTable.Button:LocalizeAndSetToolTip(g_turnModeData[turnMode].tooltip);
	controlTable.Button:SetVoid1( g_turnModeData[turnMode].index);
end
Controls.TurnModePull:CalculateInternals();

function OnTurnModePull( id )
	if(id == g_turnModeData["TXT_KEY_GAME_OPTION_SIMULTANEOUS_TURNS"].index) then
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", true);
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
	elseif(id == g_turnModeData["TXT_KEY_GAME_OPTION_SEQUENTIAL_TURNS"].index) then
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
	elseif(id == g_turnModeData["TXT_KEY_GAME_OPTION_DYNAMIC_TURNS"].index) then
		PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
		PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", true);
	end
	
	UpdateGameOptionsDisplay();
end
Controls.TurnModePull:RegisterSelectionCallback( OnTurnModePull );


----------------------------------------------------------------
----------------------------------------------------------------
function OnMinorCivsSlider( fValue )
	local numMinorCivsWanted = math.floor( (fValue * #GameInfo.MinorCivilizations) + 0.5 );
    Controls.MinorCivsLabel:LocalizeAndSetText("TXT_KEY_AD_SETUP_CITY_STATES", numMinorCivsWanted);
    -- Apply the value, but wait until the left mouse button is not being tracked (the user is through sliding)
    if (not Controls.MinorCivsSlider:IsTrackingLeftMouseButton()) then
		PreGame.SetNumMinorCivs(numMinorCivsWanted);
		SendGameOptionChanged();
	end
end
Controls.MinorCivsSlider:RegisterSliderCallback( OnMinorCivsSlider );

----------------------------------------------------------------        
----------------------------------------------------------------
function OnMaxTurnsEditBoxChange()	
	PreGame.SetMaxTurns( Controls.MaxTurnsEdit:GetText() );
	SendGameOptionChanged();
end
Controls.MaxTurnsEdit:RegisterCallback( OnMaxTurnsEditBoxChange )

-------------------------------------------------
-------------------------------------------------
function OnMaxTurnsChecked()
	if(Controls.MaxTurnsCheck:IsChecked()) then
		Controls.MaxTurnsEditbox:SetHide(false);
		if (PreGame.GetMaxTurns() <= 0) then
			PreGame.SetMaxTurns(100);
		end
	else
		Controls.MaxTurnsEditbox:SetHide(true);
		PreGame.SetMaxTurns(0);
	end
	SendGameOptionChanged();
end
Controls.MaxTurnsCheck:RegisterCallback( Mouse.eLClick, OnMaxTurnsChecked );

----------------------------------------------------------------        
----------------------------------------------------------------
function OnTurnTimerEditBoxChange()	
	PreGame.SetPitbossTurnTime( Controls.TurnTimerEdit:GetText() );
	SendGameOptionChanged();
end
Controls.TurnTimerEdit:RegisterCallback( OnTurnTimerEditBoxChange )

-------------------------------------------------
-------------------------------------------------
function SetTurnTimerOption()
	local isChecked = Controls.TurnTimerCheck:IsChecked();
	Controls.TurnTimerEditbox:SetHide(not isChecked);
	PreGame.SetGameOption("GAMEOPTION_END_TURN_TIMER_ENABLED", isChecked);
	
	-- Update editbox text based on current turn time.
	local turnTime = PreGame.GetPitbossTurnTime();
	
	-- Initially only display the turn time in the edit box if the turn time is not zero.
	-- This mimics the MaxTurns option's functionality.
	if(turnTime > 0) then
		Controls.TurnTimerEdit:SetText( turnTime );
	end
end

-------------------------------------------------
-------------------------------------------------
function OnTurnTimerChecked()
	SetTurnTimerOption();
	SendGameOptionChanged();
end
Controls.TurnTimerCheck:RegisterCallback( Mouse.eLClick, OnTurnTimerChecked );

----------------------------------------------------------------
----------------------------------------------------------------
function RefreshDropDownOptions()
	g_DropDownOptionsManager:ResetInstances();
	
	local currentMapScript = PreGame.GetMapScript();
	
	local bCanEdit = CanEditGameOptions();
		
	local options = {};
	for option in DB.Query("select * from MapScriptOptions where exists (select 1 from MapScriptOptionPossibleValues where FileName = MapScriptOptions.FileName and OptionID = MapScriptOptions.OptionID) and Hidden = 0 and FileName = ?", currentMapScript) do
		options[option.OptionID] = {
			ID = option.OptionID,
			Name = Locale.ConvertTextKey(option.Name),
			ToolTip = (option.Description) and Locale.ConvertTextKey(option.Description) or nil,
			Disabled = (option.ReadOnly == 1) and true or false,
			DefaultValue = option.DefaultValue,
			SortPriority = option.SortPriority,
			Values = {},
		}; 
	end
	
	for possibleValue in DB.Query("select * from MapScriptOptionPossibleValues where FileName = ? order by SortIndex ASC", currentMapScript) do
		if(options[possibleValue.OptionID] ~= nil) then
			table.insert(options[possibleValue.OptionID].Values, {
				Name	= Locale.ConvertTextKey(possibleValue.Name),
				ToolTip = (possibleValue.Description) and Locale.ConvertTextKey(possibleValue.Description) or nil,
				Value	= possibleValue.Value,
			});
		end
	end
	
	local sortedOptions = {};
	for k,v in pairs(options) do
		table.insert(sortedOptions, v);
	end
	 
	-- Sort the options
	table.sort(sortedOptions, function(a, b) 
		if(a.SortPriority == b.SortPriority) then
			return a.Name < b.Name; 
		else
			return a.SortPriority < b.SortPriority;
		end
	end);
	
	-- Update the UI!
	for _, option in ipairs(sortedOptions) do
	
		local gameOption = g_DropDownOptionsManager:GetInstance();
				
		gameOption.OptionName:SetText(option.Name);
		gameOption.OptionName:SetToolTipString(option.ToolTip);
		
		
		gameOption.OptionDropDown:SetDisabled(option.Disabled or not bCanEdit);
		local dropDownButton = gameOption.OptionDropDown:GetButton();
		
		gameOption.OptionDropDown:ClearEntries();
		for _, possibleValue in ipairs(option.Values) do
			controlTable = {};
			gameOption.OptionDropDown:BuildEntry( "InstanceOne", controlTable );
			controlTable.Button:SetText(possibleValue.Name);
			controlTable.Button:SetToolTipString(possibleValue.ToolTip);
			
			controlTable.Button:RegisterCallback(Mouse.eLClick, function()
				dropDownButton:SetText(possibleValue.Name);
				dropDownButton:SetToolTipString(possibleValue.ToolTip);
				
				PreGame.SetMapOption(option.ID, possibleValue.Value);
				SendGameOptionChanged();
			end);
		end
		
		--Assign the currently selected value.
		local savedValue = PreGame.GetMapOption(option.ID);
		local defaultValue;
		if(savedValue ~= -1) then
			defaultValue = option.Values[savedValue];
		else
			defaultValue = option.Values[option.DefaultValue];
		end
		
		if(defaultValue ~= nil) then
			dropDownButton:SetText(defaultValue.Name);
			dropDownButton:SetToolTipString(defaultValue.ToolTip);
		end
	
		if(option.Disabled) then
			dropDownButton:SetDisabled(true);
		end
		
		gameOption.OptionDropDown:CalculateInternals();
	end
	
	Controls.DropDownOptionsStack:CalculateSize();
	Controls.DropDownOptionsStack:ReprocessAnchoring();

	Controls.OptionsScrollPanel:CalculateInternalSize();
end

function RefreshTurnTimerUnits()
	if(PreGame.GetGameOption("GAMEOPTION_PITBOSS") == 1) then
		Controls.TurnTimerUnits:SetText(hoursStr);
	else
		Controls.TurnTimerUnits:SetText(secondsStr);
	end	
end

----------------------------------------------------------------
----------------------------------------------------------------
function RefreshGameOptions()

	g_GameOptionsManager:ResetInstances();
	g_VictoryCondtionsManager:ResetInstances();
	g_DLCAllowedManager:ResetInstances();

	local bCanEdit = CanEditGameOptions();
	---------------------------------
	-- Victory conditions
	for row in GameInfo.Victories() do
		local victoryCondition = g_VictoryCondtionsManager:GetInstance();
		
		local victoryConditionTextButton = victoryCondition.GameOptionRoot:GetTextButton();
		victoryConditionTextButton:LocalizeAndSetText(row.Description);
		
		victoryCondition.GameOptionRoot:SetCheck(PreGame.IsVictory(row.ID));
		victoryCondition.GameOptionRoot:SetDisabled( not bCanEdit );
		
		victoryCondition.GameOptionRoot:RegisterCheckHandler( function(bCheck)
				PreGame.SetVictory(row.ID, bCheck);
				SendGameOptionChanged();
			end);
		
	end

	---------------------------------
	-- General Game Options
	local options = {};
	-- First, Gather a list of all options
	for option in GameInfo.GameOptions{Visible = 1} do
		local option = {
			Type = option.Type,
			Name = Locale.ConvertTextKey(option.Description),
			ToolTip = (option.Help) and Locale.ConvertTextKey(option.Help) or nil,
			Checked = (option.Default == 1) and true or false,
			Disabled = false,
			GameOption = true,
			SortPriority = 0,
			SupportsSinglePlayer = option.SupportsSinglePlayer,
			SupportsMultiplayer = option.SupportsMultiplayer,
		};
		
		-- Don't show several of the game options because they are handled elsewhere.
		if( option.Type ~= "GAMEOPTION_END_TURN_TIMER_ENABLED" 
				and option.Type ~= "GAMEOPTION_SIMULTANEOUS_TURNS"
				and option.Type ~= "GAMEOPTION_DYNAMIC_TURNS") then 
			local savedValue = PreGame.GetGameOption(option.Type);
			if(savedValue ~= nil) then
				option.Checked = savedValue == 1;
			end

			-- Only show non-singleplayer options in network multiplayer 
			-- The turn timer ends up being an exception to this because we manually add the UI for that option.
			local DontDisplay = (PreGame.IsHotSeatGame() and not option.SupportsSinglePlayer) 
											 or (not PreGame.IsHotSeatGame() and not option.SupportsMultiplayer);
			if( not DontDisplay ) then
				table.insert(options, option);
			end
		end
	end
	
	for option in DB.Query("select * from MapScriptOptions where not exists (select 1 from MapScriptOptionPossibleValues where FileName = MapScriptOptions.FileName and OptionID = MapScriptOptions.OptionID) and Hidden = 0 and FileName = ?", PreGame.GetMapScript()) do
		local option = {
			ID = option.OptionID,
			Name = Locale.ConvertTextKey(option.Name),
			ToolTip = (option.Description) and Locale.ConvertTextKey(option.Description) or nil,
			Checked = (option.DefaultValue == 1) and true or false,
			Disabled = (option.ReadOnly == 1) and true or false,
			GameOption = false,
			SortPriority = option.SortPriority,
		};
		
		local savedValue = PreGame.GetMapOption(option.ID);
		if(savedValue ~= nil) then
			option.Checked = savedValue == 1;
		end
		
		table.insert(options, option);
	end
	
	-- Sort the options alphabetically
	table.sort(options, function(a, b) 
		if(a.SortPriority == b.SortPriority) then
			return a.Name < b.Name;
		else
			return a.SortPriority < b.SortPriority;
		end
	end);
	
	-- Add Options to UI.
	local sizeY = 0;
	for _, option in ipairs(options) do
		local gameOption = g_GameOptionsManager:GetInstance();
		
		local gameOptionTextButton = gameOption.GameOptionRoot:GetTextButton();
		gameOptionTextButton:SetText(option.Name);
								
		if(option.ToolTip ~= nil) then
			gameOption.GameOptionRoot:SetToolTipString(option.ToolTip);
		end
		
		-- Don't allow dedicated servers to edit the pitboss option.
		local bCanEditThisOption = not (option.Type == "GAMEOPTION_PITBOSS") or not Network.IsDedicatedServer();
		
		gameOption.GameOptionRoot:SetDisabled(option.Disabled or not bCanEdit or not bCanEditThisOption);
		gameOption.GameOptionRoot:SetCheck(option.Checked);
		sizeY = sizeY + gameOption.GameOptionRoot:GetSizeY();
		if(option.GameOption == true) then
			gameOption.GameOptionRoot:RegisterCheckHandler( function(bCheck)
				PreGame.SetGameOption(option.Type, bCheck);
				SendGameOptionChanged();
				if(option.Type == "GAMEOPTION_PITBOSS") then
					RefreshTurnTimerUnits();
				end
			end);
		else
			gameOption.GameOptionRoot:RegisterCheckHandler( function(bCheck)
				PreGame.SetMapOption(option.ID, bCheck);
				SendGameOptionChanged();
			end);
		end	
	end	
	
	local maxTurns = PreGame.GetMaxTurns();
	if(maxTurns <= 0) then
		Controls.MaxTurnsCheck:SetCheck(false);
		Controls.MaxTurnsEditbox:SetHide(true);
	else
		Controls.MaxTurnsCheck:SetCheck(true);
		Controls.MaxTurnsEdit:SetText( maxTurns );
		Controls.MaxTurnsEditbox:SetHide(false);
	end
	Controls.MaxTurnsCheck:SetDisabled( not bCanEdit );
	
	-- Configure initial turn timer setting.
	Controls.TurnTimerCheck:SetCheck(PreGame.GetGameOption("GAMEOPTION_END_TURN_TIMER_ENABLED") == 1);
	SetTurnTimerOption();
	Controls.TurnTimerCheck:SetDisabled( not bCanEdit );

	-- Set turn timer units label.
	RefreshTurnTimerUnits();

	local bCanEditDLC = CanEditDLCGameOptions();
	---------------------------------
	-- DLC Allowed
	for row in GameInfo.DownloadableContent() do
		if (row.IsBaseContentUpgrade == 0) then
			local dlcEntries = g_DLCAllowedManager:GetInstance();
			
			local dlcButton = dlcEntries.GameOptionRoot:GetTextButton();
			dlcButton:LocalizeAndSetText(row.FriendlyNameKey);
			
			dlcEntries.GameOptionRoot:SetCheck(PreGame.IsDLCAllowed(row.PackageID));
			dlcEntries.GameOptionRoot:SetDisabled( not bCanEditDLC );
			
			dlcEntries.GameOptionRoot:RegisterCheckHandler( function(bCheck)
																PreGame.SetDLCAllowed(row.PackageID, bCheck);
																SendGameOptionChanged();
															end
														  );
		else
			PreGame.SetDLCAllowed(row.PackageID, true);
		end
	end

	Controls.CityStateStack:CalculateSize();
	Controls.CityStateStack:ReprocessAnchoring();

	Controls.DropDownOptionsStack:CalculateSize();
	Controls.DropDownOptionsStack:ReprocessAnchoring();

	Controls.VictoryConditionsStack:CalculateSize();
	Controls.VictoryConditionsStack:ReprocessAnchoring();

	Controls.MaxTurnStack:CalculateSize();
	Controls.MaxTurnStack:ReprocessAnchoring();
	
	Controls.TurnTimerStack:CalculateSize();
	Controls.TurnTimerStack:ReprocessAnchoring();	

	Controls.GameOptionsStack:CalculateSize();
	Controls.GameOptionsStack:ReprocessAnchoring();

	Controls.GameOptionsFullStack:CalculateSize();
	Controls.GameOptionsFullStack:ReprocessAnchoring();

	Controls.OptionsScrollPanel:CalculateInternalSize();
end
