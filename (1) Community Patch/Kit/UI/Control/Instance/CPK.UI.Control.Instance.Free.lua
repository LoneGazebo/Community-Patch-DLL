local lua_next = next
local lua_error = error
local lua_tostring = tostring
local lua_getmetatable = getmetatable

local IsTable = CPK.Type.IsTable
local IsString = CPK.Type.IsString
local IsBoolean = CPK.Type.IsBoolean
local AssertIsTable = CPK.Assert.IsTable

--- @param inst table<string, Control>
--- @param detail string
local function Error(inst, detail)
	lua_error(
		'Failed to release instance: ' .. lua_tostring(inst) .. '\n' .. detail,
		2
	)
end

--- Releases an instance created by `MakeControlInstance`.
---
--- Validates the instance metadata and ensures:
---   - the instance has not already been freed
---   - its metatable contains the required fields
---   - the root control exists and is a table
---
--- Release process:
---   - iterates all child controls except the root
---   - hides each control and detaches it from the root
---   - hides the root and detaches it from its parent
---   - clears metatable parent reference
---
--- After this call, any access to fields on `inst` produces a
--- "use-after-release" error.
---
--- @param inst table<string, Control>
local function FreeControlInstance(inst)
	AssertIsTable(inst)

	local meta = lua_getmetatable(inst)

	if not meta then
		Error(inst, 'Table lacks metatable with necessary data!')
	end

	local freed = meta.freed

	if freed then
		Error(inst, 'Instance already released!')
	end

	local rootId = meta.rootId
	local parent = meta.parent
	local root = inst[rootId]

	if not IsBoolean(freed)
			or not IsString(rootId)
			or not IsTable(parent)
	then
		Error(inst, 'Specified table has metatable but lacks necessary data!')
	end

	if not IsTable(root) then
		Error(inst, 'Root control for ID "' .. rootId .. '" not found!')
	end

	for id, ctrl in lua_next, inst do
		if id ~= rootId then
			inst[id] = nil
			ctrl:SetHide(true)
			root:ReleaseChild(ctrl)
		end
	end

	root:SetHide(true)
	parent:ReleaseChild(root)

	meta.parent = nil
end

CPK.UI.Control.Instance.Free = FreeControlInstance
