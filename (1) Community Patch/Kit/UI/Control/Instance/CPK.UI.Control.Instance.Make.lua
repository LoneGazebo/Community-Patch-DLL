local lua_error = error
local lua_tostring = tostring
local lua_table_concat = table.concat
local lua_setmetatable = setmetatable

local TableKeys = CPK.Table.Keys
local TableEmpty = CPK.Table.Empty
local TableHashify = CPK.Table.Hashify
local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString

local function Error(parent, ...)
	local s = lua_tostring
	local c = lua_table_concat
	local m = c({ ... }, '\n')
			.. '\n' .. 'Details:\n\t'
			.. c({
				'State   -> ' .. s(StateName),
				'Parent  -> ' .. s(parent) .. '\t' .. s(parent:GetID())
				'Context -> ' .. s(ContextPtr) .. '\t' .. s(ContextPtr:GetID())
			}, '\n\t')

	lua_error(m, 2)
end

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
--- @return table<string, Control> # Table of child controls indexed by XML ID
--- @return Control # Root control for the created instance (Always ControlBase)
local function MakeControlInstance(name, parent)
	parent = parent or ContextPtr
	AssertIsTable(ContextPtr)
	AssertIsTable(parent)
	AssertIsString(name)

	local elem = '<Instance Name="' .. name .. '">'
	local fail = 'Failed to instantiate ' .. elem

	local inst = {} --[[@as table<string, Control>]]
	local proxy = {} --[[@as table<string, Control>]]

	lua_setmetatable(proxy, {
		__newindex = function(_, key, val)
			if inst[key] then
				Error(parent, fail, 'Duplicate control ID: "' .. key .. '"')
			end

			inst[key] = val
		end
	})

	local root = ContextPtr:BuildInstanceForControl(name, proxy, parent)
	local known = TableHashify(TableKeys(inst))

	if lua_tostring(root):match(': 00000000$') then
		parent:ReleaseChild(root)
		Error(parent, fail, elem .. ' not found or lacks root element in XML!')
	end

	if TableEmpty(inst) then
		parent:ReleaseChild(root)
		Error(parent, fail, elem .. ' defines no ID attributes within!')
	end

	lua_setmetatable(inst, {
		rootId = root:GetID(),
		parent = parent,
		freed = false,
		__index = function(_, key)
			if known[key] then
				Error(parent, 'Failed to access "' .. key .. '"', 'Use-after-release!')
			else
				Error(parent, 'Failed to access "' .. key .. '"', 'ID not defined in XML!')
			end
		end
	})

	return inst, root
end

CPK.UI.Control.Instance.Make = MakeControlInstance
