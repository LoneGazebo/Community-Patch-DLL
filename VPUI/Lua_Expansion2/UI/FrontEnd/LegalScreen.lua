if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-------------------------------------------------
Controls.ContinueButton:RegisterCallback(Mouse.eLClick, function()
	UIManager:DequeuePopup(ContextPtr);
end);
