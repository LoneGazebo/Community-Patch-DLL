if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );

----------------------------------------------------------------
-- Global Constants
----------------------------------------------------------------
DEFAULT_BACKGROUND = "MapRandom512.dds";

----------------------------------------------------------------        
-- Global Variables
----------------------------------------------------------------     
g_InstanceManager = InstanceManager:new( "LoadButton", "Button", Controls.LoadFileButtonStack );
g_ScenarioList = nil;
g_InstanceList = {};
g_iSelected = nil;
g_DynamicContexts = {};

----------------------------------------------------------------
function OnBack()
	fileListRefreshed = false;
	g_ScenarioList = nil;
	
	Modding.DeactivateMods();
	UIManager:DequeuePopup( ContextPtr );
end

----------------------------------------------------------------        
-- Event Handlers
----------------------------------------------------------------        
Controls.StartButton:RegisterCallback( Mouse.eLClick, function()	
	local entry = g_ScenarioList[ g_iSelected ];
	if(entry) then
		UIManager:SetUICursor( 1 );
		Modding.ActivateSpecificMod(entry.ModID, entry.Version);
		
		local customSetupFile = Modding.GetEvaluatedFilePath(entry.ModID, entry.Version, entry.File);
	
		local filePath = customSetupFile.EvaluatedPath;
		-- Get the absolute path and filename without extension.
		local extension = Path.GetExtension(filePath);
		local path = string.sub(filePath, 1, #filePath - #extension);
		
		-- Do not save these settings out for the "Play Now" option.
		PreGame.SetPersistSettings(false);
		
		-- Reset PreGame so that any prior settings won't bleed in.
		PreGame.Reset();

		-- Send out an event to continue on to the scenario popup.  We will send this whether or not
		-- the mod switched the UIs 		
		Events.SystemUpdateUI( SystemUpdateUIType.RestoreUI, "ScenariosMenu", path );
		
		UIManager:SetUICursor( 0 );
	end
end);
----------------------------------------------------------------       
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack);
----------------------------------------------------------------       
ContextPtr:SetInputHandler( function(uiMsg, wParam, lParam)
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
			OnBack();
        end
    end
    return true;
end);
----------------------------------------------------------------   
ContextPtr:SetShowHideHandler(function(isHide)
    if( not isHide ) then
		if (not ContextPtr:IsHotLoad()) then
			UIManager:SetUICursor( 1 );
			Modding.ActivateDLC();
			PreGame.LoadPreGameSettings();
			UIManager:SetUICursor( 0 );
			
			-- Send out an event to continue on, as the ActivateDLC may have swapped out the UI	
			Events.SystemUpdateUI( SystemUpdateUIType.RestoreUI, "ScenariosMenuReset" );
		end
		SetupFileButtonList();
	end
end);
----------------------------------------------------------------     
function SetSelected( index )
    if(g_iSelected ~= nil) then
        g_InstanceList[ g_iSelected ].SelectHighlight:SetHide( true );
    end
    
    g_iSelected = index;
        
    if(index ~= nil) then
		g_InstanceList[ index ].SelectHighlight:SetHide( false );
		
		local customMod = g_ScenarioList[index];
		Controls.Title:SetText(customMod.DisplayName);
		Controls.MapDesc:SetText(customMod.DisplayDescription);
		
		local customImage = Modding.GetModProperty(customMod.ModID, customMod.Version, "Custom_Background_" .. customMod.Name);
		if(customImage ~= nil) then
			local filePath = customImage; -- Default to image file.
			
			local customImageFile = Modding.GetEvaluatedFilePath(customMod.ModID, customMod.Version, customImage);
			if(customImageFile ~= nil) then
				filePath = customImageFile.EvaluatedPath;
			end
			
			Controls.DetailsBackgroundImage:UnloadTexture();
			Controls.DetailsBackgroundImage:SetTexture(filePath);
			
			local foregroundImage = Modding.GetModProperty(customMod.ModID, customMod.Version, "Custom_Foreground_" .. customMod.Name);
			if(foregroundImage ~= nil) then
				local foregroundImagePath = foregroundImage; -- Default to image file.
			
				local foregroundImageFile = Modding.GetEvaluatedFilePath(customMod.ModID, customMod.Version, customImage);
				if(foregroundImageFile ~= nil) then
					foregroundImagePath = foregroundImageFile.EvaluatedPath;
				end
				
				Controls.DetailsForegroundImage:UnloadTexture();
				Controls.DetailsForegroundImage:SetTexture(foregroundImagePath);
				Controls.DetailsForegroundImage:SetHide(false);
			else
				Controls.DetailsForegroundImage:SetHide(true);
			end
			
		else
			Controls.DetailsBackgroundImage:UnloadTexture();
			Controls.DetailsBackgroundImage:SetTexture(DEFAULT_BACKGROUND);	
			Controls.DetailsForegroundImage:SetHide(true);
		end
		
		local tooltips = {
			[1] = "TXT_KEY_MODDING_MOD_BLOCKED_BY_OTHER_MOD",
			[2] = "TXT_KEY_MODDING_MOD_VERSION_ALREADY_ENABLED",
			[3] = "TXT_KEY_MODDING_MOD_MISSING_DEPENDENCIES",
			[4] = "TXT_KEY_MODDING_MOD_HAS_EXCLUSIVITY_CONFLICTS",
			[5] = "TXT_KEY_MODDING_MOD_BAD_GAMEVERSION",
		};
		
		--Test whether we can enable this mod.
		local canEnableStatus = Modding.CanEnableMod(customMod.ModID, customMod.Version);
		local tooltip = tooltips[canEnableStatus];
		if(tooltip ~= nil) then
			tooltip = Locale.ConvertTextKey(tooltip);
		end
			
		Controls.StartButton:SetDisabled(canEnableStatus > 0);
		Controls.StartButton:SetToolTipString(tooltip);  
	
	else
		Controls.StartButton:SetDisabled(true);
		Controls.DetailsBackgroundImage:UnloadTexture();
		Controls.DetailsBackgroundImage:SetTexture(DEFAULT_BACKGROUND);
		Controls.DetailsForegroundImage:SetHide(true);	
		
	
		-- Set All Info to nothing
		Controls.Title:LocalizeAndSetText( "TXT_KEY_SELECT_SCENARIO" );
		Controls.MapDesc:SetText( "" );
    end
end
----------------------------------------------------------------
function SetupFileButtonList()
    SetSelected( nil );
    g_InstanceManager:ResetInstances();
    
    -- build a table of all save file names that we found
    g_ScenarioList = {};
    g_InstanceList = {};
    
    -- build a table of all save file names that we found
	for row in Modding.GetInstalledFiraxisScenarios() do
		local displayName = Locale.ConvertTextKey(row.Name);
		local displayDescription = row.Description;
		if(row.Description ~= nil) then
			displayDescription = Locale.ConvertTextKey(row.Description);
		end
	
		table.insert(g_ScenarioList, {
			Name = row.Name,
			DisplayName = displayName,
			Description = row.Description,
			DisplayDescription = displayDescription,
			File = row.File,
			ModID = row.ModID,
			Version = row.Version,
		});
	end
	
	table.sort(g_ScenarioList, function(a,b) return Locale.Compare(a.DisplayName, b.DisplayName) == -1 end);
	
	Controls.NoGames:SetHide(#g_ScenarioList > 0);
	
	for i, v in ipairs(g_ScenarioList) do
    	local controlTable = g_InstanceManager:GetInstance();
    	g_InstanceList[i] = controlTable;
    	
        controlTable.Button:SetText( v.DisplayName );
        controlTable.Button:SetVoid1( i );
        controlTable.Button:RegisterCallback( Mouse.eLClick, SetSelected );
	end
	
	Controls.LoadFileButtonStack:CalculateSize();
    Controls.ScrollPanel:CalculateInternalSize();
    Controls.LoadFileButtonStack:ReprocessAnchoring();
end

-------------------------------------------------------------------------------
function OnSystemUpdateUI( type, tag, iData1, iData2, strData1  )
    if( type == SystemUpdateUIType.RestoreUI and tag == "ScenariosMenu") then
		-- Restore after a UI reset
		local newContext = ContextPtr:LoadNewContext(strData1);		-- The context name is in the strData1 field.
		newContext:SetHide(true);
		table.insert(g_DynamicContexts, newContext);
		UIManager:QueuePopup(newContext, PopupPriority.CustomMod );
	end
end

Events.SystemUpdateUI.Add( OnSystemUpdateUI );
