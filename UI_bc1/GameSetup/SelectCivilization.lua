-------------------------------------------------
-- Select Civilization
-- modified by bc1 from 1.0.3.144 code
-------------------------------------------------
CivilopediaControl = "/FrontEnd/MainMenu/Other/Civilopedia";-- MUST be before include( "PopulateUniques" ) to enable pedia callback
include( "PopulateUniques" ); local InitializePopulateUniques = InitializePopulateUniques; local PopulateUniquesForSelectCivilization = PopulateUniquesForSelectCivilization;

local g_bIsScenario = false;
local g_bWasScenario = true;
local g_bRefreshCivs = false;

-------------------------------------------------
-- Event processing
-------------------------------------------------
local function RequireRefreshCivs()
	g_bRefreshCivs = true;
end;
Events.AfterModsActivate.Add( RequireRefreshCivs );
Events.AfterModsDeactivate.Add( RequireRefreshCivs );

local function OnBack()
	UIManager:DequeuePopup( ContextPtr );
	ContextPtr:SetHide( true );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

ContextPtr:SetInputHandler(
function( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown and wParam == Keys.VK_ESCAPE then
		OnBack();
		return true;
	end
end );

----------------------------------------------------------------
-- Set the Civ
----------------------------------------------------------------
local function CivilizationSelected( civID, scenarioPlayerID )
	PreGame.SetCivilization( 0, civID );
	if g_bIsScenario and scenarioPlayerID then
		UI.MoveScenarioPlayerToSlot( scenarioPlayerID, 0 );
		local playerList = UI.GetMapPlayers( PreGame.GetMapScript() );
		local player = playerList and playerList[ scenarioPlayerID + 1 ];
		if player then
			PreGame.SetHandicap( 0, player.DefaultHandicap );
		end
	end
	OnBack();
end

-------------------------------------------------
-- Main processing
-------------------------------------------------
ContextPtr:SetShowHideHandler(
function( bIsHide )

	local isWBMap = Path.UsesExtension( PreGame.GetMapScript(), ".Civ5Map" );
	g_bIsScenario = PreGame.GetLoadWBScenario() and isWBMap;
	if g_bWasScenario ~= g_bIsScenario then
		g_bRefreshCivs = true;
	end
	g_bWasScenario = g_bIsScenario;

	if not bIsHide and (isWBMap or g_bRefreshCivs) then

		g_bRefreshCivs = false;
		Controls.CivStack:DestroyAllChildren();
		InitializePopulateUniques();
		local civEntries = {};

		if g_bIsScenario then
			local civList = UI.GetMapPlayers( PreGame.GetMapScript() );

			if civList then

				local scenarioCivQuery = DB.CreateQuery([[ SELECT
						Civilizations.ID,
						Civilizations.Type,
						Civilizations.Description,
						Civilizations.ShortDescription,
						Civilizations.PortraitIndex,
						Civilizations.IconAtlas,
						Leaders.Type AS LeaderType,
						Leaders.Description as LeaderDescription,
						Leaders.PortraitIndex as LeaderPortraitIndex,
						Leaders.IconAtlas as LeaderIconAtlas
						FROM Civilizations, Leaders, Civilization_Leaders WHERE
						Civilizations.ID = ? AND
						Civilizations.Type = Civilization_Leaders.CivilizationType AND
						Leaders.Type = Civilization_Leaders.LeaderheadType
						LIMIT 1
				]]);

				for i, civ in pairs( civList ) do
					if civ.Playable then
						for row in scenarioCivQuery( civ.CivType ) do
							table.insert( civEntries, {Locale.Lookup(row.LeaderDescription), row, i - 1} );
						end
					end
				end
			end
		else
			-- Start with random civ slot
			civEntries = {{""}};
			-- Add playable civilizations
			for row in DB.Query([[ SELECT
						Civilizations.ID,
						Civilizations.Type,
						Civilizations.Description,
						Civilizations.ShortDescription,
						Civilizations.PortraitIndex,
						Civilizations.IconAtlas,
						Leaders.Type AS LeaderType,
						Leaders.Description as LeaderDescription,
						Leaders.PortraitIndex as LeaderPortraitIndex,
						Leaders.IconAtlas as LeaderIconAtlas
						FROM Civilizations, Leaders, Civilization_Leaders WHERE
						Civilizations.Type = Civilization_Leaders.CivilizationType AND
						Leaders.Type = Civilization_Leaders.LeaderheadType AND
						Civilizations.Playable = 1
			]]) do
				table.insert( civEntries, {Locale.Lookup(row.LeaderDescription), row} );
			end
		end

		-- Sort civs by leader description
		table.sort( civEntries, function(a, b) return Locale.Compare(a[1], b[1]) == -1 end );

		-- Populate civ slots
		for i, civEntry in ipairs( civEntries ) do
			local civControls = {};
			ContextPtr:BuildInstanceForControl( "CivInstance", civControls, Controls.CivStack );
			PopulateUniquesForSelectCivilization( civControls, civEntry[2] );
			civControls.Button:SetVoid2( civEntry[3] );
			civControls.Button:RegisterCallback( Mouse.eLClick, CivilizationSelected );
		end

		Controls.CivStack:CalculateSize();
		Controls.CivStack:ReprocessAnchoring();
		Controls.CivPanel:CalculateInternalSize();
	end
end );
