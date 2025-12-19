if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );
local g_InstanceManager = InstanceManager:new( "LeaderButtonInstance", "LeaderButton", Controls.ButtonStack );
local civListPopulated = false;

-------------------------------------------------
-- Back Button Handler
-------------------------------------------------
function BackButtonClick()
	ContextPtr:SetHide( true );
	ContextPtr:LookUpControl( "../MainMenuScreenUI" ):SetHide( false );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, BackButtonClick );


-------------------------------------------------
-- Input Handler
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			ContextPtr:SetHide( true );
			ContextPtr:LookUpControl( "../MainMenuScreenUI" ):SetHide( false );
		end
	end
	-- TODO: Is this needed?
	return true;
end
ContextPtr:SetInputHandler( InputHandler );

function populateCivList()
	-- iterate through civ info's, get leader information for
	-- playable leaders, cache selection in pregame.cpp
end

function OnUpdate()
	if not civListPopulated then
		populateCivList();
		civListPopulated = true;
	end
end
ContextPtr:SetUpdate( OnUpdate );
