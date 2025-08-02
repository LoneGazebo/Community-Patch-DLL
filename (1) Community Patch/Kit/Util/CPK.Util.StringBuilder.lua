local _lua_table_insert = table.insert
local _lua_table_concat = table.concat
local _lua_table_remove = table.remove
local _lua_setmetatable = setmetatable

local StringBuilder = {}

--- @param self StringBuilder
--- @return string
function StringBuilder.__tostring(self)
	return self:Concat('\n')
end

--- @class StringBuilder
--- @field _chunks string[] # Internal storage of string chunks.
StringBuilder.__index = {}

--- Creates a new TextBuilder instance.
--- @return StringBuilder
function StringBuilder.New()
	local this = {
		_chunks = {},
	} --[[@as StringBuilder]]

	return _lua_setmetatable(this, StringBuilder)
end

--- Returns string chunk at index if exists
--- @param idx integer
--- @return string | nil
function StringBuilder.__index:At(idx)
	return self._chunks[idx]
end

--- Removes all string chunks.
--- @return StringBuilder
function StringBuilder.__index:Clear()
	self._chunks = {}
	return self
end

--- Returns the number of string chunks.
--- @return integer
function StringBuilder.__index:Size()
	return #(self._chunks)
end

--- Returns the sum of lengths of all chunks.
--- @return integer
function StringBuilder.__index:Length()
	local len = 0
	local chunks = self._chunks
	local size = #(chunks)

	for i = 1, size do
		len = len + #(chunks[i])
	end

	return len
end

--- Checks if there are no string chunks.
--- @return boolean
function StringBuilder.__index:IsEmpty()
	return self._chunks[1] == nil
end

--- Removes and returns the chunk at the given index.
--- @param idx integer
--- @return string | nil # String chunk that was removed or nil
function StringBuilder.__index:Remove(idx)
	return _lua_table_remove(self._chunks, idx)
end

--- Appends a chunk to the end.
--- @param str string
--- @return StringBuilder
function StringBuilder.__index:Append(str)
	_lua_table_insert(self._chunks, str)
	return self
end

--- Inserts a string at the specified index.
--- @param idx integer
--- @param str string
--- @return StringBuilder
function StringBuilder.__index:Insert(idx, str)
	_lua_table_insert(self._chunks, idx, str)
	return self
end

--- Prepends a string to the beginning.
--- @param str string
--- @return StringBuilder
function StringBuilder.__index:Prepend(str)
	return self:Insert(1, str)
end

--- Concatenates all chunks into a single string with an optional separator.
--- Defaults to an empty string if no separator is specified.
--- @param separator? string
--- @param from? integer
--- @param upto? integer
--- @return string
function StringBuilder.__index:Concat(separator, from, upto)
	return _lua_table_concat(self._chunks, separator, from, upto)
end

CPK.Util.StringBuilder = StringBuilder
