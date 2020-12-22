-------------------------------------------------
-- GameSetup Screen
-- modified by bc1 from 1.0.3.144 code
-------------------------------------------------
CivilopediaControl = "/FrontEnd/MainMenu/Other/Civilopedia" -- MUST be before include( "PopulateUniques" ) to enable pedia callback
include( "EUI_utilities" )
local IconHookup = EUI.IconHookup
local pairs = pairs
local tonumber = tonumber
local table_insert = table.insert
local table_concat = table.concat

include( "PopulateUniques" )
local InitializePopulateUniques = InitializePopulateUniques
local PopulateUniquesForGameSetup = PopulateUniquesForGameSetup

local bIsModding = ContextPtr:GetID() == "ModdingGameSetupScreen";

local function isWBMap(file)
	return Path.UsesExtension(file,".Civ5Map");
end

----------------------
-- Back Button Handler
----------------------
local function OnBack()
	UIManager:DequeuePopup( ContextPtr );
	ContextPtr:SetHide( true );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

-----------------------
-- Misc Button Handlers
-----------------------
Controls.StartButton:RegisterCallback( Mouse.eLClick,
function()
	PreGame.SetPersistSettings( not bIsModding ); -- Whether or not to save settings for the "Play Now" option.

	if bIsModding and isWBMap( PreGame.GetMapScript() ) then
		PreGame.SetRandomMapScript(false);
		PreGame.SetLoadWBScenario(PreGame.GetLoadWBScenario());
		PreGame.SetOverrideScenarioHandicap(true);
	else
		PreGame.SetLoadWBScenario(false);
	end

	Events.SerialEventStartGame();
	UIManager:SetUICursor( 1 );
end );

-----------------------
Controls.AdvancedButton:RegisterCallback( Mouse.eLClick,
function()
	UIManager:QueuePopup( Controls.AdvancedSetup, PopupPriority.AdvancedSetup );
end );

-----------------------
Controls.RandomizeButton:RegisterCallback( Mouse.eLClick, 
function()
	PreGame.SetLoadWBScenario(false);
	PreGame.SetCivilization(0, -1);
	PreGame.SetRandomWorldSize(true);
	PreGame.SetRandomMapScript(true);
	UpdateDisplay();
end );

-----------------------
Controls.EditButton:RegisterCallback( Mouse.eLClick,
function()
	UIManager:PushModal( Controls.SetCivNames );
end );

-----------------------
Controls.RemoveButton:RegisterCallback( Mouse.eLClick,
function()
	Controls.RemoveButton:SetHide(true);
	PreGame.SetLeaderName( 0, "" );
	PreGame.SetCivilizationDescription( 0, "" );
	PreGame.SetCivilizationShortDescription( 0, "" );
	PreGame.SetCivilizationAdjective( 0, "" );

	SetSelectedCiv();
end );

-----------------------
Controls.Button:RegisterCallback( Mouse.eLClick,
function()
	UIManager:QueuePopup( Controls.SelectCivilization, PopupPriority.SelectCivilization );
end );

-----------------------
Controls.GameSpeedButton:RegisterCallback( Mouse.eLClick,
function()
	Controls.SelectGameSpeed:SetHide( not Controls.SelectGameSpeed:IsHidden() );

	Controls.SelectMapType:SetHide( true );
	Controls.SelectMapSize:SetHide( true );
	Controls.SelectDifficulty:SetHide( true );

end );

-----------------------
Controls.DifficultyButton:RegisterCallback( Mouse.eLClick,
function()
	Controls.SelectDifficulty:SetHide( not Controls.SelectDifficulty:IsHidden() );

	Controls.SelectGameSpeed:SetHide( true );
	Controls.SelectMapType:SetHide( true );
	Controls.SelectMapSize:SetHide( true );
end );

-----------------------
Controls.MapTypeButton:RegisterCallback( Mouse.eLClick,
function()
	Controls.SelectMapType:SetHide( not Controls.SelectMapType:IsHidden() );

	Controls.SelectMapSize:SetHide( true );
	Controls.SelectDifficulty:SetHide( true );
	Controls.SelectGameSpeed:SetHide( true );

	bScenarioSettingsLoaded = false;
end );

-----------------------
Controls.ScenarioCheck:RegisterCallback( Mouse.eLClick,
function()
	PreGame.SetLoadWBScenario(not PreGame.GetLoadWBScenario());
	if PreGame.GetLoadWBScenario() then
		Controls.AdvancedButton:SetDisabled(true);
		Controls.StartButton:SetText(Locale.ConvertTextKey("TXT_KEY_START_SCENARIO"));

		local mapScriptFileName = PreGame.GetMapScript();
		if isWBMap(mapScriptFileName) then
			ApplyScenarioSettings(mapScriptFileName);
			SetupForScenarioMap(mapScriptFileName);
		end
	else
		Controls.AdvancedButton:SetDisabled(false);
		Controls.StartButton:SetText(Locale.ConvertTextKey("TXT_KEY_START_GAME"));
	end
end );

-----------------------
Controls.MapSizeButton:RegisterCallback( Mouse.eLClick,
function()
	Controls.SelectMapSize:SetHide( not Controls.SelectMapSize:IsHidden() );

	Controls.SelectMapType:SetHide( true );
	Controls.SelectDifficulty:SetHide( true );
	Controls.SelectGameSpeed:SetHide( true );

end );


-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()

	-- In theory, PreGame should do this for me, but just to be sure.
	for i = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		if not GameInfo.Civilizations[ PreGame.GetCivilization(i) ] then
			PreGame.SetCivilization( i, -1 );
		end
	end

	-- In Modding Game Setup Screen
	if not PreGame.IsRandomMapScript() then
		local mapScriptFileName = PreGame.GetMapScript();

		if isWBMap(mapScriptFileName) then

			-- World Builder Map Selected
			SetMapTypeSizeForMap(mapScriptFileName);

			if UI.IsMapScenario(mapScriptFileName) then
				-- Load Scenario (Very Restricted)
				SetupForScenarioMap(mapScriptFileName);
			else
				-- Ignore Scenario Info
				SetupForNonScenarioMap();
			end
		else
			-- Mapscript Selected
			SetMapForScript();
		end
	else
		-- Random Mapscript Selected
		SetMapForScript();
	end

	-- Set Difficulty Slot
	SetDifficulty();

	-- Set Game Pace Slot
	SetGamePace();

	-- Sets up Selected Civ Slot
	SetSelectedCiv();

end

----------------------------------------------------------------
----------------------------------------------------------------
function SetMapForScript()

	Controls.AdvancedButton:SetDisabled(false);
	Controls.MapTypeButton:SetHide(false);
	Controls.LoadScenarioBox:SetHide( true );
	if not PreGame.IsRandomMapScript() then
		local mapScript;
		for row in GameInfo.MapScripts{ FileName = PreGame.GetMapScript() } do
			mapScript = row;
			break;
		end

		if mapScript then
			IconHookup( mapScript.IconIndex or 0, 128, mapScript.IconAtlas, Controls.TypeIcon );
			Controls.TypeHelp:SetText( Locale.ConvertTextKey( mapScript.Description or "" ) );
			Controls.TypeName:SetText( Locale.ConvertTextKey("TXT_KEY_AD_MAP_TYPE_SETTING", Locale.ConvertTextKey( mapScript.Name ) ) );
		else
			PreGame.SetRandomMapScript(true);
		end
	end

	if PreGame.IsRandomMapScript() then
		IconHookup( 4, 128, "WORLDTYPE_ATLAS", Controls.TypeIcon);
		Controls.TypeHelp:SetText(Locale.ConvertTextKey("TXT_KEY_RANDOM_MAP_SCRIPT_HELP" ));
		Controls.TypeName:SetText(Locale.ConvertTextKey("TXT_KEY_AD_MAP_TYPE_SETTING", Locale.ConvertTextKey("TXT_KEY_RANDOM_MAP_SCRIPT")));
	end
	SetMapSizeForScript();
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetMapSizeForScript()

	Controls.MapSizeButton:SetHide(false);
	Controls.MapSizeButton:SetDisabled(false);
	if not PreGame.IsRandomWorldSize() then
		local info = GameInfo.Worlds[ PreGame.GetWorldSize() ];
		if info then
			IconHookup( info.PortraitIndex, 128, info.IconAtlas, Controls.SizeIcon );
			Controls.SizeHelp:SetText( Locale.ConvertTextKey( info.Help ) );
			Controls.SizeName:SetText( Locale.ConvertTextKey( "TXT_KEY_AD_MAP_SIZE_SETTING", Locale.ConvertTextKey( info.Description ) ) );
		end
	else
		IconHookup( 6, 128, "WORLDSIZE_ATLAS", Controls.SizeIcon );
		Controls.SizeHelp:SetText( Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SIZE_HELP" ) );
		Controls.SizeName:SetText( Locale.ConvertTextKey( "TXT_KEY_AD_MAP_SIZE_SETTING", Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SIZE" ) ) );
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetDifficulty()
	-- Set Difficulty Slot
	local info = GameInfo.HandicapInfos[ PreGame.GetHandicap( 0 ) ];
	if info then
		IconHookup( info.PortraitIndex, 128, info.IconAtlas, Controls.DifficultyIcon );
		Controls.DifficultyHelp:SetText( Locale.ConvertTextKey( info.Help ) );
		Controls.DifficultyName:SetText( Locale.ConvertTextKey("TXT_KEY_AD_HANDICAP_SETTING", Locale.ConvertTextKey( info.Description ) ) );
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetGamePace()
	-- Set Game Pace Slot
	local info = GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ];
	if info then
		IconHookup( info.PortraitIndex, 128, info.IconAtlas, Controls.SpeedIcon );
		Controls.SpeedHelp:SetText( Locale.ConvertTextKey( info.Help ) );
		Controls.SpeedName:SetText( Locale.ConvertTextKey("TXT_KEY_AD_GAME_SPEED_SETTING", Locale.ConvertTextKey( info.Description ) ) );
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
bScenarioSettingsLoaded = false;
function SetMapTypeSizeForMap(mapScriptFileName)
	local mapInfo = UI.GetMapPreview(mapScriptFileName);
	if mapInfo then

		local mapType, mapName, mapDescription;
		IconHookup( 4, 128, "WORLDTYPE_ATLAS", Controls.TypeIcon);

		for row in GameInfo.Map_Sizes() do
			if Path.GetFileNameWithoutExtension(mapScriptFileName) == Path.GetFileNameWithoutExtension(row.FileName) then
				local mapEntry = GameInfo.Maps[ row.MapType ];
				if mapEntry then
					mapType = row.MapType;
					mapName = Locale.Lookup(mapEntry.Name);
					mapDescription = Locale.Lookup(mapEntry.Description);
					IconHookup( mapEntry.IconIndex, 128, mapEntry.IconAtlas, Controls.TypeIcon);

					break;
				end
			end
		end

		if not mapType then
			mapName = Path.GetFileNameWithoutExtension(mapScriptFileName);

			-- Set Map Type Slot
			if not Locale.IsNilOrWhitespace(mapInfo.Name) then
				mapName = Locale.ConvertTextKey(mapInfo.Name);
			else
				mapName = Path.GetFileNameWithoutExtension(mapScriptFileName);
			end

			mapDescription = Locale.Lookup(mapInfo.Description);
		end

		Controls.TypeName:SetText(Locale.ConvertTextKey("TXT_KEY_AD_MAP_TYPE_SETTING", mapName));
		Controls.TypeHelp:SetText(mapDescription);

		if UI.IsMapScenario(mapScriptFileName) then

			local ttEntries = {
				Locale.Lookup("TXT_KEY_AD_SETUP_PLAYER_COUNT", mapInfo.PlayerCount),
				Locale.Lookup("TXT_KEY_AD_SETUP_CITY_STATES", mapInfo.CityStateCount),
				Locale.Lookup("TXT_KEY_AD_SETUP_START_ERA", mapInfo.StartEra)
			};

			if tonumber(mapInfo.MaxTurns) > 0 then
				table_insert(ttEntries, Locale.Lookup("TXT_KEY_AD_SETUP_MAX_TURNS_1", mapInfo.MaxTurns));
			end

			Controls.LoadScenarioBox:SetToolTipString(table_concat(ttEntries, "[NEWLINE]"));
		end

		SetMapSizeForScript();

		local num_available_sizes = 0;
		if mapType then
			for row in GameInfo.Map_Sizes{ MapType = mapType } do
				num_available_sizes = num_available_sizes + 1;
			end
		end

		Controls.MapSizeButton:SetDisabled(num_available_sizes <= 1);

		if PreGame.GetLoadWBScenario() and not bScenarioSettingsLoaded then
			ApplyScenarioSettings(mapScriptFileName);
		end
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
function ApplyScenarioSettings(mapFileName)
	if PreGame.GetLoadWBScenario() and isWBMap(mapFileName) then
		UI.ResetScenarioPlayerSlots();

		local preview = UI.GetMapPreview(mapFileName);
		if preview then
			PreGame.SetGameSpeed(preview.DefaultSpeed);
			SetGamePace();
		end

		local playerList = UI.GetMapPlayers(mapFileName);
		if playerList then
			for i, v in pairs(playerList) do
				if v.Playable then
					UI.MoveScenarioPlayerToSlot(i - 1, 0);
					PreGame.SetHandicap(0, v.DefaultHandicap);
					local civ = GameInfo.Civilizations[ v.CivType ];
					if civ then
						PreGame.SetCivilization(0, v.CivType);
					else
						PreGame.SetCivilization(0, -1);
					end

					SetSelectedCiv();
					SetDifficulty();
					break;
				end
			end
		end

		bScenarioSettingsLoaded = true;
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetupForScenarioMap(mapScriptFileName)
	local loadScenarioChecked = PreGame.GetLoadWBScenario();
	Controls.LoadScenarioBox:SetHide( false );
	Controls.AdvancedButton:SetDisabled(loadScenarioChecked);
	Controls.ScenarioCheck:SetCheck( loadScenarioChecked );
	Controls.StartButton:SetText(Locale.ConvertTextKey("TXT_KEY_START_SCENARIO"));
end

----------------------------------------------------------------
----------------------------------------------------------------
function SetupForNonScenarioMap()
	Controls.AdvancedButton:SetDisabled(false);
	Controls.LoadScenarioBox:SetHide( true );
	--Controls.ScenarioCheck:SetCheck( false );
	Controls.StartButton:SetText(Locale.ConvertTextKey("TXT_KEY_START_GAME"));
	PreGame.SetLoadWBScenario(false);
end

function SetSelectedCiv()
	-- Sets valid initial index for Civ Slot
	local civIndex = PreGame.GetCivilization( 0 );

	local civ = GameInfo.Civilizations[ civIndex ];
	local mapTexture, civAndLeader;

	-- Sets up Selected Civ Slot
	if civ then
		-- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
		local leader = GameInfo.Leaders[ GameInfo.Civilization_Leaders{ CivilizationType = civ.Type }().LeaderheadType ];
		local customEmpty = true;
		local function pickNotEmpty( a, b )
			if a and #a > 0 then
				customEmpty = false;
				return a
			else
				return b
			end
		end

		civAndLeader = {
			ID = civ.ID,
			Type = civ.Type,
			Pedia = civ.ShortDescription,
			Description = pickNotEmpty( PreGame.GetCivilizationDescription(0), civ.Description ),
			ShortDescription = pickNotEmpty( PreGame.GetCivilizationShortDescription(0), civ.ShortDescription ),
			PortraitIndex = civ.PortraitIndex,
			IconAtlas = civ.IconAtlas,
			LeaderType = leader.Type,
			LeaderPedia = leader.Description,
			LeaderDescription = pickNotEmpty( PreGame.GetLeaderName(0), leader.Description ),
			LeaderPortraitIndex = leader.PortraitIndex,
			LeaderIconAtlas = leader.IconAtlas,
		};
		Controls.RemoveButton:SetHide( customEmpty );

		-- Set Selected Civ Map
		mapTexture=civ.MapImage;

	else
		-- Random Civ Slot
		PreGame.SetCivilization(0, -1);
		-- Random Civ Map
		mapTexture="MapRandom512.dds";
	end
	Controls.LargeMapImage:UnloadTexture();
	Controls.LargeMapImage:SetTexture( mapTexture );
	Controls.Icons:DestroyAllChildren();
	InitializePopulateUniques();
	PopulateUniquesForGameSetup( Controls, civAndLeader );
end

----------------------
-- Show / Hide handler
----------------------
ContextPtr:SetShowHideHandler(
function( isHide, isInit )
	if not isInit then
		if isHide then
			Controls.LargeMapImage:UnloadTexture();
		else
			UpdateDisplay();

			if bIsModding then
				Controls.ScreenTitle:SetText( Locale.ConvertTextKey( "TXT_KEY_MODDING_SETUP_TITLE" ) );
			else
				Controls.ScreenTitle:SetText( Locale.ConvertTextKey( "TXT_KEY_GAME_SELECTION_SCREEN" ) );
			end
			Controls.MainSelection:SetHide( false );
			Controls.SelectMapType:SetHide( true );
			Controls.SelectMapSize:SetHide( true );
			Controls.SelectDifficulty:SetHide( true );
			Controls.SelectGameSpeed:SetHide( true );
		end
	end
end );

-------------------
-- Input processing
-------------------
ContextPtr:SetInputHandler(
function( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			OnBack();
			return true;
		end
	end
end );
