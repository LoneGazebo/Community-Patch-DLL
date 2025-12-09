if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );
local g_InstanceManager = InstanceManager:new( "LoadButton", "Button", Controls.LoadFileButtonStack );
local g_iSelected = -1;
local g_InstanceList = {};

bStartingGame = false;

local Intro = {
	Name = "TXT_KEY_TUTORIAL_INSTRUCT",
	Num = "TXT_KEY_TUTORIAL0",
	Title = "TXT_KEY_TUTORIAL0_TITLE_ONLY",
	Desc = "TXT_KEY_TUTORIAL0_DESC",
	Image = Controls.TutorialIcon0;
	Map = "",	
}

-- Previously, tutorials were obtained via a C++ method.
-- Keeping it all in Lua makes it more flexible :)
local Tutorials = {	
	{
		Name = "TXT_KEY_TUTORIAL1_TITLE",
		Num = "TXT_KEY_TUTORIAL1",
		Title = "TXT_KEY_TUTORIAL1_TITLE_ONLY",
		Desc = "TXT_KEY_TUTORIAL1_DESC",
		Image = Controls.TutorialIcon1;
		Map = "Tutorials/Maps/Tutorial_1_MovementAndExploration.Civ5Map",	
	},
	
	{
		Name = "TXT_KEY_TUTORIAL2_TITLE",
		Num = "TXT_KEY_TUTORIAL2",
		Title = "TXT_KEY_TUTORIAL2_TITLE_ONLY",
		Desc = "TXT_KEY_TUTORIAL2_DESC",
		Image = Controls.TutorialIcon2;
		Map = "Tutorials/Maps/Tutorial_2_FoundCities.Civ5Map",
	},
	
	{
		Name = "TXT_KEY_TUTORIAL3_TITLE",
		Num = "TXT_KEY_TUTORIAL3",
		Title = "TXT_KEY_TUTORIAL3_TITLE_ONLY",
		Desc = "TXT_KEY_TUTORIAL3_DESC",
		Image = Controls.TutorialIcon3;
		Map = "Tutorials/Maps/Tutorial_3_ImprovingCities.Civ5Map",
	},
	
	{
		Name="TXT_KEY_TUTORIAL4_TITLE",
		Num = "TXT_KEY_TUTORIAL4",
		Title = "TXT_KEY_TUTORIAL4_TITLE_ONLY",
		Desc = "TXT_KEY_TUTORIAL4_DESC",
		Image = Controls.TutorialIcon4;
		Map = "Tutorials/Maps/Tutorial_4_CombatAndConquest.Civ5Map",
	},
	
	{
		Name="TXT_KEY_TUTORIAL5_TITLE",
		Num = "TXT_KEY_TUTORIAL5",
		Title = "TXT_KEY_TUTORIAL5_TITLE_ONLY",
		Desc = "TXT_KEY_TUTORIAL5_DESC",
		Image = Controls.TutorialIcon5;
		Map = "Tutorials/Maps/Tutorial_5_Diplomacy.Civ5Map",
	}
}

g_TutorialEntries = {};


function SetCivilization(playerID, civilizationType)
	local civ = GameInfo.Civilizations[civilizationType];
	if(civ and civ.Playable == 1) then
		PreGame.SetCivilization(playerID, civ.ID);
	else
		print("Could not find civilization! " .. civilizationType);
		print("Setting player to random civ");
		PreGame.SetCivilization(playerID, -1);
	end
end

-------------------------------------------------
-------------------------------------------------
function OnBack()

	--It's possible that the key press was cached and OnBack was later called AFTER the game has been started.
	--This sanity check prevents that from happening.
	if(not bStartingGame) then
		UIManager:DequeuePopup( ContextPtr );
	end
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

-------------------------------------------------
-------------------------------------------------
function OnStart()
	bStartingGame = true;
	
	PreGame.SetPersistSettings(false); -- Do not save these settings out for the "Play Now" option.
	PreGame.SetEra(0);
	PreGame.ResetGameOptions();
	PreGame.ResetMapOptions();
	PreGame.SetRandomMapScript(false);
	PreGame.SetWorldSize(0);
	PreGame.SetCivilization(-1); -- random
	PreGame.SetGameSpeed(2); -- standard
	PreGame.SetHandicap(0, 0);
	
	if (g_iSelected == -1) then
		PreGame.SetMapScript("Assets\\Maps\\Pangaea.lua");
		
		local tutorialModId = "df3333a4-44be-4fc3-9143-21706ff451d5"
		local tutorialModVersion = Modding.GetLatestInstalledModVersion(tutorialModId);
		
		Modding.ActivateSpecificMod(tutorialModId, tutorialModVersion);
		Modding.PerformActions("OnTutorial0");	
		Modding.PostProcessModActions();
	else
		OnLoadMap(g_iSelected);	
	end
	
	Events.SerialEventStartGame();
	UIManager:SetUICursor( 1 );
end
Controls.StartButton:RegisterCallback( Mouse.eLClick, OnStart );

-- Any actions that you want to perform before a custom map is loaded
function OnLoadMap(index)
	print("Tutorial - Selected " .. index);
		
	local tutorial = Tutorials[index];
	
	if(index == -1) then
		tutorial = Intro;
	end
	
	if(tutorial == nil) then
		error("SOMETHING BAD HAPPENED");
	end

	local tutorialModId = "df3333a4-44be-4fc3-9143-21706ff451d5"
	local tutorialModVersion = Modding.GetLatestInstalledModVersion(tutorialModId);
	
	Modding.ActivateSpecificMod(tutorialModId, tutorialModVersion);
	Modding.PerformActions("OnTutorial" .. index);	
	Modding.PostProcessModActions();
	
	PreGame.SetMapScript(tutorial.Map);
	PreGame.SetLoadWBScenario(true);
	PreGame.ResetSlots();
	PreGame.ResetGameOptions();
	PreGame.ResetMapOptions();
	UI.ResetScenarioPlayerSlots();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function SetSelected( index)
    if( g_iSelected ~= -1 ) then
        g_InstanceList[ g_iSelected ].SelectHighlight:SetHide( true );
        Tutorials[ g_iSelected ].Image:SetHide( true );
    else
    	Controls.LearnSelectHighlight:SetHide( true );
		Intro.Image:SetHide( true );
    end
    
    g_iSelected = index;
    
    if( g_iSelected ~= -1 ) then
        g_InstanceList[ g_iSelected ].SelectHighlight:SetHide( false );
        Controls.TutorialNumber:SetText(  Locale.ConvertTextKey(Tutorials[ g_iSelected ].Num) );
        Controls.TutorialTitle:SetText(  Locale.ConvertTextKey(Tutorials[ g_iSelected ].Title) ); 
        Controls.TutorialDesc:SetText(  Locale.ConvertTextKey(Tutorials[ g_iSelected ].Desc) );
        Tutorials[ g_iSelected ].Image:SetHide( false );
    else
		Controls.LearnSelectHighlight:SetHide( false );
        Controls.TutorialNumber:SetText(  Locale.ConvertTextKey(Intro.Num) );
        Controls.TutorialTitle:SetText(  Locale.ConvertTextKey(Intro.Title) ); 
        Controls.TutorialDesc:SetText(  Locale.ConvertTextKey(Intro.Desc) );
        Intro.Image:SetHide( false );
    end
end

function SetupFileButtonList()
    g_InstanceManager:ResetInstances();
    g_InstanceList = {};
    
    -- Load and default select intro button
    Controls.LearnSelectHighlight:SetHide( false );
    Controls.TutorialNumber:SetText(  Locale.ConvertTextKey(Intro.Num) );
    Controls.TutorialTitle:SetText(  Locale.ConvertTextKey(Intro.Title) ); 
    Controls.TutorialDesc:SetText(  Locale.ConvertTextKey(Intro.Desc) );
    Intro.Image:SetHide( false );
    Controls.LearnButton:RegisterCallback( Mouse.eLClick, function() SetSelected(-1) end);
    
    -- Load tutorial buttons
	for i, v in ipairs(Tutorials) do
    	local controlTable = g_InstanceManager:GetInstance();
		g_InstanceList[i] = controlTable;
		
        controlTable.Button:SetText( Locale.ConvertTextKey(v.Name) );
        controlTable.Button:RegisterCallback( Mouse.eLClick, function() SetSelected(i) end);
        
        table.insert(g_TutorialEntries, controlTable);
  	end
	
	Controls.LoadFileButtonStack:CalculateSize();
    Controls.LoadFileButtonStack:ReprocessAnchoring();
    Controls.ScrollPanel:CalculateInternalSize();
end


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            OnBack();
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );

function ShowHideHandler( bIsHide )
	
	local TutorialID = "df3333a4-44be-4fc3-9143-21706ff451d5";
    local latestTutorialVersion = Modding.GetLatestInstalledModVersion(TutorialID);
    
    local modUserData = Modding.OpenUserData(TutorialID, latestTutorialVersion);
    
	for i,v in ipairs(g_TutorialEntries) do

		-- Look up a completed state for tutorial (ex: Tutorial1Completed)
        local completed = modUserData.GetValue("Tutorial".. i .. "Completed");
        v.CompletedIcon:SetHide(completed ~= 1);
        v.NotCompletedIcon:SetHide(completed == 1);

	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

SetupFileButtonList();