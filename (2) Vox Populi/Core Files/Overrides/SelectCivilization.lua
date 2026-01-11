-------------------------------------------------
-- Select Civilization
-------------------------------------------------
CivilopediaControl = "/FrontEnd/MainMenu/Other/Civilopedia" -- MUST be before include( "UniqueBonuses" ) to enable pedia callback
include( "UniqueBonuses" );

------------------------------------------------
-- Global Variables
------------------------------------------------
g_bIsScenario = false;
g_bWasScenario = true;
g_bRefreshCivs = false;
  
-------------------------------------------------
-------------------------------------------------
function OnBack()
	UIManager:DequeuePopup( ContextPtr );
	ContextPtr:SetHide( true );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide )

	local isWBMap = IsWBMap(PreGame.GetMapScript());
	g_bIsScenario = (PreGame.GetLoadWBScenario() and isWBMap);
	if(g_bWasScenario ~= g_bIsScenario) then
		g_bRefreshCivs = true;
	end
	g_bWasScenario = g_bIsScenario;

   if( not bIsHide and (isWBMap or g_bRefreshCivs)) then
		g_bRefreshCivs = false;
    	Controls.Stack:DestroyAllChildren(); 
    	InitCivSelection();
   end

end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------        
---------------------------------------------------------------- 
function IsWBMap(file)
	return Path.UsesExtension(file,".Civ5Map"); 
end

----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            OnBack();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------        
-- set the Civ
----------------------------------------------------------------        
function CivilizationSelected( civID, scenarioPlayerID )
    PreGame.SetCivilization( 0, civID );
    if g_bIsScenario then
		UI.MoveScenarioPlayerToSlot( scenarioPlayerID, 0 );
		print("select civ");
		local playerList = UI.GetMapPlayers(PreGame.GetMapScript());
		if(playerList ~= nil) then
			local player = playerList[scenarioPlayerID + 1];
			if(player ~= nil) then
				PreGame.SetHandicap(0, player.DefaultHandicap);
			end
		end
    end
    
    OnBack();
end

function AddRandomCivilizationEntry()
	------------------------------------------------------------------------------------------------
	-- set up the random slot
	------------------------------------------------------------------------------------------------
	local controlTable = {};
	ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, Controls.Stack );
	controlTable.Button:SetVoid1( -1 );
	controlTable.Button:RegisterCallback( Mouse.eLClick, CivilizationSelected );

	controlTable.Title:LocalizeAndSetText("TXT_KEY_RANDOM_LEADER");
	controlTable.Description:LocalizeAndSetText("TXT_KEY_RANDOM_LEADER_HELP");

	IconHookup( 22, 128, "LEADER_ATLAS", controlTable.Portrait );
	if questionOffset ~= nil then
		controlTable.CivIcon:SetTexture( questionTextureSheet );
		controlTable.CivIcon:SetTextureOffset( questionOffset );
	end

	-- Sets Trait bonus Text
	controlTable.BonusDescription:SetText( "" );

	 -- Sets Bonus Icons
	local maxSmallButtons = GameDefines.NUM_UNIQUE_COMPONENTS;

	-- Adjust text width based on number of buttons
	local baseWrapWidth = 535;
	local adjustedWrapWidth = baseWrapWidth + math.floor((6 - maxSmallButtons) / 2) * 64;
	controlTable.BonusDescription:SetWrapWidth(adjustedWrapWidth);

	-- Hide all buttons first
	for buttonNum = 1, 6, 1 do
		local buttonName = "B"..tostring(buttonNum);
		local buttonFrameName = "BF"..tostring(buttonNum);
		controlTable[buttonName]:SetHide(true);
		controlTable[buttonFrameName]:SetHide(true);
	end

	-- Show only the buttons we need
	for buttonNum = 1, maxSmallButtons, 1 do
		local buttonName = "B"..tostring(buttonNum);
		controlTable[buttonName]:SetTexture( questionTextureSheet );
		controlTable[buttonName]:SetTextureOffset( questionOffset );
		controlTable[buttonName]:SetToolTipString( unknownString );
		controlTable[buttonName]:SetHide(false);
		local buttonFrameName = "BF"..tostring(buttonNum);
		controlTable[buttonFrameName]:SetHide(false);
	end
end

function AddCivilizationEntry(traitsQuery, populateUniqueBonuses, civ, leaderType, leaderDescription, leaderPortraitIndex, leaderIconAtlas, scenarioCivID)

	local controlTable = {};
	ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, Controls.Stack );
    
	controlTable.Button:SetVoid1( civ.ID );
	controlTable.Button:SetVoid2( scenarioCivID );
	controlTable.Button:RegisterCallback( Mouse.eLClick, CivilizationSelected );

	IconHookup( leaderPortraitIndex, 128, leaderIconAtlas, controlTable.Portrait );
	local textureOffset, textureAtlas = IconLookup( civ.PortraitIndex, 64, civ.IconAtlas );
	if textureOffset ~= nil then
		controlTable.CivIcon:SetTexture( textureAtlas );
		controlTable.CivIcon:SetTextureOffset( textureOffset );
	end

	-- Set pedia callbacks
	if CivilopediaControl then
		if controlTable.Portrait and leaderDescription then
			controlTable.Portrait:RegisterCallback( Mouse.eRClick, function()
				Events.SearchForPediaEntry( leaderDescription );
			end );
		end
		if controlTable.CivIcon and civ.ShortDescription then
			controlTable.CivIcon:RegisterCallback( Mouse.eRClick, function()
				Events.SearchForPediaEntry( civ.ShortDescription );
			end );
		end
	end

    -- Sets Trait bonus Text
	local shortDescription = "";

	for row in traitsQuery(leaderType) do
		controlTable.BonusDescription:LocalizeAndSetText(row.Description);
		shortDescription = row.ShortDescription;
	end

	-- Adjust text width based on number of buttons
	local maxSmallButtons = GameDefines.NUM_UNIQUE_COMPONENTS;
	local baseWrapWidth = 535;
	local adjustedWrapWidth = baseWrapWidth + math.floor((6 - maxSmallButtons) / 2) * 64;
	controlTable.BonusDescription:SetWrapWidth(adjustedWrapWidth);

	local title = Locale.ConvertTextKey("TXT_KEY_RANDOM_LEADER_CIV", leaderDescription, civ.ShortDescription);
	title = string.format("%s (%s)", title, Locale.ConvertTextKey( shortDescription ));

	controlTable.Title:SetText(title);
	controlTable.Description:LocalizeAndSetText(civ.Description);

	populateUniqueBonuses( controlTable, civ.Type, true);
	
	return controlTable;
end

function InitCivSelection()

	local traitsQuery = DB.CreateQuery([[SELECT Description, ShortDescription FROM Traits inner join 
										 Leader_Traits ON Traits.Type = Leader_Traits.TraitType 
										 WHERE Leader_Traits.LeaderType = ? LIMIT 1]]);
	local populateUniqueBonuses = PopulateUniqueBonuses_CreateCached();
										 
													 
	if(g_bIsScenario) then
		local civList = UI.GetMapPlayers(PreGame.GetMapScript());
		print(civList);
		if(civList ~= nil) then
			
			local sql = [[	SELECT 
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
						]];
						
			local scenarioCivQuery = DB.CreateQuery(sql);
					
			local civEntries = {};
			
			for i, v in pairs(civList) do
				if(v.Playable) then
					for row in scenarioCivQuery(v.CivType) do
						table.insert(civEntries, {Locale.Lookup(row.LeaderDescription), row, i - 1});
					end
				end
			end
			
			-- Sort by leader description;
			table.sort(civEntries, function(a, b) return Locale.Compare(a[1], b[1]) == -1 end);
			
			for i,v in ipairs(civEntries) do
				local row = v[2];
				local scenarioCivID = v[3];
				AddCivilizationEntry(traitsQuery, populateUniqueBonuses, row, row.LeaderType, row.LeaderDescription, row.LeaderPortraitIndex, row.LeaderIconAtlas, scenarioCivID);
			end	
		end
	else
		AddRandomCivilizationEntry();
					
		local civEntries = {}; 
			
		local sql = [[	SELECT 
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
					]];
		for row in DB.Query(sql) do
			table.insert(civEntries, {Locale.Lookup(row.LeaderDescription), row});
		end
		
		-- Sort by leader description;
		table.sort(civEntries, function(a, b) return Locale.Compare(a[1], b[1]) == -1 end);
		
		for i,v in ipairs(civEntries) do
			local row = v[2];
			AddCivilizationEntry(traitsQuery, populateUniqueBonuses, row, row.LeaderType, row.LeaderDescription, row.LeaderPortraitIndex, row.LeaderIconAtlas);
		end
	end

	Controls.Stack:CalculateSize();
	Controls.Stack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
end

Events.AfterModsActivate.Add(function()
	g_bRefreshCivs = true;
end);

Events.AfterModsDeactivate.Add(function()
	g_bRefreshCivs = true;
end);
