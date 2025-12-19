if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
----------------------------------------------------------------
----------------------------------------------------------------
local g_iAIPlayer = -1;
local g_iAITeam = -1;
local g_CurrentTopic = -1;

----------------------------------------------------------------
----------------------------------------------------------------
function OnBack()
    ContextPtr:SetHide( true );
    ContextPtr:LookUpControl( "../DiscussionOptions" ):SetHide( false );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------        
----------------------------------------------------------------        
function LeaderMessageHandler( iPlayer, iDiploUIState, szLeaderMessage, iAnimationAction, iData1 )
    
    g_iAIPlayer = iPlayer;
    g_iAITeam = Players[g_iAIPlayer]:GetTeam();
    
end
Events.AILeaderMessage.Add( LeaderMessageHandler );


----------------------------------------------------------------        
----------------------------------------------------------------        
local bEverShown = false;
function OnShowHide( bHide )
	if (not bHide) then
	
		bEverShown = true;

	end
end
ContextPtr:SetShowHideHandler( OnShowHide );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
            OnBack();
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );

