local lua_setmetatable = setmetatable

local lua_tostring = tostring
local lua_math_floor = math.floor
local lua_string_format = string.format

local civ_db_create_query = DB.CreateQuery

local IsString = CPK.Type.IsString
local AssertError = CPK.Assert.Error

--- @alias Rgba {
--- 	r: integer,
--- 	g: integer,
--- 	b: integer,
--- 	a: integer,
--- }

--- @class Color
--- @field repr string
--- @field type string | nil
--- @field rgba Rgba
--- @field vec4 Vector4
--- @operator call(string): string
--- @operator concat(string | Color): string
--- @overload fun(str: string): string
local ColorImpl = {}

function ColorImpl:Rgba()
	return self.rgba
end

function ColorImpl:Vec4()
	return self.vec4
end

local ColorMeta = {}

--- @package
ColorMeta.__index = ColorImpl

--- @package
function ColorMeta.__tostring(self)
	return self.repr
end

--- @package
--- @param str string
function ColorMeta.__call(self, str)
	return lua_string_format('%s%s[ENDCOLOR]', self.repr, str)
end

--- @package
--- @param other any
function ColorMeta.__concat(self, other)
	return lua_tostring(self) .. lua_tostring(other)
end

--- @param v number
local function Encode255(v)
	if not v then return v end
	if v <= 0 then return 0 end
	if v >= 1 then return 255 end

	return lua_math_floor(v * 255 + 0.5)
end

--- @param v integer
local function Decode255(v)
	if not v then return v end
	if v <= 0 then return 0 end
	if v >= 255 then return 1 end

	return v / 255
end

--- @param type string | nil
--- @param rgba Rgba | { r: integer?, g: integer?,  b: integer?,  a: integer? }
--- @return Color
function ColorMeta.New(type, rgba)
	local r = rgba.r or 0
	local g = rgba.g or 0
	local b = rgba.b or 0
	local a = rgba.a or 255

	local vec4 = {
		x = Decode255(r),
		y = Decode255(g),
		z = Decode255(b),
		w = Decode255(a)
	}

	local repr = IsString(type)
			and ('[' .. type .. ']')
			or lua_string_format('[COLOR:%i:%i:%i:%i]', r, g, b, a)

	local this = {
		type = type,
		repr = repr,
		rgba = rgba,
		vec4 = vec4
	}

	return lua_setmetatable(this, ColorMeta)
end

--- @param rgba Rgba
--- @return Color
function ColorMeta.FromRgba(rgba)
	return ColorMeta.New(nil, rgba)
end

--- @param vec4 Vector4
--- @return Color
function ColorMeta.FromVec4(vec4)
	return ColorMeta.New(nil, {
		r = Encode255(vec4.x),
		g = Encode255(vec4.y),
		b = Encode255(vec4.z),
		a = Encode255(vec4.w)
	})
end

local GetColorByType = (function()
	local query = civ_db_create_query([[
		SELECT Type as type, Red as x, Green as y, Blue as z, Alpha as w
		FROM Colors
		WHERE ID = :type OR Type = :type
		LIMIT 1
	]])

	--- @param type string | integer
	return function(type)
		return query(type)()
	end
end)()

--- @param type string
--- @return Color
function ColorMeta.FromType(type)
	local found = GetColorByType(type)

	if not found then
		AssertError(type, 'Type that exists in Colors table')
	end

	return ColorMeta.New(found.type, {
		r = Encode255(found.x),
		g = Encode255(found.y),
		b = Encode255(found.z),
		a = Encode255(found.w)
	})
end

CPK.Util.Color = ColorMeta
