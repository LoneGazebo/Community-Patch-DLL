if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Select Speed
-------------------------------------------------
include( "IconSupport" );

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
-- set the Speed
----------------------------------------------------------------        
function SpeedSelected( id )
    PreGame.SetGameSpeed( id );
    OnBack();
end


----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
        Controls.ScrollPanel:SetScrollValue( 0 );
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------        
-- build the buttons
----------------------------------------------------------------  
local m_sortSpeeds = {}      
for info in GameInfo.GameSpeeds() do
	table.insert(m_sortSpeeds, { ID=info.ID, GrowthPercent=info.GrowthPercent, IconAtlas=info.IconAtlas, Description=info.Description, Help=info.Help, PortraitIndex=info.PortraitIndex} );
end

-- Sort the options
table.sort(m_sortSpeeds, function(a, b) 
	if(a.GrowthPercent == b.GrowthPercent) then
		return a.Description < b.Description; 
	else
		return a.GrowthPercent < b.GrowthPercent;
	end
end);


for i, info in ipairs(m_sortSpeeds) do
	local controlTable = {};
    ContextPtr:BuildInstanceForControlAtIndex( "ItemInstance", controlTable, Controls.Stack, i-1 );
    IconHookup( info.PortraitIndex, 64, info.IconAtlas, controlTable.Icon );
    controlTable.Help:SetText( Locale.ConvertTextKey( info.Help ) );
    controlTable.Name:SetText( Locale.ConvertTextKey( info.Description ) );
    controlTable.Button:SetToolTipString( Locale.ConvertTextKey( info.Help ) );
    controlTable.Button:SetVoid1( info.ID );
    controlTable.Button:RegisterCallback( Mouse.eLClick, SpeedSelected );
end

Controls.Stack:CalculateSize();
Controls.Stack:ReprocessAnchoring();
Controls.ScrollPanel:CalculateInternalSize();