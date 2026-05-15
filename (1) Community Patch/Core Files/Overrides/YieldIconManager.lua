include('CPK.lua')

local lua_next = next
local lua_math_max = math.max
local lua_setmetatable = setmetatable

local CivEvents = Events
local LuaEvents = LuaEvents

local civ_db_query = DB.Query
local civ_grid_to_world = GridToWorld
local civ_to_grid_from_hex = ToGridFromHex
local civ_game_get_active_team = Game.GetActiveTeam
local civ_map_get_plot_by_index = Map.GetPlotByIndex

local Stack = CPK.Util.Stack
local Always = CPK.FP.Always

local MakeInstance = CPK.UI.Control.Instance.Make
local MoveInstance = CPK.UI.Control.Instance.Move

local C = Controls
local CP = ContextPtr

--------------------------------------------------------------------------------

local yield_max_id = 0

local yield_img_file = {} --[[@type table<integer, string>]]
local yield_img_offx = {} --[[@type table<integer, integer>]]
local yield_img_offy = {} --[[@type table<integer, integer>]]

local yield_lbl_offx = {} --[[@type table<integer, integer>]]
local yield_lbl_offy = {} --[[@type table<integer, integer>]]

for row in civ_db_query('SELECT ID, ImageTexture, ImageOffset FROM Yields') do
	yield_img_file[row.ID] = row.ImageTexture
	yield_img_offx[row.ID] = row.ImageOffset

	yield_max_id = lua_math_max(yield_max_id, row.ID)
end

for count = 0, 32 do
	yield_img_offy[count] = count > 4
			and 512
			or (128 * lua_math_max(0, count - 1))

	yield_lbl_offy[count] = count > 9
			and 768
			or 640

	yield_lbl_offx[count] = count > 12
			and 256
			or (128 * ((count - 6) % 4))
end

lua_setmetatable(yield_img_file, { __index = Always('YieldAtlas.dds') })
lua_setmetatable(yield_img_offx, { __index = Always(0) })
lua_setmetatable(yield_img_offy, { __index = Always(512) })
lua_setmetatable(yield_lbl_offx, { __index = Always(256) })
lua_setmetatable(yield_lbl_offy, { __index = Always(768) })

--------------------------------------------------------------------------------

local WORLD_GW = Map.GetGridSize()
local CALLER = CP:GetID()

local active_icons = {} --[[@type table<integer, table<integer, ControlInstance>>]]
local active_labels = {} --[[@type table<integer, ControlInstance[]>]]
local active_stacks = {} --[[@type table<integer, ControlInstance[]>]]
local active_anchors = {} --[[@type table<integer, ControlInstance>]]

local hidden_icons = {} --[[@type table<integer, Stack<ControlInstance>>]]
local hidden_labels = Stack.New() --[[@type Stack<ControlInstance>]]
local hidden_stacks = Stack.New() --[[@type Stack<ControlInstance>]]
local hidden_anchors = Stack.New() --[[@type Stack<ControlInstance>]]

for _, texture in lua_next, yield_img_file do
	hidden_icons[texture] = Stack.New()
end

--------------------------------------------------------------------------------

--- @param idx integer
local RecycleHex = function(idx)
	if not active_anchors[idx] then return end

	local labels = active_labels[idx]
	if labels then
		active_labels[idx] = nil
		for i = 1, #labels do
			labels[i].Image:SetHide(true)
			hidden_labels:Push(labels[i])
		end
	end

	local icons = active_icons[idx]
	if icons then
		active_icons[idx] = nil
		for yid, ico in lua_next, icons do
			ico.Image:SetHide(true)
			hidden_icons[yield_img_file[yid]]:Push(ico)
		end
	end

	local stacks = active_stacks[idx]
	if stacks then
		active_stacks[idx] = nil
		for i = 1, #stacks do
			stacks[i].Stack:SetHide(true)
			hidden_stacks:Push(stacks[i])
		end
	end

	local anc = active_anchors[idx]
	active_anchors[idx] = nil
	anc.Anchor:SetHide(true)
	hidden_anchors:Push(anc)
end

local RecycleAll = function()
	local indices = {}
	for idx in lua_next, active_anchors do
		indices[#indices + 1] = idx
	end
	for i = 1, #indices do
		RecycleHex(indices[i])
	end
end

--------------------------------------------------------------------------------

--- @param parent Control
--- @param idx integer
--- @param cnt integer
--- @return ControlInstance
local AcquireHexLabel = function(parent, idx, cnt)
	local lbl = hidden_labels:Pop()

	if lbl then
		MoveInstance(lbl, parent)
	else
		lbl = MakeInstance('ImageInstance', parent, CP)
		lbl.Image:SetTexture('YieldAtlas.dds')
	end

	local labels = active_labels[idx]

	if not labels then
		labels = {}
		active_labels[idx] = labels
	end

	labels[#labels + 1] = lbl

	lbl.Image:SetTextureOffsetVal(yield_lbl_offx[cnt], yield_lbl_offy[cnt])
	lbl.Image:SetHide(false)

	return lbl
end

--- @param parent Control
--- @param idx integer
--- @param yid integer
--- @param cnt integer
--- @return ControlInstance
local AcquireHexIcon = function(parent, idx, yid, cnt)
	local tex = yield_img_file[yid]

	local ico = hidden_icons[tex]:Pop()

	if ico then
		MoveInstance(ico, parent)
	else
		ico = MakeInstance('ImageInstance', parent, CP)
		ico.Image:SetTexture(tex)
	end

	local icons = active_icons[idx]

	if not icons then
		icons = {}
		active_icons[idx] = icons
	end

	icons[yid] = ico

	ico.Image:SetTextureOffsetVal(yield_img_offx[yid], yield_img_offy[cnt])
	ico.Image:SetHide(false)

	return ico
end

--- @param parent Control
---	@param idx integer
local AcquireHexStack = function(parent, idx)
	local stk = hidden_stacks:Pop()

	if stk then
		MoveInstance(stk, parent)
	else
		stk = MakeInstance('StackInstance', parent, CP)
	end

	local stacks = active_stacks[idx]

	if not stacks then
		stacks = {}
		active_stacks[idx] = stacks
	end

	stacks[#stacks + 1] = stk

	stk.Stack:SetHide(false)

	return stk
end

--- @param parent Control
--- @param idx integer
--- @param wx number
--- @param wy number
--- @param wz number
--- @return ControlInstance
local AcquireHexAnchor = function(parent, idx, wx, wy, wz)
	local anc = hidden_anchors:Pop() or MakeInstance('AnchorInstance', parent, CP)

	anc.Anchor:SetWorldPositionVal(wx, wy, wz)
	anc.Anchor:SetHide(false)

	active_anchors[idx] = anc

	return anc
end

--------------------------------------------------------------------------------

local RenderAnchor = function(idx)
	RecycleHex(idx)

	local plot = civ_map_get_plot_by_index(idx)

	if not plot or not plot:IsRevealed(civ_game_get_active_team(), false) then
		return
	end

	local anc = nil --[[@type nil | ControlInstance]]
	local stk = nil --[[@type nil | ControlInstance]]
	local total = 0
	local stacks = {}

	for yid = 0, yield_max_id do
		local cnt = plot:CalculateYield(yid, true)

		if cnt > 0 then
			if not anc then
				local wx, wy, wz = civ_grid_to_world(plot:GetX(), plot:GetY())
				anc = AcquireHexAnchor(C.Anchors, idx, wx, wy, wz)
			end

			total = total + 1

			if (total - 1) % 4 == 0 then
				stk = AcquireHexStack(anc.Stacks, idx)
				stacks[#stacks + 1] = stk
			end

			local ico = AcquireHexIcon(stk.Stack, idx, yid, cnt)

			if cnt > 5 then
				AcquireHexLabel(ico.Image, idx, cnt)
			end
		end
	end

	if anc then
		for i = #stacks, 1, -1 do
			local stack = stacks[i].Stack
			stack:CalculateSize()
			stack:ReprocessAnchoring()
		end

		anc.Stacks:CalculateSize()
		anc.Stacks:ReprocessAnchoring()
	end
end

--------------------------------------------------------------------------------

local updates_enabled = true

local RequestViewPlot = LuaEvents.RequestViewPlot
local RequestViewPlots = LuaEvents.RequestViewPlots
local RespondViewPlots = LuaEvents.RespondViewPlots

local RequestRender = function()
	if updates_enabled then RequestViewPlots(CALLER) end
end

RespondViewPlots.Add(function(caller, plots)
	if caller == CALLER then
		for idx in lua_next, plots do
			RenderAnchor(idx)
		end
	end
end)

LuaEvents.PlotEnterView.Add(function(idx)
	if updates_enabled then RenderAnchor(idx) end
end)

LuaEvents.PlotLeaveView.Add(function(idx)
	if updates_enabled then RecycleHex(idx) end
end)

LuaEvents.RespondViewPlot.Add(function(caller, idx, isInView)
	if caller == CALLER and isInView and updates_enabled then
		RenderAnchor(idx)
	end
end)

CivEvents.HexFOWStateChanged.Add(function(hex)
	local gx, gy = civ_to_grid_from_hex(hex.x, hex.y)
	local idx = gx + gy * WORLD_GW

	if not active_anchors[idx] then
		RequestViewPlot(CALLER, idx)
	end
end)

CivEvents.HexYieldMightHaveChanged.Add(function(gx, gy)
	if updates_enabled then
		local idx = gx + gy * WORLD_GW

		if active_anchors[idx] then
			RenderAnchor(idx)
		end
	end
end)

CivEvents.RequestYieldDisplay.Add(function(id)
	if id == YieldDisplayTypes.USER_ALL_ON then
		updates_enabled = true
		C.Anchors:SetHide(false)
		RequestRender()
	elseif id == YieldDisplayTypes.USER_ALL_OFF then
		updates_enabled = false
		C.Anchors:SetHide(true)
		RecycleAll()
	end
end)

CivEvents.GameplaySetActivePlayer.Add(function()
	local last_state = updates_enabled
	updates_enabled = false
	RecycleAll()
	updates_enabled = last_state
	UI.RefreshYieldVisibleMode()
	RequestRender()
end)

UI.RefreshYieldVisibleMode()

--------------------------------------------------------------------------------

function PrintYieldIconStats()
	local active_anchor_count = 0
	local active_stack_count  = 0
	local active_icon_count   = 0
	local active_label_count  = 0

	for _, _ in lua_next, active_anchors do
		active_anchor_count = active_anchor_count + 1
	end

	for _, stacks in lua_next, active_stacks do
		active_stack_count = active_stack_count + #stacks
	end

	for _, icons in lua_next, active_icons do
		for _, _ in lua_next, icons do
			active_icon_count = active_icon_count + 1
		end
	end

	for _, labels in lua_next, active_labels do
		active_label_count = active_label_count + #labels
	end

	local pooled_icon_count = 0
	for _, stack in lua_next, hidden_icons do
		pooled_icon_count = pooled_icon_count + stack:Size()
	end

	local pooled_anchor_count = hidden_anchors:Size()
	local pooled_stack_count  = hidden_stacks:Size()
	local pooled_label_count  = hidden_labels:Size()

	print(string.format([[Yield Icon Stats:
		active -> anchors: %d, stacks: %d, icons: %d, labels: %d
		pooled -> anchors: %d, stacks: %d, icons: %d, labels: %d
		total  -> anchors: %d, stacks: %d, icons: %d, labels: %d]],
		active_anchor_count, active_stack_count,
		active_icon_count, active_label_count,
		pooled_anchor_count, pooled_stack_count,
		pooled_icon_count, pooled_label_count,
		active_anchor_count + pooled_anchor_count,
		active_stack_count + pooled_stack_count,
		active_icon_count + pooled_icon_count,
		active_label_count + pooled_label_count
	))
end
