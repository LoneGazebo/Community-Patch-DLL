local lua_setmetatable = setmetatable

local lua_next = next

--- @class SimpleControlPool
--- @field protected instName string
--- @field protected rootName string
--- @field protected parent Control
--- @field protected free table<Control, boolean>
--- @field protected used table<Control, boolean>
local SimpleControlPoolImpl = {}

--- @return Control
function SimpleControlPoolImpl:AcquireOne()
	local inst = lua_next(self.free)

	if inst then
		self.free[inst] = nil
		self.used[inst] = true

		inst:SetHide(true)

		return inst
	end

	inst = ContextPtr:BuildInstanceForControl(
		self.instName,
		self.rootName,
		self.parent
	)

	inst:SetHide(true)

	self.used[inst] = true

	return inst
end

--- @param inst Control
function SimpleControlPoolImpl:RecycleOne(inst)
	if not self.used[inst] then
		return
	end

	inst:SetHide(true)

	self.used[inst] = nil
	self.free[inst] = true
end

--- comment
function SimpleControlPoolImpl:RecycleAll()
	local used = self.used
	local free = self.free

	for inst in lua_next, used do
		inst:SetHide(true)

		used[inst] = nil
		free[inst] = true
	end
end

--- comment
--- @param inst Control
function SimpleControlPoolImpl:DestroyOne(inst)
	if self.used[inst] then
		self.used[inst] = nil
	elseif self.free[inst] then
		self.free[inst] = nil
	else
		return
	end

	inst:SetHide(true)
	self.parent:DestroyChild(inst)
end

--- comment
function SimpleControlPoolImpl:DestroyAll()
	local free = self.free
	local used = self.used
	local parent = self.parent

	for inst in lua_next, used do
		inst:SetHide(true)
		used[inst] = nil

		parent:DestroyChild(inst)
	end

	for inst in lua_next, free do
		inst:SetHide(true)
		free[inst] = nil

		parent:DestroyChild(inst)
	end
end

local SimpleControlPoolMeta = {}

SimpleControlPoolMeta.__index = SimpleControlPoolImpl

--- @param instName string
--- @param rootName string
--- @param parent Control | nil
--- @return SimpleControlPool
function SimpleControlPoolMeta.New(instName, rootName, parent)
	local this = {
		instName = instName,
		rootName = rootName,
		parent = parent or ContextPtr,
		free = {},
		used = {},
	}

	return lua_setmetatable(this, SimpleControlPoolMeta)
end

CPK.UI.ControlPool.Simple = SimpleControlPoolMeta
