-------------------------------------------------
-- Worker Planner
-- Plan improvements and routes for automated workers, and see what the AI plans to build.
--
-- To keep the graphics load low, map highlights and icons are only shown for plots in view
-- (see CameraView.lua), icons are pooled and capped, and everything is released on close.
-------------------------------------------------
include("IconSupport")
include("InstanceManager")
include("FLuaVector")
include("CPK.lua")

local MakeInstance = CPK.UI.Control.Instance.Make

local MAX_PRIORITY = 3
local MAX_MAP_ICONS = 200 -- hard cap on world anchors
local POLL_INTERVAL = 0.25 -- seconds between checks for changed data
local CALLER = "WorkerPlanner"

local INTERFACEMODE_WORKER_PLANNER = GameInfoTypes.INTERFACEMODE_WORKER_PLANNER
local NO_IMPROVEMENT = -1
local Status = WorkerPlanStatusTypes
local RoutePlan = RoutePlanTypes

-------------------------------------------------
-- Highlight styles (defined in Highlights.xml)
-------------------------------------------------
local STYLE_SELECTED = "WorkerPlanSelected"
local SELECTED_COLOR = Vector4(1, 1, 1, 1)
local STYLE_BUILT = "WorkerPlanBuilt"
local STYLE_IN_PROGRESS = "WorkerPlanInProgress"
local STYLE_ASSIGNED = "WorkerPlanAssigned"
local STYLE_PRIORITY = "WorkerPlanPriority"
local STYLE_HIGH = "WorkerPlanQueuedHigh"
local STYLE_LOW = "WorkerPlanQueuedLow"
local STYLE_NOT_YET = "WorkerPlanNotYet"
local STYLE_BLOCKED = "WorkerPlanBlocked"
local STYLE_LOCKED = "WorkerPlanLocked"
local STYLE_ROUTE_CONNECTION = "RoutePlannerCityConnection"
local STYLE_ROUTE_INCLUDE = "WorkerPlanRoutePlan"
local STYLE_ROUTE_EXCLUDE = "WorkerPlanRouteNoBuild"
local STYLE_ROUTE_PRIORITY = "WorkerPlanRoutePriority"
local STYLE_ROUTE_MAIN = "WorkerPlanRouteMain"
local STYLE_ROUTE_SHORTCUT = "WorkerPlanRouteShortcut"
local STYLE_ROUTE_STRATEGIC = "WorkerPlanRouteStrategic"

local ALL_STYLES = {
	STYLE_BUILT, STYLE_IN_PROGRESS, STYLE_ASSIGNED, STYLE_PRIORITY, STYLE_HIGH, STYLE_LOW,
	STYLE_NOT_YET, STYLE_BLOCKED, STYLE_LOCKED, STYLE_ROUTE_CONNECTION, STYLE_ROUTE_INCLUDE,
	STYLE_ROUTE_EXCLUDE, STYLE_ROUTE_PRIORITY, STYLE_ROUTE_MAIN, STYLE_ROUTE_SHORTCUT, STYLE_ROUTE_STRATEGIC,
}

-- Legend colors must match Highlights.xml
local STATUS_LEGEND = {
	{ STYLE_PRIORITY, "TXT_KEY_WP_STATUS_PRIORITY", { 0, 220, 220 } },
	{ STYLE_HIGH, "TXT_KEY_WP_STATUS_HIGH", { 0, 200, 0 } },
	{ STYLE_LOW, "TXT_KEY_WP_STATUS_LOW", { 220, 220, 0 } },
	{ STYLE_ASSIGNED, "TXT_KEY_WP_STATUS_ASSIGNED", { 60, 120, 255 } },
	{ STYLE_IN_PROGRESS, "TXT_KEY_WP_STATUS_IN_PROGRESS", { 255, 255, 255 } },
	{ STYLE_NOT_YET, "TXT_KEY_WP_STATUS_NOT_YET", { 255, 140, 0 } },
	{ STYLE_BLOCKED, "TXT_KEY_WP_STATUS_BLOCKED", { 220, 0, 0 } },
	{ STYLE_LOCKED, "TXT_KEY_WP_STATUS_LOCKED", { 170, 70, 220 } },
	{ STYLE_BUILT, "TXT_KEY_WP_STATUS_BUILT", { 128, 128, 128 } },
}
local ROUTE_LEGEND = {
	{ STYLE_ROUTE_INCLUDE, "TXT_KEY_WP_DRAW_ROAD_PLAN", { 0, 196, 0 } },
	{ STYLE_ROUTE_MAIN, "TXT_KEY_WP_ROUTE_MAIN", { 255, 255, 255 } },
	{ STYLE_ROUTE_PRIORITY, "TXT_KEY_WP_DRAW_PRIORITY_ROUTE", { 0, 220, 220 } },
	{ STYLE_ROUTE_SHORTCUT, "TXT_KEY_WP_ROUTE_SHORTCUT", { 128, 255, 0 } },
	{ STYLE_ROUTE_EXCLUDE, "TXT_KEY_WP_DRAW_NO_BUILD_ZONE", { 196, 0, 0 } },
	{ STYLE_ROUTE_STRATEGIC, "TXT_KEY_WP_ROUTE_STRATEGIC", { 255, 166, 0 } },
	{ STYLE_ROUTE_CONNECTION, "TXT_KEY_WP_ROUTE_CONNECTION", { 255, 234, 0 } },
}

local STATUS_TEXT = {
	[Status.WORKER_PLAN_STATUS_NONE] = "TXT_KEY_WP_STATUS_NONE",
	[Status.WORKER_PLAN_STATUS_BUILT] = "TXT_KEY_WP_STATUS_BUILT",
	[Status.WORKER_PLAN_STATUS_IN_PROGRESS] = "TXT_KEY_WP_STATUS_IN_PROGRESS",
	[Status.WORKER_PLAN_STATUS_ASSIGNED] = "TXT_KEY_WP_STATUS_ASSIGNED",
	[Status.WORKER_PLAN_STATUS_QUEUED] = "TXT_KEY_WP_STATUS_QUEUED",
	[Status.WORKER_PLAN_STATUS_NOT_YET] = "TXT_KEY_WP_STATUS_NOT_YET",
	[Status.WORKER_PLAN_STATUS_BLOCKED] = "TXT_KEY_WP_STATUS_BLOCKED",
	[Status.WORKER_PLAN_STATUS_LOCKED] = "TXT_KEY_WP_STATUS_LOCKED",
	[Status.WORKER_PLAN_STATUS_SUGGESTED] = "TXT_KEY_WP_STATUS_SUGGESTED",
	[Status.WORKER_PLAN_STATUS_OUTSIDE_BORDERS] = "TXT_KEY_WP_STATUS_OUTSIDE_BORDERS",
}

-------------------------------------------------
-- State
-------------------------------------------------
local g_bOpen = false
local g_bClosing = false
local g_iPlayer = Game.GetActivePlayer()
local g_pPlayer = Players[g_iPlayer]
local g_bImprovementsTab = true

local g_overview = {} -- plot index -> overview entry from the DLL
local g_iRevision = -1
local g_bDirty = false -- plans changed, re-plan soon
local g_fRefreshDelay = nil -- seconds until re-planning
local REFRESH_DELAY = 0.75 -- wait for painting to stop before re-planning

local function MarkDirty()
	g_bDirty = true
	g_fRefreshDelay = REFRESH_DELAY
end
local g_iHighRankLimit = 5
local g_fTimeSinceCheck = 0

local g_inView = {} -- plot index -> true
local g_shownStyles = {} -- plot index -> { style -> true }
local g_selected = nil -- selected plot index
local g_bSelectedShown = false

local g_activeIcons = {} -- plot index -> { inst, key }
local g_iconPool = {} -- hidden icons
local g_iNumIcons = 0 -- created icons

-- What gets applied when clicking an option or painting
local g_brush = { improvement = NO_IMPROVEMENT, priority = 0, lock = false }
local g_routeBrush = RoutePlan.NO_PLANNED_ROUTE -- route plan applied when clicking an option or painting
local g_bPainting = false
local g_iLastPainted = nil

-- Collapsible option sections, great person improvements are collapsed by default
local g_sections = {
	normal = { im = InstanceManager:new("OptionInstance", "OptionButton", Controls.NormalOptionsStack), header = Controls.NormalHeader, stack = Controls.NormalOptionsStack, title = "TXT_KEY_WP_SECTION_NORMAL", collapsed = false },
	gp = { im = InstanceManager:new("OptionInstance", "OptionButton", Controls.GPOptionsStack), header = Controls.GPHeader, stack = Controls.GPOptionsStack, title = "TXT_KEY_WP_SECTION_GP", collapsed = true },
}

local function ResetOptions()
	for _, section in pairs(g_sections) do
		section.im:ResetInstances()
	end
end

local function IsGreatPersonImprovement(improvement)
	local info = GameInfo.Improvements[improvement]
	return info ~= nil and (info.CreatedByGreatPerson == true or info.CreatedByGreatPerson == 1)
end

-- Section headers show the number of options and whether they are expanded
local function LayoutSections(counts)
	for _, section in pairs(g_sections) do
		local count = counts[section] or 0
		section.header:SetHide(count == 0)
		section.header:SetText((section.collapsed and "[+] " or "[-] ") .. Locale.ConvertTextKey(section.title) .. " (" .. count .. ")")
		section.stack:SetHide(section.collapsed)
		section.stack:CalculateSize()
	end
	Controls.OptionsStack:CalculateSize()
	Controls.OptionsScrollPanel:CalculateInternalSize()
end

-- A build for each improvement, for icons
local g_improvementBuild = {}
for row in GameInfo.Builds() do
	if row.ImprovementType then
		local improvement = GameInfoTypes[row.ImprovementType]
		if improvement and not g_improvementBuild[improvement] then
			g_improvementBuild[improvement] = row
		end
	end
end

local function L(key, ...)
	return Locale.ConvertTextKey(key, ...)
end

local function ImprovementName(improvement)
	local info = improvement and improvement ~= NO_IMPROVEMENT and GameInfo.Improvements[improvement]
	return info and L(info.Description) or L("TXT_KEY_WP_NOTHING")
end

local function SetBuildIcon(improvement, control, size)
	local build = g_improvementBuild[improvement]
	if build then
		IconHookup(build.IconIndex, size or 45, build.IconAtlas, control)
		control:SetHide(false)
	else
		control:SetHide(true)
	end
end

-- Icon sizes available per atlas
local g_atlasSizes = {}
for row in GameInfo.IconTextureAtlases() do
	g_atlasSizes[row.Atlas] = g_atlasSizes[row.Atlas] or {}
	g_atlasSizes[row.Atlas][row.IconSize] = true
end

-- Map icon: build icon if it exists in this size, else the improvement icon, else the largest smaller build icon
local function SetMapIcon(improvement, control, size)
	local build = g_improvementBuild[improvement]
	local info = GameInfo.Improvements[improvement]
	local candidates = {}
	if build then
		table.insert(candidates, { build.IconIndex, build.IconAtlas })
	end
	if info then
		table.insert(candidates, { info.PortraitIndex, info.IconAtlas })
	end

	for _, candidate in ipairs(candidates) do
		local sizes = g_atlasSizes[candidate[2]]
		if sizes and sizes[size] and IconHookup(candidate[1], size, candidate[2], control) then
			control:SetSizeVal(size, size)
			control:SetHide(false)
			return
		end
	end

	for _, candidate in ipairs(candidates) do
		local best = nil
		for s in pairs(g_atlasSizes[candidate[2]] or {}) do
			if s < size and (not best or s > best) then
				best = s
			end
		end
		if best and IconHookup(candidate[1], best, candidate[2], control) then
			control:SetSizeVal(best, best)
			control:SetHide(false)
			return
		end
	end

	control:SetHide(true)
end

-------------------------------------------------
-- Data
-------------------------------------------------
local function RefreshData()
	g_iRevision = g_pPlayer:GetWorkerPlanRevision()
	g_overview = {}
	for _, entry in ipairs(g_pPlayer:GetWorkerPlanOverview()) do
		g_overview[entry.PlotIndex] = entry
	end

	-- Directives builders will likely get to soon count as high priority
	local iNumBuilders = 0
	for unit in g_pPlayer:Units() do
		if unit:WorkRate(true) > 0 then
			iNumBuilders = iNumBuilders + 1
		end
	end
	g_iHighRankLimit = math.max(5, 2 * iNumBuilders)
end

-- Highlight styles wanted for a plot in the current tab
local function GetDesiredStyles(idx)
	local styles = {}

	if g_bImprovementsTab then
		local entry = g_overview[idx]
		if not entry or not Controls.ShowStatusCheckBox:IsChecked() then
			return styles
		end

		local status = entry.Status
		if status == Status.WORKER_PLAN_STATUS_BUILT then
			if Controls.ShowBuiltCheckBox:IsChecked() then
				styles[STYLE_BUILT] = true
			end
		elseif status == Status.WORKER_PLAN_STATUS_IN_PROGRESS then
			styles[STYLE_IN_PROGRESS] = true
		elseif status == Status.WORKER_PLAN_STATUS_ASSIGNED then
			styles[STYLE_ASSIGNED] = true
		elseif status == Status.WORKER_PLAN_STATUS_QUEUED or status == Status.WORKER_PLAN_STATUS_SUGGESTED then
			if entry.HasPlan and entry.PlanPriority > 0 then
				styles[STYLE_PRIORITY] = true
			elseif entry.Rank >= 0 and entry.Rank < g_iHighRankLimit then
				styles[STYLE_HIGH] = true
			else
				styles[STYLE_LOW] = true
			end
		elseif status == Status.WORKER_PLAN_STATUS_NOT_YET or status == Status.WORKER_PLAN_STATUS_OUTSIDE_BORDERS then
			styles[STYLE_NOT_YET] = true
		elseif status == Status.WORKER_PLAN_STATUS_BLOCKED then
			styles[STYLE_BLOCKED] = true
		elseif status == Status.WORKER_PLAN_STATUS_LOCKED then
			styles[STYLE_LOCKED] = true
		end
	else
		local plot = Map.GetPlotByIndex(idx)
		if not plot then
			return styles
		end

		if Controls.ShowCityConnectionsCheckBox:IsChecked() and plot:IsTradeRoute(g_iPlayer) then
			styles[STYLE_ROUTE_CONNECTION] = true
		end

		-- One fill per plot, your plan is shown over the AI's
		local plan = plot:GetPlannedRouteState(g_iPlayer)
		if Controls.ShowRoadPlansCheckBox:IsChecked() and plan ~= RoutePlan.NO_PLANNED_ROUTE then
			if plan == RoutePlan.ROAD_PLANNING_INCLUDE then
				styles[STYLE_ROUTE_INCLUDE] = true
			elseif plan == RoutePlan.ROAD_PLANNING_EXCLUDE then
				styles[STYLE_ROUTE_EXCLUDE] = true
			elseif plan == RoutePlan.ROAD_PLANNING_PRIORITY_CONSTRUCTION then
				styles[STYLE_ROUTE_PRIORITY] = true
			end
		elseif Controls.ExplainAutomatedRoutesCheckBox:IsChecked() then
			if plot:IsMainRoutePlan(g_iPlayer) then
				styles[STYLE_ROUTE_MAIN] = true
			elseif plot:IsShortcutRoutePlan(g_iPlayer) then
				styles[STYLE_ROUTE_SHORTCUT] = true
			elseif plot:IsStrategicRoutePlan(g_iPlayer) then
				styles[STYLE_ROUTE_STRATEGIC] = true
			end
		end
	end

	return styles
end

-- Icon wanted for a plot: improvement, is player plan, badge text
local function GetDesiredIcon(idx)
	if not g_bImprovementsTab or not Controls.ShowIconsCheckBox:IsChecked() then
		return nil
	end

	local entry = g_overview[idx]
	if not entry then
		return nil
	end

	if entry.HasPlan then
		if entry.PlanImprovement == NO_IMPROVEMENT then
			return nil
		end
		local badge = entry.PlanPriority > 0 and tostring(entry.PlanPriority) or ""
		return entry.PlanImprovement, true, badge
	end

	-- Don't clutter the map with what's already there
	if entry.Status == Status.WORKER_PLAN_STATUS_BUILT or entry.AIImprovement == NO_IMPROVEMENT then
		return nil
	end

	return entry.AIImprovement, false, ""
end

-------------------------------------------------
-- Map rendering
-------------------------------------------------
local function HexOf(idx)
	local plot = Map.GetPlotByIndex(idx)
	return ToHexFromGrid(Vector2(plot:GetX(), plot:GetY()))
end

local function SetHighlight(idx, style, bOn, color)
	Events.SerialEventHexHighlight(HexOf(idx), bOn, color, style)
end

local function ReleaseIcon(idx)
	local icon = g_activeIcons[idx]
	if icon then
		icon.inst.Anchor:SetHide(true)
		table.insert(g_iconPool, icon.inst)
		g_activeIcons[idx] = nil
	end
end

local function AcquireIcon()
	local inst = table.remove(g_iconPool)
	if inst then
		return inst
	end

	if g_iNumIcons >= MAX_MAP_ICONS then
		return nil
	end

	g_iNumIcons = g_iNumIcons + 1
	-- Pass the context explicitly, this may run from another context's event
	return MakeInstance("MapIconInstance", Controls.MapIcons, ContextPtr)
end

local function ClearPlot(idx)
	local shown = g_shownStyles[idx]
	if shown then
		for style in pairs(shown) do
			SetHighlight(idx, style, false)
		end
		g_shownStyles[idx] = nil
	end
	ReleaseIcon(idx)
end

local function RenderPlot(idx)
	-- Highlights, only send changes
	local desired = GetDesiredStyles(idx)
	local shown = g_shownStyles[idx] or {}
	for style in pairs(shown) do
		if not desired[style] then
			SetHighlight(idx, style, false)
		end
	end
	for style in pairs(desired) do
		if not shown[style] then
			SetHighlight(idx, style, true)
		end
	end
	g_shownStyles[idx] = next(desired) and desired or nil

	-- Icon
	local improvement, bPlanned, badge = GetDesiredIcon(idx)
	local key = improvement and (improvement .. ":" .. tostring(bPlanned) .. ":" .. badge)
	local icon = g_activeIcons[idx]
	if icon and icon.key == key then
		return
	end

	if not improvement then
		ReleaseIcon(idx)
		return
	end

	local inst = icon and icon.inst or AcquireIcon()
	if not inst then
		return -- over the cap
	end

	local plot = Map.GetPlotByIndex(idx)
	local wx, wy, wz = GridToWorld(plot:GetX(), plot:GetY())
	-- Top of the hex, yield icons are in the middle
	local wx1 = GridToWorld(plot:GetX() + 1, plot:GetY())
	wy = wy + 0.3 * math.abs(wx1 - wx)
	inst.Anchor:SetWorldPositionVal(wx, wy, wz)
	SetMapIcon(improvement, inst.Icon, 64)
	inst.Frame:SetHide(not bPlanned)
	inst.Icon:SetAlpha(bPlanned and 1 or 0.85)
	inst.BadgeBox:SetHide(badge == "")
	inst.Badge:SetText("[COLOR_YELLOW]" .. badge .. "[ENDCOLOR]")
	inst.Anchor:SetHide(false)
	g_activeIcons[idx] = { inst = inst, key = key }
end

local function RenderAll()
	for idx in pairs(g_inView) do
		RenderPlot(idx)
	end
end

local function ClearMap()
	for idx in pairs(g_shownStyles) do
		ClearPlot(idx)
	end
	for idx in pairs(g_activeIcons) do
		ReleaseIcon(idx)
	end
	for _, style in ipairs(ALL_STYLES) do
		Events.ClearHexHighlightStyle(style)
	end
	Events.ClearHexHighlightStyle(STYLE_SELECTED)
	g_shownStyles = {}
end

-- Hide all icons. They are kept in the pool rather than destroyed, destroying world anchors may be unsafe.
local function HideIcons()
	for idx in pairs(g_activeIcons) do
		ReleaseIcon(idx)
	end
end

-------------------------------------------------
-- Selected plot panel
-------------------------------------------------
local function GetPlotDescription(plot)
	local parts = {}
	local terrain = GameInfo.Terrains[plot:GetTerrainType()]
	if terrain then
		table.insert(parts, L(terrain.Description))
	end
	if plot:IsHills() then
		table.insert(parts, L("TXT_KEY_WP_HILLS"))
	elseif plot:IsMountain() then
		table.insert(parts, L("TXT_KEY_WP_MOUNTAIN"))
	end
	local feature = GameInfo.Features[plot:GetFeatureType()]
	if feature then
		table.insert(parts, L(feature.Description))
	end
	local resource = GameInfo.Resources[plot:GetResourceType(Game.GetActiveTeam())]
	if resource then
		table.insert(parts, resource.IconString .. " " .. L(resource.Description))
	end
	if plot:GetImprovementType() ~= NO_IMPROVEMENT then
		table.insert(parts, ImprovementName(plot:GetImprovementType()))
	end
	return table.concat(parts, ", ")
end

local function UpdatePriorityButtons()
	for i = 0, MAX_PRIORITY do
		local text = i == 0 and L("TXT_KEY_WP_PRIORITY_NONE") or tostring(i)
		if g_brush.priority == i then
			text = "[COLOR_POSITIVE_TEXT]" .. text .. "[ENDCOLOR]"
		end
		Controls["Priority" .. i]:SetText(text)
	end
	Controls.LockCheckBox:SetCheck(g_brush.lock)
end

local SendPlan -- forward declaration

-- Labels change height with their text
local function RelayoutPanel()
	Controls.ImprovementsStack:CalculateSize()
	Controls.ImprovementsStack:ReprocessAnchoring()
end

local function UpdateSelectedPanel()
	ResetOptions()

	local plot = g_selected and Map.GetPlotByIndex(g_selected)
	if not plot then
		Controls.SelectedTitle:SetText(L("TXT_KEY_WP_SELECT_TILE"))
		Controls.SelectedAI:SetText("")
		Controls.SelectedStatus:SetText("")
		LayoutSections({})
		UpdatePriorityButtons()
		RelayoutPanel()
		return
	end

	Controls.SelectedTitle:SetText(GetPlotDescription(plot))

	-- What will be built: the player's plan, or else the AI's choice
	local entry = g_overview[g_selected]
	local planText = L("TXT_KEY_WP_NO_PLAN")
	local target = NO_IMPROVEMENT
	if g_brush.lock and g_brush.improvement == NO_IMPROVEMENT then
		planText = L("TXT_KEY_WP_PLAN_LEAVE_AS_IS")
	elseif g_brush.improvement ~= NO_IMPROVEMENT then
		target = g_brush.improvement
		if g_brush.priority > 0 then
			planText = L("TXT_KEY_WP_PLAN_PRIORITY", ImprovementName(target), g_brush.priority)
		else
			planText = L("TXT_KEY_WP_PLAN", ImprovementName(target))
		end
	elseif entry and entry.AIImprovement ~= NO_IMPROVEMENT then
		target = entry.AIImprovement
		planText = L("TXT_KEY_WP_PLAN_AI", ImprovementName(target))
	end

	-- Something else built first?
	if entry and target ~= NO_IMPROVEMENT then
		local interim = entry.AIInterimImprovement ~= NO_IMPROVEMENT and entry.AIInterimImprovement or entry.AIImprovement
		if interim ~= NO_IMPROVEMENT and interim ~= target then
			planText = planText .. " " .. L("TXT_KEY_WP_AI_INTERIM", ImprovementName(interim))
		end
	end
	Controls.SelectedAI:SetText(planText)

	local statusText = entry and STATUS_TEXT[entry.Status] and L(STATUS_TEXT[entry.Status]) or L("TXT_KEY_WP_STATUS_NONE")
	Controls.SelectedStatus:SetText(L("TXT_KEY_WP_STATUS", statusText))

	-- Options, buildable and best first
	local options = g_pPlayer:GetImprovementOptions(plot:GetX(), plot:GetY())
	table.sort(options, function(a, b)
		if a.CanPlan ~= b.CanPlan then
			return a.CanPlan
		end
		if a.CanBuildNow ~= b.CanBuildNow then
			return a.CanBuildNow
		end
		return a.Score > b.Score
	end)

	local counts = {}
	for _, option in ipairs(options) do
		local improvement = option.Improvement
		local section = IsGreatPersonImprovement(improvement) and g_sections.gp or g_sections.normal
		counts[section] = (counts[section] or 0) + 1
		local inst = section.im:GetInstance()
		SetBuildIcon(improvement, inst.OptionIcon)
		inst.OptionName:SetText(ImprovementName(improvement))

		local info = ""
		local reason = ""
		if improvement == plot:GetImprovementType() then
			info = L("TXT_KEY_WP_OPTION_CURRENT")
		elseif not option.CanPlan or not option.CanBuildNow then
			-- First reason on the button, all of them in the tooltip
			reason = option.Reason or ""
			info = reason:match("^(.-)%[NEWLINE%]") or reason
			if info == "" then
				if option.TechPrereq ~= -1 and GameInfo.Technologies[option.TechPrereq] then
					info = L("TXT_KEY_WP_OPTION_REQUIRES", GameInfo.Technologies[option.TechPrereq].Description)
				else
					info = L("TXT_KEY_WP_OPTION_NOT_NOW")
				end
			end
		end
		if entry and entry.AIImprovement == improvement then
			info = info ~= "" and (info .. ", " .. L("TXT_KEY_WP_OPTION_AI")) or L("TXT_KEY_WP_OPTION_AI")
		end
		inst.OptionInfo:SetText(info)
		inst.OptionButton:SetToolTipString(reason)
		inst.OptionScore:SetText(tostring(option.Score))
		inst.OptionScore:SetToolTipString(L("TXT_KEY_WP_OPTION_SCORE_TT"))
		inst.SelectedFrame:SetHide(g_brush.improvement ~= improvement)
		inst.OptionButton:SetDisabled(not option.CanPlan)
		inst.OptionName:SetAlpha(option.CanPlan and 1 or 0.5)
		inst.OptionIcon:SetAlpha(option.CanPlan and 1 or 0.5)

		inst.OptionButton:RegisterCallback(Mouse.eLClick, function()
			g_brush.improvement = improvement
			g_brush.lock = Controls.LockCheckBox:IsChecked()
			SendPlan(g_selected)
			UpdateSelectedPanel()
		end)
	end

	LayoutSections(counts)
	UpdatePriorityButtons()
	RelayoutPanel()
end

for _, section in pairs(g_sections) do
	section.header:RegisterCallback(Mouse.eLClick, function()
		section.collapsed = not section.collapsed
		UpdateSelectedPanel()
	end)
end

-- Route plan options, in display order
local ROUTE_PLANS = {
	{ Controls.RoutePlanNoneButton, RoutePlan.NO_PLANNED_ROUTE, "TXT_KEY_WP_ROUTE_PLAN_NONE" },
	{ Controls.DrawRoadPlanButton, RoutePlan.ROAD_PLANNING_INCLUDE, "TXT_KEY_WP_DRAW_ROAD_PLAN" },
	{ Controls.DrawPriorityButton, RoutePlan.ROAD_PLANNING_PRIORITY_CONSTRUCTION, "TXT_KEY_WP_DRAW_PRIORITY_ROUTE" },
	{ Controls.DrawNoBuildZoneButton, RoutePlan.ROAD_PLANNING_EXCLUDE, "TXT_KEY_WP_DRAW_NO_BUILD_ZONE" },
}

local function UpdateRoutePanel()
	local plot = g_selected and Map.GetPlotByIndex(g_selected)

	for _, option in ipairs(ROUTE_PLANS) do
		local text = L(option[3])
		if plot and g_routeBrush == option[2] then
			text = "[COLOR_POSITIVE_TEXT]" .. text .. "[ENDCOLOR]"
		end
		option[1]:SetText(text)
		option[1]:SetDisabled(plot == nil)
	end

	if not plot then
		Controls.RouteSelectedTitle:SetText(L("TXT_KEY_WP_SELECT_TILE_ROUTE"))
		Controls.RouteSelectedInfo:SetText("")
	else
		Controls.RouteSelectedTitle:SetText(GetPlotDescription(plot))

		local lines = {}
		local route = GameInfo.Routes[plot:GetRouteType()]
		if route then
			local name = L(route.Description)
			if plot:IsRoutePillaged() then
				name = L("TXT_KEY_WP_ROUTE_PILLAGED", name)
			end
			table.insert(lines, name)
		else
			table.insert(lines, L("TXT_KEY_WP_ROUTE_NONE"))
		end

		if plot:IsMainRoutePlan(g_iPlayer) then
			table.insert(lines, L("TXT_KEY_WP_ROUTE_MAIN"))
		elseif plot:IsShortcutRoutePlan(g_iPlayer) then
			table.insert(lines, L("TXT_KEY_WP_ROUTE_SHORTCUT"))
		elseif plot:IsStrategicRoutePlan(g_iPlayer) then
			table.insert(lines, L("TXT_KEY_WP_ROUTE_STRATEGIC"))
		else
			table.insert(lines, L("TXT_KEY_WP_ROUTE_AI_NONE"))
		end

		if plot:IsTradeRoute(g_iPlayer) then
			table.insert(lines, L("TXT_KEY_WP_ROUTE_IS_CONNECTION"))
		end
		Controls.RouteSelectedInfo:SetText(table.concat(lines, "[NEWLINE]"))
	end

	Controls.RoutesStack:CalculateSize()
	Controls.RoutesStack:ReprocessAnchoring()
end

local function SelectPlot(idx)
	if g_selected and g_bSelectedShown then
		SetHighlight(g_selected, STYLE_SELECTED, false, SELECTED_COLOR)
	end

	g_selected = idx
	g_bSelectedShown = idx ~= nil
	if idx then
		SetHighlight(idx, STYLE_SELECTED, true, SELECTED_COLOR)
	end

	-- Load the plot's plan into the brush, keep the priority as a default for new plans
	if idx then
		local plot = Map.GetPlotByIndex(idx)
		local improvement, priority, lock = g_pPlayer:GetWorkerPlan(plot:GetX(), plot:GetY())
		g_routeBrush = plot:GetPlannedRouteState(g_iPlayer)
		if improvement ~= NO_IMPROVEMENT or lock then
			g_brush.improvement = improvement
			g_brush.priority = priority
			g_brush.lock = lock
			-- Show the planned great person improvement
			if IsGreatPersonImprovement(improvement) then
				g_sections.gp.collapsed = false
			end
		else
			g_brush.improvement = NO_IMPROVEMENT
			g_brush.lock = false
		end
	end

	UpdateSelectedPanel()
	UpdateRoutePanel()
end

-------------------------------------------------
-- Sending changes
-------------------------------------------------
SendPlan = function(idx)
	local plot = idx and Map.GetPlotByIndex(idx)
	if not plot then
		return
	end

	local priority = g_brush.improvement ~= NO_IMPROVEMENT and g_brush.priority or 0
	g_pPlayer:SendWorkerPlan(plot:GetX(), plot:GetY(), g_brush.improvement, priority, g_brush.lock)
	MarkDirty()
end

local function PaintPlot(plot)
	if not plot then
		return
	end

	local idx = plot:GetPlotIndex()
	if idx == g_iLastPainted then
		return
	end
	g_iLastPainted = idx

	if g_bImprovementsTab then
		if not g_selected then
			return
		end
		-- Plots where it can't be planned are skipped by the DLL
		SendPlan(idx)
	elseif g_selected then
		plot:SetPlannedRouteState(g_iPlayer, g_routeBrush)
		MarkDirty()
	end
end

-------------------------------------------------
-- Mouse handling (forwarded from WorldView.lua)
-------------------------------------------------
LuaEvents.WorkerPlannerLButtonUp.Add(function()
	if not g_bOpen then
		return
	end
	local plot = Map.GetPlot(UI.GetMouseOverHex())
	if plot then
		SelectPlot(plot:GetPlotIndex())
	end
end)

LuaEvents.WorkerPlannerRButtonDown.Add(function()
	if not g_bOpen then
		return
	end
	g_bPainting = true
	g_iLastPainted = nil
	PaintPlot(Map.GetPlot(UI.GetMouseOverHex()))
end)

LuaEvents.WorkerPlannerRButtonUp.Add(function()
	g_bPainting = false
	g_iLastPainted = nil
end)

local function OnMouseOverHex(hexX, hexY)
	if g_bOpen and g_bPainting then
		PaintPlot(Map.GetPlot(hexX, hexY))
	end
end
Events.SerialEventMouseOverHex.Add(OnMouseOverHex)

-------------------------------------------------
-- Viewport tracking (CameraView.lua)
-------------------------------------------------
LuaEvents.PlotEnterView.Add(function(idx)
	if g_bOpen then
		g_inView[idx] = true
		RenderPlot(idx)
	end
end)

LuaEvents.PlotLeaveView.Add(function(idx)
	if g_bOpen then
		g_inView[idx] = nil
		ClearPlot(idx)
	end
end)

LuaEvents.RespondViewPlots.Add(function(caller, plots)
	if g_bOpen and caller == CALLER then
		g_inView = {}
		for idx in pairs(plots) do
			g_inView[idx] = true
		end
		RenderAll()
	end
end)

-------------------------------------------------
-- Controls
-------------------------------------------------
local function SetTab(bImprovements)
	g_bImprovementsTab = bImprovements
	UpdateRoutePanel()
	Controls.ImprovementsTab:SetHide(not bImprovements)
	Controls.RoutesTab:SetHide(bImprovements)
	Controls.ClearAllPlansButton:SetHide(not bImprovements)
	Controls.ImprovementsTabButton:SetDisabled(bImprovements)
	Controls.RoutesTabButton:SetDisabled(not bImprovements)
	RenderAll()
end
Controls.ImprovementsTabButton:RegisterCallback(Mouse.eLClick, function() SetTab(true) end)
Controls.RoutesTabButton:RegisterCallback(Mouse.eLClick, function() SetTab(false) end)

for _, checkBox in ipairs({ Controls.ShowStatusCheckBox, Controls.ShowBuiltCheckBox, Controls.ShowIconsCheckBox, Controls.ShowCityConnectionsCheckBox, Controls.ShowRoadPlansCheckBox }) do
	checkBox:RegisterCheckHandler(RenderAll)
end
Controls.ExplainAutomatedRoutesCheckBox:RegisterCheckHandler(RenderAll)

for i = 0, MAX_PRIORITY do
	Controls["Priority" .. i]:RegisterCallback(Mouse.eLClick, function()
		g_brush.priority = i
		if g_brush.improvement ~= NO_IMPROVEMENT then
			SendPlan(g_selected)
		end
		UpdateSelectedPanel()
	end)
end

Controls.LockCheckBox:RegisterCheckHandler(function(bChecked)
	g_brush.lock = bChecked
	if g_brush.improvement ~= NO_IMPROVEMENT then
		SendPlan(g_selected)
	end
	UpdateSelectedPanel()
end)

Controls.ClearPlanButton:RegisterCallback(Mouse.eLClick, function()
	g_brush.improvement = NO_IMPROVEMENT
	g_brush.lock = false
	SendPlan(g_selected)
	UpdateSelectedPanel()
end)

Controls.LeaveAsIsButton:RegisterCallback(Mouse.eLClick, function()
	g_brush.improvement = NO_IMPROVEMENT
	g_brush.lock = true
	SendPlan(g_selected)
	UpdateSelectedPanel()
end)

Controls.ClearAllPlansButton:RegisterCallback(Mouse.eLClick, function()
	g_pPlayer:SendClearWorkerPlans()
	MarkDirty()
end)

-- Route plan for the selected tile, also used when painting
for _, option in ipairs(ROUTE_PLANS) do
	option[1]:RegisterCallback(Mouse.eLClick, function()
		g_routeBrush = option[2]
		local plot = g_selected and Map.GetPlotByIndex(g_selected)
		if plot then
			plot:SetPlannedRouteState(g_iPlayer, g_routeBrush)
			MarkDirty()
		end
		UpdateRoutePanel()
	end)
end

Controls.AllTilesNoBuildButton:RegisterCallback(Mouse.eLClick, function()
	g_pPlayer:SendAllPlannedRouteStates(RoutePlan.ROAD_PLANNING_EXCLUDE)
	MarkDirty()
end)

Controls.ResetAllTilesButton:RegisterCallback(Mouse.eLClick, function()
	g_pPlayer:SendAllPlannedRouteStates(RoutePlan.NO_PLANNED_ROUTE)
	MarkDirty()
end)

local function BuildLegend(legend, stacks)
	for i, item in ipairs(legend) do
		local stack = stacks[(i - 1) % #stacks + 1]
		local inst = {}
		ContextPtr:BuildInstanceForControl("LegendInstance", inst, stack)
		local color = item[3]
		inst.LegendColor:SetColor(Vector4(color[1] / 255, color[2] / 255, color[3] / 255, 1))
		inst.LegendLabel:SetText(L(item[2]))
	end
	for _, stack in ipairs(stacks) do
		stack:CalculateSize()
	end
end
-- Use extra screen height for the improvement list (designed for 768 px)
local _, iScreenY = UIManager:GetScreenSizeVal()
local iExtraHeight = math.max(0, math.min(iScreenY - 160, 1000) - 740)
if iExtraHeight > 0 then
	Controls.MainPanel:SetSizeY(740 + iExtraHeight)
	Controls.OptionsScrollPanel:SetSizeY(190 + iExtraHeight)
	Controls.OptionsScrollBar:SetSizeY(154 + iExtraHeight)
	Controls.MainPanel:ReprocessAnchoring()
end

BuildLegend(STATUS_LEGEND, { Controls.LegendStackLeft, Controls.LegendStackRight })
BuildLegend(ROUTE_LEGEND, { Controls.RouteLegendStackLeft, Controls.RouteLegendStackRight })
Controls.RouteLegendStack:CalculateSize()
Controls.LegendStack:CalculateSize()

-------------------------------------------------
-- Polling for changes while open
-------------------------------------------------
local function OnUpdate(fDTime)
	-- Re-plan so the map shows what workers will actually do
	if g_fRefreshDelay then
		g_fRefreshDelay = g_fRefreshDelay - fDTime
		if g_fRefreshDelay <= 0 then
			g_fRefreshDelay = nil
			g_bDirty = false
			g_pPlayer:SendRefreshWorkerPlans()
		end
	end

	g_fTimeSinceCheck = g_fTimeSinceCheck + fDTime
	if g_fTimeSinceCheck < POLL_INTERVAL then
		return
	end
	g_fTimeSinceCheck = 0

	if g_pPlayer:GetWorkerPlanRevision() ~= g_iRevision then
		RefreshData()
		RenderAll()
		UpdateSelectedPanel()
		UpdateRoutePanel()
	end
end

-- Improvements can finish at any time
Events.SerialEventImprovementCreated.Add(function()
	if g_bOpen then
		g_iRevision = -1
	end
end)

-------------------------------------------------
-- Open / close
-------------------------------------------------
local function Open()
	if g_bOpen then
		return
	end
	g_iPlayer = Game.GetActivePlayer()
	g_pPlayer = Players[g_iPlayer]
	g_bOpen = true
	g_bDirty = false
	g_selected = nil
	g_bSelectedShown = false
	g_routeBrush = RoutePlan.NO_PLANNED_ROUTE

	ContextPtr:SetHide(false)
	UI.SetInterfaceMode(INTERFACEMODE_WORKER_PLANNER)

	RefreshData()

	-- No directives yet (e.g. right after loading a game), ask the AI to plan
	if next(g_overview) == nil and g_pPlayer:GetNumCities() > 0 then
		g_pPlayer:SendRefreshWorkerPlans()
	end

	SetTab(g_bImprovementsTab)
	UpdateSelectedPanel()

	g_fTimeSinceCheck = 0
	ContextPtr:SetUpdate(OnUpdate)
	LuaEvents.RequestViewPlots(CALLER)
end

local function Close()
	if not g_bOpen or g_bClosing then
		return
	end
	g_bClosing = true

	ContextPtr:ClearUpdate()
	ClearMap()
	HideIcons()
	ResetOptions()
	g_inView = {}
	g_overview = {}
	g_selected = nil
	g_bSelectedShown = false
	g_bPainting = false

	-- Re-plan so changes take effect this turn
	if g_bDirty then
		g_pPlayer:SendRefreshWorkerPlans()
		g_bDirty = false
	end
	g_fRefreshDelay = nil

	g_bOpen = false
	ContextPtr:SetHide(true)
	if UI.GetInterfaceMode() == INTERFACEMODE_WORKER_PLANNER then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION)
	end

	g_bClosing = false
end

Controls.CloseButton:RegisterCallback(Mouse.eLClick, Close)

ContextPtr:SetInputHandler(function(uiMsg, wParam)
	if g_bOpen and uiMsg == KeyEvents.KeyDown and (wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN) then
		Close()
		return true
	end
	return false
end)

-- Leaving the planner interface mode (e.g. selecting a unit) closes the planner
Events.InterfaceModeChanged.Add(function(_, newMode)
	if g_bOpen and newMode ~= INTERFACEMODE_WORKER_PLANNER then
		Close()
	end
end)

Events.GameplaySetActivePlayer.Add(Close)

LuaEvents.AdditionalInformationDropdownGatherEntries.Add(function(entries)
	-- art is used by EUI, which shows these entries as buttons
	table.insert(entries, { text = L("TXT_KEY_WP_TITLE"), call = Open, art = "DC45_WorkerPlanner.dds" })
end)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries()

ContextPtr:SetHide(true)
