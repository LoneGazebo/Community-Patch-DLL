local lua_setmetatable = setmetatable
local lua_table_remove = table.remove

--- @generic T
--- @class Stack<T>
--- @field protected items any[]
--- @field protected count integer
local StackImpl = {}

--- Removes and returns the value from the top of the stack.
--- @return any
function StackImpl:Pop()
	if self.count <= 0 then
		return nil
	end

	local item = lua_table_remove(self.items)
	self.count = self.count - 1

	return item
end

--- Returns the value at the top of the stack without removing it.
--- @return T
function StackImpl:Peek()
	return self.items[self.count]
end

--- Adds one value to the top of the stack.
--- If value is nil then does nothing.
--- @param item any
function StackImpl:Push(item)
	if item ~= nil then
		self.count = self.count + 1
		self.items[self.count] = item
	end

	return self
end

--- Returns the number of items currently in the stack.
--- @return integer # The number of items.
function StackImpl:Size()
	return self.count
end

--- Checks if the stack is empty (has 0 items).
--- @return boolean
function StackImpl:Empty()
	return self.items[1] == nil
end

--- @class StackMeta
local StackMeta = {}

--- @package
StackMeta.__index = StackImpl

--- Creates a new, empty stack.
--- @generic T
--- @param _ nil | `T`
--- @return Stack<T>
function StackMeta.New(_)
	local this = {
		count = 0,
		items = {}
	}

	return lua_setmetatable(this, StackMeta)
end

CPK.Util.Stack = StackMeta
