if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Select Map Size
-------------------------------------------------
include( "IconSupport" );

g_RandomSizeControl = {};
g_WorldSizeControls = {};

-------------------------------------------------
-------------------------------------------------
function OnBack()
    ContextPtr:SetHide( true );
    ContextPtr:LookUpControl( "../MainSelection" ):SetHide( false );
    ContextPtr:LookUpControl( ".." ):SetHide( false );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


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
-- set the size
----------------------------------------------------------------        
function SizeSelected( id )

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
					
					PreGame.SetEra(wb.StartEra);
					PreGame.SetGameSpeed(wb.DefaultSpeed);
					PreGame.SetMaxTurns(wb.MaxTurns);
					PreGame.SetNumMinorCivs(wb.CityStateCount);
					PreGame.SetRandomWorldSize(false);
					PreGame.SetWorldSize(wb.MapSize);
					PreGame.SetRandomWorldSize(false);
					PreGame.SetNumMinorCivs(-1);
					
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
    end
    OnBack();
end


----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
        Controls.ScrollPanel:SetScrollValue( 0 );
       
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
        
        if(mapFilter == nil) then
			g_RandomSizeControl.Root:SetHide(false);
			for k,v in pairs(g_WorldSizeControls) do
				v.Root:SetHide(false);
			end
        
        else
			g_RandomSizeControl.Root:SetHide(false);
				
			for k,v in pairs(g_WorldSizeControls) do
				v.Root:SetHide(true);
			end
			
			for row in GameInfo.Map_Sizes{MapType = mapFilter} do
				g_WorldSizeControls[row.WorldSizeType].Root:SetHide(false);
			end
		
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


----------------------------------------------------------------        
-- build the buttons
----------------------------------------------------------------        
ContextPtr:BuildInstanceForControl( "ItemInstance", g_RandomSizeControl, Controls.Stack );
IconHookup( 6, 64, "WORLDSIZE_ATLAS", g_RandomSizeControl.Icon );
if(not PreGame.IsMultiplayerGame()) then
	g_RandomSizeControl.Name:SetText( Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SIZE" ) );
	g_RandomSizeControl.Button:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_RANDOM_MAP_SIZE_HELP" ) );
else
	g_RandomSizeControl.Name:SetText( Locale.ConvertTextKey( "TXT_KEY_ANY_MAP_SIZE" ) );
	g_RandomSizeControl.Button:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ANY_MAP_SIZE_HELP" ) );
end

g_RandomSizeControl.Button:SetVoid1( -1 );
g_RandomSizeControl.Button:RegisterCallback( Mouse.eLClick, SizeSelected );

for info in GameInfo.Worlds() do
    local controlTable = {};
    ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, Controls.Stack );    
    IconHookup( info.PortraitIndex, 64, info.IconAtlas, controlTable.Icon );
    controlTable.Name:SetText( Locale.ConvertTextKey( info.Description ) );
    controlTable.Button:SetToolTipString( Locale.ConvertTextKey( info.Help ) );
    controlTable.Button:SetVoid1( info.ID );
    controlTable.Button:RegisterCallback( Mouse.eLClick, SizeSelected );
    
    g_WorldSizeControls[info.Type] = controlTable;
end

Controls.Stack:CalculateSize();
Controls.Stack:ReprocessAnchoring();
Controls.ScrollPanel:CalculateInternalSize();