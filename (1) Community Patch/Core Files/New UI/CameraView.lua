--[[

# CameraView

Viewport visibility tracker for Civ V world-space UI.

Fires `LuaEvents.PlotEnterView(idx)` and `LuaEvents.PlotLeaveView(idx)`
as plots enter and leave the camera's visible area. Other contexts
(YieldIconManager, ResourceIconManager, etc.) subscribe to these events to
show or hide world-space UI without needing any camera math of their own.

## Glossary

**Directions:** L, R, T, B = Left, Right, Top, Bottom (screen edges)

**Grid space** (hex tile coordinates, integer)
- `gx`, `gy` - tile column and row
- `gw`, `gh` - grid width and height (`MAP_GW`, `MAP_GH`)

**World space** (3D engine units, float)
- `wx`, `wy` - world X and Y position on the ground plane

**Screen space**
- `ex`, `ey` - signed NDC extents: (0,0) is screen center,
  corners are near (+/-0.5, +/-0.5)

## How it works

A timer runs 10 times per second. `CameraViewChanged` updates the stored
camera matrix; `VisibilityUpdated` and `StrategicViewStateChanged` flag the
next tick for a recompute. On a flagged tick the timer builds a fresh set of
visible plot indices and diffs it against the previous set to emit events.

**3D view:** Back-projects the four screen corners (TL, TR, BR, BL) through
the view matrix onto the ground plane (wz = 0) to get four grid-space corner
hexes. The frustum projects as a trapezoid - wide at the top (far edge) and
narrow at the bottom (near edge). A scanline sweep iterates each row between
the min and max corner rows; `intersect_edge` is called once per quad edge to
find the left/right x bounds for that row, then every tile in the range is
marked visible. Wrapping and out-of-bounds rejection are handled per-tile
inside `grid_to_index`.

**Strategic view:** The camera is flat (top-down), so the projection math
is degenerate and back-projecting corners gives no useful frustum shape.
Instead the screen center is raycast to find the center hex, and a fixed
`STRATEGIC_*` hex rectangle around it is marked visible.

## Tuning

- `ADJUST_*` - fraction of the screen half-extent for 3D view.
  1.0 = full screen edge, above 1.0 adds a preload margin beyond the edge.
- `STRATEGIC_*` - hex count from center per edge in strategic view.

## Late join

Fire `LuaEvents.RequestViewPlots(caller)` to receive the full current
in-view set. The module queues the response and delivers it on the
next timer tick via `LuaEvents.RespondViewPlots(caller, plots)`, after
`PlotEnterView` and `PlotLeaveView` events for that tick have already fired.

Fire `LuaEvents.RequestViewPlot(caller, idx)` to check a single plot.
The response arrives via
`LuaEvents.RespondViewPlot(caller, idx, isInView)`.

In both cases `caller` is echoed back so you can ignore responses
triggered by other subscribers. Register your enter and leave handlers
before calling the request so you do not miss events in the intervening
tick.

]]
local _docs
-- hover ^ for rendered markdown

--------------------------------------------------------------------------------

include('CPK.lua')

local lua_next = next
local lua_math_abs = math.abs
local lua_math_max = math.max
local lua_math_min = math.min
local lua_math_floor = math.floor
local lua_math_ceil = math.ceil
local lua_math_huge = math.huge

local CivEvents = Events
local LuaEvents = LuaEvents

local Timer = CPK.Util.Timer

--------------------------------------------------------------------------------

-- Strategic view configuration

local STRATEGIC_L = 14
local STRATEGIC_T = 9
local STRATEGIC_R = 14
local STRATEGIC_B = 9

--------------------------------------------------------------------------------

-- 3D view configuration

-- Finely tuned numbers :D
local ADJUST_L = 0.90 -- left edge
local ADJUST_T = 0.50 -- top edge (far from camera)
local ADJUST_R = 0.90 -- right edge
local ADJUST_B = 0.55 -- bottom edge (near camera)

-- Dividing by negative z (camera looks along -Z) flips both screen axes,
-- so each extent uses the opposite ADJUST and negates the bound side.
local EXTENT_L = 0.5 * ADJUST_R * -1
local EXTENT_R = 0.5 * ADJUST_L
local EXTENT_T = 0.5 * ADJUST_B
local EXTENT_B = 0.5 * ADJUST_T * -1

--------------------------------------------------------------------------------

-- Precalculate what we need

local MAP_IS_WRAP_X = Map.IsWrapX()
local MAP_IS_WRAP_Y = Map.IsWrapY()

local MAP_GW, MAP_GH = Map.GetGridSize()

local world_to_grid

do
	local wx_00, wy_00 = GridToWorld(0, 0)
	local wx_10, _ = GridToWorld(1, 0)
	local wx_01, wy_01 = GridToWorld(0, 1)

	local inv_ww = 1.0 / (wx_10 - wx_00)
	local inv_wh = 1.0 / (wy_01 - wy_00)

	local offset_wx = wx_01 - wx_00

	--- Returns raw (unwrapped, unclamped) grid coordinates.
	--- Used for frustum math where crossing the seam must be detectable.
	--- @param wx number
	--- @param wy number
	--- @return integer # Grid X
	--- @return integer # Grid Y
	world_to_grid = function(wx, wy)
		local dx = wx - wx_00
		local dy = wy - wy_00
		local gy = lua_math_floor(dy * inv_wh + 0.5)

		if lua_math_abs(gy % 2) == 1 then
			dx = dx - offset_wx
		end

		return lua_math_floor(dx * inv_ww + 0.5), gy
	end
end

--- Converts raw (possibly out-of-range) grid coordinates to a plot index.
--- Applies wrapping on enabled axes and returns nil for out-of-bounds on clamped axes.
--- @param gx integer
--- @param gy integer
--- @return integer | nil
local grid_to_index = function(gx, gy)
	if MAP_IS_WRAP_X then
		gx = gx % MAP_GW
	elseif gx < 0 or gx >= MAP_GW then
		return nil
	end

	if MAP_IS_WRAP_Y then
		gy = gy % MAP_GH
	elseif gy < 0 or gy >= MAP_GH then
		return nil
	end

	return gx + gy * MAP_GW
end

--------------------------------------------------------------------------------

--- Back-projects a screen point onto the ground plane (wz = 0).
--- ex and ey are signed NDC extents: screen center is (0, 0), corners are near (+/-0.5, +/-0.5).
--- @param cam number[][] # Civ V view matrix from CameraViewChanged
--- @param ex number # Screen X extent
--- @param ey number # Screen Y extent
--- @return number wx # World X coordinate
--- @return number wy # World Y coordinate
local raycast_ground = function(cam, ex, ey)
	local wx_cx = cam[1][1] - ex * cam[1][3]
	local wy_cx = cam[2][1] - ex * cam[2][3]

	local wx_cy = cam[1][2] - ey * cam[1][3]
	local wy_cy = cam[2][2] - ey * cam[2][3]

	local dx = ex * cam[4][3] - cam[4][1]
	local dy = ey * cam[4][3] - cam[4][2]

	local det = wx_cx * wy_cy - wx_cy * wy_cx

	if lua_math_abs(det) < 0.0001 then
		return 0, 0
	end

	local inv = 1.0 / det

	local wx = inv * (dx * wy_cy - dy * wy_cx)
	local wy = inv * (dy * wx_cx - dx * wx_cy)

	return wx, wy
end

--------------------------------------------------------------------------------

--- Back-projects the 4 screen corners onto the ground plane and returns their
--- raw (unwrapped, unclamped) grid coordinates in clockwise order:
--- `TL->TR->BR->BL`
--- @param cam number[][]
--- @return integer tl_gx, integer tl_gy # Top-Left corner
--- @return integer tr_gx, integer tr_gy # Top-Right corner
--- @return integer br_gx, integer br_gy # Bottom-Right corner
--- @return integer bl_gx, integer bl_gy # Bottom-Left corner
local get_bounds = function(cam)
	local tl_gx, tl_gy = world_to_grid(raycast_ground(cam, EXTENT_L, EXTENT_T))
	local tr_gx, tr_gy = world_to_grid(raycast_ground(cam, EXTENT_R, EXTENT_T))
	local br_gx, br_gy = world_to_grid(raycast_ground(cam, EXTENT_R, EXTENT_B))
	local bl_gx, bl_gy = world_to_grid(raycast_ground(cam, EXTENT_L, EXTENT_B))

	return tl_gx, tl_gy, tr_gx, tr_gy, br_gx, br_gy, bl_gx, bl_gy
end

--------------------------------------------------------------------------------

local is_strategic_view = false
local should_update = true
local camera_data = nil
local current = {}

--- Fills vis with plot indices visible in strategic (top-down) view.
--- Uses a fixed STRATEGIC_* hex rectangle around the screen center.
--- @param cam number[][] # Civ V view matrix
--- @param vis table<integer, boolean> # Set to write visible plot indices into
local update_st_view = function(cam, vis)
	local center_wx, center_wy = raycast_ground(cam, 0, 0)
	local center_gx, center_gy = world_to_grid(center_wx, center_wy)

	local min_gy = lua_math_max(center_gy - STRATEGIC_B, 0)
	local max_gy = lua_math_min(center_gy + STRATEGIC_T, MAP_GH - 1)

	for gy = min_gy, max_gy do
		for gx = center_gx - STRATEGIC_L, center_gx + STRATEGIC_R do
			local idx = grid_to_index(gx, gy)
			if idx then vis[idx] = true end
		end
	end
end

--- Intersects one edge of the frustum quad with a horizontal scanline and expands lo_gx/hi_gx.
--- @param fr_gx integer # Edge start X
--- @param fr_gy integer # Edge start Y
--- @param to_gx integer # Edge end X
--- @param to_gy integer # Edge end Y
--- @param row integer # Scanline row
--- @param lo_gx number # Current left bound
--- @param hi_gx number # Current right bound
--- @return number lo_gx # Updated left bound
--- @return number hi_gx # Updated right bound
local intersect_edge = function(fr_gx, fr_gy, to_gx, to_gy, row, lo_gx, hi_gx)
	local lo_gy = fr_gy < to_gy and fr_gy or to_gy
	local hi_gy = fr_gy < to_gy and to_gy or fr_gy

	if row >= lo_gy and row <= hi_gy then
		local gx

		if fr_gy == to_gy then
			-- horizontal edge: include both endpoints
			if fr_gx < lo_gx then lo_gx = fr_gx end
			if fr_gx > hi_gx then hi_gx = fr_gx end
			gx = to_gx
		else
			gx = fr_gx + (row - fr_gy) * (to_gx - fr_gx) / (to_gy - fr_gy)
		end

		if gx < lo_gx then lo_gx = gx end
		if gx > hi_gx then hi_gx = gx end
	end

	return lo_gx, hi_gx
end

--- Fills vis with plot indices visible in 3D view.
--- Back-projects screen corners to get the four frustum corner hexes, then for
--- each row intersects the left/right X bounds against the quadrilateral edges.
--- Wrap and out-of-bounds handling is delegated to grid_to_index.
--- @param cam number[][] # Civ V view matrix
--- @param vis table<integer, boolean> # Set to write visible plot indices into
local update_3d_view = function(cam, vis)
	-- corners in clockwise order: TL, TR, BR, BL
	local tl_gx, tl_gy, tr_gx, tr_gy, br_gx, br_gy, bl_gx, bl_gy = get_bounds(cam)

	local min_gy = lua_math_min(tl_gy, tr_gy, br_gy, bl_gy)
	local max_gy = lua_math_max(tl_gy, tr_gy, br_gy, bl_gy)

	if not MAP_IS_WRAP_Y then
		min_gy = lua_math_max(min_gy, 0)
		max_gy = lua_math_min(max_gy, MAP_GH - 1)
	elseif max_gy - min_gy >= MAP_GH then
		min_gy = 0
		max_gy = MAP_GH - 1
	end

	for row = min_gy, max_gy do
		local lo_gx = lua_math_huge
		local hi_gx = -lua_math_huge

		lo_gx, hi_gx = intersect_edge(tl_gx, tl_gy, tr_gx, tr_gy, row, lo_gx, hi_gx)
		lo_gx, hi_gx = intersect_edge(tr_gx, tr_gy, br_gx, br_gy, row, lo_gx, hi_gx)
		lo_gx, hi_gx = intersect_edge(br_gx, br_gy, bl_gx, bl_gy, row, lo_gx, hi_gx)
		lo_gx, hi_gx = intersect_edge(bl_gx, bl_gy, tl_gx, tl_gy, row, lo_gx, hi_gx)

		for gx = lua_math_floor(lo_gx), lua_math_ceil(hi_gx) do
			local idx = grid_to_index(gx, row)
			if idx then
				vis[idx] = true
			end
		end
	end
end

local EmitPlotEnterView = LuaEvents.PlotEnterView
local EmitPlotLeaveView = LuaEvents.PlotLeaveView

local RequestViewPlots = LuaEvents.RequestViewPlots
local RespondViewPlots = LuaEvents.RespondViewPlots

local pending_requests = {}

RequestViewPlots.Add(function(caller)
	pending_requests[#pending_requests + 1] = caller
end)

--- Recomputes the visible plot set and emits `PlotEnterView` / `PlotLeaveView`
--- for any plots that entered or left the viewport since the last tick.
--- Also flushes any pending RequestViewPlots responses after the diff.
local update = function()
	if should_update and camera_data then
		should_update = false

		local cam = camera_data
		local vis = {}

		if is_strategic_view then
			update_st_view(cam, vis)
		else
			update_3d_view(cam, vis)
		end

		for idx, _ in lua_next, current do
			if not vis[idx] then
				EmitPlotLeaveView(idx)
			end
		end

		for idx, _ in lua_next, vis do
			if not current[idx] then
				EmitPlotEnterView(idx)
			end
		end

		current = vis
	end

	if #pending_requests > 0 then
		local requests = pending_requests
		pending_requests = {}

		for i = 1, #requests do
			RespondViewPlots(requests[i], current)
		end
	end
end

LuaEvents.RequestViewPlot.Add(function(caller, idx)
	LuaEvents.RespondViewPlot(caller, idx, current[idx] == true)
end)

CivEvents.CameraViewChanged.Add(function(camera)
	camera_data = camera
end)

CivEvents.VisibilityUpdated.Add(function()
	should_update = true
end)

CivEvents.StrategicViewStateChanged.Add(function(bStrategicView)
	is_strategic_view = bStrategicView
	should_update = true
end)

--------------------------------------------------------------------------------

Timer.New({
	context = ContextPtr,
	onReady = function(timer)
		timer:CreateTask({
			interval = 0.1, -- 10 times per second
			callback = update,
		})
	end
})

--- Returns the number of plots currently tracked as visible.
function CountViewportPlots()
	local count = 0
	for _ in lua_next, current do
		count = count + 1
	end
	return count
end
