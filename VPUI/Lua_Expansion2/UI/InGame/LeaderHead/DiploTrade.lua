if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
----------------------------------------------------------------        
----------------------------------------------------------------        
include( "TradeLogic" );

Events.AILeaderMessage.Add( LeaderMessageHandler );


Controls.WhatWillMakeThisWorkButton:RegisterCallback( Mouse.eLClick, OnEqualizeDeal );
Controls.WhatWillEndThisWarButton:RegisterCallback( Mouse.eLClick, OnEqualizeDeal );
Controls.WhatConcessionsButton:RegisterCallback( Mouse.eLClick, OnEqualizeDeal );
Controls.WhatDoYouWantButton:RegisterCallback( Mouse.eLClick, OnWhatDoesAIWant );
Controls.WhatWillYouGiveMeButton:RegisterCallback( Mouse.eLClick, OnWhatWillAIGive );

ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnLeavingLeader()
    UIManager:DequeuePopup( ContextPtr );
end
Events.LeavingLeaderViewMode.Add( OnLeavingLeader );

local sgPrevCursor = 0;
----------------------------------------------------------------        
----------------------------------------------------------------  
function OnShowHide( bHide )
	if(not bHide) then 
		sgPrevCursor = UIManager:SetUICursor(0);
		LuaEvents.OpenAILeaderDiploTrade();
	else
		UIManager:SetUICursor(sgPrevCursor);
	end
end
ContextPtr:SetShowHideHandler( OnShowHide );

----------------------------------------------------------------        
----------------------------------------------------------------  
function OnOpenSimpleDiploTrade()
	-- An human trade was started while we're open, close ourselves.
	if (not ContextPtr:IsHidden()) then
		UIManager:DequeuePopup( ContextPtr );
	end
end
LuaEvents.OpenSimpleDiploTrade.Add(OnOpenSimpleDiploTrade);
