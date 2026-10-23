-------------------------------------------------
-- Worker Planner
-- Plan improvements and routes for automated workers, and see what the AI plans to build.
--
-- To keep the graphics load low, map highlights and icons are only shown for plots in view
-- (see CameraView.lua), icons are pooled and capped, and everything is released on close.
-------------------------------------------------
include("IconSupport")
include("FLuaVector")
include("CPK.lua")

local MakeInstance = CPK.UI.Control.Instance.Make
local InstanceManager = CPK.UI.Control.Instance.Manager
local Refresh = CPK.UI.Control.Refresh
local StringBuilder = CPK.Util.StringBuilder

-- Handlers are removed on shutdown, so events from other contexts can't call into this one after it is gone
local g_listeners = {}
local function Listen(event, fn)
	event.Add(fn)
	table.insert(g_listeners, { event, fn })
end

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
local STYLE_CHANGED = "WorkerPlanChanged"

local ALL_STYLES = {
	STYLE_BUILT, STYLE_IN_PROGRESS, STYLE_ASSIGNED, STYLE_PRIORITY, STYLE_HIGH, STYLE_LOW,
	STYLE_NOT_YET, STYLE_BLOCKED, STYLE_LOCKED, STYLE_CHANGED, STYLE_ROUTE_CONNECTION, STYLE_ROUTE_INCLUDE,
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
	{ STYLE_CHANGED, "TXT_KEY_WP_STATUS_CHANGED", { 255, 0, 255 } },
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
	[Status.WORKER_PLAN_STATUS_DISABLED] = "TXT_KEY_WP_STATUS_DISABLED",
}

-------------------------------------------------
-- State
-------------------------------------------------
local g_bOpen = false
local g_bClosing = false
local g_iPlayer = Game.GetActivePlayer()
local g_pPlayer = Players[g_iPlayer]
local g_tab = "improvements" -- "improvements", "routes" or "settings"
local g_bImprovementsTab = true -- improvements are shown on the map, also in the settings tab
local g_filterCity = nil -- { owner, id } of the selected tile's city
local g_changed = {} -- plot index -> true, the AI's choice changed since last turn
local g_lastTurnSnapshot = nil -- plot index -> { ai, status, plan } at the start of last turn
local g_pendingCheck = nil -- turn start check waiting for builders to plan

-- Work requested by other contexts' events, done in our own update.
-- Instances and callbacks created while another context's code runs belong to that context's Lua thread,
-- and crash the game when they're released after that context is gone (e.g. loading a game).
local g_bOpenRequested = false
local g_pendingSelect = nil -- plot index clicked in WorldView
local g_pendingRender = {} -- plot index -> true, entered or left the view
local g_bRenderAllPending = false

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
	normal = { im = InstanceManager.New({ instance = "OptionInstance", context = ContextPtr }), header = Controls.NormalHeader, stack = Controls.NormalOptionsStack, title = "TXT_KEY_WP_SECTION_NORMAL", collapsed = false },
	gp = { im = InstanceManager.New({ instance = "OptionInstance", context = ContextPtr }), header = Controls.GPHeader, stack = Controls.GPOptionsStack, title = "TXT_KEY_WP_SECTION_GP", collapsed = true },
}

local function ResetOptions()
	for _, section in pairs(g_sections) do
		section.im:RecycleAcquired()
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
		-- Recycled options may be in any order, sort by when they were acquired
		section.stack:SortChildren(function(a, b)
			return a:GetVoid1() < b:GetVoid1()
		end)
		Refresh(section.stack)
	end
	Refresh(Controls.OptionsStack, Controls.OptionsScrollPanel)
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

-- Yields as icons, e.g. "[ICON_FOOD]3 [ICON_PRODUCTION]1.5"
local function FormatYields(yields)
	local parts = {}
	for row in GameInfo.Yields() do
		local value = yields[row.ID]
		if value and value ~= 0 then
			local text = string.format("%.1f", value / 100):gsub("%.0$", "")
			table.insert(parts, row.IconString .. text)
		end
	end
	return table.concat(parts, " ")
end

-- What the option's score is made of, with how it's ranked against other tiles if detailed
local function GetScoreTooltip(option, bDetailed)
	local lines = StringBuilder.New():Append(L("TXT_KEY_WP_OPTION_SCORE_TT"))
	for _, part in ipairs(option.ScoreParts) do
		local line = L("TXT_KEY_WP_SCORE_" .. part.Key, (part.Value > 0 and "+" or "") .. part.Value)
		if part.Key == "YIELDS" then
			local yields = FormatYields(option.Yields)
			if yields ~= "" then
				line = line .. " (" .. yields .. ")"
			end
			if option.YieldsUnused then
				line = line .. "[NEWLINE]   " .. L("TXT_KEY_WP_SCORE_YIELDS_UNUSED")
			end
		end
		lines:Append("[ICON_BULLET]" .. line)
	end
	lines:Append(L("TXT_KEY_WP_SCORE_TOTAL", option.Score))

	-- Ranked differently against other tiles
	local function Signed(value)
		return (value > 0 and "+" or "") .. value
	end
	if bDetailed and option.RankScore ~= option.Score and (option.ScoreGreatPersonPenalty or 0) == 0 then
		lines:Append(L("TXT_KEY_WP_SCORE_RANK", option.RankScore))
		if option.ScorePlotLocal ~= 0 then
			lines:Append("[ICON_BULLET]" .. L("TXT_KEY_WP_SCORE_RANK_LOCAL", Signed(-option.ScorePlotLocal)))
		end
		if option.ScoreYieldGain ~= 0 then
			lines:Append("[ICON_BULLET]" .. L("TXT_KEY_WP_SCORE_RANK_GAIN", Signed(option.ScoreYieldGain)))
		end
	end
	if option.ScoreGreatPersonPenalty and option.ScoreGreatPersonPenalty > 0 then
		lines:Append(L("TXT_KEY_WP_SCORE_GREAT_PERSON", option.ScoreGreatPersonPenalty, option.Score - option.ScoreGreatPersonPenalty))
	end
	return lines:Concat("[NEWLINE]")
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

-- Only the selected tile's city, if wanted
local function IsInFilterCity(idx)
	if not Controls.OnlyCityCheckBox:IsChecked() or not g_filterCity then
		return true
	end
	local city = Map.GetPlotByIndex(idx):GetWorkingCity()
	return city ~= nil and city:GetOwner() == g_filterCity.owner and city:GetID() == g_filterCity.id
end

-- Map layers: your plans, AI choices, great person suggestions
local function IsEntryShown(idx, entry)
	if not IsInFilterCity(idx) then
		return false
	end
	if entry.HasPlan then
		return Controls.ShowPlansCheckBox:IsChecked()
	end
	if not Controls.ShowAIChoicesCheckBox:IsChecked() then
		return false
	end
	return Controls.ShowGreatPersonCheckBox:IsChecked() or not IsGreatPersonImprovement(entry.AIImprovement)
end

-- Highlight styles wanted for a plot in the current tab
local function GetDesiredStyles(idx)
	local styles = {}

	if g_bImprovementsTab then
		if g_changed[idx] and Controls.ShowChangesCheckBox:IsChecked() and IsInFilterCity(idx) then
			styles[STYLE_CHANGED] = true
		end

		local entry = g_overview[idx]
		if not entry or not Controls.ShowStatusCheckBox:IsChecked() or not IsEntryShown(idx, entry) then
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

-- Icon wanted for a plot: improvement, is player plan, badge text, interim improvement, score text
local function GetDesiredIcon(idx)
	if not g_bImprovementsTab or not Controls.ShowIconsCheckBox:IsChecked() then
		return nil
	end

	local entry = g_overview[idx]
	if not entry or not IsEntryShown(idx, entry) then
		return nil
	end

	local improvement = entry.HasPlan and entry.PlanImprovement or entry.AIImprovement
	if improvement == NO_IMPROVEMENT then
		return nil
	end

	-- Don't clutter the map with what's already there
	if not entry.HasPlan and entry.Status == Status.WORKER_PLAN_STATUS_BUILT then
		return nil
	end

	local interim = NO_IMPROVEMENT
	if Controls.ShowInterimCheckBox:IsChecked() and entry.AIInterimImprovement ~= improvement then
		interim = entry.AIInterimImprovement
	end

	-- The AI's ranked value, only if it's for what's shown
	local score = ""
	if Controls.ShowScoresCheckBox:IsChecked() and entry.AIImprovement == improvement and entry.Status ~= Status.WORKER_PLAN_STATUS_BUILT then
		score = tostring(entry.AIScore)
	end

	local badge = entry.HasPlan and entry.PlanPriority > 0 and tostring(entry.PlanPriority) or ""
	return improvement, entry.HasPlan, badge, interim, score
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

-- Hides the plot's icon and returns it to the pool
local function RecycleIcon(idx)
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
	RecycleIcon(idx)
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
	local improvement, bPlanned, badge, interim, score = GetDesiredIcon(idx)
	local key = improvement and table.concat({ improvement, tostring(bPlanned), badge, interim, score }, ":")
	local icon = g_activeIcons[idx]
	if icon and icon.key == key then
		return
	end

	if not improvement then
		RecycleIcon(idx)
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
	if interim ~= NO_IMPROVEMENT then
		-- Build icons have no smaller size
		SetMapIcon(interim, inst.InterimIcon, 45)
	else
		inst.InterimIcon:SetHide(true)
	end
	inst.ScoreLabel:SetText(score)
	inst.ScoreLabel:SetHide(score == "")
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
		RecycleIcon(idx)
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
		RecycleIcon(idx)
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

local SendPlan -- forward declarations
local UpdateSettingsPanel

-- Labels change height with their text
local function RelayoutPanel()
	Refresh(Controls.ImprovementsStack)
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
		local inst = section.im:Acquire(section.stack)
		inst.OptionButton:SetVoid1(counts[section])
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
		inst.OptionScore:SetToolTipString(GetScoreTooltip(option, true))
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

-- Route, AI route plan and city connection of a plot, also for the map tooltip
local function AppendRouteLines(lines, plot)
	local route = GameInfo.Routes[plot:GetRouteType()]
	if route then
		local name = L(route.Description)
		if plot:IsRoutePillaged() then
			name = L("TXT_KEY_WP_ROUTE_PILLAGED", name)
		end
		lines:Append(name)
	else
		lines:Append(L("TXT_KEY_WP_ROUTE_NONE"))
	end

	if plot:IsMainRoutePlan(g_iPlayer) then
		lines:Append(L("TXT_KEY_WP_ROUTE_MAIN"))
	elseif plot:IsShortcutRoutePlan(g_iPlayer) then
		lines:Append(L("TXT_KEY_WP_ROUTE_SHORTCUT"))
	elseif plot:IsStrategicRoutePlan(g_iPlayer) then
		lines:Append(L("TXT_KEY_WP_ROUTE_STRATEGIC"))
	else
		lines:Append(L("TXT_KEY_WP_ROUTE_AI_NONE"))
	end

	if plot:IsTradeRoute(g_iPlayer) then
		lines:Append(L("TXT_KEY_WP_ROUTE_IS_CONNECTION"))
	end
	return lines
end

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
		Controls.RouteSelectedInfo:SetText(AppendRouteLines(StringBuilder.New(), plot):Concat("[NEWLINE]"))
	end

	Refresh(Controls.RoutesStack)
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

	-- City for the filter and clearing city plans
	local plot = idx and Map.GetPlotByIndex(idx)
	local city = plot and plot:GetWorkingCity()
	local oldCity = g_filterCity
	g_filterCity = city and city:GetOwner() == g_iPlayer and { owner = city:GetOwner(), id = city:GetID(), name = city:GetName() } or nil
	if Controls.OnlyCityCheckBox:IsChecked() and (oldCity and oldCity.id) ~= (g_filterCity and g_filterCity.id) then
		RenderAll()
	end

	UpdateSelectedPanel()
	UpdateRoutePanel()
	UpdateSettingsPanel()
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

	if g_tab == "settings" then
		return
	elseif g_bImprovementsTab then
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
-- Replaces the game's plot tooltip (PlotHelpManager.lua) for tiles with something to show
-------------------------------------------------
local function GetMapTooltip(idx)
	local entry = g_overview[idx]
	local plot = Map.GetPlotByIndex(idx)
	local improvement = entry.HasPlan and entry.PlanImprovement or entry.AIImprovement
	local lines = StringBuilder.New():Append(GetPlotDescription(plot))

	local text
	if entry.HasPlan and entry.PlanPriority > 0 then
		text = L("TXT_KEY_WP_PLAN_PRIORITY", ImprovementName(improvement), entry.PlanPriority)
	elseif entry.HasPlan then
		text = L("TXT_KEY_WP_PLAN", ImprovementName(improvement))
	else
		text = L("TXT_KEY_WP_PLAN_AI", ImprovementName(improvement))
	end
	local interim = entry.AIInterimImprovement
	if interim ~= NO_IMPROVEMENT and interim ~= improvement then
		text = text .. " " .. L("TXT_KEY_WP_AI_INTERIM", ImprovementName(interim))
	end
	lines:Append(text)
	lines:Append(L("TXT_KEY_WP_STATUS", STATUS_TEXT[entry.Status] and L(STATUS_TEXT[entry.Status]) or L("TXT_KEY_WP_STATUS_NONE")))

	for _, option in ipairs(g_pPlayer:GetImprovementOptions(plot:GetX(), plot:GetY())) do
		if option.Improvement == improvement then
			lines:Append("")
			lines:Append(GetScoreTooltip(option))
			break
		end
	end
	return lines:Concat("[NEWLINE]")
end

-- Routes tab: every revealed tile, so the tooltip is always the same
local function GetRouteTooltip(plot)
	if not plot:IsRevealed(Game.GetActiveTeam(), false) then
		return nil
	end
	local plan = plot:GetPlannedRouteState(g_iPlayer)
	local lines = StringBuilder.New():Append(GetPlotDescription(plot))
	for _, option in ipairs(ROUTE_PLANS) do
		if option[2] == plan then
			lines:Append(L("TXT_KEY_WP_ROUTE_YOUR_PLAN", L(option[3])))
		end
	end
	return AppendRouteLines(lines, plot):Concat("[NEWLINE]")
end

Listen(LuaEvents.PlotHelpOverride, function(plot, override)
	if not g_bOpen or override.text then
		return
	end
	if not g_bImprovementsTab then
		override.text = GetRouteTooltip(plot)
		return
	end
	local idx = plot:GetPlotIndex()
	local entry = g_overview[idx]
	if entry and IsEntryShown(idx, entry) and (entry.HasPlan and entry.PlanImprovement or entry.AIImprovement) ~= NO_IMPROVEMENT then
		override.text = GetMapTooltip(idx)
	end
end)

-------------------------------------------------
-- Mouse handling (forwarded from WorldView.lua)
-------------------------------------------------
Listen(LuaEvents.WorkerPlannerLButtonUp, function()
	if not g_bOpen then
		return
	end
	local plot = Map.GetPlot(UI.GetMouseOverHex())
	if plot then
		g_pendingSelect = plot:GetPlotIndex()
	end
end)

Listen(LuaEvents.WorkerPlannerRButtonDown, function()
	if not g_bOpen then
		return
	end
	g_bPainting = true
	g_iLastPainted = nil
	PaintPlot(Map.GetPlot(UI.GetMouseOverHex()))
end)

Listen(LuaEvents.WorkerPlannerRButtonUp, function()
	g_bPainting = false
	g_iLastPainted = nil
end)

local function OnMouseOverHex(hexX, hexY)
	if g_bOpen and g_bPainting then
		PaintPlot(Map.GetPlot(hexX, hexY))
	end
end
Listen(Events.SerialEventMouseOverHex, OnMouseOverHex)

-------------------------------------------------
-- Viewport tracking (CameraView.lua)
-------------------------------------------------
Listen(LuaEvents.PlotEnterView, function(idx)
	if g_bOpen then
		g_inView[idx] = true
		g_pendingRender[idx] = true
	end
end)

Listen(LuaEvents.PlotLeaveView, function(idx)
	if g_bOpen then
		g_inView[idx] = nil
		g_pendingRender[idx] = true
	end
end)

Listen(LuaEvents.RespondViewPlots, function(caller, plots)
	if g_bOpen and caller == CALLER then
		g_inView = {}
		for idx in pairs(plots) do
			g_inView[idx] = true
		end
		g_bRenderAllPending = true
	end
end)

-- Requests from other contexts' events
local function DoPendingWork()
	if g_bRenderAllPending then
		g_bRenderAllPending = false
		g_pendingRender = {}
		RenderAll()
	elseif next(g_pendingRender) then
		local pending = g_pendingRender
		g_pendingRender = {}
		for idx in pairs(pending) do
			if g_inView[idx] then
				RenderPlot(idx)
			else
				ClearPlot(idx)
			end
		end
	end

	if g_pendingSelect then
		local idx = g_pendingSelect
		g_pendingSelect = nil
		SelectPlot(idx)
	end
end

-------------------------------------------------
-- Controls
-------------------------------------------------
local function SetTab(tab)
	g_tab = tab
	g_bImprovementsTab = tab ~= "routes"
	UpdateRoutePanel()
	UpdateSettingsPanel()
	Controls.ImprovementsTab:SetHide(tab ~= "improvements")
	Controls.RoutesTab:SetHide(tab ~= "routes")
	Controls.SettingsTab:SetHide(tab ~= "settings")
	Controls.ClearAllPlansButton:SetHide(tab ~= "improvements")
	Controls.ClearCityPlansButton:SetHide(tab ~= "improvements")
	Controls.ImprovementsTabButton:SetDisabled(tab == "improvements")
	Controls.RoutesTabButton:SetDisabled(tab == "routes")
	Controls.SettingsTabButton:SetDisabled(tab == "settings")
	RenderAll()
end
Controls.ImprovementsTabButton:RegisterCallback(Mouse.eLClick, function() SetTab("improvements") end)
Controls.RoutesTabButton:RegisterCallback(Mouse.eLClick, function() SetTab("routes") end)
Controls.SettingsTabButton:RegisterCallback(Mouse.eLClick, function() SetTab("settings") end)

-- Display settings, only for this computer and kept between games
local g_userData = Modding.OpenUserData("Worker Planner", 1)
local LOCAL_SETTINGS = {
	{ Controls.ShowStatusCheckBox, "ShowStatus", true },
	{ Controls.ShowBuiltCheckBox, "ShowBuilt", false },
	{ Controls.ShowIconsCheckBox, "ShowIcons", true },
	{ Controls.ShowRoadPlansCheckBox, "ShowRoadPlans", true },
	{ Controls.ExplainAutomatedRoutesCheckBox, "ShowAIRoutes", true },
	{ Controls.ShowCityConnectionsCheckBox, "ShowCityConnections", false },
	{ Controls.ShowAIChoicesCheckBox, "ShowAIChoices", true },
	{ Controls.ShowPlansCheckBox, "ShowPlans", true },
	{ Controls.ShowGreatPersonCheckBox, "ShowGreatPerson", true },
	{ Controls.ShowInterimCheckBox, "ShowInterim", false },
	{ Controls.ShowScoresCheckBox, "ShowScores", false },
	{ Controls.ShowChangesCheckBox, "ShowChanges", true },
	{ Controls.OnlyCityCheckBox, "OnlyCity", false },
	{ Controls.NotifyUnlockedCheckBox, "NotifyUnlocked", true },
	{ Controls.NotifyBlockedCheckBox, "NotifyBlocked", true },
}
-- Settings that only matter when another one is on, greyed out otherwise
local DEPENDENT_SETTINGS = {
	{ Controls.ShowBuiltCheckBox, Controls.ShowStatusCheckBox },
	{ Controls.ShowInterimCheckBox, Controls.ShowIconsCheckBox },
	{ Controls.ShowScoresCheckBox, Controls.ShowIconsCheckBox },
	{ Controls.ShowGreatPersonCheckBox, Controls.ShowAIChoicesCheckBox },
}
local function UpdateDependentSettings()
	for _, setting in ipairs(DEPENDENT_SETTINGS) do
		local bEnabled = setting[2]:IsChecked()
		setting[1]:SetDisabled(not bEnabled)
		setting[1]:SetAlpha(bEnabled and 1 or 0.5)
	end
end

for _, setting in ipairs(LOCAL_SETTINGS) do
	local value = g_userData.GetValue(setting[2])
	setting[1]:SetCheck(value == nil and setting[3] or value == 1)
	setting[1]:RegisterCheckHandler(function(bChecked)
		g_userData.SetValue(setting[2], bChecked and 1 or 0)
		UpdateDependentSettings()
		RenderAll()
	end)
end
UpdateDependentSettings()

-- Builder settings, saved with the game
local Setting = WorkerPlanSettingTypes
local BUILDER_SETTINGS = {
	{ Controls.PlansOnlyCheckBox, Setting.WORKER_PLAN_SETTING_PLANS_ONLY },
	{ Controls.CommitGreatPersonsCheckBox, Setting.WORKER_PLAN_SETTING_COMMIT_GREAT_PERSONS },
	{ Controls.LuxuriesFirstCheckBox, Setting.WORKER_PLAN_SETTING_LUXURIES_FIRST },
	{ Controls.StrategicsFirstCheckBox, Setting.WORKER_PLAN_SETTING_STRATEGICS_FIRST },
	-- Also game options, kept in the user profile too
	{ Controls.SafeAutomationCheckBox, Setting.WORKER_PLAN_SETTING_SAFE_AUTOMATION, OptionsManager.SetAutoWorkersDontReplace_Cached },
	{ Controls.LeaveForestsCheckBox, Setting.WORKER_PLAN_SETTING_LEAVE_FORESTS, OptionsManager.SetAutoWorkersDontRemoveFeatures_Cached },
}
for _, setting in ipairs(BUILDER_SETTINGS) do
	setting[1]:RegisterCheckHandler(function(bChecked)
		g_pPlayer:SendWorkerPlanSetting(setting[2], bChecked)
		-- The options menu and loading a game use the profile value
		if setting[3] then
			OptionsManager.SyncGameOptionsCache()
			setting[3](bChecked)
			OptionsManager.CommitGameOptions()
		end
		MarkDirty()
	end)
end

-- Overview entries in the selected tile's city
local function GetCityEntries()
	local entries = {}
	if not g_filterCity then
		return entries
	end
	for idx, entry in pairs(g_overview) do
		local city = Map.GetPlotByIndex(idx):GetWorkingCity()
		if city and city:GetOwner() == g_filterCity.owner and city:GetID() == g_filterCity.id then
			entries[idx] = entry
		end
	end
	return entries
end

UpdateSettingsPanel = function()
	for _, setting in ipairs(BUILDER_SETTINGS) do
		setting[1]:SetCheck(g_pPlayer:IsWorkerPlanSetting(setting[2]))
	end

	Controls.ClearCityPlansButton:SetDisabled(g_filterCity == nil)
	Controls.ClearCityPlansButton:SetToolTipString(g_filterCity and L("TXT_KEY_WP_CLEAR_CITY_PLANS_TT", g_filterCity.name) or L("TXT_KEY_WP_CLEAR_CITY_PLANS_NONE_TT"))

	Refresh(Controls.SettingsStack)
end

Controls.ClearCityPlansButton:RegisterCallback(Mouse.eLClick, function()
	for idx, entry in pairs(GetCityEntries()) do
		if entry.HasPlan then
			local plot = Map.GetPlotByIndex(idx)
			g_pPlayer:SendWorkerPlan(plot:GetX(), plot:GetY(), NO_IMPROVEMENT, 0, false)
		end
	end
	MarkDirty()
end)

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
		local inst = MakeInstance("LegendInstance", stack, ContextPtr)
		local color = item[3]
		inst.LegendColor:SetColor(Vector4(color[1] / 255, color[2] / 255, color[3] / 255, 1))
		inst.LegendLabel:SetText(L(item[2]))
	end
	Refresh(unpack(stacks))
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
Refresh(Controls.RouteLegendStack, Controls.LegendStack)

-------------------------------------------------
-- Polling for changes while open
-------------------------------------------------
local OnUpdate

-- What changed since last turn: the AI's choices, and plans that can now or no longer be built
local WAITING = {
	[Status.WORKER_PLAN_STATUS_NOT_YET] = true,
	[Status.WORKER_PLAN_STATUS_BLOCKED] = true,
	[Status.WORKER_PLAN_STATUS_OUTSIDE_BORDERS] = true,
}
local BUILDABLE = {
	[Status.WORKER_PLAN_STATUS_QUEUED] = true,
	[Status.WORKER_PLAN_STATUS_ASSIGNED] = true,
	[Status.WORKER_PLAN_STATUS_IN_PROGRESS] = true,
	[Status.WORKER_PLAN_STATUS_SUGGESTED] = true,
	[Status.WORKER_PLAN_STATUS_DISABLED] = true,
}

local function Notify(plots, key, checkBox)
	if #plots == 0 or not checkBox:IsChecked() then
		return
	end
	local names = {}
	for i = 1, math.min(3, #plots) do
		table.insert(names, ImprovementName(g_lastTurnSnapshot[plots[i]].plan))
	end
	if #plots > 3 then
		table.insert(names, "...")
	end
	Events.GameplayAlertMessage(L(key, table.concat(names, ", ")))
end

local function CheckTurnChanges()
	local snapshot = {}
	for _, entry in ipairs(g_pPlayer:GetWorkerPlanOverview()) do
		snapshot[entry.PlotIndex] = { ai = entry.AIImprovement, status = entry.Status, plan = entry.HasPlan and entry.PlanImprovement or nil }
	end

	local previous = g_lastTurnSnapshot
	g_lastTurnSnapshot = snapshot
	g_changed = {}
	if not previous then
		return
	end

	local unlocked, blocked = {}, {}
	for idx, current in pairs(snapshot) do
		local old = previous[idx]
		if old and not current.plan and not old.plan and old.ai ~= current.ai then
			g_changed[idx] = true
		end
		if old and current.plan and current.plan ~= NO_IMPROVEMENT and old.plan == current.plan then
			if WAITING[old.status] and BUILDABLE[current.status] then
				table.insert(unlocked, idx)
			elseif not WAITING[old.status] and current.status == Status.WORKER_PLAN_STATUS_BLOCKED then
				table.insert(blocked, idx)
			end
		end
	end
	for idx, old in pairs(previous) do
		if not snapshot[idx] and not old.plan and old.ai ~= NO_IMPROVEMENT then
			g_changed[idx] = true
		end
	end

	Notify(unlocked, "TXT_KEY_WP_NOTIFY_UNLOCKED", Controls.NotifyUnlockedCheckBox)
	Notify(blocked, "TXT_KEY_WP_NOTIFY_BLOCKED", Controls.NotifyBlockedCheckBox)
end

-- Builders plan at the start of the turn, wait for it before comparing
local TURN_CHECK_WAIT = 2
local function UpdatePendingCheck(fDTime)
	if not g_pendingCheck then
		return
	end
	g_pendingCheck.wait = g_pendingCheck.wait - fDTime
	if g_pPlayer:GetWorkerPlanRevision() ~= g_pendingCheck.revision or g_pendingCheck.wait <= 0 then
		g_pendingCheck = nil
		CheckTurnChanges()
		if g_bOpen then
			RenderAll()
		end
	end
end

Listen(Events.ActivePlayerTurnStart, function()
	if Game.GetActivePlayer() ~= g_iPlayer then
		g_iPlayer = Game.GetActivePlayer()
		g_pPlayer = Players[g_iPlayer]
		g_lastTurnSnapshot = nil
	end
	g_pendingCheck = { revision = g_pPlayer:GetWorkerPlanRevision(), wait = TURN_CHECK_WAIT }
end)

local DoOpen -- forward declaration

OnUpdate = function(fDTime)
	UpdatePendingCheck(fDTime)
	if g_bOpenRequested then
		g_bOpenRequested = false
		DoOpen()
	end
	if not g_bOpen then
		return
	end
	DoPendingWork()

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
		UpdateSettingsPanel()
	end
end

-- Improvements can finish at any time
Listen(Events.SerialEventImprovementCreated, function()
	if g_bOpen then
		g_iRevision = -1
	end
end)

-------------------------------------------------
-- Open / close
-------------------------------------------------
-- Called from other contexts (e.g. the additional information dropdown), the actual opening waits for our update.
-- The context must be shown for it to update.
local function Open()
	if g_bOpen then
		return
	end
	g_bOpenRequested = true
	ContextPtr:SetHide(false)
end

DoOpen = function()
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

	g_filterCity = nil
	SetTab(g_tab)
	UpdateSelectedPanel()

	g_fTimeSinceCheck = 0
	LuaEvents.RequestViewPlots(CALLER)
end

local function Close()
	if not g_bOpen or g_bClosing then
		return
	end
	g_bClosing = true

	ClearMap()
	HideIcons()
	ResetOptions()
	g_inView = {}
	g_overview = {}
	g_selected = nil
	g_bSelectedShown = false
	g_bPainting = false
	g_pendingSelect = nil
	g_pendingRender = {}
	g_bRenderAllPending = false

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
Listen(Events.InterfaceModeChanged, function(_, newMode)
	if g_bOpen and newMode ~= INTERFACEMODE_WORKER_PLANNER then
		Close()
	end
end)

Listen(Events.GameplaySetActivePlayer, Close)

Listen(LuaEvents.AdditionalInformationDropdownGatherEntries, function(entries)
	-- art is used by EUI, which shows these entries as buttons
	table.insert(entries, { text = L("TXT_KEY_WP_TITLE"), call = Open, art = "DC45_WorkerPlanner.dds" })
end)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries()

ContextPtr:SetHide(true)
ContextPtr:SetUpdate(OnUpdate)

ContextPtr:SetShutdown(function()
	ContextPtr:ClearUpdate()
	for _, listener in ipairs(g_listeners) do
		listener[1].Remove(listener[2])
	end
	g_listeners = {}
end)
