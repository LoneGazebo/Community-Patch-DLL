local lua_table_insert = table.insert
local lua_table_concat = table.concat
local lua_table_remove = table.remove
local lua_setmetatable = setmetatable

--- @class StringBuilder
--- @field protected items string[]
local StringBuilderImpl = {}

--- Returns string chunk at index if exists
--- @param idx integer
--- @return string | nil
function StringBuilderImpl:At(idx)
	return self.items[idx]
end

--- Removes all string chunks.
--- @return StringBuilder
function StringBuilderImpl:Clear()
	self.items = {}
	return self
end

--- Returns the number of chunks.
--- @return integer
function StringBuilderImpl:Size()
	return #(self.items)
end

--- Checks if there are no string chunks.
--- @return boolean
function StringBuilderImpl:Empty()
	return self.items[1] == nil
end

--- Removes and returns the chunk at the given index.
--- @param idx integer
--- @return string | nil # String chunk that was removed or nil
function StringBuilderImpl:Remove(idx)
	return lua_table_remove(self.items, idx)
end

--- Appends a chunk to the end.
--- @param str string
--- @return StringBuilder
function StringBuilderImpl:Append(str)
	lua_table_insert(self.items, str)
	return self
end

--- Inserts a string at the specified index.
--- @param idx integer
--- @param str string
--- @return StringBuilder
function StringBuilderImpl:Insert(idx, str)
	lua_table_insert(self.items, idx, str)
	return self
end

--- Prepends a string to the beginning.
--- @param str string
--- @return StringBuilder
function StringBuilderImpl:Prepend(str)
	return self:Insert(1, str)
end

--- Concatenates all chunks into a single string with an optional separator.
--- Defaults to an empty string if no separator is specified.
--- @param separator? string
--- @param from? integer
--- @param upto? integer
--- @return string
function StringBuilderImpl:Concat(separator, from, upto)
	return lua_table_concat(self.items, separator, from, upto)
end

--- @class StringBuilderMeta
local StringBuilderMeta = {}

--- @package
--- @param self StringBuilder
--- @return string
function StringBuilderMeta.__tostring(self)
	return self:Concat('\n')
end

--- @package
StringBuilderMeta.__index = StringBuilderImpl

--- Creates a new StringBuilder instance.
--- @return StringBuilder
function StringBuilderMeta.New()
	local this = {
		items = {}
	}

	return lua_setmetatable(this, StringBuilderMeta)
end

CPK.Util.StringBuilder = StringBuilderMeta
