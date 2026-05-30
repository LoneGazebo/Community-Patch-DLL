local lua_error = error
local lua_tostring = tostring
local lua_loadstring = loadstring
local lua_string_format = string.format

local ControlInspect = CPK.UI.Control.Inspect

local template = [[InstanceError: %s
	Instance:
		Name:     %s
		Root:     %s
		Parent:   %s
		Released: %s
	Current:
		State:   %s
		Context: %s
]]

--- @param inst ControlInstance
--- @param info string
local function InstanceError(inst, info)
	local msg = lua_string_format(
		template,
		lua_tostring(info),
		lua_tostring(inst[1]),
		ControlInspect(inst[3]),
		ControlInspect(inst[4]),
		lua_tostring(inst[2]),
		lua_tostring(lua_loadstring('return StateName')()),
		ControlInspect(lua_loadstring('return ContextPtr')())
	)

	lua_error(msg, 3)
end

CPK.UI.Control.Instance.Error = InstanceError
