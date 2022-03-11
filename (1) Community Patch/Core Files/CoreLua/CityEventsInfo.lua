-------------------------------------------------
-- Notification Log Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

local g_ItemManagers = {
	InstanceManager:new( "ItemInstance", "Button", Controls.CityEventsStack ),
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
	local activeCityRecentEventChoices = pPlayer:GetActiveCityEventChoices();
	for i,v in ipairs(activeCityRecentEventChoices) do

		print("found an event choice")
		local eventChoice = {
			EventChoice = v.EventChoice,
			Duration = v.Duration,
			ParentEvent = v.ParentEvent,
			CityX = v.CityX,
			CityY = v.CityY,
		};

		local pTargetCity = nil;
		local pLoopPlot = map.GetPlot(eventChoice.CityX, eventChoice.CityY);
		if(pLoopPlot ~= nil) then
			pTargetCity = pLoopPlot:GetPlotCity();
		end

		eventChoice.CityName = pTargetCity:GetName();

		local pEventChoiceInfo = GameInfo.CityEventChoices[eventChoice.EventChoice];
		local pParentEventInfo = GameInfo.CityEvents[eventChoice.ParentEvent];

		local szParentDescString = Locale.Lookup(pParentEventInfo.Description);
		local szChoiceDescString;
		local szChoiceHelpString;

		szChoiceDescString = Locale.Lookup(pEventChoiceInfo.Description);
		szChoiceHelpString = Locale.ConvertTextKey(pTargetCity:GetScaledEventChoiceValue(pEventChoiceInfo.ID));

		eventChoice.EventParentName = szParentDescString;
		eventChoice.EventChoiceName = szChoiceDescString;
		eventChoice.EventChoiceDescription = szChoiceHelpString;

		iTotal = iTotal + 1;
		table.insert(g_Model, eventChoice);
	end
	if(iTotal > 0) then
		Controls.NoCityEvents:SetHide(true);
	else
		Controls.NoCityEvents:SetHide(false);
	end
end

function SortByEventTitle(a, b)
	return Locale.Compare(a.EventParentName, b.EventParentName) == -1;
end

function SortByCityName(a, b)
	return Locale.Compare(a.CityName, b.CityName) == -1;
end

g_SortOptions = {
	{"TXT_KEY_EVENT_CHOICE_SORT_NAME", SortByEventTitle},
	{"TXT_KEY_EVENT_CHOICE_SORT_CITY_NAME", SortByCityName},
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

		itemInstance.CityEventChoiceLocation:SetText("[COLOR_MAGENTA]" .. eventChoice.CityName .. "[ENDCOLOR]");

		itemInstance.CityParentEventTitle:SetText("[COLOR_CYAN]" .. eventChoice.EventParentName .. "[ENDCOLOR]");
		itemInstance.CityEventChoiceTitle:SetText(Locale.ConvertTextKey("TXT_KEY_EVENT_CHOICE_UI") .. " " .. eventChoice.EventChoiceName);
		itemInstance.CityEventChoiceHelpText:SetText(Locale.ConvertTextKey("TXT_KEY_EVENT_CHOICE_RESULT_UI") .. " " .. eventChoice.EventChoiceDescription);
		if(eventChoice.Duration > 0) then
			itemInstance.CityEventChoiceDuration:SetText(Locale.ConvertTextKey("TXT_KEY_TP_TURNS_REMAINING", eventChoice.Duration));
		else
			itemInstance.CityEventChoiceDuration:SetText(Locale.ConvertTextKey("TXT_KEY_TP_PERMANENT"));
		end

		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.CityEventInfoStack:GetSizeVal();

		local newHeight = math.max(100, descHeight + 0);

		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth, newHeight + 5);

		itemInstance.GoToCity:RegisterCallback(Mouse.eLClick, function()
			local plot = Map.GetPlot(eventChoice.CityX, eventChoice.CityY);
			UI.LookAt(plot, 0);
		end);
	end

	Controls.CityEventsStack:CalculateSize();
    Controls.CityEventsStack:ReprocessAnchoring();
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
