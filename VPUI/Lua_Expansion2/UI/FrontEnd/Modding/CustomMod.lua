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
g_ModList = nil;
g_InstanceList = {};
g_iSelected = nil;
g_DynamicContexts = {};

----------------------------------------------------------------
function OnBack()
	fileListRefreshed = false;
	g_ModList = nil;
	UIManager:DequeuePopup( ContextPtr );
end

----------------------------------------------------------------        
-- Event Handlers
----------------------------------------------------------------        
Controls.StartButton:RegisterCallback( Mouse.eLClick, function()	
	local entry = g_ModList[ g_iSelected ];
	if(entry) then
		
		-- Reset PreGame so that any prior settings won't bleed in.
		PreGame.Reset();
		
		local customSetupFile = Modding.GetEvaluatedFilePath(entry.ModID, entry.Version, entry.File);
		
		local filePath = customSetupFile.EvaluatedPath;
		-- Get the absolute path and filename without extension.
		local extension = Path.GetExtension(filePath);
		local path = string.sub(filePath, 1, #filePath - #extension);
		
		local newContext = ContextPtr:LoadNewContext(path);
		newContext:SetHide(true);
		table.insert(g_DynamicContexts, newContext);
		UIManager:QueuePopup(newContext, PopupPriority.CustomMod );
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
ContextPtr:SetUpdate( function()
	if(g_ModList == nil) then
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
		
		local customMod = g_ModList[index];
		Controls.Title:LocalizeAndSetText(customMod.Name);
		Controls.MapDesc:LocalizeAndSetText(customMod.Description);
		Controls.StartButton:SetDisabled(false);
		
		local customImage = Modding.GetModProperty(customMod.ModID, customMod.Version, "Custom_Background_" .. customMod.Name);
		if(customImage ~= nil) then
			local customImageFile = Modding.GetEvaluatedFilePath(customMod.ModID, customMod.Version, customImage);
			
			local filePath = customImage;
			if(customImageFile) then
				local filePath = customImageFile.EvaluatedPath;
			end
			Controls.DetailsBackgroundImage:UnloadTexture();
			Controls.DetailsBackgroundImage:SetTexture(filePath);
		else
			Controls.DetailsBackgroundImage:UnloadTexture();
			Controls.DetailsBackgroundImage:SetTexture(DEFAULT_BACKGROUND);	
		end
		
	else
		Controls.StartButton:SetDisabled(true);
		Controls.DetailsBackgroundImage:UnloadTexture();
		Controls.DetailsBackgroundImage:SetTexture(DEFAULT_BACKGROUND);	
	
		-- Set All Info to nothing
		Controls.Title:LocalizeAndSetText( "TXT_KEY_SELECT_CUSTOM_GAME" );
		Controls.MapDesc:SetText( "" );
    end
end
----------------------------------------------------------------
function SetupFileButtonList()
    SetSelected( nil );
    g_InstanceManager:ResetInstances();
    
    -- build a table of all save file names that we found
    g_ModList = {};
    g_InstanceList = {};
    
    -- build a table of all save file names that we found
	for row in Modding.GetActivatedModEntryPoints("Custom") do
		table.insert(g_ModList, {
			Name = row.Name,
			Description = row.Description,
			File = row.File,
			ModID = row.ModID,
			Version = row.Version,
		});
	end
	table.sort(g_ModList, function(a,b) return a.Name < b.Name end);
	
	Controls.NoGames:SetHide(#g_ModList > 0);
	
	for i, v in ipairs(g_ModList) do
    	local controlTable = g_InstanceManager:GetInstance();
    	g_InstanceList[i] = controlTable;
    	
        controlTable.Button:LocalizeAndSetText( v.Name );
        controlTable.Button:SetVoid1( i );
        controlTable.Button:RegisterCallback( Mouse.eLClick, SetSelected );
	end
	
	Controls.LoadFileButtonStack:CalculateSize();
    Controls.ScrollPanel:CalculateInternalSize();
    Controls.LoadFileButtonStack:ReprocessAnchoring();
end
