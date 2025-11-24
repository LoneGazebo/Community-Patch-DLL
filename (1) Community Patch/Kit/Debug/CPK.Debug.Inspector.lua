local lua_type = type
local lua_next = next
local lua_tostring = tostring

local lua_string_rep = string.rep

local lua_table_concat = table.concat
local lua_table_insert = table.insert

local lua_getmetatable = getmetatable
local lua_setmetatable = setmetatable

local IsNumber = CPK.Type.IsNumber
local IsString = CPK.Type.IsString
local IsFunction = CPK.Type.IsFunction

local AssertError = CPK.Assert.Error
local StringEscape = CPK.String.Escape

local Always = CPK.FP.Always

--- @class Inspector
--- @field protected cache table
--- @field protected space string
--- @field protected delim string
--- @field protected indent string
local InspectorImpl = {}

--- @protected
function InspectorImpl:Clear()
	self.cache = {}

	return self
end

function InspectorImpl:Inspect(val)
	self:Clear()
	local result = self:Format(val, 1)
	self:Clear()

	return result
end

--- @protected
function InspectorImpl:Persist(val)
	local comm = self.cache[val]

	if comm then
		return comm, false
	end

	local space = self.space
	local meta = lua_getmetatable(val)
	local addr

	if meta and IsFunction(meta.__tostring) then
		lua_setmetatable(val, nil)
		addr = lua_tostring(val)
		lua_setmetatable(val, meta)
	else
		addr = lua_tostring(val)
	end


	self.cache[val] = '--[[' .. space .. '*' .. addr .. space .. ']]'

	return ('--[[' .. space .. addr .. space .. ']]'), true
end

--- @protected
function InspectorImpl:Format(val, lvl)
	local type = lua_type(val)

	if type == 'nil' or type == 'number' or type == 'boolean' then
		return lua_tostring(val)
	end

	if type == 'string' then
		return '"' .. StringEscape(val) .. '"'
	end

	if type == 'table' then
		return self:FormatTable(val, lvl)
	end

	return self:FormatOther(val, lvl)
end

--- @protected
function InspectorImpl:FormatTable(tbl, lvl)
	local space = self.space
	local comm, new = self:Persist(tbl)

	if not new then
		return '{' .. space .. comm .. space .. '}'
	end

	local lines = {}

	for key, val in lua_next, tbl do
		-- Order of calls is important!
		local v = self:Format(val, lvl + 1)
		local k = self:Format(key, lvl + 1)

		lua_table_insert(lines, '[' .. k .. ']' .. space .. '=' .. space .. v)
	end

	if lines[1] ~= nil then
		local d = self.delim
		local i = self.indent

		local outerIndent = d .. lua_string_rep(i, lvl)
		local innerIndent = ',' .. d .. lua_string_rep(i, lvl + 1)

		lua_table_insert(lines, '{' .. space .. comm)

		return lua_table_concat(lines, innerIndent) .. outerIndent
	end

	return '{' .. space .. comm .. space .. '}'
end

--- @protected
function InspectorImpl:FormatOther(oth, lvl)
	local space = self.space
	local comm = self:Persist(oth)

	return '{' .. space .. comm .. space .. '}'
end

local InspectorMeta = {}

--- @param indent integer | string | nil
--- @return Inspector
function InspectorMeta.New(indent)
	local this = {
		cache = {},
		space = '',
		delim = '',
		indent = '',
	}

	if indent ~= nil then
		if IsNumber(indent) then
			indent = lua_string_rep(' ', indent --[[@as number]])
		end

		if not IsString(indent) then
			AssertError(lua_type(indent), 'string or number')
		end

		this.space = ' '
		this.delim = '\n'
		this.indent = indent --[[@as string]]
	end

	return lua_setmetatable(this, InspectorMeta)
end

InspectorMeta.__index = InspectorImpl

CPK.Debug.Inspector = InspectorMeta
