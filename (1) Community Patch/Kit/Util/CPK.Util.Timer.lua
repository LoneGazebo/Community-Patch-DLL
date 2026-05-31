local lua_next = next
local lua_pcall = pcall
local lua_error = error
local lua_tostring = tostring
local lua_os_clock = os.clock
local lua_math_abs = math.abs
local lua_setmetatable = setmetatable

local Void = CPK.FP.Void

local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable
local AssertIsNumber = CPK.Assert.IsNumber
local AssertIsInteger = CPK.Assert.IsInteger
local AssertIsBoolean = CPK.Assert.IsBoolean
local AssertIsFunction = CPK.Assert.IsFunction

local ControlIsKind = CPK.UI.Control.IsKind

--- @class TimerTask
--- @field callback function # The function to execute when the interval is reached.
--- @field interval number # Time in seconds between executions (e.g., 1.5).
--- @field lifespan? number # Total number of times to run. If nil, runs indefinitely.
--- @field debounce? boolean # If true, resets elapsed time to 0 exactly. If false, preserves overflow for frequency accuracy.

--- @class TimerOpts
--- @field context Control # The LuaContext (ContextPtr) to attach the heartbeat to.
--- @field onClose? fun(tasks: table<integer, TimerTask>) # Optional: Callback for the engine's SetShutdown event.
--- @field onReady? fun(timer: Timer) # Optional: Callback for the engine's SetPostInit event.
--- @field onError? fun(timer: Timer, error: string, taskId: integer, task: TimerTask) # Optional: Custom error handler for task crashes.

--- @class TimerMeta
local TimerMeta = {}
TimerMeta.Symbol = {}

--- @class Timer : TimerOpts
--- @field package last_id integer # An auto-incrementing counter used to assign unique IDs to new tasks.
--- @field package is_live boolean # Indicates if the Timer is currently attached to the context's SetUpdate heartbeat.
--- @field package is_busy boolean # Re-entrancy guard; prevents the update loop from running again if the previous tick is still processing.
--- @field package context Control # The underlying LuaContext providing the execution heartbeat.
--- @field package timeval number  # The timestamp (via os.clock) of the most recent frame; used to calculate the delta time (dt).
--- @field package elapsed table<integer, number> # A map of taskId to accumulated seconds since the task's last execution.
--- @field package invokes table<integer, integer> # A map of taskId to total successful executions; used to track progress against the lifespan.
--- @field package entries table<integer, TimerTask> # The primary registry containing the configuration for all active tasks.
local TimerImpl = {}
TimerMeta.__index = TimerImpl

--- Retrieves the Timer instance associated with a context, or creates one if it doesn't exist.
--- @param context Control
--- @return Timer
function TimerMeta.For(context)
	AssertIsTable(context)
	local timer = context[TimerMeta.Symbol]
	if not timer then
		timer = TimerMeta.New({ context = context })
	end
	return timer
end

--- Creates a new Timer instance.
--- <br> **Ticks only when context is visible.**
--- <br> **This overrides SetUpdate, SetPostInit, and SetShutdown on the context.**
--- @param opts TimerOpts
--- @return Timer
function TimerMeta.New(opts)
	AssertIsTable(opts)

	local context = opts.context
	local onReady = opts.onReady or Void
	local onClose = opts.onClose or Void
	local onError = opts.onError or Void

	AssertIsFunction(onReady)
	AssertIsFunction(onClose)
	AssertIsFunction(onError)

	if not ControlIsKind('LuaContext', context) then
		lua_error('Cannot attach Timer to ' .. lua_tostring(context) .. '. Only LuaContext allowed!')
	end

	AssertIsFunction(context.GetID)
	AssertIsFunction(context.SetUpdate)
	AssertIsFunction(context.ClearUpdate)
	AssertIsFunction(context.SetShutdown)
	AssertIsFunction(context.SetPostInit)

	if context:GetID() == 'YieldIconManager' then
		lua_error('Cannot attach Timer to ' .. lua_tostring(context) .. '. Context is restricted by engine!')
	end

	if context[TimerMeta.Symbol] then
		lua_error('Cannot attach Timer to ' .. lua_tostring(context) .. '. A Timer instance already exists!')
	end

	--- @type Timer
	local this = {
		last_id = 0,
		is_live = false,
		is_busy = false,
		timeval = lua_os_clock(),
		entries = {},
		elapsed = {},
		invokes = {},
		onReady = onReady,
		onClose = onClose,
		onError = onError,
		context = context,
	}

	lua_setmetatable(this, TimerMeta)
	context[TimerMeta.Symbol] = this

	context:SetShutdown(function()
		local tasks = this:RemoveTasks()
		lua_pcall(this.onClose, tasks)
	end)

	context:SetPostInit(function()
		lua_pcall(this.onReady, this)
	end)

	return this
end

--- Detaches the timer heartbeat from the context update loop.
--- @package
function TimerImpl:Detach()
	self.is_live = false
	self.context:ClearUpdate()
end

--- Attaches the timer heartbeat to the context update loop.
--- @package
function TimerImpl:Attach()
	if self.is_live then return end

	self.is_live = true
	self.timeval = lua_os_clock()

	local this = self

	this.context:SetUpdate(function()
		if not this.is_live then return this:Detach() end

		local now = lua_os_clock()
		local dt = now - this.timeval
		this.timeval = now

		if this.is_busy then return end
		this.is_busy = true

		local removal_array = nil

		for id, task in lua_next, this.entries do
			if not this.is_live then break end

			local time_spent = (this.elapsed[id] or 0) + dt

			if time_spent >= task.interval then
				local ok, error = lua_pcall(task.callback)
				if not ok then
					lua_pcall(this.onError, this, error, id, task)
				end

				if task.lifespan then
					local executions = (this.invokes[id] or 0) + 1
					this.invokes[id] = executions

					if executions >= task.lifespan then
						removal_array = removal_array or {}
						removal_array[#removal_array + 1] = id
					end
				end

				time_spent = task.debounce and 0 or (time_spent - task.interval)
			end

			this.elapsed[id] = time_spent
		end

		-- Process bulk removals
		if removal_array then
			for i = 1, #removal_array do
				this:RemoveTask(removal_array[i])
			end
		end

		this.is_busy = false
	end)
end

--- Registers a new task to be executed. Starts the heartbeat if it's currently dormant.
--- @param opts TimerTask
--- @return integer # The unique ID of the created task.
function TimerImpl:CreateTask(opts)
	AssertIsTable(opts)

	local interval = opts.interval
	local lifespan = opts.lifespan
	local debounce = opts.debounce
	local callback = opts.callback

	AssertIsNumber(interval)
	AssertIsFunction(callback)

	interval = lua_math_abs(interval)

	if interval < 0.005 then
		AssertError(interval, '>=0.005', 'Minimum interval is 0.005s (5ms)')
	end

	if lifespan ~= nil then
		AssertIsInteger(lifespan)
		lifespan = lua_math_abs(lifespan)

		if lifespan < 1 then
			AssertError(lifespan, '>=1')
		end
	end

	if debounce ~= nil then
		AssertIsBoolean(debounce)
	else
		debounce = true
	end

	local id = self.last_id + 1
	self.last_id = id

	self.entries[id] = {
		interval = interval,
		callback = callback,
		debounce = debounce,
		lifespan = lifespan,
	}

	self.elapsed[id] = 0
	self.invokes[id] = 0

	if not self.is_live then
		self:Attach()
	end

	return id
end

--- Removes a specific task by its ID.
--- @param id integer
--- @return TimerTask | nil # The removed task definition.
function TimerImpl:RemoveTask(id)
	local task = self.entries[id]

	if task then
		self.entries[id] = nil
		self.elapsed[id] = nil
		self.invokes[id] = nil
	end

	if lua_next(self.entries) == nil and self.is_live then
		self:Detach()
	end

	return task
end

--- Clears all tasks and shuts down the heartbeat.
--- @return table<integer, TimerTask> # A list of tasks that were orphaned by the clear.
function TimerImpl:RemoveTasks()
	self:Detach()

	local orphaned = self.entries

	self.entries = {}
	self.elapsed = {}
	self.invokes = {}

	return orphaned
end

CPK.Util.Timer = TimerMeta
