local lua_error = error
local lua_tostring = tostring
local lua_loadstring = loadstring
local lua_string_format = string.format

local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

local template = [[%s
	Instance name: %s
	Instance root: %s
	Instance state: %s
	Instance parent: %s
	Instance context: %s
	Instance released: %s
	Current state: %s
	Current context: %s
]]

local function fc(control)
	local str = lua_tostring(control)

	if IsTable(control) and IsFunction(control.GetID) then
		str = lua_tostring(control:GetID()) .. ' (' .. str .. ')'
	end

	return str
end

--- @param inst ControlInstance
--- @param info string
local function InstanceError(inst, info)
	local msg = lua_string_format(
		template,
		lua_tostring(info),
		lua_tostring(inst[1]),
		fc(inst[2]),
		lua_tostring(inst[3]),
		fc(inst[4]),
		fc(inst[5]),
		lua_tostring(inst[6]),
		lua_tostring(lua_loadstring('return StateName')()),
		lua_tostring(lua_loadstring('return ContextPtr')())
	)

	lua_error(msg, 2)
end

CPK.UI.Control.Instance.Error = InstanceError
