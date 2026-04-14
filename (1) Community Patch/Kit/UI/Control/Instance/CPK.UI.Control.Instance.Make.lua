local lua_type = type
local lua_next = next
local lua_tostring = tostring
local lua_loadstring = loadstring
local lua_setmetatable = setmetatable

local IsString = CPK.Type.IsString

local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString

local InstanceError = CPK.UI.Control.Instance.Error

--- @class ControlInstance
--- @field [1] string # Instance name
--- @field [2] boolean # Is released
--- @field [3] Control | nil # Root control
--- @field [4] Control | nil # Parent control
--- @field [string] Control | nil

local ControlInstanceMeta = {
	__index = function(self, id)
		-- Prevent stack overflow
		if lua_type(id) == 'number' then
			return nil
		end

		if self[2] then
			InstanceError(self, 'Failed to access `' .. lua_tostring(id) .. '`. Use-after-release!')
		end

		InstanceError(self, 'Failed to access `' .. lua_tostring(id) .. '`. Id not found!')
	end,
	__newindex = function(self)
		InstanceError(self, 'Instance modification is prohibited!')
	end
}

--- Creates a new UI control instance from XML.
---
--- Wraps `ContextPtr:BuildInstanceForControl` and collects all controls
--- assigned by the engine into a Lua table. Performs strict validation:
---   - rejects duplicate control IDs
---   - rejects missing or invalid root control
---   - rejects instances with no defined IDs
---
--- The returned table is protected by a metatable:
---   - accessing unknown IDs triggers an error
---   - accessing valid IDs after freeing triggers an error
---
--- **Do not call `ChangeParent` on any control created by this function
--- unless it will never be freed through this API.**
---
--- @param name string # `<Instance Name="...">` as defined in XML
--- @param parent Control | nil # Parent control (defaults to `ContextPtr`)
--- @return ControlInstance # Table of child controls indexed by XML ID
--- @return Control # Root control for the created instance (Always ControlBase)
local function MakeControlInstance(name, parent)
	local context = lua_loadstring('return ContextPtr')()

	parent = parent or context

	AssertIsTable(context)
	AssertIsTable(parent)
	AssertIsString(name)

	local inst = { name, nil, nil, parent } --[[@as ControlInstance]]
	local proxy = {} --[[@as table<string, Control>]]

	lua_setmetatable(proxy, {
		__newindex = function(_, key, val)
			if inst[key] then
				InstanceError(inst, 'Duplicate control id: `' .. key .. '`!')
			end

			inst[key] = val
		end
	})

	local root = context:BuildInstanceForControl(name, proxy, parent)
	inst[3] = root

	if lua_tostring(root):match(': 00000000$') then
		parent:ReleaseChild(root)
		inst[2] = true
		InstanceError(inst, 'Instance not found or lacks root element in xml file!')
	end

	local hasId = false

	for key, _ in lua_next, inst do
		if IsString(key) then
			hasId = true
			break
		end
	end

	if not hasId then
		parent:ReleaseChild(root)
		inst[2] = true
		InstanceError(inst, 'Instance defines no id attributes within!')
	end

	inst[2] = false

	lua_setmetatable(inst, ControlInstanceMeta)

	return inst, root
end

CPK.UI.Control.Instance.Make = MakeControlInstance
