-------------------------------------------------
-- Notification Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

local g_ItemManagers = {
	InstanceManager:new( "ItemInstance", "Button", Controls.RecentEventsStack ),
}

-------------------------------------------------
-- On Popup
-------------------------------------------------
function RefreshData()

	local iActivePlayer = Game.GetActivePlayer();
	local pPlayer = Players[iActivePlayer];

	g_Model = {};

	local map = Map;

	local iTotal = 0;
	local activeRecentEventChoices = pPlayer:GetRecentPlayerEventChoices();
	for i,v in ipairs(activeRecentEventChoices) do

		print("found an event choice")
		local eventChoice = {
			EventChoice = v.EventChoice,
			Duration = v.Duration,
			ParentEvent = v.ParentEvent,
		};

		local pEventChoiceInfo = GameInfo.EventChoices[eventChoice.EventChoice];

		local szParentDescString = "";
		local szChoiceDescString;
		local szChoiceHelpString;

		szChoiceDescString = Locale.Lookup(pEventChoiceInfo.Description);
		szChoiceHelpString = Locale.ConvertTextKey(pPlayer:GetScaledEventChoiceValue(pEventChoiceInfo.ID));

		if(eventChoice.ParentEvent ~= -1) then
			local pParentEventInfo = GameInfo.Events[eventChoice.ParentEvent];
			szParentDescString = Locale.Lookup(pParentEventInfo.Description);
			eventChoice.EventParentName = szParentDescString;
			eventChoice.EventChoiceName = szChoiceDescString;
		else
			eventChoice.EventParentName = szChoiceDescString;
			eventChoice.EventChoiceName = "";
		end

		eventChoice.EventChoiceDescription = szChoiceHelpString;

		iTotal = iTotal + 1;
		table.insert(g_Model, eventChoice);
	end
	if(iTotal > 0) then
		Controls.NoRecentEvents:SetHide(true);
	else
		Controls.NoRecentEvents:SetHide(false);
	end
end

function SortByEventTitle(a, b)
	return Locale.Compare(a.EventParentName, b.EventParentName) == -1;
end

function SortByEventDuration(a, b)
	return a.Duration < b.Duration;
end

g_SortOptions = {
	{"TXT_KEY_EVENT_CHOICE_SORT_NAME", SortByEventTitle},
	{"TXT_KEY_EVENT_CHOICE_SORT_DURATION", SortByEventDuration},
}
g_CurrentSortOption = 2;

function SortData()
	table.sort(g_Model, g_SortOptions[g_CurrentSortOption][2]);
end

function Initialize()
	local sortByPulldown = Controls.SortByPullDown;
	sortByPulldown:ClearEntries();
	for i, v in ipairs(g_SortOptions) do
		local controlTable = {};
		sortByPulldown:BuildEntry( "InstanceOne", controlTable );
		controlTable.Button:LocalizeAndSetText(v[1]);

		controlTable.Button:RegisterCallback(Mouse.eLClick, function()
			sortByPulldown:GetButton():LocalizeAndSetText(v[1]);
			g_CurrentSortOption = i;

			SortData();
			DisplayData();
		end);
	end
	sortByPulldown:CalculateInternals();
	sortByPulldown:GetButton():LocalizeAndSetText(g_SortOptions[g_CurrentSortOption][1]);
end

function DisplayData()

	for _, itemManager in ipairs(g_ItemManagers) do
		itemManager:ResetInstances();
	end

	for _, eventChoice in ipairs(g_Model) do

		local itemInstance = g_ItemManagers[1]:GetInstance();

		if(eventChoice.EventParentName ~= "") then
			itemInstance.RecentParentEventTitle:SetText("[COLOR_CYAN]" .. eventChoice.EventParentName .. "[ENDCOLOR]");
			itemInstance.RecentEventChoiceTitle:SetText(eventChoice.EventChoiceName);
		else
			itemInstance.RecentParentEventTitle:SetText("[COLOR_CYAN]" .. eventChoice.EventChoiceName .. "[ENDCOLOR]");
		end
		itemInstance.RecentEventChoiceHelpText:SetText(Locale.ConvertTextKey("TXT_KEY_EVENT_CHOICE_RESULT_UI") .. " " .. eventChoice.EventChoiceDescription);
		if(eventChoice.Duration > 0) then
			itemInstance.RecentEventChoiceDuration:SetText(Locale.ConvertTextKey("TXT_KEY_TP_TURNS_REMAINING", eventChoice.Duration));
		else
			itemInstance.RecentEventChoiceDuration:SetText(Locale.ConvertTextKey("TXT_KEY_TP_BONUS"));
		end

		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.RecentEventInfoStack:GetSizeVal();

		local newHeight = math.max(100, descHeight + 0);

		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth, newHeight + 5);
	end

	Controls.RecentEventsStack:CalculateSize();
    Controls.RecentEventsStack:ReprocessAnchoring();
	Controls.ItemStack:CalculateSize();
    Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();

end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )
    if ( not bInitState and not bIsHide ) then
      	Initialize();
      	RefreshData();
      	SortData();
      	DisplayData();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
