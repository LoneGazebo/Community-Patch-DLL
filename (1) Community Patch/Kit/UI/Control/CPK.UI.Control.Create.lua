local lua_error = error
local lua_tostring = tostring

local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString

--- Creates a new UI control instance from XML.
---
--- Uses `ContextPtr:BuildInstanceForControl` to construct an instance
--- and attaches it to the given parent (or to `ContextPtr` if no parent
--- is specified).
---
--- Ensures that a dangling or `NULL` control cannot persist
--- by checking the pointer string and immediately releasing invalid instances.
--- @param name string # The `<Instance Name="...">` defined in the XML file
--- @param parent table? # Parent control (defaults to `ContextPtr`)
--- @return Control # A valid control instance
local function ControlCreate(name, parent)
	AssertIsTable(ContextPtr)

	-- This should prevent dangling control instances
	if parent == nil then
		parent = ContextPtr
	end

	AssertIsTable(parent)
	AssertIsString(name)

	local instance = {}

	ContextPtr:BuildInstanceForControl(name, instance, parent)

	local str = lua_tostring(instance)

	--- NULL control created
	if str:match(': 00000000$') then
		parent:ReleaseChild(instance)

		lua_error(
			'Failed to create ' .. name .. ' control instance.'
			.. '\n\t<Instance Name="' .. name .. '" />'
			.. ' was not found in related XML file.'
			.. '\n\tPlease verify file: ' .. lua_tostring(StateName) .. '.xml'
		)
	end

	return instance --[[@as Control]]
end

CPK.UI.Control.Create = ControlCreate
