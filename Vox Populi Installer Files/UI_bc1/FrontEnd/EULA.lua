-- modified by Temudjin from 1.0.3.142 code
-------------------------------------------------
-- EULA Screen
-------------------------------------------------

---------- Temudjin START
g_HasAcceptedEULA = true;
---------- Temudjin END


function OnAccept()
	g_HasAcceptedEULA = true;
	NavigateForward();
end

--------------------------------------------------
-- Navigation Routines
--------------------------------------------------
function NavigateBack()
	UIManager:DequeuePopup( ContextPtr );
end

function NavigateForward()
	UIManager:DequeuePopup(	ContextPtr );
	UIManager:QueuePopup( Controls.ModsBrowser, PopupPriority.ModsBrowserScreen );
end

--------------------------------------------------
-- Explicit Event Handlers
--------------------------------------------------
ContextPtr:SetInputHandler(function(uiMsg, wParam, lParam)

	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			NavigateBack();
		end
	end

	return true;
end);

ContextPtr:SetShowHideHandler(function(isHide)
    --UNCOMMENT THIS BLOCK IF YOU WISH TO ONLY 
    --SHOW THE EULA ONCE PER APPLICATION RUN
    --if not isHide and g_HasAcceptedEULA then
	--	NavigateForward();
	--end
	
	--if not isHide and g_QueueEulaToHide then
	--	NavigateBack();
	--end
	---------- Temudjin START
	if not isHide then
		NavigateForward();
	end
	---------- Temudjin END
end);

-- For now, we don't need to track any sort of acknowledgement of the policy.
Controls.AcceptButton:RegisterCallback( Mouse.eLClick, OnAccept);
Controls.DeclineButton:RegisterCallback( Mouse.eLClick, NavigateBack);
