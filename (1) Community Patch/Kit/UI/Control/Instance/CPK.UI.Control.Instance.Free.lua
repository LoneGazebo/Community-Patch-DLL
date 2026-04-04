local lua_next = next

local IsTable = CPK.Type.IsTable
local IsString = CPK.Type.IsString
local IsBoolean = CPK.Type.IsBoolean
local AssertIsTable = CPK.Assert.IsTable

local InstanceError = CPK.UI.Control.Instance.Error

--- Releases an instance created by `MakeControlInstance`.
---
--- Validates the instance metadata and ensures:
---   - the instance has not already been freed
---   - the root control exists and is a table
---
--- After this call, any access to id fields on `inst` produces a
--- "use-after-release" error.
---
--- @param inst ControlInstance
local function FreeControlInstance(inst)
	AssertIsTable(inst)

	local root = inst[2]
	local parent = inst[4]
	local released = inst[6]

	if released then
		InstanceError(inst, 'Instance already released!')
	end

	if not IsBoolean(released)
			or not IsTable(root)
			or not IsTable(parent)
	then
		InstanceError(inst, 'Failed to free instance, required data is missing!')
	end

	root:SetHide(true)
	parent:ReleaseChild(root)

	inst[2] = nil
	inst[4] = nil
	inst[5] = nil
	inst[6] = true

	for key, _ in lua_next, inst do
		if IsString(key) then
			inst[key] = nil
		end
	end
end

CPK.UI.Control.Instance.Free = FreeControlInstance
