if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Replay
-------------------------------------------------
include( "InstanceManager" );

----------------------------------------------------------------
-- Globals
----------------------------------------------------------------        
g_ReplayMessageInstanceManager = InstanceManager:new("ReplayMessageInstance", "MessageText", Controls.ReplayMessageStack);

----------------------------------------------------------------        
----------------------------------------------------------------

function Refresh()
	print("Refreshing Replay Viewer");
	g_ReplayMessageInstanceManager:ResetInstances();
	
	-- Grab messages and sort by their turn.
	local messages = Game.GetReplayMessages();
	-- table.sort(messages, function(a, b) return a.Turn < b.Turn end);
	
	print(#messages);
		
	for i,message in ipairs(messages) do
		print(message.Text);
		if(message.Text ~= nil and #message.Text > 0) then
			local messageInstance = g_ReplayMessageInstanceManager:GetInstance();
		
			local text = tostring(message.Turn) .. " - " .. message.Text;
		
			messageInstance.MessageText:SetText(text);
		end
	end
	
	Controls.ReplayMessageStack:CalculateSize();
	Controls.ReplayMessageStack:ReprocessAnchoring();
	Controls.ReplayMessageScrollPanel:CalculateInternalSize();
end
-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if(not bIsHide)then
		Refresh();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );



----------------------------------------------------------------        
----------------------------------------------------------------        