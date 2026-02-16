if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );
include( "IconSupport" );
include( "SupportFunctions" );
include( "UniqueBonuses" );
include( "MapUtilities" );

-- Global Constants
g_InstanceManager = InstanceManager:new( "LoadButton", "InstanceRoot", Controls.LoadFileButtonStack );
g_ReferencedPackages = InstanceManager:new("ReferencedPackageInstance", "Label", Controls.ReferencedPackagesStack);

-- Global Variables
g_ShowCloudSaves = false;
g_ShowAutoSaves = false;

g_CloudSaves = {};

g_FileList = {};
g_SortTable = {};
g_InstanceList = {};

g_GameType = GameTypes.GAME_SINGLE_PLAYER;
g_IsInGame = false;
g_iSelected = -1;
g_SavedGameModsRequired = nil;	-- The required mods for the currently selected save.
g_SavedGameDLCRequired = nil;	-- The required DLC for the currently selected save.

g_CurrentSort = nil;	-- The current sorting technique.
----------------------------------------------------------------        
----------------------------------------------------------------        
function OnStartButton()
	
	if g_GameType == GameTypes.GAME_SINGLE_PLAYER or g_GameType == GameTypes.GAME_HOTSEAT_MULTIPLAYER then
		UIManager:SetUICursor( 1 );
		if(g_ShowCloudSaves) then
			Events.PlayerChoseToLoadGame(Steam.GetCloudSaveFileName(g_iSelected), true);
			Controls.StartButton:SetDisabled( true );
			UIManager:DequeuePopup( ContextPtr );
		else
			local thisLoadFile = g_FileList[ g_iSelected ];
			if(thisLoadFile) then
				Events.PlayerChoseToLoadGame(thisLoadFile);
    			Controls.StartButton:SetDisabled( true );
    			UIManager:DequeuePopup( ContextPtr );
			end
		end
	else
		-- Multiplayer
		local header = nil;
		if(g_ShowCloudSaves) then
			local thisLoadFile = Steam.GetCloudSaveFileName(g_iSelected);
			if (thisLoadFile) then
				PreGame.SetLoadFileName( thisLoadFile, true );
				header = PreGame.GetFileHeader( thisLoadFile, true );
			end
		else
			local thisLoadFile = g_FileList[ g_iSelected ];
			if (thisLoadFile) then
				PreGame.SetLoadFileName( thisLoadFile );
				header = PreGame.GetFileHeader( thisLoadFile );
			end
		end
		
		if (header ~= nil) then
			local worldInfo = GameInfo.Worlds[ header.WorldSize ];
			PreGame.SetWorldSize( worldInfo.ID );

			local strGameName = ""; -- TODO

			if (Network.IsDedicatedServer()) then
				PreGame.SetGameOption("GAMEOPTION_PITBOSS", true);
				local bResult, bPending = Matchmaking.HostServerGame( strGameName, PreGame.ReadActiveSlotCountFromSaveGame(), false );
			else	
				if PreGame.IsInternetGame() then
					local bResult, bPending = Matchmaking.HostInternetGame( strGameName, PreGame.ReadActiveSlotCountFromSaveGame() );
				else
					if (PreGame.IsHotSeatGame()) then					
						local bResult, bPending = Matchmaking.HostHotSeatGame( strGameName, PreGame.ReadActiveSlotCountFromSaveGame() );
					else
						local bResult, bPending = Matchmaking.HostLANGame( strGameName, PreGame.ReadActiveSlotCountFromSaveGame() );
					end
				end
			end
			UIManager:DequeuePopup( ContextPtr );	
		end
	end
end
Controls.StartButton:RegisterCallback( Mouse.eLClick, OnStartButton );

----------------------------------------------------------------        
----------------------------------------------------------------        
Controls.AutoCheck:RegisterCheckHandler( function(checked)
    g_ShowAutoSaves = checked;
    if(g_ShowAutoSaves) then
		g_ShowCloudSaves = false;
    end
    
    UpdateControlStates();
    SetupFileButtonList();
end);
----------------------------------------------------------------
----------------------------------------------------------------
Controls.CloudCheck:RegisterCheckHandler( function(checked)
	g_ShowCloudSaves = checked;
	
	if(g_ShowCloudSaves) then
		Controls.SortByPullDown:SetHide(true);	
		g_ShowAutoSaves = false;
	else
		Controls.SortByPullDown:SetHide(false);	
	end
	
	UpdateControlStates();
	SetupFileButtonList();
end);

----------------------------------------------------------------        
----------------------------------------------------------------
function OnDelete( )
	Controls.DeleteConfirm:SetHide(false);
end
Controls.Delete:RegisterCallback( Mouse.eLClick, OnDelete );

function OnYes( )
	Controls.DeleteConfirm:SetHide(true);
	UI.DeleteSavedGame( g_FileList[ g_iSelected ] );
	SetupFileButtonList();
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );

function OnNo( )
	Controls.DeleteConfirm:SetHide(true);
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );
----------------------------------------------------------------        
----------------------------------------------------------------        
function OnBack()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------   
-- Referenced Packages popup     
----------------------------------------------------------------   
Controls.ShowModsButton:RegisterCallback(Mouse.eLClick, function()
	g_ReferencedPackages:ResetInstances();
	
	Controls.ReferencedPackagesPrompt:LocalizeAndSetText("TXT_KEY_LOAD_MENU_REQUIRED_MODS");
	for i,v in ipairs(g_SavedGameModsRequired) do
		local referencedPackage = g_ReferencedPackages:GetInstance();

		local name = v.Title;
		if(Locale.HasTextKey(name)) then
			name = Locale.ConvertTextKey(name);
		end
		
		local truncateWidth = 72;
		if(#name > truncateWidth) then
			name = string.format("%s... ", string.sub(name, 1, truncateWidth));
		end 
		--We always want to at least supply 1 argument.
		local text = string.format("[ICON_BULLET] %s (v. %i)", name, v.Version);
			
		referencedPackage.Label:SetText(text);
	end
	
	Controls.ReferencedPackagesStack:CalculateSize();
	Controls.ReferencedPackagesScrollPanel:CalculateInternalSize();
	Controls.ReferencedPackagesWindow:SetHide(false);
end);
----------------------------------------------------------------  
Controls.ShowDLCButton:RegisterCallback(Mouse.eLClick, function()
	g_ReferencedPackages:ResetInstances();
	
	Controls.ReferencedPackagesPrompt:LocalizeAndSetText("TXT_KEY_LOAD_MENU_REQUIRED_DLC");
	for i,v in ipairs(g_SavedGameDLCRequired) do
		local referencedPackage = g_ReferencedPackages:GetInstance();

		local name;
		print(v.DescriptionKey);
		if(v.DescriptionKey and Locale.HasTextKey(v.DescriptionKey)) then
			name = Locale.ConvertTextKey(v.DescriptionKey);
		else
			name = v.Title;
			if(Locale.HasTextKey(name)) then
				name = Locale.ConvertTextKey(name);
			end
		end
		
		local truncateWidth = 72;
		if(#name > truncateWidth) then
			name = string.format("%s... ", string.sub(name, 1, truncateWidth));
		end 
		
		--We always want to at least supply 1 argument.
		local text = "[ICON_BULLET] " .. name;
		referencedPackage.Label:SetText(text);
	end
	
	Controls.ReferencedPackagesStack:CalculateSize();
	Controls.ReferencedPackagesScrollPanel:CalculateInternalSize();
	Controls.ReferencedPackagesWindow:SetHide(false);
end);
----------------------------------------------------------------  
Controls.CloseReferencedPackagesButton:RegisterCallback(Mouse.eLClick, function()
	Controls.ReferencedPackagesWindow:SetHide(true);
end);
----------------------------------------------------------------        
----------------------------------------------------------------        
function SetSelected( index )
    if( g_iSelected ~= -1 ) then
        g_InstanceList[ g_iSelected ].SelectHighlight:SetHide( true );
    end
    
    g_iSelected = index;

    if( g_iSelected ~= -1 ) then
        g_InstanceList[ g_iSelected ].SelectHighlight:SetHide( false );
		
		local header;
		if(g_ShowCloudSaves) then
			header = g_CloudSaves[g_iSelected];
		else
			header = PreGame.GetFileHeader(g_FileList[g_iSelected]);
		end
		
		Controls.StartButton:SetDisabled(header == nil or header.GameType ~= g_GameType);		
		if(header ~= nil) then

			local civ = GameInfo.Civilizations[ header.PlayerCivilization ];

			local name;
			if(g_ShowCloudSaves) then
				name = "";--Locale.ConvertTextKey("TXT_KEY_STEAMCLOUD");
			else
				name = GetDisplayName(g_FileList[g_iSelected]);
			end

			--Set Save File Text
			TruncateString(Controls.SaveFileName, Controls.DetailsBox:GetSizeX(), name); 
					
			if(header.CurrentEra ~= "") then
				local currentEra = GameInfo.Eras[header.CurrentEra];
				if(currentEra ~= nil) then
					Controls.EraTurn:LocalizeAndSetText("TXT_KEY_CUR_ERA_TURNS_FORMAT", currentEra.Description, header.TurnNumber);
				else
					Controls.EraTurn:LocalizeAndSetText("TXT_KEY_CUR_ERA_TURNS_FORMAT", "TXT_KEY_MISC_UNKNOWN", header.TurnNumber);
				end	
			else
				Controls.EraTurn:SetText("");
			end
			
			if(header.StartEra ~= "") then
				local startEra = GameInfo.Eras[header.StartEra];
				if(startEra ~= nil) then
					Controls.StartEra:LocalizeAndSetText("TXT_KEY_START_ERA", startEra.Description);
				else
					Controls.StartEra:LocalizeAndSetText("TXT_KEY_START_ERA", "TXT_KEY_MISC_UNKNOWN");
				end
			else
				Controls.StartEra:SetText("");
			end

			if (header.GameType == GameTypes.GAME_HOTSEAT_MULTIPLAYER) then
				Controls.GameType:SetText( Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_HOTSEAT_GAME") );
			else
				if (header.GameType == GameTypes.GAME_NETWORK_MULTIPLAYER) then
					Controls.GameType:SetText(  Locale.ConvertTextKey("TXT_KEY_MULTIPLAYER_STRING") );
				else
					if (header.GameType == GameTypes.GAME_SINGLE_PLAYER) then
						Controls.GameType:SetText(  Locale.ConvertTextKey("TXT_KEY_SINGLE_PLAYER") );
					else
						Controls.GameType:SetText( "" );
					end
				end
			end
			
			-- Set Save file time
			local date;
			if(not g_ShowCloudSaves) then
				date = UI.GetSavedGameModificationTime(g_FileList[ g_iSelected ]);
			end
			
			
			if(date ~= nil) then
				Controls.TimeSaved:SetText(date);	
			else
				Controls.TimeSaved:SetText("");
			end		  
	
			--First, default it to unkown values.
			local civName = Locale.ConvertTextKey("TXT_KEY_MISC_UNKNOWN");
			local leaderDescription = Locale.ConvertTextKey("TXT_KEY_MISC_UNKNOWN");
			
			Controls.CivIcon:SetTexture(questionTextureSheet);
			Controls.CivIcon:SetTextureOffset(questionOffset);
			Controls.CivIcon:SetToolTipString(unknownString);
			
			-- ? leader icon
			IconHookup( 22, 128, "LEADER_ATLAS", Controls.Portrait );
			
			if(civ ~= nil) then
				-- Sets civ icon and tool tip
				civName = Locale.ConvertTextKey(civ.Description);
				local textureOffset, textureAtlas = IconLookup( civ.PortraitIndex, 64, civ.IconAtlas );
				if textureOffset ~= nil then       
					Controls.CivIcon:SetTexture( textureAtlas );
					Controls.CivIcon:SetTextureOffset( textureOffset );
					Controls.CivIcon:SetToolTipString( Locale.ConvertTextKey( civ.ShortDescription) );
				end	
				
				-- Set Selected Civ Map
				Controls.LargeMapImage:UnloadTexture();
				local mapTexture = civ.MapImage;
				Controls.LargeMapImage:SetTexture(mapTexture);
				
				local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];
				if(leader ~= nil) then
					leaderDescription = Locale.ConvertTextKey(leader.Description);
					IconHookup( leader.PortraitIndex, 128, leader.IconAtlas, Controls.Portrait );
				end
			end
			
			if(header.LeaderName ~= nil and header.LeaderName ~= "")then
				leaderDescription = header.LeaderName;
			end
			
			if(header.CivilizationName ~= nil and header.CivilizationName ~= "")then
				civName = header.CivilizationName;
			end
			Controls.Title:LocalizeAndSetText("TXT_KEY_RANDOM_LEADER_CIV", leaderDescription, civName );
	        
	        local mapInfo = MapUtilities.GetBasicInfo(header.MapScript);
	        IconHookup( mapInfo.IconIndex, 64, mapInfo.IconAtlas, Controls.MapType );
			Controls.MapType:SetToolTipString(Locale.Lookup(mapInfo.Name));
							
			-- Sets map size icon and tool tip
			info = GameInfo.Worlds[ header.WorldSize ];
			if(info ~= nil) then
				IconHookup( info.PortraitIndex, 64, info.IconAtlas, Controls.MapSize );
				Controls.MapSize:SetToolTipString( Locale.ConvertTextKey( info.Description) );
			else
				if(questionOffset ~= nil) then
					Controls.MapSize:SetTexture( questionTextureSheet );
					Controls.MapSize:SetTextureOffset( questionOffset );
					Controls.MapSize:SetToolTipString( unknownString );
				end
			end
			
			-- Sets handicap icon and tool tip
			info = GameInfo.HandicapInfos[ header.Difficulty ];
			if(info ~= nil) then
				IconHookup( info.PortraitIndex, 64, info.IconAtlas, Controls.Handicap );
				Controls.Handicap:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
			else
				if(questionOffset ~= nil) then
					Controls.Handicap:SetTexture( questionTextureSheet );
					Controls.Handicap:SetTextureOffset( questionOffset );
					Controls.Handicap:SetToolTipString( unknownString );
				end
			end
			
			-- Sets game pace icon and tool tip
			info = GameInfo.GameSpeeds[ header.GameSpeed ];
			if(info ~= nil) then
				IconHookup( info.PortraitIndex, 64, info.IconAtlas, Controls.SpeedIcon );
				Controls.SpeedIcon:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
			else
				if(questionOffset ~= nil) then
					Controls.SpeedIcon:SetTexture( questionTextureSheet );
					Controls.SpeedIcon:SetTextureOffset( questionOffset );
					Controls.SpeedIcon:SetToolTipString( unknownString );
				end
			end		
			
			local canLoadSaveResult;
			if(g_ShowCloudSaves) then
				canLoadSaveResult = Modding.CanLoadCloudSave(g_iSelected);
			else
				canLoadSaveResult = Modding.CanLoadSavedGame(g_FileList[g_iSelected]);
			end	
			  	
			  	
			  	
			-- Obtain all of the required mods for the save
			if(g_ShowCloudSaves) then
				g_SavedGameDLCRequired, g_SavedGameModsRequired = Modding.GetCloudSaveRequirements(g_iSelected);
			else
				g_SavedGameDLCRequired, g_SavedGameModsRequired = Modding.GetSavedGameRequirements(g_FileList[g_iSelected]);
			end
			
			Controls.ShowModsButton:SetHide(g_SavedGameModsRequired == nil or #g_SavedGameModsRequired == 0);
			Controls.ShowDLCButton:SetHide(g_SavedGameDLCRequired == nil or #g_SavedGameDLCRequired == 0);
								
			local tooltip = nil;
			if(canLoadSaveResult > 0) then
				local errorTooltips = {};
				errorTooltips[1] = nil;
				errorTooltips[2] = "TXT_KEY_MODDING_ERROR_SAVE_MISSING_DLC";
				errorTooltips[3] = "TXT_KEY_MODDING_ERROR_SAVE_DLC_NOT_PURCHASED";
				errorTooltips[4] = "TXT_KEY_MODDING_ERROR_SAVE_MISSING_MODS";
				errorTooltips[5] = "TXT_KEY_MODDING_ERROR_SAVE_INCOMPATIBLE_MODS";

				tooltip = Locale.ConvertTextKey(errorTooltips[canLoadSaveResult]);
			end
			
			Controls.StartButton:SetToolTipString(tooltip);
			Controls.StartButton:SetDisabled(canLoadSaveResult > 0 or header.GameType ~= g_GameType);  
			
			Controls.Delete:SetDisabled(false); 
			
		else
			SetSaveInfoToNone(true);
		end
	else -- No saves are selected
		SetSaveInfoToNone(false);
    end
end

function SetSaveInfoToNone(isInvalid)
	-- Disable ability to enter game if none are selected
	Controls.StartButton:SetDisabled(true);
	Controls.StartButton:SetToolTipString("");
	
	Controls.Delete:SetDisabled(true);
	
	Controls.ShowDLCButton:SetHide(true);
	Controls.ShowModsButton:SetHide(true);
	
	-- Empty all text fields
	if(g_ShowCloudSaves) then
		Controls.Title:LocalizeAndSetText("TXT_KEY_STEAM_EMPTY_SAVE");
	elseif(not isInvalid) then
		Controls.Title:LocalizeAndSetText("TXT_KEY_SELECT_SAVE_GAME");
	else
		Controls.Title:LocalizeAndSetText("TXT_KEY_INVALID_SAVE_GAME");
		Controls.Delete:SetDisabled(false);
	end
	
	Controls.NoGames:SetHide( isInvalid );
	Controls.SaveFileName:SetText( "" );
	Controls.EraTurn:SetText( "" );
	Controls.StartEra:SetText( "" );
	Controls.GameType:SetText( "" );
	Controls.TimeSaved:SetText( "" );
	
	-- ? leader icon
	IconHookup( 22, 128, "LEADER_ATLAS", Controls.Portrait );
	
	-- Set all icons across bottom of left panel to ?
	if questionOffset ~= nil then      
		-- Civ Icon 
		Controls.CivIcon:SetTexture( questionTextureSheet );
		Controls.CivIcon:SetTextureOffset( questionOffset );
		Controls.CivIcon:SetToolTipString( unknownString );

		-- Map Type Icon 
		Controls.MapType:SetTexture( questionTextureSheet );
		Controls.MapType:SetTextureOffset( questionOffset );
		Controls.MapType:SetToolTipString( unknownString );
		-- Map Size Icon 
		Controls.MapSize:SetTexture( questionTextureSheet );
		Controls.MapSize:SetTextureOffset( questionOffset );
		Controls.MapSize:SetToolTipString( unknownString );
		-- Handicap Icon 
		Controls.Handicap:SetTexture( questionTextureSheet );
		Controls.Handicap:SetTextureOffset( questionOffset );
		Controls.Handicap:SetToolTipString( unknownString );
		-- Game Speed Icon 
		Controls.SpeedIcon:SetTexture( questionTextureSheet );
		Controls.SpeedIcon:SetTextureOffset( questionOffset );
		Controls.SpeedIcon:SetToolTipString( unknownString );
	end
    
	-- Set Selected Civ Map
	Controls.LargeMapImage:UnloadTexture();
	local mapTexture="MapRandom512.dds";
	Controls.LargeMapImage:SetTexture(mapTexture);
end
----------------------------------------------------------------        
---------------------------------------------------------------- 
function GetDisplayName(file)
	return Path.GetFileNameWithoutExtension(file);
end
----------------------------------------------------------------        
----------------------------------------------------------------        
function SetupFileButtonList()
    SetSelected( -1 );
    g_InstanceManager:ResetInstances();
    
    -- build a table of all save file names that we found
    g_FileList = {};
    g_SortTable = {};
    g_InstanceList = {};
    
    if(g_ShowCloudSaves) then
		Controls.NoGames:SetHide( true );
		local strEmptyCloudSave = Locale.ConvertTextKey("TXT_KEY_STEAM_EMPTY_SAVE");
		
		for i = 1, Steam.GetMaxCloudSaves(), 1 do
			local controlTable = g_InstanceManager:GetInstance();
			g_InstanceList[i] = controlTable;
			
			local name = strEmptyCloudSave;
			local saveData = g_CloudSaves[i];

			if(saveData ~= nil) then
			
				local civName = Locale.ConvertTextKey("TXT_KEY_MISC_UNKNOWN");
				local leaderDescription = Locale.ConvertTextKey("TXT_KEY_MISC_UNKNOWN");
				
				local civ = GameInfo.Civilizations[ saveData.PlayerCivilization ];
				if(civ ~= nil) then
					civName = Locale.ConvertTextKey(civ.Description);
				
					local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];
					if(leader ~= nil) then
						leaderDescription = Locale.ConvertTextKey(leader.Description);
					end
				end
				
				if(saveData.CivilizationName ~= nil and saveData.CivilizationName ~= "")then
					civName = saveData.CivilizationName;
				end
				
				if(saveData.LeaderName ~= nil and saveData.LeaderName ~= "")then
					leaderDescription = saveData.LeaderName;
				end
			
				name = Locale.ConvertTextKey("TXT_KEY_RANDOM_LEADER_CIV", leaderDescription, civName );				
			end
			
			local entryName = Locale.ConvertTextKey("TXT_KEY_STEAMCLOUD_SAVE", i, name);
			TruncateString(controlTable.ButtonText, controlTable.Button:GetSizeX(), entryName);
	         
	        controlTable.Button:SetVoid1( i );
			controlTable.Button:RegisterCallback( Mouse.eLClick, SetSelected );
			
			g_SortTable[tostring(controlTable.InstanceRoot)] = i;
	    end
		
		Controls.LoadFileButtonStack:SortChildren( CloudSaveSort );
		
    else  
		UI.SaveFileList( g_FileList, g_GameType, g_ShowAutoSaves, true);
		for i, v in ipairs(g_FileList) do
			local controlTable = g_InstanceManager:GetInstance();
			g_InstanceList[i] = controlTable;
		    
			local displayName = GetDisplayName(v); 
	        
			TruncateString(controlTable.ButtonText, controlTable.Button:GetSizeX(), displayName);
	         
			controlTable.Button:SetVoid1( i );
			controlTable.Button:RegisterCallback( Mouse.eLClick, SetSelected );
			
			local high, low = UI.GetSavedGameModificationTimeRaw(v);
			
			g_SortTable[ tostring( controlTable.InstanceRoot ) ] = {Title = displayName, LastModified = {High = high, Low = low} };
			Controls.NoGames:SetHide( true );
		end
		
		Controls.LoadFileButtonStack:SortChildren( g_CurrentSort );
	end
	
	Controls.LoadFileButtonStack:CalculateSize();
    Controls.ScrollPanel:CalculateInternalSize();
    Controls.LoadFileButtonStack:ReprocessAnchoring();
end
----------------------------------------------------------------
----------------------------------------------------------------
function UpdateControlStates()

	Controls.AutoCheck:SetCheck(g_ShowAutoSaves);
	Controls.CloudCheck:SetCheck(g_ShowCloudSaves);
	Controls.Delete:SetHide(g_ShowCloudSaves);

    if( UI.GetCurrentGameState() == GameStateTypes.CIV5_GS_MAINGAMEVIEW ) then
        Controls.BGBlock:SetHide( false );
	else
        Controls.BGBlock:SetHide( true );
    end
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function CloudSaveSort( a, b ) 

    local aOrder = g_SortTable[ tostring( a ) ];
    local bOrder = g_SortTable[ tostring( b ) ];
    
    if( aOrder == nil ) then
        return true;
    elseif( bOrder == nil ) then
        return false;
    end
    
    return aOrder < bOrder;
end

function AlphabeticalSort( a, b ) 
    local oa = g_SortTable[ tostring( a ) ];
    local ob = g_SortTable[ tostring( b ) ];
    
    if( oa == nil ) then
        return true;
    elseif( ob == nil ) then
        return false;
    end
    
    return Locale.Compare(oa.Title, ob.Title) == -1;
end

function ReverseAlphabeticalSort( a, b ) 
    local oa = g_SortTable[ tostring( a ) ];
    local ob = g_SortTable[ tostring( b ) ];
    
    if( oa == nil ) then
        return false;
    elseif( ob == nil ) then
        return true;
    end
    
    return Locale.Compare(ob.Title, oa.Title) == -1;
end

function SortByName(a, b)
	if(g_ShowAutoSaves) then
		return ReverseAlphabeticalSort(a,b);
	else
		return AlphabeticalSort(a,b);
	end 
end

function SortByLastModified(a, b)
	local oa = g_SortTable[tostring(a)];
	local ob = g_SortTable[tostring(b)];
	
	if( oa == nil ) then
        return false;
    elseif( ob == nil ) then
        return true;
    end
    
	local result = UI.CompareFileTime(oa.LastModified.High, oa.LastModified.Low, ob.LastModified.High, ob.LastModified.Low);
    return result == 1;
end


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
			if(Controls.DeleteConfirm:IsHidden())then
	            OnBack();
	        else
				Controls.DeleteConfirm:SetHide(true);
			end
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );

----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler(isHide)
    if( not isHide ) then
		if( g_IsInGame ) then
            if( Game:IsGameMultiPlayer() ) then
				if( Game:IsNetworkMultiPlayer() ) then
					g_GameType = GameTypes.GAME_NETWORK_MULTIPLAYER;
				else
					g_GameType = GameTypes.GAME_HOTSEAT_MULTIPLAYER;
				end
            else
				g_GameType = GameTypes.GAME_SINGLE_PLAYER;
            end
        end
		
		g_CloudSaves = Steam.GetCloudSaves();
        
        UpdateControlStates();
		SetupFileButtonList();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

local contextID = ContextPtr:LookUpControl(".."):GetID();

g_IsInGame = (contextID == "GameMenu");
if (contextID == "ModdingSinglePlayer" or contextID == "SinglePlayerScreen") then
	g_GameType = GameTypes.GAME_SINGLE_PLAYER;
else
	if PreGame.IsHotSeatGame() then
		g_GameType = GameTypes.GAME_HOTSEAT_MULTIPLAYER;
	else
		g_GameType = GameTypes.GAME_NETWORK_MULTIPLAYER;
	end
end

local sortOptions = {
	{"TXT_KEY_SORTBY_LASTMODIFIED", SortByLastModified},
	{"TXT_KEY_SORTBY_NAME", SortByName},
};

local sortByPulldown = Controls.SortByPullDown;
sortByPulldown:ClearEntries();
for i, v in ipairs(sortOptions) do
	local controlTable = {};
	sortByPulldown:BuildEntry( "InstanceOne", controlTable );
	controlTable.Button:LocalizeAndSetText(v[1]);
	
	controlTable.Button:RegisterCallback(Mouse.eLClick, function()
		sortByPulldown:GetButton():LocalizeAndSetText(v[1]);
		Controls.LoadFileButtonStack:SortChildren( v[2] );
		
		g_CurrentSort = v[2];
	end);
	
end
sortByPulldown:CalculateInternals();

sortByPulldown:GetButton():LocalizeAndSetText(sortOptions[1][1]);
g_CurrentSort = sortOptions[1][2];

g_IsModVersion = (contextID == "ModdingSinglePlayer");
