if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );

-- the instance managers
local g_TaskItemManager = InstanceManager:new( "TaskEntryInstance", "TaskLabel", Controls.TaskStack );

local g_aTaskStatus = {};
local g_aTaskString = {};
local g_iMaxIndex = -1;

----------------------------------------------------------------        
function OnUpdate( TaskListInfo )
	local iIndex = TaskListInfo.Index;
	if (iIndex > g_iMaxIndex) then
		if (g_iMaxIndex < 0) then
			ContextPtr:SetHide(false);
		end
	
		g_iMaxIndex = iIndex;
	end
	
	g_aTaskStatus[iIndex] = TaskListInfo.TaskStatus;
	g_aTaskString[iIndex] = TaskListInfo.Text;
	
	g_TaskItemManager:ResetInstances();
	for i = 0, g_iMaxIndex do
		if (g_aTaskStatus[i] ~= nil) then
			-- build strings for entry	
			local iStatusType = g_aTaskStatus[i];
			local iOffset = 0;
			if (iStatusType == 0) then
				iOffset = 96;
			elseif (iStatusType == 1) then
				iOffset = 32;
			elseif (iStatusType == 2) then
				iOffset = 0;
			end
				
			local controlTable = g_TaskItemManager:GetInstance();
			controlTable.TaskEntryImage:SetTextureOffsetVal(0, iOffset);
			controlTable.TaskLabel:SetText(g_aTaskString[i]);
		end
	end
	Controls.TaskStack:CalculateSize();
	Controls.TaskListGrid:DoAutoSize();
	
end
Events.TaskListUpdate.Add( OnUpdate );

Controls.TaskStack:CalculateSize();
Controls.TaskListGrid:DoAutoSize();