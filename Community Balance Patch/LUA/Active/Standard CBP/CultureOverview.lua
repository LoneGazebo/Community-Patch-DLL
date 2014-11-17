local isUsingFlagsPlus = false

local FlagsPlusModId = "e1ccf71a-f248-498c-8f30-5ca6d851079d"
for _, mod in pairs(Modding.GetActivatedMods()) do
  if (mod.ID == FlagsPlusModId) then
    isUsingFlagsPlus = true
	break
  end
end

function dprint(...)
	--print(...);
end

-------------------------------------------------
-- Culture Overview Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local g_PopupInfo = nil;
local g_IconSize = 45;

--If Culture Overview UI is disabled, just exit!
if(Game.IsOption("GAMEOPTION_NO_CULTURE_OVERVIEW_UI")) then
	return;
end
-------------------------------------------------
-- Global Constants
-------------------------------------------------
g_Tabs = {
	["YourCulture"] = {
		Panel = Controls.YourCulturePanel,
		SelectHighlight = Controls.YourCultureSelectHighlight,
	},
	
	["SwapGreatWorks"] = {
		Panel = Controls.SwapGreatWorksPanel,
		SelectHighlight = Controls.SwapGreatWorksSelectHighlight,
	},
	
	["CultureVictory"] = {
		Panel = Controls.CultureVictoryPanel,
		SelectHighlight = Controls.CultureVictorySelectHighlight,
	},
	
	["PlayerInfluence"] = {
		Panel = Controls.PlayerInfluencePanel,
		SelectHighlight = Controls.PlayerInfluenceSelectHighlight,
	},
}

-------------------------------------------------
-- Global Variables
-------------------------------------------------
g_CurrentTab = nil;		-- The currently selected Tab.
g_iSelectedPlayerID = Game.GetActivePlayer();


g_YourCulture = nil;			-- The data model for the "Your Culture" tab.
g_CultureVictory = nil;			-- The data model for the "Culture Victory" tab.
g_InfluenceByPlayer = nil;		-- The data model for the "Influence by Player" tab.

g_YourCultureSelectionMode = "ClickToMove";	-- Determines whether to use "click to move" or "click to view".

g_CurrentGreatWorkSlot = nil;	-- The selected great work slot.
g_GreatWorkSlots	= {};		-- A list of all great work slots.


g_YourCultureSortOptions = {
	{
		Button = Controls.CityNameHeader,
		Column = "CityName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.CultureHeader,
		Column = "Culture",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.TourismHeader,
		Column = "Tourism",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.GreatWorksHeader,
		Column = "GreatWorks",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	}
};

-- Kyte
g_MaxColumns = 13;
g_ColumnTypes = { "Single", "Double", "Triple" };
for i = 1, g_MaxColumns do
	for _, Prefix in ipairs(g_ColumnTypes) do
		local ColumnName = Prefix .. "Column" .. i;
		local sortOption = {
			Button = Controls[ColumnName .. "Button"],
			Column = ColumnName .. "Sort",
			DefaultDirection = "desc",
			CurrentDirection = nil,
			SortType = "numeric",
		};
		table.insert(g_YourCultureSortOptions, sortOption);
	end
end
-- Kyte end	

g_CultureVictorySortOptions = {
	{
		Button = Controls.VictoryPlayerNameHeader,
		Column = "strPlayerName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.NumInfluentialHeader,
		Column = "InfluencePct",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric"
	},
	
	{
		Button = Controls.VictoryTourismHeader,
		Column = "Tourism",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.IdeologyHeader,
		Column = "strIdeology",
		DefaultDirection = "asc",
		CurrentDirection = nil,
	},
	{
		Button = Controls.PublicOpinionHeader,
		Column = "iPublicOpinion",
		DefaultDirection = "asc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.PublicOpinionUnhappinessHeader,
		Column = "iUnhappiness",
		DefaultDirection = "asc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.ExcessHappinessHeader,
		Column = "iExcessHappiness",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},

};

g_InfluenceByPlayerSortOptions = {
	{
		Button = Controls.PlayerNameHeader,
		Column = "PlayerName",
		DefaultDirection = "asc",
		CurrentDirection = "asc",
	},
	{
		Button = Controls.InfluenceLevelHeader,
		Column = "Level",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.InfluencePercentHeader,
		Column = "InfluencePercent",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.InfluenceModifiersHeader,
		Column = "Modifier",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.InfluenceTourismHeader,
		Column = "TourismPerTurn",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
	{
		Button = Controls.InfluenceTrendHeader,
		Column = "TrendRate",
		DefaultDirection = "desc",
		CurrentDirection = nil,
		SortType = "numeric",
	},
};

g_YourCultureSortFunction = nil;
g_CultureVictorySortFunction = nil;
g_InfluenceByPlayerSortFunction = nil;


-------------------------------------------------------------------------------
-- Sorting Support
-------------------------------------------------------------------------------
function AlphabeticalSortFunction(field, direction, secondarySort)
	if(direction == "asc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or "";
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or "";
			
			if(secondarySort ~= nil and va == vb) then
				return secondarySort(a,b);
			else
				return Locale.Compare(va, vb) == -1;
			end
		end
	elseif(direction == "desc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or "";
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or "";
			
			if(secondarySort ~= nil and va == vb) then
				return secondarySort(a,b);
			else
				return Locale.Compare(va, vb) == 1;
			end
		end
	end
end

function NumericSortFunction(field, direction, secondarySort)
	if(direction == "asc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;
			
			if(secondarySort ~= nil and tonumber(va) == tonumber(vb)) then
				return secondarySort(a,b);
			else
				return tonumber(va) < tonumber(vb);
			end
		end
	elseif(direction == "desc") then
		return function(a,b)
			local va = (a ~= nil and a[field] ~= nil) and a[field] or -1;
			local vb = (b ~= nil and b[field] ~= nil) and b[field] or -1;

			if(secondarySort ~= nil and tonumber(va) == tonumber(vb)) then
				return secondarySort(a,b);
			else
				return tonumber(vb) < tonumber(va);
			end
		end
	end
end

-- Registers the sort option controls click events
function RegisterSortOptions()
	
	for i,v in ipairs(g_YourCultureSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() YourCultureSortOptionSelected(v); end);
		end
	end
	
	for i,v in ipairs(g_CultureVictorySortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() CultureVictorySortOptionSelected(v); end);
		end
	end
	
	for i,v in ipairs(g_InfluenceByPlayerSortOptions) do
		if(v.Button ~= nil) then
			v.Button:RegisterCallback(Mouse.eLClick, function() InfluenceByPlayerSortOptionSelected(v); end);
		end
	end
	
	g_YourCultureSortFunction = GetSortFunction(g_YourCultureSortOptions);
	g_CultureVictorySortFunction = GetSortFunction(g_CultureVictorySortOptions);
	g_InfluenceByPlayerSortFunction = GetSortFunction(g_InfluenceByPlayerSortOptions);
end

function GetSortFunction(sortOptions)
	local orderBy = nil;
	for i,v in ipairs(sortOptions) do
		if(v.CurrentDirection ~= nil) then
			local secondarySort = nil;
			if(v.SecondaryColumn ~= nil) then
				if(v.SecondarySortType == "numeric") then
					secondarySort = NumericSortFunction(v.SecondaryColumn, v.SecondaryDirection)
				else
					secondarySort = AlphabeticalSortFunction(v.SecondaryColumn, v.SecondaryDirection);
				end
			end
		
			if(v.SortType == "numeric") then
				return NumericSortFunction(v.Column, v.CurrentDirection, secondarySort);
			else
				return AlphabeticalSortFunction(v.Column, v.CurrentDirection, secondarySort);
			end
		end
	end
	
	return nil;
end

-- Updates the sort option structure
function UpdateSortOptionState(sortOptions, selectedOption)
	-- Current behavior is to only have 1 sort option enabled at a time 
	-- though the rest of the structure is built to support multiple in the future.
	-- If a sort option was selected that wasn't already selected, use the default 
	-- direction.  Otherwise, toggle to the other direction.
	for i,v in ipairs(sortOptions) do
		if(v == selectedOption) then
			if(v.CurrentDirection == nil) then			
				v.CurrentDirection = v.DefaultDirection;
			else
				if(v.CurrentDirection == "asc") then
					v.CurrentDirection = "desc";
				else
					v.CurrentDirection = "asc";
				end
			end
		else
			v.CurrentDirection = nil;
		end
	end
end

-- Callback for when sort options are selected.
function YourCultureSortOptionSelected(option)
	local sortOptions = g_YourCultureSortOptions;
	UpdateSortOptionState(sortOptions, option);
	g_YourCultureSortFunction = GetSortFunction(sortOptions);
	
	SortAndDisplayYourCulture();
end

-- Callback for when sort options are selected.
function CultureVictorySortOptionSelected(option)
	local sortOptions = g_CultureVictorySortOptions;
	UpdateSortOptionState(sortOptions, option);
	g_CultureVictorySortFunction = GetSortFunction(sortOptions);
	
	SortAndDisplayCultureVictory();
end

-- Callback for when sort options are selected.
function InfluenceByPlayerSortOptionSelected(option)
	local sortOptions = g_InfluenceByPlayerSortOptions;
	UpdateSortOptionState(sortOptions, option);
	g_InfluenceByPlayerSortFunction = GetSortFunction(sortOptions);
	
	SortAndDisplayPlayerInfluence();
end


-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	
	local popupType = popupInfo.Type;
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CULTURE_OVERVIEW then
		return;
	end
	
	g_PopupInfo = popupInfo;
	
	-- Data 2 parameter holds desired tab to open on
	if (g_PopupInfo.Data2 == 2) then
		TabSelect("SwapGreatWorks");
	elseif (g_PopupInfo.Data2 == 3) then
		TabSelect("CultureVictory");
	elseif (g_PopupInfo.Data2 == 4) then
		TabSelect("PlayerInfluence");
	else
		TabSelect("YourCulture");
	end
	
	if( g_PopupInfo.Data1 == 1 ) then
    	if( ContextPtr:IsHidden() == false ) then
    		OnClose();
		else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
    	end
	else
		UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
	end   	
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if(uiMsg == KeyEvents.KeyDown) then
        if (wParam == Keys.VK_ESCAPE) then
			OnClose();
			return true;
        end
        
        -- Do Nothing.
        if(wParam == Keys.VK_RETURN) then
			return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
	if(g_PopupInfo ~= nil) then
		Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
    end
        
	UIManager:DequeuePopup(ContextPtr);
end
Controls.CloseButton:RegisterCallback(Mouse.eLClick, OnClose);
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function TabSelect(tab)

	for i,v in pairs(g_Tabs) do
		local bHide = i ~= tab;
		v.Panel:SetHide(bHide);
		v.SelectHighlight:SetHide(bHide);
	end
	g_CurrentTab = tab;
	g_Tabs[tab].RefreshContent();	
end
Controls.TabButtonYourCulture:RegisterCallback( Mouse.eLClick, function() TabSelect("YourCulture"); end);
Controls.TabButtonSwapGreatWorks:RegisterCallback( Mouse.eLClick, function() TabSelect("SwapGreatWorks"); end);
Controls.TabButtonCultureVictory:RegisterCallback( Mouse.eLClick, function() TabSelect("CultureVictory"); end );
Controls.TabButtonPlayerInfluence:RegisterCallback( Mouse.eLClick, function() TabSelect("PlayerInfluence"); end );

--Kyte
function getBuildings()
	local gwBuildings = {};
	
	-- Get all non-override, non-Wonder buildings with at least 1 Great Work slot.

	for row in DB.Query([[
							SELECT b.Id AS Id, bc.Type AS BuildingClass, b.Type AS BuildingType, GreatWorkCount, 
							CivilizationType, t.GridX, t.GridY,
							EXISTS (
								SELECT 1
								FROM Building_ThemingBonuses
								WHERE BuildingType = b.Type
								LIMIT 1 
							) AS HasThemeBonusInt
							FROM BuildingClasses AS bc
							JOIN Buildings AS b ON bc.Type = b.BuildingClass
							LEFT JOIN Civilization_BuildingClassOverrides AS o ON o.BuildingType = b.Type
							LEFT JOIN Technologies AS t ON b.PrereqTech = t.Type
							WHERE	MaxPlayerInstances = -1 AND
									MaxTeamInstances = -1 AND
									MaxGlobalInstances = -1 AND
									GreatWorkCount > 0 AND
									CivilizationType IS NULL
						]]) do
		gwBuildings[row.BuildingClass] = row;
		dprint(row.BuildingType);
	end
	
	-- If you get overrides after normal buildings it'll effectively replace the previous buildings so it all works out pretty nicely.
	for row in DB.Query([[
							SELECT b.Id AS Id, bc.Type AS BuildingClass, b.Type AS BuildingType, GreatWorkCount,
							CivilizationType, t.GridX, t.GridY, 
							EXISTS (
								SELECT 1
								FROM Building_ThemingBonuses
								WHERE BuildingType = b.Type
								LIMIT 1 
							) AS HasThemeBonusInt
							FROM BuildingClasses AS bc
							JOIN Buildings AS b ON bc.Type = b.BuildingClass
							LEFT JOIN Civilization_BuildingClassOverrides AS o ON o.BuildingType = b.Type
							LEFT JOIN Technologies AS t ON b.PrereqTech = t.Type
							WHERE	MaxPlayerInstances = -1 AND
									MaxTeamInstances = -1 AND
									MaxGlobalInstances = -1 AND
									GreatWorkCount > 0 AND
									CivilizationType = ?
						]], GameInfo.Civilizations[Game.GetActiveCivilizationType()].Type) do
		gwBuildings[row.BuildingClass] = row;
		dprint(row.BuildingType);
	end

	function buildingSort(a, b)
		local ax = a.GridX ~= nil and a.GridX or 99;
		local ay = a.GridX ~= nil and a.GridX or 99;
		local bx = b.GridX ~= nil and b.GridX or 99;
		local by = b.GridX ~= nil and b.GridX or 99;

		if (ax == bx) then
			if (ay == by) then
				return a.Id < b.Id;
			else
				return ay < by;
			end
		else
			return ax < bx;
		end
	end

	local sorted = {};
	for _, building in pairs(gwBuildings) do
		table.insert(sorted, building);
	end
	
	table.sort(sorted, buildingSort);

	return sorted;
end

-- Cache these to avoid constant DB queries (also sorting).
g_GreatWorkBuildings = getBuildings();

print("Non-Wonder Great Work buildings");
for _, v in pairs(g_GreatWorkBuildings) do
	print(
		v.Id .. ":" .. v.BuildingClass .. "/" .. v.BuildingType .. ": " ..
		v.GreatWorkCount .. "x GW" .. (v.HasThemeBonusInt == 1 and "+T" or "") .. " / " .. 
        (v.CivilizationType ~= nil and v.CivilizationType or "Generic") .. 
		(v.GridX ~= nil and " (" .. v.GridX .. "," .. v.GridY .. ")" or " (no tech)")
	);
end

g_ColumnSizes = { 
	[1] = Controls.SingleColumn1Background:GetSizeX(), 
	[2] = Controls.DoubleColumn1Background:GetSizeX(), 
	[3] = Controls.TripleColumn1Background:GetSizeX()
};
-- Kyte End

function RefreshYourCulture()
	g_YourCulture = nil;
	g_CurrentGreatWorkSlot = nil;
	
	local pPlayer = Players[Game.GetActivePlayer()];
	
	if (Game.GetNumArchaeologySites() == -1) then
		Controls.AntiquitySites:SetHide(true);
		Controls.HiddenSites:SetHide(true);
	else
		Controls.AntiquitySites:SetText(Locale.ConvertTextKey("TXT_KEY_ANTIQUITY_SITES_TO_EXCAVATE", Game.GetNumArchaeologySites()));
		Controls.HiddenSites:SetText(Locale.ConvertTextKey("TXT_KEY_HIDDEN_SITES_TO_EXCAVATE", Game.GetNumHiddenArchaeologySites()));
		Controls.AntiquitySites:SetHide(false);
		Controls.HiddenSites:SetHide(false);
	end

    -- Precache Slot Types
    local greatWorkSlotIcons = {};
    local selectedIcons = {
		GREAT_WORK_SLOT_ART_ARTIFACT = "GreatWorks_Art_HL_34.dds",
		GREAT_WORK_SLOT_LITERATURE = "GreatWorks_Book_HL_34.dds",
		GREAT_WORK_SLOT_MUSIC = "GreatWorks_Music_HL_34.dds",
    };
	for row in GameInfo.GreatWorkSlots() do
		greatWorkSlotIcons[row.Type] = {
			FilledIcon = row.FilledIcon,
			EmptyIcon = row.EmptyIcon,
			SelectedIcon = selectedIcons[row.Type],
		};
    end   
       
    local playerCivilization = GameInfo.Civilizations[pPlayer:GetCivilizationType()];
    local playerCivilizationType = playerCivilization.Type;
    function BuildingEntry(data)
    
		local building;
		local buildingClass;
		
		if(data.BuildingClass ~= nil) then
			buildingClass = GameInfo.BuildingClasses[data.BuildingClass];
				
			local override = GameInfo.Civilization_BuildingClassOverrides{BuildingClassType = data.BuildingClass, CivilizationType = playerCivilizationType}();
			if(override ~= nil) then
				
				building = GameInfo.Buildings[override.BuildingType];
			else
				building = GameInfo.Buildings[buildingClass.DefaultBuilding];
			end
		else
			building = GameInfo.Buildings[data.BuildingType];
			buildingClass = GameInfo.BuildingClasses[building.BuildingClass];
		end
		
		
		local entry = {
			BuildingID = building.ID,
			BuildingClassID = buildingClass.ID,
			BuildingClassType = buildingClass.Type, -- Kyte
			GreatWorkSlotType = building.GreatWorkSlotType,
			GreatWorkSlotIcons = greatWorkSlotIcons[building.GreatWorkSlotType],
			GreatWorkSlotCount = building.GreatWorkCount,
			ThemeBonusControlName = data.ThemeBonusControlName,
			GreatWorkIconControlNames = data.IconControlNames,
			GreatWorkHighlightIconControlNames = data.HighlightIconControlNames,
			GreatWorkGhostIconControlNames = data.GhostIconControlNames,
			BuildingSortColumn = data.BuildingSortColumn,
			ShowGhost = buildingClass.MaxPlayerInstances == -1,
		}
		
		IconHookup(building.PortraitIndex, g_IconSize, building.IconAtlas, data.ColumnImageControl);
		data.ColumnImageControl:LocalizeAndSetToolTip(building.Description);
  
		return entry;
    end

	-- Kyte
	-- The magic numbers are the widths of MainStack and the columns we aren't touching. See XML.
	-- MainStack CityName Culture Tourism GreatWorks ScrollBar
	local availablePixelSpace = 937 - 221 - 61 - 51 - 41 - 18;
	local columnIndex = 1;
	
	function AssignBuildingToColumn(building, buildings)
		-- The +1 is for the divider.
		local estimatedColumnSize = g_ColumnSizes[building.GreatWorkCount] + 1;
		-- Magic number 23: The width of the theme bonus text (assuming 0-9).
		if (building.GreatWorkCount > 1 and building.HasThemeBonusInt == 0) then estimatedColumnSize = estimatedColumnSize - 23; end

		dprint("Space left: " .. availablePixelSpace);
		dprint("Estimation: " .. estimatedColumnSize);

		if (availablePixelSpace < estimatedColumnSize) then
			if (building.HasThemeBonusInt == 1 and estimatedColumnSize - availablePixelSpace < 6) then
				dprint("Squeezing a last column. (" .. building.GreatWorkCount ..  " slot(s))");
			else
				dprint("No space left. (" .. building.GreatWorkCount ..  " slot(s))");
				return;
			end
		end

		if (columnIndex > g_MaxColumns) then
			dprint("No columns left.");
            return;
        end

		local columnType = g_ColumnTypes[building.GreatWorkCount];
		if (columnType == nil) then
            dprint("No columns of this type. (" .. building.GreatWorkCount ..  " slot(s))");
            return;
        end
		local untypedColumnName = "Column" .. columnIndex;
		local columnName = columnType .. untypedColumnName;
		
		availablePixelSpace = availablePixelSpace - estimatedColumnSize;

		local iconControls = { icon = {}, high = {}, ghost = {} };
		if (building.GreatWorkCount == 1) then
			table.insert(iconControls.icon, columnName .. "GreatWork");
			table.insert(iconControls.high, columnName .. "GreatWorkHL");
			table.insert(iconControls.ghost, columnName .. "GreatWorkGhost");
		else
			for i = 1, building.GreatWorkCount do
				table.insert(iconControls.icon, columnName .. "GreatWork" .. i);
				table.insert(iconControls.high, columnName .. "GreatWork" .. i .. "HL");
				table.insert(iconControls.ghost, columnName .. "GreatWork" .. i .. "Ghost");
			end
		end

		local entry = BuildingEntry{
            BuildingType = building.BuildingType,
            BuildingSortColumn = columnName .. "Sort",
            IconControlNames = iconControls.icon,
            HighlightIconControlNames = iconControls.high,
            GhostIconControlNames = iconControls.ghost,
            ThemeBonusControlName = columnName .. "ThemeBonus";
            ColumnImageControl = Controls[columnName .. "Image"],
        };

		entry.ColumnName = columnName;

        table.insert(buildings, entry);
		columnIndex = columnIndex + 1;
			
		Controls[columnName .. "Button"]:SetHide(false);
		Controls[columnName .. "Background"]:SetHide(false);
		Controls[untypedColumnName .. "Div"]:SetHide(false);
			
		if (columnIndex % 2 == 0) then
			dprint("Got " .. entry.ColumnName .. " with transparent background");
			Controls[entry.ColumnName .. "Background"]:SetColorVal(0,0,0,0);
		else
			dprint("Got " .. entry.ColumnName .. " with colored background");
			Controls[entry.ColumnName .. "Background"]:SetColorVal(1/255,26/255,23/255,1);
		end
	end
       
    local buildings = {};

	for _, building in ipairs(g_GreatWorkBuildings) do
		dprint("About to assign " .. building.BuildingClass .. "/" .. building.BuildingType);
        AssignBuildingToColumn(building, buildings);
    end

	function BuildingClassIsAColumn(item, table)
		for _, v in ipairs(table) do
			if (v.BuildingClassType == item) then 
				return true;
			end;
		end

		return false;
	end
	--Kyte end
	
	local WorldWonders = {};
	for building in GameInfo.Buildings() do
		local buildingClass = GameInfo.BuildingClasses[building.BuildingClass];
		-- Kyte
		if(building.GreatWorkCount > 0 
			and not BuildingClassIsAColumn(buildingClass.Type, buildings)) then
			table.insert(WorldWonders, {
				BuildingID = building.ID,
				BuildingClassID = buildingClass.ID,
				GreatWorkSlotCount = building.GreatWorkCount,
				PortraitIndex = building.PortraitIndex,
				IconAtlas = building.IconAtlas,
				Description = building.Description,
				GreatWorkSlotType = building.GreatWorkSlotType,
				GreatWorkSlotIcons = greatWorkSlotIcons[building.GreatWorkSlotType],
			});
		end
	end
	
    local playerID = pPlayer:GetID();
    
    local model = {};
    
    for city in pPlayer:Cities() do
		local cityData = {};
		
		local cityName = city:GetName()
		
		if(city:IsCapital())then
			cityData.CityDisplayName = "[ICON_CAPITAL] " .. cityName;
		elseif(city:IsPuppet()) then
			cityData.CityDisplayName = "[ICON_PUPPET] " .. cityName;
		elseif(city:IsOccupied() and not city:IsNoOccupiedUnhappiness()) then
			cityData.CityDisplayName = "[ICON_OCCUPIED] " .. cityName;
		else
			cityData.CityDisplayName = cityName;
		end
		cityData.ID = city:GetID();
		cityData.CityName = cityName;
		
		cityData.HealthPercent = 1 - (city:GetDamage() / city:GetMaxHitPoints());
    
		cityData.Culture = city:GetJONSCulturePerTurn();
		cityData.Tourism = city:GetBaseTourism();
		cityData.TourismToolTip = city:GetTourismTooltip();
		cityData.GreatWorks = city:GetNumGreatWorks();
		cityData.GreatWorksToolTip = city:GetTotalSlotsTooltip();
        
        
        cityData.Buildings = {};
        for _,v in ipairs(buildings) do
			
			local buildingData = {};
			
			buildingData.GreatWorkSlotType = v.GreatWorkSlotType;
			buildingData.GreatWorkSlotIcons = v.GreatWorkSlotIcons;
			buildingData.GreatWorkSlotCount = v.GreatWorkSlotCount;
			buildingData.ShowGhost = v.ShowGhost;
			local hasBuilding = city:IsHasBuilding(v.BuildingID);
			
			local greatWorksCount = 0;
			if(hasBuilding) then
			
				local greatWorks = {};
				for greatWorkSlotIndex = 1, v.GreatWorkSlotCount, 1 do				

					local greatWorkIndex = city:GetBuildingGreatWork(v.BuildingClassID, greatWorkSlotIndex - 1);
					local greatWorkToolTip = "";
					if(greatWorkIndex >= 0) then
						greatWorksCount = greatWorksCount + 10;
						greatWorkToolTip = Game.GetGreatWorkTooltip(greatWorkIndex, playerID);
					else
						greatWorksCount = greatWorksCount + 1;
					end
					 
					greatWorks[greatWorkSlotIndex] = {
						Index = greatWorkIndex,
						ToolTip = greatWorkToolTip,
					};

				end
			
				buildingData.ThemeBonus	= "+" .. city:GetThemingBonus(v.BuildingClassID);
				buildingData.ThemeBonusToolTip = city:GetThemingTooltip(v.BuildingClassID);
				buildingData.GreatWorks = greatWorks;
			end
			
			buildingData.HideThemeBonus = not city:IsThemingBonusPossible(v.BuildingClassID);
			cityData[v.BuildingSortColumn] = greatWorksCount;
			
			buildingData.CityID = city:GetID();
			buildingData.BuildingClassID = v.BuildingClassID;
			buildingData.HasBuilding = hasBuilding;
			
			buildingData.ThemeBonusControlName = v.ThemeBonusControlName;
			buildingData.GreatWorkIconControlNames = v.GreatWorkIconControlNames;
			buildingData.GreatWorkHighlightIconControlNames = v.GreatWorkHighlightIconControlNames;
			buildingData.GreatWorkGhostIconControlNames = v.GreatWorkGhostIconControlNames;
			buildingData.ColumnName = v.ColumnName; --Kyte
			
			table.insert(cityData.Buildings, buildingData);
        end
        
        local cityWonders = {};
        
        for _, wonder in ipairs(WorldWonders) do
			if (city:IsHasBuilding(wonder.BuildingID)) then
			   	
			   	local buildingClassID = wonder.BuildingClassID;
			   	
				local wonderData = {
					PortraitIndex = wonder.PortraitIndex,
					IconAtlas = wonder.IconAtlas,
					Description = wonder.Description,
					GreatWorkSlotType = wonder.GreatWorkSlotType,
					GreatWorkSlotIcons = wonder.GreatWorkSlotIcons,
					CityID = city:GetID(),
					BuildingClassID = buildingClassID,		
				}				
				
				local greatWorks = {};
				for greatWorkSlotIndex = 1, wonder.GreatWorkSlotCount, 1 do				

					local greatWorkIndex = city:GetBuildingGreatWork(buildingClassID, greatWorkSlotIndex - 1);
					local greatWorkToolTip = "";
					local isArtifact = false;
					if(greatWorkIndex >= 0) then
						greatWorkToolTip = Game.GetGreatWorkTooltip(greatWorkIndex, playerID);
						local greatWorkType = Game.GetGreatWorkType(greatWorkIndex);
						local greatWork = GameInfo.GreatWorks[greatWorkType];
						isArtifact = greatWork.GreatWorkClassType == "GREAT_WORK_ARTIFACT";
					end
										
					greatWorks[greatWorkSlotIndex] = {
						Index = greatWorkIndex,
						ToolTip = greatWorkToolTip,
						IsArtifact = isArtifact,
					};
				end
				
				wonderData.HideThemeBonus = not city:IsThemingBonusPossible(wonder.BuildingClassID);
				wonderData.ThemeBonus = "+" .. tostring(city:GetThemingBonus(wonder.BuildingClassID));
				wonderData.ThemeBonusToolTip = city:GetThemingTooltip(wonder.BuildingClassID)
				
				wonderData.GreatWorks = greatWorks;
					
				table.insert(cityWonders, wonderData);
			end
			
			cityData.Wonders = cityWonders;
        end
        
        table.insert(model, cityData);
    end
    
    g_YourCulture = model;
end

function SortAndDisplayYourCulture()
					
	local pPlayer = Players[Game.GetActivePlayer()];

	if(g_YourCulture == nil) then
		RefreshYourCulture();
	end	

	local model = g_YourCulture;
	
	table.sort(model, g_YourCultureSortFunction);

	-- Reset list of selection points.
	local previousGreatWorkSelection = g_CurrentGreatWorkSlot;
    g_CurrentGreatWorkSlot = nil;
    g_GreatWorkSlots = {};  
    
    Controls.MainStack:DestroyAllChildren();
        
    -- Utility function to determine best highlight texture
    function GetHighlightTexture(data)
		local currentGreatWorkSlot = g_CurrentGreatWorkSlot;
    
		if(currentGreatWorkSlot ~= nil and data.GreatWorkSlotType == currentGreatWorkSlot.GreatWorkSlotType ) then
			if(	currentGreatWorkSlot.CityID == data.CityID and 
				currentGreatWorkSlot.BuildingClassID == data.BuildingClassID and
				currentGreatWorkSlot.GreatWorkSlotIndex == data.GreatWorkSlotIndex) then
				
				return data.Icons.SelectedIcon;
			else
				--Design Note: This is where we could alter icon based on potential theme bonus.
				return "GreatWorks_Yellow_HL_34.dds";
			end
		else
			return nil;
		end
	end
    
    for _, cityData in ipairs(g_YourCulture) do
    
		local instance = {};
		ContextPtr:BuildInstanceForControl( "CityInstance", instance, Controls.MainStack );
        
		instance.CityName:SetText(cityData.CityDisplayName);
                       
        local pct = cityData.HealthPercent
    	if( pct ~= 1 ) then
    	
            if pct > 0.66 then
                instance.HealthBar:SetFGColor( { x = 0, y = 1, z = 0, w = 1 } );
            elseif pct > 0.33 then
                instance.HealthBar:SetFGColor( { x = 1, y = 1, z = 0, w = 1 } );
            else
                instance.HealthBar:SetFGColor( { x = 1, y = 0, z = 0, w = 1 } );
            end
            
        	instance.HealthBar:SetPercent( pct );
        	instance.HealthBarBox:SetHide( false );
    	else
        	instance.HealthBarBox:SetHide( true );
    	end
    	
    	instance.Culture:SetText(cityData.Culture);
        instance.Tourism:SetText(cityData.Tourism);
        instance.Tourism:SetToolTipString(cityData.TourismToolTip);
        instance.GreatWorks:SetText(cityData.GreatWorks);
        instance.GreatWorks:SetToolTipString(cityData.GreatWorksToolTip);
       
              
        for buildingIndex, buildingData in ipairs(cityData.Buildings) do
			-- Kyte
			if (buildingData.ColumnName ~= nil) then
				instance[buildingData.ColumnName .. "Container"]:SetHide(false);
			end
			-- Kyte end
                 
            local themeBonusControl = instance[buildingData.ThemeBonusControlName];
			local greatWorkIconControls = {};
			local greatWorkHighlightIconControls = {};
			local greatWorkGhostIconControls = {};
		
			for greatWorkIconControlIndex, greatWorkIconControlName in ipairs(buildingData.GreatWorkIconControlNames) do
				local control = instance[greatWorkIconControlName];
				greatWorkIconControls[greatWorkIconControlIndex] = control;
				if(control ~= nil) then
					control:ClearCallback(Mouse.eLClick);
				end
			end 
			
			for greatWorkHighlightIconControlIndex, greatWorkHighlightIconControlName in ipairs(buildingData.GreatWorkHighlightIconControlNames) do
				greatWorkHighlightIconControls[greatWorkHighlightIconControlIndex] = instance[greatWorkHighlightIconControlName];
			end
			
			for greatWorkGhostIconControlIndex, greatWorkGhostIconControlName in ipairs(buildingData.GreatWorkGhostIconControlNames) do
				greatWorkGhostIconControls[greatWorkGhostIconControlIndex] = instance[greatWorkGhostIconControlName];
			end                        
            
            local icons = buildingData.GreatWorkSlotIcons;
            
            if (buildingData.HasBuilding) then
			
				for greatWorkSlotIndex = 1, math.min(buildingData.GreatWorkSlotCount, #greatWorkIconControls), 1 do
				
					local icon = greatWorkIconControls[greatWorkSlotIndex];
					local highlightIcon = greatWorkHighlightIconControls[greatWorkSlotIndex];
					local ghosticon = greatWorkGhostIconControls[greatWorkSlotIndex];
					
					local greatWork = buildingData.GreatWorks[greatWorkSlotIndex];
					if(greatWork and greatWork.Index >= 0) then
						icon:SetTexture(icons.FilledIcon);
						icon:SetToolTipString(greatWork.ToolTip);
					else
						icon:SetTexture(icons.EmptyIcon);
					end
					
					local selectionPoint = {
						Control = icon,
						HighlightControl = highlightIcon,
						CultureControl = instance.Culture,
						TourismControl = instance.Tourism,
						GreatWorksControl = instance.GreatWorks,
						GreatWorkIndex = greatWork and greatWork.Index or -1,
						GreatWorkSlotIndex = greatWorkSlotIndex - 1,
						GreatWorkSlotType = buildingData.GreatWorkSlotType,
						GreatWorkIsArtifact = greatWork and greatWork.IsArtifact or false,
						Icons = icons,
						CityID = buildingData.CityID,
						BuildingClassID = buildingData.BuildingClassID,
						ThemeBonusControl = themeBonusControl,
					};
					
					if(	previousGreatWorkSelection ~= nil and
						previousGreatWorkSelection.CityID == selectionPoint.CityID and
						previousGreatWorkSelection.BuildingClassID == selectionPoint.BuildingClassID and
						previousGreatWorkSelection.GreatWorkSlotIndex == selectionPoint.GreatWorkSlotIndex) then
						g_CurrentGreatWorkSlot = selectionPoint;	
					end
					
					icon:SetHide(false);
					ghosticon:SetHide(true);
										
					table.insert(g_GreatWorkSlots, selectionPoint);					
				end
				
				if(themeBonusControl ~= nil) then
					themeBonusControl:SetHide(buildingData.HideThemeBonus);
					-- Kyte
					if (buildingData.HideThemeBonus and buildingData.ColumnName ~= nil) then
						-- Magic number 23, the size of the theme bonus text.
						local newSize = g_ColumnSizes[buildingData.GreatWorkSlotCount] - 23;

						instance[buildingData.ColumnName .. "Container"]:SetSizeX(newSize);
						Controls[buildingData.ColumnName .. "Background"]:SetSizeX(newSize);
						Controls[buildingData.ColumnName .. "Button"]:SetSizeX(newSize);
					end
					-- Kyte end
					themeBonusControl:SetText(buildingData.ThemeBonus);
					themeBonusControl:SetToolTipString(buildingData.ThemeBonusToolTip);
				end				
			else
				for greatWorkSlotIndex = 1, math.min(buildingData.GreatWorkSlotCount, #greatWorkIconControls), 1 do
					local icon = greatWorkIconControls[greatWorkSlotIndex];
					local highlightIcon = greatWorkHighlightIconControls[greatWorkSlotIndex];
					local ghosticon = greatWorkGhostIconControls[greatWorkSlotIndex];

					icon:SetHide(true);
					highlightIcon:SetHide(true);
					if(buildingData.ShowGhost == true) then
						ghosticon:SetHide(false);
					else
						ghosticon:SetHide(true);
					end
				end
				
				if(themeBonusControl ~= nil) then
					themeBonusControl:SetHide(true);

					-- Kyte
					if (buildingData.HideThemeBonus and buildingData.ColumnName ~= nil) then
						local newSize = g_ColumnSizes[buildingData.GreatWorkSlotCount] - 23;

						instance[buildingData.ColumnName .. "Container"]:SetSizeX(newSize);
						Controls[buildingData.ColumnName .. "Background"]:SetSizeX(newSize);
						Controls[buildingData.ColumnName .. "Button"]:SetSizeX(newSize);
					end
					-- Kyte end
				end

			end
        
        end
        				
		-- Count wonders with Great Work slots in this city		
		local wonderCount = #cityData.Wonders;
		instance.Divider:SetHide(wonderCount > 0);
		
		for wonderIndex, wonder in ipairs(cityData.Wonders) do
			
			local wonderInstance = {};
			ContextPtr:BuildInstanceForControl( "WonderInstance", wonderInstance, Controls.MainStack );
		
			IconHookup(wonder.PortraitIndex, g_IconSize, wonder.IconAtlas, wonderInstance.WonderImage);
			wonderInstance.WonderImage:LocalizeAndSetToolTip(wonder.Description);
					
			local icons = wonder.GreatWorkSlotIcons;
			
			local greatWorkControls = 
			{
				{
					Box = wonderInstance.WonderGreatWork1Box,
					Icon = wonderInstance.WonderGreatWork1,
					Highlight = wonderInstance.WonderGreatWork1HL,
				},
				
				{
					Box = wonderInstance.WonderGreatWork2Box,
					Icon = wonderInstance.WonderGreatWork2,
					Highlight = wonderInstance.WonderGreatWork2HL,
				},
				
				{
					Box = wonderInstance.WonderGreatWork3Box,
					Icon = wonderInstance.WonderGreatWork3,
					Highlight = wonderInstance.WonderGreatWork3HL,
				},
				
				{
					Box = wonderInstance.WonderGreatWork4Box,
					Icon = wonderInstance.WonderGreatWork4,
					Highlight = wonderInstance.WonderGreatWork4HL,
				},
			};		
			
			for _, v in ipairs(greatWorkControls) do
				v.Box:SetHide(true);
			end
			
			local themeBonusControl = wonderInstance.WonderThemeBonus;
			wonderInstance.WonderThemeBox:SetHide(wonder.HideThemeBonus);
			if(not wonderInstance.HideThemeBonus) then
				themeBonusControl:SetText(wonder.ThemeBonus);
				themeBonusControl:SetToolTipString(wonder.ThemeBonusToolTip);
			end
			
			for index, greatWork in ipairs(wonder.GreatWorks) do
				local control = greatWorkControls[index];
				if(control ~= nil) then
				
					local selectionPoint = {
						Control = control.Icon,
						HighlightControl = control.Highlight,
						CultureControl = instance.Culture,
						TourismControl = instance.Tourism,
						GreatWorksControl = instance.GreatWorks,
						GreatWorkIndex = greatWork and greatWork.Index or -1,
						GreatWorkSlotIndex = index - 1,
						GreatWorkSlotType = wonder.GreatWorkSlotType,
						GreatWorkIsArtifact = greatWork and greatWork.IsArtifact or false,
						Icons = icons,
						CityID = wonder.CityID,
						BuildingClassID = wonder.BuildingClassID,
						ThemeBonusControl = themeBonusControl,
					};
					
					table.insert(g_GreatWorkSlots, selectionPoint);
							
					control.Box:SetHide(false);
						
					if(greatWork.Index >= 0) then
						control.Icon:SetTexture(icons.FilledIcon);
						control.Icon:SetToolTipString(greatWork.ToolTip);
					else
						control.Icon:SetTexture(icons.EmptyIcon);
						control.Icon:SetToolTipString(nil);
					end	
				end
			end
				
			wonderInstance.GreatWorkStack:CalculateSize();
			wonderInstance.GreatWorkStack:ReprocessAnchoring();
			
			local x,y = wonderInstance.GreatWorkStack:GetSizeVal();
			
			wonderInstance.WonderBox:SetSizeVal(x + 10, y);
			wonderInstance.WonderBox:SetHide(false);		
			wonderInstance.Divider:SetHide(wonderIndex ~= wonderCount);
		end
    end
    
    -- Sync selection and register click handler
    for _, selectionPoint in ipairs(g_GreatWorkSlots) do
		if(	previousGreatWorkSelection ~= nil and
			previousGreatWorkSelection.CityID == selectionPoint.CityID and
			previousGreatWorkSelection.BuildingClassID == selectionPoint.BuildingClassID and
			previousGreatWorkSelection.GreatWorkSlotIndex == selectionPoint.GreatWorkSlotIndex) then
			g_CurrentGreatWorkSlot = selectionPoint;	
		end
		
		local greatWorksIndex = selectionPoint.GreatWorkIndex;
		local greatWorkIsArtifact = selectionPoint.GreatWorkIsArtifact;
		
		local viewGreatWorks = function()
			print("VIEWING GREAT WORKS!!!!");
			print(greatWorksIndex);
			
			if(greatWorksIndex ~= -1 and not greatWorkIsArtifact) then
				local popupInfo = {
					Type = ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
					Data1 = greatWorksIndex,
					Priority = PopupPriority.Current
				}
				
				Events.SerialEventGameMessagePopup(popupInfo);
			end
		end
		
		selectionPoint.Control:RegisterCallback(Mouse.eRClick, viewGreatWorks);
		
		if(g_YourCultureSelectionMode == "ClickToView") then
			selectionPoint.Control:RegisterCallback(Mouse.eLClick, viewGreatWorks);			
		else
			selectionPoint.Control:RegisterCallback(Mouse.eLClick, function()
				if(g_CurrentGreatWorkSlot and (g_CurrentGreatWorkSlot.GreatWorkSlotType == selectionPoint.GreatWorkSlotType)) then
					
					local currentGreatWorkSlot = g_CurrentGreatWorkSlot;
					if(	currentGreatWorkSlot.CityID ~= selectionPoint.CityID or
						currentGreatWorkSlot.BuildingClassID ~= selectionPoint.BuildingClassID or
						currentGreatWorkSlot.GreatWorkSlotIndex ~= selectionPoint.GreatWorkSlotIndex) then
						
						local activePlayer = Players[ Game.GetActivePlayer() ];
						local activePlayerID = activePlayer:GetID();
			
						Network.SendMoveGreatWorks(	activePlayerID, 
													g_CurrentGreatWorkSlot.CityID, g_CurrentGreatWorkSlot.BuildingClassID, g_CurrentGreatWorkSlot.GreatWorkSlotIndex, 
													selectionPoint.CityID, selectionPoint.BuildingClassID, selectionPoint.GreatWorkSlotIndex);
					end												
					-- Clear Selection
					g_CurrentGreatWorkSlot = nil;	
				else
					g_CurrentGreatWorkSlot = selectionPoint;
				end
				
				--Update Highlights	
				for _,v in ipairs(g_GreatWorkSlots) do
					local highlightControl = v.HighlightControl;
					local texture = GetHighlightTexture(v);
					if(texture == nil) then
						highlightControl:SetHide(true);
					else
						highlightControl:SetTexture(texture);
						highlightControl:SetHide(false);
					end
				end

			end);	
		end	
    end
       
    --Update Highlights	
	for i,v in ipairs(g_GreatWorkSlots) do
		local highlightControl = v.HighlightControl;
		local texture = GetHighlightTexture(v);
		if(texture == nil) then
			highlightControl:SetHide(true);
		else
			highlightControl:SetTexture(texture);
			highlightControl:SetHide(false);
		end
	end
    
    Controls.MainStack:CalculateSize();
    Controls.MainStack:ReprocessAnchoring();
    Controls.MainScroll:CalculateInternalSize();

    -- Kyte
	--[[
	Nested scroll panels look terrible. :(
	It's not impossible per se, but the vertical scrollbar will move with the rest of the panel,
	making navigation awkward.
	
	Window resizing is also a no go, there's waaaaaaaaaay too many controls to resize,
	making the whole thing extremely fiddly. Really not worth it.

	So, Firaxis: Why the hell is your UI so static. There's not even the most basic "fit to parent"
	and "fit to contents" options, which would enable 90% of everything one could ever need or want
	out of your UI. Or just allow scrollpanels work in both directions.
	]]
	-- Kyte end
end

Controls.radClickToMove:RegisterCheckHandler(function(checked)
	if(checked) then
		g_YourCultureSelectionMode = "ClickToMove";
		SortAndDisplayYourCulture();
	end
end);

Controls.radClickToView:RegisterCheckHandler(function(checked)
	if(checked) then
		g_YourCultureSelectionMode = "ClickToView";
		SortAndDisplayYourCulture();
	end
end);


g_Tabs["YourCulture"].RefreshContent = function()
	RefreshYourCulture();
	SortAndDisplayYourCulture();
end



-- Refresh great work info if dirty (SerialEventCityInfoDirty is called when great works are swapped).
function RefreshGreatWorkInfo()
	if (not ContextPtr:IsHidden()) then
		g_Tabs["YourCulture"].RefreshContent();
	end
end
Events.SerialEventCityInfoDirty.Add(RefreshGreatWorkInfo);


-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
---- Begin ---

-- Globals
local g_iTheirItem = -1;
local g_iYourItem = -1;
local g_iTradingPartner = -1;

function CheckAvailableSwap ()
	local activePlayerID = Game.GetActivePlayer();
	local activePlayer = Players[activePlayerID];	

	--print("CheckAvailableSwap");

	-- if their item is selected
	--  if we have selected an item of that type
	--   fill in the slot
	if (g_iTheirItem ~= -1) then
		-- get item type
		local iTheirClass = Game.GetGreatWorkClass(g_iTheirItem);
		local iYourItem = -1;
		if (iTheirClass == 1) then
			-- art(?)
			iYourItem = activePlayer:GetSwappableGreatArt();
		elseif (iTheirClass == 2) then
			-- artifact(?)
			iYourItem = activePlayer:GetSwappableGreatArtifact();
		elseif (iTheirClass == 3) then
			-- writing
			iYourItem = activePlayer:GetSwappableGreatWriting();
		elseif (iTheirClass == 4) then
			-- music
			iYourItem = activePlayer:GetSwappableGreatMusic();
		end
		
		if (g_iYourItem ~= iYourItem) then
			if (iYourItem ~= -1) then
				--print("Selecting your Item");
				SelectYourWork(iYourItem);	
				Controls.SwapButton:SetDisabled(false);
				Controls.SwapButton:SetToolTipString(Locale.Lookup("TXT_KEY_SWAP_BUTTON_TT"));
			else
				--print("Clearing Your Item");
				SelectYourWork(-1);
				Controls.SwapButton:SetDisabled(true);
				Controls.SwapButton:SetToolTipString(Locale.Lookup("TXT_KEY_SWAP_BUTTON_DISABLED_TT"));
			end
		end
	else
	
	end
end

function SelectYourWork (iIndex)
	g_iYourItem = iIndex;
	
	if (iIndex == -1) then
	
		Controls.SwapYourIconStack:SetHide(true);
	
		-- set great works to dotted box
		Controls.SwapOursIcon:SetTexture("GreatWorks_DottedFrame_32.dds");
		-- hide icon
		Controls.SwapOursCivIcon:SetHide(true);
		Controls.SwapOursCivIconBG:SetHide(true);
		Controls.SwapOursCivIconShadow:SetHide(true);		
		Controls.SwapOursCivIconHighlight:SetHide(true);
		-- hide era
		Controls.SwapOursEra:SetHide(true);
		
		Controls.SwapOursIcon:SetToolTipString("");
	else
		local iClass = Game.GetGreatWorkClass(iIndex);
	
		local strTexture = "";
		if (iClass == 1) then
			strTexture = "GreatWorks.dds";
		elseif (iClass == 2) then
			strTexture = "GreatWorks.dds";
		elseif (iClass == 3) then
			strTexture = "GreatWorks_Book.dds";
		elseif (iClass == 4) then
			strTexture = "GreatWorks_Music.dds";
		end

		if (strImg ~= "") then
			Controls.SwapOursIcon:SetTexture(strTexture);
		end

		Controls.SwapOursCivIcon:SetHide(false);
		Controls.SwapOursCivIconBG:SetHide(false);
		Controls.SwapOursCivIconShadow:SetHide(false);		
		Controls.SwapOursCivIconHighlight:SetHide(false);
		CivIconHookup(Game.GetGreatWorkCreator(iIndex), 32, Controls.SwapOursCivIcon, Controls.SwapOursCivIconBG, Controls.SwapOursCivIconShadow, false, true, Controls.SwapOursCivIconHighlight);

		Controls.SwapOursEra:SetHide(false);
		Controls.SwapOursEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iIndex)));

		local activePlayerID = Game.GetActivePlayer();
		local activePlayer = Players[activePlayerID];
		local activeLeader = GameInfo.Leaders[activePlayer:GetLeaderType()];
		
		IconHookup(activeLeader.PortraitIndex, 64, activeLeader.IconAtlas, Controls.SwapYourWorksIcon);
		CivIconHookup(activePlayerID, 32, Controls.SwapYourWorksSubIcon, Controls.SwapYourWorksSubIconBG, Controls.SwapYourWorksSubIconShadow, (isUsingFlagsPlus == false), true, Controls.SwapYourWorksSubIconHighlight);
		Controls.SwapYourOwnerName:SetText(activePlayer:GetCivilizationShortDescription());
		Controls.SwapYourIconStack:SetHide(false);
		
		Controls.SwapYourIconStack:CalculateSize();
		Controls.SwapYourIconStack:ReprocessAnchoring();
	
		-- Tooltips
		local strTT = Game.GetGreatWorkTooltip(iIndex, activePlayerID)
		Controls.SwapOursIcon:SetToolTipString(strTT);
		Controls.SwapOursCivIcon:SetToolTipString(strTT);
		Controls.SwapOursCivIconBG:SetToolTipString(strTT);
		Controls.SwapOursCivIconShadow:SetToolTipString(strTT);
		Controls.SwapOursCivIconHighlight:SetToolTipString(strTT);
		Controls.SwapOursEra:SetToolTipString(strTT);
	
		CheckAvailableSwap();
	end	
end

function SelectOthersWork (iIndex) 
	--print("SelectOthersWork. iIndex: " .. iIndex);
	g_iTheirItem = iIndex;
	g_iTradingPartner = -1;
	
	if (iIndex == -1) then
	
		Controls.SwapTheirIconStack:SetHide(true);
	
		-- set great works to dotted box
		Controls.SwapTheirsIcon:SetTexture("GreatWorks_DottedFrame_32.dds");
		-- hide name		
		Controls.SwapTheirsOwnerName:SetHide(true);
		
		-- hide icon
		Controls.SwapTheirsCivIcon:SetHide(true);
		Controls.SwapTheirsCivIconBG:SetHide(true);
		Controls.SwapTheirsCivIconShadow:SetHide(true);
		Controls.SwapTheirsCivIconHighlight:SetHide(true);
		-- hide era
		Controls.SwapTheirsEra:SetHide(true);
		
		Controls.SwapTheirsIcon:SetToolTipString("");
	else
		local iClass = Game.GetGreatWorkClass(iIndex);
		local strTexture = "";
		if (iClass == 1) then
			strTexture = "GreatWorks.dds";
		elseif (iClass == 2) then
			strTexture = "GreatWorks.dds";
		elseif (iClass == 3) then
			strTexture = "GreatWorks_Book.dds";
		elseif (iClass == 4) then
			strTexture = "GreatWorks_Music.dds";
		end
		
		if (strImg ~= "") then
			Controls.SwapTheirsIcon:SetTexture(strTexture);
		end

		Controls.SwapTheirsOwnerName:SetHide(false);
		Controls.SwapTheirsCivIcon:SetHide(false);
		Controls.SwapTheirsCivIconBG:SetHide(false);
		Controls.SwapTheirsCivIconShadow:SetHide(false);		
		Controls.SwapTheirsCivIconHighlight:SetHide(false);
		Controls.SwapTheirsEra:SetHide(false);

		local iOwner = Game.GetGreatWorkController(iIndex);
		local tradingPlayer = Players[iOwner];
		g_iTradingPartner = iOwner;
				
		local tradingLeader = GameInfo.Leaders[tradingPlayer:GetLeaderType()]
		IconHookup(tradingLeader.PortraitIndex, 64, tradingLeader.IconAtlas, Controls.SwapTheirWorksIcon);
		CivIconHookup(g_iTradingPartner, 32, Controls.SwapTheirWorksSubIcon, Controls.SwapTheirWorksSubIconBG, Controls.SwapTheirWorksSubIconShadow, (isUsingFlagsPlus == false), true, Controls.SwapTheirWorksSubIconHighlight);
		Controls.SwapTheirIconStack:SetHide(false);
		Controls.SwapTheirsOwnerName:SetText(tradingPlayer:GetCivilizationShortDescription());

		CivIconHookup(Game.GetGreatWorkCreator(iIndex), 32, Controls.SwapTheirsCivIcon, Controls.SwapTheirsCivIconBG, Controls.SwapTheirsCivIconShadow, false, true, Controls.SwapTheirsCivIconHighlight);

		Controls.SwapTheirsEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iIndex)));

		local strTT = Game.GetGreatWorkTooltip(iIndex, g_iTradingPartner);
		
		Controls.SwapTheirIconStack:CalculateSize();
		Controls.SwapTheirIconStack:ReprocessAnchoring();
		
		
		-- Tooltips
		local strTT = Game.GetGreatWorkTooltip(iIndex, activePlayerID)
		Controls.SwapTheirsIcon:SetToolTipString(strTT);
		Controls.SwapTheirsCivIcon:SetToolTipString(strTT);
		Controls.SwapTheirsCivIconBG:SetToolTipString(strTT);
		Controls.SwapTheirsCivIconShadow:SetToolTipString(strTT);
		Controls.SwapTheirsCivIconHighlight:SetToolTipString(strTT);
		Controls.SwapTheirsEra:SetToolTipString(strTT);

		CheckAvailableSwap();
	end
end

function DoSwap ()
	local activePlayerID = Game.GetActivePlayer();
	local activePlayer = Players[activePlayerID];
	if (g_iTheirItem ~= -1 and g_iYourItem ~= -1 and g_iTradingPartner ~= -1) then
		Network.SendSwapGreatWorks(activePlayerID, g_iYourItem, g_iTradingPartner, g_iTheirItem);
	end
end
Controls.SwapButton:RegisterCallback(Mouse.eLClick, DoSwap);

function ClearSwap ()
	SelectYourWork(-1);
	SelectOthersWork(-1);
	Controls.SwapButton:SetDisabled(true);
end
Controls.ClearSwapButton:RegisterCallback( Mouse.eLClick, ClearSwap );

ClearSwap();

function CheckSwappedItems ()
	if (g_iTheirItem ~= -1) then
		if (g_iTradingPartner ~= Game.GetGreatWorkController(g_iTheirItem)) then
			ClearSwap();
		end
	end
	
	if (g_iYourItem ~= -1) then
		if (Game.GetActivePlayer() ~= Game.GetGreatWorkController(g_iYourItem)) then
			ClearSwap();
		end
	end
end

function RefreshSwappingItems()
	
	local activePlayerID = Game.GetActivePlayer();
	local activePlayer = Players[activePlayerID];	
	
	local leader = GameInfo.Leaders[activePlayer:GetLeaderType()]
	IconHookup(leader.PortraitIndex, 64,leader.IconAtlas, Controls.YourWorksIcon);
	CivIconHookup(activePlayerID, 32, Controls.YourWorksSubIcon, Controls.YourWorksSubIconBG, Controls.YourWorksSubIconShadow, (isUsingFlagsPlus == false), true, Controls.YourWorksSubIconHighlight);
		
	function PopulatePullDown(pullDown, iGreatWorkType)
	
		pullDown:ClearEntries();
		local GreatWorks = activePlayer:GetGreatWorks(iGreatWorkType);
		local instance = {};
		pullDown:BuildEntry( "SwapWorkPulldownInstance", instance);
		instance.Name:LocalizeAndSetText("TXT_KEY_SWAP_CLEAR_SPOT");
		instance.CivIcon:SetHide(true);
		instance.CivIconBG:SetHide(true);
		instance.CivIconShadow:SetHide(true);
		instance.CivIconHighlight:SetHide(true);
		instance.Button:SetVoids(iGreatWorkType, -1);
		instance.Button:SetToolTipString(Locale.Lookup("TXT_KEY_NO_SWAPPING_TT"));
		
		for i,v in ipairs(GreatWorks) do
			local instance = {};
			pullDown:BuildEntry( "SwapWorkPulldownInstance", instance);
			instance.Name:SetText(Locale.Lookup(Game.GetGreatWorkName(v.Index)));
			CivIconHookup(v.Creator, 32, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true, instance.CivIconHighlight);
			instance.Era:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(v.Index)));
			instance.Theming:SetText("+" .. Game.GetGreatWorkCurrentThemingBonus(v.Index));
			instance.Button:SetVoids(iGreatWorkType, v.Index);		
			instance.Button:SetToolTipString(Game.GetGreatWorkTooltip(v.Index, activePlayerID));
		end		    
		pullDown:CalculateInternals();
		pullDown:RegisterSelectionCallback(function(workType, index)
			Network.SendSetSwappableGreatWork(activePlayerID, workType, index);
		end);
	end
	
	local iWritingSwapIndex = activePlayer:GetSwappableGreatWriting();
	local strTT = Locale.Lookup("TXT_KEY_WRITING_SLOT_TT");
	if (iWritingSwapIndex == -1) then
		Controls.YourSwapWritingIcon:SetTexture("GreatWorks_Book_Empty.dds");
		Controls.YourSwapWritingEra:SetHide(true);
		Controls.YourSwapWritingCivIcon:SetHide(true);
		Controls.YourSwapWritingCivIconBG:SetHide(true);
		Controls.YourSwapWritingCivIconShadow:SetHide(true);
		Controls.YourSwapWritingCivIconHilite:SetHide(true);
	else
		Controls.YourSwapWritingIcon:SetTexture("GreatWorks_Book.dds");
		Controls.YourSwapWritingEra:SetHide(false);
		Controls.YourSwapWritingEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iWritingSwapIndex)));
		Controls.YourSwapWritingCivIcon:SetHide(false);
		Controls.YourSwapWritingCivIconBG:SetHide(false);
		Controls.YourSwapWritingCivIconShadow:SetHide(false);
		Controls.YourSwapWritingCivIconHilite:SetHide(false);
		CivIconHookup(Game.GetGreatWorkCreator(iWritingSwapIndex), 32, Controls.YourSwapWritingCivIcon, Controls.YourSwapWritingCivIconBG, Controls.YourSwapWritingCivIconShadow, false, true, Controls.YourSwapWritingCivIconHilite);
		strTT = strTT .. "[NEWLINE][NEWLINE]";
		strTT = strTT .. Game.GetGreatWorkTooltip(iWritingSwapIndex, activePlayerID);
	end
	
	local pullDown = Controls.YourWritingPullDown;
	pullDown:GetButton():SetToolTipString(strTT);
	PopulatePullDown(pullDown, 3)
		
	local iArtSwapIndex = activePlayer:GetSwappableGreatArt();
	local strTT = Locale.Lookup("TXT_KEY_ART_SLOT_TT");
	if (iArtSwapIndex == -1) then
		Controls.YourSwapArtIcon:SetTexture("GreatWorks_Empty.dds");
		Controls.YourSwapArtEra:SetHide(true);
		Controls.YourSwapArtCivIcon:SetHide(true);
		Controls.YourSwapArtCivIconBG:SetHide(true);
		Controls.YourSwapArtCivIconShadow:SetHide(true);
		Controls.YourSwapArtCivIconHilite:SetHide(true);
	else
		Controls.YourSwapArtIcon:SetTexture("GreatWorks.dds");
		Controls.YourSwapArtEra:SetHide(false);
		Controls.YourSwapArtEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iArtSwapIndex)));
		Controls.YourSwapArtCivIcon:SetHide(false);
		Controls.YourSwapArtCivIconBG:SetHide(false);
		Controls.YourSwapArtCivIconShadow:SetHide(false);
		Controls.YourSwapArtCivIconHilite:SetHide(false);
		CivIconHookup(Game.GetGreatWorkCreator(iArtSwapIndex), 32, Controls.YourSwapArtCivIcon, Controls.YourSwapArtCivIconBG, Controls.YourSwapArtCivIconShadow, false, true, Controls.YourSwapArtCivIconHilite);
		strTT = strTT .. "[NEWLINE][NEWLINE]";
		strTT = strTT .. Game.GetGreatWorkTooltip(iArtSwapIndex, activePlayerID);
	end
	
	
	local pullDown = Controls.YourArtPullDown;
	pullDown:GetButton():SetToolTipString(strTT);
	PopulatePullDown(pullDown, 1)
		
	local strTT = Locale.Lookup("TXT_KEY_ARTIFACT_SLOT_TT");
	local iArtifactSwapIndex = activePlayer:GetSwappableGreatArtifact();
	if (iArtifactSwapIndex == -1) then
		Controls.YourSwapArtifactIcon:SetTexture("GreatWorks_Empty.dds");
		Controls.YourSwapArtifactEra:SetHide(true);
		Controls.YourSwapArtifactCivIcon:SetHide(true);
		Controls.YourSwapArtifactCivIconBG:SetHide(true);
		Controls.YourSwapArtifactCivIconShadow:SetHide(true);
		Controls.YourSwapArtifactCivIconHilite:SetHide(true);
	else
		Controls.YourSwapArtifactIcon:SetTexture("GreatWorks.dds");
		Controls.YourSwapArtifactEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iArtifactSwapIndex)));
		Controls.YourSwapArtifactEra:SetHide(false);
		Controls.YourSwapArtifactCivIcon:SetHide(false);
		Controls.YourSwapArtifactCivIconBG:SetHide(false);
		Controls.YourSwapArtifactCivIconShadow:SetHide(false);
		Controls.YourSwapArtifactCivIconHilite:SetHide(false);
		CivIconHookup(Game.GetGreatWorkCreator(iArtifactSwapIndex), 32, Controls.YourSwapArtifactCivIcon, Controls.YourSwapArtifactCivIconBG, Controls.YourSwapArtifactCivIconShadow, false, true, Controls.YourSwapArtifactCivIconHilite);
		strTT = strTT .. "[NEWLINE][NEWLINE]";
		strTT = strTT .. Game.GetGreatWorkTooltip(iArtifactSwapIndex, activePlayerID);
	end
	
	local pullDown = Controls.YourArtifactPullDown;
	pullDown:GetButton():SetToolTipString(strTT);
	PopulatePullDown(pullDown, 2)
	
	--local strTT = Locale.Lookup("TXT_KEY_MUSIC_SLOT_TT");
	--local iMusicSwapIndex = activePlayer:GetSwappableGreatMusic();
	--if (iMusicSwapIndex == -1) then
		--Controls.YourSwapMusicIcon:SetTexture("GreatWorks_Music_Empty.dds");
		--Controls.YourSwapMusicEra:SetHide(true);
		--Controls.YourSwapMusicCivIcon:SetHide(true);
		--Controls.YourSwapMusicCivIconBG:SetHide(true);
		--Controls.YourSwapMusicCivIconShadow:SetHide(true);
		--Controls.YourSwapMusicCivIconHilite:SetHide(true);
	--else
		--Controls.YourSwapMusicIcon:SetTexture("GreatWorks_Music.dds");
		----Controls.YourSwapMusicEra:SetText(Locale.Lookup(Game.GetGreatWorkEraAbbreviation(iMusicSwapIndex)));
		--Controls.YourSwapMusicEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(iMusicSwapIndex)));
		--Controls.YourSwapMusicEra:SetHide(false);
		--Controls.YourSwapMusicCivIcon:SetHide(false);
		--Controls.YourSwapMusicCivIconBG:SetHide(false);
		--Controls.YourSwapMusicCivIconShadow:SetHide(false);
		--Controls.YourSwapMusicCivIconHilite:SetHide(false);
		--CivIconHookup(Game.GetGreatWorkCreator(iMusicSwapIndex), 32, Controls.YourSwapMusicCivIcon, Controls.YourSwapMusicCivIconBG, Controls.YourSwapMusicCivIconShadow, false, true, Controls.YourSwapMusicCivIconHilite);
		--strTT = strTT .. "[NEWLINE][NEWLINE]";
		--strTT = strTT .. Game.GetGreatWorkTooltip(iMusicSwapIndex, activePlayerID);
	--end
	--local pullDown = Controls.YourMusicPullDown;
	--pullDown:GetButton():SetToolTipString(strTT);
	--PopulatePullDown(pullDown, 4);

	CheckAvailableSwap(); 
end

function DisplayOthersWorks ()

	local activePlayerID = Game.GetActivePlayer();
	local activePlayer = Players[activePlayerID];	

	Controls.SwapGreatWorksOthersDisplay:SetHide(false);
	
	Controls.OthersGreatWorksStack:DestroyAllChildren();
	
	local OthersGreatWorks = activePlayer:GetOthersGreatWorks();
	for i,v in ipairs(OthersGreatWorks) do
		SwapWorkLineInstance = {};
		ContextPtr:BuildInstanceForControl( "SwapWorkItemLineInstance", SwapWorkLineInstance, Controls.OthersGreatWorksStack );
		
		local player = Players[v.iPlayer];
		local leader = GameInfo.Leaders[player:GetLeaderType()]
		IconHookup(leader.PortraitIndex, 64,leader.IconAtlas, SwapWorkLineInstance.Icon);
		CivIconHookup(v.iPlayer, 32, SwapWorkLineInstance.SubIcon, SwapWorkLineInstance.SubIconBG, SwapWorkLineInstance.SubIconShadow, (isUsingFlagsPlus == false), true, SwapWorkLineInstance.SubIconHighlight);
		SwapWorkLineInstance.IconFrame:SetToolTipString(player:GetCivilizationShortDescription());
		
		
		-- Writing
		--  WritingIndex
		--  WritingCreator
		--  WritingEra
		local strGreatWorkToolTip;
		if (v.WritingIndex == -1) then
			-- show empty art outline
			SwapWorkLineInstance.WonderGreatWritingWork:SetTexture("GreatWorks_Book_Empty.dds");
			-- hide civ symbol
			SwapWorkLineInstance.WritingCivIconBG:SetHide(true);
			SwapWorkLineInstance.WritingCivIconShadow:SetHide(true);
			SwapWorkLineInstance.WritingCivIcon:SetHide(true);
			SwapWorkLineInstance.WritingCivIconHighlight:SetHide(true);
			-- empty text string
			SwapWorkLineInstance.WritingEra:SetHide(true);
			
			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_WRITING_OTHERS_SLOT_EMPTY", player:GetCivilizationShortDescriptionKey());
		else
			-- show correct art image
			SwapWorkLineInstance.WonderGreatWritingWork:SetTexture("GreatWorks_Book.dds");
			-- display civ symbol
			SwapWorkLineInstance.WritingCivIconBG:SetHide(false);
			SwapWorkLineInstance.WritingCivIconShadow:SetHide(false);
			SwapWorkLineInstance.WritingCivIcon:SetHide(false);
			SwapWorkLineInstance.WritingCivIconHighlight:SetHide(false);
			CivIconHookup(v.WritingCreator, 32, SwapWorkLineInstance.WritingCivIcon, SwapWorkLineInstance.WritingCivIconBG, SwapWorkLineInstance.WritingCivIconShadow, false, true, SwapWorkLineInstance.WritingCivIconHighlight);
			
			-- display era string
			SwapWorkLineInstance.WritingEra:SetHide(false);
			--SwapWorkLineInstance.WritingEra:SetText(Locale.Lookup(Game.GetGreatWorkEraAbbreviation(v.WritingIndex)));
			SwapWorkLineInstance.WritingEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(v.WritingIndex)));
			
			SwapWorkLineInstance.WritingWorkButton:RegisterCallback(Mouse.eLClick, function()
				SelectOthersWork(v.WritingIndex);
			end);
			
			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_WRITING_OTHERS_SLOT", player:GetCivilizationShortDescriptionKey());
			strGreatWorkToolTip = strGreatWorkToolTip .. "[NEWLINE][NEWLINE]"
			strGreatWorkToolTip = strGreatWorkToolTip .. Game.GetGreatWorkTooltip(v.WritingIndex, v.iPlayer);
		end
		-- setting up tooltips
		SwapWorkLineInstance.WritingWorkButton:SetToolTipString(strGreatWorkToolTip);

		-- Art
		--  iArtWorkIndex
		--  eArtWorkPlayer
		--  eArtWorkEra
		--  GreatWorks_Empty.dds
		if (v.ArtIndex == -1) then
			-- show empty art outline
			SwapWorkLineInstance.WonderGreatArtWork:SetTexture("GreatWorks_Empty.dds");
			-- hide civ symbol
			SwapWorkLineInstance.ArtCivIconBG:SetHide(true);
			SwapWorkLineInstance.ArtCivIconShadow:SetHide(true);
			SwapWorkLineInstance.ArtCivIcon:SetHide(true);
			SwapWorkLineInstance.ArtCivIconHighlight:SetHide(true);
			-- empty text string
			SwapWorkLineInstance.ArtEra:SetHide(true);
			
			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_ART_OTHERS_SLOT_EMPTY", Players[v.iPlayer]:GetCivilizationShortDescriptionKey());
		else
			-- show correct art image
			SwapWorkLineInstance.WonderGreatArtWork:SetTexture("GreatWorks.dds");
			-- display civ symbol
			SwapWorkLineInstance.ArtCivIconBG:SetHide(false);
			SwapWorkLineInstance.ArtCivIconShadow:SetHide(false);
			SwapWorkLineInstance.ArtCivIcon:SetHide(false);
			SwapWorkLineInstance.ArtCivIconHighlight:SetHide(false);
			CivIconHookup(v.ArtCreator, 32, SwapWorkLineInstance.ArtCivIcon, SwapWorkLineInstance.ArtCivIconBG, SwapWorkLineInstance.ArtCivIconShadow, false, true, SwapWorkLineInstance.ArtCivIconHighlight);
			
			-- display era string
			SwapWorkLineInstance.ArtEra:SetHide(false);
			--SwapWorkLineInstance.ArtEra:SetText(Locale.Lookup(Game.GetGreatWorkEraAbbreviation(v.ArtIndex)));
			SwapWorkLineInstance.ArtEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(v.ArtIndex)));
			
			SwapWorkLineInstance.ArtWorkButton:RegisterCallback(Mouse.eLClick, function()
				SelectOthersWork(v.ArtIndex);
			end);

			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_ART_OTHERS_SLOT", Players[v.iPlayer]:GetCivilizationShortDescriptionKey());
			strGreatWorkToolTip = strGreatWorkToolTip .. "[NEWLINE][NEWLINE]"
			strGreatWorkToolTip = strGreatWorkToolTip .. Game.GetGreatWorkTooltip(v.ArtIndex, v.iPlayer);
		end
		-- setting up tooltips
		SwapWorkLineInstance.ArtWorkButton:SetToolTipString(strGreatWorkToolTip);
		
		-- Artifact
		--  ArtifactIndex
		--  ArtifactCreator
		--  ArtifactEra
		--  GreatWorks_Empty.dds
		if (v.ArtifactIndex == -1) then
			-- show empty art outline
			SwapWorkLineInstance.WonderGreatArtifactWork:SetTexture("GreatWorks_Empty.dds");
			-- hide civ symbol
			SwapWorkLineInstance.ArtifactCivIconBG:SetHide(true);
			SwapWorkLineInstance.ArtifactCivIconShadow:SetHide(true);
			SwapWorkLineInstance.ArtifactCivIcon:SetHide(true);
			SwapWorkLineInstance.ArtifactCivIconHighlight:SetHide(true);
			-- empty text string
			SwapWorkLineInstance.ArtifactEra:SetHide(true);
			
			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_ARTIFACT_OTHERS_SLOT_EMPTY", Players[v.iPlayer]:GetCivilizationShortDescriptionKey());
		else
			-- show correct art image
			SwapWorkLineInstance.WonderGreatArtifactWork:SetTexture("GreatWorks.dds");
			-- display civ symbol
			SwapWorkLineInstance.ArtifactCivIconBG:SetHide(false);
			SwapWorkLineInstance.ArtifactCivIconShadow:SetHide(false);
			SwapWorkLineInstance.ArtifactCivIcon:SetHide(false);
			SwapWorkLineInstance.ArtifactCivIconHighlight:SetHide(false);
			CivIconHookup(v.ArtifactCreator, 32, SwapWorkLineInstance.ArtifactCivIcon, SwapWorkLineInstance.ArtifactCivIconBG, SwapWorkLineInstance.ArtifactCivIconShadow, false, true, SwapWorkLineInstance.ArtifactCivIconHighlight);
			
			-- display era string
			SwapWorkLineInstance.ArtifactEra:SetHide(false);
			--SwapWorkLineInstance.ArtifactEra:SetText(Locale.Lookup(Game.GetGreatWorkEraAbbreviation(v.ArtifactIndex)));		
			SwapWorkLineInstance.ArtifactEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(v.ArtifactIndex)));		
			
			SwapWorkLineInstance.ArtifactWorkButton:RegisterCallback(Mouse.eLClick, function()
				SelectOthersWork(v.ArtifactIndex);
			end);
			
			strGreatWorkToolTip = Locale.Lookup("TXT_KEY_ARTIFACT_OTHERS_SLOT", Players[v.iPlayer]:GetCivilizationShortDescriptionKey());
			strGreatWorkToolTip = strGreatWorkToolTip .. "[NEWLINE][NEWLINE]"
			strGreatWorkToolTip = strGreatWorkToolTip .. Game.GetGreatWorkTooltip(v.ArtifactIndex, v.iPlayer);
		end
		-- setting up tooltips
		SwapWorkLineInstance.ArtifactWorkButton:SetToolTipString(strGreatWorkToolTip);
		
		---- Music
		----  MusicIndex
		----  MusicCreator
		----  MusicEra	
		----  GreatWorks_Music_Empty.dds	
		--if (v.MusicIndex == -1) then
			---- show empty art outline
			--SwapWorkLineInstance.WonderGreatMusicWork:SetTexture("GreatWorks_Music_Empty.dds");
			---- hide civ symbol
			--SwapWorkLineInstance.MusicCivIconBG:SetHide(true);
			--SwapWorkLineInstance.MusicCivIconShadow:SetHide(true);
			--SwapWorkLineInstance.MusicCivIcon:SetHide(true);
			--SwapWorkLineInstance.MusicCivIconHighlight:SetHide(true);
			---- empty text string
			--SwapWorkLineInstance.MusicEra:SetHide(true);
			--
			--strGreatWorkToolTip = Locale.Lookup("TXT_KEY_MUSIC_OTHERS_SLOT_EMPTY", Players[v.iPlayer]:GetCivilizationShortDescription());
		--else
			---- show correct art image
			--SwapWorkLineInstance.WonderGreatMusicWork:SetTexture("GreatWorks_Music.dds");
			---- display civ symbol
			--SwapWorkLineInstance.MusicCivIconBG:SetHide(false);
			--SwapWorkLineInstance.MusicCivIconShadow:SetHide(false);
			--SwapWorkLineInstance.MusicCivIcon:SetHide(false);
			--SwapWorkLineInstance.MusicCivIconHighlight:SetHide(false);
			--CivIconHookup(v.MusicCreator, 32, SwapWorkLineInstance.MusicCivIcon, SwapWorkLineInstance.MusicCivIconBG, SwapWorkLineInstance.MusicCivIconShadow, false, true, SwapWorkLineInstance.MusicCivIconHighlight);
			--
			---- display era string
			--SwapWorkLineInstance.MusicEra:SetHide(false);
			----SwapWorkLineInstance.MusicEra:SetText(Locale.Lookup(Game.GetGreatWorkEraAbbreviation(v.MusicIndex)));		
			--SwapWorkLineInstance.MusicEra:SetText(Locale.Lookup(Game.GetGreatWorkEraShort(v.MusicIndex)));		
			--
			--SwapWorkLineInstance.MusicWorkButton:RegisterCallback(Mouse.eLClick, function()
				--SelectOthersWork(v.MusicIndex);
			--end);			
			--
			--strGreatWorkToolTip = Locale.Lookup("TXT_KEY_MUSIC_OTHERS_SLOT", Players[v.iPlayer]:GetCivilizationShortDescription());
			--strGreatWorkToolTip = strGreatWorkToolTip .. "[NEWLINE][NEWLINE]"
			--strGreatWorkToolTip = strGreatWorkToolTip .. Game.GetGreatWorkTooltip(v.MusicIndex, v.iPlayer);
		--end
		---- setting up tooltips
		--SwapWorkLineInstance.MusicWorkButton:SetToolTipString(strGreatWorkToolTip);
		
	end
		
	Controls.OthersGreatWorksStack:CalculateSize();
	Controls.OthersGreatWorksStack:ReprocessAnchoring();
	Controls.OthersGreatWorksScroll:CalculateInternalSize();
end

function DisplaySwapGreatWorks()
	RefreshSwappingItems();
	DisplayOthersWorks();
end

function RefreshSwapGreatWorks()
	CheckSwappedItems(); -- For if they've already been swapped
	DisplaySwapGreatWorks();
end
Events.SerialEventGreatWorksScreenDirty.Add(RefreshSwapGreatWorks);

g_Tabs["SwapGreatWorks"].RefreshContent = function()
	DisplaySwapGreatWorks();
end

---- END ----
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
function RefreshCultureVictory()

	local activePlayer = Players[Game.GetActivePlayer()];
 	local activeTeam = Teams[activePlayer:GetTeam()];
   
	g_CultureVictory = {};
	
	
	for iPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do	
		local pPlayer = Players[iPlayer];
		if (pPlayer:IsAlive() and not pPlayer:IsMinorCiv() and activeTeam:IsHasMet(pPlayer:GetTeam())) then
			local row = {};
			
			row.strPlayerName = Locale.Lookup(pPlayer:GetCivilizationShortDescriptionKey()); 
    	
    		local iNumInfluential = pPlayer:GetNumCivsInfluentialOn();
    		local iNumToBeInfluential = pPlayer:GetNumCivsToBeInfluentialOn();
    		local szText = Locale.ConvertTextKey("TXT_KEY_CO_VICTORY_INFLUENTIAL_OF", iNumInfluential, iNumToBeInfluential);
    		if (iNumInfluential == 0) then
				row.strInfluenceNum = szText;
			else
				row.strInfluenceNum = "[COLOR_MAGENTA]" .. szText .. "[ENDCOLOR]";
			end
			
			if(iNumToBeInfluential == 0) then
				row.InfluencePct = 0;		
			else
				row.InfluencePct = iNumInfluential/iNumToBeInfluential;
			end
			
			local iTourism = pPlayer:GetTourism();
			row.Tourism = iTourism;
			row.strTourism = tostring(iTourism);
			
			row.strIdeology = "";
			row.strPublicOpinion = "";
			row.strPublicOpinionToolTip = nil;
			row.strPublicOpinionUnhappiness = "";
			row.strPublicOpinionUnhappinessToolTip = nil;
			row.strExcessHappiness = "";
			
			local iIdeology = pPlayer:GetLateGamePolicyTree();
			if (iIdeology ~= PolicyBranchTypes.NO_POLICY_BRANCH_TYPE) then
				row.strIdeology = Locale.Lookup(GameInfo.PolicyBranchTypes[iIdeology].Description);
			   
				local szOpinionString;
				local iOpinion = pPlayer:GetPublicOpinionType();
				if (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_CONTENT) then
					szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CONTENT");
				elseif (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_DISSIDENTS) then
					szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_DISSIDENTS");
				elseif (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_CIVIL_RESISTANCE) then
					szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_CIVIL_RESISTANCE");
				elseif (iOpinion == PublicOpinionTypes.PUBLIC_OPINION_REVOLUTIONARY_WAVE) then
					szOpinionString = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_REVOLUTIONARY_WAVE");
				end
				row.iPublicOpinion = iOpinion;
				row.strPublicOpinion = szOpinionString;
				row.strPublicOpinionToolTip = pPlayer:GetPublicOpinionTooltip();
				
				local iUnhappiness = -1 * pPlayer:GetPublicOpinionUnhappiness();
				local strPublicOpinionUnhappiness = tostring(0);
				if (iUnhappiness < 0) then
					strPublicOpinionUnhappiness = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS", iUnhappiness);
				end
				
				row.iUnhappiness = iUnhappiness;
				row.strPublicOpinionUnhappiness = strPublicOpinionUnhappiness;
				row.strPublicOpinionUnhappinessToolTip = pPlayer:GetPublicOpinionUnhappinessTooltip();
				
				local iExcessHappiness = pPlayer:GetExcessHappiness();
				local strExcessHappiness = tostring(0);
				if (iExcessHappiness < 0) then
					strExcessHappiness = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_UNHAPPINESS", iExcessHappiness);
				elseif (iExcessHappiness> 0) then
					strExcessHappiness = Locale.ConvertTextKey("TXT_KEY_CO_PUBLIC_OPINION_HAPPINESS", iExcessHappiness);				
				end
				row.iExcessHappiness = iExcessHappiness;
				row.strExcessHappiness = strExcessHappiness;
			end
			
			table.insert(g_CultureVictory, row);
		end
    end
end

function SortAndDisplayCultureVictory()
	local model = g_CultureVictory;
	table.sort(model, g_CultureVictorySortFunction);
	
	Controls.VictoryStack:DestroyAllChildren();
	
	for i, row in ipairs(model) do
		local instance = {};
		ContextPtr:BuildInstanceForControl( "PlayerVictoryInstance", instance, Controls.VictoryStack );
	
		instance.PlayerName:SetText(row.strPlayerName);
    	
		instance.InfluentialNum:SetText( row.strInfluenceNum);
		
		instance.Tourism:SetText(row.strTourism );
		
		instance.Ideology:SetText(row.strIdeology);
		
		instance.PublicOpinion:SetText(row.strPublicOpinion);
		instance.PublicOpinion:SetToolTipString(row.strPublicOpinionToolTip);
		
		instance.PublicOpinionUnhappiness:SetText(row.strPublicOpinionUnhappiness);
		instance.PublicOpinionUnhappiness:SetToolTipString(row.strPublicOpinionUnhappinessToolTip);
		
		instance.ExcessHappiness:SetText(row.strExcessHappiness);
	end
    
    Controls.VictoryStack:CalculateSize();
    Controls.VictoryStack:ReprocessAnchoring();
    Controls.VictoryScroll:CalculateInternalSize();
end

g_Tabs["CultureVictory"].RefreshContent = function()
	RefreshCultureVictory();
	SortAndDisplayCultureVictory();
end

function RefreshPlayerInfluence()

	local influenceLookup = {
		[InfluenceLevelTypes.INFLUENCE_LEVEL_UNKNOWN] = "TXT_KEY_CO_UNKNOWN",
		[InfluenceLevelTypes.INFLUENCE_LEVEL_EXOTIC] = "TXT_KEY_CO_EXOTIC",
		[InfluenceLevelTypes.INFLUENCE_LEVEL_FAMILIAR] = "TXT_KEY_CO_FAMILIAR",
		[InfluenceLevelTypes.INFLUENCE_LEVEL_POPULAR] = "TXT_KEY_CO_POPULAR",
		[InfluenceLevelTypes.INFLUENCE_LEVEL_INFLUENTIAL] = "TXT_KEY_CO_INFLUENTIAL",
		[InfluenceLevelTypes.INFLUENCE_LEVEL_DOMINANT] = "TXT_KEY_CO_DOMINANT",
	};
					
	g_InfluenceByPlayer = {};

    Controls.InfluenceStack:DestroyAllChildren();

	local activePlayer = Players[Game.GetActivePlayer()];
	local activeTeam = Teams[activePlayer:GetTeam()];
	
	local selectedPlayer = Players[g_iSelectedPlayerID];
	
	g_InfluenceByPlayer.SelectedPlayerTourism = selectedPlayer:GetTourism();
		
	g_InfluenceByPlayer.Players = {};
	
	-- Add pulldown options	
	for iPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do	
		local pPlayer = Players[iPlayer];
		if (pPlayer:IsAlive() and not pPlayer:IsMinorCiv() and activeTeam:IsHasMet(pPlayer:GetTeam())) then
			table.insert(g_InfluenceByPlayer.Players, {
				Text = Locale.Lookup(pPlayer:GetCivilizationShortDescriptionKey()),
				Id = pPlayer:GetID()
			});	
		end
	end
	
	g_InfluenceByPlayer.PlayerInfluence = {};
	
	local pSelectedPlayer = Players[g_iSelectedPlayerID];
	for iPlayer = 0, GameDefines.MAX_CIV_PLAYERS - 1 do	
		local pPlayer = Players[iPlayer];
		if (pPlayer:IsAlive() and not pPlayer:IsMinorCiv()) then
		
			if (iPlayer ~= g_iSelectedPlayerID) then
				
				local iInfluenceLevel = pSelectedPlayer:GetInfluenceLevel(iPlayer);
    			if (iInfluenceLevel ~= InfluenceLevelTypes.NO_INFLUENCE_LEVEL) then

					local playerInfluence = {};
					playerInfluence.Level = iInfluenceLevel;
					playerInfluence.LevelText = Locale.ConvertTextKey(influenceLookup[iInfluenceLevel]);
				
				
					
				
					local iInfluence = pSelectedPlayer:GetInfluenceOn(iPlayer);
					local iCulture = pPlayer:GetJONSCultureEverGenerated();
					local iPercent = 0;
					
					if (iCulture > 0) then
						iPercent = iInfluence / iCulture;
					end
					
					playerInfluence.Influence = iInfluence;
					playerInfluence.InfluencePercent = iPercent;
					playerInfluence.Culture = iCulture;
				
					if(activeTeam:IsHasMet(pPlayer:GetTeam())) then
						playerInfluence.PlayerId = iPlayer;
						playerInfluence.PlayerName = Locale.Lookup(pPlayer:GetCivilizationShortDescriptionKey());
						playerInfluence.InfluenceToolTip = Locale.Lookup("TXT_KEY_CO_THIRD_PARTY_CULTURE_INFLUENCE", pSelectedPlayer:GetCivilizationShortDescriptionKey(), iPercent * 100, pPlayer:GetCivilizationShortDescriptionKey());
					else
						playerInfluence.PlayerId = -1;
						playerInfluence.PlayerName = Locale.Lookup("TXT_KEY_RO_WR_UNKNOWN_CIV");
						playerInfluence.InfluenceToolTip = Locale.Lookup("TXT_KEY_CO_THIRD_PARTY_CULTURE_INFLUENCE_UNMET", pSelectedPlayer:GetCivilizationShortDescriptionKey(), iPercent * 100);
					end
					
					local strText;
				    if (iInfluenceLevel == 0) then
						strText = Locale.ConvertTextKey("TXT_KEY_CO_EXOTIC");
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_TOURISM_PERCENT_TT", playerInfluence.InfluenceToolTip, strText, GameDefines.CULTURE_LEVEL_EXOTIC);
				    elseif (iInfluenceLevel == 1) then
						strText = Locale.ConvertTextKey("TXT_KEY_CO_FAMILIAR");
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_TOURISM_PERCENT_TT", playerInfluence.InfluenceToolTip, strText, GameDefines.CULTURE_LEVEL_FAMILIAR);
				    elseif (iInfluenceLevel == 2) then
						strText = Locale.ConvertTextKey("TXT_KEY_CO_POPULAR");
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_TOURISM_PERCENT_TT", playerInfluence.InfluenceToolTip, strText, GameDefines.CULTURE_LEVEL_POPULAR);
						playerInfluence.LevelToolTip = playerInfluence.LevelToolTip .. Locale.ConvertTextKey("TXT_KEY_CO_INFLUENCE_BONUSES_FAMILIAR");
				    elseif (iInfluenceLevel == 3) then
						strText = Locale.ConvertTextKey("TXT_KEY_CO_INFLUENTIAL");
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_TOURISM_PERCENT_TT", playerInfluence.InfluenceToolTip, strText, GameDefines.CULTURE_LEVEL_INFLUENTIAL);				    
						playerInfluence.LevelToolTip = playerInfluence.LevelToolTip .. Locale.ConvertTextKey("TXT_KEY_CO_INFLUENCE_BONUSES_POPULAR");
				    elseif (iInfluenceLevel == 4) then
						strText = Locale.ConvertTextKey("TXT_KEY_CO_DOMINANT");
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_TOURISM_PERCENT_TT", playerInfluence.InfluenceToolTip, strText, GameDefines.CULTURE_LEVEL_DOMINANT);	
						playerInfluence.LevelToolTip = playerInfluence.LevelToolTip .. Locale.ConvertTextKey("TXT_KEY_CO_INFLUENCE_BONUSES_INFLUENTIAL");
					else
						playerInfluence.LevelToolTip = Locale.ConvertTextKey("TXT_KEY_CO_INFLUENCE_BONUSES_DOMINANT");
				    end
			
					local iModifiers = pSelectedPlayer:GetTourismModifierWith(iPlayer);
					local strModifiersToolTip = pSelectedPlayer:GetTourismModifierWithTooltip(iPlayer);
					
					playerInfluence.Modifier = iModifiers;
					playerInfluence.ModifierToolTip = strModifiersToolTip;
					
					playerInfluence.TourismPerTurn = pSelectedPlayer:GetInfluencePerTurn(iPlayer);
					playerInfluence.TourismPerTurn = math.floor(playerInfluence.TourismPerTurn);
					
					if(playerInfluence.TourismPerTurn > 0) then
						playerInfluence.TourismPerTurnText = "+" .. tostring(playerInfluence.TourismPerTurn);
					else
						playerInfluence.TourismPerTurnText = tostring(playerInfluence.TourismPerTurn);
					end
					
					if(iModifiers == 0) then
						playerInfluence.ModifierText = "--";
					elseif(iModifiers > 0) then
						playerInfluence.ModifierText = "+" .. iModifiers .. "%";
					else
						playerInfluence.ModifierText = iModifiers .. "%";
					end
					
					local iTrend = pSelectedPlayer:GetInfluenceTrend(iPlayer);			
    				playerInfluence.Trend = iTrend;
    				playerInfluence.TrendRate = 0;
    				playerInfluence.TrendTooltip = "";
    				
    				if( iTrend == InfluenceLevelTrend.INFLUENCE_TREND_FALLING ) then
						playerInfluence.TrendText = Locale.Lookup( "TXT_KEY_CO_FALLING" );
						playerInfluence.TrendRate = -1;
					elseif( iTrend == InfluenceLevelTrend.INFLUENCE_TREND_STATIC ) then
						playerInfluence.TrendText = Locale.Lookup( "TXT_KEY_CO_STATIC");
						playerInfluence.TrendRate = 0;
					elseif (iTrend == InfluenceLevelTrend.INFLUENCE_TREND_RISING and pSelectedPlayer:GetTurnsToInfluential(iPlayer) == 999) then
						playerInfluence.TrendText = Locale.Lookup( "TXT_KEY_CO_RISING_SLOWLY");	
						playerInfluence.TrendRate = 1;				
					elseif( iTrend == InfluenceLevelTrend.INFLUENCE_TREND_RISING and iInfluenceLevel < InfluenceLevelTypes.INFLUENCE_LEVEL_DOMINANT) then
						playerInfluence.TrendText = Locale.Lookup("TXT_KEY_CO_RISING");
						playerInfluence.TrendRate = 2;
						
						if(iInfluenceLevel < InfluenceLevelTypes.INFLUENCE_LEVEL_INFLUENTIAL) then
							playerInfluence.TrendTooltip = Locale.Lookup("TXT_KEY_CO_INFLUENTIAL_TURNS_TT", pSelectedPlayer:GetTurnsToInfluential(iPlayer));
						end
					end
					
					table.insert(g_InfluenceByPlayer.PlayerInfluence, playerInfluence);
    			end
			end
		end
    end
end

function SortAndDisplayPlayerInfluence()
	
	local model = g_InfluenceByPlayer;
	table.sort(model.PlayerInfluence, g_InfluenceByPlayerSortFunction);
	
	Controls.InfluenceStack:DestroyAllChildren();

	Controls.CurrentTourismOutput:SetText(model.SelectedPlayerTourism);
	
	local playerPulldown = Controls.PlayerPD;
	
	function AddToPullDown(text, id)

		local controlTable = {};
		playerPulldown:BuildEntry( "InstanceOne", controlTable );
		
		controlTable.Button:SetVoids(id, 0);
		controlTable.Button:SetText(text);
	end
	
	function SetCurrentSelection(v1)
		for i,v in ipairs(model.Players) do
			if(v.Id == v1) then
				playerPulldown:GetButton():LocalizeAndSetText(v.Text);
				playerPulldown:GetButton():SetToolTipString("");
			end
		end
	end
	
	playerPulldown:ClearEntries();
	
	for i,v in ipairs(model.Players) do
		AddToPullDown(v.Text, v.Id);		
	end
	
	SetCurrentSelection(g_iSelectedPlayerID);
	
	playerPulldown:CalculateInternals();
	playerPulldown:RegisterSelectionCallback(function(v1, v2)
		g_iSelectedPlayerID = v1;	
		RefreshPlayerInfluence();
		SortAndDisplayPlayerInfluence();
	end);
	
	local maxBarWidth = 230;
	local maxCultureInfluence = 0;
	for i, playerInfluence in ipairs(model.PlayerInfluence) do
		maxCultureInfluence = math.max(maxCultureInfluence, playerInfluence.Culture);
		maxCultureInfluence = math.max(maxCultureInfluence, playerInfluence.Influence);	
	end
	
	for i, playerInfluence in ipairs(model.PlayerInfluence) do
		local instance = {};
		ContextPtr:BuildInstanceForControl( "PlayerInfluenceInstance", instance, Controls.InfluenceStack );
        
    	local influencePercent = playerInfluence.InfluencePercent;	
			
		-- Set Civ Icon
		CivIconHookup(playerInfluence.PlayerId, 64, instance.Icon, instance.IconBG, instance.IconShadow, true, true);
		
		CivIconHookup(g_iSelectedPlayerID, 32, instance.MyIcon, instance.MyIconBG, instance.MyIconShadow, true, true);
		CivIconHookup(playerInfluence.PlayerId, 32, instance.TheirIcon, instance.TheirIconBG, instance.TheirIconShadow, true, true);
		
		local influencePercent = math.min(1, influencePercent);
		local width, height = 64,64;	
		
		-- Rather than use the entire size of the control to display percentage, we're only using the area inside the icon frame.
		-- So instead of 0-64, we're using 12-54.
		local newHeight = 42 * (1 - influencePercent) + 12;
		instance.IconDark:SetSizeVal(width, newHeight );
		instance.IconDark:SetTextureSizeVal(width, newHeight);
		instance.IconDark:NormalizeTexture();
	
		instance.IconFrameGlow:SetHide(influencePercent < 1);
		instance.IconFrame:SetToolTipString(playerInfluence.InfluenceToolTip);
        
        instance.PlayerName:LocalizeAndSetText(playerInfluence.PlayerName); 
   		instance.InfluenceLevel:SetText(playerInfluence.LevelText);
   		instance.InfluenceLevel:SetToolTipString(playerInfluence.LevelToolTip);
		instance.InfluenceTourism:SetText(playerInfluence.TourismPerTurnText);
		instance.InfluenceModifiers:SetText(playerInfluence.ModifierText);
		instance.InfluenceModifiers:SetToolTipString(playerInfluence.ModifierToolTip);
		instance.InfluenceTrend:SetText(playerInfluence.TrendText);			
		
		local trendRate = playerInfluence.TrendRate;
		
		if(trendRate == -1 ) then
			instance.InfluenceTrendIcon:SetHide(false);
			instance.InfluenceTrendIcon:SetTextureOffsetVal(0,0);
			instance.InfluenceTrendIcon:SetColor{x = 1, y = 0, z = 0, w = 1};
		elseif(trendRate == 0 ) then
			instance.InfluenceTrendIcon:SetHide(true);
		elseif(trendRate == 1) then
			instance.InfluenceTrendIcon:SetHide(false);
			instance.InfluenceTrendIcon:SetTextureOffsetVal(36,0);
			instance.InfluenceTrendIcon:SetColor{x = 0, y = 1, z = 0, w = 1};
		elseif(trendRate == 2) then
			instance.InfluenceTrendIcon:SetHide(false)
			instance.InfluenceTrendIcon:SetTextureOffsetVal(36,36);
			instance.InfluenceTrendIcon:SetColor{x = 0, y = 1, z = 0, w = 1};
			instance.InfluenceTrendIcon:SetToolTipString(playerInfluence.TrendTooltip);
		end
		
		instance.MyTourism:SetHide(playerInfluence.Influence == 0);
		instance.TheirCulture:SetHide(playerInfluence.Culture == 0);
		
		instance.TourismBar:SetHide(playerInfluence.Influence == 0);
		instance.TourismIcon:SetHide(playerInfluence.Influence == 0);
		
		local influenceWidth = (playerInfluence.Influence/maxCultureInfluence) * maxBarWidth;
		
		instance.MyTourism:SetSizeVal(influenceWidth, 8);
		instance.MyTourism:SetToolTipString("[ICON_TOURISM] " .. playerInfluence.Influence);
		
		instance.TheirCulture:SetSizeVal((playerInfluence.Culture/maxCultureInfluence) * maxBarWidth, 8);
		instance.TheirCulture:SetToolTipString("[ICON_CULTURE] " .. playerInfluence.Culture);
			
		instance.TourismIcon:SetOffsetVal(influenceWidth + 16, 2);
		instance.TourismBar:SetOffsetVal(influenceWidth + 28, 18);
		
    end
    
    Controls.InfluenceStack:CalculateSize();
    Controls.InfluenceStack:ReprocessAnchoring();
    Controls.InfluenceScroll:CalculateInternalSize();
	
end

g_Tabs["PlayerInfluence"].RefreshContent = function()
	RefreshPlayerInfluence();
	SortAndDisplayPlayerInfluence();
end;

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	-- Set Civ Icon
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
  
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();  
        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	
        	TabSelect(g_CurrentTab);
        else
		    UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

RegisterSortOptions();
TabSelect("YourCulture");