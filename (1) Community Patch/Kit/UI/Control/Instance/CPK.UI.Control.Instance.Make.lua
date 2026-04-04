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
--- @field [2] Control # Root control
--- @field [3] string # State
--- @field [4] Control # Parent control
--- @field [5] Control # Context control
--- @field [6] boolean # Release state
--- @field [string] Control | nil

local meta = {
	__index = function(self, id)
		if self[6] then
			InstanceError(self, 'Failed to access "' .. lua_tostring(id) .. '". Use-after-release!')
		end

		InstanceError(self, 'Failed to access "' .. lua_tostring(id) .. '". Such id is not found!')
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
	local state = lua_loadstring('return StateName')()
	local context = lua_loadstring('return ContextPtr')()

	parent = parent or context

	AssertIsTable(context)
	AssertIsTable(parent)
	AssertIsString(name)

	local inst = { name, nil, state, parent, context, nil } --[[@as ControlInstance]]
	local proxy = {} --[[@as table<string, Control>]]

	lua_setmetatable(proxy, {
		__newindex = function(_, key, val)
			if inst[key] then
				InstanceError(inst, 'Duplicate control id: "' .. key .. '"!')
			end

			inst[key] = val
		end
	})

	local root = ContextPtr:BuildInstanceForControl(name, proxy, parent)
	inst[2] = root

	if lua_tostring(root):match(': 00000000$') then
		parent:ReleaseChild(root)
		inst[6] = true
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
		inst[6] = true
		InstanceError(inst, 'Instance defines no id attributes within!')
	end

	inst[6] = false

	lua_setmetatable(inst, meta)

	return inst, root
end

CPK.UI.Control.Instance.Make = MakeControlInstance
