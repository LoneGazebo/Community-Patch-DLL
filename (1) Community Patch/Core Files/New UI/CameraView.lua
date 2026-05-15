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
- `ww`, `wh` - world width and height of the full map (`MAP_WW`, `MAP_WH`),
  non-zero only on wrapping maps

**Screen space**
- `ex`, `ey` - signed NDC extents: (0,0) is screen center,
  corners are near (+/-0.5, +/-0.5)
- `sx`, `sy` - projected screen coordinates of a world point
  inside `is_visible`

## How it works

A timer runs 10 times per second. When `VisibilityUpdated` or
`CameraViewChanged` fires, it builds a fresh set of visible plot indices
and diffs it against the previous set to emit the enter/leave events.

**3D view:** Back-projects the four screen corners through the view matrix
onto the ground plane (wz = 0) to get a hex bounding box. Every candidate
hex is then forward-projected to cull the trapezoid shape of the frustum.
World positions are precomputed at load time into `IDX_TO_WX` and `IDX_TO_WY`
to avoid a C++ call per hex per tick. Map wrapping is handled by testing
each hex at its mirrored positions (offset by `MAP_WW` or `MAP_WH`).

**Strategic view:** The camera matrix stays tilted, so projection gives a
trapezoid instead of a rectangle. The screen center is back-projected to
find the center hex, then a fixed `STRATEGIC_*` hex rectangle around it
is marked visible.

## Tuning

- `ADJUST_L/R/T/B` - fraction of the screen half-extent for 3D view.
  1.0 = full screen edge, above 1.0 adds a preload margin beyond the edge.
- `STRATEGIC_L/R/T/B` - hex count from center per edge in strategic view.

## Late join

Fire `LuaEvents.RequestViewPlots(caller)` to receive the full current
in-view set. The module queues the response and delivers it on the
next timer tick via `LuaEvents.RespondViewPlots(caller, plots)`, after
PlotEnterView and PlotLeaveView events for that tick have already fired.

Fire `LuaEvents.RequestViewPlot(caller, idx)` to check a single plot.
The response arrives synchronously via
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

local CivEvents = Events
local LuaEvents = LuaEvents

local Timer = CPK.Util.Timer

--------------------------------------------------------------------------------

-- Strategic view configuration

local STRATEGIC_L = 13
local STRATEGIC_T = 9
local STRATEGIC_R = 13
local STRATEGIC_B = 9

--------------------------------------------------------------------------------

-- 3D view configuration

-- Finely tuned numbers :D
local ADJUST_L = 0.90 -- left edge
local ADJUST_T = 0.50 -- top edge  (far from camera)
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
local MAP_WW, MAP_WH = 0, 0

local IDX_TO_WX = {}
local IDX_TO_WY = {}

local world_to_grid

do
	for idx = 0, Map.GetNumPlots() - 1 do
		local pl = Map.GetPlotByIndex(idx)
		local wx, wy = GridToWorld(pl:GetX(), pl:GetY())

		IDX_TO_WX[idx] = wx
		IDX_TO_WY[idx] = wy
	end

	local wx_00, wy_00 = GridToWorld(0, 0)
	local wx_10, _____ = GridToWorld(1, 0)
	local wx_01, wy_01 = GridToWorld(0, 1)

	local hex_ww = wx_10 - wx_00
	local hex_wh = wy_01 - wy_00

	local inv_ww = 1.0 / hex_ww
	local inv_wh = 1.0 / hex_wh

	local offset_wx = wx_01 - wx_00

	if MAP_IS_WRAP_X then MAP_WW = MAP_GW * hex_ww end
	if MAP_IS_WRAP_Y then MAP_WH = MAP_GH * hex_wh end

	--- Returns raw (unwrapped, unclamped) grid coordinates.
	--- Used for bounding-box math where crossing the seam must be detectable.
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

--------------------------------------------------------------------------------

--- Back-projects a screen point onto the ground plane (wz = 0).
--- ex and ey are signed NDC extents: screen center is (0, 0), corners are near (+/-0.5, +/-0.5).
--- @param cam number[][] Civ V view matrix from CameraViewChanged
--- @param ex number Screen X extent
--- @param ey number Screen Y extent
--- @return number wx World X coordinate
--- @return number wy World Y coordinate
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

--- Returns true if the world point (ground plane, wz = 0) falls within the viewport.
--- Civ V uses negative z for visible objects (right-handed, looks along -Z).
--- @param cam number[][] Civ V view matrix from CameraViewChanged
--- @param wx number World X coordinate
--- @param wy number World Y coordinate
--- @return boolean
local is_visible = function(cam, wx, wy)
	local c1 = cam[1]
	local c2 = cam[2]
	local c4 = cam[4]

	local x = c1[1] * wx + c2[1] * wy + c4[1]
	local y = c1[2] * wx + c2[2] * wy + c4[2]
	local z = c1[3] * wx + c2[3] * wy + c4[3]

	if z == 0 then return false end

	local sx = x / z
	local sy = y / z

	return sx > EXTENT_L and sx < EXTENT_R and sy > EXTENT_B and sy < EXTENT_T
end

--------------------------------------------------------------------------------

--- Back-projects the 4 screen corners and returns the raw (unwrapped) hex bounding box.
--- @param cam number[][]
--- @return integer # Min grid x
--- @return integer # Max grid x
--- @return integer # Min grid y
--- @return integer # Max grid y
local get_bounds = function(cam)
	local gx_a, gy_a = world_to_grid(raycast_ground(cam, EXTENT_L, EXTENT_B))
	local gx_b, gy_b = world_to_grid(raycast_ground(cam, EXTENT_R, EXTENT_B))
	local gx_c, gy_c = world_to_grid(raycast_ground(cam, EXTENT_L, EXTENT_T))
	local gx_d, gy_d = world_to_grid(raycast_ground(cam, EXTENT_R, EXTENT_T))

	local min_gx = lua_math_min(gx_a, gx_b, gx_c, gx_d)
	local max_gx = lua_math_max(gx_a, gx_b, gx_c, gx_d)
	local min_gy = lua_math_min(gy_a, gy_b, gy_c, gy_d)
	local max_gy = lua_math_max(gy_a, gy_b, gy_c, gy_d)

	return min_gx, max_gx, min_gy, max_gy
end

--------------------------------------------------------------------------------

local is_strategic_view = false
local should_update = true
local camera_data = nil
local current = {}

--- Fills vis with plot indices visible in strategic (top-down) view.
--- Uses a fixed STRATEGIC_* hex rectangle around the screen center.
--- @param cam number[][] Civ V view matrix
--- @param vis table<integer, boolean> Set to write visible plot indices into
local update_st_view = function(cam, vis)
	local center_wx, center_wy = raycast_ground(cam, 0, 0)
	local cenraw_gx, cenraw_gy = world_to_grid(center_wx, center_wy)

	local center_gx = cenraw_gx % MAP_GW
	local center_gy = lua_math_max(0, lua_math_min(cenraw_gy, MAP_GH - 1))

	local min_gx = center_gx - STRATEGIC_L
	local max_gx = center_gx + STRATEGIC_R

	if not MAP_IS_WRAP_X then
		min_gx = lua_math_max(min_gx, 0)
		max_gx = lua_math_min(max_gx, MAP_GW - 1)
	end

	local min_gy = lua_math_max(center_gy - STRATEGIC_B, 0)
	local max_gy = lua_math_min(center_gy + STRATEGIC_T, MAP_GH - 1)

	for gy = min_gy, max_gy do
		local gy_base = gy * MAP_GW

		for gx_raw = min_gx, max_gx do
			vis[gx_raw % MAP_GW + gy_base] = true
		end
	end
end

--- Fills vis with plot indices visible in 3D view using exact camera projection.
--- Back-projects screen corners to get a candidate bounding box, then tests each
--- hex with is_visible. Handles cylindrical and toroidal map wrapping.
--- @param cam number[][] Civ V view matrix
--- @param vis table<integer, boolean> Set to write visible plot indices into
local update_3d_view = function(cam, vis)
	local min_gx, max_gx, min_gy, max_gy = get_bounds(cam)

	if not MAP_IS_WRAP_X then
		min_gx = lua_math_max(min_gx, 0)
		max_gx = lua_math_min(max_gx, MAP_GW - 1)
	elseif max_gx - min_gx >= MAP_GW then
		min_gx = 0
		max_gx = MAP_GW - 1
	end

	if not MAP_IS_WRAP_Y then
		min_gy = lua_math_max(min_gy, 0)
		max_gy = lua_math_min(max_gy, MAP_GH - 1)
	elseif max_gy - min_gy >= MAP_GH then
		min_gy = 0
		max_gy = MAP_GH - 1
	end

	for ry = min_gy, max_gy do
		local gy = MAP_IS_WRAP_Y and (ry % MAP_GH) or ry
		local gy_base = gy * MAP_GW

		for rx = min_gx, max_gx do
			local gx = rx % MAP_GW
			local idx = gx + gy_base

			local wx, wy = IDX_TO_WX[idx], IDX_TO_WY[idx]

			local visible = is_visible(cam, wx, wy)

			if not visible and MAP_IS_WRAP_X then
				visible = false
						or is_visible(cam, wx + MAP_WW, wy)
						or is_visible(cam, wx - MAP_WW, wy)
			end

			if not visible and MAP_IS_WRAP_Y then
				visible = false
						or is_visible(cam, wx, wy + MAP_WH)
						or is_visible(cam, wx, wy - MAP_WH)
			end

			if visible then
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

--- Recomputes the visible plot set and emits PlotEnterView / PlotLeaveView
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
function CountVisiblePlots()
	local count = 0
	for _ in lua_next, current do
		count = count + 1
	end
	return count
end
