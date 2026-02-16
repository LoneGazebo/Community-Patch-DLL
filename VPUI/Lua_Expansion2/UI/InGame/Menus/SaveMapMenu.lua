if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );
include( "IconSupport" );
include( "SupportFunctions" );
local g_InstanceManager = InstanceManager:new( "LoadButton", "Button", Controls.LoadFileButtonStack );

g_MapList = {};

local g_IsSingle = true;
local g_IsAuto   = false;
local g_Time = 0;
local g_iSelected = -1;
local g_IsDeletingFile = true;

function OnSave()
	local newSave = Controls.NameBox:GetText();
	local newFileName = true;
	for i, v in ipairs(g_MapList) do
		if(newSave == v.Name) then
			newFileName = false;
			g_IsDeletingFile = false;
			Controls.Message:LocalizeAndSetText("TXT_KEY_OVERWRITE_TXT");
			Controls.DeleteConfirm:SetHide(false);
		end
	end
	
	if(newFileName) then
		UI.SaveMap( Controls.NameBox:GetText() );
		Controls.NameBox:ClearString();
		SetupFileButtonList();
		OnBack();
	end
end
Controls.SaveButton:RegisterCallback( Mouse.eLClick, OnSave );

----------------------------------------------------------------        
----------------------------------------------------------------
function OnEditBoxChange( _, _, bIsEnter )	
	local text = Controls.NameBox:GetText();
	
	if( g_iSelected ~= -1 ) then
		
		g_MapList[g_iSelected].Instance.SelectHighlight:SetHide( true );
		SetSaveInfo(PreGame.GetWorldSize() );
	end
	
	if(not ValidateText(text)) then
		Controls.SaveButton:SetDisabled(true);
	else
		Controls.SaveButton:SetDisabled(false);
	end	
	Controls.Delete:SetDisabled(true); 
	
	if( bIsEnter ) then
	    OnSave();
	end
end
Controls.NameBox:RegisterCallback( OnEditBoxChange )

----------------------------------------------------------------        
----------------------------------------------------------------
function OnDelete()
	g_IsDeletingFile = true;
	Controls.Message:SetText( Locale.ConvertTextKey("TXT_KEY_CONFIRM_TXT") );
	Controls.DeleteConfirm:SetHide(false);
end
Controls.Delete:RegisterCallback( Mouse.eLClick, OnDelete );

----------------------------------------------------------------        
----------------------------------------------------------------
function OnYes( )
	Controls.DeleteConfirm:SetHide(true);
	if(g_IsDeletingFile) then
		UI.DeleteSavedGame( g_MapList[g_iSelected].File );
	else
		UI.SaveMap( Controls.NameBox:GetText() );
		OnBack();
	end
	
	SetupFileButtonList();
	Controls.NameBox:ClearString();
	Controls.SaveButton:SetDisabled(true);
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );
----------------------------------------------------------------        
----------------------------------------------------------------
function OnNo( )
	Controls.DeleteConfirm:SetHide(true);
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );
-------------------------------------------------
-------------------------------------------------
function OnBack()
    UIManager:DequeuePopup( ContextPtr );
    Controls.NameBox:ClearString();
    SetSelected( -1 );
    g_MapList = {};
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

-------------------------------------------------
-------------------------------------------------
function SetSelected( index )

    if( g_iSelected ~= -1 ) then
		g_MapList[g_iSelected].Instance.SelectHighlight:SetHide( true );
    end
    
    g_iSelected = index;
    OnEditBoxChange();
    
    if( g_iSelected ~= -1 ) then
    
		local entry = g_MapList[g_iSelected];
    
		Controls.NameBox:SetText(entry.Name);
        entry.Instance.SelectHighlight:SetHide( false );

		local date = UI.GetSavedGameModificationTime(entry.File);
		SetSaveInfo(UI.GetMapPreview(entry.File).MapSize, date);
		
		Controls.SaveButton:SetDisabled(false);  
		Controls.Delete:SetDisabled(false); 

	else -- No saves are selected
		SetSaveInfo(PreGame.GetWorldSize());
		Controls.Delete:SetDisabled(true);
    end
end
-------------------------------------------------
-------------------------------------------------
function SetSaveInfo(mapSize, date)
	-- Set Save file time
	if(date ~= nil) then
		Controls.TimeSaved:SetText(date);	
	else
		Controls.TimeSaved:SetText("");
	end
	
	-- Sets map type icon and tool tip
	local mapScript = PreGame.GetMapScript();
	 for row in GameInfo.MapScripts{FileName = mapScript} do
		IconHookup( row.IconIndex or 0, 64, row.IconAtlas, Controls.MapType );
		Controls.MapType:SetToolTipString( Locale.ConvertTextKey( row.Name or "" ) );
	end
	
	-- Sets map size icon and tool tip
	local info = GameInfo.Worlds[ mapSize ];
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
	
	-- Set Selected Civ Map
	Controls.LargeMapImage:UnloadTexture();
	local mapTexture="MapRandom512.dds";
	Controls.LargeMapImage:SetTexture(mapTexture);
end
----------------------------------------------------------------        
----------------------------------------------------------------
function ChopFileName(file)
	_, _, chop = string.find(file,"\\.+\\(.+)%."); 
	return chop;
end

----------------------------------------------------------------        
----------------------------------------------------------------
function ValidateText(text)
	local isAllWhiteSpace = true;
	for i = 1, #text, 1 do
		if(string.byte(text, i) ~= 32) then
			isAllWhiteSpace = false;
			break;
		end
	end
	
	if(isAllWhiteSpace) then
		return false;
	end
	
	-- don't allow % character
	for i = 1, #text, 1 do
		if string.byte(text, i) == 37 then
			return false;
		end
	end
	
	local invalidCharArray = { '\"', '<', '>', '|', '\b', '\0', '\t', '\n', '/', '\\', '*', '?', ':' };

	for i, ch in ipairs(invalidCharArray) do
		if(string.find(text, ch) ~= nil) then
			return false;
		end
	end
	
	-- don't allow control characters
	for i = 1, #text, 1 do
		if (string.byte(text, i) < 32) then
			return false;
		end
	end

	return true;
end

----------------------------------------------------------------        
----------------------------------------------------------------
function SetupFileButtonList()
	SetSelected( -1 );
    g_InstanceManager:ResetInstances();
    
    -- build a table of all save file names that we found
    g_MapList = {};
	for i, v in ipairs(Modding.GetMapFiles()) do
		print(v.File);
		if(string.match(v.File, "^Assets") == nil) then
			local name;
			if(v.Name ~= nil) then
				name = Locale.ConvertTextKey(v.Name);
			else
				name = ChopFileName(v.File);
			end
			
			local description = "";
			if(v.Description ~= nil) then
				description = Locale.ConvertTextKey(v.Description);
			end
			
			table.insert(g_MapList, {
				File = v.File,
				Name = name,
				Description = description,
			});
		end
	end
   
	for i, v in ipairs(g_MapList) do
    	local controlTable = g_InstanceManager:GetInstance();
    	
    	g_MapList[i].Instance = controlTable;
    	        
		controlTable.ButtonText:SetText(v.Name);
		controlTable.ButtonText:SetToolTipString(v.Description);
        controlTable.Button:SetVoid1( i );
        controlTable.Button:RegisterCallback( Mouse.eLClick, SetSelected );
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
function ShowHideHandler( isHide )
    if( not isHide ) then
        g_IsSingle = not Game:IsNetworkMultiPlayer();
        Controls.NameBox:TakeFocus();
		SetupFileButtonList();
		OnEditBoxChange();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );