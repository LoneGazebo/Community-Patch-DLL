local lua_setmetatable = setmetatable

--- LIFO stack. Items are pushed and popped from the same end (top).
--- @generic T
--- @class Stack<T>
--- @field protected items T[]
--- @field protected count integer
--- @field public Pop fun(self: Stack<T>): T | nil
--- @field public Peek fun(self: Stack<T>): T | nil
--- @field public Push fun(self: Stack<T>, item: T): Stack<T>
--- @field public Size fun(self: Stack<T>): integer
--- @field public Empty fun(self: Stack<T>): boolean
local StackImpl = {}

--- Removes and returns the value from the top of the stack.
function StackImpl:Pop()
	if self.count <= 0 then return nil end

	local item = self.items[self.count]

	self.items[self.count] = nil
	self.count = self.count - 1

	return item
end

--- Returns the value at the top of the stack without removing it.
function StackImpl:Peek()
	return self.items[self.count]
end

--- Adds one value to the top of the stack.
--- If value is nil then does nothing.
function StackImpl:Push(item)
	if item ~= nil then
		self.count = self.count + 1
		self.items[self.count] = item
	end

	return self
end

--- Returns the number of items currently in the stack.
function StackImpl:Size()
	return self.count
end

--- Checks if the stack is empty (has 0 items).
function StackImpl:Empty()
	return self.count <= 0
end

--- @class StackMeta
local StackMeta = {}

--- @package
StackMeta.__index = StackImpl

--- Creates a new, empty stack.
--- @return Stack<unknown>
function StackMeta.New()
	local this = {
		count = 0,
		items = {}
	}

	return lua_setmetatable(this, StackMeta)
end

CPK.Util.Stack = StackMeta
