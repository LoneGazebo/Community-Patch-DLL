local lua_next = next
local lua_error = error
local lua_tostring = tostring
local lua_loadstring = loadstring
local lua_setmetatable = setmetatable
local lua_string_format = string.format

local Stack = CPK.Util.Stack
local Queue = CPK.Util.Queue

local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString
local AssertIsFunction = CPK.Assert.IsFunction

local ControlInspect = CPK.UI.Control.Inspect

local MakeInstance = CPK.UI.Control.Instance.Make
local MoveInstance = CPK.UI.Control.Instance.Move
local FreeInstance = CPK.UI.Control.Instance.Free

local errorTemplate = [[InstanceManagerError: %s
	Manager:
		Instance: %s
		Context:  %s
		Mode:     %s
		Size:     %d (acquired: %d, recycled: %d)
	Current:
		State:    %s
		Context:  %s
	Instance: %s
	Instance state: %s
]]

--- Configuration options for creating an InstanceManager.
--- @class InstanceManagerOpts
--- @field mode? 'LIFO' | 'FIFO'
--- @field context? Control
--- @field instance string
--- @field onCreated nil | (fun(inst: ControlInstance): nil)  # Fires once when a new instance is first created.
--- @field onAcquire nil | (fun(inst: ControlInstance): nil)  # Fires every time an instance is acquired, new or recycled.
--- @field onRecycle nil | (fun(inst: ControlInstance): nil)  # Fires when an instance is returned to the pool.
--- @field onRelease nil | (fun(inst: ControlInstance): nil)  # Fires before an instance is permanently destroyed.

--- Manages the full lifecycle of UI control instances.
--- Handles creation, reuse, recycling and permanent release of ControlInstances.
--- Instances move through three states: acquired → recycled → acquired (reuse) or released (destroy).
--- @class InstanceManager : InstanceManagerOpts
--- @field instance string
--- @field mode     'LIFO' | 'FIFO'
--- @field protected context  Control
--- @field protected recycled Stack<ControlInstance> | Queue<ControlInstance>
--- @field protected tracking table<ControlInstance, boolean>  # true = acquired, false = recycled
local InstanceManagerImpl = {}

--- @protected
--- @param inst ControlInstance
--- @param info string
function InstanceManagerImpl:Error(inst, info)
	local state = self.tracking[inst]
	local stateStr = state == nil
			and 'not tracked'
			or (state and 'acquired' or 'recycled')

	local msg = lua_string_format(
		errorTemplate,
		lua_tostring(info),
		lua_tostring(self.instance),
		ControlInspect(self.context),
		lua_tostring(self.mode),
		self:Size(), self:CountAcquired(), self:CountRecycled(),
		lua_tostring(lua_loadstring('return StateName')()),
		ControlInspect(lua_loadstring('return ContextPtr')()),
		lua_tostring(inst),
		stateStr
	)

	lua_error(msg, 3)
end

--- Pops a recycled instance from the pool or creates a new one via MakeInstance.
--- Moves it to `parent`, fires `onCreated` (new instances only),
--- then `onAcquire`, then shows the root control.
--- @param parent Control
--- @return ControlInstance
function InstanceManagerImpl:Acquire(parent)
	local inst = self.recycled:Pop()

	if inst then
		MoveInstance(inst, parent)
	else
		inst = MakeInstance(self.instance, parent, self.context)
		local cb = self.onCreated
		if cb then cb(inst) end
	end

	self.tracking[inst] = true

	local cb = self.onAcquire
	if cb then cb(inst) end

	inst[3]:SetHide(false)

	return inst
end

--- Returns an acquired instance to the pool.
--- Hides the root control, fires `onRecycle`,
--- then pushes the instance onto the recycle stack.
--- Errors if the instance is already recycled or not known to this manager.
--- @param inst ControlInstance
function InstanceManagerImpl:Recycle(inst)
	if self.tracking[inst] ~= true then
		local info = self.tracking[inst] == false
				and 'Recycle called on already recycled instance'
				or 'Recycle called on instance not known to this manager'
		self:Error(inst, info)
	end

	inst[3]:SetHide(true)
	self.tracking[inst] = false

	local cb = self.onRecycle
	if cb then cb(inst) end

	self.recycled:Push(inst)
end

--- Permanently destroys an instance, freeing its underlying control.
--- Fires `onRelease` before destroying.
--- Errors if the instance is recycled or not known to this manager.
--- @param inst ControlInstance
function InstanceManagerImpl:Release(inst)
	if self.tracking[inst] ~= true then
		local info = self.tracking[inst] == false
				and 'Release called on non-acquired instance'
				or 'Release called on instance not known to this manager'
		self:Error(inst, info)
	end

	self.tracking[inst] = nil

	local cb = self.onRelease
	if cb then cb(inst) end

	FreeInstance(inst)
end

--- Returns true if this manager has ever acquired `inst` and it has not been permanently released.
--- @param inst ControlInstance
--- @return boolean
function InstanceManagerImpl:HasInstance(inst)
	return self.tracking[inst] ~= nil
end

--- Returns true if `inst` is currently acquired (not yet recycled or released).
--- @param inst ControlInstance
--- @return boolean
function InstanceManagerImpl:HasAcquired(inst)
	return self.tracking[inst] == true
end

--- Returns true if `inst` is currently sitting in the recycle pool, waiting to be reused.
--- @param inst ControlInstance
--- @return boolean
function InstanceManagerImpl:HasRecycled(inst)
	return self.tracking[inst] == false
end

--- Returns the total number of instances managed (acquired + recycled).
--- @return integer
function InstanceManagerImpl:Size()
	local count = 0
	for _ in lua_next, self.tracking do
		count = count + 1
	end
	return count
end

--- Returns the number of currently acquired instances.
--- @return integer
function InstanceManagerImpl:CountAcquired()
	local count = 0
	for _, v in lua_next, self.tracking do
		if v then count = count + 1 end
	end
	return count
end

--- Returns the number of instances currently in the recycle pool.
--- @return integer
function InstanceManagerImpl:CountRecycled()
	return self.recycled:Size()
end

--- Recycles all currently acquired instances, hiding each root and firing `onRecycle`.
function InstanceManagerImpl:RecycleAcquired()
	local tracking = self.tracking
	local recycled = self.recycled
	local cb = self.onRecycle

	for inst, v in lua_next, tracking do
		if v then
			inst[3]:SetHide(true)
			tracking[inst] = false
			if cb then cb(inst) end
			recycled:Push(inst)
		end
	end
end

--- Permanently releases all recycled instances, firing `onRelease` for each before destroying.
function InstanceManagerImpl:ReleaseRecycled()
	local tracking = self.tracking
	local recycled = self.recycled
	local cb = self.onRelease
	local inst = recycled:Pop()

	while inst do
		tracking[inst] = nil
		if cb then cb(inst) end
		FreeInstance(inst)
		inst = recycled:Pop()
	end
end

--- @class InstanceManagerMeta
local InstanceManagerMeta = {}

InstanceManagerMeta.MODE_LIFO = 'LIFO'
InstanceManagerMeta.MODE_FIFO = 'FIFO'

--- @package
InstanceManagerMeta.ModeStore = {
	[InstanceManagerMeta.MODE_FIFO] = Queue,
	[InstanceManagerMeta.MODE_LIFO] = Stack
}

--- @package
InstanceManagerMeta.__index = InstanceManagerImpl

--- @package
--- @param self InstanceManager
InstanceManagerMeta.__tostring = function(self)
	return lua_string_format(
		'InstanceManager(instance: %s, mode: %s, total: %d, acquired: %d, recycled: %d)',
		lua_tostring(self.instance),
		lua_tostring(self.mode),
		self:Size(),
		self:CountAcquired(),
		self:CountRecycled()
	)
end

--- Creates a new InstanceManager. Validates all opts fields and errors on invalid input.
--- Defaults to LIFO ordering if `opts.mode` is not provided.
--- @param opts InstanceManagerOpts
--- @return InstanceManager
function InstanceManagerMeta.New(opts)
	AssertIsTable(opts) --[[@cast opts table]]
	AssertIsString(opts.instance)

	local context = opts.context or lua_loadstring('return ContextPtr')()

	AssertIsTable(context)

	local mode = opts.mode or 'LIFO'

	AssertIsString(mode)

	local store = InstanceManagerMeta.ModeStore[mode]

	AssertIsTable(store)

	if opts.onCreated then AssertIsFunction(opts.onCreated) end
	if opts.onAcquire then AssertIsFunction(opts.onAcquire) end
	if opts.onRecycle then AssertIsFunction(opts.onRecycle) end
	if opts.onRelease then AssertIsFunction(opts.onRelease) end

	return lua_setmetatable({
		mode = mode,
		context = context,
		instance = opts.instance,
		recycled = store.New(),
		tracking = {},
		onCreated = opts.onCreated,
		onAcquire = opts.onAcquire,
		onRecycle = opts.onRecycle,
		onRelease = opts.onRelease,
	}, InstanceManagerMeta)
end

CPK.UI.Control.Instance.Manager = InstanceManagerMeta
