local lua_xpcall = xpcall
local lua_tostring = tostring

local Void = CPK.FP.Void
local IsTable = CPK.Type.IsTable
local IsFunction = CPK.Type.IsFunction

--- Formats a UI control as a string, appending its GetID() where available.
--- @param control any
--- @return string
local function ControlInspect(control)
	local str = lua_tostring(control)

	if IsTable(control) and IsFunction(control.GetID) then
		if not str:match(': 00000000$') then
			local ok, id = lua_xpcall(function() return control:GetID() end, Void)
			str = str .. ' (' .. lua_tostring(ok and id or nil) .. ')'
		end
	end

	return str
end

CPK.UI.Control.Inspect = ControlInspect
