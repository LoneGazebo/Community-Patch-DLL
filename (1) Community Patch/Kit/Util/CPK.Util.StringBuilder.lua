local lua_table_insert = table.insert
local lua_table_concat = table.concat
local lua_table_remove = table.remove
local lua_setmetatable = setmetatable

--- @class StringBuilder
--- @field protected chunks string[]
local StringBuilderPrototype = {}

--- Returns string chunk at index if exists
--- @param idx integer
--- @return string | nil
function StringBuilderPrototype:At(idx)
	return self.chunks[idx]
end

--- Removes all string chunks.
--- @return StringBuilder
function StringBuilderPrototype:Clear()
	self.chunks = {}
	return self
end

--- Returns the number of chunks.
--- @return integer
function StringBuilderPrototype:Size()
	return #(self.chunks)
end

--- Checks if there are no string chunks.
--- @return boolean
function StringBuilderPrototype:Empty()
	return self.chunks[1] == nil
end

--- Removes and returns the chunk at the given index.
--- @param idx integer
--- @return string | nil # String chunk that was removed or nil
function StringBuilderPrototype:Remove(idx)
	return lua_table_remove(self.chunks, idx)
end

--- Appends a chunk to the end.
--- @param str string
--- @return StringBuilder
function StringBuilderPrototype:Append(str)
	lua_table_insert(self.chunks, str)
	return self
end

--- Inserts a string at the specified index.
--- @param idx integer
--- @param str string
--- @return StringBuilder
function StringBuilderPrototype:Insert(idx, str)
	lua_table_insert(self.chunks, idx, str)
	return self
end

--- Prepends a string to the beginning.
--- @param str string
--- @return StringBuilder
function StringBuilderPrototype:Prepend(str)
	return self:Insert(1, str)
end

--- Concatenates all chunks into a single string with an optional separator.
--- Defaults to an empty string if no separator is specified.
--- @param separator? string
--- @param from? integer
--- @param upto? integer
--- @return string
function StringBuilderPrototype:Concat(separator, from, upto)
	return lua_table_concat(self.chunks, separator, from, upto)
end

--- @class StringBuilderMetatable
local StringBuilderMetatable = {}

--- @package
--- @param self StringBuilder
--- @return string
function StringBuilderMetatable.__tostring(self)
	return self:Concat('\n')
end

--- @package
StringBuilderMetatable.__index = StringBuilderPrototype

--- Creates a new StringBuilder instance.
--- @return StringBuilder
function StringBuilderMetatable.New()
	local this = {
		chunks = {},
	}

	return lua_setmetatable(this, StringBuilderMetatable)
end

CPK.Util.StringBuilder = StringBuilderMetatable
