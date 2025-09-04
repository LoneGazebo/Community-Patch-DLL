local lua_setmetatable = setmetatable
local lua_table_remove = table.remove

--- @generic T
--- @class Stack<T>
--- @field protected items T[]
--- @field protected count integer
local StackPrototype = {}

--- Removes and returns the value from the top of the stack.
--- @return T?
function StackPrototype:Pop()
	if self.count == 0 then
		return nil
	end

	local value = lua_table_remove(self.items)
	self.count = self.count - 1

	return value
end

--- Returns the value at the top of the stack without removing it.
--- @return T?
function StackPrototype:Peek()
	return self.items[self.count]
end

--- Adds one value to the top of the stack.
--- @param value T
--- @return self
function StackPrototype:Push(value)
	if value ~= nil then
		self.count = self.count + 1
		self.items[self.count] = value
	end

	return self
end

--- Returns the number of items currently in the stack.
--- @return integer # The number of items.
function StackPrototype:Size()
	return self.count
end

--- Checks if the stack is empty (has 0 items).
--- @return boolean
function StackPrototype:Empty()
	return self.items[1] == nil
end

--- @class StackMetatable
local StackMetatable = {}

--- @package
StackMetatable.__index = StackPrototype

--- Creates a new, empty stack.
--- @generic T
--- @param _ `T`
--- @return Stack<T>
function StackMetatable.New(_)
	local this = {
		count = 0,
		stack = {}
	}

	return lua_setmetatable(this, StackMetatable)
end

CPK.Util.Stack = StackMetatable
