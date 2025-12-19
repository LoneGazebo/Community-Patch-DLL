if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "InstanceManager" );

local g_PlayerSliceInfoIM = InstanceManager:new( "PlayerSliceInfoInstance", "PlayerSliceInfoBox", Controls.PlayerSliceInfoStack );
local PlayerSliceInfos = {};

---------------------------------------------------------------------
function BuildPlayerSliceList()
	g_PlayerSliceInfoIM:ResetInstances();
	PlayerSliceInfos = {};
	
	for i = 1, 8 do
		if (PreGame.GetSlotStatus( i-1 ) == SlotStatus.SS_TAKEN) then
			local item = g_PlayerSliceInfoIM:GetInstance();
			item.Name:SetText(PreGame.GetNickName(i-1));
			PlayerSliceInfos[i] = item;
		else
			PlayerSliceInfos[i] = nil;
		end		
	end
	
	UpdatePlayerSliceList();
end

---------------------------------------------------------------------
function UpdatePlayerSliceList()
	for i = 1, 8 do
		local item = PlayerSliceInfos[i];
		if (item ~= nil) then
			local playerInfo = Network.GetPlayerTurnSliceInfo(i - 1);
			item.SliceShortest:SetText(playerInfo.Shortest);
			item.SliceLongest:SetText(playerInfo.Longest);
			item.SliceAverage:SetText(playerInfo.Average);
		end
	end
end

---------------------------------------------------------------------
function UpdateLocalSlice()
	local localInfo = Network.GetLocalTurnSliceInfo();
	Controls.LocalSliceShortest:SetText(localInfo.Shortest);
	Controls.LocalSliceLongest:SetText(localInfo.Longest);
	Controls.LocalSliceAverage:SetText(localInfo.Average);
end

----------------------------------------------------------------   
function UpdateNetworkDisplay()
	UpdatePlayerSliceList();
	UpdateLocalSlice();
	Controls.MaxMessages:SetText( Network.GetTurnSliceMaxMessageCount() );
end
ContextPtr:SetUpdate(UpdateNetworkDisplay);

---------------------------------------------------------------------
function VerboseLoggingToggleClicked()
	if (Controls.VerboseLoggingToggle:IsChecked()) then
		Network.SetDebugLogLevel(2);
	else
		Network.SetDebugLogLevel(1);
	end
end

Controls.VerboseLoggingToggle:RegisterCallback( Mouse.eLClick, VerboseLoggingToggleClicked );

---------------------------------------------------------------------
function DecreaseMessageCountClicked()
	Network.SetTurnSliceMaxMessageCount(Network.GetTurnSliceMaxMessageCount() - 1);
	Controls.MaxMessages:SetText( Network.GetTurnSliceMaxMessageCount() );
end

Controls.DecreaseMessageCount:RegisterCallback( Mouse.eLClick, DecreaseMessageCountClicked );

---------------------------------------------------------------------
function IncreaseMessageCountClicked()
	Network.SetTurnSliceMaxMessageCount(Network.GetTurnSliceMaxMessageCount() + 1);
	Controls.MaxMessages:SetText( Network.GetTurnSliceMaxMessageCount() );
end

Controls.IncreaseMessageCount:RegisterCallback( Mouse.eLClick, IncreaseMessageCountClicked );

---------------------------------------------------------------------
function ExitClicked( void1, Control )
	ContextPtr:SetHide( true );
end

Controls.Exit_Button:RegisterCallback( Mouse.eLClick, ExitClicked );

---------------------------------------------------------------------
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
		BuildPlayerSliceList();
		UpdateLocalSlice();
		
		Controls.VerboseLoggingToggle:SetCheck(Network.GetDebugLogLevel() == 2);		
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
