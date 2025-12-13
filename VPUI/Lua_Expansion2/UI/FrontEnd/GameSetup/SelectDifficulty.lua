if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Select Difficulty
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
-- set the Difficulty
----------------------------------------------------------------        
function DifficultySelected( id )
    PreGame.SetHandicap( 0, id );
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
for info in GameInfo.HandicapInfos() do
	if ( info.Type ~= "HANDICAP_AI_DEFAULT" ) then
		local controlTable = {};
		ContextPtr:BuildInstanceForControl( "ItemInstance", controlTable, Controls.Stack );
		IconHookup( info.PortraitIndex, 64, info.IconAtlas, controlTable.Icon );
		controlTable.Help:SetText( Locale.ConvertTextKey( info.Help ) );
		controlTable.Name:SetText( Locale.ConvertTextKey( info.Description ) );
		controlTable.Button:SetToolTipString( Locale.ConvertTextKey( info.Help ) );
		controlTable.Button:SetVoid1( info.ID );
		controlTable.Button:RegisterCallback( Mouse.eLClick, DifficultySelected );
    end
end

Controls.Stack:CalculateSize();
Controls.Stack:ReprocessAnchoring();
Controls.ScrollPanel:CalculateInternalSize();