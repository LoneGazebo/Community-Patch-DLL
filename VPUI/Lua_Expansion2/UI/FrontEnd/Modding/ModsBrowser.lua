if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
---------------------------------------------------
---- Mods Browser Screen
---------------------------------------------------
include("InstanceManager");

--------------------------------------------------
-- Navigation Routines (Installed,Online,Back)
--------------------------------------------------
function NavigateBack()
	local args = {};
	LuaEvents.OnModsBrowserNavigateBack(args);
	
	if(not args.Cancel) then
		UIManager:DequeuePopup(ContextPtr);
	end
end

--------------------------------------------------
-- Explicit Event Handlers
--------------------------------------------------
function InputHandler(uiMsg, wParam, lParam)

	if (uiMsg == KeyEvents.KeyDown and wParam == Keys.VK_ESCAPE) then
		NavigateBack();
	end

	return true;
end

-------------------------------------------------
-- Global Actions
-------------------------------------------------
-- Context Event Registration
ContextPtr:SetInputHandler(InputHandler);

ContextPtr:SetShowHideHandler(function(bHiding)
	if(not bHiding) then
		Controls.SmallButton2:SetHide(not Steam.IsOverlayEnabled());
	end
end);

-- Control Event Registration
Controls.BackButton:RegisterCallback(Mouse.eLClick, NavigateBack);

function OnWorkshopButtonClicked()
	Steam.ActivateGameOverlayToWebPage("http://steamcommunity.com/workshop/browse?appid=8930");
end

function OnNextButtonClicked()
	UIManager:SetUICursor( 1 );
	Modding.ActivateEnabledMods();	
	UIManager:SetUICursor( 0 );
	
	-- Send out an event to continue on to the scenario popup.  We will send this whether or not
	-- the mod switched the UIs 	
	UIManager:QueuePopup(Controls.ModsMenu, PopupPriority.ModsMenuScreen);
	Events.SystemUpdateUI( SystemUpdateUIType.RestoreUI, "ModsMenu");	
	
end

--------------------------------------------------
-- Custom Events
--------------------------------------------------
Controls.SmallButton1:RegisterCallback(Mouse.eLClick, function() LuaEvents.OnModBrowserDeleteButtonClicked() end);
Controls.SmallButton2:RegisterCallback(Mouse.eLClick, function() OnWorkshopButtonClicked() end);

Controls.LargeButton:RegisterCallback(Mouse.eLClick, function() OnNextButtonClicked() end);

function SetButtonState(button, label, visible, enabled, caption, tooltip)
	button:SetHide(not visible);
	button:SetDisabled(not enabled);
	if(caption ~= nil) then
		label:SetText(caption);
	end
	
	button:SetToolTipString(tooltip);
end

LuaEvents.ModBrowserSetDeleteButtonState.Add(function(...)
	SetButtonState(Controls.SmallButton1, Controls.SmallButton1Label, ...);
end);
