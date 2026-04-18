local lua_select = select

local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

local AssertIsTable = CPK.Assert.IsTable

--- @param w number # Width
--- @param h number # Height
--- @param control Control
local function ControlResizeOne(w, h, control)
	if IsTable(control) and IsFunction(control.SetSize) then
		if w then control:SetSizeX(w) end
		if h then control:SetSizeY(h) end
	end
end

--- @param dims Vector2 | { [1]: number | nil, [2]: number | nil }
--- @param control Control
--- @param ... Control
--- @return Control ...
local function ControlResize(dims, control, ...)
	AssertIsTable(dims)

	local w = dims.x or dims[1]
	local h = dims.y or dims[2]

	if w or h then
		ControlResizeOne(w, h, control)

		-- Inlined ArgsEach to avoid creating a closure
		local len = lua_select('#', ...)

		for i = 1, len do
			ControlResizeOne(w, h, lua_select(i, ...))
		end
	end

	return control, ...
end

CPK.UI.Control.Resize = ControlResize
