local lua_error = error
local lua_xpcall = xpcall
local lua_tostring = tostring
local lua_loadstring = loadstring
local lua_string_format = string.format

local Void = CPK.FP.Void
local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

local template = [[InstanceError: %s
	Instance name: %s
	Instance root: %s
	Instance parent: %s
	Instance released: %s
	Current state: %s
	Current context: %s
]]

--- @param control any
--- @return string
local function fc(control)
	local str = lua_tostring(control)

	if IsTable(control) and IsFunction(control.GetID) then
		if not str:match(': 00000000$') then
			local ok, id = lua_xpcall(function() return control:GetID() end, Void)
			str = str .. ' (' .. lua_tostring(ok and id or nil) .. ')'
		end
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
		fc(inst[3]),
		fc(inst[4]),
		lua_tostring(inst[2]),
		lua_tostring(lua_loadstring('return StateName')()),
		fc(lua_loadstring('return ContextPtr')())
	)

	lua_error(msg, 3)
end

CPK.UI.Control.Instance.Error = InstanceError
