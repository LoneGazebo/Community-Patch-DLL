local lua_setmetatable = setmetatable
local lua_table_remove = table.remove

--- @class Stack
--- @field protected stack any[]
--- @field protected count integer
local StackPrototype = {}

--- @return any
function StackPrototype:Pop()
	if self.count == 0 then
		return nil
	end

	local value = lua_table_remove(self.stack)
	self.count = self.count - 1

	return value
end

--- @return any
function StackPrototype:Peek()
	return self.stack[self.count]
end

--- @param value any
function StackPrototype:Push(value)
	if value ~= nil then
		self.count = self.count + 1
		self.stack[self.count] = value
	end

	return self
end

--- @return integer
function StackPrototype:Size()
	return self.count
end

--- @return boolean
function StackPrototype:Empty()
	return self.stack[1] == nil
end

--- @class StackMetatable
local StackMetatable = {}

--- @package
StackMetatable.__index = StackPrototype

function StackMetatable.New()
	local this = {
		count = 0,
		stack = {}
	}

	return lua_setmetatable(this, StackMetatable)
end

CPK.Util.Stack = StackMetatable
