local lua_setmetatable = setmetatable

--- FIFO queue backed by two internal stacks.
--- Items are pushed onto one stack and popped from the other.
--- When the pop-side runs empty it is refilled by reversing the push-side,
--- keeping both Push and Pop amortized O(1).
--- @generic T
--- @class Queue<T>
--- @field protected pushed_items T[]
--- @field protected pushed_count integer
--- @field protected popped_items T[]
--- @field protected popped_count integer
--- @field public Pop  fun(self: Queue<T>): T | nil
--- @field public Peek fun(self: Queue<T>): T | nil
--- @field public Push fun(self: Queue<T>, item: T): Queue<T>
--- @field public Size fun(self: Queue<T>): integer
--- @field public Empty fun(self: Queue<T>): boolean
local QueueImpl = {}

--- @package
function QueueImpl:Drain()
	local pushed = self.pushed_items
	local popped = self.popped_items
	local count  = self.pushed_count

	local j      = 1

	for i = count, 1, -1 do
		popped[j] = pushed[i]
		pushed[i] = nil
		j = j + 1
	end

	self.popped_count = count
	self.pushed_count = 0
end

--- Removes and returns the value from the front of the queue.
function QueueImpl:Pop()
	if self.popped_count <= 0 then
		if self.pushed_count <= 0 then
			return nil
		end

		self:Drain()
	end

	local count = self.popped_count
	local item = self.popped_items[count]

	self.popped_items[count] = nil
	self.popped_count = count - 1

	return item
end

--- Returns the value at the front of the queue without removing it.
function QueueImpl:Peek()
	if self.popped_count <= 0 then
		if self.pushed_count <= 0 then
			return nil
		end

		self:Drain()
	end

	return self.popped_items[self.popped_count]
end

--- Adds one value to the back of the queue.
--- If value is nil then does nothing.
function QueueImpl:Push(item)
	if item ~= nil then
		local count = self.pushed_count + 1
		self.pushed_items[count] = item
		self.pushed_count = count
	end

	return self
end

--- Returns the number of items currently in the queue.
function QueueImpl:Size()
	return self.pushed_count + self.popped_count
end

--- Checks if the queue is empty (has 0 items).
function QueueImpl:Empty()
	return self.pushed_count <= 0 and self.popped_count <= 0
end

--- @class QueueMeta
local QueueMeta = {}

--- @package
QueueMeta.__index = QueueImpl

--- Creates a new, empty FIFO queue.
--- @return Queue<unknown>
function QueueMeta.New()
	return lua_setmetatable({
		pushed_items = {},
		pushed_count = 0,
		popped_items = {},
		popped_count = 0,
	}, QueueMeta)
end

CPK.Util.Queue = QueueMeta
