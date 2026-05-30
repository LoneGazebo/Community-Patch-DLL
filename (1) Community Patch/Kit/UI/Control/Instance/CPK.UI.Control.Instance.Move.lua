local AssertIsTable = CPK.Assert.IsTable

local InstanceError = CPK.UI.Control.Instance.Error

--- Moves control instance to another parent.
--- @param inst ControlInstance
--- @param dest Control
local function MoveControlInstance(inst, dest)
	AssertIsTable(inst)
	AssertIsTable(dest)

	if inst[2] then
		InstanceError(inst, 'Failed to move instance. Instance already released!')
	end

	local root = inst[3]

	AssertIsTable(root)

	--[[@cast root Control]]

	root:ChangeParent(dest)
	inst[4] = dest

	return inst
end

CPK.UI.Control.Instance.Move = MoveControlInstance
